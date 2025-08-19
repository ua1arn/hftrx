/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__DADB7421_2CE1_4351_A89A_B0338D2EB5AF__INCLUDED
#define HEADER__DADB7421_2CE1_4351_A89A_B0338D2EB5AF__INCLUDED
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
    UART0_IRQn = 32,                                  /*!< UART  */
    UART1_IRQn = 33,                                  /*!< UART  */
    UART2_IRQn = 34,                                  /*!< UART  */
    UART3_IRQn = 35,                                  /*!< UART  */
    UART4_IRQn = 36,                                  /*!< UART  */
    TWI0_IRQn = 38,                                   /*!< TWI  */
    TWI1_IRQn = 39,                                   /*!< TWI  */
    TWI2_IRQn = 40,                                   /*!< TWI  */
    PB_EINT_IRQn = 43,                                /*!< GPIOINT PB_EINT interrupt  */
    OWA_IRQn = 44,                                    /*!< OWA One Wire Audio */
    I2S_PCM0_IRQn = 45,                               /*!< I2S_PCM I2S/PCM Controller */
    I2S_PCM1_IRQn = 46,                               /*!< I2S_PCM I2S/PCM Controller */
    I2S_PCM2_IRQn = 47,                               /*!< I2S_PCM I2S/PCM Controller */
    PG_EINT_IRQn = 49,                                /*!< GPIOINT PG_EINT interrupt */
    TIMER0_IRQn = 50,                                 /*!< TIMER  */
    TIMER1_IRQn = 51,                                 /*!< TIMER  */
    PH_EINT_IRQn = 53,                                /*!< GPIOINT PH_EINT interrupt */
    AUDIO_CODEC_IRQn = 61,                            /*!< AUDIO_CODEC  */
    R_TIMER0_IRQn = 65,                               /*!< R_TIMER  */
    R_TIMER1_IRQn = 66,                               /*!< R_TIMER  */
    R_WDOG_IRQn = 68,                                 /*!< R_WDOG  */
    R_CIR_RX_IRQn = 69,                               /*!< R_CIR_RX  */
    R_UART_IRQn = 70,                                 /*!< UART  */
    R_RSB_IRQn = 71,                                  /*!< R_RSB Reduced Serial Bus Host Controller */
    R_TIMER2_IRQn = 74,                               /*!< R_TIMER  */
    R_TIMER3_IRQn = 75,                               /*!< R_TIMER  */
    R_TWI_IRQn = 76,                                  /*!< TWI  */
    SMC_IRQn = 76,                                    /*!< SMC  */
    R_PL_EINT_IRQn = 77,                              /*!< GPIOINT R_PL_EINT interrupt */
    EMAC_IRQn = 78,                                   /*!< EMAC  */
    R_TWD_IRQn = 78,                                  /*!< R_TWD R_Trusted Watchdog Timer */
    MSGBOX_IRQn = 81,                                 /*!< MSGBOX  */
    DMAC_IRQn = 82,                                   /*!< DMAC  */
    GPADC_IRQn = 89,                                  /*!< GPADC  */
    VE_IRQn = 90,                                     /*!< VE Video Encoding */
    SMHC0_IRQn = 92,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC1_IRQn = 93,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC2_IRQn = 94,                                  /*!< SMHC SD-MMC Host Controller */
    TPADC_IRQn = 94,                                  /*!< TPADC  */
    IOMMU_IRQn = 96,                                  /*!< IOMMU  */
    SPI0_IRQn = 97,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 98,                                   /*!< SPI Serial Peripheral Interface */
    NAND_IRQn = 102,                                  /*!< NDFC NAND Flash Controller Interface */
    USBOTG0_IRQn = 103,                               /*!< USBOTG USB OTG Dual-Role Device controller */
    USBEHCI0_IRQn = 104,                              /*!< USB_EHCI_Capability  */
    USBOHCI0_IRQn = 105,                              /*!< USB_OHCI_Capability  */
    USBEHCI1_IRQn = 106,                              /*!< USB_EHCI_Capability  */
    USBOHCI1_IRQn = 107,                              /*!< USB_OHCI_Capability  */
    TSC_IRQn = 113,                                   /*!< TSC Transport Stream Controller */
    TCON0_IRQn = 118,                                 /*!< TCON0 TCON0 interrupt */
    TCON1_IRQn = 119,                                 /*!< TCON1 TCON1 interrupt */
    HDMI_TX0_IRQn = 120,                              /*!< HDMI_TX  */
    DE_IRQn = 127,                                    /*!< DE_TOP Display Engine Top */
    GPU_GP_IRQn = 129,                                /*!< GPU Mali-400MP2 */
    GPU_GPMMU_IRQn = 130,                             /*!< GPU Mali-400MP2 */
    GPU_PP0_IRQn = 131,                               /*!< GPU Mali-400MP2 */
    GPU_PP0MMU_IRQn = 132,                            /*!< GPU Mali-400MP2 */
    GPU_PMU_IRQn = 133,                               /*!< GPU Mali-400MP2 */
    GPU_PP1_IRQn = 134,                               /*!< GPU Mali-400MP2 */
    GPU_PPMMU1_IRQn = 135,                            /*!< GPU Mali-400MP2 */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define DE_BASE ((uintptr_t) 0x01000000)              /*!< DE Display Engine (DE) Base */
#define DE_TOP_BASE ((uintptr_t) 0x01000000)          /*!< DE_TOP Display Engine Top Base */
#define DE_WB_BASE ((uintptr_t) 0x01010000)           /*!< DE_WB Real-time write-back controller (RT-WB) Base */
#define DE_CSR_BASE ((uintptr_t) 0x01020000)          /*!< DE_CSR Copy & Rotation Base */
#define DE_MIXER0_GLB_BASE ((uintptr_t) 0x01100000)   /*!< DE_GLB  Base */
#define DE_MIXER0_BLD_BASE ((uintptr_t) 0x01101000)   /*!< DE_BLD  Base */
#define DE_MIXER0_VI1_BASE ((uintptr_t) 0x01102000)   /*!< DE_VI  Base */
#define DE_MIXER0_UI1_BASE ((uintptr_t) 0x01103000)   /*!< DE_UI  Base */
#define DE_MIXER0_UI2_BASE ((uintptr_t) 0x01104000)   /*!< DE_UI  Base */
#define DE_MIXER0_UI3_BASE ((uintptr_t) 0x01105000)   /*!< DE_UI  Base */
#define DE_MIXER0_VSU1_BASE ((uintptr_t) 0x01120000)  /*!< DE_VSU Video Scaler Unit (VSU), VS Base */
#define DE_MIXER0_UIS1_BASE ((uintptr_t) 0x01140000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER0_UIS2_BASE ((uintptr_t) 0x01150000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER0_UIS3_BASE ((uintptr_t) 0x01160000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER1_GLB_BASE ((uintptr_t) 0x01200000)   /*!< DE_GLB  Base */
#define DE_MIXER1_BLD_BASE ((uintptr_t) 0x01201000)   /*!< DE_BLD  Base */
#define DE_MIXER1_VI1_BASE ((uintptr_t) 0x01202000)   /*!< DE_VI  Base */
#define DE_MIXER1_UI1_BASE ((uintptr_t) 0x01203000)   /*!< DE_UI  Base */
#define DE_MIXER1_UI2_BASE ((uintptr_t) 0x01204000)   /*!< DE_UI  Base */
#define DE_MIXER1_UI3_BASE ((uintptr_t) 0x01205000)   /*!< DE_UI  Base */
#define DE_MIXER1_VSU1_BASE ((uintptr_t) 0x01220000)  /*!< DE_VSU Video Scaler Unit (VSU), VS Base */
#define DE_MIXER1_UIS1_BASE ((uintptr_t) 0x01240000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define CPUX_MBIST_BASE ((uintptr_t) 0x01502000)      /*!< CPUX_MBIST  Base */
#define CPUX_CFG_BASE ((uintptr_t) 0x01700000)        /*!< CPUX_CFG  Base */
#define SRAMC_BASE ((uintptr_t) 0x01C00000)           /*!< SRAMC  Base */
#define SYS_CFG_BASE ((uintptr_t) 0x01C00000)         /*!< SYS_CFG  Base */
#define DRAMC_BASE ((uintptr_t) 0x01C01000)           /*!< DRAMC  Base */
#define DMAC_BASE ((uintptr_t) 0x01C02000)            /*!< DMAC  Base */
#define NDFC_BASE ((uintptr_t) 0x01C03000)            /*!< NDFC NAND Flash Controller Interface Base */
#define TSC_BASE ((uintptr_t) 0x01C06000)             /*!< TSC Transport Stream Controller Base */
#define TCON0_BASE ((uintptr_t) 0x01C0C000)           /*!< TCON0 TCON0 LVDS/RGB/MIPI-DSI Interface Base */
#define TCON1_BASE ((uintptr_t) 0x01C0D000)           /*!< TCON1 TCON1 HDMI Interface Base */
#define VENCODER_BASE ((uintptr_t) 0x01C0E000)        /*!< VE Video Encoding Base */
#define SMHC0_BASE ((uintptr_t) 0x01C0F000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC1_BASE ((uintptr_t) 0x01C10000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC2_BASE ((uintptr_t) 0x01C11000)           /*!< SMHC SD-MMC Host Controller Base */
#define SID_BASE ((uintptr_t) 0x01C14000)             /*!< SID Security ID Base */
#define MSGBOX_BASE ((uintptr_t) 0x01C17000)          /*!< MSGBOX  Base */
#define USBOTG0_BASE ((uintptr_t) 0x01C19000)         /*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USBEHCI0_BASE ((uintptr_t) 0x01C1A000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI0_BASE ((uintptr_t) 0x01C1A400)        /*!< USB_OHCI_Capability  Base */
#define USBPHY0_BASE ((uintptr_t) 0x01C1A800)         /*!< USBPHYC  Base */
#define USBEHCI1_BASE ((uintptr_t) 0x01C1B000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI1_BASE ((uintptr_t) 0x01C1B400)        /*!< USB_OHCI_Capability  Base */
#define USBPHY1_BASE ((uintptr_t) 0x01C1B800)         /*!< USBPHYC  Base */
#define CCU_BASE ((uintptr_t) 0x01C20000)             /*!< CCU  Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x01C20800)       /*!< GPIOBLOCK  Base */
#define GPIOB_BASE ((uintptr_t) 0x01C20824)           /*!< GPIO  Base */
#define GPIOC_BASE ((uintptr_t) 0x01C20848)           /*!< GPIO  Base */
#define GPIOD_BASE ((uintptr_t) 0x01C2086C)           /*!< GPIO  Base */
#define GPIOE_BASE ((uintptr_t) 0x01C20890)           /*!< GPIO  Base */
#define GPIOF_BASE ((uintptr_t) 0x01C208B4)           /*!< GPIO  Base */
#define GPIOG_BASE ((uintptr_t) 0x01C208D8)           /*!< GPIO  Base */
#define GPIOH_BASE ((uintptr_t) 0x01C208FC)           /*!< GPIO  Base */
#define GPIOINTB_BASE ((uintptr_t) 0x01C20A00)        /*!< GPIOINT  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x01C20A20)        /*!< GPIOINT  Base */
#define GPIOINTD_BASE ((uintptr_t) 0x01C20A40)        /*!< GPIOINT  Base */
#define GPIOINTE_BASE ((uintptr_t) 0x01C20A60)        /*!< GPIOINT  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x01C20A80)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x01C20AA0)        /*!< GPIOINT  Base */
#define GPIOINTH_BASE ((uintptr_t) 0x01C20AC0)        /*!< GPIOINT  Base */
#define TIMER_BASE ((uintptr_t) 0x01C20C00)           /*!< TIMER  Base */
#define OWA_BASE ((uintptr_t) 0x01C21000)             /*!< OWA One Wire Audio Base */
#define PWM_BASE ((uintptr_t) 0x01C21400)             /*!< PWM  Base */
#define I2S0_BASE ((uintptr_t) 0x01C22000)            /*!< I2S_PCM I2S/PCM Controller Base */
#define I2S1_BASE ((uintptr_t) 0x01C22400)            /*!< I2S_PCM I2S/PCM Controller Base */
#define I2S2_BASE ((uintptr_t) 0x01C22800)            /*!< I2S_PCM I2S/PCM Controller Base */
#define AUDIO_CODEC_BASE ((uintptr_t) 0x01C22C00)     /*!< AUDIO_CODEC  Base */
#define THS_BASE ((uintptr_t) 0x01C25000)             /*!< THS  Base */
#define UART0_BASE ((uintptr_t) 0x01C28000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x01C28400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x01C28800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x01C28C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x01C29000)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x01C2AC00)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x01C2B000)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x01C2B400)            /*!< TWI  Base */
#define EMAC_BASE ((uintptr_t) 0x01C30000)            /*!< EMAC  Base */
#define GPU_BASE ((uintptr_t) 0x01C40000)             /*!< GPU Mali-400MP2 Base */
#define DRAMCOM_BASE ((uintptr_t) 0x01C62000)         /*!< DRAMCOM  Base */
#define DRAMCTL0_BASE ((uintptr_t) 0x01C63000)        /*!< DRAMCTL0  Base */
#define DRAMPHY0_BASE ((uintptr_t) 0x01C65000)        /*!< DRAMPHY0  Base */
#define SPI0_BASE ((uintptr_t) 0x01C68000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x01C69000)            /*!< SPI Serial Peripheral Interface Base */
#define GIC_BASE ((uintptr_t) 0x01C80000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x01C81000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x01C82000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x01C84000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x01C85000)            /*!< GICV  Base */
#define MIPI_DSI_BASE ((uintptr_t) 0x01CA0000)        /*!< MIPI_DSI  Base */
#define MIPI_DSI_PHY_BASE ((uintptr_t) 0x01CA1000)    /*!< MIPI_DSI_PHY  Base */
#define DI_BASE ((uintptr_t) 0x01E00000)              /*!< DI De-interlaced Base */
#define HDMI_TX0_BASE ((uintptr_t) 0x01EE0000)        /*!< HDMI_TX  Base */
#define HDMI_PHY_BASE ((uintptr_t) 0x01EF0000)        /*!< HDMI_PHY  Base */
#define RTC_BASE ((uintptr_t) 0x01F00000)             /*!< RTC  Base */
#define R_TIMER_BASE ((uintptr_t) 0x01F00800)         /*!< R_TIMER  Base */
#define R_INTC_BASE ((uintptr_t) 0x01F00C00)          /*!< R_INTC  Base */
#define R_WDOG_BASE ((uintptr_t) 0x01F01000)          /*!< R_WDOG  Base */
#define R_PRCM_BASE ((uintptr_t) 0x01F01400)          /*!< R_PRCM  Base */
#define R_TWD_BASE ((uintptr_t) 0x01F01800)           /*!< R_TWD R_Trusted Watchdog Timer Base */
#define R_CPUCFG_BASE ((uintptr_t) 0x01F01C00)        /*!< R_CPUCFG  Base */
#define CIR_RX_BASE ((uintptr_t) 0x01F02000)          /*!< CIR_RX  Base */
#define R_CIR_RX_BASE ((uintptr_t) 0x01F02000)        /*!< R_CIR_RX  Base */
#define R_TWI_BASE ((uintptr_t) 0x01F02400)           /*!< TWI  Base */
#define R_UART_BASE ((uintptr_t) 0x01F02800)          /*!< UART  Base */
#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK  Base */
#define GPIOL_BASE ((uintptr_t) 0x01F02C00)           /*!< GPIO  Base */
#define R_RSB_BASE ((uintptr_t) 0x01F03400)           /*!< R_RSB Reduced Serial Bus Host Controller Base */
#define R_PWM_BASE ((uintptr_t) 0x01F03800)           /*!< R_PWM  Base */
#define SPC_BASE ((uintptr_t) 0x02000800)             /*!< SPC  Base */
#define GPADC_BASE ((uintptr_t) 0x02009000)           /*!< GPADC  Base */
#define TPADC_BASE ((uintptr_t) 0x02009C00)           /*!< TPADC  Base */
#define IOMMU_BASE ((uintptr_t) 0x02010000)           /*!< IOMMU  Base */
#define DMIC_BASE ((uintptr_t) 0x02031000)            /*!< DMIC  Base */
#define SMC_BASE ((uintptr_t) 0x03007000)             /*!< SMC  Base */
#define CE_NS_BASE ((uintptr_t) 0x03040000)           /*!< CE  Base */
#define CE_S_BASE ((uintptr_t) 0x03040800)            /*!< CE  Base */
#define DDRPHYC_BASE ((uintptr_t) 0x03103000)         /*!< DDRPHYC  Base */
#define CPU_SUBSYS_CTRL_BASE ((uintptr_t) 0x08100000) /*!< CPU_SUBSYS_CTRL  Base */

#if __aarch64__
    #include <core64_ca.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC  */
