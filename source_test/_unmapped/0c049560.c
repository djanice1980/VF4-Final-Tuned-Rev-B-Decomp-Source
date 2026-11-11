/*
 * _unmapped/0c049560.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void FUN_0c04a920(uint *src, uint *dst)
{
  int i;
  uint word;
  uint *d;
  uint *s;
  
  s = src + 2;
  d = dst + 2;
  i = 0x37;
  do {
    word = *s;
    s = s + 1;
    *d = word;
    d = d + 1;
    i = i + -1;
  } while (i != -1);
  *dst = *src;
  return;
}

void FUN_0c04abe0(uchar *src, uchar *dst)
{
  uchar c;
  int i;
  
  i = 0x11;
  do {
    c = *src;
    src = src + 1;
    *dst = c;
    dst = dst + 1;
    i = i + -1;
  } while (i != -1);
  return;
}

void FUN_0c04bce0(int n, uchar *dst)
{
  while (n = n + -1, n != -1) {
    *dst = 0;
    dst = dst + 1;
  }
  return;
}

void FUN_0c04c360(int n, uchar *dst)
{
  while (n = n + -1, n != -1) {
    *dst = 0;
    dst = dst + 1;
  }
  return;
}

int FUN_0c04c560(void)
{
  return 0x2a;
}

int FUN_0c04e8c0(void)
{
  return 0;
}

void FUN_0c04e8e0(void)
{
  return;
}

void FUN_0c04ed40(void)
{
  return;
}

void FUN_0c04ed60(void)
{
  return;
}
