/*
 * vf4lib/libcore/src/clip.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern clip_rect_rec clip_rect; /* 0x0c325508 */

void clip_rect_store(int x0, int y0, int x1, int y1)
{
  clip_rect.x0 = x0;
  clip_rect.y0 = y0;
  clip_rect.x1 = x1;
  clip_rect.y1 = y1;
  return;
}

void clip_rect_apply(int mode)
{
  pcw_user_clip_mode_put(0x1f,clip_rect.clip_mode);
  clip_rect.clip_mode = mode;
  hdr_or_user_clip(mode);
  clip_rect_per_list_bits(clip_rect.x0,clip_rect.y0,
               clip_rect.x1,clip_rect.y1,0x1f);
  elan_lists_end_with_clip(clip_rect.x0,clip_rect.y0,
               clip_rect.x1,clip_rect.y1,0x1f);
  return;
}

void clip_rect_clear(void)
{
  pcw_user_clip_mode_put(0x1f,clip_rect.clip_mode);
  clip_rect.clip_mode = 0;
  return;
}
