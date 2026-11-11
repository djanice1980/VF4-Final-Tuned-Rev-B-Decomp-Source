/*
 * sdk/nlasock/lookup.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void lookup_request_write(char *name, sock_slot_rec *slot)
{
  struct { uint op; int name_board; size_t len; char pad[20]; char name[64]; } m;

  memset(&m,0,0x60);
  m.op = slot->op;
  m.name_board = slot->req_addr + 0x20;
  strncpy(m.name,name,0x40);
  m.len = strlen(name);
  dimm_pio_write((ushort *)&m,slot->req_addr,0x60);
}

int host_lookup(char *name)
{
  int result;
  int r;
  sock_slot_rec *slot;
  int sock;

  sock = 0;
  dimm_board_info_fetch();
  if (name == 0) {
    return -2;
  }
  if (busy_word_try_set(&dimm_busy_word) == 0) {
    return -4;
  }
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 6;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    slot->copy_back[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  lookup_request_write(name,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}
