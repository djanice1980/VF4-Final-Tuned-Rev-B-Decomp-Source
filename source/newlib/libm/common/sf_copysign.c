/*
 * newlib/libm/common/sf_copysign.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float copysignf(float x, float y)
{
  union { float f; uint i; } ux, uy;
  ux.f = x;
  uy.f = y;
  ux.i = (ux.i & 0x7fffffff) | (uy.i & 0x80000000);
  return ux.f;
}

float copysign(float x, float y)
{
  return copysignf(x,y);
}
