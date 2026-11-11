/*
 * newlib/libc/string/strncpy.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

char * strncpy(char *dst0, const char *src0, size_t count)
{
  char *dst = dst0;
  const char *src = src0;
  long *aligned_dst;
  const long *aligned_src;

  if (!((((long)src & 3) | ((long)dst & 3))) & !(count < sizeof (long)))
    {
      aligned_dst = (long*)dst;
      aligned_src = (long*)src;
      while (count >= sizeof (long int) && !((*aligned_src - 0x01010101) & ~*aligned_src & 0x80808080))
	{
	  count -= sizeof (long int);
	  *aligned_dst++ = *aligned_src++;
	}
      dst = (char*)aligned_dst;
      src = (char*)aligned_src;
    }
  while (count > 0)
    {
      --count;
      if ((*dst++ = *src++) == '\0')
	break;
    }
  while (count-- > 0)
    *dst++ = '\0';
  return dst0;
}
