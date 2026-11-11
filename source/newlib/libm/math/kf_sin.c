/*
 * newlib/libm/math/kf_sin.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float __kernel_sinf(float x, float y, int iy)
{
  typedef union { float value; unsigned int word; } float_shape;
  float z, r, v;
  int ix;

  do { float_shape gf_u; gf_u.value = x; ix = gf_u.word; } while (0);
  ix &= 0x7fffffff;
  if (ix < 0x32000000)
    {if ((int)x == 0) return x;}
  z = x*x;
  v = z*x;
  r = 8.3333337680e-03+z*(-1.9841270114e-04+z*(2.7557314297e-06+z*(-2.5050759689e-08+z*1.5896910177e-10)));
  if (iy == 0) return x+v*(-1.6666667163e-01+z*r);
  else return x-((z*(5.0000000000e-01*y-v*r)-y)-v*-1.6666667163e-01);
}
