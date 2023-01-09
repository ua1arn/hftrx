/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
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
	CIR_TX_IRQn = (51), /*  Peripherial */
	LEDC_IRQn = (52), /*  Peripherial */
	CAN0_IRQ = (53), /*  Peripherial */
	CAN1_IRQ = (54), /*  Peripherial */
	OWA_IRQn = (55), /*  Peripherial */
	DMIC_IRQn = (56), /*  Peripherial */
	AUDIO_CODEC_IRQn = (57), /*  Peripherial */
	I2S_PCM1_IRQn = (59), /*  Peripherial */
	I2S_PCM2_IRQn = (60), /*  Peripherial */
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
	TIMER0_IRQn = (91), /*  TIMER->TMR0_CTRL_REG */
	TIMER1_IRQn = (92), /*  TIMER->TMR1_CTRL_REG */
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
	TIMER_und_0_IRQn = (172), /*  Peripherial */
	TIMER_und_1_IRQn = (173), /*  Peripherial */
	TIMER_und_2_IRQn = (174), /*  Peripherial */
	TIMER_und_3_IRQn = (175), /*  Peripherial */
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
#define AUDIO_CODEC_BASE	0x02030000
#define DMIC_BASE			0x02031000
#define I2S1_BASE			0x02033000
#define I2S2_BASE			0x02034000
#define OWA_BASE			0x02036000
#define TIMER_BASE			0x02050000

// SP1 (SYS domain)
#define UART0_BASE      	0x02500000
#define UART1_BASE      	0x02500400
#define UART2_BASE     		0x02500800
#define UART3_BASE			0x02500C00
#define UART4_BASE      	0x02501000
#define UART5_BASE      	0x02501400

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
#define	SID_BASE  		   	0x03006000	/* 4 KB Security ID (SID) */
#define SMC_BASE     		0x03007000
#define HSTIMER_BASE     	0x03008000
#define DCU_BASE      		0x03010000	/* 64 KB */
#define GIC_BASE      		0x03020000
#define CE_NS_BASE      	0x03040000
#define CE_S_BASE   	   	0x03040800
#define CE_KEY_SRAM_BASE    0x03041000	/* 4 KB (only CE access) */
#define MSI_MEMC_BASE	    0x03102000	/* 2 MB MSI and MEMC base address */
#define DDRPHYC_BASE	 	0x03103000
#define MC_MEMC_BASE		0x03202000	/* MC */

// SH2 (SYS domain)
#define SMHC0_BASE      	0x04020000
#define SMHC1_BASE      	0x04021000
#define SMHC2_BASE      	0x04022000
#define SPI0_BASE    	  	0x04025000
#define SPI_DBI_BASE  		0x04026000
#define USBOTG0_BASE 	    0x04100000
#define USBEHCI1_BASE 	    0x04200000
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

// Undocumented registers
//	The Hotplug Flag Register is 0x070005C0
//	The Soft Entry Address Register of CPU0 is 0x070005C4
//	The Soft Entry Address Register of CPU1 is 0x070005C8
//	The status of the FEL pin is the bit[8] of the system configuration module (register: 0x03000024).
//	Fast Boot register (0x07090120) in RTC module

#define CPU_0700_0000_BASE	0x07000000
#define CPU_0701_0000_BASE	0x07010000

// VIDEO_IN_SYS related
#define CSI_BASE 			0x05800000

#define CSIC_CCU_BASE		(CSI_BASE + 0x0000)
#define CSIC_TOP_BASE		(CSI_BASE + 0x0800)
#define CSIC_PARSER0_BASE	(CSI_BASE + 0x1000)
#define SIC_DMA0_BASE		(CSI_BASE + 0x9000)
#define SIC_DMA1_BASE		(CSI_BASE + 0x9200)

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
#define DRAM_SPACE_SIZE			0x08000000			/* 128 MB */

#define G2D_TOP_BASE        (0x00000 + G2D_BASE)
#define G2D_MIXER_BASE      (0x00100 + G2D_BASE)
#define G2D_BLD_BASE        (0x00400 + G2D_BASE)
#define G2D_V0_BASE         (0x00800 + G2D_BASE)
#define G2D_UI0_BASE        (0x01000 + G2D_BASE)
#define G2D_UI1_BASE        (0x01800 + G2D_BASE)
#define G2D_UI2_BASE        (0x02000 + G2D_BASE)
#define G2D_WB_BASE         (0x03000 + G2D_BASE)
#define G2D_VSU_BASE        (0x08000 + G2D_BASE)
#define G2D_ROT_BASE        (0x28000 + G2D_BASE)
#define G2D_GSU_BASE        (0x30000 + G2D_BASE)

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
	uint32_t reserved_0x004 [0x0002];
	__IO uint32_t GIC_JTAG_RST_CTRL;                     /*!< Offset 0x00C GIC and JTAG Reset Control Register */
	__IO uint32_t C0_INT_EN;                             /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
	__IO uint32_t RQ_FIQ_STATUS;                         /*!< Offset 0x014 IRQ/FIQ Status Register */
	__IO uint32_t GENER_CTRL_REG2;                       /*!< Offset 0x018 General Control Register2 */
	__IO uint32_t DBG_STATE;                             /*!< Offset 0x01C Debug State Register */
} CPU_SUBSYS_CTRL_TypeDef; /* size of structure = 0x020 */
/*
 * @brief CCU
 */
/*!< CCU Controller Interface */
typedef struct CCU_Type
{
	__IO uint32_t PLL_CPU_CTRL_REG;                      /*!< Offset 0x000 PLL_CPU Control Register */
	uint32_t reserved_0x004 [0x0003];
	__IO uint32_t PLL_DDR_CTRL_REG;                      /*!< Offset 0x010 PLL_DDR Control Register */
	uint32_t reserved_0x014 [0x0003];
	__IO uint32_t PLL_PERI_CTRL_REG;                     /*!< Offset 0x020 PLL_PERI Control Register */
	uint32_t reserved_0x024 [0x0007];
	__IO uint32_t PLL_VIDEO0_CTRL_REG;                   /*!< Offset 0x040 PLL_VIDEO0 Control Register */
	uint32_t reserved_0x044;
	__IO uint32_t PLL_VIDEO1_CTRL_REG;                   /*!< Offset 0x048 PLL_VIDEO1 Control Register */
	uint32_t reserved_0x04C [0x0003];
	__IO uint32_t PLL_VE_CTRL_REG;                       /*!< Offset 0x058 PLL_VE Control Register */
	uint32_t reserved_0x05C [0x0007];
	__IO uint32_t PLL_AUDIO0_CTRL_REG;                   /*!< Offset 0x078 PLL_AUDIO0 Control Register */
	uint32_t reserved_0x07C;
	__IO uint32_t PLL_AUDIO1_CTRL_REG;                   /*!< Offset 0x080 PLL_AUDIO1 Control Register */
	uint32_t reserved_0x084 [0x0023];
	__IO uint32_t PLL_DDR_PAT0_CTRL_REG;                 /*!< Offset 0x110 PLL_DDR Pattern0 Control Register */
	__IO uint32_t PLL_DDR_PAT1_CTRL_REG;                 /*!< Offset 0x114 PLL_DDR Pattern1 Control Register */
	uint32_t reserved_0x118 [0x0002];
	__IO uint32_t PLL_PERI_PAT0_CTRL_REG;                /*!< Offset 0x120 PLL_PERI Pattern0 Control Register */
	__IO uint32_t PLL_PERI_PAT1_CTRL_REG;                /*!< Offset 0x124 PLL_PERI Pattern1 Control Register */
	uint32_t reserved_0x128 [0x0006];
	__IO uint32_t PLL_VIDEO0_PAT0_CTRL_REG;              /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
	__IO uint32_t PLL_VIDEO0_PAT1_CTRL_REG;              /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
	__IO uint32_t PLL_VIDEO1_PAT0_CTRL_REG;              /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
	__IO uint32_t PLL_VIDEO1_PAT1_CTRL_REG;              /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
	uint32_t reserved_0x150 [0x0002];
	__IO uint32_t PLL_VE_PAT0_CTRL_REG;                  /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
	__IO uint32_t PLL_VE_PAT1_CTRL_REG;                  /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
	uint32_t reserved_0x160 [0x0006];
	__IO uint32_t PLL_AUDIO0_PAT0_CTRL_REG;              /*!< Offset 0x178 PLL_AUDIO0 Pattern0 Control Register */
	__IO uint32_t PLL_AUDIO0_PAT1_CTRL_REG;              /*!< Offset 0x17C PLL_AUDIO0 Pattern1 Control Register */
	__IO uint32_t PLL_AUDIO1_PAT0_CTRL_REG;              /*!< Offset 0x180 PLL_AUDIO1 Pattern0 Control Register */
	__IO uint32_t PLL_AUDIO1_PAT1_CTRL_REG;              /*!< Offset 0x184 PLL_AUDIO1 Pattern1 Control Register */
	uint32_t reserved_0x188 [0x005E];
	__IO uint32_t PLL_CPU_BIAS_REG;                      /*!< Offset 0x300 PLL_CPU Bias Register */
	uint32_t reserved_0x304 [0x0003];
	__IO uint32_t PLL_DDR_BIAS_REG;                      /*!< Offset 0x310 PLL_DDR Bias Register */
	uint32_t reserved_0x314 [0x0003];
	__IO uint32_t PLL_PERI_BIAS_REG;                     /*!< Offset 0x320 PLL_PERI Bias Register */
	uint32_t reserved_0x324 [0x0007];
	__IO uint32_t PLL_VIDEO0_BIAS_REG;                   /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
	uint32_t reserved_0x344;
	__IO uint32_t PLL_VIDEO1_BIAS_REG;                   /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
	uint32_t reserved_0x34C [0x0003];
	__IO uint32_t PLL_VE_BIAS_REG;                       /*!< Offset 0x358 PLL_VE Bias Register */
	uint32_t reserved_0x35C [0x0007];
	__IO uint32_t PLL_AUDIO0_BIAS_REG;                   /*!< Offset 0x378 PLL_AUDIO0 Bias Register */
	uint32_t reserved_0x37C;
	__IO uint32_t PLL_AUDIO1_BIAS_REG;                   /*!< Offset 0x380 PLL_AUDIO1 Bias Register */
	uint32_t reserved_0x384 [0x001F];
	__IO uint32_t PLL_CPU_TUN_REG;                       /*!< Offset 0x400 PLL_CPU Tuning Register */
	uint32_t reserved_0x404 [0x003F];
	__IO uint32_t CPU_AXI_CFG_REG;                       /*!< Offset 0x500 CPU_AXI Configuration Register */
	__IO uint32_t CPU_GATING_REG;                        /*!< Offset 0x504 CPU_GATING Configuration Register */
	uint32_t reserved_0x508 [0x0002];
	__IO uint32_t PSI_CLK_REG;                           /*!< Offset 0x510 PSI Clock Register */
	uint32_t reserved_0x514 [0x0003];
	__IO uint32_t APB0_CLK_REG;                          /*!< Offset 0x520 APB0 Clock Register */
	__IO uint32_t APB1_CLK_REG;                          /*!< Offset 0x524 APB1 Clock Register */
	uint32_t reserved_0x528 [0x0006];
	__IO uint32_t MBUS_CLK_REG;                          /*!< Offset 0x540 MBUS Clock Register */
	uint32_t reserved_0x544 [0x002F];
	__IO uint32_t DE_CLK_REG;                            /*!< Offset 0x600 DE Clock Register */
	uint32_t reserved_0x604 [0x0002];
	__IO uint32_t DE_BGR_REG;                            /*!< Offset 0x60C DE Bus Gating Reset Register */
	uint32_t reserved_0x610 [0x0004];
	__IO uint32_t DI_CLK_REG;                            /*!< Offset 0x620 DI Clock Register */
	uint32_t reserved_0x624 [0x0002];
	__IO uint32_t DI_BGR_REG;                            /*!< Offset 0x62C DI Bus Gating Reset Register */
	__IO uint32_t G2D_CLK_REG;                           /*!< Offset 0x630 G2D Clock Register */
	uint32_t reserved_0x634 [0x0002];
	__IO uint32_t G2D_BGR_REG;                           /*!< Offset 0x63C G2D Bus Gating Reset Register */
	uint32_t reserved_0x640 [0x0010];
	__IO uint32_t CE_CLK_REG;                            /*!< Offset 0x680 CE Clock Register */
	uint32_t reserved_0x684 [0x0002];
	__IO uint32_t CE_BGR_REG;                            /*!< Offset 0x68C CE Bus Gating Reset Register */
	__IO uint32_t VE_CLK_REG;                            /*!< Offset 0x690 VE Clock Register */
	uint32_t reserved_0x694 [0x0002];
	__IO uint32_t VE_BGR_REG;                            /*!< Offset 0x69C VE Bus Gating Reset Register */
	uint32_t reserved_0x6A0 [0x001B];
	__IO uint32_t DMA_BGR_REG;                           /*!< Offset 0x70C DMA Bus Gating Reset Register */
	uint32_t reserved_0x710 [0x0003];
	__IO uint32_t MSGBOX_BGR_REG;                        /*!< Offset 0x71C MSGBOX Bus Gating Reset Register */
	uint32_t reserved_0x720 [0x0003];
	__IO uint32_t SPINLOCK_BGR_REG;                      /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register */
	uint32_t reserved_0x730 [0x0003];
	__IO uint32_t HSTIMER_BGR_REG;                       /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
	__IO uint32_t AVS_CLK_REG;                           /*!< Offset 0x740 AVS Clock Register */
	uint32_t reserved_0x744 [0x0012];
	__IO uint32_t DBGSYS_BGR_REG;                        /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
	uint32_t reserved_0x790 [0x0007];
	__IO uint32_t PWM_BGR_REG;                           /*!< Offset 0x7AC PWM Bus Gating Reset Register */
	uint32_t reserved_0x7B0 [0x0003];
	__IO uint32_t IOMMU_BGR_REG;                         /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
	uint32_t reserved_0x7C0 [0x0010];
	__IO uint32_t DRAM_CLK_REG;                          /*!< Offset 0x800 DRAM Clock Register */
	__IO uint32_t MBUS_MAT_CLK_GATING_REG;               /*!< Offset 0x804 MBUS Master Clock Gating Register */
	uint32_t reserved_0x808;
	__IO uint32_t DRAM_BGR_REG;                          /*!< Offset 0x80C DRAM Bus Gating Reset Register */
	uint32_t reserved_0x810 [0x0008];
	__IO uint32_t SMHC0_CLK_REG;                         /*!< Offset 0x830 SMHC0 Clock Register */
	__IO uint32_t SMHC1_CLK_REG;                         /*!< Offset 0x834 SMHC1 Clock Register */
	__IO uint32_t SMHC2_CLK_REG;                         /*!< Offset 0x838 SMHC2 Clock Register */
	uint32_t reserved_0x83C [0x0004];
	__IO uint32_t SMHC_BGR_REG;                          /*!< Offset 0x84C SMHC Bus Gating Reset Register */
	uint32_t reserved_0x850 [0x002F];
	__IO uint32_t UART_BGR_REG;                          /*!< Offset 0x90C UART Bus Gating Reset Register */
	uint32_t reserved_0x910 [0x0003];
	__IO uint32_t TWI_BGR_REG;                           /*!< Offset 0x91C TWI Bus Gating Reset Register */
	uint32_t reserved_0x920 [0x0003];
	__IO uint32_t CAN_BGR_REG;                           /*!< Offset 0x92C CAN Bus Gating Reset Register */
	uint32_t reserved_0x930 [0x0004];
	__IO uint32_t SPI0_CLK_REG;                          /*!< Offset 0x940 SPI0 Clock Register */
	__IO uint32_t SPI1_CLK_REG;                          /*!< Offset 0x944 SPI1 Clock Register */
	uint32_t reserved_0x948 [0x0009];
	__IO uint32_t SPI_BGR_REG;                           /*!< Offset 0x96C SPI Bus Gating Reset Register */
	__IO uint32_t EMAC_25M_CLK_REG;                      /*!< Offset 0x970 EMAC_25M Clock Register */
	uint32_t reserved_0x974 [0x0002];
	__IO uint32_t EMAC_BGR_REG;                          /*!< Offset 0x97C EMAC Bus Gating Reset Register */
	uint32_t reserved_0x980 [0x0010];
	__IO uint32_t IRTX_CLK_REG;                          /*!< Offset 0x9C0 IRTX Clock Register */
	uint32_t reserved_0x9C4 [0x0002];
	__IO uint32_t IRTX_BGR_REG;                          /*!< Offset 0x9CC IRTX Bus Gating Reset Register */
	uint32_t reserved_0x9D0 [0x0007];
	__IO uint32_t GPADC_BGR_REG;                         /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
	uint32_t reserved_0x9F0 [0x0003];
	__IO uint32_t THS_BGR_REG;                           /*!< Offset 0x9FC THS Bus Gating Reset Register */
	uint32_t reserved_0xA00 [0x0005];
	__IO uint32_t I2S1_CLK_REG;                          /*!< Offset 0xA14 I2S1 Clock Register */
	__IO uint32_t I2S2_CLK_REG;                          /*!< Offset 0xA18 I2S2 Clock Register */
	__IO uint32_t I2S2_ASRC_CLK_REG;                     /*!< Offset 0xA1C I2S2_ASRC Clock Register */
	__IO uint32_t I2S_BGR_REG;                           /*!< Offset 0xA20 I2S Bus Gating Reset Register */
	__IO uint32_t OWA_TX_CLK_REG;                        /*!< Offset 0xA24 OWA_TX Clock Register */
	__IO uint32_t OWA_RX_CLK_REG;                        /*!< Offset 0xA28 OWA_RX Clock Register */
	__IO uint32_t OWA_BGR_REG;                           /*!< Offset 0xA2C OWA Bus Gating Reset Register */
	uint32_t reserved_0xA30 [0x0004];
	__IO uint32_t DMIC_CLK_REG;                          /*!< Offset 0xA40 DMIC Clock Register */
	uint32_t reserved_0xA44 [0x0002];
	__IO uint32_t DMIC_BGR_REG;                          /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
	__IO uint32_t AUDIO_CODEC_DAC_CLK_REG;               /*!< Offset 0xA50 AUDIO_CODEC_DAC Clock Register */
	__IO uint32_t AUDIO_CODEC_ADC_CLK_REG;               /*!< Offset 0xA54 AUDIO_CODEC_ADC Clock Register */
	uint32_t reserved_0xA58;
	__IO uint32_t AUDIO_CODEC_BGR_REG;                   /*!< Offset 0xA5C AUDIO_CODEC Bus Gating Reset Register */
	uint32_t reserved_0xA60 [0x0004];
	__IO uint32_t USB0_CLK_REG;                          /*!< Offset 0xA70 USB0 Clock Register */
	__IO uint32_t USB1_CLK_REG;                          /*!< Offset 0xA74 USB1 Clock Register */
	uint32_t reserved_0xA78 [0x0005];
	__IO uint32_t USB_BGR_REG;                           /*!< Offset 0xA8C USB Bus Gating Reset Register */
	uint32_t reserved_0xA90 [0x000B];
	__IO uint32_t DPSS_TOP_BGR_REG;                      /*!< Offset 0xABC DPSS_TOP Bus Gating Reset Register */
	uint32_t reserved_0xAC0 [0x0019];
	__IO uint32_t DSI_CLK_REG;                           /*!< Offset 0xB24 DSI Clock Register */
	uint32_t reserved_0xB28 [0x0009];
	__IO uint32_t DSI_BGR_REG;                           /*!< Offset 0xB4C DSI Bus Gating Reset Register */
	uint32_t reserved_0xB50 [0x0004];
	__IO uint32_t TCONLCD_CLK_REG;                       /*!< Offset 0xB60 TCONLCD Clock Register */
	uint32_t reserved_0xB64 [0x0006];
	__IO uint32_t TCONLCD_BGR_REG;                       /*!< Offset 0xB7C TCONLCD Bus Gating Reset Register */
	__IO uint32_t TCONTV_CLK_REG;                        /*!< Offset 0xB80 TCONTV Clock Register */
	uint32_t reserved_0xB84 [0x0006];
	__IO uint32_t TCONTV_BGR_REG;                        /*!< Offset 0xB9C TCONTV Bus Gating Reset Register */
	uint32_t reserved_0xBA0 [0x0003];
	__IO uint32_t LVDS_BGR_REG;                          /*!< Offset 0xBAC LVDS Bus Gating Reset Register */
	__IO uint32_t TVE_CLK_REG;                           /*!< Offset 0xBB0 TVE Clock Register */
	uint32_t reserved_0xBB4 [0x0002];
	__IO uint32_t TVE_BGR_REG;                           /*!< Offset 0xBBC TVE Bus Gating Reset Register */
	__IO uint32_t TVD_CLK_REG;                           /*!< Offset 0xBC0 TVD Clock Register */
	uint32_t reserved_0xBC4 [0x0006];
	__IO uint32_t TVD_BGR_REG;                           /*!< Offset 0xBDC TVD Bus Gating Reset Register */
	uint32_t reserved_0xBE0 [0x0004];
	__IO uint32_t LEDC_CLK_REG;                          /*!< Offset 0xBF0 LEDC Clock Register */
	uint32_t reserved_0xBF4 [0x0002];
	__IO uint32_t LEDC_BGR_REG;                          /*!< Offset 0xBFC LEDC Bus Gating Reset Register */
	uint32_t reserved_0xC00;
	__IO uint32_t CSI_CLK_REG;                           /*!< Offset 0xC04 CSI Clock Register */
	__IO uint32_t CSI_MASTER_CLK_REG;                    /*!< Offset 0xC08 CSI Master Clock Register */
	uint32_t reserved_0xC0C [0x0004];
	__IO uint32_t CSI_BGR_REG;                           /*!< Offset 0xC1C CSI Bus Gating Reset Register */
	uint32_t reserved_0xC20 [0x000C];
	__IO uint32_t TPADC_CLK_REG;                         /*!< Offset 0xC50 TPADC Clock Register */
	uint32_t reserved_0xC54 [0x0002];
	__IO uint32_t TPADC_BGR_REG;                         /*!< Offset 0xC5C TPADC Bus Gating Reset Register */
	uint32_t reserved_0xC60 [0x0004];
	__IO uint32_t DSP_CLK_REG;                           /*!< Offset 0xC70 DSP Clock Register */
	uint32_t reserved_0xC74 [0x0002];
	__IO uint32_t DSP_BGR_REG;                           /*!< Offset 0xC7C DSP Bus Gating Reset Register */
	uint32_t reserved_0xC80 [0x00A1];
	__IO uint32_t PLL_LOCK_DBG_CTRL_REG;                 /*!< Offset 0xF04 PLL Lock Debug Control Register */
	__IO uint32_t FRE_DET_CTRL_REG;                      /*!< Offset 0xF08 Frequency Detect Control Register */
	__IO uint32_t FRE_UP_LIM_REG;                        /*!< Offset 0xF0C Frequency Up Limit Register */
	__IO uint32_t FRE_DOWN_LIM_REG;                      /*!< Offset 0xF10 Frequency Down Limit Register */
	uint32_t reserved_0xF14 [0x0007];
	__IO uint32_t CCU_FAN_GATE_REG;                      /*!< Offset 0xF30 CCU FANOUT CLOCK GATE Register */
	__IO uint32_t CLK27M_FAN_REG;                        /*!< Offset 0xF34 CLK27M FANOUT Register */
	__IO uint32_t PCLK_FAN_REG;                          /*!< Offset 0xF38 PCLK FANOUT Register */
	__IO uint32_t CCU_FAN_REG;                           /*!< Offset 0xF3C CCU FANOUT Register */
} CCU_TypeDef; /* size of structure = 0xF40 */
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG Controller Interface */
typedef struct SYS_CFG_Type
{
	uint32_t reserved_0x000 [0x0002];
	__IO uint32_t DSP_BOOT_RAMMAP_REG;                   /*!< Offset 0x008 DSP Boot SRAM Remap Control Register */
	uint32_t reserved_0x00C [0x0006];
	__IO uint32_t VER_REG;                               /*!< Offset 0x024 Version Register */
	uint32_t reserved_0x028 [0x0002];
	__IO uint32_t EMAC_EPHY_CLK_REG0;                    /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
	uint32_t reserved_0x034 [0x0047];
	__IO uint32_t SYS_LDO_CTRL_REG;                      /*!< Offset 0x150 System LDO Control Register */
	uint32_t reserved_0x154 [0x0003];
	__IO uint32_t RESCAL_CTRL_REG;                       /*!< Offset 0x160 Resistor Calibration Control Register */
	uint32_t reserved_0x164;
	__IO uint32_t RES240_CTRL_REG;                       /*!< Offset 0x168 240ohms Resistor Manual Control Register */
	__IO uint32_t RESCAL_STATUS_REG;                     /*!< Offset 0x16C Resistor Calibration Status Register */
} SYS_CFG_TypeDef; /* size of structure = 0x170 */
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
	uint32_t reserved_0x020 [0x0017];
	__IO uint32_t UART_USR;                              /*!< Offset 0x07C UART Status Register */
	__IO uint32_t UART_TFL;                              /*!< Offset 0x080 UART Transmit FIFO Level Register */
	__IO uint32_t UART_RFL;                              /*!< Offset 0x084 UART Receive FIFO Level Register */
	__IO uint32_t UART_HSK;                              /*!< Offset 0x088 UART DMA Handshake Configuration Register */
	__IO uint32_t UART_DMA_REQ_EN;                       /*!< Offset 0x08C UART DMA Request Enable Register */
	uint32_t reserved_0x090 [0x0005];
	__IO uint32_t UART_HALT;                             /*!< Offset 0x0A4 UART Halt TX Register */
	uint32_t reserved_0x0A8 [0x0002];
	__IO uint32_t UART_DBG_DLL;                          /*!< Offset 0x0B0 UART Debug DLL Register */
	__IO uint32_t UART_DBG_DLH;                          /*!< Offset 0x0B4 UART Debug DLH Register */
	uint32_t reserved_0x0B8 [0x000E];
	__IO uint32_t UART_A_FCC;                            /*!< Offset 0x0F0 UART FIFO Clock Control Register */
	uint32_t reserved_0x0F4 [0x0003];
	__IO uint32_t UART_A_RXDMA_CTRL;                     /*!< Offset 0x100 UART RXDMA Control Register */
	__IO uint32_t UART_A_RXDMA_STR;                      /*!< Offset 0x104 UART RXDMA Start Register */
	__IO uint32_t UART_A_RXDMA_STA;                      /*!< Offset 0x108 UART RXDMA Status Register */
	__IO uint32_t UART_A_RXDMA_LMT;                      /*!< Offset 0x10C UART RXDMA Limit Register */
	__IO uint32_t UART_A_RXDMA_SADDRL;                   /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
	__IO uint32_t UART_A_RXDMA_SADDRH;                   /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
	__IO uint32_t UART_A_RXDMA_BL;                       /*!< Offset 0x118 UART RXDMA Buffer Length Register */
	uint32_t reserved_0x11C;
	__IO uint32_t UART_A_RXDMA_IE;                       /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
	__IO uint32_t UART_A_RXDMA_IS;                       /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
	__IO uint32_t UART_A_RXDMA_WADDRL;                   /*!< Offset 0x128 UART RXDMA Write Address Low Register */
	__IO uint32_t UART_A_RXDMA_WADDRH;                   /*!< Offset 0x12C UART RXDMA Write Address high Register */
	__IO uint32_t UART_A_RXDMA_RADDRL;                   /*!< Offset 0x130 UART RXDMA Read Address Low Register */
	__IO uint32_t UART_A_RXDMA_RADDRH;                   /*!< Offset 0x134 UART RXDMA Read Address high Register */
	__IO uint32_t UART_A_RXDMA_DCNT;                     /*!< Offset 0x138 UART RADMA Data Count Register */
} UART_TypeDef; /* size of structure = 0x13C */
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
	uint32_t reserved_0x02C;
} GPIO_TypeDef; /* size of structure = 0x030 */
/*
 * @brief GPIOINT
 */
