/*
 * newlib/libc/string/strtok_r.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char *strtok_r(char *s, const char *delim, char **lasts)
{
  char *spanp;
  int c, sc;
  char *tok;

  if (s == 0 && (s = *lasts) == 0)
    return 0;
cont:
  c = *s++;
  for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
    if (c == sc)
      goto cont;
  }
  if (c == 0) {
    *lasts = 0;
    return 0;
  }
  tok = s - 1;
  for (;;) {
    c = *s++;
    spanp = (char *)delim;
    do {
      if ((sc = *spanp++) == c) {
        if (c == 0)
          s = 0;
        else
          s[-1] = 0;
        *lasts = s;
        return tok;
      }
    } while (sc != 0);
  }
}