typedef struct AUDIO_CODEC_Type
{
    __IO uint32_t DA_CTL;                             /*!< Offset 0x000 Digital Audio Control Register */
    __IO uint32_t DA_FAT0;                            /*!< Offset 0x004 Digital Audio Format Register 0 */
    __IO uint32_t DA_FAT1;                            /*!< Offset 0x008 Digital Audio Format Register 1 */
    __IO uint32_t DA_ISTA;                            /*!< Offset 0x00C Digital Audio Interrupt Status Register */
    __IO uint32_t AC_ADC_RXDATA;                      /*!< Offset 0x010 Digital Audio RX FIFO Register (was: DA_RXFIFO) */
    __IO uint32_t DA_FCTL;                            /*!< Offset 0x014 Digital Audio FIFO Control Register */
    __IO uint32_t DA_FSTA;                            /*!< Offset 0x018 Digital Audio FIFO Status Register */
    __IO uint32_t DA_INT;                             /*!< Offset 0x01C Digital Audio Interrupt Control Register */
    __IO uint32_t AC_DAC_TXDATA;                      /*!< Offset 0x020 Digital Audio TX FIFO Register (was: DA_TXFIFO) */
    __IO uint32_t DA_CLKD;                            /*!< Offset 0x024 Digital Audio Clock Divide Register */
    __IO uint32_t DA_TXCNT;                           /*!< Offset 0x028 Digital Audio RX Sample Counter Register */
    __IO uint32_t DA_RXCNT;                           /*!< Offset 0x02C Digital Audio TX Sample Counter Register */
    __IO uint32_t DA_TXCHSEL;                         /*!< Offset 0x030 Digital Audio TX Channel Select register */
    __IO uint32_t DA_TXCHMAP;                         /*!< Offset 0x034 Digital Audio TX Channel Mapping Register */
    __IO uint32_t DA_RXCHSEL;                         /*!< Offset 0x038 Digital Audio RX Channel Select register */
    __IO uint32_t DA_RXCHMAP;                         /*!< Offset 0x03C Digital Audio RX Channel Mapping Register */
         RESERVED(0x040[0x0200 - 0x0040], uint8_t)
    __IO uint32_t Codec_RST;                          /*!< Offset 0x200 Chip Soft Reset Register */
         RESERVED(0x204[0x020C - 0x0204], uint8_t)
    __IO uint32_t SYSCLK_CTL;                         /*!< Offset 0x20C System Clock Control Register */
    __IO uint32_t MOD_CLK_ENA;                        /*!< Offset 0x210 Module Clock Control Register */
    __IO uint32_t MOD_RST_CTL;                        /*!< Offset 0x214 Module Reset Control Register */
    __IO uint32_t SYS_SR_CTRL;                        /*!< Offset 0x218 System Sample rate & SRC Configuration Register */
    __IO uint32_t SYS_SRC_CLK;                        /*!< Offset 0x21C System SRC Clock Source Select Register */
    __IO uint32_t SYS_DVC_MOD;                        /*!< Offset 0x220 System DVC Mode Select Register */
         RESERVED(0x224[0x0240 - 0x0224], uint8_t)
    __IO uint32_t AIF1CLK_CTRL;                       /*!< Offset 0x240 AIF1 BCLK/LRCK Control Register */
    __IO uint32_t AIF1_ADCDAT_CTRL;                   /*!< Offset 0x244 AIF1 ADCDAT Control Register */
    __IO uint32_t AIF1_DACDAT_CTRL;                   /*!< Offset 0x248 AIF1 DACDAT Control Register */
    __IO uint32_t AIF1_MIXR_SRC;                      /*!< Offset 0x24C AIF1 Digital Mixer Source Select Register */
    __IO uint32_t AIF1_VOL_CTRL1;                     /*!< Offset 0x250 AIF1 Volume Control 1 Register */
    __IO uint32_t AIF1_VOL_CTRL2;                     /*!< Offset 0x254 AIF1 Volume Control 2 Register */
    __IO uint32_t AIF1_VOL_CTRL3;                     /*!< Offset 0x258 AIF1 Volume Control 3 Register */
    __IO uint32_t AIF1_VOL_CTRL4;                     /*!< Offset 0x25C AIF1 Volume Control 4 Register */
    __IO uint32_t AIF1_MXR_GAIN;                      /*!< Offset 0x260 AIF1 Digital Mixer Gain Control Register */
    __IO uint32_t AIF1_RXD_CTRL;                      /*!< Offset 0x264 AIF1 Receiver Data Discarding Control Register */
         RESERVED(0x268[0x0280 - 0x0268], uint8_t)
    __IO uint32_t AIF2_CLK_CTRL;                      /*!< Offset 0x280 AIF2 BCLK/LRCK Control Register */
    __IO uint32_t AIF2_ADCDAT_CTRL;                   /*!< Offset 0x284 AIF2 ADCDAT Control Register */
    __IO uint32_t AIF2_DACDAT_CTRL;                   /*!< Offset 0x288 AIF2 DACDAT Control Register */
    __IO uint32_t AIF2_MXR_SRC;                       /*!< Offset 0x28C AIF2 Digital Mixer Source Select Register */
    __IO uint32_t AIF2_VOL_CTRL1;                     /*!< Offset 0x290 AIF2 Volume Control 1 Register */
         RESERVED(0x294[0x0298 - 0x0294], uint8_t)
    __IO uint32_t AIF2_VOL_CTRL2;                     /*!< Offset 0x298 AIF2 Volume Control 2 Register */
         RESERVED(0x29C[0x02A0 - 0x029C], uint8_t)
    __IO uint32_t AIF2_MXR_GAIN;                      /*!< Offset 0x2A0 AIF2 Digital Mixer Gain Control Register */
    __IO uint32_t AIF2_RXD_CTRL;                      /*!< Offset 0x2A4 AIF2 Receiver Data Discarding Control Register */
         RESERVED(0x2A8[0x02C0 - 0x02A8], uint8_t)
    __IO uint32_t AIF3_CLK_CTRL;                      /*!< Offset 0x2C0 AIF3 BCLK/LRCK Control Register */
    __IO uint32_t AIF3_ADCDAT_CTRL;                   /*!< Offset 0x2C4 AIF3 ADCDAT Control Register */
    __IO uint32_t AIF3_DACDAT_CTRL;                   /*!< Offset 0x2C8 AIF3 DACDAT Control Register */
    __IO uint32_t AIF3_SGP_CTRL;                      /*!< Offset 0x2CC AIF3 Signal Path Control Register */
         RESERVED(0x2D0[0x02E4 - 0x02D0], uint8_t)
    __IO uint32_t AIF3_RXD_CTRL;                      /*!< Offset 0x2E4 AIF3 Receiver Data Discarding Control Register */
         RESERVED(0x2E8[0x0300 - 0x02E8], uint8_t)
    __IO uint32_t ADC_DIG_CTRL;                       /*!< Offset 0x300 ADC Digital Control Register */
    __IO uint32_t ADC_VOL_CTRL;                       /*!< Offset 0x304 ADC Volume Control Register */
    __IO uint32_t ADC_DBG_CTRL;                       /*!< Offset 0x308 ADC Debug Control Register */
         RESERVED(0x30C[0x0310 - 0x030C], uint8_t)
    __IO uint32_t HMIC_CTRL1;                         /*!< Offset 0x310 HMIC Control 1 Register */
    __IO uint32_t HMIC_CTRL2;                         /*!< Offset 0x314 HMIC Control 2 Register */
    __IO uint32_t HMIC_STS;                           /*!< Offset 0x318 HMIC Status Register */
         RESERVED(0x31C[0x0320 - 0x031C], uint8_t)
    __IO uint32_t DAC_DIG_CTRL;                       /*!< Offset 0x320 DAC Digital Control Register */
    __IO uint32_t DAC_VOL_CTRL;                       /*!< Offset 0x324 DAC Volume Control Register */
    __IO uint32_t DAC_DBG_CTRL;                       /*!< Offset 0x328 DAC Debug Control Register */
         RESERVED(0x32C[0x0330 - 0x032C], uint8_t)
    __IO uint32_t DAC_MXR_SRC;                        /*!< Offset 0x330 DAC Digital Mixer Source Select Register */
    __IO uint32_t DAC_MXR_GAIN;                       /*!< Offset 0x334 DAC Digital Mixer Gain Control Register */
         RESERVED(0x338[0x0400 - 0x0338], uint8_t)
    __IO uint32_t AC_ADC_DAPLSTA;                     /*!< Offset 0x400 ADC DAP Left Status Register */
    __IO uint32_t AC_ADC_DAPRSTA;                     /*!< Offset 0x404 ADC DAP Right Status Register */
    __IO uint32_t AC_ADC_DAPLCTRL;                    /*!< Offset 0x408 ADC DAP Left Channel Control Register */
    __IO uint32_t AC_ADC_DAPRCTRL;                    /*!< Offset 0x40C ADC DAP Right Channel Control Register */
    __IO uint32_t AC_ADC_DAPLTL;                      /*!< Offset 0x410 ADC DAP Left Target Level Register */
    __IO uint32_t AC_ADC_DAPRTL;                      /*!< Offset 0x414 ADC DAP Right Target Level Register */
    __IO uint32_t AC_ADC_DAPLHAC;                     /*!< Offset 0x418 ADC DAP Left High Average Coef Register */
    __IO uint32_t AC_ADC_DAPLLAC;                     /*!< Offset 0x41C ADC DAP Left Low Average Coef Register */
    __IO uint32_t AC_ADC_DAPRHAC;                     /*!< Offset 0x420 ADC DAP Right High Average Coef Register */
    __IO uint32_t AC_ADC_DAPRLAC;                     /*!< Offset 0x424 ADC DAP Right Low Average Coef Register */
    __IO uint32_t AC_ADC_DAPLDT;                      /*!< Offset 0x428 ADC DAP Left Decay Time Register */
    __IO uint32_t AC_ADC_DAPLAT;                      /*!< Offset 0x42C ADC DAP Left Attack Time Register */
    __IO uint32_t AC_ADC_DAPRDT;                      /*!< Offset 0x430 ADC DAP Right Decay Time Register */
    __IO uint32_t AC_ADC_DAPRAT;                      /*!< Offset 0x434 ADC DAP Right Attack Time Register */
    __IO uint32_t AC_ADC_DAPNTH;                      /*!< Offset 0x438 ADC DAP Noise Threshold Register */
    __IO uint32_t AC_ADC_DAPLHNAC;                    /*!< Offset 0x43C ADC DAP Left Input Signal High Average Coef Register */
    __IO uint32_t AC_ADC_DAPLLNAC;                    /*!< Offset 0x440 ADC DAP Left Input Signal Low Average Coef Register */
    __IO uint32_t AC_ADC_DAPRHNAC;                    /*!< Offset 0x444 ADC DAP Right Input Signal High Average Coef Register */
    __IO uint32_t AC_ADC_DAPRLNAC;                    /*!< Offset 0x448 ADC DAP Right Input Signal Low Average Coef Register */
    __IO uint32_t AC_DAPHHPFC;                        /*!< Offset 0x44C ADC DAP High HPF Coef Register */
    __IO uint32_t AC_DAPLHPFC;                        /*!< Offset 0x450 ADC DAP Low HPF Coef Register */
    __IO uint32_t AC_DAPOPT;                          /*!< Offset 0x454 ADC DAP Optimum Register */
         RESERVED(0x458[0x0480 - 0x0458], uint8_t)
    __IO uint32_t AC_DAC_DAPCTRL;                     /*!< Offset 0x480 DAC DAP Control Register */
         RESERVED(0x484[0x04D0 - 0x0484], uint8_t)
    __IO uint32_t AGC_ENA;                            /*!< Offset 0x4D0 AGC Enable Register */
    __IO uint32_t DRC_ENA;                            /*!< Offset 0x4D4 DRC Enable Register */
    __IO uint32_t SRC_BISTCR;                         /*!< Offset 0x4D8 SRC Bist control Register */
    __IO uint32_t SRC_BISTST;                         /*!< Offset 0x4DC SRC Bist Status Register */
    __IO uint32_t SRC1_CTRL1;                         /*!< Offset 0x4E0 SRC1 Control 1 Register */
    __IO uint32_t SRC1_CTRL2;                         /*!< Offset 0x4E4 SRC1 Control 2 Register */
    __IO uint32_t SRC1_CTRL3;                         /*!< Offset 0x4E8 SRC1 Control 3 Register */
    __IO uint32_t SRC1_CTRL4;                         /*!< Offset 0x4EC SRC1 Control 4 Register */
    __IO uint32_t SRC2_CTRL1;                         /*!< Offset 0x4F0 SRC2 Control 1 Register */
    __IO uint32_t SRC2_CTRL2;                         /*!< Offset 0x4F4 SRC2 Control 2 Register */
    __IO uint32_t SRC2_CTRL3;                         /*!< Offset 0x4F8 SRC2 Control 3 Register */
    __IO uint32_t SRC2_CTRL4;                         /*!< Offset 0x4FC SRC2 Control 4 Register */
         RESERVED(0x500[0x0600 - 0x0500], uint8_t)
    __IO uint32_t AC_DRC0_HHPFC;                      /*!< Offset 0x600 DRC0 High HPF Coef Register */
    __IO uint32_t AC_DRC0_LHPFC;                      /*!< Offset 0x604 DRC0 Low HPF Coef Register */
    __IO uint32_t AC_DRC0_CTRL;                       /*!< Offset 0x608 DRC0 Control Register */
    __IO uint32_t AC_DRC0_LPFHAT;                     /*!< Offset 0x60C DRC0 Left Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DRC0_LPFLAT;                     /*!< Offset 0x610 DRC0 Left Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DRC0_RPFHAT;                     /*!< Offset 0x614 DRC0 Right Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DRC0_RPFLAT;                     /*!< Offset 0x618 DRC0 Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DRC0_LPFHRT;                     /*!< Offset 0x61C DRC0 Left Peak Filter High Release Time Coef Register */
    __IO uint32_t AC_DRC0_LPFLRT;                     /*!< Offset 0x620 DRC0 Left Peak Filter Low Release Time Coef Register */
    __IO uint32_t AC_DRC0_RPFHRT;                     /*!< Offset 0x624 DRC0 Right Peak filter High Release Time Coef Register */
    __IO uint32_t AC_DRC0_RPFLRT;                     /*!< Offset 0x628 DRC0 Right Peak filter Low Release Time Coef Register */
    __IO uint32_t AC_DRC0_LRMSHAT;                    /*!< Offset 0x62C DRC0 Left RMS Filter High Coef Register */
    __IO uint32_t AC_DRC0_LRMSLAT;                    /*!< Offset 0x630 DRC0 Left RMS Filter Low Coef Register */
    __IO uint32_t AC_DRC0_RRMSHAT;                    /*!< Offset 0x634 DRC0 Right RMS Filter High Coef Register */
    __IO uint32_t AC_DRC0_RRMSLAT;                    /*!< Offset 0x638 DRC0 Right RMS Filter Low Coef Register */
    __IO uint32_t AC_DRC0_HCT;                        /*!< Offset 0x63C DRC0 Compressor Theshold High Setting Register */
    __IO uint32_t AC_DRC0_LCT;                        /*!< Offset 0x640 DRC0 Compressor Slope High Setting Register */
    __IO uint32_t AC_DRC0_HKC;                        /*!< Offset 0x644 DRC0 Compressor Slope High Setting Register */
    __IO uint32_t AC_DRC0_LKC;                        /*!< Offset 0x648 DRC0 Compressor Slope Low Setting Register */
    __IO uint32_t AC_DRC0_HOPC;                       /*!< Offset 0x64C DRC0 Compressor High Output at Compressor Threshold Register */
    __IO uint32_t AC_DRC0_LOPC;                       /*!< Offset 0x650 DRC0 Compressor Low Output at Compressor Threshold Register */
    __IO uint32_t AC_DRC0_HLT;                        /*!< Offset 0x654 DRC0 Limiter Theshold High Setting Register */
    __IO uint32_t AC_DRC0_LLT;                        /*!< Offset 0x658 DRC0 Limiter Theshold Low Setting Register */
    __IO uint32_t AC_DRC0_HKl;                        /*!< Offset 0x65C DRC0 Limiter Slope High Setting Register */
    __IO uint32_t AC_DRC0_LKl;                        /*!< Offset 0x660 DRC0 Limiter Slope Low Setting Register */
    __IO uint32_t AC_DRC0_HOPL;                       /*!< Offset 0x664 DRC0 Limiter High Output at Limiter Threshold */
    __IO uint32_t AC_DRC0_LOPL;                       /*!< Offset 0x668 DRC0 Limiter Low Output at Limiter Threshold */
    __IO uint32_t AC_DRC0_HET;                        /*!< Offset 0x66C DRC0 Expander Theshold High Setting Register */
    __IO uint32_t AC_DRC0_LET;                        /*!< Offset 0x670 DRC0 Expander Theshold Low Setting Register */
    __IO uint32_t AC_DRC0_HKE;                        /*!< Offset 0x674 DRC0 Expander Slope High Setting Register */
    __IO uint32_t AC_DRC0_LKE;                        /*!< Offset 0x678 DRC0 Expander Slope Low Setting Register */
    __IO uint32_t AC_DRC0_HOPE;                       /*!< Offset 0x67C DRC0 Expander High Output at Expander Threshold */
    __IO uint32_t AC_DRC0_LOPE;                       /*!< Offset 0x680 DRC0 Expander Low Output at Expander Threshold */
    __IO uint32_t AC_DRC0_HKN;                        /*!< Offset 0x684 DRC0 Linear Slope High Setting Register */
    __IO uint32_t AC_DRC0_LKN;                        /*!< Offset 0x688 DRC0 Linear Slope Low Setting Register */
    __IO uint32_t AC_DRC0_SFHAT;                      /*!< Offset 0x68C DRC0 Smooth filter Gain High Attack Time Coef Register */
    __IO uint32_t AC_DRC0_SFLAT;                      /*!< Offset 0x690 DRC0 Smooth filter Gain Low Attack Time Coef Register */
    __IO uint32_t AC_DRC0_SFHRT;                      /*!< Offset 0x694 DRC0 Smooth filter Gain High Release Time Coef Register */
    __IO uint32_t AC_DRC0_SFLRT;                      /*!< Offset 0x698 DRC0 Smooth filter Gain Low Release Time Coef Register */
    __IO uint32_t AC_DRC0_MXGHS;                      /*!< Offset 0x69C DRC0 MAX Gain High Setting Register */
    __IO uint32_t AC_DRC0_MXGLS;                      /*!< Offset 0x6A0 DRC0 MAX Gain Low Setting Register */
    __IO uint32_t AC_DRC0_MNGHS;                      /*!< Offset 0x6A4 DRC0 MIN Gain High Setting Register */
    __IO uint32_t AC_DRC0_MNGLS;                      /*!< Offset 0x6A8 DRC0 MIN Gain Low Setting Register */
    __IO uint32_t AC_DRC0_EPSHC;                      /*!< Offset 0x6AC DRC0 Expander Smooth Time High Coef Register */
    __IO uint32_t AC_DRC0_EPSLC;                      /*!< Offset 0x6B0 DRC0 Expander Smooth Time Low Coef Register */
         RESERVED(0x6B4[0x06BC - 0x06B4], uint8_t)
    __IO uint32_t AC_DRC0_OPT;                        /*!< Offset 0x6BC Was: 0x6AC DRC0 Optimum Register */
         RESERVED(0x6C0[0x0700 - 0x06C0], uint8_t)
    __IO uint32_t AC_DRC1_HHPFC;                      /*!< Offset 0x700 DRC1 High HPF Coef Register */
    __IO uint32_t AC_DRC1_LHPFC;                      /*!< Offset 0x704 DRC1 Low HPF Coef Register */
    __IO uint32_t AC_DRC1_CTRL;                       /*!< Offset 0x708 DRC1 Control Register */
    __IO uint32_t AC_DRC1_LPFHAT;                     /*!< Offset 0x70C DRC1 Left Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DRC1_LPFLAT;                     /*!< Offset 0x710 DRC1 Left Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DRC1_RPFHAT;                     /*!< Offset 0x714 DRC1 Right Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DRC1_RPFLAT;                     /*!< Offset 0x718 DRC1 Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DRC1_LPFHRT;                     /*!< Offset 0x71C DRC1 Left Peak Filter High Release Time Coef Register */
    __IO uint32_t AC_DRC1_LPFLRT;                     /*!< Offset 0x720 DRC1 Left Peak Filter Low Release Time Coef Register */
    __IO uint32_t AC_DRC1_RPFHRT;                     /*!< Offset 0x724 DRC1 Right Peak filter High Release Time Coef Register */
    __IO uint32_t AC_DRC1_RPFLRT;                     /*!< Offset 0x728 DRC1 Right Peak filter Low Release Time Coef Register */
    __IO uint32_t AC_DRC1_LRMSHAT;                    /*!< Offset 0x72C DRC1 Left RMS Filter High Coef Register */
    __IO uint32_t AC_DRC1_LRMSLAT;                    /*!< Offset 0x730 DRC1 Left RMS Filter Low Coef Register */
    __IO uint32_t AC_DRC1_RRMSHAT;                    /*!< Offset 0x734 DRC1 Right RMS Filter High Coef Register */
    __IO uint32_t AC_DRC1_RRMSLAT;                    /*!< Offset 0x738 DRC1 Right RMS Filter Low Coef Register */
    __IO uint32_t AC_DRC1_HCT;                        /*!< Offset 0x73C DRC1 Compressor Theshold High Setting Register */
    __IO uint32_t AC_DRC1_LCT;                        /*!< Offset 0x740 DRC1 Compressor Slope High Setting Register */
    __IO uint32_t AC_DRC1_HKC;                        /*!< Offset 0x744 DRC1 Compressor Slope High Setting Register */
    __IO uint32_t AC_DRC1_LKC;                        /*!< Offset 0x748 DRC1 Compressor Slope Low Setting Register */
    __IO uint32_t AC_DRC1_HOPC;                       /*!< Offset 0x74C DRC1 Compressor High Output at Compressor Threshold */
    __IO uint32_t AC_DRC1_LOPC;                       /*!< Offset 0x750 DRC1 Compressor Low Output at Compressor Threshold Register */
    __IO uint32_t AC_DRC1_HLT;                        /*!< Offset 0x754 DRC1 Limiter Theshold High Setting Register */
    __IO uint32_t AC_DRC1_LLT;                        /*!< Offset 0x758 DRC1 Limiter Theshold Low Setting Register */
    __IO uint32_t AC_DRC1_HKl;                        /*!< Offset 0x75C DRC1 Limiter Slope High Setting Register */
    __IO uint32_t AC_DRC1_LKl;                        /*!< Offset 0x760 DRC1 Limiter Slope Low Setting Register */
    __IO uint32_t AC_DRC1_HOPL;                       /*!< Offset 0x764 DRC1 Limiter High Output at Limiter Threshold */
    __IO uint32_t AC_DRC1_LOPL;                       /*!< Offset 0x768 DRC1 Limiter Low Output at Limiter Threshold */
    __IO uint32_t AC_DRC1_HET;                        /*!< Offset 0x76C DRC1 Expander Theshold High Setting Register */
    __IO uint32_t AC_DRC1_LET;                        /*!< Offset 0x770 DRC1 Expander Theshold Low Setting Register */
    __IO uint32_t AC_DRC1_HKE;                        /*!< Offset 0x774 DRC1 Expander Slope High Setting Register */
    __IO uint32_t AC_DRC1_LKE;                        /*!< Offset 0x778 DRC1 Expander Slope Low Setting Register */
    __IO uint32_t AC_DRC1_HOPE;                       /*!< Offset 0x77C DRC1 Expander High Output at Expander Threshold */
    __IO uint32_t AC_DRC1_LOPE;                       /*!< Offset 0x780 DRC1 Expander Low Output at Expander Threshold */
    __IO uint32_t AC_DRC1_HKN;                        /*!< Offset 0x784 DRC1 Linear Slope High Setting Register */
    __IO uint32_t AC_DRC1_LKN;                        /*!< Offset 0x788 DRC1 Linear Slope Low Setting Register */
    __IO uint32_t AC_DRC1_SFHAT;                      /*!< Offset 0x78C DRC1 Smooth filter Gain High Attack Time Coef Register */
    __IO uint32_t AC_DRC1_SFLAT;                      /*!< Offset 0x790 DRC1 Smooth filter Gain Low Attack Time Coef Register */
    __IO uint32_t AC_DRC1_SFHRT;                      /*!< Offset 0x794 DRC1 Smooth filter Gain High Release Time Coef Register */
    __IO uint32_t AC_DRC1_SFLRT;                      /*!< Offset 0x798 DRC1 Smooth filter Gain Low Release Time Coef Register */
    __IO uint32_t AC_DRC1_MXGHS;                      /*!< Offset 0x79C DRC1 MAX Gain High Setting Register */
    __IO uint32_t AC_DRC1_MXGLS;                      /*!< Offset 0x7A0 DRC1 MAX Gain Low Setting Register */
    __IO uint32_t AC_DRC1_MNGHS;                      /*!< Offset 0x7A4 DRC1 MIN Gain High Setting Register */
    __IO uint32_t AC_DRC1_MNGLS;                      /*!< Offset 0x7A8 DRC1 MIN Gain Low Setting Register */
    __IO uint32_t AC_DRC1_EPSHC;                      /*!< Offset 0x7AC DRC1 Expander Smooth Time High Coef Register */
    __IO uint32_t AC_DRC1_EPSLC;                      /*!< Offset 0x7B0 DRC1 Expander Smooth Time Low Coef Register */
    __IO uint32_t AC_DRC1_OPT;                        /*!< Offset 0x7B4 DRC1 Optimum Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x7B8 */
/*
 * @brief CCU
 */
/*!< CCU  */
typedef struct CCU_Type
{
    __IO uint32_t PLL_CPUX_CTRL_REG;                  /*!< Offset 0x000 PLL_CPUX Control Register  */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t PLL_AUDIO_CTRL_REG;                 /*!< Offset 0x008 PLL_AUDIO Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t PLL_VIDEO0_CTRL_REG;                /*!< Offset 0x010 PLL_VIDEO0 Control Register */
         RESERVED(0x014[0x0018 - 0x0014], uint8_t)
    __IO uint32_t PLL_VE_CTRL_REG;                    /*!< Offset 0x018 PLL_VE Control Register  */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t PLL_DDR0_CTRL_REG;                  /*!< Offset 0x020 PLL_DDR0 Control Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t PLL_PERIPH0_CTRL_REG;               /*!< Offset 0x028 PLL_PERIPH0 Control Register  */
    __IO uint32_t PLL_PERIPH1_CTRL_REG;               /*!< Offset 0x02C PLL_PERIPH1 Control Register  */
    __IO uint32_t PLL_VIDEO1_CTRL_REG;                /*!< Offset 0x030 PLL_VIDEO1 Control Register */
         RESERVED(0x034[0x0038 - 0x0034], uint8_t)
    __IO uint32_t PLL_GPU_CTRL_REG;                   /*!< Offset 0x038 PLL_GPU Control Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t PLL_MIPI_CTRL_REG;                  /*!< Offset 0x040 PLL_MIPI Control Register */
    __IO uint32_t PLL_HSIC_CTRL_REG;                  /*!< Offset 0x044 PLL_HSIC Control Register  */
    __IO uint32_t PLL_DE_CTRL_REG;                    /*!< Offset 0x048 PLL_DE Control Register */
    __IO uint32_t PLL_DDR1_CTRL_REG;                  /*!< Offset 0x04C PLL_DDR1 Control Register */
    __IO uint32_t CPUX_AXI_CFG_REG;                   /*!< Offset 0x050 CPUX/AXI Configuration Register  */
    __IO uint32_t AHB1_APB1_CFG_REG;                  /*!< Offset 0x054 AHB1/APB1 Configuration Register */
    __IO uint32_t APB2_CFG_REG;                       /*!< Offset 0x058 APB2 Configuration Register */
    __IO uint32_t AHB2_CFG_REG;                       /*!< Offset 0x05C AHB2 Configuration Register */
    __IO uint32_t BUS_CLK_GATING_REG0;                /*!< Offset 0x060 Bus Clock Gating Register 0 */
    __IO uint32_t BUS_CLK_GATING_REG1;                /*!< Offset 0x064 Bus Clock Gating Register 1 */
    __IO uint32_t BUS_CLK_GATING_REG2;                /*!< Offset 0x068 Bus Clock Gating Register 2 */
    __IO uint32_t BUS_CLK_GATING_REG3;                /*!< Offset 0x06C Bus Clock Gating Register 3 */
    __IO uint32_t BUS_CLK_GATING_REG4;                /*!< Offset 0x070 Bus Clock Gating Register 4 */
    __IO uint32_t THS_CLK_REG;                        /*!< Offset 0x074 THS Clock Register */
         RESERVED(0x078[0x0080 - 0x0078], uint8_t)
    __IO uint32_t NAND_CLK_REG;                       /*!< Offset 0x080 NAND Clock Register */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IO uint32_t SDMMC0_CLK_REG;                     /*!< Offset 0x088 SDMMC0 Clock Register */
    __IO uint32_t SDMMC1_CLK_REG;                     /*!< Offset 0x08C SDMMC1 Clock Register */
    __IO uint32_t SDMMC2_CLK_REG;                     /*!< Offset 0x090 SDMMC2 Clock Register */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IO uint32_t TS_CLK_REG;                         /*!< Offset 0x098 TS Clock Register */
    __IO uint32_t CE_CLK_REG;                         /*!< Offset 0x09C CE Clock Register */
    __IO uint32_t SPI0_CLK_REG;                       /*!< Offset 0x0A0 SPI0 Clock Register */
    __IO uint32_t SPI1_CLK_REG;                       /*!< Offset 0x0A4 SPI1 Clock Register */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IO uint32_t I2S_PCM_0_CLK_REG;                  /*!< Offset 0x0B0 I2S/PCM-0 Clock Register */
    __IO uint32_t I2S_PCM_1_CLK_REG;                  /*!< Offset 0x0B4 I2S/PCM-1 Clock Register */
    __IO uint32_t I2S_PCM_2_CLK_REG;                  /*!< Offset 0x0B8 I2S/PCM-2 Clock Register */
         RESERVED(0x0BC[0x00C0 - 0x00BC], uint8_t)
    __IO uint32_t SPDIF_CLK_REG;                      /*!< Offset 0x0C0 SPDIF Clock Register */
         RESERVED(0x0C4[0x00CC - 0x00C4], uint8_t)
    __IO uint32_t USBPHY_CFG_REG;                     /*!< Offset 0x0CC USBPHY Configuration Register */
         RESERVED(0x0D0[0x00F4 - 0x00D0], uint8_t)
    __IO uint32_t DRAM_CFG_REG;                       /*!< Offset 0x0F4 DRAM Configuration Register */
    __IO uint32_t PLL_DDR_CFG_REG;                    /*!< Offset 0x0F8 PLL_DDR Configuration Register */
    __IO uint32_t MBUS_RST_REG;                       /*!< Offset 0x0FC MBUS Reset Register */
    __IO uint32_t DRAM_CLK_GATING_REG;                /*!< Offset 0x100 DRAM Clock Gating Register */
    __IO uint32_t DE_CLK_REG;                         /*!< Offset 0x104 DE Clock Register */
         RESERVED(0x108[0x0118 - 0x0108], uint8_t)
    __IO uint32_t TCON0_CLK_REG;                      /*!< Offset 0x118 TCON0 Clock Register */
    __IO uint32_t TCON1_CLK_REG;                      /*!< Offset 0x11C TCON1 Clock Register */
         RESERVED(0x120[0x0124 - 0x0120], uint8_t)
    __IO uint32_t DEINTERLACE_CLK_REG;                /*!< Offset 0x124 DEINTERLACE Clock Register */
         RESERVED(0x128[0x0130 - 0x0128], uint8_t)
    __IO uint32_t CSI_MISC_CLK_REG;                   /*!< Offset 0x130 CSI_MISC Clock Register */
    __IO uint32_t CSI_CLK_REG;                        /*!< Offset 0x134 CSI Clock Register */
         RESERVED(0x138[0x013C - 0x0138], uint8_t)
    __IO uint32_t VE_CLK_REG;                         /*!< Offset 0x13C VE Clock Register */
    __IO uint32_t AC_DIG_CLK_REG;                     /*!< Offset 0x140 AC Digital Clock Register */
    __IO uint32_t AVS_CLK_REG;                        /*!< Offset 0x144 AVS Clock Register */
         RESERVED(0x148[0x0150 - 0x0148], uint8_t)
    __IO uint32_t HDMI_CLK_REG;                       /*!< Offset 0x150 HDMI Clock Register */
    __IO uint32_t HDMI_SLOW_CLK_REG;                  /*!< Offset 0x154 HDMI Slow Clock Register */
         RESERVED(0x158[0x015C - 0x0158], uint8_t)
    __IO uint32_t MBUS_CLK_REG;                       /*!< Offset 0x15C MBUS Clock Register */
         RESERVED(0x160[0x0168 - 0x0160], uint8_t)
    __IO uint32_t MIPI_DSI_CLK_REG;                   /*!< Offset 0x168 (null) */
         RESERVED(0x16C[0x01A0 - 0x016C], uint8_t)
    __IO uint32_t GPU_CLK_REG;                        /*!< Offset 0x1A0 GPU Clock Register */
         RESERVED(0x1A4[0x0200 - 0x01A4], uint8_t)
    __IO uint32_t PLL_STABLE_TIME_REG0;               /*!< Offset 0x200 PLL Stable Time Register0 */
    __IO uint32_t PLL_STABLE_TIME_REG1;               /*!< Offset 0x204 PLL Stable Time Register1 */
         RESERVED(0x208[0x021C - 0x0208], uint8_t)
    __IO uint32_t PLL_PERIPH1_BIAS_REG;               /*!< Offset 0x21C PLL_PERIPH1 Bias Register */
    __IO uint32_t PLL_CPUX_BIAS_REG;                  /*!< Offset 0x220 PLL_CPUX Bias Register */
    __IO uint32_t PLL_AUDIO_BIAS_REG;                 /*!< Offset 0x224 PLL_AUDIO Bias Register */
    __IO uint32_t PLL_VIDEO0_BIAS_REG;                /*!< Offset 0x228 PLL_VIDEO0 Bias Register */
    __IO uint32_t PLL_VE_BIAS_REG;                    /*!< Offset 0x22C PLL_VE Bias Register */
    __IO uint32_t PLL_DDR0_BIAS_REG;                  /*!< Offset 0x230 PLL_DDR0 Bias Register */
    __IO uint32_t PLL_PERIPH0_BIAS_REG;               /*!< Offset 0x234 PLL_PERIPH0 Bias Register */
    __IO uint32_t PLL_VIDEO1_BIAS_REG;                /*!< Offset 0x238 PLL_VIDEO1 Bias Register */
    __IO uint32_t PLL_GPU_BIAS_REG;                   /*!< Offset 0x23C PLL_GPU Bias Register */
    __IO uint32_t PLL_MIPI_BIAS_REG;                  /*!< Offset 0x240 PLL_MIPI Bias Register */
    __IO uint32_t PLL_HSIC_BIAS_REG;                  /*!< Offset 0x244 PLL_HSIC Bias Register */
    __IO uint32_t PLL_DE_BIAS_REG;                    /*!< Offset 0x248 PLL_DE Bias Register */
    __IO uint32_t PLL_DDR1_BIAS_REG;                  /*!< Offset 0x24C PLL_DDR1 Bias Register */
    __IO uint32_t PLL_CPUX_TUN_REG;                   /*!< Offset 0x250 PLL_CPUX Tuning Register */
         RESERVED(0x254[0x0260 - 0x0254], uint8_t)
    __IO uint32_t PLL_DDR0_TUN_REG;                   /*!< Offset 0x260 PLL_DDR0 Tuning Register */
         RESERVED(0x264[0x0270 - 0x0264], uint8_t)
    __IO uint32_t PLL_MIPI_TUN_REG;                   /*!< Offset 0x270 PLL_MIPI Tuning Register */
         RESERVED(0x274[0x027C - 0x0274], uint8_t)
    __IO uint32_t PLL_PERIPH1_PAT_CTRL_REG;           /*!< Offset 0x27C PLL_PERIPH1 Pattern Control Register */
    __IO uint32_t PLL_CPUX_PAT_CTRL_REG;              /*!< Offset 0x280 PLL_CPUX Pattern Control Register  */
    __IO uint32_t PLL_AUDIO_PAT_CTRL_REG;             /*!< Offset 0x284 PLL_AUDIO Pattern Control Register */
    __IO uint32_t PLL_VIDEO0_PAT_CTRL_REG;            /*!< Offset 0x288 PLL_VIDEO0 Pattern Control Register */
    __IO uint32_t PLL_VE_PAT_CTRL_REG;                /*!< Offset 0x28C PLL_VE Pattern Control Register */
    __IO uint32_t PLL_DDR0_PAT_CTRL_REG;              /*!< Offset 0x290 PLL_DDR0 Pattern Control Register */
         RESERVED(0x294[0x0298 - 0x0294], uint8_t)
    __IO uint32_t PLL_VIDEO1_PAT_CTRL_REG;            /*!< Offset 0x298 PLL_VIDEO1 Pattern Control Register */
    __IO uint32_t PLL_GPU_PAT_CTRL_REG;               /*!< Offset 0x29C PLL_GPU Pattern Control Register */
    __IO uint32_t PLL_MIPI_PAT_CTRL_REG;              /*!< Offset 0x2A0 PLL_MIPI Pattern Control Register */
    __IO uint32_t PLL_HSIC_PAT_CTRL_REG;              /*!< Offset 0x2A4 PLL_HSIC Pattern Control Register */
    __IO uint32_t PLL_DE_PAT_CTRL_REG;                /*!< Offset 0x2A8 PLL_DE Pattern Control Register */
    __IO uint32_t PLL_DDR1_PAT_CTRL_REG0;             /*!< Offset 0x2AC PLL_DDR1 Pattern Control Register0 */
    __IO uint32_t PLL_DDR1_PAT_CTRL_REG1;             /*!< Offset 0x2B0 PLL_DDR1 Pattern Control Register1 */
         RESERVED(0x2B4[0x02C0 - 0x02B4], uint8_t)
    __IO uint32_t BUS_SOFT_RST_REG0;                  /*!< Offset 0x2C0 Bus Software Reset Register 0 */
    __IO uint32_t BUS_SOFT_RST_REG1;                  /*!< Offset 0x2C4 Bus Software Reset Register 1 */
    __IO uint32_t BUS_SOFT_RST_REG2;                  /*!< Offset 0x2C8 Bus Software Reset Register 2 */
         RESERVED(0x2CC[0x02D0 - 0x02CC], uint8_t)
    __IO uint32_t BUS_SOFT_RST_REG3;                  /*!< Offset 0x2D0 Bus Software Reset Register 3 */
         RESERVED(0x2D4[0x02D8 - 0x02D4], uint8_t)
    __IO uint32_t BUS_SOFT_RST_REG4;                  /*!< Offset 0x2D8 Bus Software Reset Register 4 */
         RESERVED(0x2DC[0x02F0 - 0x02DC], uint8_t)
    __IO uint32_t CCM_SEC_SWITCH_REG;                 /*!< Offset 0x2F0 CCM Security Switch Register */
         RESERVED(0x2F4[0x0300 - 0x02F4], uint8_t)
    __IO uint32_t PS_CTRL_REG;                        /*!< Offset 0x300 PS Control Register */
    __IO uint32_t PS_CNT_REG;                         /*!< Offset 0x304 PS Counter Register */
         RESERVED(0x308[0x0320 - 0x0308], uint8_t)
    __IO uint32_t PLL_LOCK_CTRL_REG;                  /*!< Offset 0x320 PLL Lock Control Register */
} CCU_TypeDef; /* size of structure = 0x324 */
/*
 * @brief CE
 */
/*!< CE  */
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
 * @brief CPUX_CFG
 */
/*!< CPUX_CFG  */
typedef struct CPUX_CFG_Type
{
    __IO uint32_t C_CTRL_REG0;                        /*!< Offset 0x000 Cluster Control Register0 */
    __IO uint32_t C_CTRL_REG1;                        /*!< Offset 0x004 Cluster Control Register1 */
    __IO uint32_t CACHE_CFG_REG0;                     /*!< Offset 0x008 Cache parameters configuration register0 */
    __IO uint32_t CACHE_CFG_REG1;                     /*!< Offset 0x00C Cache parameters configuration register1 */
         RESERVED(0x010[0x0028 - 0x0010], uint8_t)
    __IO uint32_t GENER_CTRL_REG0;                    /*!< Offset 0x028 General Control Register0 */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t C_CPU_STATUS;                       /*!< Offset 0x030 Cluster CPU Status Register */
         RESERVED(0x034[0x003C - 0x0034], uint8_t)
    __IO uint32_t L2_STATUS_REG;                      /*!< Offset 0x03C L2 Status Register */
         RESERVED(0x040[0x0080 - 0x0040], uint8_t)
    __IO uint32_t C_RST_CTRL;                         /*!< Offset 0x080 Cluster Reset Control Register */
         RESERVED(0x084[0x00A0 - 0x0084], uint8_t)
    struct
    {
        __IO uint32_t LOW;                            /*!< Offset 0x0A0 Reset Vector Base Address Registerx_L */
        __IO uint32_t HIGH;                           /*!< Offset 0x0A4 Reset Vector Base Address Registerx_H */
    } RVBARADDR [0x004];                              /*!< Offset 0x0A0 Reset Vector Base Address Register for core [0..3] */
} CPUX_CFG_TypeDef; /* size of structure = 0x0C0 */
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
/*!< DE_BLD  */
typedef struct DE_BLD_Type
{
    __IO uint32_t BLD_EN_COLOR_CTL;                   /*!< Offset 0x000 BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
    struct
    {
        __IO uint32_t BLD_FILL_COLOR;                 /*!< Offset 0x004 BLD fill color register */
        __IO uint32_t BLD_CH_ISIZE;                   /*!< Offset 0x008 BLD input memory size register */
        __IO uint32_t BLD_CH_OFFSET;                  /*!< Offset 0x00C BLD input memory offset register */
             RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..5] */
         RESERVED(0x064[0x0080 - 0x0064], uint8_t)
    __IO uint32_t ROUTE;                              /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    __IO uint32_t PREMULTIPLY;                        /*!< Offset 0x084 BLD pre-multiply control register */
    __IO uint32_t BKCOLOR;                            /*!< Offset 0x088  */
    __IO uint32_t OUTPUT_SIZE;                        /*!< Offset 0x08C  */
    __IO uint32_t BLD_MODE [0x006];                   /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE blender0..blaener3 (or more) */
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
 * @brief DE_CSR
 */
/*!< DE_CSR Copy & Rotation */
typedef struct DE_CSR_Type
{
    __IO uint32_t CSR_CTL;                            /*!< Offset 0x000 (null) */
    __IO uint32_t INT;                                /*!< Offset 0x004 Interrupt register */
         RESERVED(0x008[0x0020 - 0x0008], uint8_t)
    __IO uint32_t IFMT;                               /*!< Offset 0x020 Input data attribute register */
    __IO uint32_t IDATA_SIZE;                         /*!< Offset 0x024 Input data size register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t IDATA_MEN_PITCH0;                   /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    __IO uint32_t IDATA_MEN_PITCH1;                   /*!< Offset 0x034 Input U/UV memory pitch register */
    __IO uint32_t IDATA_MEN_PITCH2;                   /*!< Offset 0x038 Input V memory pitch register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t IMEN_LADD0;                         /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    __IO uint32_t IMEN_HADD0;                         /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    __IO uint32_t IMEN_LADD1;                         /*!< Offset 0x048 Input U/UV memory address register0 */
    __IO uint32_t IMEN_HADD1;                         /*!< Offset 0x04C Input U/UV memory address register1 */
    __IO uint32_t IMEN_LADD2;                         /*!< Offset 0x050 Input V memory address register0 */
    __IO uint32_t IMEN_HADD2;                         /*!< Offset 0x054 Input V memory address register1 */
         RESERVED(0x058[0x0084 - 0x0058], uint8_t)
    __IO uint32_t ODATA_SIZE;                         /*!< Offset 0x084 Output data size register */
         RESERVED(0x088[0x0090 - 0x0088], uint8_t)
    __IO uint32_t ODATA_MEN_PITCH0;                   /*!< Offset 0x090 (null) */
    __IO uint32_t ODATA_MEN_PITCH1;                   /*!< Offset 0x094 (null) */
    __IO uint32_t ODATA_MEN_PITCH2;                   /*!< Offset 0x098 (null) */
         RESERVED(0x09C[0x00A0 - 0x009C], uint8_t)
    __IO uint32_t OMEN_LADD0;                         /*!< Offset 0x0A0 Output Y/RGB/ARGB memory address register0 */
    __IO uint32_t OMEN_HADD0;                         /*!< Offset 0x0A4 Output Y/RGB/ARGB memory address register1 */
    __IO uint32_t OMEN_LADD1;                         /*!< Offset 0x0A8 Output U/UV memory address register0 */
    __IO uint32_t OMEN_HADD1;                         /*!< Offset 0x0AC Output U/UV memory address register1 */
    __IO uint32_t OMEN_LADD2;                         /*!< Offset 0x0B0 Output V memory address register0 */
    __IO uint32_t OMEN_HADD2;                         /*!< Offset 0x0B4 Output V memory address register1 */
} DE_CSR_TypeDef; /* size of structure = 0x0B8 */
/*
 * @brief DE_GLB
 */
/*!< DE_GLB  */
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
/*!< DE_TOP Display Engine Top */
typedef struct DE_TOP_Type
{
    __IO uint32_t SCLK_GATE;                          /*!< Offset 0x000 DE SCLK Gating Register */
    __IO uint32_t HCLK_GATE;                          /*!< Offset 0x004 DE HCLK Gating Register */
    __IO uint32_t AHB_RESET;                          /*!< Offset 0x008 DE AHB Reset register */
    __IO uint32_t SCLK_DIV;                           /*!< Offset 0x00C DE SCLK Division register */
    __IO uint32_t DE2TCON_MUX;                        /*!< Offset 0x010 MUX register */
    __IO uint32_t CMD_CTL;                            /*!< Offset 0x014  */
} DE_TOP_TypeDef; /* size of structure = 0x018 */
/*
 * @brief DE_UI
 */
/*!< DE_UI  */
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
} DE_UI_TypeDef; /* size of structure = 0x08C */
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
 * @brief DE_VI
 */
