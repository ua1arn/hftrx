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

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* Peripheral and RAM base address */
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
#define RTC_BASE			0x07090000				/*!< (RTC         ) Base Address */

// CPUX related
// Address (It is for Cluster CPU)
#define CPU_SUBSYS_CTRL_BASE	0x08100000
#define TimeStamp_STA_BASE		0x08110000
#define TimeStamp_CTRL_BASE		0x08120000
#define IDC_BASE				0x08130000
#define C0_CPUX_CFG_BASE		0x09010000
#define C0_CPUX_MBIST_BASE		0x09020000	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 		0x40000000			/*!< (DRAM        ) Base Address - 2GB */

// GPIO registers calculation
#define GPIOB_BASE		(GPIO_BASE + 0x030 * 1)		/*!< (GPIOB       ) Base Address */
#define GPIOC_BASE		(GPIO_BASE + 0x030 * 2)		/*!< (GPIOC       ) Base Address */
#define GPIOD_BASE		(GPIO_BASE + 0x030 * 3)		/*!< (GPIOD       ) Base Address */
#define GPIOE_BASE		(GPIO_BASE + 0x030 * 4)		/*!< (GPIOE       ) Base Address */
#define GPIOF_BASE		(GPIO_BASE + 0x030 * 5)		/*!< (GPIOF       ) Base Address */
#define GPIOG_BASE		(GPIO_BASE + 0x030 * 6)		/*!< (GPIOG       ) Base Address */

#define GIC_DISTRIBUTOR_BASE (GIC_BASE + 0x1000)
#define GIC_INTERFACE_BASE   (GIC_BASE + 0x2000)

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "core_ca.h"
#include "system_allwnr_t113s3.h"


/* Generated section start */
/*
 * @brief CPU_SUBSYS_CTRL
 */
/*!< CPU_SUBSYS_CTRL Controller Interface */
typedef struct CPU_SUBSYS_CTRL_Type
{
	__IO uint32_t GENER_CTRL_REG0;                       /*!< Offset 0x000 General Control Register0 */
	uint32_t reserved1 [0x0002];
	__IO uint32_t GIC_JTAG_RST_CTRL;                     /*!< Offset 0x00C GIC and JTAG Reset Control Register */
	__IO uint32_t C0_INT_EN;                             /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
	__IO uint32_t RQ_FIQ_STATUS;                         /*!< Offset 0x014 IRQ/FIQ Status Register */
	__IO uint32_t GENER_CTRL_REG2;                       /*!< Offset 0x018 General Control Register2 */
	__IO uint32_t DBG_STATE;                             /*!< Offset 0x01C Debug State Register */
} CPU_SUBSYS_CTRL_TypeDef;
/*
 * @brief CCU
 */
/*!< CCU Controller Interface */
typedef struct CCU_Type
{
	__IO uint32_t PLL_CPU_CTRL_REG;                      /*!< Offset 0x000 PLL_CPU Control Register */
	uint32_t reserved1 [0x0003];
	__IO uint32_t PLL_DDR_CTRL_REG;                      /*!< Offset 0x010 PLL_DDR Control Register */
	uint32_t reserved2 [0x0003];
	__IO uint32_t PLL_PERI_CTRL_REG;                     /*!< Offset 0x020 PLL_PERI Control Register */
	uint32_t reserved3 [0x0007];
	__IO uint32_t PLL_VIDEO0_CTRL_REG;                   /*!< Offset 0x040 PLL_VIDEO0 Control Register */
	uint32_t reserved4;
	__IO uint32_t PLL_VIDEO1_CTRL_REG;                   /*!< Offset 0x048 PLL_VIDEO1 Control Register */
	uint32_t reserved5 [0x0003];
	__IO uint32_t PLL_VE_CTRL_REG;                       /*!< Offset 0x058 PLL_VE Control Register */
	uint32_t reserved6 [0x0007];
	__IO uint32_t PLL_AUDIO0_CTRL_REG;                   /*!< Offset 0x078 PLL_AUDIO0 Control Register */
	uint32_t reserved7;
	__IO uint32_t PLL_AUDIO1_CTRL_REG;                   /*!< Offset 0x080 PLL_AUDIO1 Control Register */
	uint32_t reserved8 [0x0023];
	__IO uint32_t PLL_DDR_PAT0_CTRL_REG;                 /*!< Offset 0x110 PLL_DDR Pattern0 Control Register */
	__IO uint32_t PLL_DDR_PAT1_CTRL_REG;                 /*!< Offset 0x114 PLL_DDR Pattern1 Control Register */
	uint32_t reserved9 [0x0002];
	__IO uint32_t PLL_PERI_PAT0_CTRL_REG;                /*!< Offset 0x120 PLL_PERI Pattern0 Control Register */
	__IO uint32_t PLL_PERI_PAT1_CTRL_REG;                /*!< Offset 0x124 PLL_PERI Pattern1 Control Register */
	uint32_t reserved10 [0x0006];
	__IO uint32_t PLL_VIDEO0_PAT0_CTRL_REG;              /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
	__IO uint32_t PLL_VIDEO0_PAT1_CTRL_REG;              /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
	__IO uint32_t PLL_VIDEO1_PAT0_CTRL_REG;              /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
	__IO uint32_t PLL_VIDEO1_PAT1_CTRL_REG;              /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
	uint32_t reserved11 [0x0002];
	__IO uint32_t PLL_VE_PAT0_CTRL_REG;                  /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
	__IO uint32_t PLL_VE_PAT1_CTRL_REG;                  /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
	uint32_t reserved12 [0x0006];
	__IO uint32_t PLL_AUDIO0_PAT0_CTRL_REG;              /*!< Offset 0x178 PLL_AUDIO0 Pattern0 Control Register */
	__IO uint32_t PLL_AUDIO0_PAT1_CTRL_REG;              /*!< Offset 0x17C PLL_AUDIO0 Pattern1 Control Register */
	__IO uint32_t PLL_AUDIO1_PAT0_CTRL_REG;              /*!< Offset 0x180 PLL_AUDIO1 Pattern0 Control Register */
	__IO uint32_t PLL_AUDIO1_PAT1_CTRL_REG;              /*!< Offset 0x184 PLL_AUDIO1 Pattern1 Control Register */
	uint32_t reserved13 [0x005E];
	__IO uint32_t PLL_CPU_BIAS_REG;                      /*!< Offset 0x300 PLL_CPU Bias Register */
	uint32_t reserved14 [0x0003];
	__IO uint32_t PLL_DDR_BIAS_REG;                      /*!< Offset 0x310 PLL_DDR Bias Register */
	uint32_t reserved15 [0x0003];
	__IO uint32_t PLL_PERI_BIAS_REG;                     /*!< Offset 0x320 PLL_PERI Bias Register */
	uint32_t reserved16 [0x0007];
	__IO uint32_t PLL_VIDEO0_BIAS_REG;                   /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
	uint32_t reserved17;
	__IO uint32_t PLL_VIDEO1_BIAS_REG;                   /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
	uint32_t reserved18 [0x0003];
	__IO uint32_t PLL_VE_BIAS_REG;                       /*!< Offset 0x358 PLL_VE Bias Register */
	uint32_t reserved19 [0x0007];
	__IO uint32_t PLL_AUDIO0_BIAS_REG;                   /*!< Offset 0x378 PLL_AUDIO0 Bias Register */
	uint32_t reserved20;
	__IO uint32_t PLL_AUDIO1_BIAS_REG;                   /*!< Offset 0x380 PLL_AUDIO1 Bias Register */
	uint32_t reserved21 [0x001F];
	__IO uint32_t PLL_CPU_TUN_REG;                       /*!< Offset 0x400 PLL_CPU Tuning Register */
	uint32_t reserved22 [0x003F];
	__IO uint32_t CPU_AXI_CFG_REG;                       /*!< Offset 0x500 CPU_AXI Configuration Register */
	__IO uint32_t CPU_GATING_REG;                        /*!< Offset 0x504 CPU_GATING Configuration Register */
	uint32_t reserved23 [0x0002];
	__IO uint32_t PSI_CLK_REG;                           /*!< Offset 0x510 PSI Clock Register */
	uint32_t reserved24 [0x0003];
	__IO uint32_t APB0_CLK_REG;                          /*!< Offset 0x520 APB0 Clock Register */
	__IO uint32_t APB1_CLK_REG;                          /*!< Offset 0x524 APB1 Clock Register */
	uint32_t reserved25 [0x0006];
	__IO uint32_t MBUS_CLK_REG;                          /*!< Offset 0x540 MBUS Clock Register */
	uint32_t reserved26 [0x002F];
	__IO uint32_t DE_CLK_REG;                            /*!< Offset 0x600 DE Clock Register */
	uint32_t reserved27 [0x0002];
	__IO uint32_t DE_BGR_REG;                            /*!< Offset 0x60C DE Bus Gating Reset Register */
	uint32_t reserved28 [0x0004];
	__IO uint32_t DI_CLK_REG;                            /*!< Offset 0x620 DI Clock Register */
	uint32_t reserved29 [0x0002];
	__IO uint32_t DI_BGR_REG;                            /*!< Offset 0x62C DI Bus Gating Reset Register */
	__IO uint32_t G2D_CLK_REG;                           /*!< Offset 0x630 G2D Clock Register */
	uint32_t reserved30 [0x0002];
	__IO uint32_t G2D_BGR_REG;                           /*!< Offset 0x63C G2D Bus Gating Reset Register */
	uint32_t reserved31 [0x0010];
	__IO uint32_t CE_CLK_REG;                            /*!< Offset 0x680 CE Clock Register */
	uint32_t reserved32 [0x0002];
	__IO uint32_t CE_BGR_REG;                            /*!< Offset 0x68C CE Bus Gating Reset Register */
	__IO uint32_t VE_CLK_REG;                            /*!< Offset 0x690 VE Clock Register */
	uint32_t reserved33 [0x0002];
	__IO uint32_t VE_BGR_REG;                            /*!< Offset 0x69C VE Bus Gating Reset Register */
	uint32_t reserved34 [0x001B];
	__IO uint32_t DMA_BGR_REG;                           /*!< Offset 0x70C DMA Bus Gating Reset Register */
	uint32_t reserved35 [0x0003];
	__IO uint32_t MSGBOX_BGR_REG;                        /*!< Offset 0x71C MSGBOX Bus Gating Reset Register */
	uint32_t reserved36 [0x0003];
	__IO uint32_t SPINLOCK_BGR_REG;                      /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register */
	uint32_t reserved37 [0x0003];
	__IO uint32_t HSTIMER_BGR_REG;                       /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
	__IO uint32_t AVS_CLK_REG;                           /*!< Offset 0x740 AVS Clock Register */
	uint32_t reserved38 [0x0012];
	__IO uint32_t DBGSYS_BGR_REG;                        /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
	uint32_t reserved39 [0x0007];
	__IO uint32_t PWM_BGR_REG;                           /*!< Offset 0x7AC PWM Bus Gating Reset Register */
	uint32_t reserved40 [0x0003];
	__IO uint32_t IOMMU_BGR_REG;                         /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
	uint32_t reserved41 [0x0010];
	__IO uint32_t DRAM_CLK_REG;                          /*!< Offset 0x800 DRAM Clock Register */
	__IO uint32_t MBUS_MAT_CLK_GATING_REG;               /*!< Offset 0x804 MBUS Master Clock Gating Register */
	uint32_t reserved42;
	__IO uint32_t DRAM_BGR_REG;                          /*!< Offset 0x80C DRAM Bus Gating Reset Register */
	uint32_t reserved43 [0x0008];
	__IO uint32_t SMHC0_CLK_REG;                         /*!< Offset 0x830 SMHC0 Clock Register */
	__IO uint32_t SMHC1_CLK_REG;                         /*!< Offset 0x834 SMHC1 Clock Register */
	__IO uint32_t SMHC2_CLK_REG;                         /*!< Offset 0x838 SMHC2 Clock Register */
	uint32_t reserved44 [0x0004];
	__IO uint32_t SMHC_BGR_REG;                          /*!< Offset 0x84C SMHC Bus Gating Reset Register */
	uint32_t reserved45 [0x002F];
	__IO uint32_t UART_BGR_REG;                          /*!< Offset 0x90C UART Bus Gating Reset Register */
	uint32_t reserved46 [0x0003];
	__IO uint32_t TWI_BGR_REG;                           /*!< Offset 0x91C TWI Bus Gating Reset Register */
	uint32_t reserved47 [0x0008];
	__IO uint32_t SPI0_CLK_REG;                          /*!< Offset 0x940 SPI0 Clock Register */
	__IO uint32_t SPI1_CLK_REG;                          /*!< Offset 0x944 SPI1 Clock Register */
	uint32_t reserved48 [0x0009];
	__IO uint32_t SPI_BGR_REG;                           /*!< Offset 0x96C SPI Bus Gating Reset Register */
	__IO uint32_t EMAC_25M_CLK_REG;                      /*!< Offset 0x970 EMAC_25M Clock Register */
	uint32_t reserved49 [0x0002];
	__IO uint32_t EMAC_BGR_REG;                          /*!< Offset 0x97C EMAC Bus Gating Reset Register */
	uint32_t reserved50 [0x0010];
	__IO uint32_t IRTX_CLK_REG;                          /*!< Offset 0x9C0 IRTX Clock Register */
	uint32_t reserved51 [0x0002];
	__IO uint32_t IRTX_BGR_REG;                          /*!< Offset 0x9CC IRTX Bus Gating Reset Register */
	uint32_t reserved52 [0x0007];
	__IO uint32_t GPADC_BGR_REG;                         /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
	uint32_t reserved53 [0x0003];
	__IO uint32_t THS_BGR_REG;                           /*!< Offset 0x9FC THS Bus Gating Reset Register */
	uint32_t reserved54 [0x0005];
	__IO uint32_t I2S1_CLK_REG;                          /*!< Offset 0xA14 I2S1 Clock Register */
	__IO uint32_t I2S2_CLK_REG;                          /*!< Offset 0xA18 I2S2 Clock Register */
	__IO uint32_t I2S2_ASRC_CLK_REG;                     /*!< Offset 0xA1C I2S2_ASRC Clock Register */
	__IO uint32_t I2S_BGR_REG;                           /*!< Offset 0xA20 I2S Bus Gating Reset Register */
	__IO uint32_t OWA_TX_CLK_REG;                        /*!< Offset 0xA24 OWA_TX Clock Register */
	__IO uint32_t OWA_RX_CLK_REG;                        /*!< Offset 0xA28 OWA_RX Clock Register */
	__IO uint32_t OWA_BGR_REG;                           /*!< Offset 0xA2C OWA Bus Gating Reset Register */
	uint32_t reserved55 [0x0004];
	__IO uint32_t DMIC_CLK_REG;                          /*!< Offset 0xA40 DMIC Clock Register */
	uint32_t reserved56 [0x0002];
	__IO uint32_t DMIC_BGR_REG;                          /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
	__IO uint32_t AUDIO_CODEC_DAC_CLK_REG;               /*!< Offset 0xA50 AUDIO_CODEC_DAC Clock Register */
	__IO uint32_t AUDIO_CODEC_ADC_CLK_REG;               /*!< Offset 0xA54 AUDIO_CODEC_ADC Clock Register */
	uint32_t reserved57;
	__IO uint32_t AUDIO_CODEC_BGR_REG;                   /*!< Offset 0xA5C AUDIO_CODEC Bus Gating Reset Register */
	uint32_t reserved58 [0x0004];
	__IO uint32_t USB0_CLK_REG;                          /*!< Offset 0xA70 USB0 Clock Register */
	__IO uint32_t USB1_CLK_REG;                          /*!< Offset 0xA74 USB1 Clock Register */
	uint32_t reserved59 [0x0005];
	__IO uint32_t USB_BGR_REG;                           /*!< Offset 0xA8C USB Bus Gating Reset Register */
	uint32_t reserved60 [0x000B];
	__IO uint32_t DPSS_TOP_BGR_REG;                      /*!< Offset 0xABC DPSS_TOP Bus Gating Reset Register */
	uint32_t reserved61 [0x0019];
	__IO uint32_t DSI_CLK_REG;                           /*!< Offset 0xB24 DSI Clock Register */
	uint32_t reserved62 [0x0009];
	__IO uint32_t DSI_BGR_REG;                           /*!< Offset 0xB4C DSI Bus Gating Reset Register */
	uint32_t reserved63 [0x0004];
	__IO uint32_t TCONLCD_CLK_REG;                       /*!< Offset 0xB60 TCONLCD Clock Register */
	uint32_t reserved64 [0x0006];
	__IO uint32_t TCONLCD_BGR_REG;                       /*!< Offset 0xB7C TCONLCD Bus Gating Reset Register */
	__IO uint32_t TCONTV_CLK_REG;                        /*!< Offset 0xB80 TCONTV Clock Register */
	uint32_t reserved65 [0x0006];
	__IO uint32_t TCONTV_BGR_REG;                        /*!< Offset 0xB9C TCONTV Bus Gating Reset Register */
	uint32_t reserved66 [0x0003];
	__IO uint32_t LVDS_BGR_REG;                          /*!< Offset 0xBAC LVDS Bus Gating Reset Register */
	__IO uint32_t TVE_CLK_REG;                           /*!< Offset 0xBB0 TVE Clock Register */
	uint32_t reserved67 [0x0002];
	__IO uint32_t TVE_BGR_REG;                           /*!< Offset 0xBBC TVE Bus Gating Reset Register */
	__IO uint32_t TVD_CLK_REG;                           /*!< Offset 0xBC0 TVD Clock Register */
	uint32_t reserved68 [0x0006];
	__IO uint32_t TVD_BGR_REG;                           /*!< Offset 0xBDC TVD Bus Gating Reset Register */
	uint32_t reserved69 [0x0004];
	__IO uint32_t LEDC_CLK_REG;                          /*!< Offset 0xBF0 LEDC Clock Register */
	uint32_t reserved70 [0x0002];
	__IO uint32_t LEDC_BGR_REG;                          /*!< Offset 0xBFC LEDC Bus Gating Reset Register */
	uint32_t reserved71;
	__IO uint32_t CSI_CLK_REG;                           /*!< Offset 0xC04 CSI Clock Register */
	__IO uint32_t CSI_MASTER_CLK_REG;                    /*!< Offset 0xC08 CSI Master Clock Register */
	uint32_t reserved72 [0x0004];
	__IO uint32_t CSI_BGR_REG;                           /*!< Offset 0xC1C CSI Bus Gating Reset Register */
	uint32_t reserved73 [0x000C];
	__IO uint32_t TPADC_CLK_REG;                         /*!< Offset 0xC50 TPADC Clock Register */
	uint32_t reserved74 [0x0002];
	__IO uint32_t TPADC_BGR_REG;                         /*!< Offset 0xC5C TPADC Bus Gating Reset Register */
	uint32_t reserved75 [0x0004];
	__IO uint32_t DSP_CLK_REG;                           /*!< Offset 0xC70 DSP Clock Register */
	uint32_t reserved76 [0x0002];
	__IO uint32_t DSP_BGR_REG;                           /*!< Offset 0xC7C DSP Bus Gating Reset Register */
	uint32_t reserved77 [0x00A1];
	__IO uint32_t PLL_LOCK_DBG_CTRL_REG;                 /*!< Offset 0xF04 PLL Lock Debug Control Register */
	__IO uint32_t FRE_DET_CTRL_REG;                      /*!< Offset 0xF08 Frequency Detect Control Register */
	__IO uint32_t FRE_UP_LIM_REG;                        /*!< Offset 0xF0C Frequency Up Limit Register */
	__IO uint32_t FRE_DOWN_LIM_REG;                      /*!< Offset 0xF10 Frequency Down Limit Register */
	uint32_t reserved78 [0x0007];
	__IO uint32_t CCU_FAN_GATE_REG;                      /*!< Offset 0xF30 CCU FANOUT CLOCK GATE Register */
	__IO uint32_t CLK27M_FAN_REG;                        /*!< Offset 0xF34 CLK27M FANOUT Register */
	__IO uint32_t PCLK_FAN_REG;                          /*!< Offset 0xF38 PCLK FANOUT Register */
	__IO uint32_t CCU_FAN_REG;                           /*!< Offset 0xF3C CCU FANOUT Register */
} CCU_TypeDef;
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG Controller Interface */
typedef struct SYS_CFG_Type
{
	uint32_t reserved1 [0x0002];
	__IO uint32_t DSP_BOOT_RAMMAP_REG;                   /*!< Offset 0x008 DSP Boot SRAM Remap Control Register */
	uint32_t reserved2 [0x0006];
	__IO uint32_t VER_REG;                               /*!< Offset 0x024 Version Register */
	uint32_t reserved3 [0x0002];
	__IO uint32_t EMAC_EPHY_CLK_REG0;                    /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
	uint32_t reserved4 [0x0047];
	__IO uint32_t SYS_LDO_CTRL_REG;                      /*!< Offset 0x150 System LDO Control Register */
	uint32_t reserved5 [0x0003];
	__IO uint32_t RESCAL_CTRL_REG;                       /*!< Offset 0x160 Resistor Calibration Control Register */
	uint32_t reserved6;
	__IO uint32_t RES240_CTRL_REG;                       /*!< Offset 0x168 240ohms Resistor Manual Control Register */
	__IO uint32_t RESCAL_STATUS_REG;                     /*!< Offset 0x16C Resistor Calibration Status Register */
} SYS_CFG_TypeDef;
/*
 * @brief UART
 */
