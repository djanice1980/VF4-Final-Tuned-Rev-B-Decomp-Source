/*
 * sdk/nlasock/ioctl.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void ioctl_request_write(uint sock, uint command, uint arg_board, sock_slot_rec *slot)
{
  uint b[8];

  b[4] = 0;
  b[5] = 0;
  b[6] = 0;
  b[7] = 0;
  b[0] = slot->op;
  b[1] = sock;
  b[2] = command;
  b[3] = arg_board;
  dimm_pio_write((ushort *)b,slot->req_addr,0x20);
}

void ioctl_arg_write(uint board_addr, uint *arg)
{
  uint word[1];

  word[0] = *arg;
  dimm_pio_write((ushort *)word,board_addr,4);
  return;
}

void ioctl_arg_copy_back(uint sock, int arg_board, uint *arg)
{
  int data;

  if (arg != 0) {
    data = sock_slot_data_addr(sock,0,0);
    dimm_pio_read(data,(ushort *)xfer_recv_bounce,0x200);
    memcpy(arg,xfer_recv_bounce + (arg_board - data),4);
  }
  return;
}

int sock_ioctl(uint sock, uint command, uint *arg)
{
  int r;
  sock_slot_rec *slot;
  uint arg_board;
  int result;
  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
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
  if (arg != 0) {
    arg_board = sock_slot_data_addr(sock,0,4);
    ioctl_arg_write(arg_board,arg);
  }
  else {
    arg_board = 0;
  }
  slot->done = 0;
  slot->op = 7;
  slot->result = 0;
  {
    int *q = slot->copy_back;
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
  }
  ioctl_request_write(sock,command,arg_board,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  ioctl_arg_copy_back(sock,arg_board,arg);
  busy_word_drop(&dimm_busy_word);
  return result;
}
