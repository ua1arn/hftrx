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
//typedef enum IRQn
//{
//	/******  Cortex-A Processor Specific Interrupt Numbers ***************************************************************/
//	/* Software Generated Interrupts                                                                                     */
//	SGI0_IRQn                        =  0,     /*!< Software Generated Interrupt  0                                      */
//	SGI1_IRQn                        =  1,     /*!< Software Generated Interrupt  1                                      */
//	SGI2_IRQn                        =  2,     /*!< Software Generated Interrupt  2                                      */
//	SGI3_IRQn                        =  3,     /*!< Software Generated Interrupt  3                                      */
//	SGI4_IRQn                        =  4,     /*!< Software Generated Interrupt  4                                      */
//	SGI5_IRQn                        =  5,     /*!< Software Generated Interrupt  5                                      */
//	SGI6_IRQn                        =  6,     /*!< Software Generated Interrupt  6                                      */
//	SGI7_IRQn                        =  7,     /*!< Software Generated Interrupt  7                                      */
//	SGI8_IRQn                        =  8,     /*!< Software Generated Interrupt  8                                      */
//	SGI9_IRQn                        =  9,     /*!< Software Generated Interrupt  9                                      */
//	SGI10_IRQn                       = 10,     /*!< Software Generated Interrupt 10                                      */
//	SGI11_IRQn                       = 11,     /*!< Software Generated Interrupt 11                                      */
//	SGI12_IRQn                       = 12,     /*!< Software Generated Interrupt 12                                      */
//	SGI13_IRQn                       = 13,     /*!< Software Generated Interrupt 13                                      */
//	SGI14_IRQn                       = 14,     /*!< Software Generated Interrupt 14                                      */
//	SGI15_IRQn                       = 15,     /*!< Software Generated Interrupt 15                                      */
//	/* Private Peripheral Interrupts                                                                                     */
//	VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
//	HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
//	VirtualTimer_IRQn                = 27,     /*!< Virtual Timer Interrupt                                              */
//	Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
//	SecurePhysicalTimer_IRQn         = 29,     /*!< Secure Physical Timer Interrupt                                      */
//	NonSecurePhysicalTimer_IRQn      = 30,     /*!< Non-Secure Physical Timer Interrupt                                  */
//	Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */
//
//	/******  Allwinner T128-S3 specific Interrupt Numbers ****************************************************************************/
//
//	CPUX_MSGBOX_R = (32), /*  Peripherial */
//	UART0_IRQn = (34), /*  Peripherial */
//	UART1_IRQn = (35), /*  Peripherial */
//	UART2_IRQn = (36), /*  Peripherial */
//	UART3_IRQn = (37), /*  Peripherial */
//	UART4_IRQn = (38), /*  Peripherial */
//	UART5_IRQn = (39), /*  Peripherial */
//	TWI0_IRQn = (41), /*  Peripherial */
//	TWI1_IRQn = (42), /*  Peripherial */
//	TWI2_IRQn = (43), /*  Peripherial */
//	TWI3_IRQn = (44), /*  Peripherial */
//	SPI0_IRQn = (47), /*  Peripherial */
//	SPI1_IRQn = (48), /*  Peripherial */
//	PWM_IRQn = (50), /*  Peripherial */
//	CIR_TX_IRQn = (51), /*  Peripherial */
//	LEDC_IRQn = (52), /*  Peripherial */
//	CAN0_IRQ = (53), /*  Peripherial */
//	CAN1_IRQ = (54), /*  Peripherial */
//	OWA_IRQn = (55), /*  Peripherial */
//	DMIC_IRQn = (56), /*  Peripherial */
//	AUDIO_CODEC_IRQn = (57), /*  Peripherial */
//	I2S_PCM1_IRQn = (59), /*  Peripherial */
//	I2S_PCM2_IRQn = (60), /*  Peripherial */
//	USB0_DEVICE_IRQn = (61), /*  Peripherial */
//	USB0_EHCI_IRQn = (62), /*  Peripherial */
//	USB0_OHCI_IRQn = (63), /*  Peripherial */
//	USB1_EHCI_IRQn = (65), /*  Peripherial */
//	USB1_OHCI_IRQn = (66), /*  Peripherial */
//	SMHC0_IRQn = (72), /*  Peripherial */
//	SMHC1_IRQn = (73), /*  Peripherial */
//	SMHC2_IRQn = (74), /*  Peripherial */
//	MSI_IRQn = (75), /*  Peripherial */
//	SMC_IRQn = (76), /*  Peripherial */
//	EMAC_IRQn = (78), /*  Peripherial */
//	TZMA_ERR_IRQn = (79), /*  Peripherial */
//	CCU_FERR_IRQn = (80), /*  Peripherial */
//	AHB_HREADY_TIME_OUT_IRQn = (81), /*  Peripherial */
//	DMAC_NS_IRQn = (82), /*  Peripherial */
//	DMAC_S_IRQn = (83), /*  Peripherial */
//	CE_NS_IRQn = (84), /*  Peripherial */
//	CE_S_IRQn = (85), /*  Peripherial */
//	SPINLOCK_IRQn = (86), /*  Peripherial */
//	HSTIME0_IRQn = (87), /*  Peripherial */
//	HSTIME1_IRQn = (88), /*  Peripherial */
//	GPADC_IRQn = (89), /*  Peripherial */
//	THS_IRQn = (90), /*  Peripherial */
//	TIMER0_IRQn = (91), /*  TIMER->TMR0_CTRL_REG */
//	TIMER1_IRQn = (92), /*  TIMER->TMR1_CTRL_REG */
//	LRADC_IRQn = (93), /*  Peripherial */
//	TPADC_IRQn = (94), /*  Peripherial */
//	WATCHDOG_IRQn = (95), /*  Peripherial */
//	IOMMU_IRQn = (96), /*  Peripherial */
//	VE_IRQn = (98), /*  Peripherial */
//	GPIOB_NS_IRQn = (101), /*  Peripherial */
//	GPIOB_S_IRQn = (102), /*  Peripherial */
//	GPIOC_NS_IRQn = (103), /*  Peripherial */
//	GPIOC_S_IRQn = (104), /*  Peripherial */
//	GPIOD_NS_IRQn = (105), /*  Peripherial */
//	GPIOD_S_IRQn = (106), /*  Peripherial */
//	GPIOE_NS_IRQn = (107), /*  Peripherial */
//	GPIOE_S_IRQn = (108), /*  Peripherial */
//	GPIOF_NS_IRQn = (109), /*  Peripherial */
//	GPIOF_S_IRQn = (110), /*  Peripherial */
//	GPIOG_NS_IRQn = (111), /*  Peripherial */
//	CPUX_MSGBOX_DSP_W = (117), /*  Peripherial */
//	DE_IRQn = (119), /*  Peripherial */
//	DI_IRQn = (120), /*  Peripherial */
//	G2D_IRQn = (121), /*  Peripherial */
//	LCD_IRQn = (122), /*  Peripherial */
//	TV_IRQn = (123), /*  Peripherial */
//	DSI_IRQn = (124), /*  Peripherial */
//	CSI_DMA0_IRQn = (127), /*  Peripherial */
//	CSI_DMA1_IRQn = (128), /*  Peripherial */
//	CSI_PARSER0_IRQn = (132), /*  Peripherial */
//	CSI_TOP_PKT_IRQn = (138), /*  Peripherial */
//	DSP_DFE_IRQn = (152), /*  Peripherial */
//	DSP_PFE_IRQn = (153), /*  Peripherial */
//	DSP_WDG_IRQn = (154), /*  Peripherial */
//	DSP_MBOX_RISCV_W_IRQn = (155), /*  Peripherial */
//	DSP_TZMA_IRQn = (157), /*  Peripherial */
//	/* CPUs related */
//	NMI_IRQn = (168), /*  Peripherial */
//	PPU_IRQn = (169), /*  Peripherial */
//	TWD_IRQn = (170), /*  Peripherial */
//	TIMER_und_0_IRQn = (172), /*  Peripherial */
//	TIMER_und_1_IRQn = (173), /*  Peripherial */
//	TIMER_und_2_IRQn = (174), /*  Peripherial */
//	TIMER_und_3_IRQn = (175), /*  Peripherial */
//	ALARM0_IRQn = (176), /*  Peripherial */
//	IRRX_IRQn = (183), /*  Peripherial */
//	C0_CTI0_IRQn = (192), /*  Peripherial */
//	C0_CTI1_IRQn = (193), /*  Peripherial */
//	C0_COMMTX0_IRQn = (196), /*  Peripherial */
//	C0_COMMTX1_IRQn = (197), /*  Peripherial */
//	C0_COMMRX0_IRQn = (200), /*  Peripherial */
//	C0_COMMRX1_IRQn = (201), /*  Peripherial */
//	C0_PMU0_IRQn = (204), /*  Peripherial */
//	C0_PMU1_IRQn = (205), /*  Peripherial */
//	C0_AXI_ERROR_IRQn = (208), /*  Peripherial */
//	AXI_WR_IRQ_IRQn = (210), /*  Peripherial */
//	AXI_RD_IRQ_IRQn = (211), /*  Peripherial */
//	DBGWRUPREQ_OUT0_IRQn = (212), /*  Peripherial */
//	DBGWRUPREQ_OUT1_IRQn = (213), /*  Peripherial */
//
//	MAX_IRQ_n,
//	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
//} IRQn_Type;

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* Peripheral and RAM base address */
// SP0 (SYS domain)
//#define GPIO_BASE			0x02000000
//#define SPC_BASE			0x02000800
//#define PWM_BASE			0x02000C00
//#define CCU_BASE			0x02001000
//#define CIR_TX_BASE			0x02003000
//#define TZMA_BASE			0x02004000
//#define LEDC_BASE			0x02008000
//#define GPADC_BASE			0x02009000
//#define THS_BASE			0x02009400
//#define TPADC_BASE			0x02009C00
//#define IOMMU_BASE			0x02010000
//#define AUDIO_CODEC_BASE	0x02030000
//#define DMIC_BASE			0x02031000
//#define I2S1_BASE			0x02033000
//#define I2S2_BASE			0x02034000
//#define OWA_BASE			0x02036000
//#define TIMER_BASE			0x02050000

