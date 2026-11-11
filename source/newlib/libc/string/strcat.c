/*
 * newlib/libc/string/strcat.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char * strcat(char *s1, const char *s2)
{
  char *s = s1;
  if (!((long)s1 & (sizeof(long) - 1)))
    {
      unsigned long *aligned_s1 = (unsigned long *)s1;
      while (!((*aligned_s1 - 0x01010101) & ~*aligned_s1 & 0x80808080))
        aligned_s1++;
      s1 = (char *)aligned_s1;
    }
  while (*s1)
    s1++;
  strcpy (s1, s2);
  return s;
}
