/*
 * sdk/nlasock/listen.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void listen_request_write(uint sock, uint backlog, sock_slot_rec *slot)
{
  uint buf[8];

  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  buf[0] = slot->op;
  buf[1] = sock;
  buf[2] = backlog;
  dimm_pio_write((ushort *)buf,slot->req_addr,0x20);
  return;
}

int sock_listen(uint sock, uint backlog)
{
  sock_slot_rec *slot;
  int *q;

  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 8;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  listen_request_write(sock,backlog,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  {
    int r = sock_result_take(sock);
    busy_word_drop(&dimm_busy_word);
    return r;
  }
}
