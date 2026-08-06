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

/**
 * Clamp X to [MIN, MAX].
 * This is a macro to allow float, int, unsigned, etc. types.
 * We arbitrarily turn NaN into MIN.
 */
#define CLAMP( X, MIN, MAX )  ( (X)>(MIN) ? ((X)>(MAX) ? (MAX) : (X)) : (MIN) )

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

#endif /* UTIL_BITPACK_HELPERS_H */