/*!< GPIOINT Controller Interface */
typedef struct GPIOINT_Type
{
	__IO uint32_t EINT_CFG [0x004];                      /*!< Offset 0x000 External Interrupt Configure Registers */
	__IO uint32_t EINT_CTL;                              /*!< Offset 0x010 External Interrupt Control Register */
	__IO uint32_t EINT_STATUS;                           /*!< Offset 0x014 External Interrupt Status Register */
	__IO uint32_t EINT_DEB;                              /*!< Offset 0x018 External Interrupt Debounce Register */
	uint32_t reserved_0x01C;
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK Controller Interface */
typedef struct GPIOBLOCK_Type
{
	GPIO_TypeDef GPIO_PINS [0x007];                      /*!< Offset 0x000 GPIO pin control */
	uint32_t reserved_0x150 [0x002C];
	GPIOINT_TypeDef GPIO_INTS [0x007];                   /*!< Offset 0x200 GPIO interrupt control */
	uint32_t reserved_0x2E0 [0x0018];
	__IO uint32_t PIO_POW_MOD_SEL;                       /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
	__IO uint32_t PIO_POW_MS_CTL;                        /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
	__IO uint32_t PIO_POW_VAL;                           /*!< Offset 0x348 PIO Group Power Value Register */
} GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
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
	uint32_t reserved_0x060 [0x0006];
	__IO uint32_t SMHC_HWRST;                            /*!< Offset 0x078 Hardware Reset Register */
	uint32_t reserved_0x07C;
	__IO uint32_t SMHC_IDMAC;                            /*!< Offset 0x080 IDMAC Control Register */
	__IO uint32_t SMHC_DLBA;                             /*!< Offset 0x084 Descriptor List Base Address Register */
	__IO uint32_t SMHC_IDST;                             /*!< Offset 0x088 IDMAC Status Register */
	__IO uint32_t SMHC_IDIE;                             /*!< Offset 0x08C IDMAC Interrupt Enable Register */
	uint32_t reserved_0x090 [0x001C];
	__IO uint32_t SMHC_THLD;                             /*!< Offset 0x100 Card Threshold Control Register */
	__IO uint32_t SMHC_SFC;                              /*!< Offset 0x104 Sample FIFO Control Register */
	__IO uint32_t SMHC_A23A;                             /*!< Offset 0x108 Auto Command 23 Argument Register */
	__IO uint32_t EMMC_DDR_SBIT_DET;                     /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
	uint32_t reserved_0x110 [0x000A];
	__IO uint32_t SMHC_EXT_CMD;                          /*!< Offset 0x138 Extended Command Register */
	__IO uint32_t SMHC_EXT_RESP;                         /*!< Offset 0x13C Extended Response Register */
	__IO uint32_t SMHC_DRV_DL;                           /*!< Offset 0x140 Drive Delay Control Register */
	__IO uint32_t SMHC_SMAP_DL;                          /*!< Offset 0x144 Sample Delay Control Register */
	__IO uint32_t SMHC_DS_DL;                            /*!< Offset 0x148 Data Strobe Delay Control Register */
	__IO uint32_t SMHC_HS400_DL;                         /*!< Offset 0x14C HS400 Delay Control Register */
	uint32_t reserved_0x150 [0x002C];
	__IO uint32_t SMHC_FIFO;                             /*!< Offset 0x200 Read/Write FIFO */
} SMHC_TypeDef; /* size of structure = 0x204 */
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
	uint32_t reserved_0x078 [0x0002];
	__IO uint32_t MCLKCFG;                               /*!< Offset 0x080 ASRC MCLK Configuration Register */
	__IO uint32_t FsoutCFG;                              /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
	__IO uint32_t FsinEXTCFG;                            /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
	__IO uint32_t ASRCCFG;                               /*!< Offset 0x08C ASRC Enable Register */
	__IO uint32_t ASRCMANCFG;                            /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
	__IO uint32_t ASRCRATIOSTAT;                         /*!< Offset 0x094 ASRC Status Register */
	__IO uint32_t ASRCFIFOSTAT;                          /*!< Offset 0x098 ASRC FIFO Level Status Register */
	__IO uint32_t ASRCMBISTCFG;                          /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
	__IO uint32_t ASRCMBISTSTAT;                         /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief DMIC
 */
/*!< DMIC Controller Interface */
typedef struct DMIC_Type
{
	__IO uint32_t DMIC_EN;                               /*!< Offset 0x000 DMIC Enable Control Register */
	__IO uint32_t DMIC_SR;                               /*!< Offset 0x004 DMIC Sample Rate Register */
	__IO uint32_t DMIC_CTR;                              /*!< Offset 0x008 DMIC Control Register */
	uint32_t reserved_0x00C;
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
} DMIC_TypeDef; /* size of structure = 0x044 */
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
	uint32_t reserved_0x03C;
	__IO uint32_t OWA_EXP_CTL;                           /*!< Offset 0x040 OWA Expand Control Register */
	__IO uint32_t OWA_EXP_ISTA;                          /*!< Offset 0x044 OWA Expand Interrupt Status Register */
	__IO uint32_t OWA_EXP_INFO_0;                        /*!< Offset 0x048 OWA Expand Infomation Register0 */
	__IO uint32_t OWA_EXP_INFO_1;                        /*!< Offset 0x04C OWA Expand Infomation Register1 */
	__IO uint32_t OWA_EXP_DBG_0;                         /*!< Offset 0x050 OWA Expand Debug Register0 */
	__IO uint32_t OWA_EXP_DBG_1;                         /*!< Offset 0x054 OWA Expand Debug Register1 */
} OWA_TypeDef; /* size of structure = 0x058 */
/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Controller Interface */
typedef struct AUDIO_CODEC_Type
{
	__IO uint32_t AC_DAC_DPC;                            /*!< Offset 0x000 DAC Digital Part Control Register */
	__IO uint32_t DAC_VOL_CTRL;                          /*!< Offset 0x004 DAC Volume Control Register */
	uint32_t reserved_0x008 [0x0002];
	__IO uint32_t AC_DAC_FIFOC;                          /*!< Offset 0x010 DAC FIFO Control Register */
	__IO uint32_t AC_DAC_FIFOS;                          /*!< Offset 0x014 DAC FIFO Status Register */
	uint32_t reserved_0x018 [0x0002];
	__IO uint32_t AC_DAC_TXDATA;                         /*!< Offset 0x020 DAC TX DATA Register */
	__IO uint32_t AC_DAC_CNT;                            /*!< Offset 0x024 DAC TX FIFO Counter Register */
	__IO uint32_t AC_DAC_DG;                             /*!< Offset 0x028 DAC Debug Register */
	uint32_t reserved_0x02C;
	__IO uint32_t AC_ADC_FIFOC;                          /*!< Offset 0x030 ADC FIFO Control Register */
	__IO uint32_t ADC_VOL_CTRL1;                         /*!< Offset 0x034 ADC Volume Control1 Register */
	__IO uint32_t AC_ADC_FIFOS;                          /*!< Offset 0x038 ADC FIFO Status Register */
	uint32_t reserved_0x03C;
	__IO uint32_t AC_ADC_RXDATA;                         /*!< Offset 0x040 ADC RX Data Register */
	__IO uint32_t AC_ADC_CNT;                            /*!< Offset 0x044 ADC RX Counter Register */
	uint32_t reserved_0x048;
	__IO uint32_t AC_ADC_DG;                             /*!< Offset 0x04C ADC Debug Register */
	__IO uint32_t ADC_DIG_CTRL;                          /*!< Offset 0x050 ADC Digtial Control Register */
	__IO uint32_t VRA1SPEEDUP_DOWN_CTRL;                 /*!< Offset 0x054 VRA1 Speedup Down Control Register */
	uint32_t reserved_0x058 [0x0026];
	__IO uint32_t AC_DAC_DAP_CTRL;                       /*!< Offset 0x0F0 DAC DAP Control Register */
	uint32_t reserved_0x0F4;
	__IO uint32_t AC_ADC_DAP_CTR;                        /*!< Offset 0x0F8 ADC DAP Control Register */
	uint32_t reserved_0x0FC;
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
	uint32_t reserved_0x1B4;
	__IO uint32_t AC_DAC_DRC_HPFHGAIN;                   /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
	__IO uint32_t AC_DAC_DRC_HPFLGAIN;                   /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
	uint32_t reserved_0x1C0 [0x0010];
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
	uint32_t reserved_0x2B4;
	__IO uint32_t AC_ADC_DRC_HPFHGAIN;                   /*!< Offset 0x2B8 ADC DRC HPF Gain High Coef Register */
	__IO uint32_t AC_ADC_DRC_HPFLGAIN;                   /*!< Offset 0x2BC ADC DRC HPF Gain Low Coef Register */
	uint32_t reserved_0x2C0 [0x0010];
	__IO uint32_t ADC1_REG;                              /*!< Offset 0x300 ADC1 Analog Control Register */
	__IO uint32_t ADC2_REG;                              /*!< Offset 0x304 ADC2 Analog Control Register */
	__IO uint32_t ADC3_REG;                              /*!< Offset 0x308 ADC3 Analog Control Register */
	uint32_t reserved_0x30C;
	__IO uint32_t DAC_REG;                               /*!< Offset 0x310 DAC Analog Control Register */
	uint32_t reserved_0x314;
	__IO uint32_t MICBIAS_REG;                           /*!< Offset 0x318 MICBIAS Analog Control Register */
	__IO uint32_t RAMP_REG;                              /*!< Offset 0x31C BIAS Analog Control Register */
	__IO uint32_t BIAS_REG;                              /*!< Offset 0x320 BIAS Analog Control Register */
	uint32_t reserved_0x324 [0x0003];
	__IO uint32_t ADC5_REG;                              /*!< Offset 0x330 ADC5 Analog Control Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x334 */
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
	uint32_t reserved_0x024 [0x0077];
	__IO uint32_t TWI_DRV_CTRL;                          /*!< Offset 0x200 TWI_DRV Control Register */
	__IO uint32_t TWI_DRV_CFG;                           /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
	__IO uint32_t TWI_DRV_SLV;                           /*!< Offset 0x208 TWI_DRV Slave ID Register */
	__IO uint32_t TWI_DRV_FMT;                           /*!< Offset 0x20C TWI_DRV Packet Format Register */
	__IO uint32_t TWI_DRV_BUS_CTRL;                      /*!< Offset 0x210 TWI_DRV Bus Control Register */
	__IO uint32_t TWI_DRV_INT_CTRL;                      /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
	__IO uint32_t TWI_DRV_DMA_CFG;                       /*!< Offset 0x218 TWI_DRV DMA Configure Register */
	__IO uint32_t TWI_DRV_FIFO_CON;                      /*!< Offset 0x21C TWI_DRV FIFO Content Register */
	uint32_t reserved_0x220 [0x0038];
	__IO uint32_t TWI_DRV_SEND_FIFO_ACC;                 /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
	__IO uint32_t TWI_DRV_RECV_FIFO_ACC;                 /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
} TWI_TypeDef; /* size of structure = 0x308 */
/*
 * @brief SPI
 */
/*!< SPI Controller Interface */
typedef struct SPI_Type
{
	uint32_t reserved_0x000;
	__IO uint32_t SPI_GCR;                               /*!< Offset 0x004 SPI Global Control Register */
	__IO uint32_t SPI_TCR;                               /*!< Offset 0x008 SPI Transfer Control Register */
	uint32_t reserved_0x00C;
	__IO uint32_t SPI_IER;                               /*!< Offset 0x010 SPI Interrupt Control Register */
	__IO uint32_t SPI_ISR;                               /*!< Offset 0x014 SPI Interrupt Status Register */
	__IO uint32_t SPI_FCR;                               /*!< Offset 0x018 SPI FIFO Control Register */
	__IO uint32_t SPI_FSR;                               /*!< Offset 0x01C SPI FIFO Status Register */
	__IO uint32_t SPI_WCR;                               /*!< Offset 0x020 SPI Wait Clock Register */
	uint32_t reserved_0x024;
	__IO uint32_t SPI_SAMP_DL;                           /*!< Offset 0x028 SPI Sample Delay Control Register */
	uint32_t reserved_0x02C;
	__IO uint32_t SPI_MBC;                               /*!< Offset 0x030 SPI Master Burst Counter Register */
	__IO uint32_t SPI_MTC;                               /*!< Offset 0x034 SPI Master Transmit Counter Register */
	__IO uint32_t SPI_BCC;                               /*!< Offset 0x038 SPI Master Burst Control Register */
	uint32_t reserved_0x03C;
	__IO uint32_t SPI_BATCR;                             /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
	__IO uint32_t SPI_BA_CCR;                            /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
	__IO uint32_t SPI_TBR;                               /*!< Offset 0x048 SPI TX Bit Register */
	__IO uint32_t SPI_RBR;                               /*!< Offset 0x04C SPI RX Bit Register */
	uint32_t reserved_0x050 [0x000E];
	__IO uint32_t SPI_NDMA_MODE_CTL;                     /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
	uint32_t reserved_0x08C [0x005D];
	__IO uint32_t SPI_TXD;                               /*!< Offset 0x200 SPI TX Data Register */
	uint32_t reserved_0x204 [0x003F];
	__IO uint32_t SPI_RXD;                               /*!< Offset 0x300 SPI RX Data Register */
} SPI_TypeDef; /* size of structure = 0x304 */
/*
 * @brief CIR_RX
 */
