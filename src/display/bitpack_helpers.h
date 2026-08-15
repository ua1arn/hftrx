/*
 * Copyright (C) 2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef UTIL_BITPACK_HELPERS_H
#define UTIL_BITPACK_HELPERS_H

#include <math.h>
#include <stdbool.h>

//#include "util/macros.h"
//#include "util/u_math.h"

#define ASSERTED
#define ALWAYS_INLINE
#undef UNUSED
#define UNUSED
#define assert(v) do { ASSERT(v); } while (0)
/**
 * Clamp X to [MIN, MAX].
 * This is a macro to allow float, int, unsigned, etc. types.
 * We arbitrarily turn NaN into MIN.
 */
#define CLAMP( X, MIN, MAX )  ( (X)>(MIN) ? ((X)>(MAX) ? (MAX) : (X)) : (MIN) )

/* Syntax sugar occuring frequently in graphics code */
#define SATURATE( X ) CLAMP(X, 0.0f, 1.0f)

/** Minimum of two values: */
#define MIN2( A, B )   ( (A)<(B) ? (A) : (B) )

/** Maximum of two values: */
#define MAX2( A, B )   ( (A)>(B) ? (A) : (B) )

/** Minimum of three values: */
#define MIN3( A, B, C ) ((A) < (B) ? MIN2(A, C) : MIN2(B, C))

/** Maximum of three values: */
#define MAX3( A, B, C ) ((A) > (B) ? MAX2(A, C) : MAX2(B, C))

/** Minimum of four values: */
#define MIN4( A, B, C, D ) ((A) < (B) ? MIN3(A, C, D) : MIN3(B, C, D))

/** Maximum of four values: */
#define MAX4( A, B, C, D ) ((A) > (B) ? MAX3(A, C, D) : MAX3(B, C, D))

/** Align a value to a power of two */
#define ALIGN_POT(x, pot_align) (((x) + (pot_align) - 1) & ~((pot_align) - 1))

/** Checks is a value is a power of two. Does not handle zero. */
#define IS_POT(v) (((v) & ((v) - 1)) == 0)

/** Checks is a value is a power of two. Zero handled. */
#define IS_POT_NONZERO(v) ((v) != 0 && IS_POT(v))

