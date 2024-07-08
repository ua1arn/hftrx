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
    TWI0_IRQn = 38,                                   /*!< TWI  */
    TWI1_IRQn = 39,                                   /*!< TWI  */
    GPIOB_NS_IRQn = 47,                               /*!< GPIOINT  */
    GPIOG_NS_IRQn = 49,                               /*!< GPIOINT  */
    TIMER0_IRQn = 50,                                 /*!< TIMER  */
    TIMER1_IRQn = 51,                                 /*!< TIMER  */
    TIMER3_IRQn = 52,                                 /*!< TIMER  */
    WATCHDOG_IRQn = 57,                               /*!< TIMER  */
    ALARM0_IRQn = 72,                                 /*!< RTC  */
    ALARM1_IRQn = 73,                                 /*!< RTC  */
    DMAC_IRQn = 82,                                   /*!< DMAC  */
    VE_IRQn = 90,                                     /*!< VE Video Encoding */
    SPI_IRQn = 97,                                    /*!< SPI SPI interrupt */
    USB_DEVICE_IRQn = 103,                            /*!< USBOTG  */
    USB_EHCI_IRQn = 104,                              /*!< USB_EHCI_Capability  */
    USB_OHCI_IRQn = 105,                              /*!< USB_OHCI_Capability  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define SYS_CFG_BASE ((uintptr_t) 0x01C00000)         /*!< SYS_CFG  Base */
#define DMAC_BASE ((uintptr_t) 0x01C02000)            /*!< DMAC  Base */
#define VENCODER_BASE ((uintptr_t) 0x01C0E000)        /*!< VE Video Encoding Base */
#define USBOTG_BASE ((uintptr_t) 0x01C19000)          /*!< USBOTG  Base */
#define USBEHCI_BASE ((uintptr_t) 0x01C1A000)         /*!< USB_EHCI_Capability  Base */
#define USBOHCI_BASE ((uintptr_t) 0x01C1A400)         /*!< USB_OHCI_Capability  Base */
#define USBPHY_BASE ((uintptr_t) 0x01C1A800)          /*!< USBPHYC  Base */
#define CCU_BASE ((uintptr_t) 0x01C20000)             /*!< CCU  Base */
#define RTC_BASE ((uintptr_t) 0x01C20400)             /*!< RTC  Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x01C20800)       /*!< GPIOBLOCK  Base */
#define GPIOINTB_BASE ((uintptr_t) 0x01C20820)        /*!< GPIOINT  Base */
#define GPIOB_BASE ((uintptr_t) 0x01C20824)           /*!< GPIO  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x01C20840)        /*!< GPIOINT  Base */
#define GPIOC_BASE ((uintptr_t) 0x01C20848)           /*!< GPIO  Base */
#define GPIOINTE_BASE ((uintptr_t) 0x01C20880)        /*!< GPIOINT  Base */
#define GPIOE_BASE ((uintptr_t) 0x01C20890)           /*!< GPIO  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x01C208A0)        /*!< GPIOINT  Base */
#define GPIOF_BASE ((uintptr_t) 0x01C208B4)           /*!< GPIO  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x01C208C0)        /*!< GPIOINT  Base */
#define GPIOG_BASE ((uintptr_t) 0x01C208D8)           /*!< GPIO  Base */
#define TIMER_BASE ((uintptr_t) 0x01C20C00)           /*!< TIMER  Base */
#define PWM_BASE ((uintptr_t) 0x01C21400)             /*!< PWM  Base */
#define UART0_BASE ((uintptr_t) 0x01C28000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x01C28400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x01C28800)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x01C2AC00)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x01C2B000)            /*!< TWI  Base */
#define DRAM_COM_BASE ((uintptr_t) 0x01C62000)        /*!< DRAM_COM  Base */
#define DRAM_CTL_BASE ((uintptr_t) 0x01C63000)        /*!< DRAM_CTL  Base */
#define SPI_BASE ((uintptr_t) 0x01C68000)             /*!< SPI Serial Peripheral Interface Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x01C81000) /*!< GIC_DISTRIBUTOR GIC DISTRIBUTOR Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x01C82000)   /*!< GIC_INTERFACE GIC CPU IF Base */

/*
 * @brief CCU
 */
