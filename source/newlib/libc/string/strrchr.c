/*
 * newlib/libc/string/strrchr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char * strrchr(const char *s, int i)
{
  char *last = 0;
  char c = i;
  while (*s) {
    if (*s == c) last = s;
    s++;
  }
  if (*s == c) last = s;
  return last;
}
