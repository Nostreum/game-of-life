#ifndef SIMD_H_
#define SIMD_H_

#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>

#define SIMD_ALIGNMENT      16

typedef int int_a  __attribute__ ((aligned(SIMD_ALIGNMENT)));

typedef __m128i vint32;
typedef __m128i vint16;
typedef __m128i vint8;

/* SIMD 32-BITS INTEGER VECTORS */

#define vint32_load2D(tab, i, j)        _mm_load_si128((vint32*)(tab[i])+(j))
#define vint32_store2D(tab, i, j, val)  _mm_store_si128((vint32*)(tab[i])+(j), val)

#define vint32_add(a, b)                _mm_add_epi32(a, b)
#define vint32_add3(a, b, c)            vint32_add(a, vint32_add(b, c))
#define vint32_add4(a, b, c, d)         vint32_add(a, vint32_add3(b, c, d))

#define vint32_sub(a, b)                _mm_sub_epi32(a, b)

#define vint32_and(a, b)                _mm_and_si128(a, b)
#define vint32_or(a, b)                 _mm_or_si128(a, b)

#define vint32_not(a)                   _mm_xor_si128(a, _mm_cmpeq_epi32(a, a))

#define vint32_set(a0, a1, a2, a3)      _mm_set_epi32(a0, a1, a2, a3)
#define vint32_setall(a)                _mm_set1_epi32(a)

#define vint32_hadd(a0)                 _mm_hadd_epi32(a0, a0)
#define vint32_haddf(a0)                vint32_hadd(vint32_hadd(a0))
                                        
#define vint32_left(a, b)               _mm_alignr_epi8(a, b, 8)
#define vint32_right(a, b)              _mm_alignr_epi8(b, a, 8)

#define vint32_left2(a, b)              _mm_alignr_epi8(a, b, 16)
#define vint32_right2(a, b)             _mm_alignr_epi8(b, a, 16)

#define vint32_lt(a, b)                 _mm_cmplt_epi32(a, b)
#define vint32_gt(a, b)                 _mm_cmpgt_epi32(a, b)
#define vint32_eq(a, b)                 _mm_cmpeq_epi32(a, b)


/* SIMD 16-BITS INTEGER VECTORS */

/* SIMD 8-BITS INTEGER VECTORS */

#endif
