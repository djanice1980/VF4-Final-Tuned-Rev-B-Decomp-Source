/*
 * sdk/nlasock/board_switch.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void board_switch_request_write(uint value, sock_slot_rec *slot)
{
  uint req[24];

  memset(req,0,0x60);
  req[0] = slot->op;
  req[1] = value;
  dimm_pio_write((ushort *)req,slot->req_addr,0x60);
  return;
}

int board_switch_call(uint value)
{
  sock_slot_rec *slot;
  int sock = 0;
  int r;

  if ((dimm_board_info_fetch()->id_word & 0x08000000) == 0) return -3;
  if (value > 1) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0 && busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
    busy_word_drop(&dimm_busy_word);
    return -4;
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0x14;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  board_switch_request_write(value, slot);
  dimm_cmd_post_bare(0x12, sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int board_switch_start(uint value)
{
  uint sock = 0;
  sock_slot_rec *slot;
  int *q;

  if ((dimm_board_info_fetch()->id_word & 0x8000000) == 0) return -3;
  if (value > 1) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0 && busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
    busy_word_drop(&dimm_busy_word);
    return -4;
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0x14;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  board_switch_request_write(value,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
