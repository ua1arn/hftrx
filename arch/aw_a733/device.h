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
    UART0_IRQn = 34,                                  /*!< UART  */
    UART1_IRQn = 35,                                  /*!< UART  */
    UART2_IRQn = 36,                                  /*!< UART  */
    UART3_IRQn = 37,                                  /*!< UART  */
    UART4_IRQn = 38,                                  /*!< UART  */
    UART5_IRQn = 39,                                  /*!< UART  */
    UART6_IRQn = 40,                                  /*!< UART  */
    USB20_OTG_DEVICE_IRQn = 57,                       /*!< USBOTG USB OTG Dual-Role Device controller */
    USB20_HOST0_EHCI_IRQn = 58,                       /*!< USB_EHCI_Capability  */
    USB20_HOST0_OHCI_IRQn = 59,                       /*!< USB_OHCI_Capability  */
    USB20_HOST1_EHCI_IRQn = 60,                       /*!< USB_EHCI_Capability  */
    USB20_HOST1_OHCI_IRQn = 61,                       /*!< USB_OHCI_Capability  */
    USB20_HOST2_EHCI_IRQn = 62,                       /*!< USB_EHCI_Capability  */
    USB20_HOST2_OHCI_IRQn = 63,                       /*!< USB_OHCI_Capability  */
    USB20_HOST3_EHCI_IRQn = 64,                       /*!< USB_EHCI_Capability  */
    USB20_HOST3_OHCI_IRQn = 65,                       /*!< USB_OHCI_Capability  */
    CLK_DET_IRQn = 73,                                /*!< CCU Clock Controller Unit (CCU) */
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
    S_GPIOL_S_IRQn = 229,                             /*!< GPIOINT  */
    S_GPIOL_IRQn = 230,                               /*!< GPIOINT S_GPIOL_NS */
    S_GPIOM_S_IRQn = 231,                             /*!< GPIOINT  */
    S_GPIOM_IRQn = 232,                               /*!< GPIOINT S_GPIOM_NS */
    S_UART0_IRQn = 233,                               /*!< UART  */
    S_UART1_IRQn = 234,                               /*!< UART  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GPIOINTD_BASE ((uintptr_t) 0x00020002)        /*!< GPIOINT  Base */
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
#define Timer1_CPUX_BASE ((uintptr_t) 0x02052000)     /*!< TIMER1  Base */
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
#define SPI2_BASE ((uintptr_t) 0x02542000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI3_BASE ((uintptr_t) 0x02543000)            /*!< SPI Serial Peripheral Interface Base */
#define CCU_BASE ((uintptr_t) 0x03001000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define Timer0_CPUX_BASE ((uintptr_t) 0x03009000)     /*!< TIMER0  Base */
#define GIC_BASE ((uintptr_t) 0x03020000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x03024000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x03025000)            /*!< GICV  Base */
#define USB20_OTG_DEVICE_BASE ((uintptr_t) 0x05100000)/*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB20_HOST0_EHCI_BASE ((uintptr_t) 0x05101000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST0_OHCI_BASE ((uintptr_t) 0x05101400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST1_EHCI_BASE ((uintptr_t) 0x05200000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST1_OHCI_BASE ((uintptr_t) 0x05200400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST2_EHCI_BASE ((uintptr_t) 0x05310000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST2_OHCI_BASE ((uintptr_t) 0x05310400)/*!< USB_OHCI_Capability  Base */
#define USB20_HOST3_EHCI_BASE ((uintptr_t) 0x05311000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST3_OHCI_BASE ((uintptr_t) 0x05311400)/*!< USB_OHCI_Capability  Base */
#define S_GPIOL_BASE ((uintptr_t) 0x07025000)         /*!< S_GPIO Secure Port Controller Base */
#define S_GPIOM_BASE ((uintptr_t) 0x07025030)         /*!< S_GPIO Secure Port Controller Base */
#define S_UART0_BASE ((uintptr_t) 0x07080000)         /*!< UART  Base */
#define S_UART1_BASE ((uintptr_t) 0x07081000)         /*!< UART  Base */
#define S_TWI1_BASE ((uintptr_t) 0x07084000)          /*!< TWI Two Wire Interface (TWI) Base */
#define S_TWI2_BASE ((uintptr_t) 0x07085000)          /*!< TWI Two Wire Interface (TWI) Base */
#define Timer0_CPUS_BASE ((uintptr_t) 0x07091000)     /*!< TIMER0  Base */
#define S_SPI_BASE ((uintptr_t) 0x07092000)           /*!< SPI Serial Peripheral Interface Base */

