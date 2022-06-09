/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 */

#ifndef ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_
#define ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_

/**
 * @brief Allwinner T128-S3 Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum IRQn
{
	/******  Cortex-A Processor Specific Interrupt Numbers ***************************************************************/
	/* Software Generated Interrupts                                                                                     */
	SGI0_IRQn                        =  0,     /*!< Software Generated Interrupt  0                                      */
	SGI1_IRQn                        =  1,     /*!< Software Generated Interrupt  1                                      */
	SGI2_IRQn                        =  2,     /*!< Software Generated Interrupt  2                                      */
	SGI3_IRQn                        =  3,     /*!< Software Generated Interrupt  3                                      */
	SGI4_IRQn                        =  4,     /*!< Software Generated Interrupt  4                                      */
	SGI5_IRQn                        =  5,     /*!< Software Generated Interrupt  5                                      */
	SGI6_IRQn                        =  6,     /*!< Software Generated Interrupt  6                                      */
	SGI7_IRQn                        =  7,     /*!< Software Generated Interrupt  7                                      */
	SGI8_IRQn                        =  8,     /*!< Software Generated Interrupt  8                                      */
	SGI9_IRQn                        =  9,     /*!< Software Generated Interrupt  9                                      */
	SGI10_IRQn                       = 10,     /*!< Software Generated Interrupt 10                                      */
	SGI11_IRQn                       = 11,     /*!< Software Generated Interrupt 11                                      */
	SGI12_IRQn                       = 12,     /*!< Software Generated Interrupt 12                                      */
	SGI13_IRQn                       = 13,     /*!< Software Generated Interrupt 13                                      */
	SGI14_IRQn                       = 14,     /*!< Software Generated Interrupt 14                                      */
	SGI15_IRQn                       = 15,     /*!< Software Generated Interrupt 15                                      */
	/* Private Peripheral Interrupts                                                                                     */
	VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
	HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
	VirtualTimer_IRQn                = 27,     /*!< Virtual Timer Interrupt                                              */
	Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
	SecurePhysicalTimer_IRQn         = 29,     /*!< Secure Physical Timer Interrupt                                      */
	NonSecurePhysicalTimer_IRQn      = 30,     /*!< Non-Secure Physical Timer Interrupt                                  */
	Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */

	/******  Allwinner T128-S3 specific Interrupt Numbers ****************************************************************************/

	CPUX_MSGBOX_R = (32), /*  Peripherial */
	UART0_IRQn = (34), /*  Peripherial */
	UART1_IRQn = (35), /*  Peripherial */
	UART2_IRQn = (36), /*  Peripherial */
	UART3_IRQn = (37), /*  Peripherial */
	UART4_IRQn = (38), /*  Peripherial */
	UART5_IRQn = (39), /*  Peripherial */
	TWI0_IRQn = (41), /*  Peripherial */
	TWI1_IRQn = (42), /*  Peripherial */
	TWI2_IRQn = (43), /*  Peripherial */
	TWI3_IRQn = (44), /*  Peripherial */
	SPI0_IRQn = (47), /*  Peripherial */
	SPI1_IRQn = (48), /*  Peripherial */
	PWM_IRQn = (50), /*  Peripherial */
	IR_TX_IRQn = (51), /*  Peripherial */
	LEDC_IRQn = (52), /*  Peripherial */
	CAN0_IRQ = (53), /*  Peripherial */
	CAN1_IRQ = (54), /*  Peripherial */
	OWA_IRQn = (55), /*  Peripherial */
	DMIC_IRQn = (56), /*  Peripherial */
	AUDIO_CODEC_IRQn = (57), /*  Peripherial */
	I2S0_IRQn = (58), /*  Peripherial */
	I2S1_IRQn = (59), /*  Peripherial */
	I2S2_IRQn = (60), /*  Peripherial */
	USB0_DEVICE_IRQn = (61), /*  Peripherial */
	USB0_EHCI_IRQn = (62), /*  Peripherial */
	USB0_OHCI_IRQn = (63), /*  Peripherial */
	USB1_EHCI_IRQn = (65), /*  Peripherial */
	USB1_OHCI_IRQn = (66), /*  Peripherial */
	SMHC0_IRQn = (72), /*  Peripherial */
	SMHC1_IRQn = (73), /*  Peripherial */
	SMHC2_IRQn = (74), /*  Peripherial */
	MSI_IRQn = (75), /*  Peripherial */
	SMC_IRQn = (76), /*  Peripherial */
	EMAC_IRQn = (78), /*  Peripherial */
	TZMA_ERR_IRQn = (79), /*  Peripherial */
	CCU_FERR_IRQn = (80), /*  Peripherial */
	AHB_HREADY_TIME_OUT_IRQn = (81), /*  Peripherial */
	DMAC_NS_IRQn = (82), /*  Peripherial */
	DMAC_S_IRQn = (83), /*  Peripherial */
	CE_NS_IRQn = (84), /*  Peripherial */
	CE_S_IRQn = (85), /*  Peripherial */
	SPINLOCK_IRQn = (86), /*  Peripherial */
	HSTIME0_IRQn = (87), /*  Peripherial */
	HSTIME1_IRQn = (88), /*  Peripherial */
	GPADC_IRQn = (89), /*  Peripherial */
	THS_IRQn = (90), /*  Peripherial */
	TIMER0_base_IRQn = (91), /*  Peripherial */
	TIMER1_base_IRQn = (92), /*  Peripherial */
	LRADC_IRQn = (93), /*  Peripherial */
	TPADC_IRQn = (94), /*  Peripherial */
	WATCHDOG_IRQn = (95), /*  Peripherial */
	IOMMU_IRQn = (96), /*  Peripherial */
	VE_IRQn = (98), /*  Peripherial */
	GPIOB_NS_IRQn = (101), /*  Peripherial */
	GPIOB_S_IRQn = (102), /*  Peripherial */
	GPIOC_NS_IRQn = (103), /*  Peripherial */
	GPIOC_S_IRQn = (104), /*  Peripherial */
	GPIOD_NS_IRQn = (105), /*  Peripherial */
	GPIOD_S_IRQn = (106), /*  Peripherial */
	GPIOE_NS_IRQn = (107), /*  Peripherial */
	GPIOE_S_IRQn = (108), /*  Peripherial */
	GPIOF_NS_IRQn = (109), /*  Peripherial */
	GPIOF_S_IRQn = (110), /*  Peripherial */
	GPIOG_NS_IRQn = (111), /*  Peripherial */
	CPUX_MSGBOX_DSP_W = (117), /*  Peripherial */
	DE_IRQn = (119), /*  Peripherial */
	DI_IRQn = (120), /*  Peripherial */
	G2D_IRQn = (121), /*  Peripherial */
	LCD_IRQn = (122), /*  Peripherial */
	TV_IRQn = (123), /*  Peripherial */
	DSI_IRQn = (124), /*  Peripherial */
	CSI_DMA0_IRQn = (127), /*  Peripherial */
	CSI_DMA1_IRQn = (128), /*  Peripherial */
	CSI_PARSER0_IRQn = (132), /*  Peripherial */
	CSI_TOP_PKT_IRQn = (138), /*  Peripherial */
	DSP_DFE_IRQn = (152), /*  Peripherial */
	DSP_PFE_IRQn = (153), /*  Peripherial */
	DSP_WDG_IRQn = (154), /*  Peripherial */
	DSP_MBOX_RISCV_W_IRQn = (155), /*  Peripherial */
	DSP_TZMA_IRQn = (157), /*  Peripherial */
	/* CPUs related */
	NMI_IRQn = (168), /*  Peripherial */
	PPU_IRQn = (169), /*  Peripherial */
	TWD_IRQn = (170), /*  Peripherial */
	TIMER0_IRQn = (172), /*  Peripherial */
	TIMER1_IRQn = (173), /*  Peripherial */
	TIMER2_IRQn = (174), /*  Peripherial */
	TIMER3_IRQn = (175), /*  Peripherial */
	ALARM0_IRQn = (176), /*  Peripherial */
	IRRX_IRQn = (183), /*  Peripherial */
	C0_CTI0_IRQn = (192), /*  Peripherial */
	C0_CTI1_IRQn = (193), /*  Peripherial */
	C0_COMMTX0_IRQn = (196), /*  Peripherial */
	C0_COMMTX1_IRQn = (197), /*  Peripherial */
	C0_COMMRX0_IRQn = (200), /*  Peripherial */
	C0_COMMRX1_IRQn = (201), /*  Peripherial */
	C0_PMU0_IRQn = (204), /*  Peripherial */
	C0_PMU1_IRQn = (205), /*  Peripherial */
	C0_AXI_ERROR_IRQn = (208), /*  Peripherial */
	AXI_WR_IRQ_IRQn = (210), /*  Peripherial */
	AXI_RD_IRQ_IRQn = (211), /*  Peripherial */
	DBGWRUPREQ_OUT0_IRQn = (212), /*  Peripherial */
	DBGWRUPREQ_OUT1_IRQn = (213), /*  Peripherial */

	GPIOA0_IRQn = 192,	/* Marker for calculations. Each port contain 32 sources */

	GPIOB2_IRQn = ((GPIOA0_IRQn * 32 * 1) + 2), /*  Peripherial */
	GPIOB3_IRQn = (224 + 3), /*  Peripherial */
	GPIOB4_IRQn = (224 + 4), /*  Peripherial */
	GPIOB5_IRQn = (224 + 5), /*  Peripherial */
	GPIOB6_IRQn = (224 + 6), /*  Peripherial */
	GPIOB7_IRQn = (224 + 7), /*  Peripherial */

	GPIOC2_IRQn = (256 + 2), /*  Peripherial */
	GPIOC3_IRQn = (256 + 3), /*  Peripherial */
	GPIOC4_IRQn = (256 + 4), /*  Peripherial */
	GPIOC5_IRQn = (256 + 5), /*  Peripherial */
	GPIOC6_IRQn = (256 + 6), /*  Peripherial */
	GPIOC7_IRQn = (256 + 7), /*  Peripherial */

	GPIOD0_IRQn = (288 + 0), /*  Peripherial */
	GPIOT113_IRQn = (288 + 1), /*  Peripherial */
	GPIOD2_IRQn = (288 + 2), /*  Peripherial */
	GPIOD3_IRQn = (288 + 3), /*  Peripherial */
	GPIOD4_IRQn = (288 + 4), /*  Peripherial */
	GPIOD5_IRQn = (288 + 5), /*  Peripherial */
	GPIOD6_IRQn = (288 + 6), /*  Peripherial */
	GPIOD7_IRQn = (288 + 7), /*  Peripherial */
	GPIOD8_IRQn = (288 + 8), /*  Peripherial */
	GPIOD9_IRQn = (288 + 9), /*  Peripherial */
	GPIOT1130_IRQn = (288 + 10), /*  Peripherial */
	GPIOT1131_IRQn = (288 + 11), /*  Peripherial */
	GPIOT1132_IRQn = (288 + 12), /*  Peripherial */
	GPIOT1133_IRQn = (288 + 13), /*  Peripherial */
	GPIOT1134_IRQn = (288 + 14), /*  Peripherial */
	GPIOT1135_IRQn = (288 + 15), /*  Peripherial */
	GPIOT1136_IRQn = (288 + 16), /*  Peripherial */
	GPIOT1137_IRQn = (288 + 17), /*  Peripherial */
	GPIOT1138_IRQn = (288 + 18), /*  Peripherial */
	GPIOT1139_IRQn = (288 + 19), /*  Peripherial */
	GPIOD20_IRQn = (288 + 20), /*  Peripherial */
	GPIOD21_IRQn = (288 + 21), /*  Peripherial */
	GPIOD22_IRQn = (288 + 22), /*  Peripherial */

	GPIOE0_IRQn = (320 + 0), /*  Peripherial */
	GPIOE1_IRQn = (320 + 1), /*  Peripherial */
	GPIOE2_IRQn = (320 + 2), /*  Peripherial */
	GPIOE3_IRQn = (320 + 3), /*  Peripherial */
	GPIOE4_IRQn = (320 + 4), /*  Peripherial */
	GPIOE5_IRQn = (320 + 5), /*  Peripherial */
	GPIOE6_IRQn = (320 + 6), /*  Peripherial */
	GPIOE7_IRQn = (320 + 7), /*  Peripherial */
	GPIOE8_IRQn = (320 + 8), /*  Peripherial */
	GPIOE9_IRQn = (320 + 9), /*  Peripherial */
	GPIOE10_IRQn = (320 + 10), /*  Peripherial */
	GPIOE11_IRQn = (320 + 11), /*  Peripherial */
	GPIOE12_IRQn = (320 + 12), /*  Peripherial */
	GPIOE13_IRQn = (320 + 13), /*  Peripherial */

	GPIOF0_IRQn = (352 + 0), /*  Peripherial */
	GPIOF1_IRQn = (352 + 1), /*  Peripherial */
	GPIOF2_IRQn = (352 + 2), /*  Peripherial */
	GPIOF3_IRQn = (352 + 3), /*  Peripherial */
	GPIOF4_IRQn = (352 + 4), /*  Peripherial */
	GPIOF5_IRQn = (352 + 5), /*  Peripherial */
	GPIOF6_IRQn = (352 + 6), /*  Peripherial */

	GPIOG0_IRQn = (384 + 0), /*  Peripherial */
	GPIOG1_IRQn = (384 + 1), /*  Peripherial */
	GPIOG2_IRQn = (384 + 2), /*  Peripherial */
	GPIOG3_IRQn = (384 + 3), /*  Peripherial */
	GPIOG4_IRQn = (384 + 4), /*  Peripherial */
	GPIOG5_IRQn = (384 + 5), /*  Peripherial */
	GPIOG6_IRQn = (384 + 6), /*  Peripherial */
	GPIOG7_IRQn = (384 + 7), /*  Peripherial */
	GPIOG8_IRQn = (384 + 8), /*  Peripherial */
	GPIOG9_IRQn = (384 + 9), /*  Peripherial */
	GPIOG10_IRQn = (384 + 10), /*  Peripherial */
	GPIOG11_IRQn = (384 + 11), /*  Peripherial */
	GPIOG12_IRQn = (384 + 12), /*  Peripherial */
	GPIOG13_IRQn = (384 + 13), /*  Peripherial */
	GPIOG14_IRQn = (384 + 14), /*  Peripherial */
	GPIOG15_IRQn = (384 + 15), /*  Peripherial */

	MAX_IRQ_n,
	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#define GIC_BASE             0x03020000
#define GIC_DISTRIBUTOR_BASE (GIC_BASE + 0x1000)
#define GIC_INTERFACE_BASE   (GIC_BASE + 0x2000)

#include "core_ca.h"
#include "system_allwnr_t113s3.h"


/* Generated section start */
/*
 * @brief CPU_SUBSYS_CTRL
 */
/*!< CPU_SUBSYS_CTRL Controller Interface */
typedef struct CPU_SUBSYS_CTRL_Type
{
    volatile uint32_t GENER_CTRL_REG0;                /*!< Offset 0x0000 General Control Register0 */
	         uint8_t reserved1 [0x0008];
    volatile uint32_t GIC_JTAG_RST_CTRL;              /*!< Offset 0x000C GIC and JTAG Reset Control Register */
    volatile uint32_t C0_INT_EN;                      /*!< Offset 0x0010 Cluster0 Interrupt Enable Control Register */
    volatile uint32_t RQ_FIQ_STATUS;                  /*!< Offset 0x0014 IRQ/FIQ Status Register */
    volatile uint32_t GENER_CTRL_REG2;                /*!< Offset 0x0018 General Control Register2 */
    volatile uint32_t DBG_STATE;                      /*!< Offset 0x001C Debug State Register */
} CPU_SUBSYS_CTRL_TypeDef;
/*
 * @brief CCU
 */
