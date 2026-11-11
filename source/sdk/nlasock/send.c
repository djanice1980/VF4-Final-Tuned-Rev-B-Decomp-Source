/*
 * sdk/nlasock/send.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sock_send_pack_args(uint sock, uint addr, int len, uint flags, sock_slot_rec *slot)
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

int sock_send(uint sock, uint src, int len, uint flags)
{
  uint addr;
  int r;
  int result;
  sock_slot_rec *slot;
  int use_dma;
  uint flags_kept;

  flags_kept = flags;
  dimm_board_info_fetch();
  xfer_setting_get(0,&use_dma);
  addr = send_area_addr(0);
  if ((0x3f < sock | src == 0) || send_area_span_check(addr,len) != 0) {
    return -2;
  }
  if (use_dma != 0) {
    if ((src & 0x1f) != 0 || (addr & 0x1f) != 0) {
      if ((addr + len + 0x1f & 0xffffffe0) - (addr & 0xffffffe0) > 0x2000)
        len = 0x2000 - (addr & 0x1f);
    }
  }
  else {
    if ((src & 1) != 0 || (addr & 1) != 0) {
      if ((addr + len + 1 & 0xfffffffe) - (addr & 0xfffffffe) > 0x2000)
        len = 0x2000 - (addr & 1);
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
  dimm_mem_write_chunk((void *)src,addr,len,use_dma,0);
  slot->done = 0;
  slot->op = 10;
  slot->result = 0;
  {
    int *copy = slot->copy_back;
    slot->copy_back[0] = 0;
    copy[1] = 0;
    copy[2] = 0;
    copy[3] = 0;
  }
  sock_send_pack_args(sock,addr,len,flags_kept,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}

int sock_send_from_area_async(uint sock, int offset, int len, uint flags)
{
  uint addr;
  sock_slot_rec *slot0;
  sock_slot_rec *slot;

  dimm_board_info_fetch();
  addr = send_area_addr(offset);
  if (sock >= 0x40 || send_area_span_check(addr,len) != 0) return -2;
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
  slot->op = 10;
  slot->result = 0;
  {
    int *copy = slot->copy_back;
    slot->copy_back[0] = 0;
    copy[1] = 0;
    copy[2] = 0;
    copy[3] = 0;
  }
  sock_send_pack_args(sock,addr,len,flags,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

void sock_sendto_pack_args(uint sock, uint addr, int len, uint flags, uint to_board, int to_len, sock_slot_rec *slot)
{
  uint b[8];
  b[7] = 0;
  b[0] = slot->op;
  b[1] = sock;
  b[2] = addr;
  b[3] = len;
  b[4] = flags;
  b[5] = to_board;
  b[6] = to_len;
  dimm_pio_write((ushort *)b,slot->req_addr,0x20);
}

void sock_sendto_put_addr(uint board_addr, void *to)
{
  uint word[4];
  word[0] = 0;
  word[1] = 0;
  word[2] = 0;
  word[3] = 0;
  memcpy(word,to,0x10);
  dimm_pio_write((ushort *)word,board_addr,0x10);
}

int sock_sendto(uint sock, uint src, int len, uint flags, int to, int to_len)
{
  uint addr;
  int use_dma;
  sock_slot_rec *slot;
  int to_board;
  int r;
  int *copy;

  addr = 0;
  if ((dimm_board_info_fetch()->id_word & 0x8000000) == 0) return -3;
  xfer_setting_get(0,&use_dma);
  addr = send_area_addr(addr);
  if ((sock > 0x3f | src == 0) || send_area_span_check(addr,len) != 0 || (to == 0 | to_len > 0x40))
    return -2;
  if (use_dma != 0) {
    if (((src & 0x1f) != 0 || (addr & 0x1f) != 0) && ((addr + len + 0x1f & ~0x1f) - (addr & ~0x1f)) > 0x2000)
      len = 0x2000 - (addr & 0x1f);
  }
  else {
    if (((src & 1) != 0 || (addr & 1) != 0) && ((addr + len + 1 & ~1) - (addr & ~1)) > 0x2000)
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
  to_board = sock_slot_data_addr(sock,0,0x40);
  sock_sendto_put_addr(to_board,(void *)to);
  dimm_mem_write_chunk((void *)src,addr,len,use_dma,0);
  slot->done = 0;
  slot->op = 0x17;
  slot->result = 0;
  copy = slot->copy_back;
  copy[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_sendto_pack_args(sock,addr,len,flags,to_board,to_len,slot);
  dimm_cmd_post_bare(0x11,sock);
  while (sock_call_done(sock) != 1) ;
  r = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int sock_sendto_from_area_async(uint sock, int offset, int len, uint flags, int to, int to_len)
{
  sock_slot_rec *slot;
  int *copy;
  int to_board;

  if ((dimm_board_info_fetch()->id_word & 0x08000000) == 0) return -3;
  offset = send_area_addr(offset);
  if (sock > 0x3f || send_area_span_check(offset,len) != 0 || ((to == 0) | (to_len > 0x40))) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  to_board = sock_slot_data_addr(sock,0,0x40);
  sock_sendto_put_addr(to_board,(void *)to);
  slot->done = 0;
  slot->op = 0x17;
  slot->result = 0;
  copy = slot->copy_back;
  copy[0] = 0;
  copy[1] = 0;
  copy[2] = 0;
  copy[3] = 0;
  sock_sendto_pack_args(sock,offset,len,flags,to_board,to_len,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

int sock_send_from_board(uint sock, uint board_addr, int len)
{
  int result;
  int r;
  sock_slot_rec *slot;

  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 10;
  slot->result = 0;
  {
    int *copy = slot->copy_back;
    copy[0] = 0;
    copy[1] = 0;
    copy[2] = 0;
    copy[3] = 0;
  }
  sock_send_pack_args(sock,board_addr,len,0,slot);
  dimm_cmd_post_bare(0x11,sock);
  do {
    r = sock_call_done(sock);
  } while (r != 1);
  result = sock_result_take(sock);
  busy_word_drop(&dimm_busy_word);
  return result;
}

int sock_send_from_board_async(uint sock, uint board_addr, int len)
{
  sock_slot_rec *slot;
  dimm_board_info_fetch();
  if (sock >= 0x40) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -4;
  if (sock == 0) {
    if (busy_word_try_set(&sock_slot_at(0)->busy_word) == 0) {
      busy_word_drop(&dimm_busy_word);
      return -4;
    }
  }
  slot = sock_slot_at(sock);
  slot->done = 0;
  slot->op = 10;
  slot->result = 0;
  { int *copy = slot->copy_back; copy[0] = 0; copy[1] = 0; copy[2] = 0; copy[3] = 0; }
  sock_send_pack_args(sock,board_addr,len,0,slot);
  dimm_cmd_post_bare(0x12,sock);
  busy_word_drop(&dimm_busy_word);
  return 0;
}
