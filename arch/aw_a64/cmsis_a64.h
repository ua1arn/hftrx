/* Generated section start */ 
#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* Peripheral and RAM base address */

#define	CCU_BASE	 ((uintptr_t) 0x01C20000)
#define	GPIOB_BASE	 ((uintptr_t) 0x01C20824)
#define	GPIOC_BASE	 ((uintptr_t) 0x01C20848)
#define	GPIOD_BASE	 ((uintptr_t) 0x01C2086C)
#define	GPIOE_BASE	 ((uintptr_t) 0x01C20890)
#define	GPIOF_BASE	 ((uintptr_t) 0x01C208B4)
#define	GPIOG_BASE	 ((uintptr_t) 0x01C208D8)
#define	GPIOH_BASE	 ((uintptr_t) 0x01C208FC)
#define	GPIOINTB_BASE	 ((uintptr_t) 0x01C20A00)
#define	GPIOINTC_BASE	 ((uintptr_t) 0x01C20A20)
#define	GPIOINTD_BASE	 ((uintptr_t) 0x01C20A40)
#define	GPIOINTE_BASE	 ((uintptr_t) 0x01C20A60)
#define	GPIOINTF_BASE	 ((uintptr_t) 0x01C20A80)
#define	GPIOINTG_BASE	 ((uintptr_t) 0x01C20AA0)
#define	GPIOINTH_BASE	 ((uintptr_t) 0x01C20AC0)
#define	I2S0_BASE	 ((uintptr_t) 0x01C22000)
#define	I2S1_BASE	 ((uintptr_t) 0x01C22400)
#define	I2S2_BASE	 ((uintptr_t) 0x01C22800)
#define	UART0_BASE	 ((uintptr_t) 0x01C28000)
#define	UART1_BASE	 ((uintptr_t) 0x01C28400)
#define	UART2_BASE	 ((uintptr_t) 0x01C28800)
#define	UART3_BASE	 ((uintptr_t) 0x01C28C00)
#define	UART4_BASE	 ((uintptr_t) 0x01C29000)
#define	TWI0_BASE	 ((uintptr_t) 0x01C2AC00)
#define	TWI1_BASE	 ((uintptr_t) 0x01C2B000)
#define	TWI2_BASE	 ((uintptr_t) 0x01C2B400)
#define	GPIOBLOCK_BASE	 ((uintptr_t) 0x02000000)
#define	CIR_TX_BASE	 ((uintptr_t) 0x02003000)
#define	LEDC_BASE	 ((uintptr_t) 0x02008000)
#define	GPADC_BASE	 ((uintptr_t) 0x02009000)
#define	TPADC_BASE	 ((uintptr_t) 0x02009C00)
#define	AUDIO_CODEC_BASE	 ((uintptr_t) 0x02030000)
#define	DMIC_BASE	 ((uintptr_t) 0x02031000)
#define	OWA_BASE	 ((uintptr_t) 0x02036000)
#define	SYS_CFG_BASE	 ((uintptr_t) 0x03000000)
#define	GIC_DISTRIBUTOR_BASE	 ((uintptr_t) 0x03021000)
#define	GIC_INTERFACE_BASE	 ((uintptr_t) 0x03022000)
#define	CE_NS_BASE	 ((uintptr_t) 0x03040000)
#define	CE_S_BASE	 ((uintptr_t) 0x03040800)
#define	SMHC0_BASE	 ((uintptr_t) 0x04020000)
#define	SMHC1_BASE	 ((uintptr_t) 0x04021000)
#define	SMHC2_BASE	 ((uintptr_t) 0x04022000)
#define	SPI0_BASE	 ((uintptr_t) 0x04025000)
#define	SPI_DBI_BASE	 ((uintptr_t) 0x04026000)
#define	CIR_RX_BASE	 ((uintptr_t) 0x07040000)
#define	CPU_SUBSYS_CTRL_BASE	 ((uintptr_t) 0x08100000)
/*
 * @brief CPU_SUBSYS_CTRL
 */
/*!< CPU_SUBSYS_CTRL Controller Interface */
typedef struct CPU_SUBSYS_CTRL_Type
{
    volatile uint32_t GENER_CTRL_REG0;                                          /*!< Offset 0x000 General Control Register0 */
             uint32_t reserved_0x004 [0x0002];
    volatile uint32_t GIC_JTAG_RST_CTRL;                                        /*!< Offset 0x00C GIC and JTAG Reset Control Register */
    volatile uint32_t C0_INT_EN;                                                /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
    volatile uint32_t RQ_FIQ_STATUS;                                            /*!< Offset 0x014 IRQ/FIQ Status Register */
    volatile uint32_t GENER_CTRL_REG2;                                          /*!< Offset 0x018 General Control Register2 */
    volatile uint32_t DBG_STATE;                                                /*!< Offset 0x01C Debug State Register */
} CPU_SUBSYS_CTRL_TypeDef; /* size of structure = 0x020 */
/*
 * @brief CCU
 */