/*!< DE_VI  */
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
} DE_VI_TypeDef; /* size of structure = 0x100 */
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
 * @brief DE_WB
 */
/*!< DE_WB Real-time write-back controller (RT-WB) */
typedef struct DE_WB_Type
{
    __IO uint32_t WB_GCTRL_REG;                       /*!< Offset 0x000 Module general control register */
    __IO uint32_t WB_SIZE_REG;                        /*!< Offset 0x004 Input size register */
    __IO uint32_t WB_CROP_COORD_REG;                  /*!< Offset 0x008 Cropping coordinate register */
    __IO uint32_t WB_CROP_SIZE_REG;                   /*!< Offset 0x00C Cropping size register */
    __IO uint32_t WB_A_CH0_ADDR_REG;                  /*!< Offset 0x010 Write-back Group A channel 0 address register */
    __IO uint32_t WB_A_CH1_ADDR_REG;                  /*!< Offset 0x014 Write-back Group A channel 1 address register */
    __IO uint32_t WB_A_CH2_ADDR_REG;                  /*!< Offset 0x018 Write-back Group A channel 2 address register */
    __IO uint32_t WB_A_HIGH_ADDR_REG;                 /*!< Offset 0x01C Write-back Group A address high bit register */
    __IO uint32_t WB_B_CH0_ADDR_REG;                  /*!< Offset 0x020 Write-back Group B channel 0 address register */
    __IO uint32_t WB_B_CH1_ADDR_REG;                  /*!< Offset 0x024 Write-back Group B channel 1 address register */
    __IO uint32_t WB_B_CH2_ADDR_REG;                  /*!< Offset 0x028 Write-back Group B channel 2 address register */
    __IO uint32_t WB_B_HIGH_ADDR_REG;                 /*!< Offset 0x02C Write-back Group B address high bit register */
    __IO uint32_t WB_CH0_PITCH_REG;                   /*!< Offset 0x030 Write-back channel 0 pitch register */
    __IO uint32_t WB_CH12_PITCH_REG;                  /*!< Offset 0x034 Write-back channel 1/2 pitch register */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IO uint32_t WB_ADDR_SWITCH_REG;                 /*!< Offset 0x040 Write-back address switch setting register */
    __IO uint32_t WB_FORMAT_REG;                      /*!< Offset 0x044 Output format register */
    __IO uint32_t WB_INT_REG;                         /*!< Offset 0x048 Interrupt control register */
    __IO uint32_t WB_STATUS_REG;                      /*!< Offset 0x04C Module status register */
         RESERVED(0x050[0x0054 - 0x0050], uint8_t)
    __IO uint32_t WB_BYPASS_REG;                      /*!< Offset 0x054 Bypass control register */
         RESERVED(0x058[0x0070 - 0x0058], uint8_t)
    __IO uint32_t WB_CS_HORZ_REG;                     /*!< Offset 0x070 Coarse scaling horizontal setting register */
    __IO uint32_t WB_CS_VERT_REG;                     /*!< Offset 0x074 Coarse scaling vertical setting register */
         RESERVED(0x078[0x0080 - 0x0078], uint8_t)
    __IO uint32_t WB_FS_INSIZE_REG;                   /*!< Offset 0x080 Fine scaling input size register */
    __IO uint32_t WB_FS_OUTSIZE_REG;                  /*!< Offset 0x084 Fine scaling output size register */
    __IO uint32_t WB_FS_HSTEP_REG;                    /*!< Offset 0x088 Fine scaling horizontal step registe */
    __IO uint32_t WB_FS_VSTEP_REG;                    /*!< Offset 0x08C Fine scaling vertical step register */
         RESERVED(0x090[0x00FC - 0x0090], uint8_t)
    __IO uint32_t WB_DEBUG_REG;                       /*!< Offset 0x0FC Debug register */
         RESERVED(0x100[0x0200 - 0x0100], uint8_t)
    __IO uint32_t WB_CH0_HCOEF_REGN [0x010];          /*!< Offset 0x200 0x200 + N*4 Channel 0 horizontal coefficient register N ( N = 0,1,2,...,15) */
         RESERVED(0x240[0x0280 - 0x0240], uint8_t)
    __IO uint32_t WB_CH1_HCOEF_REGN [0x010];          /*!< Offset 0x280 0x280 + N*4 Channel 1/2 horizontal coefficient register N ( N = 0,1,2,...,15) */
} DE_WB_TypeDef; /* size of structure = 0x2C0 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
    __IO uint32_t DMAC_IRQ_EN_REG;                    /*!< Offset 0x000 DMAC IRQ Enable Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t DMAC_IRQ_PEND_REG;                  /*!< Offset 0x010 DMAC IRQ Pending Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t DMA_SEC_REG;                        /*!< Offset 0x020 DMA Security Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t DMAC_AUTO_GATE_REG;                 /*!< Offset 0x028 DMAC Auto Gating Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __I  uint32_t DMAC_STA_REG;                       /*!< Offset 0x030 DMAC Status Register */
         RESERVED(0x034[0x0100 - 0x0034], uint8_t)
    struct
    {
        __IO uint32_t DMAC_EN_REGN;                   /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 7) 0x0100 + N*0x0040 */
        __IO uint32_t DMAC_PAU_REGN;                  /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 7) 0x0104 + N*0x0040 */
        __IO uint32_t DMAC_DESC_ADDR_REGN;            /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 7) 0x0108 + N*0x0040 */
        __I  uint32_t DMAC_CFG_REGN;                  /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 7) 0x010C + N*0x0040 */
        __I  uint32_t DMAC_CUR_SRC_REGN;              /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 7) 0x0110 + N*0x0040 */
        __I  uint32_t DMAC_CUR_DEST_REGN;             /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 7) 0x0114 + N*0x0040 */
        __I  uint32_t DMAC_BCNT_LEFT_REGN;            /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 7) 0x0118 + N*0x0040 */
        __I  uint32_t DMAC_PARA_REGN;                 /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 7) 0x011C + N*0x0040 */
             RESERVED(0x020[0x0028 - 0x0020], uint8_t)
        __IO uint32_t DMAC_MODE_REGN;                 /*!< Offset 0x128 DMAC Mode Register N (N = 0 to 7) 0x0128 + N*0x0040 */
        __I  uint32_t DMAC_FDESC_ADDR_REGN;           /*!< Offset 0x12C DMAC Former Descriptor Address Register N (N = 0 to 7) 0x012C + N*0x0040 */
        __I  uint32_t DMAC_PKG_NUM_REGN;              /*!< Offset 0x130 DMAC Package Number Register N (N = 0 to 7) 0x0130 + N*0x0040 */
             RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    } CH [0x008];                                     /*!< Offset 0x100 Channel [0..15] */
         RESERVED(0x300[0x1000 - 0x0300], uint8_t)
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
} DMIC_TypeDef; /* size of structure = 0x044 */
/*
 * @brief DRAMC
 */
