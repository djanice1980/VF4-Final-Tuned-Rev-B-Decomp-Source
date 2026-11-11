/*
 * sdk/libam/backup.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"
#define SRAM_BANK1_AREA ((uchar *)0xa0200100)
#define SRAM_BANK0_AREA ((uchar *)0xa0200008)
#define SRAM_TIME_AREA ((uint *)0xa0200000)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void sram_banks_clear(void)
{
  uint flag;
  int *hw;
  int i;
  uchar *p;
  uint *q;
  uint crc;

  flag = *(uint *)(sram_bank0 + 4) & 1;
  hw = (int *)0xa0200000;
  for (i = 0x7d; i != -1; i--) *hw++ = 0;
  p = sram_bank0;
  q = (uint *)sram_bank1;
  i = (int)sram_ledger_bank - (int)p + 0xf0;
  crc = crc16_ccitt(p + 8, i);
  *(uint *)p = crc;
  *q = crc;
  *(uint *)(p + 4) = flag;
  q[1] = flag;
}

void sram_clear_all(void)
{
  uint flag = *(uint *)(sram_bank0 + 4) & 1;
  int *h = (int *)0xa0200000;
  int i;
  uchar *p;
  uint *q;
  uint crc;
  for (i = 0x1fff; i != -1; i--) *h++ = 0;
  p = sram_bank0;
  q = (uint *)sram_bank1;
  i = sram_ledger_bank - p + 0xf0;
  crc = crc16_ccitt(p + 8, i);
  *(uint *)p = crc;
  *q = crc;
  ((uint *)p)[1] = flag;
  q[1] = flag;
}

void sram_bank_copy(uint *src, uint *dst)
{
  int n;
  uint w;
  uint *d;
  uint *s;

  s = src + 2;
  d = dst + 2;
  n = 0x37;
  do {
    w = *s;
    s = s + 1;
    *d = w;
    d = d + 1;
    n = n + -1;
  } while (n != -1);
  *dst = *src;
  return;
}

void sram_sync_from_bank0(void)
{
  uchar *bank0;
  uchar *bank1;
  int n;
  bank0 = sram_bank0;
  bank1 = sram_bank1;
  n = (int)sram_ledger_bank - (int)bank0 + 0xf0;
  *(uint *)bank0 = crc16_ccitt(bank0 + 8,n);
  sram_bank_copy((uint *)bank0,(uint *)bank1);
  return;
}

void sram_sync_from_bank1(void)
{
  uchar *p = sram_bank0;
  uint *q = (uint *)sram_bank1;
  uint n = sram_ledger_bank - p + 0xf0;
  *q = crc16_ccitt(p + 8, n);
  sram_bank_copy(q, (uint *)p);
}

int sram_check_banks(void)
{
  uint saved0;
  uint saved1;
  uint crc0;
  uint crc1;
  uchar *p;
  uchar *q;

  sram_check_word = 0;
  sram_time = SRAM_TIME_AREA;
  p = SRAM_BANK0_AREA;
  sram_ledger_bank = p;
  sram_bank0 = p;
  q = SRAM_BANK1_AREA;
  sram_bank1 = q;
  saved0 = *(volatile ushort *)p;
  saved1 = *(volatile ushort *)q;
  {uint n = 0xf0;
  crc0 = crc16_ccitt((byte *)(p + 8),n);
  crc1 = crc16_ccitt((byte *)(q + 8),n);
  }
  if (crc0 == saved0) {
    if (crc1 == saved1) return 0;
    sram_sync_from_bank0();
    return 1;
  }
  if (crc1 == saved1) {
    sram_sync_from_bank1();
    return 2;
  }
  sram_clear_all();
  return 3;
}

int saturating_increment(int *counter)
{
  int v;

  v = 0;
  if (*counter == -1) return v;
  v = *counter + 1;
  *counter = v;
  return v;
}

int saturating_decrement(int *counter)
{
  int v;

  v = 0;
  if (*counter == 0) return v;
  v = *counter - 1;
  *counter = v;
  return v;
}

void sram_set_bank_flag(uint flag)
{
  volatile uint *p = (volatile uint *)0xa0200008;
  p[1] = flag;
  p = (volatile uint *)0xa0200100;
  p[1] = flag;
  sram_sync_from_bank0();
}

bool sram_read_bank_flag(void)
{
  uchar *q = SRAM_BANK1_AREA;
  uchar *p = SRAM_BANK0_AREA;
  int b = *(int *)(q + 4);
  int a = *(int *)(p + 4);
  int z = (a == 0) & (b == 0);
  if (z) return 0;
  return (a == 1) & (b == 1);
}
