/*
 * vf4lib/libcore/src/fatal.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern fatal_record_rec fatal_record; /* 0x0c3d88d8 */

void mem_block_set(static_heap_rec *block, void *addr, uint size, char *label)
{
  block->addr = addr;
  block->size = size;
  block->label = label;
  return;
}

void empty_fn_0c0f2340(void)
{
  return;
}

void mem_block_copy(static_heap_rec *dst, static_heap_rec *src)
{
  char *label;

  if (!(src == (static_heap_rec *)0x0)) {
    dst->addr = src->addr;
    dst->size = src->size;
    label = src->label;
  }
  else {
    dst->addr = 0;
    dst->size = 0;
    label = null_text;
  }
  dst->label = label;
  return;
}

void fatal_at(const char *file, int line, const char *format, ...)
{
  va_list ap;
  __builtin_stdarg_start(ap, format);
  fatal_at_va(file, line, format, ap);
}

void fatal_at_va(const char *file, int line, const char *format, va_list ap)
{
  char buf[256];
  vsnprintf(buf,0x100,format,ap);
  fatal_record_store(file,line,buf);
}

void fatal_record_store(const char *file, int line, char *text)
{
  fatal_record.src_file = (uint)file;
  fatal_record.line = line;
  fatal_record.text = (uint)text;
  return;
}
