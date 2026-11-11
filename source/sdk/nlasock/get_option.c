/*
 * sdk/nlasock/get_option.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void getopt_request_write(uint sock, uint level, uint option, uint value_board, uint len_board, sock_slot_rec *slot)
{
  uint b[8];

  b[6] = 0;
  b[7] = 0;
  b[0] = slot->op;
  b[1] = sock;
  b[2] = level;
  b[3] = option;
  b[4] = value_board;
  b[5] = len_board;
  dimm_pio_write((ushort *)b,slot->req_addr,0x20);
}

void getopt_len_write(uint board_addr, uint *len)
{
  uint word[1];

  word[0] = *len;
  dimm_pio_write((ushort *)word,board_addr,4);
  return;
}

void getopt_value_copy_back(uint sock, int value_board, void *value, int len_board, uint *len)
{
  int data;

  if (value != 0) {
    data = sock_slot_data_addr(sock,0,0);
    dimm_pio_read(data,(ushort *)xfer_recv_bounce,0x200);
    memcpy(len,xfer_recv_bounce + (len_board - data),4);
    memcpy(value,xfer_recv_bounce + (value_board - data),*len);
  }
  return;
}

int sock_option_get(uint sock, uint level, uint option, void *value, uint *len)
{
  int result;
  int r;
  sock_slot_rec *slot;
  uint value_board;
  uint len_board;

  dimm_board_info_fetch();
  if (((0x3f < sock) || (value != 0 && len == 0)) || (value == 0 && len != 0)) {
    return -2;
  }
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  if (value != 0) {
    value_board = sock_slot_data_addr(sock,4,0x40);
    len_board = sock_slot_data_addr(sock,0,4);
    getopt_len_write(len_board,len);
  }
  else {
    value_board = 0;
    len_board = 0;
  }
  slot->done = 0;
  slot->op = 0xf;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  getopt_request_write(sock,level,option,value_board,len_board,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  getopt_value_copy_back(sock,value_board,value,len_board,len);
  busy_word_drop(&dimm_busy_word);
  return result;
}
