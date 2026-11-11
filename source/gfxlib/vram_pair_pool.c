/*
 * gfxlib/vram_pair_pool.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint vram_pair_pool_used; /* 0x0c417494 */
extern uint vram_pair_pool_size; /* 0x0c4174a0 */
extern vram_pair_halves vram_pair_pool_halves; /* 0x0c417498 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

int vram_pair_pool_alloc(uint size)
{
  uint *halves;
  uint base0, base1, t;
  int result;

  halves = (uint *)&vram_pair_pool_halves;
  vram32_buffer_pair_alloc(size,(int *)halves,halves + 1);
  base0 = gfx_state.ta_bufs[0].base;
  base1 = gfx_state.ta_bufs[1].base;
  result = -1;
  if ((base0 > base1) != (halves[0] > halves[1])) {
    t = halves[0];
    halves[0] = halves[1];
    halves[1] = t;
  }
  if (vram_pair_pool_halves.first_half > base0) {
    if (vram_pair_pool_halves.first_half - base0 == vram_pair_pool_halves.second_half - base1) result = 0;
  }
  if (result != 0) {
    vram_pair_pool_halves.first_half = 0;
    vram_pair_pool_halves.second_half = 0;
    vram_pair_pool_size = 0;
    vram_pair_pool_used = 0;
    return -1;
  }
  vram_pair_pool_size = size;
  vram_pair_pool_used = 0;
  return result;
}

uint vram_pair_pool_take(uint size)
{
  uint addr;

  addr = 0;
  if (vram_pair_pool_size - vram_pair_pool_used < size) return addr;
  addr = vram_pair_pool_halves.second_half + vram_pair_pool_used;
  vram_pair_pool_used = size + vram_pair_pool_used + 0x1f & 0xffffffe0;
  return addr;
}
