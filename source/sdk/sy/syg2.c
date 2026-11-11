/*
 * sdk/sy/syg2.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern g2_dma_state *g2_state; /* 0x0c32a710 */
extern char *g2_version_text; /* 0x0c32a750 */
extern char g2dma_all_suspended; /* 0x0c41ddf4 */
extern uint g2_state_space; /* 0x0c41de00 */

int aica_fifo_spin_until_idle(void)
{
  char idle [1];
  
  do {
    aica_fifo_idle_flag(idle);
    if (idle[0] == '\x01') {
      g2_state->aica_fifo_waits = g2_state->aica_fifo_waits + 1;
    }
  } while (idle[0] != '\x01');
  return 0;
}

int g2dma_suspend_clear_all(void)
{
  int i;
  
  for (i = 0; i < 4; i++) {
    g2dma_suspend_clear(&g2_state->channel[i]);
  }
  g2dma_all_suspended = 0;
  return 0;
}

int g2dma_suspend_set_all(void)
{
  int i;
  
  for (i = 0; i < 4; i++) {
    g2dma_suspend_set(&g2_state->channel[i]);
  }
  g2dma_all_suspended = 1;
  return 0;
}

int g2_fifo_spin_until_idle(void)
{
  char idle [1];
  
  do {
    g2_fifo_idle_flag(idle);
    if (idle[0] == '\x01') {
      g2_state->g2_fifo_waits = g2_state->g2_fifo_waits + 1;
    }
  } while (idle[0] != '\x01');
  return 0;
}

int g2_subsystem_start(int unused)
{
  unsigned int sr;
  int lvl[1];
  int i;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (0xf0 & sr) >> 4;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  strlen(*(char * volatile *)&g2_version_text);
  g2_state = (g2_dma_state *)(((int)&g2_state_space + 0x17) & ~0x1f);
  memset(g2_state, 0xff, 0xf4);
  g2_state->regs_base = 0xa05f7800;
  g2_state->copy_masks_irq = 1;
  g2_state->field_0xb8 = 0;
  for (i = 0; i < 4; i++) {
    g2_state->channel[i].number = i;
    g2_state->channel[i].in_use = 0;
    g2_state->channel[i].running = 0;
    g2_state->channel[i].done = 0;
    g2_state->channel[i].g2_addr = 0;
    g2_state->channel[i].mem_addr = 0;
    g2_state->channel[i].length = 0;
    g2_state->channel[i].direction = 0;
    g2_state->channel[i].trigger = 0;
    g2_state->channel[i].field_0x18 = 3;
    g2_state->channel[i].done_fn = 0;
    g2_state->channel[i].done_arg = 0;
    g2dma_slot_reset(&g2_state->channel[i]);
    g2_state->start_count[i] = 0;
    g2_state->stop_count[i] = 0;
    g2_state->done_count[i] = 0;
  }
  g2_state->g2_fifo_waits = 0;
  g2_state->aica_fifo_waits = 0;
  g2dma_addr_guard_set((uint)&main_ram_first_byte, (uint)&main_ram_last_byte);
  g2_state->copy_masks_irq = 1;
  g2_state->channel[0].hook_id = irq_hook_insert(0xa60, (irq_hook_fn)g2dma_done_isr, 0x10, &g2_state->channel[0]);
  g2_state->channel[1].hook_id = irq_hook_insert(0xa80, (irq_hook_fn)g2dma_done_isr, 0x10, &g2_state->channel[1]);
  g2_state->channel[2].hook_id = irq_hook_insert(0xaa0, (irq_hook_fn)g2dma_done_isr, 0x10, &g2_state->channel[2]);
  g2_state->channel[3].hook_id = irq_hook_insert(0xac0, (irq_hook_fn)g2dma_done_isr, 0x10, &g2_state->channel[3]);

  g2dma_all_suspended = 0;
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl[0] << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
  return 0;
}