#if __aarch64__
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
    __IO uint32_t PLL_CPUX_CTRL_REG;                  /*!< Offset 0x000 PLL_CPUX Control Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t PLL_DDR0_CTRL_REG;                  /*!< Offset 0x010 PLL_DDR0 Control Register */
         RESERVED(0x014[0x0018 - 0x0014], uint8_t)
    __IO uint32_t PLL_DDR1_CTRL_REG;                  /*!< Offset 0x018 PLL_DDR1 Control Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t PLL_PERI0_CTRL_REG;                 /*!< Offset 0x020 PLL_PERI0 Control Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t PLL_PERI1_CTRL_REG;                 /*!< Offset 0x028 PLL_PERI1 Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t PLL_GPU0_CTRL_REG;                  /*!< Offset 0x030 PLL_GPU0 Control Register */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t PLL_VIDEO0_CTRL_REG;                /*!< Offset 0x040 PLL_VIDEO0 Control Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __IO uint32_t PLL_VIDEO1_CTRL_REG;                /*!< Offset 0x048 PLL_VIDEO1 Control Register */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __IO uint32_t PLL_VIDEO2_CTRL_REG;                /*!< Offset 0x050 PLL_VIDEO2 Control Register */
         RESERVED(0x054[0x0058 - 0x0054], uint8_t)
    __IO uint32_t PLL_VE_CTRL_REG;                    /*!< Offset 0x058 PLL_VE Control Register */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IO uint32_t PLL_DE_CTRL_REG;                    /*!< Offset 0x060 PLL_DE Control Register */
         RESERVED(0x064[0x0078 - 0x0064], uint8_t)
    __IO uint32_t PLL_AUDIO_CTRL_REG;                 /*!< Offset 0x078 PLL_AUDIO Control Register */
         RESERVED(0x07C[0x00E0 - 0x007C], uint8_t)
    __IO uint32_t PLL_CSI_CTRL_REG;                   /*!< Offset 0x0E0 PLL_CSI Control Register */
         RESERVED(0x0E4[0x0110 - 0x00E4], uint8_t)
    __IO uint32_t PLL_DDR0_PAT_CTRL_REG;              /*!< Offset 0x110 PLL_DDR0 Pattern Control Register */
         RESERVED(0x114[0x0118 - 0x0114], uint8_t)
    __IO uint32_t PLL_DDR1_PAT_CTRL_REG;              /*!< Offset 0x118 PLL_DDR1 Pattern Control Register */
         RESERVED(0x11C[0x0120 - 0x011C], uint8_t)
    __IO uint32_t PLL_PERI0_PAT0_CTRL_REG;            /*!< Offset 0x120 PLL_PERI0 Pattern0 Control Register */
    __IO uint32_t PLL_PERI0_PAT1_CTRL_REG;            /*!< Offset 0x124 PLL_PERI0 Pattern1 Control Register */
    __IO uint32_t PLL_PERI1_PAT0_CTRL_REG;            /*!< Offset 0x128 PLL_PERI1 Pattern0 Control Register */
    __IO uint32_t PLL_PERI1_PAT1_CTRL_REG;            /*!< Offset 0x12C PLL_PERI1 Pattern1 Control Register */
    __IO uint32_t PLL_GPU0_PAT0_CTRL_REG;             /*!< Offset 0x130 PLL_GPU0 Pattern0 Control Register */
    __IO uint32_t PLL_GPU0_PAT1_CTRL_REG;             /*!< Offset 0x134 PLL_GPU0 Pattern1 Control Register */
         RESERVED(0x138[0x0140 - 0x0138], uint8_t)
    __IO uint32_t PLL_VIDEO0_PAT0_CTRL_REG;           /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO0_PAT1_CTRL_REG;           /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
    __IO uint32_t PLL_VIDEO1_PAT0_CTRL_REG;           /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO1_PAT1_CTRL_REG;           /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
    __IO uint32_t PLL_VIDEO2_PAT0_CTRL_REG;           /*!< Offset 0x150 PLL_VIDEO2 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO2_PAT1_CTRL_REG;           /*!< Offset 0x154 PLL_VIDEO2 Pattern1 Control Register */
    __IO uint32_t PLL_VE_PAT0_CTRL_REG;               /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
    __IO uint32_t PLL_VE_PAT1_CTRL_REG;               /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
    __IO uint32_t PLL_DE_PAT0_CTRL_REG;               /*!< Offset 0x160 PLL_DE Pattern0 Control Register */
    __IO uint32_t PLL_DE_PAT1_CTRL_REG;               /*!< Offset 0x164 PLL_DE Pattern1 Control Register */
         RESERVED(0x168[0x0178 - 0x0168], uint8_t)
    __IO uint32_t PLL_AUDIO_PAT0_CTRL_REG;            /*!< Offset 0x178 PLL_AUDIO Pattern0 Control Register */
    __IO uint32_t PLL_AUDIO_PAT1_CTRL_REG;            /*!< Offset 0x17C PLL_AUDIO Pattern1 Control Register */
         RESERVED(0x180[0x01E0 - 0x0180], uint8_t)
    __IO uint32_t PLL_CSI_PAT0_CTRL_REG;              /*!< Offset 0x1E0 PLL_CSI Pattern0 Control Register */
    __IO uint32_t PLL_CSI_PAT1_CTRL_REG;              /*!< Offset 0x1E4 PLL_CSI Pattern1 Control Register */
         RESERVED(0x1E8[0x0300 - 0x01E8], uint8_t)
    __IO uint32_t PLL_CPUX_BIAS_REG;                  /*!< Offset 0x300 PLL_CPUX Bias Register */
         RESERVED(0x304[0x0310 - 0x0304], uint8_t)
    __IO uint32_t PLL_DDR0_BIAS_REG;                  /*!< Offset 0x310 PLL_DDR0 Bias Register */
         RESERVED(0x314[0x0318 - 0x0314], uint8_t)
    __IO uint32_t PLL_DDR1_BIAS_REG;                  /*!< Offset 0x318 PLL_DDR1 Bias Register */
         RESERVED(0x31C[0x0320 - 0x031C], uint8_t)
    __IO uint32_t PLL_PERI0_BIAS_REG;                 /*!< Offset 0x320 PLL_PERI0 Bias Register */
         RESERVED(0x324[0x0328 - 0x0324], uint8_t)
    __IO uint32_t PLL_PERI1_BIAS_REG;                 /*!< Offset 0x328 PLL_PERI1 Bias Register */
         RESERVED(0x32C[0x0330 - 0x032C], uint8_t)
    __IO uint32_t PLL_GPU0_BIAS_REG;                  /*!< Offset 0x330 PLL_GPU0 Bias Register */
         RESERVED(0x334[0x0340 - 0x0334], uint8_t)
    __IO uint32_t PLL_VIDEO0_BIAS_REG;                /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
         RESERVED(0x344[0x0348 - 0x0344], uint8_t)
    __IO uint32_t PLL_VIDEO1_BIAS_REG;                /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
         RESERVED(0x34C[0x0358 - 0x034C], uint8_t)
    __IO uint32_t PLL_VE_BIAS_REG;                    /*!< Offset 0x358 PLL_VE Bias Register */
         RESERVED(0x35C[0x0360 - 0x035C], uint8_t)
    __IO uint32_t PLL_DE_BIAS_REG;                    /*!< Offset 0x360 PLL_DE Bias Register */
         RESERVED(0x364[0x0378 - 0x0364], uint8_t)
    __IO uint32_t PLL_AUDIO_BIAS_REG;                 /*!< Offset 0x378 PLL_AUDIO Bias Register */
         RESERVED(0x37C[0x03E0 - 0x037C], uint8_t)
    __IO uint32_t PLL_CSI_BIAS_REG;                   /*!< Offset 0x3E0 PLL_CSI Bias Register */
         RESERVED(0x3E4[0x0400 - 0x03E4], uint8_t)
    __IO uint32_t PLL_CPUX_TUN_REG;                   /*!< Offset 0x400 PLL_CPUX Tuning Register */
         RESERVED(0x404[0x0500 - 0x0404], uint8_t)
    __IO uint32_t CPUX_AXI_CFG_REG;                   /*!< Offset 0x500 CPUX_AXI Configuration Register */
         RESERVED(0x504[0x0510 - 0x0504], uint8_t)
    __IO uint32_t PSI_AHB1_AHB2_CFG_REG;              /*!< Offset 0x510 PSI_AHB1_AHB2 Configuration Register */
         RESERVED(0x514[0x051C - 0x0514], uint8_t)
    __IO uint32_t AHB3_CFG_REG;                       /*!< Offset 0x51C AHB3 Configuration Register */
    __IO uint32_t APB1_CFG_REG;                       /*!< Offset 0x520 APB1 Configuration Register */
    __IO uint32_t APB2_CFG_REG;                       /*!< Offset 0x524 APB2 Configuration Register */
         RESERVED(0x528[0x0540 - 0x0528], uint8_t)
    __IO uint32_t MBUS_CFG_REG;                       /*!< Offset 0x540 MBUS Configuration Register */
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
         RESERVED(0x640[0x0670 - 0x0640], uint8_t)
    __IO uint32_t GPU_CLK0_REG;                       /*!< Offset 0x670 GPU Clock0 Register */
    __IO uint32_t GPU_CLK1_REG;                       /*!< Offset 0x674 GPU Clock1 Register */
         RESERVED(0x678[0x067C - 0x0678], uint8_t)
    __IO uint32_t GPU_BGR_REG;                        /*!< Offset 0x67C GPU Bus Gating Reset Register */
    __IO uint32_t CE_CLK_REG;                         /*!< Offset 0x680 CE Clock Register */
         RESERVED(0x684[0x068C - 0x0684], uint8_t)
    __IO uint32_t CE_BGR_REG;                         /*!< Offset 0x68C CE Bus Gating Reset Register */
    __IO uint32_t VE_CLK_REG;                         /*!< Offset 0x690 VE Clock Register */
         RESERVED(0x694[0x069C - 0x0694], uint8_t)
    __IO uint32_t VE_BGR_REG;                         /*!< Offset 0x69C VE Bus Gating Reset Register */
         RESERVED(0x6A0[0x070C - 0x06A0], uint8_t)
    __IO uint32_t DMA_BGR_REG;                        /*!< Offset 0x70C DMA Bus Gating Reset Register */
         RESERVED(0x710[0x073C - 0x0710], uint8_t)
    __IO uint32_t HSTIMER_BGR_REG;                    /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
    __IO uint32_t AVS_CLK_REG;                        /*!< Offset 0x740 AVS Clock Register */
         RESERVED(0x744[0x078C - 0x0744], uint8_t)
    __IO uint32_t DBGSYS_BGR_REG;                     /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
         RESERVED(0x790[0x079C - 0x0790], uint8_t)
    __IO uint32_t PSI_BGR_REG;                        /*!< Offset 0x79C PSI Bus Gating Reset Register */
         RESERVED(0x7A0[0x07AC - 0x07A0], uint8_t)
    __IO uint32_t PWM_BGR_REG;                        /*!< Offset 0x7AC PWM Bus Gating Reset Register */
         RESERVED(0x7B0[0x07BC - 0x07B0], uint8_t)
    __IO uint32_t IOMMU_BGR_REG;                      /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
         RESERVED(0x7C0[0x0800 - 0x07C0], uint8_t)
    __IO uint32_t DRAM_CLK_REG;                       /*!< Offset 0x800 DRAM Clock Register */
    __IO uint32_t MBUS_MAT_CLK_GATING_REG;            /*!< Offset 0x804 MBUS Master Clock Gating Register */
         RESERVED(0x808[0x080C - 0x0808], uint8_t)
    __IO uint32_t DRAM_BGR_REG;                       /*!< Offset 0x80C DRAM Bus Gating Reset Register */
    __IO uint32_t NAND0_0_CLK_REG;                    /*!< Offset 0x810 NAND0_0 Clock Register */
    __IO uint32_t NAND0_1_CLK_REG;                    /*!< Offset 0x814 NAND0_1 Clock Register */
         RESERVED(0x818[0x082C - 0x0818], uint8_t)
    __IO uint32_t NAND_BGR_REG;                       /*!< Offset 0x82C NAND Bus Gating Reset Register */
    __IO uint32_t SMHC0_CLK_REG;                      /*!< Offset 0x830 SMHC0 Clock Register */
    __IO uint32_t SMHC1_CLK_REG;                      /*!< Offset 0x834 SMHC1 Clock Register */
    __IO uint32_t SMHC2_CLK_REG;                      /*!< Offset 0x838 SMHC2 Clock Register */
         RESERVED(0x83C[0x084C - 0x083C], uint8_t)
    __IO uint32_t SMHC_BGR_REG;                       /*!< Offset 0x84C SMHC Bus Gating Reset Register */
         RESERVED(0x850[0x090C - 0x0850], uint8_t)
    __IO uint32_t UART_BGR_REG;                       /*!< Offset 0x90C UART Bus Gating Reset Register */
         RESERVED(0x910[0x091C - 0x0910], uint8_t)
    __IO uint32_t TWI_BGR_REG;                        /*!< Offset 0x91C TWI Bus Gating Reset Register */
         RESERVED(0x920[0x0940 - 0x0920], uint8_t)
    __IO uint32_t SPI0_CLK_REG;                       /*!< Offset 0x940 SPI0 Clock Register */
    __IO uint32_t SPI1_CLK_REG;                       /*!< Offset 0x944 SPI1 Clock Register */
         RESERVED(0x948[0x096C - 0x0948], uint8_t)
    __IO uint32_t SPI_BGR_REG;                        /*!< Offset 0x96C SPI Bus Gating Reset Register */
    __IO uint32_t EPHY_25M_CLK_REG;                   /*!< Offset 0x970 EPHY_25M Clock Register */
         RESERVED(0x974[0x097C - 0x0974], uint8_t)
    __IO uint32_t EMAC_BGR_REG;                       /*!< Offset 0x97C EMAC Bus Gating Reset Register */
         RESERVED(0x980[0x09B0 - 0x0980], uint8_t)
    __IO uint32_t TS_CLK_REG;                         /*!< Offset 0x9B0 TS Clock Register */
         RESERVED(0x9B4[0x09BC - 0x09B4], uint8_t)
    __IO uint32_t TS_BGR_REG;                         /*!< Offset 0x9BC TS Bus Gating Reset Register */
         RESERVED(0x9C0[0x09EC - 0x09C0], uint8_t)
    __IO uint32_t GPADC_BGR_REG;                      /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
         RESERVED(0x9F0[0x09FC - 0x09F0], uint8_t)
    __IO uint32_t THS_BGR_REG;                        /*!< Offset 0x9FC THS Bus Gating Reset Register */
         RESERVED(0xA00[0x0A20 - 0x0A00], uint8_t)
    __IO uint32_t OWA_CLK_REG;                        /*!< Offset 0xA20 OWA Clock Register */
         RESERVED(0xA24[0x0A2C - 0x0A24], uint8_t)
    __IO uint32_t OWA_BGR_REG;                        /*!< Offset 0xA2C OWA Bus Gating Reset Register */
         RESERVED(0xA30[0x0A40 - 0x0A30], uint8_t)
    __IO uint32_t DMIC_CLK_REG;                       /*!< Offset 0xA40 DMIC Clock Register */
         RESERVED(0xA44[0x0A4C - 0x0A44], uint8_t)
    __IO uint32_t DMIC_BGR_REG;                       /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
    __IO uint32_t AUDIO_CODEC_1X_CLK_REG;             /*!< Offset 0xA50 AUDIO CODEC 1X Clock Register */
    __IO uint32_t AUDIO_CODEC_4X_CLK_REG;             /*!< Offset 0xA54 AUDIO CODEC 4X Clock Register */
         RESERVED(0xA58[0x0A5C - 0x0A58], uint8_t)
    __IO uint32_t AUDIO_CODEC_BGR_REG;                /*!< Offset 0xA5C AUDIO CODEC Bus Gating Reset Register */
    __IO uint32_t AUDIO_HUB_CLK_REG;                  /*!< Offset 0xA60 AUDIO_HUB Clock Register */
         RESERVED(0xA64[0x0A6C - 0x0A64], uint8_t)
    __IO uint32_t AUDIO_HUB_BGR_REG;                  /*!< Offset 0xA6C AUDIO_HUB Bus Gating Reset Register */
    __IO uint32_t USB0_CLK_REG;                       /*!< Offset 0xA70 USB0 Clock Register */
    __IO uint32_t USB1_CLK_REG;                       /*!< Offset 0xA74 USB1 Clock Register */
    __IO uint32_t USB2_CLK_REG;                       /*!< Offset 0xA78 USB2 Clock Register */
    __IO uint32_t USB3_CLK_REG;                       /*!< Offset 0xA7C USB3 Clock Register */
         RESERVED(0xA80[0x0A8C - 0x0A80], uint8_t)
    __IO uint32_t USB_BGR_REG;                        /*!< Offset 0xA8C USB Bus Gating Reset Register */
         RESERVED(0xA90[0x0A9C - 0x0A90], uint8_t)
    __IO uint32_t LRADC_BGR_REG;                      /*!< Offset 0xA9C LRADC Bus Gating Reset Register */
         RESERVED(0xAA0[0x0B00 - 0x0AA0], uint8_t)
    __IO uint32_t HDMI0_CLK_REG;                      /*!< Offset 0xB00 HDMI0 Clock Register */
    __IO uint32_t HDMI0_SLOW_CLK_REG;                 /*!< Offset 0xB04 HDMI0 Slow Clock Register */
         RESERVED(0xB08[0x0B10 - 0x0B08], uint8_t)
    __IO uint32_t HDMI_CEC_CLK_REG;                   /*!< Offset 0xB10 HDMI CEC Clock Register */
         RESERVED(0xB14[0x0B1C - 0x0B14], uint8_t)
    __IO uint32_t HDMI_BGR_REG;                       /*!< Offset 0xB1C HDMI Bus Gating Reset Register */
         RESERVED(0xB20[0x0B5C - 0x0B20], uint8_t)
    __IO uint32_t DISPLAY_IF_TOP_BGR_REG;             /*!< Offset 0xB5C DISPLAY_IF_TOP BUS GATING RESET Register */
    __IO uint32_t TCON_LCD0_CLK_REG;                  /*!< Offset 0xB60 TCON LCD0 Clock Register */
    __IO uint32_t TCON_LCD1_CLK_REG;                  /*!< Offset 0xB64 TCON LCD1 Clock Register */
         RESERVED(0xB68[0x0B7C - 0x0B68], uint8_t)
    __IO uint32_t TCON_LCD_BGR_REG;                   /*!< Offset 0xB7C TCON LCD BUS GATING RESET Register */
    __IO uint32_t TCON_TV0_CLK_REG;                   /*!< Offset 0xB80 TCON TV0 Clock Register */
    __IO uint32_t TCON_TV1_CLK_REG;                   /*!< Offset 0xB84 TCON TV1 Clock Register */
         RESERVED(0xB88[0x0B9C - 0x0B88], uint8_t)
    __IO uint32_t TCON_TV_BGR_REG;                    /*!< Offset 0xB9C TCON TV GATING RESET Register */
         RESERVED(0xBA0[0x0BAC - 0x0BA0], uint8_t)
    __IO uint32_t LVDS_BGR_REG;                       /*!< Offset 0xBAC LVDS BUS GATING RESET Register */
    __IO uint32_t TVE0_CLK_REG;                       /*!< Offset 0xBB0 TVE0 Clock Register */
         RESERVED(0xBB4[0x0BBC - 0x0BB4], uint8_t)
    __IO uint32_t TVE_BGR_REG;                        /*!< Offset 0xBBC TVE BUS GATING RESET Register */
         RESERVED(0xBC0[0x0C04 - 0x0BC0], uint8_t)
    __IO uint32_t CSI_TOP_CLK_REG;                    /*!< Offset 0xC04 CSI TOP Clock Register */
    __IO uint32_t CSI_MST_CLK0_REG;                   /*!< Offset 0xC08 CSI_Master Clock0 Register */
    __IO uint32_t CSI_MST_CLK1_REG;                   /*!< Offset 0xC0C CSI_Master Clock1 Register */
         RESERVED(0xC10[0x0C2C - 0x0C10], uint8_t)
    __IO uint32_t CSI_BGR_REG;                        /*!< Offset 0xC2C CSI Bus Gating Reset Register */
         RESERVED(0xC30[0x0C40 - 0x0C30], uint8_t)
    __IO uint32_t HDMI_HDCP_CLK_REG;                  /*!< Offset 0xC40 HDMI HDCP Clock Register */
         RESERVED(0xC44[0x0C4C - 0x0C44], uint8_t)
    __IO uint32_t HDMI_HDCP_BGR_REG;                  /*!< Offset 0xC4C HDMI HDCP Bus Gating Reset Register */
         RESERVED(0xC50[0x0F00 - 0x0C50], uint8_t)
    __IO uint32_t CCU_SEC_SWITCH_REG;                 /*!< Offset 0xF00 CCU Security Switch Register */
    __IO uint32_t PLL_LOCK_DBG_CTRL_REG;              /*!< Offset 0xF04 PLL Lock Debug Control Register */
    __IO uint32_t FRE_DET_CTRL_REG;                   /*!< Offset 0xF08 Frequency Detect Control Register */
    __IO uint32_t FRE_UP_LIM_REG;                     /*!< Offset 0xF0C Frequency Up Limit Register */
    __IO uint32_t FRE_DOWN_LIM_REG;                   /*!< Offset 0xF10 Frequency Down Limit Register */
         RESERVED(0xF14[0x0F20 - 0x0F14], uint8_t)
    __IO uint32_t CCU_24M_27M_CLK_OUTPUT_REG;         /*!< Offset 0xF20 24M or 27M Clock Output Register */
         RESERVED(0xF24[0x1000 - 0x0F24], uint8_t)
} CCU_TypeDef; /* size of structure = 0x1000 */
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
 * @brief GPIO
 */
