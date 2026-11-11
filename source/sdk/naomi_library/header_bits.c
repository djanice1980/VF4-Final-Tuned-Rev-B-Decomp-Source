/*
 * sdk/naomi_library/header_bits.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */

void hdr_or_user_clip(int clip_mode)
{
  poly_header_or_words.control_or &= 0xfffcffff; poly_header_or_words.control_or |= clip_mode << 0x10;
}

void hdr_or_volume(int volume_bits)
{
  poly_header_or_words.control_or &= 0xffffff3fU;
  poly_header_or_words.control_or |= volume_bits << 6;
}

void hdr_or_fog(int fog_mode)
{
  poly_header_or_words.tsp_or &= 0xff3fffff; poly_header_or_words.tsp_or |= fog_mode << 22;
}

void hdr_or_color_clamp(int clamp)
{
  poly_header_or_words.tsp_or &= 0xffdfffff;
  poly_header_or_words.tsp_or |= clamp << 0x15;
}

void hdr_or_bits8_set(int value)
{
  poly_header_or_words.tsp_or &= 0xfffff0ff;
  poly_header_or_words.tsp_or |= value << 8;
}

void hdr_or_top_bit(int on)
{
  if (on == 0) {
    poly_header_or_words.control_or = poly_header_or_words.control_or & 0x7fffffff;
  }
  else {
    poly_header_or_words.control_or = poly_header_or_words.control_or | 0x80000000;
  }
  return;
}

void hdr_or_filter(int filter)
{
  if (filter == 0) {
    poly_header_or_words.tsp_or = poly_header_or_words.tsp_or & 0xffff9fff;
  }
  else {
    poly_header_or_words.tsp_or = filter << 0xd | poly_header_or_words.tsp_or;
  }
  return;
}
