/*
 * sdk/naomi_library/fog.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void fog_table_zero_fill(void)
{
  int i;
  float table[128];
  for (i = 0; i < 0x80; i++) {
    table[i] = 0.0f;
  }
  fog_table_regs_write(table);
}

void fog_col_ram_write_bytes(uint color)
{
  union { struct { unsigned int b : 8; unsigned int g : 8; unsigned int r : 8; unsigned int a : 8; } s; uint w; } c;
  
  int red = (int)color >> 16;
  c.s.a = color >> 24;
  c.s.r = red;
  c.s.g = color >> 8;
  c.s.b = color;
  pvr_fog_col_ram_put(c.w);
}

void fog_col_vert_write_bytes(uint color)
{
  union { uint u; struct { unsigned int b : 8; unsigned int g : 8; unsigned int r : 8; unsigned int a : 8; } c; } v;
  short red = color >> 16;
  v.c.a = color >> 24;
  v.c.r = red;
  v.c.g = color >> 8;
  v.c.b = color;
  pvr_fog_col_vert_put(v.u);
}

void fog_density_reg_write(uint density)
{
  pvr_fog_coeff_put(density);
  return;
}

void fog_table_regs_write(float *table)
{
  pvr_fog_entries_from_floats(table);
  return;
}

uint fog_density_pack_float(float density)
{
  union { float f; int i; } u;
  int exponent;
  int mantissa;
  int word;
  
  u.f = density;
  word = u.i;
  exponent = (word >> 0x17) - 0x7f;
  mantissa = (word >> 0x10 | 0x80) & 0xff;
  if (exponent < 0) {
    mantissa = mantissa >> -exponent;
    exponent = 0;
  }
  exponent = (uchar)exponent; word = mantissa << 8 | exponent;
  pvr_fog_coeff_put(word);
  return word;
}