/*!< GPIO Port Controller */
typedef struct GPIO_Type
{
    __IO uint32_t CFG [0x004];                        /*!< Offset 0x000 Configure Register */
    __IO uint32_t DATA;                               /*!< Offset 0x010 Data Register */
    __IO uint32_t DATA_SET;                           /*!< Offset 0x014 Data Set Register */
    __IO uint32_t DATA_CLR;                           /*!< Offset 0x018 Data Clear Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t DRV [0x002];                        /*!< Offset 0x020 Multi_Driving Register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t PULL [0x002];                       /*!< Offset 0x030 Pull Register */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IO uint32_t INT_CFG [0x004];                    /*!< Offset 0x040 External Interrupt Configure Register */
    __IO uint32_t INT_CTL;                            /*!< Offset 0x050 External Interrupt Control Register */
    __IO uint32_t INT_STA;                            /*!< Offset 0x054 External Interrupt Status Register */
    __IO uint32_t INT_DEB;                            /*!< Offset 0x058 External Debounce Configure Register */
         RESERVED(0x05C[0x0070 - 0x005C], uint8_t)
    __IO uint32_t SECURE;                             /*!< Offset 0x070 SECURE Configure Register */
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
        __IO uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
        __IO uint32_t DATA;                           /*!< Offset 0x010 Data Register */
        __IO uint32_t DATA_SET;                       /*!< Offset 0x014 Data Set Register */
        __IO uint32_t DATA_CLR;                       /*!< Offset 0x018 Data Clear Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
        __IO uint32_t DRV [0x002];                    /*!< Offset 0x020 Multi_Driving Register */
             RESERVED(0x028[0x0030 - 0x0028], uint8_t)
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x030 Pull Register */
             RESERVED(0x038[0x0040 - 0x0038], uint8_t)
        __IO uint32_t INT_CFG [0x004];                /*!< Offset 0x040 External Interrupt Configure Register */
        __IO uint32_t INT_CTL;                        /*!< Offset 0x050 External Interrupt Control Register */
        __IO uint32_t INT_STA;                        /*!< Offset 0x054 External Interrupt Status Register */
        __IO uint32_t INT_DEB;                        /*!< Offset 0x058 External Debounce Configure Register */
             RESERVED(0x05C[0x0070 - 0x005C], uint8_t)
        __IO uint32_t SECURE;                         /*!< Offset 0x070 SECURE Configure Register */
             RESERVED(0x074[0x0080 - 0x0074], uint8_t)
    } GPIO_PINS [0x009];                              /*!< Offset 0x000 GPIO pin control B, C, D. E, F, G, H, I, J, K */
} GPIOBLOCK_TypeDef; /* size of structure = 0x480 */
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
    __IO uint32_t SPI_CCR;                            /*!< Offset 0x024 SPI Clock Control Register */
    __IO uint32_t SPI_SAMP_DL;                        /*!< Offset 0x028 SPI Sample Delay Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t SPI_MBC;                            /*!< Offset 0x030 SPI Master Burst Counter Register */
    __IO uint32_t SPI_MTC;                            /*!< Offset 0x034 SPI Master Transmit Counter Register */
    __IO uint32_t SPI_BCC;                            /*!< Offset 0x038 SPI Master Burst Control Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t SPI_BATCR;                          /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    __IO uint32_t SPI_3W_CCR;                         /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    __IO uint32_t SPI_TBR;                            /*!< Offset 0x048 SPI TX Bit Register */
    __IO uint32_t SPI_RBR;                            /*!< Offset 0x04C SPI RX Bit Register */
         RESERVED(0x050[0x0088 - 0x0050], uint8_t)
    __IO uint32_t SPI_NDMA_MODE_CTL;                  /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
         RESERVED(0x08C[0x0200 - 0x008C], uint8_t)
    __IO uint32_t SPI_TXD;                            /*!< Offset 0x200 SPI TX Data Register */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IO uint32_t SPI_RXD;                            /*!< Offset 0x300 SPI RX Data Register */
         RESERVED(0x304[0x1000 - 0x0304], uint8_t)
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief S_GPIO
 */
