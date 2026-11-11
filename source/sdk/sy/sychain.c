/*
 * sdk/sy/sychain.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern irq_hook irq_hook_pool[256]; /* 0x0c41cdf4 */

static inline void imask_restore(unsigned int *lvl)
{
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (*lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void irq_hooks_clear(void)
{
  unsigned int sr;
  unsigned char lvl;
  unsigned int t;
  uint i;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  for (i = 0; i < 0x100; i++) {
    irq_hook_pool[i].priority = 0;
  }
  for (i = 0; i < 0xc0; i++) {
    irq_hook_heads[i] = 0;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int irq_hook_insert(int event_code,irq_hook_fn fn,uint priority,void *arg)
{
  unsigned int sr;
  unsigned int m;
  unsigned int lvl;
  short c;
  uint i;
  int vbr;
  irq_hook *e;
  irq_hook *prev;
  int ret;

  c = event_code;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  for (prev = e = irq_hook_heads[c >> 4]; e != 0 && e->priority < (priority | 0x80000000); e = e->next) {
    prev = e;
  }
  for (i = 1; i < 0x100; i++) {
    if ((irq_hook_pool[i].priority & 0x80000000) == 0) break;
  }
  if (i <= 0xff) {
    if ((e == 0) & (prev == 0)) {
      if (c <= 0x8ff) {
        __asm__ ("stc vbr,%0" : "=r" (vbr));
        vbr += (c - 0x200) >> 3;
        *(void **)(vbr + 0x200) = (void *)irq_hooks_run;
      }
      else if (c == 0x900) {
        vblank_hook_put((uint)vblank_in_hooks_run, 0);
      }
      else if (c == 0x920) {
        hblank_hook_put((int)hblank_hooks_run, 0);
      }
      else {
        __asm__ ("stc vbr,%0" : "=r" (vbr));
        *(void **)(vbr + 0x22c) = (void *)holly_irq_dispatch;
      }
      irq_hook_heads[c >> 4] = irq_hook_pool + i;
      irq_hook_pool[i].next = 0;
      irq_hook_pool[i].fn = fn;
      irq_hook_pool[i].arg = arg;
      irq_hook_pool[i].priority = priority | 0x80000000;
      if (c >> 8 == 10) {
        *(uint *)0xa05f6920 |= 1 << (((c & 0xff) >> 5) + 12);
      }
      else if (c >> 8 == 11) {
        *(uint *)0xa05f6924 |= 1 << ((c & 0xff) >> 5);
      }
    }
    else {
      if (e == irq_hook_heads[c >> 4]) {
        irq_hook_pool[i].next = e;
        irq_hook_heads[c >> 4] = irq_hook_pool + i;
        irq_hook_pool[i].fn = fn;
        irq_hook_pool[i].arg = arg;
        irq_hook_pool[i].priority = priority | 0x80000000;
      }
      else {
        prev->next = irq_hook_pool + i;
        irq_hook_pool[i].next = e;
        irq_hook_pool[i].fn = fn;
        irq_hook_pool[i].arg = arg;
        irq_hook_pool[i].priority = priority | 0x80000000;
      }
    }
    ret = i;
  }
  else {
    ret = 0;
  }
  imask_restore(&lvl);
  return ret;
}

void irq_hooks_run(int event_code)
{
  irq_hook *hook;
  
  for (hook = irq_hook_heads[(int)(short)event_code >> 4]; hook != 0
      ; hook = hook->next) {
    (*hook->fn)(hook->arg);
  }
  return;
}

void vblank_in_hooks_run(void)
{
  unsigned int sr;
  irq_hook *hook;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0x60;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  for (hook = vblank_in_hook_list; hook != 0; hook = hook->next) {
    (*hook->fn)(hook->arg);
  }
}

void hblank_hooks_run(void)
{
  unsigned int s;
  irq_hook *hook;

  __asm__ volatile ("stc sr,%0" : "=r" (s));
  s = (s & 0xffffff0f) | 0x60;
  __asm__ volatile ("ldc %0,sr" : : "r" (s));
  for (hook = hblank_hook_list; hook != 0; hook = hook->next) {

    (*hook->fn)(hook->arg);
  }
}

int holly_irq_dispatch(void)
{
  unsigned int sr;
  uint mask;
  int i;
  short t;

  if ((short)*(volatile int *)0xff000028 >> 4 == 0x36) {
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    sr = (sr & 0xffffff0f) | 0x40;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
    mask = (*(volatile uint *)0xa05f6900 & *(volatile uint *)0xa05f6920) >> 12;
    i = 0;
    if (mask != 0) {
      do {
        if (mask & (1 << i)) {
          t = i; t <<= 5; irq_hooks_run((short)(t + 0xa00));
        }
        i++;
      } while ((1 << i) != 0x80);
    }
    else {
      mask = *(volatile uint *)0xa05f6904 & *(volatile uint *)0xa05f6924;
      do {
        if (mask & (1 << i)) {
          t = i; t <<= 5; irq_hooks_run((short)(t + 0xb00));
        }
        i++;
      } while ((1 << i) != 0x10);
    }
  }
  return 0;
}

void irq_hook_remove(int hook)
{
  unsigned int sr;
  unsigned char lvl;
  unsigned int t;
  uint i;
  int vbr;
  irq_hook **h;
  irq_hook *e;
  short s;
  irq_hook **heads;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  for (i = 1; i < 0x100; i++) {
    if (irq_hook_pool[i].next == irq_hook_pool + hook && (irq_hook_pool[i].priority & 0x80000000) != 0) break;
  }
  if (i != 0x100) {
    {
      irq_hook *tab = irq_hook_pool;
      tab[i].next = tab[hook].next;
      tab[hook].priority = 0;
    }
  }
  else {
    i = 1;
    heads = irq_hook_heads;
    for (; i < 0xc0; i++) {
      h = (irq_hook **)(i * 4 + (int)heads);
      e = *h;
      if (e == irq_hook_pool + hook) {
        *h = e->next;
        e->priority = 0;

        if (*h == 0) {
          if (i == 0x90) {
            vblank_hook_put(0, 0);
          }
          else if (i == 0x92) {
            hblank_hook_put(0, 0);
          }
          else {
            s = i << 4;
            if (s >> 8 == 10) {
              *(uint *)0xa05f6920 &= ~(1 << (((s & 0xf0) >> 5) + 12));
            }
            else if (s >> 8 == 11) {
              *(volatile uint *)0xa05f6924 &= ~(1 << ((s & 0xf0) >> 5));
            }
            __asm__ ("stc vbr,%0" : "=r" (vbr));
            *(void **)(vbr + ((i * 16 - 0x200) >> 3) + 0x200) = (void *)int_break_handler;
          }
        }
        break;
      }
    }
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}