/*!< CCU Controller Interface */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPUX_CTRL_REG;                                        /*!< Offset 0x000 PLL_CPUX Control Register  */
             uint32_t reserved_0x004;
    volatile uint32_t PLL_AUDIO_CTRL_REG;                                       /*!< Offset 0x008 PLL_AUDIO Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t PLL_VIDEO0_CTRL_REG;                                      /*!< Offset 0x010 PLL_VIDEO0 Control Register */
             uint32_t reserved_0x014;
    volatile uint32_t PLL_VE_CTRL_REG;                                          /*!< Offset 0x018 PLL_VE Control Register  */
             uint32_t reserved_0x01C;
    volatile uint32_t PLL_DDR0_CTRL_REG;                                        /*!< Offset 0x020 PLL_DDR0 Control Register */
             uint32_t reserved_0x024;
    volatile uint32_t PLL_PERIPH0_CTRL_REG;                                     /*!< Offset 0x028 PLL_PERIPH0 Control Register  */
    volatile uint32_t PLL_PERIPH1_CTRL_REG;                                     /*!< Offset 0x02C PLL_PERIPH1 Control Register  */
    volatile uint32_t PLL_VIDEO1_CTRL_REG;                                      /*!< Offset 0x030 PLL_VIDEO1 Control Register */
             uint32_t reserved_0x034;
    volatile uint32_t PLL_GPU_CTRL_REG;                                         /*!< Offset 0x038 PLL_GPU Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t PLL_MIPI_CTRL_REG;                                        /*!< Offset 0x040 PLL_MIPI Control Register */
    volatile uint32_t PLL_HSIC_CTRL_REG;                                        /*!< Offset 0x044 PLL_HSIC Control Register  */
    volatile uint32_t PLL_DE_CTRL_REG;                                          /*!< Offset 0x048 PLL_DE Control Register */
    volatile uint32_t PLL_DDR1_CTRL_REG;                                        /*!< Offset 0x04C PLL_DDR1 Control Register */
    volatile uint32_t CPU_AXI_CFG_REG;                                          /*!< Offset 0x050 (null) */
    volatile uint32_t AHB1_APB1_CFG_REG;                                        /*!< Offset 0x054 (null) */
    volatile uint32_t APB2_CFG_REG;                                             /*!< Offset 0x058 APB2 Configuration Register */
    volatile uint32_t AHB2_CFG_REG;                                             /*!< Offset 0x05C AHB2 Configuration Register */
    volatile uint32_t BUS_CLK_GATING_REG0;                                      /*!< Offset 0x060 Bus Clock Gating Register 0 */
    volatile uint32_t BUS_CLK_GATING_REG1;                                      /*!< Offset 0x064 Bus Clock Gating Register 1 */
    volatile uint32_t BUS_CLK_GATING_REG2;                                      /*!< Offset 0x068 Bus Clock Gating Register 2 */
    volatile uint32_t BUS_CLK_GATING_REG3;                                      /*!< Offset 0x06C Bus Clock Gating Register 3 */
    volatile uint32_t BUS_CLK_GATING_REG4;                                      /*!< Offset 0x070 Bus Clock Gating Register 4 */
    volatile uint32_t THS_CLK_REG;                                              /*!< Offset 0x074 THS Clock Register */
             uint32_t reserved_0x078 [0x0002];
    volatile uint32_t NAND_CLK_REG;                                             /*!< Offset 0x080 NAND Clock Register */
             uint32_t reserved_0x084;
    volatile uint32_t SDMMC0_CLK_REG;                                           /*!< Offset 0x088 SDMMC0 Clock Register */
    volatile uint32_t SDMMC1_CLK_REG;                                           /*!< Offset 0x08C SDMMC1 Clock Register */
    volatile uint32_t SDMMC2_CLK_REG;                                           /*!< Offset 0x090 SDMMC2 Clock Register */
             uint32_t reserved_0x094;
    volatile uint32_t TS_CLK_REG;                                               /*!< Offset 0x098 TS Clock Register */
    volatile uint32_t CE_CLK_REG;                                               /*!< Offset 0x09C CE Clock Register */
    volatile uint32_t SPI0_CLK_REG;                                             /*!< Offset 0x0A0 SPI0 Clock Register */
    volatile uint32_t SPI1_CLK_REG;                                             /*!< Offset 0x0A4 SPI1 Clock Register */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t I2S_PCM_0_CLK_REG;                                        /*!< Offset 0x0B0 I2S/PCM-0 Clock Register */
    volatile uint32_t I2S_PCM_1_CLK_REG;                                        /*!< Offset 0x0B4 I2S/PCM-1 Clock Register */
    volatile uint32_t I2S_PCM_2_CLK_REG;                                        /*!< Offset 0x0B8 I2S/PCM-2 Clock Register */
             uint32_t reserved_0x0BC;
    volatile uint32_t SPDIF_CLK_REG;                                            /*!< Offset 0x0C0 SPDIF Clock Register */
             uint32_t reserved_0x0C4 [0x0002];
    volatile uint32_t USBPHY_CFG_REG;                                           /*!< Offset 0x0CC USBPHY Configuration Register */
             uint32_t reserved_0x0D0 [0x0009];
    volatile uint32_t DRAM_CFG_REG;                                             /*!< Offset 0x0F4 DRAM Configuration Register */
    volatile uint32_t PLL_DDR_CFG_REG;                                          /*!< Offset 0x0F8 PLL_DDR Configuration Register */
    volatile uint32_t MBUS_RST_REG;                                             /*!< Offset 0x0FC MBUS Reset Register */
    volatile uint32_t DRAM_CLK_GATING_REG;                                      /*!< Offset 0x100 DRAM Clock Gating Register */
    volatile uint32_t DE_CLK_REG;                                               /*!< Offset 0x104 (null) */
             uint32_t reserved_0x108 [0x0004];
    volatile uint32_t TCON0_CLK_REG;                                            /*!< Offset 0x118 TCON0 Clock Register */
    volatile uint32_t TCON1_CLK_REG;                                            /*!< Offset 0x11C TCON1 Clock Register */
             uint32_t reserved_0x120;
    volatile uint32_t DEINTERLACE_CLK_REG;                                      /*!< Offset 0x124 DEINTERLACE Clock Register */
             uint32_t reserved_0x128 [0x0002];
    volatile uint32_t CSI_MISC_CLK_REG;                                         /*!< Offset 0x130 CSI_MISC Clock Register */
    volatile uint32_t CSI_CLK_REG;                                              /*!< Offset 0x134 CSI Clock Register */
             uint32_t reserved_0x138;
    volatile uint32_t VE_CLK_REG;                                               /*!< Offset 0x13C VE Clock Register */
    volatile uint32_t AC_DIG_CLK_REG;                                           /*!< Offset 0x140 AC Digital Clock Register */
    volatile uint32_t AVS_CLK_REG;                                              /*!< Offset 0x144 AVS Clock Register */
             uint32_t reserved_0x148 [0x0002];
    volatile uint32_t HDMI_CLK_REG;                                             /*!< Offset 0x150 HDMI Clock Register */
    volatile uint32_t HDMI_SLOW_CLK_REG;                                        /*!< Offset 0x154 HDMI Slow Clock Register */
             uint32_t reserved_0x158;
    volatile uint32_t MBUS_CLK_REG;                                             /*!< Offset 0x15C MBUS Clock Register */
             uint32_t reserved_0x160 [0x0002];
    volatile uint32_t MIPI_DSI_CLK_REG;                                         /*!< Offset 0x168 (null) */
             uint32_t reserved_0x16C [0x000D];
    volatile uint32_t GPU_CLK_REG;                                              /*!< Offset 0x1A0 GPU Clock Register */
             uint32_t reserved_0x1A4 [0x0017];
    volatile uint32_t PLL_STABLE_TIME_REG0;                                     /*!< Offset 0x200 PLL Stable Time Register0 */
    volatile uint32_t PLL_STABLE_TIME_REG1;                                     /*!< Offset 0x204 PLL Stable Time Register1 */
             uint32_t reserved_0x208 [0x0005];
    volatile uint32_t PLL_PERIPH1_BIAS_REG;                                     /*!< Offset 0x21C PLL_PERIPH1 Bias Register */
    volatile uint32_t PLL_CPUX_BIAS_REG;                                        /*!< Offset 0x220 PLL_CPUX Bias Register */
    volatile uint32_t PLL_AUDIO_BIAS_REG;                                       /*!< Offset 0x224 PLL_AUDIO Bias Register */
    volatile uint32_t PLL_VIDEO0_BIAS_REG;                                      /*!< Offset 0x228 PLL_VIDEO0 Bias Register */
    volatile uint32_t PLL_VE_BIAS_REG;                                          /*!< Offset 0x22C PLL_VE Bias Register */
    volatile uint32_t PLL_DDR0_BIAS_REG;                                        /*!< Offset 0x230 PLL_DDR0 Bias Register */
    volatile uint32_t PLL_PERIPH0_BIAS_REG;                                     /*!< Offset 0x234 PLL_PERIPH0 Bias Register */
    volatile uint32_t PLL_VIDEO1_BIAS_REG;                                      /*!< Offset 0x238 PLL_VIDEO1 Bias Register */
    volatile uint32_t PLL_GPU_BIAS_REG;                                         /*!< Offset 0x23C PLL_GPU Bias Register */
    volatile uint32_t PLL_MIPI_BIAS_REG;                                        /*!< Offset 0x240 PLL_MIPI Bias Register */
    volatile uint32_t PLL_HSIC_BIAS_REG;                                        /*!< Offset 0x244 PLL_HSIC Bias Register */
    volatile uint32_t PLL_DE_BIAS_REG;                                          /*!< Offset 0x248 PLL_DE Bias Register */
    volatile uint32_t PLL_DDR1_BIAS_REG;                                        /*!< Offset 0x24C PLL_DDR1 Bias Register */
    volatile uint32_t PLL_CPUX_TUN_REG;                                         /*!< Offset 0x250 PLL_CPUX Tuning Register */
             uint32_t reserved_0x254 [0x0003];
    volatile uint32_t PLL_DDR0_TUN_REG;                                         /*!< Offset 0x260 PLL_DDR0 Tuning Register */
             uint32_t reserved_0x264 [0x0003];
    volatile uint32_t PLL_MIPI_TUN_REG;                                         /*!< Offset 0x270 PLL_MIPI Tuning Register */
             uint32_t reserved_0x274 [0x0002];
    volatile uint32_t PLL_PERIPH1_PAT_CTRL_REG;                                 /*!< Offset 0x27C PLL_PERIPH1 Pattern Control Register */
    volatile uint32_t PLL_CPUX_PAT_CTRL_REG;                                    /*!< Offset 0x280 PLL_CPUX Pattern Control Register  */
    volatile uint32_t PLL_AUDIO_PAT_CTRL_REG;                                   /*!< Offset 0x284 PLL_AUDIO Pattern Control Register */
    volatile uint32_t PLL_VIDEO0_PAT_CTRL_REG;                                  /*!< Offset 0x288 PLL_VIDEO0 Pattern Control Register */
    volatile uint32_t PLL_VE_PAT_CTRL_REG;                                      /*!< Offset 0x28C PLL_VE Pattern Control Register */
    volatile uint32_t PLL_DDR0_PAT_CTRL_REG;                                    /*!< Offset 0x290 PLL_DDR0 Pattern Control Register */
             uint32_t reserved_0x294;
    volatile uint32_t PLL_VIDEO1_PAT_CTRL_REG;                                  /*!< Offset 0x298 PLL_VIDEO1 Pattern Control Register */
    volatile uint32_t PLL_GPU_PAT_CTRL_REG;                                     /*!< Offset 0x29C PLL_GPU Pattern Control Register */
    volatile uint32_t PLL_MIPI_PAT_CTRL_REG;                                    /*!< Offset 0x2A0 PLL_MIPI Pattern Control Register */
    volatile uint32_t PLL_HSIC_PAT_CTRL_REG;                                    /*!< Offset 0x2A4 PLL_HSIC Pattern Control Register */
    volatile uint32_t PLL_DE_PAT_CTRL_REG;                                      /*!< Offset 0x2A8 PLL_DE Pattern Control Register */
    volatile uint32_t PLL_DDR1_PAT_CTRL_REG0;                                   /*!< Offset 0x2AC PLL_DDR1 Pattern Control Register0 */
    volatile uint32_t PLL_DDR1_PAT_CTRL_REG1;                                   /*!< Offset 0x2B0 PLL_DDR1 Pattern Control Register1 */
             uint32_t reserved_0x2B4 [0x0003];
    volatile uint32_t BUS_SOFT_RST_REG0;                                        /*!< Offset 0x2C0 Bus Software Reset Register 0 */
    volatile uint32_t BUS_SOFT_RST_REG1;                                        /*!< Offset 0x2C4 Bus Software Reset Register 1 */
    volatile uint32_t BUS_SOFT_RST_REG2;                                        /*!< Offset 0x2C8 Bus Software Reset Register 2 */
             uint32_t reserved_0x2CC;
    volatile uint32_t BUS_SOFT_RST_REG3;                                        /*!< Offset 0x2D0 Bus Software Reset Register 3 */
             uint32_t reserved_0x2D4;
    volatile uint32_t BUS_SOFT_RST_REG4;                                        /*!< Offset 0x2D8 Bus Software Reset Register 4 */
             uint32_t reserved_0x2DC [0x0005];
    volatile uint32_t CCM_SEC_SWITCH_REG;                                       /*!< Offset 0x2F0 CCM Security Switch Register */
             uint32_t reserved_0x2F4 [0x0003];
    volatile uint32_t PS_CTRL_REG;                                              /*!< Offset 0x300 PS Control Register */
    volatile uint32_t PS_CNT_REG;                                               /*!< Offset 0x304 PS Counter Register */
             uint32_t reserved_0x308 [0x0006];
    volatile uint32_t PLL_LOCK_CTRL_REG;                                        /*!< Offset 0x320 PLL Lock Control Register */
} CCU_TypeDef; /* size of structure = 0x324 */
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG Controller Interface */
typedef struct SYS_CFG_Type
{
             uint32_t reserved_0x000 [0x0002];
    volatile uint32_t DSP_BOOT_RAMMAP_REG;                                      /*!< Offset 0x008 DSP Boot SRAM Remap Control Register */
             uint32_t reserved_0x00C [0x0006];
    volatile uint32_t VER_REG;                                                  /*!< Offset 0x024 Version Register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t EMAC_EPHY_CLK_REG0;                                       /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
             uint32_t reserved_0x034 [0x0047];
    volatile uint32_t SYS_LDO_CTRL_REG;                                         /*!< Offset 0x150 System LDO Control Register */
             uint32_t reserved_0x154 [0x0003];
    volatile uint32_t RESCAL_CTRL_REG;                                          /*!< Offset 0x160 Resistor Calibration Control Register */
             uint32_t reserved_0x164;
    volatile uint32_t RES240_CTRL_REG;                                          /*!< Offset 0x168 240ohms Resistor Manual Control Register */
    volatile uint32_t RESCAL_STATUS_REG;                                        /*!< Offset 0x16C Resistor Calibration Status Register */
} SYS_CFG_TypeDef; /* size of structure = 0x170 */
/*
 * @brief UART
 */