/*!< S_GPIO Secure Port Controller */
typedef struct S_GPIO_Type
{
    __IO uint32_t CFG [0x004];                        /*!< Offset 0x000 Configure Register */
    __IO uint32_t DATA;                               /*!< Offset 0x010 Data Register */
    __IO uint32_t DRV [0x004];                        /*!< Offset 0x014 Multi_Driving Register */
    __IO uint32_t PULL [0x002];                       /*!< Offset 0x024 Pull Register */
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
        __IO uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
        __IO uint32_t DATA;                           /*!< Offset 0x010 Data Register */
        __IO uint32_t DRV [0x004];                    /*!< Offset 0x014 Multi_Driving Register */
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x024 Pull Register */
             RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    } GPIO_PINS [0x002];                              /*!< Offset 0x000 GPIO pin control L, M */
         RESERVED(0x060[0x0200 - 0x0060], uint8_t)
    struct
    {
        __IO uint32_t EINT_CFG [0x004];               /*!< Offset 0x200 External Interrupt Configure Registers */
        __IO uint32_t EINT_CTL;                       /*!< Offset 0x210 External Interrupt Control Register */
        __IO uint32_t EINT_STATUS;                    /*!< Offset 0x214 External Interrupt Status Register */
        __IO uint32_t EINT_DEB;                       /*!< Offset 0x218 External Interrupt Debounce Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } GPIO_INTS [0x002];                              /*!< Offset 0x200 GPIO interrupt control */
         RESERVED(0x240[0x0340 - 0x0240], uint8_t)
    __IO uint32_t GPIO_POW_MOD_SEL;                   /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    __IO uint32_t GPIO_POW_MS_CTL;                    /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    __IO uint32_t GPIO_POW_VAL;                       /*!< Offset 0x348 PIO Group Power Value Register */
} S_GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
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
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IO uint32_t WDOG_CTRL_REG;                      /*!< Offset 0x0B0 Watchdog Control Register */
    __IO uint32_t WDOG_CFG_REG;                       /*!< Offset 0x0B4 Watchdog Configuration Register */
    __IO uint32_t WDOG_MODE_REG;                      /*!< Offset 0x0B8 Watchdog Mode Register */
         RESERVED(0x0BC[0x00C0 - 0x00BC], uint8_t)
    __IO uint32_t AVS_CNT_CTL_REG;                    /*!< Offset 0x0C0 AVS Control Register */
    __IO uint32_t AVS_CNT0_REG;                       /*!< Offset 0x0C4 AVS Counter 0 Register */
    __IO uint32_t AVS_CNT1_REG;                       /*!< Offset 0x0C8 AVS Counter 1 Register */
    __IO uint32_t AVS_CNT_DIV_REG;                    /*!< Offset 0x0CC AVS Divisor Register */
         RESERVED(0x0D0[0x0400 - 0x00D0], uint8_t)
} TIMER_TypeDef; /* size of structure = 0x400 */
/*
 * @brief TWI
 */
