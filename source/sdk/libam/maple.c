/*
 * sdk/libam/maple.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern mie_start_rec mie_start_state; /* 0x0c3e12a8 */

uchar *maple_send(uint command, int port, int address, uint *data, int words)
{
  int i;
  volatile int pad;

  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port << 10 | 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port << 16 | address << 8 | command;
  for (i = 0; i < words; i++) {
    *maple_frame_cursor++ = *data++;
  }
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

void maple_send_start(uint command, int port, int address, uint *data, int words)
{
  int i;
  volatile int pad;

  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port << 10 | 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port << 16 | address << 8 | command;
  for (i = 0; i < words; i++) {
    *maple_frame_cursor++ = *data++;
  }
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  return;
}

uchar *mie_send_no_data(uint command)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = command | 0x2000;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

uchar *maple_send_no_data(uint command, int port, int address)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port << 10 | 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port << 16 | address << 8 | command;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

uchar *maple_port0_reset(void)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000300;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = 0x2001;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

uchar *maple_port_reset(int port_bits, int address)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port_bits << 10 | 0x80000300;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port_bits << 16 | address << 8 | 1;
    *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

void maple_hw_init(void)
{
  int reply_addr;
  int frame_addr;
  reply_addr = ((int)((char *)&sram_bank0 + 4) & -32) + 32;
  maple_reply_addr = reply_addr;
  frame_addr = ((int)((char *)&mie_start_state + 0x4c) & -32) + 32;
  maple_frame_addr = frame_addr;
  maple_reply = (uchar *)(reply_addr & 0x0fffffff | 0xa0000000);
  maple_frame = (uchar *)(frame_addr & 0x0fffffff | 0xa0000000);
  maple_frame_cursor = (uint *)(frame_addr & 0x0fffffff | 0xa0000000);
  *(volatile uint *)0xffa00020 = 0;
  *(volatile uint *)0xffa0002c = 0x12c1;
  *(volatile uint *)0xa05f6c14 = 0;
  *(volatile uint *)0xa05f6c8c = 0x6155407f;
  *(volatile uint *)0xa05f6c80 = 0x3a980000;
  *(volatile uint *)0xa05f6c10 = 0;
  *(volatile uint *)0xa05f6c04 = frame_addr;
  *(volatile uint *)0xa05f6c18 = 0;
}

int mie_wait_answer(void)
{
  int tries;
  volatile int delay;

  for (tries = 99; tries != -1; tries--) {
    for (delay = 10000; delay-- != 0; ) ;
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(int *)maple_reply = -1;
    maple_frame_cursor = (uint *)maple_frame;
    *maple_frame_cursor++ = 0x80000000;
    *maple_frame_cursor++ = maple_reply_addr;
    *maple_frame_cursor++ = 0x2003;
    *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
    *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile uint *)0xa05f6c04 = maple_frame_addr;
    *(volatile int *)0xa05f6c14 = 1;
    *(volatile int *)0xa05f6c18 = 1;
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile int *)0xa05f6c14 = 0;
    if (*(int *)maple_reply != -1) return 0;
  }
  return 1;
}

void maple_set_trigger(int trigger)
{
  while (*(volatile int *)0xa05f6c18 != 0) {
  }
  switch (trigger) {
  case 0:
    *(volatile int *)0xa05f6c10 = 0;
    *(volatile int *)0xa05f6c80 = 0x3a980000;
    break;
  case 1:
    *(volatile int *)0xa05f6c10 = 1;
    *(volatile int *)0xa05f6c80 = 0x3a980002;
    break;
  }
}

uchar * mie_send_cmd_01(void)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = 0x2001;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return maple_reply;
}

int mie_read_ident(uchar *text)
{
  int reply;
  int expected;
  char *k;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = 0x2082;
    *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  if (*(int *)maple_reply != -1) {
    reply = (int)maple_reply;
    for (k = 0; (int)k < 0x38; k++) {
      expected = (int)mie_ident_expected;
      if (*(char *)(reply + (int)k) != *(char *)(expected + (int)k)) return 1;
    }
  }
  reply = (int)maple_reply + 4;
  for (k = 0; (int)k < 0x1c; k++) *text++ = *(char *)reply++;
  reply += 4;
  for (k = (char *)0x24; (int)k < 0x40; k++) *text++ = *(char *)reply++;
  return 0;
}

int maple_unit_read_ident(uchar *text, int port, int address)
{
  int i;
  uchar *reply;

  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port << 10 | 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port << 16 | address << 8 | 0x82;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  if (*(int *)maple_reply != -1) {
    reply = maple_reply;
    for (i = 0; i < 0x38; i++) {
      if (((char *)reply)[i] != mie_ident_expected[i]) {
        return 1;
      }
    }
  }
  else {
    return 1;
  }
  reply = maple_reply + 4;
  for (i = 0; i < 0x1c; i++) {
    *text++ = *reply++;
  }
  reply += 4;
  for (i = 0x24; i < 0x40; i++) {
    *text++ = *reply++;
  }
  return 0;
}