/*!< CIR_RX Controller Interface */
typedef struct CIR_RX_Type
{
	__IO uint32_t CIR_CTL;                               /*!< Offset 0x000 CIR Control Register */
	uint32_t reserved_0x004 [0x0003];
	__IO uint32_t CIR_RXPCFG;                            /*!< Offset 0x010 CIR Receiver Pulse Configure Register */
	uint32_t reserved_0x014 [0x0003];
	__IO uint32_t CIR_RXFIFO;                            /*!< Offset 0x020 CIR Receiver FIFO Register */
	uint32_t reserved_0x024 [0x0002];
	__IO uint32_t CIR_RXINT;                             /*!< Offset 0x02C CIR Receiver Interrupt Control Register */
	__IO uint32_t CIR_RXSTA;                             /*!< Offset 0x030 CIR Receiver Status Register */
	__IO uint32_t CIR_RXCFG;                             /*!< Offset 0x034 CIR Receiver Configure Register */
} CIR_RX_TypeDef; /* size of structure = 0x038 */
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
	uint32_t reserved_0x01C;
	__IO uint32_t CIR_TEL;                               /*!< Offset 0x020 CIR TX FIFO Empty Level Register */
	__IO uint32_t CIR_TXINT;                             /*!< Offset 0x024 CIR Transmit Interrupt Control Register */
	__IO uint32_t CIR_TAC;                               /*!< Offset 0x028 CIR Transmit FIFO Available Counter Register */
	__IO uint32_t CIR_TXSTA;                             /*!< Offset 0x02C CIR Transmit Status Register */
	__IO uint32_t CIR_TXT;                               /*!< Offset 0x030 CIR Transmit Threshold Register */
	__IO uint32_t CIR_DMA;                               /*!< Offset 0x034 CIR DMA Control Register */
	uint32_t reserved_0x038 [0x0012];
	__IO uint32_t CIR_TXFIFO;                            /*!< Offset 0x080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef; /* size of structure = 0x084 */
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
	uint32_t reserved_0x024;
	__IO uint32_t LEDC_WAIT_TIME1_CTRL_REG;              /*!< Offset 0x028 LEDC Wait Time1 Control Register */
	uint32_t reserved_0x02C;
	__IO uint32_t LEDC_FIFO_DATA_REG [0x020];            /*!< Offset 0x030 LEDC FIFO Data Registers array */
} LEDC_TypeDef; /* size of structure = 0x0B0 */
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
	uint32_t reserved_0x018;
	__IO uint32_t TP_CALI_DATA_REG;                      /*!< Offset 0x01C TP Calibration Data Register */
	uint32_t reserved_0x020;
	__IO uint32_t TP_DATA_REG;                           /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef; /* size of structure = 0x028 */
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
	uint32_t reserved_0x01C;
	__IO uint32_t GP_DATAL_INTC;                         /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
	__IO uint32_t GP_DATAH_INTC;                         /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
	__IO uint32_t GP_DATA_INTC;                          /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
	uint32_t reserved_0x02C;
	__IO uint32_t GP_DATAL_INTS;                         /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
	__IO uint32_t GP_DATAH_INTS;                         /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
	__IO uint32_t GP_DATA_INTS;                          /*!< Offset 0x038 GPADC Data Interrupt Status Register */
	uint32_t reserved_0x03C;
	__IO uint32_t GP_CH0_CMP_DATA;                       /*!< Offset 0x040 GPADC CH0 Compare Data Register */
	uint32_t reserved_0x044 [0x000F];
	__IO uint32_t GP_CH0_DATA;                           /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef; /* size of structure = 0x084 */
/*
 * @brief SPI_DBI
 */
/*!< SPI_DBI Controller Interface */
typedef struct SPI_DBI_Type
{
	uint32_t reserved_0x000;
	__IO uint32_t SPI_GCR;                               /*!< Offset 0x004 SPI Global Control Register */
	__IO uint32_t SPI_TCR;                               /*!< Offset 0x008 SPI Transfer Control Register */
	uint32_t reserved_0x00C;
	__IO uint32_t SPI_IER;                               /*!< Offset 0x010 SPI Interrupt Control Register */
	__IO uint32_t SPI_ISR;                               /*!< Offset 0x014 SPI Interrupt Status Register */
	__IO uint32_t SPI_FCR;                               /*!< Offset 0x018 SPI FIFO Control Register */
	__IO uint32_t SPI_FSR;                               /*!< Offset 0x01C SPI FIFO Status Register */
	__IO uint32_t SPI_WCR;                               /*!< Offset 0x020 SPI Wait Clock Register */
	uint32_t reserved_0x024;
	__IO uint32_t SPI_SAMP_DL;                           /*!< Offset 0x028 SPI Sample Delay Control Register */
	uint32_t reserved_0x02C;
	__IO uint32_t SPI_MBC;                               /*!< Offset 0x030 SPI Master Burst Counter Register */
	__IO uint32_t SPI_MTC;                               /*!< Offset 0x034 SPI Master Transmit Counter Register */
	__IO uint32_t SPI_BCC;                               /*!< Offset 0x038 SPI Master Burst Control Register */
	uint32_t reserved_0x03C;
	__IO uint32_t SPI_BATCR;                             /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
	__IO uint32_t SPI_BA_CCR;                            /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
	__IO uint32_t SPI_TBR;                               /*!< Offset 0x048 SPI TX Bit Register */
	__IO uint32_t SPI_RBR;                               /*!< Offset 0x04C SPI RX Bit Register */
	uint32_t reserved_0x050 [0x000E];
	__IO uint32_t SPI_NDMA_MODE_CTL;                     /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
	uint32_t reserved_0x08C [0x001D];
	__IO uint32_t DBI_CTL_0;                             /*!< Offset 0x100 DBI Control Register 0 */
	__IO uint32_t DBI_CTL_1;                             /*!< Offset 0x104 DBI Control Register 1 */
	__IO uint32_t DBI_CTL_2;                             /*!< Offset 0x108 DBI Control Register 2 */
	__IO uint32_t DBI_TIMER;                             /*!< Offset 0x10C DBI Timer Control Register */
	__IO uint32_t DBI_VIDEO_SZIE;                        /*!< Offset 0x110 DBI Video Size Configuration Register */
	uint32_t reserved_0x114 [0x0003];
	__IO uint32_t DBI_INT;                               /*!< Offset 0x120 DBI Interrupt Register */
	__IO uint32_t DBI_DEBUG_0;                           /*!< Offset 0x124 DBI BEBUG 0 Register */
	__IO uint32_t DBI_DEBUG_1;                           /*!< Offset 0x128 DBI BEBUG 1 Register */
	uint32_t reserved_0x12C [0x0035];
	__IO uint32_t SPI_TXD;                               /*!< Offset 0x200 SPI TX Data register */
	uint32_t reserved_0x204 [0x003F];
	__IO uint32_t SPI_RXD;                               /*!< Offset 0x300 SPI RX Data register */
} SPI_DBI_TypeDef; /* size of structure = 0x304 */
/*
 * @brief CE
 */
/*!< CE Controller Interface */
typedef struct CE_Type
{
	__IO uint32_t CE_TDA;                                /*!< Offset 0x000 Task Descriptor Address */
	uint32_t reserved_0x004;
	__IO uint32_t CE_ICR;                                /*!< Offset 0x008 Interrupt Control Register */
	__IO uint32_t CE_ISR;                                /*!< Offset 0x00C Interrupt Status Register */
	__IO uint32_t CE_TLR;                                /*!< Offset 0x010 Task Load Register */
	__IO uint32_t CE_TSR;                                /*!< Offset 0x014 Task Status Register */
	__IO uint32_t CE_ESR;                                /*!< Offset 0x018 Error Status Register */
	uint32_t reserved_0x01C [0x0002];
	__IO uint32_t CE_CSA;                                /*!< Offset 0x024 DMA Current Source Address */
	__IO uint32_t CE_CDA;                                /*!< Offset 0x028 DMA Current Destination Address */
	__IO uint32_t CE_TPR;                                /*!< Offset 0x02C Throughput Register */
} CE_TypeDef; /* size of structure = 0x030 */
/*
 * @brief RTC
 */
/*!< RTC Controller Interface */
typedef struct RTC_Type
{
	__IO uint32_t LOSC_CTRL_REG;                         /*!< Offset 0x000 Low Oscillator Control Register */
	__IO uint32_t LOSC_AUTO_SWT_STA_REG;                 /*!< Offset 0x004 Low Oscillator Auto Switch Status Register */
	__IO uint32_t INTOSC_CLK_PRESCAL_REG;                /*!< Offset 0x008 Internal OSC Clock Pre-scaler Register */
	uint32_t reserved_0x00C;
	__IO uint32_t RTC_DAY_REG;                           /*!< Offset 0x010 RTC Year-Month-Day Register */
	__IO uint32_t RTC_HH_MM_SS_REG;                      /*!< Offset 0x014 RTC Hour-Minute-Second Register */
	uint32_t reserved_0x018 [0x0002];
	__IO uint32_t ALARM0_DAY_SET_REG;                    /*!< Offset 0x020 Alarm 0 Day Setting Register */
	__IO uint32_t ALARM0_CUR_VLU_REG;                    /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
	__IO uint32_t ALARM0_ENABLE_REG;                     /*!< Offset 0x028 Alarm 0 Enable Register */
	__IO uint32_t ALARM0_IRQ_EN;                         /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
	__IO uint32_t ALARM0_IRQ_STA_REG;                    /*!< Offset 0x030 Alarm 0 IRQ Status Register */
	uint32_t reserved_0x034 [0x0007];
	__IO uint32_t ALARM_CONFIG_REG;                      /*!< Offset 0x050 Alarm Configuration Register */
	uint32_t reserved_0x054 [0x0003];
	__IO uint32_t F32K_FOUT_CTRL_GATING_REG;             /*!< Offset 0x060 32K Fanout Control Gating Register */
	uint32_t reserved_0x064 [0x0027];
	__IO uint32_t GP_DATA_REG [0x008];                   /*!< Offset 0x100 General Purpose Register (N=0 to 7) */
	__IO uint32_t FBOOT_INFO_REG0;                       /*!< Offset 0x120 Fast Boot Information Register0 */
	__IO uint32_t FBOOT_INFO_REG1;                       /*!< Offset 0x124 Fast Boot Information Register1 */
	uint32_t reserved_0x128 [0x000E];
	__IO uint32_t DCXO_CTRL_REG;                         /*!< Offset 0x160 DCXO Control Register */
	uint32_t reserved_0x164 [0x000B];
	__IO uint32_t RTC_VIO_REG;                           /*!< Offset 0x190 RTC_VIO Regulation Register */
	uint32_t reserved_0x194 [0x0017];
	__IO uint32_t IC_CHARA_REG;                          /*!< Offset 0x1F0 IC Characteristic Register */
	__IO uint32_t VDD_OFF_GATING_CTRL_REG;               /*!< Offset 0x1F4 VDD Off Gating Control Register */
	uint32_t reserved_0x1F8 [0x0003];
	__IO uint32_t EFUSE_HV_PWRSWT_CTRL_REG;              /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
	uint32_t reserved_0x208 [0x0042];
	__IO uint32_t RTC_SPI_CLK_CTRL_REG;                  /*!< Offset 0x310 RTC SPI Clock Control Register */
} RTC_TypeDef; /* size of structure = 0x314 */
/*
 * @brief IOMMU
 */
/*!< IOMMU Controller Interface */
typedef struct IOMMU_Type
{
	uint32_t reserved_0x000 [0x0004];
	__IO uint32_t IOMMU_RESET_REG;                       /*!< Offset 0x010 IOMMU Reset Register */
	uint32_t reserved_0x014 [0x0003];
	__IO uint32_t IOMMU_ENABLE_REG;                      /*!< Offset 0x020 IOMMU Enable Register */
	uint32_t reserved_0x024 [0x0003];
	__IO uint32_t IOMMU_BYPASS_REG;                      /*!< Offset 0x030 IOMMU Bypass Register */
	uint32_t reserved_0x034 [0x0003];
	__IO uint32_t IOMMU_AUTO_GATING_REG;                 /*!< Offset 0x040 IOMMU Auto Gating Register */
	__IO uint32_t IOMMU_WBUF_CTRL_REG;                   /*!< Offset 0x044 IOMMU Write Buffer Control Register */
	__IO uint32_t IOMMU_OOO_CTRL_REG;                    /*!< Offset 0x048 IOMMU Out of Order Control Register */
	__IO uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;            /*!< Offset 0x04C IOMMU 4KB Boundary Protect Control Register */
	__IO uint32_t IOMMU_TTB_REG;                         /*!< Offset 0x050 IOMMU Translation Table Base Register */
	uint32_t reserved_0x054 [0x0003];
	__IO uint32_t IOMMU_TLB_ENABLE_REG;                  /*!< Offset 0x060 IOMMU TLB Enable Register */
	uint32_t reserved_0x064 [0x0003];
	__IO uint32_t IOMMU_TLB_PREFETCH_REG;                /*!< Offset 0x070 IOMMU TLB Prefetch Register */
	uint32_t reserved_0x074 [0x0003];
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
	uint32_t reserved_0x0D4 [0x000B];
	__IO uint32_t IOMMU_INT_ENABLE_REG;                  /*!< Offset 0x100 IOMMU Interrupt Enable Register */
	__IO uint32_t IOMMU_INT_CLR_REG;                     /*!< Offset 0x104 IOMMU Interrupt Clear Register */
	__IO uint32_t IOMMU_INT_STA_REG;                     /*!< Offset 0x108 IOMMU Interrupt Status Register */
	uint32_t reserved_0x10C;
	__IO uint32_t IOMMU_INT_ERR_ADDR0_REG;               /*!< Offset 0x110 IOMMU Interrupt Error Address 0 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR1_REG;               /*!< Offset 0x114 IOMMU Interrupt Error Address 1 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR2_REG;               /*!< Offset 0x118 IOMMU Interrupt Error Address 2 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR3_REG;               /*!< Offset 0x11C IOMMU Interrupt Error Address 3 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR4_REG;               /*!< Offset 0x120 IOMMU Interrupt Error Address 4 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR5_REG;               /*!< Offset 0x124 IOMMU Interrupt Error Address 5 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR6_REG;               /*!< Offset 0x128 IOMMU Interrupt Error Address 6 Register */
	uint32_t reserved_0x12C;
	__IO uint32_t IOMMU_INT_ERR_ADDR7_REG;               /*!< Offset 0x130 IOMMU Interrupt Error Address 7 Register */
	__IO uint32_t IOMMU_INT_ERR_ADDR8_REG;               /*!< Offset 0x134 IOMMU Interrupt Error Address 8 Register */
	uint32_t reserved_0x138 [0x0006];
	__IO uint32_t IOMMU_INT_ERR_DATA0_REG;               /*!< Offset 0x150 IOMMU Interrupt Error Data 0 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA1_REG;               /*!< Offset 0x154 IOMMU Interrupt Error Data 1 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA2_REG;               /*!< Offset 0x158 IOMMU Interrupt Error Data 2 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA3_REG;               /*!< Offset 0x15C IOMMU Interrupt Error Data 3 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA4_REG;               /*!< Offset 0x160 IOMMU Interrupt Error Data 4 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA5_REG;               /*!< Offset 0x164 IOMMU Interrupt Error Data 5 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA6_REG;               /*!< Offset 0x168 IOMMU Interrupt Error Data 6 Register */
	uint32_t reserved_0x16C;
	__IO uint32_t IOMMU_INT_ERR_DATA7_REG;               /*!< Offset 0x170 IOMMU Interrupt Error Data 7 Register */
	__IO uint32_t IOMMU_INT_ERR_DATA8_REG;               /*!< Offset 0x174 IOMMU Interrupt Error Data 8 Register */
	uint32_t reserved_0x178 [0x0002];
	__IO uint32_t IOMMU_L1PG_INT_REG;                    /*!< Offset 0x180 IOMMU L1 Page Table Interrupt Register */
	__IO uint32_t IOMMU_L2PG_INT_REG;                    /*!< Offset 0x184 IOMMU L2 Page Table Interrupt Register */
	uint32_t reserved_0x188 [0x0002];
	__IO uint32_t IOMMU_VA_REG;                          /*!< Offset 0x190 IOMMU Virtual Address Register */
	__IO uint32_t IOMMU_VA_DATA_REG;                     /*!< Offset 0x194 IOMMU Virtual Address Data Register */
	__IO uint32_t IOMMU_VA_CONFIG_REG;                   /*!< Offset 0x198 IOMMU Virtual Address Configuration Register */
	uint32_t reserved_0x19C [0x0019];
	__IO uint32_t IOMMU_PMU_ENABLE_REG;                  /*!< Offset 0x200 IOMMU PMU Enable Register */
	uint32_t reserved_0x204 [0x0003];
	__IO uint32_t IOMMU_PMU_CLR_REG;                     /*!< Offset 0x210 IOMMU PMU Clear Register */
	uint32_t reserved_0x214 [0x0007];
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
	uint32_t reserved_0x2A0 [0x000C];
	__IO uint32_t IOMMU_PMU_ACCESS_LOW7_REG;             /*!< Offset 0x2D0 IOMMU PMU Access Low 7 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH7_REG;            /*!< Offset 0x2D4 IOMMU PMU Access High 7 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW7_REG;                /*!< Offset 0x2D8 IOMMU PMU Hit Low 7 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH7_REG;               /*!< Offset 0x2DC IOMMU PMU Hit High 7 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_LOW8_REG;             /*!< Offset 0x2E0 IOMMU PMU Access Low 8 Register */
	__IO uint32_t IOMMU_PMU_ACCESS_HIGH8_REG;            /*!< Offset 0x2E4 IOMMU PMU Access High 8 Register */
	__IO uint32_t IOMMU_PMU_HIT_LOW8_REG;                /*!< Offset 0x2E8 IOMMU PMU Hit Low 8 Register */
	__IO uint32_t IOMMU_PMU_HIT_HIGH8_REG;               /*!< Offset 0x2EC IOMMU PMU Hit High 8 Register */
	uint32_t reserved_0x2F0 [0x0004];
	__IO uint32_t IOMMU_PMU_TL_LOW0_REG;                 /*!< Offset 0x300 IOMMU Total Latency Low 0 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH0_REG;                /*!< Offset 0x304 IOMMU Total Latency High 0 Register */
	__IO uint32_t IOMMU_PMU_ML0_REG;                     /*!< Offset 0x308 IOMMU Max Latency 0 Register */
	uint32_t reserved_0x30C;
	__IO uint32_t IOMMU_PMU_TL_LOW1_REG;                 /*!< Offset 0x310 IOMMU Total Latency Low 1 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH1_REG;                /*!< Offset 0x314 IOMMU Total Latency High 1 Register */
	__IO uint32_t IOMMU_PMU_ML1_REG;                     /*!< Offset 0x318 IOMMU Max Latency 1 Register */
	uint32_t reserved_0x31C;
	__IO uint32_t IOMMU_PMU_TL_LOW2_REG;                 /*!< Offset 0x320 IOMMU Total Latency Low 2 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH2_REG;                /*!< Offset 0x324 IOMMU Total Latency High 2 Register */
	__IO uint32_t IOMMU_PMU_ML2_REG;                     /*!< Offset 0x328 IOMMU Max Latency 2 Register */
	uint32_t reserved_0x32C;
	__IO uint32_t IOMMU_PMU_TL_LOW3_REG;                 /*!< Offset 0x330 IOMMU Total Latency Low 3 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH3_REG;                /*!< Offset 0x334 IOMMU Total Latency High 3 Register */
	__IO uint32_t IOMMU_PMU_ML3_REG;                     /*!< Offset 0x338 IOMMU Max Latency 3 Register */
	uint32_t reserved_0x33C;
	__IO uint32_t IOMMU_PMU_TL_LOW4_REG;                 /*!< Offset 0x340 IOMMU Total Latency Low 4 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH4_REG;                /*!< Offset 0x344 IOMMU Total Latency High 4 Register */
	__IO uint32_t IOMMU_PMU_ML4_REG;                     /*!< Offset 0x348 IOMMU Max Latency 4 Register */
	uint32_t reserved_0x34C;
	__IO uint32_t IOMMU_PMU_TL_LOW5_REG;                 /*!< Offset 0x350 IOMMU Total Latency Low 5 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH5_REG;                /*!< Offset 0x354 IOMMU Total Latency High 5 Register */
	__IO uint32_t IOMMU_PMU_ML5_REG;                     /*!< Offset 0x358 IOMMU Max Latency 5 Register */
	uint32_t reserved_0x35C;
	__IO uint32_t IOMMU_PMU_TL_LOW6_REG;                 /*!< Offset 0x360 IOMMU Total Latency Low 6 Register */
	__IO uint32_t IOMMU_PMU_TL_HIGH6_REG;                /*!< Offset 0x364 IOMMU Total Latency High 6 Register */
	__IO uint32_t IOMMU_PMU_ML6_REG;                     /*!< Offset 0x368 IOMMU Max Latency 6 Register */
} IOMMU_TypeDef; /* size of structure = 0x36C */
/*
 * @brief THS
 */
