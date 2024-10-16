/*
 * allwnr_h3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_H3_H_
#define ARCH_ALLWNR_H3_H_


/**
 * sun8iw20
 */

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */
#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */

//#define	GIC_DISTRIBUTOR_BASE	 ((uintptr_t) 0x03021000)
//#define	GIC_INTERFACE_BASE	 ((uintptr_t) 0x03022000)

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "device.h"

#include "system_allwnr_h3.h"

//typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,
	DMAC_SrcReqOWA_RX = 2,
	DMAC_SrcReqI2S1_RX = 4,
	DMAC_SrcReqI2S2_RX,
	DMAC_SrcReqAudioCodec = 7,
	DMAC_SrcReqDMIC = 8,
	DMAC_SrcReqGPADC = 12,
	DMAC_SrcReqTPADC = 13,
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
	DMAC_SrcReqUSB0_EP5,
	DMAC_SrcReqTWI0_RX = 43,

	DMAC_SrcReqTWI1_RX = 44,
	DMAC_SrcReqTWI2_RX,
	DMAC_SrcReqTWI3_RX
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
	DMAC_DstReqUSB0_EP5,

	DMAC_DstReqLEDC = 42,

	DMAC_DstReqTWI0_TX = 43,
	DMAC_DstReqTWI1_TX,
	DMAC_DstReqTWI2_TX,
	DMAC_DstReqTWI3_TX
};

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

#endif /* ARCH_ALLWNR_H3_H_ */
