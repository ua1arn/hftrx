/*
 * allwnr_f133.h
 *
 * The D1s features a single RV64GCV core XuanTie C906 from T-Head Semiconductor (subsidiary of Alibaba).
 */

#ifndef ARCH_ALLWNR_F133_ALLWNR_T13S3_H_
#define ARCH_ALLWNR_F133_ALLWNR_T13S3_H_

/**
 * @brief Allwinner T128-S3 Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum IRQn
{
	UART0_IRQn = 18, 		/*	UART0	*/
	UART1_IRQn = 19, 		/*	UART1	*/
	UART2_IRQn = 20, 		/*	UART2	*/
	UART3_IRQn = 21, 		/*	UART3	*/
	UART4_IRQn = 22, 		/*	UART4	*/
	UART5_IRQn = 23, 		/*	UART5	*/
	TWI0_IRQn = 25, 		/*	TWI0	*/
	TWI1_IRQn = 26, 		/*	TWI1	*/
	TWI2_IRQn = 27, 		/*	TWI2	*/
	TWI3_IRQn = 28, 		/*	TWI3	*/
	SPI0_IRQn = 31, 		/*	SPI0	*/
	SPI1_IRQn = 32, 		/*	SPI1	*/
	PWM_IRQn = 34, 			/*	PWM	*/
	CIR_TX_IRQn = 35, 		/*	CIR_TX	*/
	LEDC_IRQn = 36, 		/*	LEDC	*/
	CAN0_IRQ = 37, 			/*  CAN0 */
	CAN1_IRQ = 38, 			/*  CAN1 */
	OWA_IRQn = 39, 			/*	OWA	*/
	DMIC_IRQn = 40, 		/*	DMIC	*/
	AUDIO_CODEC_IRQn = 41, 	/*	AUDIO_CODEC	*/
	I2S_PCM1_IRQn = 43, 	/*	I2S/PCM1	*/
	I2S_PCM2_IRQn = 44, 	/*	I2S/PCM2	*/
	USB0_DEVICE_IRQn = 45, 	/*	USB0_DEVICE	*/
	USB0_EHCI_IRQn = 46,	/*	USB0_EHCI	*/
	USB0_OHCI_IRQn = 47, 	/*	USB0_OHCI	*/
	USB1_EHCI_IRQn = 49, 	/*	USB1_EHCI	*/
	USB1_OHCI_IRQn = 50, 	/*	USB1_OHCI	*/
	SMHC0_IRQn = 56, 		/*	SMHC0	*/
	SMHC1_IRQn = 57, 		/*	SMHC1	*/
	SMHC2_IRQn = 58, 		/*	SMHC2	*/
	MSI_IRQn = 59, 			/*	MSI	*/
	EMAC_IRQn = 62, 		/*	EMAC	*/
	CCU_FERR_IRQn = 64, 	/*	CCU_FERR	*/
	AHB_HREADY_TIME_OUT_IRQn = 65, /*	AHB_HREADY_TIME_OUT	*/
	DMAC_NS_IRQn = 66, 		/*	DMAC_NS	*/
	CE_NS_IRQn = 68, 		/*	CE_NS	*/
	HSTIMER0_IRQn = 71, 	/*	HSTIMER0	*/
	HSTIMER1_IRQn = 72, 	/*	HSTIMER1	*/
	GPADC_IRQn = 73, 		/*	GPADC	*/
	THS_IRQn = 74, 			/*	THS	*/
	TIMER0_IRQn = 75, 		/*	TIMER0	*/
	TIMER1_IRQn = 76, 		/*	TIMER1	*/
	LRADC_IRQn = 77, 		/*	LRADC ???	*/
	TPADC_IRQn = 78, 		/*	TPADC	*/
	WATCHDOG_IRQn = 79, 	/*	WATCHDOG	*/
	IOMMU_IRQn = 80, 		/*	IOMMU	*/
	VE_IRQn = 82, 			/*	VE	*/
	GPIOB_NS_IRQn = 85, 	/*	GPIOB_NS	*/
	GPIOC_NS_IRQn = 87, 	/*	GPIOC_NS	*/
	GPIOD_NS_IRQn = 89, 	/*	GPIOD_NS	*/
	GPIOE_NS_IRQn = 91, 	/*	GPIOE_NS	*/
	GPIOF_NS_IRQn = 93, 	/*	GPIOF_NS	*/
	GPIOG_NS_IRQn = 95, 	/*	GPIOG_NS	*/
	DE_IRQn = 103, 			/*	DE	*/
	DI_IRQn = 104, 			/*	DI	*/
	G2D_IRQn = 105, 		/*	G2D	*/
	LCD_IRQn = 106, 		/*	LCD	*/
	TV_IRQn = 107, 			/*	TV	*/
	DSI_IRQn = 108, 		/*	DSI	*/
	TVE_IRQn = 110, 		/*	TVE	*/
	CSI_DMA0_IRQn = 111, 	/*	CSI_DMA0	*/
	CSI_DMA1_IRQn = 112, 	/*	CSI_DMA1	*/
	CSI_PARSER0_IRQn = 116, /*	CSI_PARSER0	*/
	CSI_TOP_PKT_IRQn = 122, /*	CSI_TOP_PKT	*/
	TVD_IRQn = 123, 		/*	TVD	*/
	RISC_WDG_IRQn = 147, 	/*	RISC_WDG	*/
	IRRX_IRQn = 167, 		/*	IRRX	*/
	C0_CTI0_IRQn = 176, 	/*	C0_CTI0	*/
	C0_CTI1_IRQn = 177, 	/*	C0_CTI1	*/
	C0_COMMTX0_IRQn = 180, 	/*	C0_COMMTX0	*/
	C0_COMMTX1_IRQn = 181, 	/*	C0_COMMTX1	*/
	C0_COMMRX0_IRQn = 184, 	/*	C0_COMMRX0	*/
	C0_COMMRX1_IRQn = 185, 	/*	C0_COMMRX1	*/
	C0_PMU0_IRQn = 188, 	/*	C0_PMU0	*/
	C0_PMU1_IRQn = 189, 	/*	C0_PMU1	*/
	C0_AXI_ERROR_IRQn = 192, /*	C0_AXI_ERROR	*/
	AXI_WR_IRQ_IRQn = 194, 	/* AXI_WR_IRQ	*/
	AXI_RD_IRQ_IRQn = 195, 	/* AXI_RD_IRQ	*/
	DBGPWRUPREQ_out0_IRQn = 196, /*	DBGPWRUPREQ_out[0]	*/
	DBGPWRUPREQ_out1_IRQn = 197, /*	DBGPWRUPREQ_out[1]	*/

	MAX_IRQ_n,
	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* Peripheral and RAM base address */
