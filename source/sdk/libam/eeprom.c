/*
 * sdk/libam/eeprom.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void eeprom_copy_block(uchar *src, uchar *dst)
{
  uchar b;
  int n;

  n = 0x11;
  do {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void eeprom_read_image(void)
{
  int *reply;
  uchar *s;
  uchar *d;
  int i;
  uint args [2];

  do {
    do {
      reply = (int *)maple_send(1,0,0x20,0,0);
    } while (*reply == -1);
  } while (*reply == 0x2000fc);
  args[1] = 1;
  maple_send(0x86,0,0x20,&args[1],1);
  do {
    do {
      reply = (int *)maple_send(1,0,0x20,0,0);
    } while (*reply == -1);
  } while (*reply == 0x2000fc);
  args[1] = 3;
  s = maple_send(0x86,0,0x20,&args[1],1) + 4;
  d = eeprom_image;
  for (i = 0; i < 0x80; i++) *d++ = *s++;
  s = eeprom_image; d = eeprom_copy_a;
  for (i = 0x11; i != -1; i--) *d++ = *s++;
  s = eeprom_image + 0x12; d = eeprom_copy_b;
  for (i = 0x11; i != -1; i--) *d++ = *s++;
}

void eeprom_write_image(void)
{
  uchar *s;
  uchar *d;
  int k;
  int i;
  int j;
  int *reply;
  uchar buf[32];
  volatile int w;

  s = eeprom_copy_a;
  d = eeprom_image;
  k = 18;
  while (k--) *d++ = *s++;
  s = eeprom_copy_b;
  d = eeprom_image + 0x12;
  k = 18;
  while (k--) *d++ = *s++;
  w = 100000;
  do w--; while (w != -1);
  s = eeprom_image;
  for (i = 0; i < 8; i++) {
    buf[0] = 0xb;
    d = buf + 1;
    *d++ = i * 0x10;
    *d++ = 0x10;
    *d++ = 0;
    for (j = 0; j < 16; j++) *d++ = *s++;
    do {
      do {
        reply = (int *)maple_send(1,0,0x20,0,0);
      } while (*reply == -1);
    } while (*reply == 0x2000fc);
    maple_send(0x86,0,0x20,(uint *)buf,5);
  }
}

void eeprom_reset_copies(void)
{
  int i;
  uchar *q;
  uchar *p;

  p = eeprom_copy_a;
  q = eeprom_image;
  i = 0x23;
  do {
    *q = 0;
    q = q + 1;
    i = i + -1;
  } while (i != -1);
  q = p;
  i = 0x11;
  do {
    *q = 0;
    q = q + 1;
    i = i + -1;
  } while (i != -1);
  *(ushort *)p = crc16_ccitt(p + 2,0x10);
  eeprom_copy_block(p,eeprom_copy_b);
  return;
}

void eeprom_sync_from_a(void)
{
  uchar *p = eeprom_copy_a;
  *(short *)p = crc16_ccitt(p + 2,0x10);
  eeprom_copy_block(p,eeprom_copy_b);
  eeprom_write_image();
  return;
}

void eeprom_sync_from_b(void)
{
  uchar *p = eeprom_copy_b;
  *(short *)p = crc16_ccitt(p + 2, 0x10);
  eeprom_copy_block(p, eeprom_copy_a);
  eeprom_write_image();
  return;
}

int eeprom_check_copies(void)
{
  uint crc_a;
  uint crc_b;
  int i;
  uint saved_a;
  uint saved_b;
  ushort *p;
  ushort *q;

  for (i = 0; i < 0x80; i++) {
    ((uint *)eeprom_image)[i] = 0;
  }
  eeprom_read_image();
  p = (ushort *)eeprom_copy_a;
  q = (ushort *)eeprom_copy_b;
  eeprom_board_config = (uchar *)p + 2;
  eeprom_coin_config = (uchar *)p + 7;
  saved_a = *p;
  saved_b = *q++;
  crc_a = crc16_ccitt((byte *)p + 2,0x10);
  crc_b = crc16_ccitt((byte *)q,0x10);
  if (crc_a == saved_a) {
    if (crc_b == saved_b) {
      return 0;
    }
    eeprom_sync_from_a();
    return 1;
  }
  if (crc_b == saved_b) {
    eeprom_sync_from_b();
    return 2;
  }
  eeprom_reset_copies();
  return 3;
}

void eeprom_flush(void)
{
  eeprom_sync_from_a();
  return;
}