// SP1 (SYS domain)
//#define UART0_BASE      	0x02500000
//#define UART1_BASE      	0x02500400
//#define UART2_BASE     		0x02500800
//#define UART3_BASE			0x02500C00
//#define UART4_BASE      	0x02501000
//#define UART5_BASE      	0x02501400

//#define TWI0_BASE 	     	0x02502000
//#define TWI1_BASE 	     	0x02502400
//#define TWI2_BASE 	     	0x02502800
//#define TWI3_BASE 	     	0x02502C00

//#define CAN0_BASE  			0x02504000
//#define CAN1_BASE  			0x02504400

// SH0 (SYS domain)
//#define SYS_CFG_BASE      	0x03000000
//#define DMAC_BASE      		0x03002000
//#define CPUX_MSGBOX_BASE	0x03003000
//#define SPINLOCK_BASE     	0x03005000
//#define	SID_BASE  		   	0x03006000	/* 4 KB Security ID (SID) */
//#define SMC_BASE     		0x03007000
//#define HSTIMER_BASE     	0x03008000
//#define DCU_BASE      		0x03010000	/* 64 KB */
//#define GIC_BASE      		0x03020000
//#define CE_NS_BASE      	0x03040000
//#define CE_S_BASE   	   	0x03040800
//#define CE_KEY_SRAM_BASE    0x03041000	/* 4 KB (only CE access) */
//#define MSI_MEMC_BASE	    0x03102000	/* 2 MB MSI and MEMC base address */
//#define DDRPHYC_BASE	 	0x03103000
#define MC_MEMC_BASE		0x03202000	/* MC */

