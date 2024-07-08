/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_T113S3_ALLWNR_T113X_H_
#define ARCH_ALLWNR_T113S3_ALLWNR_T113X_H_


/**
 * xxxxxsun8iw20
 */

// DRAM Space (SYS domain)
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */

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

#include "core_ca.h"
#include "system_allwnr_v3s.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,
	DMAC_SrcReqI2S_RX = 3,

	DMAC_SrcReqUART0_RX = 6,
	DMAC_SrcReqUART1_RX,
	DMAC_SrcReqUART2_RX,

	DMAC_SrcReqAudioCodec = 15,
	DMAC_SrcReqSS = 16,

	DMAC_SrcReqUSB0_EP1 = 17,
	DMAC_SrcReqUSB0_EP2,
	DMAC_SrcReqUSB0_EP3,
	DMAC_SrcReqUSB0_EP4,

	DMAC_SrcReqSPI_TX = 23
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqI2S_TX = 3,

	DMAC_DstReqUART0_TX = 6,
	DMAC_DstReqUART1_TX,
	DMAC_DstReqUART2_TX,

	DMAC_DstReqAudioCodec = 15,
	DMAC_DstReqSS = 16,

	DMAC_DstReqUSB0_EP1 = 17,
	DMAC_DstReqUSB0_EP2,
	DMAC_DstReqUSB0_EP3,
	DMAC_DstReqUSB0_EP4,

	DMAC_DstReqSPI_TX = 23
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

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T113X_H_ */
