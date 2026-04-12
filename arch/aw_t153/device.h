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
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE GICC GIC CPU Interface */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE GICC GIC CPU Interface */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE GICC GIC CPU Interface */
    Legacy_nFIQ_IRQn = 28,                            /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE GICC GIC CPU Interface */
    Legacy_nIRQ_IRQn = 31,                            /*!< GIC_INTERFACE GICC GIC CPU Interface */
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
    GMAC0_TOP_IRQn = 78,                              /*!< GMAC_TOP  */
    GMAC1_TOP_IRQn = 79,                              /*!< GMAC_TOP  */
    GMAC0_PWR_CLK_CTRL_IRQn = 124,                    /*!< GMAC_IP  */
    GMAC0_PERCH_TX_IRQn = 125,                        /*!< GMAC_IP  */
    GMAC0_PERCH_RX_IRQn = 126,                        /*!< GMAC_IP  */
    GMAC1_PWR_CLK_CTRL_IRQn = 127,                    /*!< GMAC_IP  */
    GMAC1_PERCH_TX_IRQn = 128,                        /*!< GMAC_IP  */
    GMAC1_PERCH_RX_IRQn = 129,                        /*!< GMAC_IP  */
    GMAC2_PWR_CLK_CTRL_IRQn = 130,                    /*!< GMAC_IP  */
    GMAC2_PERCH_TX_IRQn = 131,                        /*!< GMAC_IP  */
    GMAC2_PERCH_RX_IRQn = 132,                        /*!< GMAC_IP  */
    GMAC2_TOP_IRQn = 133,                             /*!< GMAC_TOP  */
    UART8_IRQn = 180,                                 /*!< UART  */
    UART9_IRQn = 181,                                 /*!< UART  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define CCU_BASE ((uintptr_t) 0x02002000)             /*!< CCU Clock Controller Unit (CCU) Base */
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
#define SID_BASE ((uintptr_t) 0x03006000)             /*!< SID Security ID Base */
#define GIC_BASE ((uintptr_t) 0x03400000)             /*!< GIC  Base */
#define GIC_BASE ((uintptr_t) 0x03400000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03400000) /*!< GIC_DISTRIBUTOR GIC DISTRIBUTOR; GICD Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03401000)   /*!< GIC_INTERFACE GICC GIC CPU Interface Base */
#define GPIOA_BASE ((uintptr_t) 0x03604080)           /*!< GPIO Port Controller Base */
#define GPIOB_BASE ((uintptr_t) 0x03604100)           /*!< GPIO Port Controller Base */
#define GPIOC_BASE ((uintptr_t) 0x03604180)           /*!< GPIO Port Controller Base */
#define GPIOD_BASE ((uintptr_t) 0x03604200)           /*!< GPIO Port Controller Base */
#define GPIOE_BASE ((uintptr_t) 0x03604280)           /*!< GPIO Port Controller Base */
#define GPIOF_BASE ((uintptr_t) 0x03604300)           /*!< GPIO Port Controller Base */
#define GPIOG_BASE ((uintptr_t) 0x03604380)           /*!< GPIO Port Controller Base */
#define GPIOJ_BASE ((uintptr_t) 0x03604500)           /*!< GPIO Port Controller Base */
#define GPIOK_BASE ((uintptr_t) 0x03604580)           /*!< GPIO Port Controller Base */
#define SPI0_BASE ((uintptr_t) 0x04025000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x04026000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI2_BASE ((uintptr_t) 0x04027000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI3_BASE ((uintptr_t) 0x04028000)            /*!< SPI Serial Peripheral Interface Base */
#define GAMC0_IP_BASE ((uintptr_t) 0x04500000)        /*!< GMAC_IP  Base */
#define GAMC0_TOP_BASE ((uintptr_t) 0x04508000)       /*!< GMAC_TOP  Base */
#define GAMC1_IP_BASE ((uintptr_t) 0x04510000)        /*!< GMAC_IP  Base */
#define GAMC1_TOP_BASE ((uintptr_t) 0x04518000)       /*!< GMAC_TOP  Base */
#define GAMC2_IP_BASE ((uintptr_t) 0x04520000)        /*!< GMAC_IP  Base */
#define GAMC2_TOP_BASE ((uintptr_t) 0x04528000)       /*!< GMAC_TOP  Base */
#define PRCM_BASE ((uintptr_t) 0x07010000)            /*!< PRCM  Base */

#include <core_ca.h>

