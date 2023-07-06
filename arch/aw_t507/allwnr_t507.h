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
 * @brief ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
 */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

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
#define __CORTEX_A                   53U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0000U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "cmsis_t507.h"
//
//#define __IO volatile
//
///* CMSIS compiler specific defines */
//#ifndef   __ASM
//  #define __ASM                                  __asm
//#endif
//#ifndef   __INLINE
//  #define __INLINE                               inline
//#endif
//#ifndef   __FORCEINLINE
//  #define __FORCEINLINE                          __attribute__((always_inline))
//#endif
//#ifndef   __STATIC_INLINE
//  #define __STATIC_INLINE                        static inline
//#endif
//#ifndef   __STATIC_FORCEINLINE
//  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
//#endif
//#ifndef   __NO_RETURN
//  #define __NO_RETURN                            __attribute__((__noreturn__))
//#endif
//#ifndef   CMSIS_DEPRECATED
//  #define CMSIS_DEPRECATED                       __attribute__((deprecated))
//#endif
//#ifndef   __USED
//  #define __USED                                 __attribute__((used))
//#endif
//#ifndef   __WEAK
//  #define __WEAK                                 __attribute__((weak))
//#endif
//#ifndef   __PACKED
//  #define __PACKED                               __attribute__((packed, aligned(1)))
//#endif
//#ifndef   __PACKED_STRUCT
//  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
//#endif
//#ifndef   __UNALIGNED_UINT16_WRITE
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
//#endif
//#ifndef   __UNALIGNED_UINT16_READ
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
//#endif
//#ifndef   __UNALIGNED_UINT32_WRITE
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
//#endif
//#ifndef   __UNALIGNED_UINT32_READ
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
//#endif
//#ifndef   __ALIGNED
//  #define __ALIGNED(x)                           __attribute__((aligned(x)))
//#endif
//#ifndef   __RESTRICT
//  #define __RESTRICT                             __restrict
//#endif
//#ifndef   __COMPILER_BARRIER
//  #define __COMPILER_BARRIER()                   __ASM volatile("":::"memory")
//#endif

#include "core_ca.h"
#include "system_allwnr_t507.h"

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
	DMAC_SrcReqI2S0_RX = 3,
	DMAC_SrcReqI2S1_RX = 4,
	DMAC_SrcReqNAND = 5,
	DMAC_SrcReqUART0_RX = 6,
	DMAC_SrcReqUART1_RX = 7,
	DMAC_SrcReqUART2_RX = 8,
	DMAC_SrcReqUART3_RX = 9,
	DMAC_SrcReqUART4_RX = 10,
	DMAC_SrcReqAudioCodec = 15,
	DMAC_SrcReqUSB0_EP1 = 17,
	DMAC_SrcReqUSB0_EP2 = 18,
	DMAC_SrcReqUSB0_EP3 = 19,
	DMAC_SrcReqUSB0_EP4 = 20,
	DMAC_SrcReqUSB0_EP5 = 21,
	DMAC_SrcReqSPI0_RX = 23,
	DMAC_SrcReqSPI1_RX = 24
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA = 2,
	DMAC_DstReqI2S0_TX = 3,
	DMAC_DstReqI2S1_TX = 4,
	DMAC_DstReqI2S2_TX = 27,
	DMAC_DstReqNAND = 5,
	DMAC_DstReqUART0_TX = 6,
	DMAC_DstReqUART1_TX = 7,
	DMAC_DstReqUART2_TX = 8,
	DMAC_DstReqUART3_TX = 9,
	DMAC_DstReqUART4_TX = 10,
	DMAC_DstReqAudioCodec = 15,
	DMAC_DstReqUSB0_EP1 = 17,
	DMAC_DstReqUSB0_EP2 = 18,
	DMAC_DstReqUSB0_EP3 = 19,
	DMAC_DstReqUSB0_EP4 = 20,
	DMAC_DstReqUSB0_EP5 = 21,
	DMAC_DstReqSPI0_TX = 23,
	DMAC_DstReqSPI1_TX = 24
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

#if 0

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

#endif

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
