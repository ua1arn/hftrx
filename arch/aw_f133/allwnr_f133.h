/*
 * allwnr_f133.h
 *
 * The D1s features a single RV64GCV core XuanTie C906 from T-Head Semiconductor (subsidiary of Alibaba).
 */

#ifndef ARCH_ALLWNR_F133__H_
#define ARCH_ALLWNR_F133__H_

/*
 * sun20i
 *
 */

// RISC_SYS Related (SYS Domain)
#define RISC_BROM_BASE 			((uintptr_t) 0x06000000)	// RISC Core accesses the brom address: 0x00000000---0x0000FFFF
//#define RISC_CFG_BASE 			0x06010000
//#define RISC_WDG_BASE 			0x06011000
//#define RISC_TIMESTAMP_BASE 	0x06012000

#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */
#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    0U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0000U      /*!< Core revision r0p0 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 0U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 0U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */
#include "system_allwnr_f133.h"

#include "device.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */


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

#if defined (USE_HAL_DRIVER)
 #include "t113s3_hal.h"
#endif /* USE_HAL_DRIVER */


#endif /* ARCH_ALLWNR_F133__H_ */
