/*
 * sdk/nlasock/board_text.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void board_text_request_write(char *text, uint value, sock_slot_rec *slot)
{
  struct { int op; int text_board; size_t len; uint value; int pad[4]; char text[64]; } s;

  memset(&s,0,0x60);
  s.op = slot->op;
  s.text_board = slot->req_addr + 0x20;
  strncpy(s.text,text,0x40);
  s.len = strlen(text);
  s.value = value;
  dimm_pio_write((ushort *)&s,slot->req_addr,0x60);
  return;
}

int board_text_call(char *text, uint value)
{
  int sock = 0;
  sock_slot_rec *slot;
  int r;
  int *q;

  if ((dimm_board_info_fetch()->id_word & 0x08000000) == 0) {
    return -3;
  }
  if (text == 0) {
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
  slot->op = 0x15;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  board_text_request_write(text,value,slot);
  dimm_cmd_post_bare(0x12,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int sock_op15_string_async(char *text, uint value)
{
  int sock = 0;
  sock_slot_rec *slot;
  int *q;
  if ((dimm_board_info_fetch()->id_word & 0x08000000) == 0) return -3;
  if (text == 0) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0 && busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
    busy_word_drop(&dimm_busy_word);
    return -4;
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 0x15;
  slot->result = 0;
  q = slot->copy_back;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  board_text_request_write(text,value,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
