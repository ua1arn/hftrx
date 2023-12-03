#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


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
    UART0_IRQn = 32,                                  /*!< UART  */
    UART1_IRQn = 33,                                  /*!< UART  */
    UART2_IRQn = 34,                                  /*!< UART  */
    UART3_IRQn = 35,                                  /*!< UART  */
    UART4_IRQn = 36,                                  /*!< UART  */
    UART5_IRQn = 37,                                  /*!< UART  */
    TWI0_IRQn = 38,                                   /*!< TWI  */
    TWI1_IRQn = 39,                                   /*!< TWI  */
    TWI2_IRQn = 40,                                   /*!< TWI  */
    TWI3_IRQn = 41,                                   /*!< TWI  */
    TWI4_IRQn = 42,                                   /*!< TWI  */
    SPI0_IRQn = 44,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 45,                                   /*!< SPI Serial Peripheral Interface */
    EMAC0_IRQn = 46,                                  /*!< EMAC The Ethernet Medium Access Controller (EMAC) enables a host to transmi */
    EMAC1_IRQn = 47,                                  /*!< EMAC The Ethernet Medium Access Controller (EMAC) enables a host to transmi */
    PWM_IRQn = 48,                                    /*!< PWM Pulse Width Modulation module */
    TS_IRQn = 49,                                     /*!< TSC Transport Stream Controller */
    GPADC_IRQn = 50,                                  /*!< GPADC  */
    THS_IRQn = 51,                                    /*!< THS THS interrupt */
    LRADC_IRQn = 52,                                  /*!< LRADC  */
    OWA_IRQn = 53,                                    /*!< OWA One Wire Audio */
    DMIC_IRQn = 54,                                   /*!< DMIC Digital Microphone Interface */
    AudioCodec_ADC_IRQn = 55,                         /*!< AUDIO_CODEC AudioCodec_ADC interrupt */
    AHUB_IRQn = 56,                                   /*!< AHUB AudioHub interrupt */
    USB20_OTG_DEVICE_IRQn = 57,                       /*!< USBOTG USB OTG Dual-Role Device controller */
    USB20_HOST0_EHCI_IRQn = 58,                       /*!< USB_EHCI_Capability  */
    USB20_HOST0_OHCI_IRQn = 59,                       /*!< USB_OHCI_Capability  */
    USB20_HOST1_EHCI_IRQn = 60,                       /*!< USB_EHCI_Capability  */
    USB20_HOST1_OHCI_IRQn = 61,                       /*!< USB_OHCI_Capability  */
    USB20_HOST2_EHCI_IRQn = 62,                       /*!< USB_EHCI_Capability  */
    USB20_HOST2_OHCI_IRQn = 63,                       /*!< USB_OHCI_Capability  */
    USB20_HOST3_EHCI_IRQn = 64,                       /*!< USB_EHCI_Capability  */
    USB20_HOST3_OHCI_IRQn = 65,                       /*!< USB_OHCI_Capability  */
    SMHC0_IRQn = 67,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC1_IRQn = 68,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC2_IRQn = 69,                                  /*!< SMHC SD-MMC Host Controller */
    CLK_DET_IRQn = 73,                                /*!< CCU Clock Controller Unit (CCU) */
    DMAC_IRQn = 74,                                   /*!< DMAC  */
    TIMER0_IRQn = 80,                                 /*!< TIMER  */
    TIMER1_IRQn = 81,                                 /*!< TIMER  */
    WATCHDOG_IRQn = 82,                               /*!< TIMER  */
    GPIOA_IRQn = 83,                                  /*!< GPIOINT GPIOA interrupt */
    GPIOC_IRQn = 84,                                  /*!< GPIOINT GPIOC interrupt */
    GPIOD_IRQn = 85,                                  /*!< GPIOINT GPIOD interrupt */
    GPIOF_IRQn = 86,                                  /*!< GPIOINT GPIOF interrupt */
    GPIOG_IRQn = 87,                                  /*!< GPIOINT GPIOG interrupt */
    GPIOH_IRQn = 88,                                  /*!< GPIOINT GPIOH interrupt */
    GPIOI_IRQn = 89,                                  /*!< GPIOINT GPIOI interrupt */
    AudioCodec_DAC_IRQn = 90,                         /*!< AUDIO_CODEC AudioCodec_DAC interrupt */
    IOMMU_IRQn = 93,                                  /*!< IOMMU IOMMU */
    SMCARD_IRQn = 94,                                 /*!< SCR Smart Card Reader */
    HDMI_TX0_IRQn = 95,                               /*!< HDMI_TX  */
    TCON_LCD0_IRQn = 96,                              /*!< TCON_LCD TCON_LCD0 interrupt */
    TCON_LCD1_IRQn = 97,                              /*!< TCON_LCD TCON_LCD1 interrupt */
    TCON_TV0_IRQn = 98,                               /*!< TCON_TV TV Output */
    TCON_TV1_IRQn = 99,                               /*!< TCON_TV TV Output */
    DE_IRQn = 120,                                    /*!< DE_TOP DE interrupt */
    G2D_IRQn = 122,                                   /*!< G2D_TOP Graphic 2D top */
    CE_NS_IRQn = 123,                                 /*!< CE CE_NS interrupt */
    CE_S_IRQn = 124,                                  /*!< CE CE interrupt */
    GPU_EVENT_IRQn = 126,                             /*!< GPU GPU_EVENT interrupt */
    GPU_JOB_IRQn = 127,                               /*!< GPU GPU_JOB interrupt */
    GPU_MMU_IRQn = 128,                               /*!< GPU GPU_MMU interrupt */
    GPU_IRQn = 129,                                   /*!< GPU GPU interrupt */
    ALARM0_IRQn = 136,                                /*!< RTC Real Time Clock */
    S_TWI0_IRQn = 137,                                /*!< TWI  */
    TWD_IRQn = 140,                                   /*!< TWD Trust Watchdog */
    R_RSB_IRQn = 141,                                 /*!< R_RSB  */
    R_UART_IRQn = 144,                                /*!< UART R_UART */
    C0_CTI0_IRQn = 160,                               /*!< C0_CPUX_CFG_T507 C0_CTI0 Interrupt */
    C0_CTI1_IRQn = 161,                               /*!< C0_CPUX_CFG_T507 C0_CTI1 Interrupt */
    C0_CTI2_IRQn = 162,                               /*!< C0_CPUX_CFG_T507 C0_CTI2 Interrupt */
    C0_CTI3_IRQn = 163,                               /*!< C0_CPUX_CFG_T507 C0_CTI3 Interrupt */
    C0_COMMTX0_IRQn = 164,                            /*!< C0_CPUX_CFG_T507 C0_COMMTX0 Interrupt */
    C0_COMMTX1_IRQn = 165,                            /*!< C0_CPUX_CFG_T507 C0_COMMTX1 Interrupt */
    C0_COMMTX2_IRQn = 166,                            /*!< C0_CPUX_CFG_T507 C0_COMMTX2 Interrupt */
    C0_COMMTX3_IRQn = 167,                            /*!< C0_CPUX_CFG_T507 C0_COMMTX3 Interrupt */
    C0_COMMRX0_IRQn = 168,                            /*!< C0_CPUX_CFG_T507 C0_COMMRX0 Interrupt */
    C0_COMMRX1_IRQn = 169,                            /*!< C0_CPUX_CFG_T507 C0_COMMRX1 Interrupt */
    C0_COMMRX2_IRQn = 170,                            /*!< C0_CPUX_CFG_T507 C0_COMMRX2 Interrupt */
    C0_COMMRX3_IRQn = 171,                            /*!< C0_CPUX_CFG_T507 C0_COMMRX3 Interrupt */
    C0_PMU0_IRQn = 172,                               /*!< C0_CPUX_CFG_T507 C0_PMU0 Interrupt */
    C0_PMU1_IRQn = 173,                               /*!< C0_CPUX_CFG_T507 C0_PMU1 Interrupt */
    C0_PMU2_IRQn = 174,                               /*!< C0_CPUX_CFG_T507 C0_PMU2 Interrupt */
    C0_PMU3_IRQn = 175,                               /*!< C0_CPUX_CFG_T507 C0_PMU3 Interrupt */
    C0_AXI_ERROR_IRQn = 176,                          /*!< C0_CPUX_CFG_T507 C0_AXI_ERROR Interrupt */
    AXI_WR_IRQ_IRQn = 177,                            /*!< C0_CPUX_CFG_T507 AXI_WR_IRQ Interrupt */
    AXI_RD_IRQ_IRQn = 178,                            /*!< C0_CPUX_CFG_T507 AXI_RD_IRQ Interrupt */
    DBGRSTREQ0_IRQn = 179,                            /*!< C0_CPUX_CFG_T507 DBGRSTREQ0 Interrupt */
    DBGRSTREQ1_IRQn = 180,                            /*!< C0_CPUX_CFG_T507 DBGRSTREQ Interrupt */
    DBGRSTREQ2_IRQn = 181,                            /*!< C0_CPUX_CFG_T507 DBGRSTREQ2 Interrupt */
    DBGRSTREQ3_IRQn = 182,                            /*!< C0_CPUX_CFG_T507 DBGRSTREQ3 Interrupt */
    nVCPUMNTIRQ0_IRQn = 183,                          /*!< C0_CPUX_CFG_T507 nVCPUMNTIRQ0 Interrupt */
    nVCPUMNTIRQ1_IRQn = 184,                          /*!< C0_CPUX_CFG_T507 nVCPUMNTIRQ1 Interrupt */
    nVCPUMNTIRQ2_IRQn = 185,                          /*!< C0_CPUX_CFG_T507 nVCPUMNTIRQ2 Interrupt */
    nVCPUMNTIRQ3_IRQn = 186,                          /*!< C0_CPUX_CFG_T507 nVCPUMNTIRQ3 Interrupt */
    nCOMMIRQ0_IRQn = 187,                             /*!< C0_CPUX_CFG_T507 nCOMMIRQ0 Interrupt */
    nCOMMIRQ1_IRQn = 188,                             /*!< C0_CPUX_CFG_T507 nCOMMIRQ1 Interrupt */
    nCOMMIRQ2_IRQn = 189,                             /*!< C0_CPUX_CFG_T507 nCOMMIRQ2 Interrupt */
    nCOMMIRQ3_IRQn = 190,                             /*!< C0_CPUX_CFG_T507 nCOMMIRQ3 Interrupt */
    DBGPWRUPREQ_out_IRQn = 191,                       /*!< C0_CPUX_CFG_T507 DBGPWRUPREQ_out Interrupt */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define DE_BASE ((uintptr_t) 0x01000000)              /*!< DE Display Engine (DE) Base */
#define DE_XX0_BASE ((uintptr_t) 0x01000000)          /*!< DE_XX  Base */
#define DE_XX1_BASE ((uintptr_t) 0x01001000)          /*!< DE_XX  Base */
#define DE_XX2_BASE ((uintptr_t) 0x01002000)          /*!< DE_XX  Base */
#define DE_XX3_BASE ((uintptr_t) 0x01003000)          /*!< DE_XX  Base */
#define DE_TOP_BASE ((uintptr_t) 0x01008000)          /*!< DE_TOP Display Engine (DE) TOP (APB) Base */
#define DE_GLB_BASE ((uintptr_t) 0x01008100)          /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DE_VI1_BASE ((uintptr_t) 0x01101000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_VSU_BASE ((uintptr_t) 0x01104000)          /*!< DE_VSU Video Scaler Unit (VSU) Base */
#define DE_FCE_BASE ((uintptr_t) 0x01110000)          /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS_BASE ((uintptr_t) 0x01111000)          /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_VI2_BASE ((uintptr_t) 0x01121000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_VI3_BASE ((uintptr_t) 0x01141000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_UI1_BASE ((uintptr_t) 0x011C1000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_UI2_BASE ((uintptr_t) 0x011E1000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_UI3_BASE ((uintptr_t) 0x01201000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_BLD1_BASE ((uintptr_t) 0x01281000)         /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_BLD2_BASE ((uintptr_t) 0x012A1000)         /*!< DE_BLD Display Engine (DE) - Blender Base */
#define G2D_TOP_BASE ((uintptr_t) 0x01480000)         /*!< G2D_TOP Graphic 2D top Base */
#define G2D_MIXER_BASE ((uintptr_t) 0x01480100)       /*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer Base */
#define G2D_ROT_BASE ((uintptr_t) 0x014A8000)         /*!< G2D_ROT Graphic 2D Rotate Base */
#define GPU_BASE ((uintptr_t) 0x01800000)             /*!< GPU Mali G31 MP2 Base */
#define CE_NS_BASE ((uintptr_t) 0x01904000)           /*!< CE The Crypto Engine (CE) module Base */
#define CE_S_BASE ((uintptr_t) 0x01904800)            /*!< CE The Crypto Engine (CE) module Base */
#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK Port Controller Base */
#define SYS_CFG_BASE ((uintptr_t) 0x03000000)         /*!< SYS_CFG  Base */
#define CCU_BASE ((uintptr_t) 0x03001000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define DMAC_BASE ((uintptr_t) 0x03002000)            /*!< DMAC  Base */
#define SID_BASE ((uintptr_t) 0x03006000)             /*!< SID  Base */
#define TIMER_BASE ((uintptr_t) 0x03009000)           /*!< TIMER  Base */
#define PWM_BASE ((uintptr_t) 0x0300A000)             /*!< PWM Pulse Width Modulation module Base */
#define GPIOA_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Port Controller Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x0300B000)       /*!< GPIOBLOCK Port Controller Base */
#define GPIOC_BASE ((uintptr_t) 0x0300B048)           /*!< GPIO Port Controller Base */
#define GPIOD_BASE ((uintptr_t) 0x0300B06C)           /*!< GPIO Port Controller Base */
#define GPIOE_BASE ((uintptr_t) 0x0300B090)           /*!< GPIO Port Controller Base */
#define GPIOF_BASE ((uintptr_t) 0x0300B0B4)           /*!< GPIO Port Controller Base */
#define GPIOG_BASE ((uintptr_t) 0x0300B0D8)           /*!< GPIO Port Controller Base */
#define GPIOH_BASE ((uintptr_t) 0x0300B0FC)           /*!< GPIO Port Controller Base */
#define GPIOI_BASE ((uintptr_t) 0x0300B120)           /*!< GPIO Port Controller Base */
#define GPIOINTA_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x0300B240)        /*!< GPIOINT  Base */
#define GPIOINTD_BASE ((uintptr_t) 0x0300B260)        /*!< GPIOINT  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x0300B2A0)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x0300B2C0)        /*!< GPIOINT  Base */
#define GPIOINTH_BASE ((uintptr_t) 0x0300B2E0)        /*!< GPIOINT  Base */
#define GPIOINTI_BASE ((uintptr_t) 0x0300B300)        /*!< GPIOINT  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define IOMMU_BASE ((uintptr_t) 0x030F0000)           /*!< IOMMU IOMMU (I/O Memory management unit) Base */
#define SMHC0_BASE ((uintptr_t) 0x04020000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC1_BASE ((uintptr_t) 0x04021000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC2_BASE ((uintptr_t) 0x04022000)           /*!< SMHC SD-MMC Host Controller Base */
#define UART0_BASE ((uintptr_t) 0x05000000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x05000400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x05000800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x05000C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x05001000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0x05001400)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x05002000)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x05002400)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x05002800)            /*!< TWI  Base */
#define TWI3_BASE ((uintptr_t) 0x05002C00)            /*!< TWI  Base */
#define TWI4_BASE ((uintptr_t) 0x05003000)            /*!< TWI  Base */
#define SCR_BASE ((uintptr_t) 0x05005000)             /*!< SCR Smart Card Reader Base */
#define SPI0_BASE ((uintptr_t) 0x05010000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x05011000)            /*!< SPI Serial Peripheral Interface Base */
#define EMAC0_BASE ((uintptr_t) 0x05020000)           /*!< EMAC The Ethernet Medium Access Controller (EMAC) enables a host to transmi Base */
#define EMAC1_BASE ((uintptr_t) 0x05030000)           /*!< EMAC The Ethernet Medium Access Controller (EMAC) enables a host to transmi Base */
#define TSC_BASE ((uintptr_t) 0x05060000)             /*!< TSC Transport Stream Controller Base */
#define TSG_BASE ((uintptr_t) 0x05060040)             /*!< TSG Transport Stream Controller Base */
#define TSF_BASE ((uintptr_t) 0x05060100)             /*!< TSF Transport Stream Controller Base */
#define TSD_BASE ((uintptr_t) 0x05060180)             /*!< TSD Transport Stream Controller Base */
#define GPADC_BASE ((uintptr_t) 0x05070000)           /*!< GPADC  Base */
#define THS_BASE ((uintptr_t) 0x05070400)             /*!< THS Thermal Sensor Base */
#define LRADC_BASE ((uintptr_t) 0x05070800)           /*!< LRADC  Base */
#define OWA_BASE ((uintptr_t) 0x05093000)             /*!< OWA One Wire Audio Base */
#define DMIC_BASE ((uintptr_t) 0x05095000)            /*!< DMIC Digital Microphone Interface Base */
#define AUDIO_CODEC_BASE ((uintptr_t) 0x05096000)     /*!< AUDIO_CODEC Audio Codec Base */
#define AHUB_BASE ((uintptr_t) 0x05097000)            /*!< AHUB Audio HUB Base */
#define I2S0_BASE ((uintptr_t) 0x05097200)            /*!< I2S_PCM  Base */
#define I2S1_BASE ((uintptr_t) 0x05097300)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x05097400)            /*!< I2S_PCM  Base */
#define I2S3_BASE ((uintptr_t) 0x05097500)            /*!< I2S_PCM  Base */
#define USB20_OTG_DEVICE_BASE ((uintptr_t) 0x05100000)/*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB20_OTG_PHYC_BASE ((uintptr_t) 0x05100400)  /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST0_EHCI_BASE ((uintptr_t) 0x05101000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST0_OHCI_BASE ((uintptr_t) 0x05101400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC0_BASE ((uintptr_t) 0x05101800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST1_EHCI_BASE ((uintptr_t) 0x05200000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST1_OHCI_BASE ((uintptr_t) 0x05200400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC1_BASE ((uintptr_t) 0x05200800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST2_EHCI_BASE ((uintptr_t) 0x05310000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST2_OHCI_BASE ((uintptr_t) 0x05310400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC2_BASE ((uintptr_t) 0x05310800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST3_EHCI_BASE ((uintptr_t) 0x05311000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST3_OHCI_BASE ((uintptr_t) 0x05311400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC3_BASE ((uintptr_t) 0x05311800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define HDMI_TX0_BASE ((uintptr_t) 0x06000000)        /*!< HDMI_TX  Base */
#define HDMI_PHY_BASE ((uintptr_t) 0x06010000)        /*!< HDMI_PHY  Base */
#define DISP_IF_TOP_BASE ((uintptr_t) 0x06510000)     /*!< DISP_IF_TOP display interface top (DISP_IF_TOP) Base */
#define TCON_LCD0_BASE ((uintptr_t) 0x06511000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define TCON_LCD1_BASE ((uintptr_t) 0x06512000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define TCON_TV0_BASE ((uintptr_t) 0x06515000)        /*!< TCON_TV TV Output Base */
#define TCON_TV1_BASE ((uintptr_t) 0x06516000)        /*!< TCON_TV TV Output Base */
#define RTC_BASE ((uintptr_t) 0x07000000)             /*!< RTC Real Time Clock Base */
#define R_CPUCFG_BASE ((uintptr_t) 0x07000400)        /*!< R_CPUCFG  Base */
#define PRCM_BASE ((uintptr_t) 0x07010000)            /*!< PRCM Power Reset Clock Management module Base */
#define R_WDOG_BASE ((uintptr_t) 0x07020400)          /*!< R_WDOG  Base */
#define GPIOL_BASE ((uintptr_t) 0x07022000)           /*!< GPIO Port Controller Base */
#define R_PIO_BASE ((uintptr_t) 0x07022000)           /*!< GPIO Port Controller Base */
#define R_UART_BASE ((uintptr_t) 0x07080000)          /*!< UART  Base */
#define R_TWI_BASE ((uintptr_t) 0x07081400)           /*!< TWI  Base */
#define S_TWI0_BASE ((uintptr_t) 0x07081400)          /*!< TWI  Base */
#define R_CAN0_BASE ((uintptr_t) 0x07082000)          /*!< R_CAN Car Area Network controller Base */
#define R_RSB_BASE ((uintptr_t) 0x07083000)           /*!< R_RSB  Base */
#define CPU_SUBSYS_CTRL_H616_BASE ((uintptr_t) 0x08100000)/*!< CPU_SUBSYS_CTRL_H616 H616 CPU Subsystem Control Register List Base */
#define CPU_SUBSYS_CTRL_T507_BASE ((uintptr_t) 0x08100000)/*!< CPU_SUBSYS_CTRL_T507 T507 CPU Subsystem Control Register List Base */
#define C0_CPUX_CFG_H616_BASE ((uintptr_t) 0x09010000)/*!< C0_CPUX_CFG_H616 H616 Cluster 0 Configuration Register List Base */
#define C0_CPUX_CFG_T507_BASE ((uintptr_t) 0x09010000)/*!< C0_CPUX_CFG_T507 T507 Cluster 0 Configuration Register List Base */

