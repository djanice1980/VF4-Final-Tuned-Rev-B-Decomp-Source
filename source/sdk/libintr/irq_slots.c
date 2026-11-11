/*
 * sdk/libintr/irq_slots.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern irq_slot_fn_table irq_slot_fns; /* 0x0c5cab34 */
extern short irq_ipr_defaults[24]; /* 0x0c21198e */
extern irq_slot_store_fn irq_slot_store_fns[13]; /* 0x0c325a48 */
extern irq_event_ack_fn irq_event_ack_fns[6]; /* 0x0c325a7c */
extern irq_ipr_field irq_ipr_fields[12]; /* 0x0c325a94 */
extern int irq_last_event; /* 0x0c3e0370 */
extern int irq_slot_hook_ids[13]; /* 0x0c5cab68 */

static inline void imask_raise_e0(unsigned int *lvl)
{
  unsigned int sr;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  *lvl = (sr & 0xf0) >> 4;
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

static inline void imask_restore_lvl(unsigned int *lvl)
{
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (*lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int irq_render_done_relay(void)
{
  irq_last_event = 0;
  if (irq_slot_fns.render_done_fn != 0) {
    ((void (*)(void))irq_slot_fns.render_done_fn)();
  }
  return 0;
}

int irq_vblank_in_relay(void)
{
  irq_last_event = 1;
  if (irq_slot_fns.vblank_in_fn != 0) {
    ((void (*)(void))irq_slot_fns.vblank_in_fn)();
  }
  return 0;
}

int irq_hblank_relay(void)
{
  irq_last_event = 2;
  if (irq_slot_fns.hblank_fn != 0) {
    ((void (*)(void))irq_slot_fns.hblank_fn)();
  }
  return 0;
}

int irq_rom_dma_end_relay(void)
{
  irq_last_event = 3;
  if (irq_slot_fns.rom_dma_end_fn != 0) {
    ((void (*)(void))irq_slot_fns.rom_dma_end_fn)();
  }
  irq_status_reg = 0x4000;
  return 0;
}

int irq_cart_board_relay(void)
{
  irq_last_event = 5;
  if (irq_slot_fns.cart_board_fn != 0) {
    ((void (*)(void))irq_slot_fns.cart_board_fn)();
  }
  cart_board_irq_ack = 3;
  return 0;
}

int irq_tex_overflow_relay(void)
{
  irq_last_event = 4;
  if (irq_slot_fns.tex_overflow_fn != 0) {
    ((void (*)(void))irq_slot_fns.tex_overflow_fn)();
  }
  return 0;
}

void irq_slots_install(int with_board_irqs)
{
  int i;
  short *pair;
  unsigned int s[1];
  unsigned int old, sr;
  
  __asm__ volatile ("stc sr,%0" : "=r" (s[0]));
  old = s[0];
  sr = s[0] | 0x10000000;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  i = 0;
  do {
    (&irq_slot_fns.render_done_fn)[i] = 0;
    irq_slot_hook_ids[i] = -1;
    i = i + 1;
  } while (i < 0xd);
  pair = irq_ipr_defaults;
  i = 0;
  do {
    intc_ipr_level_write((int)*pair,(int)pair[1]);
    i = i + 1;
    pair = pair + 2;
  } while (i < 0xc);
  i = 0;
  do {
    irq_slot_fn_install(i,0);
    i = i + 1;
  } while (i < 0xd);
  busy_wait_fn_install(0);
  render_done_hook_put((uint)irq_render_done_relay,0);
  irq_slot_hook_ids[1] = irq_hook_insert(0x900,(irq_hook_fn)irq_vblank_in_relay,0x58,0);
  irq_slot_hook_ids[2] = irq_hook_insert(0x920,(irq_hook_fn)irq_hblank_relay,0,0);
  if (with_board_irqs != 0) {
    irq_slot_hook_ids[3] = irq_hook_insert(0xa40,(irq_hook_fn)irq_rom_dma_end_relay,0,0);
    irq_slot_hook_ids[4] = irq_hook_insert(0xb00,(irq_hook_fn)irq_cart_board_relay,0,0);
  }
  surface_alloc_fail_hook_put((uint)irq_tex_overflow_relay,0);
  irq_hblank_line_store(0);
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return;
}

void irq_slots_install_naomi(void)
{
    irq_slots_install(1);
}

void irq_slots_install_holly_only(void)
{
  irq_slots_install(0);
  return;
}

int irq_render_done_fn_store(int slot, uint fn)
{
  irq_slot_fns.render_done_fn = fn;
  return 1;
}

int irq_vblank_in_fn_store(int slot, uint fn)
{
  irq_slot_fns.vblank_in_fn = fn;
  return 1;
}

int irq_hblank_fn_store(int slot, uint fn)
{
  irq_slot_fns.hblank_fn = fn;
  return 1;
}

int irq_rom_dma_end_fn_store(int slot, uint fn)
{
  irq_slot_fns.rom_dma_end_fn = fn;
  return 1;
}

int irq_cart_board_fn_store(int slot, uint fn)
{
  irq_slot_fns.cart_board_fn = fn;
  return 1;
}

int irq_vbr_slot_fn_store(int slot, uint fn)
{
  uint code;
  int vbr;

  code = irq_vbr_intevt_codes[slot - 5];
  __asm__ ("stc vbr,%0" : "=r" (vbr));
  *(uint *)(vbr + (code >> 3) + 0x1c0) = fn;
  return 1;
}

int irq_tex_overflow_fn_store(int slot, uint fn)
{
  irq_slot_fns.tex_overflow_fn = fn;
  return 1;
}

int irq_slot_store_refuse(void)
{
  return 0;
}

int irq_slot_fn_install(int slot,uint fn)
{
  unsigned int lvl;
  int (*p)();
  int r;
  
  imask_raise_e0(&lvl);
  if (slot < 0xd) {
    p = (int (*)())irq_slot_store_fns[slot];
  }
  else {
    p = (int (*)())irq_slot_store_refuse;
  }
  r = (*p)();
  imask_restore_lvl(&lvl);
  return r;
}

int irq_slot_fn_fetch(int slot, uint *out)
{
  if (slot <= 0xc) {
    *out = (&irq_slot_fns.render_done_fn)[slot];
    return 1;
  }
  *out = 0;
  return 0;
}

int irq_last_event_get(void)
{
  return irq_last_event;
}

void irq_event_ack_none(void)
{
  return;
}

void irq_rom_dma_end_ack(void)
{
  irq_status_reg = 0x4000;
  return;
}

void irq_cart_board_ack(void)
{
  cart_board_irq_ack = 3;
  return;
}

void irq_event_ack(int event)
{
  if (event < 6) {
    (*irq_event_ack_fns[event])();
  }
  return;
}

int intc_ipr_level_write(int source, uint level)
{
  unsigned int sr;
  unsigned int old;
  char *base;
  char *entry;
  volatile ushort *r;
  if (source > 12) return 0;
  if (level > 15) return 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  old = sr;
  sr |= 0x10000000;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  base = (char *)irq_ipr_fields;
  entry = base + source * 4;
  r = (volatile ushort *)(0xffd00004 + entry[0]);
  *r = (*r & ~((ushort *)((char *)irq_ipr_fields + 2))[source * 2]) | (level << entry[1]);
  *r;
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return 1;
}

int intc_ipr_level_read(int source, uint *level)
{
  irq_ipr_field *field;
  volatile ushort *reg;

  if (source > 12) return 0;
  field = &irq_ipr_fields[source];
  reg = (volatile ushort *)(0xffd00004 + field->reg_offset);
  *level = (*reg >> field->shift) & 15;
  return 1;
}

int busy_wait_fn_install(uint fn)
{
  unsigned int sr;
  unsigned int old;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  old = sr;
  sr |= 0x10000000;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  busy_wait_hook_put(fn,0);
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return 0;
}

void spin_forever(void)
{
  for (;;) ;
}

void irq_hblank_line_store(int line)
{
  spg_hblank_line_write(line);
  return;
}

void startup_stack_words_set(uint stack)
{
  startup_stack_mode = 0x80000000;
  startup_irq_stack = stack;
  return;
}
