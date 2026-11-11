/*
 * sdk/libam/boot.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void cart_timing_slowest(void)
{
  uint dma_timing_base;
  
  dma_timing_base = 0x1000;
  if (0 < (int)bios_helper_50(0)) {
    dma_timing_base = 0x1100;
  }
  *(volatile uint *)0xa05f74b8 = 0x8843007f;
  *(volatile uint *)0xa05f7480 = 0x600;
  *(volatile uint *)0xa05f7484 = 0x600;
  *(volatile uint *)0xa05f7488 = 0x200;
  *(volatile uint *)0xa05f748c = 0x200;
  *(volatile uint *)0xa05f7490 = 0x811;
  *(volatile uint *)0xa05f7494 = 0x811;
  *(volatile uint *)0xa05f74a4 = dma_timing_base | 9;
  *(volatile uint *)0xa05f74a0 = dma_timing_base | 9;
}

void cart_timing_from_ns(int access_ns)
{
  uint dma_timing_base;
  int bios_flag;
  uint dma_timing;

  dma_timing_base = 0x1000;
  bios_flag = (int)bios_helper_50(0) > 0;
  if (bios_flag) {
    dma_timing_base = 0x1100;
  }
  *(volatile uint *)0xa05f74b8 = 0x8843007f;
  *(volatile uint *)0xa05f7480 = 0x600;
  *(volatile uint *)0xa05f7484 = 0x600;
  *(volatile uint *)0xa05f7488 = 0x200;
  *(volatile uint *)0xa05f748c = 0x200;
  switch (access_ns) {
  case 0x50:
  case 0x5a:
    *(volatile uint *)0xa05f7490 = 0x211;
    *(volatile uint *)0xa05f7494 = 0x211;
    dma_timing = dma_timing_base | 3;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 100:
  case 0x6e:
    *(volatile uint *)0xa05f7490 = 0x311;
    *(volatile uint *)0xa05f7494 = 0x311;
    dma_timing = dma_timing_base | 4;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 0x78:
  case 0x82:
    *(volatile uint *)0xa05f7490 = 0x411;
    *(volatile uint *)0xa05f7494 = 0x411;
    dma_timing = dma_timing_base | 5;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 0xa0:
  case 0xaa:
    *(volatile uint *)0xa05f7490 = 0x611;
    *(volatile uint *)0xa05f7494 = 0x611;
    dma_timing = dma_timing_base | 7;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 0xb4:
  case 0xbe:
    *(volatile uint *)0xa05f7490 = 0x711;
    *(volatile uint *)0xa05f7494 = 0x711;
    dma_timing = dma_timing_base | 8;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 200:
  case 0xd2:
    *(volatile uint *)0xa05f7490 = 0x811;
    *(volatile uint *)0xa05f7494 = 0x811;
    dma_timing = dma_timing_base | 9;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  case 0x8c:
  case 0x96:
  default:
    *(volatile uint *)0xa05f7490 = 0x511;
    *(volatile uint *)0xa05f7494 = 0x511;
    dma_timing = dma_timing_base | 6;
    *(volatile uint *)0xa05f74a4 = dma_timing;
    *(volatile uint *)0xa05f74a0 = dma_timing;
    if (bios_flag) {
      bios_g1_dma_timing = dma_timing;
    }
    break;
  }
}

void g1_dma_spin_idle(void)
{
  do {
  } while (g1_dma_status_reg != 0);
  return;
}

long rom_dma_busy(void)
{
    if (*(volatile unsigned int *)0xa05f7418)
        return 1;
    if (*(volatile unsigned int *)0xa05f700c & 0x4000)
        return 0;
    return 1;
}
