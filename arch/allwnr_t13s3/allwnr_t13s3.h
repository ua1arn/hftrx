/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 */

#ifndef ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_
#define ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_

/**
 * @brief STM32MP1XX Interrupt Number Definition, according to the selected device
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
   /******  STM32 specific Interrupt Numbers ****************************************************************************/


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
	ECCU_FERR_IRQn = (80), /*  Peripherial */
	AHB_TIMEOUT_IRQn = (81), /*  Peripherial */
	DMAC_NS_IRQn = (82), /*  Peripherial */
	DMAC_S_IRQn = (83), /*  Peripherial */
	CE_NS_IRQn = (84), /*  Peripherial */
	CE_S_IRQn = (85), /*  Peripherial */
	SPINLOCK_IRQn = (86), /*  Peripherial */
	HSTIME0_IRQn = (87), /*  Peripherial */
	HSTIME1_IRQn = (88), /*  Peripherial */
	GPADC_IRQn = (89), /*  Peripherial */
	THS_IRQn = (90), /*  Peripherial */
	////TIMER0_IRQn = (91), /*  Peripherial */
	////TIMER1_IRQn = (92), /*  Peripherial */
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
	NMI_IRQn = (168), /*  Peripherial */
	PPU_IRQn = (169), /*  Peripherial */
	TWD_IRQn = (170), /*  Peripherial */
	////TIMER0_IRQn = (172), /*  Peripherial */
	////TIMER1_IRQn = (173), /*  Peripherial */
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
//#include "system_stm32mp1xx_A7.h"




#endif /* ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_ */
