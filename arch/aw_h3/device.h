/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
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
    PA_EINT_IRQn = 43,                                /*!< GPIOINT PA_EINT interrupt  */
    PG_EINT_IRQn = 49,                                /*!< GPIOINT PG_EINT interrupt */
    TIMER0_IRQn = 50,                                 /*!< TIMER  */
    TIMER1_IRQn = 51,                                 /*!< TIMER  */
    R_UART_IRQn = 70,                                 /*!< UART  */
    R_PL_EINT_IRQn = 77,                              /*!< GPIOINT R_PL_EINT interrupt */
    USB20_OTG_DEVICE_IRQn = 103,                      /*!< USBOTG USB OTG Dual-Role Device controller */
    USB20_HOST0_EHCI_IRQn = 104,                      /*!< USB_EHCI_Capability  */
    USB20_HOST0_OHCI_IRQn = 105,                      /*!< USB_OHCI_Capability  */
    USB20_HOST1_EHCI_IRQn = 106,                      /*!< USB_EHCI_Capability  */
    USB20_HOST1_OHCI_IRQn = 107,                      /*!< USB_OHCI_Capability  */
    USB20_HOST2_EHCI_IRQn = 108,                      /*!< USB_EHCI_Capability  */
    USB20_HOST2_OHCI_IRQn = 109,                      /*!< USB_OHCI_Capability  */
    USB20_HOST3_EHCI_IRQn = 110,                      /*!< USB_EHCI_Capability  */
    USB20_HOST3_OHCI_IRQn = 111,                      /*!< USB_OHCI_Capability  */
    TCON0_IRQn = 118,                                 /*!< TCON TCON0 interrupt */
    TCON1_IRQn = 119,                                 /*!< TCON TCON1 interrupt */
    HDMI_TX0_IRQn = 120,                              /*!< HDMI_TX  */
    DE_IRQn = 127,                                    /*!< DE_TOP Display Engine Top */
    CTI0_IRQn = 140,                                  /*!< CPUCFG CTI0 Interrupt */
    CTI1_IRQn = 141,                                  /*!< CPUCFG CTI1 Interrupt */
    CTI2_IRQn = 142,                                  /*!< CPUCFG CTI2 Interrupt */
    CTI3_IRQn = 143,                                  /*!< CPUCFG CTI3 Interrupt */
    COMMTX0_IRQn = 144,                               /*!< CPUCFG COMMTX0 Interrupt */
    COMMTX1_IRQn = 145,                               /*!< CPUCFG COMMTX1 Interrupt */
    COMMTX2_IRQn = 146,                               /*!< CPUCFG COMMTX2 Interrupt */
    COMMTX3_IRQn = 147,                               /*!< CPUCFG COMMTX3 Interrupt */
    COMMRX0_IRQn = 148,                               /*!< CPUCFG COMMRX0 Interrupt */
    COMMRX1_IRQn = 149,                               /*!< CPUCFG COMMRX1 Interrupt */
    COMMRX2_IRQn = 150,                               /*!< CPUCFG COMMRX2 Interrupt */
    COMMRX3_IRQn = 151,                               /*!< CPUCFG COMMRX3 Interrupt */
    PMU0_IRQn = 152,                                  /*!< CPUCFG PMU0 Interrupt */
    PMU1_IRQn = 153,                                  /*!< CPUCFG PMU1 Interrupt */
    PMU2_IRQn = 154,                                  /*!< CPUCFG PMU2 Interrupt */
    PMU3_IRQn = 155,                                  /*!< CPUCFG PMU3 Interrupt */
    AXI_ERROR_IRQn = 156,                             /*!< CPUCFG AXI_ERROR Interrupt */

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
#define SYSCTRL_BASE ((uintptr_t) 0x01C00000)         /*!< SYSCTRL  Base */
#define TCON0_BASE ((uintptr_t) 0x01C0C000)           /*!< TCON TCON0, TCON1 Base */
#define TCON1_BASE ((uintptr_t) 0x01C0D000)           /*!< TCON TCON0, TCON1 Base */
#define USB20_OTG_DEVICE_BASE ((uintptr_t) 0x01C19000)/*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB20_HOST0_EHCI_BASE ((uintptr_t) 0x01C1A000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST0_OHCI_BASE ((uintptr_t) 0x01C1A400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST1_EHCI_BASE ((uintptr_t) 0x01C1B000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST1_OHCI_BASE ((uintptr_t) 0x01C1B400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST2_EHCI_BASE ((uintptr_t) 0x01C1C000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST2_OHCI_BASE ((uintptr_t) 0x01C1C400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST3_EHCI_BASE ((uintptr_t) 0x01C1D000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST3_OHCI_BASE ((uintptr_t) 0x01C1D400)/*!< USB_OHCI_Capability  Base */
#define CCU_BASE ((uintptr_t) 0x01C20000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define GPIOA_BASE ((uintptr_t) 0x01C20800)           /*!< GPIO  Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x01C20800)       /*!< GPIOBLOCK  Base */
#define GPIOC_BASE ((uintptr_t) 0x01C20848)           /*!< GPIO  Base */
#define GPIOD_BASE ((uintptr_t) 0x01C2086C)           /*!< GPIO  Base */
#define GPIOE_BASE ((uintptr_t) 0x01C20890)           /*!< GPIO  Base */
#define GPIOF_BASE ((uintptr_t) 0x01C208B4)           /*!< GPIO  Base */
#define GPIOG_BASE ((uintptr_t) 0x01C208D8)           /*!< GPIO  Base */
#define GPIOINTA_BASE ((uintptr_t) 0x01C20A00)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x01C20A20)        /*!< GPIOINT  Base */
#define TIMER_BASE ((uintptr_t) 0x01C20C00)           /*!< TIMER  Base */
#define UART0_BASE ((uintptr_t) 0x01C28000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x01C28400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x01C28800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x01C28C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x01C29000)           /*!< UART  Base */
#define GIC_BASE ((uintptr_t) 0x01C80000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x01C81000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x01C82000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x01C84000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x01C85000)            /*!< GICV  Base */
#define HDMI_TX0_BASE ((uintptr_t) 0x01EE0000)        /*!< HDMI_TX  Base */
#define HDMI_PHY_BASE ((uintptr_t) 0x01EF0000)        /*!< HDMI_PHY  Base */
#define RTC_BASE ((uintptr_t) 0x01F00000)             /*!< RTC  Base */
#define R_PRCM_BASE ((uintptr_t) 0x01F01400)          /*!< R_PRCM  Base */
#define CPUCFG_BASE ((uintptr_t) 0x01F01C00)          /*!< CPUCFG CPU Configuration Base */
#define R_UART_BASE ((uintptr_t) 0x01F02800)          /*!< UART  Base */
#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK  Base */
#define GPIOL_BASE ((uintptr_t) 0x01F02C00)           /*!< GPIO  Base */

