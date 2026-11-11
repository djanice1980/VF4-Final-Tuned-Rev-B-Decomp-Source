/*
 * newlib/libm/math/ef_sqrt.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float __ieee754_sqrtf(float x)
{
  typedef union { float value; unsigned int word; } float_shape;
  float z;
  int sign = (int)0x80000000;
  unsigned int r;
  int ix, s, q, m, t, i;

  do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);

  if ((ix & 0x7f800000) == 0x7f800000) {
    return x * x + x;
  }

  if (ix <= 0) {
    if (((ix & (~sign))) == 0) return x;
    else if (ix < 0)
      return (x - x) / (x - x);
  }

  m = (ix >> 23);
  if (m == 0) {
    for (i = 0; (ix & 0x00800000) == 0; i++) ix <<= 1;
    m -= i - 1;
  }
  m -= 127;
  ix = (ix & 0x007fffff) | 0x00800000;
  if (m & 1)
    ix += ix;
  m >>= 1;

  ix += ix;
  q = s = 0;
  r = 0x01000000;

  while (r != 0) {
    t = s + r;
    if (t <= ix) {
      s = t + r;
      ix -= t;
      q += r;
    }
    ix += ix;
    r >>= 1;
  }

  if (ix != 0) {
    q += (q & 1);
  }
  ix = (q >> 1) + 0x3f000000;
  ix += (m << 23);
  do { float_shape sf_u; sf_u.word = ix; z = sf_u.value; } while (0);
  return z;
}
