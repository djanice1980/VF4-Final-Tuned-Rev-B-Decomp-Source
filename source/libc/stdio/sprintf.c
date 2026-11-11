/*
 * libc/stdio/sprintf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int sprintf(char *buf, char *format, ...)
{
  int r;
  va_list ap;
  __builtin_stdarg_start(ap, format);
  r = vsnprintf(buf, 0xffffffff, format, ap);
  __builtin_va_end(ap);
  return r;
}