/*!< UART Controller Interface */
typedef struct UART_Type
{
    volatile uint32_t DATA;                                                     /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    volatile uint32_t DLH_IER;                                                  /*!< Offset 0x004 UART Divisor Latch High Register/UART Interrupt Enable Register */
    volatile uint32_t IIR_FCR;                                                  /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    volatile uint32_t UART_LCR;                                                 /*!< Offset 0x00C UART Line Control Register */
    volatile uint32_t UART_MCR;                                                 /*!< Offset 0x010 UART Modem Control Register */
    volatile uint32_t UART_LSR;                                                 /*!< Offset 0x014 UART Line Status Register */
    volatile uint32_t UART_MSR;                                                 /*!< Offset 0x018 UART Modem Status Register */
    volatile uint32_t UART_SCH;                                                 /*!< Offset 0x01C UART Scratch Register */
             uint32_t reserved_0x020 [0x0017];
    volatile uint32_t UART_USR;                                                 /*!< Offset 0x07C UART Status Register */
    volatile uint32_t UART_TFL;                                                 /*!< Offset 0x080 UART Transmit FIFO Level Register */
    volatile uint32_t UART_RFL;                                                 /*!< Offset 0x084 UART Receive FIFO Level Register */
    volatile uint32_t UART_HSK;                                                 /*!< Offset 0x088 UART DMA Handshake Configuration Register */
    volatile uint32_t UART_DMA_REQ_EN;                                          /*!< Offset 0x08C UART DMA Request Enable Register */
             uint32_t reserved_0x090 [0x0005];
    volatile uint32_t UART_HALT;                                                /*!< Offset 0x0A4 UART Halt TX Register */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t UART_DBG_DLL;                                             /*!< Offset 0x0B0 UART Debug DLL Register */
    volatile uint32_t UART_DBG_DLH;                                             /*!< Offset 0x0B4 UART Debug DLH Register */
             uint32_t reserved_0x0B8 [0x000E];
    volatile uint32_t UART_A_FCC;                                               /*!< Offset 0x0F0 UART FIFO Clock Control Register */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t UART_A_RXDMA_CTRL;                                        /*!< Offset 0x100 UART RXDMA Control Register */
    volatile uint32_t UART_A_RXDMA_STR;                                         /*!< Offset 0x104 UART RXDMA Start Register */
    volatile uint32_t UART_A_RXDMA_STA;                                         /*!< Offset 0x108 UART RXDMA Status Register */
    volatile uint32_t UART_A_RXDMA_LMT;                                         /*!< Offset 0x10C UART RXDMA Limit Register */
    volatile uint32_t UART_A_RXDMA_SADDRL;                                      /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
    volatile uint32_t UART_A_RXDMA_SADDRH;                                      /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
    volatile uint32_t UART_A_RXDMA_BL;                                          /*!< Offset 0x118 UART RXDMA Buffer Length Register */
             uint32_t reserved_0x11C;
    volatile uint32_t UART_A_RXDMA_IE;                                          /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
    volatile uint32_t UART_A_RXDMA_IS;                                          /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
    volatile uint32_t UART_A_RXDMA_WADDRL;                                      /*!< Offset 0x128 UART RXDMA Write Address Low Register */
    volatile uint32_t UART_A_RXDMA_WADDRH;                                      /*!< Offset 0x12C UART RXDMA Write Address high Register */
    volatile uint32_t UART_A_RXDMA_RADDRL;                                      /*!< Offset 0x130 UART RXDMA Read Address Low Register */
    volatile uint32_t UART_A_RXDMA_RADDRH;                                      /*!< Offset 0x134 UART RXDMA Read Address high Register */
    volatile uint32_t UART_A_RXDMA_DCNT;                                        /*!< Offset 0x138 UART RADMA Data Count Register */
} UART_TypeDef; /* size of structure = 0x13C */
/*
 * @brief GPIO
 */
/*!< GPIO Controller Interface */
typedef struct GPIO_Type
{
    volatile uint32_t CFG [0x004];                                              /*!< Offset 0x000 Configure Register */
    volatile uint32_t DATA;                                                     /*!< Offset 0x010 Data Register */
    volatile uint32_t DRV [0x002];                                              /*!< Offset 0x014 Multi_Driving Register */
    volatile uint32_t PULL [0x002];                                             /*!< Offset 0x01C Pull Register */
} GPIO_TypeDef; /* size of structure = 0x024 */
/*
 * @brief GPIOINT
 */
/*!< GPIOINT Controller Interface */
typedef struct GPIOINT_Type
{
    volatile uint32_t EINT_CFG [0x004];                                         /*!< Offset 0x000 External Interrupt Configure Registers */
    volatile uint32_t EINT_CTL;                                                 /*!< Offset 0x010 External Interrupt Control Register */
    volatile uint32_t EINT_STATUS;                                              /*!< Offset 0x014 External Interrupt Status Register */
    volatile uint32_t EINT_DEB;                                                 /*!< Offset 0x018 External Interrupt Debounce Register */
             uint32_t reserved_0x01C;
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK Controller Interface */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        volatile uint32_t CFG [0x004];                                          /*!< Offset 0x000 Configure Register */
        volatile uint32_t DATA;                                                 /*!< Offset 0x010 Data Register */
        volatile uint32_t DRV [0x004];                                          /*!< Offset 0x014 Multi_Driving Register */
        volatile uint32_t PULL [0x002];                                         /*!< Offset 0x024 Pull Register */
                 uint32_t reserved_0x02C;
    } GPIO_PINS [0x007];                                                        /*!< Offset 0x000 GPIO pin control */
             uint32_t reserved_0x150 [0x002C];
    struct
    {
        volatile uint32_t EINT_CFG [0x004];                                     /*!< Offset 0x200 External Interrupt Configure Registers */
        volatile uint32_t EINT_CTL;                                             /*!< Offset 0x210 External Interrupt Control Register */
        volatile uint32_t EINT_STATUS;                                          /*!< Offset 0x214 External Interrupt Status Register */
        volatile uint32_t EINT_DEB;                                             /*!< Offset 0x218 External Interrupt Debounce Register */
                 uint32_t reserved_0x01C;
    } GPIO_INTS [0x007];                                                        /*!< Offset 0x200 GPIO interrupt control */
             uint32_t reserved_0x2E0 [0x0018];
    volatile uint32_t PIO_POW_MOD_SEL;                                          /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    volatile uint32_t PIO_POW_MS_CTL;                                           /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    volatile uint32_t PIO_POW_VAL;                                              /*!< Offset 0x348 PIO Group Power Value Register */
} GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
/*
 * @brief SMHC
 */
