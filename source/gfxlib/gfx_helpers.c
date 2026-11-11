/*
 * gfxlib/gfx_helpers.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint proj_rect_saved; /* 0x0c32a534 */
extern float saved_proj_width; /* 0x0c41762c */
extern float saved_proj_height; /* 0x0c417630 */
extern float saved_proj_top; /* 0x0c417634 */
extern float saved_proj_left; /* 0x0c417638 */
extern float proj_rect_width; /* 0x0c41c950 */
extern float proj_rect_left; /* 0x0c41c998 */
extern float proj_rect_height; /* 0x0c41ca20 */
extern float proj_rect_top; /* 0x0c41ca24 */

void check_or_magenta_halt(int condition)
{
  if (condition == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void check_pass_stub(void)
{
  check_or_magenta_halt(1);
  return;
}

void elan_input_reset_tile_word(uint word)
{
  elan_cmd_input_reset();
  elan_tile_split_reg = word;
  return;
}

void tile_grid_set_from_counts(int last_col, int last_row)
{
  tile_grid_size_put(last_col + 1,last_row + 1);
  return;
}

void proj_rect_saved_clear(void)
{
  proj_rect_saved = 0;
  return;
}

void proj_rect_save_once(void)
{
  if (proj_rect_saved == 0) {
    saved_proj_left = proj_rect_left;
    saved_proj_top = proj_rect_top;
    saved_proj_width = proj_rect_width;
    saved_proj_height = proj_rect_height;
    proj_rect_saved = 1;
  }
  return;
}

void proj_rect_restore(void)
{
  if (proj_rect_saved != 0) {
    proj_map_to_rect(saved_proj_left,saved_proj_top,saved_proj_width,saved_proj_height);
    proj_rect_saved = 0;
  }
  return;
}

void code_halfwords_copy_purged(ushort *dst, ushort *src, ushort *end)
{
  ushort *d;
  int size;

  size = (int)end - (int)src;
  dcache_purge_lines((uint)dst,size);
  d = dst;
  for (; src < end; src = src + 1) {
    *d = *src;
    d = d + 1;
  }
  dcache_purge_lines((uint)dst,size);
  return;
}

void code_jump_patch(uint routine, uint target)
{
  jump_stub_target = target;
  code_halfwords_copy_purged((ushort *)routine,(ushort *)&jump_stub,(ushort *)&jump_stub_end);
  return;
}
