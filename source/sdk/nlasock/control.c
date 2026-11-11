/*
 * sdk/nlasock/control.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

int ctl_channel_prepare(int window_base)
{
  ctl_call_state.done = 1;
  ctl_call_state.op = 0;
  ctl_call_state.result = 0;
  ctl_call_state.req_addr = 0xc00000 + window_base;
  dimm_cmd_handler_put(0x22,ctl_reply_take);
  return 0;
}

int ctl_reply_take(uint cmd_word, uint offset, uint param_lo, int param_hi)
{
  ctl_call_block *c = &ctl_call_state; /* ORIGINAL SHAPE (corroborated 2026-11-10: ctl_call_text in this file reaches the same record through a pointer local, and there it is forced): the call record through one typed pointer, its only access path; the game loads the address once before the branch and stores through it after the join, which at -O1 only a variable gives */

  if ((uchar)cmd_word == 5) {
    c->result = param_hi << 0x10 | param_lo & 0xffff;
  }
  else {
    c->result = -1;
  }
  c->done = 1;
  return 5;
}

void ctl_request_write_text(char *text, uint value, ctl_call_block *call)
{
  struct { uint op; int text_board; uint value; char pad[20]; char text[64]; } s;

  memset(&s,0,0x60);
  s.op = call->op;
  s.text_board = call->req_addr + 0x20;
  strncpy(s.text,text,0x40);
  s.value = value;
  dimm_pio_write((ushort *)&s,call->req_addr,0x60);
  return;
}

int ctl_call_text(char *text, uint value)
{
  ctl_call_block *call; int r; call = &ctl_call_state;
  dimm_board_info_fetch();
  if (text == 0) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  call->done = 0;
  call->op = 1;
  call->result = 0;
  ctl_request_write_text(text,value,call);
  dimm_cmd_post_bare(0x21,0);
  while (*(volatile int *)&call->done != 1) {
  }
  r = call->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

void ctl_request_write_op2(ctl_call_block *call)
{
  uint b[8];
  b[1] = 0;
  b[2] = 0;
  b[3] = 0;
  b[4] = 0;
  b[5] = 0;
  b[6] = 0;
  b[7] = 0;
  b[0] = call->op;
  dimm_pio_write((ushort *)b,call->req_addr,0x20);
}

int ctl_call_op2(void)
{
  volatile ctl_call_block *call = &ctl_call_state;
  int r;

  dimm_board_info_fetch();
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  call->done = 0;
  call->op = 2;
  call->result = 0;
  ctl_request_write_op2((ctl_call_block *)call);
  dimm_cmd_post_bare(0x21,0);
  while (call->done != 1) ;
  r = call->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

void ctl_request_write_op3(ctl_call_block *call)
{
  uint b[8];

  b[1] = 0;
  b[2] = 0;
  b[3] = 0;
  b[4] = 0;
  b[5] = 0;
  b[6] = 0;
  b[7] = 0;
  b[0] = call->op;
  dimm_pio_write((ushort *)b,call->req_addr,0x20);
}

int ctl_call_op3(void)
{
  ctl_call_block *call;
  int r;

  call = &ctl_call_state;
  dimm_board_info_fetch();
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  call->done = 0;
  call->op = 3;
  call->result = 0;
  ctl_request_write_op3(call);
  dimm_cmd_post_bare(0x21,0);
  while (*(volatile int *)&call->done != 1) {
  }
  r = call->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

void ctl_request_write_op4(ctl_call_block *call)
{
  uint b[8];

  b[1] = 0;
  b[2] = 0;
  b[3] = 0;
  b[4] = 0;
  b[5] = 0;
  b[6] = 0;
  b[7] = 0;
  b[0] = call->op;
  dimm_pio_write((ushort *)b,call->req_addr,0x20);
}

int ctl_call_op4(void)
{
  ctl_call_block *call;
  int r;

  call = &ctl_call_state;
  dimm_board_info_fetch();
  if (busy_word_try_set(&dimm_busy_word) == 0) {
    return -2;
  }
  call->done = 0;
  call->op = 4;
  call->result = 0;
  ctl_request_write_op3(call);
  dimm_cmd_post_bare(0x21,0);
  while (*(volatile int *)&call->done != 1) {
  }
  r = call->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}