// SP0 (SYS domain)
//#define GPIO_BASE			0x02000000
//#define PWM_BASE			0x02000C00
//#define CCU_BASE			0x02001000
//#define CIR_TX_BASE			0x02003000
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
//#define	SID_BASE  		   	0x03006000	/* 4 KB Security ID (SID) */
//#define HSTIMER_BASE     	0x03008000
//#define DCU_BASE      		0x03010000	/* 64 KB */
//#define CE_NS_BASE      	0x03040000
//#define CE_KEY_SRAM_BASE    0x03041000	/* 4 KB (only CE access) */
//#define MSI_MEMC_BASE	    0x03102000	/* 2 MB MSI and MEMC base address */
//#define DDRPHYC_BASE	 	0x03103000
//#define MC_MEMC_BASE        0x03202000    /* MC */

// SH2 (SYS domain)
//#define SMHC0_BASE      	0x04020000
//#define SMHC1_BASE      	0x04021000
//#define SMHC2_BASE      	0x04022000
//#define SPI0_BASE    	  	0x04025000
//#define SPI_DBI_BASE  		0x04026000
//#define USBOTG0_BASE 	    0x04100000
#define USBEHCI1_BASE 	    0x04200000
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

//#define CPU_0700_0000_BASE	0x07000000
//#define CPU_0701_0000_BASE	0x07010000

// VIDEO_IN_SYS related
//#define CSI_BASE 			0x05800000

//#define TVD_TOP_BASE 		0x05C00000
//#define TVD0_BASE 			0x05C01000

// RISC_SYS Related (SYS Domain)
#define RISC_BROM_BASE 			0x06000000	// RISC Core accesses the brom address: 0x00000000---0x0000FFFF
//#define RISC_CFG_BASE 			0x06010000
//#define RISC_WDG_BASE 			0x06011000
//#define RISC_TIMESTAMP_BASE 	0x06012000

// APBS0 related
//#define CIR_RX_BASE				0x07040000