/*!< CCU  */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPU_CTRL_REG;               /*!< Offset 0x000 PLL_CPU Control Register */
             uint32_t reserved_0x004;
    volatile uint32_t PLL_AUDIO_CTRL_REG;             /*!< Offset 0x008 PLL_AUDIO Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t PLL_VIDEO_CTRL_REG;             /*!< Offset 0x010 PLL_VIDEO Control Register */
             uint32_t reserved_0x014;
    volatile uint32_t PLL_VE_CTRL_REG;                /*!< Offset 0x018 PLL_VE Control Register */
             uint32_t reserved_0x01C;
    volatile uint32_t PLL_DDR0_CTRL_REG;              /*!< Offset 0x020 PLL_DDR0 Control Register */
             uint32_t reserved_0x024;
    volatile uint32_t PLL_PERIPH0_CTRL_REG;           /*!< Offset 0x028 PLL_PERIPH0 Control Register  */
    volatile uint32_t PLL_ISP_CTRL_REG;               /*!< Offset 0x02C PLL_ISP Control Register */
             uint32_t reserved_0x030 [0x0005];
    volatile uint32_t PLL_PERIPH1_CTRL_REG;           /*!< Offset 0x044 PLL_PERIPH1 Control Register */
             uint32_t reserved_0x048;
    volatile uint32_t PLL_DDR1_CTRL_REG;              /*!< Offset 0x04C PLL_DDR1 Control Register */
    volatile uint32_t CPU_AXI_CFG_REG;                /*!< Offset 0x050 CPU/AXI Configuration Register  */
    volatile uint32_t AHB1_APB1_CFG_REG;              /*!< Offset 0x054 AHB/APB0 Configuration Register (AHB_APB0_CFG_REG) */
    volatile uint32_t APB2_CFG_REG;                   /*!< Offset 0x058 APB2 Configuration Register */
    volatile uint32_t AHB2_CFG_REG;                   /*!< Offset 0x05C AHB2 Configuration Register */
    volatile uint32_t BUS_CLK_GATING_REG0;            /*!< Offset 0x060 Bus Clock Gating Register 0 */
    volatile uint32_t BUS_CLK_GATING_REG1;            /*!< Offset 0x064 Bus Clock Gating Register 1 */
    volatile uint32_t BUS_CLK_GATING_REG2;            /*!< Offset 0x068 Bus Clock Gating Register 2 */
    volatile uint32_t BUS_CLK_GATING_REG3;            /*!< Offset 0x06C Bus Clock Gating Register 3 */
    volatile uint32_t BUS_CLK_GATING_REG4;            /*!< Offset 0x070 Bus Clock Gating Register4 */
             uint32_t reserved_0x074 [0x0005];
    volatile uint32_t SDMMC0_CLK_REG;                 /*!< Offset 0x088 SDMMC0 Clock Register */
    volatile uint32_t SDMMC1_CLK_REG;                 /*!< Offset 0x08C SDMMC1 Clock Register */
    volatile uint32_t SDMMC2_CLK_REG;                 /*!< Offset 0x090 SDMMC2 Clock Register */
             uint32_t reserved_0x094 [0x0002];
    volatile uint32_t SS_CLK_REG;                     /*!< Offset 0x09C SS Clock Register */
    volatile uint32_t SPI_CLK_REG;                    /*!< Offset 0x0A0 SPI Clock Register */
             uint32_t reserved_0x0A4 [0x0003];
    volatile uint32_t I2S_PCM_CLK_REG;                /*!< Offset 0x0B0 I2S/PCM Clock Register */
             uint32_t reserved_0x0B4 [0x0006];
    volatile uint32_t USBPHY_CFG_REG;                 /*!< Offset 0x0CC USBPHY Configuration Register */
             uint32_t reserved_0x0D0 [0x0009];
    volatile uint32_t DRAM_CFG_REG;                   /*!< Offset 0x0F4 DRAM Configuration Register */
    volatile uint32_t PLL_DDR1_CFG_REG;               /*!< Offset 0x0F8 PLL_DDR Configuration Register */
    volatile uint32_t MBUS_RST_REG;                   /*!< Offset 0x0FC MBUS Reset Register */
    volatile uint32_t DRAM_CLK_GATING_REG;            /*!< Offset 0x100 DRAM Clock Gating Register */
    volatile uint32_t DE_CLK_REG;                     /*!< Offset 0x104 DE Clock Register */
             uint32_t reserved_0x108 [0x0004];
    volatile uint32_t TCON_CLK_REG;                   /*!< Offset 0x118 TCON Clock Register */
             uint32_t reserved_0x11C [0x0005];
    volatile uint32_t CSI_MISC_CLK_REG;               /*!< Offset 0x130 CSI_MISC Clock Register */
    volatile uint32_t CSI_CLK_REG;                    /*!< Offset 0x134 CSI Clock Register */
             uint32_t reserved_0x138;
    volatile uint32_t VE_CLK_REG;                     /*!< Offset 0x13C VE Clock Register */
    volatile uint32_t AC_DIG_CLK_REG;                 /*!< Offset 0x140 AC Digital Clock Register */
    volatile uint32_t AVS_CLK_REG;                    /*!< Offset 0x144 AVS Clock Register */
             uint32_t reserved_0x148 [0x0005];
    volatile uint32_t MBUS_CLK_REG;                   /*!< Offset 0x15C MBUS Clock Register */
             uint32_t reserved_0x160 [0x0003];
    volatile uint32_t MIPI_CSI_REG;                   /*!< Offset 0x16C MIPI_CSI Register */
             uint32_t reserved_0x170 [0x0024];
    volatile uint32_t PLL_STABLE_TIME_REG0;           /*!< Offset 0x200 PLL Stable Time Register0 */
    volatile uint32_t PLL_STABLE_TIME_REG1;           /*!< Offset 0x204 PLL Stable Time Register1 */
             uint32_t reserved_0x208 [0x0006];
    volatile uint32_t PLL_CPU_BIAS_REG;               /*!< Offset 0x220 PLL_CPU Bias Register */
    volatile uint32_t PLL_AUDIO_BIAS_REG;             /*!< Offset 0x224 PLL_AUDIO Bias Register */
    volatile uint32_t PLL_VIDEO_BIAS_REG;             /*!< Offset 0x228 PLL_VIDEO Bias Register */
    volatile uint32_t PLL_VE_BIAS_REG;                /*!< Offset 0x22C PLL_VE Bias Register */
    volatile uint32_t PLL_DDR0_BIAS_REG;              /*!< Offset 0x230 PLL_DDR0 Bias Register */
    volatile uint32_t PLL_PERIPH0_BIAS_REG;           /*!< Offset 0x234 PLL_PERIPH0 Bias Register */
    volatile uint32_t PLL_ISP_BIAS_REG;               /*!< Offset 0x238 PLL_ISP Bias Register */
             uint32_t reserved_0x23C [0x0002];
    volatile uint32_t PLL_PERIPH1_BIAS_REG;           /*!< Offset 0x244 PLL_PERIPH1 Bias Register */
             uint32_t reserved_0x248;
    volatile uint32_t PLL_DDR1_BIAS_REG;              /*!< Offset 0x24C PLL_DDR1 Bias Register */
    volatile uint32_t PLL_CPU_TUN_REG;                /*!< Offset 0x250 PLL_CPU Tuning Register */
             uint32_t reserved_0x254 [0x0003];
    volatile uint32_t PLL_DDR0_TUN_REG;               /*!< Offset 0x260 PLL_DDR0 Tuning Register */
             uint32_t reserved_0x264 [0x0007];
    volatile uint32_t PLL_CPU_PAT_CTRL_REG;           /*!< Offset 0x280 PLL_CPU Pattern Control Register  */
    volatile uint32_t PLL_AUDIO_PAT_CTRL_REG;         /*!< Offset 0x284 PLL_AUDIO Pattern Control Register */
    volatile uint32_t PLL_VIDEO_PAT_CTRL_REG;         /*!< Offset 0x288 PLL_VIDEO Pattern Control Register */
    volatile uint32_t PLL_VE_PAT_CTRL_REG;            /*!< Offset 0x28C PLL_VE Pattern Control Register */
    volatile uint32_t PLL_DDR0_PAT_CTRL_REG;          /*!< Offset 0x290 PLL_DDR0 Pattern Control Register */
             uint32_t reserved_0x294;
    volatile uint32_t PLL_ISP_PAT_CTRL_REG;           /*!< Offset 0x298 PLL_ISP Pattern Control Register */
             uint32_t reserved_0x29C [0x0002];
    volatile uint32_t PLL_PERIPH1_PAT_CTRL_REG;       /*!< Offset 0x2A4 PLL_PERIPH1 Pattern Control Register */
             uint32_t reserved_0x2A8;
    volatile uint32_t PLL_DDR1_PAT_CTRL_REG0;         /*!< Offset 0x2AC PLL_DDR1 Pattern Control Register0 */
    volatile uint32_t PLL_DDR1_PAT_CTRL_REG1;         /*!< Offset 0x2B0 PLL_DDR1 Pattern Control Register1 */
             uint32_t reserved_0x2B4 [0x0003];
    volatile uint32_t BUS_SOFT_RST_REG0;              /*!< Offset 0x2C0 Bus Software Reset Register 0 */
    volatile uint32_t BUS_SOFT_RST_REG1;              /*!< Offset 0x2C4 Bus Software Reset Register 1 */
    volatile uint32_t BUS_SOFT_RST_REG2;              /*!< Offset 0x2C8 Bus Software Reset Register 2 */
             uint32_t reserved_0x2CC;
    volatile uint32_t BUS_SOFT_RST_REG3;              /*!< Offset 0x2D0 Bus Software Reset Register 3 */
             uint32_t reserved_0x2D4;
    volatile uint32_t BUS_SOFT_RST_REG4;              /*!< Offset 0x2D8 Bus Software Reset Register 4 */
             uint32_t reserved_0x2DC [0x0009];
    volatile uint32_t PS_CTRL_REG;                    /*!< Offset 0x300 PS Control Register */
    volatile uint32_t PS_CNT_REG;                     /*!< Offset 0x304 PS Counter Register */
             uint32_t reserved_0x308 [0x003E];
} CCU_TypeDef; /* size of structure = 0x400 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
    volatile uint32_t DMAC_IRQ_EN_REG;                /*!< Offset 0x000 DMAC IRQ Enable Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t DMAC_IRQ_PEND_REG;              /*!< Offset 0x010 DMAC IRQ Pending Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t DMAC_AUTO_GATE_REG;             /*!< Offset 0x020 DMAC Auto Gating Register */
             uint32_t reserved_0x024 [0x0003];
    const volatile uint32_t DMAC_STA_REG;             /*!< Offset 0x030 DMAC Status Register */
             uint32_t reserved_0x034 [0x0033];
    struct
    {
        volatile uint32_t DMAC_EN_REGN;               /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 7) 0x0100 + N*0x0040 */
        volatile uint32_t DMAC_PAU_REGN;              /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 7) 0x0104 + N*0x0040 */
        volatile uint32_t DMAC_DESC_ADDR_REGN;        /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 7) 0x0108 + N*0x0040 */
        const volatile uint32_t DMAC_CFG_REGN;        /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 7) 0x010C + N*0x0040 */
        const volatile uint32_t DMAC_CUR_SRC_REGN;    /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 7) 0x0110 + N*0x0040 */
        const volatile uint32_t DMAC_CUR_DEST_REGN;   /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 7) 0x0114 + N*0x0040 */
        const volatile uint32_t DMAC_BCNT_LEFT_REGN;  /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 7) 0x0118 + N*0x0040 */
        const volatile uint32_t DMAC_PARA_REGN;       /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 7) 0x011C + N*0x0040 */
                 uint32_t reserved_0x020 [0x0008];
    } CH [0x008];                                     /*!< Offset 0x100 Channel [0..7] */
             uint32_t reserved_0x300 [0x0340];
} DMAC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief GPIO
 */
/*!< GPIO  */
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
/*!< GPIOBLOCK  */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        volatile uint32_t CFG [0x004];                /*!< Offset 0x000 Configure Register */
        volatile uint32_t DATA;                       /*!< Offset 0x010 Data Register */
        volatile uint32_t DRV [0x002];                /*!< Offset 0x014 Multi_Driving Register */
        volatile uint32_t PULL [0x002];               /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x007];                              /*!< Offset 0x000 GPIO pin control */
             uint32_t reserved_0x0FC [0x0041];
    struct
    {
        volatile uint32_t EINT_CFG [0x004];           /*!< Offset 0x200 External Interrupt Configure Registers */
        volatile uint32_t EINT_CTL;                   /*!< Offset 0x210 External Interrupt Control Register */
        volatile uint32_t EINT_STATUS;                /*!< Offset 0x214 External Interrupt Status Register */
        volatile uint32_t EINT_DEB;                   /*!< Offset 0x218 External Interrupt Debounce Register */
                 uint32_t reserved_0x01C;
    } GPIO_INTS [0x007];                              /*!< Offset 0x200 GPIO interrupt control */
             uint32_t reserved_0x2E0 [0x0048];
} GPIOBLOCK_TypeDef; /* size of structure = 0x400 */
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
 * @brief PWM
 */