/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
         RESERVED(0x000[0x00A0 - 0x0000], uint8_t)
    __IOM uint32_t PLL_PERI0_CTRL_REG;                /*!< Offset 0x0A0 PLL_PERI0 Control Register */
         RESERVED(0x0A4[0x00A8 - 0x00A4], uint8_t)
    __IOM uint32_t PLL_PERI0_PAT0_CTRL_REG;           /*!< Offset 0x0A8 PLL_PERI0 Pattern0 Control Register */
    __IOM uint32_t PLL_PERI0_PAT1_CTRL_REG;           /*!< Offset 0x0AC PLL_PERI0 Pattern1 Control Register */
    __IOM uint32_t PLL_PERI0_BIAS_REG;                /*!< Offset 0x0B0 PLL_PERI0 Bias Register */
         RESERVED(0x0B4[0x00C0 - 0x00B4], uint8_t)
    __IOM uint32_t PLL_PERI1_CTRL_REG;                /*!< Offset 0x0C0 PLL_PERI1 Control Register */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IOM uint32_t PLL_PERI1_PAT0_CTRL_REG;           /*!< Offset 0x0C8 PLL_PERI1 Pattern0 Control Register */
    __IOM uint32_t PLL_PERI1_PAT1_CTRL_REG;           /*!< Offset 0x0CC PLL_PERI1 Pattern1 Control Register */
    __IOM uint32_t PLL_PERI1_BIAS_REG;                /*!< Offset 0x0D0 PLL_PERI1 Bias Register */
         RESERVED(0x0D4[0x0120 - 0x00D4], uint8_t)
    __IOM uint32_t PLL_VIDEO0_CTRL_REG;               /*!< Offset 0x120 PLL_VIDEO0 Control Register */
         RESERVED(0x124[0x0128 - 0x0124], uint8_t)
    __IOM uint32_t PLL_VIDEO0_PAT0_CTRL_REG;          /*!< Offset 0x128 PLL_VIDEO0 Pattern0 Control Register */
    __IOM uint32_t PLL_VIDEO0_PAT1_CTRL_REG;          /*!< Offset 0x12C PLL_VIDEO0 Pattern1 Control Register */
    __IOM uint32_t PLL_VIDEO0_BIAS_REG;               /*!< Offset 0x130 PLL_VIDEO0 Bias Register */
         RESERVED(0x134[0x0260 - 0x0134], uint8_t)
    __IOM uint32_t PLL_AUDIO0_CTRL_REG;               /*!< Offset 0x260 PLL_AUDIO0 Control Register */
         RESERVED(0x264[0x0268 - 0x0264], uint8_t)
    __IOM uint32_t PLL_AUDIO0_PAT0_CTRL_REG;          /*!< Offset 0x268 PLL_AUDIO0 Pattern0 Control Register */
    __IOM uint32_t PLL_AUDIO0_PAT1_CTRL_REG;          /*!< Offset 0x26C PLL_AUDIO0 Pattern1 Control Register */
    __IOM uint32_t PLL_AUDIO0_BIAS_REG;               /*!< Offset 0x270 PLL_AUDIO0 Bias Register */
         RESERVED(0x274[0x0340 - 0x0274], uint8_t)
    __IOM uint32_t PLL_CPU_CTRL_REG;                  /*!< Offset 0x340 PLL_CPU Control Register */
    __IOM uint32_t PLL_CPU_PAT0_CTRL_REG;             /*!< Offset 0x344 PLL_CPU Pattern0 Control Register */
    __IOM uint32_t PLL_CPU_PAT1_CTRL_REG;             /*!< Offset 0x348 PLL_CPU Pattern1 Control Register */
    __IOM uint32_t PLL_CPU_BIAS_REG;                  /*!< Offset 0x34C PLL_CPU Bias Register */
    __IOM uint32_t PLL_CPU_TUN1_REG;                  /*!< Offset 0x350 PLL_CPU Tuning1 Control Register */
    __IOM uint32_t PLL_CPU_SSC_REG;                   /*!< Offset 0x354 PLL_CPU SSC Register */
    __IOM uint32_t PLL_CPU_ECHO_REG;                  /*!< Offset 0x358 PLL_CPU Echo Register */
         RESERVED(0x35C[0x0500 - 0x035C], uint8_t)
    __IOM uint32_t AHB_CLK_REG;                       /*!< Offset 0x500 AHB Clock Register */
         RESERVED(0x504[0x0510 - 0x0504], uint8_t)
    __IOM uint32_t APB0_CLK_REG;                      /*!< Offset 0x510 APB0 Clock Register */
         RESERVED(0x514[0x0518 - 0x0514], uint8_t)
    __IOM uint32_t APB1_CLK_REG;                      /*!< Offset 0x518 APB1 Clock Register */
         RESERVED(0x51C[0x0538 - 0x051C], uint8_t)
    __IOM uint32_t APB_UART_CLK_REG;                  /*!< Offset 0x538 APB_UART Clock Register */
         RESERVED(0x53C[0x0588 - 0x053C], uint8_t)
    __IOM uint32_t MBUS_CLK_REG;                      /*!< Offset 0x588 MBUS Clock Register */
         RESERVED(0x58C[0x05C0 - 0x058C], uint8_t)
    __IOM uint32_t AHB_MAT_CLK_GATE_EN_REG;           /*!< Offset 0x5C0 AHB Master Clock Gate Enable Register */
         RESERVED(0x5C4[0x05D0 - 0x05C4], uint8_t)
    __IOM uint32_t PERI_MAT_CLK_GATE_EN_REG;          /*!< Offset 0x5D0 PERI Master Clock Gate Enable Register */
         RESERVED(0x5D4[0x05E0 - 0x05D4], uint8_t)
    __IOM uint32_t MBUS_CLK_GATE_EN_REG;              /*!< Offset 0x5E0 MBUS Clock Gate Enable Register */
    __IOM uint32_t MBUS_MAT_CLK_GATE_EN_REG;          /*!< Offset 0x5E4 MBUS Master Clock Gate Enable Register */
         RESERVED(0x5E8[0x05F0 - 0x05E8], uint8_t)
    __IOM uint32_t AHB_MAT_CLK_AUTO_GATE_EN_REG;      /*!< Offset 0x5F0 AHB Master Clock Auto Gate Enable Register */
    __IOM uint32_t MBUS_MAT_CLK_AUTO_GATE_EN_REG;     /*!< Offset 0x5F4 MBUS Master Clock Auto Gate Enable Register */
    __IOM uint32_t AHB_MAT_CLK_GATE_STAT_REG;         /*!< Offset 0x5F8 AHB Master Clock Gate Status Register */
    __IOM uint32_t MBUS_MAT_CLK_GATE_STAT_REG;        /*!< Offset 0x5FC MBUS Master Clock Gate Status Register */
         RESERVED(0x600[0x0704 - 0x0600], uint8_t)
    __IOM uint32_t DMA0_GAR_REG;                      /*!< Offset 0x704 DMA0 Gating And Reset Register */
         RESERVED(0x708[0x070C - 0x0708], uint8_t)
    __IOM uint32_t DMA1_GAR_REG;                      /*!< Offset 0x70C DMA1 Gating And Reset Register */
         RESERVED(0x710[0x0724 - 0x0710], uint8_t)
    __IOM uint32_t SPINLOCK_GAR_REG;                  /*!< Offset 0x724 SPINLOCK Gating And Reset Register */
         RESERVED(0x728[0x0744 - 0x0728], uint8_t)
    __IOM uint32_t MSGBOX_CPUX_GAR_REG;               /*!< Offset 0x744 MSGBOX_CPUX Gating And Reset Register */
         RESERVED(0x748[0x074C - 0x0748], uint8_t)
    __IOM uint32_t MSGBOX_CORE0_GAR_REG;              /*!< Offset 0x74C MSGBOX_CORE0 Gating And Reset Register */
         RESERVED(0x750[0x0754 - 0x0750], uint8_t)
    __IOM uint32_t MSGBOX_CORE1_GAR_REG;              /*!< Offset 0x754 MSGBOX_CORE1 Gating And Reset Register */
         RESERVED(0x758[0x075C - 0x0758], uint8_t)
    __IOM uint32_t MSGBOX_CORE2_GAR_REG;              /*!< Offset 0x75C MSGBOX_CORE2 Gating And Reset Register */
         RESERVED(0x760[0x0764 - 0x0760], uint8_t)
    __IOM uint32_t MSGBOX_CORE3_GAR_REG;              /*!< Offset 0x764 MSGBOX_CORE3 Gating And Reset Register */
         RESERVED(0x768[0x076C - 0x0768], uint8_t)
    __IOM uint32_t MSGBOX_MCU_GAR_REG;                /*!< Offset 0x76C MSGBOX_MCU Gating And Reset Register */
         RESERVED(0x770[0x0794 - 0x0770], uint8_t)
    __IOM uint32_t PWM2_GAR_REG;                      /*!< Offset 0x794 PWM2 Gating And Reset Register */
         RESERVED(0x798[0x07A4 - 0x0798], uint8_t)
    __IOM uint32_t DCU_GAR_REG;                       /*!< Offset 0x7A4 DCU Gating And Reset Register */
         RESERVED(0x7A8[0x07AC - 0x07A8], uint8_t)
    __IOM uint32_t DAP_GAR_REG;                       /*!< Offset 0x7AC DAP Gating And Reset Register */
         RESERVED(0x7B0[0x07C0 - 0x07B0], uint8_t)
    __IOM uint32_t PWMCS0_CLK_REG;                    /*!< Offset 0x7C0 PWMCS0 Clock Register */
    __IOM uint32_t PWMCS0_GAR_REG;                    /*!< Offset 0x7C4 PWMCS0 Gating And Reset Register */
    __IOM uint32_t PWMCS1_CLK_REG;                    /*!< Offset 0x7C8 PWMCS1 Clock Register */
    __IOM uint32_t PWMCS1_GAR_REG;                    /*!< Offset 0x7CC PWMCS1 Gating And Reset Register */
         RESERVED(0x7D0[0x0800 - 0x07D0], uint8_t)
    __IOM uint32_t TIMER0_CPUX_0_CLK_REG;             /*!< Offset 0x800 TIMER0_CPUX_0 Clock Register */
    __IOM uint32_t TIMER0_CPUX_1_CLK_REG;             /*!< Offset 0x804 TIMER0_CPUX_1 Clock Register */
    __IOM uint32_t TIMER0_CPUX_2_CLK_REG;             /*!< Offset 0x808 TIMER0_CPUX_2 Clock Register */
    __IOM uint32_t TIMER0_CPUX_3_CLK_REG;             /*!< Offset 0x80C TIMER0_CPUX_3 Clock Register */
    __IOM uint32_t TIMER0_CPUX_4_CLK_REG;             /*!< Offset 0x810 TIMER0_CPUX_4 Clock Register */
    __IOM uint32_t TIMER0_CPUX_5_CLK_REG;             /*!< Offset 0x814 TIMER0_CPUX_5 Clock Register */
    __IOM uint32_t TIMER0_CPUX_6_CLK_REG;             /*!< Offset 0x818 TIMER0_CPUX_6 Clock Register */
    __IOM uint32_t TIMER0_CPUX_7_CLK_REG;             /*!< Offset 0x81C TIMER0_CPUX_7 Clock Register */
         RESERVED(0x820[0x0850 - 0x0820], uint8_t)
    __IOM uint32_t TIMER0_CPUX_GAR_REG;               /*!< Offset 0x850 TIMER0_CPUX Gating And Reset Register */
         RESERVED(0x854[0x0860 - 0x0854], uint8_t)
    __IOM uint32_t TIEMR0_MCU_0_CLK_REG;              /*!< Offset 0x860 TIEMR0_MCU_0 Clock Register */
    __IOM uint32_t TIEMR0_MCU_1_CLK_REG;              /*!< Offset 0x864 TIEMR0_MCU_1 Clock Register */
    __IOM uint32_t TIEMR0_MCU_2_CLK_REG;              /*!< Offset 0x868 TIEMR0_MCU_2 Clock Register */
    __IOM uint32_t TIEMR0_MCU_3_CLK_REG;              /*!< Offset 0x86C TIEMR0_MCU_3 Clock Register */
    __IOM uint32_t TIMER0_MCU_GAR_REG;                /*!< Offset 0x870 TIMER0_MCU Gating And Reset Register */
         RESERVED(0x874[0x0A00 - 0x0874], uint8_t)
    __IOM uint32_t DE0_CLK_REG;                       /*!< Offset 0xA00 DE0 Clock Register */
    __IOM uint32_t DE0_GAR_REG;                       /*!< Offset 0xA04 DE0 Gating And Reset Register */
         RESERVED(0xA08[0x0A40 - 0x0A08], uint8_t)
    __IOM uint32_t G2D_CLK_REG;                       /*!< Offset 0xA40 G2D Clock Register */
    __IOM uint32_t G2D_GAR_REG;                       /*!< Offset 0xA44 G2D Gating And Reset Register */
         RESERVED(0xA48[0x0AC0 - 0x0A48], uint8_t)
    __IOM uint32_t CE_SYS_CLK_REG;                    /*!< Offset 0xAC0 CE_SYS Clock Register */
    __IOM uint32_t CE_SYS_GAR_REG;                    /*!< Offset 0xAC4 CE_SYS Gating And Reset Register */
         RESERVED(0xAC8[0x0B80 - 0x0AC8], uint8_t)
    __IOM uint32_t MCU_CORE_CLK_REG;                  /*!< Offset 0xB80 MCU_CORE Clock Register */
         RESERVED(0xB84[0x0B88 - 0x0B84], uint8_t)
    __IOM uint32_t MCU_TS_CLK_REG;                    /*!< Offset 0xB88 MCU_TS Clock Register */
         RESERVED(0xB8C[0x0B94 - 0x0B8C], uint8_t)
    __IOM uint32_t MCU_SYS_GAR_REG;                   /*!< Offset 0xB94 MCU_SYS Gating And Reset Register */
         RESERVED(0xB98[0x0B9C - 0x0B98], uint8_t)
    __IOM uint32_t MCU_CFG_GAR_REG;                   /*!< Offset 0xB9C MCU_CFG Gating And Reset Register */
         RESERVED(0xBA0[0x0C0C - 0x0BA0], uint8_t)
    __IOM uint32_t DRAMC_GAR_REG;                     /*!< Offset 0xC0C DRAMC Gating And Reset Register */
         RESERVED(0xC10[0x0D00 - 0x0C10], uint8_t)
    __IOM uint32_t SMHC0_CLK_REG;                     /*!< Offset 0xD00 SMHC0 Clock Register */
         RESERVED(0xD04[0x0D0C - 0x0D04], uint8_t)
    __IOM uint32_t SMHC0_GAR_REG;                     /*!< Offset 0xD0C SMHC0 Gating And Reset Register */
    __IOM uint32_t SMHC1_CLK_REG;                     /*!< Offset 0xD10 SMHC1 Clock Register */
         RESERVED(0xD14[0x0D1C - 0x0D14], uint8_t)
    __IOM uint32_t SMHC1_GAR_REG;                     /*!< Offset 0xD1C SMHC1 Gating And Reset Register */
    __IOM uint32_t SMHC2_CLK_REG;                     /*!< Offset 0xD20 SMHC2 Clock Register */
         RESERVED(0xD24[0x0D2C - 0x0D24], uint8_t)
    __IOM uint32_t SMHC2_GAR_REG;                     /*!< Offset 0xD2C SMHC2 Gating And Reset Register */
    __IOM uint32_t SMHC3_CLK_REG;                     /*!< Offset 0xD30 SMHC3 Clock Register */
         RESERVED(0xD34[0x0D3C - 0x0D34], uint8_t)
    __IOM uint32_t SMHC3_GAR_REG;                     /*!< Offset 0xD3C SMHC3 Gating And Reset Register */
         RESERVED(0xD40[0x0E00 - 0x0D40], uint8_t)
    __IOM uint32_t UART0_GAR_REG;                     /*!< Offset 0xE00 UART0 Gating And Reset Register */
    __IOM uint32_t UART1_GAR_REG;                     /*!< Offset 0xE04 UART1 Gating And Reset Register */
    __IOM uint32_t UART2_GAR_REG;                     /*!< Offset 0xE08 UART2 Gating And Reset Register */
    __IOM uint32_t UART3_GAR_REG;                     /*!< Offset 0xE0C UART3 Gating And Reset Register */
    __IOM uint32_t UART4_GAR_REG;                     /*!< Offset 0xE10 UART4 Gating And Reset Register */
    __IOM uint32_t UART5_GAR_REG;                     /*!< Offset 0xE14 UART5 Gating And Reset Register */
    __IOM uint32_t UART6_GAR_REG;                     /*!< Offset 0xE18 UART6 Gating And Reset Register */
         RESERVED(0xE1C[0x0E20 - 0x0E1C], uint8_t)
    __IOM uint32_t UART7_GAR_REG;                     /*!< Offset 0xE20 UART7 Gating And Reset Register */
    __IOM uint32_t UART8_GAR_REG;                     /*!< Offset 0xE24 UART8 Gating And Reset Register */
    __IOM uint32_t UART9_GAR_REG;                     /*!< Offset 0xE28 UART9 Gating And Reset Register */
         RESERVED(0xE2C[0x0E80 - 0x0E2C], uint8_t)
    __IOM uint32_t TWI0_GAR_REG;                      /*!< Offset 0xE80 TWI0 Gating And Reset Register */
    __IOM uint32_t TWI1_GAR_REG;                      /*!< Offset 0xE84 TWI1 Gating And Reset Register */
    __IOM uint32_t TWI2_GAR_REG;                      /*!< Offset 0xE88 TWI2 Gating And Reset Register */
    __IOM uint32_t TWI3_GAR_REG;                      /*!< Offset 0xE8C TWI3 Gating And Reset Register */
    __IOM uint32_t TWI4_GAR_REG;                      /*!< Offset 0xE90 TWI4 Gating And Reset Register */
    __IOM uint32_t TWI5_GAR_REG;                      /*!< Offset 0xE94 TWI5 Gating And Reset Register */
         RESERVED(0xE98[0x0F00 - 0x0E98], uint8_t)
    __IOM uint32_t SPI0_CLK_REG;                      /*!< Offset 0xF00 SPI0 Clock Register */
    __IOM uint32_t SPI0_GAR_REG;                      /*!< Offset 0xF04 SPI0 Gating And Reset Register */
    __IOM uint32_t SPI1_CLK_REG;                      /*!< Offset 0xF08 SPI1 Clock Register */
    __IOM uint32_t SPI1_GAR_REG;                      /*!< Offset 0xF0C SPI1 Gating And Reset Register */
    __IOM uint32_t SPI2_CLK_REG;                      /*!< Offset 0xF10 SPI2 Clock Register */
    __IOM uint32_t SPI2_GAR_REG;                      /*!< Offset 0xF14 SPI2 Gating And Reset Register */
    __IOM uint32_t SPIF_CLK_REG;                      /*!< Offset 0xF18 SPIF Clock Register */
    __IOM uint32_t SPIF_GAR_REG;                      /*!< Offset 0xF1C SPIF Gating And Reset Register */
    __IOM uint32_t SPI3_CLK_REG;                      /*!< Offset 0xF20 SPI3 Clock Register */
    __IOM uint32_t SPI3_GAR_REG;                      /*!< Offset 0xF24 SPI3 Gating And Reset Register */
         RESERVED(0xF28[0x0F80 - 0x0F28], uint8_t)
    __IOM uint32_t CAN0_CLK_REG;                      /*!< Offset 0xF80 CAN0 Clock Register */
    __IOM uint32_t CAN0_GAR_REG;                      /*!< Offset 0xF84 CAN0 Gating And Reset Register */
    __IOM uint32_t CAN1_CLK_REG;                      /*!< Offset 0xF88 CAN1 Clock Register */
    __IOM uint32_t CAN1_GAR_REG;                      /*!< Offset 0xF8C CAN1 Gating And Reset Register */
         RESERVED(0xF90[0x0FBC - 0x0F90], uint8_t)
    __IOM uint32_t CAN_SYS_GAR_REG;                   /*!< Offset 0xFBC CAN_SYS Gating And Reset Register */
    __IOM uint32_t GPADC0_CLK_REG;                    /*!< Offset 0xFC0 GPADC0 Clock Register */
    __IOM uint32_t GPADC0_GAR_REG;                    /*!< Offset 0xFC4 GPADC0 Gating And Reset Register */
    __IOM uint32_t GPADC1_CLK_REG;                    /*!< Offset 0xFC8 GPADC1 Clock Register */
    __IOM uint32_t GPADC1_GAR_REG;                    /*!< Offset 0xFCC GPADC1 Gating And Reset Register */
    __IOM uint32_t GPADC2_CLK_REG;                    /*!< Offset 0xFD0 GPADC2 Clock Register */
    __IOM uint32_t GPADC2_GAR_REG;                    /*!< Offset 0xFD4 GPADC2 Gating And Reset Register */
    __IOM uint32_t THS_ADC_CLK_REG;                   /*!< Offset 0xFD8 THS_ADC Clock Register  */
    __IOM uint32_t THS_ADC_GAR_REG;                   /*!< Offset 0xFDC THS_ADC Gating And Reset Register  */
         RESERVED(0xFE0[0x0FE4 - 0x0FE0], uint8_t)
    __IOM uint32_t THS_GAR_REG;                       /*!< Offset 0xFE4 THS Gating And Reset Register */
         RESERVED(0xFE8[0x1000 - 0x0FE8], uint8_t)
    __IOM uint32_t IR_RX0_CLK_REG;                    /*!< Offset 0x1000 IR_RX0 Clock Register */
    __IOM uint32_t IR_RX0_GAR_REG;                    /*!< Offset 0x1004 IR_RX0 Gating And Reset Register */
    __IOM uint32_t IR_TX_CLK_REG;                     /*!< Offset 0x1008 IR_TX Clock Register */
    __IOM uint32_t IR_TX_GAR_REG;                     /*!< Offset 0x100C IR_TX Gating And Reset Register */
         RESERVED(0x1010[0x1030 - 0x1010], uint8_t)
    __IOM uint32_t TPADC_CLK_REG;                     /*!< Offset 0x1030 TPADC Clock Register */
    __IOM uint32_t TPADC_GAR_REG;                     /*!< Offset 0x1034 TPADC Gating And Reset Register */
         RESERVED(0x1038[0x1040 - 0x1038], uint8_t)
    __IOM uint32_t LBC_CLK_REG;                       /*!< Offset 0x1040 LBC Clock Register */
         RESERVED(0x1044[0x104C - 0x1044], uint8_t)
    __IOM uint32_t LBC_GAR_REG;                       /*!< Offset 0x104C LBC Gating And Reset Register */
         RESERVED(0x1050[0x1100 - 0x1050], uint8_t)
    __IOM uint32_t IR_RX1_CLK_REG;                    /*!< Offset 0x1100 IR_RX1 Clock Register */
    __IOM uint32_t IR_RX1_GAR_REG;                    /*!< Offset 0x1104 IR_RX1 Gating And Reset Register */
    __IOM uint32_t IR_RX2_CLK_REG;                    /*!< Offset 0x1108 IR_RX2 Clock Register */
    __IOM uint32_t IR_RX2_GAR_REG;                    /*!< Offset 0x110C IR_RX2 Gating And Reset Register */
    __IOM uint32_t IR_RX3_CLK_REG;                    /*!< Offset 0x1110 IR_RX3 Clock Register */
    __IOM uint32_t IR_RX3_GAR_REG;                    /*!< Offset 0x1114 IR_RX3 Gating And Reset Register */
         RESERVED(0x1118[0x1200 - 0x1118], uint8_t)
    __IOM uint32_t I2S0_CLK_REG;                      /*!< Offset 0x1200 I2S0 Clock Register */
         RESERVED(0x1204[0x120C - 0x1204], uint8_t)
    __IOM uint32_t I2S0_GAR_REG;                      /*!< Offset 0x120C I2S0 Gating And Reset Register */
    __IOM uint32_t I2S1_CLK_REG;                      /*!< Offset 0x1210 I2S1 Clock Register */
         RESERVED(0x1214[0x121C - 0x1214], uint8_t)
    __IOM uint32_t I2S1_GAR_REG;                      /*!< Offset 0x121C I2S1 Gating And Reset Register */
    __IOM uint32_t I2S2_CLK_REG;                      /*!< Offset 0x1220 I2S2 Clock Register */
         RESERVED(0x1224[0x122C - 0x1224], uint8_t)
    __IOM uint32_t I2S2_GAR_REG;                      /*!< Offset 0x122C I2S2 Gating And Reset Register */
         RESERVED(0x1230[0x1280 - 0x1230], uint8_t)
    __IOM uint32_t OWA0_TX_CLK_REG;                   /*!< Offset 0x1280 OWA0 TX Clock Register */
    __IOM uint32_t OWA0_RX_CLK_REG;                   /*!< Offset 0x1284 OWA0 RX Clock Register */
         RESERVED(0x1288[0x128C - 0x1288], uint8_t)
    __IOM uint32_t OWA0_GAR_REG;                      /*!< Offset 0x128C OWA0 Gating And Reset Register */
         RESERVED(0x1290[0x12C0 - 0x1290], uint8_t)
    __IOM uint32_t DMIC_CLK_REG;                      /*!< Offset 0x12C0 DMIC Clock Register */
         RESERVED(0x12C4[0x12CC - 0x12C4], uint8_t)
    __IOM uint32_t DMIC_GAR_REG;                      /*!< Offset 0x12CC DMIC Gating And Reset Register */
         RESERVED(0x12D0[0x12E0 - 0x12D0], uint8_t)
    __IOM uint32_t AUDIOCODEC0_DAC_CLK_REG;           /*!< Offset 0x12E0 AUDIOCODEC0 DAC Clock Register */
         RESERVED(0x12E4[0x12EC - 0x12E4], uint8_t)
    __IOM uint32_t AUDIOCODEC0_GAR_REG;               /*!< Offset 0x12EC AUDIOCODEC0 Gating And Reset Register */
         RESERVED(0x12F0[0x1300 - 0x12F0], uint8_t)
    __IOM uint32_t USB0_CLK_REG;                      /*!< Offset 0x1300 USB0 Clock Register */
    __IOM uint32_t USB0_GAR_REG;                      /*!< Offset 0x1304 USB0 Gating And Reset Register */
    __IOM uint32_t USB1_CLK_REG;                      /*!< Offset 0x1308 USB1 Clock Register */
    __IOM uint32_t USB1_GAR_REG;                      /*!< Offset 0x130C USB1 Gating And Reset Register */
         RESERVED(0x1310[0x1340 - 0x1310], uint8_t)
    __IOM uint32_t USB2P0_SYS_PHY_REF_CLK_REG;        /*!< Offset 0x1340 USB2P0_SYS PHY Reference Clock Register */
    __IOM uint32_t USB2P0_SYS_GAR_REG;                /*!< Offset 0x1344 USB2P0_SYS Gating And Reset Register */
         RESERVED(0x1348[0x1400 - 0x1348], uint8_t)
    __IOM uint32_t GMAC0_PHY_CLK_REG;                 /*!< Offset 0x1400 GMAC0 PHY Clock Register */
    __IOM uint32_t GMAC0_PTP_REF_CLK_REG;             /*!< Offset 0x1404 GMAC0 PTP Reference Clock Register */
         RESERVED(0x1408[0x140C - 0x1408], uint8_t)
    __IOM uint32_t GMAC0_GAR_REG;                     /*!< Offset 0x140C GMAC0 Gating And Reset Register */
    __IOM uint32_t GMAC1_PHY_CLK_REG;                 /*!< Offset 0x1410 GMAC1 PHY Clock Register */
    __IOM uint32_t GMAC1_PTP_REF_CLK_REG;             /*!< Offset 0x1414 GMAC1 PTP Reference Clock Register */
         RESERVED(0x1418[0x141C - 0x1418], uint8_t)
    __IOM uint32_t GMAC1_GAR_REG;                     /*!< Offset 0x141C GMAC1 Gating And Reset Register */
    __IOM uint32_t GMAC2_PHY_CLK_REG;                 /*!< Offset 0x1420 GMAC2 PHY Clock Register */
    __IOM uint32_t GMAC2_PTP_REF_CLK_REG;             /*!< Offset 0x1424 GMAC2 PTP Reference Clock Register */
         RESERVED(0x1428[0x142C - 0x1428], uint8_t)
    __IOM uint32_t GMAC2_GAR_REG;                     /*!< Offset 0x142C GMAC2 Gating And Reset Register */
         RESERVED(0x1430[0x1500 - 0x1430], uint8_t)
    __IOM uint32_t TCON_LCD0_CLK_REG;                 /*!< Offset 0x1500 TCON_LCD0 Clock Register */
    __IOM uint32_t TCON_LCD0_GAR_REG;                 /*!< Offset 0x1504 TCON_LCD0 Gating And Reset Register */
         RESERVED(0x1508[0x1544 - 0x1508], uint8_t)
    __IOM uint32_t LVDS0_GAR_REG;                     /*!< Offset 0x1544 LVDS0 Gating And Reset Register */
         RESERVED(0x1548[0x1580 - 0x1548], uint8_t)
    __IOM uint32_t MIPI_DSI0_CLK_REG;                 /*!< Offset 0x1580 MIPI_DSI0 Clock Register */
    __IOM uint32_t MIPI_DSI0_GAR_REG;                 /*!< Offset 0x1584 MIPI_DSI0 Gating And Reset Register */
         RESERVED(0x1588[0x15C0 - 0x1588], uint8_t)
    __IOM uint32_t COMBOPHY0_CLK_REG;                 /*!< Offset 0x15C0 COMBOPHY0 Clock Register */
         RESERVED(0x15C4[0x16C4 - 0x15C4], uint8_t)
    __IOM uint32_t VO0_REG_GAR_REG;                   /*!< Offset 0x16C4 VO0_REG Gating And Reset Register */
         RESERVED(0x16C8[0x16E4 - 0x16C8], uint8_t)
    __IOM uint32_t VIDEO_OUT0_GAR_REG;                /*!< Offset 0x16E4 VIDEO_OUT0 Gating And Reset Register */
         RESERVED(0x16E8[0x1700 - 0x16E8], uint8_t)
    __IOM uint32_t LEDC_CLK_REG;                      /*!< Offset 0x1700 LEDC Clock Register */
    __IOM uint32_t LEDC_GAR_REG;                      /*!< Offset 0x1704 LEDC Gating And Reset Register */
         RESERVED(0x1708[0x1800 - 0x1708], uint8_t)
    __IOM uint32_t CSI_MASTER0_CLK_REG;               /*!< Offset 0x1800 CSI Master0 Clock Register */
    __IOM uint32_t CSI_MASTER1_CLK_REG;               /*!< Offset 0x1804 CSI Master1 Clock Register */
    __IOM uint32_t CSI_MASTER2_CLK_REG;               /*!< Offset 0x1808 CSI Master2 Clock Register */
         RESERVED(0x180C[0x1840 - 0x180C], uint8_t)
    __IOM uint32_t CSI_CLK_REG;                       /*!< Offset 0x1840 CSI Clock Register */
         RESERVED(0x1844[0x1860 - 0x1844], uint8_t)
    __IOM uint32_t ISP_CLK_REG;                       /*!< Offset 0x1860 ISP Clock Register */
         RESERVED(0x1864[0x1884 - 0x1864], uint8_t)
    __IOM uint32_t VIDEO_IN_GAR_REG;                  /*!< Offset 0x1884 VIDEO_IN Gating And Reset Register */
         RESERVED(0x1888[0x1908 - 0x1888], uint8_t)
    __IOM uint32_t PERI0PLL_GATE_EN_REG;              /*!< Offset 0x1908 PERI0PLL Gate Enable Register */
    __IOM uint32_t PERI1PLL_GATE_EN_REG;              /*!< Offset 0x190C PERI1PLL Gate Enable Register */
    __IOM uint32_t VIDEOPLL_GATE_EN_REG;              /*!< Offset 0x1910 VIDEOPLL Gate Enable Register */
         RESERVED(0x1914[0x191C - 0x1914], uint8_t)
    __IOM uint32_t AUDIOPLL_GATE_EN_REG;              /*!< Offset 0x191C AUDIOPLL Gate Enable Register */
         RESERVED(0x1920[0x1988 - 0x1920], uint8_t)
    __IOM uint32_t PERI0PLL_GATE_STAT_REG;            /*!< Offset 0x1988 PERI0PLL Gate Status Register */
    __IOM uint32_t PERI1PLL_GATE_STAT_REG;            /*!< Offset 0x198C PERI1PLL Gate Status Register */
    __IOM uint32_t VIDEOPLL_GATE_STAT_REG;            /*!< Offset 0x1990 VIDEOPLL Gate Status Register */
         RESERVED(0x1994[0x199C - 0x1994], uint8_t)
    __IOM uint32_t AUDIOPLL_GATE_STAT_REG;            /*!< Offset 0x199C AUDIOPLL Gate Status Register */
         RESERVED(0x19A0[0x1A20 - 0x19A0], uint8_t)
    __IOM uint32_t PLL_OPG_BYPASS_REG;                /*!< Offset 0x1A20 PLL Output Gate Bypass Register */
         RESERVED(0x1A24[0x1C00 - 0x1A24], uint8_t)
    __IOM uint32_t AXI_MON_GAR_REG;                   /*!< Offset 0x1C00 AXI MON Gating And Reset Register */
    __IOM uint32_t AHB_MON_GAR_REG;                   /*!< Offset 0x1C04 AHB MON Gating And Reset Register */
         RESERVED(0x1C08[0x1F00 - 0x1C08], uint8_t)
    __IOM uint32_t CCU_SEC_SWITCH_REG;                /*!< Offset 0x1F00 CCU Security Switch Register */
         RESERVED(0x1F04[0x1F10 - 0x1F04], uint8_t)
    __IOM uint32_t DAP_REQ_CTRL_REG;                  /*!< Offset 0x1F10 DAP REQ Control Register */
         RESERVED(0x1F14[0x1F20 - 0x1F14], uint8_t)
    __IOM uint32_t PLL_CFG0_REG;                      /*!< Offset 0x1F20 PLL Configuration0 Register */
    __IOM uint32_t PLL_CFG1_REG;                      /*!< Offset 0x1F24 PLL Configuration1 Register */
    __IOM uint32_t PLL_CFG2_REG;                      /*!< Offset 0x1F28 PLL Configuration2 Register */
         RESERVED(0x1F2C[0x1F30 - 0x1F2C], uint8_t)
    __IOM uint32_t CCU_FAN_GATE_REG;                  /*!< Offset 0x1F30 CCU Fanout Clock Gate Register */
    __IOM uint32_t CLK27M_FAN_REG;                    /*!< Offset 0x1F34 CLK27M Fanout Register */
    __IOM uint32_t CLK_FAN_REG;                       /*!< Offset 0x1F38 CLK Fanout Register */
    __IOM uint32_t CCU_FAN_REG;                       /*!< Offset 0x1F3C CCU Fanout Register */
         RESERVED(0x1F40[0x1F50 - 0x1F40], uint8_t)
    __IOM uint32_t CLK_DBG_REG;                       /*!< Offset 0x1F50 Clock Debug Register */
         RESERVED(0x1F54[0x1F60 - 0x1F54], uint8_t)
    __IOM uint32_t FRE_DET_CTRL_REG;                  /*!< Offset 0x1F60 Frequency Detect Control Register */
    __IOM uint32_t FRE_UP_LIM_REG;                    /*!< Offset 0x1F64 Frequency Up Limit Register */
    __IOM uint32_t FRE_DOWN_LIM_REG;                  /*!< Offset 0x1F68 Frequency Down Limit Register */
} CCU_TypeDef; /* size of structure = 0x1F6C */
/*
 * @brief GMAC_TOP
 */
