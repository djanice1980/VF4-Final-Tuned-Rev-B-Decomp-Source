/*
 * zlib/crc32.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

const uLongf *get_crc_table(void)
{
  return crc_table;
}

uLong crc32(uLong crc, const Bytef *buf, uInt len)
{
    if (buf == 0) return 0L;
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
      len -= 8;
    }
    if (len) do {
      crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
    } while (--len);
    return crc ^ 0xffffffffL;
}