/*!< PWM  */
typedef struct PWM_Type
{
    volatile uint32_t PWM_CH_CTRL;                    /*!< Offset 0x000 PWM Control Register */
    volatile uint32_t PWM_CH0_PERIOD;                 /*!< Offset 0x004 PWM Channel 0 Period Register */
    volatile uint32_t PWM_CH1_PERIOD;                 /*!< Offset 0x008 PWM Channel 1 Period Registe */
             uint32_t reserved_0x00C [0x00FD];
} PWM_TypeDef; /* size of structure = 0x400 */
/*
 * @brief RTC
 */
/*!< RTC  */
typedef struct RTC_Type
{
    volatile uint32_t LOSC_CTRL_REG;                  /*!< Offset 0x000 Low Oscillator Control Register l */
    volatile uint32_t LOSC_AUTO_SWT_STA_REG;          /*!< Offset 0x004 LOSC Auto Switch Status Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t RTC_YY_MM_DD_REG;               /*!< Offset 0x010 RTC Year-Month-Day Register */
    volatile uint32_t RTC_HH_MM_SS_REG;               /*!< Offset 0x014 RTC Hour-Minute-Second Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t ALARM0_COUNTER_REG;             /*!< Offset 0x020 Alarm 0 Counter Register  */
    volatile uint32_t ALARM0_CUR_VLU_REG;             /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    volatile uint32_t ALARM0_ENABLE_REG;              /*!< Offset 0x028 Alarm 0 Enable Register */
    volatile uint32_t ALARM0_IRQ_EN;                  /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    volatile uint32_t ALARM0_IRQ_STA_REG;             /*!< Offset 0x030 Alarm 0 IRQ Status Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t ALARM1_WK_HH_MM_SS;             /*!< Offset 0x040 Alarm 1 Week HMS Register  */
    volatile uint32_t ALARM1_ENABLE_REG;              /*!< Offset 0x044 Alarm 1 Enable Register */
    volatile uint32_t ALARM1_IRQ_EN;                  /*!< Offset 0x048 Alarm 1 IRQ Enable Register */
    volatile uint32_t ALARM1_IRQ_STA_REG;             /*!< Offset 0x04C Alarm 1 IRQ Status Register */
    volatile uint32_t ALARM_CONFIG_REG;               /*!< Offset 0x050 Alarm Config Register */
             uint32_t reserved_0x054 [0x0003];
    volatile uint32_t LOSC_OUT_GATING_REG;            /*!< Offset 0x060 LOSC output gating register */
             uint32_t reserved_0x064 [0x0027];
    volatile uint32_t GP_DATA_REG [0x008];            /*!< Offset 0x100 General Purpose Register (N=0~7) */
             uint32_t reserved_0x120 [0x001C];
    volatile uint32_t VDD_RTC_REG;                    /*!< Offset 0x190 VDD RTC Regulate Register */
             uint32_t reserved_0x194 [0x0017];
    volatile uint32_t IC_CHARA_REG;                   /*!< Offset 0x1F0 IC Characteristic Register */
             uint32_t reserved_0x1F4 [0x0083];
} RTC_TypeDef; /* size of structure = 0x400 */
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
    volatile uint32_t SPI_CCR;                        /*!< Offset 0x024 SPI Clock Rate Control register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t SPI_MBC;                        /*!< Offset 0x030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                        /*!< Offset 0x034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                        /*!< Offset 0x038 SPI Master Burst Control Register */
             uint32_t reserved_0x03C [0x0071];
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
    const volatile uint32_t VER_REG;                  /*!< Offset 0x024 Version Register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t EMAC_CLK_REG;                   /*!< Offset 0x030 EMAC-EPHY Clock Register */
} SYS_CFG_TypeDef; /* size of structure = 0x034 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    volatile uint32_t TMR_IRQ_EN_REG;                 /*!< Offset 0x000 Timer IRQ Enable Register */
    volatile uint32_t TMR_IRQ_STA_REG;                /*!< Offset 0x004 Timer Status Register */
             uint32_t reserved_0x008 [0x0002];
    struct
    {
        volatile uint32_t CTRL_REG;                   /*!< Offset 0x010 Timer n Control Register */
        volatile uint32_t INTV_VALUE_REG;             /*!< Offset 0x014 Timer n Interval Value Register */
        volatile uint32_t CUR_VALUE_REG;              /*!< Offset 0x018 Timer n Current Value Register */
                 uint32_t reserved_0x00C;
    } TMR [0x003];                                    /*!< Offset 0x010 Timer */
             uint32_t reserved_0x040 [0x0018];
    volatile uint32_t WDOG_IRQ_EN_REG;                /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
    volatile uint32_t WDOG_IRQ_STA_REG;               /*!< Offset 0x0A4 Watchdog Status Register */
    volatile uint32_t WDOG_SOFT_RST_REG;              /*!< Offset 0x0A8 Watchdog Software Reset Register */
             uint32_t reserved_0x0AC;
    volatile uint32_t WDOG_CTRL_REG;                  /*!< Offset 0x0B0 Watchdog Control Register */
    volatile uint32_t WDOG_CFG_REG;                   /*!< Offset 0x0B4 Watchdog Configuration Register */
    volatile uint32_t WDOG_MODE_REG;                  /*!< Offset 0x0B8 Watchdog Mode Register */
    volatile uint32_t WDOG_OUTPUT_CFG_REG;            /*!< Offset 0x0BC Watchdog Output Configuration Register */
    volatile uint32_t AVS_CNT_CTL_REG;                /*!< Offset 0x0C0 AVS Control Register */
    volatile uint32_t AVS_CNT0_REG;                   /*!< Offset 0x0C4 AVS Counter 0 Register */
    volatile uint32_t AVS_CNT1_REG;                   /*!< Offset 0x0C8 AVS Counter 1 Register */
    volatile uint32_t AVS_CNT_DIV_REG;                /*!< Offset 0x0CC AVS Divisor Register */
    volatile uint32_t CNT64_TEST_REG;                 /*!< Offset 0x0D0 64-bit Counter Test Mode Register */
    volatile uint32_t CNT64_CTRL_REG;                 /*!< Offset 0x0D4 64-bit Counter Control Register */
    volatile uint32_t CNT64_LOW_REG;                  /*!< Offset 0x0D8 64-bit Counter Low Register */
    volatile uint32_t CNT64_HIGH_REG;                 /*!< Offset 0x0DC 64-bit Counter High Register */
             uint32_t reserved_0x0E0 [0x00C8];
} TIMER_TypeDef; /* size of structure = 0x400 */
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
             uint32_t reserved_0x024 [0x00F7];
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
             uint32_t reserved_0x0B8 [0x000E];
    volatile uint32_t UART_A_FCC;                     /*!< Offset 0x0F0 UART FIFO Clock Control Register */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t UART_A_RXDMA_CTRL;              /*!< Offset 0x100 UART RXDMA Control Register */
    volatile uint32_t UART_A_RXDMA_STR;               /*!< Offset 0x104 UART RXDMA Start Register */
    volatile uint32_t UART_A_RXDMA_STA;               /*!< Offset 0x108 UART RXDMA Status Register */
    volatile uint32_t UART_A_RXDMA_LMT;               /*!< Offset 0x10C UART RXDMA Limit Register */
    volatile uint32_t UART_A_RXDMA_SADDRL;            /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
    volatile uint32_t UART_A_RXDMA_SADDRH;            /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
    volatile uint32_t UART_A_RXDMA_BL;                /*!< Offset 0x118 UART RXDMA Buffer Length Register */
             uint32_t reserved_0x11C;
    volatile uint32_t UART_A_RXDMA_IE;                /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
    volatile uint32_t UART_A_RXDMA_IS;                /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
    volatile uint32_t UART_A_RXDMA_WADDRL;            /*!< Offset 0x128 UART RXDMA Write Address Low Register */
    volatile uint32_t UART_A_RXDMA_WADDRH;            /*!< Offset 0x12C UART RXDMA Write Address high Register */
    volatile uint32_t UART_A_RXDMA_RADDRL;            /*!< Offset 0x130 UART RXDMA Read Address Low Register */
    volatile uint32_t UART_A_RXDMA_RADDRH;            /*!< Offset 0x134 UART RXDMA Read Address high Register */
    volatile uint32_t UART_A_RXDMA_DCNT;              /*!< Offset 0x138 UART RADMA Data Count Register */
             uint32_t reserved_0x13C [0x00B1];
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
/*!< USBOTG  */
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
/*!< USBPHYC  */
typedef struct USBPHYC_Type
{
    volatile uint32_t USB_CTRL;                       /*!< Offset 0x000 HCI Interface Register (HCI_Interface) */
    volatile uint32_t USBPHY_PHYCTL;                  /*!< Offset 0x004 USBPHY_PHYCTL */
    volatile uint32_t HCI_CTRL3;                      /*!< Offset 0x008 HCI Control 3 Register (bist) */
             uint32_t reserved_0x00C;
    volatile uint32_t PHY_CTRL;                       /*!< Offset 0x010 PHY Control Register (PHY_Control) */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t PHY_OTGCTL;                     /*!< Offset 0x020 Control PHY routing to EHCI or OTG */
    volatile uint32_t PHY_STATUS;                     /*!< Offset 0x024 PHY Status Register */
    volatile uint32_t USB_SPDCR;                      /*!< Offset 0x028 HCI SIE Port Disable Control Register */
} USBPHYC_TypeDef; /* size of structure = 0x02C */
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability  */
typedef struct USB_EHCI_Capability_Type
{
    const volatile uint32_t HCCAPBASE;                /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
    const volatile uint32_t HCSPARAMS;                /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    const volatile uint32_t HCCPARAMS;                /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
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
/*
 * @brief VE
 */
/*!< VE Video Encoding */
typedef struct VE_Type
{
    volatile uint32_t VE_CTRL;                        /*!< Offset 0x000 Sub-Engine Select and RAM type select */
    volatile uint32_t VE_RESET;                       /*!< Offset 0x004 Sub-Engines Reset */
    volatile uint32_t VE_CYCLES_COUNTER;              /*!< Offset 0x008 Clock Cycles counter */
    volatile uint32_t VE_TIMEOUT;                     /*!< Offset 0x00C VE Timeout value */
    volatile uint32_t VE_MMCREQ_WNUM;                 /*!< Offset 0x010  */
    volatile uint32_t VE_CACHEREG_WNUM;               /*!< Offset 0x014  */
             uint32_t reserved_0x018;
    volatile uint32_t VE_STATUS;                      /*!< Offset 0x01C Busy status */
    volatile uint32_t VE_RDDATA_COUNTER;              /*!< Offset 0x020 DRAM Read counter */
    volatile uint32_t VE_WRDATA_COUNTER;              /*!< Offset 0x024 DRAM Write counter */
    volatile uint32_t VE_ANAGLYPH_CTRL;               /*!< Offset 0x028 Anaglyph mode control */
             uint32_t reserved_0x02C;
    volatile uint32_t VE_MAF_CTRL;                    /*!< Offset 0x030 Motion adaptive filter config */
    volatile uint32_t VE_MAF_CLIP_TH;                 /*!< Offset 0x034  */
    volatile uint32_t VE_MAFREF1_LUMA_BUF;            /*!< Offset 0x038 Reference luma buffer {unsure} */
    volatile uint32_t VE_MAFREF1_CHROMA_BUF;          /*!< Offset 0x03C Reference chroma buffer {unsure} */
    volatile uint32_t VE_MAFCUR_ADDR;                 /*!< Offset 0x040 current maf output address {unsure} */
    volatile uint32_t VE_MAFREF1_ADDR;                /*!< Offset 0x044 reference maf input address {unsure} */
    volatile uint32_t VE_MAFREF2_ADDR;                /*!< Offset 0x048 second reference maf input address {unsure} */
    volatile uint32_t VE_MAFDIFF_GROUP_MAX;           /*!< Offset 0x04C  */
    volatile uint32_t VE_IPD_DBLK_BUF_CTRL;           /*!< Offset 0x050 deblocking and intra prediction dram buffer config register (required for A13+ SoC for H264 decoding or on A10 for video with width >= 2048) */
    volatile uint32_t VE_IPD_BUF;                     /*!< Offset 0x054 Intra prediction buffer (needed on A13+ or (width >= 2048)) */
    volatile uint32_t VE_DBLK_BUF;                    /*!< Offset 0x058 Deblocking buffer (needed on A13+ or (width >= 2048)) */
    volatile uint32_t VE_ARGB_QUEUE_START;            /*!< Offset 0x05C ARGB command queue */
    volatile uint32_t VE_ARGB_BLK_SRC1_ADDR;          /*!< Offset 0x060 ARGB source 1 address */
    volatile uint32_t VE_ARGB_BLK_SRC2_ADDR;          /*!< Offset 0x064 ARGB source 2 addres */
    volatile uint32_t VE_ARGB_BLK_DST_ADDR;           /*!< Offset 0x068 ARGB destination address */
    volatile uint32_t VE_ARGB_SRC_STRIDE;             /*!< Offset 0x06C ARGB source strides for src1 and src2 */
    volatile uint32_t VE_ARGB_DST_STRIDE;             /*!< Offset 0x070 ARGB destination stride */
    volatile uint32_t VE_ARGB_BLK_SIZE;               /*!< Offset 0x074 ARGB size */
    volatile uint32_t VE_ARGB_BLK_FILL_VALUE;         /*!< Offset 0x078 ARGB fill value */
    volatile uint32_t VE_ARGB_BLK_CTRL;               /*!< Offset 0x07C ARGB control */
    volatile uint32_t VE_LUMA_HIST_THR [0x004];       /*!< Offset 0x080 Luma histogram thresholds [0-3] */
    volatile uint32_t VE_LUMA_HIST_VAL [0x010];       /*!< Offset 0x090 Luma histogram output values [0-15] */
    volatile uint32_t VE_ANGL_R_BUF;                  /*!< Offset 0x0D0 Anaglyph red output buffer */
    volatile uint32_t VE_ANGL_G_BUF;                  /*!< Offset 0x0D4 Anaglyph green output buffer */
    volatile uint32_t VE_ANGL_B_BUF;                  /*!< Offset 0x0D8 Anaglyph blue output buffer */
             uint32_t reserved_0x0DC [0x0003];
    volatile uint32_t VE_EXTRA_OUT_FMT_OFFSET;        /*!< Offset 0x0E8 Extra output format and chroma offset (not available on A10/A13/A20) */
    volatile uint32_t VE_OUTPUT_FORMAT;               /*!< Offset 0x0EC Output formats (since H3?) */
    volatile uint32_t VE_VERSION;                     /*!< Offset 0x0F0 IP Version register */
             uint32_t reserved_0x0F4;
    volatile uint32_t VE_DBG_CTRL;                    /*!< Offset 0x0F8 Debug control */
    volatile uint32_t VE_DBG_OUTPUT;                  /*!< Offset 0x0FC Debug output */
    volatile uint32_t MPEG_PHDR;                      /*!< Offset 0x100 MPEG12 Picture Header register */
    volatile uint32_t MPEG_VOPHDR;                    /*!< Offset 0x104 MPEG Video Object Plane Header register (MPEG4 Header) */
    volatile uint32_t MPEG_SIZE;                      /*!< Offset 0x108 Frame size in MPEG macroblocks (16x16) */
    volatile uint32_t MPEG_FRAME_SIZE;                /*!< Offset 0x10C Frame size in pixels */
    volatile uint32_t MPEG_MBA;                       /*!< Offset 0x110 MPEG Macro Block Address register */
    volatile uint32_t MPEG_CTRL;                      /*!< Offset 0x114 MPEG Control Register */
    volatile uint32_t MPEG_TRIG;                      /*!< Offset 0x118 MPEG Decoding Trigger */
    volatile uint32_t MPEG_STATUS;                    /*!< Offset 0x11C MACC MPEG Status register */
    volatile uint32_t MPEG_FRAME_DIST;                /*!< Offset 0x120 MPEG P and B Frame distance */
    volatile uint32_t MPEG_TRBTRDFLD;                 /*!< Offset 0x124 Temporal References(TRB(B-VOP) and TRD) */
    volatile uint32_t MPEG_VLD_ADDR;                  /*!< Offset 0x128 MPEG Variable Length Decoding Address */
    volatile uint32_t MPEG_VLD_OFFSET;                /*!< Offset 0x12C MPEG Variable Length Decoding Offset */
    volatile uint32_t MPEG_VLD_LEN;                   /*!< Offset 0x130 MPEG Variable Length Decoding Length */
    volatile uint32_t MPEG_VBV_END;                   /*!< Offset 0x134 MPEG VBV end - video source buffer end */
    volatile uint32_t MPEG_MBH_ADDR;                  /*!< Offset 0x138 MBH buffer address */
    volatile uint32_t MPEG_DCAC_ADDR;                 /*!< Offset 0x13C DCAC Buffer address */
    volatile uint32_t MPEG_BLK_OFFSET;                /*!< Offset 0x140 MPEG Block address??? */
    volatile uint32_t MPEG_NCF_ADDR;                  /*!< Offset 0x144 NFC buffer address */
    volatile uint32_t MPEG_REC_LUMA;                  /*!< Offset 0x148 MPEG Luma reconstruct buffer */
    volatile uint32_t MPEG_REC_CHROMA;                /*!< Offset 0x14C MPEG Chroma reconstruct buffer */
    volatile uint32_t MPEG_FWD_LUMA;                  /*!< Offset 0x150 MPEG Luma forward buffer */
    volatile uint32_t MPEG_FWD_CHROMA;                /*!< Offset 0x154 MPEG forward buffer */
    volatile uint32_t MPEG_BACK_LUMA;                 /*!< Offset 0x158 MPEG Luma Back buffer */
    volatile uint32_t MPEG_BACK_CHROMA;               /*!< Offset 0x15C MPEG Chroma Back buffer */
    volatile uint32_t MPEG_SOCX;                      /*!< Offset 0x160 MS-MPEG related */
    volatile uint32_t MPEG_SOCY;                      /*!< Offset 0x164 MS-MPEG related */
    volatile uint32_t MPEG_SOL;                       /*!< Offset 0x168 MS-MPEG related */
    volatile uint32_t MPEG_SDLX;                      /*!< Offset 0x16C MS-MPEG related */
    volatile uint32_t MPEG_SDLY;                      /*!< Offset 0x170 MS-MPEG related */
    volatile uint32_t MPEG_SPRITESHFT;                /*!< Offset 0x174 MS-MPEG related */
    volatile uint32_t MPEG_SDCX;                      /*!< Offset 0x178 MS-MPEG related */
    volatile uint32_t MPEG_SDCY;                      /*!< Offset 0x17C MS-MPEG related */
    volatile uint32_t MPEG_IQ_MIN_INPUT;              /*!< Offset 0x180 MPEG Inverse Quantization minimum input level */
    volatile uint32_t MPEG_IQ_INPUT;                  /*!< Offset 0x184 MPEG Inverse Quantization input level */
    volatile uint32_t MPEG_MSMPEG4_HDR;               /*!< Offset 0x188 MPEG MS-Mpeg-4 header */
    volatile uint32_t MPEG_VP6_HDR;                   /*!< Offset 0x18C MPEG VP6 Header */
    volatile uint32_t MPEG_IQ_IDCT_INPUT;             /*!< Offset 0x190 MPEG Inverse Quantization and Inverse Discrete Cosine Transform input */
    volatile uint32_t MPEG_MB_HEIGHT;                 /*!< Offset 0x194 MPEG Macro Block Height */
    volatile uint32_t MPEG_MB_V1;                     /*!< Offset 0x198 MPEG Macro Block Vector 1 */
    volatile uint32_t MPEG_MB_V2;                     /*!< Offset 0x19C MPEG Macro Block Vector 2 */
    volatile uint32_t MPEG_MB_V3;                     /*!< Offset 0x1A0 MPEG Macro Block Vector 3 */
    volatile uint32_t MPEG_MB_V4;                     /*!< Offset 0x1A4 MPEG Macro Block Vector 4 */
    volatile uint32_t MPEG_MB_V5;                     /*!< Offset 0x1A8 MPEG Macro Block Vector 5 */
    volatile uint32_t MPEG_MB_V6;                     /*!< Offset 0x1AC MPEG Macro Block Vector 6 */
    volatile uint32_t MPEG_MB_V7;                     /*!< Offset 0x1B0 MPEG Macro Block Vector 7 */
    volatile uint32_t MPEG_MB_V8;                     /*!< Offset 0x1B4 MPEG Macro Block Vector 8 */
    volatile uint32_t MPEG_JPEG_SIZE;                 /*!< Offset 0x1B8 JPEG Size */
    volatile uint32_t MPEG_JPEG_MCU;                  /*!< Offset 0x1BC JPEG Minimum Coded Unit */
    volatile uint32_t MPEG_JPEG_RES_INT;              /*!< Offset 0x1C0 JPEG Restart Interval */
    volatile uint32_t MPEG_ERROR;                     /*!< Offset 0x1C4 MPEG Error flags */
    volatile uint32_t MPEG_CTR_MB;                    /*!< Offset 0x1C8 (Macroblock Control??) */
    volatile uint32_t MPEG_ROT_LUMA;                  /*!< Offset 0x1CC MPEG Rotate-Scale Luma buffer */
    volatile uint32_t MPEG_ROT_CHROMA;                /*!< Offset 0x1D0 MPEG Rotate-Scale Chroma buffer */
    volatile uint32_t MPEG_ROTSCALE_CTRL;             /*!< Offset 0x1D4 Control Rotate/Scale Buffer */
    volatile uint32_t MPEG_JPEG_MCU_START;            /*!< Offset 0x1D8 JPEG Macro Cell Unit Start */
    volatile uint32_t MPEG_JPEG_MCU_END;              /*!< Offset 0x1DC JPEG Macro Cell Unit End */
    volatile uint32_t MPEG_SRAM_RW_OFFSET;            /*!< Offset 0x1E0 Auto incremental pointer for read/write VE SRAM */
    volatile uint32_t MPEG_SRAM_RW_DATA;              /*!< Offset 0x1E4 FIFO Like Data register for write/read VE SRAM */
             uint32_t reserved_0x1E8 [0x0002];
    volatile uint32_t MPEG_START_CODE_BITOFFSET;      /*!< Offset 0x1F0 MPEG start code search result */
             uint32_t reserved_0x1F4 [0x0003];
    volatile uint32_t H264_SEQ_HDR;                   /*!< Offset 0x200 H264 Sequence header */
    volatile uint32_t H264_PIC_HDR;                   /*!< Offset 0x204 H264 Picture header */
    volatile uint32_t H264_SLICE_HDR;                 /*!< Offset 0x208 H264 Slice header */
    volatile uint32_t H264_SLICE_HDR2;                /*!< Offset 0x20C H264 Slice header */
    volatile uint32_t H264_PRED_WEIGHT;               /*!< Offset 0x210 H264 weighted prediction parameters */
    volatile uint32_t H264_VP8_HDR;                   /*!< Offset 0x214 H264 VP8 Picture header */
    volatile uint32_t H264_QINDEX;                    /*!< Offset 0x218 H264 Quantizer settings (VP8) */
    volatile uint32_t H264_VP8_PART_OFFSET_H264_QP;   /*!< Offset 0x21C H264 QP parameters (VP8 partition offset) */
    volatile uint32_t H264_CTRL;                      /*!< Offset 0x220 H264 Control Register */
    volatile uint32_t H264_TRIG;                      /*!< Offset 0x224 H264 Trigger Register */
    volatile uint32_t H264_STATUS;                    /*!< Offset 0x228 H264 Status Register */
    volatile uint32_t H264_CUR_MBNUM;                 /*!< Offset 0x22C H264 current Macroblock */
    volatile uint32_t H264_VLD_ADDR;                  /*!< Offset 0x230 H264 Variable Length Decoder Address */
    volatile uint32_t H264_VLD_OFFSET;                /*!< Offset 0x234 H264 Variable Length Decoder Bit Offset */
    volatile uint32_t H264_VLD_LEN;                   /*!< Offset 0x238 H264 Variable Length Decoder Bit Length */
    volatile uint32_t H264_VLD_END;                   /*!< Offset 0x23C H264 Variable Length Decoder End Address */
    volatile uint32_t H264_SDROT_CTRL;                /*!< Offset 0x240 H264 Scale Rotate buffer control */
    volatile uint32_t H264_SDROT_LUMA;                /*!< Offset 0x244 H264 Scale Rotate buffer Luma color component */
    volatile uint32_t H264_SDROT_CHROMA;              /*!< Offset 0x248 H264 Scale Rotate buffer Chroma color component */
    volatile uint32_t H264_OUTPUT_FRAME_INDEX;        /*!< Offset 0x24C H264 output frame index in dpb */
    volatile uint32_t H264_FIELD_INTRA_INFO_BUF_H264_VP8_ENTROPY_PROBS;/*!< Offset 0x250 H264 field intra info buffer address (VP8 entropy brobabilities table address) */
    volatile uint32_t H264_NEIGHBOR_INFO_BUF_H264_VP8_FSTDATA_PARTLEN;/*!< Offset 0x254 H264 neighbor info buffer address (VP8 First partition length) */
    volatile uint32_t H264_PIC_MBSIZE;                /*!< Offset 0x258 H264 Picture size in macroblocks */
    volatile uint32_t H264_PIC_BOUNDARYSIZE;          /*!< Offset 0x25C H264 Picture size in pixels */
    volatile uint32_t H264_MB_ADDR;                   /*!< Offset 0x260 H264 Current macroblock position */
    volatile uint32_t H264_MB_NB1;                    /*!< Offset 0x264 H264 ??? MbNeightbour1 */
    volatile uint32_t H264_MB_NB2;                    /*!< Offset 0x268 H264 MbNeightbour2 */
    volatile uint32_t H264_MB_NB3;                    /*!< Offset 0x26C H264 ??? */
    volatile uint32_t H264_MB_NB4;                    /*!< Offset 0x270 H264 ??? */
    volatile uint32_t H264_MB_NB5;                    /*!< Offset 0x274 H264 ??? */
    volatile uint32_t H264_MB_NB6;                    /*!< Offset 0x278 H264 ??? */
    volatile uint32_t H264_MB_NB7;                    /*!< Offset 0x27C H264 ??? */
    volatile uint32_t H264_MB_NB8;                    /*!< Offset 0x280 H264 ??? */
    volatile uint32_t H264_0x0284;                    /*!< Offset 0x284 H264 ??? */
    volatile uint32_t H264_0x0288;                    /*!< Offset 0x288 H264 ??? */
    volatile uint32_t H264_0x028c;                    /*!< Offset 0x28C H264 ??? */
    volatile uint32_t H264_MB_QP;                     /*!< Offset 0x290 H264 ??? */
    volatile uint32_t H264_0x0294;                    /*!< Offset 0x294 H264 ??? */
    volatile uint32_t H264_0x0298;                    /*!< Offset 0x298 H264 ??? */
    volatile uint32_t H264_0x029c;                    /*!< Offset 0x29C H264 ??? */
    volatile uint32_t H264_0x02a0;                    /*!< Offset 0x2A0 H264 ??? */
    volatile uint32_t H264_0x02a4;                    /*!< Offset 0x2A4 H264 ??? */
    volatile uint32_t H264_0x02a8;                    /*!< Offset 0x2A8 H264 ??? */
    volatile uint32_t H264_REC_LUMA;                  /*!< Offset 0x2AC H264 Luma reconstruct buffer */
    volatile uint32_t H264_FWD_LUMA;                  /*!< Offset 0x2B0 H264 Luma forward buffer */
    volatile uint32_t H264_BACK_LUMA;                 /*!< Offset 0x2B4 H264 Luma back buffer */
    volatile uint32_t H264_ERROR;                     /*!< Offset 0x2B8 H264 Error */
    volatile uint32_t H264_0x02bc;                    /*!< Offset 0x2BC H264 ??? */
    volatile uint32_t H264_0x02c0;                    /*!< Offset 0x2C0 H264 ??? */
    volatile uint32_t H264_0x02c4;                    /*!< Offset 0x2C4 H264 ??? */
    volatile uint32_t H264_0x02c8;                    /*!< Offset 0x2C8 H264 ??? */
    volatile uint32_t H264_0x02cc;                    /*!< Offset 0x2CC H264 ??? */
    volatile uint32_t H264_REC_CHROMA;                /*!< Offset 0x2D0 H264 Chroma reconstruct buffer */
    volatile uint32_t H264_FWD_CHROMA;                /*!< Offset 0x2D4 H264 Chroma forward buffer */
    volatile uint32_t H264_BACK_CHROMA;               /*!< Offset 0x2D8 H264 Chroma back buffer */
    volatile uint32_t H264_BASIC_BITS_DATA;           /*!< Offset 0x2DC H264 Basic bits data */
    volatile uint32_t H264_RAM_WRITE_PTR;             /*!< Offset 0x2E0 H264 ram write pointer */
    volatile uint32_t H264_RAM_WRITE_DATA;            /*!< Offset 0x2E4 H264 ram write data */
    volatile uint32_t H264_ALT_LUMA;                  /*!< Offset 0x2E8 H264 Alternate Luma buffer */
    volatile uint32_t H264_ALT_CHROMA;                /*!< Offset 0x2EC H264 Alternate Chroma buffer */
    volatile uint32_t H264_SEG_MB_LV0;                /*!< Offset 0x2F0 H264 ??? Segment Mb Level 0 */
    volatile uint32_t H264_SEG_MB_LV1;                /*!< Offset 0x2F4 H264 ??? Segment Mb Level 1 */
    volatile uint32_t H264_REF_LF_DELTA;              /*!< Offset 0x2F8 H264 ??? (VP8 ref lf deltas) */
    volatile uint32_t H264_MODE_LF_DELTA;             /*!< Offset 0x2FC H264 ??? (VP8 mode lf deltas) */
    volatile uint32_t VC1_EPHS;                       /*!< Offset 0x300 VC1 ??? */
    volatile uint32_t VC1_PIC_CTRL;                   /*!< Offset 0x304 VC1 ??? */
    volatile uint32_t VC1_PIC_QP;                     /*!< Offset 0x308 VC1 ??? */
    volatile uint32_t VC1_PIC_MV;                     /*!< Offset 0x30C VC1 ??? */
    volatile uint32_t VC1_PIC_INTEN_COMP;             /*!< Offset 0x310 VC1 ??? */
    volatile uint32_t VC1_PIC_INTERLANCE;             /*!< Offset 0x314 VC1 ??? */
    volatile uint32_t VC1_HDR_LEN;                    /*!< Offset 0x318 VC1 ??? */
    volatile uint32_t VC1_FSIZE;                      /*!< Offset 0x31C VC1 ??? */
    volatile uint32_t VC1_PIC_SIZE;                   /*!< Offset 0x320 VC1 ??? */
    volatile uint32_t VC1_CTRL;                       /*!< Offset 0x324 VC1 Decoder Control */
    volatile uint32_t VC1_START_TYPE;                 /*!< Offset 0x328 VC1 ??? */
    volatile uint32_t VC1_STATUS;                     /*!< Offset 0x32C VC1 Status */
    volatile uint32_t VC1_VBV_BASE_ADDR;              /*!< Offset 0x330 VC1 Source buffer address */
    volatile uint32_t VC1_VLD_OFFSET;                 /*!< Offset 0x334 VC1 Variable Length Decoder Offset */
    volatile uint32_t VC1_VBV_LEN;                    /*!< Offset 0x338 VC1 length of source video buffer */
    volatile uint32_t VC1_VBV_END_ADDR;               /*!< Offset 0x33C VC1 last address of source video buffer */
    volatile uint32_t VC1_REC_FRAME_CHROMA;           /*!< Offset 0x340 VC1 Chroma Reconstruct frame */
    volatile uint32_t VC1_REC_FRAME_LUMA;             /*!< Offset 0x344 VC1 Luma Reconstruct frame */
    volatile uint32_t VC1_FWD_FRAME_CHROMA;           /*!< Offset 0x348 VC1 Chroma Forward Frame */
    volatile uint32_t VC1_FWD_FRAME_LUMA;             /*!< Offset 0x34C VC1 Luma Forward Frame */
    volatile uint32_t VC1_BACK_CHROMA;                /*!< Offset 0x350 VC1 Chroma back buffer */
    volatile uint32_t VC1_BACK_LUMA;                  /*!< Offset 0x354 VC1 Luma back buffer */
    volatile uint32_t VC1_MBHADDR;                    /*!< Offset 0x358 VC1 ??? */
    volatile uint32_t VC1_DCAPRED_ADDR;               /*!< Offset 0x35C VC1 ??? */
    volatile uint32_t VC1_BITPLANE_ADDR;              /*!< Offset 0x360 VC1 ??? */
    volatile uint32_t VC1_MBINFO_ADDR;                /*!< Offset 0x364 VC1 ???(or COLMVINFOADDR) */
    volatile uint32_t VC1_0x0368;                     /*!< Offset 0x368 VC1 ??? */
    volatile uint32_t VC1_0x036c;                     /*!< Offset 0x36C VC1 ??? */
    volatile uint32_t VC1_MBA;                        /*!< Offset 0x370 VC1 ??? */
    volatile uint32_t VC1_MBHDR;                      /*!< Offset 0x374 VC1 ??? */
    volatile uint32_t VC1_LUMA_TRANSFORM;             /*!< Offset 0x378 VC1 ??? */
    volatile uint32_t VC1_MBCBF;                      /*!< Offset 0x37C VC1 ??? */
    volatile uint32_t VC1_MBM_V1;                     /*!< Offset 0x380 VC1 ??? */
    volatile uint32_t VC1_MBM_V2;                     /*!< Offset 0x384 VC1 ??? */
    volatile uint32_t VC1_MBM_V3;                     /*!< Offset 0x388 VC1 ??? */
    volatile uint32_t VC1_MBM_V4;                     /*!< Offset 0x38C VC1 ??? */
    volatile uint32_t VC1_MBM_V5;                     /*!< Offset 0x390 VC1 ??? */
    volatile uint32_t VC1_MBM_V6;                     /*!< Offset 0x394 VC1 ??? */
    volatile uint32_t VC1_MBM_V7;                     /*!< Offset 0x398 VC1 ??? */
    volatile uint32_t VC1_MBM_V8;                     /*!< Offset 0x39C VC1 ??? */
    volatile uint32_t VC1_0x03a0;                     /*!< Offset 0x3A0 VC1 ??? */
    volatile uint32_t VC1_0x03a4;                     /*!< Offset 0x3A4 VC1 ??? */
    volatile uint32_t VC1_0x03a8;                     /*!< Offset 0x3A8 VC1 ??? */
    volatile uint32_t VC1_0x03ac;                     /*!< Offset 0x3AC VC1 ??? */
    volatile uint32_t VC1_0x03b0;                     /*!< Offset 0x3B0 VC1 ??? */
    volatile uint32_t VC1_0x03b4;                     /*!< Offset 0x3B4 VC1 ??? */
    volatile uint32_t VC1_ERROR;                      /*!< Offset 0x3B8 VC1 Error result code */
    volatile uint32_t VC1_CRT_MB_NUM;                 /*!< Offset 0x3BC VC1 ??? */
    volatile uint32_t VC1_EXTRA_CTRL;                 /*!< Offset 0x3C0 VC1 ??? */
    volatile uint32_t VC1_EXTRA_CBUF_ADDR;            /*!< Offset 0x3C4 VC1 EXTRA Chroma DRAM address */
    volatile uint32_t VC1_EXTRA_YBUF_ADDR;            /*!< Offset 0x3C8 VC1 EXTRA Luma DRAM address */
             uint32_t reserved_0x3CC;
    volatile uint32_t VC1_OVERLAP_UP_ADDR;            /*!< Offset 0x3D0 VC1 ??? */
    volatile uint32_t VC1_DBLK_ABOVE_ADDR;            /*!< Offset 0x3D4 VC1 ??? */
    volatile uint32_t VC1_0x03d8;                     /*!< Offset 0x3D8 VC1 ??? */
    volatile uint32_t VC1_BITS_RETDATA;               /*!< Offset 0x3DC VC1 ??? */
             uint32_t reserved_0x3E0 [0x0007];
    volatile uint32_t VC1_DEBUG_BUF_ADDR;             /*!< Offset 0x3FC VC1 ??? */
    volatile uint32_t RMVB_SLC_HDR;                   /*!< Offset 0x400 Header */
    volatile uint32_t RMVB_FRM_SIZE;                  /*!< Offset 0x404 Framesize (in macroblocks ?) */
    volatile uint32_t RMVB_DIR_MODE_RATIO;            /*!< Offset 0x408  */
    volatile uint32_t RMVB_DIR_MB_ADDR;               /*!< Offset 0x40C  */
    volatile uint32_t RMVB_QC_INPUT;                  /*!< Offset 0x410  */
    volatile uint32_t RMVB_CTRL;                      /*!< Offset 0x414 RMVB IRQ Control */
    volatile uint32_t RMVB_TRIG;                      /*!< Offset 0x418 Trigger register */
    volatile uint32_t RMVB_STATUS;                    /*!< Offset 0x41C RMVB Status */
             uint32_t reserved_0x420 [0x0002];
    volatile uint32_t RMVB_VBV_BASE;                  /*!< Offset 0x428 Video source buffer base */
    volatile uint32_t RMVB_VLD_OFFSET;                /*!< Offset 0x42C Video source buffer DRAM address */
    volatile uint32_t RMVB_VLD_LEN;                   /*!< Offset 0x430 Video source buffer length in bytes */
    volatile uint32_t RMVB_VBV_END;                   /*!< Offset 0x434 Video source buffer last DRAM address */
             uint32_t reserved_0x438;
    volatile uint32_t RMVB_HUFF_TABLE_ADDR;           /*!< Offset 0x43C Huffman table DRAM address */
    volatile uint32_t RMVB_CUR_Y_ADDR;                /*!< Offset 0x440 Luma Current buffer DRAM address */
    volatile uint32_t RMVB_CUR_C_ADDR;                /*!< Offset 0x444 Chroma Current buffer DRAM address */
    volatile uint32_t RMVB_FOR_Y_ADDR;                /*!< Offset 0x448 Luma Forward buffer DRAM address */
    volatile uint32_t RMVB_FOR_C_ADDR;                /*!< Offset 0x44C Chroma Forward buffer DRAM address */
    volatile uint32_t RMVB_BAC_Y_ADDR;                /*!< Offset 0x450 Luma Back buffer DRAM address */
    volatile uint32_t RMVB_BAC_C_ADDR;                /*!< Offset 0x454 Chroma Back buffer DRAM address */
    volatile uint32_t RMVB_ROT_Y_ADDR;                /*!< Offset 0x458 Luma Rot buffer DRAM address */
    volatile uint32_t RMVB_ROT_C_ADDR;                /*!< Offset 0x45C Chroma Rot Buffer DRAM address */
    volatile uint32_t RMVB_MBH_ADDR;                  /*!< Offset 0x460  */
    volatile uint32_t RMVB_MV_ADDR;                   /*!< Offset 0x464  */
             uint32_t reserved_0x468 [0x0002];
    volatile uint32_t RMVB_MBH_INFO;                  /*!< Offset 0x470  */
    volatile uint32_t RMVB_MV0;                       /*!< Offset 0x474 Mountion vector 0 */
    volatile uint32_t RMVB_MV1;                       /*!< Offset 0x478 Mountion vector 1 */
    volatile uint32_t RMVB_MV2;                       /*!< Offset 0x47C Mountion vector 2 */
    volatile uint32_t RMVB_MV3;                       /*!< Offset 0x480 Mountion vector 3 */
             uint32_t reserved_0x484 [0x0003];
    volatile uint32_t RMVB_DBLK_COEF;                 /*!< Offset 0x490  */
             uint32_t reserved_0x494 [0x0007];
    volatile uint32_t RMVB_ERROR;                     /*!< Offset 0x4B0 Decode error result code */
             uint32_t reserved_0x4B4;
    volatile uint32_t RMVB_BITS_DATA;                 /*!< Offset 0x4B8  */
             uint32_t reserved_0x4BC;
    volatile uint32_t RMVB_SLC_QUEUE_ADDR;            /*!< Offset 0x4C0  */
    volatile uint32_t RMVB_SLC_QUEUE_LEN;             /*!< Offset 0x4C4  */
    volatile uint32_t RMVB_SLC_QUEUE_TRIG;            /*!< Offset 0x4C8  */
    volatile uint32_t RMVB_SLC_QUEUE_STATUS;          /*!< Offset 0x4CC  */
    volatile uint32_t RMVB_SCALE_ROT_CTRL;            /*!< Offset 0x4D0  */
             uint32_t reserved_0x4D4 [0x0003];
    volatile uint32_t RMVB_SRAM_RW_OFFSET;            /*!< Offset 0x4E0 SRAM Fifo like index register */
    volatile uint32_t RMVB_SRAM_RW_DATA;              /*!< Offset 0x4E4 SRAM Fifo like data register */
             uint32_t reserved_0x4E8 [0x0006];
    volatile uint32_t HEVC_NAL_HDR;                   /*!< Offset 0x500 HEVC NAL header */
    volatile uint32_t HEVC_SPS;                       /*!< Offset 0x504 HEVC sequence parameter set */
    volatile uint32_t HEVC_PIC_SIZE;                  /*!< Offset 0x508 HEVC picture size */
    volatile uint32_t HEVC_PCM_HDR;                   /*!< Offset 0x50C HEVC PCM header */
    volatile uint32_t HEVC_PPS0;                      /*!< Offset 0x510 HEVC picture parameter set */
    volatile uint32_t HEVC_PPS1;                      /*!< Offset 0x514 HEVC picture parameter set */
    volatile uint32_t HEVC_SCALING_LIST_CTRL;         /*!< Offset 0x518 HEVC scaling list control register */
             uint32_t reserved_0x51C;
    volatile uint32_t HEVC_SLICE_HDR0;                /*!< Offset 0x520 HEVC slice header */
    volatile uint32_t HEVC_SLICE_HDR1;                /*!< Offset 0x524 HEVC slice header */
    volatile uint32_t HEVC_SLICE_HDR2;                /*!< Offset 0x528 HEVC slice header */
    volatile uint32_t HEVC_CTB_ADDR;                  /*!< Offset 0x52C HEVC CTB address */
    volatile uint32_t HEVC_CTRL;                      /*!< Offset 0x530 HEVC control register */
    volatile uint32_t HEVC_TRIG;                      /*!< Offset 0x534 HEVC trigger register */
    volatile uint32_t HEVC_STATUS;                    /*!< Offset 0x538 HEVC status register */
    volatile uint32_t HEVC_CTU_NUM;                   /*!< Offset 0x53C HEVC current CTU number */
    volatile uint32_t HEVC_BITS_ADDR;                 /*!< Offset 0x540 HEVC bitstream address */
    volatile uint32_t HEVC_BITS_OFFSET;               /*!< Offset 0x544 HEVC bitstream offset */
    volatile uint32_t HEVC_BITS_LEN;                  /*!< Offset 0x548 HEVC bitstream length */
    volatile uint32_t HEVC_BITS_END_ADDR;             /*!< Offset 0x54C HEVC bitstream end address */
    volatile uint32_t HEVC_EXTRA_OUT_CTRL;            /*!< Offset 0x550 HEVC extra output control register */
    volatile uint32_t HEVC_EXTRA_OUT_LUMA_ADDR;       /*!< Offset 0x554 HEVC extra output luma address */
    volatile uint32_t HEVC_EXTRA_OUT_CHROMA_ADDR;     /*!< Offset 0x558 HEVC extra output chroma address */
    volatile uint32_t HEVC_REC_BUF_IDX;               /*!< Offset 0x55C HEVC reconstruct buffer index */
    volatile uint32_t HEVC_NEIGHBOR_INFO_ADDR;        /*!< Offset 0x560 HEVC neighbor info buffer address */
    volatile uint32_t HEVC_TILE_LIST_ADDR;            /*!< Offset 0x564 HEVC tile entry point list address */
    volatile uint32_t HEVC_TILE_START_CTB;            /*!< Offset 0x568 HEVC tile start CTB */
    volatile uint32_t HEVC_TILE_END_CTB;              /*!< Offset 0x56C HEVC tile end CTB */
             uint32_t reserved_0x570 [0x0002];
    volatile uint32_t HEVC_SCALING_LIST_DC_COEF0;     /*!< Offset 0x578 HEVC scaling list DC coefficients */
    volatile uint32_t HEVC_SCALING_LIST_DC_COEF1;     /*!< Offset 0x57C HEVC scaling list DC coefficients */
             uint32_t reserved_0x580 [0x0017];
    volatile uint32_t HEVC_BITS_DATA;                 /*!< Offset 0x5DC HEVC bitstream data */
    volatile uint32_t HEVC_SRAM_ADDR;                 /*!< Offset 0x5E0 HEVC SRAM address */
    volatile uint32_t HEVC_SRAM_DATA;                 /*!< Offset 0x5E4 HEVC SRAM data */
             uint32_t reserved_0x5E8 [0x0106];
    volatile uint32_t ISP_PIC_SIZE;                   /*!< Offset 0xA00 ISP source picture size in macroblocks (16x16) */
    volatile uint32_t ISP_PIC_STRIDE;                 /*!< Offset 0xA04 ISP source picture stride */
    volatile uint32_t ISP_CTRL;                       /*!< Offset 0xA08 ISP IRQ Control */
    volatile uint32_t ISP_TRIG;                       /*!< Offset 0xA0C ISP Trigger */
             uint32_t reserved_0xA10 [0x0007];
    volatile uint32_t ISP_SCALER_SIZE;                /*!< Offset 0xA2C ISP scaler frame size/16 */
    volatile uint32_t ISP_SCALER_OFFSET_Y;            /*!< Offset 0xA30 ISP scaler picture offset for luma */
    volatile uint32_t ISP_SCALER_OFFSET_C;            /*!< Offset 0xA34 ISP scaler picture offset for chroma */
    volatile uint32_t ISP_SCALER_FACTOR;              /*!< Offset 0xA38 ISP scaler picture scale factor */
             uint32_t reserved_0xA3C [0x0002];
    volatile uint32_t ISP_BUF_0x0a44;                 /*!< Offset 0xA44 ISP PHY Buffer offset */
    volatile uint32_t ISP_BUF_0x0a48;                 /*!< Offset 0xA48 ISP PHY Buffer offset */
    volatile uint32_t ISP_BUF_0x0a4C;                 /*!< Offset 0xA4C ISP PHY Buffer offset */
             uint32_t reserved_0xA50 [0x0008];
    volatile uint32_t ISP_OUTPUT_LUMA;                /*!< Offset 0xA70 ISP Output LUMA Address */
    volatile uint32_t ISP_OUTPUT_CHROMA;              /*!< Offset 0xA74 ISP Output CHROMA Address */
    volatile uint32_t ISP_WB_THUMB_LUMA;              /*!< Offset 0xA78 ISP THUMB WriteBack PHY LUMA Address */
    volatile uint32_t ISP_WB_THUMB_CHROMA;            /*!< Offset 0xA7C ISP THUMB WriteBack PHY CHROMA Adress */
             uint32_t reserved_0xA80 [0x0018];
    volatile uint32_t ISP_SRAM_INDEX;                 /*!< Offset 0xAE0 ISP VE SRAM Index */
    volatile uint32_t ISP_SRAM_DATA;                  /*!< Offset 0xAE4 ISP VE SRAM Data */
             uint32_t reserved_0xAE8 [0x0006];
    volatile uint32_t AVC_PICINFO;                    /*!< Offset 0xB00 unk(not used in blob) */
    volatile uint32_t AVC_JPEG_CTRL_MACC_AVC_H264_CTRL;/*!< Offset 0xB04 jpeg / h264 different settings */
    volatile uint32_t AVC_H264_QP;                    /*!< Offset 0xB08 H264 quantization parameters */
             uint32_t reserved_0xB0C;
    volatile uint32_t AVC_H264_MOTION_EST;            /*!< Offset 0xB10 Motion estimation parameters */
    volatile uint32_t AVC_CTRL;                       /*!< Offset 0xB14 AVC Encoder IRQ Control */
    volatile uint32_t AVC_TRIG;                       /*!< Offset 0xB18 AVC Encoder trigger */
    volatile uint32_t AVC_STATUS;                     /*!< Offset 0xB1C AVC Encoder Busy Status */
    volatile uint32_t AVC_BITS_DATA;                  /*!< Offset 0xB20 AVC Encoder Bits Data */
             uint32_t reserved_0xB24 [0x000B];
    volatile uint32_t AVC_H264_MAD;                   /*!< Offset 0xB50 AVC H264 Encoder Mean Absolute Difference */
    volatile uint32_t AVC_H264_RESIDUAL_BITS;         /*!< Offset 0xB54 AVC H264 Encoder Residual Bits */
    volatile uint32_t AVC_H264_HEADER_BITS;           /*!< Offset 0xB58 AVC H264 Encoder Header Bits */
    volatile uint32_t AVC_H264_0x0b5c;                /*!< Offset 0xB5C AVC H264 Encoder unknown statistical data, maybe motion vectors */
    volatile uint32_t AVC_H264_0x0b60;                /*!< Offset 0xB60 AVC H264 Encoder unknown buffer */
             uint32_t reserved_0xB64 [0x0007];
    volatile uint32_t AVC_VLE_ADDR;                   /*!< Offset 0xB80 AVC Variable Length Encoder Start Address */
    volatile uint32_t AVC_VLE_END;                    /*!< Offset 0xB84 AVC Variable Length Encoder End Address */
    volatile uint32_t AVC_VLE_OFFSET;                 /*!< Offset 0xB88 AVC Variable Length Encoder Bit Offset */
    volatile uint32_t AVC_VLE_MAX;                    /*!< Offset 0xB8C AVC Variable Length Encoder Maximum Bits */
    volatile uint32_t AVC_VLE_LENGTH;                 /*!< Offset 0xB90 AVC Variable Length Encoder Bit Length */
             uint32_t reserved_0xB94 [0x0003];
    volatile uint32_t AVC_REF_LUMA;                   /*!< Offset 0xBA0 Luma reference buffer */
    volatile uint32_t AVC_REF_CHROMA;                 /*!< Offset 0xBA4 Chroma reference buffer */
             uint32_t reserved_0xBA8 [0x0002];
    volatile uint32_t AVC_REC_LUMA;                   /*!< Offset 0xBB0 Luma reconstruct buffer */
    volatile uint32_t AVC_REC_CHROMA;                 /*!< Offset 0xBB4 Chroma reconstruct buffer */
    volatile uint32_t AVC_REF_SLUMA;                  /*!< Offset 0xBB8 Smaller luma reference buffer ? */
    volatile uint32_t AVC_REC_SLUMA;                  /*!< Offset 0xBBC Smaller luma reconstruct buffer ? */
    volatile uint32_t AVC_MB_INFO;                    /*!< Offset 0xBC0 Temporary buffer with macroblock information */
             uint32_t reserved_0xBC4 [0x0007];
    volatile uint32_t AVC_SRAM_INDEX;                 /*!< Offset 0xBE0 AVC VE SRAM Index */
    volatile uint32_t AVC_SRAM_DATA;                  /*!< Offset 0xBE4 AVC VE SRAM Data */
             uint32_t reserved_0xBE8 [0x0506];
} VE_TypeDef; /* size of structure = 0x2000 */


/* Defines */



/* Access pointers */

#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)    /*!< SYS_CFG  register set access pointer */
#define DMAC ((DMAC_TypeDef *) DMAC_BASE)             /*!< DMAC  register set access pointer */
#define VENCODER ((VE_TypeDef *) VENCODER_BASE)       /*!< VENCODER Video Encoding register set access pointer */
#define USBOTG ((USBOTG_TypeDef *) USBOTG_BASE)       /*!< USBOTG  register set access pointer */
#define USBEHCI ((USB_EHCI_Capability_TypeDef *) USBEHCI_BASE)/*!< USBEHCI  register set access pointer */
#define USBOHCI ((USB_OHCI_Capability_TypeDef *) USBOHCI_BASE)/*!< USBOHCI  register set access pointer */
#define USBPHY ((USBPHYC_TypeDef *) USBPHY_BASE)      /*!< USBPHY  register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU  register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC  register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define PWM ((PWM_TypeDef *) PWM_BASE)                /*!< PWM  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define SPI ((SPI_TypeDef *) SPI_BASE)                /*!< SPI Serial Peripheral Interface register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
