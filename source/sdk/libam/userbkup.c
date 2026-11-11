/*
 * sdk/libam/userbkup.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"
#define SRAM_SAVE_COPIES ((uchar *)0xa0200218)
#define SRAM_SAVE_HEADER_B_AREA ((uint *)0xa0200208)
#define SRAM_SAVE_HEADER_A_AREA ((uint *)0xa02001f8)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern sram_save_header_a_rec sram_save_header_a; /* 0x0c5caf80 */
extern sram_save_header_b_rec sram_save_header_b; /* 0x0c5caf90 */

void sram_save_zero(int n, uchar *dst)
{
  while (n = n + -1, n != -1) {
    *dst = 0;
    dst = dst + 1;
  }
  return;
}

void sram_save_read_headers(void)
{
  int n;
  uint w;
  sram_save_header_a_rec *dst_a;
  sram_save_header_b_rec *dst_b;
  uint *src;

  src = SRAM_SAVE_HEADER_A_AREA;
  dst_a = &sram_save_header_a;
  n = 3;
  do {
    w = *src;
    src = src + 1;
    dst_a->crc = w;
    dst_a = (sram_save_header_a_rec *)&dst_a->size;
    n = n + -1;
  } while (n != -1);
  src = SRAM_SAVE_HEADER_B_AREA;
  dst_b = &sram_save_header_b;
  n = 3;
  do {
    w = *src;
    src = src + 1;
    dst_b->crc = w;
    dst_b = (sram_save_header_b_rec *)&dst_b->size;
    n = n + -1;
  } while (n != -1);
  return;
}

