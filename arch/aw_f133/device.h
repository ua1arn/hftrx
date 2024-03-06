#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    UART0_IRQn = 18,                                  /*!< UART  */
    UART1_IRQn = 19,                                  /*!< UART  */
    UART2_IRQn = 20,                                  /*!< UART  */
    UART3_IRQn = 21,                                  /*!< UART  */
    UART4_IRQn = 22,                                  /*!< UART  */
    UART5_IRQn = 23,                                  /*!< UART  */
    TWI0_IRQn = 25,                                   /*!< TWI  */
    TWI1_IRQn = 26,                                   /*!< TWI  */
    TWI2_IRQn = 27,                                   /*!< TWI  */
    TWI3_IRQn = 28,                                   /*!< TWI  */
    SPI0_IRQn = 31,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 32,                                   /*!< SPI Serial Peripheral Interface */
    LEDC_IRQn = 36,                                   /*!< LEDC LED Lamp Controller */
    CAN0_IRQn = 37,                                   /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
    CAN1_IRQn = 38,                                   /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
    OWA_IRQn = 39,                                    /*!< OWA One Wire Audio (TX only) */
    AUDIO_CODEC_IRQn = 41,                            /*!< AUDIO_CODEC Audio Codec */
    I2S_PCM1_IRQn = 43,                               /*!< I2S_PCM  */
    I2S_PCM2_IRQn = 44,                               /*!< I2S_PCM  */
    USB0_DEVICE_IRQn = 45,                            /*!< USBOTG  */
    USB0_EHCI_IRQn = 46,                              /*!< USB_EHCI_Capability  */
    USB0_OHCI_IRQn = 47,                              /*!< USB_OHCI_Capability  */
    USB1_EHCI_IRQn = 49,                              /*!< USB_EHCI_Capability  */
    USB1_OHCI_IRQn = 50,                              /*!< USB_OHCI_Capability  */
    SMHC0_IRQn = 56,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC1_IRQn = 57,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC2_IRQn = 58,                                  /*!< SMHC SD-MMC Host Controller */
    MCTL_IRQn = 59,                                   /*!< MCTL_COM  */
    EMAC_IRQn = 62,                                   /*!< EMAC  */
    CCU_FERR_IRQn = 64,                               /*!< CCU Clock Controller Unit (CCU) */
    DMAC_NS_IRQn = 66,                                /*!< DMAC  */
    CE_NS_IRQn = 68,                                  /*!< CE Crypto Engine (CE) */
    HSTIMER0_IRQn = 71,                               /*!< HSTIMER High Speed Timer (HSTimer) */
    HSTIMER1_IRQn = 72,                               /*!< HSTIMER High Speed Timer (HSTimer) */
    GPADC_IRQn = 73,                                  /*!< GPADC  */
    THS_IRQn = 74,                                    /*!< THS Thermal Sensor */
    TIMER0_IRQn = 75,                                 /*!< TIMER  */
    TIMER1_IRQn = 76,                                 /*!< TIMER  */
    TPADC_IRQn = 78,                                  /*!< TPADC  */
    WATCHDOG_IRQn = 79,                               /*!< TIMER  */
    IOMMU_IRQn = 80,                                  /*!< IOMMU  */
    VE_IRQn = 82,                                     /*!< VE Video Encoding */
    GPIOB_NS_IRQn = 85,                               /*!< GPIOINT  */
    GPIOC_NS_IRQn = 87,                               /*!< GPIOINT  */
    GPIOD_NS_IRQn = 89,                               /*!< GPIOINT  */
    GPIOE_NS_IRQn = 91,                               /*!< GPIOINT  */
    GPIOF_NS_IRQn = 93,                               /*!< GPIOINT  */
    GPIOG_NS_IRQn = 95,                               /*!< GPIOINT  */
    DE_IRQn = 103,                                    /*!< DE_TOP Display Engine (DE) TOP */
    DI_IRQn = 104,                                    /*!< DI De-interlacer (DI) */
    G2D_IRQn = 105,                                   /*!< G2D_TOP Graphic 2D top */
    TVE_IRQn = 107,                                   /*!< TVE_TOP TV Output (TV_Encoder) */
    DSI0_IRQn = 108,                                  /*!< DSI MIPI DSI Display Interface */
    TV_Encoder_IRQn = 110,                            /*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) */
    CSIC_DMA0_IRQn = 111,                             /*!< CSIC_DMA  */
    CSIC_DMA1_IRQn = 112,                             /*!< CSIC_DMA  */
    CSIC_PARSER0_IRQn = 116,                          /*!< CSIC_PARSER  */
    CSI_TOP_PKT_IRQn = 122,                           /*!< CSIC_TOP  */
    TVD_IRQn = 123,                                   /*!< TVD_TOP Video Decoding */
    ALARM0_IRQn = 160,                                /*!< RTC Real Time Clock */
    C0_CTI0_IRQn = 176,                               /*!< C0_CPUX_CFG  */
    C0_CTI1_IRQn = 177,                               /*!< C0_CPUX_CFG  */
    C0_COMMTX0_IRQn = 180,                            /*!< C0_CPUX_CFG  */
    C0_COMMTX1_IRQn = 181,                            /*!< C0_CPUX_CFG  */
    C0_COMMRX0_IRQn = 184,                            /*!< C0_CPUX_CFG  */
    C0_COMMRX1_IRQn = 185,                            /*!< C0_CPUX_CFG  */
    C0_PMU0_IRQn = 188,                               /*!< C0_CPUX_CFG  */
    C0_PMU1_IRQn = 189,                               /*!< C0_CPUX_CFG  */
    C0_AXI_ERROR_IRQn = 192,                          /*!< C0_CPUX_CFG  */
    AXI_WR_IRQ_IRQn = 194,                            /*!< C0_CPUX_CFG  */
    AXI_RD_IRQ_IRQn = 195,                            /*!< C0_CPUX_CFG  */
    DBGPWRUPREQ_out_0_IRQn = 196,                     /*!< C0_CPUX_CFG  */
    DBGPWRUPREQ_out_1_IRQn = 197,                     /*!< C0_CPUX_CFG  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define VENCODER_BASE ((uintptr_t) 0x01C0E000)        /*!< VE Video Encoding Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x02000000)       /*!< GPIOBLOCK  Base */
#define GPIOB_BASE ((uintptr_t) 0x02000030)           /*!< GPIO  Base */
#define GPIOC_BASE ((uintptr_t) 0x02000060)           /*!< GPIO  Base */
#define GPIOD_BASE ((uintptr_t) 0x02000090)           /*!< GPIO  Base */
#define GPIOE_BASE ((uintptr_t) 0x020000C0)           /*!< GPIO  Base */
#define GPIOF_BASE ((uintptr_t) 0x020000F0)           /*!< GPIO  Base */
#define GPIOG_BASE ((uintptr_t) 0x02000120)           /*!< GPIO  Base */
#define GPIOINTB_BASE ((uintptr_t) 0x02000220)        /*!< GPIOINT  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x02000240)        /*!< GPIOINT  Base */
#define GPIOINTD_BASE ((uintptr_t) 0x02000260)        /*!< GPIOINT  Base */
#define GPIOINTE_BASE ((uintptr_t) 0x02000280)        /*!< GPIOINT  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x020002A0)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x020002C0)        /*!< GPIOINT  Base */
#define PWM_BASE ((uintptr_t) 0x02000C00)             /*!< PWM Pulse Width Modulation module Base */
#define CCU_BASE ((uintptr_t) 0x02001000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define CIR_TX_BASE ((uintptr_t) 0x02003000)          /*!< CIR_TX  Base */
#define LEDC_BASE ((uintptr_t) 0x02008000)            /*!< LEDC LED Lamp Controller Base */
#define GPADC_BASE ((uintptr_t) 0x02009000)           /*!< GPADC  Base */
#define THS_BASE ((uintptr_t) 0x02009400)             /*!< THS Thermal Sensor Base */
#define TPADC_BASE ((uintptr_t) 0x02009C00)           /*!< TPADC  Base */
#define IOMMU_BASE ((uintptr_t) 0x02010000)           /*!< IOMMU  Base */
#define AUDIO_CODEC_BASE ((uintptr_t) 0x02030000)     /*!< AUDIO_CODEC Audio Codec Base */
#define DMIC_BASE ((uintptr_t) 0x02031000)            /*!< DMIC  Base */
#define I2S1_BASE ((uintptr_t) 0x02033000)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x02034000)            /*!< I2S_PCM  Base */
#define OWA_BASE ((uintptr_t) 0x02036000)             /*!< OWA One Wire Audio (TX only) Base */
#define TIMER_BASE ((uintptr_t) 0x02050000)           /*!< TIMER  Base */
#define UART0_BASE ((uintptr_t) 0x02500000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x02500400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x02500800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x02500C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x02501000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0x02501400)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x02502000)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x02502400)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x02502800)            /*!< TWI  Base */
#define TWI3_BASE ((uintptr_t) 0x02502C00)            /*!< TWI  Base */
#define CAN0_BASE ((uintptr_t) 0x02504000)            /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) Base */
#define CAN1_BASE ((uintptr_t) 0x02504400)            /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) Base */
#define SYS_CFG_BASE ((uintptr_t) 0x03000000)         /*!< SYS_CFG  Base */
#define DMAC_BASE ((uintptr_t) 0x03002000)            /*!< DMAC  Base */
#define SID_BASE ((uintptr_t) 0x03006000)             /*!< SID Security ID Base */
#define HSTIMER_BASE ((uintptr_t) 0x03008000)         /*!< HSTIMER High Speed Timer (HSTimer) Base */
#define CE_NS_BASE ((uintptr_t) 0x03040000)           /*!< CE Crypto Engine (CE) Base */
#define CE_S_BASE ((uintptr_t) 0x03040800)            /*!< CE Crypto Engine (CE) Base */
#define MCTL_COM_BASE ((uintptr_t) 0x03102000)        /*!< MCTL_COM  Base */
#define DDRPHYC_BASE ((uintptr_t) 0x03103000)         /*!< DDRPHYC  Base */
#define MCTL_PHY_BASE ((uintptr_t) 0x03103000)        /*!< MCTL_PHY  Base */
#define SMHC0_BASE ((uintptr_t) 0x04020000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC1_BASE ((uintptr_t) 0x04021000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC2_BASE ((uintptr_t) 0x04022000)           /*!< SMHC SD-MMC Host Controller Base */
#define SPI0_BASE ((uintptr_t) 0x04025000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x04026000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI_DBI_BASE ((uintptr_t) 0x04026000)         /*!< SPI Serial Peripheral Interface Base */
#define USBOTG0_BASE ((uintptr_t) 0x04100000)         /*!< USBOTG  Base */
#define USBPHY0_BASE ((uintptr_t) 0x04100400)         /*!< USBPHYC  Base */
#define USBEHCI0_BASE ((uintptr_t) 0x04101000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI0_BASE ((uintptr_t) 0x04101400)        /*!< USB_OHCI_Capability  Base */
#define USBEHCI1_BASE ((uintptr_t) 0x04200000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI1_BASE ((uintptr_t) 0x04200400)        /*!< USB_OHCI_Capability  Base */
#define USBPHY1_BASE ((uintptr_t) 0x04200800)         /*!< USBPHYC  Base */
#define EMAC_BASE ((uintptr_t) 0x04500000)            /*!< EMAC  Base */
#define DE_BASE ((uintptr_t) 0x05000000)              /*!< DE Display Engine (DE) Base */
#define DE_TOP_BASE ((uintptr_t) 0x05000000)          /*!< DE_TOP Display Engine (DE) TOP Base */
#define DE_GLB_BASE ((uintptr_t) 0x05100000)          /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DE_BLD_BASE ((uintptr_t) 0x05101000)          /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_VI1_BASE ((uintptr_t) 0x05102000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_UI1_BASE ((uintptr_t) 0x05103000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_VEP0_BASE ((uintptr_t) 0x05120000)         /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DE_VEP1_BASE ((uintptr_t) 0x05140000)         /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DE_DEP_BASE ((uintptr_t) 0x051A0000)          /*!< DE_DEP DRC (dynamic range controller) Base */
#define DEb_GLB_BASE ((uintptr_t) 0x05200000)         /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DEb_BLD_BASE ((uintptr_t) 0x05201000)         /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DEb_VI1_BASE ((uintptr_t) 0x05202000)         /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DEb_VEP0_BASE ((uintptr_t) 0x05220000)        /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DI_BASE ((uintptr_t) 0x05400000)              /*!< DI De-interlacer (DI) Base */
#define G2D_TOP_BASE ((uintptr_t) 0x05410000)         /*!< G2D_TOP Graphic 2D top Base */
#define G2D_MIXER_BASE ((uintptr_t) 0x05410100)       /*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer Base */
#define G2D_BLD_BASE ((uintptr_t) 0x05410400)         /*!< G2D_BLD Graphic 2D (G2D) Engine Blender Base */
#define G2D_V0_BASE ((uintptr_t) 0x05410800)          /*!< G2D_VI Graphic 2D VI surface Base */
#define G2D_UI0_BASE ((uintptr_t) 0x05411000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI1_BASE ((uintptr_t) 0x05411800)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI2_BASE ((uintptr_t) 0x05412000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_WB_BASE ((uintptr_t) 0x05413000)          /*!< G2D_WB Graphic 2D (G2D) Engine Write Back Base */
#define G2D_VSU_BASE ((uintptr_t) 0x05418000)         /*!< G2D_VSU Graphic 2D Video Scaler Base */
#define G2D_ROT_BASE ((uintptr_t) 0x05438000)         /*!< G2D_ROT Graphic 2D Rotate Base */
#define DSI0_BASE ((uintptr_t) 0x05450000)            /*!< DSI MIPI DSI Display Interface Base */
#define DSI_DPHY_BASE ((uintptr_t) 0x05451000)        /*!< DSI_DPHY MIPI DSI Physical Interface Base */
#define DISPLAY_TOP_BASE ((uintptr_t) 0x05460000)     /*!< DISPLAY_TOP display interface top (DISPLAY_TOP) Base */
#define TCON_LCD0_BASE ((uintptr_t) 0x05461000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define TCON_TV0_BASE ((uintptr_t) 0x05470000)        /*!< TCON_TV  Base */
#define TVE_TOP_BASE ((uintptr_t) 0x05600000)         /*!< TVE_TOP TV Output (TV_Encoder) Base */
#define TV_Encoder_BASE ((uintptr_t) 0x05604000)      /*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) Base */
#define CSI_BASE ((uintptr_t) 0x05800000)             /*!< CSI  Base */
#define CSIC_CCU_BASE ((uintptr_t) 0x05800000)        /*!< CSIC_CCU  Base */
#define CSIC_TOP_BASE ((uintptr_t) 0x05800800)        /*!< CSIC_TOP  Base */
#define CSIC_PARSER0_BASE ((uintptr_t) 0x05801000)    /*!< CSIC_PARSER  Base */
#define CSIC_DMA0_BASE ((uintptr_t) 0x05809000)       /*!< CSIC_DMA  Base */
#define CSIC_DMA1_BASE ((uintptr_t) 0x05809200)       /*!< CSIC_DMA  Base */
#define TVD_TOP_BASE ((uintptr_t) 0x05C00000)         /*!< TVD_TOP Video Decoding Base */
#define TVD0_BASE ((uintptr_t) 0x05C01000)            /*!< TVD0 Video Decoding Base */
#define RISC_CFG_BASE ((uintptr_t) 0x06010000)        /*!< RISC_CFG RISC-V core configuration register Base */
#define RISC_WDG_BASE ((uintptr_t) 0x06011000)        /*!< RISC_WDG  Base */
#define RISC_TIMESTAMP_BASE ((uintptr_t) 0x06012000)  /*!< RISC_TIMESTAMP  Base */
#define R_CPUCFG_BASE ((uintptr_t) 0x07000400)        /*!< R_CPUCFG  Base */
#define R_CCU_BASE ((uintptr_t) 0x07010000)           /*!< R_CCU  Base */
#define R_PRCM_BASE ((uintptr_t) 0x07010000)          /*!< R_PRCM  Base */
#define CIR_RX_BASE ((uintptr_t) 0x07040000)          /*!< CIR_RX  Base */
#define RTC_BASE ((uintptr_t) 0x07090000)             /*!< RTC Real Time Clock Base */
#define C0_CPUX_CFG_BASE ((uintptr_t) 0x09010000)     /*!< C0_CPUX_CFG  Base */
#define C0_CPUX_MBIST_BASE ((uintptr_t) 0x09020000)   /*!< C0_CPUX_MBIST  Base */
#define PLIC_BASE ((uintptr_t) 0x10000000)            /*!< PLIC Platform-Level Interrupt Controller (PLIC) Base */
#define CLINT_BASE ((uintptr_t) 0x14000000)           /*!< CLINT  Base */