/*!< CCU Controller Interface */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPU_CTRL_REG;               /*!< Offset 0x0000 PLL_CPU Control Register */
	         uint8_t reserved1 [0x000C];
    volatile uint32_t PLL_DDR_CTRL_REG;               /*!< Offset 0x0010 PLL_DDR Control Register */
	         uint8_t reserved2 [0x000C];
    volatile uint32_t PLL_PERI_CTRL_REG;              /*!< Offset 0x0020 PLL_PERI Control Register */
	         uint8_t reserved3 [0x001C];
    volatile uint32_t PLL_VIDEO0_CTRL_REG;            /*!< Offset 0x0040 PLL_VIDEO0 Control Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t PLL_VIDEO1_CTRL_REG;            /*!< Offset 0x0048 PLL_VIDEO1 Control Register */
	         uint8_t reserved5 [0x000C];
    volatile uint32_t PLL_VE_CTRL_REG;                /*!< Offset 0x0058 PLL_VE Control Register */
	         uint8_t reserved6 [0x001C];
    volatile uint32_t PLL_AUDIO0_CTRL_REG;            /*!< Offset 0x0078 PLL_AUDIO0 Control Register */
	         uint8_t reserved7 [0x0004];
    volatile uint32_t PLL_AUDIO1_CTRL_REG;            /*!< Offset 0x0080 PLL_AUDIO1 Control Register */
	         uint8_t reserved8 [0x008C];
    volatile uint32_t PLL_DDR_PAT0_CTRL_REG;          /*!< Offset 0x0110 PLL_DDR Pattern0 Control Register */
    volatile uint32_t PLL_DDR_PAT1_CTRL_REG;          /*!< Offset 0x0114 PLL_DDR Pattern1 Control Register */
	         uint8_t reserved9 [0x0008];
    volatile uint32_t PLL_PERI_PAT0_CTRL_REG;         /*!< Offset 0x0120 PLL_PERI Pattern0 Control Register */
    volatile uint32_t PLL_PERI_PAT1_CTRL_REG;         /*!< Offset 0x0124 PLL_PERI Pattern1 Control Register */
	         uint8_t reserved10 [0x0018];
    volatile uint32_t PLL_VIDEO0_PAT0_CTRL_REG;       /*!< Offset 0x0140 PLL_VIDEO0 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO0_PAT1_CTRL_REG;       /*!< Offset 0x0144 PLL_VIDEO0 Pattern1 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT0_CTRL_REG;       /*!< Offset 0x0148 PLL_VIDEO1 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT1_CTRL_REG;       /*!< Offset 0x014C PLL_VIDEO1 Pattern1 Control Register */
	         uint8_t reserved11 [0x0008];
    volatile uint32_t PLL_VE_PAT0_CTRL_REG;           /*!< Offset 0x0158 PLL_VE Pattern0 Control Register */
    volatile uint32_t PLL_VE_PAT1_CTRL_REG;           /*!< Offset 0x015C PLL_VE Pattern1 Control Register */
	         uint8_t reserved12 [0x0018];
    volatile uint32_t PLL_AUDIO0_PAT0_CTRL_REG;       /*!< Offset 0x0178 PLL_AUDIO0 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO0_PAT1_CTRL_REG;       /*!< Offset 0x017C PLL_AUDIO0 Pattern1 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT0_CTRL_REG;       /*!< Offset 0x0180 PLL_AUDIO1 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT1_CTRL_REG;       /*!< Offset 0x0184 PLL_AUDIO1 Pattern1 Control Register */
	         uint8_t reserved13 [0x0178];
    volatile uint32_t PLL_CPU_BIAS_REG;               /*!< Offset 0x0300 PLL_CPU Bias Register */
	         uint8_t reserved14 [0x000C];
    volatile uint32_t PLL_DDR_BIAS_REG;               /*!< Offset 0x0310 PLL_DDR Bias Register */
	         uint8_t reserved15 [0x000C];
    volatile uint32_t PLL_PERI_BIAS_REG;              /*!< Offset 0x0320 PLL_PERI Bias Register */
	         uint8_t reserved16 [0x001C];
    volatile uint32_t PLL_VIDEO0_BIAS_REG;            /*!< Offset 0x0340 PLL_VIDEO0 Bias Register */
	         uint8_t reserved17 [0x0004];
    volatile uint32_t PLL_VIDEO1_BIAS_REG;            /*!< Offset 0x0348 PLL_VIDEO1 Bias Register */
	         uint8_t reserved18 [0x000C];
    volatile uint32_t PLL_VE_BIAS_REG;                /*!< Offset 0x0358 PLL_VE Bias Register */
	         uint8_t reserved19 [0x001C];
    volatile uint32_t PLL_AUDIO0_BIAS_REG;            /*!< Offset 0x0378 PLL_AUDIO0 Bias Register */
	         uint8_t reserved20 [0x0004];
    volatile uint32_t PLL_AUDIO1_BIAS_REG;            /*!< Offset 0x0380 PLL_AUDIO1 Bias Register */
	         uint8_t reserved21 [0x007C];
    volatile uint32_t PLL_CPU_TUN_REG;                /*!< Offset 0x0400 PLL_CPU Tuning Register */
	         uint8_t reserved22 [0x00FC];
    volatile uint32_t CPU_AXI_CFG_REG;                /*!< Offset 0x0500 CPU_AXI Configuration Register */
    volatile uint32_t CPU_GATING_REG;                 /*!< Offset 0x0504 CPU_GATING Configuration Register */
	         uint8_t reserved23 [0x0008];
    volatile uint32_t PSI_CLK_REG;                    /*!< Offset 0x0510 PSI Clock Register */
	         uint8_t reserved24 [0x000C];
    volatile uint32_t APB0_CLK_REG;                   /*!< Offset 0x0520 APB0 Clock Register */
    volatile uint32_t APB1_CLK_REG;                   /*!< Offset 0x0524 APB1 Clock Register */
	         uint8_t reserved25 [0x0018];
    volatile uint32_t MBUS_CLK_REG;                   /*!< Offset 0x0540 MBUS Clock Register */
	         uint8_t reserved26 [0x00BC];
    volatile uint32_t DE_CLK_REG;                     /*!< Offset 0x0600 DE Clock Register */
	         uint8_t reserved27 [0x0008];
    volatile uint32_t DE_BGR_REG;                     /*!< Offset 0x060C DE Bus Gating Reset Register */
	         uint8_t reserved28 [0x0010];
    volatile uint32_t DI_CLK_REG;                     /*!< Offset 0x0620 DI Clock Register */
	         uint8_t reserved29 [0x0008];
    volatile uint32_t DI_BGR_REG;                     /*!< Offset 0x062C DI Bus Gating Reset Register */
    volatile uint32_t G2D_CLK_REG;                    /*!< Offset 0x0630 G2D Clock Register */
	         uint8_t reserved30 [0x0008];
    volatile uint32_t G2D_BGR_REG;                    /*!< Offset 0x063C G2D Bus Gating Reset Register */
	         uint8_t reserved31 [0x0040];
    volatile uint32_t CE_CLK_REG;                     /*!< Offset 0x0680 CE Clock Register */
	         uint8_t reserved32 [0x0008];
    volatile uint32_t CE_BGR_REG;                     /*!< Offset 0x068C CE Bus Gating Reset Register */
    volatile uint32_t VE_CLK_REG;                     /*!< Offset 0x0690 VE Clock Register */
	         uint8_t reserved33 [0x0008];
    volatile uint32_t VE_BGR_REG;                     /*!< Offset 0x069C VE Bus Gating Reset Register */
	         uint8_t reserved34 [0x006C];
    volatile uint32_t DMA_BGR_REG;                    /*!< Offset 0x070C DMA Bus Gating Reset Register */
	         uint8_t reserved35 [0x000C];
    volatile uint32_t MSGBOX_BGR_REG;                 /*!< Offset 0x071C MSGBOX Bus Gating Reset Register */
	         uint8_t reserved36 [0x000C];
    volatile uint32_t SPINLOCK_BGR_REG;               /*!< Offset 0x072C SPINLOCK Bus Gating Reset Register */
	         uint8_t reserved37 [0x000C];
    volatile uint32_t HSTIMER_BGR_REG;                /*!< Offset 0x073C HSTIMER Bus Gating Reset Register */
    volatile uint32_t AVS_CLK_REG;                    /*!< Offset 0x0740 AVS Clock Register */
	         uint8_t reserved38 [0x0048];
    volatile uint32_t DBGSYS_BGR_REG;                 /*!< Offset 0x078C DBGSYS Bus Gating Reset Register */
	         uint8_t reserved39 [0x001C];
    volatile uint32_t PWM_BGR_REG;                    /*!< Offset 0x07AC PWM Bus Gating Reset Register */
	         uint8_t reserved40 [0x000C];
    volatile uint32_t IOMMU_BGR_REG;                  /*!< Offset 0x07BC IOMMU Bus Gating Reset Register */
	         uint8_t reserved41 [0x0040];
    volatile uint32_t DRAM_CLK_REG;                   /*!< Offset 0x0800 DRAM Clock Register */
    volatile uint32_t MBUS_MAT_CLK_GATING_REG;        /*!< Offset 0x0804 MBUS Master Clock Gating Register */
	         uint8_t reserved42 [0x0004];
    volatile uint32_t DRAM_BGR_REG;                   /*!< Offset 0x080C DRAM Bus Gating Reset Register */
	         uint8_t reserved43 [0x0020];
    volatile uint32_t SMHC0_CLK_REG;                  /*!< Offset 0x0830 SMHC0 Clock Register */
    volatile uint32_t SMHC1_CLK_REG;                  /*!< Offset 0x0834 SMHC1 Clock Register */
    volatile uint32_t SMHC2_CLK_REG;                  /*!< Offset 0x0838 SMHC2 Clock Register */
	         uint8_t reserved44 [0x0010];
    volatile uint32_t SMHC_BGR_REG;                   /*!< Offset 0x084C SMHC Bus Gating Reset Register */
	         uint8_t reserved45 [0x00BC];
    volatile uint32_t UART_BGR_REG;                   /*!< Offset 0x090C UART Bus Gating Reset Register */
	         uint8_t reserved46 [0x000C];
    volatile uint32_t TWI_BGR_REG;                    /*!< Offset 0x091C TWI Bus Gating Reset Register */
	         uint8_t reserved47 [0x0020];
    volatile uint32_t SPI0_CLK_REG;                   /*!< Offset 0x0940 SPI0 Clock Register */
    volatile uint32_t SPI1_CLK_REG;                   /*!< Offset 0x0944 SPI1 Clock Register */
	         uint8_t reserved48 [0x0024];
    volatile uint32_t SPI_BGR_REG;                    /*!< Offset 0x096C SPI Bus Gating Reset Register */
    volatile uint32_t EMAC_25M_CLK_REG;               /*!< Offset 0x0970 EMAC_25M Clock Register */
	         uint8_t reserved49 [0x0008];
    volatile uint32_t EMAC_BGR_REG;                   /*!< Offset 0x097C EMAC Bus Gating Reset Register */
	         uint8_t reserved50 [0x0040];
    volatile uint32_t IRTX_CLK_REG;                   /*!< Offset 0x09C0 IRTX Clock Register */
	         uint8_t reserved51 [0x0008];
    volatile uint32_t IRTX_BGR_REG;                   /*!< Offset 0x09CC IRTX Bus Gating Reset Register */
	         uint8_t reserved52 [0x001C];
    volatile uint32_t GPADC_BGR_REG;                  /*!< Offset 0x09EC GPADC Bus Gating Reset Register */
	         uint8_t reserved53 [0x000C];
    volatile uint32_t THS_BGR_REG;                    /*!< Offset 0x09FC THS Bus Gating Reset Register */
	         uint8_t reserved54 [0x0014];
    volatile uint32_t I2S1_CLK_REG;                   /*!< Offset 0x0A14 I2S1 Clock Register */
    volatile uint32_t I2S2_CLK_REG;                   /*!< Offset 0x0A18 I2S2 Clock Register */
    volatile uint32_t I2S2_ASRC_CLK_REG;              /*!< Offset 0x0A1C I2S2_ASRC Clock Register */
    volatile uint32_t I2S_BGR_REG;                    /*!< Offset 0x0A20 I2S Bus Gating Reset Register */
    volatile uint32_t OWA_TX_CLK_REG;                 /*!< Offset 0x0A24 OWA_TX Clock Register */
    volatile uint32_t OWA_RX_CLK_REG;                 /*!< Offset 0x0A28 OWA_RX Clock Register */
    volatile uint32_t OWA_BGR_REG;                    /*!< Offset 0x0A2C OWA Bus Gating Reset Register */
	         uint8_t reserved55 [0x0010];
    volatile uint32_t DMIC_CLK_REG;                   /*!< Offset 0x0A40 DMIC Clock Register */
	         uint8_t reserved56 [0x0008];
    volatile uint32_t DMIC_BGR_REG;                   /*!< Offset 0x0A4C DMIC Bus Gating Reset Register */
    volatile uint32_t AUDIO_CODEC_DAC_CLK_REG;        /*!< Offset 0x0A50 AUDIO_CODEC_DAC Clock Register */
    volatile uint32_t AUDIO_CODEC_ADC_CLK_REG;        /*!< Offset 0x0A54 AUDIO_CODEC_ADC Clock Register */
	         uint8_t reserved57 [0x0004];
    volatile uint32_t AUDIO_CODEC_BGR_REG;            /*!< Offset 0x0A5C AUDIO_CODEC Bus Gating Reset Register */
	         uint8_t reserved58 [0x0010];
    volatile uint32_t USB0_CLK_REG;                   /*!< Offset 0x0A70 USB0 Clock Register */
    volatile uint32_t USB1_CLK_REG;                   /*!< Offset 0x0A74 USB1 Clock Register */
	         uint8_t reserved59 [0x0014];
    volatile uint32_t USB_BGR_REG;                    /*!< Offset 0x0A8C USB Bus Gating Reset Register */
	         uint8_t reserved60 [0x002C];
    volatile uint32_t DPSS_TOP_BGR_REG;               /*!< Offset 0x0ABC DPSS_TOP Bus Gating Reset Register */
	         uint8_t reserved61 [0x0064];
    volatile uint32_t DSI_CLK_REG;                    /*!< Offset 0x0B24 DSI Clock Register */
	         uint8_t reserved62 [0x0024];
    volatile uint32_t DSI_BGR_REG;                    /*!< Offset 0x0B4C DSI Bus Gating Reset Register */
	         uint8_t reserved63 [0x0010];
    volatile uint32_t TCONLCD_CLK_REG;                /*!< Offset 0x0B60 TCONLCD Clock Register */
	         uint8_t reserved64 [0x0018];
    volatile uint32_t TCONLCD_BGR_REG;                /*!< Offset 0x0B7C TCONLCD Bus Gating Reset Register */
    volatile uint32_t TCONTV_CLK_REG;                 /*!< Offset 0x0B80 TCONTV Clock Register */
	         uint8_t reserved65 [0x0018];
    volatile uint32_t TCONTV_BGR_REG;                 /*!< Offset 0x0B9C TCONTV Bus Gating Reset Register */
	         uint8_t reserved66 [0x000C];
    volatile uint32_t LVDS_BGR_REG;                   /*!< Offset 0x0BAC LVDS Bus Gating Reset Register */
    volatile uint32_t TVE_CLK_REG;                    /*!< Offset 0x0BB0 TVE Clock Register */
	         uint8_t reserved67 [0x0008];
    volatile uint32_t TVE_BGR_REG;                    /*!< Offset 0x0BBC TVE Bus Gating Reset Register */
    volatile uint32_t TVD_CLK_REG;                    /*!< Offset 0x0BC0 TVD Clock Register */
	         uint8_t reserved68 [0x0018];
    volatile uint32_t TVD_BGR_REG;                    /*!< Offset 0x0BDC TVD Bus Gating Reset Register */
	         uint8_t reserved69 [0x0010];
    volatile uint32_t LEDC_CLK_REG;                   /*!< Offset 0x0BF0 LEDC Clock Register */
	         uint8_t reserved70 [0x0008];
    volatile uint32_t LEDC_BGR_REG;                   /*!< Offset 0x0BFC LEDC Bus Gating Reset Register */
	         uint8_t reserved71 [0x0004];
    volatile uint32_t CSI_CLK_REG;                    /*!< Offset 0x0C04 CSI Clock Register */
    volatile uint32_t CSI_MASTER_CLK_REG;             /*!< Offset 0x0C08 CSI Master Clock Register */
	         uint8_t reserved72 [0x0010];
    volatile uint32_t CSI_BGR_REG;                    /*!< Offset 0x0C1C CSI Bus Gating Reset Register */
	         uint8_t reserved73 [0x0030];
    volatile uint32_t TPADC_CLK_REG;                  /*!< Offset 0x0C50 TPADC Clock Register */
	         uint8_t reserved74 [0x0008];
    volatile uint32_t TPADC_BGR_REG;                  /*!< Offset 0x0C5C TPADC Bus Gating Reset Register */
	         uint8_t reserved75 [0x0010];
    volatile uint32_t DSP_CLK_REG;                    /*!< Offset 0x0C70 DSP Clock Register */
	         uint8_t reserved76 [0x0008];
    volatile uint32_t DSP_BGR_REG;                    /*!< Offset 0x0C7C DSP Bus Gating Reset Register */
	         uint8_t reserved77 [0x0284];
    volatile uint32_t PLL_LOCK_DBG_CTRL_REG;          /*!< Offset 0x0F04 PLL Lock Debug Control Register */
    volatile uint32_t FRE_DET_CTRL_REG;               /*!< Offset 0x0F08 Frequency Detect Control Register */
    volatile uint32_t FRE_UP_LIM_REG;                 /*!< Offset 0x0F0C Frequency Up Limit Register */
    volatile uint32_t FRE_DOWN_LIM_REG;               /*!< Offset 0x0F10 Frequency Down Limit Register */
	         uint8_t reserved78 [0x001C];
    volatile uint32_t CCU_FAN_GATE_REG;               /*!< Offset 0x0F30 CCU FANOUT CLOCK GATE Register */
    volatile uint32_t CLK27M_FAN_REG;                 /*!< Offset 0x0F34 CLK27M FANOUT Register */
    volatile uint32_t PCLK_FAN_REG;                   /*!< Offset 0x0F38 PCLK FANOUT Register */
    volatile uint32_t CCU_FAN_REG;                    /*!< Offset 0x0F3C CCU FANOUT Register */
} CCU_TypeDef;
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG Controller Interface */
typedef struct SYS_CFG_Type
{
	         uint8_t reserved1 [0x0008];
    volatile uint32_t DSP_BOOT_RAMMAP_REG;            /*!< Offset 0x0008 DSP Boot SRAM Remap Control Register */
	         uint8_t reserved2 [0x0018];
    volatile uint32_t VER_REG;                        /*!< Offset 0x0024 Version Register */
	         uint8_t reserved3 [0x0008];
    volatile uint32_t EMAC_EPHY_CLK_REG0;             /*!< Offset 0x0030 EMAC-EPHY Clock Register 0 */
	         uint8_t reserved4 [0x011C];
    volatile uint32_t SYS_LDO_CTRL_REG;               /*!< Offset 0x0150 System LDO Control Register */
	         uint8_t reserved5 [0x000C];
    volatile uint32_t RESCAL_CTRL_REG;                /*!< Offset 0x0160 Resistor Calibration Control Register */
	         uint8_t reserved6 [0x0004];
    volatile uint32_t RES240_CTRL_REG;                /*!< Offset 0x0168 240ohms Resistor Manual Control Register */
    volatile uint32_t RESCAL_STATUS_REG;              /*!< Offset 0x016C Resistor Calibration Status Register */
} SYS_CFG_TypeDef;
/*
 * @brief UART
 */
