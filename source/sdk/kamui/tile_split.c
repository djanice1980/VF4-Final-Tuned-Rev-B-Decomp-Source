/*
 * sdk/kamui/tile_split.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint bank_render_index; /* 0x0c41e9ec */
extern uint tile_split_columns_only; /* 0x0c601ff4 */
extern uint tile_grid_last_row; /* 0x0c601ff8 */

void tilesplit_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void tile_split_apply(uint word)
{
  list_bank_rec *banks = list_banks;
  while (*(volatile int *)&banks[*(volatile int *)&bank_render_index].state != 0) {
  }
  frame_split_word_note(word);
  elan_cmd_input_reset();
  elan_tile_split_reg = word;
  tile_split_word = word;
}

uint tile_split_restrict(uint word)
{
  uint mode;
  uint paired;
  uint x1;
  uint x0;

  if (tile_split_columns_only != 0 && (word & 1) != 0) {
    mode = (word & 0x30) >> 4;
    if (mode != 2) {
      paired = (word & 0x40) >> 6;
      if (mode == 1 || mode == 3) {
        word = 0x61;
        if (paired != 1) {
          word = 0x21;
        }
      }
      else {
        x0 = (word & 0x3f000000) >> 24;
        x1 = (word & 0x3f000) >> 12;
        tilesplit_check_or_halt(mode == 0);
        word = ((x0 << 24) & 0x3f000000) | ((x1 << 12) & 0x3f000) | ((tile_grid_last_row & 0xf) << 8) | 1;
      }
    }
  }
  return word;
}

void tile_grid_size_put(int across, int down)
{
  across--;
  down--;
  tile_grid_last_row = down;
  elan_tile_grid_reg = across << 4 | down;
}

void tile_split_columns_only_set(int on)
{
  int current;
  int restricted;
  tile_split_columns_only = on;
  if (on != 0) {
    current = elan_tile_split_reg;
    restricted = tile_split_restrict(current);
    if (current != restricted) {
      tile_split_apply(restricted);
    }
  }
}

void tile_split_owner(uint x, uint y, uint *first_chip, uint *second_chip, uint word)
{
  uint mode;
  uint out;
  uint x0;
  uint y0;
  uint x1;
  uint y1;

  if ((word & 1) == 0) {
    *first_chip = 1;
    *second_chip = 1;
    return;
  }
  mode = (word & 0x30) >> 4;
  if (mode == 0) {
    x0 = (word >> 24) & 0x3f;
    y0 = (word & 0xf00000) >> 20;
    x1 = (word & 0x3f000) >> 12;
    y1 = (word >> 8) & 0xf;
    out = 0;
    if ((x0 > x | x > x1) || (y0 > y | y > y1)) {
      out = 1;
    }
    if (out != (word & 0x80) >> 7) {
      *first_chip = 1;
      *second_chip = 0;
    } else {
      *first_chip = 0;
      *second_chip = 1;
    }
    return;
  }
  if ((word & 0x40) != 0) {
    x >>= 1;
    y >>= 1;
  }
  switch (mode) {
  case 1:
    *second_chip = y & 1;
    break;
  case 2:
    *second_chip = x & 1;
    break;
  case 3:
    *second_chip = (y ^ x) & 1;
    break;
  }
  *first_chip = *second_chip == 0;
}

uint tile_split_select(uint kind, int paired, uint x0, uint y0, uint x1, uint y1)
{
  uint v;
  
  switch(kind) {
  case 0:
    v = 0x71;
    if (paired != 1) {
      v = 0x31;
    }
    break;
  case 1:
    v = (x0 & 0x3f) << 0x18 | (y0 << 0x14 & 0xf00000) | (x1 & 0x3f) << 0xc |
        (y1 & 0xf) << 8 | 1;
    break;
  case 2:
    v = 0x51;
    if (paired != 1) {
      v = 0x11;
    }
    break;
  case 3:
    v = 0x61;
    if (paired != 1) {
      v = 0x21;
    }
    break;
  case 4:
    v = 0;
    break;
  default:
    return tile_split_word;
  }
  tile_split_apply(tile_split_restrict(v));
  return 0;
}