#if __aarch64__
    #include <core_ca53.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
    __IO uint32_t PLL_CPUX_CTRL_REG;                  /*!< Offset 0x000 PLL_CPUX Control Register */
         uint32_t reserved_0x004;
    __IO uint32_t PLL_AUDIO_CTRL_REG;                 /*!< Offset 0x008 PLL_AUDIO Control Register */
         uint32_t reserved_0x00C;
    __IO uint32_t PLL_VIDEO_CTRL_REG;                 /*!< Offset 0x010 PLL_VIDEO Control Register */
         uint32_t reserved_0x014;
    __IO uint32_t PLL_VE_CTRL_REG;                    /*!< Offset 0x018 PLL_VE Control Register */
         uint32_t reserved_0x01C;
    __IO uint32_t PLL_DDR_CTRL_REG;                   /*!< Offset 0x020 PLL_DDR Control Register */
         uint32_t reserved_0x024;
    __IO uint32_t PLL_PERIPH0_CTRL_REG;               /*!< Offset 0x028 PLL_PERIPH0 Control Register */
         uint32_t reserved_0x02C [0x0003];
    __IO uint32_t PLL_GPU_CTRL_REG;                   /*!< Offset 0x038 PLL_GPU Control Register */
         uint32_t reserved_0x03C [0x0002];
    __IO uint32_t PLL_PERIPH1_CTRL_REG;               /*!< Offset 0x044 PLL_PERIPH1_CTRL_REG */
    __IO uint32_t PLL_DE_CTRL_REG;                    /*!< Offset 0x048 PLL_DE Control Register */
         uint32_t reserved_0x04C;
    __IO uint32_t CPUX_AXI_CFG_REG;                   /*!< Offset 0x050 CPUX/AXI Configuration Register */
    __IO uint32_t AHB1_APB1_CFG_REG;                  /*!< Offset 0x054 AHB1/APB1 Configuration Register */
    __IO uint32_t APB2_CFG_REG;                       /*!< Offset 0x058 APB2 Configuration Register */
    __IO uint32_t AHB2_CFG_REG;                       /*!< Offset 0x05C AHB2 Configuration Register */
    __IO uint32_t BUS_CLK_GATING_REG0;                /*!< Offset 0x060 Bus Clock Gating Register 0 */
    __IO uint32_t BUS_CLK_GATING_REG1;                /*!< Offset 0x064 Bus Clock Gating Register 1 */
    __IO uint32_t BUS_CLK_GATING_REG2;                /*!< Offset 0x068 Bus Clock Gating Register 2 */
    __IO uint32_t BUS_CLK_GATING_REG3;                /*!< Offset 0x06C Bus Clock Gating Register 3 */
    __IO uint32_t BUS_CLK_GATING_REG4;                /*!< Offset 0x070 Bus Clock Gating Register4 */
    __IO uint32_t THS_CLK_REG;                        /*!< Offset 0x074 THS Clock Register */
         uint32_t reserved_0x078 [0x0002];
    __IO uint32_t NAND_CLK_REG;                       /*!< Offset 0x080 NAND Clock Register */
         uint32_t reserved_0x084;
    __IO uint32_t SDMMC0_CLK_REG;                     /*!< Offset 0x088 SDMMC0 Clock Register */
    __IO uint32_t SDMMC1_CLK_REG;                     /*!< Offset 0x08C SDMMC1 Clock Register */
    __IO uint32_t SDMMC2_CLK_REG;                     /*!< Offset 0x090 SDMMC2 Clock Register */
         uint32_t reserved_0x094 [0x0002];
    __IO uint32_t CE_CLK_REG;                         /*!< Offset 0x09C CE Clock Register */
    __IO uint32_t SPI0_CLK_REG;                       /*!< Offset 0x0A0 SPI0 Clock Register */
    __IO uint32_t SPI1_CLK_REG;                       /*!< Offset 0x0A4 SPI1 Clock Register */
         uint32_t reserved_0x0A8 [0x0002];
    __IO uint32_t I2S_PCM0_CLK_REG;                   /*!< Offset 0x0B0 I2S/PCM0 Clock Register */
    __IO uint32_t I2S_PCM1_CLK_REG;                   /*!< Offset 0x0B4 I2S/PCM1 Clock Register */
    __IO uint32_t I2S_PCM2_CLK_REG;                   /*!< Offset 0x0B8 I2S/PCM2 Clock Register */
         uint32_t reserved_0x0BC;
    __IO uint32_t OWA_CLK_REG;                        /*!< Offset 0x0C0 OWA Clock Register */
         uint32_t reserved_0x0C4 [0x0002];
    __IO uint32_t USBPHY_CFG_REG;                     /*!< Offset 0x0CC USBPHY Configuration Register */
         uint32_t reserved_0x0D0 [0x0009];
    __IO uint32_t DRAM_CFG_REG;                       /*!< Offset 0x0F4 DRAM Configuration Register */
         uint32_t reserved_0x0F8;
    __IO uint32_t MBUS_RST_REG;                       /*!< Offset 0x0FC MBUS Reset Register */
    __IO uint32_t DRAM_CLK_GATING_REG;                /*!< Offset 0x100 DRAM Clock Gating Register */
    __IO uint32_t DE_CLK_REG;                         /*!< Offset 0x104 DE Clock Register */
         uint32_t reserved_0x108 [0x0004];
    __IO uint32_t TCON0_CLK_REG;                      /*!< Offset 0x118 TCON0 Clock Register */
         uint32_t reserved_0x11C;
    __IO uint32_t TVE_CLK_REG;                        /*!< Offset 0x120 TVE Clock Register */
    __IO uint32_t DEINTERLACE_CLK_REG;                /*!< Offset 0x124 DEINTERLACE Clock Register */
         uint32_t reserved_0x128 [0x0002];
    __IO uint32_t CSI_MISC_CLK_REG;                   /*!< Offset 0x130 CSI_MISC Clock Register */
    __IO uint32_t CSI_CLK_REG;                        /*!< Offset 0x134 CSI Clock Register */
         uint32_t reserved_0x138;
    __IO uint32_t VE_CLK_REG;                         /*!< Offset 0x13C VE Clock Register */
    __IO uint32_t AC_DIG_CLK_REG;                     /*!< Offset 0x140 AC Digital Clock Register */
    __IO uint32_t AVS_CLK_REG;                        /*!< Offset 0x144 AVS Clock Register */
         uint32_t reserved_0x148 [0x0002];
    __IO uint32_t HDMI_CLK_REG;                       /*!< Offset 0x150 HDMI Clock Register */
    __IO uint32_t HDMI_SLOW_CLK_REG;                  /*!< Offset 0x154 HDMI Slow Clock Register */
         uint32_t reserved_0x158;
    __IO uint32_t MBUS_CLK_REG;                       /*!< Offset 0x15C MBUS Clock Register */
         uint32_t reserved_0x160 [0x0010];
    __IO uint32_t GPU_CLK_REG;                        /*!< Offset 0x1A0 GPU Clock Register */
         uint32_t reserved_0x1A4 [0x0017];
    __IO uint32_t PLL_STABLE_TIME_REG0;               /*!< Offset 0x200 PLL Stable Time Register 0 */
    __IO uint32_t PLL_STABLE_TIME_REG1;               /*!< Offset 0x204 PLL Stable Time Register 1 */
         uint32_t reserved_0x208 [0x0006];
    __IO uint32_t PLL_CPUX_BIAS_REG;                  /*!< Offset 0x220 PLL_CPUX Bias Register */
    __IO uint32_t PLL_AUDIO_BIAS_REG;                 /*!< Offset 0x224 PLL_AUDIO Bias Register */
    __IO uint32_t PLL_VIDEO_BIAS_REG;                 /*!< Offset 0x228 PLL_VIDEO Bias Register */
    __IO uint32_t PLL_VE_BIAS_REG;                    /*!< Offset 0x22C PLL_VE Bias Register */
    __IO uint32_t PLL_DDR_BIAS_REG;                   /*!< Offset 0x230 PLL_DDR Bias Register */
    __IO uint32_t PLL_PERIPH0_BIAS_REG;               /*!< Offset 0x234 PLL_PERIPH0 Bias Register */
         uint32_t reserved_0x238;
    __IO uint32_t PLL_GPU_BIAS_REG;                   /*!< Offset 0x23C PLL_GPU Bias Register */
         uint32_t reserved_0x240;
    __IO uint32_t PLL_PERIPH1_BIAS_REG;               /*!< Offset 0x244 PLL_PERIPH1 Bias Register */
    __IO uint32_t PLL_DE_BIAS_REG;                    /*!< Offset 0x248 PLL_DE Bias Register */
         uint32_t reserved_0x24C;
    __IO uint32_t PLL_CPUX_TUN_REG;                   /*!< Offset 0x250 PLL_CPUX Tuning Register */
         uint32_t reserved_0x254 [0x0003];
    __IO uint32_t PLL_DDR_TUN_REG;                    /*!< Offset 0x260 PLL_DDR Tuning Register */
         uint32_t reserved_0x264 [0x0007];
    __IO uint32_t PLL_CPUX_PAT_CTRL_REG;              /*!< Offset 0x280 PLL_CPUX Pattern Control Register */
    __IO uint32_t PLL_AUDIO_PAT_CTRL_REG0;            /*!< Offset 0x284 PLL_AUDIO Pattern Control Register */
    __IO uint32_t PLL_VIDEO_PAT_CTRL_REG0;            /*!< Offset 0x288 PLL_VIDEO Pattern Control Register */
    __IO uint32_t PLL_VE_PAT_CTRL_REG;                /*!< Offset 0x28C PLL_VE Pattern Control Register */
    __IO uint32_t PLL_DDR_PAT_CTRL_REG0;              /*!< Offset 0x290 PLL_DDR Pattern Control Register */
         uint32_t reserved_0x294 [0x0002];
    __IO uint32_t PLL_GPU_PAT_CTRL_REG;               /*!< Offset 0x29C PLL_GPU Pattern Control Register */
         uint32_t reserved_0x2A0;
    __IO uint32_t PLL_PERIPH1_PAT_CTRL_REG1;          /*!< Offset 0x2A4 PLL_PERIPH1 Pattern Control Register */
    __IO uint32_t PLL_DE_PAT_CTRL_REG;                /*!< Offset 0x2A8 PLL_DE Pattern Control Register */
         uint32_t reserved_0x2AC [0x0005];
    __IO uint32_t BUS_SOFT_RST_REG0;                  /*!< Offset 0x2C0 Bus Software Reset Register 0 */
    __IO uint32_t BUS_SOFT_RST_REG1;                  /*!< Offset 0x2C4 Bus Software Reset Register 1 */
    __IO uint32_t BUS_SOFT_RST_REG2;                  /*!< Offset 0x2C8 Bus Software Reset Register 2 */
         uint32_t reserved_0x2CC;
    __IO uint32_t BUS_SOFT_RST_REG3;                  /*!< Offset 0x2D0 Bus Software Reset Register 3 */
         uint32_t reserved_0x2D4;
    __IO uint32_t BUS_SOFT_RST_REG4;                  /*!< Offset 0x2D8 Bus Software Reset Register 4 */
         uint32_t reserved_0x2DC [0x0005];
    __IO uint32_t CCU_SEC_SWITCH_REG;                 /*!< Offset 0x2F0 CCU Security Switch Register */
         uint32_t reserved_0x2F4 [0x0003];
    __IO uint32_t PS_CTRL_REG;                        /*!< Offset 0x300 PS Control Register */
    __IO uint32_t PS_CNT_REG;                         /*!< Offset 0x304 PS Counter Register */
         uint32_t reserved_0x308 [0x003E];
} CCU_TypeDef; /* size of structure = 0x400 */
/*
 * @brief CPUCFG
 */
