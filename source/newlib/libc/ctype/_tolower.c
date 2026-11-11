/*
 * newlib/libc/ctype/_tolower.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int _tolower(int c)
{
  int upper;
  
  upper = isupper(c);
  if (!(0 == upper)) {
    return 0x20 + c;
  }
  return c;
}