int mie_send_cmd_84(void)
{
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = 0x2084;
  *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return 0;
}

int mie_upload_firmware(void)
{
  int n;
  uint load_addr;
  uint *src;
  char *cp;
  uint *q;
  int sum;
  int j;
  uint lo, hi;
  volatile int delay;

  n = (int)&maple_dma_end - (int)mie_firmware;
  load_addr = 0x8010;
  src = mie_firmware;
  cp = (char *)mie_firmware;
  for (; n > 0; n -= 0x18) {
    delay = 10000;
    do {
      delay = delay - 1;
    } while (delay != -1);
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(int *)maple_reply = -1;
    maple_frame_cursor = (uint *)maple_frame;
    *maple_frame_cursor++ = 0x80000000;
    *maple_frame_cursor++ = maple_reply_addr;
    *maple_frame_cursor++ = 0x2080;
    *maple_frame_cursor++ = (((load_addr >> 8) & 0xff) << 16) + (load_addr << 24);
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
        *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
    *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile uint *)0xa05f6c04 = maple_frame_addr;
    *(volatile int *)0xa05f6c14 = 1;
    *(volatile int *)0xa05f6c18 = 1;
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile int *)0xa05f6c14 = 0;
    sum = 0;
    for (j = 0; j < 0x18; j++) {
      sum += *cp++;
    }
    sum += load_addr & 0xff;
    sum += (load_addr & 0xff00) >> 8;
    load_addr += 0x18;
    if (*(int *)maple_reply == -1) {
      return 3;
    }
    if ((*(uint *)(maple_reply + 4) & 0xff) != (sum & 0xff)) {
      return 4;
    }
  }
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = 0x2080;
  *maple_frame_cursor++ = 0xffffffff;
    *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return 0;
}

int maple_unit_upload_firmware(int port, int address)
{
  int n;
  uint load_addr;
  uint *src;
  char *cp;
  int sum;
  int j;
  volatile int delay;

  n = (int)&maple_dma2_end - (int)card_unit_firmware;
  load_addr = 0x8000;
  src = card_unit_firmware;
  cp = (char *)card_unit_firmware;
  for (; n > 0; n -= 0x18) {
    delay = 10000;
    do {
      delay = delay - 1;
    } while (delay != -1);
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(int *)maple_reply = -1;
    maple_frame_cursor = (uint *)maple_frame;
    *maple_frame_cursor++ = port << 10 | 0x80000000;
    *maple_frame_cursor++ = maple_reply_addr;
    *maple_frame_cursor++ = port << 16 | address << 8 | 0x80;
    *maple_frame_cursor++ = (((load_addr >> 8) & 0xff) << 16) + (load_addr << 24);
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
    *maple_frame_cursor++ = *src++;
        *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
    *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile uint *)0xa05f6c04 = maple_frame_addr;
    *(volatile int *)0xa05f6c14 = 1;
    *(volatile int *)0xa05f6c18 = 1;
    while (*(volatile int *)0xa05f6c18 != 0) ;
    *(volatile int *)0xa05f6c14 = 0;
    sum = 0;
    for (j = 0; j < 0x18; j++) {
      sum += *cp++;
    }
    sum += load_addr & 0xff;
    sum += (load_addr & 0xff00) >> 8;
    load_addr += 0x18;
    if (*(int *)maple_reply == -1) {
      return 3;
    }
    if ((*(uint *)(maple_reply + 4) & 0xff) != (sum & 0xff)) {
      return 4;
    }
  }
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(int *)maple_reply = -1;
  maple_frame_cursor = (uint *)maple_frame;
  *maple_frame_cursor++ = port << 10 | 0x80000000;
  *maple_frame_cursor++ = maple_reply_addr;
  *maple_frame_cursor++ = port << 16 | address << 8 | 0x80;
  *maple_frame_cursor++ = 0xffffffff;
    *(uint *)maple_frame = ((int)maple_frame_cursor - (int)maple_frame >> 2) - 3U | *(uint *)maple_frame;
  *(uint *)(maple_frame + 8) = *(int *)maple_frame << 0x18 | *(uint *)(maple_frame + 8);
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile uint *)0xa05f6c04 = maple_frame_addr;
  *(volatile int *)0xa05f6c14 = 1;
  *(volatile int *)0xa05f6c18 = 1;
  while (*(volatile int *)0xa05f6c18 != 0) ;
  *(volatile int *)0xa05f6c14 = 0;
  return 0;
}