/*!< GMAC_TOP  */
typedef struct GMAC_TOP_Type
{
    __IOM uint32_t GMAC_CFG_REG;                      /*!< Offset 0x000 GMAC Config Register */
         RESERVED(0x004[0x0040 - 0x0004], uint8_t)
    __IOM uint32_t GMAC_PTP_TIMESTAMP_LOW_REG;        /*!< Offset 0x040 GMAC PTP Timestamp Low Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __IOM uint32_t GMAC_PTP_TIMESTAMP_HIGH_REG;       /*!< Offset 0x048 GMAC PTP Timestamp High Register */
    __IOM uint32_t GMAC_STAT_INT_REG;                 /*!< Offset 0x04C GMAC Low Power Interrupt Status Register */
         RESERVED(0x050[0x0080 - 0x0050], uint8_t)
    __IOM uint32_t GMAC_CLK_GATE_CFG_REG;             /*!< Offset 0x080 GMAC Clock Gate Config Register */
} GMAC_TOP_TypeDef; /* size of structure = 0x084 */
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
 * @brief PRCM
 */
/*!< PRCM  */
typedef struct PRCM_Type
{
    __IOM uint32_t AHBS_CLK_REG;                      /*!< Offset 0x000 AHBS Clock Register */
         RESERVED(0x004[0x000C - 0x0004], uint8_t)
    __IOM uint32_t APBS0_CLK_REG;                     /*!< Offset 0x00C APBS0 Clock Register  */
         RESERVED(0x010[0x012C - 0x0010], uint8_t)
    __IOM uint32_t TWD_BGR_REG;                       /*!< Offset 0x12C TWD Bus Gating Reset Register */
         RESERVED(0x130[0x020C - 0x0130], uint8_t)
    __IOM uint32_t RTC_BGR_REG;                       /*!< Offset 0x20C RTC Bus Gating Reset Register */
         RESERVED(0x210[0x022C - 0x0210], uint8_t)
    __IOM uint32_t CPUIDLE_BGR_REG;                   /*!< Offset 0x22C CPUIDLE Bus Gating Reset Register */
         RESERVED(0x230[0x0240 - 0x0230], uint8_t)
    __IOM uint32_t PRCM_PLL_CTRL_REG0;                /*!< Offset 0x240 PRCM PLL Control Register 0 */
    __IOM uint32_t PRCM_PLL_CTRL_REG1;                /*!< Offset 0x244 PRCM PLL Control Register 1 */
         RESERVED(0x248[0x0254 - 0x0248], uint8_t)
    __IOM uint32_t ANA_PWR_RST_REG;                   /*!< Offset 0x254 ANALOG Power Off Gating Register */
         RESERVED(0x258[0x0260 - 0x0258], uint8_t)
    __IOM uint32_t VDD_SYS_PWR_RST_REG;               /*!< Offset 0x260 VDD_SYS Power Domain Reset Register  */
         RESERVED(0x264[0x0270 - 0x0264], uint8_t)
    __IOM uint32_t RAMP2P_CFG_REG;                    /*!< Offset 0x270 RAMP2P Configuration Register  */
    __IOM uint32_t RAM2P_CFG_REG;                     /*!< Offset 0x274 RAM2P Configuration Register  */
    __IOM uint32_t RAMSP_CFG_REG;                     /*!< Offset 0x278 RAMSP Configuration Register  */
    __IOM uint32_t ROM_CFG_REG;                       /*!< Offset 0x27C ROM Configuration Register  */
         RESERVED(0x280[0x0290 - 0x0280], uint8_t)
    __IOM uint32_t PRCM_SEC_SWITCH_REG;               /*!< Offset 0x290 PRCM Security Switch Register  */
         RESERVED(0x294[0x0320 - 0x0294], uint8_t)
    __IOM uint32_t NMI_INT_CTRL_REG;                  /*!< Offset 0x320 NMI Interrupt Control Register */
    __IOM uint32_t NMI_INT_EN_REG;                    /*!< Offset 0x324 NMI Interrupt Enable Register */
    __IOM uint32_t NMI_INT_PEND_REG;                  /*!< Offset 0x328 NMI Interrupt Pending Register */
         RESERVED(0x32C[0x033C - 0x032C], uint8_t)
    __IOM uint32_t BUS_ACG_REG;                       /*!< Offset 0x33C Bus Auto Clock Gating Register */
         RESERVED(0x340[0x0378 - 0x0340], uint8_t)
    __IOM uint32_t AHBS_RDY_TOUT_CTRL_REG;            /*!< Offset 0x378 AHBS Ready Timeout Control Register */
} PRCM_TypeDef; /* size of structure = 0x37C */
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
} SID_TypeDef; /* size of structure = 0x600 */
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
 * @brief S_GPIO
 */