/*!< UART Controller Interface */
typedef struct UART_Type
{
	__IO uint32_t DATA;                                  /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
	__IO uint32_t DLH_IER;                               /*!< Offset 0x004 UART Divisor Latch High Register/UART Interrupt Enable Register */
	__IO uint32_t IIR_FCR;                               /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
	__IO uint32_t UART_LCR;                              /*!< Offset 0x00C UART Line Control Register */
	__IO uint32_t UART_MCR;                              /*!< Offset 0x010 UART Modem Control Register */
	__IO uint32_t UART_LSR;                              /*!< Offset 0x014 UART Line Status Register */
	__IO uint32_t UART_MSR;                              /*!< Offset 0x018 UART Modem Status Register */
	__IO uint32_t UART_SCH;                              /*!< Offset 0x01C UART Scratch Register */
	uint32_t reserved1 [0x0017];
	__IO uint32_t UART_USR;                              /*!< Offset 0x07C UART Status Register */
	__IO uint32_t UART_TFL;                              /*!< Offset 0x080 UART Transmit FIFO Level Register */
	__IO uint32_t UART_RFL;                              /*!< Offset 0x084 UART Receive FIFO Level Register */
	__IO uint32_t UART_HSK;                              /*!< Offset 0x088 UART DMA Handshake Configuration Register */
	__IO uint32_t UART_DMA_REQ_EN;                       /*!< Offset 0x08C UART DMA Request Enable Register */
	uint32_t reserved2 [0x0005];
	__IO uint32_t UART_HALT;                             /*!< Offset 0x0A4 UART Halt TX Register */
	uint32_t reserved3 [0x0002];
	__IO uint32_t UART_DBG_DLL;                          /*!< Offset 0x0B0 UART Debug DLL Register */
	__IO uint32_t UART_DBG_DLH;                          /*!< Offset 0x0B4 UART Debug DLH Register */
	uint32_t reserved4 [0x000E];
	__IO uint32_t UART_A_FCC;                            /*!< Offset 0x0F0 UART FIFO Clock Control Register */
	uint32_t reserved5 [0x0003];
	__IO uint32_t UART_A_RXDMA_CTRL;                     /*!< Offset 0x100 UART RXDMA Control Register */
	__IO uint32_t UART_A_RXDMA_STR;                      /*!< Offset 0x104 UART RXDMA Start Register */
	__IO uint32_t UART_A_RXDMA_STA;                      /*!< Offset 0x108 UART RXDMA Status Register */
	__IO uint32_t UART_A_RXDMA_LMT;                      /*!< Offset 0x10C UART RXDMA Limit Register */
	__IO uint32_t UART_A_RXDMA_SADDRL;                   /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
	__IO uint32_t UART_A_RXDMA_SADDRH;                   /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
	__IO uint32_t UART_A_RXDMA_BL;                       /*!< Offset 0x118 UART RXDMA Buffer Length Register */
	uint32_t reserved6;
	__IO uint32_t UART_A_RXDMA_IE;                       /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
	__IO uint32_t UART_A_RXDMA_IS;                       /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
	__IO uint32_t UART_A_RXDMA_WADDRL;                   /*!< Offset 0x128 UART RXDMA Write Address Low Register */
	__IO uint32_t UART_A_RXDMA_WADDRH;                   /*!< Offset 0x12C UART RXDMA Write Address high Register */
	__IO uint32_t UART_A_RXDMA_RADDRL;                   /*!< Offset 0x130 UART RXDMA Read Address Low Register */
	__IO uint32_t UART_A_RXDMA_RADDRH;                   /*!< Offset 0x134 UART RXDMA Read Address high Register */
	__IO uint32_t UART_A_RXDMA_DCNT;                     /*!< Offset 0x138 UART RADMA Data Count Register */
} UART_TypeDef;
/*
 * @brief GPIO
 */
/*!< GPIO Controller Interface */
typedef struct GPIO_Type
{
	__IO uint32_t CFG [0x004];                           /*!< Offset 0x000 Configure Register */
	__IO uint32_t DATA;                                  /*!< Offset 0x010 Data Register */
	__IO uint32_t DRV [0x004];                           /*!< Offset 0x014 Multi_Driving Register */
	__IO uint32_t PULL [0x002];                          /*!< Offset 0x024 Pull Register */
	__IO uint32_t padding;                               /*!< Offset 0x02C Need for address arithmetics */
} GPIO_TypeDef;
/*
 * @brief SMHC
 */
