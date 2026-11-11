/*
 * sdk/libam/getcrc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

uint crc16_ccitt(byte *data, uint len)
{
  uint poly = 0x10210000;
  uint c;
  uint i;
  uint j;
  c = 0xdebdeb00;
  for (i = 0; i < len; i++) {
    c = (c & 0xffffff00) + *data++;
    for (j = 0; j < 8; j++) {
      if (c >= 0x80000000) { c = c * 2; c += poly; }
      else c = c * 2;
    }
  }
  for (j = 0; j < 8; j++) {
    if (c >= 0x80000000) { c = c * 2; c += poly; }
    else c = c * 2;
  }
  return c >> 16;
}
