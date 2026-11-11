/*
 * sdk/nladimm/nladimm.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int dimm_module_sizes[4]; /* 0x0c211b8c */
extern dimm_command_def dimm_command_table[32]; /* 0x0c211b9c */
extern uint dimm_last_status; /* 0x0c413458 */
extern dimm_board_info_rec dimm_board_info; /* 0x0c5d4540 */
extern dimm_request_rec dimm_request; /* 0x0c5d45c0 */
extern dimm_request_arg_buf dimm_request_arg; /* 0x0c5d45d0 */

char * nladimm_version_get(void)
{
  return "\nNLADIMM Ver.0.06 Build: Oct 19 2004 19:52:03\n";
}

void dimm_spin_wait(int count)
{
  volatile int n;

  n = count;
  do {
    n = n - 1;
  } while (n != -1);
}

int dimm_g1_timing_apply(int keep)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint v;
  unsigned int m;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (keep == 0) {
    bios_g1_dma_timing = 0x1106;
    *(volatile uint *)0xa05f74a4 = 0x1106;
    *(volatile uint *)0xa05f74a0 = 0x1106;
  }
  v = dimm_request_hook_vector;
  bios_helper_58(0xffffffff,v);
  dimm_bios_word_ff1c = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

dimm_board_info_rec * dimm_board_info_fetch(void)
{
  dimm_board_info_rec *info = &dimm_board_info;
  int status;
  int i;
  uint t;

  memset(info,0,0x80);
  info->id_word = dimm_board_id_word;
  if (info->id_word == 0) {
    status = 1;
  }
  else {
    info->id_low = info->id_word;
    info->id_top = info->id_word >> 30 & 3;
    for (i = 0; i < 2; i++) {
      info->module_code[i] = info->id_word >> (i * 4 + 16) & 0xf;
    }
    t = dimm_board_word_14;
    info->word_14_byte = t;
    t = dimm_board_word_18;
    info->word_18_bit16 = t >> 16 & 1;
    info->word_18_byte = t;
    info->word_20 = dimm_board_word_20;
    info->feature_bits = dimm_board_word_24;
    info->word_28 = dimm_board_word_28;
    info->word_2c = dimm_board_word_2c;
    info->module_bytes = 0;
    for (i = 0; i < 2; i++) {
      info->module_bytes += dimm_module_sizes[info->module_code[i]];
    }
    if (info->module_bytes != 0) info->module_base = 0xff000000 + info->module_bytes;
    else info->module_base = 0;
    memcpy(info->field_0x28,&dimm_board_extra,0x58);
    status = 0;
  }
  dimm_last_status = status;
  return info;
}

int dimm_firmware_version_get(uint *major, uint *minor)
{
  uint id_bit29;
  char arg[4];
  uint reply;
  uint command;
  char *p;

  if ((major == (uint *)0x0) | (minor == (uint *)0x0)) {
    dimm_last_status = 2;
    return 2;
  }
  if (dimm_comms_mode == 2) {
    id_bit29 = dimm_board_id_word & 0x20000000;
    if (id_bit29 != 0) {
      p = arg;
      memcpy(p,dimm_board_extra.cmd10_arg,4);
      command = 10;
    }
    else {
      p = arg;
      memcpy(p,dimm_board_extra.cmd6_arg,4);
      command = 6;
    }
    dimm_command_post(command,(int)p);
    while ((int)dimm_command_poll(&reply) < 2) {
      dimm_spin_wait(0x6f540);
    }
    if (dimm_last_status == 0) {
      *major = reply >> 0x18;
      *minor = reply >> 0x10 & 0xff;
    }
    else if (id_bit29 == 0 && dimm_last_status - 3 <= 1) {
      dimm_last_status = id_bit29;
      reply = dimm_reply_word;
      *major = reply >> 0x18;
      *minor = reply >> 0x10 & 0xff;
    }
    else {
      *major = 0;
      *minor = 0;
    }
  }
  else {
    reply = dimm_reply_word;
    *major = reply >> 0x18;
    *minor = reply >> 0x10 & 0xff;
    if (*major == 0 && *minor == 0) {
      dimm_last_status = 4;
    }
    else {
      dimm_last_status = 0;
    }
  }
  return dimm_last_status;
}

