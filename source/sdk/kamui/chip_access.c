/*
 * sdk/kamui/chip_access.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void hook_slot_run(int slot)
{
  gfx_state.hooks[slot].fn(gfx_state.hooks[slot].arg);
}

void device_bringup_noop(void)
{
  return;
}

int pvr_reg_write_first(int offset, uint value)
{
  *(volatile uint *)(offset + -0x5fa08000) = value;
  return 1;
}

int pvr_reg_write_second(int offset, uint value)
{
  *(volatile uint *)(offset + -0x5da08000) = value;
  return 1;
}

int pvr_reg_write_both(int offset, uint value)
{
  *(volatile uint *)(offset + -0x57a08000) = value;
  return 1;
}

void dcache_writeback_span(uint addr, int len)
{
  uint p;
  for (p = addr; p <= addr + len; p += 0x20) ocbwb_line(p);
}

uint pvr_reg_read(int offset)
{
  return *(volatile uint *)(offset + -0x5fa08000);
}

int vram32_write(int offset, uint *src, uint bytes)
{
  copy_words((uint *)(offset + -0x5b000000),src,bytes);
  return 1;
}

int vram32_write_chip_a(int offset, uint *src, uint bytes)
{
  elan_ctrl_bit0_off_or_back(0);
  copy_words((uint *)(offset + -0x5b000000),src,bytes);
  elan_ctrl_bit0_off_or_back(1);
  return 1;
}

int vram32_write_chip_b(int offset, uint *src, uint bytes)
{
  elan_ctrl_bit0_off_or_back(0);
  copy_words((uint *)(offset + -0x59000000),src,bytes);
  elan_ctrl_bit0_off_or_back(1);
}

int vram32_read(uint *dst, int offset, uint bytes)
{
  copy_words(dst,(uint *)(offset + -0x5b000000),bytes);
  return 1;
}

int vram32_read_both_ored(uint *dst, int offset, uint bytes)
{
  words_or_merge(dst,(uint *)(offset + -0x5b000000),(uint *)(offset + -0x59000000),bytes);
  return 1;
}

int vram64_write(uint offset, uint src, uint bytes)
{
  uint n;
  unsigned int sr;
  unsigned int lvl;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  if ((offset & 0x1f) == 0 && ((src & 0x1f) == 0 & lvl == 0)) {
    dcache_writeback_span(src,bytes);
    n = bytes & 0xffffffe0;
    if (n != 0) {
      gfx_state.upload_ticket = block_write_ticketed(src,offset + 0x11000000,n);
    }
    else {
      gfx_state.upload_ticket = 0xffffffff;
    }
    if ((bytes & 0x1f) != 0) {
      offset += n;
      src += n;
      copy_words((uint *)(offset + 0xa4000000),(uint *)src,bytes & 0x1f);
    }
    if (gfx_state.upload_wait == 1) {
      ticket_wait(gfx_state.upload_ticket);
    }
  }
  else {
    copy_words((uint *)(offset + 0xa4000000),(uint *)src,bytes);
  }
  return 1;
}

int vram64_read(uint *dst, int offset, uint bytes)
{
  copy_words(dst,(uint *)(offset + -0x5c000000),bytes);
  dcache_writeback_span((uint)dst,bytes);
  return 1;
}

void pvr_reg_list_write_both(uint count, int pairs)
{
  uint *pair;
  uint i;
  
  for (i = 0; i < count; i = i + 1) {
      pair = (uint *)(i * 8 + pairs);
      pvr_reg_write_both(*pair,pair[1]);
  }
  return;
}

int return_one_0c14a5a0(void)
{
  return 1;
}