/*!< SMHC Controller Interface */
typedef struct SMHC_Type
{
	__IO uint32_t SMHC_CTRL;                             /*!< Offset 0x000 Control Register */
	__IO uint32_t SMHC_CLKDIV;                           /*!< Offset 0x004 Clock Control Register */
	__IO uint32_t SMHC_TMOUT;                            /*!< Offset 0x008 Time Out Register */
	__IO uint32_t SMHC_CTYPE;                            /*!< Offset 0x00C Bus Width Register */
	__IO uint32_t SMHC_BLKSIZ;                           /*!< Offset 0x010 Block Size Register */
	__IO uint32_t SMHC_BYTCNT;                           /*!< Offset 0x014 Byte Count Register */
	__IO uint32_t SMHC_CMD;                              /*!< Offset 0x018 Command Register */
	__IO uint32_t SMHC_CMDARG;                           /*!< Offset 0x01C Command Argument Register */
	__IO uint32_t SMHC_RESP0;                            /*!< Offset 0x020 Response 0 Register */
	__IO uint32_t SMHC_RESP1;                            /*!< Offset 0x024 Response 1 Register */
	__IO uint32_t SMHC_RESP2;                            /*!< Offset 0x028 Response 2 Register */
	__IO uint32_t SMHC_RESP3;                            /*!< Offset 0x02C Response 3 Register */
	__IO uint32_t SMHC_INTMASK;                          /*!< Offset 0x030 Interrupt Mask Register */
	__IO uint32_t SMHC_MINTSTS;                          /*!< Offset 0x034 Masked Interrupt Status Register */
	__IO uint32_t SMHC_RINTSTS;                          /*!< Offset 0x038 Raw Interrupt Status Register */
	__IO uint32_t SMHC_STATUS;                           /*!< Offset 0x03C Status Register */
	__IO uint32_t SMHC_FIFOTH;                           /*!< Offset 0x040 FIFO Water Level Register */
	__IO uint32_t SMHC_FUNS;                             /*!< Offset 0x044 FIFO Function Select Register */
	__IO uint32_t SMHC_TCBCNT;                           /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
	__IO uint32_t SMHC_TBBCNT;                           /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
	__IO uint32_t SMHC_DBGC;                             /*!< Offset 0x050 Current Debug Control Register */
	__IO uint32_t SMHC_CSDC;                             /*!< Offset 0x054 CRC Status Detect Control Registers */
	__IO uint32_t SMHC_A12A;                             /*!< Offset 0x058 Auto Command 12 Argument Register */
	__IO uint32_t SMHC_NTSR;                             /*!< Offset 0x05C SD New Timing Set Register */
	uint32_t reserved1 [0x0006];
	__IO uint32_t SMHC_HWRST;                            /*!< Offset 0x078 Hardware Reset Register */
	uint32_t reserved2;
	__IO uint32_t SMHC_IDMAC;                            /*!< Offset 0x080 IDMAC Control Register */
	__IO uint32_t SMHC_DLBA;                             /*!< Offset 0x084 Descriptor List Base Address Register */
	__IO uint32_t SMHC_IDST;                             /*!< Offset 0x088 IDMAC Status Register */
	__IO uint32_t SMHC_IDIE;                             /*!< Offset 0x08C IDMAC Interrupt Enable Register */
	uint32_t reserved3 [0x001C];
	__IO uint32_t SMHC_THLD;                             /*!< Offset 0x100 Card Threshold Control Register */
	__IO uint32_t SMHC_SFC;                              /*!< Offset 0x104 Sample FIFO Control Register */
	__IO uint32_t SMHC_A23A;                             /*!< Offset 0x108 Auto Command 23 Argument Register */
	__IO uint32_t EMMC_DDR_SBIT_DET;                     /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
	uint32_t reserved4 [0x000A];
	__IO uint32_t SMHC_EXT_CMD;                          /*!< Offset 0x138 Extended Command Register */
	__IO uint32_t SMHC_EXT_RESP;                         /*!< Offset 0x13C Extended Response Register */
	__IO uint32_t SMHC_DRV_DL;                           /*!< Offset 0x140 Drive Delay Control Register */
	__IO uint32_t SMHC_SMAP_DL;                          /*!< Offset 0x144 Sample Delay Control Register */
	__IO uint32_t SMHC_DS_DL;                            /*!< Offset 0x148 Data Strobe Delay Control Register */
	__IO uint32_t SMHC_HS400_DL;                         /*!< Offset 0x14C HS400 Delay Control Register */
	uint32_t reserved5 [0x002C];
	__IO uint32_t SMHC_FIFO;                             /*!< Offset 0x200 Read/Write FIFO */
} SMHC_TypeDef;
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM Controller Interface */
typedef struct I2S_PCM_Type
{
	__IO uint32_t I2S_PCM_CTL;                           /*!< Offset 0x000 I2S/PCM Control Register */
	__IO uint32_t I2S_PCM_FMT0;                          /*!< Offset 0x004 I2S/PCM Format Register 0 */
	__IO uint32_t I2S_PCM_FMT1;                          /*!< Offset 0x008 I2S/PCM Format Register 1 */
	__IO uint32_t I2S_PCM_ISTA;                          /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
	__IO uint32_t I2S_PCM_RXFIFO;                        /*!< Offset 0x010 I2S/PCM RXFIFO Register */
	__IO uint32_t I2S_PCM_FCTL;                          /*!< Offset 0x014 I2S/PCM FIFO Control Register */
	__IO uint32_t I2S_PCM_FSTA;                          /*!< Offset 0x018 I2S/PCM FIFO Status Register */
	__IO uint32_t I2S_PCM_INT;                           /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
	__IO uint32_t I2S_PCM_TXFIFO;                        /*!< Offset 0x020 I2S/PCM TXFIFO Register */
	__IO uint32_t I2S_PCM_CLKD;                          /*!< Offset 0x024 I2S/PCM Clock Divide Register */
	__IO uint32_t I2S_PCM_TXCNT;                         /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
	__IO uint32_t I2S_PCM_RXCNT;                         /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
	__IO uint32_t I2S_PCM_CHCFG;                         /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
	__IO uint32_t I2S_PCM_TX0CHSEL;                      /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
	__IO uint32_t I2S_PCM_TX1CHSEL;                      /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
	__IO uint32_t I2S_PCM_TX2CHSEL;                      /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
	__IO uint32_t I2S_PCM_TX3CHSEL;                      /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
	__IO uint32_t I2S_PCM_TX0CHMAP0;                     /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
	__IO uint32_t I2S_PCM_TX0CHMAP1;                     /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
	__IO uint32_t I2S_PCM_TX1CHMAP0;                     /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
	__IO uint32_t I2S_PCM_TX1CHMAP1;                     /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
	__IO uint32_t I2S_PCM_TX2CHMAP0;                     /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
	__IO uint32_t I2S_PCM_TX2CHMAP1;                     /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
	__IO uint32_t I2S_PCM_TX3CHMAP0;                     /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
	__IO uint32_t I2S_PCM_TX3CHMAP1;                     /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
	__IO uint32_t I2S_PCM_RXCHSEL;                       /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
	__IO uint32_t I2S_PCM_RXCHMAP0;                      /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0 */
	__IO uint32_t I2S_PCM_RXCHMAP1;                      /*!< Offset 0x06C I2S/PCM RX Channel Mapping Register1 */
	__IO uint32_t I2S_PCM_RXCHMAP2;                      /*!< Offset 0x070 I2S/PCM RX Channel Mapping Register2 */
	__IO uint32_t I2S_PCM_RXCHMAP3;                      /*!< Offset 0x074 I2S/PCM RX Channel Mapping Register3 */
	uint32_t reserved1 [0x0002];
	__IO uint32_t MCLKCFG;                               /*!< Offset 0x080 ASRC MCLK Configuration Register */
	__IO uint32_t FsoutCFG;                              /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
	__IO uint32_t FsinEXTCFG;                            /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
	__IO uint32_t ASRCCFG;                               /*!< Offset 0x08C ASRC Enable Register */
	__IO uint32_t ASRCMANCFG;                            /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
	__IO uint32_t ASRCRATIOSTAT;                         /*!< Offset 0x094 ASRC Status Register */
	__IO uint32_t ASRCFIFOSTAT;                          /*!< Offset 0x098 ASRC FIFO Level Status Register */
	__IO uint32_t ASRCMBISTCFG;                          /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
	__IO uint32_t ASRCMBISTSTAT;                         /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef;
/*
 * @brief DMIC
 */
/*!< DMIC Controller Interface */
typedef struct DMIC_Type
{
	__IO uint32_t DMIC_EN;                               /*!< Offset 0x000 DMIC Enable Control Register */
	__IO uint32_t DMIC_SR;                               /*!< Offset 0x004 DMIC Sample Rate Register */
	__IO uint32_t DMIC_CTR;                              /*!< Offset 0x008 DMIC Control Register */
	uint32_t reserved1;
	__IO uint32_t DMIC_DATA;                             /*!< Offset 0x010 DMIC Data Register */
	__IO uint32_t DMIC_INTC;                             /*!< Offset 0x014 MIC Interrupt Control Register */
	__IO uint32_t DMIC_INTS;                             /*!< Offset 0x018 DMIC Interrupt Status Register */
	__IO uint32_t DMIC_RXFIFO_CTR;                       /*!< Offset 0x01C DMIC RXFIFO Control Register */
	__IO uint32_t DMIC_RXFIFO_STA;                       /*!< Offset 0x020 DMIC RXFIFO Status Register */
	__IO uint32_t DMIC_CH_NUM;                           /*!< Offset 0x024 DMIC Channel Numbers Register */
	__IO uint32_t DMIC_CH_MAP;                           /*!< Offset 0x028 DMIC Channel Mapping Register */
	__IO uint32_t DMIC_CNT;                              /*!< Offset 0x02C DMIC Counter Register */
	__IO uint32_t DATA0_DATA1_VOL_CTR;                   /*!< Offset 0x030 Data0 and Data1 Volume Control Register */
	__IO uint32_t DATA2_DATA3_VOL_CTR;                   /*!< Offset 0x034 Data2 And Data3 Volume Control Register */
	__IO uint32_t HPF_EN_CTR;                            /*!< Offset 0x038 High Pass Filter Enable Control Register */
	__IO uint32_t HPF_COEF_REG;                          /*!< Offset 0x03C High Pass Filter Coefficient Register */
	__IO uint32_t HPF_GAIN_REG;                          /*!< Offset 0x040 High Pass Filter Gain Register */
} DMIC_TypeDef;
/*
 * @brief OWA
 */
/*!< OWA Controller Interface */
typedef struct OWA_Type
{
	__IO uint32_t OWA_GEN_CTL;                           /*!< Offset 0x000 OWA General Control Register */
	__IO uint32_t OWA_TX_CFIG;                           /*!< Offset 0x004 OWA TX Configuration Register */
	__IO uint32_t OWA_RX_CFIG;                           /*!< Offset 0x008 OWA RX Configuration Register */
	__IO uint32_t OWA_ISTA;                              /*!< Offset 0x00C OWA Interrupt Status Register */
	__IO uint32_t OWA_RXFIFO;                            /*!< Offset 0x010 OWA RXFIFO Register */
	__IO uint32_t OWA_FCTL;                              /*!< Offset 0x014 OWA FIFO Control Register */
	__IO uint32_t OWA_FSTA;                              /*!< Offset 0x018 OWA FIFO Status Register */
	__IO uint32_t OWA_INT;                               /*!< Offset 0x01C OWA Interrupt Control Register */
	__IO uint32_t OWA_TX_FIFO;                           /*!< Offset 0x020 OWA TX FIFO Register */
	__IO uint32_t OWA_TX_CNT;                            /*!< Offset 0x024 OWA TX Counter Register */
	__IO uint32_t OWA_RX_CNT;                            /*!< Offset 0x028 OWA RX Counter Register */
	__IO uint32_t OWA_TX_CHSTA0;                         /*!< Offset 0x02C OWA TX Channel Status Register0 */
	__IO uint32_t OWA_TX_CHSTA1;                         /*!< Offset 0x030 OWA TX Channel Status Register1 */
	__IO uint32_t OWA_RXCHSTA0;                          /*!< Offset 0x034 OWA RX Channel Status Register0 */
	__IO uint32_t OWA_RXCHSTA1;                          /*!< Offset 0x038 OWA RX Channel Status Register1 */
	uint32_t reserved1;
	__IO uint32_t OWA_EXP_CTL;                           /*!< Offset 0x040 OWA Expand Control Register */
	__IO uint32_t OWA_EXP_ISTA;                          /*!< Offset 0x044 OWA Expand Interrupt Status Register */
	__IO uint32_t OWA_EXP_INFO_0;                        /*!< Offset 0x048 OWA Expand Infomation Register0 */
	__IO uint32_t OWA_EXP_INFO_1;                        /*!< Offset 0x04C OWA Expand Infomation Register1 */
	__IO uint32_t OWA_EXP_DBG_0;                         /*!< Offset 0x050 OWA Expand Debug Register0 */
	__IO uint32_t OWA_EXP_DBG_1;                         /*!< Offset 0x054 OWA Expand Debug Register1 */
} OWA_TypeDef;
/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Controller Interface */
typedef struct AUDIO_CODEC_Type
{
	__IO uint32_t AC_DAC_DPC;                            /*!< Offset 0x000 DAC Digital Part Control Register */
	__IO uint32_t DAC_VOL_CTRL;                          /*!< Offset 0x004 DAC Volume Control Register */
	uint32_t reserved1 [0x0002];
	__IO uint32_t AC_DAC_FIFOC;                          /*!< Offset 0x010 DAC FIFO Control Register */
	__IO uint32_t AC_DAC_FIFOS;                          /*!< Offset 0x014 DAC FIFO Status Register */
	uint32_t reserved2 [0x0002];
	__IO uint32_t AC_DAC_TXDATA;                         /*!< Offset 0x020 DAC TX DATA Register */
	__IO uint32_t AC_DAC_CNT;                            /*!< Offset 0x024 DAC TX FIFO Counter Register */
	__IO uint32_t AC_DAC_DG;                             /*!< Offset 0x028 DAC Debug Register */
	uint32_t reserved3;
	__IO uint32_t AC_ADC_FIFOC;                          /*!< Offset 0x030 ADC FIFO Control Register */
	__IO uint32_t ADC_VOL_CTRL1;                         /*!< Offset 0x034 ADC Volume Control1 Register */
	__IO uint32_t AC_ADC_FIFOS;                          /*!< Offset 0x038 ADC FIFO Status Register */
	uint32_t reserved4;
	__IO uint32_t AC_ADC_RXDATA;                         /*!< Offset 0x040 ADC RX Data Register */
	__IO uint32_t AC_ADC_CNT;                            /*!< Offset 0x044 ADC RX Counter Register */
	uint32_t reserved5;
	__IO uint32_t AC_ADC_DG;                             /*!< Offset 0x04C ADC Debug Register */
	__IO uint32_t ADC_DIG_CTRL;                          /*!< Offset 0x050 ADC Digtial Control Register */
	__IO uint32_t VRA1SPEEDUP_DOWN_CTRL;                 /*!< Offset 0x054 VRA1 Speedup Down Control Register */
	uint32_t reserved6 [0x0026];
	__IO uint32_t AC_DAC_DAP_CTRL;                       /*!< Offset 0x0F0 DAC DAP Control Register */
	uint32_t reserved7;
	__IO uint32_t AC_ADC_DAP_CTR;                        /*!< Offset 0x0F8 ADC DAP Control Register */
	uint32_t reserved8;
	__IO uint32_t AC_DAC_DRC_HHPFC;                      /*!< Offset 0x100 DAC DRC High HPF Coef Register */
	__IO uint32_t AC_DAC_DRC_LHPFC;                      /*!< Offset 0x104 DAC DRC Low HPF Coef Register */
	__IO uint32_t AC_DAC_DRC_CTRL;                       /*!< Offset 0x108 DAC DRC Control Register */
	__IO uint32_t AC_DAC_DRC_LPFHAT;                     /*!< Offset 0x10C DAC DRC Left Peak Filter High Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_LPFLAT;                     /*!< Offset 0x110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_RPFHAT;                     /*!< Offset 0x114 DAC DRC Right Peak Filter High Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_RPFLAT;                     /*!< Offset 0x118 DAC DRC Peak Filter Low Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_LPFHRT;                     /*!< Offset 0x11C DAC DRC Left Peak Filter High Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_LPFLRT;                     /*!< Offset 0x120 DAC DRC Left Peak Filter Low Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_RPFHRT;                     /*!< Offset 0x124 DAC DRC Right Peak filter High Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_RPFLRT;                     /*!< Offset 0x128 DAC DRC Right Peak filter Low Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_LRMSHAT;                    /*!< Offset 0x12C DAC DRC Left RMS Filter High Coef Register */
	__IO uint32_t AC_DAC_DRC_LRMSLAT;                    /*!< Offset 0x130 DAC DRC Left RMS Filter Low Coef Register */
	__IO uint32_t AC_DAC_DRC_RRMSHAT;                    /*!< Offset 0x134 DAC DRC Right RMS Filter High Coef Register */
	__IO uint32_t AC_DAC_DRC_RRMSLAT;                    /*!< Offset 0x138 DAC DRC Right RMS Filter Low Coef Register */
	__IO uint32_t AC_DAC_DRC_HCT;                        /*!< Offset 0x13C DAC DRC Compressor Threshold High Setting Register */
	__IO uint32_t AC_DAC_DRC_LCT;                        /*!< Offset 0x140 DAC DRC Compressor Slope High Setting Register */
	__IO uint32_t AC_DAC_DRC_HKC;                        /*!< Offset 0x144 DAC DRC Compressor Slope High Setting Register */
	__IO uint32_t AC_DAC_DRC_LKC;                        /*!< Offset 0x148 DAC DRC Compressor Slope Low Setting Register */
	__IO uint32_t AC_DAC_DRC_HOPC;                       /*!< Offset 0x14C DAC DRC Compresso */
	__IO uint32_t AC_DAC_DRC_LOPC;                       /*!< Offset 0x150 DAC DRC Compressor Low Output at Compressor Threshold Register */
	__IO uint32_t AC_DAC_DRC_HLT;                        /*!< Offset 0x154 DAC DRC Limiter Threshold High Setting Register */
	__IO uint32_t AC_DAC_DRC_LLT;                        /*!< Offset 0x158 DAC DRC Limiter Threshold Low Setting Register */
	__IO uint32_t AC_DAC_DRC_HKl;                        /*!< Offset 0x15C DAC DRC Limiter Slope High Setting Register */
	__IO uint32_t AC_DAC_DRC_LKl;                        /*!< Offset 0x160 DAC DRC Limiter Slope Low Setting Register */
	__IO uint32_t AC_DAC_DRC_HOPL;                       /*!< Offset 0x164 DAC DRC Limiter High Output at Limiter Threshold */
	__IO uint32_t AC_DAC_DRC_LOPL;                       /*!< Offset 0x168 DAC DRC Limiter Low Output at Limiter Threshold */
	__IO uint32_t AC_DAC_DRC_HET;                        /*!< Offset 0x16C DAC DRC Expander Threshold High Setting Register */
	__IO uint32_t AC_DAC_DRC_LET;                        /*!< Offset 0x170 DAC DRC Expander Threshold Low Setting Register */
	__IO uint32_t AC_DAC_DRC_HKE;                        /*!< Offset 0x174 DAC DRC Expander Slope High Setting Register */
	__IO uint32_t AC_DAC_DRC_LKE;                        /*!< Offset 0x178 DAC DRC Expander Slope Low Setting Register */
	__IO uint32_t AC_DAC_DRC_HOPE;                       /*!< Offset 0x17C DAC DRC Expander High Output at Expander Threshold */
	__IO uint32_t AC_DAC_DRC_LOPE;                       /*!< Offset 0x180 DAC DRC Expander Low Output at Expander Threshold */
	__IO uint32_t AC_DAC_DRC_HKN;                        /*!< Offset 0x184 DAC DRC Linear Slope High Setting Register */
	__IO uint32_t AC_DAC_DRC_LKN;                        /*!< Offset 0x188 DAC DRC Linear Slope Low Setting Register */
	__IO uint32_t AC_DAC_DRC_SFHAT;                      /*!< Offset 0x18C DAC DRC Smooth filter Gain High Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_SFLAT;                      /*!< Offset 0x190 DAC DRC Smooth filter Gain Low Attack Time Coef Register */
	__IO uint32_t AC_DAC_DRC_SFHRT;                      /*!< Offset 0x194 DAC DRC Smooth filter Gain High Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_SFLRT;                      /*!< Offset 0x198 DAC DRC Smooth filter Gain Low Release Time Coef Register */
	__IO uint32_t AC_DAC_DRC_MXGHS;                      /*!< Offset 0x19C DAC DRC MAX Gain High Setting Register */
	__IO uint32_t AC_DAC_DRC_MXGLS;                      /*!< Offset 0x1A0 DAC DRC MAX Gain Low Setting Register */
	__IO uint32_t AC_DAC_DRC_MNGHS;                      /*!< Offset 0x1A4 DAC DRC MIN Gain High Setting Register */
	__IO uint32_t AC_DAC_DRC_MNGLS;                      /*!< Offset 0x1A8 DAC DRC MIN Gain Low Setting Register */
	__IO uint32_t AC_DAC_DRC_EPSHC;                      /*!< Offset 0x1AC DAC DRC Expander Smooth Time High Coef Register */
	__IO uint32_t AC_DAC_DRC_EPSLC;                      /*!< Offset 0x1B0 DAC DRC Expander Smooth Time Low Coef Register */
	uint32_t reserved9;
	__IO uint32_t AC_DAC_DRC_HPFHGAIN;                   /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
	__IO uint32_t AC_DAC_DRC_HPFLGAIN;                   /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
	uint32_t reserved10 [0x0010];
	__IO uint32_t AC_ADC_DRC_HHPFC;                      /*!< Offset 0x200 ADC DRC High HPF Coef Register */
	__IO uint32_t AC_ADC_DRC_LHPFC;                      /*!< Offset 0x204 ADC DRC Low HPF Coef Register */
	__IO uint32_t AC_ADC_DRC_CTRL;                       /*!< Offset 0x208 ADC DRC Control Register */
	__IO uint32_t AC_ADC_DRC_LPFHAT;                     /*!< Offset 0x20C ADC DRC Left Peak Filter High Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_LPFLAT;                     /*!< Offset 0x210 ADC DRC Left Peak Filter Low Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_RPFHAT;                     /*!< Offset 0x214 ADC DRC Right Peak Filter High Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_RPFLAT;                     /*!< Offset 0x218 ADC DRC Right Peak Filter Low Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_LPFHRT;                     /*!< Offset 0x21C ADC DRC Left Peak Filter High Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_LPFLRT;                     /*!< Offset 0x220 ADC DRC Left Peak Filter Low Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_RPFHRT;                     /*!< Offset 0x224 ADC DRC Right Peak Filter High Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_RPFLRT;                     /*!< Offset 0x228 ADC DRC Right Peak Filter Low Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_LRMSHAT;                    /*!< Offset 0x22C ADC DRC Left RMS Filter High Coef Register */
	__IO uint32_t AC_ADC_DRC_LRMSLAT;                    /*!< Offset 0x230 ADC DRC Left RMS Filter Low Coef Register */
	__IO uint32_t AC_ADC_DRC_RRMSHAT;                    /*!< Offset 0x234 ADC DRC Right RMS Filter High Coef Register */
	__IO uint32_t AC_ADC_DRC_RRMSLAT;                    /*!< Offset 0x238 ADC DRC Right RMS Filter Low Coef Register */
	__IO uint32_t AC_ADC_DRC_HCT;                        /*!< Offset 0x23C ADC DRC Compressor Threshold High Setting Register */
	__IO uint32_t AC_ADC_DRC_LCT;                        /*!< Offset 0x240 ADC DRC Compressor Slope High Setting Register */
	__IO uint32_t AC_ADC_DRC_HKC;                        /*!< Offset 0x244 ADC DRC Compressor Slope High Setting Register */
	__IO uint32_t AC_ADC_DRC_LKC;                        /*!< Offset 0x248 ADC DRC Compressor Slope Low Setting Register */
	__IO uint32_t AC_ADC_DRC_HOPC;                       /*!< Offset 0x24C ADC DRC Compressor High Output at Compressor Threshold Register */
	__IO uint32_t AC_ADC_DRC_LOPC;                       /*!< Offset 0x250 ADC DRC Compressor Low Output at Compressor Threshold Register */
	__IO uint32_t AC_ADC_DRC_HLT;                        /*!< Offset 0x254 ADC DRC Limiter Threshold High Setting Register */
	__IO uint32_t AC_ADC_DRC_LLT;                        /*!< Offset 0x258 ADC DRC Limiter Threshold Low Setting Register */
	__IO uint32_t AC_ADC_DRC_HKl;                        /*!< Offset 0x25C ADC DRC Limiter Slope High Setting Register */
	__IO uint32_t AC_ADC_DRC_LKl;                        /*!< Offset 0x260 ADC DRC Limiter Slope Low Setting Register */
	__IO uint32_t AC_ADC_DRC_HOPL;                       /*!< Offset 0x264 ADC DRC Limiter High Output at Limiter Threshold */
	__IO uint32_t AC_ADC_DRC_LOPL;                       /*!< Offset 0x268 ADC DRC Limiter Low Output at Limiter Threshold */
	__IO uint32_t AC_ADC_DRC_HET;                        /*!< Offset 0x26C ADC DRC Expander Threshold High Setting Register */
	__IO uint32_t AC_ADC_DRC_LET;                        /*!< Offset 0x270 ADC DRC Expander Threshold Low Setting Register */
	__IO uint32_t AC_ADC_DRC_HKE;                        /*!< Offset 0x274 ADC DRC Expander Slope High Setting Register */
	__IO uint32_t AC_ADC_DRC_LKE;                        /*!< Offset 0x278 ADC DRC Expander Slope Low Setting Register */
	__IO uint32_t AC_ADC_DRC_HOPE;                       /*!< Offset 0x27C ADC DRC Expander High Output at Expander Threshold */
	__IO uint32_t AC_ADC_DRC_LOPE;                       /*!< Offset 0x280 ADC DRC Expander Low Output at Expander Threshold */
	__IO uint32_t AC_ADC_DRC_HKN;                        /*!< Offset 0x284 ADC DRC Linear Slope High Setting Register */
	__IO uint32_t AC_ADC_DRC_LKN;                        /*!< Offset 0x288 ADC DRC Linear Slope Low Setting Register */
	__IO uint32_t AC_ADC_DRC_SFHAT;                      /*!< Offset 0x28C ADC DRC Smooth filter Gain High Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_SFLAT;                      /*!< Offset 0x290 ADC DRC Smooth filter Gain Low Attack Time Coef Register */
	__IO uint32_t AC_ADC_DRC_SFHRT;                      /*!< Offset 0x294 ADC DRC Smooth filter Gain High Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_SFLRT;                      /*!< Offset 0x298 ADC DRC Smooth filter Gain Low Release Time Coef Register */
	__IO uint32_t AC_ADC_DRC_MXGHS;                      /*!< Offset 0x29C ADC DRC MAX Gain High Setting Register */
	__IO uint32_t AC_ADC_DRC_MXGLS;                      /*!< Offset 0x2A0 ADC DRC MAX Gain Low Setting Register */
	__IO uint32_t AC_ADC_DRC_MNGHS;                      /*!< Offset 0x2A4 ADC DRC MIN Gain High Setting Register */
	__IO uint32_t AC_ADC_DRC_MNGLS;                      /*!< Offset 0x2A8 ADC DRC MIN Gain Low Setting Register */
	__IO uint32_t AC_ADC_DRC_EPSHC;                      /*!< Offset 0x2AC ADC DRC Expander Smooth Time High Coef Register */
	__IO uint32_t AC_ADC_DRC_EPSLC;                      /*!< Offset 0x2B0 ADC DRC Expander Smooth Time Low Coef Register */
	uint32_t reserved11;
	__IO uint32_t AC_ADC_DRC_HPFHGAIN;                   /*!< Offset 0x2B8 ADC DRC HPF Gain High Coef Register */
	__IO uint32_t AC_ADC_DRC_HPFLGAIN;                   /*!< Offset 0x2BC ADC DRC HPF Gain Low Coef Register */
	uint32_t reserved12 [0x0010];
	__IO uint32_t ADC1_REG;                              /*!< Offset 0x300 ADC1 Analog Control Register */
	__IO uint32_t ADC2_REG;                              /*!< Offset 0x304 ADC2 Analog Control Register */
	__IO uint32_t ADC3_REG;                              /*!< Offset 0x308 ADC3 Analog Control Register */
	uint32_t reserved13;
	__IO uint32_t DAC_REG;                               /*!< Offset 0x310 DAC Analog Control Register */
	uint32_t reserved14;
	__IO uint32_t MICBIAS_REG;                           /*!< Offset 0x318 MICBIAS Analog Control Register */
	__IO uint32_t RAMP_REG;                              /*!< Offset 0x31C BIAS Analog Control Register */
	__IO uint32_t BIAS_REG;                              /*!< Offset 0x320 BIAS Analog Control Register */
	uint32_t reserved15 [0x0003];
	__IO uint32_t ADC5_REG;                              /*!< Offset 0x330 ADC5 Analog Control Register */
} AUDIO_CODEC_TypeDef;
/*
 * @brief TWI
 */
/*!< TWI Controller Interface */
typedef struct TWI_Type
{
	__IO uint32_t TWI_ADDR;                              /*!< Offset 0x000 TWI Slave Address Register */
	__IO uint32_t TWI_XADDR;                             /*!< Offset 0x004 TWI Extended Slave Address Register */
	__IO uint32_t TWI_DATA;                              /*!< Offset 0x008 TWI Data Byte Register */
	__IO uint32_t TWI_CNTR;                              /*!< Offset 0x00C TWI Control Register */
	__IO uint32_t TWI_STAT;                              /*!< Offset 0x010 TWI Status Register */
	__IO uint32_t TWI_CCR;                               /*!< Offset 0x014 TWI Clock Control Register */
	__IO uint32_t TWI_SRST;                              /*!< Offset 0x018 TWI Software Reset Register */
	__IO uint32_t TWI_EFR;                               /*!< Offset 0x01C TWI Enhance Feature Register */
	__IO uint32_t TWI_LCR;                               /*!< Offset 0x020 TWI Line Control Register */
	uint32_t reserved1 [0x0077];
	__IO uint32_t TWI_DRV_CTRL;                          /*!< Offset 0x200 TWI_DRV Control Register */
	__IO uint32_t TWI_DRV_CFG;                           /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
	__IO uint32_t TWI_DRV_SLV;                           /*!< Offset 0x208 TWI_DRV Slave ID Register */
	__IO uint32_t TWI_DRV_FMT;                           /*!< Offset 0x20C TWI_DRV Packet Format Register */
	__IO uint32_t TWI_DRV_BUS_CTRL;                      /*!< Offset 0x210 TWI_DRV Bus Control Register */
	__IO uint32_t TWI_DRV_INT_CTRL;                      /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
	__IO uint32_t TWI_DRV_DMA_CFG;                       /*!< Offset 0x218 TWI_DRV DMA Configure Register */
	__IO uint32_t TWI_DRV_FIFO_CON;                      /*!< Offset 0x21C TWI_DRV FIFO Content Register */
	uint32_t reserved2 [0x0038];
	__IO uint32_t TWI_DRV_SEND_FIFO_ACC;                 /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
	__IO uint32_t TWI_DRV_RECV_FIFO_ACC;                 /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
} TWI_TypeDef;
/*
 * @brief SPI
 */
/*!< SPI Controller Interface */
typedef struct SPI_Type
{
	uint32_t reserved1;
	__IO uint32_t SPI_GCR;                               /*!< Offset 0x004 SPI Global Control Register */
	__IO uint32_t SPI_TCR;                               /*!< Offset 0x008 SPI Transfer Control Register */
	uint32_t reserved2;
	__IO uint32_t SPI_IER;                               /*!< Offset 0x010 SPI Interrupt Control Register */
	__IO uint32_t SPI_ISR;                               /*!< Offset 0x014 SPI Interrupt Status Register */
	__IO uint32_t SPI_FCR;                               /*!< Offset 0x018 SPI FIFO Control Register */
	__IO uint32_t SPI_FSR;                               /*!< Offset 0x01C SPI FIFO Status Register */
	__IO uint32_t SPI_WCR;                               /*!< Offset 0x020 SPI Wait Clock Register */
	uint32_t reserved3;
	__IO uint32_t SPI_SAMP_DL;                           /*!< Offset 0x028 SPI Sample Delay Control Register */
	uint32_t reserved4;
	__IO uint32_t SPI_MBC;                               /*!< Offset 0x030 SPI Master Burst Counter Register */
	__IO uint32_t SPI_MTC;                               /*!< Offset 0x034 SPI Master Transmit Counter Register */
	__IO uint32_t SPI_BCC;                               /*!< Offset 0x038 SPI Master Burst Control Register */
	uint32_t reserved5;
	__IO uint32_t SPI_BATCR;                             /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
	__IO uint32_t SPI_BA_CCR;                            /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
	__IO uint32_t SPI_TBR;                               /*!< Offset 0x048 SPI TX Bit Register */
	__IO uint32_t SPI_RBR;                               /*!< Offset 0x04C SPI RX Bit Register */
	uint32_t reserved6 [0x000E];
	__IO uint32_t SPI_NDMA_MODE_CTL;                     /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
	uint32_t reserved7 [0x005D];
	__IO uint32_t SPI_TXD;                               /*!< Offset 0x200 SPI TX Data Register */
	uint32_t reserved8 [0x003F];
	__IO uint32_t SPI_RXD;                               /*!< Offset 0x300 SPI RX Data Register */
} SPI_TypeDef;
/*
 * @brief CIR_RX
 */
/*!< CIR_RX Controller Interface */
typedef struct CIR_RX_Type
{
	__IO uint32_t CIR_CTL;                               /*!< Offset 0x000 CIR Control Register */
	uint32_t reserved1 [0x0003];
	__IO uint32_t CIR_RXPCFG;                            /*!< Offset 0x010 CIR Receiver Pulse Configure Register */
	uint32_t reserved2 [0x0003];
	__IO uint32_t CIR_RXFIFO;                            /*!< Offset 0x020 CIR Receiver FIFO Register */
	uint32_t reserved3 [0x0002];
	__IO uint32_t CIR_RXINT;                             /*!< Offset 0x02C CIR Receiver Interrupt Control Register */
	__IO uint32_t CIR_RXSTA;                             /*!< Offset 0x030 CIR Receiver Status Register */
	__IO uint32_t CIR_RXCFG;                             /*!< Offset 0x034 CIR Receiver Configure Register */
} CIR_RX_TypeDef;
/*
 * @brief CIR_TX
 */
/*!< CIR_TX Controller Interface */
typedef struct CIR_TX_Type
{
	__IO uint32_t CIR_TGLR;                              /*!< Offset 0x000 CIR Transmit Global Register */
	__IO uint32_t CIR_TMCR;                              /*!< Offset 0x004 CIR Transmit Modulation Control Register */
	__IO uint32_t CIR_TCR;                               /*!< Offset 0x008 CIR Transmit Control Register */
	__IO uint32_t CIR_IDC_H;                             /*!< Offset 0x00C CIR Transmit Idle Duration Threshold High Bit Register */
	__IO uint32_t CIR_IDC_L;                             /*!< Offset 0x010 CIR Transmit Idle Duration Threshold Low Bit Register */
	__IO uint32_t CIR_TICR_H;                            /*!< Offset 0x014 CIR Transmit Idle Counter High Bit Register */
	__IO uint32_t CIR_TICR_L;                            /*!< Offset 0x018 CIR Transmit Idle Counter Low Bit Register */
	uint32_t reserved1;
	__IO uint32_t CIR_TEL;                               /*!< Offset 0x020 CIR TX FIFO Empty Level Register */
	__IO uint32_t CIR_TXINT;                             /*!< Offset 0x024 CIR Transmit Interrupt Control Register */
	__IO uint32_t CIR_TAC;                               /*!< Offset 0x028 CIR Transmit FIFO Available Counter Register */
	__IO uint32_t CIR_TXSTA;                             /*!< Offset 0x02C CIR Transmit Status Register */
	__IO uint32_t CIR_TXT;                               /*!< Offset 0x030 CIR Transmit Threshold Register */
	__IO uint32_t CIR_DMA;                               /*!< Offset 0x034 CIR DMA Control Register */
	uint32_t reserved2 [0x0012];
	__IO uint32_t CIR_TXFIFO;                            /*!< Offset 0x080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef;
/*
 * @brief LEDC
 */
/*!< LEDC Controller Interface */
typedef struct LEDC_Type
{
	__IO uint32_t LEDC_CTRL_REG;                         /*!< Offset 0x000 LEDC Control Register */
	__IO uint32_t LED_T01_TIMING_CTRL_REG;               /*!< Offset 0x004 LEDC T0 & T1 Timing Control Register */
	__IO uint32_t LEDC_DATA_FINISH_CNT_REG;              /*!< Offset 0x008 LEDC Data Finish Counter Register */
	__IO uint32_t LED_RESET_TIMING_CTRL_REG;             /*!< Offset 0x00C LEDC Reset Timing Control Register */
	__IO uint32_t LEDC_WAIT_TIME0_CTRL_REG;              /*!< Offset 0x010 LEDC Wait Time0 Control Register */
	__IO uint32_t LEDC_DATA_REG;                         /*!< Offset 0x014 LEDC Data Register */
	__IO uint32_t LEDC_DMA_CTRL_REG;                     /*!< Offset 0x018 LEDC DMA Control Register */
	__IO uint32_t LEDC_INT_CTRL_REG;                     /*!< Offset 0x01C LEDC Interrupt Control Register */
	__IO uint32_t LEDC_INT_STS_REG;                      /*!< Offset 0x020 LEDC Interrupt Status Register */
	uint32_t reserved1;
	__IO uint32_t LEDC_WAIT_TIME1_CTRL_REG;              /*!< Offset 0x028 LEDC Wait Time1 Control Register */
	uint32_t reserved2;
	__IO uint32_t LEDC_FIFO_DATA_REG [0x020];            /*!< Offset 0x030 LEDC FIFO Data Registers array */
} LEDC_TypeDef;
/*
 * @brief TPADC
 */
/*!< TPADC Controller Interface */
typedef struct TPADC_Type
{
	__IO uint32_t TP_CTRL_REG0;                          /*!< Offset 0x000 TP Control Register 0 */
	__IO uint32_t TP_CTRL_REG1;                          /*!< Offset 0x004 TP Control Register 1 */
	__IO uint32_t TP_CTRL_REG2;                          /*!< Offset 0x008 TP Control Register 2 */
	__IO uint32_t TP_CTRL_REG3;                          /*!< Offset 0x00C TP Control Register 3 */
	__IO uint32_t TP_INT_FIFO_CTRL_REG;                  /*!< Offset 0x010 TP Interrupt FIFO Control Register */
	__IO uint32_t TP_INT_FIFO_STAT_REG;                  /*!< Offset 0x014 TP Interrupt FIFO Status Register */
	uint32_t reserved1;
	__IO uint32_t TP_CALI_DATA_REG;                      /*!< Offset 0x01C TP Calibration Data Register */
	uint32_t reserved2;
	__IO uint32_t TP_DATA_REG;                           /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef;
/*
 * @brief GPADC
 */
/*!< GPADC Controller Interface */
typedef struct GPADC_Type
{
	__IO uint32_t GP_SR_CON;                             /*!< Offset 0x000 GPADC Sample Rate Configure Register */
	__IO uint32_t GP_CTRL;                               /*!< Offset 0x004 GPADC Control Register */
	__IO uint32_t GP_CS_EN;                              /*!< Offset 0x008 GPADC Compare and Select Enable Register */
	__IO uint32_t GP_FIFO_INTC;                          /*!< Offset 0x00C GPADC FIFO Interrupt Control Register */
	__IO uint32_t GP_FIFO_INTS;                          /*!< Offset 0x010 GPADC FIFO Interrupt Status Register */
	__IO uint32_t GP_FIFO_DATA;                          /*!< Offset 0x014 GPADC FIFO Data Register */
	__IO uint32_t GP_CDATA;                              /*!< Offset 0x018 GPADC Calibration Data Register */
	uint32_t reserved1;
	__IO uint32_t GP_DATAL_INTC;                         /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
	__IO uint32_t GP_DATAH_INTC;                         /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
	__IO uint32_t GP_DATA_INTC;                          /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
	uint32_t reserved2;
	__IO uint32_t GP_DATAL_INTS;                         /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
	__IO uint32_t GP_DATAH_INTS;                         /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
	__IO uint32_t GP_DATA_INTS;                          /*!< Offset 0x038 GPADC Data Interrupt Status Register */
	uint32_t reserved3;
	__IO uint32_t GP_CH0_CMP_DATA;                       /*!< Offset 0x040 GPADC CH0 Compare Data Register */
	uint32_t reserved4 [0x000F];
	__IO uint32_t GP_CH0_DATA;                           /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef;
/*
 * @brief SPI_DBI
 */
/*!< SPI_DBI Controller Interface */
typedef struct SPI_DBI_Type
{
	uint32_t reserved1;
	__IO uint32_t SPI_GCR;                               /*!< Offset 0x004 SPI Global Control Register */
	__IO uint32_t SPI_TCR;                               /*!< Offset 0x008 SPI Transfer Control Register */
	uint32_t reserved2;
	__IO uint32_t SPI_IER;                               /*!< Offset 0x010 SPI Interrupt Control Register */
	__IO uint32_t SPI_ISR;                               /*!< Offset 0x014 SPI Interrupt Status Register */
	__IO uint32_t SPI_FCR;                               /*!< Offset 0x018 SPI FIFO Control Register */
	__IO uint32_t SPI_FSR;                               /*!< Offset 0x01C SPI FIFO Status Register */
	__IO uint32_t SPI_WCR;                               /*!< Offset 0x020 SPI Wait Clock Register */
	uint32_t reserved3;
	__IO uint32_t SPI_SAMP_DL;                           /*!< Offset 0x028 SPI Sample Delay Control Register */
	uint32_t reserved4;
	__IO uint32_t SPI_MBC;                               /*!< Offset 0x030 SPI Master Burst Counter Register */
	__IO uint32_t SPI_MTC;                               /*!< Offset 0x034 SPI Master Transmit Counter Register */
	__IO uint32_t SPI_BCC;                               /*!< Offset 0x038 SPI Master Burst Control Register */
	uint32_t reserved5;
	__IO uint32_t SPI_BATCR;                             /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
	__IO uint32_t SPI_BA_CCR;                            /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
	__IO uint32_t SPI_TBR;                               /*!< Offset 0x048 SPI TX Bit Register */
	__IO uint32_t SPI_RBR;                               /*!< Offset 0x04C SPI RX Bit Register */
	uint32_t reserved6 [0x000E];
	__IO uint32_t SPI_NDMA_MODE_CTL;                     /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
	uint32_t reserved7 [0x001D];
	__IO uint32_t DBI_CTL_0;                             /*!< Offset 0x100 DBI Control Register 0 */
	__IO uint32_t DBI_CTL_1;                             /*!< Offset 0x104 DBI Control Register 1 */
	__IO uint32_t DBI_CTL_2;                             /*!< Offset 0x108 DBI Control Register 2 */
	__IO uint32_t DBI_TIMER;                             /*!< Offset 0x10C DBI Timer Control Register */
	__IO uint32_t DBI_VIDEO_SZIE;                        /*!< Offset 0x110 DBI Video Size Configuration Register */
	uint32_t reserved8 [0x0003];
	__IO uint32_t DBI_INT;                               /*!< Offset 0x120 DBI Interrupt Register */
	__IO uint32_t DBI_DEBUG_0;                           /*!< Offset 0x124 DBI BEBUG 0 Register */
	__IO uint32_t DBI_DEBUG_1;                           /*!< Offset 0x128 DBI BEBUG 1 Register */
	uint32_t reserved9 [0x0035];
	__IO uint32_t SPI_TXD;                               /*!< Offset 0x200 SPI TX Data register */
	uint32_t reserved10 [0x003F];
	__IO uint32_t SPI_RXD;                               /*!< Offset 0x300 SPI RX Data register */
} SPI_DBI_TypeDef;
/*
 * @brief CE_S
 */
/*!< CE_S Controller Interface */
typedef struct CE_S_Type
{
	__IO uint32_t CE_TDA;                                /*!< Offset 0x000 Task Descriptor Address */
	uint32_t reserved1;
	__IO uint32_t CE_ICR;                                /*!< Offset 0x008 Interrupt Control Register */
	__IO uint32_t CE_ISR;                                /*!< Offset 0x00C Interrupt Status Register */
	__IO uint32_t CE_TLR;                                /*!< Offset 0x010 Task Load Register */
	__IO uint32_t CE_TSR;                                /*!< Offset 0x014 Task Status Register */
	__IO uint32_t CE_ESR;                                /*!< Offset 0x018 Error Status Register */
	uint32_t reserved2 [0x0002];
	__IO uint32_t CE_CSA;                                /*!< Offset 0x024 DMA Current Source Address */
	__IO uint32_t CE_CDA;                                /*!< Offset 0x028 DMA Current Destination Address */
	__IO uint32_t CE_TPR;                                /*!< Offset 0x02C Throughput Register */
} CE_S_TypeDef;
/*
 * @brief RTC
 */
/*!< RTC Controller Interface */
typedef struct RTC_Type
{
	__IO uint32_t LOSC_CTRL_REG;                         /*!< Offset 0x000 Low Oscillator Control Register */
	__IO uint32_t LOSC_AUTO_SWT_STA_REG;                 /*!< Offset 0x004 LOSC Auto Switch Status Register */
	__IO uint32_t INTOSC_CLK_PRESCAL_REG;                /*!< Offset 0x008 Internal OSC Clock Pre-scalar Register */
	uint32_t reserved1;
	__IO uint32_t RTC_DAY_REG;                           /*!< Offset 0x010 RTC Year-Month-Day Register */
	__IO uint32_t RTC_HH_MM_SS_REG;                      /*!< Offset 0x014 RTC Hour-Minute-Second Register */
	uint32_t reserved2 [0x0002];
	__IO uint32_t ALARM0_DAY_SET_REG;                    /*!< Offset 0x020 Alarm 0 Day Setting Register */
	__IO uint32_t ALARM0_CUR_VLU_REG;                    /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
	__IO uint32_t ALARM0_ENABLE_REG;                     /*!< Offset 0x028 Alarm 0 Enable Register */
	__IO uint32_t ALARM0_IRQ_EN;                         /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
	__IO uint32_t ALARM0_IRQ_STA_REG;                    /*!< Offset 0x030 Alarm 0 IRQ Status Register */
	uint32_t reserved3 [0x0007];
	__IO uint32_t ALARM_CONFIG_REG;                      /*!< Offset 0x050 Alarm Configuration Register */
	uint32_t reserved4 [0x0003];
	__IO uint32_t F32K_FOUT_CTRL_GATING_REG;             /*!< Offset 0x060 32K Fanout Control Gating Register */
	uint32_t reserved5 [0x0027];
	__IO uint32_t GP_DATA_REG [0x008];                   /*!< Offset 0x100 General Purpose Register (N=0 to 7) */
	__IO uint32_t FBOOT_INFO_REG0;                       /*!< Offset 0x120 Fast Boot Information Register0 */
	__IO uint32_t FBOOT_INFO_REG1;                       /*!< Offset 0x124 Fast Boot Information Register1 */
	uint32_t reserved6 [0x000E];
	__IO uint32_t DCXO_CTRL_REG;                         /*!< Offset 0x160 DCXO Control Register */
	uint32_t reserved7 [0x000B];
	__IO uint32_t RTC_VIO_REG;                           /*!< Offset 0x190 RTC_VIO Regulation Register */
	uint32_t reserved8 [0x0017];
	__IO uint32_t IC_CHARA_REG;                          /*!< Offset 0x1F0 IC Characteristic Register */
	__IO uint32_t VDD_OFF_GATING_CTRL_REG;               /*!< Offset 0x1F4 VDD Off Gating Control Register */
	uint32_t reserved9 [0x0003];
	__IO uint32_t EFUSE_HV_PWRSWT_CTRL_REG;              /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
	uint32_t reserved10 [0x0042];
	__IO uint32_t RTC_SPI_CLK_CTRL_REG;                  /*!< Offset 0x310 RTC SPI Clock Control Register */
} RTC_TypeDef;
/*
 * @brief IOMMU
 */
/*!< IOMMU Controller Interface */
typedef struct IOMMU_Type
{
	uint32_t reserved1 [0x0004];
	__IO uint32_t IOMMU_RESET_REG;                       /*!< Offset 0x010 IOMMU Reset Register */
	uint32_t reserved2 [0x0003];
	__IO uint32_t IOMMU_ENABLE_REG;                      /*!< Offset 0x020 IOMMU Enable Register */
	uint32_t reserved3 [0x0003];
	__IO uint32_t IOMMU_BYPASS_REG;                      /*!< Offset 0x030 IOMMU Bypass Register */
	uint32_t reserved4 [0x0003];
	__IO uint32_t IOMMU_AUTO_GATING_REG;                 /*!< Offset 0x040 IOMMU Auto Gating Register */
	__IO uint32_t IOMMU_WBUF_CTRL_REG;                   /*!< Offset 0x044 IOMMU Write Buffer Control Register */
	__IO uint32_t IOMMU_OOO_CTRL_REG;                    /*!< Offset 0x048 IOMMU Out of Order Control Register */
	__IO uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;            /*!< Offset 0x04C IOMMU 4KB Boundary Protect Control Register */
	__IO uint32_t IOMMU_TTB_REG;                         /*!< Offset 0x050 IOMMU Translation Table Base Register */
	uint32_t reserved5 [0x0003];
	__IO uint32_t IOMMU_TLB_ENABLE_REG;                  /*!< Offset 0x060 IOMMU TLB Enable Register */
	uint32_t reserved6 [0x0003];
	__IO uint32_t IOMMU_TLB_PREFETCH_REG;                /*!< Offset 0x070 IOMMU TLB Prefetch Register */
	uint32_t reserved7 [0x0003];
	__IO uint32_t IOMMU_TLB_FLUSH_ENABLE_REG;            /*!< Offset 0x080 IOMMU TLB Flush Enable Register */
	__IO uint32_t IOMMU_TLB_IVLD_MODE_SEL_REG;           /*!< Offset 0x084 IOMMU TLB Invalidation Mode Select Register */
	__IO uint32_t IOMMU_TLB_IVLD_STA_ADDR_REG;           /*!< Offset 0x088 IOMMU TLB Invalidation Start Address Register */
	__IO uint32_t IOMMU_TLB_IVLD_END_ADDR_REG;           /*!< Offset 0x08C IOMMU TLB Invalidation End Address Register */
	__IO uint32_t IOMMU_TLB_IVLD_ADDR_REG;               /*!< Offset 0x090 IOMMU TLB Invalidation Address Register */
	__IO uint32_t IOMMU_TLB_IVLD_ADDR_MASK_REG;          /*!< Offset 0x094 IOMMU TLB Invalidation Address Mask Register */
	__IO uint32_t IOMMU_TLB_IVLD_ENABLE_REG;             /*!< Offset 0x098 IOMMU TLB Invalidation Enable Register */
	__IO uint32_t IOMMU_PC_IVLD_MODE_SEL_REG;            /*!< Offset 0x09C IOMMU PC Invalidation Mode Select Register */
	__IO uint32_t IOMMU_PC_IVLD_ADDR_REG;                /*!< Offset 0x0A0 IOMMU PC Invalidation Address Register */
	__IO uint32_t IOMMU_PC_IVLD_STA_ADDR_REG;            /*!< Offset 0x0A4 IOMMU PC Invalidation Start Address Register */
	__IO uint32_t IOMMU_PC_IVLD_ENABLE_REG;              /*!< Offset 0x0A8 IOMMU PC Invalidation Enable Register */
	__IO uint32_t IOMMU_PC_IVLD_END_ADDR_REG;            /*!< Offset 0x0AC IOMMU PC Invalidation End Address Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL0_REG;                /*!< Offset 0x0B0 IOMMU Domain Authority Control 0 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL1_REG;                /*!< Offset 0x0B4 IOMMU Domain Authority Control 1 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL2_REG;                /*!< Offset 0x0B8 IOMMU Domain Authority Control 2 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL3_REG;                /*!< Offset 0x0BC IOMMU Domain Authority Control 3 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL4_REG;                /*!< Offset 0x0C0 IOMMU Domain Authority Control 4 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL5_REG;                /*!< Offset 0x0C4 IOMMU Domain Authority Control 5 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL6_REG;                /*!< Offset 0x0C8 IOMMU Domain Authority Control 6 Register */
	__IO uint32_t IOMMU_DM_AUT_CTRL7_REG;                /*!< Offset 0x0CC IOMMU Domain Authority Control 7 Register */
	__IO uint32_t IOMMU_DM_AUT_OVWT_REG;                 /*!< Offset 0x0D0 IOMMU Domain Authority Overwrite Register */
	uint32_t reserved8 [0x000B];
	__IO uint32_t IOMMU_INT_ENABLE_REG;                  /*!< Offset 0x100 IOMMU Interrupt Enable Register */
	__IO uint32_t IOMMU_INT_CLR_REG;                     /*!< Offset 0x104 IOMMU Interrupt Clear Register */
	__IO uint32_t IOMMU_INT_STA_REG;                     /*!< Offset 0x108 IOMMU Interrupt Status Register */
	uint32_t reserved9;
	__IO uint32_t IOMMU_INT_ERR_ADDR0_REG;               /*!< Offset 0x110 IOMMU Interrupt Error Address 0 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR1_REG;               /*!< Offset 0x114 IOMMU Interrupt Error Address 1 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR2_REG;               /*!< Offset 0x118 IOMMU Interrupt Error Address 2 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR3_REG;               /*!< Offset 0x11C IOMMU Interrupt Error Address 3 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR4_REG;               /*!< Offset 0x120 IOMMU Interrupt Error Address 4 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR5_REG;               /*!< Offset 0x124 IOMMU Interrupt Error Address 5 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR6_REG;               /*!< Offset 0x128 IOMMU Interrupt Error Address 6 Register */
	uint32_t reserved10;
	__IO uint32_t IOMMU_INT_ERR_ADDR7_REG;               /*!< Offset 0x130 IOMMU Interrupt Error Address 7 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR8_REG;               /*!< Offset 0x134 IOMMU Interrupt Error Address 8 Register */
	uint32_t reserved11 [0x0006];
	__IO uint32_t IOMMU_INT_ERR_DATA0_REG;               /*!< Offset 0x150 IOMMU Interrupt Error Data 0 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA1_REG;               /*!< Offset 0x154 IOMMU Interrupt Error Data 1 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA2_REG;               /*!< Offset 0x158 IOMMU Interrupt Error Data 2 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA3_REG;               /*!< Offset 0x15C IOMMU Interrupt Error Data 3 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA4_REG;               /*!< Offset 0x160 IOMMU Interrupt Error Data 4 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA5_REG;               /*!< Offset 0x164 IOMMU Interrupt Error Data 5 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA6_REG;               /*!< Offset 0x168 IOMMU Interrupt Error Data 6 Register */
	uint32_t reserved12;
	__IO uint32_t IOMMU_INT_ERR_DATA7_REG;               /*!< Offset 0x170 IOMMU Interrupt Error Data 7 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA8_REG;               /*!< Offset 0x174 IOMMU Interrupt Error Data 8 Register */
	uint32_t reserved13 [0x0002];
	__IO uint32_t IOMMU_L1PG_INT_REG;                    /*!< Offset 0x180 IOMMU L1 Page Table Interrupt Register */
	__IO uint32_t IOMMU_L2PG_INT_REG;                    /*!< Offset 0x184 IOMMU L2 Page Table Interrupt Register */
	uint32_t reserved14 [0x0002];
	__IO uint32_t IOMMU_VA_REG;                          /*!< Offset 0x190 IOMMU Virtual Address Register */
	__IO uint32_t IOMMU_VA_DATA_REG;                     /*!< Offset 0x194 IOMMU Virtual Address Data Register */
	__IO uint32_t IOMMU_VA_CONFIG_REG;                   /*!< Offset 0x198 IOMMU Virtual Address Configuration Register */
	uint32_t reserved15 [0x0019];
	__IO uint32_t IOMMU_PMU_ENABLE_REG;                  /*!< Offset 0x200 IOMMU PMU Enable Register */
	uint32_t reserved16 [0x0003];
	__IO uint32_t IOMMU_PMU_CLR_REG;                     /*!< Offset 0x210 IOMMU PMU Clear Register */
	uint32_t reserved17 [0x0007];
	__IO uint32_t IOMMU_PMU_ACCESS_LOW0_REG;             /*!< Offset 0x230 IOMMU PMU Access Low 0 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH0_REG;            /*!< Offset 0x234 IOMMU PMU Access High 0 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW0_REG;                /*!< Offset 0x238 IOMMU PMU Hit Low 0 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH0_REG;               /*!< Offset 0x23C IOMMU PMU Hit High 0 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW1_REG;             /*!< Offset 0x240 IOMMU PMU Access Low 1 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH1_REG;            /*!< Offset 0x244 IOMMU PMU Access High 1 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW1_REG;                /*!< Offset 0x248 IOMMU PMU Hit Low 1 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH1_REG;               /*!< Offset 0x24C IOMMU PMU Hit High 1 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW2_REG;             /*!< Offset 0x250 IOMMU PMU Access Low 2 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH2_REG;            /*!< Offset 0x254 IOMMU PMU Access High 2 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW2_REG;                /*!< Offset 0x258 IOMMU PMU Hit Low 2 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH2_REG;               /*!< Offset 0x25C IOMMU PMU Hit High 2 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW3_REG;             /*!< Offset 0x260 IOMMU PMU Access Low 3 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH3_REG;            /*!< Offset 0x264 IOMMU PMU Access High 3 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW3_REG;                /*!< Offset 0x268 IOMMU PMU Hit Low 3 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH3_REG;               /*!< Offset 0x26C IOMMU PMU Hit High 3 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW4_REG;             /*!< Offset 0x270 IOMMU PMU Access Low 4 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH4_REG;            /*!< Offset 0x274 IOMMU PMU Access High 4 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW4_REG;                /*!< Offset 0x278 IOMMU PMU Hit Low 4 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH4_REG;               /*!< Offset 0x27C IOMMU PMU Hit High 4 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW5_REG;             /*!< Offset 0x280 IOMMU PMU Access Low 5 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH5_REG;            /*!< Offset 0x284 IOMMU PMU Access High 5 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW5_REG;                /*!< Offset 0x288 IOMMU PMU Hit Low 5 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH5_REG;               /*!< Offset 0x28C IOMMU PMU Hit High 5 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW6_REG;             /*!< Offset 0x290 IOMMU PMU Access Low 6 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH6_REG;            /*!< Offset 0x294 IOMMU PMU Access High 6 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW6_REG;                /*!< Offset 0x298 IOMMU PMU Hit Low 6 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH6_REG;               /*!< Offset 0x29C IOMMU PMU Hit High 6 Register */
	uint32_t reserved18 [0x000C];
	__IO uint32_t IOMMU_PMU_ACCESS_LOW7_REG;             /*!< Offset 0x2D0 IOMMU PMU Access Low 7 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH7_REG;            /*!< Offset 0x2D4 IOMMU PMU Access High 7 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW7_REG;                /*!< Offset 0x2D8 IOMMU PMU Hit Low 7 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH7_REG;               /*!< Offset 0x2DC IOMMU PMU Hit High 7 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW8_REG;             /*!< Offset 0x2E0 IOMMU PMU Access Low 8 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH8_REG;            /*!< Offset 0x2E4 IOMMU PMU Access High 8 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW8_REG;                /*!< Offset 0x2E8 IOMMU PMU Hit Low 8 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH8_REG;               /*!< Offset 0x2EC IOMMU PMU Hit High 8 Register */
	uint32_t reserved19 [0x0004];
	__IO uint32_t IOMMU_PMU_TL_LOW0_REG;                 /*!< Offset 0x300 IOMMU Total Latency Low 0 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH0_REG;                /*!< Offset 0x304 IOMMU Total Latency High 0 Register */
	__IO uint32_t IOMMU_PMU_ML0_REG;                     /*!< Offset 0x308 IOMMU Max Latency 0 Register */
	uint32_t reserved20;
	__IO uint32_t IOMMU_PMU_TL_LOW1_REG;                 /*!< Offset 0x310 IOMMU Total Latency Low 1 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH1_REG;                /*!< Offset 0x314 IOMMU Total Latency High 1 Register */
	__IO uint32_t IOMMU_PMU_ML1_REG;                     /*!< Offset 0x318 IOMMU Max Latency 1 Register */
	uint32_t reserved21;
	__IO uint32_t IOMMU_PMU_TL_LOW2_REG;                 /*!< Offset 0x320 IOMMU Total Latency Low 2 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH2_REG;                /*!< Offset 0x324 IOMMU Total Latency High 2 Register */
	__IO uint32_t IOMMU_PMU_ML2_REG;                     /*!< Offset 0x328 IOMMU Max Latency 2 Register */
	uint32_t reserved22;
	__IO uint32_t IOMMU_PMU_TL_LOW3_REG;                 /*!< Offset 0x330 IOMMU Total Latency Low 3 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH3_REG;                /*!< Offset 0x334 IOMMU Total Latency High 3 Register */
	__IO uint32_t IOMMU_PMU_ML3_REG;                     /*!< Offset 0x338 IOMMU Max Latency 3 Register */
	uint32_t reserved23;
	__IO uint32_t IOMMU_PMU_TL_LOW4_REG;                 /*!< Offset 0x340 IOMMU Total Latency Low 4 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH4_REG;                /*!< Offset 0x344 IOMMU Total Latency High 4 Register */
	__IO uint32_t IOMMU_PMU_ML4_REG;                     /*!< Offset 0x348 IOMMU Max Latency 4 Register */
	uint32_t reserved24;
	__IO uint32_t IOMMU_PMU_TL_LOW5_REG;                 /*!< Offset 0x350 IOMMU Total Latency Low 5 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH5_REG;                /*!< Offset 0x354 IOMMU Total Latency High 5 Register */
	__IO uint32_t IOMMU_PMU_ML5_REG;                     /*!< Offset 0x358 IOMMU Max Latency 5 Register */
	uint32_t reserved25;
	__IO uint32_t IOMMU_PMU_TL_LOW6_REG;                 /*!< Offset 0x360 IOMMU Total Latency Low 6 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH6_REG;                /*!< Offset 0x364 IOMMU Total Latency High 6 Register */
	__IO uint32_t IOMMU_PMU_ML6_REG;                     /*!< Offset 0x368 IOMMU Max Latency 6 Register */
} IOMMU_TypeDef;
/*
 * @brief THS
 */
/*!< THS Controller Interface */
typedef struct THS_Type
{
	__IO uint32_t THS_CTRL;                              /*!< Offset 0x000 THS Control Register */
	__IO uint32_t THS_EN;                                /*!< Offset 0x004 THS Enable Register */
	__IO uint32_t THS_PER;                               /*!< Offset 0x008 THS Period Control Register */
	uint32_t reserved1;
	__IO uint32_t THS_DATA_INTC;                         /*!< Offset 0x010 THS Data Interrupt Control Register */
	__IO uint32_t THS_SHUT_INTC;                         /*!< Offset 0x014 THS Shut Interrupt Control Register */
	__IO uint32_t THS_ALARM_INTC;                        /*!< Offset 0x018 THS Alarm Interrupt Control Register */
	uint32_t reserved2;
	__IO uint32_t THS_DATA_INTS;                         /*!< Offset 0x020 THS Data Interrupt Status Register */
	__IO uint32_t THS_SHUT_INTS;                         /*!< Offset 0x024 THS Shut Interrupt Status Register */
	__IO uint32_t THS_ALARMO_INTS;                       /*!< Offset 0x028 THS Alarm off Interrupt Status Register */
	__IO uint32_t THS_ALARM_INTS;                        /*!< Offset 0x02C THS Alarm Interrupt Status Register */
	__IO uint32_t THS_FILTER;                            /*!< Offset 0x030 THS Median Filter Control Register */
	uint32_t reserved3 [0x0003];
	__IO uint32_t THS_ALARM_CTRL;                        /*!< Offset 0x040 THS Alarm Threshold Control Register */
	uint32_t reserved4 [0x000F];
	__IO uint32_t THS_SHUTDOWN_CTRL;                     /*!< Offset 0x080 THS Shutdown Threshold Control Register */
	uint32_t reserved5 [0x0007];
	__IO uint32_t THS_CDATA;                             /*!< Offset 0x0A0 THS Calibration Data */
	uint32_t reserved6 [0x0007];
	__IO uint32_t THS_DATA;                              /*!< Offset 0x0C0 THS Data Register */
} THS_TypeDef;
/*
 * @brief TIMER
 */
/*!< TIMER Controller Interface */
typedef struct TIMER_Type
{
	__IO uint32_t TMR_IRQ_EN_REG;                        /*!< Offset 0x000 Timer IRQ Enable Register */
	__IO uint32_t TMR_IRQ_STA_REG;                       /*!< Offset 0x004 Timer Status Register */
	uint32_t reserved1 [0x0002];
	__IO uint32_t TMR0_CTRL_REG;                         /*!< Offset 0x010 Timer0 Control Register */
	__IO uint32_t TMR0_INTV_VALUE_REG;                   /*!< Offset 0x014 Timer0 Interval Value Register */
	__IO uint32_t TMR0_CUR_VALUE_REG;                    /*!< Offset 0x018 Timer0 Current Value Register */
	uint32_t reserved2;
	__IO uint32_t TMR1_CTRL_REG;                         /*!< Offset 0x020 Timer1 Control Register */
	__IO uint32_t TMR1_INTV_VALUE_REG;                   /*!< Offset 0x024 Timer1 Interval Value Register */
	__IO uint32_t TMR1_CUR_VALUE_REG;                    /*!< Offset 0x028 Timer1 Current Value Register */
	uint32_t reserved3 [0x001D];
	__IO uint32_t WDOG_IRQ_EN_REG;                       /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
	__IO uint32_t WDOG_IRQ_STA_REG;                      /*!< Offset 0x0A4 Watchdog Status Register */
	__IO uint32_t WDOG_SOFT_RST_REG;                     /*!< Offset 0x0A8 Watchdog Software Reset Register */
	uint32_t reserved4;
	__IO uint32_t WDOG_CTRL_REG;                         /*!< Offset 0x0B0 Watchdog Control Register */
	__IO uint32_t WDOG_CFG_REG;                          /*!< Offset 0x0B4 Watchdog Configuration Register */
	__IO uint32_t WDOG_MODE_REG;                         /*!< Offset 0x0B8 Watchdog Mode Register */
	__IO uint32_t WDOG_OUTPUT_CFG_REG;                   /*!< Offset 0x0BC Watchdog Output Configuration Register */
	__IO uint32_t AVS_CNT_CTL_REG;                       /*!< Offset 0x0C0 AVS Control Register */
	__IO uint32_t AVS_CNT0_REG;                          /*!< Offset 0x0C4 AVS Counter 0 Register */
	__IO uint32_t AVS_CNT1_REG;                          /*!< Offset 0x0C8 AVS Counter 1 Register */
	__IO uint32_t AVS_CNT_DIV_REG;                       /*!< Offset 0x0CC AVS Divisor Register */
} TIMER_TypeDef;
/*
 * @brief HSTIMER
 */
/*!< HSTIMER Controller Interface */
typedef struct HSTIMER_Type
{
	__IO uint32_t HS_TMR_IRQ_EN_REG;                     /*!< Offset 0x000 HS Timer IRQ Enable Register */
	__IO uint32_t HS_TMR_IRQ_STAS_REG;                   /*!< Offset 0x004 HS Timer Status Register */
	uint32_t reserved1 [0x0006];
	__IO uint32_t HS_TMR0_CTRL_REG;                      /*!< Offset 0x020 HS Timer0 Control Register */
	__IO uint32_t HS_TMR0_INTV_LO_REG;                   /*!< Offset 0x024 HS Timer0 Interval Value Low Register */
	__IO uint32_t HS_TMR0_INTV_HI_REG;                   /*!< Offset 0x028 HS Timer0 Interval Value High Register */
	__IO uint32_t HS_TMR0_CURNT_LO_REG;                  /*!< Offset 0x02C HS Timer0 Current Value Low Register */
	__IO uint32_t HS_TMR0_CURNT_HI_REG;                  /*!< Offset 0x030 HS Timer0 Current Value High Register */
	uint32_t reserved2 [0x0003];
	__IO uint32_t HS_TMR1_CTRL_REG;                      /*!< Offset 0x040 HS Timer1 Control Register */
	__IO uint32_t HS_TMR1_INTV_LO_REG;                   /*!< Offset 0x044 HS Timer1 Interval Value Low Register */
	__IO uint32_t HS_TMR1_INTV_HI_REG;                   /*!< Offset 0x048 HS Timer1 Interval Value High Register */
	__IO uint32_t HS_TMR1_CURNT_LO_REG;                  /*!< Offset 0x04C HS Timer1 Current Value Low Register */
	__IO uint32_t HS_TMR1_CURNT_HI_REG;                  /*!< Offset 0x050 HS Timer1 Current Value High Register */
} HSTIMER_TypeDef;
/*
 * @brief CAN
 */
/*!< CAN Controller Interface */
typedef struct CAN_Type
{
	__IO uint32_t CAN_MSEL;                              /*!< Offset 0x000 CAN mode select register */
	__IO uint32_t CAN_CMD;                               /*!< Offset 0x004 CAN command register */
	__IO uint32_t CAN_STA;                               /*!< Offset 0x008 CAN status register */
	__IO uint32_t CAN_INT;                               /*!< Offset 0x00C CAN interrupt register */
	__IO uint32_t CAN_INTEN;                             /*!< Offset 0x010 CAN interrupt enable register */
	__IO uint32_t CAN_BUSTIME;                           /*!< Offset 0x014 CAN bus timing register */
	__IO uint32_t CAN_TEWL;                              /*!< Offset 0x018 CAN TX error warning limit register */
	__IO uint32_t CAN_ERRC;                              /*!< Offset 0x01C CAN error counter register */
	__IO uint32_t CAN_RMCNT;                             /*!< Offset 0x020 CAN receive message counter register */
	__IO uint32_t CAN_RBUF_SADDR;                        /*!< Offset 0x024 CAN receive buffer start address register */
	__IO uint32_t CAN_ACPC;                              /*!< Offset 0x028 CAN acceptance code 0 register(reset mode) */
	__IO uint32_t CAN_ACPM;                              /*!< Offset 0x02C CAN acceptance mask 0 register(reset mode) */
	uint32_t reserved1 [0x0004];
	__IO uint32_t CAN_TRBUF0;                            /*!< Offset 0x040 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF1;                            /*!< Offset 0x044 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF2;                            /*!< Offset 0x048 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF3;                            /*!< Offset 0x04C CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF4;                            /*!< Offset 0x050 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF5;                            /*!< Offset 0x054 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF6;                            /*!< Offset 0x058 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF7;                            /*!< Offset 0x05C CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF8;                            /*!< Offset 0x060 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF9;                            /*!< Offset 0x064 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF10;                           /*!< Offset 0x068 CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF11;                           /*!< Offset 0x06C CAN TX/RX message buffer 0 register */
	__IO uint32_t CAN_TRBUF12;                           /*!< Offset 0x070 CAN TX/RX message buffer 0 register */
	uint32_t reserved2 [0x0043];
	__IO uint32_t CAN_RBUF_RBACK [0x030];                /*!< Offset 0x180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
	uint32_t reserved3 [0x0030];
	__IO uint32_t CAN_VERSION;                           /*!< Offset 0x300 CAN Version Register */
} CAN_TypeDef;
/*
 * @brief USB1
 */
/*!< USB1 Controller Interface */
typedef struct USB1_Type
{
	__IO uint16_t E_CAPLENGTH;                           /*!< Offset 0x000 EHCI Capability Register Length Register */
	__IO uint16_t E_HCIVERSION;                          /*!< Offset 0x002 EHCI Host Interface Version Number Register */
	__IO uint32_t E_HCSPARAMS;                           /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
	__IO uint32_t E_HCCPARAMS;                           /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
	__IO uint32_t E_HCSPPORTROUTE;                       /*!< Offset 0x00C EHCI Companion Port Route Description */
	__IO uint32_t E_USBCMD;                              /*!< Offset 0x010 EHCI USB Command Register */
	__IO uint32_t E_USBSTS;                              /*!< Offset 0x014 EHCI USB Status Register */
	__IO uint32_t E_USBINTR;                             /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
	__IO uint32_t E_FRINDEX;                             /*!< Offset 0x01C EHCI USB Frame Index Register */
	__IO uint32_t E_CTRLDSSEGMENT;                       /*!< Offset 0x020 EHCI 4G Segment Selector Register */
	__IO uint32_t E_PERIODICLISTBASE;                    /*!< Offset 0x024 EHCI Frame List Base Address Register */
	__IO uint32_t E_ASYNCLISTADDR;                       /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
	uint32_t reserved1 [0x0009];
	__IO uint32_t E_CONFIGFLAG;                          /*!< Offset 0x050 EHCI Configured Flag Register */
	__IO uint32_t E_PORTSC;                              /*!< Offset 0x054 EHCI Port Status/Control Register */
	uint32_t reserved2 [0x00EA];
	__IO uint32_t O_HcRevision;                          /*!< Offset 0x400 OHCI Revision Register (not documented) */
	__IO uint32_t O_HcControl;                           /*!< Offset 0x404 OHCI Control Register */
	__IO uint32_t O_HcCommandStatus;                     /*!< Offset 0x408 OHCI Command Status Register */
	__IO uint32_t O_HcInterruptStatus;                   /*!< Offset 0x40C OHCI Interrupt Status Register */
	__IO uint32_t O_HcInterruptEnable;                   /*!< Offset 0x410 OHCI Interrupt Enable Register */
	__IO uint32_t O_HcInterruptDisable;                  /*!< Offset 0x414 OHCI Interrupt Disable Register */
	__IO uint32_t O_HcHCCA;                              /*!< Offset 0x418 OHCI HCCA Base */
	__IO uint32_t O_HcPeriodCurrentED;                   /*!< Offset 0x41C OHCI Period Current ED Base */
	__IO uint32_t O_HcControlHeadED;                     /*!< Offset 0x420 OHCI Control Head ED Base */
	__IO uint32_t O_HcControlCurrentED;                  /*!< Offset 0x424 OHCI Control Current ED Base */
	__IO uint32_t O_HcBulkHeadED;                        /*!< Offset 0x428 OHCI Bulk Head ED Base */
	__IO uint32_t O_HcBulkCurrentED;                     /*!< Offset 0x42C OHCI Bulk Current ED Base */
	__IO uint32_t O_HcDoneHead;                          /*!< Offset 0x430 OHCI Done Head Base */
	__IO uint32_t O_HcFmInterval;                        /*!< Offset 0x434 OHCI Frame Interval Register */
	__IO uint32_t O_HcFmRemaining;                       /*!< Offset 0x438 OHCI Frame Remaining Register */
	__IO uint32_t O_HcFmNumber;                          /*!< Offset 0x43C OHCI Frame Number Register */
	__IO uint32_t O_HcPerioddicStart;                    /*!< Offset 0x440 OHCI Periodic Start Register */
	__IO uint32_t O_HcLSThreshold;                       /*!< Offset 0x444 OHCI LS Threshold Register */
	__IO uint32_t O_HcRhDescriptorA;                     /*!< Offset 0x448 OHCI Root Hub Descriptor Register A */
	__IO uint32_t O_HcRhDesriptorB;                      /*!< Offset 0x44C OHCI Root Hub Descriptor Register B */
	__IO uint32_t O_HcRhStatus;                          /*!< Offset 0x450 OHCI Root Hub Status Register */
	__IO uint32_t O_HcRhPortStatus;                      /*!< Offset 0x454 OHCI Root Hub Port Status Register */
	uint32_t reserved3 [0x00EA];
	__IO uint32_t USB_CTRL;                              /*!< Offset 0x800 HCI Interface Register (HCI_Interface) */
	uint32_t reserved4;
	__IO uint32_t HCI_CTRL3;                             /*!< Offset 0x808 HCI Control 3 Register */
	uint32_t reserved5;
	__IO uint32_t PHY_CTRL;                              /*!< Offset 0x810 PHY Control Register (PHY_Control) */
	uint32_t reserved6 [0x0004];
	__IO uint32_t PHY_STATUS;                            /*!< Offset 0x824 PHY Status Register */
	__IO uint32_t USB_SPDCR;                             /*!< Offset 0x828 HCI SIE Port Disable Control Register */
} USB1_TypeDef;
/*
 * @brief DRD
 */
/*!< DRD Controller Interface */
typedef struct DRD_Type
{
	__IO uint32_t USB_REVISION_REG;                      /*!< Offset 0x000 usb_revision_reg	0x00 */
	__IO uint32_t USB_CTRL_REG;                          /*!< Offset 0x004 usb_ctrl_reg	0x04 */
	__IO uint32_t USB_STAT_REG;                          /*!< Offset 0x008 usb_stat_reg	0x08 */
	__IO uint32_t USB_EMULATION_REG;                     /*!< Offset 0x00C usb_emulation_reg 0x0c */
	uint32_t reserved1 [0x0002];
	__IO uint32_t USB_SRP_FIX_TIME_REG;                  /*!< Offset 0x018 usb_srp_fix_time_reg 0x18 */
	uint32_t reserved2;
	__IO uint32_t USB_INTR_SRC_REG;                      /*!< Offset 0x020 usb_intr_src_reg	0x20 */
	__IO uint32_t USB_INTR_SRC_SET_REG;                  /*!< Offset 0x024 usb_intr_src_set_reg 0x24 */
	__IO uint32_t USB_INTR_SRC_CLEAR_REG;                /*!< Offset 0x028 usb_intr_src_clear_reg 0x28 */
	__IO uint32_t USB_INTR_MASK_REG;                     /*!< Offset 0x02C usb_intr_mask_reg 0x2c */
	__IO uint32_t USB_INTR_MASK_SET_REG;                 /*!< Offset 0x030 usb_intr_mask_set_reg 0x30 */
	__IO uint32_t USB_INTR_MASK_CLEAR_REG;               /*!< Offset 0x034 usb_intr_mask_clear_reg 0x34 */
	__IO uint32_t USB_INTR_SRC_MASKED_REG;               /*!< Offset 0x038 usb_intr_src_masked_reg 0x38 */
	__IO uint32_t USB_END_OF_INTR_REG;                   /*!< Offset 0x03C usb_end_of_intr_reg 0x3c */
} DRD_TypeDef;
/*
 * @brief CSI
 */
/*!< CSI Controller Interface */
typedef struct CSI_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} CSI_TypeDef;
/*
 * @brief TVD
 */
/*!< TVD Controller Interface */
typedef struct TVD_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} TVD_TypeDef;
/*
 * @brief DE
 */
/*!< DE Controller Interface */
typedef struct DE_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} DE_TypeDef;
/*
 * @brief DI
 */
/*!< DI Controller Interface */
typedef struct DI_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} DI_TypeDef;
/*
 * @brief G2D
 */
