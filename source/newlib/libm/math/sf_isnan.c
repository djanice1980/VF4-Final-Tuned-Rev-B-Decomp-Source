/*
 * newlib/libm/math/sf_isnan.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int isnanf(float x)
{
  union { float value; uint word; } u;
  int ix;
  u.value = x;
  ix = u.word;
  ix &= 0x7fffffff;
  ix = 0x7f800000 - ix;
  return (int)((uint)ix >> 31);
}

int isnan(float x)
{
  return isnanf(x);
}