/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Audio Codec */
typedef struct AUDIO_CODEC_Type
{
    volatile uint32_t AC_DAC_DPC;                     /*!< Offset 0x000 DAC Digital Part Control Register */
    volatile uint32_t DAC_VOL_CTRL;                   /*!< Offset 0x004 DAC Volume Control Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t AC_DAC_FIFOC;                   /*!< Offset 0x010 DAC FIFO Control Register */
    volatile uint32_t AC_DAC_FIFOS;                   /*!< Offset 0x014 DAC FIFO Status Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t AC_DAC_TXDATA;                  /*!< Offset 0x020 DAC TX DATA Register */
    volatile uint32_t AC_DAC_CNT;                     /*!< Offset 0x024 DAC TX FIFO Counter Register */
    volatile uint32_t AC_DAC_DG;                      /*!< Offset 0x028 DAC Debug Register */
             uint32_t reserved_0x02C;
    volatile uint32_t AC_ADC_FIFOC;                   /*!< Offset 0x030 ADC FIFO Control Register */
    volatile uint32_t ADC_VOL_CTRL1;                  /*!< Offset 0x034 ADC Volume Control1 Register */
    volatile uint32_t AC_ADC_FIFOS;                   /*!< Offset 0x038 ADC FIFO Status Register */
             uint32_t reserved_0x03C;
    volatile uint32_t AC_ADC_RXDATA;                  /*!< Offset 0x040 ADC RX Data Register */
    volatile uint32_t AC_ADC_CNT;                     /*!< Offset 0x044 ADC RX Counter Register */
             uint32_t reserved_0x048;
    volatile uint32_t AC_ADC_DG;                      /*!< Offset 0x04C ADC Debug Register */
    volatile uint32_t ADC_DIG_CTRL;                   /*!< Offset 0x050 ADC Digtial Control Register */
    volatile uint32_t VRA1SPEEDUP_DOWN_CTRL;          /*!< Offset 0x054 VRA1 Speedup Down Control Register */
             uint32_t reserved_0x058 [0x0026];
    volatile uint32_t AC_DAC_DAP_CTRL;                /*!< Offset 0x0F0 DAC DAP Control Register */
             uint32_t reserved_0x0F4;
    volatile uint32_t AC_ADC_DAP_CTR;                 /*!< Offset 0x0F8 ADC DAP Control Register */
             uint32_t reserved_0x0FC;
    volatile uint32_t AC_DAC_DRC_HHPFC;               /*!< Offset 0x100 DAC DRC High HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_LHPFC;               /*!< Offset 0x104 DAC DRC Low HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_CTRL;                /*!< Offset 0x108 DAC DRC Control Register */
    volatile uint32_t AC_DAC_DRC_LPFHAT;              /*!< Offset 0x10C DAC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLAT;              /*!< Offset 0x110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHAT;              /*!< Offset 0x114 DAC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLAT;              /*!< Offset 0x118 DAC DRC Right Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFHRT;              /*!< Offset 0x11C DAC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLRT;              /*!< Offset 0x120 DAC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHRT;              /*!< Offset 0x124 DAC DRC Right Peak filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLRT;              /*!< Offset 0x128 DAC DRC Right Peak filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSHAT;             /*!< Offset 0x12C DAC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSLAT;             /*!< Offset 0x130 DAC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSHAT;             /*!< Offset 0x134 DAC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSLAT;             /*!< Offset 0x138 DAC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_HCT;                 /*!< Offset 0x13C DAC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LCT;                 /*!< Offset 0x140 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_HKC;                 /*!< Offset 0x144 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKC;                 /*!< Offset 0x148 DAC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPC;                /*!< Offset 0x14C DAC DRC Compresso */
    volatile uint32_t AC_DAC_DRC_LOPC;                /*!< Offset 0x150 DAC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_DAC_DRC_HLT;                 /*!< Offset 0x154 DAC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LLT;                 /*!< Offset 0x158 DAC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKl;                 /*!< Offset 0x15C DAC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKl;                 /*!< Offset 0x160 DAC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPL;                /*!< Offset 0x164 DAC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_LOPL;                /*!< Offset 0x168 DAC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_HET;                 /*!< Offset 0x16C DAC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LET;                 /*!< Offset 0x170 DAC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKE;                 /*!< Offset 0x174 DAC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKE;                 /*!< Offset 0x178 DAC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPE;                /*!< Offset 0x17C DAC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_LOPE;                /*!< Offset 0x180 DAC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_HKN;                 /*!< Offset 0x184 DAC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKN;                 /*!< Offset 0x188 DAC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_SFHAT;               /*!< Offset 0x18C DAC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLAT;               /*!< Offset 0x190 DAC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFHRT;               /*!< Offset 0x194 DAC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLRT;               /*!< Offset 0x198 DAC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_MXGHS;               /*!< Offset 0x19C DAC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MXGLS;               /*!< Offset 0x1A0 DAC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGHS;               /*!< Offset 0x1A4 DAC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGLS;               /*!< Offset 0x1A8 DAC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_EPSHC;               /*!< Offset 0x1AC DAC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_DAC_DRC_EPSLC;               /*!< Offset 0x1B0 DAC DRC Expander Smooth Time Low Coef Register */
             uint32_t reserved_0x1B4;
    volatile uint32_t AC_DAC_DRC_HPFHGAIN;            /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_DAC_DRC_HPFLGAIN;            /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
             uint32_t reserved_0x1C0 [0x0010];
    volatile uint32_t AC_ADC_DRC_HHPFC;               /*!< Offset 0x200 ADC DRC High HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_LHPFC;               /*!< Offset 0x204 ADC DRC Low HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_CTRL;                /*!< Offset 0x208 ADC DRC Control Register */
    volatile uint32_t AC_ADC_DRC_LPFHAT;              /*!< Offset 0x20C ADC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLAT;              /*!< Offset 0x210 ADC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHAT;              /*!< Offset 0x214 ADC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLAT;              /*!< Offset 0x218 ADC DRC Right Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFHRT;              /*!< Offset 0x21C ADC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLRT;              /*!< Offset 0x220 ADC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHRT;              /*!< Offset 0x224 ADC DRC Right Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLRT;              /*!< Offset 0x228 ADC DRC Right Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSHAT;             /*!< Offset 0x22C ADC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSLAT;             /*!< Offset 0x230 ADC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSHAT;             /*!< Offset 0x234 ADC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSLAT;             /*!< Offset 0x238 ADC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_HCT;                 /*!< Offset 0x23C ADC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LCT;                 /*!< Offset 0x240 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_HKC;                 /*!< Offset 0x244 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKC;                 /*!< Offset 0x248 ADC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPC;                /*!< Offset 0x24C ADC DRC Compressor High Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_LOPC;                /*!< Offset 0x250 ADC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_HLT;                 /*!< Offset 0x254 ADC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LLT;                 /*!< Offset 0x258 ADC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKl;                 /*!< Offset 0x25C ADC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKl;                 /*!< Offset 0x260 ADC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPL;                /*!< Offset 0x264 ADC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_LOPL;                /*!< Offset 0x268 ADC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_HET;                 /*!< Offset 0x26C ADC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LET;                 /*!< Offset 0x270 ADC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKE;                 /*!< Offset 0x274 ADC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKE;                 /*!< Offset 0x278 ADC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPE;                /*!< Offset 0x27C ADC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_LOPE;                /*!< Offset 0x280 ADC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_HKN;                 /*!< Offset 0x284 ADC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKN;                 /*!< Offset 0x288 ADC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_SFHAT;               /*!< Offset 0x28C ADC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLAT;               /*!< Offset 0x290 ADC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFHRT;               /*!< Offset 0x294 ADC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLRT;               /*!< Offset 0x298 ADC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_MXGHS;               /*!< Offset 0x29C ADC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MXGLS;               /*!< Offset 0x2A0 ADC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGHS;               /*!< Offset 0x2A4 ADC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGLS;               /*!< Offset 0x2A8 ADC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_EPSHC;               /*!< Offset 0x2AC ADC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_ADC_DRC_EPSLC;               /*!< Offset 0x2B0 ADC DRC Expander Smooth Time Low Coef Register */
             uint32_t reserved_0x2B4;
    volatile uint32_t AC_ADC_DRC_HPFHGAIN;            /*!< Offset 0x2B8 ADC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_ADC_DRC_HPFLGAIN;            /*!< Offset 0x2BC ADC DRC HPF Gain Low Coef Register */
             uint32_t reserved_0x2C0 [0x0010];
    volatile uint32_t ADC1_REG;                       /*!< Offset 0x300 ADC1 Analog Control Register */
    volatile uint32_t ADC2_REG;                       /*!< Offset 0x304 ADC2 Analog Control Register */
    volatile uint32_t ADC3_REG;                       /*!< Offset 0x308 ADC3 Analog Control Register */
             uint32_t reserved_0x30C;
    volatile uint32_t DAC_REG;                        /*!< Offset 0x310 DAC Analog Control Register */
             uint32_t reserved_0x314;
    volatile uint32_t MICBIAS_REG;                    /*!< Offset 0x318 MICBIAS Analog Control Register */
    volatile uint32_t RAMP_REG;                       /*!< Offset 0x31C BIAS Analog Control Register */
    volatile uint32_t BIAS_REG;                       /*!< Offset 0x320 BIAS Analog Control Register */
             uint32_t reserved_0x324;
    volatile uint32_t HMIC_CTRL;                      /*!< Offset 0x328 * HMIC Control Register */
    volatile uint32_t HMIC_STS;                       /*!< Offset 0x32C * HMIC Status Register ( */
    volatile uint32_t ADC5_REG;                       /*!< Offset 0x330 ADC5 Analog Control Register */
             uint32_t reserved_0x334 [0x0003];
    volatile uint32_t HP2_REG;                        /*!< Offset 0x340 * Headphone2 Analog Control Register */
             uint32_t reserved_0x344;
    volatile uint32_t POWER_REG;                      /*!< Offset 0x348 * POWER Analog Control Register */
    volatile uint32_t ADC_CUR_REG;                    /*!< Offset 0x34C * ADC Current Analog Control Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x350 */
/*
 * @brief C0_CPUX_CFG
 */
/*!< C0_CPUX_CFG  */
typedef struct C0_CPUX_CFG_Type
{
    volatile uint32_t C0_RST_CTRL;                    /*!< Offset 0x000 Cluster 0 Reset Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t C0_CTRL_REG0;                   /*!< Offset 0x010 Cluster 0 Control Register0 */
    volatile uint32_t C0_CTRL_REG1;                   /*!< Offset 0x014 Cluster 0 Control Register1 */
    volatile uint32_t C0_CTRL_REG2;                   /*!< Offset 0x018 Cluster 0 Control Register2 */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CACHE_CFG_REG;                  /*!< Offset 0x024 Cache Configuration Register */
             uint32_t reserved_0x028 [0x0016];
    volatile uint32_t C0_CPU_STATUS;                  /*!< Offset 0x080 Cluster 0 CPU Status Register */
    volatile uint32_t L2_STATUS_REG;                  /*!< Offset 0x084 Cluster 0 L2 Status Register */
             uint32_t reserved_0x088 [0x000E];
    volatile uint32_t DBG_REG0;                       /*!< Offset 0x0C0 Cluster 0 Debug Control Register0 */
    volatile uint32_t DBG_REG1;                       /*!< Offset 0x0C4 Cluster 0 Debug Control Register1 */
             uint32_t reserved_0x0C8 [0x0002];
    volatile uint32_t AXI_MNT_CTRL_REG;               /*!< Offset 0x0D0 AXI Monitor Control Register */
    volatile uint32_t AXI_MNT_PRD_REG;                /*!< Offset 0x0D4 AXI Monitor Period Register */
    volatile uint32_t AXI_MNT_RLTCY_REG;              /*!< Offset 0x0D8 AXI Monitor Read Total Latency Register */
    volatile uint32_t AXI_MNT_WLTCY_REG;              /*!< Offset 0x0DC AXI Monitor Write Total Latency Register */
    volatile uint32_t AXI_MNT_RREQ_REG;               /*!< Offset 0x0E0 AXI Monitor Read Request Times Register */
    volatile uint32_t AXI_MNT_WREQ_REG;               /*!< Offset 0x0E4 AXI Monitor Write Request Times Register */
    volatile uint32_t AXI_MNT_RBD_REG;                /*!< Offset 0x0E8 AXI Monitor Read Bandwidth Register */
    volatile uint32_t AXI_MNT_WBD_REG;                /*!< Offset 0x0EC AXI Monitor Write Bandwidth Register */
} C0_CPUX_CFG_TypeDef; /* size of structure = 0x0F0 */
/*
 * @brief CAN
 */
/*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
typedef struct CAN_Type
{
    volatile uint32_t CAN_MSEL;                       /*!< Offset 0x000 CAN mode select register */
    volatile uint32_t CAN_CMD;                        /*!< Offset 0x004 CAN command register */
    volatile uint32_t CAN_STA;                        /*!< Offset 0x008 CAN status register */
    volatile uint32_t CAN_INT;                        /*!< Offset 0x00C CAN interrupt register */
    volatile uint32_t CAN_INTEN;                      /*!< Offset 0x010 CAN interrupt enable register */
    volatile uint32_t CAN_BUSTIME;                    /*!< Offset 0x014 CAN bus timing register */
    volatile uint32_t CAN_TEWL;                       /*!< Offset 0x018 CAN TX error warning limit register */
    volatile uint32_t CAN_ERRC;                       /*!< Offset 0x01C CAN error counter register */
    volatile uint32_t CAN_RMCNT;                      /*!< Offset 0x020 CAN receive message counter register */
    volatile uint32_t CAN_RBUF_SADDR;                 /*!< Offset 0x024 CAN receive buffer start address register */
    volatile uint32_t CAN_ACPC;                       /*!< Offset 0x028 CAN acceptance code 0 register(reset mode) */
    volatile uint32_t CAN_ACPM;                       /*!< Offset 0x02C CAN acceptance mask 0 register(reset mode) */
             uint32_t reserved_0x030 [0x0004];
    volatile uint32_t CAN_TRBUF [0x00D];              /*!< Offset 0x040 CAN TX/RX message buffer N (n=0..12) register */
             uint32_t reserved_0x074 [0x0043];
    volatile uint32_t CAN_RBUF_RBACK [0x030];         /*!< Offset 0x180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
             uint32_t reserved_0x240 [0x0030];
    volatile uint32_t CAN_VERSION;                    /*!< Offset 0x300 CAN Version Register */
             uint32_t reserved_0x304 [0x003F];
} CAN_TypeDef; /* size of structure = 0x400 */
/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPU_CTRL_REG;               /*!< Offset 0x000 PLL_CPU Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t PLL_DDR_CTRL_REG;               /*!< Offset 0x010 PLL_DDR Control Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t PLL_PERI_CTRL_REG;              /*!< Offset 0x020 PLL_PERI Control Register */
             uint32_t reserved_0x024 [0x0007];
    volatile uint32_t PLL_VIDEO0_CTRL_REG;            /*!< Offset 0x040 PLL_VIDEO0 Control Register */
             uint32_t reserved_0x044;
    volatile uint32_t PLL_VIDEO1_CTRL_REG;            /*!< Offset 0x048 PLL_VIDEO1 Control Register */
             uint32_t reserved_0x04C [0x0003];
    volatile uint32_t PLL_VE_CTRL_REG;                /*!< Offset 0x058 PLL_VE Control Register */
             uint32_t reserved_0x05C [0x0007];
    volatile uint32_t PLL_AUDIO0_CTRL_REG;            /*!< Offset 0x078 PLL_AUDIO0 Control Register */
             uint32_t reserved_0x07C;
    volatile uint32_t PLL_AUDIO1_CTRL_REG;            /*!< Offset 0x080 PLL_AUDIO1 Control Register */
             uint32_t reserved_0x084 [0x0023];
    volatile uint32_t PLL_DDR_PAT0_CTRL_REG;          /*!< Offset 0x110 PLL_DDR Pattern0 Control Register */
    volatile uint32_t PLL_DDR_PAT1_CTRL_REG;          /*!< Offset 0x114 PLL_DDR Pattern1 Control Register */
             uint32_t reserved_0x118 [0x0002];
    volatile uint32_t PLL_PERI_PAT0_CTRL_REG;         /*!< Offset 0x120 PLL_PERI Pattern0 Control Register */
    volatile uint32_t PLL_PERI_PAT1_CTRL_REG;         /*!< Offset 0x124 PLL_PERI Pattern1 Control Register */
             uint32_t reserved_0x128 [0x0006];
    volatile uint32_t PLL_VIDEO0_PAT0_CTRL_REG;       /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO0_PAT1_CTRL_REG;       /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT0_CTRL_REG;       /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT1_CTRL_REG;       /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
             uint32_t reserved_0x150 [0x0002];
    volatile uint32_t PLL_VE_PAT0_CTRL_REG;           /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
    volatile uint32_t PLL_VE_PAT1_CTRL_REG;           /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
             uint32_t reserved_0x160 [0x0006];
    volatile uint32_t PLL_AUDIO0_PAT0_CTRL_REG;       /*!< Offset 0x178 PLL_AUDIO0 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO0_PAT1_CTRL_REG;       /*!< Offset 0x17C PLL_AUDIO0 Pattern1 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT0_CTRL_REG;       /*!< Offset 0x180 PLL_AUDIO1 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT1_CTRL_REG;       /*!< Offset 0x184 PLL_AUDIO1 Pattern1 Control Register */
             uint32_t reserved_0x188 [0x005E];
    volatile uint32_t PLL_CPU_BIAS_REG;               /*!< Offset 0x300 PLL_CPU Bias Register */
             uint32_t reserved_0x304 [0x0003];
    volatile uint32_t PLL_DDR_BIAS_REG;               /*!< Offset 0x310 PLL_DDR Bias Register */
             uint32_t reserved_0x314 [0x0003];
    volatile uint32_t PLL_PERI_BIAS_REG;              /*!< Offset 0x320 PLL_PERI Bias Register */
             uint32_t reserved_0x324 [0x0007];
    volatile uint32_t PLL_VIDEO0_BIAS_REG;            /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
             uint32_t reserved_0x344;
    volatile uint32_t PLL_VIDEO1_BIAS_REG;            /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
             uint32_t reserved_0x34C [0x0003];
    volatile uint32_t PLL_VE_BIAS_REG;                /*!< Offset 0x358 PLL_VE Bias Register */
             uint32_t reserved_0x35C [0x0007];
    volatile uint32_t PLL_AUDIO0_BIAS_REG;            /*!< Offset 0x378 PLL_AUDIO0 Bias Register */
             uint32_t reserved_0x37C;
    volatile uint32_t PLL_AUDIO1_BIAS_REG;            /*!< Offset 0x380 PLL_AUDIO1 Bias Register */
             uint32_t reserved_0x384 [0x001F];
    volatile uint32_t PLL_CPU_TUN_REG;                /*!< Offset 0x400 PLL_CPU Tuning Register */
             uint32_t reserved_0x404 [0x003F];
    volatile uint32_t CPU_AXI_CFG_REG;                /*!< Offset 0x500 CPU_AXI Configuration Register */
    volatile uint32_t CPU_GATING_REG;                 /*!< Offset 0x504 CPU_GATING Configuration Register */
             uint32_t reserved_0x508 [0x0002];
    volatile uint32_t PSI_CLK_REG;                    /*!< Offset 0x510 PSI Clock Register */
             uint32_t reserved_0x514 [0x0003];
    volatile uint32_t APB0_CLK_REG;                   /*!< Offset 0x520 APB0 Clock Register */
    volatile uint32_t APB1_CLK_REG;                   /*!< Offset 0x524 APB1 Clock Register */
             uint32_t reserved_0x528 [0x0006];
    volatile uint32_t MBUS_CLK_REG;                   /*!< Offset 0x540 MBUS Clock Register */
             uint32_t reserved_0x544 [0x002F];
    volatile uint32_t DE_CLK_REG;                     /*!< Offset 0x600 DE Clock Register */
             uint32_t reserved_0x604 [0x0002];
    volatile uint32_t DE_BGR_REG;                     /*!< Offset 0x60C DE Bus Gating Reset Register */
             uint32_t reserved_0x610 [0x0004];
    volatile uint32_t DI_CLK_REG;                     /*!< Offset 0x620 DI Clock Register */
             uint32_t reserved_0x624 [0x0002];
    volatile uint32_t DI_BGR_REG;                     /*!< Offset 0x62C DI Bus Gating Reset Register */
    volatile uint32_t G2D_CLK_REG;                    /*!< Offset 0x630 G2D Clock Register */
             uint32_t reserved_0x634 [0x0002];
    volatile uint32_t G2D_BGR_REG;                    /*!< Offset 0x63C G2D Bus Gating Reset Register */
             uint32_t reserved_0x640 [0x0010];
    volatile uint32_t CE_CLK_REG;                     /*!< Offset 0x680 CE Clock Register */
             uint32_t reserved_0x684 [0x0002];
    volatile uint32_t CE_BGR_REG;                     /*!< Offset 0x68C CE Bus Gating Reset Register */
    volatile uint32_t VE_CLK_REG;                     /*!< Offset 0x690 VE Clock Register */
             uint32_t reserved_0x694 [0x0002];
    volatile uint32_t VE_BGR_REG;                     /*!< Offset 0x69C VE Bus Gating Reset Register */
             uint32_t reserved_0x6A0 [0x001B];
    volatile uint32_t DMA_BGR_REG;                    /*!< Offset 0x70C DMA Bus Gating Reset Register */
             uint32_t reserved_0x710 [0x0003];
    volatile uint32_t MSGBOX_BGR_REG;                 /*!< Offset 0x71C MSGBOX Bus Gating Reset Register */
             uint32_t reserved_0x720 [0x0003];
    volatile uint32_t SPINLOCK_BGR_REG;               /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register */
             uint32_t reserved_0x730 [0x0003];
    volatile uint32_t HSTIMER_BGR_REG;                /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
    volatile uint32_t AVS_CLK_REG;                    /*!< Offset 0x740 AVS Clock Register */
             uint32_t reserved_0x744 [0x0012];
    volatile uint32_t DBGSYS_BGR_REG;                 /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
             uint32_t reserved_0x790 [0x0007];
    volatile uint32_t PWM_BGR_REG;                    /*!< Offset 0x7AC PWM Bus Gating Reset Register */
             uint32_t reserved_0x7B0 [0x0003];
    volatile uint32_t IOMMU_BGR_REG;                  /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
             uint32_t reserved_0x7C0 [0x0010];
    volatile uint32_t DRAM_CLK_REG;                   /*!< Offset 0x800 DRAM Clock Register */
    volatile uint32_t MBUS_MAT_CLK_GATING_REG;        /*!< Offset 0x804 MBUS Master Clock Gating Register */
             uint32_t reserved_0x808;
    volatile uint32_t DRAM_BGR_REG;                   /*!< Offset 0x80C DRAM Bus Gating Reset Register */
             uint32_t reserved_0x810 [0x0008];
    volatile uint32_t SMHC0_CLK_REG;                  /*!< Offset 0x830 SMHC0 Clock Register */
    volatile uint32_t SMHC1_CLK_REG;                  /*!< Offset 0x834 SMHC1 Clock Register */
    volatile uint32_t SMHC2_CLK_REG;                  /*!< Offset 0x838 SMHC2 Clock Register */
             uint32_t reserved_0x83C [0x0004];
    volatile uint32_t SMHC_BGR_REG;                   /*!< Offset 0x84C SMHC Bus Gating Reset Register */
             uint32_t reserved_0x850 [0x002F];
    volatile uint32_t UART_BGR_REG;                   /*!< Offset 0x90C UART Bus Gating Reset Register */
             uint32_t reserved_0x910 [0x0003];
    volatile uint32_t TWI_BGR_REG;                    /*!< Offset 0x91C TWI Bus Gating Reset Register */
             uint32_t reserved_0x920 [0x0003];
    volatile uint32_t CAN_BGR_REG;                    /*!< Offset 0x92C CAN Bus Gating Reset Register */
             uint32_t reserved_0x930 [0x0004];
    volatile uint32_t SPI0_CLK_REG;                   /*!< Offset 0x940 SPI0 Clock Register */
    volatile uint32_t SPI1_CLK_REG;                   /*!< Offset 0x944 SPI1 Clock Register */
             uint32_t reserved_0x948 [0x0009];
    volatile uint32_t SPI_BGR_REG;                    /*!< Offset 0x96C SPI Bus Gating Reset Register */
    volatile uint32_t EMAC_25M_CLK_REG;               /*!< Offset 0x970 EMAC_25M Clock Register */
             uint32_t reserved_0x974 [0x0002];
    volatile uint32_t EMAC_BGR_REG;                   /*!< Offset 0x97C EMAC Bus Gating Reset Register */
             uint32_t reserved_0x980 [0x0010];
    volatile uint32_t IRTX_CLK_REG;                   /*!< Offset 0x9C0 IRTX Clock Register */
             uint32_t reserved_0x9C4 [0x0002];
    volatile uint32_t IRTX_BGR_REG;                   /*!< Offset 0x9CC IRTX Bus Gating Reset Register */
             uint32_t reserved_0x9D0 [0x0007];
    volatile uint32_t GPADC_BGR_REG;                  /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
             uint32_t reserved_0x9F0 [0x0003];
    volatile uint32_t THS_BGR_REG;                    /*!< Offset 0x9FC THS Bus Gating Reset Register */
             uint32_t reserved_0xA00 [0x0005];
    volatile uint32_t I2S1_CLK_REG;                   /*!< Offset 0xA14 I2S1 Clock Register */
    volatile uint32_t I2S2_CLK_REG;                   /*!< Offset 0xA18 I2S2 Clock Register */
    volatile uint32_t I2S2_ASRC_CLK_REG;              /*!< Offset 0xA1C I2S2_ASRC Clock Register */
    volatile uint32_t I2S_BGR_REG;                    /*!< Offset 0xA20 I2S Bus Gating Reset Register */
    volatile uint32_t OWA_TX_CLK_REG;                 /*!< Offset 0xA24 OWA_TX Clock Register */
             uint32_t reserved_0xA28;
    volatile uint32_t OWA_BGR_REG;                    /*!< Offset 0xA2C OWA Bus Gating Reset Register */
             uint32_t reserved_0xA30 [0x0004];
    volatile uint32_t DMIC_CLK_REG;                   /*!< Offset 0xA40 DMIC Clock Register */
             uint32_t reserved_0xA44 [0x0002];
    volatile uint32_t DMIC_BGR_REG;                   /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
    volatile uint32_t AUDIO_CODEC_DAC_CLK_REG;        /*!< Offset 0xA50 AUDIO_CODEC_DAC Clock Register */
    volatile uint32_t AUDIO_CODEC_ADC_CLK_REG;        /*!< Offset 0xA54 AUDIO_CODEC_ADC Clock Register */
             uint32_t reserved_0xA58;
    volatile uint32_t AUDIO_CODEC_BGR_REG;            /*!< Offset 0xA5C AUDIO_CODEC Bus Gating Reset Register */
             uint32_t reserved_0xA60 [0x0004];
    volatile uint32_t USB0_CLK_REG;                   /*!< Offset 0xA70 USB0 Clock Register */
    volatile uint32_t USB1_CLK_REG;                   /*!< Offset 0xA74 USB1 Clock Register */
             uint32_t reserved_0xA78 [0x0005];
    volatile uint32_t USB_BGR_REG;                    /*!< Offset 0xA8C USB Bus Gating Reset Register */
             uint32_t reserved_0xA90 [0x000B];
    volatile uint32_t DPSS_TOP_BGR_REG;               /*!< Offset 0xABC DPSS_TOP Bus Gating Reset Register */
             uint32_t reserved_0xAC0 [0x0019];
    volatile uint32_t DSI_CLK_REG;                    /*!< Offset 0xB24 DSI Clock Register */
             uint32_t reserved_0xB28 [0x0009];
    volatile uint32_t DSI_BGR_REG;                    /*!< Offset 0xB4C DSI Bus Gating Reset Register */
             uint32_t reserved_0xB50 [0x0004];
    volatile uint32_t TCONLCD_CLK_REG;                /*!< Offset 0xB60 TCONLCD Clock Register */
             uint32_t reserved_0xB64 [0x0006];
    volatile uint32_t TCONLCD_BGR_REG;                /*!< Offset 0xB7C TCONLCD Bus Gating Reset Register */
    volatile uint32_t TCONTV_CLK_REG;                 /*!< Offset 0xB80 TCONTV Clock Register */
             uint32_t reserved_0xB84 [0x0006];
    volatile uint32_t TCONTV_BGR_REG;                 /*!< Offset 0xB9C TCONTV Bus Gating Reset Register */
             uint32_t reserved_0xBA0 [0x0003];
    volatile uint32_t LVDS_BGR_REG;                   /*!< Offset 0xBAC LVDS Bus Gating Reset Register */
    volatile uint32_t TVE_CLK_REG;                    /*!< Offset 0xBB0 TVE Clock Register */
             uint32_t reserved_0xBB4 [0x0002];
    volatile uint32_t TVE_BGR_REG;                    /*!< Offset 0xBBC TVE Bus Gating Reset Register */
    volatile uint32_t TVD_CLK_REG;                    /*!< Offset 0xBC0 TVD Clock Register */
             uint32_t reserved_0xBC4 [0x0006];
    volatile uint32_t TVD_BGR_REG;                    /*!< Offset 0xBDC TVD Bus Gating Reset Register */
             uint32_t reserved_0xBE0 [0x0004];
    volatile uint32_t LEDC_CLK_REG;                   /*!< Offset 0xBF0 LEDC Clock Register */
             uint32_t reserved_0xBF4 [0x0002];
    volatile uint32_t LEDC_BGR_REG;                   /*!< Offset 0xBFC LEDC Bus Gating Reset Register */
             uint32_t reserved_0xC00;
    volatile uint32_t CSI_CLK_REG;                    /*!< Offset 0xC04 CSI Clock Register */
    volatile uint32_t CSI_MASTER_CLK_REG;             /*!< Offset 0xC08 CSI Master Clock Register */
             uint32_t reserved_0xC0C [0x0004];
    volatile uint32_t CSI_BGR_REG;                    /*!< Offset 0xC1C CSI Bus Gating Reset Register */
             uint32_t reserved_0xC20 [0x000C];
    volatile uint32_t TPADC_CLK_REG;                  /*!< Offset 0xC50 TPADC Clock Register */
             uint32_t reserved_0xC54 [0x0002];
    volatile uint32_t TPADC_BGR_REG;                  /*!< Offset 0xC5C TPADC Bus Gating Reset Register */
             uint32_t reserved_0xC60 [0x0004];
    volatile uint32_t DSP_CLK_REG;                    /*!< Offset 0xC70 DSP Clock Register */
             uint32_t reserved_0xC74 [0x0002];
    volatile uint32_t DSP_BGR_REG;                    /*!< Offset 0xC7C DSP Bus Gating Reset Register */
             uint32_t reserved_0xC80 [0x0020];
    volatile uint32_t RISC_CLK_REG;                   /*!< Offset 0xD00 RISC Clock Register */
    volatile uint32_t RISC_GATING_REG;                /*!< Offset 0xD04 RISC Gating Configuration Register */
             uint32_t reserved_0xD08;
    volatile uint32_t RISC_CFG_BGR_REG;               /*!< Offset 0xD0C RISC_CFG Bus Gating Reset Register */
             uint32_t reserved_0xD10 [0x007D];
    volatile uint32_t PLL_LOCK_DBG_CTRL_REG;          /*!< Offset 0xF04 PLL Lock Debug Control Register */
    volatile uint32_t FRE_DET_CTRL_REG;               /*!< Offset 0xF08 Frequency Detect Control Register */
    volatile uint32_t FRE_UP_LIM_REG;                 /*!< Offset 0xF0C Frequency Up Limit Register */
    volatile uint32_t FRE_DOWN_LIM_REG;               /*!< Offset 0xF10 Frequency Down Limit Register */
             uint32_t reserved_0xF14 [0x0003];
    volatile uint32_t RISC_RST_REG;                   /*!< Offset 0xF20 RISC LOCK RESET Register */
             uint32_t reserved_0xF24 [0x0003];
    volatile uint32_t CCU_FAN_GATE_REG;               /*!< Offset 0xF30 CCU FANOUT CLOCK GATE Register */
    volatile uint32_t CLK27M_FAN_REG;                 /*!< Offset 0xF34 CLK27M FANOUT Register */
    volatile uint32_t PCLK_FAN_REG;                   /*!< Offset 0xF38 PCLK FANOUT Register */
    volatile uint32_t CCU_FAN_REG;                    /*!< Offset 0xF3C CCU FANOUT Register */
             uint32_t reserved_0xF40 [0x0030];
} CCU_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief CE
 */
/*!< CE Crypto Engine (CE) */
typedef struct CE_Type
{
    volatile uint32_t CE_TDA;                         /*!< Offset 0x000 Task Descriptor Address */
             uint32_t reserved_0x004;
    volatile uint32_t CE_ICR;                         /*!< Offset 0x008 Interrupt Control Register */
    volatile uint32_t CE_ISR;                         /*!< Offset 0x00C Interrupt Status Register */
    volatile uint32_t CE_TLR;                         /*!< Offset 0x010 Task Load Register */
    volatile uint32_t CE_TSR;                         /*!< Offset 0x014 Task Status Register */
    volatile uint32_t CE_ESR;                         /*!< Offset 0x018 Error Status Register */
             uint32_t reserved_0x01C [0x0002];
    volatile uint32_t CE_CSA;                         /*!< Offset 0x024 DMA Current Source Address */
    volatile uint32_t CE_CDA;                         /*!< Offset 0x028 DMA Current Destination Address */
    volatile uint32_t CE_TPR;                         /*!< Offset 0x02C Throughput Register */
} CE_TypeDef; /* size of structure = 0x030 */
/*
 * @brief CIR_RX
 */
/*!< CIR_RX  */
typedef struct CIR_RX_Type
{
    volatile uint32_t CIR_CTL;                        /*!< Offset 0x000 CIR Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t CIR_RXPCFG;                     /*!< Offset 0x010 CIR Receiver Pulse Configure Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t CIR_RXFIFO;                     /*!< Offset 0x020 CIR Receiver FIFO Register */
             uint32_t reserved_0x024 [0x0002];
    volatile uint32_t CIR_RXINT;                      /*!< Offset 0x02C CIR Receiver Interrupt Control Register */
    volatile uint32_t CIR_RXSTA;                      /*!< Offset 0x030 CIR Receiver Status Register */
    volatile uint32_t CIR_RXCFG;                      /*!< Offset 0x034 CIR Receiver Configure Register */
} CIR_RX_TypeDef; /* size of structure = 0x038 */
/*
 * @brief CIR_TX
 */