uint dimm_page_fd00_get(void)
{
  dimm_last_status = 0;
  return 0x0c01fd00;
}

void dimm_page_fd00_blank(void)
{
  memset(dimm_page_fd00,0x20,0x100);
  dimm_last_status = 0;
  return;
}

int dimm_command_post(uint command, int arg)
{
  dimm_command_def *e;
  int status;
  if (dimm_board_info.id_word == 0) {
    status = 1;
  }
  else if (command >= 0x20 || (e = dimm_command_table + command, (e->arg_size > 0) & (arg == 0))) {
    status = 2;
  }
  else if (dimm_pending_command != 0) {
    status = 5;
  }
  else {
    goto ok;
  }
  dimm_last_status = status;
  return 0;
ok:
    if (e->arg_size > 0) {
      if (command == 0x10) {
        memcpy((void *)(e->arg_shared_offset + (int)&dimm_shared_area),(void *)arg,e->arg_size);
      }
      else {
        dimm_pio_write((ushort *)arg,dimm_board_info.module_base + e->arg_dimm_offset + 0xff8000,e->arg_size);
      }
    }
    dimm_reply_word = 0;
    dimm_pending_command = 1 << command;
    dimm_request.command = command;
    dimm_request.stall_polls = 0;
    dimm_request.reply = 0;
    dimm_request.fail_code = 0;
    if (e->arg_size > 0) {
      memcpy(&dimm_request.arg,(void *)arg,e->arg_size);
    }
    else {
      dimm_request_arg.field_0x0 = 0;
      dimm_request_arg.field_0x4 = 0;
      dimm_request_arg.field_0x8 = 0;
      dimm_request_arg.field_0xc = 0;
      dimm_request_arg.field_0x10 = 0;
      dimm_request_arg.field_0x14 = 0;
      dimm_request_arg.field_0x18 = 0;
      dimm_request_arg.field_0x1c = 0;
    }
    dimm_last_status = 0;
    return 1;
}

uint dimm_command_poll(uint *reply_out)
{
  uint cur;
  uint r;
  dimm_command_def *e;
  int n;
  int i;
  uchar *p;
  uchar *q;

  cur = dimm_reply_word;
  if (reply_out != (uint *)0x0) {
    *reply_out = dimm_request.reply;
  }
  e = dimm_command_table + dimm_request.command;
  if (dimm_request.fail_code != 0) {
    dimm_last_status = dimm_request.fail_code;
    return 3;
  }
  r = dimm_request.reply >> 8 & 3;
  switch (r) {
  case 0:
  case 1:
    if (cur != dimm_request.reply) {
      dimm_request.stall_polls = 0;
    }
    else if ((dimm_link_flags & 1) == 0) {
      if (dimm_request.stall_polls++ >= e->poll_limit) {
        dimm_request.fail_code = 3;
      }
    }
    if (dimm_request.fail_code == 0) {
      dimm_request.reply = cur;
    }
    break;
  case 2:
    if (e->arg_size > 0) {
      p = (uchar *)&dimm_request_arg;
      q = (uchar *)&dimm_shared_area + e->arg_shared_offset;
      n = e->arg_size;
      for (i = 0; i < n; i++) {
        if (*q++ != *p++) break;
      }
      if (i < n) {
        r = 1;
        if ((dimm_link_flags & 1) == 0) {
          if (dimm_request.stall_polls++ >= e->poll_limit) {
            dimm_request.fail_code = 3;
          }
        }
      }
    }
    break;
  case 3:
    r = 1;
    dimm_request.fail_code = 4;
    break;
  }
  dimm_last_status = 0;
  return r;
}

void dimm_command_cancel(void)
{
  dimm_pending_command = 0;
  dimm_request.command = 0;
  dimm_request.stall_polls = 0;
  dimm_request.reply = 0;
  dimm_request.fail_code = 0;
  memset(&dimm_request.arg,0,0x20);
}