/*!< SMHC Controller Interface */
typedef struct SMHC_Type
{
    volatile uint32_t SMHC_CTRL;                                                /*!< Offset 0x000 Control Register */
    volatile uint32_t SMHC_CLKDIV;                                              /*!< Offset 0x004 Clock Control Register */
    volatile uint32_t SMHC_TMOUT;                                               /*!< Offset 0x008 Time Out Register */
    volatile uint32_t SMHC_CTYPE;                                               /*!< Offset 0x00C Bus Width Register */
    volatile uint32_t SMHC_BLKSIZ;                                              /*!< Offset 0x010 Block Size Register */
    volatile uint32_t SMHC_BYTCNT;                                              /*!< Offset 0x014 Byte Count Register */
    volatile uint32_t SMHC_CMD;                                                 /*!< Offset 0x018 Command Register */
    volatile uint32_t SMHC_CMDARG;                                              /*!< Offset 0x01C Command Argument Register */
    volatile uint32_t SMHC_RESP0;                                               /*!< Offset 0x020 Response 0 Register */
    volatile uint32_t SMHC_RESP1;                                               /*!< Offset 0x024 Response 1 Register */
    volatile uint32_t SMHC_RESP2;                                               /*!< Offset 0x028 Response 2 Register */
    volatile uint32_t SMHC_RESP3;                                               /*!< Offset 0x02C Response 3 Register */
    volatile uint32_t SMHC_INTMASK;                                             /*!< Offset 0x030 Interrupt Mask Register */
    volatile uint32_t SMHC_MINTSTS;                                             /*!< Offset 0x034 Masked Interrupt Status Register */
    volatile uint32_t SMHC_RINTSTS;                                             /*!< Offset 0x038 Raw Interrupt Status Register */
    volatile uint32_t SMHC_STATUS;                                              /*!< Offset 0x03C Status Register */
    volatile uint32_t SMHC_FIFOTH;                                              /*!< Offset 0x040 FIFO Water Level Register */
    volatile uint32_t SMHC_FUNS;                                                /*!< Offset 0x044 FIFO Function Select Register */
    volatile uint32_t SMHC_TCBCNT;                                              /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
    volatile uint32_t SMHC_TBBCNT;                                              /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
    volatile uint32_t SMHC_DBGC;                                                /*!< Offset 0x050 Current Debug Control Register */
    volatile uint32_t SMHC_CSDC;                                                /*!< Offset 0x054 CRC Status Detect Control Registers */
    volatile uint32_t SMHC_A12A;                                                /*!< Offset 0x058 Auto Command 12 Argument Register */
    volatile uint32_t SMHC_NTSR;                                                /*!< Offset 0x05C SD New Timing Set Register */
             uint32_t reserved_0x060 [0x0006];
    volatile uint32_t SMHC_HWRST;                                               /*!< Offset 0x078 Hardware Reset Register */
             uint32_t reserved_0x07C;
    volatile uint32_t SMHC_IDMAC;                                               /*!< Offset 0x080 IDMAC Control Register */
    volatile uint32_t SMHC_DLBA;                                                /*!< Offset 0x084 Descriptor List Base Address Register */
    volatile uint32_t SMHC_IDST;                                                /*!< Offset 0x088 IDMAC Status Register */
    volatile uint32_t SMHC_IDIE;                                                /*!< Offset 0x08C IDMAC Interrupt Enable Register */
             uint32_t reserved_0x090 [0x001C];
    volatile uint32_t SMHC_THLD;                                                /*!< Offset 0x100 Card Threshold Control Register */
    volatile uint32_t SMHC_SFC;                                                 /*!< Offset 0x104 Sample FIFO Control Register */
    volatile uint32_t SMHC_A23A;                                                /*!< Offset 0x108 Auto Command 23 Argument Register */
    volatile uint32_t EMMC_DDR_SBIT_DET;                                        /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
             uint32_t reserved_0x110 [0x000A];
    volatile uint32_t SMHC_EXT_CMD;                                             /*!< Offset 0x138 Extended Command Register */
    volatile uint32_t SMHC_EXT_RESP;                                            /*!< Offset 0x13C Extended Response Register */
    volatile uint32_t SMHC_DRV_DL;                                              /*!< Offset 0x140 Drive Delay Control Register */
    volatile uint32_t SMHC_SMAP_DL;                                             /*!< Offset 0x144 Sample Delay Control Register */
    volatile uint32_t SMHC_DS_DL;                                               /*!< Offset 0x148 Data Strobe Delay Control Register */
    volatile uint32_t SMHC_HS400_DL;                                            /*!< Offset 0x14C HS400 Delay Control Register */
             uint32_t reserved_0x150 [0x002C];
    volatile uint32_t SMHC_FIFO;                                                /*!< Offset 0x200 Read/Write FIFO */
} SMHC_TypeDef; /* size of structure = 0x204 */
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM Controller Interface */
typedef struct I2S_PCM_Type
{
    volatile uint32_t I2S_PCM_CTL;                                              /*!< Offset 0x000 I2S/PCM Control Register */
    volatile uint32_t I2S_PCM_FMT0;                                             /*!< Offset 0x004 I2S/PCM Format Register 0 */
    volatile uint32_t I2S_PCM_FMT1;                                             /*!< Offset 0x008 I2S/PCM Format Register 1 */
    volatile uint32_t I2S_PCM_ISTA;                                             /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    volatile uint32_t I2S_PCM_RXFIFO;                                           /*!< Offset 0x010 I2S/PCM RXFIFO Register */
    volatile uint32_t I2S_PCM_FCTL;                                             /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    volatile uint32_t I2S_PCM_FSTA;                                             /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    volatile uint32_t I2S_PCM_INT;                                              /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    volatile uint32_t I2S_PCM_TXFIFO;                                           /*!< Offset 0x020 I2S/PCM TXFIFO Register */
    volatile uint32_t I2S_PCM_CLKD;                                             /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    volatile uint32_t I2S_PCM_TXCNT;                                            /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    volatile uint32_t I2S_PCM_RXCNT;                                            /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    volatile uint32_t I2S_PCM_CHCFG;                                            /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
    volatile uint32_t I2S_PCM_TX0CHSEL;                                         /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
    volatile uint32_t I2S_PCM_TX1CHSEL;                                         /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
    volatile uint32_t I2S_PCM_TX2CHSEL;                                         /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
    volatile uint32_t I2S_PCM_TX3CHSEL;                                         /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
    volatile uint32_t I2S_PCM_TX0CHMAP0;                                        /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX0CHMAP1;                                        /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX1CHMAP0;                                        /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX1CHMAP1;                                        /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX2CHMAP0;                                        /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX2CHMAP1;                                        /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX3CHMAP0;                                        /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX3CHMAP1;                                        /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_RXCHSEL;                                          /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
    volatile uint32_t I2S_PCM_RXCHMAP0;                                         /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_RXCHMAP1;                                         /*!< Offset 0x06C I2S/PCM RX Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_RXCHMAP2;                                         /*!< Offset 0x070 I2S/PCM RX Channel Mapping Register2 */
    volatile uint32_t I2S_PCM_RXCHMAP3;                                         /*!< Offset 0x074 I2S/PCM RX Channel Mapping Register3 */
             uint32_t reserved_0x078 [0x0002];
    volatile uint32_t MCLKCFG;                                                  /*!< Offset 0x080 ASRC MCLK Configuration Register */
    volatile uint32_t FsoutCFG;                                                 /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
    volatile uint32_t FsinEXTCFG;                                               /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
    volatile uint32_t ASRCCFG;                                                  /*!< Offset 0x08C ASRC Enable Register */
    volatile uint32_t ASRCMANCFG;                                               /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
    volatile uint32_t ASRCRATIOSTAT;                                            /*!< Offset 0x094 ASRC Status Register */
    volatile uint32_t ASRCFIFOSTAT;                                             /*!< Offset 0x098 ASRC FIFO Level Status Register */
    volatile uint32_t ASRCMBISTCFG;                                             /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
    volatile uint32_t ASRCMBISTSTAT;                                            /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief DMIC
 */
/*!< DMIC Controller Interface */
typedef struct DMIC_Type
{
    volatile uint32_t DMIC_EN;                                                  /*!< Offset 0x000 DMIC Enable Control Register */
    volatile uint32_t DMIC_SR;                                                  /*!< Offset 0x004 DMIC Sample Rate Register */
    volatile uint32_t DMIC_CTR;                                                 /*!< Offset 0x008 DMIC Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t DMIC_DATA;                                                /*!< Offset 0x010 DMIC Data Register */
    volatile uint32_t DMIC_INTC;                                                /*!< Offset 0x014 MIC Interrupt Control Register */
    volatile uint32_t DMIC_INTS;                                                /*!< Offset 0x018 DMIC Interrupt Status Register */
    volatile uint32_t DMIC_RXFIFO_CTR;                                          /*!< Offset 0x01C DMIC RXFIFO Control Register */
    volatile uint32_t DMIC_RXFIFO_STA;                                          /*!< Offset 0x020 DMIC RXFIFO Status Register */
    volatile uint32_t DMIC_CH_NUM;                                              /*!< Offset 0x024 DMIC Channel Numbers Register */
    volatile uint32_t DMIC_CH_MAP;                                              /*!< Offset 0x028 DMIC Channel Mapping Register */
    volatile uint32_t DMIC_CNT;                                                 /*!< Offset 0x02C DMIC Counter Register */
    volatile uint32_t DATA0_DATA1_VOL_CTR;                                      /*!< Offset 0x030 Data0 and Data1 Volume Control Register */
    volatile uint32_t DATA2_DATA3_VOL_CTR;                                      /*!< Offset 0x034 Data2 And Data3 Volume Control Register */
    volatile uint32_t HPF_EN_CTR;                                               /*!< Offset 0x038 High Pass Filter Enable Control Register */
    volatile uint32_t HPF_COEF_REG;                                             /*!< Offset 0x03C High Pass Filter Coefficient Register */
    volatile uint32_t HPF_GAIN_REG;                                             /*!< Offset 0x040 High Pass Filter Gain Register */
} DMIC_TypeDef; /* size of structure = 0x044 */
/*
 * @brief OWA
 */
/*!< OWA Controller Interface */
typedef struct OWA_Type
{
    volatile uint32_t OWA_GEN_CTL;                                              /*!< Offset 0x000 OWA General Control Register */
    volatile uint32_t OWA_TX_CFIG;                                              /*!< Offset 0x004 OWA TX Configuration Register */
    volatile uint32_t OWA_RX_CFIG;                                              /*!< Offset 0x008 OWA RX Configuration Register */
    volatile uint32_t OWA_ISTA;                                                 /*!< Offset 0x00C OWA Interrupt Status Register */
    volatile uint32_t OWA_RXFIFO;                                               /*!< Offset 0x010 OWA RXFIFO Register */
    volatile uint32_t OWA_FCTL;                                                 /*!< Offset 0x014 OWA FIFO Control Register */
    volatile uint32_t OWA_FSTA;                                                 /*!< Offset 0x018 OWA FIFO Status Register */
    volatile uint32_t OWA_INT;                                                  /*!< Offset 0x01C OWA Interrupt Control Register */
    volatile uint32_t OWA_TX_FIFO;                                              /*!< Offset 0x020 OWA TX FIFO Register */
    volatile uint32_t OWA_TX_CNT;                                               /*!< Offset 0x024 OWA TX Counter Register */
    volatile uint32_t OWA_RX_CNT;                                               /*!< Offset 0x028 OWA RX Counter Register */
    volatile uint32_t OWA_TX_CHSTA0;                                            /*!< Offset 0x02C OWA TX Channel Status Register0 */
    volatile uint32_t OWA_TX_CHSTA1;                                            /*!< Offset 0x030 OWA TX Channel Status Register1 */
    volatile uint32_t OWA_RXCHSTA0;                                             /*!< Offset 0x034 OWA RX Channel Status Register0 */
    volatile uint32_t OWA_RXCHSTA1;                                             /*!< Offset 0x038 OWA RX Channel Status Register1 */
             uint32_t reserved_0x03C;
    volatile uint32_t OWA_EXP_CTL;                                              /*!< Offset 0x040 OWA Expand Control Register */
    volatile uint32_t OWA_EXP_ISTA;                                             /*!< Offset 0x044 OWA Expand Interrupt Status Register */
    volatile uint32_t OWA_EXP_INFO_0;                                           /*!< Offset 0x048 OWA Expand Infomation Register0 */
    volatile uint32_t OWA_EXP_INFO_1;                                           /*!< Offset 0x04C OWA Expand Infomation Register1 */
    volatile uint32_t OWA_EXP_DBG_0;                                            /*!< Offset 0x050 OWA Expand Debug Register0 */
    volatile uint32_t OWA_EXP_DBG_1;                                            /*!< Offset 0x054 OWA Expand Debug Register1 */
} OWA_TypeDef; /* size of structure = 0x058 */
/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Controller Interface */
typedef struct AUDIO_CODEC_Type
{
    volatile uint32_t AC_DAC_DPC;                                               /*!< Offset 0x000 DAC Digital Part Control Register */
    volatile uint32_t DAC_VOL_CTRL;                                             /*!< Offset 0x004 DAC Volume Control Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t AC_DAC_FIFOC;                                             /*!< Offset 0x010 DAC FIFO Control Register */
    volatile uint32_t AC_DAC_FIFOS;                                             /*!< Offset 0x014 DAC FIFO Status Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t AC_DAC_TXDATA;                                            /*!< Offset 0x020 DAC TX DATA Register */
    volatile uint32_t AC_DAC_CNT;                                               /*!< Offset 0x024 DAC TX FIFO Counter Register */
    volatile uint32_t AC_DAC_DG;                                                /*!< Offset 0x028 DAC Debug Register */
             uint32_t reserved_0x02C;
    volatile uint32_t AC_ADC_FIFOC;                                             /*!< Offset 0x030 ADC FIFO Control Register */
    volatile uint32_t ADC_VOL_CTRL1;                                            /*!< Offset 0x034 ADC Volume Control1 Register */
    volatile uint32_t AC_ADC_FIFOS;                                             /*!< Offset 0x038 ADC FIFO Status Register */
             uint32_t reserved_0x03C;
    volatile uint32_t AC_ADC_RXDATA;                                            /*!< Offset 0x040 ADC RX Data Register */
    volatile uint32_t AC_ADC_CNT;                                               /*!< Offset 0x044 ADC RX Counter Register */
             uint32_t reserved_0x048;
    volatile uint32_t AC_ADC_DG;                                                /*!< Offset 0x04C ADC Debug Register */
    volatile uint32_t ADC_DIG_CTRL;                                             /*!< Offset 0x050 ADC Digtial Control Register */
    volatile uint32_t VRA1SPEEDUP_DOWN_CTRL;                                    /*!< Offset 0x054 VRA1 Speedup Down Control Register */
             uint32_t reserved_0x058 [0x0026];
    volatile uint32_t AC_DAC_DAP_CTRL;                                          /*!< Offset 0x0F0 DAC DAP Control Register */
             uint32_t reserved_0x0F4;
    volatile uint32_t AC_ADC_DAP_CTR;                                           /*!< Offset 0x0F8 ADC DAP Control Register */
             uint32_t reserved_0x0FC;
    volatile uint32_t AC_DAC_DRC_HHPFC;                                         /*!< Offset 0x100 DAC DRC High HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_LHPFC;                                         /*!< Offset 0x104 DAC DRC Low HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_CTRL;                                          /*!< Offset 0x108 DAC DRC Control Register */
    volatile uint32_t AC_DAC_DRC_LPFHAT;                                        /*!< Offset 0x10C DAC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLAT;                                        /*!< Offset 0x110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHAT;                                        /*!< Offset 0x114 DAC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLAT;                                        /*!< Offset 0x118 DAC DRC Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFHRT;                                        /*!< Offset 0x11C DAC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLRT;                                        /*!< Offset 0x120 DAC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHRT;                                        /*!< Offset 0x124 DAC DRC Right Peak filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLRT;                                        /*!< Offset 0x128 DAC DRC Right Peak filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSHAT;                                       /*!< Offset 0x12C DAC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSLAT;                                       /*!< Offset 0x130 DAC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSHAT;                                       /*!< Offset 0x134 DAC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSLAT;                                       /*!< Offset 0x138 DAC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_HCT;                                           /*!< Offset 0x13C DAC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LCT;                                           /*!< Offset 0x140 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_HKC;                                           /*!< Offset 0x144 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKC;                                           /*!< Offset 0x148 DAC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPC;                                          /*!< Offset 0x14C DAC DRC Compresso */
    volatile uint32_t AC_DAC_DRC_LOPC;                                          /*!< Offset 0x150 DAC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_DAC_DRC_HLT;                                           /*!< Offset 0x154 DAC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LLT;                                           /*!< Offset 0x158 DAC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKl;                                           /*!< Offset 0x15C DAC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKl;                                           /*!< Offset 0x160 DAC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPL;                                          /*!< Offset 0x164 DAC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_LOPL;                                          /*!< Offset 0x168 DAC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_HET;                                           /*!< Offset 0x16C DAC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LET;                                           /*!< Offset 0x170 DAC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKE;                                           /*!< Offset 0x174 DAC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKE;                                           /*!< Offset 0x178 DAC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPE;                                          /*!< Offset 0x17C DAC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_LOPE;                                          /*!< Offset 0x180 DAC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_HKN;                                           /*!< Offset 0x184 DAC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKN;                                           /*!< Offset 0x188 DAC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_SFHAT;                                         /*!< Offset 0x18C DAC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLAT;                                         /*!< Offset 0x190 DAC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFHRT;                                         /*!< Offset 0x194 DAC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLRT;                                         /*!< Offset 0x198 DAC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_MXGHS;                                         /*!< Offset 0x19C DAC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MXGLS;                                         /*!< Offset 0x1A0 DAC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGHS;                                         /*!< Offset 0x1A4 DAC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGLS;                                         /*!< Offset 0x1A8 DAC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_EPSHC;                                         /*!< Offset 0x1AC DAC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_DAC_DRC_EPSLC;                                         /*!< Offset 0x1B0 DAC DRC Expander Smooth Time Low Coef Register */
             uint32_t reserved_0x1B4;
    volatile uint32_t AC_DAC_DRC_HPFHGAIN;                                      /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_DAC_DRC_HPFLGAIN;                                      /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
             uint32_t reserved_0x1C0 [0x0010];
    volatile uint32_t AC_ADC_DRC_HHPFC;                                         /*!< Offset 0x200 ADC DRC High HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_LHPFC;                                         /*!< Offset 0x204 ADC DRC Low HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_CTRL;                                          /*!< Offset 0x208 ADC DRC Control Register */
    volatile uint32_t AC_ADC_DRC_LPFHAT;                                        /*!< Offset 0x20C ADC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLAT;                                        /*!< Offset 0x210 ADC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHAT;                                        /*!< Offset 0x214 ADC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLAT;                                        /*!< Offset 0x218 ADC DRC Right Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFHRT;                                        /*!< Offset 0x21C ADC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLRT;                                        /*!< Offset 0x220 ADC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHRT;                                        /*!< Offset 0x224 ADC DRC Right Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLRT;                                        /*!< Offset 0x228 ADC DRC Right Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSHAT;                                       /*!< Offset 0x22C ADC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSLAT;                                       /*!< Offset 0x230 ADC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSHAT;                                       /*!< Offset 0x234 ADC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSLAT;                                       /*!< Offset 0x238 ADC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_HCT;                                           /*!< Offset 0x23C ADC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LCT;                                           /*!< Offset 0x240 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_HKC;                                           /*!< Offset 0x244 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKC;                                           /*!< Offset 0x248 ADC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPC;                                          /*!< Offset 0x24C ADC DRC Compressor High Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_LOPC;                                          /*!< Offset 0x250 ADC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_HLT;                                           /*!< Offset 0x254 ADC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LLT;                                           /*!< Offset 0x258 ADC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKl;                                           /*!< Offset 0x25C ADC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKl;                                           /*!< Offset 0x260 ADC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPL;                                          /*!< Offset 0x264 ADC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_LOPL;                                          /*!< Offset 0x268 ADC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_HET;                                           /*!< Offset 0x26C ADC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LET;                                           /*!< Offset 0x270 ADC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKE;                                           /*!< Offset 0x274 ADC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKE;                                           /*!< Offset 0x278 ADC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPE;                                          /*!< Offset 0x27C ADC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_LOPE;                                          /*!< Offset 0x280 ADC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_HKN;                                           /*!< Offset 0x284 ADC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKN;                                           /*!< Offset 0x288 ADC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_SFHAT;                                         /*!< Offset 0x28C ADC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLAT;                                         /*!< Offset 0x290 ADC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFHRT;                                         /*!< Offset 0x294 ADC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLRT;                                         /*!< Offset 0x298 ADC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_MXGHS;                                         /*!< Offset 0x29C ADC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MXGLS;                                         /*!< Offset 0x2A0 ADC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGHS;                                         /*!< Offset 0x2A4 ADC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGLS;                                         /*!< Offset 0x2A8 ADC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_EPSHC;                                         /*!< Offset 0x2AC ADC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_ADC_DRC_EPSLC;                                         /*!< Offset 0x2B0 ADC DRC Expander Smooth Time Low Coef Register */
             uint32_t reserved_0x2B4;
    volatile uint32_t AC_ADC_DRC_HPFHGAIN;                                      /*!< Offset 0x2B8 ADC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_ADC_DRC_HPFLGAIN;                                      /*!< Offset 0x2BC ADC DRC HPF Gain Low Coef Register */
             uint32_t reserved_0x2C0 [0x0010];
    volatile uint32_t ADC1_REG;                                                 /*!< Offset 0x300 ADC1 Analog Control Register */
    volatile uint32_t ADC2_REG;                                                 /*!< Offset 0x304 ADC2 Analog Control Register */
    volatile uint32_t ADC3_REG;                                                 /*!< Offset 0x308 ADC3 Analog Control Register */
             uint32_t reserved_0x30C;
    volatile uint32_t DAC_REG;                                                  /*!< Offset 0x310 DAC Analog Control Register */
             uint32_t reserved_0x314;
    volatile uint32_t MICBIAS_REG;                                              /*!< Offset 0x318 MICBIAS Analog Control Register */
    volatile uint32_t RAMP_REG;                                                 /*!< Offset 0x31C BIAS Analog Control Register */
    volatile uint32_t BIAS_REG;                                                 /*!< Offset 0x320 BIAS Analog Control Register */
             uint32_t reserved_0x324;
    volatile uint32_t HMIC_CTRL;                                                /*!< Offset 0x328 * HMIC Control Register */
    volatile uint32_t HMIC_STS;                                                 /*!< Offset 0x32C * HMIC Status Register ( */
    volatile uint32_t ADC5_REG;                                                 /*!< Offset 0x330 ADC5 Analog Control Register */
             uint32_t reserved_0x334 [0x0003];
    volatile uint32_t HP2_REG;                                                  /*!< Offset 0x340 * Headphone2 Analog Control Register */
             uint32_t reserved_0x344;
    volatile uint32_t POWER_REG;                                                /*!< Offset 0x348 * POWER Analog Control Register */
    volatile uint32_t ADC_CUR_REG;                                              /*!< Offset 0x34C * ADC Current Analog Control Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x350 */
/*
 * @brief TWI
 */
/*!< TWI Controller Interface */
typedef struct TWI_Type
{
    volatile uint32_t TWI_ADDR;                                                 /*!< Offset 0x000 TWI Slave Address Register */
    volatile uint32_t TWI_XADDR;                                                /*!< Offset 0x004 TWI Extended Slave Address Register */
    volatile uint32_t TWI_DATA;                                                 /*!< Offset 0x008 TWI Data Byte Register */
    volatile uint32_t TWI_CNTR;                                                 /*!< Offset 0x00C TWI Control Register */
    volatile uint32_t TWI_STAT;                                                 /*!< Offset 0x010 TWI Status Register */
    volatile uint32_t TWI_CCR;                                                  /*!< Offset 0x014 TWI Clock Control Register */
    volatile uint32_t TWI_SRST;                                                 /*!< Offset 0x018 TWI Software Reset Register */
    volatile uint32_t TWI_EFR;                                                  /*!< Offset 0x01C TWI Enhance Feature Register */
    volatile uint32_t TWI_LCR;                                                  /*!< Offset 0x020 TWI Line Control Register */
             uint32_t reserved_0x024 [0x0077];
    volatile uint32_t TWI_DRV_CTRL;                                             /*!< Offset 0x200 TWI_DRV Control Register */
    volatile uint32_t TWI_DRV_CFG;                                              /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    volatile uint32_t TWI_DRV_SLV;                                              /*!< Offset 0x208 TWI_DRV Slave ID Register */
    volatile uint32_t TWI_DRV_FMT;                                              /*!< Offset 0x20C TWI_DRV Packet Format Register */
    volatile uint32_t TWI_DRV_BUS_CTRL;                                         /*!< Offset 0x210 TWI_DRV Bus Control Register */
    volatile uint32_t TWI_DRV_INT_CTRL;                                         /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    volatile uint32_t TWI_DRV_DMA_CFG;                                          /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    volatile uint32_t TWI_DRV_FIFO_CON;                                         /*!< Offset 0x21C TWI_DRV FIFO Content Register */
             uint32_t reserved_0x220 [0x0038];
    volatile uint32_t TWI_DRV_SEND_FIFO_ACC;                                    /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    volatile uint32_t TWI_DRV_RECV_FIFO_ACC;                                    /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
} TWI_TypeDef; /* size of structure = 0x308 */
/*
 * @brief SPI
 */
/*!< SPI Controller Interface */
typedef struct SPI_Type
{
             uint32_t reserved_0x000;
    volatile uint32_t SPI_GCR;                                                  /*!< Offset 0x004 SPI Global Control Register */
    volatile uint32_t SPI_TCR;                                                  /*!< Offset 0x008 SPI Transfer Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t SPI_IER;                                                  /*!< Offset 0x010 SPI Interrupt Control Register */
    volatile uint32_t SPI_ISR;                                                  /*!< Offset 0x014 SPI Interrupt Status Register */
    volatile uint32_t SPI_FCR;                                                  /*!< Offset 0x018 SPI FIFO Control Register */
    volatile uint32_t SPI_FSR;                                                  /*!< Offset 0x01C SPI FIFO Status Register */
    volatile uint32_t SPI_WCR;                                                  /*!< Offset 0x020 SPI Wait Clock Register */
             uint32_t reserved_0x024;
    volatile uint32_t SPI_SAMP_DL;                                              /*!< Offset 0x028 SPI Sample Delay Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t SPI_MBC;                                                  /*!< Offset 0x030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                                                  /*!< Offset 0x034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                                                  /*!< Offset 0x038 SPI Master Burst Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t SPI_BATCR;                                                /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_BA_CCR;                                               /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                                                  /*!< Offset 0x048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                                                  /*!< Offset 0x04C SPI RX Bit Register */
             uint32_t reserved_0x050 [0x000E];
    volatile uint32_t SPI_NDMA_MODE_CTL;                                        /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
             uint32_t reserved_0x08C [0x005D];
    volatile uint32_t SPI_TXD;                                                  /*!< Offset 0x200 SPI TX Data Register */
             uint32_t reserved_0x204 [0x003F];
    volatile uint32_t SPI_RXD;                                                  /*!< Offset 0x300 SPI RX Data Register */
} SPI_TypeDef; /* size of structure = 0x304 */
/*
 * @brief CIR_RX
 */
/*!< CIR_RX Controller Interface */
typedef struct CIR_RX_Type
{
    volatile uint32_t CIR_CTL;                                                  /*!< Offset 0x000 CIR Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t CIR_RXPCFG;                                               /*!< Offset 0x010 CIR Receiver Pulse Configure Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t CIR_RXFIFO;                                               /*!< Offset 0x020 CIR Receiver FIFO Register */
             uint32_t reserved_0x024 [0x0002];
    volatile uint32_t CIR_RXINT;                                                /*!< Offset 0x02C CIR Receiver Interrupt Control Register */
    volatile uint32_t CIR_RXSTA;                                                /*!< Offset 0x030 CIR Receiver Status Register */
    volatile uint32_t CIR_RXCFG;                                                /*!< Offset 0x034 CIR Receiver Configure Register */
} CIR_RX_TypeDef; /* size of structure = 0x038 */
/*
 * @brief CIR_TX
 */
/*!< CIR_TX Controller Interface */
typedef struct CIR_TX_Type
{
    volatile uint32_t CIR_TGLR;                                                 /*!< Offset 0x000 CIR Transmit Global Register */
    volatile uint32_t CIR_TMCR;                                                 /*!< Offset 0x004 CIR Transmit Modulation Control Register */
    volatile uint32_t CIR_TCR;                                                  /*!< Offset 0x008 CIR Transmit Control Register */
    volatile uint32_t CIR_IDC_H;                                                /*!< Offset 0x00C CIR Transmit Idle Duration Threshold High Bit Register */
    volatile uint32_t CIR_IDC_L;                                                /*!< Offset 0x010 CIR Transmit Idle Duration Threshold Low Bit Register */
    volatile uint32_t CIR_TICR_H;                                               /*!< Offset 0x014 CIR Transmit Idle Counter High Bit Register */
    volatile uint32_t CIR_TICR_L;                                               /*!< Offset 0x018 CIR Transmit Idle Counter Low Bit Register */
             uint32_t reserved_0x01C;
    volatile uint32_t CIR_TEL;                                                  /*!< Offset 0x020 CIR TX FIFO Empty Level Register */
    volatile uint32_t CIR_TXINT;                                                /*!< Offset 0x024 CIR Transmit Interrupt Control Register */
    volatile uint32_t CIR_TAC;                                                  /*!< Offset 0x028 CIR Transmit FIFO Available Counter Register */
    volatile uint32_t CIR_TXSTA;                                                /*!< Offset 0x02C CIR Transmit Status Register */
    volatile uint32_t CIR_TXT;                                                  /*!< Offset 0x030 CIR Transmit Threshold Register */
    volatile uint32_t CIR_DMA;                                                  /*!< Offset 0x034 CIR DMA Control Register */
             uint32_t reserved_0x038 [0x0012];
    volatile uint32_t CIR_TXFIFO;                                               /*!< Offset 0x080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef; /* size of structure = 0x084 */
/*
 * @brief LEDC
 */
/*!< LEDC Controller Interface */
typedef struct LEDC_Type
{
    volatile uint32_t LEDC_CTRL_REG;                                            /*!< Offset 0x000 LEDC Control Register */
    volatile uint32_t LED_T01_TIMING_CTRL_REG;                                  /*!< Offset 0x004 LEDC T0 & T1 Timing Control Register */
    volatile uint32_t LEDC_DATA_FINISH_CNT_REG;                                 /*!< Offset 0x008 LEDC Data Finish Counter Register */
    volatile uint32_t LED_RESET_TIMING_CTRL_REG;                                /*!< Offset 0x00C LEDC Reset Timing Control Register */
    volatile uint32_t LEDC_WAIT_TIME0_CTRL_REG;                                 /*!< Offset 0x010 LEDC Wait Time0 Control Register */
    volatile uint32_t LEDC_DATA_REG;                                            /*!< Offset 0x014 LEDC Data Register */
    volatile uint32_t LEDC_DMA_CTRL_REG;                                        /*!< Offset 0x018 LEDC DMA Control Register */
    volatile uint32_t LEDC_INT_CTRL_REG;                                        /*!< Offset 0x01C LEDC Interrupt Control Register */
    volatile uint32_t LEDC_INT_STS_REG;                                         /*!< Offset 0x020 LEDC Interrupt Status Register */
             uint32_t reserved_0x024;
    volatile uint32_t LEDC_WAIT_TIME1_CTRL_REG;                                 /*!< Offset 0x028 LEDC Wait Time1 Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t LEDC_FIFO_DATA_REG [0x020];                               /*!< Offset 0x030 LEDC FIFO Data Registers array */
} LEDC_TypeDef; /* size of structure = 0x0B0 */
/*
 * @brief TPADC
 */
/*!< TPADC Controller Interface */
typedef struct TPADC_Type
{
    volatile uint32_t TP_CTRL_REG0;                                             /*!< Offset 0x000 TP Control Register 0 */
    volatile uint32_t TP_CTRL_REG1;                                             /*!< Offset 0x004 TP Control Register 1 */
    volatile uint32_t TP_CTRL_REG2;                                             /*!< Offset 0x008 TP Control Register 2 */
    volatile uint32_t TP_CTRL_REG3;                                             /*!< Offset 0x00C TP Control Register 3 */
    volatile uint32_t TP_INT_FIFO_CTRL_REG;                                     /*!< Offset 0x010 TP Interrupt FIFO Control Register */
    volatile uint32_t TP_INT_FIFO_STAT_REG;                                     /*!< Offset 0x014 TP Interrupt FIFO Status Register */
             uint32_t reserved_0x018;
    volatile uint32_t TP_CALI_DATA_REG;                                         /*!< Offset 0x01C TP Calibration Data Register */
             uint32_t reserved_0x020;
    volatile uint32_t TP_DATA_REG;                                              /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef; /* size of structure = 0x028 */
/*
 * @brief GPADC
 */
/*!< GPADC Controller Interface */
typedef struct GPADC_Type
{
    volatile uint32_t GP_SR_CON;                                                /*!< Offset 0x000 GPADC Sample Rate Configure Register */
    volatile uint32_t GP_CTRL;                                                  /*!< Offset 0x004 GPADC Control Register */
    volatile uint32_t GP_CS_EN;                                                 /*!< Offset 0x008 GPADC Compare and Select Enable Register */
    volatile uint32_t GP_FIFO_INTC;                                             /*!< Offset 0x00C GPADC FIFO Interrupt Control Register */
    volatile uint32_t GP_FIFO_INTS;                                             /*!< Offset 0x010 GPADC FIFO Interrupt Status Register */
    volatile uint32_t GP_FIFO_DATA;                                             /*!< Offset 0x014 GPADC FIFO Data Register */
    volatile uint32_t GP_CDATA;                                                 /*!< Offset 0x018 GPADC Calibration Data Register */
             uint32_t reserved_0x01C;
    volatile uint32_t GP_DATAL_INTC;                                            /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
    volatile uint32_t GP_DATAH_INTC;                                            /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
    volatile uint32_t GP_DATA_INTC;                                             /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
             uint32_t reserved_0x02C;
    volatile uint32_t GP_DATAL_INTS;                                            /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
    volatile uint32_t GP_DATAH_INTS;                                            /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
    volatile uint32_t GP_DATA_INTS;                                             /*!< Offset 0x038 GPADC Data Interrupt Status Register */
             uint32_t reserved_0x03C;
    volatile uint32_t GP_CH0_CMP_DATA;                                          /*!< Offset 0x040 GPADC CH0 Compare Data Register */
             uint32_t reserved_0x044 [0x000F];
    volatile uint32_t GP_CH0_DATA;                                              /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef; /* size of structure = 0x084 */
/*
 * @brief SPI_DBI
 */
/*!< SPI_DBI Controller Interface */
typedef struct SPI_DBI_Type
{
             uint32_t reserved_0x000;
    volatile uint32_t SPI_GCR;                                                  /*!< Offset 0x004 SPI Global Control Register */
    volatile uint32_t SPI_TCR;                                                  /*!< Offset 0x008 SPI Transfer Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t SPI_IER;                                                  /*!< Offset 0x010 SPI Interrupt Control Register */
    volatile uint32_t SPI_ISR;                                                  /*!< Offset 0x014 SPI Interrupt Status Register */
    volatile uint32_t SPI_FCR;                                                  /*!< Offset 0x018 SPI FIFO Control Register */
    volatile uint32_t SPI_FSR;                                                  /*!< Offset 0x01C SPI FIFO Status Register */
    volatile uint32_t SPI_WCR;                                                  /*!< Offset 0x020 SPI Wait Clock Register */
             uint32_t reserved_0x024;
    volatile uint32_t SPI_SAMP_DL;                                              /*!< Offset 0x028 SPI Sample Delay Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t SPI_MBC;                                                  /*!< Offset 0x030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                                                  /*!< Offset 0x034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                                                  /*!< Offset 0x038 SPI Master Burst Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t SPI_BATCR;                                                /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_BA_CCR;                                               /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                                                  /*!< Offset 0x048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                                                  /*!< Offset 0x04C SPI RX Bit Register */
             uint32_t reserved_0x050 [0x000E];
    volatile uint32_t SPI_NDMA_MODE_CTL;                                        /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
             uint32_t reserved_0x08C [0x001D];
    volatile uint32_t DBI_CTL_0;                                                /*!< Offset 0x100 DBI Control Register 0 */
    volatile uint32_t DBI_CTL_1;                                                /*!< Offset 0x104 DBI Control Register 1 */
    volatile uint32_t DBI_CTL_2;                                                /*!< Offset 0x108 DBI Control Register 2 */
    volatile uint32_t DBI_TIMER;                                                /*!< Offset 0x10C DBI Timer Control Register */
    volatile uint32_t DBI_VIDEO_SZIE;                                           /*!< Offset 0x110 DBI Video Size Configuration Register */
             uint32_t reserved_0x114 [0x0003];
    volatile uint32_t DBI_INT;                                                  /*!< Offset 0x120 DBI Interrupt Register */
    volatile uint32_t DBI_DEBUG_0;                                              /*!< Offset 0x124 DBI BEBUG 0 Register */
    volatile uint32_t DBI_DEBUG_1;                                              /*!< Offset 0x128 DBI BEBUG 1 Register */
             uint32_t reserved_0x12C [0x0035];
    volatile uint32_t SPI_TXD;                                                  /*!< Offset 0x200 SPI TX Data register */
             uint32_t reserved_0x204 [0x003F];
    volatile uint32_t SPI_RXD;                                                  /*!< Offset 0x300 SPI RX Data register */
} SPI_DBI_TypeDef; /* size of structure = 0x304 */
/*
 * @brief CE
 */
/*!< CE Controller Interface */
typedef struct CE_Type
{
    volatile uint32_t CE_TDA;                                                   /*!< Offset 0x000 Task Descriptor Address */
             uint32_t reserved_0x004;
    volatile uint32_t CE_ICR;                                                   /*!< Offset 0x008 Interrupt Control Register */
    volatile uint32_t CE_ISR;                                                   /*!< Offset 0x00C Interrupt Status Register */
    volatile uint32_t CE_TLR;                                                   /*!< Offset 0x010 Task Load Register */
    volatile uint32_t CE_TSR;                                                   /*!< Offset 0x014 Task Status Register */
    volatile uint32_t CE_ESR;                                                   /*!< Offset 0x018 Error Status Register */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CE_CSA;                                                   /*!< Offset 0x024 DMA Current Source Address */
    volatile uint32_t CE_CDA;                                                   /*!< Offset 0x028 DMA Current Destination Address */
    volatile uint32_t CE_TPR;                                                   /*!< Offset 0x02C Throughput Register */
} CE_TypeDef; /* size of structure = 0x030 */

/* Access pointers */

#define CPU_SUBSYS_CTRL ((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)      /*!< \brief CPU_SUBSYS_CTRL Interface register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                                          /*!< \brief CCU Interface register set access pointer */
#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)                              /*!< \brief SYS_CFG Interface register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)                                     /*!< \brief UART0 Interface register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)                                     /*!< \brief UART1 Interface register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)                                     /*!< \brief UART2 Interface register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)                                     /*!< \brief UART3 Interface register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)                                     /*!< \brief UART4 Interface register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)                                     /*!< \brief GPIOB Interface register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)                                     /*!< \brief GPIOC Interface register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)                                     /*!< \brief GPIOD Interface register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)                                     /*!< \brief GPIOE Interface register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)                                     /*!< \brief GPIOF Interface register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)                                     /*!< \brief GPIOG Interface register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)                                     /*!< \brief GPIOH Interface register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)                            /*!< \brief GPIOINTB Interface register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)                            /*!< \brief GPIOINTC Interface register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)                            /*!< \brief GPIOINTD Interface register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)                            /*!< \brief GPIOINTE Interface register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)                            /*!< \brief GPIOINTF Interface register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)                            /*!< \brief GPIOINTG Interface register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)                            /*!< \brief GPIOINTH Interface register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)                        /*!< \brief GPIOBLOCK Interface register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)                                     /*!< \brief SMHC0 Interface register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)                                     /*!< \brief SMHC1 Interface register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)                                     /*!< \brief SMHC2 Interface register set access pointer */
#define I2S0 ((I2S_PCM_TypeDef *) I2S0_BASE)                                    /*!< \brief I2S0 Interface register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)                                    /*!< \brief I2S1 Interface register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)                                    /*!< \brief I2S2 Interface register set access pointer */
#define DMIC ((DMIC_TypeDef *) DMIC_BASE)                                       /*!< \brief DMIC Interface register set access pointer */
#define OWA ((OWA_TypeDef *) OWA_BASE)                                          /*!< \brief OWA Interface register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)                  /*!< \brief AUDIO_CODEC Interface register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)                                        /*!< \brief TWI0 Interface register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)                                        /*!< \brief TWI1 Interface register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)                                        /*!< \brief TWI2 Interface register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)                                        /*!< \brief SPI0 Interface register set access pointer */
#define CIR_RX ((CIR_RX_TypeDef *) CIR_RX_BASE)                                 /*!< \brief CIR_RX Interface register set access pointer */
#define CIR_TX ((CIR_TX_TypeDef *) CIR_TX_BASE)                                 /*!< \brief CIR_TX Interface register set access pointer */
#define LEDC ((LEDC_TypeDef *) LEDC_BASE)                                       /*!< \brief LEDC Interface register set access pointer */
#define TPADC ((TPADC_TypeDef *) TPADC_BASE)                                    /*!< \brief TPADC Interface register set access pointer */
#define GPADC ((GPADC_TypeDef *) GPADC_BASE)                                    /*!< \brief GPADC Interface register set access pointer */
#define SPI_DBI ((SPI_DBI_TypeDef *) SPI_DBI_BASE)                              /*!< \brief SPI_DBI Interface register set access pointer */
#define CE_NS ((CE_TypeDef *) CE_NS_BASE)                                       /*!< \brief CE_NS Interface register set access pointer */
#define CE_S ((CE_TypeDef *) CE_S_BASE)                                         /*!< \brief CE_S Interface register set access pointer */

#endif /* HEADER_00003039_INCLUDED */
/* Generated section end */ 
