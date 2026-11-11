/*
 * newlib/libm/math/kf_cos.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float __kernel_cosf(float x, float y)
{
  typedef union { float value; uint word; } float_shape;
  float a, hz, z, r, qx;
  int ix;

  do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);
  ix &= 0x7fffffff;
  if (ix < 0x32000000) {
    if (((int)x) == 0) return 1.0;
  }
  z = x * x;
  r = z * (4.1666667908e-02 + z * (-1.3888889225e-03 + z * (2.4801587642e-05 + z * (-2.7557314297e-07 + z * (2.0875723372e-09 + z * -1.1359647598e-11)))));
  if (ix < 0x3e99999a)
    return 1.0 - ((float)0.5 * z - (z * r - x * y));
  else {
    if (ix > 0x3f480000) {
      qx = (float)0.28125;
    }
    else {
      do { float_shape sf_u; sf_u.word = ix - 0x01000000; qx = sf_u.value; } while (0);
    }
    hz = (float)0.5 * z - qx;
    a = 1.0 - qx;
    return a - (hz - (z * r - x * y));
  }
}