/*
 * @brief AHUB
 */
/*!< AHUB Audio HUB */
typedef struct AHUB_Type
{
             uint32_t reserved_0x000 [0x0002];
    volatile uint32_t AHUB_RST;                       /*!< Offset 0x008 AHUB Reset */
    volatile uint32_t AHUB_GAT;                       /*!< Offset 0x00C AHUB Gating */
    struct
    {
        volatile uint32_t APBIF_TXn_CTRL;             /*!< Offset 0x010 APBIF TXn Control */
        volatile uint32_t APBIF_TXnIRQ_CTRL;          /*!< Offset 0x014 APBIF TXn DMA & Interrupt Control */
        volatile uint32_t APBIF_TXnIRQ_STS;           /*!< Offset 0x018 AHUB APBIF TXn DMA & Interrupt Status */
                 uint32_t reserved_0x00C;
        volatile uint32_t APBIF_TXnFIFO_CTRL;         /*!< Offset 0x020 AHUB APBIF TXn FIFO Control */
        volatile uint32_t APBIF_TXnFIFO_STS;          /*!< Offset 0x024 APBIF TXn FIFO Status */
                 uint32_t reserved_0x018 [0x0002];
        volatile uint32_t APBIF_TXnFIFO;              /*!< Offset 0x030 APBIF TXn FIFO */
        volatile uint32_t APBIF_TXnFIFO_CNT;          /*!< Offset 0x034 APBIF TXn FIFO Counter */
                 uint32_t reserved_0x028 [0x0002];
    } APBIF_TX [0x003];                               /*!< Offset 0x010 APBIF TX (n=0~2) */
             uint32_t reserved_0x0A0 [0x0018];
    struct
    {
        volatile uint32_t APBIF_RXn_CTRL;             /*!< Offset 0x100 APBIF RXn Control */
        volatile uint32_t APBIF_RXnIRQ_CTRL;          /*!< Offset 0x104 APBIF RXn DMA & Interrupt Control */
        volatile uint32_t APBIF_RXnIRQ_STS;           /*!< Offset 0x108 APBIF RXn DMA & Interrupt Status */
                 uint32_t reserved_0x00C;
        volatile uint32_t APBIF_RXnFIFO_CTRL;         /*!< Offset 0x110 APBIF RXn FIFO Control */
        volatile uint32_t APBIF_RXnFIFO_STS;          /*!< Offset 0x114 APBIF RX0 FIFO Status */
        volatile uint32_t APBIF_RXn_CONT;             /*!< Offset 0x118 APBIF RXn Contact Select */
                 uint32_t reserved_0x01C;
        volatile uint32_t APBIF_RXnFIFO;              /*!< Offset 0x120 APBIF RXn FIFO */
        volatile uint32_t APBIF_RXnFIFO_CNT;          /*!< Offset 0x124 APBIF RXn FIFO Counter */
                 uint32_t reserved_0x028 [0x0002];
    } APBIF_RX [0x003];                               /*!< Offset 0x100 APBIF RX (n=0~2) */
             uint32_t reserved_0x190 [0x021C];
    struct
    {
        volatile uint32_t DAMn_CTRL;                  /*!< Offset 0xA00 DAM Control */
                 uint32_t reserved_0x004 [0x0003];
        volatile uint32_t DAMn_RXx_SRC [0x003];       /*!< Offset 0xA10 DAM RXDIF0..RXDIF2 Source Select */
                 uint32_t reserved_0x01C [0x0005];
        volatile uint32_t DAMn_MIX_CTRLx [0x008];     /*!< Offset 0xA30 DAM MIX Control 0..7 */
        volatile uint32_t DAMn_GAIN_CTRLx [0x008];    /*!< Offset 0xA50 DAM GAIN Control 0..7 */
                 uint32_t reserved_0x070 [0x0004];
    } DAM [0x002];                                    /*!< Offset 0xA00 DAMn (n=0~2) */
} AHUB_TypeDef; /* size of structure = 0xB00 */
/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Audio Codec */
typedef struct AUDIO_CODEC_Type
{
    volatile uint32_t AC_DAC_DPC;                     /*!< Offset 0x000 DAC Digital Part Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t AC_DAC_FIFOC;                   /*!< Offset 0x010 DAC FIFO Control Register */
    volatile uint32_t AC_DAC_FIFOS;                   /*!< Offset 0x014 DAC FIFO Status Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t AC_DAC_TXDATA;                  /*!< Offset 0x020 DAC TX DATA Register */
    volatile uint32_t AC_DAC_CNT;                     /*!< Offset 0x024 DAC TX FIFO Counter Register */
    volatile uint32_t AC_DAC_DG;                      /*!< Offset 0x028 DAC Debug Register */
             uint32_t reserved_0x02C [0x0031];
    volatile uint32_t AC_DAC_DAP_CTRL;                /*!< Offset 0x0F0 DAC DAP Control Register */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t AC_DAC_DRC_HHPFC;               /*!< Offset 0x100 DAC DRC High HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_LHPFC;               /*!< Offset 0x104 DAC DRC Low HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_CTRL;                /*!< Offset 0x108 DAC DRC Control Register */
    volatile uint32_t AC_DAC_DRC_LPFHAT;              /*!< Offset 0x10C DAC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLAT;              /*!< Offset 0x110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHAT;              /*!< Offset 0x114 DAC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLAT;              /*!< Offset 0x118 DAC DRC Right Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFHRT;              /*!< Offset 0x11C DAC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLRT;              /*!< Offset 0x120 DAC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHRT;              /*!< Offset 0x124 DAC DRC Right Peak filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLRT;              /*!< Offset 0x128 DAC DRC Right Peak filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSHAT;             /*!< Offset 0x12C DAC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSLAT;             /*!< Offset 0x130 DAC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSHAT;             /*!< Offset 0x134 DAC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSLAT;             /*!< Offset 0x138 DAC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_HCT;                 /*!< Offset 0x13C DAC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LCT;                 /*!< Offset 0x140 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_HKC;                 /*!< Offset 0x144 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKC;                 /*!< Offset 0x148 DAC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPC;                /*!< Offset 0x14C DAC DRC Compressor High Output at Compressor Threshold Register */
    volatile uint32_t AC_DAC_DRC_LOPC;                /*!< Offset 0x150 DAC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_DAC_DRC_HLT;                 /*!< Offset 0x154 DAC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LLT;                 /*!< Offset 0x158 DAC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKl;                 /*!< Offset 0x15C DAC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKl;                 /*!< Offset 0x160 DAC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPL;                /*!< Offset 0x164 DAC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_LOPL;                /*!< Offset 0x168 DAC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_HET;                 /*!< Offset 0x16C DAC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LET;                 /*!< Offset 0x170 DAC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKE;                 /*!< Offset 0x174 DAC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKE;                 /*!< Offset 0x178 DAC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPE;                /*!< Offset 0x17C DAC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_LOPE;                /*!< Offset 0x180 DAC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_HKN;                 /*!< Offset 0x184 DAC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKN;                 /*!< Offset 0x188 DAC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_SFHAT;               /*!< Offset 0x18C DAC DRC Smooth Filter */
    volatile uint32_t AC_DAC_DRC_SFLAT;               /*!< Offset 0x190 DAC DRC Smooth Filter */
    volatile uint32_t AC_DAC_DRC_SFHRT;               /*!< Offset 0x194 DAC DRC Smooth Filter Gain High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLRT;               /*!< Offset 0x198 DAC DRC Smooth Filter */
    volatile uint32_t AC_DAC_DRC_MXGHS;               /*!< Offset 0x19C DAC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MXGLS;               /*!< Offset 0x1A0 DAC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGHS;               /*!< Offset 0x1A4 DAC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGLS;               /*!< Offset 0x1A8 DAC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_EPSHC;               /*!< Offset 0x1AC DAC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_DAC_DRC_EPSLC;               /*!< Offset 0x1B0 DAC DRC Expander Smooth Time Low Coef Register */
             uint32_t reserved_0x1B4;
    volatile uint32_t AC_DAC_DRC_HPFHGAIN;            /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_DAC_DRC_HPFLGAIN;            /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
             uint32_t reserved_0x1C0 [0x0054];
    volatile uint32_t DAC_REG;                        /*!< Offset 0x310 DAC Analog Control Register */
    volatile uint32_t MIXER_REG;                      /*!< Offset 0x314 MIXER Analog Control Register */
             uint32_t reserved_0x318;
    volatile uint32_t RAMP_REG;                       /*!< Offset 0x31C RAMP Control Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x320 */
/*
 * @brief C0_CPUX_CFG_H616
 */
/*!< C0_CPUX_CFG_H616 H616 Cluster 0 Configuration Register List */
typedef struct C0_CPUX_CFG_H616_Type
{
    volatile uint32_t C0_RST_CTRL;                    /*!< Offset 0x000 Cluster 0 Reset Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t C0_CTRL_REG0;                   /*!< Offset 0x010 Cluster 0 Control Register0 */
    volatile uint32_t C0_CTRL_REG1;                   /*!< Offset 0x014 Cluster 0 Control Register1 */
    volatile uint32_t C0_CTRL_REG2;                   /*!< Offset 0x018 Cluster 0 Control Register2 */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CACHE_CFG_REG;                  /*!< Offset 0x024 Cache Configuration Register */
             uint32_t reserved_0x028 [0x0006];
    struct
    {
        volatile uint32_t LOW;                        /*!< Offset 0x040 Reset Vector Base Address Registerx_L */
        volatile uint32_t HIGH;                       /*!< Offset 0x044 Reset Vector Base Address Registerx_H */
    } RVBARADDR [0x004];                              /*!< Offset 0x040 Reset Vector Base Address Register for core [0..3] */
             uint32_t reserved_0x060 [0x0008];
    volatile uint32_t C0_CPU_STATUS;                  /*!< Offset 0x080 Cluster 0 CPU Status Register */
    volatile uint32_t L2_STATUS_REG;                  /*!< Offset 0x084 Cluster 0 L2 Status Register */
             uint32_t reserved_0x088 [0x000E];
    volatile uint32_t DBG_REG0;                       /*!< Offset 0x0C0 Cluster 0 Debug State Register0 */
    volatile uint32_t DBG_REG1;                       /*!< Offset 0x0C4 Cluster 0 Debug State Register1 */
} C0_CPUX_CFG_H616_TypeDef; /* size of structure = 0x0C8 */
/*
 * @brief C0_CPUX_CFG_T507
 */
/*!< C0_CPUX_CFG_T507 T507 Cluster 0 Configuration Register List */
typedef struct C0_CPUX_CFG_T507_Type
{
    volatile uint32_t C0_RST_CTRL;                    /*!< Offset 0x000 Cluster 0 Reset Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t C0_CTRL_REG0;                   /*!< Offset 0x010 Cluster 0 Control Register0 */
    volatile uint32_t C0_CTRL_REG1;                   /*!< Offset 0x014 Cluster 0 Control Register1 */
    volatile uint32_t C0_CTRL_REG2;                   /*!< Offset 0x018 Cluster 0 Control Register2 */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CACHE_CFG_REG;                  /*!< Offset 0x024 Cache Configuration Register */
             uint32_t reserved_0x028 [0x000E];
    volatile uint32_t C0_CPUx_CTRL_REG [0x004];       /*!< Offset 0x060 Cluster 0 CPU0..CPU03 Control Register */
             uint32_t reserved_0x070 [0x0004];
    volatile uint32_t C0_CPU_STATUS;                  /*!< Offset 0x080 Cluster 0 CPU Status Register */
    volatile uint32_t L2_STATUS_REG;                  /*!< Offset 0x084 Cluster 0 L2 Status Register */
} C0_CPUX_CFG_T507_TypeDef; /* size of structure = 0x088 */
/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPUX_CTRL_REG;              /*!< Offset 0x000 PLL_CPUX Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t PLL_DDR0_CTRL_REG;              /*!< Offset 0x010 PLL_DDR0 Control Register */
             uint32_t reserved_0x014;
    volatile uint32_t PLL_DDR1_CTRL_REG;              /*!< Offset 0x018 PLL_DDR1 Control Register */
             uint32_t reserved_0x01C;
    volatile uint32_t PLL_PERI0_CTRL_REG;             /*!< Offset 0x020 PLL_PERI0 Control Register */
             uint32_t reserved_0x024;
    volatile uint32_t PLL_PERI1_CTRL_REG;             /*!< Offset 0x028 PLL_PERI1 Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t PLL_GPU0_CTRL_REG;              /*!< Offset 0x030 PLL_GPU0 Control Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t PLL_VIDEO0_CTRL_REG;            /*!< Offset 0x040 PLL_VIDEO0 Control Register */
             uint32_t reserved_0x044;
    volatile uint32_t PLL_VIDEO1_CTRL_REG;            /*!< Offset 0x048 PLL_VIDEO1 Control Register */
             uint32_t reserved_0x04C;
    volatile uint32_t PLL_VIDEO2_CTRL_REG;            /*!< Offset 0x050 PLL_VIDEO2 Control Register */
             uint32_t reserved_0x054;
    volatile uint32_t PLL_VE_CTRL_REG;                /*!< Offset 0x058 PLL_VE Control Register */
             uint32_t reserved_0x05C;
    volatile uint32_t PLL_DE_CTRL_REG;                /*!< Offset 0x060 PLL_DE Control Register */
             uint32_t reserved_0x064 [0x0005];
    volatile uint32_t PLL_AUDIO_CTRL_REG;             /*!< Offset 0x078 PLL_AUDIO Control Register */
             uint32_t reserved_0x07C [0x0019];
    volatile uint32_t PLL_CSI_CTRL_REG;               /*!< Offset 0x0E0 PLL_CSI Control Register */
             uint32_t reserved_0x0E4 [0x000B];
    volatile uint32_t PLL_DDR0_PAT_CTRL_REG;          /*!< Offset 0x110 PLL_DDR0 Pattern Control Register */
             uint32_t reserved_0x114;
    volatile uint32_t PLL_DDR1_PAT_CTRL_REG;          /*!< Offset 0x118 PLL_DDR1 Pattern Control Register */
             uint32_t reserved_0x11C;
    volatile uint32_t PLL_PERI0_PAT0_CTRL_REG;        /*!< Offset 0x120 PLL_PERI0 Pattern0 Control Register */
    volatile uint32_t PLL_PERI0_PAT1_CTRL_REG;        /*!< Offset 0x124 PLL_PERI0 Pattern1 Control Register */
    volatile uint32_t PLL_PERI1_PAT0_CTRL_REG;        /*!< Offset 0x128 PLL_PERI1 Pattern0 Control Register */
    volatile uint32_t PLL_PERI1_PAT1_CTRL_REG;        /*!< Offset 0x12C PLL_PERI1 Pattern1 Control Register */
    volatile uint32_t PLL_GPU0_PAT0_CTRL_REG;         /*!< Offset 0x130 PLL_GPU0 Pattern0 Control Register */
    volatile uint32_t PLL_GPU0_PAT1_CTRL_REG;         /*!< Offset 0x134 PLL_GPU0 Pattern1 Control Register */
             uint32_t reserved_0x138 [0x0002];
    volatile uint32_t PLL_VIDEO0_PAT0_CTRL_REG;       /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO0_PAT1_CTRL_REG;       /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT0_CTRL_REG;       /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT1_CTRL_REG;       /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
    volatile uint32_t PLL_VIDEO2_PAT0_CTRL_REG;       /*!< Offset 0x150 PLL_VIDEO2 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO2_PAT1_CTRL_REG;       /*!< Offset 0x154 PLL_VIDEO2 Pattern1 Control Register */
    volatile uint32_t PLL_VE_PAT0_CTRL_REG;           /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
    volatile uint32_t PLL_VE_PAT1_CTRL_REG;           /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
    volatile uint32_t PLL_DE_PAT0_CTRL_REG;           /*!< Offset 0x160 PLL_DE Pattern0 Control Register */
    volatile uint32_t PLL_DE_PAT1_CTRL_REG;           /*!< Offset 0x164 PLL_DE Pattern1 Control Register */
             uint32_t reserved_0x168 [0x0004];
    volatile uint32_t PLL_AUDIO_PAT0_CTRL_REG;        /*!< Offset 0x178 PLL_AUDIO Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO_PAT1_CTRL_REG;        /*!< Offset 0x17C PLL_AUDIO Pattern1 Control Register */
             uint32_t reserved_0x180 [0x0018];
    volatile uint32_t PLL_CSI_PAT0_CTRL_REG;          /*!< Offset 0x1E0 PLL_CSI Pattern0 Control Register */
    volatile uint32_t PLL_CSI_PAT1_CTRL_REG;          /*!< Offset 0x1E4 PLL_CSI Pattern1 Control Register */
             uint32_t reserved_0x1E8 [0x0046];
    volatile uint32_t PLL_CPUX_BIAS_REG;              /*!< Offset 0x300 PLL_CPUX Bias Register */
             uint32_t reserved_0x304 [0x0003];
    volatile uint32_t PLL_DDR0_BIAS_REG;              /*!< Offset 0x310 PLL_DDR0 Bias Register */
             uint32_t reserved_0x314;
    volatile uint32_t PLL_DDR1_BIAS_REG;              /*!< Offset 0x318 PLL_DDR1 Bias Register */
             uint32_t reserved_0x31C;
    volatile uint32_t PLL_PERI0_BIAS_REG;             /*!< Offset 0x320 PLL_PERI0 Bias Register */
             uint32_t reserved_0x324;
    volatile uint32_t PLL_PERI1_BIAS_REG;             /*!< Offset 0x328 PLL_PERI1 Bias Register */
             uint32_t reserved_0x32C;
    volatile uint32_t PLL_GPU0_BIAS_REG;              /*!< Offset 0x330 PLL_GPU0 Bias Register */
             uint32_t reserved_0x334 [0x0003];
    volatile uint32_t PLL_VIDEO0_BIAS_REG;            /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
             uint32_t reserved_0x344;
    volatile uint32_t PLL_VIDEO1_BIAS_REG;            /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
             uint32_t reserved_0x34C [0x0003];
    volatile uint32_t PLL_VE_BIAS_REG;                /*!< Offset 0x358 PLL_VE Bias Register */
             uint32_t reserved_0x35C;
    volatile uint32_t PLL_DE_BIAS_REG;                /*!< Offset 0x360 PLL_DE Bias Register */
             uint32_t reserved_0x364 [0x0005];
    volatile uint32_t PLL_AUDIO_BIAS_REG;             /*!< Offset 0x378 PLL_AUDIO Bias Register */
             uint32_t reserved_0x37C [0x0019];
    volatile uint32_t PLL_CSI_BIAS_REG;               /*!< Offset 0x3E0 PLL_CSI Bias Register */
             uint32_t reserved_0x3E4 [0x0007];
    volatile uint32_t PLL_CPUX_TUN_REG;               /*!< Offset 0x400 PLL_CPUX Tuning Register */
             uint32_t reserved_0x404 [0x003F];
    volatile uint32_t CPUX_AXI_CFG_REG;               /*!< Offset 0x500 CPUX_AXI Configuration Register */
             uint32_t reserved_0x504 [0x0003];
    volatile uint32_t PSI_AHB1_AHB2_CFG_REG;          /*!< Offset 0x510 PSI_AHB1_AHB2 Configuration Register */
             uint32_t reserved_0x514 [0x0002];
    volatile uint32_t AHB3_CFG_REG;                   /*!< Offset 0x51C AHB3 Configuration Register */
    volatile uint32_t APB1_CFG_REG;                   /*!< Offset 0x520 APB1 Configuration Register */
    volatile uint32_t APB2_CFG_REG;                   /*!< Offset 0x524 APB2 Configuration Register */
             uint32_t reserved_0x528 [0x0006];
    volatile uint32_t MBUS_CFG_REG;                   /*!< Offset 0x540 MBUS Configuration Register */
             uint32_t reserved_0x544 [0x002F];
    volatile uint32_t DE_CLK_REG;                     /*!< Offset 0x600 DE Clock Register */
             uint32_t reserved_0x604 [0x0002];
    volatile uint32_t DE_BGR_REG;                     /*!< Offset 0x60C DE Bus Gating Reset Register */
             uint32_t reserved_0x610 [0x0004];
    volatile uint32_t DI_CLK_REG;                     /*!< Offset 0x620 DI Clock Register */
             uint32_t reserved_0x624 [0x0002];
    volatile uint32_t DI_BGR_REG;                     /*!< Offset 0x62C DI Bus Gating Reset Register */
    volatile uint32_t G2D_CLK_REG;                    /*!< Offset 0x630 G2D Clock Register */
             uint32_t reserved_0x634 [0x0002];
    volatile uint32_t G2D_BGR_REG;                    /*!< Offset 0x63C G2D Bus Gating Reset Register */
             uint32_t reserved_0x640 [0x000C];
    volatile uint32_t GPU_CLK0_REG;                   /*!< Offset 0x670 GPU Clock0 Register */
    volatile uint32_t GPU_CLK1_REG;                   /*!< Offset 0x674 GPU Clock1 Register */
             uint32_t reserved_0x678;
    volatile uint32_t GPU_BGR_REG;                    /*!< Offset 0x67C GPU Bus Gating Reset Register */
    volatile uint32_t CE_CLK_REG;                     /*!< Offset 0x680 CE Clock Register */
             uint32_t reserved_0x684 [0x0002];
    volatile uint32_t CE_BGR_REG;                     /*!< Offset 0x68C CE Bus Gating Reset Register */
    volatile uint32_t VE_CLK_REG;                     /*!< Offset 0x690 VE Clock Register */
             uint32_t reserved_0x694 [0x0002];
    volatile uint32_t VE_BGR_REG;                     /*!< Offset 0x69C VE Bus Gating Reset Register */
             uint32_t reserved_0x6A0 [0x001B];
    volatile uint32_t DMA_BGR_REG;                    /*!< Offset 0x70C DMA Bus Gating Reset Register */
             uint32_t reserved_0x710 [0x000B];
    volatile uint32_t HSTIMER_BGR_REG;                /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
    volatile uint32_t AVS_CLK_REG;                    /*!< Offset 0x740 AVS Clock Register */
             uint32_t reserved_0x744 [0x0012];
    volatile uint32_t DBGSYS_BGR_REG;                 /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
             uint32_t reserved_0x790 [0x0003];
    volatile uint32_t PSI_BGR_REG;                    /*!< Offset 0x79C PSI Bus Gating Reset Register */
             uint32_t reserved_0x7A0 [0x0003];
    volatile uint32_t PWM_BGR_REG;                    /*!< Offset 0x7AC PWM Bus Gating Reset Register */
             uint32_t reserved_0x7B0 [0x0003];
    volatile uint32_t IOMMU_BGR_REG;                  /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
             uint32_t reserved_0x7C0 [0x0010];
    volatile uint32_t DRAM_CLK_REG;                   /*!< Offset 0x800 DRAM Clock Register */
    volatile uint32_t MBUS_MAT_CLK_GATING_REG;        /*!< Offset 0x804 MBUS Master Clock Gating Register */
             uint32_t reserved_0x808;
    volatile uint32_t DRAM_BGR_REG;                   /*!< Offset 0x80C DRAM Bus Gating Reset Register */
    volatile uint32_t NAND0_0_CLK_REG;                /*!< Offset 0x810 NAND0_0 Clock Register */
    volatile uint32_t NAND0_1_CLK_REG;                /*!< Offset 0x814 NAND0_1 Clock Register */
             uint32_t reserved_0x818 [0x0005];
    volatile uint32_t NAND_BGR_REG;                   /*!< Offset 0x82C NAND Bus Gating Reset Register */
    volatile uint32_t SMHC0_CLK_REG;                  /*!< Offset 0x830 SMHC0 Clock Register */
    volatile uint32_t SMHC1_CLK_REG;                  /*!< Offset 0x834 SMHC1 Clock Register */
    volatile uint32_t SMHC2_CLK_REG;                  /*!< Offset 0x838 SMHC2 Clock Register */
             uint32_t reserved_0x83C [0x0004];
    volatile uint32_t SMHC_BGR_REG;                   /*!< Offset 0x84C SMHC Bus Gating Reset Register */
             uint32_t reserved_0x850 [0x002F];
    volatile uint32_t UART_BGR_REG;                   /*!< Offset 0x90C UART Bus Gating Reset Register */
             uint32_t reserved_0x910 [0x0003];
    volatile uint32_t TWI_BGR_REG;                    /*!< Offset 0x91C TWI Bus Gating Reset Register */
             uint32_t reserved_0x920 [0x0007];
    volatile uint32_t SCR_BGR_REG;                    /*!< Offset 0x93C SCR Bus Gating Reset Register */
    volatile uint32_t SPI0_CLK_REG;                   /*!< Offset 0x940 SPI0 Clock Register */
    volatile uint32_t SPI1_CLK_REG;                   /*!< Offset 0x944 SPI1 Clock Register */
             uint32_t reserved_0x948 [0x0009];
    volatile uint32_t SPI_BGR_REG;                    /*!< Offset 0x96C SPI Bus Gating Reset Register */
    volatile uint32_t EPHY_25M_CLK_REG;               /*!< Offset 0x970 EPHY_25M Clock Register */
             uint32_t reserved_0x974 [0x0002];
    volatile uint32_t EMAC_BGR_REG;                   /*!< Offset 0x97C EMAC Bus Gating Reset Register */
             uint32_t reserved_0x980 [0x000C];
    volatile uint32_t TS_CLK_REG;                     /*!< Offset 0x9B0 TS Clock Register */
             uint32_t reserved_0x9B4 [0x0002];
    volatile uint32_t TS_BGR_REG;                     /*!< Offset 0x9BC TS Bus Gating Reset Register */
             uint32_t reserved_0x9C0 [0x000B];
    volatile uint32_t GPADC_BGR_REG;                  /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
             uint32_t reserved_0x9F0 [0x0003];
    volatile uint32_t THS_BGR_REG;                    /*!< Offset 0x9FC THS Bus Gating Reset Register */
             uint32_t reserved_0xA00 [0x0008];
    volatile uint32_t OWA_CLK_REG;                    /*!< Offset 0xA20 OWA Clock Register */
             uint32_t reserved_0xA24 [0x0002];
    volatile uint32_t OWA_BGR_REG;                    /*!< Offset 0xA2C OWA Bus Gating Reset Register */
             uint32_t reserved_0xA30 [0x0004];
    volatile uint32_t DMIC_CLK_REG;                   /*!< Offset 0xA40 DMIC Clock Register */
             uint32_t reserved_0xA44 [0x0002];
    volatile uint32_t DMIC_BGR_REG;                   /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
    volatile uint32_t AUDIO_CODEC_1X_CLK_REG;         /*!< Offset 0xA50 AUDIO CODEC 1X Clock Register */
    volatile uint32_t AUDIO_CODEC_4X_CLK_REG;         /*!< Offset 0xA54 AUDIO CODEC 4X Clock Register */
             uint32_t reserved_0xA58;
    volatile uint32_t AUDIO_CODEC_BGR_REG;            /*!< Offset 0xA5C AUDIO CODEC Bus Gating Reset Register */
    volatile uint32_t AUDIO_HUB_CLK_REG;              /*!< Offset 0xA60 AUDIO_HUB Clock Register */
             uint32_t reserved_0xA64 [0x0002];
    volatile uint32_t AUDIO_HUB_BGR_REG;              /*!< Offset 0xA6C AUDIO_HUB Bus Gating Reset Register */
    volatile uint32_t USB0_CLK_REG;                   /*!< Offset 0xA70 USB0 Clock Register */
    volatile uint32_t USB1_CLK_REG;                   /*!< Offset 0xA74 USB1 Clock Register */
    volatile uint32_t USB2_CLK_REG;                   /*!< Offset 0xA78 USB2 Clock Register */
    volatile uint32_t USB3_CLK_REG;                   /*!< Offset 0xA7C USB3 Clock Register */
             uint32_t reserved_0xA80 [0x0003];
    volatile uint32_t USB_BGR_REG;                    /*!< Offset 0xA8C USB Bus Gating Reset Register */
             uint32_t reserved_0xA90 [0x001C];
    volatile uint32_t HDMI0_CLK_REG;                  /*!< Offset 0xB00 HDMI0 Clock Register */
    volatile uint32_t HDMI0_SLOW_CLK_REG;             /*!< Offset 0xB04 HDMI0 Slow Clock Register */
             uint32_t reserved_0xB08 [0x0002];
    volatile uint32_t HDMI_CEC_CLK_REG;               /*!< Offset 0xB10 HDMI CEC Clock Register */
             uint32_t reserved_0xB14 [0x0002];
    volatile uint32_t HDMI_BGR_REG;                   /*!< Offset 0xB1C HDMI Bus Gating Reset Register */
             uint32_t reserved_0xB20 [0x000F];
    volatile uint32_t DISPLAY_IF_TOP_BGR_REG;         /*!< Offset 0xB5C DISPLAY_IF_TOP BUS GATING RESET Register */
    volatile uint32_t TCON_LCD0_CLK_REG;              /*!< Offset 0xB60 TCON LCD0 Clock Register */
    volatile uint32_t TCON_LCD1_CLK_REG;              /*!< Offset 0xB64 TCON LCD1 Clock Register */
             uint32_t reserved_0xB68 [0x0005];
    volatile uint32_t TCON_LCD_BGR_REG;               /*!< Offset 0xB7C TCON LCD BUS GATING RESET Register */
    volatile uint32_t TCON_TV0_CLK_REG;               /*!< Offset 0xB80 TCON TV0 Clock Register */
    volatile uint32_t TCON_TV1_CLK_REG;               /*!< Offset 0xB84 TCON TV1 Clock Register */
             uint32_t reserved_0xB88 [0x0005];
    volatile uint32_t TCON_TV_BGR_REG;                /*!< Offset 0xB9C TCON TV GATING RESET Register */
             uint32_t reserved_0xBA0 [0x0003];
    volatile uint32_t LVDS_BGR_REG;                   /*!< Offset 0xBAC LVDS BUS GATING RESET Register */
    volatile uint32_t TVE0_CLK_REG;                   /*!< Offset 0xBB0 TVE0 Clock Register */
             uint32_t reserved_0xBB4 [0x0002];
    volatile uint32_t TVE_BGR_REG;                    /*!< Offset 0xBBC TVE BUS GATING RESET Register */
             uint32_t reserved_0xBC0 [0x0011];
    volatile uint32_t CSI_TOP_CLK_REG;                /*!< Offset 0xC04 CSI TOP Clock Register */
    volatile uint32_t CSI_MST_CLK0_REG;               /*!< Offset 0xC08 CSI_Master Clock0 Register */
    volatile uint32_t CSI_MST_CLK1_REG;               /*!< Offset 0xC0C CSI_Master Clock1 Register */
             uint32_t reserved_0xC10 [0x0007];
    volatile uint32_t CSI_BGR_REG;                    /*!< Offset 0xC2C CSI Bus Gating Reset Register */
             uint32_t reserved_0xC30 [0x0004];
    volatile uint32_t HDMI_HDCP_CLK_REG;              /*!< Offset 0xC40 HDMI HDCP Clock Register */
             uint32_t reserved_0xC44 [0x0002];
    volatile uint32_t HDMI_HDCP_BGR_REG;              /*!< Offset 0xC4C HDMI HDCP Bus Gating Reset Register */
             uint32_t reserved_0xC50 [0x00AC];
    volatile uint32_t CCU_SEC_SWITCH_REG;             /*!< Offset 0xF00 CCU Security Switch Register */
    volatile uint32_t PLL_LOCK_DBG_CTRL_REG;          /*!< Offset 0xF04 PLL Lock Debug Control Register */
    volatile uint32_t FRE_DET_CTRL_REG;               /*!< Offset 0xF08 Frequency Detect Control Register */
    volatile uint32_t FRE_UP_LIM_REG;                 /*!< Offset 0xF0C Frequency Up Limit Register */
    volatile uint32_t FRE_DOWN_LIM_REG;               /*!< Offset 0xF10 Frequency Down Limit Register */
             uint32_t reserved_0xF14 [0x0003];
    volatile uint32_t CCU_24M_27M_CLK_OUTPUT_REG;     /*!< Offset 0xF20 24M or 27M Clock Output Register */
} CCU_TypeDef; /* size of structure = 0xF24 */
/*
 * @brief CE
 */