/*!< S_GPIO Port Controller */
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

#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
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
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID Security ID register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA Port Controller register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB Port Controller register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Port Controller register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Port Controller register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE Port Controller register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF Port Controller register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG Port Controller register set access pointer */
#define GPIOJ ((GPIO_TypeDef *) GPIOJ_BASE)           /*!< GPIOJ Port Controller register set access pointer */
#define GPIOK ((GPIO_TypeDef *) GPIOK_BASE)           /*!< GPIOK Port Controller register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define SPI2 ((SPI_TypeDef *) SPI2_BASE)              /*!< SPI2 Serial Peripheral Interface register set access pointer */
#define SPI3 ((SPI_TypeDef *) SPI3_BASE)              /*!< SPI3 Serial Peripheral Interface register set access pointer */
#define GAMC0_TOP ((GMAC_TOP_TypeDef *) GAMC0_TOP_BASE)/*!< GAMC0_TOP  register set access pointer */
#define GAMC1_TOP ((GMAC_TOP_TypeDef *) GAMC1_TOP_BASE)/*!< GAMC1_TOP  register set access pointer */
#define GAMC2_TOP ((GMAC_TOP_TypeDef *) GAMC2_TOP_BASE)/*!< GAMC2_TOP  register set access pointer */
#define PRCM ((PRCM_TypeDef *) PRCM_BASE)             /*!< PRCM  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__858514A9_1AE9_4033_9FAD_BC80BB4264BB__INCLUDED */