void sram_save_write_headers(void)
{
  int n;
  uint *dst;
  uint *src;

  src = (uint *)&sram_save_header_a;
  dst = SRAM_SAVE_HEADER_A_AREA;
  n = 3;
  do {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  src = (uint *)&sram_save_header_b;
  dst = SRAM_SAVE_HEADER_B_AREA;
  n = 3;
  do {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void sram_save_write_header_a(void)
{
  uint *word;
  int n;
  uint *dst;
  sram_save_header_a_rec *src;

  src = &sram_save_header_a;
  dst = SRAM_SAVE_HEADER_A_AREA;
  n = 3;
  do {
    word = &src->crc;
    *dst = *word;
    src = (sram_save_header_a_rec *)&src->size;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void sram_save_write_header_b(void)
{
  uint *word;
  int n;
  uint *dst;
  sram_save_header_b_rec *src;

  src = &sram_save_header_b;
  dst = SRAM_SAVE_HEADER_B_AREA;
  n = 3;
  do {
    word = &src->crc;
    *dst = *word;
    src = (sram_save_header_b_rec *)&src->size;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void sram_save_write_copies(void)
{
  uint n;
  uint w;
  uint *dst;
  uint *src;

  src = (uint *)sram_save_buffer;
  dst = (uint *)SRAM_SAVE_COPIES;
  n = sram_save_padded_size >> 2;
  while (n = n - 1, n != 0xffffffff) {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
  }
  src = (uint *)sram_save_buffer;
  dst = (uint *)(SRAM_SAVE_COPIES + sram_save_padded_size);
  n = sram_save_padded_size >> 2;
  while (n = n - 1, n != 0xffffffff) {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
  }
  return;
}

void sram_save_write_copy_a(void)
{
  uint n;
  uint w;
  uint *dst;
  uint *src;

  src = (uint *)sram_save_buffer;
  dst = (uint *)SRAM_SAVE_COPIES;
  n = sram_save_padded_size >> 2;
  while (n = n - 1, n != 0xffffffff) {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
  }
  return;
}

void sram_save_write_copy_b(void)
{
  uint n;
  uint w;
  uint *dst;
  uint *src;

  src = (uint *)sram_save_buffer;
  dst = (uint *)(SRAM_SAVE_COPIES + sram_save_padded_size);
  n = sram_save_padded_size >> 2;
  while (n = n - 1, n != 0xffffffff) {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
  }
  return;
}

void sram_save_write_range(int offset, int bytes)
{
  uchar b;
  uchar *src;
  uchar *dst;
  int n;

  n = bytes;
  src = sram_save_buffer + offset;
  dst = SRAM_SAVE_COPIES + offset;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  n = bytes;
  src = sram_save_buffer + offset;
  dst = SRAM_SAVE_COPIES + offset + sram_save_padded_size;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

void sram_save_write_range_a(int offset, int n)
{
  uchar b;
  uchar *src;
  uchar *dst;

  src = sram_save_buffer + offset;
  dst = SRAM_SAVE_COPIES + offset;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

void sram_save_write_range_b(int offset, int n)
{
  uchar b;
  uchar *src;
  uchar *dst;

  src = sram_save_buffer + offset;
  dst = SRAM_SAVE_COPIES + offset + sram_save_padded_size;
  while (n = n + -1, n != -1) {
    b = *src;
    src = src + 1;
    *dst = b;
    dst = dst + 1;
  }
  return;
}

void sram_save_store(void)
{
  uint crc;
  crc = crc16_ccitt(sram_save_buffer,sram_save_padded_size);
  sram_save_header_a.crc = crc;
  sram_save_header_a.size = sram_save_size;
  sram_save_header_a.padded_size = sram_save_padded_size;
  sram_save_header_a.field_0xc = 0;
  sram_save_header_b.crc = sram_save_header_a.crc;
  sram_save_header_b.size = sram_save_header_a.size;
  sram_save_header_b.padded_size = sram_save_header_a.padded_size;
  sram_save_header_b.field_0xc = sram_save_header_a.field_0xc;
  sram_save_write_header_a();
  sram_save_write_copy_a();
  sram_save_write_header_b();
  sram_save_write_copy_b();
  return;
}

void sram_save_store_range(int offset, int n)
{
  uint crc;

  crc = crc16_ccitt(sram_save_buffer,sram_save_padded_size);
  sram_save_header_a.crc = crc;
  sram_save_header_a.size = sram_save_size;
  sram_save_header_a.padded_size = sram_save_padded_size;
  sram_save_header_a.field_0xc = 0;
  *(sram_save_header_a_rec *)&sram_save_header_b = sram_save_header_a;
  sram_save_write_header_a();
  sram_save_write_range_a(offset,n);
  sram_save_write_header_b();
  sram_save_write_range_b(offset,n);
}

void sram_save_read_copy_a(uint size, uint *dst)
{
  uint *s = (uint *)SRAM_SAVE_COPIES; size >>= 2; while (size--) *dst++ = *s++;
}

void sram_save_read_copy_b(uint size, uint *dst)
{
  uint n;
  uint *src;
  n = size;
  src = (uint *)(SRAM_SAVE_COPIES + size);
  for (n >>= 2; n--; ) {
    *dst++ = *src++;
  }
}

int sram_save_is_ready(void)
{
  int none;

  if (sram_save_ready == 0) none = 1; else none = 0;
  return !none;
}

int sram_save_max_size(void)
{
  return 0x3ef4;
}

uint sram_save_load(uint size, uchar *buffer)
{
  uint padded_size;
  uint r;
  uint crc_a;
  uint crc_b;
  int t;
  uint crc;
  sram_save_header_a_rec *header_a;
  sram_save_header_b_rec *header_b;
  sram_save_ready = 0;
  padded_size = size;
  if ((size & 3) != 0) {
    padded_size = size >> 2;
    padded_size = padded_size * 4 + 4;
  }
  if (0x8000 < padded_size * 2 + 0x218 || size == 0) {
    return 0xffffffff;
  }
  sram_save_ready = 1;
  sram_save_buffer = buffer;
  sram_save_size = size;
  sram_save_padded_size = padded_size;
  sram_save_read_headers();
  header_a = &sram_save_header_a;
  header_b = &sram_save_header_b;
  crc_a = (ushort)header_a->crc;
  crc_b = (ushort)header_b->crc;
  if (header_a->size != size || header_b->size != size ||
      header_a->padded_size != padded_size || header_b->padded_size != padded_size) {
    r = 3;
  }
  else {
    sram_save_read_copy_a(padded_size,(uint *)buffer);
    if (crc16_ccitt((byte *)buffer,padded_size) == crc_a) {
      sram_save_read_copy_b(padded_size,(uint *)buffer);
      crc = crc16_ccitt((byte *)buffer,padded_size);
      t = crc == crc_b;
      r = t == 0;
    }
    else {
      sram_save_read_copy_b(padded_size,(uint *)buffer);
      crc = crc16_ccitt((byte *)buffer,padded_size);
      t = crc == crc_b;
      r = t == 0;
      r += 2;
    }
  }
  switch (r) {
  case 1:
    sram_save_read_copy_a(padded_size,(uint *)buffer);
    break;
  case 2:
    sram_save_read_copy_b(padded_size,(uint *)buffer);
    break;
  case 3:
    sram_save_zero(padded_size,buffer);
    break;
  case 0:
  default:
    return r;
  }
  sram_save_store();
  return r;
}
