/*
 * newlib/libc/stdlib/__ten_mu.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int __ten_mul(float *acc, int digit)
{
  *acc = *acc * 10.0 + (float)digit;
  return 0;
}