/*!< CE The Crypto Engine (CE) module */
typedef struct CE_Type
{
    volatile uint32_t CE_TDA;                         /*!< Offset 0x000 Task Descriptor Address */
             uint32_t reserved_0x004;
    volatile uint32_t CE_ICR;                         /*!< Offset 0x008 Interrupt Control Register */
    volatile uint32_t CE_ISR;                         /*!< Offset 0x00C Interrupt Status Register */
    volatile uint32_t CE_TLR;                         /*!< Offset 0x010 Task Load Register */
    volatile uint32_t CE_TSR;                         /*!< Offset 0x014 Task Status Register */
    volatile uint32_t CE_ESR;                         /*!< Offset 0x018 Error Status Register */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CE_SCSA;                        /*!< Offset 0x024 Symmetric Algorithm DMA Current Source Address */
    volatile uint32_t CE_SCDA;                        /*!< Offset 0x028 Symmetric Algorithm DMA Current Destination Address */
             uint32_t reserved_0x02C [0x0002];
    volatile uint32_t CE_HCSA;                        /*!< Offset 0x034 HASH Algorithm DMA Current Source Address */
    volatile uint32_t CE_HCDA;                        /*!< Offset 0x038 HASH Algorithm DMA Current Destination Address */
             uint32_t reserved_0x03C [0x0002];
    volatile uint32_t CE_ACSA;                        /*!< Offset 0x044 Asymmetric Algorithm DMA Current Source Address */
    volatile uint32_t CE_ACDA;                        /*!< Offset 0x048 Asymmetric Algorithm DMA Current Destination Address */
             uint32_t reserved_0x04C [0x0002];
    volatile uint32_t CE_XCSA;                        /*!< Offset 0x054 XTS Algorithm DMA Current Source Address */
    volatile uint32_t CE_XCDA;                        /*!< Offset 0x058 XTS Algorithm DMA Current Destination Address */
} CE_TypeDef; /* size of structure = 0x05C */
/*
 * @brief CPU_SUBSYS_CTRL_H616
 */
/*!< CPU_SUBSYS_CTRL_H616 H616 CPU Subsystem Control Register List */
typedef struct CPU_SUBSYS_CTRL_H616_Type
{
    volatile uint32_t GENER_CTRL_REG0;                /*!< Offset 0x000 General Control Register0 */
    volatile uint32_t GENER_CTRL_REG1;                /*!< Offset 0x004 General Control Register1 */
             uint32_t reserved_0x008;
    volatile uint32_t GIC_JTAG_RST_CTRL;              /*!< Offset 0x00C GIC and Jtag Reset Control Register */
    volatile uint32_t C0_INT_EN;                      /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
    volatile uint32_t IRQ_FIQ_STATUS;                 /*!< Offset 0x014 IRQ/FIQ Status Register */
    volatile uint32_t GENER_CTRL_REG2;                /*!< Offset 0x018 General Control Register2 */
    volatile uint32_t DBG_STATE;                      /*!< Offset 0x01C Debug State Register */
} CPU_SUBSYS_CTRL_H616_TypeDef; /* size of structure = 0x020 */
/*
 * @brief CPU_SUBSYS_CTRL_T507
 */
/*!< CPU_SUBSYS_CTRL_T507 T507 CPU Subsystem Control Register List */
typedef struct CPU_SUBSYS_CTRL_T507_Type
{
    volatile uint32_t GENER_CTRL_REG0;                /*!< Offset 0x000 General Control Register0 */
    volatile uint32_t GENER_CTRL_REG1;                /*!< Offset 0x004 General Control Register1 */
             uint32_t reserved_0x008;
    volatile uint32_t GIC_JTAG_RST_CTRL;              /*!< Offset 0x00C GIC and Jtag Reset Control Register */
    volatile uint32_t C0_INT_EN;                      /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
    volatile uint32_t IRQ_FIQ_STATUS;                 /*!< Offset 0x014 IRQ/FIQ Status Register */
    volatile uint32_t GENER_CTRL_REG2;                /*!< Offset 0x018 General Control Register2 */
    volatile uint32_t DBG_STATE;                      /*!< Offset 0x01C Debug State Register */
    volatile uint32_t CPUx_CTRL_REG [0x004];          /*!< Offset 0x020 CPU0..CPU3 Control Register */
             uint32_t reserved_0x030 [0x0004];
    struct
    {
        volatile uint32_t LOW;                        /*!< Offset 0x040 Reset Vector Base Address Registerx_L */
        volatile uint32_t HIGH;                       /*!< Offset 0x044 Reset Vector Base Address Registerx_H */
    } RVBARADDR [0x004];                              /*!< Offset 0x040 Reset Vector Base Address Register for core [0..3] */
} CPU_SUBSYS_CTRL_T507_TypeDef; /* size of structure = 0x060 */
/*
 * @brief DE_BLD
 */
/*!< DE_BLD Display Engine (DE) - Blender */
typedef struct DE_BLD_Type
{
    volatile uint32_t BLD_EN_COLOR_CTL;               /*!< Offset 0x000 BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
    struct
    {
        volatile uint32_t BLD_FILL_COLOR;             /*!< Offset 0x004 BLD fill color register */
        volatile uint32_t BLD_CH_ISIZE;               /*!< Offset 0x008 BLD input memory size register */
        volatile uint32_t BLD_CH_OFFSET;              /*!< Offset 0x00C BLD input memory offset register */
                 uint32_t reserved_0x00C;
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..5] */
             uint32_t reserved_0x064 [0x0007];
    volatile uint32_t ROUTE;                          /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    volatile uint32_t PREMULTIPLY;                    /*!< Offset 0x084 BLD pre-multiply control register */
    volatile uint32_t BKCOLOR;                        /*!< Offset 0x088  */
    volatile uint32_t OUTPUT_SIZE;                    /*!< Offset 0x08C  */
    volatile uint32_t BLD_MODE [0x006];               /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE blender0..blaener3 (or more) */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t CK_CTL;                         /*!< Offset 0x0B0  */
    volatile uint32_t CK_CFG;                         /*!< Offset 0x0B4  */
             uint32_t reserved_0x0B8 [0x0002];
    volatile uint32_t CK_MAX [0x004];                 /*!< Offset 0x0C0  */
             uint32_t reserved_0x0D0 [0x0004];
    volatile uint32_t CK_MIN [0x004];                 /*!< Offset 0x0E0  */
             uint32_t reserved_0x0F0 [0x0003];
    volatile uint32_t OUT_CTL;                        /*!< Offset 0x0FC  */
    volatile uint32_t CSC_CTL;                        /*!< Offset 0x100 SUN50I_MIXER_BLEND_CSC_CTL  */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t CSC_COEFF [0x00C];              /*!< Offset 0x110 SUN50I_MIXER_BLEND_CSC_COEFF(base, layer, x) ((base) + 0x110 + (layer)*0x30 + (x)*4) */
} DE_BLD_TypeDef; /* size of structure = 0x140 */
/*
 * @brief DE_BLS
 */
/*!< DE_BLS Blue Level Stretch (BLS) */
typedef struct DE_BLS_Type
{
    volatile uint32_t BLS_CTRL_REG;                   /*!< Offset 0x000 BLS module control register */
    volatile uint32_t BLS_SIZE_REG;                   /*!< Offset 0x004 BLS size register */
    volatile uint32_t BLS_WIN0_REG;                   /*!< Offset 0x008 BLS window setting register0 */
    volatile uint32_t BLS_WIN1_REG;                   /*!< Offset 0x00C BLS window setting register1 */
    volatile uint32_t BLS_ATTLUT_REG [0x004];         /*!< Offset 0x010 0x10+N*0x4 BLS attenuation LUT register, +N*0x4 (N = 0,1,2,3) */
    volatile uint32_t BLS_POS_REG;                    /*!< Offset 0x020 BLS blue zone position register */
             uint32_t reserved_0x024 [0x0003];
    volatile uint32_t BLS_GAINLUT_REG [0x004];        /*!< Offset 0x030 0x30+N*0x4 BLS GainLUT access register, +N*0x4, Total 16byte, 16*8bit (N = 0,1,2,3) */
} DE_BLS_TypeDef; /* size of structure = 0x040 */
/*
 * @brief DE_FCE
 */