// AHBS related
//#define RTC_BASE				0x07090000				/*!< (RTC         ) Base Address */

// CPUX related
// Address (It is for Cluster CPU)
//#define TimeStamp_STA_BASE		0x08110000
//#define TimeStamp_CTRL_BASE		0x08120000
//#define IDC_BASE				0x08130000
//#define C0_CPUX_CFG_BASE		0x09010000
//#define C0_CPUX_MBIST_BASE		0x09020000	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

//#define PLIC_BASE 			0x10000000
//#define CLINT_BASE			0x14000000

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 		0x40000000			/*!< (DRAM        ) Base Address - 2GB */
#define DRAM_SPACE_SIZE			0x04000000			/* 64 MB */

//#define G2D_TOP_BASE        (G2D_BASE + 0x00000)
//#define G2D_MIXER_BASE      (G2D_BASE + 0x00100)
//#define G2D_BLD_BASE        (G2D_BASE + 0x00400)
//#define G2D_V0_BASE         (G2D_BASE + 0x00800)
//#define G2D_UI0_BASE        (G2D_BASE + 0x01000)
//#define G2D_UI1_BASE        (G2D_BASE + 0x01800)
//#define G2D_UI2_BASE        (G2D_BASE + 0x02000)
//#define G2D_WB_BASE         (G2D_BASE + 0x03000)
//#define G2D_VSU_BASE        (G2D_BASE + 0x08000)
//#define G2D_ROT_BASE        (G2D_BASE + 0x28000)
//#define G2D_GSU_BASE        (G2D_BASE + 0x30000)

//#define GIC_DISTRIBUTOR_BASE (GIC_BASE + 0x1000)
//#define GIC_INTERFACE_BASE   (GIC_BASE + 0x2000)

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    0U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 0U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 0U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include <stdint.h>
#define __IO volatile

//#include "core_ca.h"
#include "system_allwnr_f133.h"

#include "cmsis_f133.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

/* Access pointers */

#define UART0      ((UART_TypeDef *) UART0_BASE)		/*!< \brief UART0 Interface register set access pointer */
#define UART1      ((UART_TypeDef *) UART1_BASE)		/*!< \brief UART1 Interface register set access pointer */
#define UART2      ((UART_TypeDef *) UART2_BASE)		/*!< \brief UART2 Interface register set access pointer */
#define UART3      ((UART_TypeDef *) UART3_BASE)		/*!< \brief UART3 Interface register set access pointer */
#define UART4      ((UART_TypeDef *) UART4_BASE)		/*!< \brief UART4 Interface register set access pointer */
#define UART5      ((UART_TypeDef *) UART5_BASE)		/*!< \brief UART5 Interface register set access pointer */

#define RISC_CFG 	((RISC_CFG_TypeDef *) RISC_CFG_BASE)	/*!< \brief RISC_CFG Interface register set access pointer */
#define RISC_WDG 	((RISC_WDG_TypeDef *) RISC_WDG_BASE)	/*!< \brief RISC_WDG Interface register set access pointer */
#define RISC_TIMESTAMP 	((RISC_TIMESTAMP_TypeDef *) RISC_TIMESTAMP_BASE)	/*!< \brief RISC_TIMESTAMP Interface register set access pointer */
#define PLIC 		((PLIC_TypeDef *) PLIC_BASE)		/*!< \brief PLIC Platform-Level Interrupt Controller Interface register set access pointer */
#define CLINT 		((CLINT_TypeDef *) CLINT_BASE)		/*!< \brief CLINT RISC-V Advanced Core Local Interruptor Specification Interface register set access pointer */

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

//#define DE 			((DE20_TypeDef *) DE_BASE)					/*!< \brief DE Interface register set access pointer */
//#define DI 			((DI_TypeDef *) DI_BASE)					/*!< \brief DI Interface register set access pointer */