/*!< THS Controller Interface */
typedef struct THS_Type
{
	__IO uint32_t THS_CTRL;                              /*!< Offset 0x000 THS Control Register */
	__IO uint32_t THS_EN;                                /*!< Offset 0x004 THS Enable Register */
	__IO uint32_t THS_PER;                               /*!< Offset 0x008 THS Period Control Register */
	uint32_t reserved_0x00C;
	__IO uint32_t THS_DATA_INTC;                         /*!< Offset 0x010 THS Data Interrupt Control Register */
	__IO uint32_t THS_SHUT_INTC;                         /*!< Offset 0x014 THS Shut Interrupt Control Register */
	__IO uint32_t THS_ALARM_INTC;                        /*!< Offset 0x018 THS Alarm Interrupt Control Register */
	uint32_t reserved_0x01C;
	__IO uint32_t THS_DATA_INTS;                         /*!< Offset 0x020 THS Data Interrupt Status Register */
	__IO uint32_t THS_SHUT_INTS;                         /*!< Offset 0x024 THS Shut Interrupt Status Register */
	__IO uint32_t THS_ALARMO_INTS;                       /*!< Offset 0x028 THS Alarm off Interrupt Status Register */
	__IO uint32_t THS_ALARM_INTS;                        /*!< Offset 0x02C THS Alarm Interrupt Status Register */
	__IO uint32_t THS_FILTER;                            /*!< Offset 0x030 THS Median Filter Control Register */
	uint32_t reserved_0x034 [0x0003];
	__IO uint32_t THS_ALARM_CTRL;                        /*!< Offset 0x040 THS Alarm Threshold Control Register */
	uint32_t reserved_0x044 [0x000F];
	__IO uint32_t THS_SHUTDOWN_CTRL;                     /*!< Offset 0x080 THS Shutdown Threshold Control Register */
	uint32_t reserved_0x084 [0x0007];
	__IO uint32_t THS_CDATA;                             /*!< Offset 0x0A0 THS Calibration Data */
	uint32_t reserved_0x0A4 [0x0007];
	__IO uint32_t THS_DATA;                              /*!< Offset 0x0C0 THS Data Register */
} THS_TypeDef; /* size of structure = 0x0C4 */
/*
 * @brief TIMER
 */
/*!< TIMER Controller Interface */
typedef struct TIMER_Type
{
	__IO uint32_t TMR_IRQ_EN_REG;                        /*!< Offset 0x000 Timer IRQ Enable Register */
	__IO uint32_t TMR_IRQ_STA_REG;                       /*!< Offset 0x004 Timer Status Register */
	uint32_t reserved_0x008 [0x0002];
	__IO uint32_t TMR0_CTRL_REG;                         /*!< Offset 0x010 Timer0 Control Register */
	__IO uint32_t TMR0_INTV_VALUE_REG;                   /*!< Offset 0x014 Timer0 Interval Value Register */
	__IO uint32_t TMR0_CUR_VALUE_REG;                    /*!< Offset 0x018 Timer0 Current Value Register */
	uint32_t reserved_0x01C;
	__IO uint32_t TMR1_CTRL_REG;                         /*!< Offset 0x020 Timer1 Control Register */
	__IO uint32_t TMR1_INTV_VALUE_REG;                   /*!< Offset 0x024 Timer1 Interval Value Register */
	__IO uint32_t TMR1_CUR_VALUE_REG;                    /*!< Offset 0x028 Timer1 Current Value Register */
	uint32_t reserved_0x02C [0x001D];
	__IO uint32_t WDOG_IRQ_EN_REG;                       /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
	__IO uint32_t WDOG_IRQ_STA_REG;                      /*!< Offset 0x0A4 Watchdog Status Register */
	__IO uint32_t WDOG_SOFT_RST_REG;                     /*!< Offset 0x0A8 Watchdog Software Reset Register */
	uint32_t reserved_0x0AC;
	__IO uint32_t WDOG_CTRL_REG;                         /*!< Offset 0x0B0 Watchdog Control Register */
	__IO uint32_t WDOG_CFG_REG;                          /*!< Offset 0x0B4 Watchdog Configuration Register */
	__IO uint32_t WDOG_MODE_REG;                         /*!< Offset 0x0B8 Watchdog Mode Register */
	__IO uint32_t WDOG_OUTPUT_CFG_REG;                   /*!< Offset 0x0BC Watchdog Output Configuration Register */
	__IO uint32_t AVS_CNT_CTL_REG;                       /*!< Offset 0x0C0 AVS Control Register */
	__IO uint32_t AVS_CNT0_REG;                          /*!< Offset 0x0C4 AVS Counter 0 Register */
	__IO uint32_t AVS_CNT1_REG;                          /*!< Offset 0x0C8 AVS Counter 1 Register */
	__IO uint32_t AVS_CNT_DIV_REG;                       /*!< Offset 0x0CC AVS Divisor Register */
} TIMER_TypeDef; /* size of structure = 0x0D0 */
/*
 * @brief HSTIMER
 */
/*!< HSTIMER Controller Interface */
typedef struct HSTIMER_Type
{
	__IO uint32_t HS_TMR_IRQ_EN_REG;                     /*!< Offset 0x000 HS Timer IRQ Enable Register */
	__IO uint32_t HS_TMR_IRQ_STAS_REG;                   /*!< Offset 0x004 HS Timer Status Register */
	uint32_t reserved_0x008 [0x0006];
	__IO uint32_t HS_TMR0_CTRL_REG;                      /*!< Offset 0x020 HS Timer0 Control Register */
	__IO uint32_t HS_TMR0_INTV_LO_REG;                   /*!< Offset 0x024 HS Timer0 Interval Value Low Register */
	__IO uint32_t HS_TMR0_INTV_HI_REG;                   /*!< Offset 0x028 HS Timer0 Interval Value High Register */
	__IO uint32_t HS_TMR0_CURNT_LO_REG;                  /*!< Offset 0x02C HS Timer0 Current Value Low Register */
	__IO uint32_t HS_TMR0_CURNT_HI_REG;                  /*!< Offset 0x030 HS Timer0 Current Value High Register */
	uint32_t reserved_0x034 [0x0003];
	__IO uint32_t HS_TMR1_CTRL_REG;                      /*!< Offset 0x040 HS Timer1 Control Register */
	__IO uint32_t HS_TMR1_INTV_LO_REG;                   /*!< Offset 0x044 HS Timer1 Interval Value Low Register */
	__IO uint32_t HS_TMR1_INTV_HI_REG;                   /*!< Offset 0x048 HS Timer1 Interval Value High Register */
	__IO uint32_t HS_TMR1_CURNT_LO_REG;                  /*!< Offset 0x04C HS Timer1 Current Value Low Register */
	__IO uint32_t HS_TMR1_CURNT_HI_REG;                  /*!< Offset 0x050 HS Timer1 Current Value High Register */
} HSTIMER_TypeDef; /* size of structure = 0x054 */
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
	uint32_t reserved_0x030 [0x0004];
	__IO uint32_t CAN_TRBUF [0x00D];                     /*!< Offset 0x040 CAN TX/RX message buffer N (n=0..12) register */
	uint32_t reserved_0x074 [0x0043];
	__IO uint32_t CAN_RBUF_RBACK [0x030];                /*!< Offset 0x180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
	uint32_t reserved_0x240 [0x0030];
	__IO uint32_t CAN_VERSION;                           /*!< Offset 0x300 CAN Version Register */
} CAN_TypeDef; /* size of structure = 0x304 */
/*
 * @brief USBEHCI
 */
/*!< USBEHCI Controller Interface */
typedef struct USBEHCI_Type
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
	uint32_t reserved_0x02C [0x0009];
	__IO uint32_t E_CONFIGFLAG;                          /*!< Offset 0x050 EHCI Configured Flag Register */
	__IO uint32_t E_PORTSC;                              /*!< Offset 0x054 EHCI Port Status/Control Register */
	uint32_t reserved_0x058 [0x00EA];
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
} USBEHCI_TypeDef; /* size of structure = 0x458 */
/*
 * @brief USBOTGFIFO
 */
/*!< USBOTGFIFO Controller Interface */
typedef struct USBOTGFIFO_Type
{
	__IO uint16_t USB_TXFADDR;                           /*!< Offset 0x000 USB_TXFADDR */
	__IO uint8_t  USB_TXHADDR;                           /*!< Offset 0x002 USB_TXHADDR */
	__IO uint8_t  USB_TXHUBPORT;                         /*!< Offset 0x003 USB_TXHUBPORT */
	__IO uint8_t  USB_RXFADDR;                           /*!< Offset 0x004 USB_RXFADDR */
	uint8_t reserved_0x005 [0x0001];
	__IO uint8_t  USB_RXHADDR;                           /*!< Offset 0x006 USB_RXHADDR */
	__IO uint8_t  USB_RXHUBPORT;                         /*!< Offset 0x007 USB_RXHUBPORT */
} USBOTGFIFO_TypeDef; /* size of structure = 0x008 */
/*
 * @brief USBOTG
 */
/*!< USBOTG Controller Interface */
typedef struct USBOTG_Type
{
	__IO uint32_t USB_EPFIFO [0x006];                    /*!< Offset 0x000 USB_EPFIFO [0..5] */
	uint32_t reserved_0x018 [0x000A];
	__IO uint8_t  USB_POWER;                             /*!< Offset 0x040 USB_POWER */
	__IO uint8_t  USB_DEVCTL;                            /*!< Offset 0x041 USB_DEVCTL */
	__IO uint8_t  USB_EPINDEX;                           /*!< Offset 0x042 USB_EPINDEX */
	__IO uint8_t  USB_DMACTL;                            /*!< Offset 0x043 USB_DMACTL */
	__IO uint16_t USB_INTTX;                             /*!< Offset 0x044 USB_INTTX */
	__IO uint16_t USB_INTRX;                             /*!< Offset 0x046 USB_INTRX */
	__IO uint16_t USB_INTTXE;                            /*!< Offset 0x048 USB_INTTXE */
	__IO uint16_t USB_INTRXE;                            /*!< Offset 0x04A USB_INTRXE */
	__IO uint32_t USB_INTUSB;                            /*!< Offset 0x04C USB_INTUSB */
	__IO uint32_t USB_INTUSBE;                           /*!< Offset 0x050 USB_INTUSBE */
	__IO uint16_t USB_FRAME;                             /*!< Offset 0x054 USB_FRAME */
	uint8_t reserved_0x056 [0x0026];
	__IO uint8_t  USB_TESTMODE;                          /*!< Offset 0x07C USB_TESTMODE */
	uint8_t reserved_0x07D [0x0001];
	__IO uint8_t  USB_FSM;                               /*!< Offset 0x07E USB_FSM */
	uint8_t reserved_0x07F [0x0001];
	__IO uint16_t USB_TXMAXP;                            /*!< Offset 0x080 USB_TXMAXP */
	__IO uint16_t USB_CSR0;                              /*!< Offset 0x082 [15:8]: USB_TXCSRH, [7:0]: USB_TXCSRL */
	__IO uint16_t USB_RXMAXP;                            /*!< Offset 0x084 USB_RXMAXP */
	__IO uint16_t USB_RXCSR;                             /*!< Offset 0x086 USB_RXCSR */
	__IO uint16_t USB_RXCOUNT;                           /*!< Offset 0x088 USB_RXCOUNT */
	__IO uint16_t USB_RXPKTCNT;                          /*!< Offset 0x08A USB_RXPKTCNT */
	__IO uint8_t  USB_TXTI;                              /*!< Offset 0x08C USB_TXTI */
	__IO uint8_t  USB_TXNAKLIMIT;                        /*!< Offset 0x08D USB_TXNAKLIMIT */
	__IO uint8_t  USB_RXTI;                              /*!< Offset 0x08E USB_RXTI */
	__IO uint8_t  USB_RXNAKLIMIT;                        /*!< Offset 0x08F USB_RXNAKLIMIT */
	__IO uint16_t USB_TXFIFOSZ;                          /*!< Offset 0x090 USB_TXFIFOSZ */
	__IO uint16_t USB_TXFIFOADD;                         /*!< Offset 0x092 USB_TXFIFOADD */
	__IO uint16_t USB_RXFIFOSZ;                          /*!< Offset 0x094 USB_RXFIFOSZ */
	__IO uint16_t USB_RXFIFOADD;                         /*!< Offset 0x096 USB_RXFIFOADD */
	USBOTGFIFO_TypeDef FIFO [0x006];                     /*!< Offset 0x098 FIFOs [0..5] */
	uint32_t reserved_0x0C8 [0x00CE];
	__IO uint32_t USB_ISCR;                              /*!< Offset 0x400 HCI Interface Register (HCI_Interface) */
	__IO uint32_t USBPHY_PHYCTL;                         /*!< Offset 0x404 USBPHY_PHYCTL */
	__IO uint32_t HCI_CTRL3;                             /*!< Offset 0x408 HCI Control 3 Register (bist) */
	uint32_t reserved_0x40C;
	__IO uint32_t PHY_CTRL;                              /*!< Offset 0x410 PHY Control Register (PHY_Control) */
	uint32_t reserved_0x414 [0x0003];
	__IO uint32_t PHY_OTGCTL;                            /*!< Offset 0x420 Control PHY routing to EHCI or OTG */
	__IO uint32_t PHY_STATUS;                            /*!< Offset 0x424 PHY Status Register */
	__IO uint32_t USB_SPDCR;                             /*!< Offset 0x428 HCI SIE Port Disable Control Register */
} USBOTG_TypeDef; /* size of structure = 0x42C */
/*
 * @brief USBPHYC
 */
/*!< USBPHYC Controller Interface */
typedef struct USBPHYC_Type
{
	__IO uint32_t USB_CTRL;                              /*!< Offset 0x000 HCI Interface Register (HCI_Interface) */
	__IO uint32_t USBPHY_PHYCTL;                         /*!< Offset 0x004 USBPHY_PHYCTL */
	__IO uint32_t HCI_CTRL3;                             /*!< Offset 0x008 HCI Control 3 Register (bist) */
	uint32_t reserved_0x00C;
	__IO uint32_t PHY_CTRL;                              /*!< Offset 0x010 PHY Control Register (PHY_Control) */
	uint32_t reserved_0x014 [0x0003];
	__IO uint32_t PHY_OTGCTL;                            /*!< Offset 0x020 Control PHY routing to EHCI or OTG */
	__IO uint32_t PHY_STATUS;                            /*!< Offset 0x024 PHY Status Register */
	__IO uint32_t USB_SPDCR;                             /*!< Offset 0x028 HCI SIE Port Disable Control Register */
} USBPHYC_TypeDef; /* size of structure = 0x02C */
/*
 * @brief G2D_TOP
 */
/*!< G2D_TOP Controller Interface */
typedef struct G2D_TOP_Type
{
	__IO uint32_t G2D_SCLK_GATE;                         /*!< Offset 0x000 G2D SCLK gate   */
	__IO uint32_t G2D_HCLK_GATE;                         /*!< Offset 0x004 g2d HCLK gate   */
	__IO uint32_t G2D_AHB_RESET;                         /*!< Offset 0x008 G2D AHB reset   */
	__IO uint32_t G2D_SCLK_DIV;                          /*!< Offset 0x00C G2D SCLK div    */
} G2D_TOP_TypeDef; /* size of structure = 0x010 */
/*
 * @brief G2D_MIXER
 */
/*!< G2D_MIXER Controller Interface */
typedef struct G2D_MIXER_Type
{
	__IO uint32_t G2D_MIXER_CTL;                         /*!< Offset 0x000 G2D mixer control */
	__IO uint32_t G2D_MIXER_INT;                         /*!< Offset 0x004 G2D mixer interrupt */
	__IO uint32_t G2D_MIXER_CLK;                         /*!< Offset 0x008 G2D mixer clock */
} G2D_MIXER_TypeDef; /* size of structure = 0x00C */
/*
 * @brief G2D_LAY
 */
/*!< G2D_LAY Controller Interface */
typedef struct G2D_LAY_Type
{
	__IO uint32_t V0_ATTCTL;                             /*!< Offset 0x000 V0_ATTCTL */
	__IO uint32_t V0_MBSIZE;                             /*!< Offset 0x004 V0_MBSIZE */
	__IO uint32_t V0_COOR;                               /*!< Offset 0x008 V0_COOR */
	__IO uint32_t V0_PITCH0;                             /*!< Offset 0x00C V0_PITCH0 */
	__IO uint32_t V0_PITCH1;                             /*!< Offset 0x010 V0_PITCH1 */
	__IO uint32_t V0_PITCH2;                             /*!< Offset 0x014 V0_PITCH2 */
	__IO uint32_t V0_LADD0;                              /*!< Offset 0x018 V0_LADD0 */
	__IO uint32_t V0_LADD1;                              /*!< Offset 0x01C V0_LADD1 */
	__IO uint32_t V0_LADD2;                              /*!< Offset 0x020 V0_LADD2 */
	__IO uint32_t V0_FILLC;                              /*!< Offset 0x024 V0_FILLC */
	__IO uint32_t V0_HADD;                               /*!< Offset 0x028 V0_HADD */
	__IO uint32_t V0_SIZE;                               /*!< Offset 0x02C V0_SIZE */
	__IO uint32_t V0_HDS_CTL0;                           /*!< Offset 0x030 V0_HDS_CTL0 */
	__IO uint32_t V0_HDS_CTL1;                           /*!< Offset 0x034 V0_HDS_CTL1 */
	__IO uint32_t V0_VDS_CTL0;                           /*!< Offset 0x038 V0_VDS_CTL0 */
	__IO uint32_t V0_VDS_CTL1;                           /*!< Offset 0x03C V0_VDS_CTL1 */
} G2D_LAY_TypeDef; /* size of structure = 0x040 */
/*
 * @brief G2D_UI
 */
/*!< G2D_UI Controller Interface */
typedef struct G2D_UI_Type
{
	__IO uint32_t UI_ATTR;                               /*!< Offset 0x000 UIx_ATTR */
	__IO uint32_t UI_MBSIZE;                             /*!< Offset 0x004 UIx_MBSIZE */
	__IO uint32_t UI_COOR;                               /*!< Offset 0x008 UIx_COOR */
	__IO uint32_t UI_PITCH;                              /*!< Offset 0x00C UIx_PITCH */
	__IO uint32_t UI_LADD;                               /*!< Offset 0x010 UIx_LADD */
	__IO uint32_t UI_FILLC;                              /*!< Offset 0x014 UIx_FILLC */
	__IO uint32_t UI_HADD;                               /*!< Offset 0x018 UIx_HADD */
	__IO uint32_t UI_SIZE;                               /*!< Offset 0x01C UIx_SIZE */
} G2D_UI_TypeDef; /* size of structure = 0x020 */
/*
 * @brief G2D_VSU
 */
/*!< G2D_VSU Controller Interface */
typedef struct G2D_VSU_Type
{
	__IO uint32_t VS_CTRL;                               /*!< Offset 0x000 VS_CTRL */
	uint32_t reserved_0x004 [0x000F];
	__IO uint32_t VS_OUT_SIZE;                           /*!< Offset 0x040 VS_OUT_SIZE */
	__IO uint32_t VS_GLB_ALPHA;                          /*!< Offset 0x044 VS_GLB_ALPHA */
	uint32_t reserved_0x048 [0x000E];
	__IO uint32_t VS_Y_SIZE;                             /*!< Offset 0x080 VS_Y_SIZE */
	uint32_t reserved_0x084;
	__IO uint32_t VS_Y_HSTEP;                            /*!< Offset 0x088 VS_Y_HSTEP */
	__IO uint32_t VS_Y_VSTEP;                            /*!< Offset 0x08C VS_Y_VSTEP */
	__IO uint32_t VS_Y_HPHASE;                           /*!< Offset 0x090 VS_Y_HPHASE */
	uint32_t reserved_0x094;
	__IO uint32_t VS_Y_VPHASE0;                          /*!< Offset 0x098 VS_Y_VPHASE0 */
	uint32_t reserved_0x09C [0x0009];
	__IO uint32_t VS_C_SIZE;                             /*!< Offset 0x0C0 VS_C_SIZE */
	uint32_t reserved_0x0C4;
	__IO uint32_t VS_C_HSTEP;                            /*!< Offset 0x0C8 VS_C_HSTEP */
	__IO uint32_t VS_C_VSTEP;                            /*!< Offset 0x0CC VS_C_VSTEP */
	__IO uint32_t VS_C_HPHASE;                           /*!< Offset 0x0D0 VS_C_HPHASE */
	uint32_t reserved_0x0D4;
	__IO uint32_t VS_C_VPHASE0;                          /*!< Offset 0x0D8 VS_C_VPHASE0 */
	uint32_t reserved_0x0DC [0x0049];
	__IO uint32_t VS_Y_HCOEF [0x020];                    /*!< Offset 0x200 VS_Y_HCOEF[N]. N=0..31 */
	uint32_t reserved_0x280 [0x0020];
	__IO uint32_t VS_Y_VCOEF [0x020];                    /*!< Offset 0x300 VS_Y_VCOEF[N]. N=0..31 */
	uint32_t reserved_0x380 [0x0020];
	__IO uint32_t VS_C_HCOEF [0x020];                    /*!< Offset 0x400 VS_C_HCOEF[N]. N=0..31 */
} G2D_VSU_TypeDef; /* size of structure = 0x480 */
/*
 * @brief G2D_BLD
 */
