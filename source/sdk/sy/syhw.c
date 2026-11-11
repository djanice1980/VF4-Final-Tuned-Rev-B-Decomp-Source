/*
 * sdk/sy/syhw.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint *sysbus_reg_defaults; /* 0x0c2164fc */

void sys_bring_up_all(void)
{
  sysbus_regs_load_defaults();
  ic_flush_and_unblock(startup_stack_mode,startup_irq_stack);
  sq_windows_map();
  (*(void (*)(uint))((uint)caches_reset_to_mode | 0xa0000000))(0x105);
  tmu0_free_run_start();
}

void sys_bring_up_vectors_timer(void)
{
  ic_flush_and_unblock(startup_stack_mode,startup_irq_stack);
  tmu0_free_run_start();
  return;
}

void sys_bring_up_buses(void)
{
  irq_hooks_clear();
  g2_subsystem_start(0);
  ext_devices_probe();
  return;
}

void sys_wind_down(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  ext_devices_release();
  sq_windows_unmap();
  cache_shutdown_uncached();
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void cache_shutdown_uncached(void)
{
  (*(void (*)(void))(((uint)cache_shutdown_uncached + 0x20) | 0xa0000000))();
}

void cache_shutdown(void)
{
  int i;

  *(volatile uint *)0xff00001c = 0;
  for (i = 0; i < 0x200; i++) {
    *(volatile uint *)(0xf4000000 + i * 0x20) = 0;
  }
  *(volatile uint *)0xff00001c = 0x800;
  *(volatile uint *)0xff00001c = 0;
}

void sysbus_regs_load_defaults(void)
{
  uint **entry;
  uint *reg;
  *(volatile ushort *)0xffd00000 = 0;
  *(volatile ushort *)0xffd00004 = 0;
  *(volatile ushort *)0xffd00008 = 0;
  *(volatile ushort *)0xffd0000c = 0;
  *(volatile ushort *)0xffe80020 = 0xc0;
  entry = &sysbus_reg_defaults;
  while (1) {
    reg = *entry;
    if (reg == 0) break;
    *reg = (uint)entry[1];
    entry += 2;
  }

}
