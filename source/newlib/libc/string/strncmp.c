/*
 * newlib/libc/string/strncmp.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int strncmp(const char *a, const char *b, size_t n)
{
  unsigned long *a1;
  unsigned long *a2;

  if (n == 0)
    return 0;
  if (!(((long)a & 3) | ((long)b & 3))) {
    a1 = (unsigned long *)a;
    a2 = (unsigned long *)b;
    while (n >= 4 && *a1 == *a2) {
      n -= 4;
      if (n == 0 || ((*a1 - 0x01010101) & ~*a1 & 0x80808080))
        return 0;
      a1++;
      a2++;
    }
    a = (char *)a1;
    b = (char *)a2;
  }
  while (n-- > 0 && *a == *b) {
    if (n == 0 || *a == '\0')
      return 0;
    a++;
    b++;
  }
  return (*(unsigned char *)a) - (*(unsigned char *)b);
}