/*!< G2D_BLD Controller Interface */
typedef struct G2D_BLD_Type
{
	__IO uint32_t BLD_EN_CTL;                            /*!< Offset 0x000 BLD_EN_CTL          */
	uint32_t reserved_0x004 [0x0003];
	__IO uint32_t BLD_FILLC0;                            /*!< Offset 0x010 BLD_FILLC0          */
	__IO uint32_t BLD_FILLC1;                            /*!< Offset 0x014 BLD_FILLC1          */
	uint32_t reserved_0x018 [0x0002];
	__IO uint32_t BLD_CH_ISIZE0;                         /*!< Offset 0x020 BLD_CH_ISIZE0       */
	__IO uint32_t BLD_CH_ISIZE1;                         /*!< Offset 0x024 BLD_CH_ISIZE1       */
	uint32_t reserved_0x028 [0x0002];
	__IO uint32_t BLD_CH_OFFSET0;                        /*!< Offset 0x030 BLD_CH_OFFSET0      */
	__IO uint32_t BLD_CH_OFFSET1;                        /*!< Offset 0x034 BLD_CH_OFFSET1      */
	uint32_t reserved_0x038 [0x0002];
	__IO uint32_t BLD_PREMUL_CTL;                        /*!< Offset 0x040 BLD_PREMUL_CTL      */
	__IO uint32_t BLD_BK_COLOR;                          /*!< Offset 0x044 BLD_BK_COLOR        */
	__IO uint32_t BLD_SIZE;                              /*!< Offset 0x048 BLD_SIZE            */
	__IO uint32_t BLD_CTL;                               /*!< Offset 0x04C BLD_CTL             */
	__IO uint32_t BLD_KEY_CTL;                           /*!< Offset 0x050 BLD_KEY_CTL         */
	__IO uint32_t BLD_KEY_CON;                           /*!< Offset 0x054 BLD_KEY_CON         */
	__IO uint32_t BLD_KEY_MAX;                           /*!< Offset 0x058 BLD_KEY_MAX         */
	__IO uint32_t BLD_KEY_MIN;                           /*!< Offset 0x05C BLD_KEY_MIN         */
	__IO uint32_t BLD_OUT_COLOR;                         /*!< Offset 0x060 BLD_OUT_COLOR       */
	uint32_t reserved_0x064 [0x0007];
	__IO uint32_t ROP_CTL;                               /*!< Offset 0x080 ROP_CTL             */
	__IO uint32_t ROP_INDEX0;                            /*!< Offset 0x084 ROP_INDEX0          */
	__IO uint32_t ROP_INDEX1;                            /*!< Offset 0x088 ROP_INDEX1          */
	uint32_t reserved_0x08C [0x001D];
	__IO uint32_t BLD_CSC_CTL;                           /*!< Offset 0x100 BLD_CSC_CTL         */
	uint32_t reserved_0x104 [0x0003];
	__IO uint32_t BLD_CSC0_COEF00;                       /*!< Offset 0x110 BLD_CSC0_COEF00     */
	__IO uint32_t BLD_CSC0_COEF01;                       /*!< Offset 0x114 BLD_CSC0_COEF01     */
	__IO uint32_t BLD_CSC0_COEF02;                       /*!< Offset 0x118 BLD_CSC0_COEF02     */
	__IO uint32_t BLD_CSC0_CONST0;                       /*!< Offset 0x11C BLD_CSC0_CONST0     */
	__IO uint32_t BLD_CSC0_COEF10;                       /*!< Offset 0x120 BLD_CSC0_COEF10     */
	__IO uint32_t BLD_CSC0_COEF11;                       /*!< Offset 0x124 BLD_CSC0_COEF11     */
	__IO uint32_t BLD_CSC0_COEF12;                       /*!< Offset 0x128 BLD_CSC0_COEF12     */
	__IO uint32_t BLD_CSC0_CONST1;                       /*!< Offset 0x12C BLD_CSC0_CONST1     */
	__IO uint32_t BLD_CSC0_COEF20;                       /*!< Offset 0x130 BLD_CSC0_COEF20     */
	__IO uint32_t BLD_CSC0_COEF21;                       /*!< Offset 0x134 BLD_CSC0_COEF21     */
	__IO uint32_t BLD_CSC0_COEF22;                       /*!< Offset 0x138 BLD_CSC0_COEF22     */
	__IO uint32_t BLD_CSC0_CONST2;                       /*!< Offset 0x13C BLD_CSC0_CONST2     */
	__IO uint32_t BLD_CSC1_COEF00;                       /*!< Offset 0x140 BLD_CSC1_COEF00     */
	__IO uint32_t BLD_CSC1_COEF01;                       /*!< Offset 0x144 BLD_CSC1_COEF01     */
	__IO uint32_t BLD_CSC1_COEF02;                       /*!< Offset 0x148 BLD_CSC1_COEF02     */
	__IO uint32_t BLD_CSC1_CONST0;                       /*!< Offset 0x14C BLD_CSC1_CONST0     */
	__IO uint32_t BLD_CSC1_COEF10;                       /*!< Offset 0x150 BLD_CSC1_COEF10     */
	__IO uint32_t BLD_CSC1_COEF11;                       /*!< Offset 0x154 BLD_CSC1_COEF11     */
	__IO uint32_t BLD_CSC1_COEF12;                       /*!< Offset 0x158 BLD_CSC1_COEF12     */
	__IO uint32_t BLD_CSC1_CONST1;                       /*!< Offset 0x15C BLD_CSC1_CONST1     */
	__IO uint32_t BLD_CSC1_COEF20;                       /*!< Offset 0x160 BLD_CSC1_COEF20     */
	__IO uint32_t BLD_CSC1_COEF21;                       /*!< Offset 0x164 BLD_CSC1_COEF21     */
	__IO uint32_t BLD_CSC1_COEF22;                       /*!< Offset 0x168 BLD_CSC1_COEF22     */
	__IO uint32_t BLD_CSC1_CONST2;                       /*!< Offset 0x16C BLD_CSC1_CONST2     */
	__IO uint32_t BLD_CSC2_COEF00;                       /*!< Offset 0x170 BLD_CSC2_COEF00     */
	__IO uint32_t BLD_CSC2_COEF01;                       /*!< Offset 0x174 BLD_CSC2_COEF01     */
	__IO uint32_t BLD_CSC2_COEF02;                       /*!< Offset 0x178 BLD_CSC2_COEF02     */
	__IO uint32_t BLD_CSC2_CONST0;                       /*!< Offset 0x17C BLD_CSC2_CONST0     */
	__IO uint32_t BLD_CSC2_COEF10;                       /*!< Offset 0x180 BLD_CSC2_COEF10     */
	__IO uint32_t BLD_CSC2_COEF11;                       /*!< Offset 0x184 BLD_CSC2_COEF11     */
	__IO uint32_t BLD_CSC2_COEF12;                       /*!< Offset 0x188 BLD_CSC2_COEF12     */
	__IO uint32_t BLD_CSC2_CONST1;                       /*!< Offset 0x18C BLD_CSC2_CONST1     */
	__IO uint32_t BLD_CSC2_COEF20;                       /*!< Offset 0x190 BLD_CSC2_COEF20     */
	__IO uint32_t BLD_CSC2_COEF21;                       /*!< Offset 0x194 BLD_CSC2_COEF21     */
	__IO uint32_t BLD_CSC2_COEF22;                       /*!< Offset 0x198 BLD_CSC2_COEF22     */
	__IO uint32_t BLD_CSC2_CONST2;                       /*!< Offset 0x19C BLD_CSC2_CONST2     */
} G2D_BLD_TypeDef; /* size of structure = 0x1A0 */
/*
 * @brief G2D_WB
 */
/*!< G2D_WB Controller Interface */
typedef struct G2D_WB_Type
{
	__IO uint32_t WB_ATT;                                /*!< Offset 0x000 WB_ATT */
	__IO uint32_t WB_SIZE;                               /*!< Offset 0x004 WB_SIZE */
	__IO uint32_t WB_PITCH0;                             /*!< Offset 0x008 WB_PITCH0 */
	__IO uint32_t WB_PITCH1;                             /*!< Offset 0x00C WB_PITCH1 */
	__IO uint32_t WB_PITCH2;                             /*!< Offset 0x010 WB_PITCH2 */
	__IO uint32_t WB_LADD0;                              /*!< Offset 0x014 WB_LADD0 */
	__IO uint32_t WB_HADD0;                              /*!< Offset 0x018 WB_HADD0 */
	__IO uint32_t WB_LADD1;                              /*!< Offset 0x01C WB_LADD1 */
	__IO uint32_t WB_HADD1;                              /*!< Offset 0x020 WB_HADD1 */
	__IO uint32_t WB_LADD2;                              /*!< Offset 0x024 WB_LADD2 */
	__IO uint32_t WB_HADD2;                              /*!< Offset 0x028 WB_HADD2 */
} G2D_WB_TypeDef; /* size of structure = 0x02C */
/*
 * @brief G2D_ROT
 */
/*!< G2D_ROT Controller Interface */
typedef struct G2D_ROT_Type
{
	__IO uint32_t ROT_CTL;                               /*!< Offset 0x000 ROT_CTL */
	__IO uint32_t ROT_INT;                               /*!< Offset 0x004 ROT_INT */
	__IO uint32_t ROT_TIMEOUT;                           /*!< Offset 0x008 ROT_TIMEOUT */
	uint32_t reserved_0x00C [0x0005];
	__IO uint32_t ROT_IFMT;                              /*!< Offset 0x020 ROT_IFMT */
	__IO uint32_t ROT_ISIZE;                             /*!< Offset 0x024 ROT_ISIZE */
	uint32_t reserved_0x028 [0x0002];
	__IO uint32_t ROT_IPITCH0;                           /*!< Offset 0x030 ROT_IPITCH0 */
	__IO uint32_t ROT_IPITCH1;                           /*!< Offset 0x034 ROT_IPITCH1 */
	__IO uint32_t ROT_IPITCH2;                           /*!< Offset 0x038 ROT_IPITCH2 */
	uint32_t reserved_0x03C;
	__IO uint32_t ROT_ILADD0;                            /*!< Offset 0x040 ROT_ILADD0 */
	__IO uint32_t ROT_IHADD0;                            /*!< Offset 0x044 ROT_IHADD0 */
	__IO uint32_t ROT_ILADD1;                            /*!< Offset 0x048 ROT_ILADD1 */
	__IO uint32_t ROT_IHADD1;                            /*!< Offset 0x04C ROT_IHADD1 */
	__IO uint32_t ROT_ILADD2;                            /*!< Offset 0x050 ROT_ILADD2 */
	__IO uint32_t ROT_IHADD2;                            /*!< Offset 0x054 ROT_IHADD2 */
	uint32_t reserved_0x058 [0x000B];
	__IO uint32_t ROT_OSIZE;                             /*!< Offset 0x084 ROT_OSIZE */
	uint32_t reserved_0x088 [0x0002];
	__IO uint32_t ROT_OPITCH0;                           /*!< Offset 0x090 ROT_OPITCH0 */
	__IO uint32_t ROT_OPITCH1;                           /*!< Offset 0x094 ROT_OPITCH1 */
	__IO uint32_t ROT_OPITCH2;                           /*!< Offset 0x098 ROT_OPITCH2 */
	uint32_t reserved_0x09C;
	__IO uint32_t ROT_OLADD0;                            /*!< Offset 0x0A0 ROT_OLADD0 */
	__IO uint32_t ROT_OHADD0;                            /*!< Offset 0x0A4 ROT_OHADD0 */
	__IO uint32_t ROT_OLADD1;                            /*!< Offset 0x0A8 ROT_OLADD1 */
	__IO uint32_t ROT_OHADD1;                            /*!< Offset 0x0AC ROT_OHADD1 */
	__IO uint32_t ROT_OLADD2;                            /*!< Offset 0x0B0 ROT_OLADD2 */
	__IO uint32_t ROT_OHADD2;                            /*!< Offset 0x0B4 ROT_OHADD2 */
} G2D_ROT_TypeDef; /* size of structure = 0x0B8 */
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Controller Interface */
typedef struct TCON_LCD_Type
{
	__IO uint32_t LCD_GCTL_REG;                          /*!< Offset 0x000 LCD Global Control Register */
	__IO uint32_t LCD_GINT0_REG;                         /*!< Offset 0x004 LCD Global Interrupt Register0 */
	__IO uint32_t LCD_GINT1_REG;                         /*!< Offset 0x008 LCD Global Interrupt Register1 */
	uint32_t reserved_0x00C;
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
	uint32_t reserved_0x05C;
	__IO uint32_t LCD_CPU_IF_REG;                        /*!< Offset 0x060 LCD CPU Panel Interface Register */
	__IO uint32_t LCD_CPU_WR_REG;                        /*!< Offset 0x064 LCD CPU Panel Write Data Register */
	__IO uint32_t LCD_CPU_RD0_REG;                       /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
	__IO uint32_t LCD_CPU_RD1_REG;                       /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
	uint32_t reserved_0x070 [0x0005];
	__IO uint32_t LCD_LVDS_IF_REG;                       /*!< Offset 0x084 LCD LVDS Configure Register */
	__IO uint32_t LCD_IO_POL_REG;                        /*!< Offset 0x088 LCD IO Polarity Register */
	__IO uint32_t LCD_IO_TRI_REG;                        /*!< Offset 0x08C LCD IO Control Register */
	uint32_t reserved_0x090 [0x001B];
	__IO uint32_t LCD_DEBUG_REG;                         /*!< Offset 0x0FC LCD Debug Register */
	__IO uint32_t LCD_CEU_CTL_REG;                       /*!< Offset 0x100 LCD CEU Control Register */
	uint32_t reserved_0x104 [0x0003];
	__IO uint32_t LCD_CEU_COEF_MUL_REG [0x003];          /*!< Offset 0x110 LCD CEU Coefficient Register0 0x0110+N*0x04 (N=0..10) */
	__IO uint32_t LCD_CEU_COEF_ADD_REG [0x003];          /*!< Offset 0x11C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
	uint32_t reserved_0x128 [0x0006];
	__IO uint32_t LCD_CEU_COEF_RANG_REG [0x003];         /*!< Offset 0x140 LCD CEU Coefficient Register2 0x0140+N*0x04 (N=0,1,2) */
	uint32_t reserved_0x14C [0x0005];
	__IO uint32_t LCD_CPU_TRI0_REG;                      /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
	__IO uint32_t LCD_CPU_TRI1_REG;                      /*!< Offset 0x164 LCD CPU Panel Trigger Register1 */
	__IO uint32_t LCD_CPU_TRI2_REG;                      /*!< Offset 0x168 LCD CPU Panel Trigger Register2 */
	__IO uint32_t LCD_CPU_TRI3_REG;                      /*!< Offset 0x16C LCD CPU Panel Trigger Register3 */
	__IO uint32_t LCD_CPU_TRI4_REG;                      /*!< Offset 0x170 LCD CPU Panel Trigger Register4 */
	__IO uint32_t LCD_CPU_TRI5_REG;                      /*!< Offset 0x174 LCD CPU Panel Trigger Register5 */
	uint32_t reserved_0x178 [0x0002];
	__IO uint32_t LCD_CMAP_CTL_REG;                      /*!< Offset 0x180 LCD Color Map Control Register */
	uint32_t reserved_0x184 [0x0003];
	__IO uint32_t LCD_CMAP_ODD0_REG;                     /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
	__IO uint32_t LCD_CMAP_ODD1_REG;                     /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
	__IO uint32_t LCD_CMAP_EVEN0_REG;                    /*!< Offset 0x198 LCD Color Map Even Line Register0 */
	__IO uint32_t LCD_CMAP_EVEN1_REG;                    /*!< Offset 0x19C LCD Color Map Even Line Register1 */
	uint32_t reserved_0x1A0 [0x0014];
	__IO uint32_t LCD_SAFE_PERIOD_REG;                   /*!< Offset 0x1F0 LCD Safe Period Register */
	uint32_t reserved_0x1F4 [0x000B];
	__IO uint32_t LCD_LVDS0_ANA_REG;                     /*!< Offset 0x220 LCD LVDS Analog Register 0 */
	__IO uint32_t LCD_LVDS1_ANA_REG;                     /*!< Offset 0x224 LCD LVDS Analog Register 1 */
	uint32_t reserved_0x228 [0x0002];
	__IO uint32_t LCD_SYNC_CTL_REG;                      /*!< Offset 0x230 LCD Sync Control Register */
	__IO uint32_t LCD_SYNC_POS_REG;                      /*!< Offset 0x234 LCD Sync Position Register */
	__IO uint32_t LCD_SLAVE_STOP_POS_REG;                /*!< Offset 0x238 LCD Slave Stop Position Register */
	uint32_t reserved_0x23C [0x0071];
	__IO uint32_t LCD_GAMMA_TABLE_REG [0x100];           /*!< Offset 0x400 LCD Gamma Table Register */
} TCON_LCD_TypeDef; /* size of structure = 0x800 */
/*
 * @brief TVE_TOP
 */
/*!< TVE_TOP Controller Interface */
typedef struct TVE_TOP_Type
{
	uint32_t reserved_0x000 [0x0008];
	__IO uint32_t TVE_DAC_MAP;                           /*!< Offset 0x020 TV Encoder DAC MAP Register */
	__IO uint32_t TVE_DAC_STATUS;                        /*!< Offset 0x024 TV Encoder DAC STAUTS Register */
	__IO uint32_t TVE_DAC_CFG0;                          /*!< Offset 0x028 TV Encoder DAC CFG0 Register */
	__IO uint32_t TVE_DAC_CFG1;                          /*!< Offset 0x02C TV Encoder DAC CFG1 Register */
	__IO uint32_t TVE_DAC_CFG2;                          /*!< Offset 0x030 TV Encoder DAC CFG2 Register */
	__IO uint32_t TVE_DAC_CFG3;                          /*!< Offset 0x034 TV Encoder DAC CFG2 Register */
	uint32_t reserved_0x038 [0x002E];
	__IO uint32_t TVE_DAC_TEST;                          /*!< Offset 0x0F0 TV Encoder DAC TEST Register */
} TVE_TOP_TypeDef; /* size of structure = 0x0F4 */
/*
 * @brief TVE
 */
