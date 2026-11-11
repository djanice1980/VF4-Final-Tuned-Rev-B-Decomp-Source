/*
 * sdk/sy/syint.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint irq_stack_in_use; /* 0x0c32a744 */
extern uint irq_stack_before; /* 0x0c32a74c */

void ic_flush_and_unblock(int mode, uint stack)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  cache_control_reg = cache_control_reg | 0x800;
  if (mode == -0x80000000) {
    irq_stack_switch(stack);
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  sr_bl_clear();
}

void irq_stack_switch(uint stack)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (stack != 0 || irq_stack_in_use != 0) {
    if (stack == 0) stack = irq_stack_before;
    if (irq_stack_in_use == 0) {
      register int r0 __asm__ ("r0");
      register int r4 __asm__ ("r4") = 1;
      register int r5 __asm__ ("r5") = stack;
      __asm__ volatile ("trapa #0x23" : "=r" (r0) : "r" (r4), "r" (r5));
      irq_stack_before = r0;
    }
    irq_stack_in_use = stack;

  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}