// SH2 (SYS domain)
//#define SMHC0_BASE      	0x04020000
//#define SMHC1_BASE      	0x04021000
//#define SMHC2_BASE      	0x04022000
//#define SPI0_BASE    	  	0x04025000
//#define SPI_DBI_BASE  		0x04026000
//#define USBOTG0_BASE 	    0x04100000
//#define USBEHCI1_BASE 	    0x04200000
//#define EMAC_BASE 	     	0x04500000

// VIDEO_OUT_SYS related
//#define DE_BASE 			0x05000000
//#define DI_BASE 			0x05400000
//#define G2D_BASE 			0x05410000
//#define DSI_BASE 			0x05450000
//#define DISPLAY_TOP_BASE 	0x05460000
//#define TCON_LCD0_BASE 		0x05461000
//#define TCON_TV0_BASE 		0x05470000
//#define TVE_TOP_BASE 		0x05600000
//#define TV_Encoder_BASE 	0x05604000

// Undocumented registers
//	The Hotplug Flag Register is 0x070005C0
//	The Soft Entry Address Register of CPU0 is 0x070005C4
//	The Soft Entry Address Register of CPU1 is 0x070005C8
//	The status of the FEL pin is the bit[8] of the system configuration module (register: 0x03000024).
//	Fast Boot register (0x07090120) in RTC module