/*!< TWI Two Wire Interface (TWI) */
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
         RESERVED(0x08C[0x00B0 - 0x008C], uint8_t)
    __IO uint32_t UART_DBG_DLL;                       /*!< Offset 0x0B0 UART Debug DLL Register */
    __IO uint32_t UART_DBG_DLH;                       /*!< Offset 0x0B4 UART Debug DLH Register */
         RESERVED(0x0B8[0x00C0 - 0x00B8], uint8_t)
    __IO uint32_t UART_485_CTL;                       /*!< Offset 0x0C0 UART RS485 Control and Status Register */
    __IO uint32_t RS485_ADDR_MATCH;                   /*!< Offset 0x0C4 UART RS485 Addres Match Register  */
    __IO uint32_t BUS_IDLE_CHECK;                     /*!< Offset 0x0C8 UART RS485 Bus Idle Check Register */
    __IO uint32_t TX_DLY;                             /*!< Offset 0x0CC UART TX Delay Register */
         RESERVED(0x0D0[0x0400 - 0x00D0], uint8_t)
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
    __IO uint32_t USB_EPFIFO [0x005];                 /*!< Offset 0x000 USB_EPFIFO [0..4] USB FIFO Entry for Endpoint N */
         RESERVED(0x014[0x0040 - 0x0014], uint8_t)
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

