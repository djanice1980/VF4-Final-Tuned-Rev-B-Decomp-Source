/*
 * gfxlib/frame_end.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint render_busy_flag; /* 0x0c41bb00 */

void frame_end_with_targets(void)
{
  target_screen_state_restore();
  target_queue_terminate();
  tex_queue_terminate();
  elan_lists_end();
  elan_cmd_buffer_flip();
  render_busy_flag = 1;
  scene_lists_close();
  render_busy_flag = 0;
  frame_lists_send_and_rearm();
  tex_queue_flip();
  target_queues_flip();
  target_frame_flags_clear();
  last_link_cmd = 0;
  list_end_extra_mask = 0;
  return;
}
