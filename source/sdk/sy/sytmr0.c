/*
 * sdk/sy/sytmr0.c (file name ours)
 * The TMU0 time base: written from the game's bytes (assembly listings in
 * source/asm/ until 2026-11-12); compiles to them with the tree's flags
 * plus -fno-omit-frame-pointer, which the rest of sytmr.c does not take,
 * so these four were a file of their own (python3 -I tools/match/match.py
 * checks it).
 * match-flags: -fno-omit-frame-pointer
 */
#include "vf4ft.h"

void tmu0_free_run_start(void)
{
  *(volatile char *)0xffd80000 = 0;
  *(volatile char *)0xffd80004 &= ~1;
  *(volatile short *)0xffd80010 = 2;
  *(volatile uint *)0xffd80008 = 0xffffffff;
  *(volatile uint *)0xffd8000c = 0xffffffff;
  *(volatile char *)0xffd80004 |= 1;
}

uint tmu0_elapsed_ticks(void)
{
  return ~*(volatile uint *)0xffd8000c;
}

uint ticks_between(uint first, uint second)
{
  return second - first;
}

uint tmu0_ticks_to_usec(uint ticks)
{
  return (ticks / 100) * 128 + ((ticks % 100) * 128) / 100;
}