#define CPU_0700_0000_BASE	((uintptr_t) 0x07000000)
#define CPU_0701_0000_BASE	((uintptr_t) 0x07010000)

// VIDEO_IN_SYS related
//#define CSI_BASE 			0x05800000

//#define TVD_TOP_BASE 		0x05C00000
//#define TVD0_BASE 			0x05C01000

// APBS0 related
//#define CIR_RX_BASE			0x07040000

// AHBS related
//#define RTC_BASE			0x07090000				/*!< (RTC         ) Base Address */

// CPUX related
// Address (It is for Cluster CPU)
//#define CPU_SUBSYS_CTRL_BASE	((uintptr_t) 0x08100000)
#define TimeStamp_STA_BASE		((uintptr_t) 0x08110000)
#define TimeStamp_CTRL_BASE		((uintptr_t) 0x08120000)
#define IDC_BASE				((uintptr_t) 0x08130000)
//#define C0_CPUX_CFG_BASE		((uintptr_t) 0x09010000)
#define C0_CPUX_MBIST_BASE		((uintptr_t) 0x09020000)	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

// DRAM Space (SYS domain)
#define DRAM_SPACE_SIZE			0x08000000u			/* 128 MB */
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */
#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */

//#define G2D_TOP_BASE        (0x00000 + G2D_BASE)
//#define G2D_MIXER_BASE      (0x00100 + G2D_BASE)
//#define G2D_BLD_BASE        (0x00400 + G2D_BASE)
//#define G2D_V0_BASE         (0x00800 + G2D_BASE)
//#define G2D_UI0_BASE        (0x01000 + G2D_BASE)
//#define G2D_UI1_BASE        (0x01800 + G2D_BASE)
//#define G2D_UI2_BASE        (0x02000 + G2D_BASE)
//#define G2D_WB_BASE         (0x03000 + G2D_BASE)
//#define G2D_VSU_BASE        (0x08000 + G2D_BASE)
//#define G2D_ROT_BASE        (0x28000 + G2D_BASE)
//#define G2D_GSU_BASE        (0x30000 + G2D_BASE)

