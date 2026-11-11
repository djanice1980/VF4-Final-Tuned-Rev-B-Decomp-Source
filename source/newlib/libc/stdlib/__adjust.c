/*
 * newlib/libc/stdlib/__adjust.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float float_huge; /* 0x0c2184cc */
extern int lib_errno; /* 0x0c41eb2c */

float strtod_scale(float *acc, int exponent, int negative)
{
  float v;
  
  if (exponent > 38) {
    lib_errno = 0x22;
    float h = float_huge;
    if (negative != 0) h = -h;
    return h;
  }
  if (exponent < -37) {
    lib_errno = 0x22;
    return 0.0f;
  }
  v = *acc;
  if (negative != 0) v = -v;
  if (exponent == 0) return v;
  if (exponent < 0) return v / pow10_float(abs(exponent));
  return pow10_float(exponent) * v;
}
