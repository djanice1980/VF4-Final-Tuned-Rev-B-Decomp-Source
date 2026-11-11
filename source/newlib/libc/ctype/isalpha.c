/*
 * newlib/libc/ctype/isalpha.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int isalpha(int c)
{
  return (int)(char)(ctype_table + 1)[c] & 3;
}
