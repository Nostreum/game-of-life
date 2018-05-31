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

#define vint32_load2D(tab, i, j)        _mm_load_si128((vint32*)(tab[i]) + (j))
#define vint32_store2D(tab, i, j, val)  _mm_store_si128((vint32*)(tab[i]) + (j), val)

#define vint32_add(a, b)                _mm_add_epi32(a, b)
#define vint32_add3(a, b, c)            vint32_add(a, vint32_add(b, c))
#define vint32_add4(a, b, c, d)         vint32_add(a, vint32_add3(b, c, d))

#define vint32_sub(a, b)                _mm_sub_epi32(a, b)

#define vint32_and(a, b)                _mm_and_si128(a, b)
#define vint32_or(a, b)                 _mm_or_si128(a, b)

#define vint32_not32(a)                 vint32_and(_mm_xor_si128(a, _mm_cmpeq_epi32(vint32_setall(0), vint32_setall(0))), vint32_setall(1))

#define vint32_set(a0, a1, a2, a3)      _mm_set_epi32(a0, a1, a2, a3)
#define vint32_setall(a)                _mm_set1_epi32(a)

#define vint32_hadd(a0)                 _mm_hadd_epi32(a0, a0)
#define vint32_haddf(a0)                vint32_hadd(vint32_hadd(a0))
                                        
#define vint32_left(a, b)               _mm_alignr_epi8(b, a, 12)
#define vint32_right(a, b)              _mm_alignr_epi8(b, a, 4)

#define vint32_left2(a, b)              _mm_alignr_epi8(a, b, 4)
#define vint32_right2(a, b)             _mm_alignr_epi8(a, b, 12)

#define vint32_lt(a, b)                 _mm_cmplt_epi32(a, b)
#define vint32_gt(a, b)                 _mm_cmpgt_epi32(a, b)
#define vint32_eq(a, b)                 _mm_cmpeq_epi32(a, b)

/* SIMD 16-BITS INTEGER VECTORS */

#define vint16_load2D(tab, i, j)        _mm_load_si128((vint16*)(tab[i]) + (j))
#define vint16_store2D(tab, i, j , val) _mm_store_si128((vint16*)(tab[i]) + (j), val)

#define vint16_add(a, b)                _mm_add_epi16(a, b)
#define vint16_add3(a, b, c)            vint16_add(a, vint16_add(b, c))
#define vint16_add4(a, b, c, d)         vint16_add(a, vint16_add3(b, c, d))
#define vint16_add8(a, b, c, d, e, f, g, h) vint16_add(vint16_add4(a, b, c, d), vint16_add4(e, f, g, h))

#define vint16_sub(a, b)                _mm_sub_epi16(a, b)

#define vint16_and(a, b)                _mm_and_si128(a, b)
#define vint16_or(a, b)                 _mm_or_si128(a, b)

#define vint16_not16(a)                 vint16_and(_mm_xor_si128(a, _mm_cmpeq_epi16(vint16_setall(0), vint16_setall(0))), vint16_setall(1))

#define vint16_set\
(a0, a1, a2, a3, a4, a5, a6, a7)        _mm_set_epi16(a0, a1, a2, a3, a4, a5, a6, a7)

#define vint16_setall(a)                _mm_set1_epi16(a)

#define vint16_hadd(a0)                 _mm_hadd_epi16(a0, a0)
#define vint16_haddf(a0)                vint16_hadd(vint16_hadd(a0))
                                        
#define vint16_left(a, b)               _mm_alignr_epi8(b, a, 6)
#define vint16_right(a, b)              _mm_alignr_epi8(b, a, 2)

#define vint16_left2(a, b)              _mm_alignr_epi8(a, b, 2)
#define vint16_right2(a, b)             _mm_alignr_epi8(a, b, 6)

#define vint16_lt(a, b)                 _mm_cmplt_epi16(a, b)
#define vint16_gt(a, b)                 _mm_cmpgt_epi16(a, b)
#define vint16_eq(a, b)                 _mm_cmpeq_epi16(a, b)

#define vint16_sl(a, imm)               _mm_slli_epi16(a, imm)
#define vint16_sr(a, imm)               _mm_srli_epi16(a, imm)

/* SIMD 8-BITS INTEGER VECTORS */

#define vint8_load2D(tab, i, j)        _mm_load_si128((vint8*)(tab[i]) + (j))
#define vint8_store2D(tab, i, j , val) _mm_store_si128((vint8*)(tab[i]) + (j), val)

#define vint8_add(a, b)                _mm_add_epi8(a, b)
#define vint8_add3(a, b, c)            vint8_add(a, vint8_add(b, c))
#define vint8_add4(a, b, c, d)         vint8_add(a, vint8_add3(b, c, d))
#define vint8_add8(a, b, c, d, e, f, g, h) vint8_add(vint8_add4(a, b, c, d), vint8_add4(e, f, g, h))

#define vint8_sub(a, b)                _mm_sub_epi8(a, b)

#define vint8_and(a, b)                _mm_and_si128(a, b)
#define vint8_or(a, b)                 _mm_or_si128(a, b)

#define vint8_not8(a)                 vint8_and(_mm_xor_si128(a, _mm_cmpeq_epi8(vint8_setall(0), vint8_setall(0))), vint8_setall(1))

#define vint8_set\
(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)\
                                        _mm_set_epi8(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)

#define vint8_setall(a)                _mm_set1_epi8(a)

#define vint8_hadd(a0)                 _mm_hadd_epi16(a0, a0)
#define vint8_haddf(a0)                vint8_hadd(vint8_hadd(a0))
                                        
#define vint8_left(a, b)               _mm_alignr_epi8(b, a, 3)
#define vint8_right(a, b)              _mm_alignr_epi8(b, a, 1)

#define vint8_left2(a, b)              _mm_alignr_epi8(a, b, 1)
#define vint8_right2(a, b)             _mm_alignr_epi8(a, b, 3)

#define vint8_lt(a, b)                 _mm_cmplt_epi8(a, b)
#define vint8_gt(a, b)                 _mm_cmpgt_epi8(a, b)
#define vint8_eq(a, b)                 _mm_cmpeq_epi8(a, b)

#define vint8_sl(a, imm)               _mm_slli_epi8(a, imm)
#define vint8_sr(a, imm)               _mm_srli_epi8(a, imm)


#endif