/*!< DE_FCE Fresh and Contrast Enhancement (FCE) */
typedef struct DE_FCE_Type
{
    volatile uint32_t GCTRL_REG;                      /*!< Offset 0x000 Control register */
    volatile uint32_t FCE_SIZE_REG;                   /*!< Offset 0x004 Size setting register */
    volatile uint32_t FCE_WIN0_REG;                   /*!< Offset 0x008 Window setting 0 register */
    volatile uint32_t FCE_WIN1_REG;                   /*!< Offset 0x00C Window setting 1 register */
             uint32_t reserved_0x010 [0x0004];
    volatile uint32_t HIST_SUM_REG;                   /*!< Offset 0x020 Histogram sum register */
    volatile uint32_t HIST_STATUS_REG;                /*!< Offset 0x024 Histogram status register */
    volatile uint32_t CE_STATUS_REG;                  /*!< Offset 0x028 CE LUT status register */
    volatile uint32_t CE_CC_REG;                      /*!< Offset 0x02C CE chroma compensation function setting register */
    volatile uint32_t FTC_GAIN_REG;                   /*!< Offset 0x030 FTC gain setting register */
    volatile uint32_t FTD_HUE_THR_REG;                /*!< Offset 0x034 FTD hue threshold setting register */
    volatile uint32_t FTD_CHROMA_THR_REG;             /*!< Offset 0x038 FTD chroma threshold setting register */
    volatile uint32_t FTD_SLP_REG;                    /*!< Offset 0x03C FTD slop setting register */
    volatile uint32_t CSC_ENABLE_REG;                 /*!< Offset 0x040 CSC enable setting register */
    volatile uint32_t CSC_D0_REG;                     /*!< Offset 0x044 CSC Constant D0 Register */
    volatile uint32_t CSC_D1_REG;                     /*!< Offset 0x048 CSC Constant D1 Register */
    volatile uint32_t CSC_D2_REG;                     /*!< Offset 0x04C CSC Constant D2 Register */
    volatile uint32_t CSC_C00_REG;                    /*!< Offset 0x050 CSC Coefficient 00 Register */
    volatile uint32_t CSC_C01_REG;                    /*!< Offset 0x054 CSC Coefficient 01 Register */
    volatile uint32_t CSC_C02_REG;                    /*!< Offset 0x058 CSC Coefficient 02 Register */
    volatile uint32_t CSC_C03_REG;                    /*!< Offset 0x05C CSC Constant 03 Register */
    volatile uint32_t CSC_C10_REG;                    /*!< Offset 0x060 CSC Coefficient 10 Register */
    volatile uint32_t CSC_C11_REG;                    /*!< Offset 0x064 CSC Coefficient 11 Register */
    volatile uint32_t CSC_C12_REG;                    /*!< Offset 0x068 CSC Coefficient 12 Register */
    volatile uint32_t CSC_C13_REG;                    /*!< Offset 0x06C CSC Constant 13 Register */
    volatile uint32_t CSC_C20_REG;                    /*!< Offset 0x070 CSC Coefficient 20 Register */
    volatile uint32_t CSC_C21_REG;                    /*!< Offset 0x074 CSC Coefficient 21 Register */
    volatile uint32_t CSC_C22_REG;                    /*!< Offset 0x078 CSC Coefficient 22 Register */
    volatile uint32_t CSC_C23_REG;                    /*!< Offset 0x07C CSC Constant 23 Register */
             uint32_t reserved_0x080 [0x0060];
    volatile uint32_t CE_LUT_REGN [0x080];            /*!< Offset 0x200 0x200+N*4 CE LUT register N (N=0:127) */
    volatile uint32_t HIST_CNT_REGN [0x100];          /*!< Offset 0x400 0x400+N*4 Histogram count register N (N=0:255) */
} DE_FCE_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_GLB
 */
/*!< DE_GLB Display Engine (DE) - Global Control */
typedef struct DE_GLB_Type
{
    volatile uint32_t GLB_CTL;                        /*!< Offset 0x000 Global control register */
    volatile uint32_t GLB_STS;                        /*!< Offset 0x004 Global status register */
    volatile uint32_t GLB_SIZE;                       /*!< Offset 0x008 Global size register */
    volatile uint32_t GLB_CLK;                        /*!< Offset 0x00C Global clock register */
    volatile uint32_t GLB_DBUFFER;                    /*!< Offset 0x010 Global double buffer control register */
} DE_GLB_TypeDef; /* size of structure = 0x014 */
/*
 * @brief DE_TOP
 */
/*!< DE_TOP Display Engine (DE) TOP (APB) */
typedef struct DE_TOP_Type
{
    volatile uint32_t DE_SCLK_GATE;                   /*!< Offset 0x000 DE SCLK Gating Register */
    volatile uint32_t DE_HCLK_GATE;                   /*!< Offset 0x004 DE HCLK Gating Register */
    volatile uint32_t DE_AHB_RESET;                   /*!< Offset 0x008 DE AHB Reset Register */
    volatile uint32_t DE_SCLK_DIV;                    /*!< Offset 0x00C DE SCLK Division Register */
    volatile uint32_t DE2TCON_MUX;                    /*!< Offset 0x010 DE MUX Register */
    volatile uint32_t DE_CMD;                         /*!< Offset 0x014 DE CMD Register */
             uint32_t reserved_0x018;
    volatile uint32_t DE_BIST_CTL;                    /*!< Offset 0x01C DE Bist Control Register */
             uint32_t reserved_0x020;
    volatile uint32_t DE_IP_CFG;                      /*!< Offset 0x024 DE IP Configure Register */
} DE_TOP_TypeDef; /* size of structure = 0x028 */
/*
 * @brief DE_UI
 */
/*!< DE_UI Display Engine (DE) - UI surface */
typedef struct DE_UI_Type
{
    struct
    {
        volatile uint32_t ATTR;                       /*!< Offset 0x000  */
        volatile uint32_t SIZE;                       /*!< Offset 0x004  */
        volatile uint32_t COORD;                      /*!< Offset 0x008  */
        volatile uint32_t PITCH;                      /*!< Offset 0x00C  */
        volatile uint32_t TOP_LADDR;                  /*!< Offset 0x010  */
        volatile uint32_t BOT_LADDR;                  /*!< Offset 0x014  */
        volatile uint32_t FCOLOR;                     /*!< Offset 0x018  */
                 uint32_t reserved_0x01C;
    } CFG [0x004];                                    /*!< Offset 0x000  */
    volatile uint32_t TOP_HADDR;                      /*!< Offset 0x080  */
    volatile uint32_t BOT_HADDR;                      /*!< Offset 0x084  */
    volatile uint32_t OVL_SIZE;                       /*!< Offset 0x088  */
} DE_UI_TypeDef; /* size of structure = 0x08C */
/*
 * @brief DE_VI
 */
/*!< DE_VI Display Engine (DE) - VI surface */
typedef struct DE_VI_Type
{
    struct
    {
        volatile uint32_t ATTR;                       /*!< Offset 0x000  */
        volatile uint32_t SIZE;                       /*!< Offset 0x004  */
        volatile uint32_t COORD;                      /*!< Offset 0x008  */
        volatile uint32_t PITCH [0x003];              /*!< Offset 0x00C ix=0: Y, ix=1: U/UV channel, ix=3: V channel  */
        volatile uint32_t TOP_LADDR [0x003];          /*!< Offset 0x018  */
        volatile uint32_t BOT_LADDR [0x003];          /*!< Offset 0x024  */
    } CFG [0x004];                                    /*!< Offset 0x000  */
    volatile uint32_t FCOLOR [0x004];                 /*!< Offset 0x0C0  */
    volatile uint32_t TOP_HADDR [0x003];              /*!< Offset 0x0D0  */
    volatile uint32_t BOT_HADDR [0x003];              /*!< Offset 0x0DC  */
    volatile uint32_t OVL_SIZE [0x002];               /*!< Offset 0x0E8 OVL_V overlay window size register */
    volatile uint32_t HORI [0x002];                   /*!< Offset 0x0F0 OVL_V horizontal down sample control register */
    volatile uint32_t VERT [0x002];                   /*!< Offset 0x0F8 OVL_V vertical down sample control register */
             uint32_t reserved_0x100 [0x0080];
    volatile uint32_t FBD_V_CTL;                      /*!< Offset 0x300 OVL_V FBD control register */
} DE_VI_TypeDef; /* size of structure = 0x304 */
/*
 * @brief DE_VSU
 */
/*!< DE_VSU Video Scaler Unit (VSU) */
typedef struct DE_VSU_Type
{
    volatile uint32_t VSU_CTRL_REG;                   /*!< Offset 0x000 VSU Module Control Register */
             uint32_t reserved_0x004;
    volatile uint32_t VSU_STATUS_REG;                 /*!< Offset 0x008 VSU Status Register */
    volatile uint32_t VSU_FIELD_CTRL_REG;             /*!< Offset 0x00C VSU Field Control Register */
    volatile uint32_t VSU_SCALE_MODE_REG;             /*!< Offset 0x010 VSU Scale Mode Setting Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t VSU_DIRECTION_THR_REG;          /*!< Offset 0x020 VSU Direction Detection Threshold Register */
    volatile uint32_t VSU_EDGE_THR_REG;               /*!< Offset 0x024 VSU Edge Detection Setting Register */
    volatile uint32_t VSU_EDSCALER_CTRL_REG;          /*!< Offset 0x028 VSU Edge-Direction Scaler Control Register */
    volatile uint32_t VSU_ANGLE_THR_REG;              /*!< Offset 0x02C VSU Angle Reliability Setting Register */
    volatile uint32_t VSU_SHARP_EN_REG;               /*!< Offset 0x030 VSU Sharpness Control Enable Register */
    volatile uint32_t VSU_SHARP_CORING_REG;           /*!< Offset 0x034 VSU Sharpness Control Coring Setting Register */
    volatile uint32_t VSU_SHARP_GAIN0_REG;            /*!< Offset 0x038 VSU Sharpness Control Gain Setting 0 Register */
    volatile uint32_t VSU_SHARP_GAIN1_REG;            /*!< Offset 0x03C VSU Sharpness Control Gain Setting 1 Register */
    volatile uint32_t VSU_OUT_SIZE_REG;               /*!< Offset 0x040 VSU Output Size Register */
    volatile uint32_t VSU_GLOBAL_ALPHA_REG;           /*!< Offset 0x044 (null) */
             uint32_t reserved_0x048 [0x000E];
    volatile uint32_t VSU_Y_SIZE_REG;                 /*!< Offset 0x080 VSU Y Channel Size Register */
             uint32_t reserved_0x084;
    volatile uint32_t VSU_Y_HSTEP_REG;                /*!< Offset 0x088 VSU Y Channel Horizontal Step Register */
    volatile uint32_t VSU_Y_VSTEP_REG;                /*!< Offset 0x08C VSU Y Channel Vertical Step Register */
    volatile uint32_t VSU_Y_HPHASE_REG;               /*!< Offset 0x090 VSU Y Channel Horizontal Initial Phase Register */
             uint32_t reserved_0x094;
    volatile uint32_t VSU_Y_VPHASE0_REG;              /*!< Offset 0x098 VSU Y Channel Vertical Initial Phase 0 Register */
    volatile uint32_t VSU_Y_VPHASE1_REG;              /*!< Offset 0x09C VSU Y Channel Vertical Initial Phase 1 Register */
             uint32_t reserved_0x0A0 [0x0008];
    volatile uint32_t VSU_C_SIZE_REG;                 /*!< Offset 0x0C0 VSU C Channel Size Register */
             uint32_t reserved_0x0C4;
    volatile uint32_t VSU_C_HSTEP_REG;                /*!< Offset 0x0C8 VSU C Channel Horizontal Step Register */
    volatile uint32_t VSU_C_VSTEP_REG;                /*!< Offset 0x0CC VSU C Channel Vertical Step Register */
    volatile uint32_t VSU_C_HPHASE_REG;               /*!< Offset 0x0D0 VSU C Channel Horizontal Initial Phase Register */
             uint32_t reserved_0x0D4;
    volatile uint32_t VSU_C_VPHASE0_REG;              /*!< Offset 0x0D8 VSU C Channel Vertical Initial Phase 0 Register */
    volatile uint32_t VSU_C_VPHASE1_REG;              /*!< Offset 0x0DC VSU C Channel Vertical Initial Phase 1 Register */
             uint32_t reserved_0x0E0 [0x0048];
    volatile uint32_t VSU_Y_HCOEF0_REGN [0x020];      /*!< Offset 0x200 0x200+N*4 VSU Y Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
             uint32_t reserved_0x280 [0x0020];
    volatile uint32_t VSU_Y_HCOEF1_REGN [0x020];      /*!< Offset 0x300 0x300+N*4 VSU Y Channel Horizontal Filter Coefficient1 Register N N = M 1 */
             uint32_t reserved_0x380 [0x0020];
    volatile uint32_t VSU_Y_VCOEF_REGN [0x020];       /*!< Offset 0x400 0x400+N*4 VSU Y Channel Vertical Filter Coefficient Register N N = M 1)) */
             uint32_t reserved_0x480 [0x0060];
    volatile uint32_t VSU_C_HCOEF0_REGN [0x020];      /*!< Offset 0x600 0x600+N*4 VSU C Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
             uint32_t reserved_0x680 [0x0020];
    volatile uint32_t VSU_C_HCOEF1_REGN [0x020];      /*!< Offset 0x700 0x700+N*4 VSU C Channel Horizontal Filter Co efficient1 Register N N = M 1)) */
             uint32_t reserved_0x780 [0x0020];
    volatile uint32_t VSU_C_VCOEF_REGN [0x020];       /*!< Offset 0x800 0x800+N*4 VSU C Channel Vertical Filter Coefficient Register N N = M 1)) */
} DE_VSU_TypeDef; /* size of structure = 0x880 */
/*
 * @brief DE_XX
 */
/*!< DE_XX  */
typedef struct DE_XX_Type
{
             uint32_t reserved_0x000 [0x0039];
    volatile uint32_t ADDR [0x003];                   /*!< Offset 0x0E4 какое-то поле 32 бит */
    volatile uint32_t FLAGS;                          /*!< Offset 0x0F0 какое-то поле с работающими битами 0x00030010 (17, 16, 4) */
    volatile uint32_t VALUE1;                         /*!< Offset 0x0F4 какое-то поле 32 бит */
    volatile uint32_t FLAGS2;                         /*!< Offset 0x0F8 какое-то поле с работающими битами 0x000083FC (15, 9,8, 7..2) */
    volatile uint32_t VALUE2;                         /*!< Offset 0x0FC какое-то поле 32 бит */
} DE_XX_TypeDef; /* size of structure = 0x100 */
/*
 * @brief DISP_IF_TOP
 */
/*!< DISP_IF_TOP display interface top (DISP_IF_TOP) */
typedef struct DISP_IF_TOP_Type
{
    volatile uint32_t TV_CLK_SRC_RGB_SRC;             /*!< Offset 0x000 TCON_TV CClock SSelect and RGB Source S Select Register */
             uint32_t reserved_0x004 [0x0006];
    volatile uint32_t DE_PORT_PERH_SEL;               /*!< Offset 0x01C DE Source Select TCON R Register */
    volatile uint32_t MODULE_GATING;                  /*!< Offset 0x020 TCON Output to LVDS/DSI/TVE/HDMI Gating RRegister */
} DISP_IF_TOP_TypeDef; /* size of structure = 0x024 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
    volatile uint32_t DMAC_IRQ_EN_REG0;               /*!< Offset 0x000 DMAC IRQ Enable Register 0 */
    volatile uint32_t DMAC_IRQ_EN_REG1;               /*!< Offset 0x004 DMAC IRQ Enable Register 1 */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t DMAC_IRQ_PEND_REG0;             /*!< Offset 0x010 DMAC IRQ Pending Register 0 */
    volatile uint32_t DMAC_IRQ_PEND_REG1;             /*!< Offset 0x014 DMAC IRQ Pending Register 1 */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t DMAC_SEC_REG;                   /*!< Offset 0x020 DMA Security Register */
             uint32_t reserved_0x024;
    volatile uint32_t DMAC_AUTO_GATE_REG;             /*!< Offset 0x028 DMAC Auto Gating Register */
             uint32_t reserved_0x02C;
    volatile uint32_t DMAC_STA_REG;                   /*!< Offset 0x030 DMAC Status Register */
             uint32_t reserved_0x034 [0x0033];
    struct
    {
        volatile uint32_t DMAC_EN_REGN;               /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 15) 0x0100 + N*0x0040 */
        volatile uint32_t DMAC_PAU_REGN;              /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 15) 0x0104 + N*0x0040 */
        volatile uint32_t DMAC_DESC_ADDR_REGN;        /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 15) 0x0108 + N*0x0040 */
        volatile uint32_t DMAC_CFG_REGN;              /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 15) 0x010C + N*0x0040 */
        volatile uint32_t DMAC_CUR_SRC_REGN;          /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 15) 0x0110 + N*0x0040 */
        volatile uint32_t DMAC_CUR_DEST_REGN;         /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 15) 0x0114 + N*0x0040 */
        volatile uint32_t DMAC_BCNT_LEFT_REGN;        /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 15) 0x0118 + N*0x0040 */
        volatile uint32_t DMAC_PARA_REGN;             /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 15) 0x011C + N*0x0040 */
                 uint32_t reserved_0x020 [0x0002];
        volatile uint32_t DMAC_MODE_REGN;             /*!< Offset 0x128 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
        volatile uint32_t DMAC_FDESC_ADDR_REGN;       /*!< Offset 0x12C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
        volatile uint32_t DMAC_PKG_NUM_REGN;          /*!< Offset 0x130 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
                 uint32_t reserved_0x034 [0x0003];
    } CH [0x010];                                     /*!< Offset 0x100 Channel [0..15] */
} DMAC_TypeDef; /* size of structure = 0x500 */
/*
 * @brief DMIC
 */
/*!< DMIC Digital Microphone Interface */
typedef struct DMIC_Type
{
    volatile uint32_t DMIC_EN;                        /*!< Offset 0x000 DMIC Enable Control Register */
    volatile uint32_t DMIC_SR;                        /*!< Offset 0x004 DMIC Sample Rate Register */
    volatile uint32_t DMIC_CTR;                       /*!< Offset 0x008 DMIC Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t DMIC_DATA;                      /*!< Offset 0x010 DMIC Data Register */
    volatile uint32_t DMIC_INTC;                      /*!< Offset 0x014 MIC Interrupt Control Register */
    volatile uint32_t DMIC_INTS;                      /*!< Offset 0x018 DMIC Interrupt Status Register */
    volatile uint32_t DMIC_RXFIFO_CTR;                /*!< Offset 0x01C DMIC RX FIFO Control Register */
    volatile uint32_t DMIC_RXFIFO_STA;                /*!< Offset 0x020 DMIC RX FIFO Status Register */
    volatile uint32_t DMIC_CH_NUM;                    /*!< Offset 0x024 DMIC Channel Numbers Register */
    volatile uint32_t DMIC_CH_MAP;                    /*!< Offset 0x028 DMIC Channel Mapping Register */
    volatile uint32_t DMIC_CNT;                       /*!< Offset 0x02C DMIC Counter Register */
    volatile uint32_t DATA0_DATA1_VOL_CTR;            /*!< Offset 0x030 Data 0 and Data 1 Volume Control Register */
    volatile uint32_t DATA2_DATA3_VOL_CTR;            /*!< Offset 0x034 Data 2 and Data 3 Volume Control Register */
    volatile uint32_t HPF_EN_CTR;                     /*!< Offset 0x038 High Pass Filter Enable Control Register */
    volatile uint32_t HPF_COEF_REG;                   /*!< Offset 0x03C High Pass Filter Coef Register */
    volatile uint32_t HPF_GAIN_REG;                   /*!< Offset 0x040 High Pass Filter Gain Register */
} DMIC_TypeDef; /* size of structure = 0x044 */
/*
 * @brief EMAC
 */
/*!< EMAC The Ethernet Medium Access Controller (EMAC) enables a host to transmi */
typedef struct EMAC_Type
{
    volatile uint32_t EMAC_BASIC_CTL0;                /*!< Offset 0x000 EMAC Basic Control Register0 */
    volatile uint32_t EMAC_BASIC_CTL1;                /*!< Offset 0x004 EMAC Basic Control Register1 */
    volatile uint32_t EMAC_INT_STA;                   /*!< Offset 0x008 EMAC Interrupt Status Register */
    volatile uint32_t EMAC_INT_EN;                    /*!< Offset 0x00C EMAC Interrupt Enable Register */
    volatile uint32_t EMAC_TX_CTL0;                   /*!< Offset 0x010 EMAC Transmit Control Register0 */
    volatile uint32_t EMAC_TX_CTL1;                   /*!< Offset 0x014 (null) */
             uint32_t reserved_0x018;
    volatile uint32_t EMAC_TX_FLOW_CTL;               /*!< Offset 0x01C EMAC Transmit Flow Control Register */
    volatile uint32_t EMAC_TX_DMA_DESC_LIST;          /*!< Offset 0x020 EMAC Transmit Descriptor List Address Register */
    volatile uint32_t EMAC_RX_CTL0;                   /*!< Offset 0x024 EMAC Receive Control Register0 */
    volatile uint32_t EMAC_RX_CTL1;                   /*!< Offset 0x028 EMAC Receive Control Register1 */
             uint32_t reserved_0x02C [0x0002];
    volatile uint32_t EMAC_RX_DMA_DESC_LIST;          /*!< Offset 0x034 EMAC Receive Descriptor List Address Register */
    volatile uint32_t EMAC_RX_FRM_FLT;                /*!< Offset 0x038 EMAC Receive Frame Filter Register */
             uint32_t reserved_0x03C;
    volatile uint32_t EMAC_RX_HASH0;                  /*!< Offset 0x040 EMAC Hash Table Register0 */
    volatile uint32_t EMAC_RX_HASH1;                  /*!< Offset 0x044 EMAC Hash Table Register1 */
    volatile uint32_t EMAC_MII_CMD;                   /*!< Offset 0x048 EMAC Management Interface Command Register */
    volatile uint32_t EMAC_MII_DATA;                  /*!< Offset 0x04C EMAC Management Interface Data Register */
    struct
    {
        volatile uint32_t HIGH;                       /*!< Offset 0x050 EMAC MAC Address High Register */
        volatile uint32_t LOW;                        /*!< Offset 0x054 EMAC MAC Address Low Register */
    } EMAC_ADDR [0x008];                              /*!< Offset 0x050 EMAC MAC Address N (N=0-7) */
             uint32_t reserved_0x090 [0x0008];
    volatile uint32_t EMAC_TX_DMA_STA;                /*!< Offset 0x0B0 EMAC Transmit DMA Status Register */
    volatile uint32_t EMAC_TX_CUR_DESC;               /*!< Offset 0x0B4 EMAC Current Transmit Descriptor Register */
    volatile uint32_t EMAC_TX_CUR_BUF;                /*!< Offset 0x0B8 EMAC Current Transmit Buffer Address Register */
             uint32_t reserved_0x0BC;
    volatile uint32_t EMAC_RX_DMA_STA;                /*!< Offset 0x0C0 EMAC Receive DMA Status Register */
    volatile uint32_t EMAC_RX_CUR_DESC;               /*!< Offset 0x0C4 EMAC Current Receive Descriptor Register */
    volatile uint32_t EMAC_RX_CUR_BUF;                /*!< Offset 0x0C8 EMAC Current Receive Buffer Address Register */
             uint32_t reserved_0x0CC;
    volatile uint32_t EMAC_RGMII_STA;                 /*!< Offset 0x0D0 EMAC RGMII Status Register */
             uint32_t reserved_0x0D4 [0x3FCB];
} EMAC_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief G2D_MIXER
 */
