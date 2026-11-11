/*
 * sdk/nlasock/set_option.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_setopt_pack_args(uint sock, uint level, uint option, void *value, int value_len, sock_slot_rec *slot)
{
  struct { uint op, sock, level, option; int value_board; int value_len; int pad[2]; char value[64]; } m;
  uint t;

  memset(&m,0,0x60);
  m.op = slot->op;
  m.sock = sock;
  m.level = level;
  m.option = option;
  m.value_board = slot->req_addr + 0x20;
  memcpy(m.value,value,value_len);
  t = value_len + 3U & 0xfffffffc;
  m.value_len = value_len;
  dimm_pio_write((ushort *)&m,slot->req_addr,t + 0x20);
}

int sock_set_option(uint sock, uint level, uint option, void *value, int value_len)
{
  int result;
  sock_slot_rec *slot;
  int *q;

  dimm_board_info_fetch();
  if (sock > 0x3f || value == 0 || value_len > 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0xe;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  sock_setopt_pack_args(sock,level,option,value,value_len,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}
