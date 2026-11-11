/*
 * sdk/nlasock/create.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_create_pack_args(uint domain, uint type, uint protocol, sock_slot_rec *slot)
{
  uint b[8];

  b[4] = 0;
  b[5] = 0;
  b[6] = 0;
  b[7] = 0;
  b[0] = slot->op;
  b[1] = domain;
  b[2] = type;
  b[3] = protocol;
  dimm_pio_write((ushort *)b,slot->req_addr,0x20);
  return;
}

int sock_create(uint domain, uint type, uint protocol)
{
  int r;
  sock_slot_rec *slot;
  int *q;
  int sock = 0;

  dimm_board_info_fetch();
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0xb;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  sock_create_pack_args(domain,type,protocol,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}
