/*
 * sdk/nlasock/resolve.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void resolve_request_write(char *name, uint server1, uint server2, uint sock, sock_slot_rec *slot)
{
  struct {
    uint op;
    int name_board;
    size_t len;
    uint server1;
    uint server2;
    uint sock;
    uint pad[2];
    char name[64];
  } s;

  memset(&s,0,0x60);
  s.op = slot->op;
  s.name_board = slot->req_addr + 0x20;
  strncpy(s.name,name,0x40);
  s.len = strlen(name);
  s.server1 = server1;
  s.server2 = server2;
  s.sock = sock;
  dimm_pio_write((ushort *)&s,slot->req_addr,0x60);
}

int host_resolve(char *name, uint server1, uint server2, uint sock)
{
  int result;
  int r;
  sock_slot_rec *slot;

  dimm_board_info_fetch();
  if ((name == 0) | (sock > 0x3f)) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 5;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  resolve_request_write(name,server1,server2,sock,slot);
  dimm_cmd_post_bare(0x12,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}

int host_resolve_start(char *name, uint server1, uint server2, int sock)
{
  int r;
  sock_slot_rec *slot;
  int *q;

  dimm_board_info_fetch();
  if (name == 0) return -2;
  r = busy_word_try_set(&dimm_busy_word);
  if (r == 0) return -4;
  if (sock == 0) {
    r = busy_word_try_set(&sock_slot_at(0)->busy_word);
    if (r == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 5;
  slot->result = 0;
  q = slot->copy_back;
  slot->copy_back[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  resolve_request_write(name,server1,server2,sock,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
