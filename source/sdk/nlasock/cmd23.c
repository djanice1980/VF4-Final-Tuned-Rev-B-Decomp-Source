/*
 * sdk/nlasock/cmd23.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

int dimm_cmd24_hook_install(uint window_base)
{
  dimm_cmd_handler_put(0x24,dimm_cmd24_ignore);
  return 0;
}

int dimm_cmd24_ignore(void)
{
  return 0;
}

int dimm_cmd23_send_pair(ushort value1, ushort value2)
{
  if ((dimm_board_info_fetch()->feature_bits & 8) == 0) return -3;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  dimm_cmd_post(0x23,0,1,value1,value2);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

int dimm_cmd23_send_op2(void)
{
  dimm_board_info_rec *info;

  info = dimm_board_info_fetch();
  if ((info->feature_bits & 8) == 0) return -3;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  dimm_cmd_post(0x23,0,2,0,0);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