/*!< CPUCFG CPU Configuration */
typedef struct CPUCFG_Type
{
    __IO uint32_t CPUS_RST_CTRL_REG;                  /*!< Offset 0x000 CPUS reset control register */
         uint32_t reserved_0x004 [0x000F];
    struct
    {
        __IO uint32_t CPU_RST_CTRL_REG;               /*!< Offset 0x040 CPU0 reset control */
        __IO uint32_t CPU_CTRL_REG;                   /*!< Offset 0x044 CPU0 control register */
        __IO uint32_t CPU_STATUS_REG;                 /*!< Offset 0x048 CPU0 status register */
             uint32_t reserved_0x00C [0x000D];
    } CPU [0x004];                                    /*!< Offset 0x040 CPU control [0..3] */
    __IO uint32_t CPU_SYS_RST_REG;                    /*!< Offset 0x140 CPU System Reset Register */
    __IO uint32_t CPU_CLK_GATING_REG;                 /*!< Offset 0x144 CPU clock gating Register */
         uint32_t reserved_0x148 [0x000F];
    __IO uint32_t GENER_CTRL_REG;                     /*!< Offset 0x184 General Control Register */
         uint32_t reserved_0x188 [0x0006];
    __IO uint32_t SUP_STAN_FLAG_REG;                  /*!< Offset 0x1A0 Super Standby Flag Register */
    __IO uint32_t RVADDR;                             /*!< Offset 0x1A4 Hot plugin start address */
         uint32_t reserved_0x1A8 [0x0036];
    __IO uint32_t CNT64_CTRL_REG;                     /*!< Offset 0x280 64-bit Counter Control Register */
    __IO uint32_t CNT64_LOW_REG;                      /*!< Offset 0x284 64-bit Counter Low Register */
    __IO uint32_t CNT64_HIGH_REG;                     /*!< Offset 0x288 64-bit Counter High Register */
} CPUCFG_TypeDef; /* size of structure = 0x28C */
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
             uint32_t reserved_0x00C;
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..5] */
         uint32_t reserved_0x064 [0x0007];
    __IO uint32_t ROUTE;                              /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    __IO uint32_t PREMULTIPLY;                        /*!< Offset 0x084 BLD pre-multiply control register */
    __IO uint32_t BKCOLOR;                            /*!< Offset 0x088  */
    __IO uint32_t OUTPUT_SIZE;                        /*!< Offset 0x08C  */
    __IO uint32_t BLD_MODE [0x006];                   /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE blender0..blaener3 (or more) */
         uint32_t reserved_0x0A8 [0x0002];
    __IO uint32_t CK_CTL;                             /*!< Offset 0x0B0  */
    __IO uint32_t CK_CFG;                             /*!< Offset 0x0B4  */
         uint32_t reserved_0x0B8 [0x0002];
    __IO uint32_t CK_MAX [0x004];                     /*!< Offset 0x0C0  */
         uint32_t reserved_0x0D0 [0x0004];
    __IO uint32_t CK_MIN [0x004];                     /*!< Offset 0x0E0  */
         uint32_t reserved_0x0F0 [0x0003];
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
         uint32_t reserved_0x008 [0x0006];
    __IO uint32_t IFMT;                               /*!< Offset 0x020 Input data attribute register */
    __IO uint32_t IDATA_SIZE;                         /*!< Offset 0x024 Input data size register */
         uint32_t reserved_0x028 [0x0002];
    __IO uint32_t IDATA_MEN_PITCH0;                   /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    __IO uint32_t IDATA_MEN_PITCH1;                   /*!< Offset 0x034 Input U/UV memory pitch register */
    __IO uint32_t IDATA_MEN_PITCH2;                   /*!< Offset 0x038 Input V memory pitch register */
         uint32_t reserved_0x03C;
    __IO uint32_t IMEN_LADD0;                         /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    __IO uint32_t IMEN_HADD0;                         /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    __IO uint32_t IMEN_LADD1;                         /*!< Offset 0x048 Input U/UV memory address register0 */
    __IO uint32_t IMEN_HADD1;                         /*!< Offset 0x04C Input U/UV memory address register1 */
    __IO uint32_t IMEN_LADD2;                         /*!< Offset 0x050 Input V memory address register0 */
    __IO uint32_t IMEN_HADD2;                         /*!< Offset 0x054 Input V memory address register1 */
         uint32_t reserved_0x058 [0x000B];
    __IO uint32_t ODATA_SIZE;                         /*!< Offset 0x084 Output data size register */
         uint32_t reserved_0x088 [0x0002];
    __IO uint32_t ODATA_MEN_PITCH0;                   /*!< Offset 0x090 (null) */
    __IO uint32_t ODATA_MEN_PITCH1;                   /*!< Offset 0x094 (null) */
    __IO uint32_t ODATA_MEN_PITCH2;                   /*!< Offset 0x098 (null) */
         uint32_t reserved_0x09C;
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
             uint32_t reserved_0x01C;
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
         uint32_t reserved_0x004;
    __IO uint32_t UIS_STATUS_REG;                     /*!< Offset 0x008 Status register */
    __IO uint32_t UIS_FIELD_CTRL_REG;                 /*!< Offset 0x00C Field control register */
    __IO uint32_t UIS_BIST_REG;                       /*!< Offset 0x010 BIST control register */
         uint32_t reserved_0x014 [0x000B];
    __IO uint32_t UIS_OUTSIZE_REG;                    /*!< Offset 0x040 Output size register */
         uint32_t reserved_0x044 [0x000F];
    __IO uint32_t UIS_INSIZE_REG;                     /*!< Offset 0x080 Input size register */
         uint32_t reserved_0x084;
    __IO uint32_t UIS_HSTEP_REG;                      /*!< Offset 0x088 Horizontal step register */
    __IO uint32_t UIS_VSTEP_REG;                      /*!< Offset 0x08C Vertical step register */
    __IO uint32_t UIS_HPHASE_REG;                     /*!< Offset 0x090 Horizontal initial phase register */
         uint32_t reserved_0x094;
    __IO uint32_t UIS_VPHASE0_REG;                    /*!< Offset 0x098 Vertical initial phase 0 register */
    __IO uint32_t UIS_VPHASE1_REG;                    /*!< Offset 0x09C Vertical initial phase 1 register */
         uint32_t reserved_0x0A0 [0x0058];
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
         uint32_t reserved_0x004;
    __IO uint32_t VSU_STATUS_REG;                     /*!< Offset 0x008 VSU Status Register */
    __IO uint32_t VSU_FIELD_CTRL_REG;                 /*!< Offset 0x00C VSU Field Control Register */
    __IO uint32_t VSU_SCALE_MODE_REG;                 /*!< Offset 0x010 VSU Scale Mode Setting Register */
         uint32_t reserved_0x014 [0x0003];
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
         uint32_t reserved_0x048 [0x000E];
    __IO uint32_t VSU_Y_SIZE_REG;                     /*!< Offset 0x080 VSU Y Channel Size Register */
         uint32_t reserved_0x084;
    __IO uint32_t VSU_Y_HSTEP_REG;                    /*!< Offset 0x088 VSU Y Channel Horizontal Step Register */
    __IO uint32_t VSU_Y_VSTEP_REG;                    /*!< Offset 0x08C VSU Y Channel Vertical Step Register */
    __IO uint32_t VSU_Y_HPHASE_REG;                   /*!< Offset 0x090 VSU Y Channel Horizontal Initial Phase Register */
         uint32_t reserved_0x094;
    __IO uint32_t VSU_Y_VPHASE0_REG;                  /*!< Offset 0x098 VSU Y Channel Vertical Initial Phase 0 Register */
    __IO uint32_t VSU_Y_VPHASE1_REG;                  /*!< Offset 0x09C VSU Y Channel Vertical Initial Phase 1 Register */
         uint32_t reserved_0x0A0 [0x0008];
    __IO uint32_t VSU_C_SIZE_REG;                     /*!< Offset 0x0C0 VSU C Channel Size Register */
         uint32_t reserved_0x0C4;
    __IO uint32_t VSU_C_HSTEP_REG;                    /*!< Offset 0x0C8 VSU C Channel Horizontal Step Register */
    __IO uint32_t VSU_C_VSTEP_REG;                    /*!< Offset 0x0CC VSU C Channel Vertical Step Register */
    __IO uint32_t VSU_C_HPHASE_REG;                   /*!< Offset 0x0D0 VSU C Channel Horizontal Initial Phase Register */
         uint32_t reserved_0x0D4;
    __IO uint32_t VSU_C_VPHASE0_REG;                  /*!< Offset 0x0D8 VSU C Channel Vertical Initial Phase 0 Register */
    __IO uint32_t VSU_C_VPHASE1_REG;                  /*!< Offset 0x0DC VSU C Channel Vertical Initial Phase 1 Register */
         uint32_t reserved_0x0E0 [0x0048];
    __IO uint32_t VSU_Y_HCOEF0_REGN [0x020];          /*!< Offset 0x200 0x200+N*4 VSU Y Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         uint32_t reserved_0x280 [0x0020];
    __IO uint32_t VSU_Y_HCOEF1_REGN [0x020];          /*!< Offset 0x300 0x300+N*4 VSU Y Channel Horizontal Filter Coefficient1 Register N N = M 1 */
         uint32_t reserved_0x380 [0x0020];
    __IO uint32_t VSU_Y_VCOEF_REGN [0x020];           /*!< Offset 0x400 0x400+N*4 VSU Y Channel Vertical Filter Coefficient Register N N = M 1)) */
         uint32_t reserved_0x480 [0x0060];
    __IO uint32_t VSU_C_HCOEF0_REGN [0x020];          /*!< Offset 0x600 0x600+N*4 VSU C Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         uint32_t reserved_0x680 [0x0020];
    __IO uint32_t VSU_C_HCOEF1_REGN [0x020];          /*!< Offset 0x700 0x700+N*4 VSU C Channel Horizontal Filter Co efficient1 Register N N = M 1)) */
         uint32_t reserved_0x780 [0x0020];
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
         uint32_t reserved_0x038 [0x0002];
    __IO uint32_t WB_ADDR_SWITCH_REG;                 /*!< Offset 0x040 Write-back address switch setting register */
    __IO uint32_t WB_FORMAT_REG;                      /*!< Offset 0x044 Output format register */
    __IO uint32_t WB_INT_REG;                         /*!< Offset 0x048 Interrupt control register */
    __IO uint32_t WB_STATUS_REG;                      /*!< Offset 0x04C Module status register */
         uint32_t reserved_0x050;
    __IO uint32_t WB_BYPASS_REG;                      /*!< Offset 0x054 Bypass control register */
         uint32_t reserved_0x058 [0x0006];
    __IO uint32_t WB_CS_HORZ_REG;                     /*!< Offset 0x070 Coarse scaling horizontal setting register */
    __IO uint32_t WB_CS_VERT_REG;                     /*!< Offset 0x074 Coarse scaling vertical setting register */
         uint32_t reserved_0x078 [0x0002];
    __IO uint32_t WB_FS_INSIZE_REG;                   /*!< Offset 0x080 Fine scaling input size register */
    __IO uint32_t WB_FS_OUTSIZE_REG;                  /*!< Offset 0x084 Fine scaling output size register */
    __IO uint32_t WB_FS_HSTEP_REG;                    /*!< Offset 0x088 Fine scaling horizontal step registe */
    __IO uint32_t WB_FS_VSTEP_REG;                    /*!< Offset 0x08C Fine scaling vertical step register */
         uint32_t reserved_0x090 [0x001B];
    __IO uint32_t WB_DEBUG_REG;                       /*!< Offset 0x0FC Debug register */
         uint32_t reserved_0x100 [0x0040];
    __IO uint32_t WB_CH0_HCOEF_REGN [0x010];          /*!< Offset 0x200 0x200 + N*4 Channel 0 horizontal coefficient register N ( N = 0,1,2,...,15) */
         uint32_t reserved_0x240 [0x0010];
    __IO uint32_t WB_CH1_HCOEF_REGN [0x010];          /*!< Offset 0x280 0x280 + N*4 Channel 1/2 horizontal coefficient register N ( N = 0,1,2,...,15) */
} DE_WB_TypeDef; /* size of structure = 0x2C0 */
/*
 * @brief GICV
 */
