/*
 * allwnr_a64.h
 *
 * Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved.
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_A64_H_
#define ARCH_ALLWNR_A64_H_

/**
 * @brief ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
 *
 * sun50iw1
 */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */

// CPUX related
// Address (It is for Cluster CPU)
//#define CPU_SUBSYS_CTRL_BASE	((uintptr_t) 0x08100000)
#define TimeStamp_STA_BASE		((uintptr_t) 0x08110000)
#define TimeStamp_CTRL_BASE		((uintptr_t) 0x08120000)
#define IDC_BASE				((uintptr_t) 0x08130000)
//#define C0_CPUX_CFG_BASE		((uintptr_t) 0x09010000)
#define C0_CPUX_MBIST_BASE		((uintptr_t) 0x09020000)	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                   53U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0004U      /*!< Core revision r0p4 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "device.h"

#include "system_allwnr_a64.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,

	DMAC_SrcReqI2S0_RX = 3,
	DMAC_SrcReqI2S1_RX,

	DMAC_SrcReqNAND = 5,

	DMAC_SrcReqUART0_RX = 6,
	DMAC_SrcReqUART1_RX,
	DMAC_SrcReqUART2_RX,
	DMAC_SrcReqUART3_RX,
	DMAC_SrcReqUART4_RX,

	DMAC_SrcReqAudioCodec = 15,

	DMAC_SrcReqUSB0_EP1 = 17,
	DMAC_SrcReqUSB0_EP2,
	DMAC_SrcReqUSB0_EP3,
	DMAC_SrcReqUSB0_EP4,
	DMAC_SrcReqUSB0_EP5,

	DMAC_SrcReqSPI0_RX = 23,
	DMAC_SrcReqSPI1_RX
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA = 2,

	DMAC_DstReqI2S0_TX = 3,
	DMAC_DstReqI2S1_TX,
	DMAC_DstReqI2S2_TX = 27,

	DMAC_DstReqNAND = 5,

	DMAC_DstReqUART0_TX = 6,
	DMAC_DstReqUART1_TX,
	DMAC_DstReqUART2_TX,
	DMAC_DstReqUART3_TX,
	DMAC_DstReqUART4_TX,

	DMAC_DstReqAudioCodec = 15,

	DMAC_DstReqUSB0_EP1 = 17,
	DMAC_DstReqUSB0_EP2,
	DMAC_DstReqUSB0_EP3,
	DMAC_DstReqUSB0_EP4,
	DMAC_DstReqUSB0_EP5,

	DMAC_DstReqSPI0_TX = 23,
	DMAC_DstReqSPI1_TX
};

typedef enum {
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

#endif /* ARCH_ALLWNR_A64_H_ */