/*!< DRAMC  */
typedef struct DRAMC_Type
{
    __IO uint32_t ccr;                                /*!< Offset 0x000 controller configuration register */
    __IO uint32_t dcr;                                /*!< Offset 0x004 dram configuration register */
    __IO uint32_t iocr;                               /*!< Offset 0x008 i/o configuration register */
    __IO uint32_t csr;                                /*!< Offset 0x00C controller status register */
    __IO uint32_t drr;                                /*!< Offset 0x010 dram refresh register */
    __IO uint32_t tpr0;                               /*!< Offset 0x014 dram timing parameters register 0 */
    __IO uint32_t tpr1;                               /*!< Offset 0x018 dram timing parameters register 1 */
    __IO uint32_t tpr2;                               /*!< Offset 0x01C dram timing parameters register 2 */
    __IO uint32_t gdllcr;                             /*!< Offset 0x020 global dll control register */
         RESERVED(0x024[0x004C - 0x0024], uint8_t)
    __IO uint32_t rslr0;                              /*!< Offset 0x04C rank system latency register */
    __IO uint32_t rslr1;                              /*!< Offset 0x050 rank system latency register */
         RESERVED(0x054[0x005C - 0x0054], uint8_t)
    __IO uint32_t rdgr0;                              /*!< Offset 0x05C rank dqs gating register */
    __IO uint32_t rdgr1;                              /*!< Offset 0x060 rank dqs gating register */
         RESERVED(0x064[0x0098 - 0x0064], uint8_t)
    __IO uint32_t odtcr;                              /*!< Offset 0x098 odt configuration register */
    __IO uint32_t dtr0;                               /*!< Offset 0x09C data training register 0 */
    __IO uint32_t dtr1;                               /*!< Offset 0x0A0 data training register 1 */
    __IO uint32_t dtar;                               /*!< Offset 0x0A4 data training address register */
    __IO uint32_t zqcr0;                              /*!< Offset 0x0A8 zq control register 0 */
    __IO uint32_t zqcr1;                              /*!< Offset 0x0AC zq control register 1 */
    __IO uint32_t zqsr;                               /*!< Offset 0x0B0 zq status register */
    __IO uint32_t idcr;                               /*!< Offset 0x0B4 initializaton delay configure reg */
         RESERVED(0x0B8[0x01F0 - 0x00B8], uint8_t)
    __IO uint32_t mr;                                 /*!< Offset 0x1F0 mode register */
    __IO uint32_t emr;                                /*!< Offset 0x1F4 extended mode register */
    __IO uint32_t emr2;                               /*!< Offset 0x1F8 (null) */
    __IO uint32_t emr3;                               /*!< Offset 0x1FC extended mode register */
    __IO uint32_t dllctr;                             /*!< Offset 0x200 dll control register */
    __IO uint32_t dllcr [0x005];                      /*!< Offset 0x204 dll control register 0(byte 0)..4(byte 4) */
    __IO uint32_t dqtr0;                              /*!< Offset 0x218 dq timing register */
    __IO uint32_t dqtr1;                              /*!< Offset 0x21C dq timing register */
    __IO uint32_t dqtr2;                              /*!< Offset 0x220 dq timing register */
    __IO uint32_t dqtr3;                              /*!< Offset 0x224 dq timing register */
    __IO uint32_t dqstr;                              /*!< Offset 0x228 dqs timing register */
    __IO uint32_t dqsbtr;                             /*!< Offset 0x22C dqsb timing register */
    __IO uint32_t mcr;                                /*!< Offset 0x230 mode configure register */
         RESERVED(0x234[0x023C - 0x0234], uint8_t)
    __IO uint32_t ppwrsctl;                           /*!< Offset 0x23C pad power save control */
    __IO uint32_t apr;                                /*!< Offset 0x240 arbiter period register */
    __IO uint32_t pldtr;                              /*!< Offset 0x244 priority level data threshold reg */
         RESERVED(0x248[0x0250 - 0x0248], uint8_t)
    __IO uint32_t hpcr [0x020];                       /*!< Offset 0x250 host port configure register */
         RESERVED(0x2D0[0x02E0 - 0x02D0], uint8_t)
    __IO uint32_t csel;                               /*!< Offset 0x2E0 controller select register */
} DRAMC_TypeDef; /* size of structure = 0x2E4 */
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
    __IO uint32_t GP_FIFO_DATA;                       /*!< Offset 0x014 GPADC FIFO Data Register */
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
    __IO uint32_t GP_CH0_DATA;                        /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef; /* size of structure = 0x084 */