//#define G2D_TOP     ((G2D_TOP_TypeDef *) G2D_TOP_BASE)			/*!< \brief G2D_TOP Interface register set access pointer */
//#define G2D_MIXER   ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)		/*!< \brief G2D_MIXER Interface register set access pointer */
//#define G2D_BLD     ((G2D_BLD_TypeDef *) G2D_BLD_BASE)			/*!< \brief G2D_BLD Interface register set access pointer */
//#define G2D_V0      ((G2D_LAY_TypeDef *) G2D_V0_BASE)			/*!< \brief G2D_V0 Interface register set access pointer */
//#define G2D_UI0     ((G2D_UI_TypeDef *) G2D_UI0_BASE)			/*!< \brief G2D_UI0 Interface register set access pointer */
//#define G2D_UI1     ((G2D_UI_TypeDef *) G2D_UI1_BASE)			/*!< \brief G2D_UI1 Interface register set access pointer */
//#define G2D_UI2     ((G2D_UI_TypeDef *) G2D_UI2_BASE)			/*!< \brief G2D_UI2 Interface register set access pointer */
//#define G2D_WB      ((G2D_WB_TypeDef *) G2D_WB_BASE)			/*!< \brief G2D_WB Interface register set access pointer */
//#define G2D_VSU     ((G2D_VSU_TypeDef *) G2D_VSU_BASE)			/*!< \brief G2D_VSU Interface register set access pointer */
//#define G2D_ROT     ((G2D_ROT_TypeDef *) G2D_ROT_BASE)			/*!< \brief G2D_ROT Interface register set access pointer */
//#define G2D_GSU     ((G2D_GSU_TypeDef *) G2D_GSU_BASE)			/*!< \brief G2D_GSU Interface register set access pointer */

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

//#define TimeStamp_STA	((TimeStamp_STA_TypeDef *) TimeStamp_STA_BASE)
//#define TimeStamp_CTRL	((TimeStamp_CTRL_TypeDef *) TimeStamp_CTRL_BASE)
//#define IDC				(IDC_TypeDef *) IDC_BASE)
//#define C0_CPUX_CFG		((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)			/*!< \brief C0_CPUX_CFG Interface register set access pointer */
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


/* CMSIS compiler specific defines */
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((__noreturn__))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#endif


// https://github.com/yinglangli/rt-thread/blob/514be9cc47420ff970ae9bcba19d071f5293ea5c/bsp/hifive1/freedom-e-sdk/bsp/env/encoding.h
// https://github.com/yinglangli/rt-thread/blob/514be9cc47420ff970ae9bcba19d071f5293ea5c/libcpu/risc-v/common/riscv-ops.h
// https://github.com/Ouyancheng/FlatHeadBro/blob/c33df09f9f79523f51eabc7404e1eef35c36afa9/modules/c906/include/mcsr-ext.h
// https://github.com/Ouyancheng/FlatHeadBro/blob/c33df09f9f79523f51eabc7404e1eef35c36afa9/modules/c906/include/cache.h

#if defined(__riscv_zicsr)
#include "riscv_csr.h"
#endif

__STATIC_INLINE void __DMB(void)
{
  //__asm volatile ("dmb 0xF":::"memory");
}

__STATIC_INLINE void __WFI(void)
{
	__asm volatile ("wfi":::);
}

__STATIC_INLINE void __NOP(void)
{
	__asm volatile ("nop":::);
}

/** \brief  Enable Floating Point Unit
 */
__STATIC_INLINE void __FPU_Enable(void)
{
	csr_set_bits_mstatus(0x00006000);	/* MSTATUS_FS = 0x00006000 = Dirty */
 	csr_write_fcsr(0);             		/* initialize rounding mode, undefined at reset */
  __ASM volatile(
			" fcvt.d.w f0, zero \n"
			" fcvt.d.w f1, zero \n"
			" fcvt.d.w f2, zero \n"
			" fcvt.d.w f3, zero \n"
			" fcvt.d.w f4, zero \n"
			" fcvt.d.w f5, zero \n"
			" fcvt.d.w f6, zero \n"
			" fcvt.d.w f7, zero \n"
			" fcvt.d.w f8, zero \n"
			" fcvt.d.w f9, zero \n"
			" fcvt.d.w f10, zero \n"
			" fcvt.d.w f11, zero \n"
			" fcvt.d.w f12, zero \n"
			" fcvt.d.w f13, zero \n"
			" fcvt.d.w f14, zero \n"
			" fcvt.d.w f15, zero \n"
			" fcvt.d.w f16, zero \n"
			" fcvt.d.w f17, zero \n"
			" fcvt.d.w f18, zero \n"
			" fcvt.d.w f19, zero \n"
			" fcvt.d.w f20, zero \n"
			" fcvt.d.w f21, zero \n"
			" fcvt.d.w f22, zero \n"
			" fcvt.d.w f23, zero \n"
			" fcvt.d.w f24, zero \n"
			" fcvt.d.w f25, zero \n"
			" fcvt.d.w f26, zero \n"
			" fcvt.d.w f27, zero \n"
			" fcvt.d.w f28, zero \n"
			" fcvt.d.w f29, zero \n"
			" fcvt.d.w f30, zero \n"
			" fcvt.d.w f31, zero \n"
		  	  : : :
				"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9",
				"f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19",
				"f20", "f21", "f22", "f23", "f24", "f25", "f26", "f27", "f28", "f29",
				"f30", "f31"
  );
}