//#define	GIC_DISTRIBUTOR_BASE	 ((uintptr_t) 0x03021000)
//#define	GIC_INTERFACE_BASE	 ((uintptr_t) 0x03022000)

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "cmsis_t113s3.h"

#include "core_ca.h"
#include "system_allwnr_t113s3.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

/* Access pointers */

#define UART0      ((UART_TypeDef *) UART0_BASE)		/*!< \brief UART0 Interface register set access pointer */
#define UART1      ((UART_TypeDef *) UART1_BASE)		/*!< \brief UART1 Interface register set access pointer */
#define UART2      ((UART_TypeDef *) UART2_BASE)		/*!< \brief UART2 Interface register set access pointer */
#define UART3      ((UART_TypeDef *) UART3_BASE)		/*!< \brief UART3 Interface register set access pointer */
#define UART4      ((UART_TypeDef *) UART4_BASE)		/*!< \brief UART4 Interface register set access pointer */
#define UART5      ((UART_TypeDef *) UART5_BASE)		/*!< \brief UART5 Interface register set access pointer */

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

#define CPU_SUBSYS_CTRL	((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)			/*!< \brief CPU_SUBSYS_CTRL Interface register set access pointer */
//#define TimeStamp_STA	((TimeStamp_STA_TypeDef *) TimeStamp_STA_BASE)
//#define TimeStamp_CTRL	((TimeStamp_CTRL_TypeDef *) TimeStamp_CTRL_BASE)
//#define IDC				(IDC_TypeDef *) IDC_BASE)
#define C0_CPUX_CFG		((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)			/*!< \brief C0_CPUX_CFG Interface register set access pointer */
//#define C0_CPUX_MBIST	((C0_CPUX_MBIST_TypeDef *) C0_CPUX_MBIST_BASE)			/*!< \brief C0_CPUX_MBIST Interface register set access pointer */
#define DDRPHYC			((DDRPHYC_TypeDef *) DDRPHYC_BASE)				/*!< \brief DDRPHYC Interface register set access pointer */
#define MSI_MEMC		((MSI_MEMC_TypeDef *) MSI_MEMC_BASE)			/*!< \brief MSI_MEMC Interface register set access pointer */
#define SID				((SID_TypeDef *) SID_BASE)						/*!< \brief SID Interface register set access pointer */

//#define CAN0		((CAN_TypeDef *) CAN0_BASE)							/*!< \brief CAN0 Interface register set access pointer */
//#define CAN1		((CAN_TypeDef *) CAN1_BASE)							/*!< \brief CAN1 Interface register set access pointer */

//#define USBOTG0		((USBOTG_TypeDef *) USBOTG0_BASE)								/*!< \brief USBOTG0 Interface register set access pointer */
//#define USBEHCI0	((USB_EHCI_CapabilityTypeDef *) (USBOTG0_BASE + 0x1000))	/*!< \brief USBEHCI0 Interface register set access pointer */
//#define USBOHCI0	((struct ohci_registers *) (USBOTG0_BASE + 0x1400))		/*!< \brief USBOHCI0 Interface register set access pointer */
//#define USBPHY0		((USBPHYC_TypeDef *) (USBOTG0_BASE + 0x0400))				/*!< \brief USBPHY0 Interface register set access pointer */

//#define USBEHCI1	((USB_EHCI_CapabilityTypeDef *) USBEHCI1_BASE)				/*!< \brief USBEHCI1 Interface register set access pointer */
//#define USBOHCI1	((struct ohci_registers *) (USBEHCI1_BASE + 0x0400))		/*!< \brief USBOHCI1 Interface register set access pointer */
//#define USBPHY1		((USBPHYC_TypeDef *) (USBEHCI1_BASE + 0x0800))				/*!< \brief USBPHY1 Interface register set access pointer */

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


__STATIC_FORCEINLINE uint32_t __UQADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __ASM ("uqadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}


__STATIC_FORCEINLINE uint32_t __UQSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __ASM ("uqsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
