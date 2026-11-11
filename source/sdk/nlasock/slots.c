/*
 * sdk/nlasock/slots.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

sock_slot_rec * sock_slot_at(uint sock)
{
  sock_slot_rec *slot;

  if (sock < 0x40) {
    slot = &sock_slot_table[sock];
  }
  else {
    slot = sock_slot_table;
  }
  return slot;
}

int sock_slot_data_addr(uint sock, int offset, int size)
{
  sock_slot_rec *slot;
  int r; slot = sock_slot_at(sock); r = slot->req_addr + 0x200; return r + offset;
}

int sock_slots_prepare(int window_base)
{
  sock_slot_rec *slot;
  int i;

  net_window_base = window_base;
  send_area_base = 0xa00000 + window_base;
  recv_area_base = 0xb00000 + window_base;
  for (i = 0; i < 0x40; i++) {
    slot = sock_slot_at(i);
    slot->done = 1;
    slot->op = 0;
    slot->result = 0;
    slot->req_addr = 0x800000 + i * 0x1000 + window_base;
    busy_word_clear(&slot->busy_word);
  }
  xfer_setting_put(0,0);
  xfer_setting_put(1,0);
  dimm_cmd_handler_put(0x13,sock_reply_take);
  dimm_cmd_handler_put(0x14,sock_reply_take);
  return 0;
}

int sock_reply_take(byte cmd_low_byte, ushort sock, uint arg_lo, int arg_hi)
{
  sock_slot_rec *slot;

  slot = sock_slot_at(sock);
  if (cmd_low_byte == 4) {
    slot->result = arg_hi << 0x10 | (ushort)arg_lo;
  }
  else {
    slot->result = -1;
  }
  slot->done = 1;
  return 4;
}

int xfer_setting_put(int which, int value)
{
  int r = -1;
  switch (which) {
  case 0:
    xfer_use_dma = value;
    r = 0;
    break;
  case 1:
    xfer_no_wait = value;
    r = 0;
    break;
  }
  return r;
}

int xfer_setting_get(int which, int *out)
{
  int r;

  r = -1;
  if (out != (int *)0x0) {
    switch (which) {
    case 0:
      r = (int)&xfer_use_dma;
      break;
    case 1:
      r = (int)&xfer_no_wait;
      break;
    default:
      return r;
    }
    *out = *(int *)r;
    r = 0;
  }
  return r;
}

int sock_result_take(int sock)
{
  sock_slot_rec *slot;

  if (0x3f < sock) goto bad;
  slot = sock_slot_at(sock);
  if (slot->done != 1) goto bad;
  if (slot->op == 0) goto bad;
  if (slot->copy_back[0] != 0) {
    busy_word_try_set(&dimm_busy_word);
    switch (slot->op) {
    case 1:
      accept_addr_copy_back(sock,slot->copy_back[0],slot->copy_back[1],slot->copy_back[2],(uint *)slot->copy_back[3]);
      break;
    case 0x16:
      sock_recvfrom_copy_addr(sock,slot->copy_back[0],slot->copy_back[1],slot->copy_back[2],(uint *)slot->copy_back[3]);
      break;
    }
    busy_word_drop(&dimm_busy_word);
  }
  if (sock == 0) {
    busy_word_drop(&sock_slot_at(0)->busy_word);
  }
  return slot->result;
bad:
  return -2;
}

int sock_call_done(int sock)
{
  sock_slot_rec *slot;
  int done;

  if (sock < 0x40) {
    slot = sock_slot_at(sock);
    done = slot->done;
  }
  else {
    done = 1;
  }
  return done;
}

int recv_area_addr(int offset)
{
  return offset + recv_area_base;
}

int send_area_addr(int offset)
{
  return offset + send_area_base;
}

int recv_area_span_check(uint addr, int len)
{
  uint lo = recv_area_addr(0);
  uint hi = recv_area_addr(0x100000);
  if (addr < lo || hi <= addr || addr + len <= lo || hi < addr + len) return -1;
  return 0;
}

int send_area_span_check(uint addr, int len)
{
  uint lo;
  uint hi;
  lo = send_area_addr(0);
  hi = send_area_addr(0x100000);
  if (addr < lo || hi <= addr || addr + len <= lo || hi < addr + len)
    return -1;
  return 0;
}

int dimm_mem_read_chunk(uint addr, void *dst, int len, int use_dma, int no_wait)
{
  int bad;
  uint base;
  char *buf;
  uint size;

  if (use_dma != 0) {
    bad = 0;
    if ((((uint)dst & 0x1f) != 0) || ((addr & 0x1f) != 0)) bad = 1;
    base = addr & 0xffffffe0;
    if (bad) buf = xfer_recv_bounce;
    else buf = dst;
    size = (addr + len + 0x1f & 0xffffffe0) - base;
    if ((size > 0x2000) & (bad != 0)) return -1;
    dcache_range_purge(buf,size);
    dimm_dma_read(base,(uint)buf,size,no_wait);
    if (bad) memcpy(dst,buf + (addr & 0x1f),len);
  }
  else {
    bad = 0;
    if ((((uint)dst & 1) != 0) || ((addr & 1) != 0)) bad = 1;
    base = addr & 0xfffffffe;
    if (bad) buf = xfer_recv_bounce;
    else buf = dst;
    size = (addr + len + 1 & 0xfffffffe) - base;
    if ((size > 0x2000) & (bad != 0)) return -1;
    dimm_pio_read(base,(ushort *)buf,size);
    if (bad) memcpy(dst,buf + (addr & 1),len);
  }
  return len;
}

int recv_area_read(int offset, uint dst, int len)
{
  int total = 0;
  uint addr;
  int n;
  int r;
  int mode[2];

  addr = recv_area_addr(offset);
  if (recv_area_span_check(addr,len) != 0) return -1;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -1;
  xfer_setting_get(0,&mode[0]);
  xfer_setting_get(1,&mode[1]);
  while (len > 0) {
    n = len;
    if (mode[0] != 0) {
      if ((dst & 0x1f) != 0 || (addr & 0x1f) != 0) {
        if (((addr + n + 0x1f) & ~0x1f) - (addr & ~0x1f) > 0x2000) n = 0x2000 - (addr & 0x1f);
      }
    }
    else {
      if ((dst & 1) != 0 || (addr & 1) != 0) {
        if (((addr + n + 1) & ~1) - (addr & ~1) > 0x2000) n = 0x2000 - (addr & 1);
      }
    }
    r = dimm_mem_read_chunk(addr,(void *)dst,n,mode[0],mode[1]);
    if (r == -1) break;
    total += r;
    addr += r;
    dst += r;
    len -= r;
  }
  busy_word_drop(&dimm_busy_word);
  return total;
}

int dimm_mem_read(uint addr, uint dst, int len, int use_dma, int no_wait)
{
  int total = 0;
  int n;
  int r;

  if (busy_word_try_set(&dimm_busy_word) == 0) return -1;
  while (len > 0) {
    n = len;
    if (use_dma != 0) {
      if (((dst & 0x1f) != 0 || (addr & 0x1f) != 0) &&
          ((addr + n + 0x1f & 0xffffffe0) - (addr & 0xffffffe0)) > 0x2000)
        n = 0x2000 - (addr & 0x1f);
    }
    else {
      if (((dst & 1) != 0 || (addr & 1) != 0) &&
          ((addr + n + 1 & 0xfffffffe) - (addr & 0xfffffffe)) > 0x2000)
        n = 0x2000 - (addr & 1);
    }
    r = dimm_mem_read_chunk(addr,(void *)dst,n,use_dma,no_wait);
    if (r == -1) break;
    total += r;
    addr += r;
    dst += r;
    len -= r;
  }
  busy_word_drop(&dimm_busy_word);
  return total;
}

int dimm_mem_write_chunk(void *src, uint addr, int len, int use_dma, int no_wait)
{
  int bad;
  char *buf;
  uint lo;
  uint size;
  int big;
  if (use_dma != 0) {
    bad = 0;
    if (((uint)src & 0x1f) != 0 || (addr & 0x1f) != 0) bad = 1;
    if (bad) buf = xfer_send_bounce;
    else buf = src;
    lo = addr & 0xffffffe0;
    size = (addr + len + 0x1f & 0xffffffe0) - lo;
    big = size > 0x2000;
    if (big & bad) return -1;
    if (bad) memcpy(buf + (addr & 0x1f), src, len);
    dcache_range_purge(buf, size);
    dimm_dma_write((uint)buf, lo, size, no_wait);
  } else {
    bad = 0;
    if (((uint)src & 1) != 0 || (addr & 1) != 0) bad = 1;
    if (bad) buf = xfer_send_bounce;
    else buf = src;
    lo = addr & 0xfffffffe;
    size = (addr + len + 1 & 0xfffffffe) - lo;
    big = size > 0x2000;
    if (big & bad) return -1;
    if (bad) memcpy(buf + (addr & 1), src, len);
    dimm_pio_write((ushort *)buf, lo, size);
  }
  return len;
}

int send_area_write(uint src, int offset, int len)
{
  uint addr;
  int n;
  int r;
  int total;
  int mode[2];

  total = 0;
  addr = send_area_addr(offset);
  if (send_area_span_check(addr,len) != 0) return -1;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -1;
  xfer_setting_get(0,&mode[0]);
  xfer_setting_get(1,&mode[1]);
  while (len > 0) {
    n = len;
    if (mode[0] != 0) {
      if (((src & 0x1f) != 0 || (addr & 0x1f) != 0) &&
          ((addr + n + 0x1f & 0xffffffe0) - (addr & 0xffffffe0)) > 0x2000) {
        n = 0x2000 - (addr & 0x1f);
      }
    }
    else {
      if (((src & 1) != 0 || (addr & 1) != 0) &&
          ((addr + n + 1 & 0xfffffffe) - (addr & 0xfffffffe)) > 0x2000) {
        n = 0x2000 - (addr & 1);
      }
    }
    r = dimm_mem_write_chunk((void *)src,addr,n,mode[0],mode[1]);
    if (r == -1) break;
    total += r;
    addr += r;
    src += r;
    len -= r;
  }
  busy_word_drop(&dimm_busy_word);
  return total;
}

int dimm_mem_write(uint src, uint addr, int len, int use_dma, int no_wait)
{
  int total;
  int n;
  int r;

  total = 0;
  if (busy_word_try_set(&dimm_busy_word) == 0) {
    return -1;
  }
  while (len > 0) {
    n = len;
    if (use_dma != 0) {
      if (((src & 0x1f) != 0 || (addr & 0x1f) != 0) &&
          ((addr + n + 0x1f) & ~0x1f) - (addr & ~0x1f) > 0x2000) {
        n = 0x2000 - (addr & 0x1f);
      }
    }
    else {
      if (((src & 1) != 0 || (addr & 1) != 0) &&
          ((addr + n + 1) & ~1) - (addr & ~1) > 0x2000) {
        n = 0x2000 - (addr & 1);
      }
    }
    r = dimm_mem_write_chunk((void *)src,addr,n,use_dma,no_wait);
    if (r == -1) break;
    total += r;
    addr += r;
    src += r;
    len -= r;
  }
  busy_word_drop(&dimm_busy_word);
  return total;
}

uint ip_text_parse(char *text)
{
  char *part;
  int i;
  char buf[29];
  uint addr;

  strncpy(buf,text,0x1f);
  part = strtok(buf," \t\n.");
  i = 0;
  do {
    if (part == (char *)0x0) {
      return addr;
    }
    ((char *)&addr)[i] = (strtol(part,0,10));
    part = strtok((char *)0x0," \t\n.");
    i = i + 1;
  } while (4 > i);
  return addr;
}

int sock_close_all(void)
{
  uint r;
  int sock;
  int count;

  count = 0;
  sock = 0;
  do {
    r = sock_close(sock);
    count = count + (uint)((r & 0x80000000) == 0);
    sock = sock + 1;
  } while (sock < 0x40);
  return count;
}

int board_status_read(uint *status)
{
  uint addr;
  uint *buf;
  uint *out;

  if (status == (uint *)0x0) return -1;
  addr = 0xff0000 + net_window_base;
  buf = (uint *)xfer_recv_bounce;
  out = status;
  if (busy_word_try_set(&dimm_busy_word) == 0) return -1;
  dimm_pio_read(addr,(ushort *)buf,0x20);
  busy_word_drop(&dimm_busy_word);
  out[0] = buf[0];
  out[1] = buf[1];
  out[2] = buf[2];
  out[3] = buf[3];
  out[4] = buf[4];
  out[5] = buf[5];
  out[6] = buf[6];
  status[7] = addr;
  return 0;
}