/*!< G2D Controller Interface */
typedef struct G2D_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} G2D_TypeDef;
/*
 * @brief DSI
 */
/*!< DSI Controller Interface */
typedef struct DSI_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} DSI_TypeDef;
/*
 * @brief DISPLAY_TOP
 */
/*!< DISPLAY_TOP Controller Interface */
typedef struct DISPLAY_TOP_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} DISPLAY_TOP_TypeDef;
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Controller Interface */
typedef struct TCON_LCD_Type
{
	__IO uint32_t LCD_GCTL_REG;                          /*!< Offset 0x000 LCD Global Control Register */
	__IO uint32_t LCD_GINT0_REG;                         /*!< Offset 0x004 LCD Global Interrupt Register0 */
	__IO uint32_t LCD_GINT1_REG;                         /*!< Offset 0x008 LCD Global Interrupt Register1 */
	uint32_t reserved1;
	__IO uint32_t LCD_FRM_CTL_REG;                       /*!< Offset 0x010 LCD FRM Control Register */
	__IO uint32_t LCD_FRM_SEED_REG [0x006];              /*!< Offset 0x014 LCD FRM Seed Register (N=0,1,2,3,4,5) */
	__IO uint32_t LCD_FRM_TAB_REG [0x004];               /*!< Offset 0x02C LCD FRM Table Register (N=0,1,2,3) */
	__IO uint32_t LCD_3D_FIFO_REG;                       /*!< Offset 0x03C LCD 3D FIFO Register */
	__IO uint32_t LCD_CTL_REG;                           /*!< Offset 0x040 LCD Control Register */
	__IO uint32_t LCD_DCLK_REG;                          /*!< Offset 0x044 LCD Data Clock Register */
	__IO uint32_t LCD_BASIC0_REG;                        /*!< Offset 0x048 LCD Basic Timing Register0 */
	__IO uint32_t LCD_BASIC1_REG;                        /*!< Offset 0x04C LCD Basic Timing Register1 */
	__IO uint32_t LCD_BASIC2_REG;                        /*!< Offset 0x050 LCD Basic Timing Register2 */
	__IO uint32_t LCD_BASIC3_REG;                        /*!< Offset 0x054 LCD Basic Timing Register3 */
	__IO uint32_t LCD_HV_IF_REG;                         /*!< Offset 0x058 LCD HV Panel Interface Register */
	uint32_t reserved2;
	__IO uint32_t LCD_CPU_IF_REG;                        /*!< Offset 0x060 LCD CPU Panel Interface Register */
	__IO uint32_t LCD_CPU_WR_REG;                        /*!< Offset 0x064 LCD CPU Panel Write Data Register */
	__IO uint32_t LCD_CPU_RD0_REG;                       /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
	__IO uint32_t LCD_CPU_RD1_REG;                       /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
	uint32_t reserved3 [0x0005];
	__IO uint32_t LCD_LVDS_IF_REG;                       /*!< Offset 0x084 LCD LVDS Configure Register */
	__IO uint32_t LCD_IO_POL_REG;                        /*!< Offset 0x088 LCD IO Polarity Register */
	__IO uint32_t LCD_IO_TRI_REG;                        /*!< Offset 0x08C LCD IO Control Register */
	uint32_t reserved4 [0x001B];
	__IO uint32_t LCD_DEBUG_REG;                         /*!< Offset 0x0FC LCD Debug Register */
	__IO uint32_t LCD_CEU_CTL_REG;                       /*!< Offset 0x100 LCD CEU Control Register */
	uint32_t reserved5 [0x0003];
	__IO uint32_t LCD_CEU_COEF_MUL_REG [0x003];          /*!< Offset 0x110 LCD CEU Coefficient Register0 0x0110+N*0x04 (N=0..10) */
	__IO uint32_t LCD_CEU_COEF_ADD_REG [0x003];          /*!< Offset 0x11C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
	uint32_t reserved6 [0x0006];
	__IO uint32_t LCD_CEU_COEF_RANG_REG [0x003];         /*!< Offset 0x140 LCD CEU Coefficient Register2 0x0140+N*0x04 (N=0,1,2) */
	uint32_t reserved7 [0x0005];
	__IO uint32_t LCD_CPU_TRI0_REG;                      /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
	__IO uint32_t LCD_CPU_TRI1_REG;                      /*!< Offset 0x164 LCD CPU Panel Trigger Register1 */
	__IO uint32_t LCD_CPU_TRI2_REG;                      /*!< Offset 0x168 LCD CPU Panel Trigger Register2 */
	__IO uint32_t LCD_CPU_TRI3_REG;                      /*!< Offset 0x16C LCD CPU Panel Trigger Register3 */
	__IO uint32_t LCD_CPU_TRI4_REG;                      /*!< Offset 0x170 LCD CPU Panel Trigger Register4 */
	__IO uint32_t LCD_CPU_TRI5_REG;                      /*!< Offset 0x174 LCD CPU Panel Trigger Register5 */
	uint32_t reserved8 [0x0002];
	__IO uint32_t LCD_CMAP_CTL_REG;                      /*!< Offset 0x180 LCD Color Map Control Register */
	uint32_t reserved9 [0x0003];
	__IO uint32_t LCD_CMAP_ODD0_REG;                     /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
	__IO uint32_t LCD_CMAP_ODD1_REG;                     /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
	__IO uint32_t LCD_CMAP_EVEN0_REG;                    /*!< Offset 0x198 LCD Color Map Even Line Register0 */
	__IO uint32_t LCD_CMAP_EVEN1_REG;                    /*!< Offset 0x19C LCD Color Map Even Line Register1 */
	uint32_t reserved10 [0x0014];
	__IO uint32_t LCD_SAFE_PERIOD_REG;                   /*!< Offset 0x1F0 LCD Safe Period Register */
	uint32_t reserved11 [0x000B];
	__IO uint32_t LCD_LVDS0_ANA_REG;                     /*!< Offset 0x220 LCD LVDS Analog Register 0 */
	__IO uint32_t LCD_LVDS1_ANA_REG;                     /*!< Offset 0x224 LCD LVDS Analog Register 1 */
	uint32_t reserved12 [0x0002];
	__IO uint32_t LCD_SYNC_CTL_REG;                      /*!< Offset 0x230 LCD Sync Control Register */
	__IO uint32_t LCD_SYNC_POS_REG;                      /*!< Offset 0x234 LCD Sync Position Register */
	__IO uint32_t LCD_SLAVE_STOP_POS_REG;                /*!< Offset 0x238 LCD Slave Stop Position Register */
	uint32_t reserved13 [0x0071];
	__IO uint32_t LCD_GAMMA_TABLE_REG [0x100];           /*!< Offset 0x400 LCD Gamma Table Register */
} TCON_LCD_TypeDef;
/*
 * @brief TVE
 */
