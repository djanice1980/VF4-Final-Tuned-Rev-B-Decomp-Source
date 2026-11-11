/*
 * sdk/sy/g2_lowlevel.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern g2_dma_state *g2_state; /* 0x0c32a710 */
extern uint g2_dma_guard_word; /* 0x0c32a754 */

static inline void set_imask_level(int lvl)
{
  unsigned int sr;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

int aica_fifo_idle_flag(byte *idle)
{
  *idle = ((byte)holly_fifo_status_reg ^ 1) & 1;
  return 0;
}

int g2dma_slot_reset(g2_dma_channel *ch)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  int n;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  { g2_dma_state *b = g2_state; b->channel[ch->number].number = ch->number; }
  { g2_dma_state *b = g2_state; n = ch->number; b->channel[n].in_use = 0; }
  g2_state->channel[n].running = 0;
  g2_state->channel[n].done = 0;
  g2_state->channel[n].g2_addr = 0;
  g2_state->channel[n].mem_addr = 0;
  g2_state->channel[n].length = 0;
  g2_state->channel[n].direction = 0;
  g2_state->channel[n].trigger = 0;
  g2_state->channel[n].field_0x18 = 3;
  g2_state->channel[n].done_fn = 0;
  g2_state->channel[n].done_arg = 0;
  { g2_dma_state *d = g2_state; ((g2_dma_regs *)((n << 5) + d->regs_base))->enable = 0; }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

int g2dma_suspend_clear(g2_dma_channel *ch)
{
  g2_dma_state *d = g2_state;
  ((g2_dma_regs *)((ch->number << 5) + d->regs_base))->suspend = 0;
  return 0;
}

int g2dma_addr_guard_set(uint start, uint end)
{
  unsigned int sr;
  int lvl;
  uint guard;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  start = (start & 0x7f00000) >> 20;
  end = (end & 0x7f00000) >> 20;
  guard = start << 8 | end | 0x46590000;
  g2_dma_guard_reg = guard;
  g2_dma_guard_word = guard;
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
  return 0;
}

int g2dma_suspend_set(g2_dma_channel *ch)
{
  unsigned int sr;
  int lvl;
  g2_dma_state *d;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  d = g2_state;
  ((g2_dma_regs *)((ch->number << 5) + d->regs_base))->suspend = 1;
  set_imask_level(lvl);
  return 0;
}

int g2_fifo_idle_flag(byte *idle)
{
  *idle = ((byte)(holly_fifo_status_reg >> 4) ^ 1) & 1;

  return 0;
}