#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
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
#define SPI2 ((SPI_TypeDef *) SPI2_BASE)              /*!< SPI2 Serial Peripheral Interface register set access pointer */
#define SPI3 ((SPI_TypeDef *) SPI3_BASE)              /*!< SPI3 Serial Peripheral Interface register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define USB20_OTG_DEVICE ((USBOTG_TypeDef *) USB20_OTG_DEVICE_BASE)/*!< USB20_OTG_DEVICE USB OTG Dual-Role Device controller register set access pointer */
#define USB20_HOST0_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST0_EHCI_BASE)/*!< USB20_HOST0_EHCI  register set access pointer */
#define USB20_HOST0_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST0_OHCI_BASE)/*!< USB20_HOST0_OHCI  register set access pointer */
#define USB20_HOST1_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST1_EHCI_BASE)/*!< USB20_HOST1_EHCI  register set access pointer */
#define USB20_HOST1_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST1_OHCI_BASE)/*!< USB20_HOST1_OHCI  register set access pointer */
#define USB20_HOST2_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST2_EHCI_BASE)/*!< USB20_HOST2_EHCI  register set access pointer */
#define USB20_HOST2_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST2_OHCI_BASE)/*!< USB20_HOST2_OHCI  register set access pointer */
#define USB20_HOST3_EHCI ((USB_EHCI_Capability_TypeDef *) USB20_HOST3_EHCI_BASE)/*!< USB20_HOST3_EHCI  register set access pointer */
#define USB20_HOST3_OHCI ((USB_OHCI_Capability_TypeDef *) USB20_HOST3_OHCI_BASE)/*!< USB20_HOST3_OHCI  register set access pointer */
#define S_GPIOL ((S_GPIO_TypeDef *) S_GPIOL_BASE)     /*!< S_GPIOL Secure Port Controller register set access pointer */
#define S_GPIOM ((S_GPIO_TypeDef *) S_GPIOM_BASE)     /*!< S_GPIOM Secure Port Controller register set access pointer */
#define S_UART0 ((UART_TypeDef *) S_UART0_BASE)       /*!< S_UART0  register set access pointer */
#define S_UART1 ((UART_TypeDef *) S_UART1_BASE)       /*!< S_UART1  register set access pointer */
#define S_TWI1 ((TWI_TypeDef *) S_TWI1_BASE)          /*!< S_TWI1 Two Wire Interface (TWI) register set access pointer */
#define S_TWI2 ((TWI_TypeDef *) S_TWI2_BASE)          /*!< S_TWI2 Two Wire Interface (TWI) register set access pointer */
#define S_SPI ((SPI_TypeDef *) S_SPI_BASE)            /*!< S_SPI Serial Peripheral Interface register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__2A4CD94A_A755_4786_BB96_E52021B2959C__INCLUDED */