/*
 * @brief GPIO
 */
/*!< GPIO  */
typedef struct GPIO_Type
{
    __IO uint32_t CFG [0x004];                        /*!< Offset 0x000 Configure Register */
    __IO uint32_t DATA;                               /*!< Offset 0x010 Data Register */
    __IO uint32_t DRV [0x002];                        /*!< Offset 0x014 Multi_Driving Register */
    __IO uint32_t PULL [0x002];                       /*!< Offset 0x01C Pull Register */
} GPIO_TypeDef; /* size of structure = 0x024 */
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
        __IO uint32_t DRV [0x002];                    /*!< Offset 0x014 Multi_Driving Register */
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x008];                              /*!< Offset 0x000 GPIO pin control */
         RESERVED(0x120[0x0200 - 0x0120], uint8_t)
    struct
    {
        __IO uint32_t EINT_CFG [0x004];               /*!< Offset 0x200 External Interrupt Configure Registers */
        __IO uint32_t EINT_CTL;                       /*!< Offset 0x210 External Interrupt Control Register */
        __IO uint32_t EINT_STATUS;                    /*!< Offset 0x214 External Interrupt Status Register */
        __IO uint32_t EINT_DEB;                       /*!< Offset 0x218 External Interrupt Debounce Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } GPIO_INTS [0x008];                              /*!< Offset 0x200 GPIO interrupt control */
} GPIOBLOCK_TypeDef; /* size of structure = 0x300 */
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
 * @brief GPU
 */
/*!< GPU Mali-400MP2 */
typedef struct GPU_Type
{
    __IO uint32_t MALIGP2_MGMT_VSCL_START_ADDR;       /*!< Offset 0x000 MALIGP2_REG_ADDR_MGMT_VSCL_START_ADDR */
    __IO uint32_t MALIGP2_MGMT_VSCL_END_ADDR;         /*!< Offset 0x004 MALIGP2_REG_ADDR_MGMT_VSCL_END_ADDR */
    __IO uint32_t MALIGP2_MGMT_PLBUCL_START_ADDR;     /*!< Offset 0x008 MALIGP2_REG_ADDR_MGMT_PLBUCL_START_ADDR */
    __IO uint32_t MALIGP2_MGMT_PLBUCL_END_ADDR;       /*!< Offset 0x00C MALIGP2_REG_ADDR_MGMT_PLBUCL_END_ADDR */
    __IO uint32_t MALIGP2_MGMT_PLBU_ALLOC_START_ADDR; /*!< Offset 0x010 MALIGP2_REG_ADDR_MGMT_PLBU_ALLOC_START_ADDR */
    __IO uint32_t MALIGP2_MGMT_PLBU_ALLOC_END_ADDR;   /*!< Offset 0x014 MALIGP2_REG_ADDR_MGMT_PLBU_ALLOC_END_ADDR */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t MALIGP2_MGMT_CMD;                   /*!< Offset 0x020 MALIGP2_REG_ADDR_MGMT_CMD */
    __IO uint32_t MALIGP2_MGMT_INT_RAWSTAT;           /*!< Offset 0x024 MALIGP2_REG_ADDR_MGMT_INT_RAWSTAT */
    __IO uint32_t MALIGP2_MGMT_INT_CLEAR;             /*!< Offset 0x028 MALIGP2_REG_ADDR_MGMT_INT_CLEAR */
    __IO uint32_t MALIGP2_MGMT_INT_MASK;              /*!< Offset 0x02C MALIGP2_REG_ADDR_MGMT_INT_MASK */
    __IO uint32_t MALIGP2_MGMT_INT_STAT;              /*!< Offset 0x030 MALIGP2_REG_ADDR_MGMT_INT_STAT */
         RESERVED(0x034[0x003C - 0x0034], uint8_t)
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_0_ENABLE;     /*!< Offset 0x03C MALIGP2_REG_ADDR_MGMT_PERF_CNT_0_ENABLE */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_1_ENABLE;     /*!< Offset 0x040 MALIGP2_REG_ADDR_MGMT_PERF_CNT_1_ENABLE */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_0_SRC;        /*!< Offset 0x044 MALIGP2_REG_ADDR_MGMT_PERF_CNT_0_SRC */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_1_SRC;        /*!< Offset 0x048 MALIGP2_REG_ADDR_MGMT_PERF_CNT_1_SRC */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_0_VALUE;      /*!< Offset 0x04C MALIGP2_REG_ADDR_MGMT_PERF_CNT_0_VALUE */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_1_VALUE;      /*!< Offset 0x050 MALIGP2_REG_ADDR_MGMT_PERF_CNT_1_VALUE */
    __IO uint32_t MALIGP2_MGMT_PERF_CNT_0_LIMIT;      /*!< Offset 0x054 MALIGP2_REG_ADDR_MGMT_PERF_CNT_0_LIMIT */
         RESERVED(0x058[0x0068 - 0x0058], uint8_t)
    __IO uint32_t MALIGP2_MGMT_STATUS;                /*!< Offset 0x068 MALIGP2_REG_ADDR_MGMT_STATUS */
    __IO uint32_t MALIGP2_MGMT_VERSION;               /*!< Offset 0x06C MALIGP2_REG_ADDR_MGMT_VERSION */
         RESERVED(0x070[0x0080 - 0x0070], uint8_t)
    __IO uint32_t MALIGP2_MGMT_VSCL_START_ADDR_READ;  /*!< Offset 0x080 MALIGP2_REG_ADDR_MGMT_VSCL_START_ADDR_READ */
    __IO uint32_t MALIGP2_MGMT_PLBCL_START_ADDR_READ; /*!< Offset 0x084 MALIGP2_REG_ADDR_MGMT_PLBCL_START_ADDR_READ */
         RESERVED(0x088[0x0094 - 0x0088], uint8_t)
    __IO uint32_t MALIGP2_CONTR_AXI_BUS_ERROR_STAT;   /*!< Offset 0x094 MALIGP2_CONTR_AXI_BUS_ERROR_STAT */
    __IO uint32_t MALIGP2_REGISTER_ADDRESS_SPACE_SIZE;/*!< Offset 0x098 MALIGP2_REGISTER_ADDRESS_SPACE_SIZE */
         RESERVED(0x09C[0x1000 - 0x009C], uint8_t)
    __IO uint32_t MALI200_MGMT_VERSION;               /*!< Offset 0x1000 MALI200_REG_ADDR_MGMT_VERSION */
    __IO uint32_t MALI200_MGMT_CURRENT_REND_LIST_ADDR;/*!< Offset 0x1004 MALI200_REG_ADDR_MGMT_CURRENT_REND_LIST_ADDR */
    __IO uint32_t MALI200_MGMT_STATUS;                /*!< Offset 0x1008 MALI200_REG_ADDR_MGMT_STATUS */
    __IO uint32_t MALI200_MGMT_CTRL_MGMT;             /*!< Offset 0x100C MALI200_REG_ADDR_MGMT_CTRL_MGMT */
         RESERVED(0x1010[0x1020 - 0x1010], uint8_t)
    __IO uint32_t MALI200_MGMT_INT_RAWSTAT;           /*!< Offset 0x1020 MALI200_REG_ADDR_MGMT_INT_RAWSTAT */
    __IO uint32_t MALI200_MGMT_INT_CLEAR;             /*!< Offset 0x1024 MALI200_REG_ADDR_MGMT_INT_CLEAR */
    __IO uint32_t MALI200_MGMT_INT_MASK;              /*!< Offset 0x1028 MALI200_REG_ADDR_MGMT_INT_MASK */
    __IO uint32_t MALI200_MGMT_INT_STATUS;            /*!< Offset 0x102C MALI200_REG_ADDR_MGMT_INT_STATUS */
         RESERVED(0x1030[0x1050 - 0x1030], uint8_t)
    __IO uint32_t MALI200_MGMT_BUS_ERROR_STATUS;      /*!< Offset 0x1050 MALI200_REG_ADDR_MGMT_BUS_ERROR_STATUS */
         RESERVED(0x1054[0x1080 - 0x1054], uint8_t)
    __IO uint32_t MALI200_MGMT_PERF_CNT_0_ENABLE;     /*!< Offset 0x1080 MALI200_REG_ADDR_MGMT_PERF_CNT_0_ENABLE */
    __IO uint32_t MALI200_MGMT_PERF_CNT_0_SRC;        /*!< Offset 0x1084 MALI200_REG_ADDR_MGMT_PERF_CNT_0_SRC */
    __IO uint32_t MALI200_MGMT_PERF_CNT_0_LIMIT;      /*!< Offset 0x1088 MALI200_REG_ADDR_MGMT_PERF_CNT_0_LIMIT */
    __IO uint32_t MALI200_MGMT_PERF_CNT_0_VALUE;      /*!< Offset 0x108C MALI200_REG_ADDR_MGMT_PERF_CNT_0_VALUE */
         RESERVED(0x1090[0x10A0 - 0x1090], uint8_t)
    __IO uint32_t MALI200_MGMT_PERF_CNT_1_ENABLE;     /*!< Offset 0x10A0 MALI200_REG_ADDR_MGMT_PERF_CNT_1_ENABLE */
    __IO uint32_t MALI200_MGMT_PERF_CNT_1_SRC;        /*!< Offset 0x10A4 MALI200_REG_ADDR_MGMT_PERF_CNT_1_SRC */
         RESERVED(0x10A8[0x10AC - 0x10A8], uint8_t)
    __IO uint32_t MALI200_MGMT_PERF_CNT_1_VALUE;      /*!< Offset 0x10AC MALI200_REG_ADDR_MGMT_PERF_CNT_1_VALUE */
    __IO uint32_t MALI200_MGMT_PERFMON_CONTR;         /*!< Offset 0x10B0 MALI200_REG_ADDR_MGMT_PERFMON_CONTR */
    __IO uint32_t MALI200_MGMT_PERFMON_BASE [0x00F];  /*!< Offset 0x10B4 MALI200_REG_ADDR_MGMT_PERFMON_BASE */
    __IO uint32_t MALI200_REG_SIZEOF_REGISTER_BANK;   /*!< Offset 0x10F0 MALI200_REG_SIZEOF_REGISTER_BANK */
} GPU_TypeDef; /* size of structure = 0x10F4 */
/*
 * @brief HDMI_PHY
 */
