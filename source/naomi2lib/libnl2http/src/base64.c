/*
 * naomi2lib/libnl2http/src/base64.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int base64_encode(char *out, int *out_size, byte *in, int src_len)
{
  int ret = 0;
  char *end = out + *out_size;
  int cnt = 0;
  int acc = 0;
  int col = 0;
  char *p = out;

  while (src_len-- != 0) {
    acc += *in++;
    cnt++;
    if (cnt == 3) {
      if (p + 4 >= end) {
        ret = -1;
        acc = 0;
        cnt = 0;
        break;
      }
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[acc >> 18];
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[(acc >> 12) & 0x3f];
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[(acc >> 6) & 0x3f];
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[acc & 0x3f];
      col += 4;
      if (col == 72) {
        if (p >= end) {
          ret = -1;
          acc = 0;
          cnt = 0;
          break;
        }
        *p++ = '\n';
        col = 0;
      }
      acc = 0;
      cnt = 0;
    } else {
      acc <<= 8;
    }
  }
  if (cnt != 0) {
    if (p + 4 < end) {
      acc <<= 16 - cnt * 8;
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[acc >> 18];
      *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[(acc >> 12) & 0x3f];
      if (cnt == 1) {
        *p++ = '=';
        *p++ = '=';
      } else {
        *p++ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\nNLASOCK Ver.0.12 COPYRIGHT (C) WOW ENTERTAINMENT INC.\n"[(acc >> 6) & 0x3f];
        *p++ = '=';
      }
    } else {
      ret = -1;
    }
  }
  if (col > 0) {
    if (p < end) {
      *p++ = '\n';
    }
  }
  *out_size = p - out;
  return ret;
}

int base64_decode(byte *out, int *out_size, byte *in, int src_len)
{
  byte *dst;
  byte *end;
  int bits;
  uint acc;
  uint c;
  int ret;
  
  ret = 0;
  dst = out;
  end = out + *out_size;
  bits = 8;
  acc = 0;
  while (--src_len != -1) {
    c = *in++;
    if (c - 'A' < 26) {
      c = c - 'A';
    }
    else if (c - 'a' < 26) {
      c = c - 'a' + 26;
    }
    else if (c - '0' < 10) {
      c = c - '0' + 52;
    }
    else if (c == '+') {
      c = 62;
    }
    else if (c == '/') {
      c = 63;
    }
    else {
      if (c == '=') {
        bits = 8;
        acc = 0;
      }
      continue;
    }
    if (bits > 6) {
      acc = acc << 6 | c;
      bits -= 6;
    }
    else {
      uint b = acc << bits | ((int)c >> (6 - bits)) & 0x3f;
      bits += 2;
      acc = c;
      if (dst < end) {
        *dst++ = b;
      }
      else {
        ret = -1;
        break;
      }
    }
  }
  *out_size = dst - out;
  return ret;
}