/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
__STATIC_INLINE int32_t __SSAT(int32_t val, uint32_t sat)
{
  if ((sat >= 1U) && (sat <= 32U))
  {
    const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
    const int32_t min = -1 - max ;
    if (val > max)
    {
      return max;
    }
    else if (val < min)
    {
      return min;
    }
  }
  return val;
}

/**
  \brief   Unsigned Saturate
  \details Saturates an unsigned value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (0..31)
  \return             Saturated value
 */
__STATIC_INLINE uint32_t __USAT(int32_t val, uint32_t sat)
{
  if (sat <= 31U)
  {
    const uint32_t max = ((1U << sat) - 1U);
    if (val > (int32_t)max)
    {
      return max;
    }
    else if (val < 0)
    {
      return 0U;
    }
  }
  return (uint32_t)val;
}

/**
  \brief   Count leading zeros
  \details Counts the number of leading zeros of a data value.
  \param [in]  value  Value to count the leading zeros
  \return             number of leading zeros in value
 */
__STATIC_INLINE uint8_t __CLZ(uint32_t value)
{
  /* Even though __builtin_clz produces a CLZ instruction on ARM, formally
     __builtin_clz(0) is undefined behaviour, so handle this case specially.
     This guarantees ARM-compatible results if happening to compile on a non-ARM
     target, and ensures the compiler doesn't decide to activate any
     optimisations using the logic "value was passed to __builtin_clz, so it
     is non-zero".
     ARM Compiler 6.10 and possibly earlier will optimise this test away, leaving a
     single CLZ instruction.
   */
  if (value == 0U)
  {
    return 32U;
  }
  return __builtin_clz(value);
}


/**
  \brief   Rotate Right in unsigned value (32 bit)
  \details Rotate Right (immediate) provides the value of the contents of a register rotated by a variable number of bits.
  \param [in]    op1  Value to rotate
  \param [in]    op2  Number of Bits to rotate
  \return               Rotated value
 */
__STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U)
  {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}

// See https://locklessinc.com/articles/sat_arithmetic/

__STATIC_FORCEINLINE uint8_t __UADD8_Sat(uint8_t op1, uint8_t op2)
{
	  uint_fast16_t result = (uint_fast16_t) op1 + op2;
	  result |= -(result < op1);

	  return result;
}

__STATIC_FORCEINLINE uint8_t __USUB8_Sat(uint8_t op1, uint8_t op2)
{
	  return op1 <= op2 ?  0 : (op1 - op2);
}

/**
 * Unsigned Saturating Add 8 performs four unsigned 8-bit integer additions,
 * saturates the results to the 8-bit unsigned integer range 0 ≤ x ≤ 256 - 1,
 * and writes the results to the destination register.
 *
 * @param op1
 * @param op2
 * @return
 */
__STATIC_FORCEINLINE uint32_t __UQADD8(uint32_t op1, uint32_t op2)
{
	  uint32_t result =
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 24), 0xFF & (op2 >> 24)) << 24) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 16), 0xFF & (op2 >> 16)) << 16) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 8), 0xFF & (op2 >> 8)) << 8) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 0), 0xFF & (op2 >> 0)) << 0);

	  return (result);
}

/**
 * Unsigned Saturating Subtract 8 performs four unsigned 8-bit integer subtractions,
 * saturates the results to the 8-bit unsigned integer
 * range 0 ≤ x ≤ 256 - 1, and writes the results to the destination register

 * @param op1
 * @param op2
 * @return
 */
__STATIC_FORCEINLINE uint32_t __UQSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result =
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 24), 0xFF & (op2 >> 24)) << 24) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 16), 0xFF & (op2 >> 16)) << 16) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 8), 0xFF & (op2 >> 8)) << 8) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 0), 0xFF & (op2 >> 0)) << 0);

  return (result);
}

#endif /* ARCH_ALLWNR_F133_ALLWNR_T13S3_H_ */
