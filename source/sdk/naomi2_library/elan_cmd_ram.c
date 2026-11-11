/*
 * sdk/naomi2_library/elan_cmd_ram.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

bool elan_ram_region_take(void)
{
  int status;
  uint base;
  uint size;

  if (render_frames_in_flight == 2) status = elan_ram_span_get(1,&base,&size);
  else status = elan_ram_span_get(0,&base,&size);
  elan_ram_region_start = base;
  elan_ram_region_end = base + size;
  elan_ram_top = base + size;
  elan_cmd_write_ptr = (uint *)base;
  return status == 0;
}

void elan_ram_pointers_clear(void)
{
  elan_ram_top = 0;
  elan_cmd_write_ptr = 0;
  return;
}

uint *elan_cmd_alloc(int size)
{
  int rounded = size + 0x1fU & 0xffffffe0;
  uint *start = elan_cmd_write_ptr;
  elan_cmd_write_ptr = (uint *)(rounded + (int)start);
  return start;
}

int elan_ram_alloc_top(int size)
{
  int rounded = size + 0x1fU & 0xffffffe0;
  elan_ram_top = elan_ram_top - rounded;
  return elan_ram_top;
}

void elan_ram_top_release_to(int p, int size)
{
    elan_ram_top = p + ((size + 0x1f) & ~0x1f);
}

void elan_cmd_buffer_flip(void)
{
  if (render_frames_in_flight == 2) {
    elan_cmd_write_ptr = (uint *)elan_ram_region_start;
  }
  else {
    elan_cmd_half = elan_cmd_half ^ 1;
    elan_cmd_write_ptr = (uint *)(elan_ram_region_start + elan_cmd_half * elan_cmd_half_size);
  }
}

void elan_ram_top_release_all(void)
{
  elan_ram_top = elan_ram_region_end;
  return;
}
