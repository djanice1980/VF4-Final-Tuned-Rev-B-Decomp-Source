/*
 * zlib/adler32.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

uLong adler32(uLong adler, const Bytef *buf, uInt len)
{
  unsigned long s1 = adler & 0xffff;
  unsigned long s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == 0) return 1L;

  while (len > 0) {
    k = len < 5552 ? len : 5552;
    len -= k;
    while (k >= 16) {
      {s1 += buf[0]; s2 += s1;} {s1 += buf[0+1]; s2 += s1;}
      {s1 += buf[0+2]; s2 += s1;} {s1 += buf[0+2+1]; s2 += s1;}
      {s1 += buf[0+4]; s2 += s1;} {s1 += buf[0+4+1]; s2 += s1;}
      {s1 += buf[0+4+2]; s2 += s1;} {s1 += buf[0+4+2+1]; s2 += s1;}
      {s1 += buf[8]; s2 += s1;} {s1 += buf[8+1]; s2 += s1;}
      {s1 += buf[8+2]; s2 += s1;} {s1 += buf[8+2+1]; s2 += s1;}
      {s1 += buf[8+4]; s2 += s1;} {s1 += buf[8+4+1]; s2 += s1;}
      {s1 += buf[8+4+2]; s2 += s1;} {s1 += buf[8+4+2+1]; s2 += s1;}
      buf += 16;
      k -= 16;
    }
    if (k != 0) do {
      s1 += *buf++;
      s2 += s1;
    } while (--k);
    s1 %= 65521L;
    s2 %= 65521L;
  }
  return (s2 << 16) | s1;
}
