#ifndef INC_CORE_RV64_H_
#define INC_CORE_RV64_H_

#include <stdint.h>
/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< \brief Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< \brief Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< \brief Defines 'write only' permissions */
#define     __IO    volatile             /*!< \brief Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*!< \brief Defines 'read only' structure member permissions */
#define     __OM     volatile            /*!< \brief Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*!< \brief Defines 'read / write' structure member permissions */
#define RESERVED(N, T) T RESERVED##N;    // placeholder struct members used for "reserved" areas

//#include "core_ca.h"
#include "cmsis_gcc.h"


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

#ifdef __cplusplus
 extern "C" {
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


#ifdef __cplusplus
 }
#endif

#endif /* INC_CORE_RV64_H_ */
