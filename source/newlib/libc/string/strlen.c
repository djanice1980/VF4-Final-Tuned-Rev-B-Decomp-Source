/*
 * newlib/libc/string/strlen.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

size_t strlen(const char *s)
{
  const char *start = s;
  unsigned long *aligned_addr;

  if (!((long)s & 3)) {
    aligned_addr = (unsigned long *)s;
    while (!((*aligned_addr - 0x01010101) & ~*aligned_addr & 0x80808080))
      aligned_addr++;
    s = (const char *)aligned_addr;
  }
  while (*s)
    s++;
  return s - start;
}