/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer */
typedef struct G2D_MIXER_Type
{
    volatile uint32_t G2D_MIXER_CTRL;                 /*!< Offset 0x000  */
    volatile uint32_t G2D_MIXER_INTERRUPT;            /*!< Offset 0x004  */
} G2D_MIXER_TypeDef; /* size of structure = 0x008 */
/*
 * @brief G2D_ROT
 */
/*!< G2D_ROT Graphic 2D Rotate */
typedef struct G2D_ROT_Type
{
    volatile uint32_t ROT_CTL;                        /*!< Offset 0x000 ROT_CTL						 */
    volatile uint32_t ROT_INT;                        /*!< Offset 0x004 ROT_INT				 */
    volatile uint32_t ROT_TIMEOUT;                    /*!< Offset 0x008 ROT_TIMEOUT */
             uint32_t reserved_0x00C [0x0005];
    volatile uint32_t ROT_IFMT;                       /*!< Offset 0x020 Input data attribute register */
    volatile uint32_t ROT_ISIZE;                      /*!< Offset 0x024 Input data size register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t ROT_IPITCH0;                    /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    volatile uint32_t ROT_IPITCH1;                    /*!< Offset 0x034 Input U/UV memory pitch register */
    volatile uint32_t ROT_IPITCH2;                    /*!< Offset 0x038 Input V memory pitch register */
             uint32_t reserved_0x03C;
    volatile uint32_t ROT_ILADD0;                     /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    volatile uint32_t ROT_IHADD0;                     /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    volatile uint32_t ROT_ILADD1;                     /*!< Offset 0x048 Input U/UV memory address register0 */
    volatile uint32_t ROT_IHADD1;                     /*!< Offset 0x04C Input U/UV memory address register1 */
    volatile uint32_t ROT_ILADD2;                     /*!< Offset 0x050 Input V memory address register0 */
    volatile uint32_t ROT_IHADD2;                     /*!< Offset 0x054 Input V memory address register1 */
             uint32_t reserved_0x058 [0x000B];
    volatile uint32_t ROT_OSIZE;                      /*!< Offset 0x084 Output data size register */
             uint32_t reserved_0x088 [0x0002];
    volatile uint32_t ROT_OPITCH0;                    /*!< Offset 0x090 Output Y/RGB/ARGB memory pitch register */
    volatile uint32_t ROT_OPITCH1;                    /*!< Offset 0x094 Output U/UV memory pitch register */
    volatile uint32_t ROT_OPITCH2;                    /*!< Offset 0x098 Output V memory pitch register */
             uint32_t reserved_0x09C;
    volatile uint32_t ROT_OLADD0;                     /*!< Offset 0x0A0 Output Y/RGB/ARGB memory address register0 */
    volatile uint32_t ROT_OHADD0;                     /*!< Offset 0x0A4 Output Y/RGB/ARGB memory address register1 */
    volatile uint32_t ROT_OLADD1;                     /*!< Offset 0x0A8 Output U/UV memory address register0 */
    volatile uint32_t ROT_OHADD1;                     /*!< Offset 0x0AC Output U/UV memory address register1 */
    volatile uint32_t ROT_OLADD2;                     /*!< Offset 0x0B0 Output V memory address register0 */
    volatile uint32_t ROT_OHADD2;                     /*!< Offset 0x0B4 Output V memory address register1 */
             uint32_t reserved_0x0B8 [0x0004];
    volatile uint32_t LBC_ENC_CTL;                    /*!< Offset 0x0C8 Encode control (bit 31 - is lossy) */
    volatile uint32_t LBC_CTL;                        /*!< Offset 0x0CC  */
    volatile uint32_t LBC_DEC_CTL;                    /*!< Offset 0x0D0 Decode control (bit 31 - is lossy)  */
} G2D_ROT_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_TOP
 */
/*!< G2D_TOP Graphic 2D top */
typedef struct G2D_TOP_Type
{
    volatile uint32_t G2D_SCLK_GATE;                  /*!< Offset 0x000  */
    volatile uint32_t G2D_HCLK_GATE;                  /*!< Offset 0x004  */
    volatile uint32_t G2D_AHB_RST;                    /*!< Offset 0x008  */
    volatile uint32_t G2D_SCLK_DIV;                   /*!< Offset 0x00C  */
    volatile uint32_t G2D_VERSION;                    /*!< Offset 0x010  */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t RCQ_IRQ_CTL;                    /*!< Offset 0x020  */
    volatile uint32_t RCQ_STATUS;                     /*!< Offset 0x024  */
    volatile uint32_t RCQ_CTRL;                       /*!< Offset 0x028  */
    volatile uint32_t RCQ_HEADER_LOW_ADDR;            /*!< Offset 0x02C Register Configuration Queue Header bits 31..0 */
    volatile uint32_t RCQ_HEADER_HIGH_ADDR;           /*!< Offset 0x030 Register Configuration Queue Header bits 39..32 */
    volatile uint32_t RCQ_HEADER_LEN;                 /*!< Offset 0x034 Register Configuration Queue length bits 15..0 */
} G2D_TOP_TypeDef; /* size of structure = 0x038 */
/*
 * @brief GPADC
 */
/*!< GPADC  */
typedef struct GPADC_Type
{
    volatile uint32_t GP_SR_CON;                      /*!< Offset 0x000 GPADC Sample Rate Configure Register */
    volatile uint32_t GP_CTRL;                        /*!< Offset 0x004 GPADC Control Register */
    volatile uint32_t GP_CS_EN;                       /*!< Offset 0x008 GPADC */
    volatile uint32_t GP_FIFO_INTC;                   /*!< Offset 0x00C GPADC FIFO Interrupt Control Register */
    volatile uint32_t GP_FIFO_INTS;                   /*!< Offset 0x010 GPADC FIFO Interrupt Status Register */
    volatile uint32_t GP_FIFO_DATA;                   /*!< Offset 0x014 GPADC FIFO Data Register */
    volatile uint32_t GP_CDATA;                       /*!< Offset 0x018 GPADC Calibration Data Register */
             uint32_t reserved_0x01C;
    volatile uint32_t GP_DATAL_INTC;                  /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
    volatile uint32_t GP_DATAH_INTC;                  /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
    volatile uint32_t GP_DATA_INTC;                   /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
             uint32_t reserved_0x02C;
    volatile uint32_t GP_DATAL_INTS;                  /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
    volatile uint32_t GP_DATAH_INTS;                  /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
    volatile uint32_t GP_DATA_INTS;                   /*!< Offset 0x038 GPADC Data Interrupt Status Register */
             uint32_t reserved_0x03C;
    volatile uint32_t GP_CH0_CMP_DATA;                /*!< Offset 0x040 GPADC CH0 Compare Data Register */
    volatile uint32_t GP_CH1_CMP_DATA;                /*!< Offset 0x044 GPADC CH1 Compare Data Register */
    volatile uint32_t GP_CH2_CMP_DATA;                /*!< Offset 0x048 GPADC CH2 Compare Data Register */
    volatile uint32_t GP_CH3_CMP_DATA;                /*!< Offset 0x04C GPADC CH3 Compare Data Register */
             uint32_t reserved_0x050 [0x000C];
    volatile uint32_t GP_CH0_DATA;                    /*!< Offset 0x080 GPADC CH0 Data Register */
    volatile uint32_t GP_CH1_DATA;                    /*!< Offset 0x084 GPADC CH1 Data Register */
    volatile uint32_t GP_CH2_DATA;                    /*!< Offset 0x088 GPADC CH2 Data Register */
    volatile uint32_t GP_CH3_DATA;                    /*!< Offset 0x08C GPADC CH3 Data Register */
} GPADC_TypeDef; /* size of structure = 0x090 */
/*
 * @brief GPIO
 */
/*!< GPIO Port Controller */
typedef struct GPIO_Type
{
    volatile uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
    volatile uint32_t DATA;                           /*!< Offset 0x010 Data Register */
    volatile uint32_t DRV [0x002];                    /*!< Offset 0x014 Multi_Driving Register */
    volatile uint32_t PULL [0x002];                   /*!< Offset 0x01C Pull Register */
} GPIO_TypeDef; /* size of structure = 0x024 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK Port Controller */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        volatile uint32_t CFG [0x004];                /*!< Offset 0x000 Configure Register */
        volatile uint32_t DATA;                       /*!< Offset 0x010 Data Register */
        volatile uint32_t DRV [0x002];                /*!< Offset 0x014 Multi_Driving Register */
        volatile uint32_t PULL [0x002];               /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x009];                              /*!< Offset 0x000 GPIO pin control */
             uint32_t reserved_0x144 [0x002F];
    struct
    {
        volatile uint32_t EINT_CFG [0x004];           /*!< Offset 0x200 External Interrupt Configure Registers */
        volatile uint32_t EINT_CTL;                   /*!< Offset 0x210 External Interrupt Control Register */
        volatile uint32_t EINT_STATUS;                /*!< Offset 0x214 External Interrupt Status Register */
        volatile uint32_t EINT_DEB;                   /*!< Offset 0x218 External Interrupt Debounce Register */
                 uint32_t reserved_0x01C;
    } GPIO_INTS [0x009];                              /*!< Offset 0x200 GPIO interrupt control */
             uint32_t reserved_0x320 [0x0008];
    volatile uint32_t PIO_POW_MOD_SEL;                /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    volatile uint32_t PIO_POW_MS_CTL;                 /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    volatile uint32_t PIO_POW_VAL;                    /*!< Offset 0x348 PIO Group Power Value Register */
} GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
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
 * @brief I2S_PCM
 */
/*!< I2S_PCM  */
typedef struct I2S_PCM_Type
{
    volatile uint32_t I2Sn_CTL;                       /*!< Offset 0x000 I2Sn Control */
    volatile uint32_t I2Sn_FMT0;                      /*!< Offset 0x004 I2Sn Format 0 */
    volatile uint32_t I2Sn_FMT1;                      /*!< Offset 0x008 I2Sn Format 1 */
    volatile uint32_t I2Sn_CLKD;                      /*!< Offset 0x00C I2Sn Clock Divide */
             uint32_t reserved_0x010 [0x0004];
    volatile uint32_t I2Sn_RXDIF_CONT;                /*!< Offset 0x020 I2Sn RXDIF Contact Select */
    volatile uint32_t I2Sn_CHCFG;                     /*!< Offset 0x024 I2Sn Channel Configuration */
    volatile uint32_t I2Sn_IRQ_CTRL;                  /*!< Offset 0x028 I2Sn DMA & Interrupt Control */
    volatile uint32_t I2Sn_IRQ_STS;                   /*!< Offset 0x02C I2Sn DMA & Interrupt Status */
    struct
    {
        volatile uint32_t I2Sn_SDOUTm_SLOTCTR;        /*!< Offset 0x030 (n=0~3)(m=0~3) */
        volatile uint32_t I2Sn_SDOUTmCHMAP0;          /*!< Offset 0x034 I2Sn SDOUTm Channel Mapping 0 */
        volatile uint32_t I2Sn_SDOUTmCHMAP1;          /*!< Offset 0x038 I2Sn SDOUTm Channel Mapping 1 */
                 uint32_t reserved_0x00C;
    } I2Sn_SDOUT [0x004];                             /*!< Offset 0x030 SDOUTm (m=0~3) */
    volatile uint32_t I2Sn_SDIN_SLOTCTR;              /*!< Offset 0x070 I2Sn Input Slot Control */
    volatile uint32_t I2Sn_SDINCHMAP [0x004];         /*!< Offset 0x074 I2Sn SDIN Channel Mapping 0..3 */
             uint32_t reserved_0x084 [0x001F];
} I2S_PCM_TypeDef; /* size of structure = 0x100 */
/*
 * @brief IOMMU
 */
/*!< IOMMU IOMMU (I/O Memory management unit) */
typedef struct IOMMU_Type
{
             uint32_t reserved_0x000 [0x0004];
    volatile uint32_t IOMMU_RESET_REG;                /*!< Offset 0x010 IOMMU ResetRegister */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t IOMMU_ENABLE_REG;               /*!< Offset 0x020 IOMMU EnableRegister */
             uint32_t reserved_0x024 [0x0003];
    volatile uint32_t IOMMU_BYPASS_REG;               /*!< Offset 0x030 IOMMU BypassRegister */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t IOMMU_AUTO_GATING_REG;          /*!< Offset 0x040 IOMMU Auto GatingRegister */
    volatile uint32_t IOMMU_WBUF_CTRL_REG;            /*!< Offset 0x044 IOMMU Write Buffer Control Register */
    volatile uint32_t IOMMU_OOO_CTRL_REG;             /*!< Offset 0x048 IOMMU Out Of Order Control Register */
    volatile uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;     /*!< Offset 0x04C IOMMU 4KB Boundary Protect Control Register */
    volatile uint32_t IOMMU_TTB_REG;                  /*!< Offset 0x050 IOMMU Translation Table BaseRegister */
             uint32_t reserved_0x054 [0x0003];
    volatile uint32_t IOMMU_TLB_ENABLE_REG;           /*!< Offset 0x060 IOMMU TLB EnableRegister */
             uint32_t reserved_0x064 [0x0003];
    volatile uint32_t IOMMU_TLB_PREFETCH_REG;         /*!< Offset 0x070 IOMMU TLB PrefetchRegister */
             uint32_t reserved_0x074 [0x0003];
    volatile uint32_t IOMMU_TLB_FLUSH_ENABLE_REG;     /*!< Offset 0x080 IOMMU TLB Flush Enable Register */
    volatile uint32_t IOMMU_TLB_IVLD_MODE_SEL_REG;    /*!< Offset 0x084 IOMMU TLB Invalidation Mode Select Register */
    volatile uint32_t IOMMU_TLB_IVLD_STA_ADDR_REG;    /*!< Offset 0x088 IOMMU TLB Invalidation Start Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_END_ADDR_REG;    /*!< Offset 0x08C IOMMU TLB Invalidation End Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_REG;        /*!< Offset 0x090 IOMMU TLB Invalidation Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_MASK_REG;   /*!< Offset 0x094 IOMMU TLB Invalidation Address Mask Register */
    volatile uint32_t IOMMU_TLB_IVLD_ENABLE_REG;      /*!< Offset 0x098 IOMMU TLB Invalidation Enable Register */
             uint32_t reserved_0x09C;
    volatile uint32_t IOMMU_PC_IVLD_ADDR_REG;         /*!< Offset 0x0A0 IOMMU PC Invalidation Address Register */
             uint32_t reserved_0x0A4;
    volatile uint32_t IOMMU_PC_IVLD_ENABLE_REG;       /*!< Offset 0x0A8 IOMMU */
             uint32_t reserved_0x0AC;
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG0;         /*!< Offset 0x0B0 IOMMU Domain Authority Control Register 0 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG1;         /*!< Offset 0x0B4 IOMMU Domain Authority Control Register 1 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG2;         /*!< Offset 0x0B8 IOMMU Domain Authority Control Register 2 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG3;         /*!< Offset 0x0BC IOMMU Domain Authority Control Register 3 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG4;         /*!< Offset 0x0C0 IOMMU Domain Authority Control Register 4 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG5;         /*!< Offset 0x0C4 IOMMU Domain Authority Control Register 5 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG6;         /*!< Offset 0x0C8 IOMMU Domain Authority Control Register 6 */
    volatile uint32_t IOMMU_DM_AUT_CTRL_REG7;         /*!< Offset 0x0CC IOMMU Domain Authority Control Register 7 */
    volatile uint32_t IOMMU_DM_AUT_OVWT_REG;          /*!< Offset 0x0D0 IOMMU Domain Authority Overwrite Register */
             uint32_t reserved_0x0D4 [0x000B];
    volatile uint32_t IOMMU_INT_ENABLE_REG;           /*!< Offset 0x100 IOMMU Interrupt Enable Register */
    volatile uint32_t IOMMU_INT_CLR_REG;              /*!< Offset 0x104 IOMMU Interrupt Clear Register */
    volatile uint32_t IOMMU_INT_STA_REG;              /*!< Offset 0x108 IOMMU Interrupt Status Register */
             uint32_t reserved_0x10C;
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG0;        /*!< Offset 0x110 IOMMU Interrupt Error Address Register 0 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG1;        /*!< Offset 0x114 IOMMU Interrupt Error Address Register 1 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG2;        /*!< Offset 0x118 IOMMU Interrupt Error Address Register 2 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG3;        /*!< Offset 0x11C IOMMU Interrupt Error Address Register 3 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG4;        /*!< Offset 0x120 IOMMU Interrupt Error Address Register 4 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG5;        /*!< Offset 0x124 IOMMU Interrupt Error Address Register 5 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG6;        /*!< Offset 0x128 IOMMU Interrupt Error Address Register 6 */
             uint32_t reserved_0x12C;
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG7;        /*!< Offset 0x130 IOMMU Interrupt Error Address Register 7 */
    volatile uint32_t IOMMU_INT_ERR_ADDR_REG8;        /*!< Offset 0x134 IOMMU Interrupt Error Address Register 8 */
             uint32_t reserved_0x138 [0x0006];
    volatile uint32_t IOMMU_INT_ERR_DATA_REG0;        /*!< Offset 0x150 IOMMU Interrupt Error Data Register 0 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG1;        /*!< Offset 0x154 IOMMU Interrupt Error Data Register 1 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG2;        /*!< Offset 0x158 IOMMU Interrupt Error Data Register 2 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG3;        /*!< Offset 0x15C IOMMU Interrupt Error Data Register 3 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG4;        /*!< Offset 0x160 IOMMU Interrupt Error Data Register 4 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG5;        /*!< Offset 0x164 IOMMU Interrupt Error Data Register 5 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG6;        /*!< Offset 0x168 IOMMU Interrupt Error Data Register 6 */
             uint32_t reserved_0x16C;
    volatile uint32_t IOMMU_INT_ERR_DATA_REG7;        /*!< Offset 0x170 IOMMU Interrupt Error Data Register 7 */
    volatile uint32_t IOMMU_INT_ERR_DATA_REG8;        /*!< Offset 0x174 IOMMU Interrupt Error Data Register 8 */
             uint32_t reserved_0x178 [0x0002];
    volatile uint32_t IOMMU_L1PG_INT_REG;             /*!< Offset 0x180 IOMMU L1 Page Table Interrupt Register */
    volatile uint32_t IOMMU_L2PG_INT_REG;             /*!< Offset 0x184 IOMMU L2 Page Table Interrupt Register */
             uint32_t reserved_0x188 [0x0002];
    volatile uint32_t IOMMU_VA_REG;                   /*!< Offset 0x190 IOMMU Virtual Address Register */
    volatile uint32_t IOMMU_VA_DATA_REG;              /*!< Offset 0x194 IOMMU Virtual Address Data Register */
    volatile uint32_t IOMMU_VA_CONFIG_REG;            /*!< Offset 0x198 IOMMU Virtual Address Configuration Register */
             uint32_t reserved_0x19C [0x0019];
    volatile uint32_t IOMMU_PMU_ENABLE_REG;           /*!< Offset 0x200 IOMMU PMU Enable Register */
             uint32_t reserved_0x204 [0x0003];
    volatile uint32_t IOMMU_PMU_CLR_REG;              /*!< Offset 0x210 IOMMU PMU Clear Register */
             uint32_t reserved_0x214 [0x0007];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG0;      /*!< Offset 0x230 IOMMU PMU Access Low Register 0 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG0;     /*!< Offset 0x234 IOMMU PMU Access High Register 0 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG0;         /*!< Offset 0x238 IOMMU PMU Hit Low Register 0 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG0;        /*!< Offset 0x23C IOMMU PMU Hit High Register 0 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG1;      /*!< Offset 0x240 IOMMU PMU Access Low Register 1 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG1;     /*!< Offset 0x244 IOMMU PMU Access High Register 1 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG1;         /*!< Offset 0x248 IOMMU PMU Hit Low Register 1 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG1;        /*!< Offset 0x24C IOMMU PMU Hit High Register 1 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG2;      /*!< Offset 0x250 IOMMU PMU Access Low Register 2 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG2;     /*!< Offset 0x254 IOMMU PMU Access High Register 2 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG2;         /*!< Offset 0x258 IOMMU PMU Hit Low Register 2 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG2;        /*!< Offset 0x25C IOMMU PMU Hit High Register 2 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG3;      /*!< Offset 0x260 IOMMU PMU Access Low Register 3 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG3;     /*!< Offset 0x264 IOMMU PMU Access High Register 3 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG3;         /*!< Offset 0x268 IOMMU PMU Hit Low Register 3 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG3;        /*!< Offset 0x26C IOMMU PMU Hit High Register 3 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG4;      /*!< Offset 0x270 IOMMU PMU Access Low Register 4 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG4;     /*!< Offset 0x274 IOMMU PMU Access High Register 4 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG4;         /*!< Offset 0x278 IOMMU PMU Hit Low Register 4 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG4;        /*!< Offset 0x27C IOMMU PMU Hit High Register 4 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG5;      /*!< Offset 0x280 IOMMU PMU Access Low Register 5 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG5;     /*!< Offset 0x284 IOMMU PMU Access High Register 5 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG5;         /*!< Offset 0x288 IOMMU PMU Hit Low Register 5 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG5;        /*!< Offset 0x28C IOMMU PMU Hit High Register 5 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG6;      /*!< Offset 0x290 IOMMU PMU Access Low Register 6 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG6;     /*!< Offset 0x294 IOMMU PMU Access High Register 6 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG6;         /*!< Offset 0x298 IOMMU PMU Hit Low Register 6 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG6;        /*!< Offset 0x29C IOMMU PMU Hit High Register 6 */
             uint32_t reserved_0x2A0 [0x000C];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG7;      /*!< Offset 0x2D0 IOMMU PMU Access Low Register 7 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG7;     /*!< Offset 0x2D4 IOMMU PMU Access High Register 7 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG7;         /*!< Offset 0x2D8 IOMMU PMU Hit Low Register 7 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG7;        /*!< Offset 0x2DC IOMMU PMU Hit High Register 7 */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW_REG8;      /*!< Offset 0x2E0 IOMMU PMU Access Low Register 8 */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH_REG8;     /*!< Offset 0x2E4 IOMMU PMU Access High Register 8 */
    volatile uint32_t IOMMU_PMU_HIT_LOW_REG8;         /*!< Offset 0x2E8 IOMMU PMU Hit Low Register 8 */
    volatile uint32_t IOMMU_PMU_HIT_HIGH_REG8;        /*!< Offset 0x2EC IOMMU PMU Hit High Register 8 */
             uint32_t reserved_0x2F0 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW_REG0;          /*!< Offset 0x300 IOMMU Total Latency Low Register 0 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG0;         /*!< Offset 0x304 IOMMU Total Latency High Register 0 */
    volatile uint32_t IOMMU_PMU_ML_REG0;              /*!< Offset 0x308 IOMMU Max Latency Register 0 */
             uint32_t reserved_0x30C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG1;          /*!< Offset 0x310 IOMMU Total Latency Low Register 1 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG1;         /*!< Offset 0x314 IOMMU Total Latency High Register 1 */
    volatile uint32_t IOMMU_PMU_ML_REG1;              /*!< Offset 0x318 IOMMU Max Latency Register 1 */
             uint32_t reserved_0x31C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG2;          /*!< Offset 0x320 IOMMU Total Latency Low Register 2 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG2;         /*!< Offset 0x324 IOMMU Total Latency High Register 2 */
    volatile uint32_t IOMMU_PMU_ML_REG2;              /*!< Offset 0x328 IOMMU Max Latency Register 2 */
             uint32_t reserved_0x32C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG3;          /*!< Offset 0x330 IOMMU Total Latency Low Register 3 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG3;         /*!< Offset 0x334 IOMMU Total Latency High Register 3 */
    volatile uint32_t IOMMU_PMU_ML_REG3;              /*!< Offset 0x338 IOMMU Max Latency Register 3 */
             uint32_t reserved_0x33C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG4;          /*!< Offset 0x340 IOMMU Total Latency Low Register 4 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG4;         /*!< Offset 0x344 IOMMU Total Latency High Register 4 */
    volatile uint32_t IOMMU_PMU_ML_REG4;              /*!< Offset 0x348 IOMMU Max Latency Register 4 */
             uint32_t reserved_0x34C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG5;          /*!< Offset 0x350 IOMMU Total Latency Low Register 5 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG5;         /*!< Offset 0x354 IOMMU Total Latency High Register 5 */
    volatile uint32_t IOMMU_PMU_ML_REG5;              /*!< Offset 0x358 IOMMU Max Latency Register 5 */
             uint32_t reserved_0x35C;
    volatile uint32_t IOMMU_PMU_TL_LOW_REG6;          /*!< Offset 0x360 IOMMU Total Latency Low Register 6 */
    volatile uint32_t IOMMU_PMU_TL_HIGH_REG6;         /*!< Offset 0x364 IOMMU Total Latency High Register 6 */
    volatile uint32_t IOMMU_PMU_ML_REG6;              /*!< Offset 0x368 IOMMU Max Latency Register 6 */
} IOMMU_TypeDef; /* size of structure = 0x36C */
/*
 * @brief LRADC
 */
