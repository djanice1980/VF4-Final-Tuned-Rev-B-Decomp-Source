/*
 * sdk/kamui/hw_defaults.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void pvr_reset_pulse(void)
{
  pvr_reg_write_both(8,3);
  pvr_reg_write_both(0,0);
  pvr_reg_write_both(8,0);
  return;
}

void empty_fn_0c147560(void)
{
  return;
}

int pvr_regs_defaults_and_vram_clear(void)
{
  int chip_id;
  uint revision;
  int reg30_word;
  int memory_config_word;
  int i;
  uint zero[1];
  memory_config_word = 0xdd18957;
  gfx_state.core_reg98_value = 0;
  if (elan_start_and_probe() == 0) return 0;
  chip_id = pvr_reg_read(0);
  revision = pvr_reg_read(4);
  gfx_state.chip_rev = 5;
  if (chip_id != 0x17fd11db) {
    gfx_state.chip_rev = 5;
  } else {
    if (revision == 1) gfx_state.chip_rev = 5;
    else if (revision > 16) {
      gfx_state.chip_rev = 6;
      gfx_state.core_reg98_value = 0x800408;
      memory_config_word = 0x15d1c955;
    }
  }
  switch (chip_revision_reg) {
  case 0:
    gfx_state.flags = 0;
    reg30_word = 0;
    break;
  case 1:
    gfx_state.flags = 1;
    reg30_word = 0;
    break;
  case 2:
    gfx_state.flags = 3;
    reg30_word = 0x101;
    break;
  case 4:
  case 5:
  case 6:
    gfx_state.flags = 0x7f;
    reg30_word = 0x10101;
    break;
  case 8:
    gfx_state.flags = 0x7f;
    reg30_word = 0x101;
    break;
  case 9:
    gfx_state.flags = 0x6f;
    reg30_word = 0x101;
    break;
  default:
    gfx_state.flags = 0x8f;
    reg30_word = 0x101;
    break;
  }
  pvr_reg_write_both(0xa8, memory_config_word);
  pvr_reg_write_both(0xa0, 0x20);
  pvr_reg_write_both(8, 0);
  pvr_reg_write_both(0xe4, 3);
  pvr_reg_write_both(0x48, 9);
  if (gfx_state.video_out & 1) pvr_reg_write_both(0x44, 0x800000);
  else pvr_reg_write_both(0x44, 0);
  pvr_reg_write_both(0x5c, 0);
  pvr_reg_write_both(0x68, 0x2800000);
  pvr_reg_write_both(0x6c, 0x1e00000);
  if (gfx_state.chip_rev == 6) {
    pvr_reg_write_both(0x110, 0x93f39);
    pvr_reg_write_both(0x98, gfx_state.core_reg98_value);
  } else {
    pvr_reg_write_both(0x110, 0x92039);
  }
  pvr_reg_write_both(0x4c, 0xa0);
  pvr_reg_write_both(0x74, 0);
  pvr_reg_write_both(0x78, 0x3f800000);
  pvr_reg_write_both(0x84, 0);
  pvr_reg_write_both(0x30, reg30_word);
  pvr_reg_write_both(0xb0, 0x7f7f7f);
  pvr_reg_write_both(0xb4, 0x7f7f7f);
  pvr_reg_write_both(0xc0, 0);
  pvr_reg_write_both(0xbc, 0xffffffff);
  pvr_reg_write_both(0x80, 7);
  pvr_reg_write_both(0x88, 0x3727c5a0);
  pvr_reg_write_both(0x8c, 0x1203000);
  pvr_reg_write_both(0x74, 1);
  if (gfx_state.chip_rev != 6) for (;;) ;
  if (gfx_state.chip_rev == 6) pvr_reg_write_both(0x7c, 0x27df77);
  else pvr_reg_write_both(0x7c, 0x7df77);
  zero[0] = 0;
  for (i = 0; i < 0x200000; i++) copy_words((uint *)(i * 4 + 0xa5000000), zero, 4);
  return 1;
}

void fog_regs_defaults(void)
{
  int i;
  int entry;

  pvr_reg_write_both(0xb8,0xff07);
  pvr_reg_write_both(0xb4,0x7f7f7f);
  pvr_reg_write_both(0xb0,0x7f7f7f);
  for (i = 0; i < 0x80; i++) {
    entry = ((0xfe - i) & 0xff | ((0xff - i) & 0xff) << 8) & 0xffff;
    pvr_reg_write_both(0x200 + i * 4,entry);
  }
}

void teardown_empty_step(void)
{
  return;
}