/*!< CIR_TX  */
typedef struct CIR_TX_Type
{
    volatile uint32_t CIR_TGLR;                       /*!< Offset 0x000 CIR Transmit Global Register */
    volatile uint32_t CIR_TMCR;                       /*!< Offset 0x004 CIR Transmit Modulation Control Register */
    volatile uint32_t CIR_TCR;                        /*!< Offset 0x008 CIR Transmit Control Register */
    volatile uint32_t CIR_IDC_H;                      /*!< Offset 0x00C CIR Transmit Idle Duration Threshold High Bit Register */
    volatile uint32_t CIR_IDC_L;                      /*!< Offset 0x010 CIR Transmit Idle Duration Threshold Low Bit Register */
    volatile uint32_t CIR_TICR_H;                     /*!< Offset 0x014 CIR Transmit Idle Counter High Bit Register */
    volatile uint32_t CIR_TICR_L;                     /*!< Offset 0x018 CIR Transmit Idle Counter Low Bit Register */
             uint32_t reserved_0x01C;
    volatile uint32_t CIR_TEL;                        /*!< Offset 0x020 CIR TX FIFO Empty Level Register */
    volatile uint32_t CIR_TXINT;                      /*!< Offset 0x024 CIR Transmit Interrupt Control Register */
    volatile uint32_t CIR_TAC;                        /*!< Offset 0x028 CIR Transmit FIFO Available Counter Register */
    volatile uint32_t CIR_TXSTA;                      /*!< Offset 0x02C CIR Transmit Status Register */
    volatile uint32_t CIR_TXT;                        /*!< Offset 0x030 CIR Transmit Threshold Register */
    volatile uint32_t CIR_DMA;                        /*!< Offset 0x034 CIR DMA Control Register */
             uint32_t reserved_0x038 [0x0012];
    volatile uint32_t CIR_TXFIFO;                     /*!< Offset 0x080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef; /* size of structure = 0x084 */
/*
 * @brief CLINT
 */
/*!< CLINT  */
typedef struct CLINT_Type
{
    volatile uint32_t msip;                           /*!< Offset 0x000 This register generates machine mode software interrupts when set. */
             uint32_t reserved_0x004 [0x0FFF];
    volatile uint32_t mtimecmplo;                     /*!< Offset 0x4000 This register holds the compare value for the timer. */
    volatile uint32_t mtimecmphi;                     /*!< Offset 0x4004 This register holds the compare value for the timer. */
             uint32_t reserved_0x4008 [0x1FFC];
    volatile uint32_t mtimelo;                        /*!< Offset 0xBFF8 Provides the current timer value. */
    volatile uint32_t mtimehi;                        /*!< Offset 0xBFFC Provides the current timer value. */
} CLINT_TypeDef; /* size of structure = 0xC000 */
/*
 * @brief CSIC_CCU
 */
/*!< CSIC_CCU  */
typedef struct CSIC_CCU_Type
{
    volatile uint32_t CCU_CLK_MODE_REG;               /*!< Offset 0x000 CCU Clock Mode Register */
    volatile uint32_t CCU_PARSER_CLK_EN_REG;          /*!< Offset 0x004 CCU Parser Clock Enable Register */
             uint32_t reserved_0x008;
    volatile uint32_t CCU_POST0_CLK_EN_REG;           /*!< Offset 0x00C CCU Post0 Clock Enable Register */
} CSIC_CCU_TypeDef; /* size of structure = 0x010 */
/*
 * @brief CSIC_DMA
 */
/*!< CSIC_DMA  */
typedef struct CSIC_DMA_Type
{
    volatile uint32_t CSIC_DMA_EN_REG;                /*!< Offset 0x000 CSIC DMA Enable Register */
    volatile uint32_t CSIC_DMA_CFG_REG;               /*!< Offset 0x004 CSIC DMA Configuration Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t CSIC_DMA_HSIZE_REG;             /*!< Offset 0x010 CSIC DMA Horizontal Size Register */
    volatile uint32_t CSIC_DMA_VSIZE_REG;             /*!< Offset 0x014 CSIC DMA Vertical Size Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t CSIC_DMA_F0_BUFA_REG;           /*!< Offset 0x020 CSIC DMA FIFO 0 Output Buffer-A Address Register */
    volatile uint32_t CSIC_DMA_F0_BUFA_RESULT_REG;    /*!< Offset 0x024 CSIC DMA FIFO 0 Output Buffer-A Address Result Register */
    volatile uint32_t CSIC_DMA_F1_BUFA_REG;           /*!< Offset 0x028 CSIC DMA FIFO 1 Output Buffer-A Address Register */
    volatile uint32_t CSIC_DMA_F1_BUFA_RESULT_REG;    /*!< Offset 0x02C CSIC DMA FIFO 1 Output Buffer-A Address Result Register */
    volatile uint32_t CSIC_DMA_F2_BUFA_REG;           /*!< Offset 0x030 CSIC DMA FIFO 2 Output Buffer-A Address Register */
    volatile uint32_t CSIC_DMA_F2_BUFA_RESULT_REG;    /*!< Offset 0x034 CSIC DMA FIFO 2 Output Buffer-A Address Result Register */
    volatile uint32_t CSIC_DMA_BUF_LEN_REG;           /*!< Offset 0x038 CSIC DMA Buffer Length Register */
    volatile uint32_t CSIC_DMA_FLIP_SIZE_REG;         /*!< Offset 0x03C CSIC DMA Flip Size Register */
    volatile uint32_t CSIC_DMA_VI_TO_TH0_REG;         /*!< Offset 0x040 CSIC DMA Video Input Timeout Threshold0 Register */
    volatile uint32_t CSIC_DMA_VI_TO_TH1_REG;         /*!< Offset 0x044 CSIC DMA Video Input Timeout Threshold1 Register */
    volatile uint32_t CSIC_DMA_VI_TO_CNT_VAL_REG;     /*!< Offset 0x048 CSIC DMA Video Input Timeout Counter Value Register */
    volatile uint32_t CSIC_DMA_CAP_STA_REG;           /*!< Offset 0x04C CSIC DMA Capture Status Register */
    volatile uint32_t CSIC_DMA_INT_EN_REG;            /*!< Offset 0x050 CSIC DMA Interrupt Enable Register */
    volatile uint32_t CSIC_DMA_INT_STA_REG;           /*!< Offset 0x054 CSIC DMA Interrupt Status Register */
    volatile uint32_t CSIC_DMA_LINE_CNT_REG;          /*!< Offset 0x058 CSIC DMA LINE Counter Register */
    volatile uint32_t CSIC_DMA_FRM_CNT_REG;           /*!< Offset 0x05C CSIC DMA Frame Counter Register */
    volatile uint32_t CSIC_DMA_FRM_CLK_CNT_REG;       /*!< Offset 0x060 CSIC DMA Frame Clock Counter Register */
    volatile uint32_t CSIC_DMA_ACC_ITNL_CLK_CNT_REG;  /*!< Offset 0x064 CSIC DMA Accumulated And Internal Clock Counter Register */
    volatile uint32_t CSIC_DMA_FIFO_STAT_REG;         /*!< Offset 0x068 CSIC DMA FIFO Statistic Register */
    volatile uint32_t CSIC_DMA_FIFO_THRS_REG;         /*!< Offset 0x06C CSIC DMA FIFO Threshold Register */
    volatile uint32_t CSIC_DMA_PCLK_STAT_REG;         /*!< Offset 0x070 CSIC DMA PCLK Statistic Register */
             uint32_t reserved_0x074 [0x0003];
    volatile uint32_t CSIC_DMA_BUF_ADDR_FIFO0_ENTRY_REG;/*!< Offset 0x080 CSIC DMA BUF Address FIFO0 Entry Register */
    volatile uint32_t CSIC_DMA_BUF_ADDR_FIFO1_ENTRY_REG;/*!< Offset 0x084 CSIC DMA BUF Address FIFO1 Entry Register */
    volatile uint32_t CSIC_DMA_BUF_ADDR_FIFO2_ENTRY_REG;/*!< Offset 0x088 CSIC DMA BUF Address FIFO2 Entry Register */
    volatile uint32_t CSIC_DMA_BUF_TH_REG;            /*!< Offset 0x08C CSIC DMA BUF Threshold Register */
    volatile uint32_t CSIC_DMA_BUF_ADDR_FIFO_CON_REG; /*!< Offset 0x090 CSIC DMA BUF Address FIFO Content Register */
    volatile uint32_t CSIC_DMA_STORED_FRM_CNT_REG;    /*!< Offset 0x094 CSIC DMA Stored Frame Counter Register */
             uint32_t reserved_0x098 [0x0057];
    volatile uint32_t CSIC_FEATURE_REG;               /*!< Offset 0x1F4 CSIC DMA Feature List Register */
} CSIC_DMA_TypeDef; /* size of structure = 0x1F8 */
/*
 * @brief CSIC_PARSER
 */
/*!< CSIC_PARSER  */
typedef struct CSIC_PARSER_Type
{
    volatile uint32_t PRS_EN_REG;                     /*!< Offset 0x000 Parser Enable Register */
    volatile uint32_t PRS_NCSIC_IF_CFG_REG;           /*!< Offset 0x004 Parser NCSIC Interface Configuration Register */
             uint32_t reserved_0x008;
    volatile uint32_t PRS_CAP_REG;                    /*!< Offset 0x00C Parser Capture Register */
    volatile uint32_t CSIC_PRS_SIGNAL_STA_REG;        /*!< Offset 0x010 CSIC Parser Signal Status Register */
    volatile uint32_t CSIC_PRS_NCSIC_BT656_HEAD_CFG_REG;/*!< Offset 0x014 CSIC Parser NCSIC BT656 Header Configuration Register */
             uint32_t reserved_0x018 [0x0003];
    volatile uint32_t PRS_C0_INFMT_REG;               /*!< Offset 0x024 Parser Channel_0 Input Format Register */
    volatile uint32_t PRS_C0_OUTPUT_HSIZE_REG;        /*!< Offset 0x028 Parser Channel_0 Output Horizontal Size Register */
    volatile uint32_t PRS_C0_OUTPUT_VSIZE_REG;        /*!< Offset 0x02C Parser Channel_0 Output Vertical Size Register */
    volatile uint32_t PRS_C0_INPUT_PARA0_REG;         /*!< Offset 0x030 Parser Channel_0 Input Parameter0 Register */
    volatile uint32_t PRS_C0_INPUT_PARA1_REG;         /*!< Offset 0x034 Parser Channel_0 Input Parameter1 Register */
    volatile uint32_t PRS_C0_INPUT_PARA2_REG;         /*!< Offset 0x038 Parser Channel_0 Input Parameter2 Register */
    volatile uint32_t PRS_C0_INPUT_PARA3_REG;         /*!< Offset 0x03C Parser Channel_0 Input Parameter3 Register */
    volatile uint32_t PRS_C0_INT_EN_REG;              /*!< Offset 0x040 Parser Channel_0 Interrupt Enable Register */
    volatile uint32_t PRS_C0_INT_STA_REG;             /*!< Offset 0x044 Parser Channel_0 Interrupt Status Register */
    volatile uint32_t PRS_CH0_LINE_TIME_REG;          /*!< Offset 0x048 Parser Channel_0 Line Time Register */
             uint32_t reserved_0x04C [0x0036];
    volatile uint32_t PRS_C1_INFMT_REG;               /*!< Offset 0x124 Parser Channel_1 Input Format Register */
    volatile uint32_t PRS_C1_OUTPUT_HSIZE_REG;        /*!< Offset 0x128 Parser Channel_1 Output Horizontal Size Register */
    volatile uint32_t PRS_C1_OUTPUT_VSIZE_REG;        /*!< Offset 0x12C Parser Channel_1 Output Vertical Size Register */
    volatile uint32_t PRS_C1_INPUT_PARA0_REG;         /*!< Offset 0x130 Parser Channel_1 Input Parameter0 Register */
    volatile uint32_t PRS_C1_INPUT_PARA1_REG;         /*!< Offset 0x134 Parser Channel_1 Input Parameter1 Register */
    volatile uint32_t PRS_C1_INPUT_PARA2_REG;         /*!< Offset 0x138 Parser Channel_1 Input Parameter2 Register */
    volatile uint32_t PRS_C1_INPUT_PARA3_REG;         /*!< Offset 0x13C Parser Channel_1 Input Parameter3 Register */
    volatile uint32_t PRS_C1_INT_EN_REG;              /*!< Offset 0x140 Parser Channel_1 Interrupt Enable Register */
    volatile uint32_t PRS_C1_INT_STA_REG;             /*!< Offset 0x144 Parser Channel_1 Interrupt Status Register */
    volatile uint32_t PRS_CH1_LINE_TIME_REG;          /*!< Offset 0x148 Parser Channel_1 Line Time Register */
             uint32_t reserved_0x14C [0x0036];
    volatile uint32_t PRS_C2_INFMT_REG;               /*!< Offset 0x224 Parser Channel_2 Input Format Register */
    volatile uint32_t PRS_C2_OUTPUT_HSIZE_REG;        /*!< Offset 0x228 Parser Channel_2 Output Horizontal Size Register */
    volatile uint32_t PRS_C2_OUTPUT_VSIZE_REG;        /*!< Offset 0x22C Parser Channel_2 Output Vertical Size Register */
    volatile uint32_t PRS_C2_INPUT_PARA0_REG;         /*!< Offset 0x230 Parser Channel_2 Input Parameter0 Register */
    volatile uint32_t PRS_C2_INPUT_PARA1_REG;         /*!< Offset 0x234 Parser Channel_2 Input Parameter1 Register */
    volatile uint32_t PRS_C2_INPUT_PARA2_REG;         /*!< Offset 0x238 Parser Channel_2 Input Parameter2 Register */
    volatile uint32_t PRS_C2_INPUT_PARA3_REG;         /*!< Offset 0x23C Parser Channel_2 Input Parameter3 Register */
    volatile uint32_t PRS_C2_INT_EN_REG;              /*!< Offset 0x240 Parser Channel_2 Interrupt Enable Register */
    volatile uint32_t PRS_C2_INT_STA_REG;             /*!< Offset 0x244 Parser Channel_2 Interrupt Status Register */
    volatile uint32_t PRS_CH2_LINE_TIME_REG;          /*!< Offset 0x248 Parser Channel_2 Line Time Register */
             uint32_t reserved_0x24C [0x0036];
    volatile uint32_t PRS_C3_INFMT_REG;               /*!< Offset 0x324 Parser Channel_3 Input Format Register */
    volatile uint32_t PRS_C3_OUTPUT_HSIZE_REG;        /*!< Offset 0x328 Parser Channel_3 Output Horizontal Size Register */
    volatile uint32_t PRS_C3_OUTPUT_VSIZE_REG;        /*!< Offset 0x32C Parser Channel_3 Output Vertical Size Register */
    volatile uint32_t PRS_C3_INPUT_PARA0_REG;         /*!< Offset 0x330 Parser Channel_3 Input Parameter0 Register */
    volatile uint32_t PRS_C3_INPUT_PARA1_REG;         /*!< Offset 0x334 Parser Channel_3 Input Parameter1 Register */
    volatile uint32_t PRS_C3_INPUT_PARA2_REG;         /*!< Offset 0x338 Parser Channel_3 Input Parameter2 Register */
    volatile uint32_t PRS_C3_INPUT_PARA3_REG;         /*!< Offset 0x33C Parser Channel_3 Input Parameter3 Register */
    volatile uint32_t PRS_C3_INT_EN_REG;              /*!< Offset 0x340 Parser Channel_3 Interrupt Enable Register */
    volatile uint32_t PRS_C3_INT_STA_REG;             /*!< Offset 0x344 Parser Channel_3 Interrupt Status Register */
    volatile uint32_t PRS_CH3_LINE_TIME_REG;          /*!< Offset 0x348 Parser Channel_3 Line Time Register */
             uint32_t reserved_0x34C [0x006D];
    volatile uint32_t CSIC_PRS_NCSIC_RX_SIGNAL0_DLY_ADJ_REG;/*!< Offset 0x500 CSIC Parser NCSIC RX Signal0 Delay Adjust Register */
             uint32_t reserved_0x504 [0x0004];
    volatile uint32_t CSIC_PRS_NCSIC_RX_SIGNAL5_DLY_ADJ_REG;/*!< Offset 0x514 CSIC Parser NCSIC RX Signal5 Delay Adjust Register */
    volatile uint32_t CSIC_PRS_NCSIC_RX_SIGNAL6_DLY_ADJ_REG;/*!< Offset 0x518 CSIC Parser NCSIC RX Signal6 Delay Adjust Register */
} CSIC_PARSER_TypeDef; /* size of structure = 0x51C */
/*
 * @brief CSIC_TOP
 */
/*!< CSIC_TOP  */
typedef struct CSIC_TOP_Type
{
    volatile uint32_t CSIC_TOP_EN_REG;                /*!< Offset 0x000 CSIC TOP Enable Register */
    volatile uint32_t CSIC_PTN_GEN_EN_REG;            /*!< Offset 0x004 CSIC Pattern Generation Enable Register */
    volatile uint32_t CSIC_PTN_CTRL_REG;              /*!< Offset 0x008 CSIC Pattern Control Register */
             uint32_t reserved_0x00C [0x0005];
    volatile uint32_t CSIC_PTN_LEN_REG;               /*!< Offset 0x020 CSIC Pattern Generation Length Register */
    volatile uint32_t CSIC_PTN_ADDR_REG;              /*!< Offset 0x024 CSIC Pattern Generation Address Register */
    volatile uint32_t CSIC_PTN_ISP_SIZE_REG;          /*!< Offset 0x028 CSIC Pattern ISP Size Register */
             uint32_t reserved_0x02C [0x001D];
    volatile uint32_t CSIC_DMA0_INPUT_SEL_REG;        /*!< Offset 0x0A0 CSIC DMA0 Input Select Register */
    volatile uint32_t CSIC_DMA1_INPUT_SEL_REG;        /*!< Offset 0x0A4 CSIC DMA1 Input Select Register */
             uint32_t reserved_0x0A8 [0x000D];
    volatile uint32_t CSIC_BIST_CS_REG;               /*!< Offset 0x0DC CSIC BIST CS Register */
    volatile uint32_t CSIC_BIST_CONTROL_REG;          /*!< Offset 0x0E0 CSIC BIST Control Register */
    volatile uint32_t CSIC_BIST_START_REG;            /*!< Offset 0x0E4 CSIC BIST Start Register */
    volatile uint32_t CSIC_BIST_END_REG;              /*!< Offset 0x0E8 CSIC BIST End Register */
    volatile uint32_t CSIC_BIST_DATA_MASK_REG;        /*!< Offset 0x0EC CSIC BIST Data Mask Register */
    volatile uint32_t CSIC_MBUS_REQ_MAX_REG;          /*!< Offset 0x0F0 CSIC MBUS REQ MAX Register */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t CSIC_MULF_MOD_REG;              /*!< Offset 0x100 CSIC Multi-Frame Mode Register */
    volatile uint32_t CSIC_MULF_INT_REG;              /*!< Offset 0x104 CSIC Multi-Frame Interrupt Register */
} CSIC_TOP_TypeDef; /* size of structure = 0x108 */
/*
 * @brief DDRPHYC
 */
/*!< DDRPHYC  */
typedef struct DDRPHYC_Type
{
    volatile uint32_t PHYC_REG_000;                   /*!< Offset 0x000 Reg_000 */
    volatile uint32_t PHYC_REG_004;                   /*!< Offset 0x004 Reg_004 */
             uint32_t reserved_0x008;
    volatile uint32_t PHYC_REG_00C;                   /*!< Offset 0x00C Reg_00C */
    volatile uint32_t PHYC_REG_010;                   /*!< Offset 0x010 Reg_010 */
             uint32_t reserved_0x014;
    volatile uint32_t PHYC_REG_018;                   /*!< Offset 0x018 Reg_018 */
             uint32_t reserved_0x01C [0x0004];
    volatile uint32_t PHYC_REG_02C;                   /*!< Offset 0x02C Reg_02C */
    volatile uint32_t PHYC_REG_030;                   /*!< Offset 0x030 Reg_030 */
    volatile uint32_t PHYC_REG_034;                   /*!< Offset 0x034 Reg_034 */
    volatile uint32_t PHYC_REG_038;                   /*!< Offset 0x038 Reg_038 */
    volatile uint32_t PHYC_REG_03C;                   /*!< Offset 0x03C Reg_03C */
             uint32_t reserved_0x040 [0x0004];
    volatile uint32_t PHYC_REG_050;                   /*!< Offset 0x050 Reg_050 */
    volatile uint32_t PHYC_REG_054;                   /*!< Offset 0x054 Reg_054 */
    volatile uint32_t PHYC_REG_058;                   /*!< Offset 0x058 Reg_058 */
    volatile uint32_t PHYC_REG_05C;                   /*!< Offset 0x05C Reg_05C */
    volatile uint32_t PHYC_REG_060;                   /*!< Offset 0x060 Reg_060 */
    volatile uint32_t PHYC_REG_064;                   /*!< Offset 0x064 Reg_064 */
    volatile uint32_t PHYC_REG_068;                   /*!< Offset 0x068 Reg_068 */
    volatile uint32_t PHYC_REG_06C;                   /*!< Offset 0x06C Reg_06C */
             uint32_t reserved_0x070 [0x0002];
    volatile uint32_t PHYC_REG_078;                   /*!< Offset 0x078 Reg_078 */
    volatile uint32_t PHYC_REG_07C;                   /*!< Offset 0x07C Reg_07c */
    volatile uint32_t PHYC_REG_080;                   /*!< Offset 0x080 Reg_080 */
             uint32_t reserved_0x084 [0x0002];
    volatile uint32_t PHYC_REG_08C;                   /*!< Offset 0x08C Reg_08C */
    volatile uint32_t PHYC_REG_090;                   /*!< Offset 0x090 Reg_090 */
    volatile uint32_t PHYC_REG_094;                   /*!< Offset 0x094 Reg_094 */
             uint32_t reserved_0x098;
    volatile uint32_t PHYC_REG_09C;                   /*!< Offset 0x09C Reg_09C */
    volatile uint32_t PHYC_REG_0A0;                   /*!< Offset 0x0A0 Reg_0a0 */
             uint32_t reserved_0x0A4 [0x0005];
    volatile uint32_t PHYC_REG_0B8;                   /*!< Offset 0x0B8 Reg_0B8 */
    volatile uint32_t PHYC_REG_0BC;                   /*!< Offset 0x0BC Reg_0BC */
    volatile uint32_t PHYC_REG_0C0;                   /*!< Offset 0x0C0 Reg_0C0 */
             uint32_t reserved_0x0C4 [0x000F];
    volatile uint32_t PHYC_REG_100;                   /*!< Offset 0x100 Reg_100 */
             uint32_t reserved_0x104;
    volatile uint32_t PHYC_REG_108;                   /*!< Offset 0x108 Reg_108 */
    volatile uint32_t PHYC_REG_10C;                   /*!< Offset 0x10C Reg_10C */
    volatile uint32_t PHYC_REG_110;                   /*!< Offset 0x110 Reg_110 */
    volatile uint32_t PHYC_REG_114;                   /*!< Offset 0x114 Reg_114 */
             uint32_t reserved_0x118;
    volatile uint32_t PHYC_REG_11C;                   /*!< Offset 0x11C Reg_11C */
    volatile uint32_t PHYC_REG_120;                   /*!< Offset 0x120 Reg_120 */
             uint32_t reserved_0x124 [0x0007];
    volatile uint32_t PHYC_REG_140;                   /*!< Offset 0x140 Reg_140 */
             uint32_t reserved_0x144 [0x0031];
    volatile uint32_t PHYC_REG_208;                   /*!< Offset 0x208 Reg_208 */
             uint32_t reserved_0x20C [0x0003];
    volatile uint32_t PHYC_REG_218;                   /*!< Offset 0x218 Reg_218 */
    volatile uint32_t PHYC_REG_21C;                   /*!< Offset 0x21C Reg_21C */
             uint32_t reserved_0x220 [0x0002];
    volatile uint32_t PHYC_REG_228 [0x006];           /*!< Offset 0x228 Reg_228 */
    volatile uint32_t PHYC_REG_240 [0x010];           /*!< Offset 0x240 Reg_240 */
    volatile uint32_t PHYC_REG_280;                   /*!< Offset 0x280 Reg_280 */
             uint32_t reserved_0x284 [0x0023];
    volatile uint32_t PHYC_REG_310 [0x009];           /*!< Offset 0x310 Reg_310 Rank 0 */
    volatile uint32_t PHYC_REG_334;                   /*!< Offset 0x334 Reg_334 Rank 0 */
    volatile uint32_t PHYC_REG_338;                   /*!< Offset 0x338 Reg_338 Rank 0 */
    volatile uint32_t PHYC_REG_33C;                   /*!< Offset 0x33C Reg_33C Rank 0 */
             uint32_t reserved_0x340;
    volatile uint32_t PHYC_REG_344;                   /*!< Offset 0x344 Reg_344 Rank 0 */
    volatile uint32_t PHYC_REG_348;                   /*!< Offset 0x348 Reg_348 Rank 0 */
             uint32_t reserved_0x34C [0x0011];
    volatile uint32_t PHYC_REG_390 [0x009];           /*!< Offset 0x390 Reg_390 */
    volatile uint32_t PHYC_REG_3B4;                   /*!< Offset 0x3B4 Reg_3B4 Rank 1 */
    volatile uint32_t PHYC_REG_3B8;                   /*!< Offset 0x3B8 Reg_3B8 Rank 1 */
    volatile uint32_t PHYC_REG_3BC;                   /*!< Offset 0x3BC Reg_3BC Rank 1 */
             uint32_t reserved_0x3C0;
    volatile uint32_t PHYC_REG_3C4;                   /*!< Offset 0x3C4 Reg_3C4 Rank 1 */
    volatile uint32_t PHYC_REG_3C8;                   /*!< Offset 0x3C8 Reg_3C8 Rank 1 */
} DDRPHYC_TypeDef; /* size of structure = 0x3CC */
/*
 * @brief DE_BLD
 */
/*!< DE_BLD Display Engine (DE) - Blender */
typedef struct DE_BLD_Type
{
    volatile uint32_t BLD_EN_COLOR_CTL;               /*!< Offset 0x000 BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
    struct
    {
        volatile uint32_t BLD_FILL_COLOR;             /*!< Offset 0x004 BLD fill color register */
        volatile uint32_t BLD_CH_ISIZE;               /*!< Offset 0x008 BLD input memory size register */
        volatile uint32_t BLD_CH_OFFSET;              /*!< Offset 0x00C BLD input memory offset register */
                 uint32_t reserved_0x00C;
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..3] - but six elements */
             uint32_t reserved_0x064 [0x0007];
    volatile uint32_t ROUTE;                          /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    volatile uint32_t PREMULTIPLY;                    /*!< Offset 0x084 BLD pre-multiply control register */
    volatile uint32_t BKCOLOR;                        /*!< Offset 0x088  */
    volatile uint32_t OUTPUT_SIZE;                    /*!< Offset 0x08C  */
    volatile uint32_t BLD_MODE [0x006];               /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t CK_CTL;                         /*!< Offset 0x0B0  */
    volatile uint32_t CK_CFG;                         /*!< Offset 0x0B4  */
             uint32_t reserved_0x0B8 [0x0002];
    volatile uint32_t CK_MAX [0x004];                 /*!< Offset 0x0C0  */
             uint32_t reserved_0x0D0 [0x0004];
    volatile uint32_t CK_MIN [0x004];                 /*!< Offset 0x0E0  */
             uint32_t reserved_0x0F0 [0x0003];
    volatile uint32_t OUT_CTL;                        /*!< Offset 0x0FC  */
} DE_BLD_TypeDef; /* size of structure = 0x100 */
/*
 * @brief DE_DEP
 */
/*!< DE_DEP DRC (dynamic range controller) */
typedef struct DE_DEP_Type
{
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x000  */
                 uint32_t reserved_0x004 [0x1FFF];
    } DRC [0x001];                                    /*!< Offset 0x000  */
} DE_DEP_TypeDef; /* size of structure = 0x8000 */
/*
 * @brief DE_GLB
 */
/*!< DE_GLB Display Engine (DE) - Global Control */
typedef struct DE_GLB_Type
{
    volatile uint32_t GLB_CTL;                        /*!< Offset 0x000 Global control register */
    volatile uint32_t GLB_STS;                        /*!< Offset 0x004 Global status register */
    volatile uint32_t GLB_DBUFFER;                    /*!< Offset 0x008 Global double buffer control register */
    volatile uint32_t GLB_SIZE;                       /*!< Offset 0x00C Global size register */
} DE_GLB_TypeDef; /* size of structure = 0x010 */
/*
 * @brief DE_TOP
 */
/*!< DE_TOP Display Engine (DE) TOP */
typedef struct DE_TOP_Type
{
    volatile uint32_t GATE_CFG;                       /*!< Offset 0x000 SCLK_GATE DE SCLK Gating Register */
    volatile uint32_t BUS_CFG;                        /*!< Offset 0x004 ? HCLK_GATE ? DE HCLK Gating Register */
    volatile uint32_t RST_CFG;                        /*!< Offset 0x008 AHB_RESET DE AHB Reset register */
    volatile uint32_t DIV_CFG;                        /*!< Offset 0x00C SCLK_DIV DE SCLK Division register */
    volatile uint32_t SEL_CFG;                        /*!< Offset 0x010 ? DE2TCON ? MUX register */
             uint32_t reserved_0x014 [0x0004];
    volatile uint32_t DE_IP_CFG;                      /*!< Offset 0x024 DE IP Configure Register */
} DE_TOP_TypeDef; /* size of structure = 0x028 */
/*
 * @brief DE_UI
 */
/*!< DE_UI Display Engine (DE) - UI surface */
typedef struct DE_UI_Type
{
    struct
    {
        volatile uint32_t ATTR;                       /*!< Offset 0x000  */
        volatile uint32_t SIZE;                       /*!< Offset 0x004  */
        volatile uint32_t COORD;                      /*!< Offset 0x008  */
        volatile uint32_t PITCH;                      /*!< Offset 0x00C  */
        volatile uint32_t TOP_LADDR;                  /*!< Offset 0x010  */
        volatile uint32_t BOT_LADDR;                  /*!< Offset 0x014  */
        volatile uint32_t FCOLOR;                     /*!< Offset 0x018  */
                 uint32_t reserved_0x01C;
    } CFG [0x004];                                    /*!< Offset 0x000  */
    volatile uint32_t TOP_HADDR;                      /*!< Offset 0x080  */
    volatile uint32_t BOT_HADDR;                      /*!< Offset 0x084  */
    volatile uint32_t OVL_SIZE;                       /*!< Offset 0x088  */
} DE_UI_TypeDef; /* size of structure = 0x08C */
/*
 * @brief DE_VEP
 */
/*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks */
typedef struct DE_VEP_Type
{
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x000  */
                 uint32_t reserved_0x004 [0x01FF];
    } FCE [0x001];                                    /*!< Offset 0x000  */
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x800  */
                 uint32_t reserved_0x004 [0x01FF];
    } PEAK [0x001];                                   /*!< Offset 0x800  */
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x1000  */
                 uint32_t reserved_0x004 [0x01FF];
    } LCTI [0x001];                                   /*!< Offset 0x1000  */
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x1800  */
                 uint32_t reserved_0x004 [0x01FF];
    } BLS [0x001];                                    /*!< Offset 0x1800  */
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x2000  */
                 uint32_t reserved_0x004 [0x01FF];
    } FCC [0x001];                                    /*!< Offset 0x2000  */
             uint32_t reserved_0x2800 [0x1200];
    struct
    {
        volatile uint32_t CFG;                        /*!< Offset 0x7000  */
                 uint32_t reserved_0x004 [0x03FF];
    } VEP_TOP [0x001];                                /*!< Offset 0x7000  */
} DE_VEP_TypeDef; /* size of structure = 0x8000 */
/*
 * @brief DE_VI
 */
