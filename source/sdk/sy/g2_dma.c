/*
 * sdk/sy/g2_dma.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern g2_dma_state *g2_state; /* 0x0c32a710 */

static inline void imask_raise(unsigned int *lvl)
{
  unsigned int sr;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  *lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

static inline void imask_restore(unsigned int *lvl)
{
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (*lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int g2dma_slot_mark_unused(g2_dma_channel *ch)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  int r;

  r = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (ch->in_use != 1) {
    r = 0x90102;
  }
  else {
    ch->in_use = 0;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return r;
}

int g2dma_done_isr(g2_dma_channel *ch)
{
  volatile uint *hw = (volatile uint *)0xa05f6900;
  unsigned int sr;
  unsigned int m;
  unsigned int lvl;
  unsigned int t;
  volatile int dummy;
  g2_dma_state *d;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  d = g2_state;
  ((g2_dma_regs *)((ch->number << 5) + d->regs_base))->enable = 0;
  ch->running = 0;
  ch->done = 1;
  { g2_dma_state *b = g2_state; b->done_count[ch->number]++; }
  *hw = 1 << (ch->number + 0xf);
  dummy = *hw;
  dummy = *hw;
  if (ch->done_fn != 0) {
    (*ch->done_fn)(ch->done_arg);
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int g2dma_start_bit_read(g2_dma_channel *ch, byte *started)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  int i;
  g2_dma_state *d;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  i = ch->number;
  d = g2_state;
  *started = ((struct { unsigned int f : 1; } *)&((g2_dma_regs *)((i << 5) + d->regs_base))->start)->f;

  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

int g2dma_slot_init_in_use(g2_dma_channel **out, int number)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  int r;
  g2_dma_channel *e;
  int n;

  r = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  *out = 0;
  e = &g2_state->channel[number];
  if (e->in_use == 1) {
    r = 0x90101;
  }
  else {
    { g2_dma_state *b = g2_state; b->channel[e->number].number = e->number; }
    { g2_dma_state *b = g2_state; n = e->number; b->channel[n].in_use = 0; }
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
    e->in_use = 1;
    *out = e;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return r;
}

int g2dma_slot_store_transfer(g2_dma_channel *ch,uint g2_addr,uint mem_addr,uint length,uint direction,uchar trigger_flag0,uchar trigger_flag2,g2_dma_done_fn done_fn,int done_arg)
{
  unsigned int lvl;
  imask_raise(&lvl);
  ch->g2_addr = g2_addr & 0x1fffffe0;
  ch->mem_addr = mem_addr & 0x1fffffe0;
  ch->length = length & 0x1ffffe0 | 0x80000000;
  ch->direction = direction & 1;
  if (trigger_flag0 == 1) {
    ch->trigger = ch->trigger | 1;
  }
  else {
    ch->trigger = ch->trigger & 0xfffffffe;
  }
  if (trigger_flag2 == 1) {
    ch->trigger = ch->trigger | 4;
  }
  else {
    ch->trigger = ch->trigger & 0xfffffffb;
  }
  ch->trigger = ch->trigger & 7;
  ch->done_fn = done_fn;
  ch->done_arg = done_arg;
  ch->done = 0;
  imask_restore(&lvl);
  return 0;
}

int g2dma_start_bit_clear(g2_dma_channel *ch)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  g2_dma_state *d;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  d = g2_state;
  ((g2_dma_regs *)((ch->number << 5) + d->regs_base))->start = 0;
  ch->running = 0;
  ch->done = 1;
  g2_state->stop_count[ch->number]++;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

int g2dma_regs_load_and_start(g2_dma_channel *channel, int trigger_bit1)
{
  int flag = trigger_bit1;
  int ret = 0;
  uint sr;
  uint lvl;
  uint t;
  g2_dma_channel *ch;
  g2_dma_regs *regs;
  uint v;
  char started[1];

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  { uint m = sr & 0xf0; lvl = m >> 4; }
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  ch = channel;
  regs = (g2_dma_regs *)(g2_state->regs_base + (ch->number << 5));
  g2dma_start_bit_read(ch,started);
  if (started[0] == '\x01') {
    ret = 0x90180;
  }
  else {
    dcache_writeback_range_uncached(ch->mem_addr & 0x1fffffffU | 0xa0000000,ch->length & 0x7fffffe0);
    regs->enable = 0;
    regs->g2_addr = ch->g2_addr;
    regs->mem_addr = ch->mem_addr;
    regs->length = ch->length;
    regs->direction = ch->direction;
    v = ch->trigger;
    if (flag != 0) {
      v |= 2;
    }
    regs->trigger = v;
    regs->enable = 1;
    ch->running = 1;
    g2_state->start_count[ch->number]++;
    regs->start = 1;
  }

  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return ret;
}

int g2_cpu_copy_from_bus(void *src, void *dst, uint unit, int count, int src_step, int dst_step)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  int sx;
  int dx;
  int i;
  char *s;
  char *d;

  if (g2_state->copy_masks_irq == 1) {
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    lvl[0] = (sr & 0xf0) >> 4;
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  }
  switch ((uint)src_step) {
  case 0:
    sx = 0;
    break;
  case 1:
    sx = 1;
    break;
  case 2:
    sx = -1;
    break;
  }
  switch ((uint)dst_step) {
  case 0:
    dx = 0;
    break;
  case 1:
    dx = 1;
    break;
  case 2:
    dx = -1;
    break;
  }
  g2dma_suspend_set_all();
  g2_fifo_spin_until_idle();
  if (((uint)src & 0x1fffffff) + 0xff900000 <= 0x900000) {
    aica_fifo_spin_until_idle();
  }
  switch (unit) {
  case 1:
    d = (char *)dst;
    s = (char *)src;
    for (i = 0; i < count; i++) {
      *d = *s;
      s += sx;
      d += dx;
    }
    break;
  case 2:
    d = (char *)dst;
    s = (char *)src;
    for (i = 0; i < count; i++) {
      *(short *)d = *(short *)s;
      s += sx * 2;
      d += dx * 2;
    }
    break;
  case 4:
    d = (char *)dst;
    s = (char *)src;
    for (i = 0; i < count; i++) {
      *(int *)d = *(int *)s;
      s += sx * 4;
      d += dx * 4;
    }
    break;
  }
  g2dma_suspend_clear_all();
  if (g2_state->copy_masks_irq == 1) {
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | ((lvl[0] & 0xf) << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
  return 0;
}

int g2_cpu_copy_to_bus(void *src, void *dst, uint unit, int count, int src_step, int dst_step)
{
  int sx;
  int dx;
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  int i;

  if (g2_state->copy_masks_irq == 1) {
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    lvl[0] = (sr & 0xf0) >> 4;
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  }
  switch ((uint)src_step) {
  case 0:
    sx = 0;
    break;
  case 1:
    sx = 1;
    break;
  case 2:
    sx = -1;
    break;
  }
  switch ((uint)dst_step) {
  case 0:
    dx = 0;
    break;
  case 1:
    dx = 1;
    break;
  case 2:
    dx = -1;
    break;
  }
  g2dma_suspend_set_all();
  g2_fifo_spin_until_idle();
  switch (unit) {
  case 1:
    {
    char *d = (char *)dst;
    char *s = (char *)src;
    uint a;
    for (i = 0; i < count; i++) {
      if ((i & 7) == 0) {
        a = (uint)d & 0x1fffffff;
        g2_fifo_spin_until_idle();
        a += 0xff900000;
        if (a <= 0x900000) {
          aica_fifo_spin_until_idle();
        }
      }
      *d = *s;
      s += sx;
      d += dx;
    }
    }
    break;
  case 2:
    {
    char *d = (char *)dst;
    char *s = (char *)src;
    uint a;
    for (i = 0; i < count; i++) {
      if ((i & 7) == 0) {
        a = (uint)d & 0x1fffffff;
        g2_fifo_spin_until_idle();
        a += 0xff900000;
        if (a <= 0x900000) {
          aica_fifo_spin_until_idle();
        }
      }
      *(short *)d = *(short *)s;
      s += sx * 2;
      d += dx * 2;
    }
    }
    break;
  case 4:
    {
    char *d = (char *)dst;
    char *s = (char *)src;
    uint a;
    for (i = 0; i < count; i++) {
      if ((i & 7) == 0) {
        a = (uint)d & 0x1fffffff;
        g2_fifo_spin_until_idle();
        a += 0xff900000;
        if (a <= 0x900000) {
          aica_fifo_spin_until_idle();
        }
      }
      *(int *)d = *(int *)s;
      s += sx * 4;
      d += dx * 4;
    }
    }
    break;
  }
  g2dma_suspend_clear_all();
  if (g2_state->copy_masks_irq == 1) {
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | ((lvl[0] & 0xf) << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
  return 0;
}
