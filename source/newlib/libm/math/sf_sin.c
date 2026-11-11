/*
 * newlib/libm/math/sf_sin.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float sinf(float x)
{
  typedef union { float value; uint word; } float_shape;
  float y[2], z = 0.0;
  int n, ix;
  do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);
  ix &= 0x7fffffff;
  if (ix <= 0x3f490fd8) return __kernel_sinf(x, z, 0);
  else if (!(ix < 0x7f800000)) return x - x;
  else {
    n = __ieee754_rem_pio2f(y, x, z);
    switch (n & 3) {
    case 0: return __kernel_sinf(y[0], y[1], 1);
    case 1: return __kernel_cosf(y[0], y[1]);
    case 2: return -__kernel_sinf(y[0], y[1], 1);
    default:
      return -__kernel_cosf(y[0], y[1]);
    }
  }
}

float sin(float x)
{
  return sinf(x);
}