/*!< UART Controller Interface */
typedef struct UART_Type
{
    volatile uint32_t DATA;                           /*!< Offset 0x0000 UART Receive Buffer Register/Transmit Holding Register */
    volatile uint32_t DLH_IER;                        /*!< Offset 0x0004 UART Divisor Latch High Register/UART Interrupt Enable Register */
    volatile uint32_t IIR_FCR;                        /*!< Offset 0x0008 UART Interrupt Identity Register/UART FIFO Control Register */
    volatile uint32_t UART_LCR;                       /*!< Offset 0x000C UART Line Control Register */
    volatile uint32_t UART_MCR;                       /*!< Offset 0x0010 UART Modem Control Register */
    volatile uint32_t UART_LSR;                       /*!< Offset 0x0014 UART Line Status Register */
    volatile uint32_t UART_MSR;                       /*!< Offset 0x0018 UART Modem Status Register */
    volatile uint32_t UART_SCH;                       /*!< Offset 0x001C UART Scratch Register */
	         uint8_t reserved1 [0x005C];
    volatile uint32_t UART_USR;                       /*!< Offset 0x007C UART Status Register */
    volatile uint32_t UART_TFL;                       /*!< Offset 0x0080 UART Transmit FIFO Level Register */
    volatile uint32_t UART_RFL;                       /*!< Offset 0x0084 UART Receive FIFO Level Register */
    volatile uint32_t UART_HSK;                       /*!< Offset 0x0088 UART DMA Handshake Configuration Register */
    volatile uint32_t UART_DMA_REQ_EN;                /*!< Offset 0x008C UART DMA Request Enable Register */
	         uint8_t reserved2 [0x0014];
    volatile uint32_t UART_HALT;                      /*!< Offset 0x00A4 UART Halt TX Register */
	         uint8_t reserved3 [0x0008];
    volatile uint32_t UART_DBG_DLL;                   /*!< Offset 0x00B0 UART Debug DLL Register */
    volatile uint32_t UART_DBG_DLH;                   /*!< Offset 0x00B4 UART Debug DLH Register */
	         uint8_t reserved4 [0x0038];
    volatile uint32_t UART_A_FCC;                     /*!< Offset 0x00F0 UART FIFO Clock Control Register */
	         uint8_t reserved5 [0x000C];
    volatile uint32_t UART_A_RXDMA_CTRL;              /*!< Offset 0x0100 UART RXDMA Control Register */
    volatile uint32_t UART_A_RXDMA_STR;               /*!< Offset 0x0104 UART RXDMA Start Register */
    volatile uint32_t UART_A_RXDMA_STA;               /*!< Offset 0x0108 UART RXDMA Status Register */
    volatile uint32_t UART_A_RXDMA_LMT;               /*!< Offset 0x010C UART RXDMA Limit Register */
    volatile uint32_t UART_A_RXDMA_SADDRL;            /*!< Offset 0x0110 UART RXDMA Buffer Start Address Low Register */
    volatile uint32_t UART_A_RXDMA_SADDRH;            /*!< Offset 0x0114 UART RXDMA Buffer Start Address High Register */
    volatile uint32_t UART_A_RXDMA_BL;                /*!< Offset 0x0118 UART RXDMA Buffer Length Register */
	         uint8_t reserved6 [0x0004];
    volatile uint32_t UART_A_RXDMA_IE;                /*!< Offset 0x0120 UART RXDMA Interrupt Enable Register */
    volatile uint32_t UART_A_RXDMA_IS;                /*!< Offset 0x0124 UART RXDMA Interrupt Status Register */
    volatile uint32_t UART_A_RXDMA_WADDRL;            /*!< Offset 0x0128 UART RXDMA Write Address Low Register */
    volatile uint32_t UART_A_RXDMA_WADDRH;            /*!< Offset 0x012C UART RXDMA Write Address high Register */
    volatile uint32_t UART_A_RXDMA_RADDRL;            /*!< Offset 0x0130 UART RXDMA Read Address Low Register */
    volatile uint32_t UART_A_RXDMA_RADDRH;            /*!< Offset 0x0134 UART RXDMA Read Address high Register */
    volatile uint32_t UART_A_RXDMA_DCNT;              /*!< Offset 0x0138 UART RADMA Data Count Register */
} UART_TypeDef;
/*
 * @brief GPIO
 */
/*!< GPIO Controller Interface */
typedef struct GPIO_Type
{
    volatile uint32_t CFG [0x004];                    /*!< Offset 0x0000 Configure Register */
    volatile uint32_t DATA;                           /*!< Offset 0x0010 Data Register */
    volatile uint32_t DRV [0x004];                    /*!< Offset 0x0014 Multi_Driving Register */
    volatile uint32_t PULL [0x002];                   /*!< Offset 0x0024 Pull Register */
} GPIO_TypeDef;
/*
 * @brief SMHC
 */