/*!< DE_VI Display Engine (DE) - VI surface */
typedef struct DE_VI_Type
{
    struct
    {
        volatile uint32_t ATTR;                       /*!< Offset 0x000  */
        volatile uint32_t SIZE;                       /*!< Offset 0x004  */
        volatile uint32_t COORD;                      /*!< Offset 0x008  */
        volatile uint32_t PITCH [0x003];              /*!< Offset 0x00C ix=0: Y, ix=1: U/UV channel, ix=3: V channel  */
        volatile uint32_t TOP_LADDR [0x003];          /*!< Offset 0x018  */
        volatile uint32_t BOT_LADDR [0x003];          /*!< Offset 0x024  */
    } CFG [0x004];                                    /*!< Offset 0x000  */
    volatile uint32_t FCOLOR [0x004];                 /*!< Offset 0x0C0  */
    volatile uint32_t TOP_HADDR [0x003];              /*!< Offset 0x0D0  */
    volatile uint32_t BOT_HADDR [0x003];              /*!< Offset 0x0DC  */
    volatile uint32_t OVL_SIZE [0x002];               /*!< Offset 0x0E8 OVL_V overlay window size register */
    volatile uint32_t HORI [0x002];                   /*!< Offset 0x0F0 OVL_V horizontal down sample control register */
    volatile uint32_t VERT [0x002];                   /*!< Offset 0x0F8 OVL_V vertical down sample control register */
} DE_VI_TypeDef; /* size of structure = 0x100 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
    volatile uint32_t DMAC_IRQ_EN_REG0;               /*!< Offset 0x000 DMAC IRQ Enable Register 0 */
    volatile uint32_t DMAC_IRQ_EN_REG1;               /*!< Offset 0x004 DMAC IRQ Enable Register 1 */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t DMAC_IRQ_PEND_REG0;             /*!< Offset 0x010 DMAC IRQ Pending Register 0 */
    volatile uint32_t DMAC_IRQ_PEND_REG1;             /*!< Offset 0x014 DMAC IRQ Pending Register 1 */
             uint32_t reserved_0x018 [0x0004];
    volatile uint32_t DMAC_AUTO_GATE_REG;             /*!< Offset 0x028 DMAC Auto Gating Register */
             uint32_t reserved_0x02C;
    volatile uint32_t DMAC_STA_REG;                   /*!< Offset 0x030 DMAC Status Register */
             uint32_t reserved_0x034 [0x0033];
    struct
    {
        volatile uint32_t DMAC_EN_REGN;               /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 15) 0x0100 + N*0x0040 */
        volatile uint32_t DMAC_PAU_REGN;              /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 15) 0x0104 + N*0x0040 */
        volatile uint32_t DMAC_DESC_ADDR_REGN;        /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 15) 0x0108 + N*0x0040 */
        volatile uint32_t DMAC_CFG_REGN;              /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 15) 0x010C + N*0x0040 */
        volatile uint32_t DMAC_CUR_SRC_REGN;          /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 15) 0x0110 + N*0x0040 */
        volatile uint32_t DMAC_CUR_DEST_REGN;         /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 15) 0x0114 + N*0x0040 */
        volatile uint32_t DMAC_BCNT_LEFT_REGN;        /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 15) 0x0118 + N*0x0040 */
        volatile uint32_t DMAC_PARA_REGN;             /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 15) 0x011C + N*0x0040 */
                 uint32_t reserved_0x020 [0x0002];
        volatile uint32_t DMAC_MODE_REGN;             /*!< Offset 0x128 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
        volatile uint32_t DMAC_FDESC_ADDR_REGN;       /*!< Offset 0x12C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
        volatile uint32_t DMAC_PKG_NUM_REGN;          /*!< Offset 0x130 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
                 uint32_t reserved_0x034 [0x0003];
    } CH [0x010];                                     /*!< Offset 0x100 Channel [0..15] */
} DMAC_TypeDef; /* size of structure = 0x500 */
/*
 * @brief DMIC
 */
/*!< DMIC  */
typedef struct DMIC_Type
{
    volatile uint32_t DMIC_EN;                        /*!< Offset 0x000 DMIC Enable Control Register */
    volatile uint32_t DMIC_SR;                        /*!< Offset 0x004 DMIC Sample Rate Register */
    volatile uint32_t DMIC_CTR;                       /*!< Offset 0x008 DMIC Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t DMIC_DATA;                      /*!< Offset 0x010 DMIC Data Register */
    volatile uint32_t DMIC_INTC;                      /*!< Offset 0x014 MIC Interrupt Control Register */
    volatile uint32_t DMIC_INTS;                      /*!< Offset 0x018 DMIC Interrupt Status Register */
    volatile uint32_t DMIC_RXFIFO_CTR;                /*!< Offset 0x01C DMIC RXFIFO Control Register */
    volatile uint32_t DMIC_RXFIFO_STA;                /*!< Offset 0x020 DMIC RXFIFO Status Register */
    volatile uint32_t DMIC_CH_NUM;                    /*!< Offset 0x024 DMIC Channel Numbers Register */
    volatile uint32_t DMIC_CH_MAP;                    /*!< Offset 0x028 DMIC Channel Mapping Register */
    volatile uint32_t DMIC_CNT;                       /*!< Offset 0x02C DMIC Counter Register */
    volatile uint32_t DATA0_DATA1_VOL_CTR;            /*!< Offset 0x030 Data0 and Data1 Volume Control Register */
    volatile uint32_t DATA2_DATA3_VOL_CTR;            /*!< Offset 0x034 Data2 And Data3 Volume Control Register */
    volatile uint32_t HPF_EN_CTR;                     /*!< Offset 0x038 High Pass Filter Enable Control Register */
    volatile uint32_t HPF_COEF_REG;                   /*!< Offset 0x03C High Pass Filter Coefficient Register */
    volatile uint32_t HPF_GAIN_REG;                   /*!< Offset 0x040 High Pass Filter Gain Register */
} DMIC_TypeDef; /* size of structure = 0x044 */
/*
 * @brief DSI
 */
/*!< DSI MIPI DSI Display Interface */
typedef struct DSI_Type
{
    volatile uint32_t DSI_CTL;                        /*!< Offset 0x000 dsi_gctl 0x00 - 0x0c */
    volatile uint32_t DSI_GINT0;                      /*!< Offset 0x004 dsi_gint0  */
    volatile uint32_t DSI_GINT1;                      /*!< Offset 0x008 dsi_gint1  */
    volatile uint32_t DSI_BASIC_CTL;                  /*!< Offset 0x00C dsi_basic_ctl  */
    volatile uint32_t DSI_BASIC_CTL0;                 /*!< Offset 0x010 dsi_basic_ctl0 0x10 - 0x1c */
    volatile uint32_t DSI_BASIC_CTL1;                 /*!< Offset 0x014 dsi_basic_ctl1  */
    volatile uint32_t DSI_BASIC_SIZE0;                /*!< Offset 0x018 dsi_basic_size0  */
    volatile uint32_t DSI_BASIC_SIZE1;                /*!< Offset 0x01C dsi_basic_size1  */
    volatile uint32_t DSI_BASIC_INST0 [0x008];        /*!< Offset 0x020 dsi_inst_func[8] 0x20 - 0x3c */
    volatile uint32_t DSI_INST_LOOP_SEL;              /*!< Offset 0x040 dsi_inst_loop_sel 0x40 - 0x5c */
    volatile uint32_t DSI_INST_LOOP_NUM;              /*!< Offset 0x044 dsi_inst_loop_num  */
    volatile uint32_t DSI_INST_JUMP_SEL;              /*!< Offset 0x048 dsi_inst_jump_sel  */
    volatile uint32_t DSI_INST_JUMP_CFG [0x002];      /*!< Offset 0x04C dsi_inst_jump_cfg[2]  */
    volatile uint32_t DSI_INST_LOOP_NUM2;             /*!< Offset 0x054 dsi_inst_loop_num2  */
             uint32_t reserved_0x058 [0x0002];
    volatile uint32_t DSI_TRANS_START;                /*!< Offset 0x060 dsi_trans_start 0x60 - 0x6c */
             uint32_t reserved_0x064 [0x0005];
    volatile uint32_t DSI_TRANS_ZERO;                 /*!< Offset 0x078 dsi_trans_zero  */
    volatile uint32_t DSI_TCON_DRQ;                   /*!< Offset 0x07C dsi_tcon_drq  */
    volatile uint32_t DSI_PIXEL_CTL0;                 /*!< Offset 0x080 dsi_pixel_ctl0 0x80 - 0x8c */
    volatile uint32_t DSI_PIXEL_CTL1;                 /*!< Offset 0x084 dsi_pixel_ctl1  */
             uint32_t reserved_0x088 [0x0002];
    volatile uint32_t DSI_PIXEL_PH;                   /*!< Offset 0x090 dsi_pixel_ph 0x90 - 0x9c */
    volatile uint32_t DSI_PIXEL_PD;                   /*!< Offset 0x094 dsi_pixel_pd  */
    volatile uint32_t DSI_PIXEL_PF0;                  /*!< Offset 0x098 dsi_pixel_pf0  */
    volatile uint32_t DSI_PIXEL_PF1;                  /*!< Offset 0x09C dsi_pixel_pf1  */
             uint32_t reserved_0x0A0 [0x0004];
    volatile uint32_t DSI_SYNC_HSS;                   /*!< Offset 0x0B0 dsi_sync_hss 0xb0 - 0xbc */
    volatile uint32_t DSI_SYNC_HSE;                   /*!< Offset 0x0B4 dsi_sync_hse  */
    volatile uint32_t DSI_SYNC_VSS;                   /*!< Offset 0x0B8 dsi_sync_vss  */
    volatile uint32_t DSI_SYNC_VSE;                   /*!< Offset 0x0BC dsi_sync_vse  */
    volatile uint32_t DSI_BLK_HSA0;                   /*!< Offset 0x0C0 dsi_blk_hsa0 0xc0 - 0xcc */
    volatile uint32_t DSI_BLK_HSA1;                   /*!< Offset 0x0C4 dsi_blk_hsa1  */
    volatile uint32_t DSI_BLK_HBP0;                   /*!< Offset 0x0C8 dsi_blk_hbp0  */
    volatile uint32_t DSI_BLK_HBP1;                   /*!< Offset 0x0CC dsi_blk_hbp1  */
    volatile uint32_t DSI_BLK_HFP0;                   /*!< Offset 0x0D0 dsi_blk_hfp0 0xd0 - 0xdc */
    volatile uint32_t DSI_BLK_HFP1;                   /*!< Offset 0x0D4 dsi_blk_hfp1  */
             uint32_t reserved_0x0D8 [0x0002];
    volatile uint32_t DSI_BLK_HBLK0;                  /*!< Offset 0x0E0 dsi_blk_hblk0 0xe0 - 0xec */
    volatile uint32_t DSI_BLK_HBLK1;                  /*!< Offset 0x0E4 dsi_blk_hblk1  */
    volatile uint32_t DSI_BLK_VBLK0;                  /*!< Offset 0x0E8 dsi_blk_vblk0  */
    volatile uint32_t DSI_BLK_VBLK1;                  /*!< Offset 0x0EC dsi_blk_vblk1  */
    volatile uint32_t DSI_BURST_LINE;                 /*!< Offset 0x0F0 dsi_burst_line 0xf0 - 0x1fc */
    volatile uint32_t DSI_BURST_DRQ;                  /*!< Offset 0x0F4 dsi_burst_drq  */
             uint32_t reserved_0x0F8 [0x0042];
    volatile uint32_t DSI_CMD_CTL;                    /*!< Offset 0x200 dsi_cmd_ctl 0x200 - 0x23c */
             uint32_t reserved_0x204 [0x000F];
    volatile uint32_t DSI_CMD_RX [0x008];             /*!< Offset 0x240 dsi_cmd_rx[8] 0x240 - 0x2dc */
             uint32_t reserved_0x260 [0x0020];
    volatile uint32_t DSI_DEBUG_VIDEO0;               /*!< Offset 0x2E0 dsi_debug_video0 0x2e0 - 0x2ec */
    volatile uint32_t DSI_DEBUG_VIDEO1;               /*!< Offset 0x2E4 dsi_debug_video1  */
             uint32_t reserved_0x2E8 [0x0002];
    volatile uint32_t DSI_DEBUG_INST;                 /*!< Offset 0x2F0 dsi_debug_inst 0x2f0 - 0x2fc */
    volatile uint32_t DSI_DEBUG_FIFO;                 /*!< Offset 0x2F4 dsi_debug_fifo  */
    volatile uint32_t DSI_DEBUG_DATA;                 /*!< Offset 0x2F8 dsi_debug_data  */
             uint32_t reserved_0x2FC;
    volatile uint32_t DSI_CMD_TX [0x040];             /*!< Offset 0x300 dsi_cmd_tx[64] 0x300 - 0x3fc */
} DSI_TypeDef; /* size of structure = 0x400 */
/*
 * @brief DSI_DPHY
 */
/*!< DSI_DPHY MIPI DSI Physical Interface */
typedef struct DSI_DPHY_Type
{
    volatile uint32_t DPHY_GCTL;                      /*!< Offset 0x000  */
    volatile uint32_t DPHY_TX_CTL;                    /*!< Offset 0x004  */
    volatile uint32_t DPHY_RX_CTL;                    /*!< Offset 0x008  */
             uint32_t reserved_0x00C;
    volatile uint32_t DPHY_TX_TIME0;                  /*!< Offset 0x010  */
    volatile uint32_t DPHY_TX_TIME1;                  /*!< Offset 0x014  */
    volatile uint32_t DPHY_TX_TIME2;                  /*!< Offset 0x018  */
    volatile uint32_t DPHY_TX_TIME3;                  /*!< Offset 0x01C  */
    volatile uint32_t DPHY_TX_TIME4;                  /*!< Offset 0x020  */
             uint32_t reserved_0x024 [0x0003];
    volatile uint32_t DPHY_RX_TIME0;                  /*!< Offset 0x030  */
    volatile uint32_t DPHY_RX_TIME1;                  /*!< Offset 0x034  */
    volatile uint32_t DPHY_RX_TIME2;                  /*!< Offset 0x038  */
             uint32_t reserved_0x03C;
    volatile uint32_t DPHY_RX_TIME3;                  /*!< Offset 0x040  */
             uint32_t reserved_0x044 [0x0002];
    volatile uint32_t DPHY_ANA0;                      /*!< Offset 0x04C  */
    volatile uint32_t DPHY_ANA1;                      /*!< Offset 0x050  */
    volatile uint32_t DPHY_ANA2;                      /*!< Offset 0x054  */
    volatile uint32_t DPHY_ANA3;                      /*!< Offset 0x058  */
    volatile uint32_t DPHY_ANA4;                      /*!< Offset 0x05C  */
    volatile uint32_t DPHY_INT_EN0;                   /*!< Offset 0x060  */
    volatile uint32_t DPHY_INT_EN1;                   /*!< Offset 0x064  */
    volatile uint32_t DPHY_INT_EN2;                   /*!< Offset 0x068  */
             uint32_t reserved_0x06C;
    volatile uint32_t DPHY_INT_PD0;                   /*!< Offset 0x070  */
    volatile uint32_t DPHY_INT_PD1;                   /*!< Offset 0x074  */
    volatile uint32_t DPHY_INT_PD2;                   /*!< Offset 0x078  */
             uint32_t reserved_0x07C [0x0019];
    volatile uint32_t DPHY_DBG0;                      /*!< Offset 0x0E0  */
    volatile uint32_t DPHY_DBG1;                      /*!< Offset 0x0E4  */
    volatile uint32_t DPHY_DBG2;                      /*!< Offset 0x0E8  */
    volatile uint32_t DPHY_DBG3;                      /*!< Offset 0x0EC  */
    volatile uint32_t DPHY_DBG4;                      /*!< Offset 0x0F0  */
    volatile uint32_t DPHY_DBG5;                      /*!< Offset 0x0F4  */
    volatile uint32_t DPHY_TX_SKEW_REG0;              /*!< Offset 0x0F8  */
    volatile uint32_t DPHY_TX_SKEW_REG1;              /*!< Offset 0x0FC  */
    volatile uint32_t DPHY_TX_SKEW_REG2;              /*!< Offset 0x100  */
    volatile uint32_t DPHY_PLL_REG0;                  /*!< Offset 0x104  */
    volatile uint32_t DPHY_PLL_REG1;                  /*!< Offset 0x108  */
    volatile uint32_t DPHY_PLL_REG2;                  /*!< Offset 0x10C  */
    volatile uint32_t COMBO_PHY_REG0;                 /*!< Offset 0x110 The TCON LCD0 PHY0 is controlled by COMBO_PHY_REG (reg0x1110, reg0x1114) */
    volatile uint32_t COMBO_PHY_REG1;                 /*!< Offset 0x114 The TCON LCD0 PHY0 is controlled by COMBO_PHY_REG (reg0x1110, reg0x1114) */
    volatile uint32_t COMBO_PHY_REG2;                 /*!< Offset 0x118  */
} DSI_DPHY_TypeDef; /* size of structure = 0x11C */
/*
 * @brief EMAC
 */
/*!< EMAC  */
typedef struct EMAC_Type
{
    volatile uint32_t EMAC_BASIC_CTL0;                /*!< Offset 0x000 EMAC Basic Control Register0 */
    volatile uint32_t EMAC_BASIC_CTL1;                /*!< Offset 0x004 EMAC Basic Control Register1 */
    volatile uint32_t EMAC_INT_STA;                   /*!< Offset 0x008 EMAC Interrupt Status Register */
    volatile uint32_t EMAC_INT_EN;                    /*!< Offset 0x00C EMAC Interrupt Enable Register */
    volatile uint32_t EMAC_TX_CTL0;                   /*!< Offset 0x010 EMAC Transmit Control Register0 */
    volatile uint32_t EMAC_TX_CTL1;                   /*!< Offset 0x014 EMAC Transmit Control Register1 */
             uint32_t reserved_0x018;
    volatile uint32_t EMAC_TX_FLOW_CTL;               /*!< Offset 0x01C EMAC Transmit Flow Control Register */
    volatile uint32_t EMAC_TX_DMA_DESC_LIST;          /*!< Offset 0x020 EMAC Transmit Descriptor List Address Register */
    volatile uint32_t EMAC_RX_CTL0;                   /*!< Offset 0x024 EMAC Receive Control Register0 */
    volatile uint32_t EMAC_RX_CTL1;                   /*!< Offset 0x028 EMAC Receive Control Register1 */
             uint32_t reserved_0x02C [0x0002];
    volatile uint32_t EMAC_RX_DMA_DESC_LIST;          /*!< Offset 0x034 EMAC Receive Descriptor List Address Register */
    volatile uint32_t EMAC_RX_FRM_FLT;                /*!< Offset 0x038 EMAC Receive Frame Filter Register */
             uint32_t reserved_0x03C;
    volatile uint32_t EMAC_RX_HASH0;                  /*!< Offset 0x040 EMAC Hash Table Register0 */
    volatile uint32_t EMAC_RX_HASH1;                  /*!< Offset 0x044 EMAC Hash Table Register1 */
    volatile uint32_t EMAC_MII_CMD;                   /*!< Offset 0x048 EMAC Management Interface Command Register */
    volatile uint32_t EMAC_MII_DATA;                  /*!< Offset 0x04C EMAC Management Interface Data Register */
    struct
    {
        volatile uint32_t HIGH;                       /*!< Offset 0x050 EMAC MAC Address High Register */
        volatile uint32_t LOW;                        /*!< Offset 0x054 EMAC MAC Address Low Register */
    } EMAC_ADDR [0x008];                              /*!< Offset 0x050 EMAC MAC Address N (N=0-7) */
             uint32_t reserved_0x090 [0x0008];
    volatile uint32_t EMAC_TX_DMA_STA;                /*!< Offset 0x0B0 EMAC Transmit DMA Status Register */
    volatile uint32_t EMAC_TX_CUR_DESC;               /*!< Offset 0x0B4 EMAC Current Transmit Descriptor Register */
    volatile uint32_t EMAC_TX_CUR_BUF;                /*!< Offset 0x0B8 EMAC Current Transmit Buffer Address Register */
             uint32_t reserved_0x0BC;
    volatile uint32_t EMAC_RX_DMA_STA;                /*!< Offset 0x0C0 EMAC Receive DMA Status Register */
    volatile uint32_t EMAC_RX_CUR_DESC;               /*!< Offset 0x0C4 EMAC Current Receive Descriptor Register */
    volatile uint32_t EMAC_RX_CUR_BUF;                /*!< Offset 0x0C8 EMAC Current Receive Buffer Address Register */
             uint32_t reserved_0x0CC;
    volatile uint32_t EMAC_RGMII_STA;                 /*!< Offset 0x0D0 EMAC RGMII Status Register */
} EMAC_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_BLD
 */
/*!< G2D_BLD Graphic 2D (G2D) Engine Blender */
typedef struct G2D_BLD_Type
{
    volatile uint32_t BLD_FILL_COLOR_CTL;             /*!< Offset 0x000 BLD_EN_CTL BLD_FILL_COLOR_CTL Allwinner_DE2.0_Spec_V1.0.pdf page 106   */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t BLD_FILL_COLOR [0x004];         /*!< Offset 0x010 BLD_FILLC  N=0..1  */
    volatile uint32_t BLD_CH_ISIZE [0x004];           /*!< Offset 0x020 BLD_CH_ISIZE N=0..1   */
    volatile uint32_t BLD_CH_OFFSET [0x004];          /*!< Offset 0x030 BLD_CH_OFFSET N=0..1 */
    volatile uint32_t BLD_PREMUL_CTL;                 /*!< Offset 0x040 BLD_PREMUL_CTL      */
    volatile uint32_t BLD_BK_COLOR;                   /*!< Offset 0x044 BLD_BK_COLOR        */
    volatile uint32_t BLD_SIZE;                       /*!< Offset 0x048 BLD_SIZE            */
    volatile uint32_t BLD_CTL;                        /*!< Offset 0x04C BLD_CTL             */
    volatile uint32_t BLD_KEY_CTL;                    /*!< Offset 0x050 BLD_KEY_CTL         */
    volatile uint32_t BLD_KEY_CON;                    /*!< Offset 0x054 BLD_KEY_CON         */
    volatile uint32_t BLD_KEY_MAX;                    /*!< Offset 0x058 BLD_KEY_MAX         */
    volatile uint32_t BLD_KEY_MIN;                    /*!< Offset 0x05C BLD_KEY_MIN         */
    volatile uint32_t BLD_OUT_COLOR;                  /*!< Offset 0x060 BLD_OUT_COLOR       */
             uint32_t reserved_0x064 [0x0007];
    volatile uint32_t ROP_CTL;                        /*!< Offset 0x080 ROP_CTL             */
    volatile uint32_t ROP_INDEX [0x004];              /*!< Offset 0x084 ROP_INDEX N=0..1      */
             uint32_t reserved_0x094 [0x001B];
    volatile uint32_t BLD_CSC_CTL;                    /*!< Offset 0x100 BLD_CSC_CTL         */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t BLD_CSC0_COEF00;                /*!< Offset 0x110 BLD_CSC0_COEF00     */
    volatile uint32_t BLD_CSC0_COEF01;                /*!< Offset 0x114 BLD_CSC0_COEF01     */
    volatile uint32_t BLD_CSC0_COEF02;                /*!< Offset 0x118 BLD_CSC0_COEF02     */
    volatile uint32_t BLD_CSC0_CONST0;                /*!< Offset 0x11C BLD_CSC0_CONST0     */
    volatile uint32_t BLD_CSC0_COEF10;                /*!< Offset 0x120 BLD_CSC0_COEF10     */
    volatile uint32_t BLD_CSC0_COEF11;                /*!< Offset 0x124 BLD_CSC0_COEF11     */
    volatile uint32_t BLD_CSC0_COEF12;                /*!< Offset 0x128 BLD_CSC0_COEF12     */
    volatile uint32_t BLD_CSC0_CONST1;                /*!< Offset 0x12C BLD_CSC0_CONST1     */
    volatile uint32_t BLD_CSC0_COEF20;                /*!< Offset 0x130 BLD_CSC0_COEF20     */
    volatile uint32_t BLD_CSC0_COEF21;                /*!< Offset 0x134 BLD_CSC0_COEF21     */
    volatile uint32_t BLD_CSC0_COEF22;                /*!< Offset 0x138 BLD_CSC0_COEF22     */
    volatile uint32_t BLD_CSC0_CONST2;                /*!< Offset 0x13C BLD_CSC0_CONST2     */
    volatile uint32_t BLD_CSC1_COEF00;                /*!< Offset 0x140 BLD_CSC1_COEF00     */
    volatile uint32_t BLD_CSC1_COEF01;                /*!< Offset 0x144 BLD_CSC1_COEF01     */
    volatile uint32_t BLD_CSC1_COEF02;                /*!< Offset 0x148 BLD_CSC1_COEF02     */
    volatile uint32_t BLD_CSC1_CONST0;                /*!< Offset 0x14C BLD_CSC1_CONST0     */
    volatile uint32_t BLD_CSC1_COEF10;                /*!< Offset 0x150 BLD_CSC1_COEF10     */
    volatile uint32_t BLD_CSC1_COEF11;                /*!< Offset 0x154 BLD_CSC1_COEF11     */
    volatile uint32_t BLD_CSC1_COEF12;                /*!< Offset 0x158 BLD_CSC1_COEF12     */
    volatile uint32_t BLD_CSC1_CONST1;                /*!< Offset 0x15C BLD_CSC1_CONST1     */
    volatile uint32_t BLD_CSC1_COEF20;                /*!< Offset 0x160 BLD_CSC1_COEF20     */
    volatile uint32_t BLD_CSC1_COEF21;                /*!< Offset 0x164 BLD_CSC1_COEF21     */
    volatile uint32_t BLD_CSC1_COEF22;                /*!< Offset 0x168 BLD_CSC1_COEF22     */
    volatile uint32_t BLD_CSC1_CONST2;                /*!< Offset 0x16C BLD_CSC1_CONST2     */
    volatile uint32_t BLD_CSC2_COEF00;                /*!< Offset 0x170 BLD_CSC2_COEF00     */
    volatile uint32_t BLD_CSC2_COEF01;                /*!< Offset 0x174 BLD_CSC2_COEF01     */
    volatile uint32_t BLD_CSC2_COEF02;                /*!< Offset 0x178 BLD_CSC2_COEF02     */
    volatile uint32_t BLD_CSC2_CONST0;                /*!< Offset 0x17C BLD_CSC2_CONST0     */
    volatile uint32_t BLD_CSC2_COEF10;                /*!< Offset 0x180 BLD_CSC2_COEF10     */
    volatile uint32_t BLD_CSC2_COEF11;                /*!< Offset 0x184 BLD_CSC2_COEF11     */
    volatile uint32_t BLD_CSC2_COEF12;                /*!< Offset 0x188 BLD_CSC2_COEF12     */
    volatile uint32_t BLD_CSC2_CONST1;                /*!< Offset 0x18C BLD_CSC2_CONST1     */
    volatile uint32_t BLD_CSC2_COEF20;                /*!< Offset 0x190 BLD_CSC2_COEF20     */
    volatile uint32_t BLD_CSC2_COEF21;                /*!< Offset 0x194 BLD_CSC2_COEF21     */
    volatile uint32_t BLD_CSC2_COEF22;                /*!< Offset 0x198 BLD_CSC2_COEF22     */
    volatile uint32_t BLD_CSC2_CONST2;                /*!< Offset 0x19C BLD_CSC2_CONST2     */
} G2D_BLD_TypeDef; /* size of structure = 0x1A0 */
/*
 * @brief G2D_MIXER
 */
/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer */
typedef struct G2D_MIXER_Type
{
    volatile uint32_t G2D_MIXER_CTRL;                 /*!< Offset 0x000 G2D mixer control */
    volatile uint32_t G2D_MIXER_INTERRUPT;            /*!< Offset 0x004 G2D mixer interrupt */
    volatile uint32_t G2D_MIXER_CLK;                  /*!< Offset 0x008 G2D mixer clock */
} G2D_MIXER_TypeDef; /* size of structure = 0x00C */
/*
 * @brief G2D_ROT
 */
