/*
 * gfxlib/dcache_purge.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void dcache_purge_lines(uint addr, int size)
{
  uint end = addr + size;
  for (addr &= 0xffffffe0; addr < end; addr += 0x20) {
    __asm__ volatile ("ocbp @%0" : : "r" (addr));
  }
}
