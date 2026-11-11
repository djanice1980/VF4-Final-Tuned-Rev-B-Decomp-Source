/*
 * sdk/sy/sytmr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern timer_done_handler tmu1_done_fn; /* 0x0c5d5c10 */
extern int tmu1_done_arg; /* 0x0c5d5c14 */
extern timer_done_handler tmu2_done_fn; /* 0x0c5d5c18 */
extern int tmu2_done_arg; /* 0x0c5d5c1c */

int usec_to_tmu0_ticks(uint usec)
{
  return (usec >> 7) * 100 + (((usec & 0x7f) * 100) >> 7);
}

void tmu1_oneshot(uint count, timer_done_handler fn, int arg)
{
  tmu1_oneshot_clocked(count,fn,arg,2);
  return;
}

int tmu1_oneshot_clocked(uint count, timer_done_handler fn, int arg, ushort control)
{
  int vbr;
  if (*(volatile char *)0xffd80004 & 2) return -1;
  *(volatile char *)0xffd80000 = 0;
  *(volatile char *)0xffd80004 &= ~2;
  *(volatile short *)0xffd8001c = control;
  if (fn != 0) {
    __asm__ ("stc vbr,%0" : "=r" (vbr));
    *(void **)(vbr + 0x244) = (void *)tmu1_underflow_isr;
    tmu1_done_fn = fn;
    tmu1_done_arg = arg;
    *(volatile short *)0xffd8001c |= 0x20;
    *(volatile short *)0xffd00004 = *(volatile short *)0xffd00004 & 0xf0ff | 0xf00;
    *(volatile short *)0xffd8001c &= ~0x100;
  }
  *(volatile int *)0xffd80014 = count;
  *(volatile int *)0xffd80018 = count;
  *(volatile char *)0xffd80004 |= 2;
  if (fn == 0) {
    while ((*(volatile short *)0xffd8001c & 0x100) == 0) ;
    *(volatile char *)0xffd80004 &= ~2;
    *(volatile short *)0xffd8001c &= ~0x100;
  }
  return 0;
}

void tmu1_stop(void)
{
  *(volatile char *)0xffd80004 &= ~2;
  *(volatile short *)0xffd8001c &= ~0x20;
  return;
}

int tmu1_underflow_isr(void)
{
  *(volatile char *)0xffd80004 &= ~2;
  *(volatile short *)0xffd8001c &= ~0x20;
  *(volatile short *)0xffd8001c &= ~0x100;
  tmu1_done_fn(tmu1_done_arg);
  return 0;
}

int tmu2_underflow_isr(void)
{
  *(volatile char *)0xffd80004 &= ~4;
  *(volatile short *)0xffd80028 &= ~0x20;
  *(volatile short *)0xffd80028 &= ~0x100;
  (*tmu2_done_fn)(tmu2_done_arg);
  return 0;
}

void tmu2_start(void)
{
  *(volatile char *)0xffd80000 = 0;
  *(volatile char *)0xffd80004 |= 4;
  return;
}

void tmu2_stop(void)
{
  *(volatile char *)0xffd80004 &= ~4;
  return;
}

void tmu2_handler_set(timer_done_handler fn, int arg)
{
  int vbr;
  uchar started;

  started = *(volatile uchar *)0xffd80004;
  *(volatile char *)0xffd80004 &= ~4;
  if (fn != 0) {
    __asm__ ("stc vbr,%0" : "=r" (vbr));
    *(void **)(vbr + 0x248) = (void *)tmu2_underflow_isr;
    tmu2_done_fn = fn;
    tmu2_done_arg = arg;
    *(volatile short *)0xffd80028 |= 0x20;
    *(volatile short *)0xffd00004 |= 0xf0;
    *(volatile short *)0xffd80028 &= ~0x100;
  }
  *(volatile uchar *)0xffd80004 = started;
  if (fn == 0) {
    *(volatile char *)0xffd80000 = 0;
    *(volatile char *)0xffd80004 |= 4;
    while ((*(volatile short *)0xffd80028 & 0x100) == 0) ;
    *(volatile char *)0xffd80004 &= ~4;
    *(volatile short *)0xffd80028 &= ~0x100;
  }
}

void tmu2_irq_off(void)
{
  uchar started;

  started = *(volatile uchar *)0xffd80004;
  *(volatile char *)0xffd80004 &= ~4;
  *(volatile short *)0xffd80028 &= ~0x20;
  *(volatile uchar *)0xffd80004 = started;
  return;
}

void tmu2_rate_set(ushort rate)
{
  uchar started;

  if (rate > 4) rate = 2;
  started = *(volatile uchar *)0xffd80004;
  *(volatile char *)0xffd80004 &= ~4;
  *(volatile ushort *)0xffd80028 = (*(volatile ushort *)0xffd80028 & 0xfff8) | rate;
  *(volatile uchar *)0xffd80004 = started;
  return;
}

void tmu2_count_set(int ticks)
{
  uchar started;

  started = *(volatile uchar *)0xffd80004;
  *(volatile char *)0xffd80004 &= ~4;
  *(volatile uint *)0xffd80020 = 0xffffffff;
  *(volatile uint *)0xffd80024 = -ticks;
  *(volatile uchar *)0xffd80004 = started;
  return;
}

uint tmu2_count_inverted(void)
{
  return ~*(volatile uint *)0xffd80024;
}

int ticks_to_usec_at_rate(uint ticks, ushort rate)
{
  uint scale;
  switch (rate) {
  case 0:
    scale = 8;
    break;
  case 1:
    scale = 0x20;
    break;
  case 3:
    scale = 0x200;
    break;
  case 4:
    scale = 0x800;
    break;
  default:
    scale = 0x80;
    break;
  }
  return (ticks / 100) * scale + ((ticks % 100) * scale) / 100;
}

int usec_to_ticks_at_rate(int usec, uint rate)
{
  uint d;
  switch ((ushort)rate) {
  case 0:
    d = 8;
    break;
  case 1:
    d = 0x20;
    break;
  case 3:
    d = 0x200;
    break;
  case 4:
    d = 0x800;
    break;
  default:
    d = 0x80;
    break;
  }
  return ((uint)usec / d) * 100 + (((uint)usec % d) * 100) / d;
}

void tmu2_ticks_between(int first, int second)
{
  ticks_between(first,second);

  return;
}