/*!< G2D_ROT Graphic 2D Rotate */
typedef struct G2D_ROT_Type
{
    volatile uint32_t ROT_CTL;                        /*!< Offset 0x000 ROT_CTL						 */
    volatile uint32_t ROT_INT;                        /*!< Offset 0x004 ROT_INT				 */
    volatile uint32_t ROT_TIMEOUT;                    /*!< Offset 0x008 ROT_TIMEOUT */
             uint32_t reserved_0x00C [0x0005];
    volatile uint32_t ROT_IFMT;                       /*!< Offset 0x020 Input data attribute register */
    volatile uint32_t ROT_ISIZE;                      /*!< Offset 0x024 Input data size register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t ROT_IPITCH0;                    /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    volatile uint32_t ROT_IPITCH1;                    /*!< Offset 0x034 Input U/UV memory pitch register */
    volatile uint32_t ROT_IPITCH2;                    /*!< Offset 0x038 Input V memory pitch register */
             uint32_t reserved_0x03C;
    volatile uint32_t ROT_ILADD0;                     /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    volatile uint32_t ROT_IHADD0;                     /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    volatile uint32_t ROT_ILADD1;                     /*!< Offset 0x048 Input U/UV memory address register0 */
    volatile uint32_t ROT_IHADD1;                     /*!< Offset 0x04C Input U/UV memory address register1 */
    volatile uint32_t ROT_ILADD2;                     /*!< Offset 0x050 Input V memory address register0 */
    volatile uint32_t ROT_IHADD2;                     /*!< Offset 0x054 Input V memory address register1 */
             uint32_t reserved_0x058 [0x000B];
    volatile uint32_t ROT_OSIZE;                      /*!< Offset 0x084 Output data size register */
             uint32_t reserved_0x088 [0x0002];
    volatile uint32_t ROT_OPITCH0;                    /*!< Offset 0x090 Output Y/RGB/ARGB memory pitch register */
    volatile uint32_t ROT_OPITCH1;                    /*!< Offset 0x094 Output U/UV memory pitch register */
    volatile uint32_t ROT_OPITCH2;                    /*!< Offset 0x098 Output V memory pitch register */
             uint32_t reserved_0x09C;
    volatile uint32_t ROT_OLADD0;                     /*!< Offset 0x0A0 Output Y/RGB/ARGB memory address register0 */
    volatile uint32_t ROT_OHADD0;                     /*!< Offset 0x0A4 Output Y/RGB/ARGB memory address register1 */
    volatile uint32_t ROT_OLADD1;                     /*!< Offset 0x0A8 Output U/UV memory address register0 */
    volatile uint32_t ROT_OHADD1;                     /*!< Offset 0x0AC Output U/UV memory address register1 */
    volatile uint32_t ROT_OLADD2;                     /*!< Offset 0x0B0 Output V memory address register0 */
    volatile uint32_t ROT_OHADD2;                     /*!< Offset 0x0B4 Output V memory address register1 */
             uint32_t reserved_0x0B8 [0x0004];
    volatile uint32_t LBC_ENC_CTL;                    /*!< Offset 0x0C8 Encode control (bit 31 - is lossy) */
    volatile uint32_t LBC_CTL;                        /*!< Offset 0x0CC  */
    volatile uint32_t LBC_DEC_CTL;                    /*!< Offset 0x0D0 Decode control (bit 31 - is lossy)  */
} G2D_ROT_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_TOP
 */
/*!< G2D_TOP Graphic 2D top */
typedef struct G2D_TOP_Type
{
    volatile uint32_t G2D_SCLK_GATE;                  /*!< Offset 0x000 G2D SCLK gate   */
    volatile uint32_t G2D_HCLK_GATE;                  /*!< Offset 0x004 g2d HCLK gate   */
    volatile uint32_t G2D_AHB_RST;                    /*!< Offset 0x008 G2D AHB reset   */
    volatile uint32_t G2D_SCLK_DIV;                   /*!< Offset 0x00C G2D SCLK div    */
    volatile uint32_t G2D_VERSION;                    /*!< Offset 0x010  */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t RCQ_IRQ_CTL;                    /*!< Offset 0x020  */
    volatile uint32_t RCQ_STATUS;                     /*!< Offset 0x024  */
    volatile uint32_t RCQ_CTRL;                       /*!< Offset 0x028  */
    volatile uint32_t RCQ_HEADER_LOW_ADDR;            /*!< Offset 0x02C Register Configuration Queue Header bits 31..0 */
    volatile uint32_t RCQ_HEADER_HIGH_ADDR;           /*!< Offset 0x030 Register Configuration Queue Header bits 39..32 */
    volatile uint32_t RCQ_HEADER_LEN;                 /*!< Offset 0x034 Register Configuration Queue length bits 15..0 */
} G2D_TOP_TypeDef; /* size of structure = 0x038 */
/*
 * @brief G2D_UI
 */
/*!< G2D_UI Graphic 2D UI surface */
typedef struct G2D_UI_Type
{
    volatile uint32_t UI_ATTR;                        /*!< Offset 0x000 UIx_ATTR */
    volatile uint32_t UI_MBSIZE;                      /*!< Offset 0x004 UIx_MBSIZE */
    volatile uint32_t UI_COOR;                        /*!< Offset 0x008 UIx_COOR */
    volatile uint32_t UI_PITCH;                       /*!< Offset 0x00C UIx_PITCH */
    volatile uint32_t UI_LADD;                        /*!< Offset 0x010 UIx_LADD */
    volatile uint32_t UI_FILLC;                       /*!< Offset 0x014 UIx_FILLC */
    volatile uint32_t UI_HADD;                        /*!< Offset 0x018 UIx_HADD */
    volatile uint32_t UI_SIZE;                        /*!< Offset 0x01C UIx_SIZE */
} G2D_UI_TypeDef; /* size of structure = 0x020 */
/*
 * @brief G2D_VI
 */
/*!< G2D_VI Graphic 2D VI surface */
typedef struct G2D_VI_Type
{
    volatile uint32_t V0_ATTCTL;                      /*!< Offset 0x000 V0_ATTCTL */
    volatile uint32_t V0_MBSIZE;                      /*!< Offset 0x004 Source rectangle size (may be empty) */
    volatile uint32_t V0_COOR;                        /*!< Offset 0x008 Target window position */
    volatile uint32_t V0_PITCH0;                      /*!< Offset 0x00C V0_PITCH0 */
    volatile uint32_t V0_PITCH1;                      /*!< Offset 0x010 V0_PITCH1 */
    volatile uint32_t V0_PITCH2;                      /*!< Offset 0x014 V0_PITCH2 */
    volatile uint32_t V0_LADD0;                       /*!< Offset 0x018 V0_LADD0 */
    volatile uint32_t V0_LADD1;                       /*!< Offset 0x01C V0_LADD1 */
    volatile uint32_t V0_LADD2;                       /*!< Offset 0x020 V0_LADD2 */
    volatile uint32_t V0_FILLC;                       /*!< Offset 0x024 V0_FILLC */
    volatile uint32_t V0_HADD;                        /*!< Offset 0x028 [31:24]: 0, [23:16]: LADD2 bits 40..32, [15:8]: LADD1 bits 40..32, [7:0]: LADD0 bits 40..32 */
    volatile uint32_t V0_SIZE;                        /*!< Offset 0x02C Source window size (may not be empty) */
    volatile uint32_t V0_HDS_CTL0;                    /*!< Offset 0x030 V0_HDS_CTL0 */
    volatile uint32_t V0_HDS_CTL1;                    /*!< Offset 0x034 V0_HDS_CTL1 */
    volatile uint32_t V0_VDS_CTL0;                    /*!< Offset 0x038 V0_VDS_CTL0 */
    volatile uint32_t V0_VDS_CTL1;                    /*!< Offset 0x03C V0_VDS_CTL1 */
} G2D_VI_TypeDef; /* size of structure = 0x040 */
/*
 * @brief G2D_VSU
 */
/*!< G2D_VSU Graphic 2D Video Scaler */
typedef struct G2D_VSU_Type
{
    volatile uint32_t VS_CTRL;                        /*!< Offset 0x000 VS_CTRL */
             uint32_t reserved_0x004 [0x000F];
    volatile uint32_t VS_OUT_SIZE;                    /*!< Offset 0x040 VS_OUT_SIZE */
    volatile uint32_t VS_GLB_ALPHA;                   /*!< Offset 0x044 VS_GLB_ALPHA */
             uint32_t reserved_0x048 [0x000E];
    volatile uint32_t VS_Y_SIZE;                      /*!< Offset 0x080 VS_Y_SIZE */
             uint32_t reserved_0x084;
    volatile uint32_t VS_Y_HSTEP;                     /*!< Offset 0x088 VS_Y_HSTEP */
    volatile uint32_t VS_Y_VSTEP;                     /*!< Offset 0x08C VS_Y_VSTEP */
    volatile uint32_t VS_Y_HPHASE;                    /*!< Offset 0x090 VS_Y_HPHASE */
             uint32_t reserved_0x094;
    volatile uint32_t VS_Y_VPHASE0;                   /*!< Offset 0x098 VS_Y_VPHASE0 */
             uint32_t reserved_0x09C [0x0009];
    volatile uint32_t VS_C_SIZE;                      /*!< Offset 0x0C0 VS_C_SIZE */
             uint32_t reserved_0x0C4;
    volatile uint32_t VS_C_HSTEP;                     /*!< Offset 0x0C8 VS_C_HSTEP */
    volatile uint32_t VS_C_VSTEP;                     /*!< Offset 0x0CC VS_C_VSTEP */
    volatile uint32_t VS_C_HPHASE;                    /*!< Offset 0x0D0 VS_C_HPHASE */
             uint32_t reserved_0x0D4;
    volatile uint32_t VS_C_VPHASE0;                   /*!< Offset 0x0D8 VS_C_VPHASE0 */
             uint32_t reserved_0x0DC [0x0049];
    volatile uint32_t VS_Y_HCOEF [0x040];             /*!< Offset 0x200 VS_Y_HCOEF[N]. N=0..31 */
    volatile uint32_t VS_Y_VCOEF [0x040];             /*!< Offset 0x300 VS_Y_VCOEF[N]. N=0..31 */
    volatile uint32_t VS_C_HCOEF [0x040];             /*!< Offset 0x400 VS_C_HCOEF[N]. N=0..31 */
} G2D_VSU_TypeDef; /* size of structure = 0x500 */
/*
 * @brief G2D_WB
 */
/*!< G2D_WB Graphic 2D (G2D) Engine Write Back */
typedef struct G2D_WB_Type
{
    volatile uint32_t WB_ATT;                         /*!< Offset 0x000 WB_ATT */
    volatile uint32_t WB_SIZE;                        /*!< Offset 0x004 WB_SIZE */
    volatile uint32_t WB_PITCH0;                      /*!< Offset 0x008 WB_PITCH0 */
    volatile uint32_t WB_PITCH1;                      /*!< Offset 0x00C WB_PITCH1 */
    volatile uint32_t WB_PITCH2;                      /*!< Offset 0x010 WB_PITCH2 */
    volatile uint32_t WB_LADD0;                       /*!< Offset 0x014 WB_LADD0 */
    volatile uint32_t WB_HADD0;                       /*!< Offset 0x018 WB_HADD0 */
    volatile uint32_t WB_LADD1;                       /*!< Offset 0x01C WB_LADD1 */
    volatile uint32_t WB_HADD1;                       /*!< Offset 0x020 WB_HADD1 */
    volatile uint32_t WB_LADD2;                       /*!< Offset 0x024 WB_LADD2 */
    volatile uint32_t WB_HADD2;                       /*!< Offset 0x028 WB_HADD2 */
} G2D_WB_TypeDef; /* size of structure = 0x02C */
/*
 * @brief GPADC
 */
/*!< GPADC  */
typedef struct GPADC_Type
{
    volatile uint32_t GP_SR_CON;                      /*!< Offset 0x000 GPADC Sample Rate Configure Register */
    volatile uint32_t GP_CTRL;                        /*!< Offset 0x004 GPADC Control Register */
    volatile uint32_t GP_CS_EN;                       /*!< Offset 0x008 GPADC Compare and Select Enable Register */
    volatile uint32_t GP_FIFO_INTC;                   /*!< Offset 0x00C GPADC FIFO Interrupt Control Register */
    volatile uint32_t GP_FIFO_INTS;                   /*!< Offset 0x010 GPADC FIFO Interrupt Status Register */
    volatile uint32_t GP_FIFO_DATA;                   /*!< Offset 0x014 GPADC FIFO Data Register */
    volatile uint32_t GP_CDATA;                       /*!< Offset 0x018 GPADC Calibration Data Register */
             uint32_t reserved_0x01C;
    volatile uint32_t GP_DATAL_INTC;                  /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
    volatile uint32_t GP_DATAH_INTC;                  /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
    volatile uint32_t GP_DATA_INTC;                   /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
             uint32_t reserved_0x02C;
    volatile uint32_t GP_DATAL_INTS;                  /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
    volatile uint32_t GP_DATAH_INTS;                  /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
    volatile uint32_t GP_DATA_INTS;                   /*!< Offset 0x038 GPADC Data Interrupt Status Register */
             uint32_t reserved_0x03C;
    volatile uint32_t GP_CH0_CMP_DATA;                /*!< Offset 0x040 GPADC CH0 Compare Data Register */
             uint32_t reserved_0x044 [0x000F];
    volatile uint32_t GP_CH0_DATA;                    /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef; /* size of structure = 0x084 */
/*
 * @brief GPIO
 */
/*!< GPIO  */
typedef struct GPIO_Type
{
    volatile uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
    volatile uint32_t DATA;                           /*!< Offset 0x010 Data Register */
    volatile uint32_t DRV [0x004];                    /*!< Offset 0x014 Multi_Driving Register */
    volatile uint32_t PULL [0x002];                   /*!< Offset 0x024 Pull Register */
             uint32_t reserved_0x02C;
} GPIO_TypeDef; /* size of structure = 0x030 */
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
        volatile uint32_t DRV [0x004];                /*!< Offset 0x014 Multi_Driving Register */
        volatile uint32_t PULL [0x002];               /*!< Offset 0x024 Pull Register */
                 uint32_t reserved_0x02C;
    } GPIO_PINS [0x007];                              /*!< Offset 0x000 GPIO pin control */
             uint32_t reserved_0x150 [0x002C];
    struct
    {
        volatile uint32_t EINT_CFG [0x004];           /*!< Offset 0x200 External Interrupt Configure Registers */
        volatile uint32_t EINT_CTL;                   /*!< Offset 0x210 External Interrupt Control Register */
        volatile uint32_t EINT_STATUS;                /*!< Offset 0x214 External Interrupt Status Register */
        volatile uint32_t EINT_DEB;                   /*!< Offset 0x218 External Interrupt Debounce Register */
                 uint32_t reserved_0x01C;
    } GPIO_INTS [0x007];                              /*!< Offset 0x200 GPIO interrupt control */
             uint32_t reserved_0x2E0 [0x0018];
    volatile uint32_t PIO_POW_MOD_SEL;                /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    volatile uint32_t PIO_POW_MS_CTL;                 /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    volatile uint32_t PIO_POW_VAL;                    /*!< Offset 0x348 PIO Group Power Value Register */
} GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
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
 * @brief HSTIMER
 */
