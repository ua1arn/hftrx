/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__9E291677_B6C7_4f4a_BBFD_D0F40B363F83__INCLUDED
#define HEADER__9E291677_B6C7_4f4a_BBFD_D0F40B363F83__INCLUDED
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
    TWI0_IRQn = 39,                                   /*!< TWI  */
    TWI1_IRQn = 40,                                   /*!< TWI  */
    TWI2_IRQn = 41,                                   /*!< TWI  */
    TWI3_IRQn = 42,                                   /*!< TWI  */
    I2S0_IRQn = 58,                                   /*!< I2S_PCM  */
    I2S1_IRQn = 59,                                   /*!< I2S_PCM  */
    I2S2_IRQn = 60,                                   /*!< I2S_PCM  */
    I2S3_IRQn = 62,                                   /*!< I2S_PCM  */
    USB20_HOST0_EHCI_IRQn = 62,                       /*!< USB_EHCI_Capability  */
    USB20_HOST0_OHCI_IRQn = 63,                       /*!< USB_OHCI_Capability  */
    USB20_OTG_DEVICE_IRQn = 64,                       /*!< USBOTG USB OTG Dual-Role Device controller */
    USB20_HOST1_EHCI_IRQn = 65,                       /*!< USB_EHCI_Capability  */
    USB20_HOST1_OHCI_IRQn = 66,                       /*!< USB_OHCI_Capability  */
    TIMER0_IRQn = 83,                                 /*!< TIMER  */
    TIMER1_IRQn = 84,                                 /*!< TIMER  */
    GPIOB_IRQn = 86,                                  /*!< GPIOINT GPIOC interrupt */
    GPIOC_IRQn = 87,                                  /*!< GPIOINT GPIOC interrupt */
    GPIOD_IRQn = 88,                                  /*!< GPIOINT GPIOD interrupt */
    GPIOE_IRQn = 89,                                  /*!< GPIOINT GPIOE interrupt */
    GPIOF_IRQn = 90,                                  /*!< GPIOINT GPIOF interrupt */
    GPIOG_IRQn = 91,                                  /*!< GPIOINT GPIOG interrupt */
    GPIOH_IRQn = 92,                                  /*!< GPIOINT GPIOH interrupt */
    GPIOI_IRQn = 93,                                  /*!< GPIOINT GPIOI interrupt */
    R_TIMER0_IRQn = 136,                              /*!< TIMER  */
    R_TIMER1_IRQn = 137,                              /*!< TIMER  */
    R_TIMER2_IRQn = 138,                              /*!< TIMER  */
    R_TIMER3_IRQn = 139,                              /*!< TIMER  */
    R_WDOG_IRQn = 141,                                /*!< TIMER Watchdog interrupt in CPU */
    R_TWD_IRQn = 142,                                 /*!< TIMER Trust watchdog interrupt in CPU */
    R_GPIOL_IRQn = 143,                               /*!< GPIOINT GPIOL interrupt in CPUS */
    R_UART_IRQn = 144,                                /*!< UART R_UART */
    S_TWI0_IRQn = 145,                                /*!< TWI  */
    S_TWI1_IRQn = 146,                                /*!< TWI  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK Port Controller Base */
