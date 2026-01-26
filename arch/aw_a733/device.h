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
    CLK_DET_IRQn = 73,                                /*!< CCU Clock Controller Unit (CCU) */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define CCU_BASE ((uintptr_t) 0x03001000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define GIC_BASE ((uintptr_t) 0x03020000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!< GIC_DISTRIBUTOR  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x03024000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x03025000)            /*!< GICV  Base */

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


/* Defines */



/* Access pointers */

#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__2A4CD94A_A755_4786_BB96_E52021B2959C__INCLUDED */
