/*
 * sdk/libam/usereep.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern eeprom_save_header_a_rec eeprom_save_header_a; /* 0x0c5cafb0 */
extern eeprom_save_header_b_rec eeprom_save_header_b; /* 0x0c5cafb4 */

void eeprom_save_zero(int n, uchar *dst)
{
  while (n = n + -1, n != -1) {
    *dst = 0;
    dst = dst + 1;
  }
  return;
}

void eeprom_save_read_headers(void)
{
  uchar b;
  int n;
  uchar *dst;
  uchar *src;

  src = eeprom_image + 0x24;
  dst = (uchar *)&eeprom_save_header_a;
  n = 3;
  do {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  src = eeprom_image + 0x28;
  dst = (uchar *)&eeprom_save_header_b;
  n = 3;
  do {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void eeprom_save_write_headers(void)
{
  uchar b;
  int n;
  uchar *dst;
  uchar *src;

  src = (uchar *)&eeprom_save_header_a;
  dst = eeprom_image + 0x24;
  n = 3;
  do {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  src = (uchar *)&eeprom_save_header_b;
  dst = eeprom_image + 0x28;
  n = 3;
  do {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void eeprom_save_write_copies(void)
{
  uchar b;
  int n;
  uchar *src;
  uchar *dst;

  src = eeprom_save_buffer;
  dst = eeprom_image + 0x2c;
  n = eeprom_save_padded_size;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  src = eeprom_save_buffer;
  dst = eeprom_image + 0x2c + eeprom_save_padded_size;
  n = eeprom_save_padded_size;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

void eeprom_save_store(void)
{
  uint crc;

  crc = crc16_ccitt((byte *)eeprom_save_buffer,eeprom_save_padded_size);
  *(short *)&eeprom_save_header_a = crc;
  eeprom_save_header_a.size = eeprom_save_size;
  eeprom_save_header_a.padded_size = eeprom_save_padded_size;
  *(short *)&eeprom_save_header_b = crc;
  eeprom_save_header_b.size = eeprom_save_size;
  eeprom_save_header_b.padded_size = eeprom_save_padded_size;
  eeprom_save_write_headers();
  eeprom_save_write_copies();
}

void eeprom_save_read_copy_a(int n, uchar *dst)
{
  uchar b;
  uchar *src;

  src = eeprom_image + 0x2c;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

void eeprom_save_read_copy_b(int n, uchar *dst)
{
  uchar b;
  uchar *src;

  src = eeprom_image + 0x2c + n;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

int eeprom_save_max_size(void)
{
  return 0x2a;
}

uint eeprom_save_load(uint size, uchar *buffer)
{
  uint crc;
  uint padded_size;
  uint crc_a;
  uint crc_b;
  uint r;
  int t;
  uchar *a;
  uchar *b;

  eeprom_save_ready = 0;
  padded_size = size;
  if ((size & 3) != 0) {
    padded_size = (size / 4) * 4 + 4;
  }
  if (0x80 < padded_size * 2 + 0x2c || size == 0) {
    return 0xffffffff;
  }
  eeprom_save_ready = 1;
  eeprom_save_buffer = buffer;
  eeprom_save_size = size;
  eeprom_save_padded_size = padded_size;
  eeprom_save_read_headers();
  a = (uchar *)&eeprom_save_header_a;
  b = (uchar *)&eeprom_save_header_b;
  crc_a = *(ushort *)a;
  crc_b = *(ushort *)b;
  if (!((((a[2] == size) && (b[2] == size)
       ) && (a[3] == padded_size)) &&
     (b[3] == padded_size))) {
    r = 3;
  }
  else {
    eeprom_save_read_copy_a(padded_size,buffer);
    crc = crc16_ccitt(buffer,padded_size);
    if (crc == crc_a) {
      eeprom_save_read_copy_b(padded_size,buffer);
      crc = crc16_ccitt(buffer,padded_size);
      t = crc == crc_b;
      r = t == 0;
    }
    else {
      eeprom_save_read_copy_b(padded_size,buffer);
      crc = crc16_ccitt(buffer,padded_size);
      t = crc == crc_b;
      r = t == 0;
      r += 2;
    }
  }
  switch(r) {
  case 0:
    break;
  case 1:
    eeprom_save_read_copy_a(padded_size,buffer);
    eeprom_save_store();
    break;
  case 2:
    eeprom_save_read_copy_b(padded_size,buffer);
    eeprom_save_store();
    break;
  case 3:
    eeprom_save_zero(padded_size,buffer);
    eeprom_save_store();
    break;
  }
  return r;
}