#define CCU_BASE ((uintptr_t) 0x03001000)             /*!< CCU  Base */
#define TIMER_BASE ((uintptr_t) 0x03009000)           /*!< TIMER  Base */
#define GPIOA_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Port Controller Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x0300B000)       /*!< GPIOBLOCK Port Controller Base */
#define GPIOB_BASE ((uintptr_t) 0x0300B024)           /*!< GPIO Port Controller Base */
#define GPIOC_BASE ((uintptr_t) 0x0300B048)           /*!< GPIO Port Controller Base */
#define GPIOD_BASE ((uintptr_t) 0x0300B06C)           /*!< GPIO Port Controller Base */
#define GPIOE_BASE ((uintptr_t) 0x0300B090)           /*!< GPIO Port Controller Base */
#define GPIOF_BASE ((uintptr_t) 0x0300B0B4)           /*!< GPIO Port Controller Base */
#define GPIOG_BASE ((uintptr_t) 0x0300B0D8)           /*!< GPIO Port Controller Base */
#define GPIOH_BASE ((uintptr_t) 0x0300B0FC)           /*!< GPIO Port Controller Base */
#define GPIOI_BASE ((uintptr_t) 0x0300B120)           /*!< GPIO Port Controller Base */
#define GPIOINTA_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT  Base */
#define GPIOINTB_BASE ((uintptr_t) 0x0300B220)        /*!< GPIOINT  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x0300B240)        /*!< GPIOINT  Base */
#define GPIOINTD_BASE ((uintptr_t) 0x0300B260)        /*!< GPIOINT  Base */
#define GPIOINTE_BASE ((uintptr_t) 0x0300B280)        /*!< GPIOINT  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x0300B2A0)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x0300B2C0)        /*!< GPIOINT  Base */
#define GPIOINTH_BASE ((uintptr_t) 0x0300B2E0)        /*!< GPIOINT  Base */
#define GPIOINTI_BASE ((uintptr_t) 0x0300B300)        /*!< GPIOINT  Base */
#define GIC_BASE ((uintptr_t) 0x03020000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!< GIC_DISTRIBUTOR GIC DISTRIBUTOR Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x03024000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x03025000)            /*!< GICV  Base */
#define UART0_BASE ((uintptr_t) 0x05000000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x05000400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x05000800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x05000C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x05001000)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x05002000)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x05002400)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x05002800)            /*!< TWI  Base */
#define TWI3_BASE ((uintptr_t) 0x05002C00)            /*!< TWI  Base */
#define I2S0_BASE ((uintptr_t) 0x05090000)            /*!< I2S_PCM  Base */
#define I2S1_BASE ((uintptr_t) 0x05091000)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x05092000)            /*!< I2S_PCM  Base */
#define I2S3_BASE ((uintptr_t) 0x05093000)            /*!< I2S_PCM  Base */
#define USB20_OTG_DEVICE_BASE ((uintptr_t) 0x05100000)/*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB20_OTG_PHYC_BASE ((uintptr_t) 0x05100400)  /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST0_EHCI_BASE ((uintptr_t) 0x05101000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST0_OHCI_BASE ((uintptr_t) 0x05101400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC0_BASE ((uintptr_t) 0x05101800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define USB20_HOST1_EHCI_BASE ((uintptr_t) 0x05200000)/*!< USB_EHCI_Capability  Base */
#define USB20_HOST1_OHCI_BASE ((uintptr_t) 0x05200400)/*!< USB_OHCI_Capability  Base */
#define USBPHYC1_BASE ((uintptr_t) 0x05200800)        /*!< USBPHYC HCI Contgroller and PHY Interface Description Base */
#define R_WDG_BASE ((uintptr_t) 0x07020400)           /*!< TIMER  Base */
#define R_TWDG_BASE ((uintptr_t) 0x07020800)          /*!< TIMER  Base */
#define GPIOL_BASE ((uintptr_t) 0x07022000)           /*!< GPIO Port Controller Base */
#define R_PIO_BASE ((uintptr_t) 0x07022000)           /*!< GPIO Port Controller Base */
#define R_TIMER_BASE ((uintptr_t) 0x07029000)         /*!< TIMER  Base */
#define R_UART_BASE ((uintptr_t) 0x07080000)          /*!< UART  Base */
#define R_UART_BASE ((uintptr_t) 0x07080000)          /*!< UART  Base */
#define S_TWI0_BASE ((uintptr_t) 0x07081400)          /*!< TWI  Base */
#define S_TWI1_BASE ((uintptr_t) 0x07081800)          /*!< TWI  Base */

#if __aarch64__
    #include <core64_ca.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief CCU
 */
