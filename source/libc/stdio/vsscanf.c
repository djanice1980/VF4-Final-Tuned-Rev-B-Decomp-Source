/*
 * libc/stdio/vsscanf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int vsscanf(char *str, char *format, va_list ap)
{
  scan_stream_rec f;
  f.left = f.size = strlen(str);
  f.pos = f.base = str;
  f.flags = 0x41;
  f.file = -1;
  f.pushback = 0;
  return scanf_core(&f,format,ap);
}

int vsscanf_counted(char *str, uint *count, char *format, va_list ap)
{
  scan_stream_rec f;
  int r;
  size_t n;

  n = strlen(str);
  if (count != (uint *)0x0) {
    size_t t = n;
    if (t > *count) t = *count;
    f.size = t;
  }
  else {
    f.size = n;
  }
  f.left = f.size;
  f.base = str;
  f.pos = str;
  f.flags = 0x41;
  f.file = -1;
  f.pushback = 0;
  r = scanf_core(&f, format, ap);
  if (count != (uint *)0x0) {
    *count = f.size - f.left;
  }
  return r;
}
