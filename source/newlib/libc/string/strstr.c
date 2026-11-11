/*
 * newlib/libc/string/strstr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char * strstr(const char *s, const char *find)
{
  int i;
  if (*s == '\0') {
    if (*find != '\0') return 0;
    return s;
  }
  while (*s != '\0') {
    for (i = 0; ; i++) {
      if (find[i] == '\0') return s;
      if (find[i] != s[i]) break;
    }
    s++;
  }
  return 0;
}