/*!< HSTIMER High Speed Timer (HSTimer) */
typedef struct HSTIMER_Type
{
    volatile uint32_t HS_TMR_IRQ_EN_REG;              /*!< Offset 0x000 HS Timer IRQ Enable Register */
    volatile uint32_t HS_TMR_IRQ_STAS_REG;            /*!< Offset 0x004 HS Timer Status Register */
             uint32_t reserved_0x008 [0x0006];
    volatile uint32_t HS_TMR0_CTRL_REG;               /*!< Offset 0x020 HS Timer0 Control Register */
    volatile uint32_t HS_TMR0_INTV_LO_REG;            /*!< Offset 0x024 HS Timer0 Interval Value Low Register */
    volatile uint32_t HS_TMR0_INTV_HI_REG;            /*!< Offset 0x028 HS Timer0 Interval Value High Register */
    volatile uint32_t HS_TMR0_CURNT_LO_REG;           /*!< Offset 0x02C HS Timer0 Current Value Low Register */
    volatile uint32_t HS_TMR0_CURNT_HI_REG;           /*!< Offset 0x030 HS Timer0 Current Value High Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t HS_TMR1_CTRL_REG;               /*!< Offset 0x040 HS Timer1 Control Register */
    volatile uint32_t HS_TMR1_INTV_LO_REG;            /*!< Offset 0x044 HS Timer1 Interval Value Low Register */
    volatile uint32_t HS_TMR1_INTV_HI_REG;            /*!< Offset 0x048 HS Timer1 Interval Value High Register */
    volatile uint32_t HS_TMR1_CURNT_LO_REG;           /*!< Offset 0x04C HS Timer1 Current Value Low Register */
    volatile uint32_t HS_TMR1_CURNT_HI_REG;           /*!< Offset 0x050 HS Timer1 Current Value High Register */
} HSTIMER_TypeDef; /* size of structure = 0x054 */
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM  */
typedef struct I2S_PCM_Type
{
    volatile uint32_t I2S_PCM_CTL;                    /*!< Offset 0x000 I2S/PCM Control Register */
    volatile uint32_t I2S_PCM_FMT0;                   /*!< Offset 0x004 I2S/PCM Format Register 0 */
    volatile uint32_t I2S_PCM_FMT1;                   /*!< Offset 0x008 I2S/PCM Format Register 1 */
    volatile uint32_t I2S_PCM_ISTA;                   /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    volatile uint32_t I2S_PCM_RXFIFO;                 /*!< Offset 0x010 I2S/PCM RXFIFO Register */
    volatile uint32_t I2S_PCM_FCTL;                   /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    volatile uint32_t I2S_PCM_FSTA;                   /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    volatile uint32_t I2S_PCM_INT;                    /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    volatile uint32_t I2S_PCM_TXFIFO;                 /*!< Offset 0x020 I2S/PCM TXFIFO Register */
    volatile uint32_t I2S_PCM_CLKD;                   /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    volatile uint32_t I2S_PCM_TXCNT;                  /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    volatile uint32_t I2S_PCM_RXCNT;                  /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    volatile uint32_t I2S_PCM_CHCFG;                  /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
    volatile uint32_t I2S_PCM_TX0CHSEL;               /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
    volatile uint32_t I2S_PCM_TX1CHSEL;               /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
    volatile uint32_t I2S_PCM_TX2CHSEL;               /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
    volatile uint32_t I2S_PCM_TX3CHSEL;               /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
    volatile uint32_t I2S_PCM_TX0CHMAP0;              /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX0CHMAP1;              /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX1CHMAP0;              /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX1CHMAP1;              /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX2CHMAP0;              /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX2CHMAP1;              /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX3CHMAP0;              /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX3CHMAP1;              /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_RXCHSEL;                /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
    volatile uint32_t I2S_PCM_RXCHMAP [0x004];        /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0..3 */
             uint32_t reserved_0x078 [0x0002];
    volatile uint32_t MCLKCFG;                        /*!< Offset 0x080 ASRC MCLK Configuration Register */
    volatile uint32_t FsoutCFG;                       /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
    volatile uint32_t FsinEXTCFG;                     /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
    volatile uint32_t ASRCCFG;                        /*!< Offset 0x08C ASRC Enable Register */
    volatile uint32_t ASRCMANCFG;                     /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
    volatile uint32_t ASRCRATIOSTAT;                  /*!< Offset 0x094 ASRC Status Register */
    volatile uint32_t ASRCFIFOSTAT;                   /*!< Offset 0x098 ASRC FIFO Level Status Register */
    volatile uint32_t ASRCMBISTCFG;                   /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
    volatile uint32_t ASRCMBISTSTAT;                  /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief IOMMU
 */
/*!< IOMMU  */
typedef struct IOMMU_Type
{
             uint32_t reserved_0x000 [0x0004];
    volatile uint32_t IOMMU_RESET_REG;                /*!< Offset 0x010 IOMMU Reset Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t IOMMU_ENABLE_REG;               /*!< Offset 0x020 IOMMU Enable Register */
             uint32_t reserved_0x024 [0x0003];
    volatile uint32_t IOMMU_BYPASS_REG;               /*!< Offset 0x030 IOMMU Bypass Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t IOMMU_AUTO_GATING_REG;          /*!< Offset 0x040 IOMMU Auto Gating Register */
    volatile uint32_t IOMMU_WBUF_CTRL_REG;            /*!< Offset 0x044 IOMMU Write Buffer Control Register */
    volatile uint32_t IOMMU_OOO_CTRL_REG;             /*!< Offset 0x048 IOMMU Out of Order Control Register */
    volatile uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;     /*!< Offset 0x04C IOMMU 4KB Boundary Protect Control Register */
    volatile uint32_t IOMMU_TTB_REG;                  /*!< Offset 0x050 IOMMU Translation Table Base Register */
             uint32_t reserved_0x054 [0x0003];
    volatile uint32_t IOMMU_TLB_ENABLE_REG;           /*!< Offset 0x060 IOMMU TLB Enable Register */
             uint32_t reserved_0x064 [0x0003];
    volatile uint32_t IOMMU_TLB_PREFETCH_REG;         /*!< Offset 0x070 IOMMU TLB Prefetch Register */
             uint32_t reserved_0x074 [0x0003];
    volatile uint32_t IOMMU_TLB_FLUSH_ENABLE_REG;     /*!< Offset 0x080 IOMMU TLB Flush Enable Register */
    volatile uint32_t IOMMU_TLB_IVLD_MODE_SEL_REG;    /*!< Offset 0x084 IOMMU TLB Invalidation Mode Select Register */
    volatile uint32_t IOMMU_TLB_IVLD_STA_ADDR_REG;    /*!< Offset 0x088 IOMMU TLB Invalidation Start Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_END_ADDR_REG;    /*!< Offset 0x08C IOMMU TLB Invalidation End Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_REG;        /*!< Offset 0x090 IOMMU TLB Invalidation Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_MASK_REG;   /*!< Offset 0x094 IOMMU TLB Invalidation Address Mask Register */
    volatile uint32_t IOMMU_TLB_IVLD_ENABLE_REG;      /*!< Offset 0x098 IOMMU TLB Invalidation Enable Register */
    volatile uint32_t IOMMU_PC_IVLD_MODE_SEL_REG;     /*!< Offset 0x09C IOMMU PC Invalidation Mode Select Register */
    volatile uint32_t IOMMU_PC_IVLD_ADDR_REG;         /*!< Offset 0x0A0 IOMMU PC Invalidation Address Register */
    volatile uint32_t IOMMU_PC_IVLD_STA_ADDR_REG;     /*!< Offset 0x0A4 IOMMU PC Invalidation Start Address Register */
    volatile uint32_t IOMMU_PC_IVLD_ENABLE_REG;       /*!< Offset 0x0A8 IOMMU PC Invalidation Enable Register */
    volatile uint32_t IOMMU_PC_IVLD_END_ADDR_REG;     /*!< Offset 0x0AC IOMMU PC Invalidation End Address Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL0_REG;         /*!< Offset 0x0B0 IOMMU Domain Authority Control 0 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL1_REG;         /*!< Offset 0x0B4 IOMMU Domain Authority Control 1 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL2_REG;         /*!< Offset 0x0B8 IOMMU Domain Authority Control 2 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL3_REG;         /*!< Offset 0x0BC IOMMU Domain Authority Control 3 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL4_REG;         /*!< Offset 0x0C0 IOMMU Domain Authority Control 4 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL5_REG;         /*!< Offset 0x0C4 IOMMU Domain Authority Control 5 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL6_REG;         /*!< Offset 0x0C8 IOMMU Domain Authority Control 6 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL7_REG;         /*!< Offset 0x0CC IOMMU Domain Authority Control 7 Register */
    volatile uint32_t IOMMU_DM_AUT_OVWT_REG;          /*!< Offset 0x0D0 IOMMU Domain Authority Overwrite Register */
             uint32_t reserved_0x0D4 [0x000B];
    volatile uint32_t IOMMU_INT_ENABLE_REG;           /*!< Offset 0x100 IOMMU Interrupt Enable Register */
    volatile uint32_t IOMMU_INT_CLR_REG;              /*!< Offset 0x104 IOMMU Interrupt Clear Register */
    volatile uint32_t IOMMU_INT_STA_REG;              /*!< Offset 0x108 IOMMU Interrupt Status Register */
             uint32_t reserved_0x10C;
    volatile uint32_t IOMMU_INT_ERR_ADDR0_REG;        /*!< Offset 0x110 IOMMU Interrupt Error Address 0 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR1_REG;        /*!< Offset 0x114 IOMMU Interrupt Error Address 1 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR2_REG;        /*!< Offset 0x118 IOMMU Interrupt Error Address 2 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR3_REG;        /*!< Offset 0x11C IOMMU Interrupt Error Address 3 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR4_REG;        /*!< Offset 0x120 IOMMU Interrupt Error Address 4 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR5_REG;        /*!< Offset 0x124 IOMMU Interrupt Error Address 5 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR6_REG;        /*!< Offset 0x128 IOMMU Interrupt Error Address 6 Register */
             uint32_t reserved_0x12C;
    volatile uint32_t IOMMU_INT_ERR_ADDR7_REG;        /*!< Offset 0x130 IOMMU Interrupt Error Address 7 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR8_REG;        /*!< Offset 0x134 IOMMU Interrupt Error Address 8 Register */
             uint32_t reserved_0x138 [0x0006];
    volatile uint32_t IOMMU_INT_ERR_DATA0_REG;        /*!< Offset 0x150 IOMMU Interrupt Error Data 0 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA1_REG;        /*!< Offset 0x154 IOMMU Interrupt Error Data 1 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA2_REG;        /*!< Offset 0x158 IOMMU Interrupt Error Data 2 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA3_REG;        /*!< Offset 0x15C IOMMU Interrupt Error Data 3 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA4_REG;        /*!< Offset 0x160 IOMMU Interrupt Error Data 4 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA5_REG;        /*!< Offset 0x164 IOMMU Interrupt Error Data 5 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA6_REG;        /*!< Offset 0x168 IOMMU Interrupt Error Data 6 Register */
             uint32_t reserved_0x16C;
    volatile uint32_t IOMMU_INT_ERR_DATA7_REG;        /*!< Offset 0x170 IOMMU Interrupt Error Data 7 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA8_REG;        /*!< Offset 0x174 IOMMU Interrupt Error Data 8 Register */
             uint32_t reserved_0x178 [0x0002];
    volatile uint32_t IOMMU_L1PG_INT_REG;             /*!< Offset 0x180 IOMMU L1 Page Table Interrupt Register */
    volatile uint32_t IOMMU_L2PG_INT_REG;             /*!< Offset 0x184 IOMMU L2 Page Table Interrupt Register */
             uint32_t reserved_0x188 [0x0002];
    volatile uint32_t IOMMU_VA_REG;                   /*!< Offset 0x190 IOMMU Virtual Address Register */
    volatile uint32_t IOMMU_VA_DATA_REG;              /*!< Offset 0x194 IOMMU Virtual Address Data Register */
    volatile uint32_t IOMMU_VA_CONFIG_REG;            /*!< Offset 0x198 IOMMU Virtual Address Configuration Register */
             uint32_t reserved_0x19C [0x0019];
    volatile uint32_t IOMMU_PMU_ENABLE_REG;           /*!< Offset 0x200 IOMMU PMU Enable Register */
             uint32_t reserved_0x204 [0x0003];
    volatile uint32_t IOMMU_PMU_CLR_REG;              /*!< Offset 0x210 IOMMU PMU Clear Register */
             uint32_t reserved_0x214 [0x0007];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW0_REG;      /*!< Offset 0x230 IOMMU PMU Access Low 0 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH0_REG;     /*!< Offset 0x234 IOMMU PMU Access High 0 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW0_REG;         /*!< Offset 0x238 IOMMU PMU Hit Low 0 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH0_REG;        /*!< Offset 0x23C IOMMU PMU Hit High 0 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW1_REG;      /*!< Offset 0x240 IOMMU PMU Access Low 1 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH1_REG;     /*!< Offset 0x244 IOMMU PMU Access High 1 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW1_REG;         /*!< Offset 0x248 IOMMU PMU Hit Low 1 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH1_REG;        /*!< Offset 0x24C IOMMU PMU Hit High 1 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW2_REG;      /*!< Offset 0x250 IOMMU PMU Access Low 2 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH2_REG;     /*!< Offset 0x254 IOMMU PMU Access High 2 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW2_REG;         /*!< Offset 0x258 IOMMU PMU Hit Low 2 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH2_REG;        /*!< Offset 0x25C IOMMU PMU Hit High 2 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW3_REG;      /*!< Offset 0x260 IOMMU PMU Access Low 3 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH3_REG;     /*!< Offset 0x264 IOMMU PMU Access High 3 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW3_REG;         /*!< Offset 0x268 IOMMU PMU Hit Low 3 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH3_REG;        /*!< Offset 0x26C IOMMU PMU Hit High 3 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW4_REG;      /*!< Offset 0x270 IOMMU PMU Access Low 4 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH4_REG;     /*!< Offset 0x274 IOMMU PMU Access High 4 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW4_REG;         /*!< Offset 0x278 IOMMU PMU Hit Low 4 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH4_REG;        /*!< Offset 0x27C IOMMU PMU Hit High 4 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW5_REG;      /*!< Offset 0x280 IOMMU PMU Access Low 5 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH5_REG;     /*!< Offset 0x284 IOMMU PMU Access High 5 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW5_REG;         /*!< Offset 0x288 IOMMU PMU Hit Low 5 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH5_REG;        /*!< Offset 0x28C IOMMU PMU Hit High 5 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW6_REG;      /*!< Offset 0x290 IOMMU PMU Access Low 6 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH6_REG;     /*!< Offset 0x294 IOMMU PMU Access High 6 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW6_REG;         /*!< Offset 0x298 IOMMU PMU Hit Low 6 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH6_REG;        /*!< Offset 0x29C IOMMU PMU Hit High 6 Register */
             uint32_t reserved_0x2A0 [0x000C];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW7_REG;      /*!< Offset 0x2D0 IOMMU PMU Access Low 7 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH7_REG;     /*!< Offset 0x2D4 IOMMU PMU Access High 7 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW7_REG;         /*!< Offset 0x2D8 IOMMU PMU Hit Low 7 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH7_REG;        /*!< Offset 0x2DC IOMMU PMU Hit High 7 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW8_REG;      /*!< Offset 0x2E0 IOMMU PMU Access Low 8 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH8_REG;     /*!< Offset 0x2E4 IOMMU PMU Access High 8 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW8_REG;         /*!< Offset 0x2E8 IOMMU PMU Hit Low 8 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH8_REG;        /*!< Offset 0x2EC IOMMU PMU Hit High 8 Register */
             uint32_t reserved_0x2F0 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW0_REG;          /*!< Offset 0x300 IOMMU Total Latency Low 0 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH0_REG;         /*!< Offset 0x304 IOMMU Total Latency High 0 Register */
    volatile uint32_t IOMMU_PMU_ML0_REG;              /*!< Offset 0x308 IOMMU Max Latency 0 Register */
             uint32_t reserved_0x30C;
    volatile uint32_t IOMMU_PMU_TL_LOW1_REG;          /*!< Offset 0x310 IOMMU Total Latency Low 1 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH1_REG;         /*!< Offset 0x314 IOMMU Total Latency High 1 Register */
    volatile uint32_t IOMMU_PMU_ML1_REG;              /*!< Offset 0x318 IOMMU Max Latency 1 Register */
             uint32_t reserved_0x31C;
    volatile uint32_t IOMMU_PMU_TL_LOW2_REG;          /*!< Offset 0x320 IOMMU Total Latency Low 2 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH2_REG;         /*!< Offset 0x324 IOMMU Total Latency High 2 Register */
    volatile uint32_t IOMMU_PMU_ML2_REG;              /*!< Offset 0x328 IOMMU Max Latency 2 Register */
             uint32_t reserved_0x32C;
    volatile uint32_t IOMMU_PMU_TL_LOW3_REG;          /*!< Offset 0x330 IOMMU Total Latency Low 3 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH3_REG;         /*!< Offset 0x334 IOMMU Total Latency High 3 Register */
    volatile uint32_t IOMMU_PMU_ML3_REG;              /*!< Offset 0x338 IOMMU Max Latency 3 Register */
             uint32_t reserved_0x33C;
    volatile uint32_t IOMMU_PMU_TL_LOW4_REG;          /*!< Offset 0x340 IOMMU Total Latency Low 4 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH4_REG;         /*!< Offset 0x344 IOMMU Total Latency High 4 Register */
    volatile uint32_t IOMMU_PMU_ML4_REG;              /*!< Offset 0x348 IOMMU Max Latency 4 Register */
             uint32_t reserved_0x34C;
    volatile uint32_t IOMMU_PMU_TL_LOW5_REG;          /*!< Offset 0x350 IOMMU Total Latency Low 5 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH5_REG;         /*!< Offset 0x354 IOMMU Total Latency High 5 Register */
    volatile uint32_t IOMMU_PMU_ML5_REG;              /*!< Offset 0x358 IOMMU Max Latency 5 Register */
             uint32_t reserved_0x35C;
    volatile uint32_t IOMMU_PMU_TL_LOW6_REG;          /*!< Offset 0x360 IOMMU Total Latency Low 6 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH6_REG;         /*!< Offset 0x364 IOMMU Total Latency High 6 Register */
    volatile uint32_t IOMMU_PMU_ML6_REG;              /*!< Offset 0x368 IOMMU Max Latency 6 Register */
} IOMMU_TypeDef; /* size of structure = 0x36C */
/*
 * @brief LEDC
 */
/*!< LEDC LED Lamp Controller */
typedef struct LEDC_Type
{
    volatile uint32_t LEDC_CTRL_REG;                  /*!< Offset 0x000 LEDC Control Register */
    volatile uint32_t LED_T01_TIMING_CTRL_REG;        /*!< Offset 0x004 LEDC T0 & T1 Timing Control Register */
    volatile uint32_t LEDC_DATA_FINISH_CNT_REG;       /*!< Offset 0x008 LEDC Data Finish Counter Register */
    volatile uint32_t LED_RESET_TIMING_CTRL_REG;      /*!< Offset 0x00C LEDC Reset Timing Control Register */
    volatile uint32_t LEDC_WAIT_TIME0_CTRL_REG;       /*!< Offset 0x010 LEDC Wait Time0 Control Register */
    volatile uint32_t LEDC_DATA_REG;                  /*!< Offset 0x014 LEDC Data Register */
    volatile uint32_t LEDC_DMA_CTRL_REG;              /*!< Offset 0x018 LEDC DMA Control Register */
    volatile uint32_t LEDC_INT_CTRL_REG;              /*!< Offset 0x01C LEDC Interrupt Control Register */
    volatile uint32_t LEDC_INT_STS_REG;               /*!< Offset 0x020 LEDC Interrupt Status Register */
             uint32_t reserved_0x024;
    volatile uint32_t LEDC_WAIT_TIME1_CTRL_REG;       /*!< Offset 0x028 LEDC Wait Time1 Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t LEDC_FIFO_DATA_REG [0x020];     /*!< Offset 0x030 LEDC FIFO Data Registers array */
} LEDC_TypeDef; /* size of structure = 0x0B0 */
/*
 * @brief MCTL_COM
 */
/*!< MCTL_COM  */
typedef struct MCTL_COM_Type
{
    volatile uint32_t MCTL_COM_WORK_MODE0;            /*!< Offset 0x000 Reg_000 */
    volatile uint32_t MCTL_COM_WORK_MODE1;            /*!< Offset 0x004 Reg_004 */
    volatile uint32_t MCTL_COM_DBGCR;                 /*!< Offset 0x008 Reg_008 */
    volatile uint32_t MCTL_COM_TMR;                   /*!< Offset 0x00C Reg_00C */
             uint32_t reserved_0x010;
    volatile uint32_t MCTL_COM_CCCR;                  /*!< Offset 0x014 Reg_014 */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t MCTL_COM_MAER0;                 /*!< Offset 0x020 Reg_020 */
    volatile uint32_t MCTL_COM_MAER1;                 /*!< Offset 0x024 Reg_024 */
    volatile uint32_t MCTL_COM_MAER2;                 /*!< Offset 0x028 Reg_028 */
             uint32_t reserved_0x02C [0x0135];
    volatile uint32_t MCTL_COM_REMAP0;                /*!< Offset 0x500 REMAP0 */
    volatile uint32_t MCTL_COM_REMAP1;                /*!< Offset 0x504 REMAP1 */
    volatile uint32_t MCTL_COM_REMAP2;                /*!< Offset 0x508 REMAP2 */
    volatile uint32_t MCTL_COM_REMAP3;                /*!< Offset 0x50C REMAP3 */
} MCTL_COM_TypeDef; /* size of structure = 0x510 */
/*
 * @brief MCTL_PHY
 */
/*!< MCTL_PHY  */
typedef struct MCTL_PHY_Type
{
    volatile uint32_t MCTL_PHY_PIR;                   /*!< Offset 0x000  */
    volatile uint32_t MCTL_PHY_PWRCTL;                /*!< Offset 0x004  */
    volatile uint32_t MCTL_PHY_MRCTRL0;               /*!< Offset 0x008  */
    volatile uint32_t MCTL_PHY_CLKEN;                 /*!< Offset 0x00C  */
    volatile uint32_t MCTL_PHY_PGSR0;                 /*!< Offset 0x010  */
    volatile uint32_t MCTL_PHY_PGSR1;                 /*!< Offset 0x014  */
    volatile uint32_t MCTL_PHY_STATR;                 /*!< Offset 0x018  */
             uint32_t reserved_0x01C [0x0004];
    volatile uint32_t MCTL_PHY_LP3MR11;               /*!< Offset 0x02C  */
    volatile uint32_t MCTL_PHY_DRAM_MR0;              /*!< Offset 0x030  */
    volatile uint32_t MCTL_PHY_DRAM_MR1;              /*!< Offset 0x034  */
    volatile uint32_t MCTL_PHY_DRAM_MR2;              /*!< Offset 0x038  */
    volatile uint32_t MCTL_PHY_DRAM_MR3;              /*!< Offset 0x03C  */
             uint32_t reserved_0x040;
    volatile uint32_t MCTL_PHY_PTR0;                  /*!< Offset 0x044  */
             uint32_t reserved_0x048;
    volatile uint32_t MCTL_PHY_PTR2;                  /*!< Offset 0x04C  */
    volatile uint32_t MCTL_PHY_PTR3;                  /*!< Offset 0x050  */
    volatile uint32_t MCTL_PHY_PTR4;                  /*!< Offset 0x054  */
    volatile uint32_t MCTL_PHY_DRAMTMG0;              /*!< Offset 0x058  */
    volatile uint32_t MCTL_PHY_DRAMTMG1;              /*!< Offset 0x05C  */
    volatile uint32_t MCTL_PHY_DRAMTMG2;              /*!< Offset 0x060  */
    volatile uint32_t MCTL_PHY_DRAMTMG3;              /*!< Offset 0x064  */
    volatile uint32_t MCTL_PHY_DRAMTMG4;              /*!< Offset 0x068  */
    volatile uint32_t MCTL_PHY_DRAMTMG5;              /*!< Offset 0x06C  */
    volatile uint32_t MCTL_PHY_DRAMTMG6;              /*!< Offset 0x070  */
    volatile uint32_t MCTL_PHY_DRAMTMG7;              /*!< Offset 0x074  */
    volatile uint32_t MCTL_PHY_DRAMTMG8;              /*!< Offset 0x078  */
    volatile uint32_t MCTL_PHY_ODTCFG;                /*!< Offset 0x07C  */
    volatile uint32_t MCTL_PHY_PITMG0;                /*!< Offset 0x080  */
    volatile uint32_t MCTL_PHY_PITMG1;                /*!< Offset 0x084  */
    volatile uint32_t MCTL_PHY_LPTPR;                 /*!< Offset 0x088  */
    volatile uint32_t MCTL_PHY_RFSHCTL0;              /*!< Offset 0x08C  */
    volatile uint32_t MCTL_PHY_RFSHTMG;               /*!< Offset 0x090  */
    volatile uint32_t MCTL_PHY_RFSHCTL1;              /*!< Offset 0x094  */
    volatile uint32_t MCTL_PHY_PWRTMG;                /*!< Offset 0x098  */
    volatile uint32_t MCTL_PHY_ASRC;                  /*!< Offset 0x09C  */
    volatile uint32_t MCTL_PHY_ASRTC;                 /*!< Offset 0x0A0  */
             uint32_t reserved_0x0A4 [0x0005];
    volatile uint32_t MCTL_PHY_VTFCR;                 /*!< Offset 0x0B8  */
    volatile uint32_t MCTL_PHY_DQSGMR;                /*!< Offset 0x0BC  */
    volatile uint32_t MCTL_PHY_DTCR;                  /*!< Offset 0x0C0  */
    volatile uint32_t MCTL_PHY_DTAR0;                 /*!< Offset 0x0C4  */
             uint32_t reserved_0x0C8 [0x000E];
    volatile uint32_t MCTL_PHY_PGCR0;                 /*!< Offset 0x100  */
    volatile uint32_t MCTL_PHY_PGCR1;                 /*!< Offset 0x104  */
    volatile uint32_t MCTL_PHY_PGCR2;                 /*!< Offset 0x108  */
    volatile uint32_t MCTL_PHY_PGCR3;                 /*!< Offset 0x10C  */
    volatile uint32_t MCTL_PHY_IOVCR0;                /*!< Offset 0x110  */
    volatile uint32_t MCTL_PHY_IOVCR1;                /*!< Offset 0x114  */
             uint32_t reserved_0x118;
    volatile uint32_t MCTL_PHY_DXCCR;                 /*!< Offset 0x11C  */
    volatile uint32_t MCTL_PHY_ODTMAP;                /*!< Offset 0x120  */
    volatile uint32_t MCTL_PHY_ZQCTL0;                /*!< Offset 0x124  */
    volatile uint32_t MCTL_PHY_ZQCTL1;                /*!< Offset 0x128  */
             uint32_t reserved_0x12C [0x0005];
    volatile uint32_t MCTL_PHY_ZQCR;                  /*!< Offset 0x140  */
    volatile uint32_t MCTL_PHY_ZQSR;                  /*!< Offset 0x144  */
    volatile uint32_t MCTL_PHY_ZQDR0;                 /*!< Offset 0x148  */
    volatile uint32_t MCTL_PHY_ZQDR1;                 /*!< Offset 0x14C  */
    volatile uint32_t MCTL_PHY_ZQDR2;                 /*!< Offset 0x150  */
             uint32_t reserved_0x154 [0x001B];
    volatile uint32_t MCTL_PHY_SCHED;                 /*!< Offset 0x1C0  */
    volatile uint32_t MCTL_PHY_PERFHPR0;              /*!< Offset 0x1C4  */
    volatile uint32_t MCTL_PHY_PERFHPR1;              /*!< Offset 0x1C8  */
    volatile uint32_t MCTL_PHY_PERFLPR0;              /*!< Offset 0x1CC  */
    volatile uint32_t MCTL_PHY_PERFLPR1;              /*!< Offset 0x1D0  */
    volatile uint32_t MCTL_PHY_PERFWR0;               /*!< Offset 0x1D4  */
    volatile uint32_t MCTL_PHY_PERFWR1;               /*!< Offset 0x1D8  */
             uint32_t reserved_0x1DC [0x0009];
    volatile uint32_t MCTL_PHY_ACMDLR;                /*!< Offset 0x200  */
    volatile uint32_t MCTL_PHY_ACLDLR;                /*!< Offset 0x204  */
    volatile uint32_t MCTL_PHY_ACIOCR0;               /*!< Offset 0x208  */
             uint32_t reserved_0x20C;
    volatile uint32_t MCTL_PHY_ACIOCR1 [0x010];       /*!< Offset 0x210 0x210 + 0x4 * x */
} MCTL_PHY_TypeDef; /* size of structure = 0x250 */
/*
 * @brief OWA
 */
/*!< OWA One Wire Audio (TX only) */
typedef struct OWA_Type
{
    volatile uint32_t OWA_GEN_CTL;                    /*!< Offset 0x000 OWA General Control Register */
    volatile uint32_t OWA_TX_CFIG;                    /*!< Offset 0x004 OWA TX Configuration Register */
             uint32_t reserved_0x008;
    volatile uint32_t OWA_ISTA;                       /*!< Offset 0x00C OWA Interrupt Status Register */
             uint32_t reserved_0x010;
    volatile uint32_t OWA_FCTL;                       /*!< Offset 0x014 OWA FIFO Control Register */
    volatile uint32_t OWA_FSTA;                       /*!< Offset 0x018 OWA FIFO Status Register */
    volatile uint32_t OWA_INT;                        /*!< Offset 0x01C OWA Interrupt Control Register */
    volatile uint32_t OWA_TX_FIFO;                    /*!< Offset 0x020 OWA TX FIFO Register */
    volatile uint32_t OWA_TX_CNT;                     /*!< Offset 0x024 OWA TX Counter Register */
             uint32_t reserved_0x028;
    volatile uint32_t OWA_TX_CHSTA0;                  /*!< Offset 0x02C OWA TX Channel Status Register0 */
    volatile uint32_t OWA_TX_CHSTA1;                  /*!< Offset 0x030 OWA TX Channel Status Register1 */
} OWA_TypeDef; /* size of structure = 0x034 */
/*
 * @brief PLIC
 */
/*!< PLIC Platform-Level Interrupt Controller (PLIC) */
typedef struct PLIC_Type
{
    volatile uint32_t PLIC_PRIO_REGn [0x400];         /*!< Offset 0x000 (0<n<256) PLIC Priority Register n - Interrupt Source Priority */
    volatile uint32_t PLIC_IP_REGn [0x020];           /*!< Offset 0x1000 (0≤n<9) PLIC Interrupt Pending Register n - Interrupt Pending Bits */
             uint32_t reserved_0x1080 [0x03E0];
    volatile uint32_t PLIC_MIE_REGn [0x020];          /*!< Offset 0x2000 (0≤n<9) PLIC Machine Mode Interrupt Enable Register n - Machine Interrupt Enable Bits */
    volatile uint32_t PLIC_SIE_REGn [0x020];          /*!< Offset 0x2080 (0≤n<9) PLIC Superuser Mode Interrupt Enable Register n - Superuser Interrupt Enable Bits */
             uint32_t reserved_0x2100 [0x7F7BF];
    volatile uint32_t PLIC_CTRL_REG;                  /*!< Offset 0x1FFFFC PLIC Control Register */
    volatile uint32_t PLIC_MTH_REG;                   /*!< Offset 0x200000 PLIC Machine Threshold Register - Priority Threshold  for context 0 */
    volatile uint32_t PLIC_MCLAIM_REG;                /*!< Offset 0x200004 PLIC Machine Claim Register (EOI) - Interrupt Claim Process for context 0 */
             uint32_t reserved_0x200008 [0x03FE];
    volatile uint32_t PLIC_STH_REG;                   /*!< Offset 0x201000 PLIC Superuser Threshold Register - Priority Threshold  for context 1 */
    volatile uint32_t PLIC_SCLAIM_REG;                /*!< Offset 0x201004 PLIC Superuser Claim Register (EOI) - Interrupt Claim Process for context 1 */
} PLIC_TypeDef; /* size of structure = 0x201008 */
/*
 * @brief PWM
 */
/*!< PWM Pulse Width Modulation module */
typedef struct PWM_Type
{
    volatile uint32_t PIER;                           /*!< Offset 0x000 PWM IRQ Enable Register */
    volatile uint32_t PISR;                           /*!< Offset 0x004 PWM IRQ Status Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t CIER;                           /*!< Offset 0x010 Capture IRQ Enable Register */
    volatile uint32_t CISR;                           /*!< Offset 0x014 Capture IRQ Status Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t PCCR [0x004];                   /*!< Offset 0x020 PWM01, PWM23, PWM45, PWM67 Clock Configuration Register */
             uint32_t reserved_0x030 [0x0004];
    volatile uint32_t PCGR;                           /*!< Offset 0x040 PWM Clock Gating Register */
             uint32_t reserved_0x044 [0x0007];
    volatile uint32_t PDZCR [0x004];                  /*!< Offset 0x060 PWM01, PWM23, PWM45, PWM67 Dead Zone Control Register */
             uint32_t reserved_0x070 [0x0004];
    volatile uint32_t PER;                            /*!< Offset 0x080 PWM Enable Register */
             uint32_t reserved_0x084 [0x0003];
    volatile uint32_t PGR0 [0x004];                   /*!< Offset 0x090 PWM Group0, Group1, Group2, Group3 Register */
             uint32_t reserved_0x0A0 [0x0008];
    volatile uint32_t CER;                            /*!< Offset 0x0C0 Capture Enable Register */
             uint32_t reserved_0x0C4 [0x000F];
    struct
    {
        volatile uint32_t PCR;                        /*!< Offset 0x100 PWM Control Register */
        volatile uint32_t PPR;                        /*!< Offset 0x104 PWM Period Register */
        volatile uint32_t PCNTR;                      /*!< Offset 0x108 PWM Count Register */
        volatile uint32_t PPCNTR;                     /*!< Offset 0x10C PWM Pulse Count Register */
        volatile uint32_t CCR;                        /*!< Offset 0x110 Capture Control Register */
        volatile uint32_t CRLR;                       /*!< Offset 0x114 Capture Rise Lock Register */
        volatile uint32_t CFLR;                       /*!< Offset 0x118 Capture Fall Lock Register */
                 uint32_t reserved_0x01C;
    } CH [0x008];                                     /*!< Offset 0x100 Channels[0..7] */
} PWM_TypeDef; /* size of structure = 0x200 */
/*
 * @brief RISC_CFG
 */
/*!< RISC_CFG RISC-V core configuration register */
typedef struct RISC_CFG_Type
{
             uint32_t reserved_0x000;
    volatile uint32_t RISC_STA_ADD0_REG;              /*!< Offset 0x004 RISC Start Address0 Register */
    volatile uint32_t RISC_STA_ADD1_REG;              /*!< Offset 0x008 RISC Start Address1 Register */
             uint32_t reserved_0x00C;
    volatile uint32_t RF1P_CFG_REG;                   /*!< Offset 0x010 RF1P Configuration Register */
             uint32_t reserved_0x014 [0x0002];
    volatile uint32_t ROM_CFG_REG;                    /*!< Offset 0x01C ROM Configuration Register */
    volatile uint32_t WAKEUP_EN_REG;                  /*!< Offset 0x020 Wakeup Enable Register */
    volatile uint32_t WAKEUP_MASK_REG [0x005];        /*!< Offset 0x024 Wakeup Mask0..Mask4 Register */
             uint32_t reserved_0x038 [0x0002];
    volatile uint32_t TS_TMODE_SEL_REG;               /*!< Offset 0x040 Timestamp Test Mode Select Register */
    volatile uint32_t SRAM_ADDR_TWIST_REG;            /*!< Offset 0x044 SRAM Address Twist Register */
    volatile uint32_t WORK_MODE_REG;                  /*!< Offset 0x048 Work Mode Register */
             uint32_t reserved_0x04C;
    volatile uint32_t RETITE_PC0_REG;                 /*!< Offset 0x050 Retire PC0 Register */
    volatile uint32_t RETITE_PC1_REG;                 /*!< Offset 0x054 Retire PC1 Register */
             uint32_t reserved_0x058 [0x0002];
    volatile uint32_t IRQ_MODE_REG [0x005];           /*!< Offset 0x060 IRQ Mode0..Mode4 Register */
             uint32_t reserved_0x074 [0x0024];
    volatile uint32_t RISC_AXI_PMU_CTRL;              /*!< Offset 0x104 RISC AXI PMU Control Register */
    volatile uint32_t RISC_AXI_PMU_PRD;               /*!< Offset 0x108 RISC AXI PMU Period Register */
    volatile uint32_t RISC_AXI_PMU_LAT_RD;            /*!< Offset 0x10C RISC AXI PMU Read Latency Register */
    volatile uint32_t RISC_AXI_PMU_LAT_WR;            /*!< Offset 0x110 RISC AXI PMU Write Latency Register */
    volatile uint32_t RISC_AXI_PMU_REQ_RD;            /*!< Offset 0x114 RISC AXI PMU Read Request Register */
    volatile uint32_t RISC_AXI_PMU_REQ_WR;            /*!< Offset 0x118 RISC AXI PMU Write Request Register */
    volatile uint32_t RISC_AXI_PMU_BW_RD;             /*!< Offset 0x11C RISC AXI PMU Read Bandwidth Register */
    volatile uint32_t RISC_AXI_PMU_BW_WR;             /*!< Offset 0x120 RISC AXI PMU Write Bandwidth Register */
} RISC_CFG_TypeDef; /* size of structure = 0x124 */
/*
 * @brief RTC
 */
/*!< RTC Real Time Clock */
typedef struct RTC_Type
{
    volatile uint32_t LOSC_CTRL_REG;                  /*!< Offset 0x000 Low Oscillator Control Register */
    volatile uint32_t LOSC_AUTO_SWT_STA_REG;          /*!< Offset 0x004 Low Oscillator Auto Switch Status Register */
    volatile uint32_t INTOSC_CLK_PRESCAL_REG;         /*!< Offset 0x008 Internal OSC Clock Pre-scaler Register */
             uint32_t reserved_0x00C;
    volatile uint32_t RTC_DAY_REG;                    /*!< Offset 0x010 RTC Year-Month-Day Register */
    volatile uint32_t RTC_HH_MM_SS_REG;               /*!< Offset 0x014 RTC Hour-Minute-Second Register */
             uint32_t reserved_0x018 [0x0002];
    volatile uint32_t ALARM0_DAY_SET_REG;             /*!< Offset 0x020 Alarm 0 Day Setting Register */
    volatile uint32_t ALARM0_CUR_VLU_REG;             /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    volatile uint32_t ALARM0_ENABLE_REG;              /*!< Offset 0x028 Alarm 0 Enable Register */
    volatile uint32_t ALARM0_IRQ_EN;                  /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    volatile uint32_t ALARM0_IRQ_STA_REG;             /*!< Offset 0x030 Alarm 0 IRQ Status Register */
             uint32_t reserved_0x034 [0x0007];
    volatile uint32_t ALARM_CONFIG_REG;               /*!< Offset 0x050 Alarm Configuration Register */
             uint32_t reserved_0x054 [0x0003];
    volatile uint32_t F32K_FOUT_CTRL_GATING_REG;      /*!< Offset 0x060 32K Fanout Control Gating Register */
             uint32_t reserved_0x064 [0x0027];
    volatile uint32_t GP_DATA_REG [0x008];            /*!< Offset 0x100 General Purpose Register (N=0 to 7) */
    volatile uint32_t FBOOT_INFO_REG0;                /*!< Offset 0x120 Fast Boot Information Register0 */
    volatile uint32_t FBOOT_INFO_REG1;                /*!< Offset 0x124 Fast Boot Information Register1 */
             uint32_t reserved_0x128 [0x000E];
    volatile uint32_t DCXO_CTRL_REG;                  /*!< Offset 0x160 DCXO Control Register */
             uint32_t reserved_0x164 [0x000B];
    volatile uint32_t RTC_VIO_REG;                    /*!< Offset 0x190 RTC_VIO Regulation Register */
             uint32_t reserved_0x194 [0x0017];
    volatile uint32_t IC_CHARA_REG;                   /*!< Offset 0x1F0 IC Characteristic Register */
    volatile uint32_t VDD_OFF_GATING_CTRL_REG;        /*!< Offset 0x1F4 VDD Off Gating Control Register */
             uint32_t reserved_0x1F8 [0x0003];
    volatile uint32_t EFUSE_HV_PWRSWT_CTRL_REG;       /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
             uint32_t reserved_0x208 [0x0042];
    volatile uint32_t RTC_SPI_CLK_CTRL_REG;           /*!< Offset 0x310 RTC SPI Clock Control Register */
} RTC_TypeDef; /* size of structure = 0x314 */
/*
 * @brief R_CPUCFG
 */
/*!< R_CPUCFG  */
typedef struct R_CPUCFG_Type
{
    volatile uint32_t REGxx;                          /*!< Offset 0x000 Bit 31 and bit 0 R/W, bit 0 can be related to RISC-C vore */
             uint32_t reserved_0x004 [0x006F];
    volatile uint32_t HOTPLUGFLAG;                    /*!< Offset 0x1C0 The Hotplug Flag Register is 0x070005C0. */
    volatile uint32_t SOFTENTRY [0x004];              /*!< Offset 0x1C4 The Soft Entry Address Register of CPUx (x=0..1) */
    volatile uint32_t SUP_STAN_FLAG;                  /*!< Offset 0x1D4 Super Standby Flag (bit 16) */
             uint32_t reserved_0x1D8;
    volatile uint32_t RV_HOTPLUGFLAG;                 /*!< Offset 0x1DC RV The Hotplug Flag Register (key value 0xFA50392F) */
    volatile uint32_t RC_SOFTENTRY [0x004];           /*!< Offset 0x1E0 RV The Soft Entry Address Register */
} R_CPUCFG_TypeDef; /* size of structure = 0x1F0 */
/*
 * @brief R_PRCM
 */
/*!< R_PRCM  */
typedef struct R_PRCM_Type
{
             uint32_t reserved_0x000 [0x0094];
    volatile uint32_t VDD_SYS_PWROFF_GATING_REG;      /*!< Offset 0x250  */
    volatile uint32_t ANALOG_PWROFF_GATING_REG;       /*!< Offset 0x254  */
} R_PRCM_TypeDef; /* size of structure = 0x258 */
/*
 * @brief SID
 */
/*!< SID Security ID */
typedef struct SID_Type
{
    volatile uint32_t SID_RKEY0;                      /*!< Offset 0x000 Securiy root key[31:0] */
    volatile uint32_t SID_RKEY1;                      /*!< Offset 0x004 Securiy root key[63:32] */
    volatile uint32_t SID_RKEY2;                      /*!< Offset 0x008 Securiy root key[95:64] */
    volatile uint32_t SID_RKEY3;                      /*!< Offset 0x00C Securiy root key[127:96] */
             uint32_t reserved_0x010;
    volatile uint32_t SID_THS;                        /*!< Offset 0x014 [27:16]: The calibration value of the T-sensor. */
             uint32_t reserved_0x018 [0x007A];
    volatile uint32_t SID_DATA [0x004];               /*!< Offset 0x200 SID data (xfel display as 'sid' replay) */
    volatile uint32_t BOOT_MODE;                      /*!< Offset 0x210 [27:16]: eFUSE boot select status, [0]: 0: GPIO boot select, 1: eFuse boot select */
    volatile uint32_t SID_UNDOC [0x1FB];              /*!< Offset 0x214  */
} SID_TypeDef; /* size of structure = 0xA00 */
/*
 * @brief SMHC
 */
/*!< SMHC SD-MMC Host Controller */
typedef struct SMHC_Type
{
    volatile uint32_t SMHC_CTRL;                      /*!< Offset 0x000 Control Register */
    volatile uint32_t SMHC_CLKDIV;                    /*!< Offset 0x004 Clock Control Register */
    volatile uint32_t SMHC_TMOUT;                     /*!< Offset 0x008 Time Out Register */
    volatile uint32_t SMHC_CTYPE;                     /*!< Offset 0x00C Bus Width Register */
    volatile uint32_t SMHC_BLKSIZ;                    /*!< Offset 0x010 Block Size Register */
    volatile uint32_t SMHC_BYTCNT;                    /*!< Offset 0x014 Byte Count Register */
    volatile uint32_t SMHC_CMD;                       /*!< Offset 0x018 Command Register */
    volatile uint32_t SMHC_CMDARG;                    /*!< Offset 0x01C Command Argument Register */
    volatile uint32_t SMHC_RESP0;                     /*!< Offset 0x020 Response 0 Register */
    volatile uint32_t SMHC_RESP1;                     /*!< Offset 0x024 Response 1 Register */
    volatile uint32_t SMHC_RESP2;                     /*!< Offset 0x028 Response 2 Register */
    volatile uint32_t SMHC_RESP3;                     /*!< Offset 0x02C Response 3 Register */
    volatile uint32_t SMHC_INTMASK;                   /*!< Offset 0x030 Interrupt Mask Register */
    volatile uint32_t SMHC_MINTSTS;                   /*!< Offset 0x034 Masked Interrupt Status Register */
    volatile uint32_t SMHC_RINTSTS;                   /*!< Offset 0x038 Raw Interrupt Status Register */
    volatile uint32_t SMHC_STATUS;                    /*!< Offset 0x03C Status Register */
    volatile uint32_t SMHC_FIFOTH;                    /*!< Offset 0x040 FIFO Water Level Register */
    volatile uint32_t SMHC_FUNS;                      /*!< Offset 0x044 FIFO Function Select Register */
    volatile uint32_t SMHC_TCBCNT;                    /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
    volatile uint32_t SMHC_TBBCNT;                    /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
    volatile uint32_t SMHC_DBGC;                      /*!< Offset 0x050 Current Debug Control Register */
    volatile uint32_t SMHC_CSDC;                      /*!< Offset 0x054 CRC Status Detect Control Registers */
    volatile uint32_t SMHC_A12A;                      /*!< Offset 0x058 Auto Command 12 Argument Register */
    volatile uint32_t SMHC_NTSR;                      /*!< Offset 0x05C SD New Timing Set Register */
             uint32_t reserved_0x060 [0x0006];
    volatile uint32_t SMHC_HWRST;                     /*!< Offset 0x078 Hardware Reset Register */
             uint32_t reserved_0x07C;
    volatile uint32_t SMHC_IDMAC;                     /*!< Offset 0x080 IDMAC Control Register */
    volatile uint32_t SMHC_DLBA;                      /*!< Offset 0x084 Descriptor List Base Address Register */
    volatile uint32_t SMHC_IDST;                      /*!< Offset 0x088 IDMAC Status Register */
    volatile uint32_t SMHC_IDIE;                      /*!< Offset 0x08C IDMAC Interrupt Enable Register */
             uint32_t reserved_0x090 [0x001C];
    volatile uint32_t SMHC_THLD;                      /*!< Offset 0x100 Card Threshold Control Register */
    volatile uint32_t SMHC_SFC;                       /*!< Offset 0x104 Sample FIFO Control Register */
    volatile uint32_t SMHC_A23A;                      /*!< Offset 0x108 Auto Command 23 Argument Register */
    volatile uint32_t EMMC_DDR_SBIT_DET;              /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
             uint32_t reserved_0x110 [0x000A];
    volatile uint32_t SMHC_EXT_CMD;                   /*!< Offset 0x138 Extended Command Register */
    volatile uint32_t SMHC_EXT_RESP;                  /*!< Offset 0x13C Extended Response Register */
    volatile uint32_t SMHC_DRV_DL;                    /*!< Offset 0x140 Drive Delay Control Register */
    volatile uint32_t SMHC_SAMP_DL;                   /*!< Offset 0x144 Sample Delay Control Register */
    volatile uint32_t SMHC_DS_DL;                     /*!< Offset 0x148 Data Strobe Delay Control Register */
    volatile uint32_t SMHC_HS400_DL;                  /*!< Offset 0x14C HS400 Delay Control Register */
             uint32_t reserved_0x150 [0x002C];
    volatile uint32_t SMHC_FIFO;                      /*!< Offset 0x200 Read/Write FIFO */
             uint32_t reserved_0x204 [0x037F];
} SMHC_TypeDef; /* size of structure = 0x1000 */
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
             uint32_t reserved_0x024;
    volatile uint32_t SPI_SAMP_DL;                    /*!< Offset 0x028 SPI Sample Delay Control Register */
             uint32_t reserved_0x02C;
    volatile uint32_t SPI_MBC;                        /*!< Offset 0x030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                        /*!< Offset 0x034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                        /*!< Offset 0x038 SPI Master Burst Control Register */
             uint32_t reserved_0x03C;
    volatile uint32_t SPI_BATCR;                      /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_BA_CCR;                     /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                        /*!< Offset 0x048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                        /*!< Offset 0x04C SPI RX Bit Register */
             uint32_t reserved_0x050 [0x000E];
    volatile uint32_t SPI_NDMA_MODE_CTL;              /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
             uint32_t reserved_0x08C [0x001D];
    volatile uint32_t DBI_CTL_0;                      /*!< Offset 0x100 DBI Control Register 0 */
    volatile uint32_t DBI_CTL_1;                      /*!< Offset 0x104 DBI Control Register 1 */
    volatile uint32_t DBI_CTL_2;                      /*!< Offset 0x108 DBI Control Register 2 */
    volatile uint32_t DBI_TIMER;                      /*!< Offset 0x10C DBI Timer Control Register */
    volatile uint32_t DBI_VIDEO_SZIE;                 /*!< Offset 0x110 DBI Video Size Configuration Register */
             uint32_t reserved_0x114 [0x0003];
    volatile uint32_t DBI_INT;                        /*!< Offset 0x120 DBI Interrupt Register */
    volatile uint32_t DBI_DEBUG_0;                    /*!< Offset 0x124 DBI BEBUG 0 Register */
    volatile uint32_t DBI_DEBUG_1;                    /*!< Offset 0x128 DBI BEBUG 1 Register */
             uint32_t reserved_0x12C [0x0035];
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
             uint32_t reserved_0x000 [0x0002];
    volatile uint32_t DSP_BOOT_RAMMAP_REG;            /*!< Offset 0x008 DSP Boot SRAM Remap Control Register */
             uint32_t reserved_0x00C [0x0006];
    volatile uint32_t VER_REG;                        /*!< Offset 0x024 Version Register */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t EMAC_EPHY_CLK_REG0;             /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
             uint32_t reserved_0x034 [0x0047];
    volatile uint32_t SYS_LDO_CTRL_REG;               /*!< Offset 0x150 System LDO Control Register */
             uint32_t reserved_0x154 [0x0003];
    volatile uint32_t RESCAL_CTRL_REG;                /*!< Offset 0x160 Resistor Calibration Control Register */
             uint32_t reserved_0x164;
    volatile uint32_t RES240_CTRL_REG;                /*!< Offset 0x168 240ohms Resistor Manual Control Register */
    volatile uint32_t RESCAL_STATUS_REG;              /*!< Offset 0x16C Resistor Calibration Status Register */
} SYS_CFG_TypeDef; /* size of structure = 0x170 */
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
typedef struct TCON_LCD_Type
{
    volatile uint32_t LCD_GCTL_REG;                   /*!< Offset 0x000 LCD Global Control Register */
    volatile uint32_t LCD_GINT0_REG;                  /*!< Offset 0x004 LCD Global Interrupt Register0 */
    volatile uint32_t LCD_GINT1_REG;                  /*!< Offset 0x008 LCD Global Interrupt Register1 */
             uint32_t reserved_0x00C;
    volatile uint32_t LCD_FRM_CTL_REG;                /*!< Offset 0x010 LCD FRM Control Register */
    volatile uint32_t LCD_FRM_SEED_REG [0x006];       /*!< Offset 0x014 LCD FRM Seed Register (N=0,1,2,3,4,5) */
    volatile uint32_t LCD_FRM_TAB_REG [0x004];        /*!< Offset 0x02C LCD FRM Table Register (N=0,1,2,3) */
    volatile uint32_t LCD_3D_FIFO_REG;                /*!< Offset 0x03C LCD 3D FIFO Register */
    volatile uint32_t LCD_CTL_REG;                    /*!< Offset 0x040 LCD Control Register */
    volatile uint32_t LCD_DCLK_REG;                   /*!< Offset 0x044 LCD Data Clock Register */
    volatile uint32_t LCD_BASIC0_REG;                 /*!< Offset 0x048 LCD Basic Timing Register0 */
    volatile uint32_t LCD_BASIC1_REG;                 /*!< Offset 0x04C LCD Basic Timing Register1 */
    volatile uint32_t LCD_BASIC2_REG;                 /*!< Offset 0x050 LCD Basic Timing Register2 */
    volatile uint32_t LCD_BASIC3_REG;                 /*!< Offset 0x054 LCD Basic Timing Register3 */
    volatile uint32_t LCD_HV_IF_REG;                  /*!< Offset 0x058 LCD HV Panel Interface Register */
             uint32_t reserved_0x05C;
    volatile uint32_t LCD_CPU_IF_REG;                 /*!< Offset 0x060 LCD CPU Panel Interface Register */
    volatile uint32_t LCD_CPU_WR_REG;                 /*!< Offset 0x064 LCD CPU Panel Write Data Register */
    volatile uint32_t LCD_CPU_RD0_REG;                /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
    volatile uint32_t LCD_CPU_RD1_REG;                /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
             uint32_t reserved_0x070 [0x0005];
    volatile uint32_t LCD_LVDS_IF_REG;                /*!< Offset 0x084 LCD LVDS Configure Register */
    volatile uint32_t LCD_IO_POL_REG;                 /*!< Offset 0x088 LCD IO Polarity Register */
    volatile uint32_t LCD_IO_TRI_REG;                 /*!< Offset 0x08C LCD IO Control Register */
             uint32_t reserved_0x090 [0x001B];
    volatile uint32_t LCD_DEBUG_REG;                  /*!< Offset 0x0FC LCD Debug Register */
    volatile uint32_t LCD_CEU_CTL_REG;                /*!< Offset 0x100 LCD CEU Control Register */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t LCD_CEU_COEF_MUL_REG [0x003];   /*!< Offset 0x110 LCD CEU Coefficient Register0 0x0110+N*0x04 (N=0..10) N=0: Rr, N=1: Rg, N=2: Rb, N=4: Gr, N=5: Gg, N=6: Gb, N=8: Br, N=9: Bg, N=10: Bb */
    volatile uint32_t LCD_CEU_COEF_ADD0_REG;          /*!< Offset 0x11C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) N=0: Rc, N=1: Gc, N=2: Bc */
             uint32_t reserved_0x120 [0x0003];
    volatile uint32_t LCD_CEU_COEF_ADD1_REG;          /*!< Offset 0x12C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
             uint32_t reserved_0x130 [0x0003];
    volatile uint32_t LCD_CEU_COEF_ADD2_REG;          /*!< Offset 0x13C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
    volatile uint32_t LCD_CEU_COEF_RANG_REG [0x003];  /*!< Offset 0x140 LCD CEU Coefficient Register2 0x0140+N*0x04 (N=0,1,2) */
             uint32_t reserved_0x14C [0x0005];
    volatile uint32_t LCD_CPU_TRIx_REG [0x006];       /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
             uint32_t reserved_0x178 [0x0002];
    volatile uint32_t LCD_CMAP_CTL_REG;               /*!< Offset 0x180 LCD Color Map Control Register */
             uint32_t reserved_0x184 [0x0003];
    volatile uint32_t LCD_CMAP_ODD0_REG;              /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
    volatile uint32_t LCD_CMAP_ODD1_REG;              /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
    volatile uint32_t LCD_CMAP_EVEN0_REG;             /*!< Offset 0x198 LCD Color Map Even Line Register0 */
    volatile uint32_t LCD_CMAP_EVEN1_REG;             /*!< Offset 0x19C LCD Color Map Even Line Register1 */
             uint32_t reserved_0x1A0 [0x0014];
    volatile uint32_t LCD_SAFE_PERIOD_REG;            /*!< Offset 0x1F0 LCD Safe Period Register */
             uint32_t reserved_0x1F4 [0x000B];
    volatile uint32_t LCD_LVDS_ANA_REG [0x002];       /*!< Offset 0x220 LCD LVDS Analog Register N */
             uint32_t reserved_0x228 [0x0002];
    volatile uint32_t LCD_SYNC_CTL_REG;               /*!< Offset 0x230 LCD Sync Control Register */
    volatile uint32_t LCD_SYNC_POS_REG;               /*!< Offset 0x234 LCD Sync Position Register */
    volatile uint32_t LCD_SLAVE_STOP_POS_REG;         /*!< Offset 0x238 LCD Slave Stop Position Register */
             uint32_t reserved_0x23C [0x0071];
    volatile uint32_t LCD_GAMMA_TABLE_REG [0x100];    /*!< Offset 0x400 LCD Gamma Table Register */
             uint32_t reserved_0x800 [0x0200];
} TCON_LCD_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TCON_TV
 */
