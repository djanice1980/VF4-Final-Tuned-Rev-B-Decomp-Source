/*
 * vf4lib/libbase/src/ending_roll.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern ending_roll_row ending_roll_list[114]; /* 0x0c28c22c */
extern int ending_roll_y; /* 0x0c41f080 */

void ending_roll_reset(void)
{
  ending_roll_y = 0x244;
  ending_roll_frames = 0;
  return;
}

void ending_roll_draw(int speed)
{
  ending_roll_row *row;
  float y;

  y = 0.0;
  for (row = ending_roll_list; row->sprite != -1; row = row + 1) {
    y = y + row->gap;
    if (row->sprite == 0x2a31) {
      sprite_next_blend_set(0);
      sprite_put_flags_xyz(row->sprite,0x100a,320.0,y + (float)ending_roll_y + 17.0,0.02);
      sprite_next_blend_normal();
    }
    else {
      sprite_put_flags_xyz(row->sprite,0x100a,320.0,y + (float)ending_roll_y,0.01);
    }
  }
  if (pause_frame_held() == 0) {
    ending_roll_y = ending_roll_y - speed;
    if ((float)ending_roll_y + y < 220.0) {
      ending_roll_y = (int)(220.0 - y);
    }
    ending_roll_frames = ending_roll_frames + 1;
  }
}