/*!< TVE Controller Interface */
typedef struct TVE_Type
{
	__IO uint32_t DUMMY;                                 /*!< Offset 0x000 Dummy field definition */
} TVE_TypeDef;
/*
 * @brief C0_CPUX_CFG
 */
/*!< C0_CPUX_CFG Controller Interface */
typedef struct C0_CPUX_CFG_Type
{
	__IO uint32_t C0_RST_CTRL;                           /*!< Offset 0x000 Cluster 0 Reset Control Register */
	uint32_t reserved1 [0x0003];
	__IO uint32_t C0_CTRL_REG0;                          /*!< Offset 0x010 Cluster 0 Control Register0 */
	__IO uint32_t C0_CTRL_REG1;                          /*!< Offset 0x014 Cluster 0 Control Register1 */
	__IO uint32_t C0_CTRL_REG2;                          /*!< Offset 0x018 Cluster 0 Control Register2 */
	uint32_t reserved2 [0x0002];
	__IO uint32_t CACHE_CFG_REG;                         /*!< Offset 0x024 Cache Configuration Register */
	uint32_t reserved3 [0x0016];
	__IO uint32_t C0_CPU_STATUS;                         /*!< Offset 0x080 Cluster 0 CPU Status Register */
	__IO uint32_t L2_STATUS_REG;                         /*!< Offset 0x084 Cluster 0 L2 Status Register */
	uint32_t reserved4 [0x000E];
	__IO uint32_t DBG_REG0;                              /*!< Offset 0x0C0 Cluster 0 Debug Control Register0 */
	__IO uint32_t DBG_REG1;                              /*!< Offset 0x0C4 Cluster 0 Debug Control Register1 */
	uint32_t reserved5 [0x0002];
	__IO uint32_t AXI_MNT_CTRL_REG;                      /*!< Offset 0x0D0 AXI Monitor Control Register */
	__IO uint32_t AXI_MNT_PRD_REG;                       /*!< Offset 0x0D4 AXI Monitor Period Register */
	__IO uint32_t AXI_MNT_RLTCY_REG;                     /*!< Offset 0x0D8 AXI Monitor Read Total Latency Register */
	__IO uint32_t AXI_MNT_WLTCY_REG;                     /*!< Offset 0x0DC AXI Monitor Write Total Latency Register */
	__IO uint32_t AXI_MNT_RREQ_REG;                      /*!< Offset 0x0E0 AXI Monitor Read Request Times Register */
	__IO uint32_t AXI_MNT_WREQ_REG;                      /*!< Offset 0x0E4 AXI Monitor Write Request Times Register */
	__IO uint32_t AXI_MNT_RBD_REG;                       /*!< Offset 0x0E8 AXI Monitor Read Bandwidth Register */
	__IO uint32_t AXI_MNT_WBD_REG;                       /*!< Offset 0x0EC AXI Monitor Write Bandwidth Register */
} C0_CPUX_CFG_TypeDef;
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability Controller Interface */
typedef struct USB_EHCI_CapabilityType
{
	__IO uint32_t HCCAPBASE;                             /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
	__IO uint32_t HCSPARAMS;                             /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
	__IO uint32_t HCCPARAMS;                             /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
	__IO uint32_t HCSPPORTROUTE;                         /*!< Offset 0x00C EHCI Companion Port Route Description */
	__IO uint32_t USBCMD;                                /*!< Offset 0x010 EHCI USB Command Register */
	__IO uint32_t USBSTS;                                /*!< Offset 0x014 EHCI USB Status Register */
	__IO uint32_t USBINTR;                               /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
	__IO uint32_t FRINDEX;                               /*!< Offset 0x01C EHCI USB Frame Index Register */
	__IO uint32_t CTRLDSSEGMENT;                         /*!< Offset 0x020 EHCI 4G Segment Selector Register */
	__IO uint32_t PERIODICLISTBASE;                      /*!< Offset 0x024 EHCI Frame List Base Address Register */
	__IO uint32_t ASYNCLISTADDR;                         /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
} USB_EHCI_CapabilityTypeDef;
/* Generated section end */
/*
 * @brief DMAC_CH
 */