/*!< TCON_TV  */
typedef struct TCON_TV_Type
{
    volatile uint32_t TV_GCTL_REG;                    /*!< Offset 0x000 TV Global Control Register */
    volatile uint32_t TV_GINT0_REG;                   /*!< Offset 0x004 TV Global Interrupt Register0 */
    volatile uint32_t TV_GINT1_REG;                   /*!< Offset 0x008 TV Global Interrupt Register1 */
             uint32_t reserved_0x00C [0x000D];
    volatile uint32_t TV_SRC_CTL_REG;                 /*!< Offset 0x040 TV Source Control Register */
             uint32_t reserved_0x044 [0x0013];
    volatile uint32_t TV_CTL_REG;                     /*!< Offset 0x090 TV Control Register */
    volatile uint32_t TV_BASIC0_REG;                  /*!< Offset 0x094 TV Basic Timing Register0 */
    volatile uint32_t TV_BASIC1_REG;                  /*!< Offset 0x098 TV Basic Timing Register1 */
    volatile uint32_t TV_BASIC2_REG;                  /*!< Offset 0x09C TV Basic Timing Register2 */
    volatile uint32_t TV_BASIC3_REG;                  /*!< Offset 0x0A0 TV Basic Timing Register3 */
    volatile uint32_t TV_BASIC4_REG;                  /*!< Offset 0x0A4 TV Basic Timing Register4 */
    volatile uint32_t TV_BASIC5_REG;                  /*!< Offset 0x0A8 TV Basic Timing Register5 */
             uint32_t reserved_0x0AC [0x0003];
    volatile uint32_t TV_IO_POL_REG;                  /*!< Offset 0x0B8 TV SYNC Signal Polarity Register was: 0x0088 */
    volatile uint32_t TV_IO_TRI_REG;                  /*!< Offset 0x0BC TV SYNC Signal IO Control Register was: 0x008C */
             uint32_t reserved_0x0C0 [0x000F];
    volatile uint32_t TV_DEBUG_REG;                   /*!< Offset 0x0FC TV Debug Register */
    volatile uint32_t TV_CEU_CTL_REG;                 /*!< Offset 0x100 TV CEU Control Register */
             uint32_t reserved_0x104 [0x0003];
    volatile uint32_t TV_CEU_COEF_MUL_REG [0x00B];    /*!< Offset 0x110 TV CEU Coefficient Register0 0x0110+N*0x04(N=0-10) */
             uint32_t reserved_0x13C;
    volatile uint32_t TV_CEU_COEF_RANG_REG [0x003];   /*!< Offset 0x140 TV CEU Coefficient Register2 0x0140+N*0x04(N=0-2) */
             uint32_t reserved_0x14C [0x0029];
    volatile uint32_t TV_SAFE_PERIOD_REG;             /*!< Offset 0x1F0 TV Safe Period Register */
             uint32_t reserved_0x1F4 [0x0043];
    volatile uint32_t TV_FILL_CTL_REG;                /*!< Offset 0x300 TV Fill Data Control Register */
    struct
    {
        volatile uint32_t TV_FILL_BEGIN_REG;          /*!< Offset 0x304 TV Fill Data Begin Register 0x0304+N*0x0C(N=0–2) */
        volatile uint32_t TV_FILL_END_REG;            /*!< Offset 0x308 TV Fill Data End Register 0x0308+N*0x0C(N=0–2) */
        volatile uint32_t TV_FILL_DATA_REG;           /*!< Offset 0x30C TV Fill Data Value Register 0x030C+N*0x0C(N=0–2) */
    } TV_FILL [0x003];                                /*!< Offset 0x304 TV Fill Channel [0..2] */
             uint32_t reserved_0x328 [0x0002];
    volatile uint32_t TV_DATA_IO_POL0_REG;            /*!< Offset 0x330 TCON Data IO Polarity Control0 */
    volatile uint32_t TV_DATA_IO_POL1_REG;            /*!< Offset 0x334 TCON Data IO Polarity Control1 */
    volatile uint32_t TV_DATA_IO_TRI0_REG;            /*!< Offset 0x338 TCON Data IO Enable Control0 */
    volatile uint32_t TV_DATA_IO_TRI1_REG;            /*!< Offset 0x33C TCON Data IO Enable Control1 */
    volatile uint32_t TV_PIXELDEPTH_MODE_REG;         /*!< Offset 0x340 TV Pixeldepth Mode Control Register */
             uint32_t reserved_0x344 [0x032F];
} TCON_TV_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief THS
 */
/*!< THS Thermal Sensor */
typedef struct THS_Type
{
    volatile uint32_t THS_CTRL;                       /*!< Offset 0x000 THS Control Register */
    volatile uint32_t THS_EN;                         /*!< Offset 0x004 THS Enable Register */
    volatile uint32_t THS_PER;                        /*!< Offset 0x008 THS Period Control Register */
             uint32_t reserved_0x00C;
    volatile uint32_t THS_DATA_INTC;                  /*!< Offset 0x010 THS Data Interrupt Control Register */
    volatile uint32_t THS_SHUT_INTC;                  /*!< Offset 0x014 THS Shut Interrupt Control Register */
    volatile uint32_t THS_ALARM_INTC;                 /*!< Offset 0x018 THS Alarm Interrupt Control Register */
             uint32_t reserved_0x01C;
    volatile uint32_t THS_DATA_INTS;                  /*!< Offset 0x020 THS Data Interrupt Status Register */
    volatile uint32_t THS_SHUT_INTS;                  /*!< Offset 0x024 THS Shut Interrupt Status Register */
    volatile uint32_t THS_ALARMO_INTS;                /*!< Offset 0x028 THS Alarm off Interrupt Status Register */
    volatile uint32_t THS_ALARM_INTS;                 /*!< Offset 0x02C THS Alarm Interrupt Status Register */
    volatile uint32_t THS_FILTER;                     /*!< Offset 0x030 THS Median Filter Control Register */
             uint32_t reserved_0x034 [0x0003];
    volatile uint32_t THS_ALARM_CTRL;                 /*!< Offset 0x040 THS Alarm Threshold Control Register */
             uint32_t reserved_0x044 [0x000F];
    volatile uint32_t THS_SHUTDOWN_CTRL;              /*!< Offset 0x080 THS Shutdown Threshold Control Register */
             uint32_t reserved_0x084 [0x0007];
    volatile uint32_t THS_CDATA;                      /*!< Offset 0x0A0 THS Calibration Data */
             uint32_t reserved_0x0A4 [0x0007];
    volatile uint32_t THS_DATA;                       /*!< Offset 0x0C0 THS Data Register */
} THS_TypeDef; /* size of structure = 0x0C4 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    volatile uint32_t TMR_IRQ_EN_REG;                 /*!< Offset 0x000 Timer IRQ Enable Register */
    volatile uint32_t TMR_IRQ_STA_REG;                /*!< Offset 0x004 Timer Status Register */
             uint32_t reserved_0x008 [0x0002];
    volatile uint32_t TMR0_CTRL_REG;                  /*!< Offset 0x010 Timer0 Control Register */
    volatile uint32_t TMR0_INTV_VALUE_REG;            /*!< Offset 0x014 Timer0 Interval Value Register */
    volatile uint32_t TMR0_CUR_VALUE_REG;             /*!< Offset 0x018 Timer0 Current Value Register */
             uint32_t reserved_0x01C;
    volatile uint32_t TMR1_CTRL_REG;                  /*!< Offset 0x020 Timer1 Control Register */
    volatile uint32_t TMR1_INTV_VALUE_REG;            /*!< Offset 0x024 Timer1 Interval Value Register */
    volatile uint32_t TMR1_CUR_VALUE_REG;             /*!< Offset 0x028 Timer1 Current Value Register */
             uint32_t reserved_0x02C [0x001D];
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
} TIMER_TypeDef; /* size of structure = 0x0D0 */
/*
 * @brief TPADC
 */
