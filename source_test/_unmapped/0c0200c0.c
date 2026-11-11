/*
 * _unmapped/0c0200c0.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void FUN_0c0204c0(void)
{
  return;
}

void FUN_0c0204e0(void)
{
  return;
}

void FUN_0c0253c0(void)
{
  return;
}

void FUN_0c0257e0(button_word *word, uint now)
{
  uint old;

  old = word->now;
  word->now = now;
  word->on = ~old & now;
  word->off = old & ~now;
  word->old = old;
  return;
}

void FUN_0c026300(player_input_rec *input, ushort *dst)
{
  *dst = *(ushort *)&input->now;
  return;
}

uint FUN_0c027c80(card_header_rec *header)
{
  uint format;

  format = header->flags >> 4 & 3;
  if (format == 3) {
    format = (header->ftf.b[0] >> 1 & 3) + 3;
  }
  return format;
}

uint FUN_0c027cc0(card_header_rec *header)
{
  return header->flags >> 4 & 3;
}

void FUN_0c029b60(static_heap_rec *block, void *addr, uint size, char *label)
{
  block->addr = addr;
  block->size = size;
  block->label = label;
  return;
}

void FUN_0c02d9c0(int job)
{
  *(int *)(job + 8) = 0;
  return;
}

int FUN_0c02e960(int conn)
{
  return *(int *)(conn + 0x10);
}

uint FUN_0c032300(int index)
{
  return index + 1U & 0xf;
}

uint FUN_0c0323e0(int index)
{
  return index + 0xfU & 0xf;
}

int FUN_0c034520(uint bits)
{
  int count;

  count = 0;
  for (; 0 < (int)bits; bits = (int)bits >> 1) {
    count = count + (bits & 1);
  }
  return count;
}

void FUN_0c0355e0(int *out, int start, int tile_count, int *sizes)
{
  int size;
  int i;

  i = 0;
  do {
    *out = start;
    out = out + 1;
    size = *sizes;
    sizes = sizes + 1;
    start = start + tile_count * size;
    i = i + 1;
  } while (i < 5);
  *out = start;
  return;
}

void FUN_0c035880(int *out, int *layout, int parameter_base, int block_addr)
{
  *out = parameter_base;
  out[1] = layout[1] + parameter_base + block_addr;
  out[2] = layout[2] + parameter_base + block_addr;
  out[3] = layout[3] + parameter_base + block_addr;
  out[4] = parameter_base + layout[4] + block_addr;
  out[5] = layout[5];
  return;
}

int FUN_0c02e8e0(int conn, uint addr)
{
  if (conn != 0) {
    *(uint *)(conn + 0x1c) = addr;
    return 0;
  }
  return -1;
}

int FUN_0c02e900(int conn, uint limit_a, uint limit_b, uint limit_c)
{
  if (!(conn != 0)) {
    return -1;
  }
  *(uint *)(conn + 0x28) = limit_a;
  *(uint *)(conn + 0x2c) = limit_b;
  *(uint *)(conn + 0x30) = limit_c;
  return 0;
}

int FUN_0c02e920(int conn, uint header, uint body)
{
  if (!(conn != 0)) {
    return -1;
  }
  *(uint *)(conn + 0x44) = header;
  *(uint *)(conn + 0x48) = body;
  return 0;
}

int FUN_0c02e940(int conn, uint header, uint body)
{
  if (!(conn != 0)) {
    return -1;
  }
  *(uint *)(conn + 0x6c) = header;
  *(uint *)(conn + 0x70) = body;
  return 0;
}
