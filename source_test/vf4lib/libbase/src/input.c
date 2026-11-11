/*
 * vf4lib/libbase/src/input.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

uint input_merged_held(uint mask)
{
  return mask & input_merged.now;
}
