/*
 * newlib/libc/string/strchr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char * strchr(const char *s1, int i)
{
  unsigned char *s = (unsigned char *)s1;
  unsigned char c = (unsigned char)i;
  unsigned long mask, j;
  unsigned long *aligned_addr;

  if (!((long)s & 3)) {
    mask = 0;
    for (j = 0; j < 4; j++)
      mask = (mask << 8) | c;
    aligned_addr = (unsigned long *)s;
    while (!(((*aligned_addr) - 0x01010101) & ~(*aligned_addr) & 0x80808080) &&
           !((((*aligned_addr) ^ mask) - 0x01010101) & ~((*aligned_addr) ^ mask) & 0x80808080))
      aligned_addr++;
    s = (unsigned char *)aligned_addr;
  }
  while (*s && *s != c)
    s++;
  if (*s == c)
    return (char *)s;
  return 0;
}
