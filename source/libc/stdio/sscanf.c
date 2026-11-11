/*
 * libc/stdio/sscanf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int sscanf(char *str, char *format, ...)
{
  va_list ap;
  int r;

  __builtin_stdarg_start(ap, format);
  r = vsscanf(str, format, ap);
  return r;
}

void sscanf_counted(char *str, uint *count, char *format, ...)
{
  va_list ap;
  __builtin_stdarg_start(ap, format);
  vsscanf_counted(str, count, format, ap);
}