/*!< TVE Controller Interface */
typedef struct TVE_Type
{
	__IO uint32_t TVE_000_REG;                           /*!< Offset 0x000 TV Encoder Clock Gating Register */
	__IO uint32_t TVE_004_REG;                           /*!< Offset 0x004 TV Encoder Configuration Register */
	__IO uint32_t TVE_008_REG;                           /*!< Offset 0x008 TV Encoder DAC Register1 */
	__IO uint32_t TVE_00C_REG;                           /*!< Offset 0x00C TV Encoder Notch and DAC Delay Register */
	__IO uint32_t TVE_010_REG;                           /*!< Offset 0x010 TV Encoder Chroma Frequency Register */
	__IO uint32_t TVE_014_REG;                           /*!< Offset 0x014 TV Encoder Front/Back Porch Register */
	__IO uint32_t TVE_018_REG;                           /*!< Offset 0x018 TV Encoder HD Mode VSYNC Register */
	__IO uint32_t TVE_01C_REG;                           /*!< Offset 0x01C TV Encoder Line Number Register */
	__IO uint32_t TVE_020_REG;                           /*!< Offset 0x020 TV Encoder Level Register */
	__IO uint32_t TVE_024_REG;                           /*!< Offset 0x024 TV Encoder DAC Register2 */
	uint32_t reserved_0x028 [0x0002];
	__IO uint32_t TVE_030_REG;                           /*!< Offset 0x030 TV Encoder Auto Detection Enable Register */
	__IO uint32_t TVE_034_REG;                           /*!< Offset 0x034 TV Encoder Auto Detection Interrupt Status Register */
	__IO uint32_t TVE_038_REG;                           /*!< Offset 0x038 TV Encoder Auto Detection Status Register */
	__IO uint32_t TVE_03C_REG;                           /*!< Offset 0x03C TV Encoder Auto Detection De-bounce Setting Register */
	uint32_t reserved_0x040 [0x002E];
	__IO uint32_t TVE_0F8_REG;                           /*!< Offset 0x0F8 TV Encoder Auto Detect Configuration Register0 */
	__IO uint32_t TVE_0FC_REG;                           /*!< Offset 0x0FC TV Encoder Auto Detect Configuration Register1 */
	__IO uint32_t TVE_100_REG;                           /*!< Offset 0x100 TV Encoder Color Burst Phase Reset Configuration Register */
	__IO uint32_t TVE_104_REG;                           /*!< Offset 0x104 TV Encoder VSYNC Number Register */
	__IO uint32_t TVE_108_REG;                           /*!< Offset 0x108 TV Encoder Notch Filter Frequency Register */
	__IO uint32_t TVE_10C_REG;                           /*!< Offset 0x10C TV Encoder Cb/Cr Level/Gain Register */
	__IO uint32_t TVE_110_REG;                           /*!< Offset 0x110 TV Encoder Tint and Color Burst Phase Register */
	__IO uint32_t TVE_114_REG;                           /*!< Offset 0x114 TV Encoder Burst Width Register */
	__IO uint32_t TVE_118_REG;                           /*!< Offset 0x118 TV Encoder Cb/Cr Gain Register */
	__IO uint32_t TVE_11C_REG;                           /*!< Offset 0x11C TV Encoder Sync and VBI Level Register */
	__IO uint32_t TVE_120_REG;                           /*!< Offset 0x120 TV Encoder White Level Register */
	__IO uint32_t TVE_124_REG;                           /*!< Offset 0x124 TV Encoder Video Active Line Register */
	__IO uint32_t TVE_128_REG;                           /*!< Offset 0x128 TV Encoder Video Chroma BW and CompGain Register */
	__IO uint32_t TVE_12C_REG;                           /*!< Offset 0x12C TV Encoder Register */
	__IO uint32_t TVE_130_REG;                           /*!< Offset 0x130 TV Encoder Re-sync Parameters Register */
	__IO uint32_t TVE_134_REG;                           /*!< Offset 0x134 TV Encoder Slave Parameter Register */
	__IO uint32_t TVE_138_REG;                           /*!< Offset 0x138 TV Encoder Configuration Register0 */
	__IO uint32_t TVE_13C_REG;                           /*!< Offset 0x13C TV Encoder Configuration Register1 */
	uint32_t reserved_0x140 [0x0090];
	__IO uint32_t TVE_380_REG;                           /*!< Offset 0x380 TV Encoder Low Pass Control Register */
	__IO uint32_t TVE_384_REG;                           /*!< Offset 0x384 TV Encoder Low Pass Filter Control Register */
	__IO uint32_t TVE_388_REG;                           /*!< Offset 0x388 TV Encoder Low Pass Gain Register */
	__IO uint32_t TVE_38C_REG;                           /*!< Offset 0x38C TV Encoder Low Pass Gain Control Register */
	__IO uint32_t TVE_390_REG;                           /*!< Offset 0x390 TV Encoder Low Pass Shoot Control Register */
	__IO uint32_t TVE_394_REG;                           /*!< Offset 0x394 TV Encoder Low Pass Coring Register */
	uint32_t reserved_0x398 [0x0002];
	__IO uint32_t TVE_3A0_REG;                           /*!< Offset 0x3A0 TV Encoder Noise Reduction Register */
} TVE_TypeDef; /* size of structure = 0x3A4 */
/*
 * @brief CSIC_CCU
 */
/*!< CSIC_CCU Controller Interface */
typedef struct CSIC_CCU_Type
{
	__IO uint32_t CCU_CLK_MODE_REG;                      /*!< Offset 0x000 CCU Clock Mode Register */
	__IO uint32_t CCU_PARSER_CLK_EN_REG;                 /*!< Offset 0x004 CCU Parser Clock Enable Register */
	uint32_t reserved_0x008;
	__IO uint32_t CCU_POST0_CLK_EN_REG;                  /*!< Offset 0x00C CCU Post0 Clock Enable Register */
} CSIC_CCU_TypeDef; /* size of structure = 0x010 */
/*
 * @brief CSIC_TOP
 */
/*!< CSIC_TOP Controller Interface */
typedef struct CSIC_TOP_Type
{
	__IO uint32_t CSIC_TOP_EN_REG;                       /*!< Offset 0x000 CSIC TOP Enable Register */
	__IO uint32_t CSIC_PTN_GEN_EN_REG;                   /*!< Offset 0x004 CSIC Pattern Generation Enable Register */
	__IO uint32_t CSIC_PTN_CTRL_REG;                     /*!< Offset 0x008 CSIC Pattern Control Register */
	uint32_t reserved_0x00C [0x0005];
	__IO uint32_t CSIC_PTN_LEN_REG;                      /*!< Offset 0x020 CSIC Pattern Generation Length Register */
	__IO uint32_t CSIC_PTN_ADDR_REG;                     /*!< Offset 0x024 CSIC Pattern Generation Address Register */
	__IO uint32_t CSIC_PTN_ISP_SIZE_REG;                 /*!< Offset 0x028 CSIC Pattern ISP Size Register */
	uint32_t reserved_0x02C [0x001D];
	__IO uint32_t CSIC_DMA0_INPUT_SEL_REG;               /*!< Offset 0x0A0 CSIC DMA0 Input Select Register */
	__IO uint32_t CSIC_DMA1_INPUT_SEL_REG;               /*!< Offset 0x0A4 CSIC DMA1 Input Select Register */
	uint32_t reserved_0x0A8 [0x000D];
	__IO uint32_t CSIC_BIST_CS_REG;                      /*!< Offset 0x0DC CSIC BIST CS Register */
	__IO uint32_t CSIC_BIST_CONTROL_REG;                 /*!< Offset 0x0E0 CSIC BIST Control Register */
	__IO uint32_t CSIC_BIST_START_REG;                   /*!< Offset 0x0E4 CSIC BIST Start Register */
	__IO uint32_t CSIC_BIST_END_REG;                     /*!< Offset 0x0E8 CSIC BIST End Register */
	__IO uint32_t CSIC_BIST_DATA_MASK_REG;               /*!< Offset 0x0EC CSIC BIST Data Mask Register */
	__IO uint32_t CSIC_MBUS_REQ_MAX_REG;                 /*!< Offset 0x0F0 CSIC MBUS REQ MAX Register */
	uint32_t reserved_0x0F4 [0x0003];
	__IO uint32_t CSIC_MULF_MOD_REG;                     /*!< Offset 0x100 CSIC Multi-Frame Mode Register */
	__IO uint32_t CSIC_MULF_INT_REG;                     /*!< Offset 0x104 CSIC Multi-Frame Interrupt Register */
} CSIC_TOP_TypeDef; /* size of structure = 0x108 */
/*
 * @brief CSIC_PARSER
 */
/*!< CSIC_PARSER Controller Interface */
typedef struct CSIC_PARSER_Type
{
	__IO uint32_t PRS_EN_REG;                            /*!< Offset 0x000 Parser Enable Register */
	__IO uint32_t PRS_NCSIC_IF_CFG_REG;                  /*!< Offset 0x004 Parser NCSIC Interface Configuration Register */
	uint32_t reserved_0x008;
	__IO uint32_t PRS_CAP_REG;                           /*!< Offset 0x00C Parser Capture Register */
	__IO uint32_t CSIC_PRS_SIGNAL_STA_REG;               /*!< Offset 0x010 CSIC Parser Signal Status Register */
	__IO uint32_t CSIC_PRS_NCSIC_BT656_HEAD_CFG_REG;     /*!< Offset 0x014 CSIC Parser NCSIC BT656 Header Configuration Register */
	uint32_t reserved_0x018 [0x0003];
	__IO uint32_t PRS_C0_INFMT_REG;                      /*!< Offset 0x024 Parser Channel_0 Input Format Register */
	__IO uint32_t PRS_C0_OUTPUT_HSIZE_REG;               /*!< Offset 0x028 Parser Channel_0 Output Horizontal Size Register */
	__IO uint32_t PRS_C0_OUTPUT_VSIZE_REG;               /*!< Offset 0x02C Parser Channel_0 Output Vertical Size Register */
	__IO uint32_t PRS_C0_INPUT_PARA0_REG;                /*!< Offset 0x030 Parser Channel_0 Input Parameter0 Register */
	__IO uint32_t PRS_C0_INPUT_PARA1_REG;                /*!< Offset 0x034 Parser Channel_0 Input Parameter1 Register */
	__IO uint32_t PRS_C0_INPUT_PARA2_REG;                /*!< Offset 0x038 Parser Channel_0 Input Parameter2 Register */
	__IO uint32_t PRS_C0_INPUT_PARA3_REG;                /*!< Offset 0x03C Parser Channel_0 Input Parameter3 Register */
	__IO uint32_t PRS_C0_INT_EN_REG;                     /*!< Offset 0x040 Parser Channel_0 Interrupt Enable Register */
	__IO uint32_t PRS_C0_INT_STA_REG;                    /*!< Offset 0x044 Parser Channel_0 Interrupt Status Register */
	__IO uint32_t PRS_CH0_LINE_TIME_REG;                 /*!< Offset 0x048 Parser Channel_0 Line Time Register */
	uint32_t reserved_0x04C [0x0036];
	__IO uint32_t PRS_C1_INFMT_REG;                      /*!< Offset 0x124 Parser Channel_1 Input Format Register */
	__IO uint32_t PRS_C1_OUTPUT_HSIZE_REG;               /*!< Offset 0x128 Parser Channel_1 Output Horizontal Size Register */
	__IO uint32_t PRS_C1_OUTPUT_VSIZE_REG;               /*!< Offset 0x12C Parser Channel_1 Output Vertical Size Register */
	__IO uint32_t PRS_C1_INPUT_PARA0_REG;                /*!< Offset 0x130 Parser Channel_1 Input Parameter0 Register */
	__IO uint32_t PRS_C1_INPUT_PARA1_REG;                /*!< Offset 0x134 Parser Channel_1 Input Parameter1 Register */
	__IO uint32_t PRS_C1_INPUT_PARA2_REG;                /*!< Offset 0x138 Parser Channel_1 Input Parameter2 Register */
	__IO uint32_t PRS_C1_INPUT_PARA3_REG;                /*!< Offset 0x13C Parser Channel_1 Input Parameter3 Register */
	__IO uint32_t PRS_C1_INT_EN_REG;                     /*!< Offset 0x140 Parser Channel_1 Interrupt Enable Register */
	__IO uint32_t PRS_C1_INT_STA_REG;                    /*!< Offset 0x144 Parser Channel_1 Interrupt Status Register */
	__IO uint32_t PRS_CH1_LINE_TIME_REG;                 /*!< Offset 0x148 Parser Channel_1 Line Time Register */
	uint32_t reserved_0x14C [0x0036];
	__IO uint32_t PRS_C2_INFMT_REG;                      /*!< Offset 0x224 Parser Channel_2 Input Format Register */
	__IO uint32_t PRS_C2_OUTPUT_HSIZE_REG;               /*!< Offset 0x228 Parser Channel_2 Output Horizontal Size Register */
	__IO uint32_t PRS_C2_OUTPUT_VSIZE_REG;               /*!< Offset 0x22C Parser Channel_2 Output Vertical Size Register */
	__IO uint32_t PRS_C2_INPUT_PARA0_REG;                /*!< Offset 0x230 Parser Channel_2 Input Parameter0 Register */
	__IO uint32_t PRS_C2_INPUT_PARA1_REG;                /*!< Offset 0x234 Parser Channel_2 Input Parameter1 Register */
	__IO uint32_t PRS_C2_INPUT_PARA2_REG;                /*!< Offset 0x238 Parser Channel_2 Input Parameter2 Register */
	__IO uint32_t PRS_C2_INPUT_PARA3_REG;                /*!< Offset 0x23C Parser Channel_2 Input Parameter3 Register */
	__IO uint32_t PRS_C2_INT_EN_REG;                     /*!< Offset 0x240 Parser Channel_2 Interrupt Enable Register */
	__IO uint32_t PRS_C2_INT_STA_REG;                    /*!< Offset 0x244 Parser Channel_2 Interrupt Status Register */
	__IO uint32_t PRS_CH2_LINE_TIME_REG;                 /*!< Offset 0x248 Parser Channel_2 Line Time Register */
	uint32_t reserved_0x24C [0x0036];
	__IO uint32_t PRS_C3_INFMT_REG;                      /*!< Offset 0x324 Parser Channel_3 Input Format Register */
	__IO uint32_t PRS_C3_OUTPUT_HSIZE_REG;               /*!< Offset 0x328 Parser Channel_3 Output Horizontal Size Register */
	__IO uint32_t PRS_C3_OUTPUT_VSIZE_REG;               /*!< Offset 0x32C Parser Channel_3 Output Vertical Size Register */
	__IO uint32_t PRS_C3_INPUT_PARA0_REG;                /*!< Offset 0x330 Parser Channel_3 Input Parameter0 Register */
	__IO uint32_t PRS_C3_INPUT_PARA1_REG;                /*!< Offset 0x334 Parser Channel_3 Input Parameter1 Register */
	__IO uint32_t PRS_C3_INPUT_PARA2_REG;                /*!< Offset 0x338 Parser Channel_3 Input Parameter2 Register */
	__IO uint32_t PRS_C3_INPUT_PARA3_REG;                /*!< Offset 0x33C Parser Channel_3 Input Parameter3 Register */
	__IO uint32_t PRS_C3_INT_EN_REG;                     /*!< Offset 0x340 Parser Channel_3 Interrupt Enable Register */
	__IO uint32_t PRS_C3_INT_STA_REG;                    /*!< Offset 0x344 Parser Channel_3 Interrupt Status Register */
	__IO uint32_t PRS_CH3_LINE_TIME_REG;                 /*!< Offset 0x348 Parser Channel_3 Line Time Register */
	uint32_t reserved_0x34C [0x006D];
	__IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL0_DLY_ADJ_REG; /*!< Offset 0x500 CSIC Parser NCSIC RX Signal0 Delay Adjust Register */
	uint32_t reserved_0x504 [0x0004];
	__IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL5_DLY_ADJ_REG; /*!< Offset 0x514 CSIC Parser NCSIC RX Signal5 Delay Adjust Register */
	__IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL6_DLY_ADJ_REG; /*!< Offset 0x518 CSIC Parser NCSIC RX Signal6 Delay Adjust Register */
} CSIC_PARSER_TypeDef; /* size of structure = 0x51C */
/*
 * @brief CSIC_DMA
 */
/*!< CSIC_DMA Controller Interface */
typedef struct CSIC_DMA_Type
{
	__IO uint32_t CSIC_DMA_EN_REG;                       /*!< Offset 0x000 CSIC DMA Enable Register */
	__IO uint32_t CSIC_DMA_CFG_REG;                      /*!< Offset 0x004 CSIC DMA Configuration Register */
	uint32_t reserved_0x008 [0x0002];
	__IO uint32_t CSIC_DMA_HSIZE_REG;                    /*!< Offset 0x010 CSIC DMA Horizontal Size Register */
	__IO uint32_t CSIC_DMA_VSIZE_REG;                    /*!< Offset 0x014 CSIC DMA Vertical Size Register */
	uint32_t reserved_0x018 [0x0002];
	__IO uint32_t CSIC_DMA_F0_BUFA_REG;                  /*!< Offset 0x020 CSIC DMA FIFO 0 Output Buffer-A Address Register */
	__IO uint32_t CSIC_DMA_F0_BUFA_RESULT_REG;           /*!< Offset 0x024 CSIC DMA FIFO 0 Output Buffer-A Address Result Register */
	__IO uint32_t CSIC_DMA_F1_BUFA_REG;                  /*!< Offset 0x028 CSIC DMA FIFO 1 Output Buffer-A Address Register */
	__IO uint32_t CSIC_DMA_F1_BUFA_RESULT_REG;           /*!< Offset 0x02C CSIC DMA FIFO 1 Output Buffer-A Address Result Register */
	__IO uint32_t CSIC_DMA_F2_BUFA_REG;                  /*!< Offset 0x030 CSIC DMA FIFO 2 Output Buffer-A Address Register */
	__IO uint32_t CSIC_DMA_F2_BUFA_RESULT_REG;           /*!< Offset 0x034 CSIC DMA FIFO 2 Output Buffer-A Address Result Register */
	__IO uint32_t CSIC_DMA_BUF_LEN_REG;                  /*!< Offset 0x038 CSIC DMA Buffer Length Register */
	__IO uint32_t CSIC_DMA_FLIP_SIZE_REG;                /*!< Offset 0x03C CSIC DMA Flip Size Register */
	__IO uint32_t CSIC_DMA_VI_TO_TH0_REG;                /*!< Offset 0x040 CSIC DMA Video Input Timeout Threshold0 Register */
	__IO uint32_t CSIC_DMA_VI_TO_TH1_REG;                /*!< Offset 0x044 CSIC DMA Video Input Timeout Threshold1 Register */
	__IO uint32_t CSIC_DMA_VI_TO_CNT_VAL_REG;            /*!< Offset 0x048 CSIC DMA Video Input Timeout Counter Value Register */
	__IO uint32_t CSIC_DMA_CAP_STA_REG;                  /*!< Offset 0x04C CSIC DMA Capture Status Register */
	__IO uint32_t CSIC_DMA_INT_EN_REG;                   /*!< Offset 0x050 CSIC DMA Interrupt Enable Register */
	__IO uint32_t CSIC_DMA_INT_STA_REG;                  /*!< Offset 0x054 CSIC DMA Interrupt Status Register */
	__IO uint32_t CSIC_DMA_LINE_CNT_REG;                 /*!< Offset 0x058 CSIC DMA LINE Counter Register */
	__IO uint32_t CSIC_DMA_FRM_CNT_REG;                  /*!< Offset 0x05C CSIC DMA Frame Counter Register */
	__IO uint32_t CSIC_DMA_FRM_CLK_CNT_REG;              /*!< Offset 0x060 CSIC DMA Frame Clock Counter Register */
	__IO uint32_t CSIC_DMA_ACC_ITNL_CLK_CNT_REG;         /*!< Offset 0x064 CSIC DMA Accumulated And Internal Clock Counter Register */
	__IO uint32_t CSIC_DMA_FIFO_STAT_REG;                /*!< Offset 0x068 CSIC DMA FIFO Statistic Register */
	__IO uint32_t CSIC_DMA_FIFO_THRS_REG;                /*!< Offset 0x06C CSIC DMA FIFO Threshold Register */
	__IO uint32_t CSIC_DMA_PCLK_STAT_REG;                /*!< Offset 0x070 CSIC DMA PCLK Statistic Register */
	uint32_t reserved_0x074 [0x0003];
	__IO uint32_t CSIC_DMA_BUF_ADDR_FIFO0_ENTRY_REG;     /*!< Offset 0x080 CSIC DMA BUF Address FIFO0 Entry Register */
	__IO uint32_t CSIC_DMA_BUF_ADDR_FIFO1_ENTRY_REG;     /*!< Offset 0x084 CSIC DMA BUF Address FIFO1 Entry Register */
	__IO uint32_t CSIC_DMA_BUF_ADDR_FIFO2_ENTRY_REG;     /*!< Offset 0x088 CSIC DMA BUF Address FIFO2 Entry Register */
	__IO uint32_t CSIC_DMA_BUF_TH_REG;                   /*!< Offset 0x08C CSIC DMA BUF Threshold Register */
	__IO uint32_t CSIC_DMA_BUF_ADDR_FIFO_CON_REG;        /*!< Offset 0x090 CSIC DMA BUF Address FIFO Content Register */
	__IO uint32_t CSIC_DMA_STORED_FRM_CNT_REG;           /*!< Offset 0x094 CSIC DMA Stored Frame Counter Register */
	uint32_t reserved_0x098 [0x0057];
	__IO uint32_t CSIC_FEATURE_REG;                      /*!< Offset 0x1F4 CSIC DMA Feature List Register */
} CSIC_DMA_TypeDef; /* size of structure = 0x1F8 */
/*
 * @brief TVD_ADC
 */
