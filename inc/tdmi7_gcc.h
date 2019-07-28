/* $Id$ */
#ifndef TDMI7_GCC_H_INCLUDED
#define TDMI7_GCC_H_INCLUDED

__attribute__( ( always_inline ) ) static __INLINE void __DSB(void)
{
}

__attribute__( ( always_inline ) ) static __INLINE void __ISB(void)
{
}

__attribute__( ( always_inline ) ) static __INLINE void __NOP(void)
{
	__ASM volatile ("nop");
}

#endif /* TDMI7_GCC_H_INCLUDED */