/*!< DMAC_CH Controller Interface */
typedef struct DMAC_CH_Type
{
	__IO uint32_t DMAC_EN_REGN;                          /*!< Offset 0x000 DMAC Channel Enable Register N (N = 0 to 15) 0x0100 + N*0x0040 */
	__IO uint32_t DMAC_PAU_REGN;                         /*!< Offset 0x004 DMAC Channel Pause Register N (N = 0 to 15) 0x0104 + N*0x0040 */
	__IO uint32_t DMAC_DESC_ADDR_REGN;                   /*!< Offset 0x008 DMAC Channel Start Address Register N (N = 0 to 15) 0x0108 + N*0x0040 */
	__IO uint32_t DMAC_CFG_REGN;                         /*!< Offset 0x00C DMAC Channel Configuration Register N (N = 0 to 15) 0x010C + N*0x0040 */
	__IO uint32_t DMAC_CUR_SRC_REGN;                     /*!< Offset 0x010 DMAC Channel Current Source Register N (N = 0 to 15) 0x0110 + N*0x0040 */
	__IO uint32_t DMAC_CUR_DEST_REGN;                    /*!< Offset 0x014 DMAC Channel Current Destination Register N (N = 0 to 15) 0x0114 + N*0x0040 */
	__IO uint32_t DMAC_BCNT_LEFT_REGN;                   /*!< Offset 0x018 DMAC Channel Byte Counter Left Register N (N = 0 to 15) 0x0118 + N*0x0040 */
	__IO uint32_t DMAC_PARA_REGN;                        /*!< Offset 0x01C DMAC Channel Parameter Register N (N = 0 to 15) 0x011C + N*0x0040 */
	uint32_t reserved1 [0x0002];
	__IO uint32_t DMAC_MODE_REGN;                        /*!< Offset 0x028 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
	__IO uint32_t DMAC_FDESC_ADDR_REGN;                  /*!< Offset 0x02C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
	__IO uint32_t DMAC_PKG_NUM_REGN;                     /*!< Offset 0x030 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
	uint32_t reserved2 [0x0003];
} DMAC_CH_TypeDef;
/*
 * @brief DMAC
 */
/*!< DMAC Controller Interface */
typedef struct DMAC_Type
{
	__IO uint32_t DMAC_IRQ_EN_REG0;                      /*!< Offset 0x000 DMAC IRQ Enable Register 0 */
	__IO uint32_t DMAC_IRQ_EN_REG1;                      /*!< Offset 0x004 DMAC IRQ Enable Register 1 */
	uint32_t reserved1 [0x0002];
	__IO uint32_t DMAC_IRQ_PEND_REG0;                    /*!< Offset 0x010 DMAC IRQ Pending Register 0 */
	__IO uint32_t DMAC_IRQ_PEND_REG1;                    /*!< Offset 0x014 DMAC IRQ Pending Register 1 */
	uint32_t reserved2 [0x0004];
	__IO uint32_t DMAC_AUTO_GATE_REG;                    /*!< Offset 0x028 DMAC Auto Gating Register */
	uint32_t reserved3;
	__IO uint32_t DMAC_STA_REG;                          /*!< Offset 0x030 DMAC Status Register */
	uint32_t reserved4 [0x0033];
	__IO DMAC_CH_TypeDef CH [16];                        /*!< Offset 0x100 regisrers */
} DMAC_TypeDef;

/* Access pointers */

#define UART0      ((UART_TypeDef *) UART0_BASE)		/*!< \brief UART0 Interface register set access pointer */
#define UART1      ((UART_TypeDef *) UART1_BASE)		/*!< \brief UART1 Interface register set access pointer */
#define UART2      ((UART_TypeDef *) UART2_BASE)		/*!< \brief UART2 Interface register set access pointer */
#define UART3      ((UART_TypeDef *) UART3_BASE)		/*!< \brief UART3 Interface register set access pointer */
#define UART4      ((UART_TypeDef *) UART4_BASE)		/*!< \brief UART4 Interface register set access pointer */
#define UART5      ((UART_TypeDef *) UART5_BASE)		/*!< \brief UART5 Interface register set access pointer */
#define GPIOB      ((GPIO_TypeDef *) GPIOB_BASE)		/*!< \brief GPIOB Interface register set access pointer */
#define GPIOC      ((GPIO_TypeDef *) GPIOC_BASE)		/*!< \brief GPIOC Interface register set access pointer */
#define GPIOD      ((GPIO_TypeDef *) GPIOD_BASE)		/*!< \brief GPIOD Interface register set access pointer */
#define GPIOE      ((GPIO_TypeDef *) GPIOE_BASE)		/*!< \brief GPIOE Interface register set access pointer */
#define GPIOF      ((GPIO_TypeDef *) GPIOF_BASE)		/*!< \brief GPIOF Interface register set access pointer */
#define GPIOG      ((GPIO_TypeDef *) GPIOG_BASE)		/*!< \brief GPIOG Interface register set access pointer */
#define SYS_CFG 	((SYS_CFG_TypeDef *) SYS_CFG_BASE)	/*!< \brief SYS_CFG Interface register set access pointer */
#define SMHC0      	((SMHC_TypeDef *) SMHC0_BASE)		/*!< \brief SMHC0 Interface register set access pointer */
#define SMHC1      	((SMHC_TypeDef *) SMHC1_BASE)		/*!< \brief SMHC1 Interface register set access pointer */
#define SMHC2      	((SMHC_TypeDef *) SMHC2_BASE)		/*!< \brief SMHC2 Interface register set access pointer */
#define I2S1      	((I2S_PCM_TypeDef *) I2S1_BASE)		/*!< \brief I2S1 Interface register set access pointer */
#define I2S2      	((I2S_PCM_TypeDef *) I2S2_BASE)		/*!< \brief I2S2 Interface register set access pointer */
#define DMIC      	((DMIC_TypeDef *) DMIC_BASE)		/*!< \brief DMIC Interface register set access pointer */
#define OWA			((OWA_TypeDef *) OWA_BASE)			/*!< \brief OWA Interface register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)		/*!< \brief AUDIO_CODEC Interface register set access pointer */
#define TWI0		((TWI_TypeDef *) TWI0_BASE)			/*!< \brief TWI0 Interface register set access pointer */
#define TWI1		((TWI_TypeDef *) TWI1_BASE)			/*!< \brief TWI1 Interface register set access pointer */
#define TWI2		((TWI_TypeDef *) TWI2_BASE)			/*!< \brief TWI2 Interface register set access pointer */
#define TWI3		((TWI_TypeDef *) TWI3_BASE)			/*!< \brief TWI3 Interface register set access pointer */
#define SPI0		((SPI_TypeDef *) SPI0_BASE)			/*!< \brief SPI0 Interface register set access pointer */
#define SPI_DBI		((SPI_DBI_TypeDef *) SPI_DBI_BASE)	/*!< \brief SPI_DBI Interface register set access pointer */
#define LEDC		((LEDC_TypeDef *) LEDC_BASE)		/*!< \brief LEDC Interface register set access pointer */
#define GPADC		((GPADC_TypeDef *) GPADC_BASE)		/*!< \brief GPADC Interface register set access pointer */
#define TPADC		((TPADC_TypeDef *) TPADC_BASE)		/*!< \brief TPADC Interface register set access pointer */
#define CIR_RX		((CIR_RX_TypeDef *) CIR_RX_BASE)	/*!< \brief CIR_RX Interface register set access pointer */
#define CIR_TX		((CIR_TX_TypeDef *) CIR_TX_BASE)	/*!< \brief CIR_TX Interface register set access pointer */
#define CE_S		((CE_S_TypeDef *) CE_S_BASE)		/*!< \brief CE_S Interface register set access pointer */
#define HSTIMER		((HSTIMER_TypeDef *) HSTIMER_BASE)	/*!< \brief HSTIMER Interface register set access pointer */
#define TIMER		((TIMER_TypeDef *) TIMER_BASE)		/*!< \brief TIMER Interface register set access pointer */
#define THS			((THS_TypeDef *) THS_BASE)			/*!< \brief THS Interface register set access pointer */
#define IOMMU		((IOMMU_TypeDef *) IOMMU_BASE)		/*!< \brief IOMMU Interface register set access pointer */
#define CCU			((CCU_TypeDef *) CCU_BASE)			/*!< \brief CCU Interface register set access pointer */

#define DE 			((DE_TypeDef *) DE_BASE)					/*!< \brief DE Interface register set access pointer */
#define DI 			((DI_TypeDef *) DI_BASE)					/*!< \brief DI Interface register set access pointer */
#define G2D 		((G2D_TypeDef *) G2D_BASE)					/*!< \brief G2D Interface register set access pointer */
#define DSI 		((DSI_TypeDef *) DSI_BASE)					/*!< \brief DSI Interface register set access pointer */
#define DISPLAY_TOP ((DISPLAY_TOP_TypeDef *) DISPLAY_TOP_BASE)	/*!< \brief DISPLAY_TOP Interface register set access pointer */
#define TCON_LCD0 	((TCON_LCD_TypeDef *) TCON_LCD0_BASE)		/*!< \brief TCON_LCD0 Interface register set access pointer */
#define TCON_TV0 	((TCON_TV_TypeDef *) TCON_TV0_BASE)			/*!< \brief TCON_TV0 Interface register set access pointer */
#define TVE_TOP 	((TVE_TypeDef *) TVE_TOP_BASE)				/*!< \brief TVE_TOP Interface register set access pointer */
#define TV_Encoder 	((TVE_TypeDef *) TV_Encoder_BASE)			/*!< \brief TV_Encoder Interface register set access pointer */

#define CSI			((CSI_TypeDef *) CSI_BASE)					/*!< \brief CSI Interface register set access pointer */
#define TVD_TOP		((TVD_TypeDef *) TVD_TOP_BASE)				/*!< \brief TVD_TOP Interface register set access pointer */
#define TVD0		((TV_TypeDef *) TVD0_BASE)					/*!< \brief TVD0 Interface register set access pointer */

#define RTC			((RTC_TypeDef *) RTC_BASE)					/*!< \brief RTC Interface register set access pointer */

#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)			/*!< \brief CPU_SUBSYS_CTRL Interface register set access pointer */
//#define TimeStamp_STA	((TimeStamp_STA_TypeDef *) TimeStamp_STA_BASE)
//#define TimeStamp_CTRL	((TimeStamp_CTRL_TypeDef *) TimeStamp_CTRL_BASE)
//#define IDC				(IDC_TypeDef *) IDC_BASE)
#define C0_CPUX_CFG		((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)			/*!< \brief C0_CPUX_CFG Interface register set access pointer */
//#define C0_CPUX_MBIST	((C0_CPUX_MBIST_TypeDef *) C0_CPUX_MBIST_BASE)			/*!< \brief C0_CPUX_MBIST Interface register set access pointer */

#define CAN0			((CAN_TypeDef *) CAN0_BASE)						/*!< \brief CAN0 Interface register set access pointer */
#define CAN1			((CAN_TypeDef *) CAN1_BASE)						/*!< \brief CAN1 Interface register set access pointer */

#define USB0			((USB1_TypeDef *) USB0_BASE)					/*!< \brief USB0 Interface register set access pointer */
#define USB1			((USB1_TypeDef *) USB1_BASE)					/*!< \brief USB1 Interface register set access pointer */

#define USB0_DEVICE		((DRD_TypeDef *) USB0_BASE)					/*!< \brief USB0_OTG Interface register set access pointer */

#define USB0_EHCI		((USB_EHCI_CapabilityTypeDef * ) (USB0_BASE + 0x1000))	/*!< \brief USB0_EHCI Interface register set access pointer */
#define USB1_EHCI		((USB_EHCI_CapabilityTypeDef * ) USB1_BASE)				/*!< \brief USB1_EHCI Interface register set access pointer */
#define DMAC			((DMAC_TypeDef *) DMAC_BASE)							/*!< \brief DMAC Interface register set access pointer */


enum DMAC_SrcDrqType
{
	DMAC_SrcDrqSRAM = 0,
	DMAC_SrcDrqDRAM = 1,
	DMAC_SrcDrqOWA_RX = 2,
	DMAC_SrcDrqI2S1_RX = 4,
	DMAC_SrcDrqI2S2_RX = 5,
	DMAC_SrcDrqAudioCodec = 7,
	DMAC_SrcDrqDMIC = 8,
	DMAC_SrcDrqGPADC = 12,
	DMAC_SrcDrqTPADC = 13,
	DMAC_SrcDrqUART0_RX = 14,
	DMAC_SrcDrqUART1_RX = 15,
	DMAC_SrcDrqUART2_RX = 16,
	DMAC_SrcDrqUART3_RX = 17,
	DMAC_SrcDrqUART4_RX = 18,
	DMAC_SrcDrqUART5_RX = 19,
	DMAC_SrcDrqSPI0_RX = 22,
	DMAC_SrcDrqSPI1_RX = 23,
	DMAC_SrcDrqUSB0_EP1 = 30,
	DMAC_SrcDrqUSB0_EP2 = 31,
	DMAC_SrcDrqUSB0_EP3 = 32,
	DMAC_SrcDrqUSB0_EP4 = 33,
	DMAC_SrcDrqUSB0_EP5 = 34,
	DMAC_SrcDrqTWI0_RX = 43,
	DMAC_SrcDrqTWI1_RX = 44,
	DMAC_SrcDrqTWI2_RX = 45,
	DMAC_SrcDrqTWI3_RX = 46,
};

enum DMAC_DstDrqType
{
	DMAC_DstDrqSRAM = 0,
	DMAC_DstDrqDRAM = 1,
	DMAC_DstDrqOWA_TX = 2,
	DMAC_DstDrqI2S1_TX = 4,
	DMAC_DstDrqI2S2_TX = 5,
	DMAC_DstDrqAudioCodec = 7,
	DMAC_DstDrqIR_TX = 13,
	DMAC_DstDrqUART0_TX = 14,
	DMAC_DstDrqUART1_TX = 15,
	DMAC_DstDrqUART2_TX = 16,
	DMAC_DstDrqUART3_TX = 17,
	DMAC_DstDrqUART4_TX = 18,
	DMAC_DstDrqUART5_TX = 19,
	DMAC_DstDrqSPI0_TX = 22,
	DMAC_DstDrqSPI1_TX = 23,
	DMAC_DstDrqUSB0_EP1 = 30,
	DMAC_DstDrqUSB0_EP2 = 31,
	DMAC_DstDrqUSB0_EP3 = 32,
	DMAC_DstDrqUSB0_EP4 = 33,
	DMAC_DstDrqUSB0_EP5 = 34,
	DMAC_DstDrqLEDC = 42,
	DMAC_DstDrqTWI0_TX = 43,
	DMAC_DstDrqTWI1_TX = 44,
	DMAC_DstDrqTWI2_TX = 45,
	DMAC_DstDrqTWI3_TX = 46,
};

/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#if defined (USE_HAL_DRIVER)
 #include "allwnr_t113s3_hal.h"
#endif /* USE_HAL_DRIVER */

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
