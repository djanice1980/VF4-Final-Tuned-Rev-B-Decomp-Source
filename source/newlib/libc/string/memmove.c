/*
 * newlib/libc/string/memmove.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void * memmove(void *dst, const void *src, size_t len)
{
  char *d = dst;
  const char *s = src;
  size_t n = len;
  long *ad;
  const long *as;

  if (s < d && d < s + len) {
    s += len;
    d += len;
    while (n--) {
      *--d = *--s;
    }
  }
  else {
    if (!(n < 16) && !(((long)s & 3) | ((long)d & 3))) {
      ad = (long *)d;
      as = (const long *)s;
      while (n >= 16) {
        *ad++ = *as++;
        *ad++ = *as++;
        *ad++ = *as++;
        *ad++ = *as++;
        n -= 16;
      }
      while (n >= 4) {
        *ad++ = *as++;
        n -= 4;
      }
      d = (char *)ad;
      s = (const char *)as;
    }
    while (n--) {
      *d++ = *s++;
    }
  }
  return dst;
}