/*!< GICV  */
typedef struct GICV_Type
{
    __IO uint32_t GICH_HCR;                           /*!< Offset 0x000 RW 0x00000000 Hypervisor Control Register */
    __IO uint32_t GICH_VTR;                           /*!< Offset 0x004 RO 0x90000003 VGIC Type Register, GICH_VTR on page 3-13 */
    __IO uint32_t GICH_VMCR;                          /*!< Offset 0x008 RW 0x004C0000 Virtual Machine Control Register */
         uint32_t reserved_0x00C;
    __IO uint32_t GICH_MISR;                          /*!< Offset 0x010 RO 0x00000000 Maintenance Interrupt Status Register */
         uint32_t reserved_0x014 [0x0003];
    __IO uint32_t GICH_EISR0;                         /*!< Offset 0x020 RO 0x00000000 End of Interrupt Status Register */
         uint32_t reserved_0x024 [0x0003];
    __IO uint32_t GICH_ELSR0;                         /*!< Offset 0x030 RO 0x0000000F Empty List register Status Register */
         uint32_t reserved_0x034 [0x002F];
    __IO uint32_t GICH_APR0;                          /*!< Offset 0x0F0 RW 0x00000000 Active Priority Register */
         uint32_t reserved_0x0F4 [0x0003];
    __IO uint32_t GICH_LR0;                           /*!< Offset 0x100 RW 0x00000000 List Register 0 */
    __IO uint32_t GICH_LR1;                           /*!< Offset 0x104 RW 0x00000000 List Register 1 */
    __IO uint32_t GICH_LR2;                           /*!< Offset 0x108 RW 0x00000000 List Register 2 */
    __IO uint32_t GICH_LR3;                           /*!< Offset 0x10C RW 0x00000000 List Register 3 */
         uint32_t reserved_0x110 [0x003C];
} GICV_TypeDef; /* size of structure = 0x200 */
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
         uint32_t reserved_0x120 [0x0038];
    struct
    {
        __IO uint32_t EINT_CFG [0x004];               /*!< Offset 0x200 External Interrupt Configure Registers */
        __IO uint32_t EINT_CTL;                       /*!< Offset 0x210 External Interrupt Control Register */
        __IO uint32_t EINT_STATUS;                    /*!< Offset 0x214 External Interrupt Status Register */
        __IO uint32_t EINT_DEB;                       /*!< Offset 0x218 External Interrupt Debounce Register */
             uint32_t reserved_0x01C;
    } GPIO_INTS [0x002];                              /*!< Offset 0x200 GPIOA & GPIOG interrupt control */
} GPIOBLOCK_TypeDef; /* size of structure = 0x240 */
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
         uint32_t reserved_0x01C;
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief HDMI_PHY
 */
