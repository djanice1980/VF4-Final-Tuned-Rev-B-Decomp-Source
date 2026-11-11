/*
 * sdk/sy/sycache.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void caches_reset_to_mode(uint mode)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dcache_tags_clear_uncached();
  cache_control_reg = mode & 0xfffff7ff | 0x808;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return;
}

void cache_turn_off(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dcache_tags_clear_uncached();
  cache_control_reg = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

uint cache_control_get(void)
{
  return *(volatile uint *)0xff00001c;
}

void caches_switch_mode(uint mode)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dcache_tags_clear_uncached();
  cache_control_reg = mode & 0xfffff7ff | 0x808;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void icache_invalidate_bit_set(void)
{
  *(volatile uint *)0xff00001c |= 0x800;
  return;
}

void ocache_invalidate_bit_set(void)
{
  *(volatile uint *)0xff00001c |= 8;
  return;
}

void dcache_discard_range(uint addr, uint size)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint start;
  uint end;
  uint cache_ctrl;
  uint oc_ram;
  uint oc_index;
  uint k;
  
  if (size != 0) {
    if (size < 0x30000) {
      dcache_discard_lines(addr & 0x9fffffe0,size + (addr & 0x1f));
    }
    else {
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      k = sr & 0xf0;
      lvl = k >> 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
      start = addr & 0x9fffffe0;
      end = (addr + size) - 1 & 0x1fffffe0;
      cache_ctrl = cache_control_reg;
      oc_ram = cache_ctrl >> 5 & 1;
      oc_index = cache_ctrl >> 7 & 1;
      dcache_discard_scan_lo(start,end,oc_ram);
      if (oc_index != 0) {
        dcache_discard_scan_hi_oix(start,end,oc_ram);
      }
      else {
        dcache_discard_scan_hi(start,end,oc_ram);
      }
      __asm__ volatile ("stc sr,%0" : "=r" (t));
      t = (t & 0xffffff0f) | (lvl << 4);
      __asm__ volatile ("ldc %0,sr" : : "r" (t));
    }
  }
}

void dcache_discard_range_uncached(uint addr, uint size)
{
  ((void (*)(uint,uint))((uint)dcache_discard_range | 0xa0000000))(addr,size);
}

void dcache_writeback_discard_range(uint addr, uint size)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint start;
  uint end;
  uint cache_ctrl;
  uint oc_ram;
  uint oc_index;
  uint u;

  if (size == 0) return;
  if (size < 0x10000) {
    dcache_writeback_discard_lines(addr & 0x9fffffe0,size + (addr & 0x1f));
    return;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  u = sr & 0xf0;
  lvl = u >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  start = addr & 0x9fffffe0;
  end = (addr + size) - 1 & 0x1fffffe0;
  cache_ctrl = cache_control_reg;
  oc_ram = cache_ctrl >> 5 & 1;
  oc_index = cache_ctrl >> 7 & 1;
  dcache_writeback_discard_scan_lo(start,end,oc_ram);
  if (oc_index) dcache_writeback_discard_scan_hi_oix(start,end,oc_ram);
  else dcache_writeback_discard_scan_hi(start,end,oc_ram);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void dcache_writeback_discard_range_uncached(uint addr, uint size)
{
  (*(void (*)(uint, uint))((uint)dcache_writeback_discard_range | 0xa0000000))(addr, size);
}

void dcache_writeback_range(uint addr, uint size)
{
  uint sr;
  uint lvl;
  uint t;
  uint u;
  uint start;
  uint end;
  uint cache_ctrl;
  uint oc_ram;
  uint oc_index;
  if (size != 0) {
    if (size < 0x10000) {
      dcache_writeback_lines(addr & 0x9fffffe0,size + (addr & 0x1f));
    }
    else {
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      u = sr & 0xf0;
      lvl = u >> 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
      start = addr & 0x9fffffe0;
      end = (addr + size) - 1 & 0x1fffffe0;
      cache_ctrl = cache_control_reg;
      oc_ram = cache_ctrl >> 5 & 1;
      oc_index = cache_ctrl >> 7 & 1;
      dcache_writeback_scan_lo(start,end,oc_ram);
      if (oc_index != 0) {
        dcache_writeback_scan_hi_oix(start,end,oc_ram);
      }
      else {
        dcache_writeback_scan_hi(start,end,oc_ram);
      }
      __asm__ volatile ("stc sr,%0" : "=r" (t));
      t = (t & 0xffffff0f) | (lvl << 4);
      __asm__ volatile ("ldc %0,sr" : : "r" (t));
    }
  }
  return;
}

void dcache_writeback_range_uncached(uint addr, uint size)
{
  ((void (*)(uint,uint))((uint)dcache_writeback_range | 0xa0000000))(addr,size);
}

void cache_prefetch_unaligned(uint addr, int size)
{
  cache_prefetch_range(addr & 0x9fffffe0,size + (addr & 0x1f));

  return;
}

void dcache_tags_clear(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  uint i;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if ((cache_control_reg >> 5 & 1) != 0) {
    for (i = 0; i < 0x80; i++) {
      *(uint *)((i << 5) | 0xf4000000) = 0;
    }
    for (i = 0x100; i < 0x180; i++) {
      *(uint *)((i << 5) | 0xf4000000) = 0;
    }
  }
  else {
    for (i = 0; i < 0x200; i++) {
      *(uint *)((i << 5) | 0xf4000000) = 0;
    }
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  __asm__ volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");
}

void dcache_tags_clear_uncached(void)
{
  (*(void (*)(void))(((uint)dcache_tags_clear_uncached - 0xc0) | 0xa0000000))();
}

void cache_ram_blocks_take(uint *address_out, int blocks)
{
  uint block;

  for (block = cache_ram_next; (block < blocks + cache_ram_next) & (block < 8); block++) {
    cache_ram_block_tags[block] = (ushort)block * 0x400;
  }
  if (block != 8) {
    *address_out = cache_ram_next << 10 | 0x7c000000;
    cache_ram_next = cache_ram_next + blocks;
  }
}

void cache_ram_block_release(uint address)
{
  int i;

  for (i = cache_ram_next; i > 0; i--) {
    if (cache_ram_block_tags[i] == (address >> 10 & 7)) {
      cache_ram_next = cache_ram_next - 1;
    }
  }
}
