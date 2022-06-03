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

	GPIOB2_IRQn = (224 + 2), /*  Peripherial */
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
#define GIC_DISTRIBUTOR_BASE GIC_BASE
#define GIC_INTERFACE_BASE   (GIC_BASE+0x1000)

#include "core_ca.h"
#include "system_allwnr_t113s3.h"


/*
* @brief CPU Subsystem Control Registers
*/
/*!< CPU Subsystem Control Register List */
typedef struct CPU_SUBSYS_CTRL_Type
{
	volatile uint32_t GENER_CTRL_REG0;        /*!< Offset 0x0000 General Control Register0 */
			uint8_t reserved1 [0x0008];
	volatile uint32_t GIC_JTAG_RST_CTRL;      /*!< Offset 0x000C GIC and JTAG Reset Control Register */
	volatile uint32_t C0_INT_EN;              /*!< Offset 0x0010 Cluster0 Interrupt Enable Control Register */
	volatile uint32_t RQ_FIQ_STATUS;          /*!< Offset 0x0014 IRQ/FIQ Status Register */
	volatile uint32_t GENER_CTRL_REG2;        /*!< Offset 0x0018 General Control Register2 */
	volatile uint32_t DBG_STATE;              /*!< Offset 0x001C Debug State Register */
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
	volatile	uint32_t CFG [4];		// Offset: 0x00
	volatile	uint32_t DATA;			// Offset: 0x10
	volatile	uint32_t DRV [4];		// Offset: 0x14
	volatile	uint32_t PULL [2];		// Offset: 0x24
}  GPIO_TypeDef;

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
#define SPI1_BASE    	  	0x04026000
#define USB0_BASE 	     	0x04100000
#define USB1_BASE 	     	0x04200000
#define EMAC_BASE 	     	0x04500000

// VIDEO_OUT_SYS related

// VIDEO_IN_SYS related

// APBS0 related

// AHBS related

 // CPUX related

 // DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 	0x40000000	/* 2 GB */

#define CPU_SUBSYS_CTRL_BASE	0x08100000

#define GPIOB_BASE		(GPIO_BASE + 0x030 * 1)
#define GPIOC_BASE		(GPIO_BASE + 0x030 * 2)
#define GPIOD_BASE		(GPIO_BASE + 0x030 * 3)
#define GPIOE_BASE		(GPIO_BASE + 0x030 * 4)
#define GPIOF_BASE		(GPIO_BASE + 0x030 * 5)
#define GPIOG_BASE		(GPIO_BASE + 0x030 * 6)

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
#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)
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
#define SPI1		((SPI_TypeDef *) SPI1_BASE)


#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