/*!< SMHC Controller Interface */
typedef struct SMHC_Type
{
    volatile uint32_t SMHC_CTRL;                      /*!< Offset 0x0000 Control Register */
    volatile uint32_t SMHC_CLKDIV;                    /*!< Offset 0x0004 Clock Control Register */
    volatile uint32_t SMHC_TMOUT;                     /*!< Offset 0x0008 Time Out Register */
    volatile uint32_t SMHC_CTYPE;                     /*!< Offset 0x000C Bus Width Register */
    volatile uint32_t SMHC_BLKSIZ;                    /*!< Offset 0x0010 Block Size Register */
    volatile uint32_t SMHC_BYTCNT;                    /*!< Offset 0x0014 Byte Count Register */
    volatile uint32_t SMHC_CMD;                       /*!< Offset 0x0018 Command Register */
    volatile uint32_t SMHC_CMDARG;                    /*!< Offset 0x001C Command Argument Register */
    volatile uint32_t SMHC_RESP0;                     /*!< Offset 0x0020 Response 0 Register */
    volatile uint32_t SMHC_RESP1;                     /*!< Offset 0x0024 Response 1 Register */
    volatile uint32_t SMHC_RESP2;                     /*!< Offset 0x0028 Response 2 Register */
    volatile uint32_t SMHC_RESP3;                     /*!< Offset 0x002C Response 3 Register */
    volatile uint32_t SMHC_INTMASK;                   /*!< Offset 0x0030 Interrupt Mask Register */
    volatile uint32_t SMHC_MINTSTS;                   /*!< Offset 0x0034 Masked Interrupt Status Register */
    volatile uint32_t SMHC_RINTSTS;                   /*!< Offset 0x0038 Raw Interrupt Status Register */
    volatile uint32_t SMHC_STATUS;                    /*!< Offset 0x003C Status Register */
    volatile uint32_t SMHC_FIFOTH;                    /*!< Offset 0x0040 FIFO Water Level Register */
    volatile uint32_t SMHC_FUNS;                      /*!< Offset 0x0044 FIFO Function Select Register */
    volatile uint32_t SMHC_TCBCNT;                    /*!< Offset 0x0048 Transferred Byte Count between Controller and Card */
    volatile uint32_t SMHC_TBBCNT;                    /*!< Offset 0x004C Transferred Byte Count between Host Memory and Internal FIFO */
    volatile uint32_t SMHC_DBGC;                      /*!< Offset 0x0050 Current Debug Control Register */
    volatile uint32_t SMHC_CSDC;                      /*!< Offset 0x0054 CRC Status Detect Control Registers */
    volatile uint32_t SMHC_A12A;                      /*!< Offset 0x0058 Auto Command 12 Argument Register */
    volatile uint32_t SMHC_NTSR;                      /*!< Offset 0x005C SD New Timing Set Register */
	         uint8_t reserved1 [0x0018];
    volatile uint32_t SMHC_HWRST;                     /*!< Offset 0x0078 Hardware Reset Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t SMHC_IDMAC;                     /*!< Offset 0x0080 IDMAC Control Register */
    volatile uint32_t SMHC_DLBA;                      /*!< Offset 0x0084 Descriptor List Base Address Register */
    volatile uint32_t SMHC_IDST;                      /*!< Offset 0x0088 IDMAC Status Register */
    volatile uint32_t SMHC_IDIE;                      /*!< Offset 0x008C IDMAC Interrupt Enable Register */
	         uint8_t reserved3 [0x0070];
    volatile uint32_t SMHC_THLD;                      /*!< Offset 0x0100 Card Threshold Control Register */
    volatile uint32_t SMHC_SFC;                       /*!< Offset 0x0104 Sample FIFO Control Register */
    volatile uint32_t SMHC_A23A;                      /*!< Offset 0x0108 Auto Command 23 Argument Register */
    volatile uint32_t EMMC_DDR_SBIT_DET;              /*!< Offset 0x010C eMMC4.5 DDR Start Bit Detection Control Register */
	         uint8_t reserved4 [0x0028];
    volatile uint32_t SMHC_EXT_CMD;                   /*!< Offset 0x0138 Extended Command Register */
    volatile uint32_t SMHC_EXT_RESP;                  /*!< Offset 0x013C Extended Response Register */
    volatile uint32_t SMHC_DRV_DL;                    /*!< Offset 0x0140 Drive Delay Control Register */
    volatile uint32_t SMHC_SMAP_DL;                   /*!< Offset 0x0144 Sample Delay Control Register */
    volatile uint32_t SMHC_DS_DL;                     /*!< Offset 0x0148 Data Strobe Delay Control Register */
    volatile uint32_t SMHC_HS400_DL;                  /*!< Offset 0x014C HS400 Delay Control Register */
	         uint8_t reserved5 [0x00B0];
    volatile uint32_t SMHC_FIFO;                      /*!< Offset 0x0200 Read/Write FIFO */
} SMHC_TypeDef;
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM Controller Interface */
typedef struct I2S_PCM_Type
{
    volatile uint32_t I2S_PCM_CTL;                    /*!< Offset 0x0000 I2S/PCM Control Register */
    volatile uint32_t I2S_PCM_FMT0;                   /*!< Offset 0x0004 I2S/PCM Format Register 0 */
    volatile uint32_t I2S_PCM_FMT1;                   /*!< Offset 0x0008 I2S/PCM Format Register 1 */
    volatile uint32_t I2S_PCM_ISTA;                   /*!< Offset 0x000C I2S/PCM Interrupt Status Register */
    volatile uint32_t I2S_PCM_RXFIFO;                 /*!< Offset 0x0010 I2S/PCM RXFIFO Register */
    volatile uint32_t I2S_PCM_FCTL;                   /*!< Offset 0x0014 I2S/PCM FIFO Control Register */
    volatile uint32_t I2S_PCM_FSTA;                   /*!< Offset 0x0018 I2S/PCM FIFO Status Register */
    volatile uint32_t I2S_PCM_INT;                    /*!< Offset 0x001C I2S/PCM DMA & Interrupt Control Register */
    volatile uint32_t I2S_PCM_TXFIFO;                 /*!< Offset 0x0020 I2S/PCM TXFIFO Register */
    volatile uint32_t I2S_PCM_CLKD;                   /*!< Offset 0x0024 I2S/PCM Clock Divide Register */
    volatile uint32_t I2S_PCM_TXCNT;                  /*!< Offset 0x0028 I2S/PCM TX Sample Counter Register */
    volatile uint32_t I2S_PCM_RXCNT;                  /*!< Offset 0x002C I2S/PCM RX Sample Counter Register */
    volatile uint32_t I2S_PCM_CHCFG;                  /*!< Offset 0x0030 I2S/PCM Channel Configuration Register */
    volatile uint32_t I2S_PCM_TX0CHSEL;               /*!< Offset 0x0034 I2S/PCM TX0 Channel Select Register */
    volatile uint32_t I2S_PCM_TX1CHSEL;               /*!< Offset 0x0038 I2S/PCM TX1 Channel Select Register */
    volatile uint32_t I2S_PCM_TX2CHSEL;               /*!< Offset 0x003C I2S/PCM TX2 Channel Select Register */
    volatile uint32_t I2S_PCM_TX3CHSEL;               /*!< Offset 0x0040 I2S/PCM TX3 Channel Select Register */
    volatile uint32_t I2S_PCM_TX0CHMAP0;              /*!< Offset 0x0044 I2S/PCM TX0 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX0CHMAP1;              /*!< Offset 0x0048 I2S/PCM TX0 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX1CHMAP0;              /*!< Offset 0x004C I2S/PCM TX1 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX1CHMAP1;              /*!< Offset 0x0050 I2S/PCM TX1 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX2CHMAP0;              /*!< Offset 0x0054 I2S/PCM TX2 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX2CHMAP1;              /*!< Offset 0x0058 I2S/PCM TX2 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_TX3CHMAP0;              /*!< Offset 0x005C I2S/PCM TX3 Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_TX3CHMAP1;              /*!< Offset 0x0060 I2S/PCM TX3 Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_RXCHSEL;                /*!< Offset 0x0064 I2S/PCM RX Channel Select Register */
    volatile uint32_t I2S_PCM_RXCHMAP0;               /*!< Offset 0x0068 I2S/PCM RX Channel Mapping Register0 */
    volatile uint32_t I2S_PCM_RXCHMAP1;               /*!< Offset 0x006C I2S/PCM RX Channel Mapping Register1 */
    volatile uint32_t I2S_PCM_RXCHMAP2;               /*!< Offset 0x0070 I2S/PCM RX Channel Mapping Register2 */
    volatile uint32_t I2S_PCM_RXCHMAP3;               /*!< Offset 0x0074 I2S/PCM RX Channel Mapping Register3 */
	         uint8_t reserved1 [0x0008];
    volatile uint32_t MCLKCFG;                        /*!< Offset 0x0080 ASRC MCLK Configuration Register */
    volatile uint32_t FsoutCFG;                       /*!< Offset 0x0084 ASRC Out Sample Rate Configuration Register */
    volatile uint32_t FsinEXTCFG;                     /*!< Offset 0x0088 ASRC Input Sample Pulse Extend Configuration Register */
    volatile uint32_t ASRCCFG;                        /*!< Offset 0x008C ASRC Enable Register */
    volatile uint32_t ASRCMANCFG;                     /*!< Offset 0x0090 ASRC Manual Ratio Configuration Register */
    volatile uint32_t ASRCRATIOSTAT;                  /*!< Offset 0x0094 ASRC Status Register */
    volatile uint32_t ASRCFIFOSTAT;                   /*!< Offset 0x0098 ASRC FIFO Level Status Register */
    volatile uint32_t ASRCMBISTCFG;                   /*!< Offset 0x009C ASRC MBIST Test Configuration Register */
    volatile uint32_t ASRCMBISTSTAT;                  /*!< Offset 0x00A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef;
/*
 * @brief DMIC
 */
/*!< DMIC Controller Interface */
typedef struct DMIC_Type
{
    volatile uint32_t DMIC_EN;                        /*!< Offset 0x0000 DMIC Enable Control Register */
    volatile uint32_t DMIC_SR;                        /*!< Offset 0x0004 DMIC Sample Rate Register */
    volatile uint32_t DMIC_CTR;                       /*!< Offset 0x0008 DMIC Control Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t DMIC_DATA;                      /*!< Offset 0x0010 DMIC Data Register */
    volatile uint32_t DMIC_INTC;                      /*!< Offset 0x0014 MIC Interrupt Control Register */
    volatile uint32_t DMIC_INTS;                      /*!< Offset 0x0018 DMIC Interrupt Status Register */
    volatile uint32_t DMIC_RXFIFO_CTR;                /*!< Offset 0x001C DMIC RXFIFO Control Register */
    volatile uint32_t DMIC_RXFIFO_STA;                /*!< Offset 0x0020 DMIC RXFIFO Status Register */
    volatile uint32_t DMIC_CH_NUM;                    /*!< Offset 0x0024 DMIC Channel Numbers Register */
    volatile uint32_t DMIC_CH_MAP;                    /*!< Offset 0x0028 DMIC Channel Mapping Register */
    volatile uint32_t DMIC_CNT;                       /*!< Offset 0x002C DMIC Counter Register */
    volatile uint32_t DATA0_DATA1_VOL_CTR;            /*!< Offset 0x0030 Data0 and Data1 Volume Control Register */
    volatile uint32_t DATA2_DATA3_VOL_CTR;            /*!< Offset 0x0034 Data2 And Data3 Volume Control Register */
    volatile uint32_t HPF_EN_CTR;                     /*!< Offset 0x0038 High Pass Filter Enable Control Register */
    volatile uint32_t HPF_COEF_REG;                   /*!< Offset 0x003C High Pass Filter Coefficient Register */
    volatile uint32_t HPF_GAIN_REG;                   /*!< Offset 0x0040 High Pass Filter Gain Register */
} DMIC_TypeDef;
/*
 * @brief OWA
 */
/*!< OWA Controller Interface */
typedef struct OWA_Type
{
    volatile uint32_t OWA_GEN_CTL;                    /*!< Offset 0x0000 OWA General Control Register */
    volatile uint32_t OWA_TX_CFIG;                    /*!< Offset 0x0004 OWA TX Configuration Register */
    volatile uint32_t OWA_RX_CFIG;                    /*!< Offset 0x0008 OWA RX Configuration Register */
    volatile uint32_t OWA_ISTA;                       /*!< Offset 0x000C OWA Interrupt Status Register */
    volatile uint32_t OWA_RXFIFO;                     /*!< Offset 0x0010 OWA RXFIFO Register */
    volatile uint32_t OWA_FCTL;                       /*!< Offset 0x0014 OWA FIFO Control Register */
    volatile uint32_t OWA_FSTA;                       /*!< Offset 0x0018 OWA FIFO Status Register */
    volatile uint32_t OWA_INT;                        /*!< Offset 0x001C OWA Interrupt Control Register */
    volatile uint32_t OWA_TX_FIFO;                    /*!< Offset 0x0020 OWA TX FIFO Register */
    volatile uint32_t OWA_TX_CNT;                     /*!< Offset 0x0024 OWA TX Counter Register */
    volatile uint32_t OWA_RX_CNT;                     /*!< Offset 0x0028 OWA RX Counter Register */
    volatile uint32_t OWA_TX_CHSTA0;                  /*!< Offset 0x002C OWA TX Channel Status Register0 */
    volatile uint32_t OWA_TX_CHSTA1;                  /*!< Offset 0x0030 OWA TX Channel Status Register1 */
    volatile uint32_t OWA_RXCHSTA0;                   /*!< Offset 0x0034 OWA RX Channel Status Register0 */
    volatile uint32_t OWA_RXCHSTA1;                   /*!< Offset 0x0038 OWA RX Channel Status Register1 */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t OWA_EXP_CTL;                    /*!< Offset 0x0040 OWA Expand Control Register */
    volatile uint32_t OWA_EXP_ISTA;                   /*!< Offset 0x0044 OWA Expand Interrupt Status Register */
    volatile uint32_t OWA_EXP_INFO_0;                 /*!< Offset 0x0048 OWA Expand Infomation Register0 */
    volatile uint32_t OWA_EXP_INFO_1;                 /*!< Offset 0x004C OWA Expand Infomation Register1 */
    volatile uint32_t OWA_EXP_DBG_0;                  /*!< Offset 0x0050 OWA Expand Debug Register0 */
    volatile uint32_t OWA_EXP_DBG_1;                  /*!< Offset 0x0054 OWA Expand Debug Register1 */
} OWA_TypeDef;
/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Controller Interface */
typedef struct AUDIO_CODEC_Type
{
    volatile uint32_t AC_DAC_DPC;                     /*!< Offset 0x0000 DAC Digital Part Control Register */
    volatile uint32_t DAC_VOL_CTRL;                   /*!< Offset 0x0004 DAC Volume Control Register */
	         uint8_t reserved1 [0x0008];
    volatile uint32_t AC_DAC_FIFOC;                   /*!< Offset 0x0010 DAC FIFO Control Register */
    volatile uint32_t AC_DAC_FIFOS;                   /*!< Offset 0x0014 DAC FIFO Status Register */
	         uint8_t reserved2 [0x0008];
    volatile uint32_t AC_DAC_TXDATA;                  /*!< Offset 0x0020 DAC TX DATA Register */
    volatile uint32_t AC_DAC_CNT;                     /*!< Offset 0x0024 DAC TX FIFO Counter Register */
    volatile uint32_t AC_DAC_DG;                      /*!< Offset 0x0028 DAC Debug Register */
	         uint8_t reserved3 [0x0004];
    volatile uint32_t AC_ADC_FIFOC;                   /*!< Offset 0x0030 ADC FIFO Control Register */
    volatile uint32_t ADC_VOL_CTRL1;                  /*!< Offset 0x0034 ADC Volume Control1 Register */
    volatile uint32_t AC_ADC_FIFOS;                   /*!< Offset 0x0038 ADC FIFO Status Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t AC_ADC_RXDATA;                  /*!< Offset 0x0040 ADC RX Data Register */
    volatile uint32_t AC_ADC_CNT;                     /*!< Offset 0x0044 ADC RX Counter Register */
	         uint8_t reserved5 [0x0004];
    volatile uint32_t AC_ADC_DG;                      /*!< Offset 0x004C ADC Debug Register */
    volatile uint32_t ADC_DIG_CTRL;                   /*!< Offset 0x0050 ADC Digtial Control Register */
    volatile uint32_t VRA1SPEEDUP_DOWN_CTRL;          /*!< Offset 0x0054 VRA1 Speedup Down Control Register */
	         uint8_t reserved6 [0x0098];
    volatile uint32_t AC_DAC_DAP_CTRL;                /*!< Offset 0x00F0 DAC DAP Control Register */
	         uint8_t reserved7 [0x0004];
    volatile uint32_t AC_ADC_DAP_CTR;                 /*!< Offset 0x00F8 ADC DAP Control Register */
	         uint8_t reserved8 [0x0004];
    volatile uint32_t AC_DAC_DRC_HHPFC;               /*!< Offset 0x0100 DAC DRC High HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_LHPFC;               /*!< Offset 0x0104 DAC DRC Low HPF Coef Register */
    volatile uint32_t AC_DAC_DRC_CTRL;                /*!< Offset 0x0108 DAC DRC Control Register */
    volatile uint32_t AC_DAC_DRC_LPFHAT;              /*!< Offset 0x010C DAC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLAT;              /*!< Offset 0x0110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHAT;              /*!< Offset 0x0114 DAC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLAT;              /*!< Offset 0x0118 DAC DRC Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFHRT;              /*!< Offset 0x011C DAC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LPFLRT;              /*!< Offset 0x0120 DAC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFHRT;              /*!< Offset 0x0124 DAC DRC Right Peak filter High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_RPFLRT;              /*!< Offset 0x0128 DAC DRC Right Peak filter Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSHAT;             /*!< Offset 0x012C DAC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_LRMSLAT;             /*!< Offset 0x0130 DAC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSHAT;             /*!< Offset 0x0134 DAC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_DAC_DRC_RRMSLAT;             /*!< Offset 0x0138 DAC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_DAC_DRC_HCT;                 /*!< Offset 0x013C DAC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LCT;                 /*!< Offset 0x0140 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_HKC;                 /*!< Offset 0x0144 DAC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKC;                 /*!< Offset 0x0148 DAC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPC;                /*!< Offset 0x014C DAC DRC Compresso */
    volatile uint32_t AC_DAC_DRC_LOPC;                /*!< Offset 0x0150 DAC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_DAC_DRC_HLT;                 /*!< Offset 0x0154 DAC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LLT;                 /*!< Offset 0x0158 DAC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKl;                 /*!< Offset 0x015C DAC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKl;                 /*!< Offset 0x0160 DAC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPL;                /*!< Offset 0x0164 DAC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_LOPL;                /*!< Offset 0x0168 DAC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_DAC_DRC_HET;                 /*!< Offset 0x016C DAC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_DAC_DRC_LET;                 /*!< Offset 0x0170 DAC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HKE;                 /*!< Offset 0x0174 DAC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKE;                 /*!< Offset 0x0178 DAC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_HOPE;                /*!< Offset 0x017C DAC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_LOPE;                /*!< Offset 0x0180 DAC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_DAC_DRC_HKN;                 /*!< Offset 0x0184 DAC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_DAC_DRC_LKN;                 /*!< Offset 0x0188 DAC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_DAC_DRC_SFHAT;               /*!< Offset 0x018C DAC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLAT;               /*!< Offset 0x0190 DAC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFHRT;               /*!< Offset 0x0194 DAC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_SFLRT;               /*!< Offset 0x0198 DAC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_DAC_DRC_MXGHS;               /*!< Offset 0x019C DAC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MXGLS;               /*!< Offset 0x01A0 DAC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGHS;               /*!< Offset 0x01A4 DAC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_DAC_DRC_MNGLS;               /*!< Offset 0x01A8 DAC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_DAC_DRC_EPSHC;               /*!< Offset 0x01AC DAC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_DAC_DRC_EPSLC;               /*!< Offset 0x01B0 DAC DRC Expander Smooth Time Low Coef Register */
	         uint8_t reserved9 [0x0004];
    volatile uint32_t AC_DAC_DRC_HPFHGAIN;            /*!< Offset 0x01B8 DAC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_DAC_DRC_HPFLGAIN;            /*!< Offset 0x01BC DAC DRC HPF Gain Low Coef Register */
	         uint8_t reserved10 [0x0040];
    volatile uint32_t AC_ADC_DRC_HHPFC;               /*!< Offset 0x0200 ADC DRC High HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_LHPFC;               /*!< Offset 0x0204 ADC DRC Low HPF Coef Register */
    volatile uint32_t AC_ADC_DRC_CTRL;                /*!< Offset 0x0208 ADC DRC Control Register */
    volatile uint32_t AC_ADC_DRC_LPFHAT;              /*!< Offset 0x020C ADC DRC Left Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLAT;              /*!< Offset 0x0210 ADC DRC Left Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHAT;              /*!< Offset 0x0214 ADC DRC Right Peak Filter High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLAT;              /*!< Offset 0x0218 ADC DRC Right Peak Filter Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFHRT;              /*!< Offset 0x021C ADC DRC Left Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LPFLRT;              /*!< Offset 0x0220 ADC DRC Left Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFHRT;              /*!< Offset 0x0224 ADC DRC Right Peak Filter High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_RPFLRT;              /*!< Offset 0x0228 ADC DRC Right Peak Filter Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSHAT;             /*!< Offset 0x022C ADC DRC Left RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_LRMSLAT;             /*!< Offset 0x0230 ADC DRC Left RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSHAT;             /*!< Offset 0x0234 ADC DRC Right RMS Filter High Coef Register */
    volatile uint32_t AC_ADC_DRC_RRMSLAT;             /*!< Offset 0x0238 ADC DRC Right RMS Filter Low Coef Register */
    volatile uint32_t AC_ADC_DRC_HCT;                 /*!< Offset 0x023C ADC DRC Compressor Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LCT;                 /*!< Offset 0x0240 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_HKC;                 /*!< Offset 0x0244 ADC DRC Compressor Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKC;                 /*!< Offset 0x0248 ADC DRC Compressor Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPC;                /*!< Offset 0x024C ADC DRC Compressor High Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_LOPC;                /*!< Offset 0x0250 ADC DRC Compressor Low Output at Compressor Threshold Register */
    volatile uint32_t AC_ADC_DRC_HLT;                 /*!< Offset 0x0254 ADC DRC Limiter Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LLT;                 /*!< Offset 0x0258 ADC DRC Limiter Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKl;                 /*!< Offset 0x025C ADC DRC Limiter Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKl;                 /*!< Offset 0x0260 ADC DRC Limiter Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPL;                /*!< Offset 0x0264 ADC DRC Limiter High Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_LOPL;                /*!< Offset 0x0268 ADC DRC Limiter Low Output at Limiter Threshold */
    volatile uint32_t AC_ADC_DRC_HET;                 /*!< Offset 0x026C ADC DRC Expander Threshold High Setting Register */
    volatile uint32_t AC_ADC_DRC_LET;                 /*!< Offset 0x0270 ADC DRC Expander Threshold Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HKE;                 /*!< Offset 0x0274 ADC DRC Expander Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKE;                 /*!< Offset 0x0278 ADC DRC Expander Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_HOPE;                /*!< Offset 0x027C ADC DRC Expander High Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_LOPE;                /*!< Offset 0x0280 ADC DRC Expander Low Output at Expander Threshold */
    volatile uint32_t AC_ADC_DRC_HKN;                 /*!< Offset 0x0284 ADC DRC Linear Slope High Setting Register */
    volatile uint32_t AC_ADC_DRC_LKN;                 /*!< Offset 0x0288 ADC DRC Linear Slope Low Setting Register */
    volatile uint32_t AC_ADC_DRC_SFHAT;               /*!< Offset 0x028C ADC DRC Smooth filter Gain High Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLAT;               /*!< Offset 0x0290 ADC DRC Smooth filter Gain Low Attack Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFHRT;               /*!< Offset 0x0294 ADC DRC Smooth filter Gain High Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_SFLRT;               /*!< Offset 0x0298 ADC DRC Smooth filter Gain Low Release Time Coef Register */
    volatile uint32_t AC_ADC_DRC_MXGHS;               /*!< Offset 0x029C ADC DRC MAX Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MXGLS;               /*!< Offset 0x02A0 ADC DRC MAX Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGHS;               /*!< Offset 0x02A4 ADC DRC MIN Gain High Setting Register */
    volatile uint32_t AC_ADC_DRC_MNGLS;               /*!< Offset 0x02A8 ADC DRC MIN Gain Low Setting Register */
    volatile uint32_t AC_ADC_DRC_EPSHC;               /*!< Offset 0x02AC ADC DRC Expander Smooth Time High Coef Register */
    volatile uint32_t AC_ADC_DRC_EPSLC;               /*!< Offset 0x02B0 ADC DRC Expander Smooth Time Low Coef Register */
	         uint8_t reserved11 [0x0004];
    volatile uint32_t AC_ADC_DRC_HPFHGAIN;            /*!< Offset 0x02B8 ADC DRC HPF Gain High Coef Register */
    volatile uint32_t AC_ADC_DRC_HPFLGAIN;            /*!< Offset 0x02BC ADC DRC HPF Gain Low Coef Register */
	         uint8_t reserved12 [0x0040];
    volatile uint32_t ADC1_REG;                       /*!< Offset 0x0300 ADC1 Analog Control Register */
    volatile uint32_t ADC2_REG;                       /*!< Offset 0x0304 ADC2 Analog Control Register */
    volatile uint32_t ADC3_REG;                       /*!< Offset 0x0308 ADC3 Analog Control Register */
	         uint8_t reserved13 [0x0004];
    volatile uint32_t DAC_REG;                        /*!< Offset 0x0310 DAC Analog Control Register */
	         uint8_t reserved14 [0x0004];
    volatile uint32_t MICBIAS_REG;                    /*!< Offset 0x0318 MICBIAS Analog Control Register */
    volatile uint32_t RAMP_REG;                       /*!< Offset 0x031C BIAS Analog Control Register */
    volatile uint32_t BIAS_REG;                       /*!< Offset 0x0320 BIAS Analog Control Register */
	         uint8_t reserved15 [0x000C];
    volatile uint32_t ADC5_REG;                       /*!< Offset 0x0330 ADC5 Analog Control Register */
} AUDIO_CODEC_TypeDef;
/*
 * @brief TWI
 */
/*!< TWI Controller Interface */
typedef struct TWI_Type
{
    volatile uint32_t TWI_ADDR;                       /*!< Offset 0x0000 TWI Slave Address Register */
    volatile uint32_t TWI_XADDR;                      /*!< Offset 0x0004 TWI Extended Slave Address Register */
    volatile uint32_t TWI_DATA;                       /*!< Offset 0x0008 TWI Data Byte Register */
    volatile uint32_t TWI_CNTR;                       /*!< Offset 0x000C TWI Control Register */
    volatile uint32_t TWI_STAT;                       /*!< Offset 0x0010 TWI Status Register */
    volatile uint32_t TWI_CCR;                        /*!< Offset 0x0014 TWI Clock Control Register */
    volatile uint32_t TWI_SRST;                       /*!< Offset 0x0018 TWI Software Reset Register */
    volatile uint32_t TWI_EFR;                        /*!< Offset 0x001C TWI Enhance Feature Register */
    volatile uint32_t TWI_LCR;                        /*!< Offset 0x0020 TWI Line Control Register */
	         uint8_t reserved1 [0x01DC];
    volatile uint32_t TWI_DRV_CTRL;                   /*!< Offset 0x0200 TWI_DRV Control Register */
    volatile uint32_t TWI_DRV_CFG;                    /*!< Offset 0x0204 TWI_DRV Transmission Configuration Register */
    volatile uint32_t TWI_DRV_SLV;                    /*!< Offset 0x0208 TWI_DRV Slave ID Register */
    volatile uint32_t TWI_DRV_FMT;                    /*!< Offset 0x020C TWI_DRV Packet Format Register */
    volatile uint32_t TWI_DRV_BUS_CTRL;               /*!< Offset 0x0210 TWI_DRV Bus Control Register */
    volatile uint32_t TWI_DRV_INT_CTRL;               /*!< Offset 0x0214 TWI_DRV Interrupt Control Register */
    volatile uint32_t TWI_DRV_DMA_CFG;                /*!< Offset 0x0218 TWI_DRV DMA Configure Register */
    volatile uint32_t TWI_DRV_FIFO_CON;               /*!< Offset 0x021C TWI_DRV FIFO Content Register */
	         uint8_t reserved2 [0x00E0];
    volatile uint32_t TWI_DRV_SEND_FIFO_ACC;          /*!< Offset 0x0300 TWI_DRV Send Data FIFO Access Register */
    volatile uint32_t TWI_DRV_RECV_FIFO_ACC;          /*!< Offset 0x0304 TWI_DRV Receive Data FIFO Access Register */
} TWI_TypeDef;
/*
 * @brief SPI
 */
/*!< SPI Controller Interface */
typedef struct SPI_Type
{
	         uint8_t reserved1 [0x0004];
    volatile uint32_t SPI_GCR;                        /*!< Offset 0x0004 SPI Global Control Register */
    volatile uint32_t SPI_TCR;                        /*!< Offset 0x0008 SPI Transfer Control Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t SPI_IER;                        /*!< Offset 0x0010 SPI Interrupt Control Register */
    volatile uint32_t SPI_ISR;                        /*!< Offset 0x0014 SPI Interrupt Status Register */
    volatile uint32_t SPI_FCR;                        /*!< Offset 0x0018 SPI FIFO Control Register */
    volatile uint32_t SPI_FSR;                        /*!< Offset 0x001C SPI FIFO Status Register */
    volatile uint32_t SPI_WCR;                        /*!< Offset 0x0020 SPI Wait Clock Register */
	         uint8_t reserved3 [0x0004];
    volatile uint32_t SPI_SAMP_DL;                    /*!< Offset 0x0028 SPI Sample Delay Control Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t SPI_MBC;                        /*!< Offset 0x0030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                        /*!< Offset 0x0034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                        /*!< Offset 0x0038 SPI Master Burst Control Register */
	         uint8_t reserved5 [0x0004];
    volatile uint32_t SPI_BATCR;                      /*!< Offset 0x0040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_BA_CCR;                     /*!< Offset 0x0044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                        /*!< Offset 0x0048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                        /*!< Offset 0x004C SPI RX Bit Register */
	         uint8_t reserved6 [0x0038];
    volatile uint32_t SPI_NDMA_MODE_CTL;              /*!< Offset 0x0088 SPI Normal DMA Mode Control Register */
	         uint8_t reserved7 [0x0174];
    volatile uint32_t SPI_TXD;                        /*!< Offset 0x0200 SPI TX Data Register */
	         uint8_t reserved8 [0x00FC];
    volatile uint32_t SPI_RXD;                        /*!< Offset 0x0300 SPI RX Data Register */
} SPI_TypeDef;
/*
 * @brief CIR_RX
 */
/*!< CIR_RX Controller Interface */
typedef struct CIR_RX_Type
{
    volatile uint32_t CIR_CTL;                        /*!< Offset 0x0000 CIR Control Register */
	         uint8_t reserved1 [0x000C];
    volatile uint32_t CIR_RXPCFG;                     /*!< Offset 0x0010 CIR Receiver Pulse Configure Register */
	         uint8_t reserved2 [0x000C];
    volatile uint32_t CIR_RXFIFO;                     /*!< Offset 0x0020 CIR Receiver FIFO Register */
	         uint8_t reserved3 [0x0008];
    volatile uint32_t CIR_RXINT;                      /*!< Offset 0x002C CIR Receiver Interrupt Control Register */
    volatile uint32_t CIR_RXSTA;                      /*!< Offset 0x0030 CIR Receiver Status Register */
    volatile uint32_t CIR_RXCFG;                      /*!< Offset 0x0034 CIR Receiver Configure Register */
} CIR_RX_TypeDef;
/*
 * @brief CIR_TX
 */
/*!< CIR_TX Controller Interface */
typedef struct CIR_TX_Type
{
    volatile uint32_t CIR_TGLR;                       /*!< Offset 0x0000 CIR Transmit Global Register */
    volatile uint32_t CIR_TMCR;                       /*!< Offset 0x0004 CIR Transmit Modulation Control Register */
    volatile uint32_t CIR_TCR;                        /*!< Offset 0x0008 CIR Transmit Control Register */
    volatile uint32_t CIR_IDC_H;                      /*!< Offset 0x000C CIR Transmit Idle Duration Threshold High Bit Register */
    volatile uint32_t CIR_IDC_L;                      /*!< Offset 0x0010 CIR Transmit Idle Duration Threshold Low Bit Register */
    volatile uint32_t CIR_TICR_H;                     /*!< Offset 0x0014 CIR Transmit Idle Counter High Bit Register */
    volatile uint32_t CIR_TICR_L;                     /*!< Offset 0x0018 CIR Transmit Idle Counter Low Bit Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t CIR_TEL;                        /*!< Offset 0x0020 CIR TX FIFO Empty Level Register */
    volatile uint32_t CIR_TXINT;                      /*!< Offset 0x0024 CIR Transmit Interrupt Control Register */
    volatile uint32_t CIR_TAC;                        /*!< Offset 0x0028 CIR Transmit FIFO Available Counter Register */
    volatile uint32_t CIR_TXSTA;                      /*!< Offset 0x002C CIR Transmit Status Register */
    volatile uint32_t CIR_TXT;                        /*!< Offset 0x0030 CIR Transmit Threshold Register */
    volatile uint32_t CIR_DMA;                        /*!< Offset 0x0034 CIR DMA Control Register */
	         uint8_t reserved2 [0x0048];
    volatile uint32_t CIR_TXFIFO;                     /*!< Offset 0x0080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef;
/*
 * @brief LEDC
 */
/*!< LEDC Controller Interface */
typedef struct LEDC_Type
{
    volatile uint32_t LEDC_CTRL_REG;                  /*!< Offset 0x0000 LEDC Control Register */
    volatile uint32_t LED_T01_TIMING_CTRL_REG;        /*!< Offset 0x0004 LEDC T0 & T1 Timing Control Register */
    volatile uint32_t LEDC_DATA_FINISH_CNT_REG;       /*!< Offset 0x0008 LEDC Data Finish Counter Register */
    volatile uint32_t LED_RESET_TIMING_CTRL_REG;      /*!< Offset 0x000C LEDC Reset Timing Control Register */
    volatile uint32_t LEDC_WAIT_TIME0_CTRL_REG;       /*!< Offset 0x0010 LEDC Wait Time0 Control Register */
    volatile uint32_t LEDC_DATA_REG;                  /*!< Offset 0x0014 LEDC Data Register */
    volatile uint32_t LEDC_DMA_CTRL_REG;              /*!< Offset 0x0018 LEDC DMA Control Register */
    volatile uint32_t LEDC_INT_CTRL_REG;              /*!< Offset 0x001C LEDC Interrupt Control Register */
    volatile uint32_t LEDC_INT_STS_REG;               /*!< Offset 0x0020 LEDC Interrupt Status Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t LEDC_WAIT_TIME1_CTRL_REG;       /*!< Offset 0x0028 LEDC Wait Time1 Control Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t LEDC_FIFO_DATA_REG [0x020];     /*!< Offset 0x0030 LEDC FIFO Data Registers array */
} LEDC_TypeDef;
/*
 * @brief TPADC
 */
/*!< TPADC Controller Interface */
typedef struct TPADC_Type
{
    volatile uint32_t TP_CTRL_REG0;                   /*!< Offset 0x0000 TP Control Register 0 */
    volatile uint32_t TP_CTRL_REG1;                   /*!< Offset 0x0004 TP Control Register 1 */
    volatile uint32_t TP_CTRL_REG2;                   /*!< Offset 0x0008 TP Control Register 2 */
    volatile uint32_t TP_CTRL_REG3;                   /*!< Offset 0x000C TP Control Register 3 */
    volatile uint32_t TP_INT_FIFO_CTRL_REG;           /*!< Offset 0x0010 TP Interrupt FIFO Control Register */
    volatile uint32_t TP_INT_FIFO_STAT_REG;           /*!< Offset 0x0014 TP Interrupt FIFO Status Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t TP_CALI_DATA_REG;               /*!< Offset 0x001C TP Calibration Data Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t TP_DATA_REG;                    /*!< Offset 0x0024 TP Data Register */
} TPADC_TypeDef;
/*
 * @brief GPADC
 */
/*!< GPADC Controller Interface */
typedef struct GPADC_Type
{
    volatile uint32_t GP_SR_CON;                      /*!< Offset 0x0000 GPADC Sample Rate Configure Register */
    volatile uint32_t GP_CTRL;                        /*!< Offset 0x0004 GPADC Control Register */
    volatile uint32_t GP_CS_EN;                       /*!< Offset 0x0008 GPADC Compare and Select Enable Register */
    volatile uint32_t GP_FIFO_INTC;                   /*!< Offset 0x000C GPADC FIFO Interrupt Control Register */
    volatile uint32_t GP_FIFO_INTS;                   /*!< Offset 0x0010 GPADC FIFO Interrupt Status Register */
    volatile uint32_t GP_FIFO_DATA;                   /*!< Offset 0x0014 GPADC FIFO Data Register */
    volatile uint32_t GP_CDATA;                       /*!< Offset 0x0018 GPADC Calibration Data Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t GP_DATAL_INTC;                  /*!< Offset 0x0020 GPADC Data Low Interrupt Configure Register */
    volatile uint32_t GP_DATAH_INTC;                  /*!< Offset 0x0024 GPADC Data High Interrupt Configure Register */
    volatile uint32_t GP_DATA_INTC;                   /*!< Offset 0x0028 GPADC Data Interrupt Configure Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t GP_DATAL_INTS;                  /*!< Offset 0x0030 GPADC Data Low Interrupt Status Register */
    volatile uint32_t GP_DATAH_INTS;                  /*!< Offset 0x0034 GPADC Data High Interrupt Status Register */
    volatile uint32_t GP_DATA_INTS;                   /*!< Offset 0x0038 GPADC Data Interrupt Status Register */
	         uint8_t reserved3 [0x0004];
    volatile uint32_t GP_CH0_CMP_DATA;                /*!< Offset 0x0040 GPADC CH0 Compare Data Register */
	         uint8_t reserved4 [0x003C];
    volatile uint32_t GP_CH0_DATA;                    /*!< Offset 0x0080 GPADC CH0 Data Register */
} GPADC_TypeDef;
/*
 * @brief SPI_DBI
 */
/*!< SPI_DBI Controller Interface */
typedef struct SPI_DBI_Type
{
	         uint8_t reserved1 [0x0004];
    volatile uint32_t SPI_GCR;                        /*!< Offset 0x0004 SPI Global Control Register */
    volatile uint32_t SPI_TCR;                        /*!< Offset 0x0008 SPI Transfer Control Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t SPI_IER;                        /*!< Offset 0x0010 SPI Interrupt Control Register */
    volatile uint32_t SPI_ISR;                        /*!< Offset 0x0014 SPI Interrupt Status Register */
    volatile uint32_t SPI_FCR;                        /*!< Offset 0x0018 SPI FIFO Control Register */
    volatile uint32_t SPI_FSR;                        /*!< Offset 0x001C SPI FIFO Status Register */
    volatile uint32_t SPI_WCR;                        /*!< Offset 0x0020 SPI Wait Clock Register */
	         uint8_t reserved3 [0x0004];
    volatile uint32_t SPI_SAMP_DL;                    /*!< Offset 0x0028 SPI Sample Delay Control Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t SPI_MBC;                        /*!< Offset 0x0030 SPI Master Burst Counter Register */
    volatile uint32_t SPI_MTC;                        /*!< Offset 0x0034 SPI Master Transmit Counter Register */
    volatile uint32_t SPI_BCC;                        /*!< Offset 0x0038 SPI Master Burst Control Register */
	         uint8_t reserved5 [0x0004];
    volatile uint32_t SPI_BATCR;                      /*!< Offset 0x0040 SPI Bit-Aligned Transfer Configure Register */
    volatile uint32_t SPI_BA_CCR;                     /*!< Offset 0x0044 SPI Bit-Aligned Clock Configuration Register */
    volatile uint32_t SPI_TBR;                        /*!< Offset 0x0048 SPI TX Bit Register */
    volatile uint32_t SPI_RBR;                        /*!< Offset 0x004C SPI RX Bit Register */
	         uint8_t reserved6 [0x0038];
    volatile uint32_t SPI_NDMA_MODE_CTL;              /*!< Offset 0x0088 SPI Normal DMA Mode Control Register */
	         uint8_t reserved7 [0x0074];
    volatile uint32_t DBI_CTL_0;                      /*!< Offset 0x0100 DBI Control Register 0 */
    volatile uint32_t DBI_CTL_1;                      /*!< Offset 0x0104 DBI Control Register 1 */
    volatile uint32_t DBI_CTL_2;                      /*!< Offset 0x0108 DBI Control Register 2 */
    volatile uint32_t DBI_TIMER;                      /*!< Offset 0x010C DBI Timer Control Register */
    volatile uint32_t DBI_VIDEO_SZIE;                 /*!< Offset 0x0110 DBI Video Size Configuration Register */
	         uint8_t reserved8 [0x000C];
    volatile uint32_t DBI_INT;                        /*!< Offset 0x0120 DBI Interrupt Register */
    volatile uint32_t DBI_DEBUG_0;                    /*!< Offset 0x0124 DBI BEBUG 0 Register */
    volatile uint32_t DBI_DEBUG_1;                    /*!< Offset 0x0128 DBI BEBUG 1 Register */
	         uint8_t reserved9 [0x00D4];
    volatile uint32_t SPI_TXD;                        /*!< Offset 0x0200 SPI TX Data register */
	         uint8_t reserved10 [0x00FC];
    volatile uint32_t SPI_RXD;                        /*!< Offset 0x0300 SPI RX Data register */
} SPI_DBI_TypeDef;
/*
 * @brief CE_S
 */
/*!< CE_S Controller Interface */
typedef struct CE_S_Type
{
    volatile uint32_t CE_TDA;                         /*!< Offset 0x0000 Task Descriptor Address */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t CE_ICR;                         /*!< Offset 0x0008 Interrupt Control Register */
    volatile uint32_t CE_ISR;                         /*!< Offset 0x000C Interrupt Status Register */
    volatile uint32_t CE_TLR;                         /*!< Offset 0x0010 Task Load Register */
    volatile uint32_t CE_TSR;                         /*!< Offset 0x0014 Task Status Register */
    volatile uint32_t CE_ESR;                         /*!< Offset 0x0018 Error Status Register */
	         uint8_t reserved2 [0x0008];
    volatile uint32_t CE_CSA;                         /*!< Offset 0x0024 DMA Current Source Address */
    volatile uint32_t CE_CDA;                         /*!< Offset 0x0028 DMA Current Destination Address */
    volatile uint32_t CE_TPR;                         /*!< Offset 0x002C Throughput Register */
} CE_S_TypeDef;
/*
 * @brief RTC
 */
/*!< RTC Controller Interface */
typedef struct RTC_Type
{
    volatile uint32_t LOSC_CTRL_REG;                  /*!< Offset 0x0000 Low Oscillator Control Register */
    volatile uint32_t LOSC_AUTO_SWT_STA_REG;          /*!< Offset 0x0004 LOSC Auto Switch Status Register */
    volatile uint32_t INTOSC_CLK_PRESCAL_REG;         /*!< Offset 0x0008 Internal OSC Clock Pre-scalar Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t RTC_DAY_REG;                    /*!< Offset 0x0010 RTC Year-Month-Day Register */
    volatile uint32_t RTC_HH_MM_SS_REG;               /*!< Offset 0x0014 RTC Hour-Minute-Second Register */
	         uint8_t reserved2 [0x0008];
    volatile uint32_t ALARM0_DAY_SET_REG;             /*!< Offset 0x0020 Alarm 0 Day Setting Register */
    volatile uint32_t ALARM0_CUR_VLU_REG;             /*!< Offset 0x0024 Alarm 0 Counter Current Value Register */
    volatile uint32_t ALARM0_ENABLE_REG;              /*!< Offset 0x0028 Alarm 0 Enable Register */
    volatile uint32_t ALARM0_IRQ_EN;                  /*!< Offset 0x002C Alarm 0 IRQ Enable Register */
    volatile uint32_t ALARM0_IRQ_STA_REG;             /*!< Offset 0x0030 Alarm 0 IRQ Status Register */
	         uint8_t reserved3 [0x001C];
    volatile uint32_t ALARM_CONFIG_REG;               /*!< Offset 0x0050 Alarm Configuration Register */
	         uint8_t reserved4 [0x000C];
    volatile uint32_t F32K_FOUT_CTRL_GATING_REG;      /*!< Offset 0x0060 32K Fanout Control Gating Register */
	         uint8_t reserved5 [0x009C];
    volatile uint32_t GP_DATA_REG [0x008];            /*!< Offset 0x0100 General Purpose Register (N=0 to 7) */
    volatile uint32_t FBOOT_INFO_REG0;                /*!< Offset 0x0120 Fast Boot Information Register0 */
    volatile uint32_t FBOOT_INFO_REG1;                /*!< Offset 0x0124 Fast Boot Information Register1 */
	         uint8_t reserved6 [0x0038];
    volatile uint32_t DCXO_CTRL_REG;                  /*!< Offset 0x0160 DCXO Control Register */
	         uint8_t reserved7 [0x002C];
    volatile uint32_t RTC_VIO_REG;                    /*!< Offset 0x0190 RTC_VIO Regulation Register */
	         uint8_t reserved8 [0x005C];
    volatile uint32_t IC_CHARA_REG;                   /*!< Offset 0x01F0 IC Characteristic Register */
    volatile uint32_t VDD_OFF_GATING_CTRL_REG;        /*!< Offset 0x01F4 VDD Off Gating Control Register */
	         uint8_t reserved9 [0x000C];
    volatile uint32_t EFUSE_HV_PWRSWT_CTRL_REG;       /*!< Offset 0x0204 Efuse High Voltage Power Switch Control Register */
	         uint8_t reserved10 [0x0108];
    volatile uint32_t RTC_SPI_CLK_CTRL_REG;           /*!< Offset 0x0310 RTC SPI Clock Control Register */
} RTC_TypeDef;
/*
 * @brief IOMMU
 */
/*!< IOMMU Controller Interface */
typedef struct IOMMU_Type
{
	         uint8_t reserved1 [0x0010];
    volatile uint32_t IOMMU_RESET_REG;                /*!< Offset 0x0010 IOMMU Reset Register */
	         uint8_t reserved2 [0x000C];
    volatile uint32_t IOMMU_ENABLE_REG;               /*!< Offset 0x0020 IOMMU Enable Register */
	         uint8_t reserved3 [0x000C];
    volatile uint32_t IOMMU_BYPASS_REG;               /*!< Offset 0x0030 IOMMU Bypass Register */
	         uint8_t reserved4 [0x000C];
    volatile uint32_t IOMMU_AUTO_GATING_REG;          /*!< Offset 0x0040 IOMMU Auto Gating Register */
    volatile uint32_t IOMMU_WBUF_CTRL_REG;            /*!< Offset 0x0044 IOMMU Write Buffer Control Register */
    volatile uint32_t IOMMU_OOO_CTRL_REG;             /*!< Offset 0x0048 IOMMU Out of Order Control Register */
    volatile uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;     /*!< Offset 0x004C IOMMU 4KB Boundary Protect Control Register */
    volatile uint32_t IOMMU_TTB_REG;                  /*!< Offset 0x0050 IOMMU Translation Table Base Register */
	         uint8_t reserved5 [0x000C];
    volatile uint32_t IOMMU_TLB_ENABLE_REG;           /*!< Offset 0x0060 IOMMU TLB Enable Register */
	         uint8_t reserved6 [0x000C];
    volatile uint32_t IOMMU_TLB_PREFETCH_REG;         /*!< Offset 0x0070 IOMMU TLB Prefetch Register */
	         uint8_t reserved7 [0x000C];
    volatile uint32_t IOMMU_TLB_FLUSH_ENABLE_REG;     /*!< Offset 0x0080 IOMMU TLB Flush Enable Register */
    volatile uint32_t IOMMU_TLB_IVLD_MODE_SEL_REG;    /*!< Offset 0x0084 IOMMU TLB Invalidation Mode Select Register */
    volatile uint32_t IOMMU_TLB_IVLD_STA_ADDR_REG;    /*!< Offset 0x0088 IOMMU TLB Invalidation Start Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_END_ADDR_REG;    /*!< Offset 0x008C IOMMU TLB Invalidation End Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_REG;        /*!< Offset 0x0090 IOMMU TLB Invalidation Address Register */
    volatile uint32_t IOMMU_TLB_IVLD_ADDR_MASK_REG;   /*!< Offset 0x0094 IOMMU TLB Invalidation Address Mask Register */
    volatile uint32_t IOMMU_TLB_IVLD_ENABLE_REG;      /*!< Offset 0x0098 IOMMU TLB Invalidation Enable Register */
    volatile uint32_t IOMMU_PC_IVLD_MODE_SEL_REG;     /*!< Offset 0x009C IOMMU PC Invalidation Mode Select Register */
    volatile uint32_t IOMMU_PC_IVLD_ADDR_REG;         /*!< Offset 0x00A0 IOMMU PC Invalidation Address Register */
    volatile uint32_t IOMMU_PC_IVLD_STA_ADDR_REG;     /*!< Offset 0x00A4 IOMMU PC Invalidation Start Address Register */
    volatile uint32_t IOMMU_PC_IVLD_ENABLE_REG;       /*!< Offset 0x00A8 IOMMU PC Invalidation Enable Register */
    volatile uint32_t IOMMU_PC_IVLD_END_ADDR_REG;     /*!< Offset 0x00AC IOMMU PC Invalidation End Address Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL0_REG;         /*!< Offset 0x00B0 IOMMU Domain Authority Control 0 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL1_REG;         /*!< Offset 0x00B4 IOMMU Domain Authority Control 1 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL2_REG;         /*!< Offset 0x00B8 IOMMU Domain Authority Control 2 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL3_REG;         /*!< Offset 0x00BC IOMMU Domain Authority Control 3 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL4_REG;         /*!< Offset 0x00C0 IOMMU Domain Authority Control 4 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL5_REG;         /*!< Offset 0x00C4 IOMMU Domain Authority Control 5 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL6_REG;         /*!< Offset 0x00C8 IOMMU Domain Authority Control 6 Register */
    volatile uint32_t IOMMU_DM_AUT_CTRL7_REG;         /*!< Offset 0x00CC IOMMU Domain Authority Control 7 Register */
    volatile uint32_t IOMMU_DM_AUT_OVWT_REG;          /*!< Offset 0x00D0 IOMMU Domain Authority Overwrite Register */
	         uint8_t reserved8 [0x002C];
    volatile uint32_t IOMMU_INT_ENABLE_REG;           /*!< Offset 0x0100 IOMMU Interrupt Enable Register */
    volatile uint32_t IOMMU_INT_CLR_REG;              /*!< Offset 0x0104 IOMMU Interrupt Clear Register */
    volatile uint32_t IOMMU_INT_STA_REG;              /*!< Offset 0x0108 IOMMU Interrupt Status Register */
	         uint8_t reserved9 [0x0004];
    volatile uint32_t IOMMU_INT_ERR_ADDR0_REG;        /*!< Offset 0x0110 IOMMU Interrupt Error Address 0 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR1_REG;        /*!< Offset 0x0114 IOMMU Interrupt Error Address 1 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR2_REG;        /*!< Offset 0x0118 IOMMU Interrupt Error Address 2 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR3_REG;        /*!< Offset 0x011C IOMMU Interrupt Error Address 3 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR4_REG;        /*!< Offset 0x0120 IOMMU Interrupt Error Address 4 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR5_REG;        /*!< Offset 0x0124 IOMMU Interrupt Error Address 5 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR6_REG;        /*!< Offset 0x0128 IOMMU Interrupt Error Address 6 Register */
	         uint8_t reserved10 [0x0004];
    volatile uint32_t IOMMU_INT_ERR_ADDR7_REG;        /*!< Offset 0x0130 IOMMU Interrupt Error Address 7 Register */
    volatile uint32_t IOMMU_INT_ERR_ADDR8_REG;        /*!< Offset 0x0134 IOMMU Interrupt Error Address 8 Register */
	         uint8_t reserved11 [0x0018];
    volatile uint32_t IOMMU_INT_ERR_DATA0_REG;        /*!< Offset 0x0150 IOMMU Interrupt Error Data 0 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA1_REG;        /*!< Offset 0x0154 IOMMU Interrupt Error Data 1 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA2_REG;        /*!< Offset 0x0158 IOMMU Interrupt Error Data 2 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA3_REG;        /*!< Offset 0x015C IOMMU Interrupt Error Data 3 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA4_REG;        /*!< Offset 0x0160 IOMMU Interrupt Error Data 4 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA5_REG;        /*!< Offset 0x0164 IOMMU Interrupt Error Data 5 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA6_REG;        /*!< Offset 0x0168 IOMMU Interrupt Error Data 6 Register */
	         uint8_t reserved12 [0x0004];
    volatile uint32_t IOMMU_INT_ERR_DATA7_REG;        /*!< Offset 0x0170 IOMMU Interrupt Error Data 7 Register */
    volatile uint32_t IOMMU_INT_ERR_DATA8_REG;        /*!< Offset 0x0174 IOMMU Interrupt Error Data 8 Register */
	         uint8_t reserved13 [0x0008];
    volatile uint32_t IOMMU_L1PG_INT_REG;             /*!< Offset 0x0180 IOMMU L1 Page Table Interrupt Register */
    volatile uint32_t IOMMU_L2PG_INT_REG;             /*!< Offset 0x0184 IOMMU L2 Page Table Interrupt Register */
	         uint8_t reserved14 [0x0008];
    volatile uint32_t IOMMU_VA_REG;                   /*!< Offset 0x0190 IOMMU Virtual Address Register */
    volatile uint32_t IOMMU_VA_DATA_REG;              /*!< Offset 0x0194 IOMMU Virtual Address Data Register */
    volatile uint32_t IOMMU_VA_CONFIG_REG;            /*!< Offset 0x0198 IOMMU Virtual Address Configuration Register */
	         uint8_t reserved15 [0x0064];
    volatile uint32_t IOMMU_PMU_ENABLE_REG;           /*!< Offset 0x0200 IOMMU PMU Enable Register */
	         uint8_t reserved16 [0x000C];
    volatile uint32_t IOMMU_PMU_CLR_REG;              /*!< Offset 0x0210 IOMMU PMU Clear Register */
	         uint8_t reserved17 [0x001C];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW0_REG;      /*!< Offset 0x0230 IOMMU PMU Access Low 0 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH0_REG;     /*!< Offset 0x0234 IOMMU PMU Access High 0 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW0_REG;         /*!< Offset 0x0238 IOMMU PMU Hit Low 0 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH0_REG;        /*!< Offset 0x023C IOMMU PMU Hit High 0 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW1_REG;      /*!< Offset 0x0240 IOMMU PMU Access Low 1 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH1_REG;     /*!< Offset 0x0244 IOMMU PMU Access High 1 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW1_REG;         /*!< Offset 0x0248 IOMMU PMU Hit Low 1 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH1_REG;        /*!< Offset 0x024C IOMMU PMU Hit High 1 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW2_REG;      /*!< Offset 0x0250 IOMMU PMU Access Low 2 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH2_REG;     /*!< Offset 0x0254 IOMMU PMU Access High 2 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW2_REG;         /*!< Offset 0x0258 IOMMU PMU Hit Low 2 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH2_REG;        /*!< Offset 0x025C IOMMU PMU Hit High 2 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW3_REG;      /*!< Offset 0x0260 IOMMU PMU Access Low 3 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH3_REG;     /*!< Offset 0x0264 IOMMU PMU Access High 3 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW3_REG;         /*!< Offset 0x0268 IOMMU PMU Hit Low 3 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH3_REG;        /*!< Offset 0x026C IOMMU PMU Hit High 3 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW4_REG;      /*!< Offset 0x0270 IOMMU PMU Access Low 4 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH4_REG;     /*!< Offset 0x0274 IOMMU PMU Access High 4 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW4_REG;         /*!< Offset 0x0278 IOMMU PMU Hit Low 4 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH4_REG;        /*!< Offset 0x027C IOMMU PMU Hit High 4 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW5_REG;      /*!< Offset 0x0280 IOMMU PMU Access Low 5 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH5_REG;     /*!< Offset 0x0284 IOMMU PMU Access High 5 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW5_REG;         /*!< Offset 0x0288 IOMMU PMU Hit Low 5 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH5_REG;        /*!< Offset 0x028C IOMMU PMU Hit High 5 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW6_REG;      /*!< Offset 0x0290 IOMMU PMU Access Low 6 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH6_REG;     /*!< Offset 0x0294 IOMMU PMU Access High 6 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW6_REG;         /*!< Offset 0x0298 IOMMU PMU Hit Low 6 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH6_REG;        /*!< Offset 0x029C IOMMU PMU Hit High 6 Register */
	         uint8_t reserved18 [0x0030];
    volatile uint32_t IOMMU_PMU_ACCESS_LOW7_REG;      /*!< Offset 0x02D0 IOMMU PMU Access Low 7 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH7_REG;     /*!< Offset 0x02D4 IOMMU PMU Access High 7 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW7_REG;         /*!< Offset 0x02D8 IOMMU PMU Hit Low 7 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH7_REG;        /*!< Offset 0x02DC IOMMU PMU Hit High 7 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_LOW8_REG;      /*!< Offset 0x02E0 IOMMU PMU Access Low 8 Register */
    volatile uint32_t IOMMU_PMU_ACCESS_HIGH8_REG;     /*!< Offset 0x02E4 IOMMU PMU Access High 8 Register */
    volatile uint32_t IOMMU_PMU_HIT_LOW8_REG;         /*!< Offset 0x02E8 IOMMU PMU Hit Low 8 Register */
    volatile uint32_t IOMMU_PMU_HIT_HIGH8_REG;        /*!< Offset 0x02EC IOMMU PMU Hit High 8 Register */
	         uint8_t reserved19 [0x0010];
    volatile uint32_t IOMMU_PMU_TL_LOW0_REG;          /*!< Offset 0x0300 IOMMU Total Latency Low 0 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH0_REG;         /*!< Offset 0x0304 IOMMU Total Latency High 0 Register */
    volatile uint32_t IOMMU_PMU_ML0_REG;              /*!< Offset 0x0308 IOMMU Max Latency 0 Register */
	         uint8_t reserved20 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW1_REG;          /*!< Offset 0x0310 IOMMU Total Latency Low 1 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH1_REG;         /*!< Offset 0x0314 IOMMU Total Latency High 1 Register */
    volatile uint32_t IOMMU_PMU_ML1_REG;              /*!< Offset 0x0318 IOMMU Max Latency 1 Register */
	         uint8_t reserved21 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW2_REG;          /*!< Offset 0x0320 IOMMU Total Latency Low 2 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH2_REG;         /*!< Offset 0x0324 IOMMU Total Latency High 2 Register */
    volatile uint32_t IOMMU_PMU_ML2_REG;              /*!< Offset 0x0328 IOMMU Max Latency 2 Register */
	         uint8_t reserved22 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW3_REG;          /*!< Offset 0x0330 IOMMU Total Latency Low 3 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH3_REG;         /*!< Offset 0x0334 IOMMU Total Latency High 3 Register */
    volatile uint32_t IOMMU_PMU_ML3_REG;              /*!< Offset 0x0338 IOMMU Max Latency 3 Register */
	         uint8_t reserved23 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW4_REG;          /*!< Offset 0x0340 IOMMU Total Latency Low 4 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH4_REG;         /*!< Offset 0x0344 IOMMU Total Latency High 4 Register */
    volatile uint32_t IOMMU_PMU_ML4_REG;              /*!< Offset 0x0348 IOMMU Max Latency 4 Register */
	         uint8_t reserved24 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW5_REG;          /*!< Offset 0x0350 IOMMU Total Latency Low 5 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH5_REG;         /*!< Offset 0x0354 IOMMU Total Latency High 5 Register */
    volatile uint32_t IOMMU_PMU_ML5_REG;              /*!< Offset 0x0358 IOMMU Max Latency 5 Register */
	         uint8_t reserved25 [0x0004];
    volatile uint32_t IOMMU_PMU_TL_LOW6_REG;          /*!< Offset 0x0360 IOMMU Total Latency Low 6 Register */
    volatile uint32_t IOMMU_PMU_TL_HIGH6_REG;         /*!< Offset 0x0364 IOMMU Total Latency High 6 Register */
    volatile uint32_t IOMMU_PMU_ML6_REG;              /*!< Offset 0x0368 IOMMU Max Latency 6 Register */
} IOMMU_TypeDef;
/*
 * @brief THS
 */
/*!< THS Controller Interface */
typedef struct THS_Type
{
    volatile uint32_t THS_CTRL;                       /*!< Offset 0x0000 THS Control Register */
    volatile uint32_t THS_EN;                         /*!< Offset 0x0004 THS Enable Register */
    volatile uint32_t THS_PER;                        /*!< Offset 0x0008 THS Period Control Register */
	         uint8_t reserved1 [0x0004];
    volatile uint32_t THS_DATA_INTC;                  /*!< Offset 0x0010 THS Data Interrupt Control Register */
    volatile uint32_t THS_SHUT_INTC;                  /*!< Offset 0x0014 THS Shut Interrupt Control Register */
    volatile uint32_t THS_ALARM_INTC;                 /*!< Offset 0x0018 THS Alarm Interrupt Control Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t THS_DATA_INTS;                  /*!< Offset 0x0020 THS Data Interrupt Status Register */
    volatile uint32_t THS_SHUT_INTS;                  /*!< Offset 0x0024 THS Shut Interrupt Status Register */
    volatile uint32_t THS_ALARMO_INTS;                /*!< Offset 0x0028 THS Alarm off Interrupt Status Register */
    volatile uint32_t THS_ALARM_INTS;                 /*!< Offset 0x002C THS Alarm Interrupt Status Register */
    volatile uint32_t THS_FILTER;                     /*!< Offset 0x0030 THS Median Filter Control Register */
	         uint8_t reserved3 [0x000C];
    volatile uint32_t THS_ALARM_CTRL;                 /*!< Offset 0x0040 THS Alarm Threshold Control Register */
	         uint8_t reserved4 [0x003C];
    volatile uint32_t THS_SHUTDOWN_CTRL;              /*!< Offset 0x0080 THS Shutdown Threshold Control Register */
	         uint8_t reserved5 [0x001C];
    volatile uint32_t THS_CDATA;                      /*!< Offset 0x00A0 THS Calibration Data */
	         uint8_t reserved6 [0x001C];
    volatile uint32_t THS_DATA;                       /*!< Offset 0x00C0 THS Data Register */
} THS_TypeDef;
/*
 * @brief TIMER
 */
/*!< TIMER Controller Interface */
typedef struct TIMER_Type
{
    volatile uint32_t TMR_IRQ_EN_REG;                 /*!< Offset 0x0000 Timer IRQ Enable Register */
    volatile uint32_t TMR_IRQ_STA_REG;                /*!< Offset 0x0004 Timer Status Register */
	         uint8_t reserved1 [0x0008];
    volatile uint32_t TMR0_CTRL_REG;                  /*!< Offset 0x0010 Timer0 Control Register */
    volatile uint32_t TMR0_INTV_VALUE_REG;            /*!< Offset 0x0014 Timer0 Interval Value Register */
    volatile uint32_t TMR0_CUR_VALUE_REG;             /*!< Offset 0x0018 Timer0 Current Value Register */
	         uint8_t reserved2 [0x0004];
    volatile uint32_t TMR1_CTRL_REG;                  /*!< Offset 0x0020 Timer1 Control Register */
    volatile uint32_t TMR1_INTV_VALUE_REG;            /*!< Offset 0x0024 Timer1 Interval Value Register */
    volatile uint32_t TMR1_CUR_VALUE_REG;             /*!< Offset 0x0028 Timer1 Current Value Register */
	         uint8_t reserved3 [0x0074];
    volatile uint32_t WDOG_IRQ_EN_REG;                /*!< Offset 0x00A0 Watchdog IRQ Enable Register */
    volatile uint32_t WDOG_IRQ_STA_REG;               /*!< Offset 0x00A4 Watchdog Status Register */
    volatile uint32_t WDOG_SOFT_RST_REG;              /*!< Offset 0x00A8 Watchdog Software Reset Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t WDOG_CTRL_REG;                  /*!< Offset 0x00B0 Watchdog Control Register */
    volatile uint32_t WDOG_CFG_REG;                   /*!< Offset 0x00B4 Watchdog Configuration Register */
    volatile uint32_t WDOG_MODE_REG;                  /*!< Offset 0x00B8 Watchdog Mode Register */
    volatile uint32_t WDOG_OUTPUT_CFG_REG;            /*!< Offset 0x00BC Watchdog Output Configuration Register */
    volatile uint32_t AVS_CNT_CTL_REG;                /*!< Offset 0x00C0 AVS Control Register */
    volatile uint32_t AVS_CNT0_REG;                   /*!< Offset 0x00C4 AVS Counter 0 Register */
    volatile uint32_t AVS_CNT1_REG;                   /*!< Offset 0x00C8 AVS Counter 1 Register */
    volatile uint32_t AVS_CNT_DIV_REG;                /*!< Offset 0x00CC AVS Divisor Register */
} TIMER_TypeDef;
/*
 * @brief HSTIMER
 */
/*!< HSTIMER Controller Interface */
typedef struct HSTIMER_Type
{
    volatile uint32_t HS_TMR_IRQ_EN_REG;              /*!< Offset 0x0000 HS Timer IRQ Enable Register */
    volatile uint32_t HS_TMR_IRQ_STAS_REG;            /*!< Offset 0x0004 HS Timer Status Register */
	         uint8_t reserved1 [0x0018];
    volatile uint32_t HS_TMR0_CTRL_REG;               /*!< Offset 0x0020 HS Timer0 Control Register */
    volatile uint32_t HS_TMR0_INTV_LO_REG;            /*!< Offset 0x0024 HS Timer0 Interval Value Low Register */
    volatile uint32_t HS_TMR0_INTV_HI_REG;            /*!< Offset 0x0028 HS Timer0 Interval Value High Register */
    volatile uint32_t HS_TMR0_CURNT_LO_REG;           /*!< Offset 0x002C HS Timer0 Current Value Low Register */
    volatile uint32_t HS_TMR0_CURNT_HI_REG;           /*!< Offset 0x0030 HS Timer0 Current Value High Register */
	         uint8_t reserved2 [0x000C];
    volatile uint32_t HS_TMR1_CTRL_REG;               /*!< Offset 0x0040 HS Timer1 Control Register */
    volatile uint32_t HS_TMR1_INTV_LO_REG;            /*!< Offset 0x0044 HS Timer1 Interval Value Low Register */
    volatile uint32_t HS_TMR1_INTV_HI_REG;            /*!< Offset 0x0048 HS Timer1 Interval Value High Register */
    volatile uint32_t HS_TMR1_CURNT_LO_REG;           /*!< Offset 0x004C HS Timer1 Current Value Low Register */
    volatile uint32_t HS_TMR1_CURNT_HI_REG;           /*!< Offset 0x0050 HS Timer1 Current Value High Register */
} HSTIMER_TypeDef;
/*
 * @brief CAN
 */
/*!< CAN Controller Interface */
typedef struct CAN_Type
{
    volatile uint32_t CAN_MSEL;                       /*!< Offset 0x0000 CAN mode select register */
    volatile uint32_t CAN_CMD;                        /*!< Offset 0x0004 CAN command register */
    volatile uint32_t CAN_STA;                        /*!< Offset 0x0008 CAN status register */
    volatile uint32_t CAN_INT;                        /*!< Offset 0x000C CAN interrupt register */
    volatile uint32_t CAN_INTEN;                      /*!< Offset 0x0010 CAN interrupt enable register */
    volatile uint32_t CAN_BUSTIME;                    /*!< Offset 0x0014 CAN bus timing register */
    volatile uint32_t CAN_TEWL;                       /*!< Offset 0x0018 CAN TX error warning limit register */
    volatile uint32_t CAN_ERRC;                       /*!< Offset 0x001C CAN error counter register */
    volatile uint32_t CAN_RMCNT;                      /*!< Offset 0x0020 CAN receive message counter register */
    volatile uint32_t CAN_RBUF_SADDR;                 /*!< Offset 0x0024 CAN receive buffer start address register */
    volatile uint32_t CAN_ACPC;                       /*!< Offset 0x0028 CAN acceptance code 0 register(reset mode) */
    volatile uint32_t CAN_ACPM;                       /*!< Offset 0x002C CAN acceptance mask 0 register(reset mode) */
	         uint8_t reserved1 [0x0010];
    volatile uint32_t CAN_TRBUF0;                     /*!< Offset 0x0040 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF1;                     /*!< Offset 0x0044 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF2;                     /*!< Offset 0x0048 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF3;                     /*!< Offset 0x004C CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF4;                     /*!< Offset 0x0050 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF5;                     /*!< Offset 0x0054 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF6;                     /*!< Offset 0x0058 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF7;                     /*!< Offset 0x005C CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF8;                     /*!< Offset 0x0060 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF9;                     /*!< Offset 0x0064 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF10;                    /*!< Offset 0x0068 CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF11;                    /*!< Offset 0x006C CAN TX/RX message buffer 0 register */
    volatile uint32_t CAN_TRBUF12;                    /*!< Offset 0x0070 CAN TX/RX message buffer 0 register */
	         uint8_t reserved2 [0x010C];
    volatile uint32_t CAN_RBUF_RBACK [0x030];         /*!< Offset 0x0180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
	         uint8_t reserved3 [0x00C0];
    volatile uint32_t CAN_VERSION;                    /*!< Offset 0x0300 CAN Version Register */
} CAN_TypeDef;
/*
 * @brief USB1
 */
/*!< USB1 Controller Interface */
typedef struct USB1_Type
{
    volatile uint16_t E_CAPLENGTH;                    /*!< Offset 0x0000 EHCI Capability Register Length Register */
    volatile uint16_t E_HCIVERSION;                   /*!< Offset 0x0002 EHCI Host Interface Version Number Register */
    volatile uint32_t E_HCSPARAMS;                    /*!< Offset 0x0004 EHCI Host Control Structural Parameter Register */
    volatile uint32_t E_HCCPARAMS;                    /*!< Offset 0x0008 EHCI Host Control Capability Parameter Register */
    volatile uint32_t E_HCSPPORTROUTE;                /*!< Offset 0x000C EHCI Companion Port Route Description */
    volatile uint32_t E_USBCMD;                       /*!< Offset 0x0010 EHCI USB Command Register */
    volatile uint32_t E_USBSTS;                       /*!< Offset 0x0014 EHCI USB Status Register */
    volatile uint32_t E_USBINTR;                      /*!< Offset 0x0018 EHCI USB Interrupt Enable Register */
    volatile uint32_t E_FRINDEX;                      /*!< Offset 0x001C EHCI USB Frame Index Register */
    volatile uint32_t E_CTRLDSSEGMENT;                /*!< Offset 0x0020 EHCI 4G Segment Selector Register */
    volatile uint32_t E_PERIODICLISTBASE;             /*!< Offset 0x0024 EHCI Frame List Base Address Register */
    volatile uint32_t E_ASYNCLISTADDR;                /*!< Offset 0x0028 EHCI Next Asynchronous List Address Register */
	         uint8_t reserved1 [0x0024];
    volatile uint32_t E_CONFIGFLAG;                   /*!< Offset 0x0050 EHCI Configured Flag Register */
    volatile uint32_t E_PORTSC;                       /*!< Offset 0x0054 EHCI Port Status/Control Register */
	         uint8_t reserved2 [0x03AC];
    volatile uint32_t O_HcControl;                    /*!< Offset 0x0404 OHCI Control Register */
    volatile uint32_t O_HcCommandStatus;              /*!< Offset 0x0408 OHCI Command Status Register */
    volatile uint32_t O_HcInterruptStatus;            /*!< Offset 0x040C OHCI Interrupt Status Register */
    volatile uint32_t O_HcInterruptEnable;            /*!< Offset 0x0410 OHCI Interrupt Enable Register */
    volatile uint32_t O_HcInterruptDisable;           /*!< Offset 0x0414 OHCI Interrupt Disable Register */
    volatile uint32_t O_HcHCCA;                       /*!< Offset 0x0418 OHCI HCCA Base */
    volatile uint32_t O_HcPeriodCurrentED;            /*!< Offset 0x041C OHCI Period Current ED Base */
    volatile uint32_t O_HcControlHeadED;              /*!< Offset 0x0420 OHCI Control Head ED Base */
    volatile uint32_t O_HcControlCurrentED;           /*!< Offset 0x0424 OHCI Control Current ED Base */
    volatile uint32_t O_HcBulkHeadED;                 /*!< Offset 0x0428 OHCI Bulk Head ED Base */
    volatile uint32_t O_HcBulkCurrentED;              /*!< Offset 0x042C OHCI Bulk Current ED Base */
    volatile uint32_t O_HcDoneHead;                   /*!< Offset 0x0430 OHCI Done Head Base */
    volatile uint32_t O_HcFmInterval;                 /*!< Offset 0x0434 OHCI Frame Interval Register */
    volatile uint32_t O_HcFmRemaining;                /*!< Offset 0x0438 OHCI Frame Remaining Register */
    volatile uint32_t O_HcFmNumber;                   /*!< Offset 0x043C OHCI Frame Number Register */
    volatile uint32_t O_HcPerioddicStart;             /*!< Offset 0x0440 OHCI Periodic Start Register */
    volatile uint32_t O_HcLSThreshold;                /*!< Offset 0x0444 OHCI LS Threshold Register */
    volatile uint32_t O_HcRhDescriptorA;              /*!< Offset 0x0448 OHCI Root Hub Descriptor Register A */
    volatile uint32_t O_HcRhDesriptorB;               /*!< Offset 0x044C OHCI Root Hub Descriptor Register B */
    volatile uint32_t O_HcRhStatus;                   /*!< Offset 0x0450 OHCI Root Hub Status Register */
    volatile uint32_t O_HcRhPortStatus;               /*!< Offset 0x0454 OHCI Root Hub Port Status Register */
	         uint8_t reserved3 [0x03A8];
    volatile uint32_t HCI_Interface;                  /*!< Offset 0x0800 HCI Interface Register */
	         uint8_t reserved4 [0x0004];
    volatile uint32_t HCI_CTRL3;                      /*!< Offset 0x0808 HCI Control Register */
	         uint8_t reserved5 [0x0004];
    volatile uint32_t PHY_Control;                    /*!< Offset 0x0810 PHY Control Register */
	         uint8_t reserved6 [0x0010];
    volatile uint32_t PHY_STATUS;                     /*!< Offset 0x0824 PHY Status Register */
    volatile uint32_t HCI;                            /*!< Offset 0x0828 HCI SIE Port Disable Control Register */
} USB1_TypeDef;
/* Generated section end */

/* Base addresses */

// SP0 (SYS domain)
#define GPIO_BASE			0x02000000
#define SPC_BASE			0x02000800
#define PWM_BASE			0x02000C00
#define CCU_BASE			0x02001000
#define CIR_TX_BASE			0x02003000
#define TZMA_BASE			0x02004000
#define LEDC_BASE			0x02008000
#define GPADC_BASE			0x02009000
#define THS_BASE			0x02009400
#define TPADC_BASE			0x02009C00
#define IOMMU_BASE			0x02010000
#define AUDIOCODEC_BASE		0x02030000
#define DMIC_BASE			0x02031000
#define I2S1_BASE			0x02033000
#define I2S2_BASE			0x02034000
#define OWA_BASE			0x02036000
#define TIMER_BASE			0x02050000

// SP1 (SYS domain)
#define UART0_BASE      	0x02500000
#define UART1_BASE      	0x02000400
#define UART2_BASE      	0x02000800
#define UART3_BASE     		0x02000C00
#define UART4_BASE      	0x02001000
#define UART5_BASE      	0x02001400

#define TWI0_BASE 	     	0x02502000
#define TWI1_BASE 	     	0x02502400
#define TWI2_BASE 	     	0x02502800
#define TWI3_BASE 	     	0x02502C00

#define CAN0_BASE  			0x02504000
#define CAN1_BASE  			0x02504400

// SH0 (SYS domain)
#define SYS_CFG_BASE      	0x03000000
#define DMAC_BASE      		0x03002000
#define CPUX_MSGBOX_BASE	0x03003000
#define SPINLOCK_BASE     	0x03005000
#define	SID_BASE  		   	0x03006000
#define SMC_BASE     		0x03007000
#define HSTIMER_BASE     	0x03008000
#define DCU_BASE      		0x03010000
#define GIC_BASE      		0x03020000
#define CE_NS_BASE      	0x03040000
#define CE_S_BASE   	   	0x03040800
#define CE_KEY_SRAM_BASE    0x03041000
#define MSI_MEMC_BASE	    0x03102000

// SH2 (SYS domain)
#define SMHC0_BASE      	0x04020000
#define SMHC1_BASE      	0x04021000
#define SMHC2_BASE      	0x04022000
#define SPI0_BASE    	  	0x04025000
#define SPI_DBI_BASE  		0x04026000
#define USB0_BASE 	     	0x04100000
#define USB1_BASE 	     	0x04200000
#define EMAC_BASE 	     	0x04500000

// VIDEO_OUT_SYS related
#define DE_BASE 			0x05000000
#define DI_BASE 			0x05400000
#define G2D_BASE 			0x05410000
#define DSI_BASE 			0x05450000
#define DISPLAY_TOP_BASE 	0x05460000
#define TCON_LCD0_BASE 		0x05461000
#define TCON_TV0_BASE 		0x05470000
#define TVE_TOP_BASE 		0x05600000
#define TV_Encoder_BASE 	0x05604000

// VIDEO_IN_SYS related
#define CSI_BASE 			0x05800000
#define TVD_TOP_BASE 		0x05C00000
#define TVD0_BASE 			0x05C01000

// APBS0 related
#define CIR_RX_BASE			0x07040000

// AHBS related
#define RTC_BASE			0x07090000

// CPUX related
// Address (It is for Cluster CPU)
#define CPU_SUBSYS_CTRL_BASE	0x08100000
#define TimeStamp_STA_BASE		0x08110000
#define TimeStamp_CTRL_BASE		0x08120000
#define IDC_BASE				0x08130000
#define C0_CPUX_CFG_BASE		0x09010000
#define C0_CPUX_MBIST_BASE		0x09020000

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 	0x40000000	/* 2 GB */

// GPIO registers calculation
#define GPIOB_BASE		(GPIO_BASE + 0x030 * 1)
#define GPIOC_BASE		(GPIO_BASE + 0x030 * 2)
#define GPIOD_BASE		(GPIO_BASE + 0x030 * 3)
#define GPIOE_BASE		(GPIO_BASE + 0x030 * 4)
#define GPIOF_BASE		(GPIO_BASE + 0x030 * 5)
#define GPIOG_BASE		(GPIO_BASE + 0x030 * 6)

/////////////////////
/* Access pointers */

#define UART0      ((UART_TypeDef *) UART0_BASE)
#define UART1      ((UART_TypeDef *) UART1_BASE)
#define UART2      ((UART_TypeDef *) UART2_BASE)
#define UART3      ((UART_TypeDef *) UART3_BASE)
#define UART4      ((UART_TypeDef *) UART4_BASE)
#define UART5      ((UART_TypeDef *) UART5_BASE)
#define GPIOB      ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC      ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD      ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE      ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF      ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG      ((GPIO_TypeDef *) GPIOG_BASE)
#define SYS_CFG 	((SYS_CFG_TypeDef *) SYS_CFG_BASE)
#define SMHC0      	((SMHC_TypeDef *) SMHC0_BASE)
#define SMHC1      	((SMHC_TypeDef *) SMHC1_BASE)
#define SMHC2      	((SMHC_TypeDef *) SMHC2_BASE)
#define I2S1      	((I2S_PCM_TypeDef *) I2S1_BASE)
#define I2S2      	((I2S_PCM_TypeDef *) I2S2_BASE)
#define DMIC      	((DMIC_TypeDef *) DMIC_BASE)
#define OWA			((OWA_TypeDef *) OWA_BASE)
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)
#define TWI0		((TWI_TypeDef *) TWI0_BASE)
#define TWI1		((TWI_TypeDef *) TWI1_BASE)
#define TWI2		((TWI_TypeDef *) TWI2_BASE)
#define TWI3		((TWI_TypeDef *) TWI3_BASE)
#define SPI0		((SPI_TypeDef *) SPI0_BASE)
#define SPI_DBI		((SPI_DBI_TypeDef *) SPI_DB_BASEI)
#define LEDC		((LEDC_TypeDef *) LEDC_BASE)
#define GPADC		((GPADC_TypeDef *) GPADC_BASE)
#define TPADC		((TPADC_TypeDef *) TPADC_BASE)
#define CIR_RX		((CIR_RX_TypeDef *) CIR_RX_BASE)
#define CIR_TX		((CIR_TX_TypeDef *) CIR_TX_BASE)
#define CE_S		((CE_S_TypeDef *) CE_S_BASE)
#define HSTIMER		((HSTIMER_TypeDef *) HSTIMER_BASE)
#define TIMER		((TIMER_TypeDef *) TIMER_BASE)
#define THS			((THS_TypeDef *) THS_BASE)
#define IOMMU		((IOMMU_TypeDef *) IOMMU_BASE)
#define CCU			((CCU_TypeDef *) CCU_BASE)

#define DE 			((DE_TypeDef *) DE_BASE)
#define DI 			((DI_TypeDef *) DI_BASE)
#define G2D 		((G2D_TypeDef *) G2D_BASE)
#define DSI 		((DSI_TypeDef *) DSI_BASE)
#define DISPLAY_TOP ((DISPLAY_TOP_TypeDef *) DISPLAY_TOP_BASE)
#define TCON_LCD0 	((TCON_LCD0_TypeDef *) TCON_LCD0_BASE)
#define TCON_TV0 	((TCON_TV0_TypeDef *) TCON_TV0_BASE)
#define TVE_TOP 	((TVE_TypeDef *) TVE_TOP_BASE)
#define TV_Encoder 	((TVE_TypeDef *) TV_Encoder_BASE)

#define CSI			((CSI_TypeDef *) CSI_BASE)
#define TVD_TOP		((TVD_TypeDef *) TVD_TOP_BASE)
#define TVD0		((TVD_TypeDef *) TVD0_BASE)

#define RTC			((RTC_TypeDef *) RTC_BASE)

#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)
#define TimeStamp_STA	((TimeStamp_STA_TypeDef *) TimeStamp_STA_BASE)
#define TimeStamp_CTRL	((TimeStamp_CTRL_TypeDef *) TimeStamp_CTRL_BASE)
#define IDC				(IDC_TypeDef *) IDC_BASE)
#define C0_CPUX_CFG		((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)
#define C0_CPUX_MBIST	((C0_CPUX_MBIST_TypeDef *) C0_CPUX_MBIST_BASE)

#define CAN0			((CAN_TypeDef *) CAN0_BASE)
#define CAN1			((CAN_TypeDef *) CAN1_BASE)

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
