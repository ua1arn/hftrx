#ifndef _DO_DIV_H_
#define _DO_DIV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <strings.h> //fls, __always_inline
#include <stdbool.h> //bool

#define BITS_PER_LONG 32 /* ����� ��� � long �� 32-������ ���������� */

#define likely(x)       __builtin_expect((x),1)

#if BITS_PER_LONG == 32
static __always_inline int fls64(uint64_t x)
{
	uint32_t h = x >> 32;
	if (h)
		return fls(h) + 32;
	return fls(x);
}
#elif BITS_PER_LONG == 64
static __always_inline int fls64(uint64_t x)
{
	if (x == 0)
		return 0;
	return __fls(x) + 1;
}
#endif

/**
 * is_power_of_2() - check if a value is a power of two
 * @n: the value to check
 *
 * Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 * Return: true if @n is a power of 2, otherwise false.
 */
static inline __attribute__((const))
bool is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

/*
 * non-constant log of base 2 calculators
 * - the arch may override these in asm/bitops.h if they can be implemented
 *   more efficiently than using fls() and fls64()
 * - the arch is not required to handle n==0 if implementing the fallback
 */
#ifndef CONFIG_ARCH_HAS_ILOG2_U32
static __always_inline __attribute__((const))
int __ilog2_u32(uint32_t n)
{
	return fls(n) - 1;
}
#endif

#ifndef CONFIG_ARCH_HAS_ILOG2_U64
static __always_inline __attribute__((const))
int __ilog2_u64(uint64_t n)
{
	return fls64(n) - 1;
}
#endif

/*
 * Default C implementation for __arch_xprod_64()
 *
 * Prototype: uint64_t __arch_xprod_64(const uint64_t m, uint64_t n, bool bias)
 * Semantic:  retval = ((bias ? m : 0) + m * n) >> 64
 *
 * The product is a 128-bit value, scaled down to 64 bits.
 * Assuming constant propagation to optimize away unused conditional code.
 * Architectures may provide their own optimized assembly implementation.
 */
static inline uint64_t __arch_xprod_64(const uint64_t m, uint64_t n, bool bias)
{
	uint32_t m_lo = m;
	uint32_t m_hi = m >> 32;
	uint32_t n_lo = n;
	uint32_t n_hi = n >> 32;
	uint64_t res;
	uint32_t res_lo, res_hi, tmp;

	if (!bias) {
		res = ((uint64_t)m_lo * n_lo) >> 32;
	} else if (!(m & ((1ULL << 63) | (1ULL << 31)))) {
		/* there can't be any overflow here */
		res = (m + (uint64_t)m_lo * n_lo) >> 32;
	} else {
		res = m + (uint64_t)m_lo * n_lo;
		res_lo = res >> 32;
		res_hi = (res_lo < m_hi);
		res = res_lo | ((uint64_t)res_hi << 32);
	}

	if (!(m & ((1ULL << 63) | (1ULL << 31)))) {
		/* there can't be any overflow here */
		res += (uint64_t)m_lo * n_hi;
		res += (uint64_t)m_hi * n_lo;
		res >>= 32;
	} else {
		res += (uint64_t)m_lo * n_hi;
		tmp = res >> 32;
		res += (uint64_t)m_hi * n_lo;
		res_lo = res >> 32;
		res_hi = (res_lo < tmp);
		res = res_lo | ((uint64_t)res_hi << 32);
	}

	res += (uint64_t)m_hi * n_hi;

	return res;
}

/* Not needed on 64bit architectures */
#if BITS_PER_LONG == 32

#ifndef __div64_32
uint32_t __attribute__((weak)) __div64_32(uint64_t *n, uint32_t base)
{
	uint64_t rem = *n;
	uint64_t b = base;
	uint64_t res, d = 1;
	uint32_t high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (uint64_t) high << 32;
		rem -= (uint64_t) (high*base) << 32;
	}

	while ((int64_t)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}
#endif

#endif

/**
 * ilog2 - log base 2 of 32-bit or a 64-bit unsigned value
 * @n: parameter
 *
 * constant-capable log of base 2 calculation
 * - this can be used to initialise global variables from constant data, hence
 * the massive ternary operator construction
 *
 * selects the appropriately-sized optimised version depending on sizeof(n)
 */
#define ilog2(n) \
( \
	__builtin_constant_p(n) ?	\
	((n) < 2 ? 0 :			\
	 63 - __builtin_clzll(n)) :	\
	(sizeof(n) <= 4) ?		\
	__ilog2_u32(n) :		\
	__ilog2_u64(n)			\
 )

#ifndef __div64_const32_is_OK
#define __div64_const32_is_OK (__GNUC__ >= 4)
#endif