/*!< HDMI_PHY  */
typedef struct HDMI_PHY_Type
{
    __IO uint32_t HDMI_PHY_POL;                       /*!< Offset 0x000 tbd */
         uint32_t reserved_0x004 [0x0003];
    __IO uint32_t HDMI_PHY_READ_EN;                   /*!< Offset 0x010 tbd */
    __IO uint32_t HDMI_PHY_UNSCRAMBLE;                /*!< Offset 0x014 tbd */
         uint32_t reserved_0x018 [0x0002];
    __IO uint32_t HDMI_PHY_CFG1;                      /*!< Offset 0x020 tbd */
    __IO uint32_t HDMI_PHY_CFG2;                      /*!< Offset 0x024 tbd */
    __IO uint32_t HDMI_PHY_CFG3;                      /*!< Offset 0x028 tbd */
    __IO uint32_t HDMI_PHY_PLL1;                      /*!< Offset 0x02C tbd */
    __IO uint32_t HDMI_PHY_PLL2;                      /*!< Offset 0x030 tbd */
    __IO uint32_t HDMI_PHY_PLL3;                      /*!< Offset 0x034 tbd */
    __IO uint32_t HDMI_PHY_STS;                       /*!< Offset 0x038 tbd */
    __IO uint32_t HDMI_PHY_CEC;                       /*!< Offset 0x03C tbd */
         uint32_t reserved_0x040 [0x03EE];
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
         uint32_t reserved_0x008 [0x003E];
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
         uint8_t reserved_0x10A [0x0076];
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
         uint8_t reserved_0x18A [0x0075];
    __IO uint8_t  HDMI_IH_MUTE;                       /*!< Offset 0x1FF  */
    __IO uint8_t  HDMI_TX_INVID0;                     /*!< Offset 0x200  */
    __IO uint8_t  HDMI_TX_INSTUFFING;                 /*!< Offset 0x201  */
    __IO uint8_t  HDMI_TX_GYDATA0;                    /*!< Offset 0x202  */
    __IO uint8_t  HDMI_TX_GYDATA1;                    /*!< Offset 0x203  */
    __IO uint8_t  HDMI_TX_RCRDATA0;                   /*!< Offset 0x204  */
    __IO uint8_t  HDMI_TX_RCRDATA1;                   /*!< Offset 0x205  */
    __IO uint8_t  HDMI_TX_BCBDATA0;                   /*!< Offset 0x206  */
    __IO uint8_t  HDMI_TX_BCBDATA1;                   /*!< Offset 0x207  */
         uint32_t reserved_0x208 [0x017E];
    __IO uint8_t  HDMI_VP_STATUS;                     /*!< Offset 0x800  */
    __IO uint8_t  HDMI_VP_PR_CD;                      /*!< Offset 0x801  */
    __IO uint8_t  HDMI_VP_STUFF;                      /*!< Offset 0x802  */
    __IO uint8_t  HDMI_VP_REMAP;                      /*!< Offset 0x803  */
    __IO uint8_t  HDMI_VP_CONF;                       /*!< Offset 0x804  */
    __IO uint8_t  HDMI_VP_STAT;                       /*!< Offset 0x805  */
    __IO uint8_t  HDMI_VP_INT;                        /*!< Offset 0x806  */
    __IO uint8_t  HDMI_VP_MASK;                       /*!< Offset 0x807  */
    __IO uint8_t  HDMI_VP_POL;                        /*!< Offset 0x808  */
         uint8_t reserved_0x809 [0x07F7];
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
         uint8_t reserved_0x102B [0x0005];
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
         uint8_t reserved_0x1065 [0x000B];
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
         uint32_t reserved_0x10C0 [0x0004];
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
         uint32_t reserved_0x10DC;
    __IO uint8_t  HDMI_FC_PRCONF;                     /*!< Offset 0x10E0  */
         uint8_t reserved_0x10E1 [0x001F];
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
         uint8_t reserved_0x1121 [0x00DF];
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
         uint32_t reserved_0x121C [0x0779];
    __IO uint8_t  HDMI_PHY_CONF0;                     /*!< Offset 0x3000  */
    __IO uint8_t  HDMI_PHY_TST0;                      /*!< Offset 0x3001  */
    __IO uint8_t  HDMI_PHY_TST1;                      /*!< Offset 0x3002  */
    __IO uint8_t  HDMI_PHY_TST2;                      /*!< Offset 0x3003  */
    __IO uint8_t  HDMI_PHY_STAT0;                     /*!< Offset 0x3004  */
    __IO uint8_t  HDMI_PHY_INT0;                      /*!< Offset 0x3005  */
    __IO uint8_t  HDMI_PHY_MASK0;                     /*!< Offset 0x3006  */
    __IO uint8_t  HDMI_PHY_POL0;                      /*!< Offset 0x3007  */
         uint32_t reserved_0x3008 [0x0006];
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
         uint8_t reserved_0x3033 [0x00CD];
    __IO uint8_t  HDMI_AUD_CONF0;                     /*!< Offset 0x3100  */
    __IO uint8_t  HDMI_AUD_CONF1;                     /*!< Offset 0x3101  */
    __IO uint8_t  HDMI_AUD_INT;                       /*!< Offset 0x3102  */
    __IO uint8_t  HDMI_AUD_CONF2;                     /*!< Offset 0x3103  */
         uint32_t reserved_0x3104 [0x003F];
    __IO uint8_t  HDMI_AUD_N1;                        /*!< Offset 0x3200  */
    __IO uint8_t  HDMI_AUD_N2;                        /*!< Offset 0x3201  */
    __IO uint8_t  HDMI_AUD_N3;                        /*!< Offset 0x3202  */
    __IO uint8_t  HDMI_AUD_CTS1;                      /*!< Offset 0x3203  */
    __IO uint8_t  HDMI_AUD_CTS2;                      /*!< Offset 0x3204  */
    __IO uint8_t  HDMI_AUD_CTS3;                      /*!< Offset 0x3205  */
    __IO uint8_t  HDMI_AUD_INPUTCLKFS;                /*!< Offset 0x3206  */
         uint8_t reserved_0x3207 [0x00FB];
    __IO uint8_t  HDMI_AUD_SPDIFINT;                  /*!< Offset 0x3302  */
         uint8_t reserved_0x3303 [0x00FD];
    __IO uint8_t  HDMI_AUD_CONF0_HBR;                 /*!< Offset 0x3400  */
    __IO uint8_t  HDMI_AUD_HBR_STATUS;                /*!< Offset 0x3401  */
    __IO uint8_t  HDMI_AUD_HBR_INT;                   /*!< Offset 0x3402  */
    __IO uint8_t  HDMI_AUD_HBR_POL;                   /*!< Offset 0x3403  */
    __IO uint8_t  HDMI_AUD_HBR_MASK;                  /*!< Offset 0x3404  */
         uint8_t reserved_0x3405 [0x00FB];
    __IO uint8_t  HDMI_GP_CONF0;                      /*!< Offset 0x3500  */
    __IO uint8_t  HDMI_GP_CONF1;                      /*!< Offset 0x3501  */
    __IO uint8_t  HDMI_GP_CONF2;                      /*!< Offset 0x3502  */
    __IO uint8_t  HDMI_GP_STAT;                       /*!< Offset 0x3503  */
    __IO uint8_t  HDMI_GP_INT;                        /*!< Offset 0x3504  */
    __IO uint8_t  HDMI_GP_MASK;                       /*!< Offset 0x3505  */
    __IO uint8_t  HDMI_GP_POL;                        /*!< Offset 0x3506  */
         uint8_t reserved_0x3507 [0x00F9];
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
         uint8_t reserved_0x361B [0x09E5];
    __IO uint8_t  HDMI_MC_SFRDIV;                     /*!< Offset 0x4000  */
    __IO uint8_t  HDMI_MC_CLKDIS;                     /*!< Offset 0x4001  */
    __IO uint8_t  HDMI_MC_SWRSTZ;                     /*!< Offset 0x4002  */
    __IO uint8_t  HDMI_MC_OPCTRL;                     /*!< Offset 0x4003  */
    __IO uint8_t  HDMI_MC_FLOWCTRL;                   /*!< Offset 0x4004  */
    __IO uint8_t  HDMI_MC_PHYRSTZ;                    /*!< Offset 0x4005  */
    __IO uint8_t  HDMI_MC_LOCKONCLOCK;                /*!< Offset 0x4006  */
    __IO uint8_t  HDMI_MC_HEACPHY_RST;                /*!< Offset 0x4007  */
         uint32_t reserved_0x4008 [0x003E];
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
         uint8_t reserved_0x411A [0x0EE6];
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
         uint8_t reserved_0x501B [0x0005];
    __IO uint8_t  HDMI_A_SRM_BASE;                    /*!< Offset 0x5020  */
         uint8_t reserved_0x5021 [0x2CDF];
    __IO uint8_t  HDMI_CEC_CTRL;                      /*!< Offset 0x7D00  */
    __IO uint8_t  HDMI_CEC_STAT;                      /*!< Offset 0x7D01  */
    __IO uint8_t  HDMI_CEC_MASK;                      /*!< Offset 0x7D02  */
    __IO uint8_t  HDMI_CEC_POLARITY;                  /*!< Offset 0x7D03  */
    __IO uint8_t  HDMI_CEC_INT;                       /*!< Offset 0x7D04  */
    __IO uint8_t  HDMI_CEC_ADDR_L;                    /*!< Offset 0x7D05  */
    __IO uint8_t  HDMI_CEC_ADDR_H;                    /*!< Offset 0x7D06  */
    __IO uint8_t  HDMI_CEC_TX_CNT;                    /*!< Offset 0x7D07  */
    __IO uint8_t  HDMI_CEC_RX_CNT;                    /*!< Offset 0x7D08  */
         uint8_t reserved_0x7D09 [0x0007];
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
         uint8_t reserved_0x7D32 [0x00CE];
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
 * @brief RTC
 */
/*!< RTC  */
typedef struct RTC_Type
{
    __IO uint32_t LOSC_CTRL_REG;                      /*!< Offset 0x000 Low Oscillator Control Register */
    __IO uint32_t LOSC_AUTO_SWT_STA_REG;              /*!< Offset 0x004 LOSC Auto Switch Status Register */
    __IO uint32_t INTOSC_CLK_PRESCAL_REG;             /*!< Offset 0x008 Internal OSC Clock Prescalar Register */
         uint32_t reserved_0x00C;
    __IO uint32_t RTC_YY_MM_DD_REG;                   /*!< Offset 0x010 RTC Year-Month-Day Register */
    __IO uint32_t RTC_HH_MM_SS_REG;                   /*!< Offset 0x014 RTC Hour-Minute-Second Register */
         uint32_t reserved_0x018 [0x0002];
    __IO uint32_t ALARM0_COUNTER_REG;                 /*!< Offset 0x020 Alarm 0 Counter Register  */
    __IO uint32_t ALARM0_CUR_VLU_REG;                 /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    __IO uint32_t ALARM0_ENABLE_REG;                  /*!< Offset 0x028 Alarm 0 Enable Register */
    __IO uint32_t ALARM0_IRQ_EN;                      /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    __IO uint32_t ALARM0_IRQ_STA_REG;                 /*!< Offset 0x030 Alarm 0 IRQ Status Register */
         uint32_t reserved_0x034 [0x0003];
    __IO uint32_t ALARM1_WK_HH_MM_SS;                 /*!< Offset 0x040 Alarm 1 Week HMS Register  */
    __IO uint32_t ALARM1_ENABLE_REG;                  /*!< Offset 0x044 Alarm 1 Enable Register */
    __IO uint32_t ALARM1_IRQ_EN;                      /*!< Offset 0x048 Alarm 1 IRQ Enable Register */
    __IO uint32_t ALARM1_IRQ_STA_REG;                 /*!< Offset 0x04C (null) */
    __IO uint32_t ALARM_CONFIG_REG;                   /*!< Offset 0x050 Alarm Config Register */
         uint32_t reserved_0x054 [0x0003];
    __IO uint32_t LOSC_OUT_GATING_REG;                /*!< Offset 0x060 LOSC output gating register */
         uint32_t reserved_0x064 [0x0027];
    __IO uint32_t GP_DATA_REG [0x008];                /*!< Offset 0x100 General Purpose Register (N=0~3) */
         uint32_t reserved_0x120 [0x0018];
    __IO uint32_t GPL_HOLD_OUTPUT_REG;                /*!< Offset 0x180 GPL Hold Output Register */
         uint32_t reserved_0x184 [0x0003];
    __IO uint32_t VDD_RTC_REG;                        /*!< Offset 0x190 VDD RTC Regulate Register */
         uint32_t reserved_0x194 [0x0017];
    __IO uint32_t IC_CHARA_REG;                       /*!< Offset 0x1F0 IC Characteristic Register */
} RTC_TypeDef; /* size of structure = 0x1F4 */
/*
 * @brief R_PRCM
 */
/*!< R_PRCM  */
typedef struct R_PRCM_Type
{
    __IO uint32_t CPUS_CLK_REG;                       /*!< Offset 0x000 CPUS Clock Register */
         uint32_t reserved_0x004 [0x0002];
    __IO uint32_t APB0_CFG_REG;                       /*!< Offset 0x00C APB0 Configuration Register */
         uint32_t reserved_0x010 [0x0006];
    __IO uint32_t APB0_CLK_GATING_REG;                /*!< Offset 0x028 APB0 Clock Gating Register */
         uint32_t reserved_0x02C [0x0006];
    __IO uint32_t PLL_CTRL_REG1;                      /*!< Offset 0x044 PLL Control Register 1 */
         uint32_t reserved_0x048 [0x0003];
    __IO uint32_t R_CIR_RX_CLK_REG;                   /*!< Offset 0x054 R_CIR_RX Clock Register */
         uint32_t reserved_0x058 [0x0016];
    __IO uint32_t APB0_SOFT_RST_REG;                  /*!< Offset 0x0B0 APB0 Software Reset Register */
         uint32_t reserved_0x0B4 [0x0013];
    __IO uint32_t C0CPUX_PWROFF_GATING_REG;           /*!< Offset 0x100 Cluster0 CPUX Power Off Gating Register */
    __IO uint32_t C1CPUX_PWROFF_GATING_REG;           /*!< Offset 0x104 Cluster1 CPUX Power Off Gating Register */
         uint32_t reserved_0x108 [0x0002];
    __IO uint32_t VDD_SYS_PWROFF_GATING_REG;          /*!< Offset 0x110 VDD_SYS Power Off Gating Register */
         uint32_t reserved_0x114;
    __IO uint32_t GPU_PWROFF_GATING_REG;              /*!< Offset 0x118 GPU Power Off Gating Register */
         uint32_t reserved_0x11C;
    __IO uint32_t VDD_SYS_PWROFF_RST_REG;             /*!< Offset 0x120 VDD_SYS Power Domain Reset Register */
         uint32_t reserved_0x124 [0x0007];
    __IO uint32_t C0_CPU0_PWR_SWITCH_CTRL;            /*!< Offset 0x140 C0_CPU0 Power Switch Control Register */
    __IO uint32_t C0_CPU1_PWR_SWITCH_CTRL;            /*!< Offset 0x144 C0_CPU1 Power Switch Control Register */
    __IO uint32_t C0_CPU2_PWR_SWITCH_CTRL;            /*!< Offset 0x148 C0_CPU2 Power Switch Control Register */
    __IO uint32_t C0_CPU3_PWR_SWITCH_CTRL;            /*!< Offset 0x14C C0_CPU3 Power Switch Control Register */
    __IO uint32_t C1_CPU0_PWR_SWITCH_CTRL;            /*!< Offset 0x150 C1_CPU0 Power Switch Control Register */
    __IO uint32_t C1_CPU1_PWR_SWITCH_CTRL;            /*!< Offset 0x154 C1_CPU1 Power Switch Control Register */
    __IO uint32_t C1_CPU2_PWR_SWITCH_CTRL;            /*!< Offset 0x158 C1_CPU2 Power Switch Control Register */
    __IO uint32_t C1_CPU3_PWR_SWITCH_CTRL;            /*!< Offset 0x15C C1_CPU3 Power Switch Control Register */
         uint32_t reserved_0x160 [0x0024];
    __IO uint32_t RPIO_HOLD_CTRL_REG;                 /*!< Offset 0x1F0 R_PIO Hold Control Register */
    __IO uint32_t OSC24M_CTRL_REG;                    /*!< Offset 0x1F4 OSC24M Control Register */
} R_PRCM_TypeDef; /* size of structure = 0x1F8 */
/*
 * @brief SYSCTRL
 */
/*!< SYSCTRL  */
typedef struct SYSCTRL_Type
{
         uint32_t reserved_0x000 [0x0009];
    __I  uint32_t VER_REG;                            /*!< Offset 0x024 Version Register */
         uint32_t reserved_0x028 [0x0002];
    __IO uint32_t EMAC_EPHY_CLK_REG;                  /*!< Offset 0x030 EMAC-EPHY Clock Register */
} SYSCTRL_TypeDef; /* size of structure = 0x034 */
/*
 * @brief TCON
 */
/*!< TCON TCON0, TCON1 */
typedef struct TCON_Type
{
    __IO uint32_t TCON_GCTL_REG;                      /*!< Offset 0x000 TCON global control register */
    __IO uint32_t TCON_GINT0_REG;                     /*!< Offset 0x004 TCON global interrupt register0 */
    __IO uint32_t TCON_GINT1_REG;                     /*!< Offset 0x008 TCON global interrupt register1 */
         uint32_t reserved_0x00C [0x0021];
    __IO uint32_t TCON1_CTL_REG;                      /*!< Offset 0x090 TCON1 control register */
    __IO uint32_t TCON1_BASIC0_REG;                   /*!< Offset 0x094 TCON1 basic timing register0 */
    __IO uint32_t TCON1_BASIC1_REG;                   /*!< Offset 0x098 TCON1 basic timing register1 */
    __IO uint32_t TCON1_BASIC2_REG;                   /*!< Offset 0x09C TCON1 basic timing register2 */
    __IO uint32_t TCON1_BASIC3_REG;                   /*!< Offset 0x0A0 TCON1 basic timing register3 */
    __IO uint32_t TCON1_BASIC4_REG;                   /*!< Offset 0x0A4 TCON1 basic timing register4 */
    __IO uint32_t TCON1_BASIC5_REG;                   /*!< Offset 0x0A8 TCON1 basic timing register5 */
         uint32_t reserved_0x0AC;
    __IO uint32_t TCON1_PS_SYNC_REG;                  /*!< Offset 0x0B0 TCON1 sync register */
         uint32_t reserved_0x0B4 [0x000F];
    __IO uint32_t TCON1_IO_POL_REG;                   /*!< Offset 0x0F0 TCON1 IO polarity register */
    __IO uint32_t TCON1_IO_TRI_REG;                   /*!< Offset 0x0F4 TCON1 IO control register */
    __IO uint32_t TCON_ECC_FIFO_REG;                  /*!< Offset 0x0F8 TCON ECC FIFO register */
         uint32_t reserved_0x0FC;
    __IO uint32_t TCON_CEU_CTL_REG;                   /*!< Offset 0x100 TCON CEU control register */
         uint32_t reserved_0x104 [0x0003];
    __IO uint32_t TCON_CEU_COEF_MUL_REG [0x00B];      /*!< Offset 0x110 TCON CEU coefficient register0 (N=0,1,2,4,5,6,8,9,10) */
         uint32_t reserved_0x13C;
    __IO uint32_t TCON_CEU_COEF_RANG_REG [0x003];     /*!< Offset 0x140 TCON CEU coefficient register2 (N=0,1,2) */
         uint32_t reserved_0x14C [0x0029];
    __IO uint32_t TCON_SAFE_PERIOD_REG;               /*!< Offset 0x1F0 TCON safe period register */
         uint32_t reserved_0x1F4 [0x0043];
    __IO uint32_t TCON1_FILL_CTL_REG;                 /*!< Offset 0x300 TCON1 fill data control register */
    struct
    {
        __IO uint32_t TCON1_FILL_BEGIN_REG;           /*!< Offset 0x304 TCON1 fill data begin register (N=0,1,2) */
        __IO uint32_t TCON1_FILL_END_REG;             /*!< Offset 0x308 TCON1 fill data end register (N=0,1,2) */
        __IO uint32_t TCON1_FILL_DATA0_REG;           /*!< Offset 0x30C TCON1 fill data value register (N=0,1,2) */
    } TCON1_FILL [0x003];                             /*!< Offset 0x304 TCON1 fill data (N=0,1,2) */
         uint32_t reserved_0x328 [0x0036];
    __IO uint32_t TCON1_GAMMA_TABLE_REG [0x100];      /*!< Offset 0x400 Gamma Table 0x400-0x7FF */
         uint32_t reserved_0x800 [0x01FF];
    __IO uint32_t TCON_ECC_FIFO_BIST_REG;             /*!< Offset 0xFFC tbd */
} TCON_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    __IO uint32_t TMR_IRQ_EN_REG;                     /*!< Offset 0x000 Timer IRQ Enable Register */
    __IO uint32_t TMR_IRQ_STA_REG;                    /*!< Offset 0x004 Timer Status Register */
         uint32_t reserved_0x008 [0x0002];
    struct
    {
        __IO uint32_t CTRL_REG;                       /*!< Offset 0x010 Timer n Control Register */
        __IO uint32_t INTV_VALUE_REG;                 /*!< Offset 0x014 Timer n Interval Value Register */
        __IO uint32_t CUR_VALUE_REG;                  /*!< Offset 0x018 Timer n Current Value Register */
             uint32_t reserved_0x00C;
    } TMR [0x002];                                    /*!< Offset 0x010 Timer */
         uint32_t reserved_0x030 [0x0014];
    __IO uint32_t AVS_CNT_CTL_REG;                    /*!< Offset 0x080 AVS Control Register */
    __IO uint32_t AVS_CNT0_REG;                       /*!< Offset 0x084 AVS Counter 0 Register */
    __IO uint32_t AVS_CNT1_REG;                       /*!< Offset 0x088 AVS Counter 1 Register */
         uint32_t reserved_0x08C [0x0005];
    __IO uint32_t WDOG_IRQ_EN_REG;                    /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
    __IO uint32_t WDOG_IRQ_STA_REG;                   /*!< Offset 0x0A4 Watchdog Status Register */
         uint32_t reserved_0x0A8 [0x0002];
    __IO uint32_t WDOG_CTRL_REG;                      /*!< Offset 0x0B0 Watchdog Control Register */
    __IO uint32_t WDOG_CFG_REG;                       /*!< Offset 0x0B4 Watchdog Configuration Register */
    __IO uint32_t WDOG_MODE_REG;                      /*!< Offset 0x0B8 Watchdog Mode Register */
         uint32_t reserved_0x0BC [0x00D1];
} TIMER_TypeDef; /* size of structure = 0x400 */
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
         uint32_t reserved_0x020 [0x0017];
    __IO uint32_t UART_USR;                           /*!< Offset 0x07C UART Status Register */
    __IO uint32_t UART_TFL;                           /*!< Offset 0x080 UART Transmit FIFO Level Register */
    __IO uint32_t UART_RFL;                           /*!< Offset 0x084 UART Receive FIFO Level Register */
         uint32_t reserved_0x088;
    __IO uint32_t UART_DMA_REQ_EN;                    /*!< Offset 0x08C UART DMA Request Enable Register */
         uint32_t reserved_0x090 [0x0005];
    __IO uint32_t UART_HALT;                          /*!< Offset 0x0A4 UART Halt TX Register */
         uint32_t reserved_0x0A8 [0x00D6];
} UART_TypeDef; /* size of structure = 0x400 */
/*
 * @brief USBOTG
 */
