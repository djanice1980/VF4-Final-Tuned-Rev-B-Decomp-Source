/*
 * sdk/nlasock/select.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_select_pack_args(uint count, uint read_board, uint write_board, uint error_board, void *timeout, sock_slot_rec *slot)
{
  int extra;
  uint s[10];

  extra = 0;
  s[5] = 0;
  s[6] = 0;
  s[7] = 0;
  s[8] = 0;
  s[9] = 0;
  s[0] = slot->op;
  s[1] = count;
  s[2] = read_board;
  s[3] = write_board;
  s[4] = error_board;
  if (timeout != 0) {
    s[5] = slot->req_addr + 0x20;
    memcpy(&s[8],timeout,8);
    extra = 8;
  }
  else {
    s[5] = 0;
  }
  dimm_pio_write((ushort *)s,slot->req_addr,extra + 0x20);
}

void sock_select_put_sets(uint board_addr, uint *read_set, uint *write_set, uint *error_set)
{
  typedef struct { uint w[8]; } set_words;
  struct { set_words read; set_words write; set_words error; } b;

  memset(&b,0,0x60);
  if (read_set != (uint *)0x0) {
    b.read = *(set_words *)read_set;
  }
  if (write_set != (uint *)0x0) {
    b.write = *(set_words *)write_set;
  }
  if (error_set != (uint *)0x0) {
    b.error = *(set_words *)error_set;
  }
  dimm_pio_write((ushort *)&b,board_addr,0x60);
}

void sock_select_get_sets(uint sock, int read_board, void *read_set, int write_board, void *write_set, int error_board, void *error_set)
{
  int data;

  if (read_set == 0 && write_set == 0 && error_set == 0) return;
  data = sock_slot_data_addr(sock,0,0);
  dimm_pio_read(data,(ushort *)xfer_recv_bounce,0x200);
  if (read_set != 0) {
    memcpy(read_set,xfer_recv_bounce + (read_board - data),0x20);
  }
  if (write_set != 0) {
    memcpy(write_set,xfer_recv_bounce + (write_board - data),0x20);
  }
  if (error_set != 0) {
    memcpy(error_set,xfer_recv_bounce + (error_board - data),0x20);
  }
}

int sock_select(uint count, uint *read_set, uint *write_set, uint *error_set, void *timeout)
{
  uint sock = 0;
  sock_slot_rec *slot;
  int *q;
  int sets_board;
  int error_board;
  int read_board;
  int write_board;
  int result;
  if ((read_set == 0) & (write_set == 0) && error_set == 0) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  read_board = 0;
  write_board = 0;
  error_board = 0;
  if (read_set != 0 || write_set != 0 || error_set != 0) {
    sets_board = sock_slot_data_addr(sock,0,0x60);
    if (read_set != 0) read_board = sets_board;
    if (write_set != 0) write_board = sets_board + 0x20;
    if (error_set != 0) {
      error_board = sets_board;
      error_board += 0x40;
    }
    sock_select_put_sets(sets_board,read_set,write_set,error_set);
  }
  slot->done = 0;
  slot->op = 0xc;
  slot->result = 0;
  q = slot->copy_back;
  slot->copy_back[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  sock_select_pack_args(count,read_board,write_board,error_board,timeout,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  result = sock_result_take(sock);
  sock_select_get_sets(sock,read_board,read_set,write_board,write_set,error_board,error_set);
  busy_word_drop(&dimm_busy_word);
  return result;
}
