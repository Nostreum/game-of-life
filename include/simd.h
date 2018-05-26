#ifndef SIMD_H_
#define SIMD_H_

#include <emmintrin.h>

#define SIMD_ALIGNMENT      16

typedef int int_a  __attribute__ ((aligned(SIMD_ALIGNMENT)));

typedef __m128i vint32;

#define vint32_load2D(tab, i, j)        _mm_load_si128((__m128i*)tab[i][j])

#define vint32_left(a, b)               _mm_shuffle_epi32(_mm_shuffle_epi32(a, b, _MM_SHUFFLE(1, 0, 3, 2)), b, _MM_SHUFFLE(2, 1, 2, 1));
#define vint32_right(a, b)              _mm_shuffle_epi32(a, _mm_shuffle_epi32(a, b, _MM_SHUFFLE(1, 0, 3, 2)), _MM_SHUFFLE(2, 1, 2, 1));

#define vint32_add(a, b)                _mm_add_epi32(a, b)
#define vint32_add3(a, b, c)            vint32_add(a, vint32_add(b, c))

#define vint32_sub(a, b)                _mm_sub_epi32(a, b)

#define vint32_and(a, b)                _mm_and_si128(a, b)

#define vint32_set(a0, a1, a2, a3)      _mm_set_epi32(a0, a1, a2, a3)

#define vint32_hadd(a0)                 _mm_hadd_epi32(a0, a0)

#endif
