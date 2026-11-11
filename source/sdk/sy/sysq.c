/*
 * sdk/sy/sysq.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sq_windows_map(void)
{
  uint a;
  uint b;
  uint off;
  int i;
  
  if ((mmu_ctrl_reg & 1) != 0) {
    a = 0x0c000000;
    b = 0xe0000000;
    for (off = 0, i = 0; a + off < 0x0d000000; off += 0x100000, i++) {
      uint *q = &sq_ram_entries[i];
      *q = utlb_install(a + off, b + off, 4);
    }
    a = 0x10000000;
    b = 0xe1000000;
    for (off = 0, i = 0; a + off < 0x10800000; off += 0x100000, i++) {
      uint *q = &sq_ta_entries[i];
      *q = utlb_install(a + off, b + off, 4);
    }
    sq_ta_yuv_entries[0] = utlb_install(0x10800000, 0xe1800000, 4);
  }
  return;
}

void sq_windows_unmap(void)
{
  uint i;

  for (i = 0; i < 0x10; i++) {
    utlb_uninstall(sq_ram_entries[i]);
  }
  for (i = 0; i < 8; i++) {
    utlb_uninstall(sq_ta_entries[i]);
  }
  for (i = 0; i < 1; i++) {
    utlb_uninstall(sq_ta_yuv_entries[i]);
  }
}

uint sq_window_address(uint address)
{
  uint window;

  window = address;
  if ((address >> 24 & 0x1f) < 0xd) {
    window = window << 8 >> 8 | 0xe0000000;
  }
  else {
    window = window << 8 >> 8 | 0xe1000000;
  }
  return window;
}

uint sq_window_to_memory(uint address)
{
  uint memory;

  memory = address;
  if (address < 0xe1000000) {
    memory = memory << 8 >> 8 | 0x8c000000;
  }
  else {
    memory = memory << 8 >> 8 | 0x10000000;
  }
  return memory;
}
