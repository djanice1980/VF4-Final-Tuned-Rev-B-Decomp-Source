/*
 * newlib/libm/common/sf_scalbn.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float scalbnf(float x, int n)
{
  typedef union { float value; unsigned int word; } float_shape;
  int k;
  int ix;

  do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);
  k = (ix & 0x7f800000) >> 23;
  if (k == 0) {
    if ((ix & 0x7fffffff) == 0) return x;
    x *= 3.355443200e+07f;
    do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);
    k = ((ix & 0x7f800000) >> 23) - 25;
    if (n < -50000) return 1.0e-30f * x;
  }
  if (k == 0xff) return x + x;
  k = k + n;
  if (k > 0xfe) return 1.0e+30f * copysignf(1.0e+30f, x);
  if (k > 0) {
    do { float_shape sf_u; sf_u.word = (ix & 0x807fffff) | (k << 23); x = sf_u.value; } while (0);
    return x;
  }
  if (k <= -25) {
    if (n > 50000)
      return 1.0e+30f * copysignf(1.0e+30f, x);
    else return 1.0e-30f * copysignf(1.0e-30f, x);
  }
  k += 25;
  do { float_shape sf_u; sf_u.word = (ix & 0x807fffff) | (k << 23); x = sf_u.value; } while (0);
  return x * 2.9802322388e-08f;
}

void scalbn(int n, float x)
{
  scalbnf(x,n);
  return;
}
