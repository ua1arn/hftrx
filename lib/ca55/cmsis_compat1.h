/*
 * new_cmsis_compat.h
 *
 *  Created on: Feb 9, 2026
 *      Author: Gena
 */

#ifndef ARCH_AW_T507_NEW_CMSIS_COMPAT_H_
#define ARCH_AW_T507_NEW_CMSIS_COMPAT_H_


#ifndef   __RESTRICT
  #define __RESTRICT                             __restrict
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((__noreturn__))
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
#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif
#ifndef   __UNALIGNED_UINT16_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT16_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
#endif
#ifndef   __UNALIGNED_UINT32_WRITE
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT32_READ
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpacked"
  #pragma GCC diagnostic ignored "-Wattributes"
  __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
  #pragma GCC diagnostic pop
  #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
#endif


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

#ifndef IRQN_ID_T
#define IRQN_ID_T
/// Interrupt ID number data type
typedef int32_t IRQn_ID_t;
#endif



#define __WFI()    __asm__ volatile ("wfi":::"memory")

#define __WFE()    __asm__ volatile ("wfe":::"memory")

#define __SEV()    __asm__ volatile ("sev")

#endif /* ARCH_AW_T507_NEW_CMSIS_COMPAT_H_ */
