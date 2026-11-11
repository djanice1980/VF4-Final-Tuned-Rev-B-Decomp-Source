/*
 * sdk/nlasock/peer.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

int peer_area_reset(int window_base)
{
  peer_request_rec *s = &peer_request_state;
  uint area = 0xd00000 + window_base;
  peer_area_base = area;
  s->done = 1;
  s->result = 0;
  s->area_addr = area;
  peer_set_transfer_mode(0,0);
  peer_set_transfer_mode(1,0);
  dimm_cmd_handler_put(0x30,peer_reply_handler);
  return 0;
}

int peer_reply_handler(uint cmd_word, uint offset, uint arg_lo, int arg_hi)
{
  peer_request_state.result = arg_hi << 0x10 | arg_lo & 0xffff;
  peer_request_state.done = 1;
  return 0;
}

int peer_set_transfer_mode(int which, int value)
{
  int r = -1;
  switch (which) {
  case 0:
    peer_area_use_dma = value;
    r = 0;
    break;
  case 1:
    peer_area_dma_nowait = value;
    r = 0;
    break;
  }
  return r;
}

int peer_get_transfer_mode(int which, int *out)
{
  int r = -1;
  if (out != (int *)0x0) {
    switch (which) {
    case 0: *out = peer_area_use_dma; r = 0; break;
    case 1: *out = peer_area_dma_nowait; r = 0; break;
    }
  }
  return r;
}

int peer_request_result_get(void)
{
  if (peer_request_state.done == 1) {
    return peer_request_state.result;
  }
  return -2;
}

int peer_request_done_get(void)
{
  return peer_request_state.done;
}

int peer_area_range_check(uint offset, int len)
{
  if (offset > 0xffff || offset + len > 0x10000) return -1;
  return 0;
}

int peer_area_address(int offset)
{
  return offset + peer_area_base;
}

int peer_area_read_chunk(uint offset, void *dst, int len, int use_dma, int no_wait)
{
  int bad;
  uint addr;
  uint size;
  char *buf;
  int big;
  uint base;

  if (use_dma != 0) {
    bad = 0;
    if ((((uint)dst & 0x1f) != 0) || ((offset & 0x1f) != 0)) {
      bad = 1;
    }
    base = offset & 0xffffffe0;
    if (bad) buf = peer_area_bounce;
    else buf = dst;
    addr = peer_area_address(base);
    size = ((offset + len + 0x1f) & 0xffffffe0) - base;
    big = 0x1000 < size;
    if ((big & bad) || peer_area_range_check(offset,len) != 0) {
      return -1;
    }
    dcache_range_purge(buf,size);
    dimm_dma_read(addr,(uint)buf,size,no_wait);
    if (bad) {
      memcpy(dst,buf + (offset & 0x1f),len);
    }
  }
  else {
    bad = 0;
    if ((((uint)dst & 1) != 0) || ((offset & 1) != 0)) {
      bad = 1;
    }
    base = offset & 0xfffffffe;
    if (bad) buf = peer_area_bounce;
    else buf = dst;
    addr = peer_area_address(base);
    size = ((offset + len + 1) & 0xfffffffe) - base;
    big = 0x1000 < size;
    if ((big & bad) || peer_area_range_check(offset,len) != 0) {
      return -1;
    }
    dimm_pio_read(addr,(ushort *)buf,size);
    if (bad) {
      memcpy(dst,buf + (offset & 1),len);
    }
  }
  return len;
}

int peer_area_read(uint offset, void *dst, int len)
{
  int held;
  int r;
  int use_dma;
  int no_wait;

  held = busy_word_try_set(&dimm_busy_word);
  r = -1;
  if (held == 0) return r;
  peer_get_transfer_mode(0,&use_dma);
  peer_get_transfer_mode(1,&no_wait);
  r = peer_area_read_chunk(offset,dst,len,use_dma,no_wait);
  busy_word_drop(&dimm_busy_word);
  return r;
}

int peer_area_write_chunk(void *src, uint offset, int len, int use_dma, int no_wait)
{
  int bad;
  uint addr;
  uint size;
  char *buf;
  int big;
  uint base;

  if (use_dma != 0) {
    bad = 0;
    if ((((uint)src & 0x1f) != 0) || ((offset & 0x1f) != 0)) {
      bad = 1;
    }
    if (bad) buf = peer_area_bounce;
    else buf = src;
    base = offset & 0xffffffe0;
    addr = peer_area_address(base);
    size = ((offset + len + 0x1f) & 0xffffffe0) - base;
    big = 0x1000 < size;
    if ((big & bad) || peer_area_range_check(offset,len) != 0) {
      return -1;
    }
    if (bad) {
      memcpy(buf + (offset & 0x1f),src,len);
    }
    dcache_range_purge(buf,size);
    dimm_dma_write((uint)buf,addr,size,no_wait);
  }
  else {
    bad = 0;
    if ((((uint)src & 1) != 0) || ((offset & 1) != 0)) {
      bad = 1;
    }
    if (bad) buf = peer_area_bounce;
    else buf = src;
    base = offset & 0xfffffffe;
    addr = peer_area_address(base);
    size = ((offset + len + 1) & 0xfffffffe) - base;
    big = 0x1000 < size;
    if ((big & bad) || peer_area_range_check(offset,len) != 0) {
      return -1;
    }
    if (bad) {
      memcpy(buf + (offset & 1),src,len);
    }
    dimm_pio_write((ushort *)buf,addr,size);
  }
  return len;
}

int peer_area_write(void *src, uint offset, int len)
{
  int held;
  int r;
  int use_dma;
  int no_wait;

  held = busy_word_try_set(&dimm_busy_word);
  r = -1;
  if (held == 0) return r;
  peer_get_transfer_mode(0,&use_dma);
  peer_get_transfer_mode(1,&no_wait);
  r = peer_area_write_chunk(src,offset,len,use_dma,no_wait);
  busy_word_drop(&dimm_busy_word);
  return r;
}

void peer_start_put_table(peer_request_rec *req, uint start_value, uint port, int count, void *list, uint *table_addr)
{
  *table_addr = req->area_addr + 0xff000;
  peer_start_table.start_value = (ushort)start_value;
  peer_start_table.port = (ushort)port;
  memcpy(peer_start_table.list,list,count * 4);
  dimm_pio_write((ushort *)&peer_start_table,req->area_addr + 0xff000,
               count * 4 + 0x23U & 0xffffffe0);
}

int peer_start(uint value, uint port, int count, uint index, uint mode, uint *list)
{
  ushort value16 = value;
  ushort port16 = port;
  ushort mode16 = mode;
  volatile peer_request_rec *req = &peer_request_state;
  uint table_addr[1];
  int r;

  if ((dimm_board_info_fetch()->feature_bits & 0x80000) == 0) return -3;
  if (req->done != 1) return -2;
  if ((ushort)(value16 - 1) > 0xfff) goto bad;
  if ((uint)((port16 & 0xff) << 8 | port16 >> 8) - 0x400 > 0xfbff) goto bad;
  if ((uint)(count - 1) > 15) goto bad;
  if ((int)index < 0) goto bad;
  if ((int)index > count - 1) goto bad;
  if (list != 0) goto ok;
bad:
  return -2;
ok:
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  peer_start_put_table((peer_request_rec *)req,value16,port16,count,list,table_addr);
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x33,mode16 * 2 | 1,count << 8 | index,table_addr[0] & 0xffff,table_addr[0] >> 16 & 0xffff);
  while (req->done != 1) ;
  r = req->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

int peer_cmd35_post_wait(uint value, ushort param, ushort channel)
{
  volatile peer_request_rec *req = &peer_request_state;
  if ((dimm_board_info_fetch()->feature_bits & 0x200000) == 0) return -3;
  if (req->done != 1) return -2;
  if (channel >= 8) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x35,channel * 2,value & 0xffff,value >> 0x10 & 0xffff,param);
  while (req->done != 1) ;
  {
    int r = req->result;
    busy_word_drop(&dimm_busy_word);
    return r;
  }
}

int peer_cmd35_post(uint value, ushort param, ushort channel)
{
  dimm_board_info_rec *info;
  peer_request_rec *req = &peer_request_state;

  info = dimm_board_info_fetch();
  if ((info->feature_bits & 0x200000) == 0) return -3;
  if (req->done != 1) return -2;
  if (channel >= 8) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x35,channel * 2,value & 0xffff,value >> 0x10 & 0xffff,param);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

int peer_cmd31_query0(void)
{
  volatile peer_request_rec *req;
  dimm_board_info_rec *info;
  int r;

  req = &peer_request_state;
  info = dimm_board_info_fetch();
  if ((info->feature_bits & 0x20000) == 0) {
    return -3;
  }
  if (req->done != 1) {
    return -2;
  }
  if (busy_word_try_set(&dimm_busy_word) == 0) {
    return -2;
  }
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x31,0,0,0,0);
  while (req->done != 1) {
  }
  r = req->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

int peer_cmd31_query1(void)
{
  volatile peer_request_rec *req = &peer_request_state;
  int r;

  if ((dimm_board_info_fetch()->feature_bits & 0x20000) == 0) return -3;
  if (req->done != 1) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x31,1,0,0,0);
  while (req->done != 1) ;
  r = req->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

int peer_area_send(int len, uint mode)
{
  dimm_board_info_rec *info;
  int r;
  peer_request_rec *req = &peer_request_state;

  info = dimm_board_info_fetch();
  if ((info->feature_bits & 0x100000) == 0) return -3;
  if (req->done != 1) return -2;
  if (peer_area_range_check(0,len) != 0) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x34,mode,len,0,0);
  while (*(volatile int *)&req->done != 1);
  r = req->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}

int peer_area_send_async(int len, uint mode)
{
  peer_request_rec *req = &peer_request_state;

  if ((dimm_board_info_fetch()->feature_bits & 0x100000) == 0) return -3;
  if (req->done != 1) return -2;
  if (peer_area_range_check(0,len) != 0) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x34,mode,len,0,0);
  busy_word_drop(&dimm_busy_word);
  return 0;
}

int peer_stop(void)
{
  volatile peer_request_rec *req = &peer_request_state;
  dimm_board_info_rec *info;
  int r;
  info = dimm_board_info_fetch();
  if ((info->feature_bits & 0x80000) == 0) return -3;
  if (req->done != 1) return -2;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -2;
  req->done = 0;
  req->result = 0;
  dimm_cmd_post(0x33,0,0,0,0);
  while (req->done != 1) ;
  r = req->result;
  busy_word_drop(&dimm_busy_word);
  return r;
}
