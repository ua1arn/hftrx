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

// DRAM Space (SYS domain)
#define DRAM_SPACE_SIZE			0x04000000u			/* 64 MB */
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    0U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0000U      /*!< Core revision r0p0 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 0U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 0U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include <stdint.h>
#define __IO volatile

//#include "core_ca.h"
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
	asm volatile ("FENCE" ::: "memory");
}

__STATIC_INLINE void __DSB(void)
{
	asm volatile ("FENCE" ::: "memory");
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

#endif /* ARCH_ALLWNR_F133__H_ */
