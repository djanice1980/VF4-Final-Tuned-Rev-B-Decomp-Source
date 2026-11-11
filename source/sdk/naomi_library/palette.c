/*
 * sdk/naomi_library/palette.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern ushort pal_block_bitmap[4]; /* 0x0c5d48b4 */
extern uint pal_shadow[1024]; /* 0x0c5d4c00 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void pal_groups_setup(pal_group_rec *groups, int count)
{
  int i;
  pal_group_rec *p = groups;
  
  graphics_work.pal_group_table = (uint)groups;
  graphics_work.pal_group_count = (short)count;
  for (i = count; i--; ) {
    char *q = &p->field_0x2;
    p->field_0x6 = -1;
    p->first_block = -1;
    *q++ = -1;
    p->use_count = 0;
    *q = 0;
    p++;
  }
  for (i = 0; i < 4; i++) {
    pal_block_bitmap[i] = 0;
  }
}

void pal_groups_reset(void)
{
  pal_groups_setup((pal_group_rec *)graphics_work.pal_group_table,(int)graphics_work.pal_group_count);
  return;
}

void pal_group_unref(int group)
{
  pal_group_rec *e;
  int n;
  int bit;
  int idx;
  char *cp;

  e = (pal_group_rec *)graphics_work.pal_group_table;
  e += group;
  if (e->use_count > 1) {
    e->use_count--;
  } else {
    group = e->first_block;
    n = e->block_count;
    cp = &e->field_0x2;
    e->field_0x6 = -1;
    e->first_block = -1;
    *cp = -1;
    e->use_count = 0;
    bit = 1 << (group & 0xf);
    idx = group >> 4;
    while (n-- > 0) {
      pal_block_bitmap[idx] &= ~bit;
      bit += bit;
      if (bit == 0x10000) {
        bit = 1;
        idx++;
      }
    }
  }
}

int pal_group_word0_get(int group)
{
  return *(short *)(group * 8 + graphics_work.pal_group_table);
}

void pal_group_store_colors(int group, uint *colors)
{
  pal_group_rec *e;

  e = (pal_group_rec *)graphics_work.pal_group_table;
  e += group;
  if (e->use_count != 0) {
    int first = e->first_block; int blocks = e->block_count;
    palette_shadow_write(first << 4,colors,blocks << 4);
  }
}

void pal_group_store_colors_wide(int group,uint *colors)
{
  pal_group_store_colors(group,colors);
  return;
}

int pal_find_free_blocks(int kind, int color_count)
{
  int n;
  int cnt;
  int start;
  int i;
  int k;
  int j;
  int bit;
  int mask;

  n = (color_count + 0xf) >> 4;
  if (kind == 0x10) {
    cnt = 0;
    start = -1;
    for (i = 0; i < 4; i++) {
      for (bit = 1, j = 0; j < 0x10; bit <<= 1, j++) {
        if (((short)pal_block_bitmap[i] & bit) == 0) {
          if (cnt == 0) start = i * 0x10 + j;
          cnt++;
          if (cnt >= n) break;
        }
        else {
          cnt = 0;
        }
      }
    }
  }
  else {
    cnt = 0;
    start = -1;
    mask = (1 << n) - 1;
    for (k = 0; k < 4; k++) {
      if (((short)pal_block_bitmap[k] & mask) == 0) {
        start = k << 4;
        cnt = n;
        break;
      }
    }
  }
  if (cnt < n) start = -1;
  return start;
}

void pal_mark_blocks_used(uint first_block, int color_count)
{
  int idx;
  int bit;
  int n;
  
  idx = (int)first_block >> 4;
  bit = 1 << (first_block & 0xf);
  n = color_count + 0xf >> 4;
  while (n-- > 0) {
    pal_block_bitmap[idx] |= bit;
    bit += bit;
    if (bit > 0xffff) {
      idx++;
      bit = 1;
    }
  }
}

void pal_shadow_flush(void)
{
  if (graphics_work.pal_shadow_changed != '\0') {
    graphics_work.pal_shadow_changed = '\0';
    pvr_pal_ram_load_all(pal_shadow);
  }
  return;
}

void pal_entry_format_select(int format)
{
  graphics_work.pal_entry_format = (uchar)format;
  pvr_pal_ram_ctrl_put(pal_hw_format_table[format]);
  return;
}