#define __div64_const32(n, ___b)					\
({									\
	/*								\
	 * Multiplication by reciprocal of b: n / b = n * (p / b) / p	\
	 *								\
	 * We rely on the fact that most of this code gets optimized	\
	 * away at compile time due to constant propagation and only	\
	 * a few multiplication instructions should remain.		\
	 * Hence this monstrous macro (static inline doesn't always	\
	 * do the trick here).						\
	 */								\
	uint64_t ___res, ___x, ___t, ___m, ___n = (n);			\
	uint32_t ___p, ___bias;						\
									\
	/* determine MSB of b */					\
	___p = 1 << ilog2(___b);					\
									\
	/* compute m = ((p << 64) + b - 1) / b */			\
	___m = (~0ULL / ___b) * ___p;					\
	___m += (((~0ULL % ___b + 1) * ___p) + ___b - 1) / ___b;	\
									\
	/* one less than the dividend with highest result */		\
	___x = ~0ULL / ___b * ___b - 1;					\
									\
	/* test our ___m with res = m * x / (p << 64) */		\
	___res = ((___m & 0xffffffff) * (___x & 0xffffffff)) >> 32;	\
	___t = ___res += (___m & 0xffffffff) * (___x >> 32);		\
	___res += (___x & 0xffffffff) * (___m >> 32);			\
	___t = (___res < ___t) ? (1ULL << 32) : 0;			\
	___res = (___res >> 32) + ___t;					\
	___res += (___m >> 32) * (___x >> 32);				\
	___res /= ___p;							\
									\
	/* Now sanitize and optimize what we've got. */			\
	if (~0ULL % (___b / (___b & -___b)) == 0) {			\
		/* special case, can be simplified to ... */		\
		___n /= (___b & -___b);					\
		___m = ~0ULL / (___b / (___b & -___b));			\
		___p = 1;						\
		___bias = 1;						\
	} else if (___res != ___x / ___b) {				\
		/*							\
		 * We can't get away without a bias to compensate	\
		 * for bit truncation errors.  To avoid it we'd need an	\
		 * additional bit to represent m which would overflow	\
		 * a 64-bit variable.					\
		 *							\
		 * Instead we do m = p / b and n / b = (n * m + m) / p.	\
		 */							\
		___bias = 1;						\
		/* Compute m = (p << 64) / b */				\
		___m = (~0ULL / ___b) * ___p;				\
		___m += ((~0ULL % ___b + 1) * ___p) / ___b;		\
	} else {							\
		/*							\
		 * Reduce m / p, and try to clear bit 31 of m when	\
		 * possible, otherwise that'll need extra overflow	\
		 * handling later.					\
		 */							\
		uint32_t ___bits = -(___m & -___m);			\
		___bits |= ___m >> 32;					\
		___bits = (~___bits) << 1;				\
		/*							\
		 * If ___bits == 0 then setting bit 31 is  unavoidable.	\
		 * Simply apply the maximum possible reduction in that	\
		 * case. Otherwise the MSB of ___bits indicates the	\
		 * best reduction we should apply.			\
		 */							\
		if (!___bits) {						\
			___p /= (___m & -___m);				\
			___m /= (___m & -___m);				\
		} else {						\
			___p >>= ilog2(___bits);			\
			___m >>= ilog2(___bits);			\
		}							\
		/* No bias needed. */					\
		___bias = 0;						\
	}								\
									\
	/*								\
	 * Now we have a combination of 2 conditions:			\
	 *								\
	 * 1) whether or not we need to apply a bias, and		\
	 *								\
	 * 2) whether or not there might be an overflow in the cross	\
	 *    product determined by (___m & ((1 << 63) | (1 << 31))).	\
	 *								\
	 * Select the best way to do (m_bias + m * n) / (1 << 64).	\
	 * From now on there will be actual runtime code generated.	\
	 */								\
	___res = __arch_xprod_64(___m, ___n, ___bias);			\
									\
	___res /= ___p;							\
})

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({				\
	uint32_t __base = (base);			\
	uint32_t __rem;					\
	(void)(((typeof((n)) *)0) == ((uint64_t *)0));	\
	if (__builtin_constant_p(__base) &&		\
	    is_power_of_2(__base)) {			\
		__rem = (n) & (__base - 1);		\
		(n) >>= ilog2(__base);			\
	} else if (__div64_const32_is_OK &&		\
		   __builtin_constant_p(__base) &&	\
		   __base != 0) {			\
		uint32_t __res_lo, __n_lo = (n);	\
		(n) = __div64_const32(n, __base);	\
/* the remainder can be computed with 32-bit regs */	\
		__res_lo = (n);				\
		__rem = __n_lo - __res_lo * __base;	\
	} else if (likely(((n) >> 32) == 0)) {		\
		__rem = (uint32_t)(n) % __base;		\
		(n) = (uint32_t)(n) / __base;		\
	} else 						\
		__rem = __div64_32(&(n), __base);	\
	__rem;						\
 })

#ifdef __cplusplus
}
#endif

#endif
