/*
 * sdk/nlasock/recv.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_recv_pack_args(uint sock, uint addr, int len, uint flags, sock_slot_rec *slot)
{
  uint buf[8];

  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  buf[0] = slot->op;
  buf[1] = sock;
  buf[2] = addr;
  buf[3] = len;
  buf[4] = flags;
  dimm_pio_write((ushort *)buf,slot->req_addr,0x20);
  return;
}

int sock_recv(uint sock, uint dst, int len, uint flags)
{
  int r;
  sock_slot_rec *slot;
  int *copy;
  uint addr;
  int use_dma;

  dimm_board_info_fetch();
  xfer_setting_get(0,&use_dma);
  addr = recv_area_addr(0);
  if (((0x3f < sock) | (dst == 0)) || recv_area_span_check(addr,len) != 0) {
    return -2;
  }
  if (use_dma != 0) {
    if ((dst & 0x1f) != 0 || (addr & 0x1f) != 0) {
      if ((addr + len + 0x1f & 0xffffffe0) - (addr & 0xffffffe0) > 0x2000) {
        len = 0x2000 - (addr & 0x1f);
      }
    }
  }
  else {
    if ((dst & 1) != 0 || (addr & 1) != 0) {
      if ((addr + len + 1 & 0xfffffffe) - (addr & 0xfffffffe) > 0x2000) {
        len = 0x2000 - (addr & 1);
      }
    }
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
  slot->op = 9;
  slot->result = 0;
  copy = slot->copy_back;
  copy[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_recv_pack_args(sock,addr,len,flags,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  dimm_mem_read_chunk(addr,(void *)dst,len,use_dma,0);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int sock_recv_to_area_async(uint sock, int offset, int len, uint flags)
{
  uint addr;
  int taken;
  sock_slot_rec *slot;
  int *copy;

  dimm_board_info_fetch();
  addr = recv_area_addr(offset);
  if (sock >= 0x40 || recv_area_span_check(addr,len) != 0) return -2;
  taken = busy_word_try_set(&dimm_busy_word);
  if (taken == 0) return -4;
  if (sock == 0) {
    taken = busy_word_try_set(&sock_slot_at(0)->busy_word);
    if (taken == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 9;
  slot->result = 0;
  copy = slot->copy_back;
  slot->copy_back[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_recv_pack_args(sock,addr,len,flags,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

void sock_recvfrom_pack_args(uint sock, uint addr, int len, uint flags, uint from_board, uint from_len_board, sock_slot_rec *slot)
{
  uint b[8];
  b[7] = 0;
  b[0] = slot->op;
  b[1] = sock;
  b[2] = addr;
  b[3] = len;
  b[4] = flags;
  b[5] = from_board;
  b[6] = from_len_board;
  dimm_pio_write((ushort *)b,slot->req_addr,0x20);
}

void sock_recvfrom_put_addrlen(uint board_addr, uint *from_len)
{
  uint word;

  word = *from_len;
  dimm_pio_write((ushort *)&word,board_addr,4);
  return;
}

void sock_recvfrom_copy_addr(uint sock, int from_board, int from, int from_len_board, uint *from_len)
{
  int data;
  size_t n;
  if (from != 0) {
    data = sock_slot_data_addr(sock,0,0);
    dimm_pio_read(data,(ushort *)xfer_recv_bounce,0x200);
    memcpy(from_len,xfer_recv_bounce + (from_len_board - data),4);
    n = *from_len;
    n = n >= 0x10 ? n : 0x10;
    memcpy((void *)from,xfer_recv_bounce + (from_board - data),n);
  }
}

int sock_recvfrom(uint sock, uint dst, int len, uint flags, int from, int from_len)
{
  dimm_board_info_rec *info;
  uint addr;
  int r;
  int result;
  int use_dma;
  sock_slot_rec *slot;
  int from_board;
  int from_len_board;
  int *copy;

  addr = 0;
  info = dimm_board_info_fetch();
  if ((info->id_word & 0x8000000) == 0) {
    return -3;
  }
  xfer_setting_get(0,&use_dma);
  addr = recv_area_addr(addr);
  if ((((0x3f < sock || dst == 0) || (r = recv_area_span_check(addr,len), r != 0)) ||
      (from != 0 && from_len == 0)) || (from == 0 && from_len != 0)) {
    return -2;
  }
  if (use_dma != 0) {
    if ((((dst & 0x1f) != 0) || ((addr & 0x1f) != 0)) &&
        ((addr + len + 0x1f & 0xffffffe0) - (addr & 0xffffffe0) > 0x2000))
      len = 0x2000 - (addr & 0x1f);
  }
  else {
    if ((((dst & 1) != 0) || ((addr & 1) != 0)) &&
        ((addr + len + 1 & 0xfffffffe) - (addr & 0xfffffffe) > 0x2000))
      len = 0x2000 - (addr & 1);
  }
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  if (from != 0) {
    from_board = sock_slot_data_addr(sock,4,0x40);
    from_len_board = sock_slot_data_addr(sock,0,4);
    sock_recvfrom_put_addrlen(from_len_board,(uint *)from_len);
  }
  else {
    from_board = 0;
    from_len_board = 0;
  }
  slot->done = 0;
  slot->op = 0x16;
  slot->result = 0;
  copy = slot->copy_back;
  copy[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_recvfrom_pack_args(sock,addr,len,flags,from_board,from_len_board,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  sock_recvfrom_copy_addr(sock,from_board,from,from_len_board,(uint *)from_len);
  dimm_mem_read_chunk(addr,(void *)dst,len,use_dma,0);
  busy_word_drop(&dimm_busy_word);
  return result;
}

int sock_recvfrom_to_area_async(uint sock, int offset, int len, uint flags, int from, int from_len)
{
  sock_slot_rec *slot;
  int *copy;
  int from_board;
  int from_len_board;

  if ((dimm_board_info_fetch()->id_word & 0x8000000) == 0) return -3;
  offset = recv_area_addr(offset);
  if (0x3f < sock || recv_area_span_check(offset,len) != 0 ||
      (from != 0 && from_len == 0) || (from == 0 && from_len != 0))
    return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  if (from != 0) {
    from_board = sock_slot_data_addr(sock,4,0x40);
    from_len_board = sock_slot_data_addr(sock,0,4);
    sock_recvfrom_put_addrlen(from_len_board,(uint *)from_len);
  }
  else {
    from_board = 0;
    from_len_board = 0;
  }
  slot->done = 0;
  slot->op = 0x16;
  slot->result = 0;
  copy = slot->copy_back;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  slot->copy_back[0] = from_board;
  slot->copy_back[1] = from;
  slot->copy_back[2] = from_len_board;
  slot->copy_back[3] = from_len;
  sock_recvfrom_pack_args(sock,offset,len,flags,from_board,from_len_board,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

int sock_recv_to_board(uint sock, uint board_addr, int len)
{
  int r;
  sock_slot_rec *slot0;
  sock_slot_rec *slot;
  int *copy;

  dimm_board_info_fetch();
  if (sock > 0x3f) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    slot0 = sock_slot_at(0);
    if (busy_word_try_set(&slot0->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 9;
  slot->result = 0;
  copy = slot->copy_back;
  slot->copy_back[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_recv_pack_args(sock,board_addr,len,0,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int sock_recv_to_board_async(uint sock, uint board_addr, int len)
{
  sock_slot_rec *slot;
  sock_slot_rec *slot0;

  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    slot0 = sock_slot_at(0);
    if (busy_word_try_set(&slot0->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 9;
  slot->result = 0;
  {
    int *copy = slot->copy_back;
    copy[0] = 0;
    copy[1] = 0;
    copy[2] = 0;
    copy[3] = 0;
  }
  sock_recv_pack_args(sock,board_addr,len,0,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
