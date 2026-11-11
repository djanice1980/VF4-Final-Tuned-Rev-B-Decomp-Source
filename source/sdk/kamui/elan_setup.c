/*
 * sdk/kamui/elan_setup.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint elan_reg_d0_value; /* 0x0c32ac4c */
extern uint tile_split_seen[2]; /* 0x0c32ac64 */
extern uint sq_save_top; /* 0x0c41eb28 */
extern uint elan_ctrl_saved; /* 0x0c617014 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern tile_bitmap_ptrs tile_owner_bitmaps; /* 0x0c41ea10 */
extern elan_rev_rec elan_rev_config; /* 0x0c41eb20 */

void elanhw_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void elanhw_sq_save(uint *save)
{
  unsigned int lvl;
  uint q;
  uint v;
  
  {
    unsigned int sr;
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    lvl = 0xf0;
    lvl &= sr;
    lvl >>= 4;
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  }
  q = sq_save_top & 0x03ffffff | 0xe0000000;
  elanhw_check_or_halt((q & 0x3f) == 0);
  save[0] = *(volatile uint *)0xff000038;
  save[1] = *(volatile uint *)0xff00003c;
  save[2] = sq_save_top;
  sq_save_top = sq_save_top + 0x40;
  v = sq_save_top >> 0x18 & 0x1c;
  *(volatile uint *)0xff000038 = v;
  *(volatile uint *)0xff00003c = v;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  q += 0x20;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  *(volatile uint *)0xff000038 = save[0];
  *(volatile uint *)0xff00003c = save[1];
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
}

void elanhw_sq_restore(uint *save)
{
  uint *p;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint *q;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  q = (uint *)0xe0000000;
  p = (uint *)(save[2] & 0x1fffffff | 0xa0000000);
  sq_save_top = sq_save_top + -0x40;
  elanhw_check_or_halt((uint)(save[2] == sq_save_top));
  *(volatile uint *)0xff000038 = *save;
  *(volatile uint *)0xff00003c = save[1];
  q[0] = *p;
  q[1] = p[1];
  q[2] = p[2];
  q[3] = p[3];
  q[4] = p[4];
  q[5] = p[5];
  q[6] = p[6];
  q[7] = p[7];
  q[8] = p[8];
  q[9] = p[9];
  q[10] = p[10];
  q[11] = p[0xb];
  q[12] = p[0xc];
  q[13] = p[0xd];
  q[14] = p[0xe];
  q[15] = p[0xf];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return;
}

void elan_cmd_input_reset(void)
{
  *(volatile int *)0xa8800008 = 1;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800000 = 0;
  *(volatile int *)0xa8800008 = 0;
}

int elan_start_and_probe(void)
{
  int revision;
  
  xfer_job_pool_init();
  bank_lists_seed();
  *(volatile int *)0xa8800014 = 0x2029;
  elan_cmd_input_reset();
  *(volatile int *)0xa8800010 = 7;
  *(volatile int *)0xa8800030 = 0x71;
  *(volatile int *)0xa8800000;
  revision = *(volatile int *)0xa8800004;
  if (revision == 1) {
    elan_rev_config.queue_limit = 2;
    elan_rev_config.flags = 0x1f;
  }
  else {
    elan_rev_config.queue_limit = 3;
    elan_rev_config.flags = 0x20;
  }
  return revision;
}

void elan_irq_regs_init(void)
{
  *(volatile int *)0xa880007c = 0;
  *(volatile int *)0xa8800080 = 0x17;
  *(volatile int *)0xa8800084 = 0x2b;
  *(volatile int *)0xa8800088 = 0;
  *(volatile int *)0xa8800078 = 0x3f;
}

void elan_ctrl_bit0_off_or_back(int back)
{
  volatile int x;
  x = irq_status_reg;
  if (back != 0) {
    elan_ctrl_reg = elan_ctrl_saved;
  }
  else {
    elan_ctrl_saved = elan_ctrl_reg;
    elan_ctrl_reg = elan_ctrl_saved & 0xfffffffe;
  }
  x = irq_status_reg;
}

void elan_ram_window(uint *base, uint *size)
{
  *base = 0xaa000000;
  *size = 0x1ffc000;
  return;
}

void empty_fn_0c151820(void)
{
  return;
}

void tile_split_bitmap_update(int buffer)
{
  uint v;
  uint *cur;
  uint *q;
  uint lo_x, lo_y, hi_x, hi_y, flag;
  uint x, y;
  uint bits;
  int t;

  v = gfx_state.ta_bufs[buffer].split_word;
  cur = &tile_split_seen[buffer];
  if (v != *cur) {
    *cur = v;
    if ((v & 1) != 0) {
      switch ((v & 0x30) >> 4) {
      case 3:
        if ((v & 0x40) != 0) {
          q = tile_bitmap_checker_paired;
        }
        else {
          q = tile_bitmap_checker;
        }
        break;
      case 1:
        if ((v & 0x40) != 0) {
          q = tile_bitmap_rows_paired;
        }
        else {
          q = tile_bitmap_rows;
        }
        break;
      case 2:
        if ((v & 0x40) != 0) {
          q = tile_bitmap_columns_paired;
        }
        else {
          q = tile_bitmap_columns;
        }
        break;
      case 0:
        lo_x = (v >> 24) & 0x3f;
        lo_y = (v & 0xf00000) >> 20;
        hi_x = (v & 0x3f000) >> 12;
        hi_y = (v >> 8) & 0xf;
        flag = (v & 0x80) >> 7;
        if (buffer != 0) {
          q = tile_rect_bitmap_buffer1;
        }
        else {
          q = tile_rect_bitmap_buffer0;
        }
        *q++ = 5;
        for (y = 0; y < gfx_state.tile_rows; y++) {
          bits = 0;
          for (x = 0; x < gfx_state.tile_cols; x++) {
            bits += bits;
            t = 0;
            if (((x < lo_x) | (x > hi_x)) || ((y < lo_y) | (y > hi_y))) {
              t = 1;
            }
            if (t != flag) {
              bits |= 1;
            }
          }
          *q++ = bits << (32 - gfx_state.tile_cols);
        }
        if (buffer != 0) {
          q = tile_rect_bitmap_buffer1;
        }
        else {
          q = tile_rect_bitmap_buffer0;
        }
        break;
      default:
        q = tile_bitmap_all;
        break;
      }
    }
    else {
      q = tile_bitmap_all;
    }
    (&tile_owner_bitmaps.buffer0)[buffer] = (uint)q;
  }
}

void elan_d0_set_for_list(uint kind)
{
  uint value;
  
  value = 6;
  if ((kind & 1) == 0) {
    value = elan_reg_d0_value;
  }
  elan_reg_d0 = value;
  return;
}
