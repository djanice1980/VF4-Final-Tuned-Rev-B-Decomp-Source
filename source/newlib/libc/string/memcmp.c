/*
 * newlib/libc/string/memcmp.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int memcmp(const void *s1, const void *s2, size_t n)
{
  unsigned char *a = (unsigned char *)s1;
  unsigned char *b = (unsigned char *)s2;
  unsigned long *a1;
  unsigned long *a2;
  if (!(n < 4) && !(((long)a | (long)b) & 3)) {
    a1 = (unsigned long *)a;
    a2 = (unsigned long *)b;
    while (n >= 4) {
      if (*a1 != *a2) break;
      a1++;
      a2++;
      n -= 4;
    }
    a = (unsigned char *)a1;
    b = (unsigned char *)a2;
  }
  while (n--) {
    if (*a != *b) return *a - *b;
    a++;
    b++;
  }
  return 0;
}