void dimm_status_mode_set(int mode_one)
{
  volatile int tries;
  uint status;
  ushort now;

  if ((dimm_board_id_word & 0x20000000) != 0) {
    tries = 0x22ca4;
    while ((*(volatile ushort *)0xa05f704c & 1) == 0) {
      tries = tries - 1;
      if (tries <= 0) break;
    }
  }
  now = *(volatile ushort *)0xa05f704c;
  status = now & 0xcfff;
  status |= mode_one != 0 ? 0x1000 : 0x2000;
  if ((dimm_board_id_word & 0x20000000) != 0) {
    status &= ~0x100;
  }
  *(volatile ushort *)0xa05f704c = status;
}

void dimm_pio_read(uint addr, ushort *dst, uint size)
{
  uint hi;
  ushort lo;
  int cnt;
  int n;
  hi = addr >> 0x10 & 0x1fff;
  lo = addr;
  *(volatile ushort *)0xa05f7000 = hi | 0x8000;
  *(volatile ushort *)0xa05f7004 = lo;
  n = size >> 1;
  size = n - 1;
  while (n != 0) {
    *dst = *(volatile ushort *)0xa05f7008;
    dst = dst + 1;
    n = size;
    size = n - 1;
  }
}

void dimm_dma_read(uint addr, uint buf, uint size, int no_wait)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  volatile int x;
  addr &= 0xffffffe0;
  if (size == 0) return;
  while (*(volatile int *)0xa05f7418 != 0) ;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  *(volatile uint *)0xa05f7004 = 0;
  *(volatile uint *)0xa05f7000 = 0;
  *(volatile uint *)0xa05f7014 = size >> 5;
  *(volatile uint *)0xa05f7010 = addr & 0xffff;
  *(volatile uint *)0xa05f700c = addr >> 16 | 0x8000;
  *(volatile uint *)0xa05f7404 = buf;
  *(volatile uint *)0xa05f7408 = size;
  *(volatile uint *)0xa05f740c = 1;
  *(volatile uint *)0xa05f7414 = 1;
  *(volatile uint *)0xa05f7418 = 1;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  if (no_wait == 0) {
    do {
      x = *(volatile int *)0xa05f7418;
    } while (x != 0);
    *(volatile uint *)0xa05f700c = 0xc000;
  }
}

void dimm_pio_write(ushort *src, uint addr, uint size)
{
  uint hi;
  ushort lo;
  int cnt;
  int n;
  hi = addr >> 0x10 & 0x1fff;
  lo = addr;
  *(volatile ushort *)0xa05f7000 = hi | 0x8000;
  *(volatile ushort *)0xa05f7004 = lo;
  n = size >> 1;
  size = n - 1;
  while (n != 0) {
    *(volatile ushort *)0xa05f7008 = *src;
    src = src + 1;
    n = size;
    size = n - 1;
  }
}

void dimm_dma_write(uint buf, uint addr, uint size, int no_wait)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  volatile int v;

  addr &= 0xffffffe0;
  if (size == 0) return;
  while (*(volatile int *)0xa05f7418 != 0) ;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = 0xf0;
  lvl &= sr;
  lvl >>= 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  *(volatile uint *)0xa05f7004 = 0;
  *(volatile uint *)0xa05f7000 = 0;
  *(volatile uint *)0xa05f7014 = size >> 5;
  *(volatile uint *)0xa05f7010 = addr & 0xffff;
  *(volatile uint *)0xa05f700c = addr >> 0x10 | 0x8000;
  *(volatile uint *)0xa05f7404 = buf;
  *(volatile uint *)0xa05f7408 = size;
  *(volatile uint *)0xa05f740c = 0;
  *(volatile uint *)0xa05f7414 = 1;
  *(volatile uint *)0xa05f7418 = 1;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  if (no_wait == 0) {
    do {
      v = *(volatile int *)0xa05f7418;
    } while (v != 0);
    *(volatile uint *)0xa05f700c = 0xc000;
  }
}
