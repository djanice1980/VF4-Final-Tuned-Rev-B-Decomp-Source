/*
 * newlib/libc/string/memchr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void *memchr(const void *s, int c, size_t n)
{
  const unsigned char *src = (const unsigned char *) s;
  unsigned long *asrc;
  unsigned long buffer;
  unsigned long mask;
  unsigned int i, j;

  c &= 0xff;

  if (!(n < 4) && !((long)src & 3))
    {
      asrc = (unsigned long *) src;
      mask = 0;
      for (i = 0; i < 4; i++)
        mask = (mask << 8) + c;

      while (n >= 4)
        {
          buffer = *asrc;
          buffer ^= mask;
          if (((buffer - 0x01010101) & ~buffer & 0x80808080))
            {
              src = (unsigned char *) asrc;
              for (j = 0; j < 4; j++)
                {
                  if (*src == c)
                    return (char *) src;
                  src++;
                }
            }
          n -= 4;
          asrc++;
        }

      src = (unsigned char *) asrc;
    }

  while (n--)
    {
      if (*src == c)
        return (char *) src;
      src++;
    }

  return 0;
}