/*!< TPADC  */
typedef struct TPADC_Type
{
    volatile uint32_t TP_CTRL_REG0;                   /*!< Offset 0x000 TP Control Register 0 */
    volatile uint32_t TP_CTRL_REG1;                   /*!< Offset 0x004 TP Control Register 1 */
    volatile uint32_t TP_CTRL_REG2;                   /*!< Offset 0x008 TP Control Register 2 */
    volatile uint32_t TP_CTRL_REG3;                   /*!< Offset 0x00C TP Control Register 3 */
    volatile uint32_t TP_INT_FIFO_CTRL_REG;           /*!< Offset 0x010 TP Interrupt FIFO Control Register */
    volatile uint32_t TP_INT_FIFO_STAT_REG;           /*!< Offset 0x014 TP Interrupt FIFO Status Register */
             uint32_t reserved_0x018;
    volatile uint32_t TP_CALI_DATA_REG;               /*!< Offset 0x01C TP Calibration Data Register */
             uint32_t reserved_0x020;
    volatile uint32_t TP_DATA_REG;                    /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef; /* size of structure = 0x028 */
/*
 * @brief TVD0
 */
/*!< TVD0 Video Decoding */
typedef struct TVD0_Type
{
    volatile uint32_t TVD_EN;                         /*!< Offset 0x000 TVD MODULE CONTROL Register */
    volatile uint32_t TVD_MODE;                       /*!< Offset 0x004 TVD MODE CONTROL Register */
    volatile uint32_t TVD_CLAMP_AGC1;                 /*!< Offset 0x008 TVD CLAMP & AGC CONTROL Register1 */
    volatile uint32_t TVD_CLAMP_AGC2;                 /*!< Offset 0x00C TVD CLAMP & AGC CONTROL Register2 */
    volatile uint32_t TVD_HLOCK1;                     /*!< Offset 0x010 TVD HLOCK CONTROL Register1 */
    volatile uint32_t TVD_HLOCK2;                     /*!< Offset 0x014 TVD HLOCK CONTROL Register2 */
    volatile uint32_t TVD_HLOCK3;                     /*!< Offset 0x018 TVD HLOCK CONTROL Register3 */
    volatile uint32_t TVD_HLOCK4;                     /*!< Offset 0x01C TVD HLOCK CONTROL Register4 */
    volatile uint32_t TVD_HLOCK5;                     /*!< Offset 0x020 TVD HLOCK CONTROL Register5 */
    volatile uint32_t TVD_VLOCK1;                     /*!< Offset 0x024 TVD VLOCK CONTROL Register1 */
    volatile uint32_t TVD_VLOCK2;                     /*!< Offset 0x028 TVD VLOCK CONTROL Register2 */
             uint32_t reserved_0x02C;
    volatile uint32_t TVD_CLOCK1;                     /*!< Offset 0x030 TVD CHROMA LOCK CONTROL Register1 */
    volatile uint32_t TVD_CLOCK2;                     /*!< Offset 0x034 TVD CHROMA LOCK CONTROL Register2 */
             uint32_t reserved_0x038 [0x0002];
    volatile uint32_t TVD_YC_SEP1;                    /*!< Offset 0x040 TVD YC SEPERATION CONROL Register1 */
    volatile uint32_t TVD_YC_SEP2;                    /*!< Offset 0x044 TVD YC SEPERATION CONROL Register2 */
             uint32_t reserved_0x048 [0x0002];
    volatile uint32_t TVD_ENHANCE1;                   /*!< Offset 0x050 TVD ENHANCEMENT CONTROL Register1 */
    volatile uint32_t TVD_ENHANCE2;                   /*!< Offset 0x054 TVD ENHANCEMENT CONTROL Register2 */
    volatile uint32_t TVD_ENHANCE3;                   /*!< Offset 0x058 TVD ENHANCEMENT CONTROL Register3 */
             uint32_t reserved_0x05C;
    volatile uint32_t TVD_WB1;                        /*!< Offset 0x060 TVD WB DMA CONTROL Register1 */
    volatile uint32_t TVD_WB2;                        /*!< Offset 0x064 TVD WB DMA CONTROL Register2 */
    volatile uint32_t TVD_WB3;                        /*!< Offset 0x068 TVD WB DMA CONTROL Register3 */
    volatile uint32_t TVD_WB4;                        /*!< Offset 0x06C TVD WB DMA CONTROL Register4 */
             uint32_t reserved_0x070 [0x0004];
    volatile uint32_t TVD_IRQ_CTL;                    /*!< Offset 0x080 TVD DMA Interrupt Control Register */
             uint32_t reserved_0x084 [0x0003];
    volatile uint32_t TVD_IRQ_STATUS;                 /*!< Offset 0x090 TVD DMA Interrupt Status Register */
             uint32_t reserved_0x094 [0x001B];
    volatile uint32_t TVD_DEBUG1;                     /*!< Offset 0x100 TVD DEBUG CONTROL Register1 */
             uint32_t reserved_0x104 [0x001F];
    volatile uint32_t TVD_STATUS1;                    /*!< Offset 0x180 TVD DEBUG STATUS Register1 */
    volatile uint32_t TVD_STATUS2;                    /*!< Offset 0x184 TVD DEBUG STATUS Register2 */
    volatile uint32_t TVD_STATUS3;                    /*!< Offset 0x188 TVD DEBUG STATUS Register3 */
    volatile uint32_t TVD_STATUS4;                    /*!< Offset 0x18C TVD DEBUG STATUS Register4 */
    volatile uint32_t TVD_STATUS5;                    /*!< Offset 0x190 TVD DEBUG STATUS Register5 */
    volatile uint32_t TVD_STATUS6;                    /*!< Offset 0x194 TVD DEBUG STATUS Register6 */
} TVD0_TypeDef; /* size of structure = 0x198 */
/*
 * @brief TVD_TOP
 */
/*!< TVD_TOP Video Decoding */
typedef struct TVD_TOP_Type
{
    volatile uint32_t TVD_TOP_MAP;                    /*!< Offset 0x000 TVD TOP MAP Register */
             uint32_t reserved_0x004;
    volatile uint32_t TVD_3D_CTL1;                    /*!< Offset 0x008 TVD 3D DMA CONTROL Register1 */
    volatile uint32_t TVD_3D_CTL2;                    /*!< Offset 0x00C TVD 3D DMA CONTROL Register2 */
    volatile uint32_t TVD_3D_CTL3;                    /*!< Offset 0x010 TVD 3D DMA CONTROL Register3 */
    volatile uint32_t TVD_3D_CTL4;                    /*!< Offset 0x014 TVD 3D DMA CONTROL Register4 */
    volatile uint32_t TVD_3D_CTL5;                    /*!< Offset 0x018 TVD 3D DMA CONTROL Register5 */
             uint32_t reserved_0x01C;
    struct
    {
                 uint32_t reserved_0x000;
        volatile uint32_t TVD_TOP_CTL;                /*!< Offset 0x024 TVD TOP CONTROL Register */
        volatile uint32_t TVD_ADC_CTL;                /*!< Offset 0x028 TVD ADC CONTROL Register */
        volatile uint32_t TVD_ADC_CFG;                /*!< Offset 0x02C TVD ADC CONFIGURATION Register */
                 uint32_t reserved_0x010 [0x0004];
    } TVD_ADC [0x004];                                /*!< Offset 0x020 TVD ADC Registers N (N = 0 to 3) */
} TVD_TOP_TypeDef; /* size of structure = 0x0A0 */
/*
 * @brief TVE_TOP
 */
/*!< TVE_TOP TV Output (TV_Encoder) */
typedef struct TVE_TOP_Type
{
             uint32_t reserved_0x000 [0x0008];
    volatile uint32_t TVE_DAC_MAP;                    /*!< Offset 0x020 TV Encoder DAC MAP Register */
    volatile uint32_t TVE_DAC_STATUS;                 /*!< Offset 0x024 TV Encoder DAC STAUTS Register */
    volatile uint32_t TVE_DAC_CFG0;                   /*!< Offset 0x028 TV Encoder DAC CFG0 Register */
    volatile uint32_t TVE_DAC_CFG1;                   /*!< Offset 0x02C TV Encoder DAC CFG1 Register */
    volatile uint32_t TVE_DAC_CFG2;                   /*!< Offset 0x030 TV Encoder DAC CFG2 Register */
    volatile uint32_t TVE_DAC_CFG3;                   /*!< Offset 0x034 TV Encoder DAC CFG2 Register */
             uint32_t reserved_0x038 [0x002E];
    volatile uint32_t TVE_DAC_TEST;                   /*!< Offset 0x0F0 TV Encoder DAC TEST Register */
} TVE_TOP_TypeDef; /* size of structure = 0x0F4 */
/*
 * @brief TV_Encoder
 */
/*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) */
typedef struct TV_Encoder_Type
{
    volatile uint32_t TVE_000_REG;                    /*!< Offset 0x000 TV Encoder Clock Gating Register */
    volatile uint32_t TVE_004_REG;                    /*!< Offset 0x004 TV Encoder Configuration Register */
    volatile uint32_t TVE_008_REG;                    /*!< Offset 0x008 TV Encoder DAC Register1 */
    volatile uint32_t TVE_00C_REG;                    /*!< Offset 0x00C TV Encoder Notch and DAC Delay Register */
    volatile uint32_t TVE_010_REG;                    /*!< Offset 0x010 TV Encoder Chroma Frequency Register */
    volatile uint32_t TVE_014_REG;                    /*!< Offset 0x014 TV Encoder Front/Back Porch Register */
    volatile uint32_t TVE_018_REG;                    /*!< Offset 0x018 TV Encoder HD Mode VSYNC Register */
    volatile uint32_t TVE_01C_REG;                    /*!< Offset 0x01C TV Encoder Line Number Register */
    volatile uint32_t TVE_020_REG;                    /*!< Offset 0x020 TV Encoder Level Register */
    volatile uint32_t TVE_024_REG;                    /*!< Offset 0x024 TV Encoder DAC Register2 */
             uint32_t reserved_0x028 [0x0002];
    volatile uint32_t TVE_030_REG;                    /*!< Offset 0x030 TV Encoder Auto Detection Enable Register */
    volatile uint32_t TVE_034_REG;                    /*!< Offset 0x034 TV Encoder Auto Detection Interrupt Status Register */
    volatile uint32_t TVE_038_REG;                    /*!< Offset 0x038 TV Encoder Auto Detection Status Register */
    volatile uint32_t TVE_03C_REG;                    /*!< Offset 0x03C TV Encoder Auto Detection De-bounce Setting Register */
             uint32_t reserved_0x040 [0x002E];
    volatile uint32_t TVE_0F8_REG;                    /*!< Offset 0x0F8 TV Encoder Auto Detect Configuration Register0 */
    volatile uint32_t TVE_0FC_REG;                    /*!< Offset 0x0FC TV Encoder Auto Detect Configuration Register1 */
    volatile uint32_t TVE_100_REG;                    /*!< Offset 0x100 TV Encoder Color Burst Phase Reset Configuration Register */
    volatile uint32_t TVE_104_REG;                    /*!< Offset 0x104 TV Encoder VSYNC Number Register */
    volatile uint32_t TVE_108_REG;                    /*!< Offset 0x108 TV Encoder Notch Filter Frequency Register */
    volatile uint32_t TVE_10C_REG;                    /*!< Offset 0x10C TV Encoder Cb/Cr Level/Gain Register */
    volatile uint32_t TVE_110_REG;                    /*!< Offset 0x110 TV Encoder Tint and Color Burst Phase Register */
    volatile uint32_t TVE_114_REG;                    /*!< Offset 0x114 TV Encoder Burst Width Register */
    volatile uint32_t TVE_118_REG;                    /*!< Offset 0x118 TV Encoder Cb/Cr Gain Register */
    volatile uint32_t TVE_11C_REG;                    /*!< Offset 0x11C TV Encoder Sync and VBI Level Register */
    volatile uint32_t TVE_120_REG;                    /*!< Offset 0x120 TV Encoder White Level Register */
    volatile uint32_t TVE_124_REG;                    /*!< Offset 0x124 TV Encoder Video Active Line Register */
    volatile uint32_t TVE_128_REG;                    /*!< Offset 0x128 TV Encoder Video Chroma BW and CompGain Register */
    volatile uint32_t TVE_12C_REG;                    /*!< Offset 0x12C TV Encoder Register */
    volatile uint32_t TVE_130_REG;                    /*!< Offset 0x130 TV Encoder Re-sync Parameters Register */
    volatile uint32_t TVE_134_REG;                    /*!< Offset 0x134 TV Encoder Slave Parameter Register */
    volatile uint32_t TVE_138_REG;                    /*!< Offset 0x138 TV Encoder Configuration Register0 */
    volatile uint32_t TVE_13C_REG;                    /*!< Offset 0x13C TV Encoder Configuration Register1 */
             uint32_t reserved_0x140 [0x0090];
    volatile uint32_t TVE_380_REG;                    /*!< Offset 0x380 TV Encoder Low Pass Control Register */
    volatile uint32_t TVE_384_REG;                    /*!< Offset 0x384 TV Encoder Low Pass Filter Control Register */
    volatile uint32_t TVE_388_REG;                    /*!< Offset 0x388 TV Encoder Low Pass Gain Register */
    volatile uint32_t TVE_38C_REG;                    /*!< Offset 0x38C TV Encoder Low Pass Gain Control Register */
    volatile uint32_t TVE_390_REG;                    /*!< Offset 0x390 TV Encoder Low Pass Shoot Control Register */
    volatile uint32_t TVE_394_REG;                    /*!< Offset 0x394 TV Encoder Low Pass Coring Register */
             uint32_t reserved_0x398 [0x0002];
    volatile uint32_t TVE_3A0_REG;                    /*!< Offset 0x3A0 TV Encoder Noise Reduction Register */
} TV_Encoder_TypeDef; /* size of structure = 0x3A4 */
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
             uint32_t reserved_0x024 [0x0077];
    volatile uint32_t TWI_DRV_CTRL;                   /*!< Offset 0x200 TWI_DRV Control Register */
    volatile uint32_t TWI_DRV_CFG;                    /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    volatile uint32_t TWI_DRV_SLV;                    /*!< Offset 0x208 TWI_DRV Slave ID Register */
    volatile uint32_t TWI_DRV_FMT;                    /*!< Offset 0x20C TWI_DRV Packet Format Register */
    volatile uint32_t TWI_DRV_BUS_CTRL;               /*!< Offset 0x210 TWI_DRV Bus Control Register */
    volatile uint32_t TWI_DRV_INT_CTRL;               /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    volatile uint32_t TWI_DRV_DMA_CFG;                /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    volatile uint32_t TWI_DRV_FIFO_CON;               /*!< Offset 0x21C TWI_DRV FIFO Content Register */
             uint32_t reserved_0x220 [0x0038];
    volatile uint32_t TWI_DRV_SEND_FIFO_ACC;          /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    volatile uint32_t TWI_DRV_RECV_FIFO_ACC;          /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
             uint32_t reserved_0x308 [0x003E];
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
    volatile uint32_t HCCAPBASE;                      /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
    volatile uint32_t HCSPARAMS;                      /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    volatile uint32_t HCCPARAMS;                      /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
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
    volatile uint32_t VE_CTRL;                        /*!< Offset 0x000  */
             uint32_t reserved_0x004 [0x003B];
    volatile uint32_t VE_VERSION;                     /*!< Offset 0x0F0  */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t VE_MPEG_PIC_HDR;                /*!< Offset 0x100  */
             uint32_t reserved_0x104;
    volatile uint32_t VE_MPEG_SIZE;                   /*!< Offset 0x108  */
    volatile uint32_t VE_MPEG_FRAME_SIZE;             /*!< Offset 0x10C  */
             uint32_t reserved_0x110;
    volatile uint32_t VE_MPEG_CTRL;                   /*!< Offset 0x114  */
    volatile uint32_t VE_MPEG_TRIGGER;                /*!< Offset 0x118  */
    volatile uint32_t VE_MPEG_STATUS;                 /*!< Offset 0x11C  */
             uint32_t reserved_0x120 [0x0002];
    volatile uint32_t VE_MPEG_VLD_ADDR;               /*!< Offset 0x128  */
    volatile uint32_t VE_MPEG_VLD_OFFSET;             /*!< Offset 0x12C  */
    volatile uint32_t VE_MPEG_VLD_LEN;                /*!< Offset 0x130  */
    volatile uint32_t VE_MPEG_VLD_END;                /*!< Offset 0x134  */
             uint32_t reserved_0x138 [0x0004];
    volatile uint32_t VE_MPEG_REC_LUMA;               /*!< Offset 0x148  */
    volatile uint32_t VE_MPEG_REC_CHROMA;             /*!< Offset 0x14C  */
    volatile uint32_t VE_MPEG_FWD_LUMA;               /*!< Offset 0x150  */
    volatile uint32_t VE_MPEG_FWD_CHROMA;             /*!< Offset 0x154  */
    volatile uint32_t VE_MPEG_BACK_LUMA;              /*!< Offset 0x158  */
    volatile uint32_t VE_MPEG_BACK_CHROMA;            /*!< Offset 0x15C  */
             uint32_t reserved_0x160 [0x0008];
    volatile uint32_t VE_MPEG_IQ_MIN_INPUT;           /*!< Offset 0x180  */
             uint32_t reserved_0x184 [0x000D];
    volatile uint32_t VE_MPEG_JPEG_SIZE;              /*!< Offset 0x1B8  */
             uint32_t reserved_0x1BC;
    volatile uint32_t VE_MPEG_JPEG_RES_INT;           /*!< Offset 0x1C0  */
             uint32_t reserved_0x1C4 [0x0002];
    volatile uint32_t VE_MPEG_ROT_LUMA;               /*!< Offset 0x1CC  */
    volatile uint32_t VE_MPEG_ROT_CHROMA;             /*!< Offset 0x1D0  */
    volatile uint32_t VE_MPEG_SDROT_CTRL;             /*!< Offset 0x1D4  */
             uint32_t reserved_0x1D8 [0x0002];
    volatile uint32_t VE_MPEG_RAM_WRITE_PTR;          /*!< Offset 0x1E0  */
    volatile uint32_t VE_MPEG_RAM_WRITE_DATA;         /*!< Offset 0x1E4  */
             uint32_t reserved_0x1E8 [0x0006];
    volatile uint32_t VE_H264_FRAME_SIZE;             /*!< Offset 0x200  */
    volatile uint32_t VE_H264_PIC_HDR;                /*!< Offset 0x204  */
    volatile uint32_t VE_H264_SLICE_HDR;              /*!< Offset 0x208  */
    volatile uint32_t VE_H264_SLICE_HDR2;             /*!< Offset 0x20C  */
    volatile uint32_t VE_H264_PRED_WEIGHT;            /*!< Offset 0x210  */
             uint32_t reserved_0x214 [0x0002];
    volatile uint32_t VE_H264_QP_PARAM;               /*!< Offset 0x21C  */
    volatile uint32_t VE_H264_CTRL;                   /*!< Offset 0x220  */
    volatile uint32_t VE_H264_TRIGGER;                /*!< Offset 0x224  */
    volatile uint32_t VE_H264_STATUS;                 /*!< Offset 0x228  */
    volatile uint32_t VE_H264_CUR_MB_NUM;             /*!< Offset 0x22C  */
    volatile uint32_t VE_H264_VLD_ADDR;               /*!< Offset 0x230  */
    volatile uint32_t VE_H264_VLD_OFFSET;             /*!< Offset 0x234  */
    volatile uint32_t VE_H264_VLD_LEN;                /*!< Offset 0x238  */
    volatile uint32_t VE_H264_VLD_END;                /*!< Offset 0x23C  */
    volatile uint32_t VE_H264_SDROT_CTRL;             /*!< Offset 0x240  */
             uint32_t reserved_0x244 [0x0002];
    volatile uint32_t VE_H264_OUTPUT_FRAME_IDX;       /*!< Offset 0x24C  */
    volatile uint32_t VE_H264_EXTRA_BUFFER1;          /*!< Offset 0x250  */
    volatile uint32_t VE_H264_EXTRA_BUFFER2;          /*!< Offset 0x254  */
             uint32_t reserved_0x258 [0x0021];
    volatile uint32_t VE_H264_BASIC_BITS;             /*!< Offset 0x2DC  */
    volatile uint32_t VE_H264_RAM_WRITE_PTR;          /*!< Offset 0x2E0  */
    volatile uint32_t VE_H264_RAM_WRITE_DATA;         /*!< Offset 0x2E4  */
             uint32_t reserved_0x2E8 [0x0046];
    volatile uint32_t VE_SRAM_H264_FRAMEBUFFER_LIST;  /*!< Offset 0x400  */
             uint32_t reserved_0x404 [0x008F];
    volatile uint32_t VE_SRAM_H264_REF_LIST0;         /*!< Offset 0x640  */
             uint32_t reserved_0x644 [0x0008];
    volatile uint32_t VE_SRAM_H264_REF_LIST1;         /*!< Offset 0x664  */
             uint32_t reserved_0x668 [0x0066];
    volatile uint32_t VE_SRAM_H264_SCALING_LISTS;     /*!< Offset 0x800  */
             uint32_t reserved_0x804 [0x007F];
    volatile uint32_t VE_ISP_PIC_SIZE;                /*!< Offset 0xA00  */
    volatile uint32_t VE_ISP_PIC_STRIDE;              /*!< Offset 0xA04  */
    volatile uint32_t VE_ISP_CTRL;                    /*!< Offset 0xA08  */
             uint32_t reserved_0xA0C [0x001B];
    volatile uint32_t VE_ISP_PIC_LUMA;                /*!< Offset 0xA78  */
    volatile uint32_t VE_ISP_PIC_CHROMA;              /*!< Offset 0xA7C  */
             uint32_t reserved_0xA80 [0x0021];
    volatile uint32_t VE_AVC_PARAM;                   /*!< Offset 0xB04  */
    volatile uint32_t VE_AVC_QP;                      /*!< Offset 0xB08  */
             uint32_t reserved_0xB0C;
    volatile uint32_t VE_AVC_MOTION_EST;              /*!< Offset 0xB10  */
    volatile uint32_t VE_AVC_CTRL;                    /*!< Offset 0xB14  */
    volatile uint32_t VE_AVC_TRIGGER;                 /*!< Offset 0xB18  */
    volatile uint32_t VE_AVC_STATUS;                  /*!< Offset 0xB1C  */
    volatile uint32_t VE_AVC_BASIC_BITS;              /*!< Offset 0xB20  */
             uint32_t reserved_0xB24 [0x0017];
    volatile uint32_t VE_AVC_VLE_ADDR;                /*!< Offset 0xB80  */
    volatile uint32_t VE_AVC_VLE_END;                 /*!< Offset 0xB84  */
    volatile uint32_t VE_AVC_VLE_OFFSET;              /*!< Offset 0xB88  */
    volatile uint32_t VE_AVC_VLE_MAX;                 /*!< Offset 0xB8C  */
    volatile uint32_t VE_AVC_VLE_LENGTH;              /*!< Offset 0xB90  */
             uint32_t reserved_0xB94 [0x0003];
    volatile uint32_t VE_AVC_REF_LUMA;                /*!< Offset 0xBA0  */
    volatile uint32_t VE_AVC_REF_CHROMA;              /*!< Offset 0xBA4  */
             uint32_t reserved_0xBA8 [0x0002];
    volatile uint32_t VE_AVC_REC_LUMA;                /*!< Offset 0xBB0  */
    volatile uint32_t VE_AVC_REC_CHROMA;              /*!< Offset 0xBB4  */
    volatile uint32_t VE_AVC_REF_SLUMA;               /*!< Offset 0xBB8  */
    volatile uint32_t VE_AVC_REC_SLUMA;               /*!< Offset 0xBBC  */
    volatile uint32_t VE_AVC_MB_INFO;                 /*!< Offset 0xBC0  */
             uint32_t reserved_0xBC4 [0x0007];
    volatile uint32_t VE_AVC_SDRAM_INDEX;             /*!< Offset 0xBE0  */
    volatile uint32_t VE_AVC_SDRAM_DATA;              /*!< Offset 0xBE4  */
} VE_TypeDef; /* size of structure = 0xBE8 */


/* Access pointers */

#define VENCODER ((VE_TypeDef *) VENCODER_BASE)       /*!< VENCODER Video Encoding register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define PWM ((PWM_TypeDef *) PWM_BASE)                /*!< PWM Pulse Width Modulation module register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define CIR_TX ((CIR_TX_TypeDef *) CIR_TX_BASE)       /*!< CIR_TX  register set access pointer */
#define LEDC ((LEDC_TypeDef *) LEDC_BASE)             /*!< LEDC LED Lamp Controller register set access pointer */
#define GPADC ((GPADC_TypeDef *) GPADC_BASE)          /*!< GPADC  register set access pointer */
#define THS ((THS_TypeDef *) THS_BASE)                /*!< THS Thermal Sensor register set access pointer */
#define TPADC ((TPADC_TypeDef *) TPADC_BASE)          /*!< TPADC  register set access pointer */
#define IOMMU ((IOMMU_TypeDef *) IOMMU_BASE)          /*!< IOMMU  register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)/*!< AUDIO_CODEC Audio Codec register set access pointer */
#define DMIC ((DMIC_TypeDef *) DMIC_BASE)             /*!< DMIC  register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1  register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2  register set access pointer */
#define OWA ((OWA_TypeDef *) OWA_BASE)                /*!< OWA One Wire Audio (TX only) register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3  register set access pointer */
#define CAN0 ((CAN_TypeDef *) CAN0_BASE)              /*!< CAN0 CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) register set access pointer */
#define CAN1 ((CAN_TypeDef *) CAN1_BASE)              /*!< CAN1 CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) register set access pointer */
#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)    /*!< SYS_CFG  register set access pointer */
#define DMAC ((DMAC_TypeDef *) DMAC_BASE)             /*!< DMAC  register set access pointer */
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID Security ID register set access pointer */
#define HSTIMER ((HSTIMER_TypeDef *) HSTIMER_BASE)    /*!< HSTIMER High Speed Timer (HSTimer) register set access pointer */
#define CE_NS ((CE_TypeDef *) CE_NS_BASE)             /*!< CE_NS Crypto Engine (CE) register set access pointer */
#define CE_S ((CE_TypeDef *) CE_S_BASE)               /*!< CE_S Crypto Engine (CE) register set access pointer */
#define MCTL_COM ((MCTL_COM_TypeDef *) MCTL_COM_BASE) /*!< MCTL_COM  register set access pointer */
#define DDRPHYC ((DDRPHYC_TypeDef *) DDRPHYC_BASE)    /*!< DDRPHYC  register set access pointer */
#define MCTL_PHY ((MCTL_PHY_TypeDef *) MCTL_PHY_BASE) /*!< MCTL_PHY  register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)           /*!< SMHC0 SD-MMC Host Controller register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)           /*!< SMHC1 SD-MMC Host Controller register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)           /*!< SMHC2 SD-MMC Host Controller register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define SPI_DBI ((SPI_TypeDef *) SPI_DBI_BASE)        /*!< SPI_DBI Serial Peripheral Interface register set access pointer */
#define USBOTG0 ((USBOTG_TypeDef *) USBOTG0_BASE)     /*!< USBOTG0  register set access pointer */
#define USBPHY0 ((USBPHYC_TypeDef *) USBPHY0_BASE)    /*!< USBPHY0  register set access pointer */
#define USBEHCI0 ((USB_EHCI_Capability_TypeDef *) USBEHCI0_BASE)/*!< USBEHCI0  register set access pointer */
#define USBOHCI0 ((USB_OHCI_Capability_TypeDef *) USBOHCI0_BASE)/*!< USBOHCI0  register set access pointer */
#define USBEHCI1 ((USB_EHCI_Capability_TypeDef *) USBEHCI1_BASE)/*!< USBEHCI1  register set access pointer */
#define USBOHCI1 ((USB_OHCI_Capability_TypeDef *) USBOHCI1_BASE)/*!< USBOHCI1  register set access pointer */
#define USBPHY1 ((USBPHYC_TypeDef *) USBPHY1_BASE)    /*!< USBPHY1  register set access pointer */
#define EMAC ((EMAC_TypeDef *) EMAC_BASE)             /*!< EMAC  register set access pointer */
#define DE_TOP ((DE_TOP_TypeDef *) DE_TOP_BASE)       /*!< DE_TOP Display Engine (DE) TOP register set access pointer */
#define DE_GLB ((DE_GLB_TypeDef *) DE_GLB_BASE)       /*!< DE_GLB Display Engine (DE) - Global Control register set access pointer */
#define DE_BLD ((DE_BLD_TypeDef *) DE_BLD_BASE)       /*!< DE_BLD Display Engine (DE) - Blender register set access pointer */
#define DE_VI1 ((DE_VI_TypeDef *) DE_VI1_BASE)        /*!< DE_VI1 Display Engine (DE) - VI surface register set access pointer */
#define DE_UI1 ((DE_UI_TypeDef *) DE_UI1_BASE)        /*!< DE_UI1 Display Engine (DE) - UI surface register set access pointer */
#define DE_VEP0 ((DE_VEP_TypeDef *) DE_VEP0_BASE)     /*!< DE_VEP0 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define DE_VEP1 ((DE_VEP_TypeDef *) DE_VEP1_BASE)     /*!< DE_VEP1 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define DE_DEP ((DE_DEP_TypeDef *) DE_DEP_BASE)       /*!< DE_DEP DRC (dynamic range controller) register set access pointer */
#define DEb_GLB ((DE_GLB_TypeDef *) DEb_GLB_BASE)     /*!< DEb_GLB Display Engine (DE) - Global Control register set access pointer */
#define DEb_BLD ((DE_BLD_TypeDef *) DEb_BLD_BASE)     /*!< DEb_BLD Display Engine (DE) - Blender register set access pointer */
#define DEb_VI1 ((DE_VI_TypeDef *) DEb_VI1_BASE)      /*!< DEb_VI1 Display Engine (DE) - VI surface register set access pointer */
#define DEb_VEP0 ((DE_VEP_TypeDef *) DEb_VEP0_BASE)   /*!< DEb_VEP0 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define G2D_TOP ((G2D_TOP_TypeDef *) G2D_TOP_BASE)    /*!< G2D_TOP Graphic 2D top register set access pointer */
#define G2D_MIXER ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer register set access pointer */
#define G2D_BLD ((G2D_BLD_TypeDef *) G2D_BLD_BASE)    /*!< G2D_BLD Graphic 2D (G2D) Engine Blender register set access pointer */
#define G2D_V0 ((G2D_VI_TypeDef *) G2D_V0_BASE)       /*!< G2D_V0 Graphic 2D VI surface register set access pointer */
#define G2D_UI0 ((G2D_UI_TypeDef *) G2D_UI0_BASE)     /*!< G2D_UI0 Graphic 2D UI surface register set access pointer */
#define G2D_UI1 ((G2D_UI_TypeDef *) G2D_UI1_BASE)     /*!< G2D_UI1 Graphic 2D UI surface register set access pointer */
#define G2D_UI2 ((G2D_UI_TypeDef *) G2D_UI2_BASE)     /*!< G2D_UI2 Graphic 2D UI surface register set access pointer */
#define G2D_WB ((G2D_WB_TypeDef *) G2D_WB_BASE)       /*!< G2D_WB Graphic 2D (G2D) Engine Write Back register set access pointer */
#define G2D_VSU ((G2D_VSU_TypeDef *) G2D_VSU_BASE)    /*!< G2D_VSU Graphic 2D Video Scaler register set access pointer */
#define G2D_ROT ((G2D_ROT_TypeDef *) G2D_ROT_BASE)    /*!< G2D_ROT Graphic 2D Rotate register set access pointer */
#define DSI0 ((DSI_TypeDef *) DSI0_BASE)              /*!< DSI0 MIPI DSI Display Interface register set access pointer */
#define DSI_DPHY ((DSI_DPHY_TypeDef *) DSI_DPHY_BASE) /*!< DSI_DPHY MIPI DSI Physical Interface register set access pointer */
#define TCON_LCD0 ((TCON_LCD_TypeDef *) TCON_LCD0_BASE)/*!< TCON_LCD0 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define TCON_TV0 ((TCON_TV_TypeDef *) TCON_TV0_BASE)  /*!< TCON_TV0  register set access pointer */
#define TVE_TOP ((TVE_TOP_TypeDef *) TVE_TOP_BASE)    /*!< TVE_TOP TV Output (TV_Encoder) register set access pointer */
#define TV_Encoder ((TV_Encoder_TypeDef *) TV_Encoder_BASE)/*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) register set access pointer */
#define CSIC_CCU ((CSIC_CCU_TypeDef *) CSIC_CCU_BASE) /*!< CSIC_CCU  register set access pointer */
#define CSIC_TOP ((CSIC_TOP_TypeDef *) CSIC_TOP_BASE) /*!< CSIC_TOP  register set access pointer */
#define CSIC_PARSER0 ((CSIC_PARSER_TypeDef *) CSIC_PARSER0_BASE)/*!< CSIC_PARSER0  register set access pointer */
#define CSIC_DMA0 ((CSIC_DMA_TypeDef *) CSIC_DMA0_BASE)/*!< CSIC_DMA0  register set access pointer */
#define CSIC_DMA1 ((CSIC_DMA_TypeDef *) CSIC_DMA1_BASE)/*!< CSIC_DMA1  register set access pointer */
#define TVD_TOP ((TVD_TOP_TypeDef *) TVD_TOP_BASE)    /*!< TVD_TOP Video Decoding register set access pointer */
#define TVD0 ((TVD0_TypeDef *) TVD0_BASE)             /*!< TVD0 Video Decoding register set access pointer */
#define RISC_CFG ((RISC_CFG_TypeDef *) RISC_CFG_BASE) /*!< RISC_CFG RISC-V core configuration register register set access pointer */
#define R_CPUCFG ((R_CPUCFG_TypeDef *) R_CPUCFG_BASE) /*!< R_CPUCFG  register set access pointer */
#define R_PRCM ((R_PRCM_TypeDef *) R_PRCM_BASE)       /*!< R_PRCM  register set access pointer */
#define CIR_RX ((CIR_RX_TypeDef *) CIR_RX_BASE)       /*!< CIR_RX  register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC Real Time Clock register set access pointer */
#define C0_CPUX_CFG ((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)/*!< C0_CPUX_CFG  register set access pointer */
#define PLIC ((PLIC_TypeDef *) PLIC_BASE)             /*!< PLIC Platform-Level Interrupt Controller (PLIC) register set access pointer */
#define CLINT ((CLINT_TypeDef *) CLINT_BASE)          /*!< CLINT  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