/*!< USBOTG USB OTG Dual-Role Device controller */
typedef struct USBOTG_Type
{
    __IO uint32_t USB_EPFIFO [0x006];                 /*!< Offset 0x000 USB_EPFIFO [0..5] USB FIFO Entry for Endpoint N */
         uint32_t reserved_0x018 [0x000A];
    __IO uint32_t USB_GCS;                            /*!< Offset 0x040 USB_POWER, USB_DEVCTL, USB_EPINDEX, USB_DMACTL USB Global Control and Status Register */
    __IO uint16_t USB_INTTX;                          /*!< Offset 0x044 USB_INTTX USB_EPINTF USB Endpoint Interrupt Flag Register */
    __IO uint16_t USB_INTRX;                          /*!< Offset 0x046 USB_INTRX USB_EPINTF */
    __IO uint16_t USB_INTTXE;                         /*!< Offset 0x048 USB_INTTXE USB_EPINTE USB Endpoint Interrupt Enable Register */
    __IO uint16_t USB_INTRXE;                         /*!< Offset 0x04A USB_INTRXE USB_EPINTE */
    __IO uint32_t USB_INTUSB;                         /*!< Offset 0x04C USB_INTUSB USB_BUSINTF USB Bus Interrupt Flag Register */
    __IO uint32_t USB_INTUSBE;                        /*!< Offset 0x050 USB_INTUSBE USB_BUSINTE USB Bus Interrupt Enable Register */
    __IO uint32_t USB_FNUM;                           /*!< Offset 0x054 USB Frame Number Register */
         uint32_t reserved_0x058 [0x0009];
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
             uint8_t reserved_0x005 [0x0001];
        __IO uint8_t  USB_RXHADDR;                    /*!< Offset 0x09E USB_RXHADDR */
        __IO uint8_t  USB_RXHUBPORT;                  /*!< Offset 0x09F USB_RXHUBPORT */
    } FIFO [0x010];                                   /*!< Offset 0x098 FIFOs [0..5] */
         uint32_t reserved_0x118 [0x00BA];
    __IO uint32_t USB_ISCR;                           /*!< Offset 0x400 HCI Interface Register (HCI_Interface) */
    __IO uint32_t USBPHY_PHYCTL;                      /*!< Offset 0x404 USBPHY_PHYCTL */
    __IO uint32_t HCI_CTRL3;                          /*!< Offset 0x408 HCI Control 3 Register (bist) */
         uint32_t reserved_0x40C;
    __IO uint32_t PHY_CTRL;                           /*!< Offset 0x410 PHY Control Register (PHY_Control) */
         uint32_t reserved_0x414 [0x0003];
    __IO uint32_t PHY_OTGCTL;                         /*!< Offset 0x420 Control PHY routing to EHCI or OTG */
    __IO uint32_t PHY_STATUS;                         /*!< Offset 0x424 PHY Status Register */
    __IO uint32_t USB_SPDCR;                          /*!< Offset 0x428 HCI SIE Port Disable Control Register */
         uint32_t reserved_0x42C [0x0035];
    __IO uint32_t USB_DMA_INTE;                       /*!< Offset 0x500 USB DMA Interrupt Enable Register */
    __IO uint32_t USB_DMA_INTS;                       /*!< Offset 0x504 USB DMA Interrupt Status Register */
         uint32_t reserved_0x508 [0x000E];
    struct
    {
        __IO uint32_t CHAN_CFG;                       /*!< Offset 0x540 USB DMA Channel Configuration Register */
        __IO uint32_t SDRAM_ADD;                      /*!< Offset 0x544 USB DMA Channel Configuration Register */
        __IO uint32_t BC;                             /*!< Offset 0x548 USB DMA Byte Counter Register/USB DMA RESIDUAL Byte Counter Register */
             uint32_t reserved_0x00C;
    } USB_DMA [0x008];                                /*!< Offset 0x540  */
} USBOTG_TypeDef; /* size of structure = 0x5C0 */
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
#define SYSCTRL ((SYSCTRL_TypeDef *) SYSCTRL_BASE)    /*!< SYSCTRL  register set access pointer */
#define TCON0 ((TCON_TypeDef *) TCON0_BASE)           /*!< TCON0 TCON0, TCON1 register set access pointer */
#define TCON1 ((TCON_TypeDef *) TCON1_BASE)           /*!< TCON1 TCON0, TCON1 register set access pointer */
#define USB20_OTG_DEVICE ((USBOTG_TypeDef *) USB20_OTG_DEVICE_BASE)/*!< USB20_OTG_DEVICE USB OTG Dual-Role Device controller register set access pointer */
#define USB20_HOST0_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST0_EHCI_BASE)/*!< USB20_HOST0_EHCI  register set access pointer */
#define USB20_HOST0_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST0_OHCI_BASE)/*!< USB20_HOST0_OHCI  register set access pointer */
#define USB20_HOST1_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST1_EHCI_BASE)/*!< USB20_HOST1_EHCI  register set access pointer */
#define USB20_HOST1_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST1_OHCI_BASE)/*!< USB20_HOST1_OHCI  register set access pointer */
#define USB20_HOST2_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST2_EHCI_BASE)/*!< USB20_HOST2_EHCI  register set access pointer */
#define USB20_HOST2_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST2_OHCI_BASE)/*!< USB20_HOST2_OHCI  register set access pointer */
#define USB20_HOST3_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST3_EHCI_BASE)/*!< USB20_HOST3_EHCI  register set access pointer */
#define USB20_HOST3_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST3_OHCI_BASE)/*!< USB20_HOST3_OHCI  register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA  register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define GPIOINTA ((GPIOINT_TypeDef *) GPIOINTA_BASE)  /*!< GPIOINTA  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define HDMI_TX0 ((HDMI_TX_TypeDef *) HDMI_TX0_BASE)  /*!< HDMI_TX0  register set access pointer */
#define HDMI_PHY ((HDMI_PHY_TypeDef *) HDMI_PHY_BASE) /*!< HDMI_PHY  register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC  register set access pointer */
#define R_PRCM ((R_PRCM_TypeDef *) R_PRCM_BASE)       /*!< R_PRCM  register set access pointer */
#define CPUCFG ((CPUCFG_TypeDef *) CPUCFG_BASE)       /*!< CPUCFG CPU Configuration register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L  register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