/** Set a single bit */
#define BITFIELD_BIT(b)      (1u << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD_MASK(b)      \
   ((b) == 32 ? (~0u) : BITFIELD_BIT((b) & 31) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD_RANGE(b, count) \
   (BITFIELD_MASK((b) + (count)) & ~BITFIELD_MASK(b))

/** Set a single bit */
#define BITFIELD64_BIT(b)      (UINT64_C(1) << (b))
/** Set all bits up to excluding bit b */
#define BITFIELD64_MASK(b)      \
   ((b) == 64 ? (~UINT64_C(0)) : BITFIELD64_BIT((b) & 63) - 1)
/** Set count bits starting from bit b  */
#define BITFIELD64_RANGE(b, count) \
   (BITFIELD64_MASK((b) + (count)) & ~BITFIELD64_MASK(b))

/* Determine if an uint32_t value is a power of two.
 *
 * \note
 * Zero is treated as a power of two.
 */
static inline bool
util_is_power_of_two_or_zero(uint32_t v)
{
   return IS_POT(v);
}

/* Determine if an uint64_t value is a power of two.
 *
 * \note
 * Zero is treated as a power of two.
 */
static inline bool
util_is_power_of_two_or_zero64(uint64_t v)
{
   return IS_POT(v);
}

/* Determine if an uint32_t value is a power of two.
 *
 * \note
 * Zero is \b not treated as a power of two.
 */
static inline bool
util_is_power_of_two_nonzero(uint32_t v)
{
   /* __POPCNT__ is different from HAVE___BUILTIN_POPCOUNT.  The latter
    * indicates the existence of the __builtin_popcount function.  The former
    * indicates that _mm_popcnt_u32 exists and is a native instruction.
    *
    * The other alternative is to use SSE 4.2 compile-time flags.  This has
    * two drawbacks.  First, there is currently no build infrastructure for
    * SSE 4.2 (only 4.1), so that would have to be added.  Second, some AMD
    * CPUs support POPCNT but not SSE 4.2 (e.g., Barcelona).
    */
#ifdef __POPCNT__
   return _mm_popcnt_u32(v) == 1;
#else
   return IS_POT_NONZERO(v);
#endif
}

/* Determine if an uint64_t value is a power of two.
 *
 * \note
 * Zero is \b not treated as a power of two.
 */
static inline bool
util_is_power_of_two_nonzero64(uint64_t v)
{
   return IS_POT_NONZERO(v);
}

/* Determine if an size_t/uintptr_t/intptr_t value is a power of two.
 *
 * \note
 * Zero is \b not treated as a power of two.
 */
static inline bool
util_is_power_of_two_nonzero_uintptr(uintptr_t v)
{
   return IS_POT_NONZERO(v);
}

static inline int64_t
u_intN_max(unsigned bit_size)
{
   ASSERT(bit_size <= 64 && bit_size > 0);
   return INT64_MAX >> (64 - bit_size);
}

static inline int64_t
u_intN_min(unsigned bit_size)
{
   /* On 2's compliment platforms, which is every platform Mesa is likely to
    * every worry about, stdint.h generally calculated INT##_MIN in this
    * manner.
    */
   return (-u_intN_max(bit_size)) - 1;
}

static inline uint64_t
u_uintN_max(unsigned bit_size)
{
   ASSERT(bit_size <= 64 && bit_size > 0);
   return UINT64_MAX >> (64 - bit_size);
}

/**
 * Calc log base 2
 */
static inline unsigned
util_logbase2(unsigned n)
{
#if defined(HAVE___BUILTIN_CLZ)
   return ((sizeof(unsigned) * 8 - 1) - __builtin_clz(n | 1));
#else
   unsigned pos = 0;
   if (n >= 1<<16) { n >>= 16; pos += 16; }
   if (n >= 1<< 8) { n >>=  8; pos +=  8; }
   if (n >= 1<< 4) { n >>=  4; pos +=  4; }
   if (n >= 1<< 2) { n >>=  2; pos +=  2; }
   if (n >= 1<< 1) {           pos +=  1; }
   return pos;
#endif
}

static inline uint64_t
util_logbase2_64(uint64_t n)
{
#if defined(HAVE___BUILTIN_CLZLL)
   return ((sizeof(uint64_t) * 8 - 1) - __builtin_clzll(n | 1));
#else
   uint64_t pos = 0ull;
   if (n >= 1ull<<32) { n >>= 32; pos += 32; }
   if (n >= 1ull<<16) { n >>= 16; pos += 16; }
   if (n >= 1ull<< 8) { n >>=  8; pos +=  8; }
   if (n >= 1ull<< 4) { n >>=  4; pos +=  4; }
   if (n >= 1ull<< 2) { n >>=  2; pos +=  2; }
   if (n >= 1ull<< 1) {           pos +=  1; }
   return pos;
#endif
}

/**
 * Returns the ceiling of log n base 2, and 0 when n == 0. Equivalently,
 * returns the smallest x such that n <= 2**x.
 */
static inline unsigned
util_logbase2_ceil(unsigned n)
{
   if (n <= 1)
      return 0;

   return 1 + util_logbase2(n - 1);
}

static inline uint64_t
util_logbase2_ceil64(uint64_t n)
{
   if (n <= 1)
      return 0;

   return 1ull + util_logbase2_64(n - 1);
}

/**
 * Returns the smallest power of two >= x
 */
static inline unsigned
util_next_power_of_two(unsigned x)
{
#if defined(HAVE___BUILTIN_CLZ)
   if (x <= 1)
       return 1;

   return (1 << ((sizeof(unsigned) * 8) - __builtin_clz(x - 1)));
#else
   unsigned val = x;

   if (x <= 1)
      return 1;

   if (util_is_power_of_two_or_zero(x))
      return x;

   val--;
   val = (val >> 1) | val;
   val = (val >> 2) | val;
   val = (val >> 4) | val;
   val = (val >> 8) | val;
   val = (val >> 16) | val;
   val++;
   return val;
#endif
}

static inline uint64_t
util_next_power_of_two64(uint64_t x)
{
#if defined(HAVE___BUILTIN_CLZLL)
   if (x <= 1)
       return 1;

   return (1ull << ((sizeof(uint64_t) * 8) - __builtin_clzll(x - 1)));
#else
   uint64_t val = x;

   if (x <= 1)
      return 1;

   if (util_is_power_of_two_or_zero64(x))
      return x;

   val--;
   val = (val >> 1)  | val;
   val = (val >> 2)  | val;
   val = (val >> 4)  | val;
   val = (val >> 8)  | val;
   val = (val >> 16) | val;
   val = (val >> 32) | val;
   val++;
   return val;
#endif
}

/**
 * Reverse bits in n
 * Algorithm taken from:
 * http://stackoverflow.com/questions/9144800/c-reverse-bits-in-unsigned-integer
 */
static inline unsigned
util_bitreverse(unsigned n)
{
    n = ((n >> 1) & 0x55555555u) | ((n & 0x55555555u) << 1);
    n = ((n >> 2) & 0x33333333u) | ((n & 0x33333333u) << 2);
    n = ((n >> 4) & 0x0f0f0f0fu) | ((n & 0x0f0f0f0fu) << 4);
    n = ((n >> 8) & 0x00ff00ffu) | ((n & 0x00ff00ffu) << 8);
    n = ((n >> 16) & 0xffffu) | ((n & 0xffffu) << 16);
    return n;
}

#ifdef HAVE_VALGRIND
#include <valgrind.h>
#include <memcheck.h>
#ifndef NDEBUG
#define util_bitpack_validate_value(x) \
   VALGRIND_CHECK_MEM_IS_DEFINED(&(x), sizeof(x))
#endif
#endif

#ifndef util_bitpack_validate_value
#define util_bitpack_validate_value(x)
#endif

ALWAYS_INLINE static uint64_t
util_bitpack_ones(uint32_t start, uint32_t end)
{
   return (UINT64_MAX >> (64 - (end - start + 1))) << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_uint(uint64_t v, uint32_t start, UNUSED uint32_t end)
{
   util_bitpack_validate_value(v);

#ifndef NDEBUG
   const int bits = end - start + 1;
   if (bits < 64) {
      const uint64_t max = u_uintN_max(bits);
      ASSERT(v <= max);
   }
#endif

   return v << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_uint_nonzero(uint64_t v, uint32_t start, uint32_t end)
{
   ASSERT(v != 0ull);
   return util_bitpack_uint(v, start, end);
}

ALWAYS_INLINE static uint64_t
util_bitpack_sint(int64_t v, uint32_t start, uint32_t end)
{
   const int bits = end - start + 1;

   util_bitpack_validate_value(v);

#ifndef NDEBUG
   if (bits < 64) {
      const int64_t min = u_intN_min(bits);
      const int64_t max = u_intN_max(bits);
      ASSERT(min <= v && v <= max);
   }
#endif

   const uint64_t mask = BITFIELD64_MASK(bits);

   return (v & mask) << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_sint_nonzero(int64_t v, uint32_t start, uint32_t end)
{
   ASSERT(v != 0ll);
   return util_bitpack_sint(v, start, end);
}

ALWAYS_INLINE static uint32_t
util_bitpack_float(float v)
{
   util_bitpack_validate_value(v);
   union { float f; uint32_t dw; } x;
   x.f = v;
   return x.dw;
}

ALWAYS_INLINE static uint32_t
util_bitpack_float_nonzero(float v)
{
   ASSERT(v != 0.0f);
   return util_bitpack_float(v);
}

ALWAYS_INLINE static uint64_t
util_bitpack_sfixed(float v, uint32_t start, uint32_t end,
                    uint32_t fract_bits)
{
   util_bitpack_validate_value(v);

   const float factor = (1 << fract_bits);

#ifndef NDEBUG
   const int total_bits = end - start + 1;
   const float min = u_intN_min(total_bits) / factor;
   const float max = u_intN_max(total_bits) / factor;
   ASSERT(min <= v && v <= max);
#endif

   const int64_t int_val = llroundf(v * factor);
   const uint64_t mask = UINT64_MAX >> (64 - (end - start + 1));

   return (int_val & mask) << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_sfixed_clamp(float v, uint32_t start, uint32_t end,
                          uint32_t fract_bits)
{
   util_bitpack_validate_value(v);

   const float factor = (1 << fract_bits);

   const int total_bits = end - start + 1;
   const float min = u_intN_min(total_bits) / factor;
   const float max = u_intN_max(total_bits) / factor;

   const int64_t int_val = llroundf(CLAMP(v, min, max) * factor);
   const uint64_t mask = UINT64_MAX >> (64 - (end - start + 1));

   return (int_val & mask) << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_sfixed_nonzero(float v, uint32_t start, uint32_t end,
                            uint32_t fract_bits)
{
   ASSERT(v != 0.0f);
   return util_bitpack_sfixed(v, start, end, fract_bits);
}

ALWAYS_INLINE static uint64_t
util_bitpack_ufixed(float v, uint32_t start, ASSERTED uint32_t end,
                    uint32_t fract_bits)
{
   util_bitpack_validate_value(v);

   const float factor = (1 << fract_bits);

#ifndef NDEBUG
   const int total_bits = end - start + 1;
   const float min = 0.0f;
   const float max = u_uintN_max(total_bits) / factor;
   ASSERT(min <= v && v <= max);
#endif

   const uint64_t uint_val = llroundf(v * factor);

   return uint_val << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_ufixed_clamp(float v, uint32_t start, ASSERTED uint32_t end,
                          uint32_t fract_bits)
{
   util_bitpack_validate_value(v);

   const float factor = (1 << fract_bits);

   const int total_bits = end - start + 1;
   const float min = 0.0f;
   const float max = u_uintN_max(total_bits) / factor;

   const uint64_t uint_val = llroundf(CLAMP(v, min, max) * factor);

   return uint_val << start;
}

ALWAYS_INLINE static uint64_t
util_bitpack_ufixed_nonzero(float v, uint32_t start, uint32_t end,
                            uint32_t fract_bits)
{
   ASSERT(v != 0.0f);
   return util_bitpack_ufixed(v, start, end, fract_bits);
}


static inline uint32_t
__gen_padded(uint32_t v, uint32_t start, uint32_t end)
{
   unsigned shift = __builtin_ctz(v);
   unsigned odd = v >> (shift + 1);

#ifndef NDEBUG
   assert((v >> shift) & 1);
   assert(shift <= 31);
   assert(odd <= 7);
   assert((end - start + 1) == 8);
#endif

   return util_bitpack_uint(shift | (odd << 5), start, end);
}


#endif /* UTIL_BITPACK_HELPERS_H */
