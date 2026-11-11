/*
 * libc/stdio/snprintf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int snprintf(char *str, size_t size, const char *format, ...)
{
  va_list ap;
  int r;
  __builtin_stdarg_start(ap, format);
  r = vsnprintf(str, size, format, ap);
  __builtin_va_end(ap);
  return r;
}
