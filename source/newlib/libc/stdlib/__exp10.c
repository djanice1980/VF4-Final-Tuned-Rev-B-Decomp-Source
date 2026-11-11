/*
 * newlib/libc/stdlib/__exp10.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float pow10_small[5]; /* 0x0c2184b8 */

float pow10_float(uint exponent)
{
  float result;
  float half;
  
  if (exponent < 5) {
    result = pow10_small[exponent];
  }
  else if (!((exponent & 1) == 0)) {
    result = (float)pow10_float(exponent - 1);
    result = result * 10.0;
  }
  else {
    half = (float)pow10_float(exponent >> 1);
    result = (float)pow10_float(exponent >> 1);
    result = result * half;
  }
  return result;
}