/*!< HDMI_PHY  */
typedef struct HDMI_PHY_Type
{
    __IO uint32_t HDMI_PHY_POL;                       /*!< Offset 0x000 tbd */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t HDMI_PHY_READ_EN;                   /*!< Offset 0x010 tbd */
    __IO uint32_t HDMI_PHY_UNSCRAMBLE;                /*!< Offset 0x014 tbd */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t HDMI_PHY_CFG1;                      /*!< Offset 0x020 tbd */
    __IO uint32_t HDMI_PHY_CFG2;                      /*!< Offset 0x024 tbd */
    __IO uint32_t HDMI_PHY_CFG3;                      /*!< Offset 0x028 tbd */
    __IO uint32_t HDMI_PHY_PLL1;                      /*!< Offset 0x02C tbd */
    __IO uint32_t HDMI_PHY_PLL2;                      /*!< Offset 0x030 tbd */
    __IO uint32_t HDMI_PHY_PLL3;                      /*!< Offset 0x034 tbd */
    __IO uint32_t HDMI_PHY_STS;                       /*!< Offset 0x038 tbd */
    __IO uint32_t HDMI_PHY_CEC;                       /*!< Offset 0x03C tbd */
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
    __IO uint8_t  HDMI_DESIGN_ID;                     /*!< Offset 0x000  */
    __IO uint8_t  HDMI_REVISION_ID;                   /*!< Offset 0x001  */
    __IO uint8_t  HDMI_PRODUCT_ID0;                   /*!< Offset 0x002  */
    __IO uint8_t  HDMI_PRODUCT_ID1;                   /*!< Offset 0x003  */
    __IO uint8_t  HDMI_CONFIG0_ID;                    /*!< Offset 0x004  */
    __IO uint8_t  HDMI_CONFIG1_ID;                    /*!< Offset 0x005  */
    __IO uint8_t  HDMI_CONFIG2_ID;                    /*!< Offset 0x006  */
    __IO uint8_t  HDMI_CONFIG3_ID;                    /*!< Offset 0x007  */
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
/*!< HSTIMER  */
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
} HSTIMER_TypeDef; /* size of structure = 0x054 */
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM I2S/PCM Controller */
typedef struct I2S_PCM_Type
{
    __IO uint32_t I2S_PCM_CTL;                        /*!< Offset 0x000 I2S/PCM Control Register */
    __IO uint32_t I2S_PCM_FMT0;                       /*!< Offset 0x004 I2S/PCM Format Register 0 */
    __IO uint32_t I2S_PCM_FMT1;                       /*!< Offset 0x008 I2S/PCM Format Register 1 */
    __IO uint32_t I2S_PCM_ISTA;                       /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    __IO uint32_t I2S_PCM_RXFIFO;                     /*!< Offset 0x010 I2S/PCM RX FIFO Register */
    __IO uint32_t I2S_PCM_FCTL;                       /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    __IO uint32_t I2S_PCM_FSTA;                       /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    __IO uint32_t I2S_PCM_INT;                        /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    __IO uint32_t I2S_PCM_TXFIFO;                     /*!< Offset 0x020 I2S/PCM TX FIFO Register */
    __IO uint32_t I2S_PCM_CLKD;                       /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    __IO uint32_t I2S_PCM_TXCNT;                      /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    __IO uint32_t I2S_PCM_RXCNT;                      /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    __IO uint32_t I2S_PCM_CHCFG;                      /*!< Offset 0x030 I2S/PCM Channel Configuration register */
    __IO uint32_t I2S_PCM_TX0CHSEL;                   /*!< Offset 0x034 I2S/PCM TX0 Channel Configuration register I2S/PCM_TXnCHSEL */
    __IO uint32_t I2S_PCM_TX1CHSEL;                   /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register I2S/PCM_TXnCHSEL */
    __IO uint32_t I2S_PCM_TX2CHSEL;                   /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register I2S/PCM_TXnCHSEL */
    __IO uint32_t I2S_PCM_TX3CHSEL;                   /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register I2S/PCM_TXnCHSEL */
    __IO uint32_t I2S_PCM_TX0CHMAP;                   /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register I2S/PCM_TXnCHMAP */
    __IO uint32_t I2S_PCM_TX1CHMAP;                   /*!< Offset 0x048 I2S/PCM TX1 Channel Mapping Register I2S/PCM_TXnCHMAP */
    __IO uint32_t I2S_PCM_TX2CHMAP;                   /*!< Offset 0x04C I2S/PCM TX2 Channel Mapping Register I2S/PCM_TXnCHMAP */
    __IO uint32_t I2S_PCM_TX3CHMAP;                   /*!< Offset 0x050 I2S/PCM TX3 Channel Mapping Register I2S/PCM_TXnCHMAP */
    __IO uint32_t I2S_PCM_RXCHSEL;                    /*!< Offset 0x054 I2S/PCM RX Channel Select register */
    __IO uint32_t I2S_PCM_RXCHMAP;                    /*!< Offset 0x058 I2S/PCM RX Channel Mapping Register */
} I2S_PCM_TypeDef; /* size of structure = 0x05C */
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
 * @brief MSGBOX
 */
/*!< MSGBOX  */
typedef struct MSGBOX_Type
{
    __IO uint32_t MSGBOX_CTRL_REG0;                   /*!< Offset 0x000 Message Queue Attribute Control Register 0 */
    __IO uint32_t MSGBOX_CTRL_REG1;                   /*!< Offset 0x004 Message Queue Attribute Control Register 1 */
         RESERVED(0x008[0x0040 - 0x0008], uint8_t)
    struct
    {
        __IO uint32_t MSGBOXU_IRQ_EN_REG;             /*!< Offset 0x040 IRQ Enable For User N(N=0,1) */
             RESERVED(0x004[0x0010 - 0x0004], uint8_t)
        __IO uint32_t MSGBOXU_IRQ_STATUS_REG;         /*!< Offset 0x050 IRQ Status For User N(N=0,1) */
    } USER [0x002];                                   /*!< Offset 0x040 User [0..1] */
         RESERVED(0x068[0x0100 - 0x0068], uint8_t)
    __IO uint32_t MSGBOXM_FIFO_STATUS_REG [0x008];    /*!< Offset 0x100 FIFO Status For Message Queue N(N = 0~7) */
         RESERVED(0x120[0x0140 - 0x0120], uint8_t)
    __IO uint32_t MSGBOXM_MSG_STATUS_REG [0x008];     /*!< Offset 0x140 Message Status For Message Queue N(N=0~7) */
         RESERVED(0x160[0x0180 - 0x0160], uint8_t)
    __IO uint32_t MSGBOXM_MSG_REG [0x008];            /*!< Offset 0x180 Message Register For Message Queue N(N=0~7) */
} MSGBOX_TypeDef; /* size of structure = 0x1A0 */
/*
 * @brief NDFC
 */
/*!< NDFC NAND Flash Controller Interface */
typedef struct NDFC_Type
{
    __IO uint32_t NDFC_CTL;                           /*!< Offset 0x000 NDFC Configure and Control Register */
    __IO uint32_t NDFC_ST;                            /*!< Offset 0x004 NDFC Status Information Register */
    __IO uint32_t NDFC_INT;                           /*!< Offset 0x008 NDFC Interrupt Control Register */
    __IO uint32_t NDFC_TIMING_CTL;                    /*!< Offset 0x00C NDFC Timing Control Register */
    __IO uint32_t NDFC_TIMING_CFG;                    /*!< Offset 0x010 NDFC Timing Configure Register */
    __IO uint32_t NDFC_ADDR_LOW;                      /*!< Offset 0x014 NDFC Low Word Address Register */
    __IO uint32_t NDFC_ADDR_HIGH;                     /*!< Offset 0x018 NDFC High Word Address Register */
    __IO uint32_t NDFC_BLOCK_NUM;                     /*!< Offset 0x01C NDFC Data Block Number Register */
    __IO uint32_t NDFC_CNT;                           /*!< Offset 0x020 NDFC Data Counter for data transfer Register */
    __IO uint32_t NDFC_CMD;                           /*!< Offset 0x024 Set up NDFC commands Register */
    __IO uint32_t NDFC_RCMD_SET;                      /*!< Offset 0x028 Read Command Set Register for vendor’s NAND memory */
    __IO uint32_t NDFC_WCMD_SET;                      /*!< Offset 0x02C Write Command Set Register for vendor’s NAND memory */
         RESERVED(0x030[0x0034 - 0x0030], uint8_t)
    __IO uint32_t NDFC_ECC_CTL;                       /*!< Offset 0x034 ECC Configure and Control Register */
    __IO uint32_t NDFC_ECC_ST;                        /*!< Offset 0x038 ECC Status and Operation information Register */
    __IO uint32_t NDFC_EFR;                           /*!< Offset 0x03C Enhanced Feature Register */
    __IO uint32_t NDFC_ERR_CNT0;                      /*!< Offset 0x040 Corrected Error Bit Counter Register 0 */
    __IO uint32_t NDFC_ERR_CNT1;                      /*!< Offset 0x044 Corrected Error Bit Counter Register 1 */
         RESERVED(0x048[0x0050 - 0x0048], uint8_t)
    __IO uint32_t NDFC_USER_DATA [0x010];             /*!< Offset 0x050 User Data Field Register n (n from 0 to 15) */
    __IO uint32_t NDFC_EFNAND_STA;                    /*!< Offset 0x090 EFNAND Status Register */
         RESERVED(0x094[0x00A0 - 0x0094], uint8_t)
    __IO uint32_t NDFC_SPARE_AREA;                    /*!< Offset 0x0A0 Spare Area Configure Register */
    __IO uint32_t NDFC_PAT_ID;                        /*!< Offset 0x0A4 Pattern ID Register */
    __IO uint32_t NDFC_RDATA_STA_CTL;                 /*!< Offset 0x0A8 Read Data Status Control Register */
    __IO uint32_t NDFC_RDATA_STA_0;                   /*!< Offset 0x0AC Read Data Status Register 0 */
    __IO uint32_t NDFC_RDATA_STA_1;                   /*!< Offset 0x0B0 Read Data Status Register 1 */
         RESERVED(0x0B4[0x00C0 - 0x00B4], uint8_t)
    __IO uint32_t NDFC_MDMA_ADDR;                     /*!< Offset 0x0C0 MBUS DMA Address Register */
    __IO uint32_t NDFC_MDMA_CNT;                      /*!< Offset 0x0C4 MBUS DMA Data Counter Register */
         RESERVED(0x0C8[0x00D0 - 0x00C8], uint8_t)
    __IO uint32_t NDFC_NDMA_MODE_CTL;                 /*!< Offset 0x0D0 DFC Normal DMA Mode Control Register */
         RESERVED(0x0D4[0x0300 - 0x00D4], uint8_t)
    __IO uint32_t NDFC_IO_DATA;                       /*!< Offset 0x300 Data Input/ Output Port Address Register */
         RESERVED(0x304[0x0400 - 0x0304], uint8_t)
    __IO uint8_t  RAM0_BASE [0x400];                  /*!< Offset 0x400 1024 Bytes RAM0 base */
    __IO uint8_t  RAM1_BASE [0x400];                  /*!< Offset 0x800 1024 Bytes RAM1 base */
} NDFC_TypeDef; /* size of structure = 0xC00 */
/*
 * @brief OWA
 */
/*!< OWA One Wire Audio */
typedef struct OWA_Type
{
    __IO uint32_t OWA_GEN_CTL;                        /*!< Offset 0x000 OWA General Control Register */
    __IO uint32_t OWA_TX_CFIG;                        /*!< Offset 0x004 OWA TX Configuration Register */
    __IO uint32_t OWA_RX_CFIG;                        /*!< Offset 0x008 OWA RX Configuration Register */
    __IO uint32_t OWA_ISTA;                           /*!< Offset 0x00C OWA Interrupt Status Register */
    __IO uint32_t OWA_RXFIFO;                         /*!< Offset 0x010 OWA RXFIFO Register */
    __IO uint32_t OWA_FCTL;                           /*!< Offset 0x014 OWA FIFO Control Register */
    __IO uint32_t OWA_FSTA;                           /*!< Offset 0x018 OWA FIFO Status Register */
    __IO uint32_t OWA_INT;                            /*!< Offset 0x01C OWA Interrupt Control Register */
    __IO uint32_t OWA_TX_FIFO;                        /*!< Offset 0x020 OWA TX FIFO Register */
    __IO uint32_t OWA_TX_CNT;                         /*!< Offset 0x024 OWA TX Counter Register */
    __IO uint32_t OWA_RX_CNT;                         /*!< Offset 0x028 OWA RX Counter Register */
    __IO uint32_t OWA_TX_CHSTA0;                      /*!< Offset 0x02C OWA TX Channel Status Register0 */
    __IO uint32_t OWA_TX_CHSTA1;                      /*!< Offset 0x030 OWA TX Channel Status Register1 */
    __IO uint32_t OWA_RXCHSTA0;                       /*!< Offset 0x034 OWA RX Channel Status Register0 */
    __IO uint32_t OWA_RXCHSTA1;                       /*!< Offset 0x038 OWA RX Channel Status Register1 */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t OWA_EXP_CTL;                        /*!< Offset 0x040 OWA Expand Control Register */
    __IO uint32_t OWA_EXP_ISTA;                       /*!< Offset 0x044 OWA Expand Interrupt Status Register */
    __IO uint32_t OWA_EXP_INFO_0;                     /*!< Offset 0x048 OWA Expand Infomation Register0 */
    __IO uint32_t OWA_EXP_INFO_1;                     /*!< Offset 0x04C OWA Expand Infomation Register1 */
    __IO uint32_t OWA_EXP_DBG_0;                      /*!< Offset 0x050 OWA Expand Debug Register0 */
    __IO uint32_t OWA_EXP_DBG_1;                      /*!< Offset 0x054 OWA Expand Debug Register1 */
} OWA_TypeDef; /* size of structure = 0x058 */
/*
 * @brief PWM
 */
/*!< PWM  */
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
 * @brief RTC
 */
/*!< RTC  */
typedef struct RTC_Type
{
    __IO uint32_t LOSC_CTRL_REG;                      /*!< Offset 0x000 Low Oscillator Control Register */
    __IO uint32_t LOSC_AUTO_SWT_STA_REG;              /*!< Offset 0x004 LOSC Auto Switch Status Register */
    __IO uint32_t INTOSC_CLK_PRESCAL_REG;             /*!< Offset 0x008 Internal OSC Clock Prescalar Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t RTC_YY_MM_DD_REG;                   /*!< Offset 0x010 RTC Year-Month-Day Register */
    __IO uint32_t RTC_HH_MM_SS_REG;                   /*!< Offset 0x014 RTC Hour-Minute-Second Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t ALARM0_COUNTER_REG;                 /*!< Offset 0x020 Alarm 0 Counter Register  */
    __IO uint32_t ALARM0_CUR_VLU_REG;                 /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    __IO uint32_t ALARM0_ENABLE_REG;                  /*!< Offset 0x028 Alarm 0 Enable Register */
    __IO uint32_t ALARM0_IRQ_EN;                      /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    __IO uint32_t ALARM0_IRQ_STA_REG;                 /*!< Offset 0x030 Alarm 0 IRQ Status Register */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t ALARM1_WK_HH_MM_SS;                 /*!< Offset 0x040 Alarm 1 Week HMS Register  */
    __IO uint32_t ALARM1_ENABLE_REG;                  /*!< Offset 0x044 Alarm 1 Enable Register */
    __IO uint32_t ALARM1_IRQ_EN;                      /*!< Offset 0x048 Alarm 1 IRQ Enable Register */
    __IO uint32_t ALARM1_IRQ_STA_REG;                 /*!< Offset 0x04C (null) */
    __IO uint32_t ALARM_CONFIG_REG;                   /*!< Offset 0x050 Alarm Config Register */
         RESERVED(0x054[0x0060 - 0x0054], uint8_t)
    __IO uint32_t LOSC_OUT_GATING_REG;                /*!< Offset 0x060 LOSC output gating register */
         RESERVED(0x064[0x0100 - 0x0064], uint8_t)
    __IO uint32_t GP_DATA_REG [0x004];                /*!< Offset 0x100 General Purpose Register (N=0~3) */
         RESERVED(0x110[0x0180 - 0x0110], uint8_t)
    __IO uint32_t GPL_HOLD_OUTPUT_REG;                /*!< Offset 0x180 GPL Hold Output Register */
         RESERVED(0x184[0x0190 - 0x0184], uint8_t)
    __IO uint32_t VDD_RTC_REG;                        /*!< Offset 0x190 VDD RTC Regulate Register */
         RESERVED(0x194[0x01F0 - 0x0194], uint8_t)
    __IO uint32_t IC_CHARA_REG;                       /*!< Offset 0x1F0 IC Characteristic Register */
         RESERVED(0x1F4[0x0210 - 0x01F4], uint8_t)
    __IO uint32_t CRY_CONFIG_REG;                     /*!< Offset 0x210 Crypt Configuration Register */
    __IO uint32_t CRY_KEY_REG;                        /*!< Offset 0x214 Crypt Key Register */
    __IO uint32_t CRY_ENABLE_REG;                     /*!< Offset 0x218 Crypt Enable Register */
} RTC_TypeDef; /* size of structure = 0x21C */
/*
 * @brief R_PRCM
 */
