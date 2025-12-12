/*
 * allwnr_t507.h
 *
 * Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved.
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_T507_H_
#define ARCH_ALLWNR_T507_H_

/**
 * Allwinner T507/T517/T507-H/T517-H
 *
 * @brief ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
 *
 * sun50iw9p1
 */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                   53U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0004U      /*!< Core revision r0p4 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "device.h"

#include "system_allwnr_t507.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

typedef enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,

	DMAC_SrcReqAHUB_drqr0_RX = 3,
	DMAC_SrcReqAHUB_drqr1_RX,
	DMAC_SrcReqAHUB_drqr2_RX,

	DMAC_SrcReqDMIC = 7,
	DMAC_SrcReqNAND = 10,
	DMAC_SrcReqGPADC = 12,

	DMAC_SrcReqUART0_RX = 14,
	DMAC_SrcReqUART1_RX,
	DMAC_SrcReqUART2_RX,
	DMAC_SrcReqUART3_RX,
	DMAC_SrcReqUART4_RX,
	DMAC_SrcReqUART5_RX,

	DMAC_SrcReqSPI0_RX = 22,
	DMAC_SrcReqSPI1_RX,

	DMAC_SrcReqUSB0_EP1 = 30,
	DMAC_SrcReqUSB0_EP2,
	DMAC_SrcReqUSB0_EP3,
	DMAC_SrcReqUSB0_EP4,
	DMAC_SrcReqUSB0_EP5,	// ?

	DMAC_SrcReqTWI0_RX = 43,
	DMAC_SrcReqTWI1_RX,
	DMAC_SrcReqTWI2_RX,
	DMAC_SrcReqTWI3_RX,
	DMAC_SrcReqTWI4_RX,

	DMAC_SrcReqS_TWI0_RX = 48	// S_TWI0
} DMAC_SrcReq_TypeDef;

typedef enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA = 2,

	DMAC_DstReqAHUB_drqt0_TX = 3,
	DMAC_DstReqAHUB_drqt1_TX,
	DMAC_DstReqAHUB_drqt2_TX,

	DMAC_DstReqAudioCodec = 6,

	DMAC_DstReqNAND = 10,

	DMAC_DstReqUART0_TX = 14,
	DMAC_DstReqUART1_TX,
	DMAC_DstReqUART2_TX,
	DMAC_DstReqUART3_TX,
	DMAC_DstReqUART4_TX,
	DMAC_DstReqUART5_TX,

	DMAC_DstReqSPI0_TX = 22,
	DMAC_DstReqSPI1_TX,

	DMAC_DstReqUSB0_EP1 = 30,
	DMAC_DstReqUSB0_EP2,
	DMAC_DstReqUSB0_EP3,
	DMAC_DstReqUSB0_EP4,
	DMAC_DstReqUSB0_EP5,	// ?

	DMAC_DstReqTWI0_TX = 43,
	DMAC_DstReqTWI1_TX,
	DMAC_DstReqTWI2_TX,
	DMAC_DstReqTWI3_TX,
	DMAC_DstReqTWI4_TX,

	DMAC_SrcReqS_TWI0_TX = 48	// S_TWI0
} DMAC_DstReq_TypeDef;

typedef enum
{
	GPIO_CFG_IN  = 0x00,
	GPIO_CFG_OUT = 0x01,
	GPIO_CFG_AF2 = 0x02,
	GPIO_CFG_AF3 = 0x03,
	GPIO_CFG_AF4 = 0x04,
	GPIO_CFG_AF5 = 0x05,
	GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
	GPIO_CFG_IODISABLE = 0x07,
} GPIOMode_TypeDef;

typedef enum
{
	GPIO_DRV_0 = 0x00,	// Level 0 minimal
	GPIO_DRV_1 = 0x01,	// Level 1
	GPIO_DRV_2 = 0x02,	// Level 2
	GPIO_DRV_3 = 0x03	// LEvel 3 maximal
} GPIODrv_TypeDef;

typedef enum
{
	GPIO_PULL_NONE = 0x00,	// Pull-up/down disable
	GPIO_PULL_UP = 0x01,	// Pull-up
	GPIO_PULL_DOWN = 0x02	// Pull-down. Other codes reserved
} GPIOPull_TypeDef;

#if defined (USE_HAL_DRIVER)
  #include "t113s3_hal.h"
#endif /* USE_HAL_DRIVER */

#endif /* ARCH_ALLWNR_T507_H_ */
