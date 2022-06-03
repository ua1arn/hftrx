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
 	volatile	uint32_t DATA;				// Offset: 0x0000
 	volatile	uint32_t DLH_IER;			// Offset: 0x0004
 	volatile	uint32_t IIR_FCR;			// Offset: 0x0008
 	volatile	uint32_t LCR;				// Offset: 0x000C
 	volatile	uint32_t MCR;				// Offset: 0x0010
 	volatile	uint32_t LSR;				// Offset: 0x0014
 	volatile	uint32_t MSR;				// Offset: 0x0018
 	volatile	uint32_t SCH;				// Offset: 0x001C
 				uint32_t reserved1 [23];
 	volatile	uint32_t USR;				// Offset: 0x007C
 	volatile	uint32_t TFL;				// Offset: 0x0080
 	volatile	uint32_t RFL;				// Offset: 0x0084
 	volatile	uint32_t HSK;				// Offset: 0x0088
 	volatile	uint32_t DMA_REQ_EN;		// Offset: 0x008C
 				uint32_t reserved2 [5];
 	volatile	uint32_t HALT;				// Offset: 0x00A4
 				uint32_t reserved3 [2];
 	volatile	uint32_t DBG_DLL;			// Offset: 0x00B0
 	volatile	uint32_t DBG_DLH;			// Offset: 0x00B4
 				uint32_t reserved4 [14];
 	volatile	uint32_t A_FCC;				// Offset: 0x00F0
 				uint32_t reserved5 [3];
 	volatile	uint32_t A_RXDMA_CTRL;		// Offset: 0x0100
 	volatile	uint32_t A_RXDMA_STR;		// Offset: 0x0104
 	volatile	uint32_t A_RXDMA_STA;		// Offset: 0x0108
 	volatile	uint32_t A_RXDMA_LMT;		// Offset: 0x010C
 	volatile	uint32_t A_RXDMA_SADDRL;	// Offset: 0x0110
 	volatile	uint32_t A_RXDMA_SADDRH;	// Offset: 0x0114
 	volatile	uint32_t A_RXDMA_BL;		// Offset: 0x0118
 				uint32_t reserved6;
 	volatile	uint32_t A_RXDMA_IE;		// Offset: 0x0120
 	volatile	uint32_t A_RXDMA_IS;		// Offset: 0x0124
 	volatile	uint32_t A_RXDMA_WADDRL;	// Offset: 0x0128
 	volatile	uint32_t A_RXDMA_WADDRH;	// Offset: 0x012C
 	volatile	uint32_t A_RXDMA_RADDRL;	// Offset: 0x0130
 	volatile	uint32_t A_RXDMA_RADDRH;	// Offset: 0x0134
 	volatile	uint32_t A_RXDMA_DCNT;		// Offset: 0x0138
}  UART_TypeDef;


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

#define CPU_SUBSYS_CTRL_BASE	0x08100000

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

#define GPIOB_BASE		(GPIO_BASE + 0x030 * 1)
#define GPIOC_BASE		(GPIO_BASE + 0x030 * 2)
#define GPIOD_BASE		(GPIO_BASE + 0x030 * 3)
#define GPIOE_BASE		(GPIO_BASE + 0x030 * 4)
#define GPIOF_BASE		(GPIO_BASE + 0x030 * 5)
#define GPIOG_BASE		(GPIO_BASE + 0x030 * 6)

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

#define SYS_CFG 		((SYS_CFG_TypeDef *) SYS_CFG_BASE)
#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