/*!< R_PRCM  */
typedef struct R_PRCM_Type
{
    __IO uint32_t CPUS_CLK_REG;                       /*!< Offset 0x000 CPUS Clock Register */
         RESERVED(0x004[0x000C - 0x0004], uint8_t)
    __IO uint32_t APB0_CFG_REG;                       /*!< Offset 0x00C APB0 Configuration Register */
         RESERVED(0x010[0x0028 - 0x0010], uint8_t)
    __IO uint32_t APB0_CLK_GATING_REG;                /*!< Offset 0x028 APB0 Clock Gating Register */
         RESERVED(0x02C[0x0044 - 0x002C], uint8_t)
    __IO uint32_t PLL_CTRL_REG1;                      /*!< Offset 0x044 PLL Control Register 1 */
         RESERVED(0x048[0x0054 - 0x0048], uint8_t)
    __IO uint32_t R_CIR_RX_CLK_REG;                   /*!< Offset 0x054 R_CIR_RX Clock Register */
         RESERVED(0x058[0x00B0 - 0x0058], uint8_t)
    __IO uint32_t APB0_SOFT_RST_REG;                  /*!< Offset 0x0B0 APB0 Software Reset Register */
         RESERVED(0x0B4[0x0100 - 0x00B4], uint8_t)
    __IO uint32_t C0CPUX_PWROFF_GATING_REG;           /*!< Offset 0x100 Cluster0 CPUX Power Off Gating Register */
    __IO uint32_t C1CPUX_PWROFF_GATING_REG;           /*!< Offset 0x104 Cluster1 CPUX Power Off Gating Register */
         RESERVED(0x108[0x0110 - 0x0108], uint8_t)
    __IO uint32_t VDD_SYS_PWROFF_GATING_REG;          /*!< Offset 0x110 VDD_SYS Power Off Gating Register */
         RESERVED(0x114[0x0118 - 0x0114], uint8_t)
    __IO uint32_t GPU_PWROFF_GATING_REG;              /*!< Offset 0x118 GPU Power Off Gating Register */
         RESERVED(0x11C[0x0120 - 0x011C], uint8_t)
    __IO uint32_t VDD_SYS_PWROFF_RST_REG;             /*!< Offset 0x120 VDD_SYS Power Domain Reset Register */
         RESERVED(0x124[0x0140 - 0x0124], uint8_t)
    __IO uint32_t C0_CPU0_PWR_SWITCH_CTRL;            /*!< Offset 0x140 C0_CPU0 Power Switch Control Register */
    __IO uint32_t C0_CPU1_PWR_SWITCH_CTRL;            /*!< Offset 0x144 C0_CPU1 Power Switch Control Register */
    __IO uint32_t C0_CPU2_PWR_SWITCH_CTRL;            /*!< Offset 0x148 C0_CPU2 Power Switch Control Register */
    __IO uint32_t C0_CPU3_PWR_SWITCH_CTRL;            /*!< Offset 0x14C C0_CPU3 Power Switch Control Register */
    __IO uint32_t C1_CPU0_PWR_SWITCH_CTRL;            /*!< Offset 0x150 C1_CPU0 Power Switch Control Register */
    __IO uint32_t C1_CPU1_PWR_SWITCH_CTRL;            /*!< Offset 0x154 C1_CPU1 Power Switch Control Register */
    __IO uint32_t C1_CPU2_PWR_SWITCH_CTRL;            /*!< Offset 0x158 C1_CPU2 Power Switch Control Register */
    __IO uint32_t C1_CPU3_PWR_SWITCH_CTRL;            /*!< Offset 0x15C C1_CPU3 Power Switch Control Register */
         RESERVED(0x160[0x01F0 - 0x0160], uint8_t)
    __IO uint32_t RPIO_HOLD_CTRL_REG;                 /*!< Offset 0x1F0 R_PIO Hold Control Register */
    __IO uint32_t OSC24M_CTRL_REG;                    /*!< Offset 0x1F4 OSC24M Control Register */
} R_PRCM_TypeDef; /* size of structure = 0x1F8 */
/*
 * @brief R_RSB
 */
/*!< R_RSB Reduced Serial Bus Host Controller */
typedef struct R_RSB_Type
{
    __IO uint32_t RSB_CTRL;                           /*!< Offset 0x000 RSB Control Register */
    __IO uint32_t RSB_CCR;                            /*!< Offset 0x004 RSB Clock Control Register */
    __IO uint32_t RSB_INTE;                           /*!< Offset 0x008 RSB Interrupt Enable Register */
    __IO uint32_t RSB_STAT;                           /*!< Offset 0x00C RSB Status Register */
    __IO uint32_t RSB_DADDR0;                         /*!< Offset 0x010 RSB Data Acess Address Register0 */
         RESERVED(0x014[0x0018 - 0x0014], uint8_t)
    __IO uint32_t RSB_DLEN;                           /*!< Offset 0x018 RSB Data Length Register */
    __IO uint32_t RSB_DATA0;                          /*!< Offset 0x01C RSB Data Buffer0 Register */
         RESERVED(0x020[0x0024 - 0x0020], uint8_t)
    __IO uint32_t RSB_LCR;                            /*!< Offset 0x024 RSB Line Control register */
    __IO uint32_t RSB_PMCR;                           /*!< Offset 0x028 RSB PMU Mode Control register */
    __IO uint32_t RSB_CMD;                            /*!< Offset 0x02C RSB Command Register */
    __IO uint32_t RSB_SADDR;                          /*!< Offset 0x030 RSB Slave address Register */
} R_RSB_TypeDef; /* size of structure = 0x034 */
/*
 * @brief R_TIMER
 */
/*!< R_TIMER  */
typedef struct R_TIMER_Type
{
    __IO uint32_t RTMR_IRQ_EN_REG;                    /*!< Offset 0x000 R_timer IRQ Enable Register */
    __IO uint32_t RTMR_IRQ_STA_REG;                   /*!< Offset 0x004 R_timer IRQ Status Register */
         RESERVED(0x008[0x0020 - 0x0008], uint8_t)
    __IO uint32_t RTMR0_CTRL_REG;                     /*!< Offset 0x020 R_timer 0 Control Register */
    __IO uint32_t RTMR0_INTV_VALUE_REG;               /*!< Offset 0x024 R_timer 0 Interval Value Register */
    __IO uint32_t RTMR0_CUR_VALUE_REG;                /*!< Offset 0x028 R_timer 0 Current Value Register */
         RESERVED(0x02C[0x0040 - 0x002C], uint8_t)
    __IO uint32_t RTMR1_CTRL_REG;                     /*!< Offset 0x040 R_timer 1 Control Register */
    __IO uint32_t RTMR1_INTV_VALUE_REG;               /*!< Offset 0x044 R_timer 1 Interval Value Register */
    __IO uint32_t RTMR1_CUR_VALUE_REG;                /*!< Offset 0x048 R_timer 1 Current Value Register */
} R_TIMER_TypeDef; /* size of structure = 0x04C */
/*
 * @brief R_WDOG
 */
/*!< R_WDOG  */
typedef struct R_WDOG_Type
{
    __IO uint32_t RWDOG_IRQ_EN_REG;                   /*!< Offset 0x000 R_watchdog IRQ Enable Register */
    __IO uint32_t RWDOG_IRQ_STA_REG;                  /*!< Offset 0x004 R_watchdog Status Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t RWDOG_CTRL_REG;                     /*!< Offset 0x010 R_watchdog Control Register */
    __IO uint32_t RWDOG_CFG_REG;                      /*!< Offset 0x014 R_watchdog Configuration Register */
    __IO uint32_t RWDOG_MODE_REG;                     /*!< Offset 0x018 R_watchdog Mode Register */
} R_WDOG_TypeDef; /* size of structure = 0x01C */
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
} SID_TypeDef; /* size of structure = 0x010 */
/*
 * @brief SMHC
 */
/*!< SMHC SD-MMC Host Controller */
typedef struct SMHC_Type
{
    __IO uint32_t SMHC_CTRL;                          /*!< Offset 0x000 Control Register */
    __IO uint32_t SMHC_CLKDIV;                        /*!< Offset 0x004 Clock control Register */
    __IO uint32_t SMHC_TMOUT;                         /*!< Offset 0x008 Time out Register */
    __IO uint32_t SMHC_CTYPE;                         /*!< Offset 0x00C Bus width Register */
    __IO uint32_t SMHC_BLKSIZ;                        /*!< Offset 0x010 Block size Register */
    __IO uint32_t SMHC_BYTCNT;                        /*!< Offset 0x014 Byte count Register */
    __IO uint32_t SMHC_CMD;                           /*!< Offset 0x018 Command Register */
    __IO uint32_t SMHC_CMDARG;                        /*!< Offset 0x01C Command argument Register */
    __IO uint32_t SMHC_RESP0;                         /*!< Offset 0x020 Response 0 Register */
    __IO uint32_t SMHC_RESP1;                         /*!< Offset 0x024 Response 1 Register */
    __IO uint32_t SMHC_RESP2;                         /*!< Offset 0x028 Response 2 Register */
    __IO uint32_t SMHC_RESP3;                         /*!< Offset 0x02C Response 3 Register */
    __IO uint32_t SMHC_INTMASK;                       /*!< Offset 0x030 Interrupt mask Register */
    __IO uint32_t SMHC_MINTSTS;                       /*!< Offset 0x034 Masked interrupt status Register */
    __IO uint32_t SMHC_RINTSTS;                       /*!< Offset 0x038 Raw interrupt status Register */
    __IO uint32_t SMHC_STATUS;                        /*!< Offset 0x03C Status Register */
    __IO uint32_t SMHC_FIFOTH;                        /*!< Offset 0x040 FIFO water level Register */
    __IO uint32_t SMHC_FUNS;                          /*!< Offset 0x044 FIFO function select Register */
    __IO uint32_t SMHC_TBC0;                          /*!< Offset 0x048 Transferred byte count0 between controller and card */
    __IO uint32_t SMHC_TBC1;                          /*!< Offset 0x04C Transferred byte count1 between host memory and internal FIFO */
         RESERVED(0x050[0x0054 - 0x0050], uint8_t)
    __IO uint32_t SMHC_CSDC;                          /*!< Offset 0x054 CRC status detect control Register(only for SMHC2) */
    __IO uint32_t SMHC_A12A;                          /*!< Offset 0x058 Auto command 12 argument Register */
    __IO uint32_t SMHC_NTSR;                          /*!< Offset 0x05C SD new timing set Register(only for SMHC0&1) */
         RESERVED(0x060[0x0078 - 0x0060], uint8_t)
    __IO uint32_t SMHC_HWRST;                         /*!< Offset 0x078 Hardware reset Register */
         RESERVED(0x07C[0x0080 - 0x007C], uint8_t)
    __IO uint32_t SMHC_DMAC;                          /*!< Offset 0x080 DMA control Register */
    __IO uint32_t SMHC_DLBA;                          /*!< Offset 0x084 Descriptor list base address Register */
    __IO uint32_t SMHC_IDST;                          /*!< Offset 0x088 DMAC status Register */
    __IO uint32_t SMHC_IDIE;                          /*!< Offset 0x08C DMAC interrupt enable Register */
         RESERVED(0x090[0x0100 - 0x0090], uint8_t)
    __IO uint32_t SMHC_THLD;                          /*!< Offset 0x100 Card threshold control Register */
         RESERVED(0x104[0x010C - 0x0104], uint8_t)
    __IO uint32_t SMHC_EDSD;                          /*!< Offset 0x10C eMMC4.5 DDR start bit detection control Register */
    __IO uint32_t SMHC_RES_CRC;                       /*!< Offset 0x110 Response CRC from device(only for SMHC0&1) */
    __IO uint32_t SMHC_D7_D0_CRC [0x008];             /*!< Offset 0x114 CRC in Data7..Data0 from Device (Only for SMHC0, SMHC1) */
    __IO uint32_t SMHC_CRC_STA;                       /*!< Offset 0x134 CRC status from device in write operation(only for SMHC0&1) */
         RESERVED(0x138[0x0140 - 0x0138], uint8_t)
    __IO uint32_t SMHC_DRV_DL;                        /*!< Offset 0x140 Drive delay control Register */
    __IO uint32_t SMHC_SAMP_DL;                       /*!< Offset 0x144 Sample delay control Register */
    __IO uint32_t SMHC_DS_DL;                         /*!< Offset 0x148 Data strobe delay control Register */
         RESERVED(0x14C[0x0200 - 0x014C], uint8_t)
    __IO uint32_t SMHC_FIFO;                          /*!< Offset 0x200 Read/ Write FIFO */
         RESERVED(0x204[0x1000 - 0x0204], uint8_t)
} SMHC_TypeDef; /* size of structure = 0x1000 */
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
    __IO uint32_t SPI_CCR;                            /*!< Offset 0x024 SPI Clock Rate Control register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t SPI_MBC;                            /*!< Offset 0x030 SPI Master Burst Counter Register */
    __IO uint32_t SPI_MTC;                            /*!< Offset 0x034 SPI Master Transmit Counter Register */
    __IO uint32_t SPI_BCC;                            /*!< Offset 0x038 SPI Master Burst Control Register */
         RESERVED(0x03C[0x0200 - 0x003C], uint8_t)
    __IO uint32_t SPI_TXD;                            /*!< Offset 0x200 SPI TX Data Register */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IO uint32_t SPI_RXD;                            /*!< Offset 0x300 SPI RX Data Register */
         RESERVED(0x304[0x1000 - 0x0304], uint8_t)
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SPINLOCK
 */
/*!< SPINLOCK  */
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
         RESERVED(0x000[0x0004 - 0x0000], uint8_t)
    __IO uint32_t MEMMAP_REG;                         /*!< Offset 0x004 DE memory control Register */
         RESERVED(0x008[0x0024 - 0x0008], uint8_t)
    __I  uint32_t VER_REG;                            /*!< Offset 0x024 Version Register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t EMAC_EPHY_CLK_REG0;                 /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
} SYS_CFG_TypeDef; /* size of structure = 0x034 */
/*
 * @brief TCON0
 */
/*!< TCON0 TCON0 LVDS/RGB/MIPI-DSI Interface */
typedef struct TCON0_Type
{
    __IO uint32_t TCON_GCTL_REG;                      /*!< Offset 0x000 TCON global control register */
    __IO uint32_t TCON_GINT0_REG;                     /*!< Offset 0x004 TCON global interrupt register0 */
    __IO uint32_t TCON_GINT1_REG;                     /*!< Offset 0x008 TCON global interrupt register1 */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t TCON0_FRM_CTL_REG;                  /*!< Offset 0x010 TCON FRM control register */
    __IO uint32_t TCON0_FRM_SEED_REG [0x006];         /*!< Offset 0x014 TCON FRM seed register (N=0,1,2,3,4,5) */
    __IO uint32_t TCON0_FRM_TAB_REG [0x004];          /*!< Offset 0x02C TCON FRM table register (N=0,1,2,3) */
    __IO uint32_t TCON0_3D_FIFO_REG;                  /*!< Offset 0x03C TCON0 3D fifo register */
    __IO uint32_t TCON0_CTL_REG;                      /*!< Offset 0x040 TCON0 control register */
    __IO uint32_t TCON0_DCLK_REG;                     /*!< Offset 0x044 TCON0 data clock register */
    __IO uint32_t TCON0_BASIC0_REG;                   /*!< Offset 0x048 TCON0 basic timing register0 */
    __IO uint32_t TCON0_BASIC1_REG;                   /*!< Offset 0x04C TCON0 basic timing register */
    __IO uint32_t TCON0_BASIC2_REG;                   /*!< Offset 0x050 TCON0 basic timing register2 */
    __IO uint32_t TCON0_BASIC3_REG;                   /*!< Offset 0x054 TCON0 basic timing register3 */
    __IO uint32_t TCON0_HV_IF_REG;                    /*!< Offset 0x058 TCON0 hv panel interface register */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IO uint32_t TCON0_CPU_IF_REG;                   /*!< Offset 0x060 TCON0 cpu panel interface register */
    __IO uint32_t TCON0_CPU_WR_REG;                   /*!< Offset 0x064 TCON0 cpu panel write data register */
    __IO uint32_t TCON0_CPU_RD0_REG;                  /*!< Offset 0x068 TCON0 cpu panel read data register0 */
    __IO uint32_t TCON0_CPU_RD1_REG;                  /*!< Offset 0x06C TCON0 cpu panel read data register1 */
         RESERVED(0x070[0x0084 - 0x0070], uint8_t)
    __IO uint32_t TCON0_LVDS_IF_REG;                  /*!< Offset 0x084 TCON0 lvds panel interface register */
    __IO uint32_t TCON0_IO_POL_REG;                   /*!< Offset 0x088 TCON0 IO polarity register */
    __IO uint32_t TCON0_IO_TRI_REG;                   /*!< Offset 0x08C TCON0 IO trigger register */
         RESERVED(0x090[0x0100 - 0x0090], uint8_t)
    __IO uint32_t TCON_CEU_CTL_REG;                   /*!< Offset 0x100 TCON CEU control register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IO uint32_t TCON_CEU_COEF_MUL_REG [0x00B];      /*!< Offset 0x110 TCON CEU coefficient register0 (N=0,1,2,4,5,6,8,9,10) */
    struct
    {
             RESERVED(0x000[0x000C - 0x0000], uint8_t)
        __IO uint32_t TCON_CEU_COEF_ADD_REG;          /*!< Offset 0x148 0x11C+N*0x10 TCON CEU coefficient register1 (N=0,1,2) */
             RESERVED(0x010[0x0010 - 0x0010], uint8_t)
    } TCON_CEU_COEF [0x003];                          /*!< Offset 0x13C TCON CEU coefficient register1 (N=0,1,2) */
         RESERVED(0x16C[0x01F0 - 0x016C], uint8_t)
    __IO uint32_t TCON_SAFE_PERIOD_REG;               /*!< Offset 0x1F0 TCON safe period register */
         RESERVED(0x1F4[0x0220 - 0x01F4], uint8_t)
    __IO uint32_t TCON0_LVDS_ANA0_REG;                /*!< Offset 0x220 TCON LVDS analog register0 */
         RESERVED(0x224[0x0400 - 0x0224], uint8_t)
    __IO uint32_t TCON0_GAMMA_TABLE_REG [0x100];      /*!< Offset 0x400 Gamma Table 0x400-0x7FF */
         RESERVED(0x800[0x0FF4 - 0x0800], uint8_t)
    __IO uint32_t TCON0_3D_FIFO_BIST_REG;             /*!< Offset 0xFF4  */
    __IO uint32_t TCON_TRI_FIFO_BIST_REG;             /*!< Offset 0xFF8  */
} TCON0_TypeDef; /* size of structure = 0xFFC */
/*
 * @brief TCON1
 */
