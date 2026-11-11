/*
 * sdk/kamui/list_writes.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint clip_dummy_header[8]; /* 0x0c32a8b0 */
extern uint clip_dummy_volume[16]; /* 0x0c32a8d0 */
extern list_write_rec list_write_state; /* 0x0c41e000 */

int list_header_sq_write(vertex_buffer_desc_rec *desc)
{
  uint area;
  uint a;
  int b;
  uint bytes;
  int list;

  list = ((list_write_rec *)desc->list_and_size)->list_type;
  bytes = ((list_write_rec *)desc->list_and_size)->header_bytes;
  a = ((uint *)desc->list_write_ptrs)[list];
  b = (int)desc->header;
  area = a >> 0x18;
  *(volatile uint *)0xff000038 = area;
  *(volatile uint *)0xff00003c = area;
  a = a & 0x3ffffff | 0xe0000000;
  copy_by_pairs_or_words((uint *)a,(uint *)b,bytes);
  __asm__ volatile ("pref @%0" : : "r" (a));
  if (bytes == 0x40) {
    __asm__ volatile ("pref @%0" : : "r" (a + 32));
  }
  ((int **)desc->list_write_ptrs)[list] += bytes >> 2;
  return 0;
}

int list_block_sq_write(vertex_buffer_desc_rec *desc, void *block, uint kind, uint bytes)
{
  int idx;
  uint v;
  list_write_state.block_kind = kind;
  idx = ((list_write_rec *)desc->list_and_size)->list_type;
  v = ((uint *)desc->list_write_ptrs)[idx];
  *(volatile uint *)0xff000038 = v >> 24;
  *(volatile uint *)0xff00003c = v >> 24;
  v = v & 0x3ffffff | 0xe0000000;
  copy_by_pairs_or_words((uint *)v,(uint *)block,bytes);
  __asm__ __volatile__ ("pref @%0" : : "r" (v));
  if (bytes == 64) {
    __asm__ __volatile__ ("pref @%0" : : "r" (v + 32));
  }
  ((uint *)desc->list_write_ptrs)[idx] += bytes & ~3;
  return 0;
}

int list_tile_clip_param_write(vertex_buffer_desc_rec *desc, uint list, int x_min, int y_min, int x_max, int y_max)
{
  uint buf[8];
  int n;
  uint v;
  uint a;
  uint i;

  if (listbuf_list_used_words(list) != 8) {
    v = *(uint *)desc->header;
    n = 32;
    if (list & 1) {
      clip_dummy_header[0] = (v & 0x830000) | 0x81000040;
    } else {
      clip_dummy_header[0] = (v & 0x830000) | 0xa0000000;
    }
    a = ((uint *)desc->list_write_ptrs)[list];
    *(volatile uint *)0xff000038 = a >> 24;
    *(volatile uint *)0xff00003c = a >> 24;
    a = (a & 0x3ffffff) | 0xe0000000;
    copy_by_pairs_or_words((uint *)a, clip_dummy_header, n);
    __asm__ __volatile__ ("pref @%0" : : "r" (a));
    if (n == 64) {
      __asm__ __volatile__ ("pref @%0" : : "r" (a + 32));
    }
    ((uint *)desc->list_write_ptrs)[list] += 32;
    n = 64;
    for (i = 0; i < 3; i++) {
      a = ((uint *)desc->list_write_ptrs)[list];
      *(volatile uint *)0xff000038 = a >> 24;
      *(volatile uint *)0xff00003c = a >> 24;
      a = (a & 0x3ffffff) | 0xe0000000;
      copy_by_pairs_or_words((uint *)a, clip_dummy_volume, n);
      __asm__ __volatile__ ("pref @%0" : : "r" (a));
      if (n == 64) {
        __asm__ __volatile__ ("pref @%0" : : "r" (a + 32));
      }
      ((uint *)desc->list_write_ptrs)[list] += 64;
    }
  }
  n = 32;
  buf[0] = 0x20000000;
  buf[4] = x_min;
  buf[5] = y_min;
  buf[6] = x_max;
  buf[7] = y_max;
  a = ((uint *)desc->list_write_ptrs)[list];
  *(volatile uint *)0xff000038 = a >> 24;
  *(volatile uint *)0xff00003c = a >> 24;
  a = (a & 0x3ffffff) | 0xe0000000;
  copy_by_pairs_or_words((uint *)a, buf, n);
  __asm__ __volatile__ ("pref @%0" : : "r" (a));
  if (n == 64) {
    __asm__ __volatile__ ("pref @%0" : : "r" (a + 32));
  }
  ((uint *)desc->list_write_ptrs)[list] += 32;
  return 0;
}

uint * strip_list_write_ptr_addr(void)
{
  return &list_write_ptrs[strip_list_type];
}