/*!< CCU  */
typedef struct CCU_Type
{
    __IO uint32_t PLL_CPUX_CTRL_REG;                  /*!< Offset 0x000 PLL_CPUX Control Register  */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t PLL_DDR_CTRL_REG;                   /*!< Offset 0x010 PLL_DDR Control Register  */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t PLL_PERI0_CTRL_REG;                 /*!< Offset 0x020 PLL_PERI0 Control Register  */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t PLL_PERI1_CTRL_REG;                 /*!< Offset 0x028 PLL_PERI1 Control Register  */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t PLL_GPU_CTRL_REG;                   /*!< Offset 0x030 PLL_GPU Control Register  */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t PLL_VIDEO0_CTRL_REG;                /*!< Offset 0x040 PLL_VIDEO0 Control Register  */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __IO uint32_t PLL_VIDEO1_CTRL_REG;                /*!< Offset 0x048 PLL_VIDEO1 Control Register  */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __IO uint32_t PLL_VIDEO2_CTRL_REG;                /*!< Offset 0x050 PLL_VIDEO2 Control Register  */
         RESERVED(0x054[0x0058 - 0x0054], uint8_t)
    __IO uint32_t PLL_VE_CTRL_REG;                    /*!< Offset 0x058 PLL_VE Control Register  */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IO uint32_t PLL_COM_CTRL_REG;                   /*!< Offset 0x060 PLL_COM Control Register  */
         RESERVED(0x064[0x0068 - 0x0064], uint8_t)
    __IO uint32_t PLL_VIDEO3_CTRL_REG;                /*!< Offset 0x068 PLL_VIDEO3 Control Register  */
         RESERVED(0x06C[0x0078 - 0x006C], uint8_t)
    __IO uint32_t PLL_AUDIO_CTRL_REG;                 /*!< Offset 0x078 PLL_AUDIO Control Register  */
         RESERVED(0x07C[0x0110 - 0x007C], uint8_t)
    __IO uint32_t PLL_DDR_PAT0_CTRL_REG;              /*!< Offset 0x110 PLL_DDR Pattern0 Control Register  */
    __IO uint32_t PLL_DDR_PAT1_CTRL_REG;              /*!< Offset 0x114 PLL_DDR Pattern1 Control Register  */
         RESERVED(0x118[0x0120 - 0x0118], uint8_t)
    __IO uint32_t PLL_PERI0_PAT0_CTRL_REG;            /*!< Offset 0x120 PLL_PERI0 Pattern0 Control Register  */
    __IO uint32_t PLL_PERI0_PAT1_CTRL_REG;            /*!< Offset 0x124 PLL_PERI0 Pattern1 Control Register  */
    __IO uint32_t PLL_PERI1_PAT0_CTRL_REG;            /*!< Offset 0x128 PLL_PERI1 Pattern0 Control Register  */
    __IO uint32_t PLL_PERI1_PAT1_CTRL_REG;            /*!< Offset 0x12C PLL_PERI1 Pattern1 Control Register  */
    __IO uint32_t PLL_GPU0_PAT0_CTRL_REG;             /*!< Offset 0x130 PLL_GPU0 Pattern0 Control Register  */
    __IO uint32_t PLL_GPU0_PAT1_CTRL_REG;             /*!< Offset 0x134 PLL_GPU0 Pattern1 Control Register  */
         RESERVED(0x138[0x0140 - 0x0138], uint8_t)
    __IO uint32_t PLL_VIDEO0_PAT0_CTRL_REG;           /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register  */
    __IO uint32_t PLL_VIDEO0_PAT1_CTRL_REG;           /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register  */
    __IO uint32_t PLL_VIDEO1_PAT0_CTRL_REG;           /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register  */
    __IO uint32_t PLL_VIDEO1_PAT1_CTRL_REG;           /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register  */
    __IO uint32_t PLL_VIDEO2_PAT0_CTRL_REG;           /*!< Offset 0x150 PLL_VIDEO2 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO2_PAT1_CTRL_REG;           /*!< Offset 0x154 PLL_VIDEO2 Pattern1 Control Register  */
    __IO uint32_t PLL_VE_PAT0_CTRL_REG;               /*!< Offset 0x158 PLL_VE Pattern0 Control Register  */
    __IO uint32_t PLL_VE_PAT1_CTRL_REG;               /*!< Offset 0x15C PLL_VE Pattern1 Control Register  */
    __IO uint32_t PLL_COM_PAT0_CTRL_REG;              /*!< Offset 0x160 PLL_COM Pattern0 Control Register  */
    __IO uint32_t PLL_COM_PAT1_CTRL_REG;              /*!< Offset 0x164 PLL_COM Pattern1 Control Register  */
    __IO uint32_t PLL_VIDEO3_PAT0_CTRL_REG;           /*!< Offset 0x168 PLL_VIDEO3 Pattern0 Control Register  */
    __IO uint32_t PLL_VIDEO3_PAT1_CTRL_REG;           /*!< Offset 0x16C PLL_VIDEO3 Pattern1 Control Register  */
         RESERVED(0x170[0x0178 - 0x0170], uint8_t)
    __IO uint32_t PLL_AUDIO_PAT0_CTRL_REG;            /*!< Offset 0x178 PLL_AUDIO Pattern0 Control Register  */
    __IO uint32_t PLL_AUDIO_PAT1_CTRL_REG;            /*!< Offset 0x17C PLL_AUDIO Pattern1 Control Register  */
         RESERVED(0x180[0x0300 - 0x0180], uint8_t)
    __IO uint32_t PLL_CPUX_BIAS_REG;                  /*!< Offset 0x300 PLL_CPUX Bias Register  */
         RESERVED(0x304[0x0310 - 0x0304], uint8_t)
    __IO uint32_t PLL_DDR_BIAS_REG;                   /*!< Offset 0x310 PLL_DDR Bias Register */
         RESERVED(0x314[0x0320 - 0x0314], uint8_t)
    __IO uint32_t PLL_PERI0_BIAS_REG;                 /*!< Offset 0x320 PLL_PERI0 Bias Register  */
         RESERVED(0x324[0x0328 - 0x0324], uint8_t)
    __IO uint32_t PLL_PERI1_BIAS_REG;                 /*!< Offset 0x328 PLL_PERI1 Bias Register  */
         RESERVED(0x32C[0x0330 - 0x032C], uint8_t)
    __IO uint32_t PLL_GPU0_BIAS_REG;                  /*!< Offset 0x330 PLL_GPU0 Bias Register  */
         RESERVED(0x334[0x0340 - 0x0334], uint8_t)
    __IO uint32_t PLL_VIDEO0_BIAS_REG;                /*!< Offset 0x340 PLL_VIDEO0 Bias Register  */
         RESERVED(0x344[0x0348 - 0x0344], uint8_t)
    __IO uint32_t PLL_VIDEO1_BIAS_REG;                /*!< Offset 0x348 PLL_VIDEO1 Bias Register  */
         RESERVED(0x34C[0x0350 - 0x034C], uint8_t)
    __IO uint32_t PLL_VIDEO2_BIAS_REG;                /*!< Offset 0x350 PLL_VIDEO2 Bias Register  */
         RESERVED(0x354[0x0358 - 0x0354], uint8_t)
    __IO uint32_t PLL_VE_BIAS_REG;                    /*!< Offset 0x358 PLL_VE Bias Register  */
         RESERVED(0x35C[0x0360 - 0x035C], uint8_t)
    __IO uint32_t PLL_COM_BIAS_REG;                   /*!< Offset 0x360 PLL_COM Bias Register  */
         RESERVED(0x364[0x0368 - 0x0364], uint8_t)
    __IO uint32_t PLL_VIDEO3_BIAS_REG;                /*!< Offset 0x368 PLL_VIDEO3 Bias Register  */
         RESERVED(0x36C[0x0378 - 0x036C], uint8_t)
    __IO uint32_t PLL_AUDIO_BIAS_REG;                 /*!< Offset 0x378 PLL_AUDIO Bias Register  */
         RESERVED(0x37C[0x0400 - 0x037C], uint8_t)
    __IO uint32_t PLL_CPUX_TUN_REG;                   /*!< Offset 0x400 PLL_CPUX Tuning Register  */
         RESERVED(0x404[0x0500 - 0x0404], uint8_t)
    __IO uint32_t C0_CPUX_AXI_CFG_REG;                /*!< Offset 0x500 CPUX_AXI Configuration Register  */
         RESERVED(0x504[0x0510 - 0x0504], uint8_t)
    __IO uint32_t PSI_AHB1_AHB2_CFG_REG;              /*!< Offset 0x510 PSI_AHB1_AHB2 Configuration Register  */
         RESERVED(0x514[0x051C - 0x0514], uint8_t)
    __IO uint32_t AHB3_CFG_REG;                       /*!< Offset 0x51C AHB3 Configuration Register  */
    __IO uint32_t APB1_CFG_REG;                       /*!< Offset 0x520 APB1 Configuration Register  */
    __IO uint32_t APB2_CFG_REG;                       /*!< Offset 0x524 APB2 Configuration Register  */
         RESERVED(0x528[0x0540 - 0x0528], uint8_t)
    __IO uint32_t MBUS_CFG_REG;                       /*!< Offset 0x540 MBUS Configuration Register  */
         RESERVED(0x544[0x0600 - 0x0544], uint8_t)
    __IO uint32_t DE_CLK_REG;                         /*!< Offset 0x600 DE Clock Register  */
         RESERVED(0x604[0x060C - 0x0604], uint8_t)
    __IO uint32_t DE_BGR_REG;                         /*!< Offset 0x60C DE Bus Gating Reset Register  */
         RESERVED(0x610[0x0630 - 0x0610], uint8_t)
    __IO uint32_t G2D_CLK_REG;                        /*!< Offset 0x630 G2D Clock Register  */
         RESERVED(0x634[0x063C - 0x0634], uint8_t)
    __IO uint32_t G2D_BGR_REG;                        /*!< Offset 0x63C G2D Bus Gating Reset Register  */
         RESERVED(0x640[0x0670 - 0x0640], uint8_t)
    __IO uint32_t GPU_CORE_CLK_REG;                   /*!< Offset 0x670 GPU Clock Register  */
         RESERVED(0x674[0x067C - 0x0674], uint8_t)
    __IO uint32_t GPU_BGR_REG;                        /*!< Offset 0x67C GPU Bus Gating Reset Register  */
    __IO uint32_t CE_CLK_REG;                         /*!< Offset 0x680 CE Clock Register  */
         RESERVED(0x684[0x068C - 0x0684], uint8_t)
    __IO uint32_t CE_BGR_REG;                         /*!< Offset 0x68C CE Bus Gating Reset Register  */
    __IO uint32_t VE_CLK_REG;                         /*!< Offset 0x690 VE Clock Register  */
         RESERVED(0x694[0x069C - 0x0694], uint8_t)
    __IO uint32_t VE_BGR_REG;                         /*!< Offset 0x69C VE Bus Gating Reset Register  */
         RESERVED(0x6A0[0x070C - 0x06A0], uint8_t)
    __IO uint32_t DMA_BGR_REG;                        /*!< Offset 0x70C DMA Bus Gating Reset Register  */
         RESERVED(0x710[0x071C - 0x0710], uint8_t)
    __IO uint32_t MSGBOX_BGR_REG;                     /*!< Offset 0x71C MSGBOX Bus Gating Reset Register  */
         RESERVED(0x720[0x072C - 0x0720], uint8_t)
    __IO uint32_t SPINLOCK_BGR_REG;                   /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register  */
         RESERVED(0x730[0x073C - 0x0730], uint8_t)
    __IO uint32_t HSTIMER_BGR_REG;                    /*!< Offset 0x73C HSTIMER Bus Gating Reset Register  */
    __IO uint32_t AVS_CLK_REG;                        /*!< Offset 0x740 AVS Clock Register */
         RESERVED(0x744[0x078C - 0x0744], uint8_t)
    __IO uint32_t DBGSYS_BGR_REG;                     /*!< Offset 0x78C DBGSYS Bus Gating Reset Register  */
         RESERVED(0x790[0x079C - 0x0790], uint8_t)
    __IO uint32_t PSI_BGR_REG;                        /*!< Offset 0x79C PSI Bus Gating Reset Register  */
         RESERVED(0x7A0[0x07AC - 0x07A0], uint8_t)
    __IO uint32_t PWM_BGR_REG;                        /*!< Offset 0x7AC PWM Bus Gating Reset Register  */
         RESERVED(0x7B0[0x07BC - 0x07B0], uint8_t)
    __IO uint32_t IOMMU_BGR_REG;                      /*!< Offset 0x7BC IOMMU Bus Gating Reset Register  */
         RESERVED(0x7C0[0x0800 - 0x07C0], uint8_t)
    __IO uint32_t DRAM_CLK_REG;                       /*!< Offset 0x800 DRAM Clock Register  */
    __IO uint32_t MBUS_MAT_CLK_GATING_REG;            /*!< Offset 0x804 MBUS Master Clock Gating Register  */
         RESERVED(0x808[0x080C - 0x0808], uint8_t)
    __IO uint32_t DRAM_BGR_REG;                       /*!< Offset 0x80C DRAM Bus Gating Reset Register  */
    __IO uint32_t NAND0_0_CLK_REG;                    /*!< Offset 0x810 NAND0_0 Clock Register  */
    __IO uint32_t NAND0_1_CLK_REG;                    /*!< Offset 0x814 NAND0_1 Clock Register  */
         RESERVED(0x818[0x082C - 0x0818], uint8_t)
    __IO uint32_t NAND_BGR_REG;                       /*!< Offset 0x82C NAND Bus Gating Reset Register  */
    __IO uint32_t SMHC0_CLK_REG;                      /*!< Offset 0x830 SMHC0 Clock Register  */
    __IO uint32_t SMHC1_CLK_REG;                      /*!< Offset 0x834 SMHC1 Clock Register  */
    __IO uint32_t SMHC2_CLK_REG;                      /*!< Offset 0x838 SMHC2 Clock Register  */
         RESERVED(0x83C[0x084C - 0x083C], uint8_t)
    __IO uint32_t SMHC_BGR_REG;                       /*!< Offset 0x84C SMHC Bus Gating Reset Register  */
         RESERVED(0x850[0x090C - 0x0850], uint8_t)
    __IO uint32_t UART_BGR_REG;                       /*!< Offset 0x90C UART Bus Gating Reset Register  */
         RESERVED(0x910[0x091C - 0x0910], uint8_t)
    __IO uint32_t TWI_BGR_REG;                        /*!< Offset 0x91C TWI Bus Gating Reset Register  */
         RESERVED(0x920[0x0940 - 0x0920], uint8_t)
    __IO uint32_t SPI0_CLK_REG;                       /*!< Offset 0x940 SPI0 Clock Register  */
    __IO uint32_t SPI1_CLK_REG;                       /*!< Offset 0x944 SPI1 Clock Register  */
    __IO uint32_t SPI2_CLK_REG;                       /*!< Offset 0x948 SPI2 Clock Register  */
         RESERVED(0x94C[0x096C - 0x094C], uint8_t)
    __IO uint32_t SPI_BGR_REG;                        /*!< Offset 0x96C SPI Bus Gating Reset Register  */
    __IO uint32_t EMAC0_25M_CLK_REG;                  /*!< Offset 0x970 EMAC0_25M Clock Register  */
         RESERVED(0x974[0x097C - 0x0974], uint8_t)
    __IO uint32_t EMAC_BGR_REG;                       /*!< Offset 0x97C EMAC Bus Gating Reset Register  */
         RESERVED(0x980[0x0990 - 0x0980], uint8_t)
    __IO uint32_t IRRX_CLK_REG;                       /*!< Offset 0x990 IRRX Clock Register  */
         RESERVED(0x994[0x099C - 0x0994], uint8_t)
    __IO uint32_t IRRX_BGR_REG;                       /*!< Offset 0x99C IRRX Bus Gating Reset Register  */
         RESERVED(0x9A0[0x09C0 - 0x09A0], uint8_t)
    __IO uint32_t IRTX_CLK_REG;                       /*!< Offset 0x9C0 IRTX Clock Register  */
         RESERVED(0x9C4[0x09CC - 0x09C4], uint8_t)
    __IO uint32_t IRTX_BGR_REG;                       /*!< Offset 0x9CC IRTX Bus Gating Reset Register  */
         RESERVED(0x9D0[0x09EC - 0x09D0], uint8_t)
    __IO uint32_t GPADC_BGR_REG;                      /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
         RESERVED(0x9F0[0x09FC - 0x09F0], uint8_t)
    __IO uint32_t THS_BGR_REG;                        /*!< Offset 0x9FC THS Bus Gating Reset Register  */
         RESERVED(0xA00[0x0A10 - 0x0A00], uint8_t)
    __IO uint32_t I2SPCM0_CLK_REG;                    /*!< Offset 0xA10 I2S/PCM0 Clock Register  */
    __IO uint32_t I2SPCM1_CLK_REG;                    /*!< Offset 0xA14 I2S/PCM1 Clock Register  */
    __IO uint32_t I2SPCM2_CLK_REG;                    /*!< Offset 0xA18 I2S/PCM2 Clock Register  */
    __IO uint32_t I2SPCM3_CLK_REG;                    /*!< Offset 0xA1C I2S/PCM3 Clock Register  */
    __IO uint32_t I2SPCM_BGR_REG;                     /*!< Offset 0xA20 I2S/PCM Bus Gating Reset Register  */
    __IO uint32_t OWA_CLK_REG;                        /*!< Offset 0xA24 OWA Clock Register  */
         RESERVED(0xA28[0x0A2C - 0x0A28], uint8_t)
    __IO uint32_t OWA_BGR_REG;                        /*!< Offset 0xA2C OWA Bus Gating Reset Register  */
         RESERVED(0xA30[0x0A40 - 0x0A30], uint8_t)
    __IO uint32_t DMIC_CLK_REG;                       /*!< Offset 0xA40 DMIC Clock Register  */
         RESERVED(0xA44[0x0A4C - 0x0A44], uint8_t)
    __IO uint32_t DMIC_BGR_REG;                       /*!< Offset 0xA4C DMIC Bus Gating Reset Register  */
    __IO uint32_t AUDIO_CODEC_DAC_1X_CLK_REG;         /*!< Offset 0xA50 AUDIO CODEC DAC 1X Clock Register  */
    __IO uint32_t AUDIO_CODEC_ADC_1X_CLK_REG;         /*!< Offset 0xA54 AUDIO CODEC ADC 1X Clock Register  */
    __IO uint32_t AUDIO_CODEC_4X_CLK_REG;             /*!< Offset 0xA58 AUDIO CODEC 4X Clock Register  */
    __IO uint32_t AUDIO_CODEC_BGR_REG;                /*!< Offset 0xA5C AUDIO CODEC Bus Gating Reset Register  */
         RESERVED(0xA60[0x0A70 - 0x0A60], uint8_t)
    __IO uint32_t USB0_CLK_REG;                       /*!< Offset 0xA70 USB0 Clock Register  */
    __IO uint32_t USB1_CLK_REG;                       /*!< Offset 0xA74 USB1 Clock Register */
         RESERVED(0xA78[0x0A8C - 0x0A78], uint8_t)
    __IO uint32_t USB_BGR_REG;                        /*!< Offset 0xA8C USB Bus Gating Reset Register  */
         RESERVED(0xA90[0x0A9C - 0x0A90], uint8_t)
    __IO uint32_t LRADC_BGR_REG;                      /*!< Offset 0xA9C LRADC Bus Gating Reset Register */
         RESERVED(0xAA0[0x0ABC - 0x0AA0], uint8_t)
    __IO uint32_t DPSS_TOP0_BGR_REG;                  /*!< Offset 0xABC DPSS_TOP0 Bus Gating Reset Register  */
         RESERVED(0xAC0[0x0ACC - 0x0AC0], uint8_t)
    __IO uint32_t DPSS_TOP1_BGR_REG;                  /*!< Offset 0xACC DPSS_TOP1 Bus Gating Reset Register  */
         RESERVED(0xAD0[0x0B24 - 0x0AD0], uint8_t)
    __IO uint32_t MIPI_DSI_HOST0_CLK_REG;             /*!< Offset 0xB24 MIPI DSI Host0 Clock Register  */
         RESERVED(0xB28[0x0B4C - 0x0B28], uint8_t)
    __IO uint32_t MIPI_BGR_REG;                       /*!< Offset 0xB4C MIPI DSI Bus Gating Reset Register  */
         RESERVED(0xB50[0x0B5C - 0x0B50], uint8_t)
    __IO uint32_t DISPLAY_IF_TOP_BGR_REG;             /*!< Offset 0xB5C DISPLAY_IF_TOP Bus Gating Reset Register  */
    __IO uint32_t TCON_LCD0_CLK_REG;                  /*!< Offset 0xB60 TCON LCD0 Clock Register  */
         RESERVED(0xB64[0x0B7C - 0x0B64], uint8_t)
    __IO uint32_t TCON_LCD_BGR_REG;                   /*!< Offset 0xB7C TCON LCD Bus Gating Reset Register  */
         RESERVED(0xB80[0x0BAC - 0x0B80], uint8_t)
    __IO uint32_t LVDS_BGR_REG;                       /*!< Offset 0xBAC LVDS Bus Gating Reset Register  */
         RESERVED(0xBB0[0x0BF0 - 0x0BB0], uint8_t)
    __IO uint32_t LEDC_CLK_REG;                       /*!< Offset 0xBF0 LEDC Clock Register  */
         RESERVED(0xBF4[0x0BFC - 0x0BF4], uint8_t)
    __IO uint32_t LEDC_BGR_REG;                       /*!< Offset 0xBFC LEDC Bus Gating Reset Register  */
         RESERVED(0xC00[0x0C04 - 0x0C00], uint8_t)
    __IO uint32_t CSI0_TOP_CLK_REG;                   /*!< Offset 0xC04 CSI0 TOP Clock Register  */
    __IO uint32_t CSI0_0_MST_CLK_REG;                 /*!< Offset 0xC08 CSI0_0 Master Clock Register  */
    __IO uint32_t CSI0_1_MST_CLK_REG;                 /*!< Offset 0xC0C CSI0_1 Master Clock Register  */
         RESERVED(0xC10[0x0C1C - 0x0C10], uint8_t)
    __IO uint32_t CSI_BGR_REG;                        /*!< Offset 0xC1C CSI Bus Gating Reset Register  */
    __IO uint32_t CSI_ISP_CLK_REG;                    /*!< Offset 0xC20 CSI ISP Clock Register  */
         RESERVED(0xC24[0x0C2C - 0x0C24], uint8_t)
    __IO uint32_t CSI_ISP_BGR_REG;                    /*!< Offset 0xC2C CSI ISP Bus Gating Reset Register  */
         RESERVED(0xC30[0x0F00 - 0x0C30], uint8_t)
    __IO uint32_t CCU_SEC_SWITCH_REG;                 /*!< Offset 0xF00 CCU Security Switch Register  */
    __IO uint32_t PLL_LOCK_DBG_CTRL_REG;              /*!< Offset 0xF04 PLL Lock Debug Control Register  */
         RESERVED(0xF08[0x0F20 - 0x0F08], uint8_t)
    __IO uint32_t PLL_CPUX_HW_FM_REG;                 /*!< Offset 0xF20 PLL_CPUX Hardware FM Register */
} CCU_TypeDef; /* size of structure = 0xF24 */
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
    __IO uint32_t DRV [0x002];                        /*!< Offset 0x014 Multi_Driving Register */
    __IO uint32_t PULL [0x002];                       /*!< Offset 0x01C Pull Register */
} GPIO_TypeDef; /* size of structure = 0x024 */
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
        __IO uint32_t DRV [0x002];                    /*!< Offset 0x014 Multi_Driving Register */
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x009];                              /*!< Offset 0x000 GPIO pin control */
         RESERVED(0x144[0x0200 - 0x0144], uint8_t)
    struct
    {
        __IO uint32_t EINT_CFG [0x004];               /*!< Offset 0x200 External Interrupt Configure Registers */
        __IO uint32_t EINT_CTL;                       /*!< Offset 0x210 External Interrupt Control Register */
        __IO uint32_t EINT_STATUS;                    /*!< Offset 0x214 External Interrupt Status Register */
        __IO uint32_t EINT_DEB;                       /*!< Offset 0x218 External Interrupt Debounce Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } GPIO_INTS [0x009];                              /*!< Offset 0x200 GPIO interrupt control */
         RESERVED(0x320[0x0340 - 0x0320], uint8_t)
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
/*!< USBPHYC HCI Contgroller and PHY Interface Description */
typedef struct USBPHYC_Type
{
    __IO uint32_t USB_CTRL;                           /*!< Offset 0x000 HCI Interface Register - REG_ISCR 0x00 */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t HCI_CTRL3;                          /*!< Offset 0x008 HCI Control 3 Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t PHY_CTRL;                           /*!< Offset 0x010 PHY Control Register  */
         RESERVED(0x014[0x001C - 0x0014], uint8_t)
    __IO uint32_t HSIC_PHY_tune1;                     /*!< Offset 0x01C HSIC PHY Tune1 Register  */
    __IO uint32_t HSIC_PHY_tune2;                     /*!< Offset 0x020 HSIC PHY Tune2 Register - REG_PHY_OTGCTL 0x20 */
    __IO uint32_t HSIC_PHY_tune3;                     /*!< Offset 0x024 HSIC PHY Tune3 Register  */
    __IO uint32_t USB_SPDCR;                          /*!< Offset 0x028 HCI SIE Port Disable Control Register  */
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


/* Defines */



/* Access pointers */

#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L Port Controller register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU  register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA Port Controller register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK Port Controller register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB Port Controller register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Port Controller register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Port Controller register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE Port Controller register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF Port Controller register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG Port Controller register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)           /*!< GPIOH Port Controller register set access pointer */
#define GPIOI ((GPIO_TypeDef *) GPIOI_BASE)           /*!< GPIOI Port Controller register set access pointer */
#define GPIOINTA ((GPIOINT_TypeDef *) GPIOINTA_BASE)  /*!< GPIOINTA  register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)  /*!< GPIOINTH  register set access pointer */
#define GPIOINTI ((GPIOINT_TypeDef *) GPIOINTI_BASE)  /*!< GPIOINTI  register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3  register set access pointer */
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
#define R_WDG ((TIMER_TypeDef *) R_WDG_BASE)          /*!< R_WDG  register set access pointer */
#define R_TWDG ((TIMER_TypeDef *) R_TWDG_BASE)        /*!< R_TWDG  register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL Port Controller register set access pointer */
#define R_PIO ((GPIO_TypeDef *) R_PIO_BASE)           /*!< R_PIO Port Controller register set access pointer */
#define R_TIMER ((TIMER_TypeDef *) R_TIMER_BASE)      /*!< R_TIMER  register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define S_TWI0 ((TWI_TypeDef *) S_TWI0_BASE)          /*!< S_TWI0  register set access pointer */
#define S_TWI1 ((TWI_TypeDef *) S_TWI1_BASE)          /*!< S_TWI1  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__9E291677_B6C7_4f4a_BBFD_D0F40B363F83__INCLUDED */