/*!< TCON1 TCON1 HDMI Interface */
typedef struct TCON1_Type
{
    __IO uint32_t TCON_GCTL_REG;                      /*!< Offset 0x000 TCON global control register */
    __IO uint32_t TCON_GINT0_REG;                     /*!< Offset 0x004 TCON global interrupt register0 */
    __IO uint32_t TCON_GINT1_REG;                     /*!< Offset 0x008 TCON global interrupt register1 */
         RESERVED(0x00C[0x0090 - 0x000C], uint8_t)
    __IO uint32_t TCON1_CTL_REG;                      /*!< Offset 0x090 TCON1 control register */
    __IO uint32_t TCON1_BASIC0_REG;                   /*!< Offset 0x094 TCON1 basic timing register0 */
    __IO uint32_t TCON1_BASIC1_REG;                   /*!< Offset 0x098 TCON1 basic timing register1 */
    __IO uint32_t TCON1_BASIC2_REG;                   /*!< Offset 0x09C TCON1 basic timing register2 */
    __IO uint32_t TCON1_BASIC3_REG;                   /*!< Offset 0x0A0 TCON1 basic timing register3 */
    __IO uint32_t TCON1_BASIC4_REG;                   /*!< Offset 0x0A4 TCON1 basic timing register4 */
    __IO uint32_t TCON1_BASIC5_REG;                   /*!< Offset 0x0A8 TCON1 basic timing register5 */
         RESERVED(0x0AC[0x00B0 - 0x00AC], uint8_t)
    __IO uint32_t TCON1_PS_SYNC_REG;                  /*!< Offset 0x0B0 TCON1 sync register */
         RESERVED(0x0B4[0x00F0 - 0x00B4], uint8_t)
    __IO uint32_t TCON1_IO_POL_REG;                   /*!< Offset 0x0F0 TCON1 IO polarity register */
    __IO uint32_t TCON1_IO_TRI_REG;                   /*!< Offset 0x0F4 TCON1 IO trigger register */
         RESERVED(0x0F8[0x0100 - 0x00F8], uint8_t)
    __IO uint32_t TCON_CEU_CTL_REG;                   /*!< Offset 0x100 TCON CEU control register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IO uint32_t TCON_CEU_COEF_MUL_REG [0x00B];      /*!< Offset 0x110 TCON CEU coefficient register0 (N=0,1,2,4,5,6,8,9,10) */
         RESERVED(0x13C[0x0160 - 0x013C], uint8_t)
    __IO uint32_t TCON_CEU_COEF_RANG_REG [0x003];     /*!< Offset 0x160 TCON CEU coefficient register2 (N=0,1,2) */
         RESERVED(0x16C[0x01F0 - 0x016C], uint8_t)
    __IO uint32_t TCON_SAFE_PERIOD_REG;               /*!< Offset 0x1F0 TCON safe period register */
         RESERVED(0x1F4[0x0300 - 0x01F4], uint8_t)
    __IO uint32_t TCON1_FILL_CTL_REG;                 /*!< Offset 0x300 TCON1 fill data control register */
    struct
    {
        __IO uint32_t TCON1_FILL_BEGIN_REG;           /*!< Offset 0x304 TCON1 fill data begin register (N=0,1,2) */
        __IO uint32_t TCON1_FILL_END_REG;             /*!< Offset 0x308 TCON1 fill data end register (N=0,1,2) */
        __IO uint32_t TCON1_FILL_DATA0_REG;           /*!< Offset 0x30C TCON1 fill data value register (N=0,1,2) */
    } TCON1_FILL [0x003];                             /*!< Offset 0x304 TCON1 fill data (N=0,1,2) */
         RESERVED(0x328[0x0400 - 0x0328], uint8_t)
    __IO uint32_t TCON1_GAMMA_TABLE_REG [0x100];      /*!< Offset 0x400 Gamma Table 0x400-0x7FF */
         RESERVED(0x800[0x0FFC - 0x0800], uint8_t)
    __IO uint32_t TCON_ECC_FIFO_BIST_REG;             /*!< Offset 0xFFC  */
} TCON1_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief THS
 */
/*!< THS  */
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
         RESERVED(0x030[0x0080 - 0x0030], uint8_t)
    __IO uint32_t AVS_CNT_CTL_REG;                    /*!< Offset 0x080 AVS Control Register */
    __IO uint32_t AVS_CNT0_REG;                       /*!< Offset 0x084 AVS Counter 0 Register */
    __IO uint32_t AVS_CNT1_REG;                       /*!< Offset 0x088 AVS Counter 1 Register */
         RESERVED(0x08C[0x00A0 - 0x008C], uint8_t)
    __IO uint32_t WDOG_IRQ_EN_REG;                    /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
    __IO uint32_t WDOG_IRQ_STA_REG;                   /*!< Offset 0x0A4 Watchdog Status Register */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IO uint32_t WDOG_CTRL_REG;                      /*!< Offset 0x0B0 Watchdog Control Register */
    __IO uint32_t WDOG_CFG_REG;                       /*!< Offset 0x0B4 Watchdog Configuration Register */
    __IO uint32_t WDOG_MODE_REG;                      /*!< Offset 0x0B8 Watchdog Mode Register */
         RESERVED(0x0BC[0x0400 - 0x00BC], uint8_t)
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
    __IO uint32_t TP_DATA_REG;                        /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef; /* size of structure = 0x028 */
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
         RESERVED(0x024[0x0400 - 0x0024], uint8_t)
} TWI_TypeDef; /* size of structure = 0x400 */
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
/*!< USBOTG USB OTG Dual-Role Device controller */
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
    __IO uint32_t HCI_ICR;                            /*!< Offset 0x000 HCI Interface Register (HCI_Interface) */
    __IO uint32_t USBPHY_PHYCTL;                      /*!< Offset 0x004 (HSIC_STATUS) HSIC status Register This register is valid on HCI1. */
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
         RESERVED(0xBE8[0x1000 - 0x0BE8], uint8_t)
} VE_TypeDef; /* size of structure = 0x1000 */


/* Defines */



/* Access pointers */

#define DE_TOP ((DE_TOP_TypeDef *) DE_TOP_BASE)       /*!< DE_TOP Display Engine Top register set access pointer */
#define DE_WB ((DE_WB_TypeDef *) DE_WB_BASE)          /*!< DE_WB Real-time write-back controller (RT-WB) register set access pointer */
#define DE_CSR ((DE_CSR_TypeDef *) DE_CSR_BASE)       /*!< DE_CSR Copy & Rotation register set access pointer */
#define DE_MIXER0_GLB ((DE_GLB_TypeDef *) DE_MIXER0_GLB_BASE)/*!< DE_MIXER0_GLB  register set access pointer */
#define DE_MIXER0_BLD ((DE_BLD_TypeDef *) DE_MIXER0_BLD_BASE)/*!< DE_MIXER0_BLD  register set access pointer */
#define DE_MIXER0_VI1 ((DE_VI_TypeDef *) DE_MIXER0_VI1_BASE)/*!< DE_MIXER0_VI1  register set access pointer */
#define DE_MIXER0_UI1 ((DE_UI_TypeDef *) DE_MIXER0_UI1_BASE)/*!< DE_MIXER0_UI1  register set access pointer */
#define DE_MIXER0_UI2 ((DE_UI_TypeDef *) DE_MIXER0_UI2_BASE)/*!< DE_MIXER0_UI2  register set access pointer */
#define DE_MIXER0_UI3 ((DE_UI_TypeDef *) DE_MIXER0_UI3_BASE)/*!< DE_MIXER0_UI3  register set access pointer */
#define DE_MIXER0_VSU1 ((DE_VSU_TypeDef *) DE_MIXER0_VSU1_BASE)/*!< DE_MIXER0_VSU1 Video Scaler Unit (VSU), VS register set access pointer */
#define DE_MIXER0_UIS1 ((DE_UIS_TypeDef *) DE_MIXER0_UIS1_BASE)/*!< DE_MIXER0_UIS1 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER0_UIS2 ((DE_UIS_TypeDef *) DE_MIXER0_UIS2_BASE)/*!< DE_MIXER0_UIS2 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER0_UIS3 ((DE_UIS_TypeDef *) DE_MIXER0_UIS3_BASE)/*!< DE_MIXER0_UIS3 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER1_GLB ((DE_GLB_TypeDef *) DE_MIXER1_GLB_BASE)/*!< DE_MIXER1_GLB  register set access pointer */
#define DE_MIXER1_BLD ((DE_BLD_TypeDef *) DE_MIXER1_BLD_BASE)/*!< DE_MIXER1_BLD  register set access pointer */
#define DE_MIXER1_VI1 ((DE_VI_TypeDef *) DE_MIXER1_VI1_BASE)/*!< DE_MIXER1_VI1  register set access pointer */
#define DE_MIXER1_UI1 ((DE_UI_TypeDef *) DE_MIXER1_UI1_BASE)/*!< DE_MIXER1_UI1  register set access pointer */
#define DE_MIXER1_UI2 ((DE_UI_TypeDef *) DE_MIXER1_UI2_BASE)/*!< DE_MIXER1_UI2  register set access pointer */
#define DE_MIXER1_UI3 ((DE_UI_TypeDef *) DE_MIXER1_UI3_BASE)/*!< DE_MIXER1_UI3  register set access pointer */
#define DE_MIXER1_VSU1 ((DE_VSU_TypeDef *) DE_MIXER1_VSU1_BASE)/*!< DE_MIXER1_VSU1 Video Scaler Unit (VSU), VS register set access pointer */
#define DE_MIXER1_UIS1 ((DE_UIS_TypeDef *) DE_MIXER1_UIS1_BASE)/*!< DE_MIXER1_UIS1 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define CPUX_CFG ((CPUX_CFG_TypeDef *) CPUX_CFG_BASE) /*!< CPUX_CFG  register set access pointer */
#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)    /*!< SYS_CFG  register set access pointer */
#define DRAMC ((DRAMC_TypeDef *) DRAMC_BASE)          /*!< DRAMC  register set access pointer */
#define DMAC ((DMAC_TypeDef *) DMAC_BASE)             /*!< DMAC  register set access pointer */
#define NDFC ((NDFC_TypeDef *) NDFC_BASE)             /*!< NDFC NAND Flash Controller Interface register set access pointer */
#define TCON0 ((TCON0_TypeDef *) TCON0_BASE)          /*!< TCON0 TCON0 LVDS/RGB/MIPI-DSI Interface register set access pointer */
#define TCON1 ((TCON1_TypeDef *) TCON1_BASE)          /*!< TCON1 TCON1 HDMI Interface register set access pointer */
#define VENCODER ((VE_TypeDef *) VENCODER_BASE)       /*!< VENCODER Video Encoding register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)           /*!< SMHC0 SD-MMC Host Controller register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)           /*!< SMHC1 SD-MMC Host Controller register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)           /*!< SMHC2 SD-MMC Host Controller register set access pointer */
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID Security ID register set access pointer */
#define MSGBOX ((MSGBOX_TypeDef *) MSGBOX_BASE)       /*!< MSGBOX  register set access pointer */
#define USBOTG0 ((USBOTG_TypeDef *) USBOTG0_BASE)     /*!< USBOTG0 USB OTG Dual-Role Device controller register set access pointer */
#define USBEHCI0 ((USB_EHCI_Capability_TypeDef *) USBEHCI0_BASE)/*!< USBEHCI0  register set access pointer */
#define USBOHCI0 ((USB_OHCI_Capability_TypeDef *) USBOHCI0_BASE)/*!< USBOHCI0  register set access pointer */
#define USBPHY0 ((USBPHYC_TypeDef *) USBPHY0_BASE)    /*!< USBPHY0  register set access pointer */
#define USBEHCI1 ((USB_EHCI_Capability_TypeDef *) USBEHCI1_BASE)/*!< USBEHCI1  register set access pointer */
#define USBOHCI1 ((USB_OHCI_Capability_TypeDef *) USBOHCI1_BASE)/*!< USBOHCI1  register set access pointer */
#define USBPHY1 ((USBPHYC_TypeDef *) USBPHY1_BASE)    /*!< USBPHY1  register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU  register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)           /*!< GPIOH  register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)  /*!< GPIOINTH  register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define OWA ((OWA_TypeDef *) OWA_BASE)                /*!< OWA One Wire Audio register set access pointer */
#define PWM ((PWM_TypeDef *) PWM_BASE)                /*!< PWM  register set access pointer */
#define I2S0 ((I2S_PCM_TypeDef *) I2S0_BASE)          /*!< I2S0 I2S/PCM Controller register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1 I2S/PCM Controller register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2 I2S/PCM Controller register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)/*!< AUDIO_CODEC  register set access pointer */
#define THS ((THS_TypeDef *) THS_BASE)                /*!< THS  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define EMAC ((EMAC_TypeDef *) EMAC_BASE)             /*!< EMAC  register set access pointer */
#define GPU ((GPU_TypeDef *) GPU_BASE)                /*!< GPU Mali-400MP2 register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define HDMI_TX0 ((HDMI_TX_TypeDef *) HDMI_TX0_BASE)  /*!< HDMI_TX0  register set access pointer */
#define HDMI_PHY ((HDMI_PHY_TypeDef *) HDMI_PHY_BASE) /*!< HDMI_PHY  register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC  register set access pointer */
#define R_TIMER ((R_TIMER_TypeDef *) R_TIMER_BASE)    /*!< R_TIMER  register set access pointer */
#define R_WDOG ((R_WDOG_TypeDef *) R_WDOG_BASE)       /*!< R_WDOG  register set access pointer */
#define R_PRCM ((R_PRCM_TypeDef *) R_PRCM_BASE)       /*!< R_PRCM  register set access pointer */
#define CIR_RX ((CIR_RX_TypeDef *) CIR_RX_BASE)       /*!< CIR_RX  register set access pointer */
#define R_TWI ((TWI_TypeDef *) R_TWI_BASE)            /*!< R_TWI  register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L  register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL  register set access pointer */
#define R_RSB ((R_RSB_TypeDef *) R_RSB_BASE)          /*!< R_RSB Reduced Serial Bus Host Controller register set access pointer */
#define GPADC ((GPADC_TypeDef *) GPADC_BASE)          /*!< GPADC  register set access pointer */
#define TPADC ((TPADC_TypeDef *) TPADC_BASE)          /*!< TPADC  register set access pointer */
#define IOMMU ((IOMMU_TypeDef *) IOMMU_BASE)          /*!< IOMMU  register set access pointer */
#define DMIC ((DMIC_TypeDef *) DMIC_BASE)             /*!< DMIC  register set access pointer */
#define CE_NS ((CE_TypeDef *) CE_NS_BASE)             /*!< CE_NS  register set access pointer */
#define CE_S ((CE_TypeDef *) CE_S_BASE)               /*!< CE_S  register set access pointer */
#define DDRPHYC ((DDRPHYC_TypeDef *) DDRPHYC_BASE)    /*!< DDRPHYC  register set access pointer */
#define CPU_SUBSYS_CTRL ((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)/*!< CPU_SUBSYS_CTRL  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__DADB7421_2CE1_4351_A89A_B0338D2EB5AF__INCLUDED */
