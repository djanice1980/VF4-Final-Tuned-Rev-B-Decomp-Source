/*
 * newlib/libm/math/sf_floor.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float floorf(float x)
{
  typedef union { float value; unsigned int word; } float_shape;
  int i0;
  int j0;
  unsigned int i;

  do { float_shape gf_u; gf_u.value = x; i0 = gf_u.word; } while (0);
  j0 = ((i0 >> 23) & 0xff) - 0x7f;
  if (j0 < 23) {
    if (j0 < 0) {
      if (1.0e30f + x > 0.0f) {
        if (i0 >= 0) {
          i0 = 0;
        }
        else if ((i0 & 0x7fffffff) != 0) {
          i0 = 0xbf800000;
        }
      }
    }
    else {
      i = 0x007fffff >> j0;
      if ((i0 & i) == 0) return x;
      if (1.0e30f + x > 0.0f) {
        if (i0 < 0) i0 += 0x00800000 >> j0;
        i0 &= ~i;
      }
    }
  }
  else {
    if (j0 == 0x80) return x + x;
    else return x;
  }
  do { float_shape sf_u; sf_u.word = i0; x = sf_u.value; } while (0);
  return x;
}

float floor(float x)
{
  return floorf(x);
}
