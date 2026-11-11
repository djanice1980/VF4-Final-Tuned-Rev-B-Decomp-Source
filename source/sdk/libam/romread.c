/*
 * sdk/libam/romread.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

int cart_control_clear(void)
{
  cart_control_bits = 0;
}

int cart_control_write(int flag_0x2000, int flag_0x1000)
{
  switch (flag_0x2000) {
  case 0: cart_control_bits = 0; break;
  case 1: cart_control_bits = 0x2000; break;
  default: return 0;
  }
  switch (flag_0x1000) {
  case 0: break;
  case 1: cart_control_bits |= 0x1000; break;
  default: return 0;
  }
  cart_control_reg = cart_control_bits;
  return 1;
}

void cart_pio_read(uint offset, ushort *dest, uint bytes)
{
  cart_addr_low_reg = offset & 0xffff;
  cart_control_reg = offset >> 16 | cart_control_bits | 0x8000;
  bytes >>= 1;
  while (bytes-- != 0) {
    *dest++ = cart_data_reg;
  }
}

void cart_dma_read(uint offset, uint dest, uint bytes, int no_wait)
{
  unsigned int lvl;
  volatile uint status;

  offset &= 0xffffffe0;
  if (bytes != 0) {
    while (g1_dma_status_reg != 0) {
    }
    {
      unsigned int sr;
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      lvl = 0xf0;
      lvl &= sr;
      lvl >>= 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
    }
    cart_addr_low_reg = 0;
    cart_control_reg = cart_control_bits;
    *(volatile uint *)0xa05f7014 = bytes >> 5;
    *(volatile uint *)0xa05f7010 = offset & 0xffff;
    cart_dma_ctrl_reg = offset >> 16 | cart_control_bits | 0xa000;
    *(volatile uint *)0xa05f7404 = dest;
    *(volatile uint *)0xa05f7408 = bytes;
    *(volatile uint *)0xa05f740c = 1;
    *(volatile uint *)0xa05f7414 = 1;
    *(volatile uint *)0xa05f7418 = 1;
    {
      unsigned int t;
      __asm__ volatile ("stc sr,%0" : "=r" (t));
      t = (t & 0xffffff0f) | (lvl << 4);
      __asm__ volatile ("ldc %0,sr" : : "r" (t));
    }
    if (no_wait == 0) {
      do {
        status = g1_dma_status_reg;
      } while (status != 0);
      cart_dma_ctrl_reg = 0xc000;
    }
  }
}

bool g1_dma_ack_if_idle(void)
{
  int r;
  if (g1_dma_status_reg != 0) {
    r = 0;
  } else {
    cart_dma_ctrl_reg = 0xc000;
    r = 1;
  }
  return r;
}

void rom_crc_start(uint *state)
{
  *state = 0xdebdeb00;
  return;
}

void rom_crc_feed(uint *state, char *bytes, uint count)
{
  uint polynomial;
  uint i;
  uint bit;

  polynomial = 0x10210000;
  for (i = 0; i < count; i++) {
    *state = (*state & 0xffffff00) + *bytes++;
    for (bit = 0; bit < 8; bit++) {
      if (*state >= 0x80000000) {
        *state = (*state << 1) + polynomial;
      }
      else {
        *state = *state << 1;
      }
    }
  }
  for (bit = 0; bit < 8; bit++) {
    if (*state >= 0x80000000) {
      *state = (*state << 1) + polynomial;
    }
    else {
      *state = *state << 1;
    }
  }
}