/*!< TVD_ADC Controller Interface */
typedef struct TVD_ADC_Type
{
	uint32_t reserved_0x000;
	__IO uint32_t TVD_TOP_CTL;                           /*!< Offset 0x004 TVD TOP CONTROL Register */
	__IO uint32_t TVD_ADC_CTL;                           /*!< Offset 0x008 TVD ADC CONTROL Register */
	__IO uint32_t TVD_ADC_CFG;                           /*!< Offset 0x00C TVD ADC CONFIGURATION Register */
	uint32_t reserved_0x010 [0x0004];
} TVD_ADC_TypeDef; /* size of structure = 0x020 */
/*
 * @brief TVD_TOP
 */
/*!< TVD_TOP Controller Interface */
typedef struct TVD_TOP_Type
{
	__IO uint32_t TVD_TOP_MAP;                           /*!< Offset 0x000 TVD TOP MAP Register */
	uint32_t reserved_0x004;
	__IO uint32_t TVD_3D_CTL1;                           /*!< Offset 0x008 TVD 3D DMA CONTROL Register1 */
	__IO uint32_t TVD_3D_CTL2;                           /*!< Offset 0x00C TVD 3D DMA CONTROL Register2 */
	__IO uint32_t TVD_3D_CTL3;                           /*!< Offset 0x010 TVD 3D DMA CONTROL Register3 */
	__IO uint32_t TVD_3D_CTL4;                           /*!< Offset 0x014 TVD 3D DMA CONTROL Register4 */
	__IO uint32_t TVD_3D_CTL5;                           /*!< Offset 0x018 TVD 3D DMA CONTROL Register5 */
	uint32_t reserved_0x01C;
	TVD_ADC_TypeDef TVD_ADC [0x004];                     /*!< Offset 0x020 TVD ADC Registers N (N = 0 to 3) */
} TVD_TOP_TypeDef; /* size of structure = 0x0A0 */
/*
 * @brief TVD
 */
/*!< TVD Controller Interface */
typedef struct TVD_Type
{
	__IO uint32_t TVD_EN;                                /*!< Offset 0x000 TVD MODULE CONTROL Register */
	__IO uint32_t TVD_MODE;                              /*!< Offset 0x004 TVD MODE CONTROL Register */
	__IO uint32_t TVD_CLAMP_AGC1;                        /*!< Offset 0x008 TVD CLAMP & AGC CONTROL Register1 */
	__IO uint32_t TVD_CLAMP_AGC2;                        /*!< Offset 0x00C TVD CLAMP & AGC CONTROL Register2 */
	__IO uint32_t TVD_HLOCK1;                            /*!< Offset 0x010 TVD HLOCK CONTROL Register1 */
	__IO uint32_t TVD_HLOCK2;                            /*!< Offset 0x014 TVD HLOCK CONTROL Register2 */
	__IO uint32_t TVD_HLOCK3;                            /*!< Offset 0x018 TVD HLOCK CONTROL Register3 */
	__IO uint32_t TVD_HLOCK4;                            /*!< Offset 0x01C TVD HLOCK CONTROL Register4 */
	__IO uint32_t TVD_HLOCK5;                            /*!< Offset 0x020 TVD HLOCK CONTROL Register5 */
	__IO uint32_t TVD_VLOCK1;                            /*!< Offset 0x024 TVD VLOCK CONTROL Register1 */
	__IO uint32_t TVD_VLOCK2;                            /*!< Offset 0x028 TVD VLOCK CONTROL Register2 */
	uint32_t reserved_0x02C;
	__IO uint32_t TVD_CLOCK1;                            /*!< Offset 0x030 TVD CHROMA LOCK CONTROL Register1 */
	__IO uint32_t TVD_CLOCK2;                            /*!< Offset 0x034 TVD CHROMA LOCK CONTROL Register2 */
	uint32_t reserved_0x038 [0x0002];
	__IO uint32_t TVD_YC_SEP1;                           /*!< Offset 0x040 TVD YC SEPERATION CONROL Register1 */
	__IO uint32_t TVD_YC_SEP2;                           /*!< Offset 0x044 TVD YC SEPERATION CONROL Register2 */
	uint32_t reserved_0x048 [0x0002];
	__IO uint32_t TVD_ENHANCE1;                          /*!< Offset 0x050 TVD ENHANCEMENT CONTROL Register1 */
	__IO uint32_t TVD_ENHANCE2;                          /*!< Offset 0x054 TVD ENHANCEMENT CONTROL Register2 */
	__IO uint32_t TVD_ENHANCE3;                          /*!< Offset 0x058 TVD ENHANCEMENT CONTROL Register3 */
	uint32_t reserved_0x05C;
	__IO uint32_t TVD_WB1;                               /*!< Offset 0x060 TVD WB DMA CONTROL Register1 */
	__IO uint32_t TVD_WB2;                               /*!< Offset 0x064 TVD WB DMA CONTROL Register2 */
	__IO uint32_t TVD_WB3;                               /*!< Offset 0x068 TVD WB DMA CONTROL Register3 */
	__IO uint32_t TVD_WB4;                               /*!< Offset 0x06C TVD WB DMA CONTROL Register4 */
	uint32_t reserved_0x070 [0x0004];
	__IO uint32_t TVD_IRQ_CTL;                           /*!< Offset 0x080 TVD DMA Interrupt Control Register */
	uint32_t reserved_0x084 [0x0003];
	__IO uint32_t TVD_IRQ_STATUS;                        /*!< Offset 0x090 TVD DMA Interrupt Status Register */
	uint32_t reserved_0x094 [0x001B];
	__IO uint32_t TVD_DEBUG1;                            /*!< Offset 0x100 TVD DEBUG CONTROL Register1 */
	uint32_t reserved_0x104 [0x001F];
	__IO uint32_t TVD_STATUS1;                           /*!< Offset 0x180 TVD DEBUG STATUS Register1 */
	__IO uint32_t TVD_STATUS2;                           /*!< Offset 0x184 TVD DEBUG STATUS Register2 */
	__IO uint32_t TVD_STATUS3;                           /*!< Offset 0x188 TVD DEBUG STATUS Register3 */
	__IO uint32_t TVD_STATUS4;                           /*!< Offset 0x18C TVD DEBUG STATUS Register4 */
	__IO uint32_t TVD_STATUS5;                           /*!< Offset 0x190 TVD DEBUG STATUS Register5 */
	__IO uint32_t TVD_STATUS6;                           /*!< Offset 0x194 TVD DEBUG STATUS Register6 */
} TVD_TypeDef; /* size of structure = 0x198 */
/*
 * @brief C0_CPUX_CFG
 */
/*!< C0_CPUX_CFG Controller Interface */
typedef struct C0_CPUX_CFG_Type
{
	__IO uint32_t C0_RST_CTRL;                           /*!< Offset 0x000 Cluster 0 Reset Control Register */
	uint32_t reserved_0x004 [0x0003];
	__IO uint32_t C0_CTRL_REG0;                          /*!< Offset 0x010 Cluster 0 Control Register0 */
	__IO uint32_t C0_CTRL_REG1;                          /*!< Offset 0x014 Cluster 0 Control Register1 */
	__IO uint32_t C0_CTRL_REG2;                          /*!< Offset 0x018 Cluster 0 Control Register2 */
	uint32_t reserved_0x01C [0x0002];
	__IO uint32_t CACHE_CFG_REG;                         /*!< Offset 0x024 Cache Configuration Register */
	uint32_t reserved_0x028 [0x0016];
	__IO uint32_t C0_CPU_STATUS;                         /*!< Offset 0x080 Cluster 0 CPU Status Register */
	__IO uint32_t L2_STATUS_REG;                         /*!< Offset 0x084 Cluster 0 L2 Status Register */
	uint32_t reserved_0x088 [0x000E];
	__IO uint32_t DBG_REG0;                              /*!< Offset 0x0C0 Cluster 0 Debug Control Register0 */
	__IO uint32_t DBG_REG1;                              /*!< Offset 0x0C4 Cluster 0 Debug Control Register1 */
	uint32_t reserved_0x0C8 [0x0002];
	__IO uint32_t AXI_MNT_CTRL_REG;                      /*!< Offset 0x0D0 AXI Monitor Control Register */
	__IO uint32_t AXI_MNT_PRD_REG;                       /*!< Offset 0x0D4 AXI Monitor Period Register */
	__IO uint32_t AXI_MNT_RLTCY_REG;                     /*!< Offset 0x0D8 AXI Monitor Read Total Latency Register */
	__IO uint32_t AXI_MNT_WLTCY_REG;                     /*!< Offset 0x0DC AXI Monitor Write Total Latency Register */
	__IO uint32_t AXI_MNT_RREQ_REG;                      /*!< Offset 0x0E0 AXI Monitor Read Request Times Register */
	__IO uint32_t AXI_MNT_WREQ_REG;                      /*!< Offset 0x0E4 AXI Monitor Write Request Times Register */
	__IO uint32_t AXI_MNT_RBD_REG;                       /*!< Offset 0x0E8 AXI Monitor Read Bandwidth Register */
	__IO uint32_t AXI_MNT_WBD_REG;                       /*!< Offset 0x0EC AXI Monitor Write Bandwidth Register */
} C0_CPUX_CFG_TypeDef; /* size of structure = 0x0F0 */
/*
 * @brief DDRPHYC
 */
/*!< DDRPHYC Controller Interface */
typedef struct DDRPHYC_Type
{
	__IO uint32_t PHYC_REG_000;                          /*!< Offset 0x000 Reg_000 */
	__IO uint32_t PHYC_REG_004;                          /*!< Offset 0x004 Reg_004 */
	uint32_t reserved_0x008;
	__IO uint32_t PHYC_REG_00C;                          /*!< Offset 0x00C Reg_00C */
	__IO uint32_t PHYC_REG_010;                          /*!< Offset 0x010 Reg_010 */
	uint32_t reserved_0x014;
	__IO uint32_t PHYC_REG_018;                          /*!< Offset 0x018 Reg_018 */
	uint32_t reserved_0x01C [0x0004];
	__IO uint32_t PHYC_REG_02C;                          /*!< Offset 0x02C Reg_02C */
	__IO uint32_t PHYC_REG_030;                          /*!< Offset 0x030 Reg_030 */
	__IO uint32_t PHYC_REG_034;                          /*!< Offset 0x034 Reg_034 */
	__IO uint32_t PHYC_REG_038;                          /*!< Offset 0x038 Reg_038 */
	__IO uint32_t PHYC_REG_03C;                          /*!< Offset 0x03C Reg_03C */
	uint32_t reserved_0x040 [0x0004];
	__IO uint32_t PHYC_REG_050;                          /*!< Offset 0x050 Reg_050 */
	__IO uint32_t PHYC_REG_054;                          /*!< Offset 0x054 Reg_054 */
	__IO uint32_t PHYC_REG_058;                          /*!< Offset 0x058 Reg_058 */
	__IO uint32_t PHYC_REG_05C;                          /*!< Offset 0x05C Reg_05C */
	__IO uint32_t PHYC_REG_060;                          /*!< Offset 0x060 Reg_060 */
	__IO uint32_t PHYC_REG_064;                          /*!< Offset 0x064 Reg_064 */
	__IO uint32_t PHYC_REG_068;                          /*!< Offset 0x068 Reg_068 */
	__IO uint32_t PHYC_REG_06C;                          /*!< Offset 0x06C Reg_06C */
	uint32_t reserved_0x070 [0x0002];
	__IO uint32_t PHYC_REG_078;                          /*!< Offset 0x078 Reg_078 */
	__IO uint32_t PHYC_REG_07C;                          /*!< Offset 0x07C Reg_07c */
	__IO uint32_t PHYC_REG_080;                          /*!< Offset 0x080 Reg_080 */
	uint32_t reserved_0x084 [0x0002];
	__IO uint32_t PHYC_REG_08C;                          /*!< Offset 0x08C Reg_08C */
	__IO uint32_t PHYC_REG_090;                          /*!< Offset 0x090 Reg_090 */
	__IO uint32_t PHYC_REG_094;                          /*!< Offset 0x094 Reg_094 */
	uint32_t reserved_0x098;
	__IO uint32_t PHYC_REG_09C;                          /*!< Offset 0x09C Reg_09C */
	__IO uint32_t PHYC_REG_0A0;                          /*!< Offset 0x0A0 Reg_0a0 */
	uint32_t reserved_0x0A4 [0x0005];
	__IO uint32_t PHYC_REG_0B8;                          /*!< Offset 0x0B8 Reg_0B8 */
	__IO uint32_t PHYC_REG_0BC;                          /*!< Offset 0x0BC Reg_0BC */
	__IO uint32_t PHYC_REG_0C0;                          /*!< Offset 0x0C0 Reg_0C0 */
	uint32_t reserved_0x0C4 [0x000F];
	__IO uint32_t PHYC_REG_100;                          /*!< Offset 0x100 Reg_100 */
	uint32_t reserved_0x104;
	__IO uint32_t PHYC_REG_108;                          /*!< Offset 0x108 Reg_108 */
	__IO uint32_t PHYC_REG_10C;                          /*!< Offset 0x10C Reg_10C */
	__IO uint32_t PHYC_REG_110;                          /*!< Offset 0x110 Reg_110 */
	__IO uint32_t PHYC_REG_114;                          /*!< Offset 0x114 Reg_114 */
	uint32_t reserved_0x118;
	__IO uint32_t PHYC_REG_11C;                          /*!< Offset 0x11C Reg_11C */
	__IO uint32_t PHYC_REG_120;                          /*!< Offset 0x120 Reg_120 */
	uint32_t reserved_0x124 [0x0007];
	__IO uint32_t PHYC_REG_140;                          /*!< Offset 0x140 Reg_140 */
	uint32_t reserved_0x144 [0x0031];
	__IO uint32_t PHYC_REG_208;                          /*!< Offset 0x208 Reg_208 */
	uint32_t reserved_0x20C [0x0003];
	__IO uint32_t PHYC_REG_218;                          /*!< Offset 0x218 Reg_218 */
	__IO uint32_t PHYC_REG_21C;                          /*!< Offset 0x21C Reg_21C */
	uint32_t reserved_0x220 [0x0002];
	__IO uint32_t PHYC_REG_228 [0x006];                  /*!< Offset 0x228 Reg_228 */
	__IO uint32_t PHYC_REG_240 [0x010];                  /*!< Offset 0x240 Reg_240 */
	__IO uint32_t PHYC_REG_280;                          /*!< Offset 0x280 Reg_280 */
	uint32_t reserved_0x284 [0x0023];
	__IO uint32_t PHYC_REG_310 [0x009];                  /*!< Offset 0x310 Reg_310 Rank 0 */
	__IO uint32_t PHYC_REG_334;                          /*!< Offset 0x334 Reg_334 Rank 0 */
	__IO uint32_t PHYC_REG_338;                          /*!< Offset 0x338 Reg_338 Rank 0 */
	__IO uint32_t PHYC_REG_33C;                          /*!< Offset 0x33C Reg_33C Rank 0 */
	uint32_t reserved_0x340;
	__IO uint32_t PHYC_REG_344;                          /*!< Offset 0x344 Reg_344 Rank 0 */
	__IO uint32_t PHYC_REG_348;                          /*!< Offset 0x348 Reg_348 Rank 0 */
	uint32_t reserved_0x34C [0x0011];
	__IO uint32_t PHYC_REG_390 [0x009];                  /*!< Offset 0x390 Reg_390 */
	__IO uint32_t PHYC_REG_3B4;                          /*!< Offset 0x3B4 Reg_3B4 Rank 1 */
	__IO uint32_t PHYC_REG_3B8;                          /*!< Offset 0x3B8 Reg_3B8 Rank 1 */
	__IO uint32_t PHYC_REG_3BC;                          /*!< Offset 0x3BC Reg_3BC Rank 1 */
	uint32_t reserved_0x3C0;
	__IO uint32_t PHYC_REG_3C4;                          /*!< Offset 0x3C4 Reg_3C4 Rank 1 */
	__IO uint32_t PHYC_REG_3C8;                          /*!< Offset 0x3C8 Reg_3C8 Rank 1 */
} DDRPHYC_TypeDef; /* size of structure = 0x3CC */
/*
 * @brief MSI_MEMC
 */
/*!< MSI_MEMC Controller Interface */
typedef struct MSI_MEMC_Type
{
	__IO uint32_t MEMC_REG_000;                          /*!< Offset 0x000 Reg_000 */
	__IO uint32_t MEMC_REG_004;                          /*!< Offset 0x004 Reg_004 */
	__IO uint32_t MEMC_REG_008;                          /*!< Offset 0x008 Reg_008 */
	__IO uint32_t MEMC_REG_00C;                          /*!< Offset 0x00C Reg_00C */
	uint32_t reserved_0x010 [0x0004];
	__IO uint32_t MEMC_REG_020;                          /*!< Offset 0x020 Reg_020 */
	__IO uint32_t MEMC_REG_024;                          /*!< Offset 0x024 Reg_024 */
	__IO uint32_t MEMC_REG_028;                          /*!< Offset 0x028 Reg_028 */
} MSI_MEMC_TypeDef; /* size of structure = 0x02C */
/*
 * @brief SID
 */
/*!< SID Controller Interface */
typedef struct SID_Type
{
	uint32_t reserved_0x000 [0x0005];
	__IO uint32_t THS;                                   /*!< Offset 0x014 [27:16]: The calibration value of the T-sensor. */
	uint32_t reserved_0x018 [0x007E];
	__IO uint32_t BOOT_MODE;                             /*!< Offset 0x210 [27:16]: eFUSE boot select status, [0]: 0: GPIO boot select, 1: eFuse boot select */
} SID_TypeDef; /* size of structure = 0x214 */
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability Controller Interface */
typedef struct USB_EHCI_Capability_Type
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
} USB_EHCI_Capability_TypeDef; /* size of structure = 0x02C */
/*
 * @brief SPINLOCK
 */
/*!< SPINLOCK Controller Interface */
typedef struct SPINLOCK_Type
{
	__IO uint32_t SPINLOCK_SYSTATUS_REG;                 /*!< Offset 0x000 Spinlock System Status Register */
	uint32_t reserved_0x004 [0x0003];
	__IO uint32_t SPINLOCK_STATUS_REG;                   /*!< Offset 0x010 Spinlock Status Register */
	uint32_t reserved_0x014 [0x0003];
	__IO uint32_t SPINLOCK_IRQ_EN_REG;                   /*!< Offset 0x020 Spinlock Interrupt Enable Register */
	uint32_t reserved_0x024 [0x0007];
	__IO uint32_t SPINLOCK_IRQ_STA_REG;                  /*!< Offset 0x040 Spinlock Interrupt Status Register */
	uint32_t reserved_0x044 [0x000F];
	__IO uint32_t SPINLOCK_LOCKID0_REG;                  /*!< Offset 0x080 Spinlock Lockid0 Register */
	__IO uint32_t SPINLOCK_LOCKID1_REG;                  /*!< Offset 0x084 Spinlock Lockid1 Register */
	__IO uint32_t SPINLOCK_LOCKID2_REG;                  /*!< Offset 0x088 Spinlock Lockid2 Register */
	__IO uint32_t SPINLOCK_LOCKID3_REG;                  /*!< Offset 0x08C Spinlock Lockid3 Register */
	__IO uint32_t SPINLOCK_LOCKID4_REG;                  /*!< Offset 0x090 Spinlock Lockid4 Register */
	uint32_t reserved_0x094 [0x001B];
	__IO uint32_t SPINLOCK_LOCK_REG [0x020];             /*!< Offset 0x100 Spinlock Register N (N = 0 to 31) */
} SPINLOCK_TypeDef; /* size of structure = 0x180 */
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
	uint32_t reserved_0x020 [0x0002];
	__IO uint32_t DMAC_MODE_REGN;                        /*!< Offset 0x028 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
	__IO uint32_t DMAC_FDESC_ADDR_REGN;                  /*!< Offset 0x02C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
	__IO uint32_t DMAC_PKG_NUM_REGN;                     /*!< Offset 0x030 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
	uint32_t reserved_0x034 [0x0003];
} DMAC_CH_TypeDef; /* size of structure = 0x040 */
/*
 * @brief DMAC
 */
/*!< DMAC Controller Interface */
typedef struct DMAC_Type
{
	__IO uint32_t DMAC_IRQ_EN_REG0;                      /*!< Offset 0x000 DMAC IRQ Enable Register 0 */
	__IO uint32_t DMAC_IRQ_EN_REG1;                      /*!< Offset 0x004 DMAC IRQ Enable Register 1 */
	uint32_t reserved_0x008 [0x0002];
	__IO uint32_t DMAC_IRQ_PEND_REG0;                    /*!< Offset 0x010 DMAC IRQ Pending Register 0 */
	__IO uint32_t DMAC_IRQ_PEND_REG1;                    /*!< Offset 0x014 DMAC IRQ Pending Register 1 */
	uint32_t reserved_0x018 [0x0004];
	__IO uint32_t DMAC_AUTO_GATE_REG;                    /*!< Offset 0x028 DMAC Auto Gating Register */
	uint32_t reserved_0x02C;
	__IO uint32_t DMAC_STA_REG;                          /*!< Offset 0x030 DMAC Status Register */
	uint32_t reserved_0x034 [0x0033];
	DMAC_CH_TypeDef CH [0x010];                          /*!< Offset 0x100 Channel [0..15] */
} DMAC_TypeDef; /* size of structure = 0x500 */
/*
 * @brief PWM_CH
 */
