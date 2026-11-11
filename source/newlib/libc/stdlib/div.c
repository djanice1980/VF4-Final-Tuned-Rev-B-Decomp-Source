/*
 * newlib/libc/stdlib/div.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

div_t div(int num, int denom)
{
  div_t r;

  r.quot = num / denom;
  r.rem = num % denom;
  if (num >= 0 && r.rem < 0) {
    ++r.quot;
    r.rem -= denom;
  }
  return r;
}
