/* $Id$ */
#ifndef TDMI7_GCC_H_INCLUDED
#define TDMI7_GCC_H_INCLUDED

//ensure completion of the invalidation
__attribute__( ( always_inline ) ) static __INLINE void __DSB(void)
{
}

//ensure the ordering of data cache maintenance operations and their effects
__attribute__( ( always_inline ) ) static __INLINE void __DMB(void)
{
}

//ensure instruction fetch path sees new state
__attribute__( ( always_inline ) ) static __INLINE void __ISB(void)
{
}

__attribute__( ( always_inline ) ) static __INLINE void __NOP(void)
{
	__ASM volatile ("nop");
}

#define __NO_RETURN

#endif /* TDMI7_GCC_H_INCLUDED */
