/*
 * sdk/libam/nlasrvdt.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"
#define BIOS_HELPER_CODE ((uint *)0xac018000)
#define BIOS_HELPER_ROM ((uint *)0xa0060000)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void bios_helpers_copy(void)
{
  uint *dst;
  uint *src;
  uint key;
  uint code_addr;
  int i;
  int n;
  bios_helper_table = (uint)BIOS_HELPER_CODE;
  code_addr = (uint) (BIOS_HELPER_CODE);
  src = BIOS_HELPER_ROM;
  key = code_addr & 0xfff0000;
  for (i = 0; i < 8; i++)
  {
    if (((*(src++)) & 0xfff0000) != key)
    {
      return;
    }
  }

  src = BIOS_HELPER_ROM;
  dst = BIOS_HELPER_CODE;
  for (n = 0x1bff; n != (-1); n--)
  {
    if (n)
    {
      if (key)
      {
        *(dst++) = *(src++);
      }
      else
      {
        *(dst++) = *(src++);
      }
    }
    else
    {
      *(dst++) = *(src++);
    }
  }

}

void bios_helpers_install(uint *table)
{
  uint *dst;
  int n;

  bios_helper_table = (uint)BIOS_HELPER_CODE;
  dst = BIOS_HELPER_CODE;
  for (n = 0x1bff; n != -1; n--) {
    *dst++ = *table++;
  }
}

int bios_coin_config_check(coin_settings_rec *settings)
{
  int result;
  
  result = (**(code **)(bios_helper_table + 0x10))();
  return result;
}

int bios_coin_setting(int setting)
{
  int result;
  
  result = (**(code **)(bios_helper_table + 0x14))();
  return result;
}

void bios_credit_init(coin_settings_rec *settings, uchar *state, uint *ledger, uchar *status)
{
  (**(void (**)())(bios_helper_table + 0x18))();
}

int bios_helper_34(uint bank_flag)
{
  int result;
  
  if (*(code **)(bios_helper_table + 0x34) != (code *)0x0) {
    return (**(code **)(bios_helper_table + 0x34))();
  }
  return 0;
}

int bios_ledger_check(uint max_credit, uint *ledger)
{
  int result;
  
  result = (**(code **)(bios_helper_table + 0x1c))();
  return result;
}

void bios_credit_refresh(coin_settings_rec *settings, uchar *state, uchar *status)
{
  (*(void (*)())*(int *)(bios_helper_table + 0x20))();
}

int bios_coin_frame(coin_settings_rec *settings, uchar *state, uchar *status, uint coin_switches, uint *coins, uint *ledger)
{
  code *f = *(code **)(bios_helper_table + 0x24);
  (*f)(settings,state,status,coin_switches,coins,ledger);
}

void bios_helper_28(board_settings_rec *board)
{
  (**(void (**)())(bios_helper_table + 0x28))();
}

void bios_helper_2c(coin_settings_rec *settings)
{
  (*(void (**)())(bios_helper_table + 0x2c))();
}

int bios_helper_08(void)
{
/* ORIGINAL SHAPE (corroborated 2026-11-10: bios_coin_frame above has the same shape, declared int with no return statement; warning_init in libbase/mode.c uses this routine's result, so it is declared to return one; a body of `return (*f)();` does not match): the BIOS entry's r0 is left in place as the result */
  (*(void (*)(void))(*(int *)(bios_helper_table + 8) + -0x60000000))();
}

void bios_helper_0c(void)
{
  (*(void (*)(void))(*(int *)(bios_helper_table + 0xc) + -0x60000000))();
}

void bios_helper_40(void)
{
  (*(void (*)(void))(*(int *)(bios_helper_table + 0x40) + -0x60000000))();
}

void bios_helper_44(void)
{
  int idle;

  do {
    idle = g1_dma_ack_if_idle();
  } while (idle == 0);
  ((void (*)(void))(*(int *)(bios_helper_table + 0x44) + -0x60000000))();
}

void bios_helper_48(void)
{
  (*(void (*)())(*(int *)(bios_helper_table + 0x48) + -0x60000000))();
  return;
}

int bios_helper_04(void)
{
  int p;

  p = *(int *)(bios_helper_table + 4);
  if (p == 0) return 0;
  p += 0xa0000000;
  ((void (*)(void))p)();
  return 1;
}

int bios_helper_get(int index)
{
  return ((int *)bios_helper_table)[index];
}

void bios_helper_4c(void)
{
  (*(void (*)(void))(*(int *)(bios_helper_table + 0x4c) + -0x60000000))();
}

void bios_helper_30(void)
{
  (*(void (*)())(*(int *)(bios_helper_table + 0x30) + -0x60000000))();
}

int bios_helper_50(int arg)
{
  int result;
  
  if (*(code **)(bios_helper_table + 0x50) != (code *)0x0) {
    return (**(code **)(0x50 + bios_helper_table))();
  }
  return 0xffffffff;
}

int bios_helper_54(void)
{
  int result;
  
  if (*(code **)(bios_helper_table + 0x54) != (code *)0x0) {
    return (**(code **)(bios_helper_table + 0x54))();
  }
  return 0;
}

void bios_helper_58(uint arg0, uint arg1)
{
  if (*(code **)(bios_helper_table + 0x58) != (code *)0x0) {
    (**(code **)(bios_helper_table + 0x58))();
  }
  return;
}

void bios_helper_5c(void)
{
  if (*(code **)(bios_helper_table + 0x5c) != (code *)0x0) {
    (**(code **)(bios_helper_table + 0x5c))();
  }
  return;
}

int bios_helper_60(int arg)
{
  int result;
  
  if (*(code **)(bios_helper_table + 0x60) != (code *)0x0) {
    return (**(code **)(bios_helper_table + 0x60))();
  }
  return 0;
}
