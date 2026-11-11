/*
 * sdk/nlasock/bind.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void bind_request_write(uint sock, void *addr, int addr_len, sock_slot_rec *slot)
{
  struct { uint op; uint sock; int addr_board; int addr_len; char pad[16]; char addr[64]; } s;
  uint sz;
  memset(&s,0,0x60);
  s.op = slot->op;
  s.sock = sock;
  s.addr_board = slot->req_addr + 0x20;
  memcpy(s.addr,addr,addr_len);
  sz = addr_len + 3U & 0xfffffffc;
  s.addr_len = addr_len;
  dimm_pio_write((ushort *)&s,slot->req_addr,sz + 0x20);
}

int sock_bind(uint sock, void *addr, int addr_len)
{
  sock_slot_rec *slot;
  int r;
  dimm_board_info_fetch();
  if (((0x3f < sock) | (addr == 0)) || addr_len > 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 2;
  slot->result = 0;
  { int *q = slot->copy_back; q[0] = 0; q[1] = 0; q[2] = 0; q[3] = 0; }
  bind_request_write(sock,addr,addr_len,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}