/*!< LRADC  */
typedef struct LRADC_Type
{
    volatile uint32_t LRADC_CTRL;                     /*!< Offset 0x000 LRADC Control Register */
    volatile uint32_t LRADC_INTC;                     /*!< Offset 0x004 LRADC Interrupt Control Register */
    volatile uint32_t LRADC_INTS;                     /*!< Offset 0x008 LRADC Interrupt Status Register */
    volatile uint32_t LRADC_DATA0;                    /*!< Offset 0x00C LRADC Data Register0 */
} LRADC_TypeDef; /* size of structure = 0x010 */
/*
 * @brief OWA
 */
/*!< OWA One Wire Audio */
typedef struct OWA_Type
{
    volatile uint32_t OWA_GEN_CTL;                    /*!< Offset 0x000 (null) */
    volatile uint32_t OWA_TX_CFIG;                    /*!< Offset 0x004 OWA TX Configuration Register */
             uint32_t reserved_0x008;
    volatile uint32_t OWA_ISTA;                       /*!< Offset 0x00C OWA Interrupt Status Register */
             uint32_t reserved_0x010;
    volatile uint32_t OWA_FCTL;                       /*!< Offset 0x014 OWA FIFO Control Register */
    volatile uint32_t OWA_FSTA;                       /*!< Offset 0x018 OWA FIFO */
    volatile uint32_t OWA_INT;                        /*!< Offset 0x01C OWA Interrupt Control Register */
    volatile uint32_t OWA_TX_FIFO;                    /*!< Offset 0x020 OWA TX FIFO Register */
    volatile uint32_t OWA_TX_CNT;                     /*!< Offset 0x024 OWA TX Counter Register */
             uint32_t reserved_0x028;
    volatile uint32_t OWA_TX_CHSTA0;                  /*!< Offset 0x02C OWA TX Channel Status Register0 */
    volatile uint32_t OWA_TX_CHSTA1;                  /*!< Offset 0x030 OWA TX Channel Status Register1 */
} OWA_TypeDef; /* size of structure = 0x034 */
/*
 * @brief PRCM
 */
/*!< PRCM Power Reset Clock Management module */
typedef struct PRCM_Type
{
    volatile uint32_t CPUS_CFG_REG;                   /*!< Offset 0x000 CPUS Configuration Register */
             uint32_t reserved_0x004 [0x0002];
    volatile uint32_t APBS1_CFG_REG;                  /*!< Offset 0x00C APBS1 Configuration Register */
             uint32_t reserved_0x010 [0x0047];
    volatile uint32_t R_TWD_BGR_REG;                  /*!< Offset 0x12C R_TWD Bus Gating Reset Register */
             uint32_t reserved_0x130 [0x001B];
    volatile uint32_t R_TWI_BGR_REG;                  /*!< Offset 0x19C R_TWI Bus Gating Reset Register */
             uint32_t reserved_0x1A0 [0x0003];
    volatile uint32_t R_CAN_BGR_REG;                  /*!< Offset 0x1AC R_CAN Bus Gating Reset Register */
             uint32_t reserved_0x1B0 [0x0003];
    volatile uint32_t R_RSB_BGR_REG;                  /*!< Offset 0x1BC R_RSB Bus Gating Reset Register */
    volatile uint32_t R_IR_RX_CLK_REG;                /*!< Offset 0x1C0 R_IR_RX Clock Register */
             uint32_t reserved_0x1C4 [0x0002];
    volatile uint32_t R_IR_RX_BGR_REG;                /*!< Offset 0x1CC R_IR_RX Bus Gating Reset Register */
             uint32_t reserved_0x1D0 [0x000F];
    volatile uint32_t RTC_BGR_REG;                    /*!< Offset 0x20C RTC Bus Gating Reset Register */
             uint32_t reserved_0x210 [0x000C];
    volatile uint32_t PLL_CTRL_REG0;                  /*!< Offset 0x240 PLL Control Register 0 */
    volatile uint32_t PLL_CTRL_REG1;                  /*!< Offset 0x244 PLL Control Register 1 */
             uint32_t reserved_0x248 [0x0002];
    volatile uint32_t VDD_SYS_PWROFF_GATING_REG;      /*!< Offset 0x250 VDD_SYS Power Off Gating Register */
    volatile uint32_t GPU_PWROFF_GATING;              /*!< Offset 0x254 GPU Power Off Gating Register */
    volatile uint32_t VE_PWROFF_GATING;               /*!< Offset 0x258 VE Power Off Gating Register */
             uint32_t reserved_0x25C [0x0005];
    volatile uint32_t RAM_CFG_REG;                    /*!< Offset 0x270 RAM Configuration Register */
    volatile uint32_t RAM_TEST_CTRL_REG;              /*!< Offset 0x274 RAM Test Control Register */
             uint32_t reserved_0x278 [0x0006];
    volatile uint32_t PRCM_SEC_SWITCH_REG;            /*!< Offset 0x290 PRCM Security Switch Register */
             uint32_t reserved_0x294 [0x001F];
    volatile uint32_t RES_CAL_CTRL_REG;               /*!< Offset 0x310 Resistor Calibration Control Register */
    volatile uint32_t RES200_CTRL_REG;                /*!< Offset 0x314 200ohms Resistor Manual Control Register */
    volatile uint32_t RES240_CTRL_REG;                /*!< Offset 0x318 240ohms Resistor Manual Control Register */
    volatile uint32_t RES_CAL_STATUS_REG;             /*!< Offset 0x31C Resistor Calibration Status Register */
    volatile uint32_t NMI_IRQ_CTRL_REG;               /*!< Offset 0x320 NMI Interrupt Control Register */
    volatile uint32_t NMI_IRQ_ENABLE_REG;             /*!< Offset 0x324 NMI Interrupt Enable Register */
    volatile uint32_t NMI_IRQ_PEND_REG;               /*!< Offset 0x328 NMI Interrupt Pending Register */
             uint32_t reserved_0x32C [0x0031];
    volatile uint32_t PRCM_VERSION_REG;               /*!< Offset 0x3F0 PRCM Version Register */
} PRCM_TypeDef; /* size of structure = 0x3F4 */
/*
 * @brief PWM
 */
/*!< PWM Pulse Width Modulation module */
typedef struct PWM_Type
{
    volatile uint32_t PIER;                           /*!< Offset 0x000 PWM IRQ Enable Register */
    volatile uint32_t PISR;                           /*!< Offset 0x004 PWM IRQ Status Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t CIER;                           /*!< Offset 0x010 Capture IRQ Enable Register */
    volatile uint32_t CISR;                           /*!< Offset 0x014 Capture IRQ Status Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t PCCR [0x003];                   /*!< Offset 0x020 PWM01, PWM23, PWM45 Clock Configuration Register */
             uint32_t reserved_0x02C;
    volatile uint32_t PDZCR [0x003];                  /*!< Offset 0x030 PWM01, PWM23, PWM45 Dead Zone Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t PER;                            /*!< Offset 0x040 PWM Enable Register */
    volatile uint32_t CER;                            /*!< Offset 0x044 Capture Enable Register */
             uint32_t reserved_0x048 [0x0006];
    struct
    {
        volatile uint32_t PCR;                        /*!< Offset 0x060 PWM Control Register */
        volatile uint32_t PPR;                        /*!< Offset 0x064 PWM Period Register */
        volatile uint32_t PCNTR;                      /*!< Offset 0x068 PWM Count Register */
        volatile uint32_t CCR;                        /*!< Offset 0x06C Capture Control Register */
        volatile uint32_t CRLR;                       /*!< Offset 0x070 Capture Rise Lock Register */
        volatile uint32_t CFLR;                       /*!< Offset 0x074 Capture Fall Lock Register */
                 uint32_t reserved_0x018 [0x0002];
    } CH [0x006];                                     /*!< Offset 0x060 Channels[0..5] */
} PWM_TypeDef; /* size of structure = 0x120 */
/*
 * @brief RTC
 */
/*!< RTC Real Time Clock */
typedef struct RTC_Type
{
    volatile uint32_t LOSC_CTRL_REG;                  /*!< Offset 0x000 Low Oscillator Control Register */
    volatile uint32_t LOSC_AUTO_SWT_STA_REG;          /*!< Offset 0x004 LOSC Auto Switch Status Register */
    volatile uint32_t INTOSC_CLK_PRESCAL_REG;         /*!< Offset 0x008 Internal OSC Clock Prescalar Register */
    volatile uint32_t INTOSC_CLK_AUTO_CALI_REG;       /*!< Offset 0x00C Internal OSC Clock Auto Calibration Register */
    volatile uint32_t RTC_DAY_REG;                    /*!< Offset 0x010 RTC Year-Month-Day Register */
    volatile uint32_t RTC_HH_MM_SS_REG;               /*!< Offset 0x014 RTC Hour-Minute-Second Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t ALARM0_COUNTER_REG;             /*!< Offset 0x020 Alarm 0 Counter Register */
    volatile uint32_t ALARM0_CUR_VLU_REG;             /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    volatile uint32_t ALARM0_ENABLE_REG;              /*!< Offset 0x028 Alarm 0 Enable Register */
    volatile uint32_t ALARM0_IRQ_EN;                  /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    volatile uint32_t ALARM0_IRQ_STA_REG;             /*!< Offset 0x030 Alarm 0 IRQ Status Register */
             uint32_t reserved_0x034 [0x0007];
    volatile uint32_t ALARM_CONFIG_REG;               /*!< Offset 0x050 Alarm Configuration Register */
             uint32_t reserved_0x054 [0x0003];
    volatile uint32_t F32K_FANOUT_GATING_REG;         /*!< Offset 0x060 32k Fanout Output Gating Register */
             uint32_t reserved_0x064 [0x0027];
    volatile uint32_t GP_DATA_REG [0x010];            /*!< Offset 0x100 General Purpose Register (N=0~15) */
             uint32_t reserved_0x140 [0x0008];
    volatile uint32_t DCXO_CTRL_REG;                  /*!< Offset 0x160 DCXO Control Register */
             uint32_t reserved_0x164 [0x000B];
    volatile uint32_t RTC_VIO_REG;                    /*!< Offset 0x190 RTC_VIO Regulate Register */
             uint32_t reserved_0x194 [0x0017];
    volatile uint32_t IC_CHARA_REG;                   /*!< Offset 0x1F0 IC Characteristic Register */
    volatile uint32_t VDDOFF_GATING_SOF_REG;          /*!< Offset 0x1F4 VDD To RTC Isolation Software Control Register */
    volatile uint32_t SP_STDBY_FLAG_REG;              /*!< Offset 0x1F8 Super Standby Flag Register */
    volatile uint32_t SP_STDBY_SOFT_ENTRY_REG;        /*!< Offset 0x1FC Super Standby Software Entry Register */
    volatile uint32_t USB_STBY_CTRL_REG;              /*!< Offset 0x200 USB Standby Control Register */
    volatile uint32_t EFUSE_HV_PWRSWT_CTRL_REG;       /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
             uint32_t reserved_0x208 [0x0002];
    volatile uint32_t CRY_CONFIG_REG;                 /*!< Offset 0x210 Crypt Configuration Register */
    volatile uint32_t CRY_KEY_REG;                    /*!< Offset 0x214 Crypt Key Register */
    volatile uint32_t CRY_EN_REG;                     /*!< Offset 0x218 Crypt Enable Register */
} RTC_TypeDef; /* size of structure = 0x21C */
/*
 * @brief R_CAN
 */
