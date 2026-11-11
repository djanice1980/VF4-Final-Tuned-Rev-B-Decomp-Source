/*
 * zlib/zutil.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

voidpf zcalloc(voidpf opaque, uint items, uint size)
{
  void *p;
  
  p = calloc(items,size);
  return p;
}

void zcfree(voidpf opaque, voidpf ptr)
{
  free(ptr);
  return;
}

const char *zlibVersion(void)
{
  return "1.1.4";
}

char * zError(int err)
{
  return z_errmsg[2 - err];
}
