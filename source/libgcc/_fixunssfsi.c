/*
 * libgcc/_fixunssfsi.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -O2
 */
#include "vf4ft.h"

int __fixunssfsi(float value)
{
  if (value >= 2147483648.0f) return (int)(value + -2147483648.0f) - 0x80000000;
  return (int)value;
}