/*!< R_CAN Car Area Network controller */
typedef struct R_CAN_Type
{
    volatile uint32_t CAN_MSEL;                       /*!< Offset 0x000 CAN mode select register */
    volatile uint32_t CAN_CMD;                        /*!< Offset 0x004 CAN command register */
    volatile uint32_t CAN_STA;                        /*!< Offset 0x008 CAN status register */
    volatile uint32_t CAN_INT;                        /*!< Offset 0x00C CAN interrupt register */
    volatile uint32_t CAN_INTEN;                      /*!< Offset 0x010 CAN interrupt enable register */
    volatile uint32_t CAN_BUSTIME;                    /*!< Offset 0x014 CAN bus timing register */
    volatile uint32_t CAN_TEWL;                       /*!< Offset 0x018 CAN TX error warning limit register */
    volatile uint32_t CAN_ERRC;                       /*!< Offset 0x01C CAN error counter register */
    volatile uint32_t CAN_RMCNT;                      /*!< Offset 0x020 CAN receive message counter register */
    volatile uint32_t CAN_RBUF_SADDR;                 /*!< Offset 0x024 CAN receive buffer start address register */
    volatile uint32_t CAN_ACPC;                       /*!< Offset 0x028 CAN acceptance code 0 register(reset mode) */
    volatile uint32_t CAN_ACPM;                       /*!< Offset 0x02C CAN acceptance mask 0 register(reset mode) */
             uint32_t reserved_0x030 [0x0004];
    volatile uint32_t CAN_TRBUF [0x00D];              /*!< Offset 0x040 CAN TX/RX message buffer N (n=0..12) register */
             uint32_t reserved_0x074 [0x0043];
    volatile uint32_t CAN_RBUF_RBACK [0x030];         /*!< Offset 0x180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
} R_CAN_TypeDef; /* size of structure = 0x240 */
/*
 * @brief SCR
 */
/*!< SCR Smart Card Reader */
typedef struct SCR_Type
{
    volatile uint32_t SCR_CSR;                        /*!< Offset 0x000 Smart Card Reader Control and Status Register */
    volatile uint32_t SCR_INTEN;                      /*!< Offset 0x004 Smart Card Reader Interrupt Enable Register 1 */
    volatile uint32_t SCR_INTST;                      /*!< Offset 0x008 Smart Card Reader Interrupt Status Register 1 */
    volatile uint32_t SCR_FCSR;                       /*!< Offset 0x00C Smart Card Reader FIFO Control and Status Register */
    volatile uint32_t SCR_FCNT;                       /*!< Offset 0x010 Smart Card Reader RX and TX FIFO Counter Register */
    volatile uint32_t SCR_RPT;                        /*!< Offset 0x014 Smart Card Reader RX and TX Repeat Register */
    volatile uint32_t SCR_DIV;                        /*!< Offset 0x018 Smart Card Reader Clock and Baud Divisor Register */
    volatile uint32_t SCR_LTIM;                       /*!< Offset 0x01C Smart Card Reader Line Time Register */
    volatile uint32_t SCR_CTIM;                       /*!< Offset 0x020 Smart Card Reader Character Time Register */
             uint32_t reserved_0x024 [0x0003];
    volatile uint32_t SCR_LCTLR;                      /*!< Offset 0x030 Smart Card Reader Line Control Register */
             uint32_t reserved_0x034 [0x0002];
    volatile uint32_t SCR_FSM;                        /*!< Offset 0x03C Smart Card Reader FSM Register */
    volatile uint32_t SCR_DT;                         /*!< Offset 0x040 Smart Card Reader Debounce Time Register */
             uint32_t reserved_0x044 [0x002F];
    volatile uint32_t SCR_FIFO;                       /*!< Offset 0x100 Smart Card Reader RX and TX FIFO Access Point */
} SCR_TypeDef; /* size of structure = 0x104 */
/*
 * @brief SID
 */
/*!< SID  */
typedef struct SID_Type
{
             uint32_t reserved_0x000 [0x0005];
    volatile uint32_t SID_THS;                        /*!< Offset 0x014 [27:16]: The calibration value of the T-sensor. */
             uint32_t reserved_0x018 [0x007A];
    volatile uint32_t SID_DATA [0x004];               /*!< Offset 0x200 SID data (xfel display as 'sid' replay) */
} SID_TypeDef; /* size of structure = 0x210 */
/*
 * @brief SMHC
 */
/*!< SMHC SD-MMC Host Controller */
typedef struct SMHC_Type
{
    volatile uint32_t SMHC_CTRL;                      /*!< Offset 0x000 Control Register */
    volatile uint32_t SMHC_CLKDIV;                    /*!< Offset 0x004 Clock Control Register */
    volatile uint32_t SMHC_TMOUT;                     /*!< Offset 0x008 Time Out Register */
    volatile uint32_t SMHC_CTYPE;                     /*!< Offset 0x00C Bus Width Register */
    volatile uint32_t SMHC_BLKSIZ;                    /*!< Offset 0x010 Block Size Register */
    volatile uint32_t SMHC_BYTCNT;                    /*!< Offset 0x014 Byte Count Register */
    volatile uint32_t SMHC_CMD;                       /*!< Offset 0x018 Command Register */
    volatile uint32_t SMHC_CMDARG;                    /*!< Offset 0x01C Command Argument Register */
    volatile uint32_t SMHC_RESP0;                     /*!< Offset 0x020 Response 0 Register */
    volatile uint32_t SMHC_RESP1;                     /*!< Offset 0x024 Response 1 Register */
    volatile uint32_t SMHC_RESP2;                     /*!< Offset 0x028 Response 2 Register */
    volatile uint32_t SMHC_RESP3;                     /*!< Offset 0x02C Response 3 Register */
    volatile uint32_t SMHC_INTMASK;                   /*!< Offset 0x030 Interrupt Mask Register */
    volatile uint32_t SMHC_MINTSTS;                   /*!< Offset 0x034 Masked Interrupt Status Register */
    volatile uint32_t SMHC_RINTSTS;                   /*!< Offset 0x038 Raw Interrupt Status Register */
    volatile uint32_t SMHC_STATUS;                    /*!< Offset 0x03C Status Register */
    volatile uint32_t SMHC_FIFOTH;                    /*!< Offset 0x040 FIFO Water Level Register */
    volatile uint32_t SMHC_FUNS;                      /*!< Offset 0x044 FIFO Function Select Register */
    volatile uint32_t SMHC_TCBCNT;                    /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
    volatile uint32_t SMHC_TBBCNT;                    /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
    volatile uint32_t SMHC_DBGC;                      /*!< Offset 0x050 Current Debug Control Register */
    volatile uint32_t SMHC_CSDC;                      /*!< Offset 0x054 CRC Status Detect Control Register (Only for SMHC2) */
    volatile uint32_t SMHC_A12A;                      /*!< Offset 0x058 Auto Command 12 Argument Register */
    volatile uint32_t SMHC_NTSR;                      /*!< Offset 0x05C SD New Timing Set Register (Only for SMHC0, SMHC1) */
             uint32_t reserved_0x060 [0x0006];
    volatile uint32_t SMHC_HWRST;                     /*!< Offset 0x078 Hardware Reset Register */
             uint32_t reserved_0x07C;
    volatile uint32_t SMHC_IDMAC;                     /*!< Offset 0x080 IDMAC Control Register */
    volatile uint32_t SMHC_DLBA;                      /*!< Offset 0x084 Descriptor List Base Address Register */
    volatile uint32_t SMHC_IDST;                      /*!< Offset 0x088 IDMAC Status Register */
    volatile uint32_t SMHC_IDIE;                      /*!< Offset 0x08C IDMAC Interrupt Enable Register */
             uint32_t reserved_0x090 [0x001C];
    volatile uint32_t SMHC_THLD;                      /*!< Offset 0x100 Card Threshold Control Register */
    volatile uint32_t SMHC_SFC;                       /*!< Offset 0x104 Sample FIFO Control Register (Only for SMHC2) */
    volatile uint32_t SMHC_A23A;                      /*!< Offset 0x108 Auto Command 23 Argument Register (Only for SMHC2) */
    volatile uint32_t EMMC_DDR_SBIT_DET;              /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
    volatile uint32_t SMHC_RES_CRC;                   /*!< Offset 0x110 Response CRC from Device (Only for SMHC0, SMHC1) */
    volatile uint32_t SMHC_D7_D0_CRC [0x008];         /*!< Offset 0x114 CRC in Data7..Data0 from Device (Only for SMHC0, SMHC1) */
    volatile uint32_t SMHC_CRC_STA;                   /*!< Offset 0x134 Write CRC Status Register (Only for SMHC0, SMHC1) */
    volatile uint32_t SMHC_EXT_CMD;                   /*!< Offset 0x138 Extended Command Register (Only for SMHC2) */
    volatile uint32_t SMHC_EXT_RESP;                  /*!< Offset 0x13C Extended Response Register (Only for SMHC2) */
    volatile uint32_t SMHC_DRV_DL;                    /*!< Offset 0x140 Drive Delay Control Register */
    volatile uint32_t SMHC_SAMP_DL;                   /*!< Offset 0x144 Sample Delay Control Register */
    volatile uint32_t SMHC_DS_DL;                     /*!< Offset 0x148 Data Strobe Delay Control Register (Only for SMHC2) */
             uint32_t reserved_0x14C [0x002D];
    volatile uint32_t SMHC_FIFO;                      /*!< Offset 0x200 Read/Write FIFO */
             uint32_t reserved_0x204 [0x037F];
} SMHC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SPI
 */
/*!< SPI Serial Peripheral Interface */
typedef struct SPI_Type
{
             uint32_t reserved_0x000;
    volatile uint32_t SPI_GCR;                        /*!< Offset 0x004 SPI Global Control Register */
    volatile uint32_t SPI_TCR;                        /*!< Offset 0x008 SPI Transfer Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t SPI_IER;                        /*!< Offset 0x010 SPI Interrupt Control Register */
    volatile uint32_t SPI_ISR;                        /*!< Offset 0x014 SPI Interrupt Status Register */
    volatile uint32_t SPI_FCR;                        /*!< Offset 0x018 SPI FIFO Control Register */
    volatile uint32_t SPI_FSR;                        /*!< Offset 0x01C SPI FIFO Status Register */
    volatile uint32_t SPI_WCR;                        /*!< Offset 0x020 SPI Wait Clock Register */
             uint32_t reserved_0x024;
    volatile uint32_t SPI_SAMP_DL;                    /*!< Offset 0x028 SPI Sample Delay Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t SPI_MBC;                        /*!< Offset 0x030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                        /*!< Offset 0x034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                        /*!< Offset 0x038 SPI Master Burst Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t SPI_BATCR;                      /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_3W_CCR;                     /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                        /*!< Offset 0x048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                        /*!< Offset 0x04C SPI RX Bit Register */
             uint32_t reserved_0x050 [0x000E];
    volatile uint32_t SPI_NDMA_MODE_CTL;              /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
             uint32_t reserved_0x08C [0x005D];
    volatile uint32_t SPI_TXD;                        /*!< Offset 0x200 SPI TX Data Register */
             uint32_t reserved_0x204 [0x003F];
    volatile uint32_t SPI_RXD;                        /*!< Offset 0x300 SPI RX Data Register */
             uint32_t reserved_0x304 [0x033F];
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG  */
typedef struct SYS_CFG_Type
{
             uint32_t reserved_0x000 [0x0009];
    volatile uint32_t VER_REG;                        /*!< Offset 0x024 Version Register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t EMAC_EPHY_CLK_REG0;             /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
    volatile uint32_t EMAC_EPHY_CLK_REG1;             /*!< Offset 0x034 EMAC-EPHY Clock Register 1 (T507 only) */
} SYS_CFG_TypeDef; /* size of structure = 0x038 */
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
typedef struct TCON_LCD_Type
{
    volatile uint32_t LCD_GCTL_REG;                   /*!< Offset 0x000 LCD Global Control Register */
    volatile uint32_t LCD_GINT0_REG;                  /*!< Offset 0x004 LCD Global Interrupt Register0 */
    volatile uint32_t LCD_GINT1_REG;                  /*!< Offset 0x008 LCD Global Interrupt Register1 */
             uint32_t reserved_0x00C;
    volatile uint32_t LCD_FRM_CTL_REG;                /*!< Offset 0x010 LCD FRM Control Register */
    volatile uint32_t LCD_FRM_SEED_REG [0x006];       /*!< Offset 0x014 LCD FRM Seed Register (N=0,1,2,3,4,5) 0x0014+N*0x04 */
    volatile uint32_t LCD_FRM_TAB_REG;                /*!< Offset 0x02C 4 LCD FRM Table Reg ister (N=0,1,2,3) 0x002C+N*0x04 */
             uint32_t reserved_0x030 [0x0003];
    volatile uint32_t LCD_3D_FIFO_REG;                /*!< Offset 0x03C LCD 3D FIFO Register */
    volatile uint32_t LCD_CTL_REG;                    /*!< Offset 0x040 LCD Control Register */
    volatile uint32_t LCD_DCLK_REG;                   /*!< Offset 0x044 LCD Data Clock Register */
    volatile uint32_t LCD_BASIC0_REG;                 /*!< Offset 0x048 LCD Basic Timing Register0 */
    volatile uint32_t LCD_BASIC1_REG;                 /*!< Offset 0x04C LCD Basic Timing Register1 */
    volatile uint32_t LCD_BASIC2_REG;                 /*!< Offset 0x050 LCD Basic Timing Register2 */
    volatile uint32_t LCD_BASIC3_REG;                 /*!< Offset 0x054 LCD Basic Timing Register3 */
    volatile uint32_t LCD_HV_IF_REG;                  /*!< Offset 0x058 LCD HV Panel Interface Register */
             uint32_t reserved_0x05C;
    volatile uint32_t LCD_CPU_IF_REG;                 /*!< Offset 0x060 LCD CPU Panel Interface Register */
    volatile uint32_t LCD_CPU_WR_REG;                 /*!< Offset 0x064 LCD CPU Panel Write Data Regist er */
    volatile uint32_t LCD_CPU_RD0_REG;                /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
    volatile uint32_t LCD_CPU_RD1_REG;                /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
             uint32_t reserved_0x070 [0x0005];
    volatile uint32_t LCD_LVDS_IF_REG;                /*!< Offset 0x084 LCD LVDS Configure Register - AKA tcon0_lvds_ctl */
    volatile uint32_t LCD_IO_POL_REG;                 /*!< Offset 0x088 LCD IO Polarity Register */
    volatile uint32_t LCD_IO_TRI_REG;                 /*!< Offset 0x08C LCD IO Control Register */
             uint32_t reserved_0x090 [0x001B];
    volatile uint32_t LCD_DEBUG_REG;                  /*!< Offset 0x0FC LCD Debug Register */
    volatile uint32_t LCD_CEU_CTL_REG;                /*!< Offset 0x100 LCD CEU Control Register */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t LCD_CEU_COEF_MUL_REG [0x001];   /*!< Offset 0x110 LCD CEU Coefficient Register0(N=0..10) 0x0110+N*0x04 */
             uint32_t reserved_0x114 [0x0002];
    volatile uint32_t LCD_CEU_COEF_ADD_REG [0x003];   /*!< Offset 0x11C LCD CEU Coefficient Register1(N=0,1,2) 0x011C+N*0x10 */
             uint32_t reserved_0x128 [0x0006];
    volatile uint32_t LCD_CEU_COEF_RANG_REG [0x003];  /*!< Offset 0x140 LCD CEU Coefficient Register2(N=0,1,2) 0x0140+N*0x04 */
             uint32_t reserved_0x14C [0x0005];
    volatile uint32_t LCD_CPU_TRI0_REG;               /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
    volatile uint32_t LCD_CPU_TRI1_REG;               /*!< Offset 0x164 LCD CPU Panel Trigger Register1 */
    volatile uint32_t LCD_CPU_TRI2_REG;               /*!< Offset 0x168 LCD CPU Panel Trigger Register2 */
    volatile uint32_t LCD_CPU_TRI3_REG;               /*!< Offset 0x16C LCD CPU Panel Trigger Register3 */
    volatile uint32_t LCD_CPU_TRI4_REG;               /*!< Offset 0x170 LCD CPU Panel Trigger Register4 */
    volatile uint32_t LCD_CPU_TRI5_REG;               /*!< Offset 0x174 LCD CPU Panel Trigger Register5 */
             uint32_t reserved_0x178 [0x0002];
    volatile uint32_t LCD_CMAP_CTL_REG;               /*!< Offset 0x180 LCD Color Map Control Register */
             uint32_t reserved_0x184 [0x0003];
    volatile uint32_t LCD_CMAP_ODD0_REG;              /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
    volatile uint32_t LCD_CMAP_ODD1_REG;              /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
    volatile uint32_t LCD_CMAP_EVEN0_REG;             /*!< Offset 0x198 LCD Color Map Even Line Register0 */
    volatile uint32_t LCD_CMAP_EVEN1_REG;             /*!< Offset 0x19C LCD Color Map Even Line Register1 */
             uint32_t reserved_0x1A0 [0x0014];
    volatile uint32_t LCD_SAFE_PERIOD_REG;            /*!< Offset 0x1F0 LCD Safe Period Register */
             uint32_t reserved_0x1F4 [0x0003];
    volatile uint32_t tcon_mul_ctl;                   /*!< Offset 0x200 https://github.com/qiaoweibiao/T507_Kernel/blob/98fcc7d3f112e51b0edfb71536da89cb2115106f/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_lcd_type.h#L691 */
             uint32_t reserved_0x204 [0x0007];
    volatile uint32_t LCD_LVDS_ANA_REG [0x002];       /*!< Offset 0x220 LCD LVDS Analog Register 0/1 LCD_LVDS0_ANA_REG LCD_LVDS1_ANA_REG */
             uint32_t reserved_0x228 [0x0005];
    volatile uint32_t LCD_FSYNC_GEN_CTRL_REG;         /*!< Offset 0x23C Module Enable and Output Value Register */
    volatile uint32_t LCD_FSYNC_GEN_DLY_REG;          /*!< Offset 0x240 Fsync Active Time Register */
             uint32_t reserved_0x244 [0x006F];
    volatile uint32_t LCD_GAMMA_TABLE_REG [0x100];    /*!< Offset 0x400 LCD Gamma Table Register 0x0400-0x07FF */
             uint32_t reserved_0x800 [0x01FD];
    volatile uint32_t LCD_3D_FIFO_BIST_REG;           /*!< Offset 0xFF4 LCD 3D FIFO Bist Register */
    volatile uint32_t LCD_TRI_FIFO_BIST_REG;          /*!< Offset 0xFF8 LCD Trigger FIFO Bist Register */
             uint32_t reserved_0xFFC;
} TCON_LCD_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TCON_TV
 */
/*!< TCON_TV TV Output */
typedef struct TCON_TV_Type
{
    volatile uint32_t TV_GCTL_REG;                    /*!< Offset 0x000 TV Global Control Register */
    volatile uint32_t TV_GINT0_REG;                   /*!< Offset 0x004 TV Global Interrupt Register0 */
    volatile uint32_t TV_GINT1_REG;                   /*!< Offset 0x008 TV Global Interrupt Register1 */
             uint32_t reserved_0x00C [0x000D];
    volatile uint32_t TV_SRC_CTL_REG;                 /*!< Offset 0x040 TV Source Control Register */
             uint32_t reserved_0x044 [0x0011];
    volatile uint32_t TV_IO_POL_REG;                  /*!< Offset 0x088 TV IO Polarity Register */
    volatile uint32_t TV_IO_TRI_REG;                  /*!< Offset 0x08C TV IO Control Register */
    volatile uint32_t TV_CTL_REG;                     /*!< Offset 0x090 TV Control Register */
    volatile uint32_t TV_BASIC0_REG;                  /*!< Offset 0x094 TV Basic Timing Register0 */
    volatile uint32_t TV_BASIC1_REG;                  /*!< Offset 0x098 TV Basic Timing Register1 */
    volatile uint32_t TV_BASIC2_REG;                  /*!< Offset 0x09C TV Basic Timing Register2 */
    volatile uint32_t TV_BASIC3_REG;                  /*!< Offset 0x0A0 TV Basic Timing Regi ster3 */
    volatile uint32_t TV_BASIC4_REG;                  /*!< Offset 0x0A4 TV Basic Timing Register4 */
    volatile uint32_t TV_BASIC5_REG;                  /*!< Offset 0x0A8 TV Basic Timing Register5 */
             uint32_t reserved_0x0AC [0x0013];
    volatile uint32_t TV_ECC_FIFO_REG;                /*!< Offset 0x0F8 TV ECC FIFO Register */
    volatile uint32_t TV_DEBUG_REG;                   /*!< Offset 0x0FC TV Debug Register */
    volatile uint32_t TV_CEU_CTL_REG;                 /*!< Offset 0x100 TV CEU Control Register */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t TV_CEU_COEF_MUL_REG [0x00B];    /*!< Offset 0x110 TV CEU Coefficient MUL R egister (N=0..10) */
             uint32_t reserved_0x13C;
    volatile uint32_t TV_CEU_COEF_RANG_REG [0x003];   /*!< Offset 0x140 TV CEU Coefficient Range R egister (N=0,1,2) */
             uint32_t reserved_0x14C [0x0029];
    volatile uint32_t TV_SAFE_PERIOD_REG;             /*!< Offset 0x1F0 TV Safe Period Register */
             uint32_t reserved_0x1F4 [0x0043];
    volatile uint32_t TV_FILL_CTL_REG;                /*!< Offset 0x300 TV Fill Data Control Register */
    struct
    {
        volatile uint32_t TV_FILL_BEGIN_REG;          /*!< Offset 0x304 TV Fill Data Begin Register 0x0304+N*0x0C(N=0..2) */
        volatile uint32_t TV_FILL_END_REG;            /*!< Offset 0x308 TV Fill Data End Register  0x0308+N*0x0C(N=0..2) */
        volatile uint32_t TV_FILL_DATA_REG;           /*!< Offset 0x30C TV Fill Data Value Register  0x030C+N*0x0C(N=0..2) */
    } TV_FILL [0x003];                                /*!< Offset 0x304 V Fill Data 0..2 */
             uint32_t reserved_0x328 [0x0002];
    volatile uint32_t TV_DATA_IO_POL0_REG;            /*!< Offset 0x330 TV Data IO Polarity0 Register */
    volatile uint32_t TV_DATA_IO_POL1_REG;            /*!< Offset 0x334 TV Data IO Polarity1 Register */
    volatile uint32_t TV_DATA_TRI0_REG;               /*!< Offset 0x338 TV Data IO Trigger0 Register */
    volatile uint32_t TV_DATA_TRI1_REG;               /*!< Offset 0x33C TV Data IO Trigger1 Register */
    volatile uint32_t TV_PIXELDEPTH_MODE_REG;         /*!< Offset 0x340 TV Pixel */
             uint32_t reserved_0x344 [0x032F];
} TCON_TV_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief THS
 */
/*!< THS Thermal Sensor */
typedef struct THS_Type
{
    volatile uint32_t THS_CTRL;                       /*!< Offset 0x000 THS Control Register */
    volatile uint32_t THS_EN;                         /*!< Offset 0x004 THS Enable Register */
    volatile uint32_t THS_PER;                        /*!< Offset 0x008 THS Period Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t THS_DATA_INTC;                  /*!< Offset 0x010 THS Data Interrupt Control Register */
    volatile uint32_t THS_SHUT_INTC;                  /*!< Offset 0x014 THS Shut Interrupt Control Register */
    volatile uint32_t THS_ALARM_INTC;                 /*!< Offset 0x018 THS Alarm Interrupt Control Register */
             uint32_t reserved_0x01C;
    volatile uint32_t THS_DATA_INTS;                  /*!< Offset 0x020 THS Data Interrupt Status Register */
    volatile uint32_t THS_SHUT_INTS;                  /*!< Offset 0x024 THS Shut Interrupt Status Register */
    volatile uint32_t THS_ALARMO_INTS;                /*!< Offset 0x028 THS Alarm off Interrupt Status Register */
    volatile uint32_t THS_ALARM_INTS;                 /*!< Offset 0x02C THS Alarm Interrupt Status Register */
    volatile uint32_t THS_FILTER;                     /*!< Offset 0x030 THS Median Filter Control Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t THSx_ALARM_CTRL [0x004];        /*!< Offset 0x040 THS0..THS3 Alarm Threshold Control Register */
             uint32_t reserved_0x050 [0x000C];
    volatile uint32_t THSx_SHUTDOWN_CTRL [0x002];     /*!< Offset 0x080 THS0 & THS1, HS2 & THS3  Shutdown Threshold Control Register */
             uint32_t reserved_0x088 [0x0006];
    volatile uint32_t THSx_CDATA [0x002];             /*!< Offset 0x0A0 THS0 & THS1, THS2 & THS3  Calibration Data */
             uint32_t reserved_0x0A8 [0x0006];
    volatile uint32_t THSx_DATA [0x004];              /*!< Offset 0x0C0 THS0..THS3 Data Register */
} THS_TypeDef; /* size of structure = 0x0D0 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    volatile uint32_t TMR_IRQ_EN_REG;                 /*!< Offset 0x000 Timer IRQ Enable Register */
    volatile uint32_t TMR_IRQ_STA_REG;                /*!< Offset 0x004 Timer Status Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t TMR0_CTRL_REG;                  /*!< Offset 0x010 Timer 0 Control Register */
    volatile uint32_t TMR0_INTV_VALUE_REG;            /*!< Offset 0x014 Timer 0 Interval Value Register */
    volatile uint32_t TMR0_CUR_VALUE_REG;             /*!< Offset 0x018 Timer 0 Current Value Register */
             uint32_t reserved_0x01C;
    volatile uint32_t TMR1_CTRL_REG;                  /*!< Offset 0x020 Timer 1 Control Register */
    volatile uint32_t TMR1_INTV_VALUE_REG;            /*!< Offset 0x024 Timer 1 Interval Value Register */
    volatile uint32_t TMR1_CUR_VALUE_REG;             /*!< Offset 0x028 Timer 1 Current Value Register */
             uint32_t reserved_0x02C [0x001D];
    volatile uint32_t WDOG_IRQ_EN_REG;                /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
    volatile uint32_t WDOG_IRQ_STA_REG;               /*!< Offset 0x0A4 Watchdog Status Register */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t WDOG_CTRL_REG;                  /*!< Offset 0x0B0 Watchdog Control Register */
    volatile uint32_t WDOG_CFG_REG;                   /*!< Offset 0x0B4 Watchdog Configuration Register */
    volatile uint32_t WDOG_MODE_REG;                  /*!< Offset 0x0B8 Watchdog Mode Register */
             uint32_t reserved_0x0BC;
    volatile uint32_t AVS_CNT_CTL_REG;                /*!< Offset 0x0C0 AVS Control Register */
    volatile uint32_t AVS_CNT0_REG;                   /*!< Offset 0x0C4 AVS Counter 0 Register */
    volatile uint32_t AVS_CNT1_REG;                   /*!< Offset 0x0C8 AVS Counter 1 Register */
    volatile uint32_t AVS_CNT_DIV_REG;                /*!< Offset 0x0CC AVS Divisor Register */
} TIMER_TypeDef; /* size of structure = 0x0D0 */
/*
 * @brief TSC
 */
/*!< TSC Transport Stream Controller */
typedef struct TSC_Type
{
             uint32_t reserved_0x000 [0x0004];
    volatile uint32_t TSC_PCTLR;                      /*!< Offset 0x010 TSC Port Control Register */
    volatile uint32_t TSC_PPARR;                      /*!< Offset 0x014 TSC Port Parameter Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t TSC_TSFMUXR;                    /*!< Offset 0x020 TSC TSF Input Multiplex Control Register */
             uint32_t reserved_0x024;
    volatile uint32_t TSC_OUTMUXR;                    /*!< Offset 0x028 TSC Port Output Multiplex Control Register */
} TSC_TypeDef; /* size of structure = 0x02C */
/*
 * @brief TSD
 */
/*!< TSD Transport Stream Controller */
typedef struct TSD_Type
{
    volatile uint32_t TSD_CTLR;                       /*!< Offset 0x000 TSD Control Register */
    volatile uint32_t TSD_STAR;                       /*!< Offset 0x004 TSD Status Register */
             uint32_t reserved_0x008 [0x0005];
    volatile uint32_t TSD_CWIR;                       /*!< Offset 0x01C TSD Control Word Index Register */
    volatile uint32_t TSD_CWR;                        /*!< Offset 0x020 TSD Control Word Register */
} TSD_TypeDef; /* size of structure = 0x024 */
/*
 * @brief TSF
 */
/*!< TSF Transport Stream Controller */
typedef struct TSF_Type
{
    volatile uint32_t TSF_CTLR;                       /*!< Offset 0x000 TSF Control Register */
    volatile uint32_t TSF_PPR;                        /*!< Offset 0x004 TSF Packet Parameter Register */
    volatile uint32_t TSF_STAR;                       /*!< Offset 0x008 TSF Status Register */
             uint32_t reserved_0x00C;
    volatile uint32_t TSF_DIER;                       /*!< Offset 0x010 TSF DMA Interrupt Enable Register */
    volatile uint32_t TSF_OIER;                       /*!< Offset 0x014 TSF Overlap Interrupt Enable Register */
    volatile uint32_t TSF_DISR;                       /*!< Offset 0x018 TSF DMA Interrupt Status Register */
    volatile uint32_t TSF_OISR;                       /*!< Offset 0x01C TSF Overlap Interrupt Status Register */
    volatile uint32_t TSF_PCRCR;                      /*!< Offset 0x020 TSF PCR Control Register */
    volatile uint32_t TSF_PCRDR;                      /*!< Offset 0x024 TSF PCR Data Register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t TSF_CENR;                       /*!< Offset 0x030 TSF Channel Enable Register */
    volatile uint32_t TSF_CPER;                       /*!< Offset 0x034 TSF Channel PES Enable Register */
    volatile uint32_t TSF_CDER;                       /*!< Offset 0x038 TSF Channel Descramble Enable Register */
    volatile uint32_t TSF_CINDR;                      /*!< Offset 0x03C TSF Channel Index Register */
    volatile uint32_t TSF_CCTLR;                      /*!< Offset 0x040 TSF Channel Control Register */
    volatile uint32_t TSF_CSTAR;                      /*!< Offset 0x044 TSF Channel Status Register */
    volatile uint32_t TSF_CCWIR;                      /*!< Offset 0x048 TSF Channel CW Index Register */
    volatile uint32_t TSF_CPIDR;                      /*!< Offset 0x04C TSF Channel PID Register */
    volatile uint32_t TSF_CBBAR;                      /*!< Offset 0x050 TSF Channel Buffer Base Address Register */
    volatile uint32_t TSF_CBSZR;                      /*!< Offset 0x054 TSF Channel Buffer Size Register */
    volatile uint32_t TSF_CBWPR;                      /*!< Offset 0x058 TSF Channel Buffer Write Pointer Register */
    volatile uint32_t TSF_CBRPR;                      /*!< Offset 0x05C TSF Channel Buffer Read Pointer Register */
} TSF_TypeDef; /* size of structure = 0x060 */
/*
 * @brief TSG
 */
/*!< TSG Transport Stream Controller */
typedef struct TSG_Type
{
    volatile uint32_t TSG_CTLR;                       /*!< Offset 0x000 TSG Control Register */
    volatile uint32_t TSG_PPR;                        /*!< Offset 0x004 TSG Packet Parameter Register */
    volatile uint32_t TSG_STAR;                       /*!< Offset 0x008 TSG Status Register */
    volatile uint32_t TSG_CCR;                        /*!< Offset 0x00C TSG Clock Control Register */
    volatile uint32_t TSG_BBAR;                       /*!< Offset 0x010 TSG Buffer Base Address Register */
    volatile uint32_t TSG_BSZR;                       /*!< Offset 0x014 TSG Buffer Size Register */
    volatile uint32_t TSG_BPR;                        /*!< Offset 0x018 TSG Buffer Pointer Register */
} TSG_TypeDef; /* size of structure = 0x01C */
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
             uint32_t reserved_0x308 [0x003E];
} TWI_TypeDef; /* size of structure = 0x400 */
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
             uint32_t reserved_0x0B8 [0x0002];
    volatile uint32_t UART_485_CTL;                   /*!< Offset 0x0C0 UART RS485 Control and Status Register */
    volatile uint32_t RS485_ADDR_MATCH;               /*!< Offset 0x0C4 UART RS485 Addres Match Register  */
    volatile uint32_t BUS_IDLE_CHECK;                 /*!< Offset 0x0C8 UART RS485 Bus Idle Check Register */
    volatile uint32_t TX_DLY;                         /*!< Offset 0x0CC UART TX Delay Register */
             uint32_t reserved_0x0D0 [0x00CC];
} UART_TypeDef; /* size of structure = 0x400 */
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
    volatile uint16_t USB_TXCSRHI;                    /*!< Offset 0x082 [15:8]: USB_TXCSRH, [7:0]: USB_TXCSRL */
    volatile uint16_t USB_RXMAXP;                     /*!< Offset 0x084 USB_RXMAXP USB EP1~5 Rx Control and Status Register */
    volatile uint16_t USB_RXCSRHI;                    /*!< Offset 0x086 USB_RXCSR */
    volatile uint16_t USB_RXCOUNT;                    /*!< Offset 0x088 USB_RXCOUNT */
    volatile uint16_t USB_RXPKTCNT;                   /*!< Offset 0x08A USB_RXPKTCNT */
    volatile uint32_t USB_EPATTR;                     /*!< Offset 0x08C USB_EPATTR USB EP0 Attribute Register, USB EP1~5 Attribute Register */
    volatile uint32_t USB_TXFIFO;                     /*!< Offset 0x090 USB_TXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
    volatile uint32_t USB_RXFIFO;                     /*!< Offset 0x094 USB_RXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
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
/*!< USBPHYC HCI Contgroller and PHY Interface Description */
typedef struct USBPHYC_Type
{
    volatile uint32_t USB_CTRL;                       /*!< Offset 0x000 HCI Interface Register - REG_ISCR 0x00 */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t PHY_CTRL;                       /*!< Offset 0x010 PHY Control Register  */
             uint32_t reserved_0x014 [0x0002];
    volatile uint32_t HSIC_PHY_tune1;                 /*!< Offset 0x01C HSIC PHY Tune1 Register  */
    volatile uint32_t HSIC_PHY_tune2;                 /*!< Offset 0x020 HSIC PHY Tune2 Register - REG_PHY_OTGCTL 0x20 */
    volatile uint32_t HSIC_PHY_tune3;                 /*!< Offset 0x024 HSIC PHY Tune3 Register  */
    volatile uint32_t USB_SPDCR;                      /*!< Offset 0x028 HCI SIE Port Disable Control Register  */
} USBPHYC_TypeDef; /* size of structure = 0x02C */
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

