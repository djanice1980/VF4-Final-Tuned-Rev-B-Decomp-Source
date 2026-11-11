/*
 * sdk/nlasock/op12_op13.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_op12_pack_strings(char *first, char *second, sock_slot_rec *slot)
{
  struct { uint op; int first_board; size_t first_len; int second_board; size_t second_len; int pad[3]; char first[64]; char second[64]; } m;

  memset(&m,0,sizeof(m));
  m.op = slot->op;
  m.first_board = slot->req_addr + 0x20;
  strncpy(m.first,first,0x40);
  m.first_len = strlen(first);
  m.second_board = slot->req_addr + 0x60;
  strncpy(m.second,second,0x40);
  m.second_len = strlen(second);
  dimm_pio_write((ushort *)&m,slot->req_addr,sizeof(m));
  return;
}

int sock_op12_two_strings(char *first, char *second)
{
  int sock;
  sock_slot_rec *slot;
  int *q;
  int r;

  sock = 0;
  dimm_board_info_fetch();
  if ((first == 0) | (second == 0)) {
    return -2;
  }
  if (busy_word_try_set(&dimm_busy_word) == 0) {
    return -4;
  }
  if (sock == 0 && busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
    busy_word_drop(&dimm_busy_word);
    return -4;
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0x12;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  sock_op12_pack_strings(first, second, slot);
  dimm_cmd_post_bare(0x12, sock);
  while (sock_call_done(sock) != 1) {
  }
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}

void sock_op13_pack_strings(char *first, char *second, sock_slot_rec *slot)
{
  sock_op12_pack_strings(first,second,slot);
  return;
}

int sock_op13_two_strings(char *first, char *second)
{
  int result;
  int r;
  sock_slot_rec *slot;
  int sock;

  sock = 0;
  dimm_board_info_fetch();
  if ((first == 0) | (second == 0)) {
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
  slot->op = 0x13;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    slot->copy_back[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  sock_op13_pack_strings(first,second,slot);
  dimm_cmd_post_bare(0x12,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}
