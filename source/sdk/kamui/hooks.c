/*
 * sdk/kamui/hooks.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 lib_state_ptr; /* 0x0c32ab88 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void hook_slots_defaults(void)
{
  gfx_state.field_0x2a4 = 0;
  gfx_state.hooks[0].fn = (void (*)(uint))render_done_hook_noop;
  gfx_state.hooks[0].arg = 0;
  gfx_state.hooks[1].fn = (void (*)(uint))vblank_hook_noop;
  gfx_state.hooks[1].arg = 0;
  gfx_state.hooks[2].fn = (void (*)(uint))hblank_hook_noop;
  gfx_state.hooks[2].arg = 0;
  gfx_state.hooks[3].fn = (void (*)(uint))surface_alloc_fail_hook_noop;
  gfx_state.hooks[3].arg = 0;
  gfx_state.hooks[4].fn = (void (*)(uint))strip_hazard_hook_noop;
  gfx_state.hooks[4].arg = 0;
  gfx_state.hooks[5].fn = (void (*)(uint))list_input_end_hook_noop;
  gfx_state.hooks[5].arg = 0;
  gfx_state.hooks[6].fn = (void (*)(uint))yuv_done_hook_noop;
  gfx_state.hooks[6].arg = 0;
  gfx_state.hooks[7].fn = (void (*)(uint))busy_wait_hook_noop;
  gfx_state.hooks[7].arg = 0;
  gfx_state.hooks[8].fn = border_color_by_code_hang;
  gfx_state.hooks[8].arg = 0;
}

void hook_slot_default(gfx_state_rec *state, int slot)
{
  switch(slot) {
  case 0:
    state->hooks[0].fn = (void (*)(uint))render_done_hook_noop;
    break;
  case 1:
    state->hooks[1].fn = (void (*)(uint))vblank_hook_noop;
    break;
  case 2:
    state->hooks[2].fn = (void (*)(uint))hblank_hook_noop;
    break;
  case 3:
    state->hooks[3].fn = (void (*)(uint))surface_alloc_fail_hook_noop;
    break;
  case 4:
    state->hooks[4].fn = (void (*)(uint))strip_hazard_hook_noop;
    break;
  case 5:
    state->hooks[5].fn = (void (*)(uint))list_input_end_hook_noop;
    break;
  case 6:
    state->hooks[6].fn = (void (*)(uint))yuv_done_hook_noop;
    break;
  case 7:
    state->hooks[7].fn = (void (*)(uint))busy_wait_hook_noop;
    break;
  case 8:
    state->hooks[8].fn = border_color_by_code_hang;
  }
  return;
}

void hook_slot_store(int state, int slot, int fn, int arg)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (fn != 0) {
    int q = slot * 8 + state;
    char *o = (char *)0x230;
    *(int *)(o + q) = fn;
    o += 4;
    *(int *)(o + q) = arg;
  }
  else {
    hook_slot_default((gfx_state_rec *)state,slot);
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int render_done_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,0,fn,arg);
  return 0;
}

int vblank_hook_put(int fn, int arg)
{
  vblank_irq_gate_noop(1);
  hook_slot_store(lib_state_ptr,1,fn,arg);
  return 0;
}

int hblank_hook_put(int fn, int arg)
{
  hblank_irq_gate_noop((uint)(fn != 0));
  hook_slot_store(lib_state_ptr,2,fn,arg);
  return 0;
}

int spg_hblank_line_write(int line)
{
  int t;
  int p;
  int max;
  uint w;

  p = line;
  t = gfx_state.vo_start_y1 + gfx_state.vo_offset_y;
  if (((gfx_state.display_mode & 1) == 0) &&
     ((gfx_state.display_mode & 0x2c) != 0)) {
    max = gfx_state.sync_load_word >> 0x11;
  }
  else {
    max = gfx_state.sync_load_word >> 0x10;
  }
  if ((gfx_state.display_mode & 1) != 0) line = p + t;
  else if ((gfx_state.display_mode & 0x20) != 0) line = p / 2 + t;
  else line = p + t;
  if (line < 0) {
    line = line + max;
  }
  if (line > max) {
    line = max;
  }
  if (line < 0) {
    line = 0;
  }
  p = line & 0x3ff;
  w = gfx_state.hblank_int_word & 0x3ff0000;
  w |= p;
  gfx_state.hblank_int_word = w;
  pvr_reg_write_both(200,w);
  return 0;
}

int spg_status_display_line(int *line_out)
{
  uint raw;
  int v;
  int y;
  int lim;
  int off;
  int t;
  float scale;

  if ((gfx_state.flags & 2) != 0) {
    raw = pvr_reg_read(0x10c);
    v = raw & 0x3ff;
  }
  else {
    v = 0;
  }
  scale = 1.0;
  y = gfx_state.vo_start_y1 + gfx_state.vo_offset_y;
  if ((gfx_state.display_mode & 1) != 0) {
    lim = 0x1e0;
  }
  else if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) != 0) {
    if ((gfx_state.display_mode & 0x20) != 0) {
      scale = 1.0;
      lim = 0x1e0;
      y = y * 2;
    }
    else {
      y = y * 2;
      scale = 0.5;
      lim = 0xf0;
    }
  }
  else {
    lim = 0xf0;
  }
  off = gfx_state.sync_load_word >> 16;
  if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) != 0) {
    t = 0x40000;
    t = (raw & t) == 0;
    v += t == 0;
  }
  v -= y;
  v = (int)((float)v * scale);
  if (v <= lim) {
    *line_out = v;
  }
  else {
    *line_out = v - off;
  }
  return 0;
}

int surface_alloc_fail_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,3,fn,arg);
  return 0;
}

int strip_hazard_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,4,fn,arg);
  return 0;
}

int slot5_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,5,fn,arg);
  return 0;
}

int yuv_done_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,6,fn,arg);
  return 0;
}

int busy_wait_hook_put(int fn, int arg)
{
  hook_slot_store(lib_state_ptr,7,fn,arg);
  return 0;
}

int error_hook_put(int fn)
{
  hook_slot_store(lib_state_ptr,8,fn,0);
  return 0;
}

void render_done_hook_noop(void)
{
  return;
}

void vblank_hook_noop(void)
{
  return;
}

void hblank_hook_noop(void)
{
  return;
}

void surface_alloc_fail_hook_noop(void)
{
  return;
}

void strip_hazard_hook_noop(void)
{
  return;
}

void list_input_end_hook_noop(void)
{
  return;
}

void yuv_done_hook_noop(void)
{
  return;
}

void busy_wait_hook_noop(void)
{
  return;
}

void border_color_by_code_hang(uint code)
{
  uint color;
  switch(code) {
  case 1:
    color = 0xff0000ff;
    break;
  case 2:
    color = 0xffff0000;
    break;
  case 3:
    color = 0xff00ff00;
    break;
  case 4:
    color = 0xffff00ff;
  }
  pvr_vo_border_col_put(color);
  for (;;) {
  }
}
