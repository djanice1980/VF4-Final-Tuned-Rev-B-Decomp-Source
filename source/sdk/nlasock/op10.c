/*
 * sdk/nlasock/op10.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_op10_pack_args(uint sock, uint value1, uint value2, uint value3, sock_slot_rec *slot)
{
  uint buf[8];

  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  buf[0] = slot->op;
  buf[1] = sock;
  buf[2] = value1;
  buf[3] = value2;
  buf[4] = value3;
  dimm_pio_write((ushort *)buf,slot->req_addr,0x20);
}

int sock_op10_three_values(uint sock, uint value1, uint value2, uint value3)
{
  int r;
  sock_slot_rec *first;
  sock_slot_rec *slot;
  int *q;

  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    first = sock_slot_at(0);
    if (busy_word_try_set(&first->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0x10;
  slot->result = 0;
  q = slot->copy_back;
  slot->copy_back[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  sock_op10_pack_args(sock,value1,value2,value3,slot);
  dimm_cmd_post_bare(0x12,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}
