/*
 * vf4lib/libcore/src/strbuf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void strbuf_init(strbuf_rec *sb, char *base, int size, int read, int write)
{
  sb->base = base;
  sb->end = base + size;
  sb->write = write + base;
  sb->read = (char *)(read + (int)base);
  return;
}

void strbuf_release_nop(strbuf_rec *sb)
{
  return;
}

int strbuf_vprintf(strbuf_rec *sb, const char *format, va_list ap)
{
  uint r;
  r = vsnprintf(sb->write,sb->end - sb->write,format,ap);
  if (sb->write + r >= sb->end) {
    sb->write = sb->end;
  }
  else {
    sb->write = sb->write + r;
  }
  return r;
}

int strbuf_printf(strbuf_rec *sb, char *format, ...)
{
  va_list ap;
  __builtin_stdarg_start(ap, format);
  return strbuf_vprintf(sb, format, ap);
}

void strbuf_drop_line(strbuf_rec *sb, int offset)
{
  char *src;
  char *b;
  uint d;
  src = sb->base + offset;
  while (src < sb->write) {
    if (*src == '\n') {
      src++;
      break;
    }
    src++;
  }
  if (src > sb->write) {
    src = sb->write;
  }
  memmove(sb->base,src,sb->write - src);
  b = sb->base;
  d = src - b;
  sb->write -= d;
  sb->read -= d;
  if (sb->read < b) {
    sb->read = b;
  }
}

int strbuf_vscanf(strbuf_rec *sb, char *format, va_list ap)
{
  uint count [1];

  count[0] = 0;
  if (sb->read < sb->write) {
    count[0] = sb->write - sb->read;
  }
  vsscanf_counted(sb->read,count,format,ap);
  sb->read = sb->read + count[0];
  return;
}

void strbuf_scanf(strbuf_rec *sb, char *format, ...)
{
  va_list ap;

  __builtin_stdarg_start(ap, format);
  strbuf_vscanf(sb, format, ap);
  __builtin_va_end(ap);
}
