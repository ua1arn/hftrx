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
#define DMA1_BASE ((uintptr_t) 0x04024000)            /*!< DMAC  Base */
#define USB0_DEVICE_BASE ((uintptr_t) 0x04100000)     /*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB0_EHCI_BASE ((uintptr_t) 0x04101000)       /*!< USB_EHCI_Capability  Base */
#define USB1_EHCI_BASE ((uintptr_t) 0x04201000)       /*!< USB_EHCI_Capability  Base */
#define USB1_OHCI_BASE ((uintptr_t) 0x04201400)       /*!< USB_OHCI_Capability  Base */
#define DMA0_BASE ((uintptr_t) 0x04601000)            /*!< DMAC  Base */
#define USB0_OHCI_BASE ((uintptr_t) 0x05101400)       /*!< USB_OHCI_Capability  Base */
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
#define DMA1 ((DMAC_TypeDef *) DMA1_BASE)             /*!< DMA1  register set access pointer */
#define USB0_DEVICE ((USBOTG_TypeDef *) USB0_DEVICE_BASE)/*!< USB0_DEVICE USB OTG Dual-Role Device controller register set access pointer */
#define USB0_EHCI ((USB_EHCI_Capability_TypeDef *) USB0_EHCI_BASE)/*!< USB0_EHCI  register set access pointer */
#define USB1_EHCI ((USB_EHCI_Capability_TypeDef *) USB1_EHCI_BASE)/*!< USB1_EHCI  register set access pointer */
#define USB1_OHCI ((USB_OHCI_Capability_TypeDef *) USB1_OHCI_BASE)/*!< USB1_OHCI  register set access pointer */
#define DMA0 ((DMAC_TypeDef *) DMA0_BASE)             /*!< DMA0  register set access pointer */
#define USB0_OHCI ((USB_OHCI_Capability_TypeDef *) USB0_OHCI_BASE)/*!< USB0_OHCI  register set access pointer */
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