#define DE_XX0 ((DE_XX_TypeDef *) DE_XX0_BASE)        /*!< DE_XX0  register set access pointer */
#define DE_XX1 ((DE_XX_TypeDef *) DE_XX1_BASE)        /*!< DE_XX1  register set access pointer */
#define DE_XX2 ((DE_XX_TypeDef *) DE_XX2_BASE)        /*!< DE_XX2  register set access pointer */
#define DE_XX3 ((DE_XX_TypeDef *) DE_XX3_BASE)        /*!< DE_XX3  register set access pointer */
#define DE_TOP ((DE_TOP_TypeDef *) DE_TOP_BASE)       /*!< DE_TOP Display Engine (DE) TOP (APB) register set access pointer */
#define DE_GLB ((DE_GLB_TypeDef *) DE_GLB_BASE)       /*!< DE_GLB Display Engine (DE) - Global Control register set access pointer */
#define DE_VI1 ((DE_VI_TypeDef *) DE_VI1_BASE)        /*!< DE_VI1 Display Engine (DE) - VI surface register set access pointer */
#define DE_VSU ((DE_VSU_TypeDef *) DE_VSU_BASE)       /*!< DE_VSU Video Scaler Unit (VSU) register set access pointer */
#define DE_FCE ((DE_FCE_TypeDef *) DE_FCE_BASE)       /*!< DE_FCE Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS ((DE_BLS_TypeDef *) DE_BLS_BASE)       /*!< DE_BLS Blue Level Stretch (BLS) register set access pointer */
#define DE_VI2 ((DE_VI_TypeDef *) DE_VI2_BASE)        /*!< DE_VI2 Display Engine (DE) - VI surface register set access pointer */
#define DE_VI3 ((DE_VI_TypeDef *) DE_VI3_BASE)        /*!< DE_VI3 Display Engine (DE) - VI surface register set access pointer */
#define DE_UI1 ((DE_UI_TypeDef *) DE_UI1_BASE)        /*!< DE_UI1 Display Engine (DE) - UI surface register set access pointer */
#define DE_UI2 ((DE_UI_TypeDef *) DE_UI2_BASE)        /*!< DE_UI2 Display Engine (DE) - UI surface register set access pointer */
#define DE_UI3 ((DE_UI_TypeDef *) DE_UI3_BASE)        /*!< DE_UI3 Display Engine (DE) - UI surface register set access pointer */
#define DE_BLD1 ((DE_BLD_TypeDef *) DE_BLD1_BASE)     /*!< DE_BLD1 Display Engine (DE) - Blender register set access pointer */
#define DE_BLD2 ((DE_BLD_TypeDef *) DE_BLD2_BASE)     /*!< DE_BLD2 Display Engine (DE) - Blender register set access pointer */
#define G2D_TOP ((G2D_TOP_TypeDef *) G2D_TOP_BASE)    /*!< G2D_TOP Graphic 2D top register set access pointer */
#define G2D_MIXER ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer register set access pointer */
#define G2D_ROT ((G2D_ROT_TypeDef *) G2D_ROT_BASE)    /*!< G2D_ROT Graphic 2D Rotate register set access pointer */
#define CE_NS ((CE_TypeDef *) CE_NS_BASE)             /*!< CE_NS The Crypto Engine (CE) module register set access pointer */
#define CE_S ((CE_TypeDef *) CE_S_BASE)               /*!< CE_S The Crypto Engine (CE) module register set access pointer */
#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L Port Controller register set access pointer */
#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)    /*!< SYS_CFG  register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define DMAC ((DMAC_TypeDef *) DMAC_BASE)             /*!< DMAC  register set access pointer */
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID  register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define PWM ((PWM_TypeDef *) PWM_BASE)                /*!< PWM Pulse Width Modulation module register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA Port Controller register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK Port Controller register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Port Controller register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Port Controller register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE Port Controller register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF Port Controller register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG Port Controller register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)           /*!< GPIOH Port Controller register set access pointer */
#define GPIOI ((GPIO_TypeDef *) GPIOI_BASE)           /*!< GPIOI Port Controller register set access pointer */
#define GPIOINTA ((GPIOINT_TypeDef *) GPIOINTA_BASE)  /*!< GPIOINTA  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)  /*!< GPIOINTH  register set access pointer */
#define GPIOINTI ((GPIOINT_TypeDef *) GPIOINTI_BASE)  /*!< GPIOINTI  register set access pointer */
#define IOMMU ((IOMMU_TypeDef *) IOMMU_BASE)          /*!< IOMMU IOMMU (I/O Memory management unit) register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)           /*!< SMHC0 SD-MMC Host Controller register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)           /*!< SMHC1 SD-MMC Host Controller register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)           /*!< SMHC2 SD-MMC Host Controller register set access pointer */
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
#define TWI4 ((TWI_TypeDef *) TWI4_BASE)              /*!< TWI4  register set access pointer */
#define SCR ((SCR_TypeDef *) SCR_BASE)                /*!< SCR Smart Card Reader register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define EMAC0 ((EMAC_TypeDef *) EMAC0_BASE)           /*!< EMAC0 The Ethernet Medium Access Controller (EMAC) enables a host to transmi register set access pointer */
#define EMAC1 ((EMAC_TypeDef *) EMAC1_BASE)           /*!< EMAC1 The Ethernet Medium Access Controller (EMAC) enables a host to transmi register set access pointer */
#define TSC ((TSC_TypeDef *) TSC_BASE)                /*!< TSC Transport Stream Controller register set access pointer */
#define TSG ((TSG_TypeDef *) TSG_BASE)                /*!< TSG Transport Stream Controller register set access pointer */
#define TSF ((TSF_TypeDef *) TSF_BASE)                /*!< TSF Transport Stream Controller register set access pointer */
#define TSD ((TSD_TypeDef *) TSD_BASE)                /*!< TSD Transport Stream Controller register set access pointer */
#define GPADC ((GPADC_TypeDef *) GPADC_BASE)          /*!< GPADC  register set access pointer */
#define THS ((THS_TypeDef *) THS_BASE)                /*!< THS Thermal Sensor register set access pointer */
#define LRADC ((LRADC_TypeDef *) LRADC_BASE)          /*!< LRADC  register set access pointer */
#define OWA ((OWA_TypeDef *) OWA_BASE)                /*!< OWA One Wire Audio register set access pointer */
#define DMIC ((DMIC_TypeDef *) DMIC_BASE)             /*!< DMIC Digital Microphone Interface register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)/*!< AUDIO_CODEC Audio Codec register set access pointer */
#define AHUB ((AHUB_TypeDef *) AHUB_BASE)             /*!< AHUB Audio HUB register set access pointer */
#define I2S0 ((I2S_PCM_TypeDef *) I2S0_BASE)          /*!< I2S0  register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1  register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2  register set access pointer */
#define I2S3 ((I2S_PCM_TypeDef *) I2S3_BASE)          /*!< I2S3  register set access pointer */
#define USB20_OTG_DEVICE ((USBOTG_TypeDef *) USB20_OTG_DEVICE_BASE)/*!< USB20_OTG_DEVICE USB OTG Dual-Role Device controller register set access pointer */
#define USB20_OTG_PHYC ((USBPHYC_TypeDef *) USB20_OTG_PHYC_BASE)/*!< USB20_OTG_PHYC HCI Contgroller and PHY Interface Description register set access pointer */
#define USB20_HOST0_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST0_EHCI_BASE)/*!< USB20_HOST0_EHCI  register set access pointer */
#define USB20_HOST0_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST0_OHCI_BASE)/*!< USB20_HOST0_OHCI  register set access pointer */
#define USBPHYC0 ((USBPHYC_TypeDef *) USBPHYC0_BASE)  /*!< USBPHYC0 HCI Contgroller and PHY Interface Description register set access pointer */
#define USB20_HOST1_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST1_EHCI_BASE)/*!< USB20_HOST1_EHCI  register set access pointer */
#define USB20_HOST1_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST1_OHCI_BASE)/*!< USB20_HOST1_OHCI  register set access pointer */
#define USBPHYC1 ((USBPHYC_TypeDef *) USBPHYC1_BASE)  /*!< USBPHYC1 HCI Contgroller and PHY Interface Description register set access pointer */
#define USB20_HOST2_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST2_EHCI_BASE)/*!< USB20_HOST2_EHCI  register set access pointer */
#define USB20_HOST2_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST2_OHCI_BASE)/*!< USB20_HOST2_OHCI  register set access pointer */
#define USBPHYC2 ((USBPHYC_TypeDef *) USBPHYC2_BASE)  /*!< USBPHYC2 HCI Contgroller and PHY Interface Description register set access pointer */
#define USB20_HOST3_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST3_EHCI_BASE)/*!< USB20_HOST3_EHCI  register set access pointer */
#define USB20_HOST3_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST3_OHCI_BASE)/*!< USB20_HOST3_OHCI  register set access pointer */
#define USBPHYC3 ((USBPHYC_TypeDef *) USBPHYC3_BASE)  /*!< USBPHYC3 HCI Contgroller and PHY Interface Description register set access pointer */
#define DISP_IF_TOP ((DISP_IF_TOP_TypeDef *) DISP_IF_TOP_BASE)/*!< DISP_IF_TOP display interface top (DISP_IF_TOP) register set access pointer */
#define TCON_LCD0 ((TCON_LCD_TypeDef *) TCON_LCD0_BASE)/*!< TCON_LCD0 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define TCON_LCD1 ((TCON_LCD_TypeDef *) TCON_LCD1_BASE)/*!< TCON_LCD1 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define TCON_TV0 ((TCON_TV_TypeDef *) TCON_TV0_BASE)  /*!< TCON_TV0 TV Output register set access pointer */
#define TCON_TV1 ((TCON_TV_TypeDef *) TCON_TV1_BASE)  /*!< TCON_TV1 TV Output register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC Real Time Clock register set access pointer */
#define PRCM ((PRCM_TypeDef *) PRCM_BASE)             /*!< PRCM Power Reset Clock Management module register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL Port Controller register set access pointer */
#define R_PIO ((GPIO_TypeDef *) R_PIO_BASE)           /*!< R_PIO Port Controller register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define R_TWI ((TWI_TypeDef *) R_TWI_BASE)            /*!< R_TWI  register set access pointer */
#define S_TWI0 ((TWI_TypeDef *) S_TWI0_BASE)          /*!< S_TWI0  register set access pointer */
#define R_CAN0 ((R_CAN_TypeDef *) R_CAN0_BASE)        /*!< R_CAN0 Car Area Network controller register set access pointer */
#define CPU_SUBSYS_CTRL_H616 ((CPU_SUBSYS_CTRL_H616_TypeDef *) CPU_SUBSYS_CTRL_H616_BASE)/*!< CPU_SUBSYS_CTRL_H616 H616 CPU Subsystem Control Register List register set access pointer */
#define CPU_SUBSYS_CTRL_T507 ((CPU_SUBSYS_CTRL_T507_TypeDef *) CPU_SUBSYS_CTRL_T507_BASE)/*!< CPU_SUBSYS_CTRL_T507 T507 CPU Subsystem Control Register List register set access pointer */
#define C0_CPUX_CFG_H616 ((C0_CPUX_CFG_H616_TypeDef *) C0_CPUX_CFG_H616_BASE)/*!< C0_CPUX_CFG_H616 H616 Cluster 0 Configuration Register List register set access pointer */
#define C0_CPUX_CFG_T507 ((C0_CPUX_CFG_T507_TypeDef *) C0_CPUX_CFG_T507_BASE)/*!< C0_CPUX_CFG_T507 T507 Cluster 0 Configuration Register List register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
