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
    PB_EINT_IRQn = 43,                                /*!< GPIOINT PB_EINT interrupt  */
    PG_EINT_IRQn = 49,                                /*!< GPIOINT PG_EINT interrupt */
    PH_EINT_IRQn = 53,                                /*!< GPIOINT PH_EINT interrupt */
    R_UART_IRQn = 70,                                 /*!< UART  */
    R_PL_EINT_IRQn = 77,                              /*!< GPIOINT R_PL_EINT interrupt */
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

#define SYSCTRL_BASE ((uintptr_t) 0x01C00000)         /*!< SYSCTRL  Base */
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
#define UART0_BASE ((uintptr_t) 0x01C28000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x01C28400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x01C28800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x01C28C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x01C29000)           /*!< UART  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x01C81000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x01C82000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define CPUCFG_BASE ((uintptr_t) 0x01F01C00)          /*!< CPUCFG CPU Configuration Base */
#define R_UART_BASE ((uintptr_t) 0x01F02800)          /*!< UART  Base */
#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK  Base */
#define GPIOL_BASE ((uintptr_t) 0x01F02C00)           /*!< GPIO  Base */

#include <core_ca.h>

/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef __PACKED_STRUCT CCU_Type
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
         uint32_t reserved_0x104 [0x0005];
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
typedef __PACKED_STRUCT CPUCFG_Type
{
    __IO uint32_t CPUS_RST_CTRL_REG;                  /*!< Offset 0x000 CPUS reset control register */
         uint32_t reserved_0x004 [0x000F];
    __PACKED_STRUCT
    {
        __IO uint32_t CPU_RST_CTRL;                   /*!< Offset 0x040 CPU0 reset control */
        __IO uint32_t CPU_CTRL_REG;                   /*!< Offset 0x044 CPU0 control register */
        __IO uint32_t CPU_STATUS_REG;                 /*!< Offset 0x048 CPU0 status register */
    } CPU [0x004];                                    /*!< Offset 0x040 CPU control [0..3] */
         uint32_t reserved_0x070 [0x0034];
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
 * @brief GPIO
 */
/*!< GPIO  */
typedef __PACKED_STRUCT GPIO_Type
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
typedef __PACKED_STRUCT GPIOBLOCK_Type
{
    __PACKED_STRUCT
    {
        __IO uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
        __IO uint32_t DATA;                           /*!< Offset 0x010 Data Register */
        __IO uint32_t DRV [0x002];                    /*!< Offset 0x014 Multi_Driving Register */
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x008];                              /*!< Offset 0x000 GPIO pin control */
         uint32_t reserved_0x120 [0x0038];
    __PACKED_STRUCT
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
typedef __PACKED_STRUCT GPIOINT_Type
{
    __IO uint32_t EINT_CFG [0x004];                   /*!< Offset 0x000 External Interrupt Configure Registers */
    __IO uint32_t EINT_CTL;                           /*!< Offset 0x010 External Interrupt Control Register */
    __IO uint32_t EINT_STATUS;                        /*!< Offset 0x014 External Interrupt Status Register */
    __IO uint32_t EINT_DEB;                           /*!< Offset 0x018 External Interrupt Debounce Register */
         uint32_t reserved_0x01C;
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief SYSCTRL
 */
/*!< SYSCTRL  */
typedef __PACKED_STRUCT SYSCTRL_Type
{
         uint32_t reserved_0x000 [0x0009];
    __I  uint32_t VER_REG;                            /*!< Offset 0x024 Version Register */
         uint32_t reserved_0x028 [0x0002];
    __IO uint32_t EMAC_EPHY_CLK_REG;                  /*!< Offset 0x030 EMAC-EPHY Clock Register */
} SYSCTRL_TypeDef; /* size of structure = 0x034 */
/*
 * @brief UART
 */
/*!< UART  */
typedef __PACKED_STRUCT UART_Type
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


/* Defines */



/* Access pointers */

#define SYSCTRL ((SYSCTRL_TypeDef *) SYSCTRL_BASE)    /*!< SYSCTRL  register set access pointer */
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
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define CPUCFG ((CPUCFG_TypeDef *) CPUCFG_BASE)       /*!< CPUCFG CPU Configuration register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L  register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