/*!< PWM_CH Controller Interface */
typedef struct PWM_CH_Type
{
	__IO uint32_t PCR;                                   /*!< Offset 0x000 PWM Control Register */
	__IO uint32_t PPR;                                   /*!< Offset 0x004 PWM Period Register */
	__IO uint32_t PCNTR;                                 /*!< Offset 0x008 PWM Count Register */
	__IO uint32_t PPCNTR;                                /*!< Offset 0x00C PWM Pulse Count Register */
	__IO uint32_t CCR;                                   /*!< Offset 0x010 Capture Control Register */
	__IO uint32_t CRLR;                                  /*!< Offset 0x014 Capture Rise Lock Register */
	__IO uint32_t CFLR;                                  /*!< Offset 0x018 Capture Fall Lock Register */
	uint32_t reserved_0x01C;
} PWM_CH_TypeDef; /* size of structure = 0x020 */
/*
 * @brief PWM
 */
/*!< PWM Controller Interface */
typedef struct PWM_Type
{
	__IO uint32_t PIER;                                  /*!< Offset 0x000 PWM IRQ Enable Register */
	__IO uint32_t PISR;                                  /*!< Offset 0x004 PWM IRQ Status Register */
	uint32_t reserved_0x008 [0x0002];
	__IO uint32_t CIER;                                  /*!< Offset 0x010 Capture IRQ Enable Register */
	__IO uint32_t CISR;                                  /*!< Offset 0x014 Capture IRQ Status Register */
	uint32_t reserved_0x018 [0x0002];
	__IO uint32_t PCCR [0x004];                          /*!< Offset 0x020 PWM01, PWM23, PWM45, PWM67 Clock Configuration Register */
	uint32_t reserved_0x030 [0x0004];
	__IO uint32_t PCGR;                                  /*!< Offset 0x040 PWM Clock Gating Register */
	uint32_t reserved_0x044 [0x0007];
	__IO uint32_t PDZCR [0x004];                         /*!< Offset 0x060 PWM01, PWM23, PWM45, PWM67 Dead Zone Control Register */
	uint32_t reserved_0x070 [0x0004];
	__IO uint32_t PER;                                   /*!< Offset 0x080 PWM Enable Register */
	uint32_t reserved_0x084 [0x0003];
	__IO uint32_t PGR0 [0x004];                          /*!< Offset 0x090 PWM Group0, Group1, Group2, Group3 Register */
	uint32_t reserved_0x0A0 [0x0008];
	__IO uint32_t CER;                                   /*!< Offset 0x0C0 Capture Enable Register */
	uint32_t reserved_0x0C4 [0x000F];
	PWM_CH_TypeDef CH [0x008];                           /*!< Offset 0x100 Channels[0..7] */
} PWM_TypeDef; /* size of structure = 0x200 */
/*
 * @brief EMAC_ADDR
 */
/*!< EMAC_ADDR Controller Interface */
typedef struct EMAC_ADDR_Type
{
	__IO uint32_t HIGH;                                  /*!< Offset 0x000 EMAC MAC Address High Register */
	__IO uint32_t LOW;                                   /*!< Offset 0x004 EMAC MAC Address Low Register */
} EMAC_ADDR_TypeDef; /* size of structure = 0x008 */
/*
 * @brief EMAC
 */
/*!< EMAC Controller Interface */
typedef struct EMAC_Type
{
	__IO uint32_t EMAC_BASIC_CTL0;                       /*!< Offset 0x000 EMAC Basic Control Register0 */
	__IO uint32_t EMAC_BASIC_CTL1;                       /*!< Offset 0x004 EMAC Basic Control Register1 */
	__IO uint32_t EMAC_INT_STA;                          /*!< Offset 0x008 EMAC Interrupt Status Register */
	__IO uint32_t EMAC_INT_EN;                           /*!< Offset 0x00C EMAC Interrupt Enable Register */
	__IO uint32_t EMAC_TX_CTL0;                          /*!< Offset 0x010 EMAC Transmit Control Register0 */
	__IO uint32_t EMAC_TX_CTL1;                          /*!< Offset 0x014 EMAC Transmit Control Register1 */
	uint32_t reserved_0x018;
	__IO uint32_t EMAC_TX_FLOW_CTL;                      /*!< Offset 0x01C EMAC Transmit Flow Control Register */
	__IO uint32_t EMAC_TX_DMA_DESC_LIST;                 /*!< Offset 0x020 EMAC Transmit Descriptor List Address Register */
	__IO uint32_t EMAC_RX_CTL0;                          /*!< Offset 0x024 EMAC Receive Control Register0 */
	__IO uint32_t EMAC_RX_CTL1;                          /*!< Offset 0x028 EMAC Receive Control Register1 */
	uint32_t reserved_0x02C [0x0002];
	__IO uint32_t EMAC_RX_DMA_DESC_LIST;                 /*!< Offset 0x034 EMAC Receive Descriptor List Address Register */
	__IO uint32_t EMAC_RX_FRM_FLT;                       /*!< Offset 0x038 EMAC Receive Frame Filter Register */
	uint32_t reserved_0x03C;
	__IO uint32_t EMAC_RX_HASH0;                         /*!< Offset 0x040 EMAC Hash Table Register0 */
	__IO uint32_t EMAC_RX_HASH1;                         /*!< Offset 0x044 EMAC Hash Table Register1 */
	__IO uint32_t EMAC_MII_CMD;                          /*!< Offset 0x048 EMAC Management Interface Command Register */
	__IO uint32_t EMAC_MII_DATA;                         /*!< Offset 0x04C EMAC Management Interface Data Register */
	EMAC_ADDR_TypeDef EMAC_ADDR [0x008];                 /*!< Offset 0x050 EMAC MAC Address N (N=0-7) */
	uint32_t reserved_0x090 [0x0008];
	__IO uint32_t EMAC_TX_DMA_STA;                       /*!< Offset 0x0B0 EMAC Transmit DMA Status Register */
	__IO uint32_t EMAC_TX_CUR_DESC;                      /*!< Offset 0x0B4 EMAC Current Transmit Descriptor Register */
	__IO uint32_t EMAC_TX_CUR_BUF;                       /*!< Offset 0x0B8 EMAC Current Transmit Buffer Address Register */
	uint32_t reserved_0x0BC;
	__IO uint32_t EMAC_RX_DMA_STA;                       /*!< Offset 0x0C0 EMAC Receive DMA Status Register */
	__IO uint32_t EMAC_RX_CUR_DESC;                      /*!< Offset 0x0C4 EMAC Current Receive Descriptor Register */
	__IO uint32_t EMAC_RX_CUR_BUF;                       /*!< Offset 0x0C8 EMAC Current Receive Buffer Address Register */
	uint32_t reserved_0x0CC;
	__IO uint32_t EMAC_RGMII_STA;                        /*!< Offset 0x0D0 EMAC RGMII Status Register */
} EMAC_TypeDef; /* size of structure = 0x0D4 */
/* Generated section end */

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

/* Access pointers */

#define UART0      ((UART_TypeDef *) UART0_BASE)		/*!< \brief UART0 Interface register set access pointer */
#define UART1      ((UART_TypeDef *) UART1_BASE)		/*!< \brief UART1 Interface register set access pointer */
#define UART2      ((UART_TypeDef *) UART2_BASE)		/*!< \brief UART2 Interface register set access pointer */
#define UART3      ((UART_TypeDef *) UART3_BASE)		/*!< \brief UART3 Interface register set access pointer */
#define UART4      ((UART_TypeDef *) UART4_BASE)		/*!< \brief UART4 Interface register set access pointer */
#define UART5      ((UART_TypeDef *) UART5_BASE)		/*!< \brief UART5 Interface register set access pointer */


// GPIO registers calculation
#define GPIOBLOCK		((GPIOBLOCK_TypeDef *) GPIO_BASE)		/*!< \brief GPIOBLOCK Interface register set access pointer */

#define GPIOB		(& GPIOBLOCK->GPIO_PINS [1])		/*!< \brief GPIOB Interface register set access pointer */
#define GPIOC		(& GPIOBLOCK->GPIO_PINS [2])		/*!< \brief GPIOC Interface register set access pointer */
#define GPIOD		(& GPIOBLOCK->GPIO_PINS [3])		/*!< \brief GPIOD Interface register set access pointer */
#define GPIOE		(& GPIOBLOCK->GPIO_PINS [4])		/*!< \brief GPIOE Interface register set access pointer */
#define GPIOF		(& GPIOBLOCK->GPIO_PINS [5])		/*!< \brief GPIOF Interface register set access pointer */
#define GPIOG		(& GPIOBLOCK->GPIO_PINS [6])		/*!< \brief GPIOG Interface register set access pointer */

#define GPIOINTB   (& GPIOBLOCK->GPIO_INTS [1])		/*!< \brief GPIOINTB Interface register set access pointer */
#define GPIOINTC   (& GPIOBLOCK->GPIO_INTS [2])		/*!< \brief GPIOINTC Interface register set access pointer */
#define GPIOINTD   (& GPIOBLOCK->GPIO_INTS [3])		/*!< \brief GPIOINTD Interface register set access pointer */
#define GPIOINTE   (& GPIOBLOCK->GPIO_INTS [4])		/*!< \brief GPIOINTE Interface register set access pointer */
#define GPIOINTF   (& GPIOBLOCK->GPIO_INTS [5])		/*!< \brief GPIOINTF Interface register set access pointer */
#define GPIOINTG   (& GPIOBLOCK->GPIO_INTS [6])		/*!< \brief GPIOINTG Interface register set access pointer */

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
#define CE_NS		((CE_TypeDef *) CE_NS_BASE)			/*!< \brief CE_NS Interface register set access pointer */
#define CE_S		((CE_TypeDef *) CE_S_BASE)			/*!< \brief CE_S Interface register set access pointer */
#define HSTIMER		((HSTIMER_TypeDef *) HSTIMER_BASE)	/*!< \brief HSTIMER Interface register set access pointer */
#define TIMER		((TIMER_TypeDef *) TIMER_BASE)		/*!< \brief TIMER Interface register set access pointer */
#define THS			((THS_TypeDef *) THS_BASE)			/*!< \brief THS Interface register set access pointer */
#define IOMMU		((IOMMU_TypeDef *) IOMMU_BASE)		/*!< \brief IOMMU Interface register set access pointer */
#define CCU			((CCU_TypeDef *) CCU_BASE)			/*!< \brief CCU Interface register set access pointer */

#define DE 			((DE_TypeDef *) DE_BASE)					/*!< \brief DE Interface register set access pointer */
#define DI 			((DI_TypeDef *) DI_BASE)					/*!< \brief DI Interface register set access pointer */

#define G2D_TOP     ((G2D_TOP_TypeDef *) G2D_TOP_BASE)			/*!< \brief G2D_TOP Interface register set access pointer */
#define G2D_MIXER   ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)		/*!< \brief G2D_MIXER Interface register set access pointer */
#define G2D_BLD     ((G2D_BLD_TypeDef *) G2D_BLD_BASE)			/*!< \brief G2D_BLD Interface register set access pointer */
#define G2D_V0      ((G2D_LAY_TypeDef *) G2D_V0_BASE)			/*!< \brief G2D_V0 Interface register set access pointer */
#define G2D_UI0     ((G2D_UI_TypeDef *) G2D_UI0_BASE)			/*!< \brief G2D_UI0 Interface register set access pointer */
#define G2D_UI1     ((G2D_UI_TypeDef *) G2D_UI1_BASE)			/*!< \brief G2D_UI1 Interface register set access pointer */
#define G2D_UI2     ((G2D_UI_TypeDef *) G2D_UI2_BASE)			/*!< \brief G2D_UI2 Interface register set access pointer */
#define G2D_WB      ((G2D_WB_TypeDef *) G2D_WB_BASE)			/*!< \brief G2D_WB Interface register set access pointer */
#define G2D_VSU     ((G2D_VSU_TypeDef *) G2D_VSU_BASE)			/*!< \brief G2D_VSU Interface register set access pointer */
#define G2D_ROT     ((G2D_ROT_TypeDef *) G2D_ROT_BASE)			/*!< \brief G2D_ROT Interface register set access pointer */
#define G2D_GSU     ((G2D_GSU_TypeDef *) G2D_GSU_BASE)			/*!< \brief G2D_GSU Interface register set access pointer */

#define DSI 		((DSI_TypeDef *) DSI_BASE)					/*!< \brief DSI Interface register set access pointer */
#define DISPLAY_TOP ((DISPLAY_TOP_TypeDef *) DISPLAY_TOP_BASE)	/*!< \brief DISPLAY_TOP Interface register set access pointer */
#define TCON_LCD0 	((TCON_LCD_TypeDef *) TCON_LCD0_BASE)		/*!< \brief TCON_LCD0 Interface register set access pointer */
#define TCON_TV0 	((TCON_TV_TypeDef *) TCON_TV0_BASE)			/*!< \brief TCON_TV0 Interface register set access pointer */
#define TVE_TOP 	((TVE_TOP_TypeDef *) TVE_TOP_BASE)				/*!< \brief TVE_TOP Interface register set access pointer */
#define TV_Encoder 	((TVE_TypeDef *) TV_Encoder_BASE)			/*!< \brief TV_Encoder Interface register set access pointer */

#define CSIC_CCU		((CSIC_CCU_TypeDef *) CSIC_CCU_BASE)		/*!< \brief CSIC_CCU Interface register set access pointer */
#define CSIC_TOP		((CSIC_TOP_TypeDef *) CSIC_TOP_BASE)		/*!< \brief CSIC_TOP Interface register set access pointer */
#define CSIC_PARSER0	((CSIC_PARSER_TypeDef *) CSIC_PARSER0_BASE)	/*!< \brief CSIC_PARSER0 Interface register set access pointer */
#define SIC_DMA0		((CSIC_DMA_TypeDef *) SIC_DMA0_BASE)		/*!< \brief SIC_DMA0 Interface register set access pointer */
#define SIC_DMA1		((CSIC_DMA_TypeDef *) SIC_DMA1_BASE)		/*!< \brief SIC_DMA1 Interface register set access pointer */

#define TVD_TOP			((TVD_TOP_TypeDef *) TVD_TOP_BASE)			/*!< \brief TVD_TOP Interface register set access pointer */
#define TVD0			((TVD_TypeDef *) TVD0_BASE)					/*!< \brief TVD0 Interface register set access pointer */

#define RTC				((RTC_TypeDef *) RTC_BASE)					/*!< \brief RTC Interface register set access pointer */

#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)			/*!< \brief CPU_SUBSYS_CTRL Interface register set access pointer */
//#define TimeStamp_STA	((TimeStamp_STA_TypeDef *) TimeStamp_STA_BASE)
//#define TimeStamp_CTRL	((TimeStamp_CTRL_TypeDef *) TimeStamp_CTRL_BASE)
//#define IDC				(IDC_TypeDef *) IDC_BASE)
#define C0_CPUX_CFG		((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)			/*!< \brief C0_CPUX_CFG Interface register set access pointer */
//#define C0_CPUX_MBIST	((C0_CPUX_MBIST_TypeDef *) C0_CPUX_MBIST_BASE)			/*!< \brief C0_CPUX_MBIST Interface register set access pointer */
#define DDRPHYC			((DDRPHYC_TypeDef *) DDRPHYC_BASE)				/*!< \brief DDRPHYC Interface register set access pointer */
#define MSI_MEMC		((MSI_MEMC_TypeDef *) MSI_MEMC_BASE)			/*!< \brief MSI_MEMC Interface register set access pointer */
#define SID				((SID_TypeDef *) SID_BASE)						/*!< \brief SID Interface register set access pointer */

#define CAN0		((CAN_TypeDef *) CAN0_BASE)							/*!< \brief CAN0 Interface register set access pointer */
#define CAN1		((CAN_TypeDef *) CAN1_BASE)							/*!< \brief CAN1 Interface register set access pointer */

#define USBOTG0		((USBOTG_TypeDef *) USBOTG0_BASE)								/*!< \brief USBOTG0 Interface register set access pointer */
#define USBEHCI0	((USB_EHCI_CapabilityTypeDef *) (USBOTG0_BASE + 0x1000))	/*!< \brief USBEHCI0 Interface register set access pointer */
#define USBOHCI0	((struct ohci_registers *) (USBOTG0_BASE + 0x1400))		/*!< \brief USBOHCI0 Interface register set access pointer */
#define USBPHY0		((USBPHYC_TypeDef *) (USBOTG0_BASE + 0x0400))				/*!< \brief USBPHY0 Interface register set access pointer */

#define USBEHCI1	((USB_EHCI_CapabilityTypeDef *) USBEHCI1_BASE)				/*!< \brief USBEHCI1 Interface register set access pointer */
#define USBOHCI1	((struct ohci_registers *) (USBEHCI1_BASE + 0x0400))		/*!< \brief USBOHCI1 Interface register set access pointer */
#define USBPHY1		((USBPHYC_TypeDef *) (USBEHCI1_BASE + 0x0800))				/*!< \brief USBPHY1 Interface register set access pointer */

#define SPINLOCK	((SPINLOCK_TypeDef *) SPINLOCK_BASE)				/*!< \brief SPINLOCK Interface register set access pointer */
#define DMAC		((DMAC_TypeDef *) DMAC_BASE)						/*!< \brief DMAC Interface register set access pointer */
#define PWM			((PWM_TypeDef *) PWM_BASE)							/*!< \brief PWM Interface register set access pointer */
#define EMAC		((EMAC_TypeDef *) EMAC_BASE)						/*!< \brief EMAC Interface register set access pointer */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,
	DMAC_SrcReqOWA_RX = 2,
	DMAC_SrcReqI2S1_RX = 4,
	DMAC_SrcReqI2S2_RX = 5,
	DMAC_SrcReqAudioCodec = 7,
	DMAC_SrcReqDMIC = 8,
	DMAC_SrcReqGPADC = 12,
	DMAC_SrcReqTPADC = 13,
	DMAC_SrcReqUART0_RX = 14,
	DMAC_SrcReqUART1_RX = 15,
	DMAC_SrcReqUART2_RX = 16,
	DMAC_SrcReqUART3_RX = 17,
	DMAC_SrcReqUART4_RX = 18,
	DMAC_SrcReqUART5_RX = 19,
	DMAC_SrcReqSPI0_RX = 22,
	DMAC_SrcReqSPI1_RX = 23,
	DMAC_SrcReqUSB0_EP1 = 30,
	DMAC_SrcReqUSB0_EP2 = 31,
	DMAC_SrcReqUSB0_EP3 = 32,
	DMAC_SrcReqUSB0_EP4 = 33,
	DMAC_SrcReqUSB0_EP5 = 34,
	DMAC_SrcReqTWI0_RX = 43,
	DMAC_SrcReqTWI1_RX = 44,
	DMAC_SrcReqTWI2_RX = 45,
	DMAC_SrcReqTWI3_RX = 46
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA_TX = 2,
	DMAC_DstReqI2S1_TX = 4,
	DMAC_DstReqI2S2_TX = 5,
	DMAC_DstReqAudioCodec = 7,
	DMAC_DstReqIR_TX = 13,
	DMAC_DstReqUART0_TX = 14,
	DMAC_DstReqUART1_TX = 15,
	DMAC_DstReqUART2_TX = 16,
	DMAC_DstReqUART3_TX = 17,
	DMAC_DstReqUART4_TX = 18,
	DMAC_DstReqUART5_TX = 19,
	DMAC_DstReqSPI0_TX = 22,
	DMAC_DstReqSPI1_TX = 23,
	DMAC_DstReqUSB0_EP1 = 30,
	DMAC_DstReqUSB0_EP2 = 31,
	DMAC_DstReqUSB0_EP3 = 32,
	DMAC_DstReqUSB0_EP4 = 33,
	DMAC_DstReqUSB0_EP5 = 34,
	DMAC_DstReqLEDC = 42,
	DMAC_DstReqTWI0_TX = 43,
	DMAC_DstReqTWI1_TX = 44,
	DMAC_DstReqTWI2_TX = 45,
	DMAC_DstReqTWI3_TX = 46
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
 #include "t113s3_hal.h"
#endif /* USE_HAL_DRIVER */

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
