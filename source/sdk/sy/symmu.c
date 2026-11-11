/*
 * sdk/sy/symmu.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void mmu_control_default_put(void)
{
  *(volatile uint *)0xff000010 = 0x40005;
  return;
}

uint utlb_install(uint phys, uint virt, uint page_size)
{
  uint entry;
  uint page_mask;
  uint size_bits;
  char size_code;
  uint data;

  entry = *(volatile uint *)0xff000010 >> 18 & 0x3f;
  size_code = page_size;
  for (; entry < 0x40; entry++) {
    if ((*(uint *)((entry << 8) + 0xf6000000) & 0x100) == 0) break;
  }
  if (entry == 0x40) return 0xff;
  virt = virt & 0xfffffc00 | 0x100;
  switch (size_code) {
  case 1:
    page_mask = 0x1ffffc00;
    size_bits = 0;
    break;
  case 2:
    page_mask = 0x1ffff000;
    size_bits = 0x10;
    break;
  case 3:
    size_bits = 0x80;
    page_mask = 0x1fff0000;
    break;
  case 4:
    size_bits = 0x90;
    page_mask = 0x1ff00000;
    break;
  }
  data = phys & page_mask | size_bits | 0x13e;
  *(uint *)((entry << 8) + 0xf6000000) = virt;
  *(uint *)((entry << 8) + 0xf7000000) = data;
  return entry;
}

void utlb_uninstall(uint entry)
{
  if (entry < 0x40) {
    uint o = entry << 8;
    *(volatile uint *)(o + 0xf6000000) &= 0xfffffeff;
    *(uint *)(o + 0xf7000000) = 0;
  }
}

void utlb_entry_low_bits_set(uint entry, char bits)
{
  if (entry < 0x40) {
    uint offset = entry << 8;
    *(volatile uint *)(offset + 0xf7000000) = *(volatile uint *)(offset + 0xf7000000) & 0xfffffff6 | bits;
  }
}
