/*
 * sdk/nlajamma/credit.c
 * nlajamma: coin and credit counting.
 * Reconstructed C: every function here matches the game byte for byte
 * (python3 -I tools/match/match.py checks the whole tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 coin_input_held; /* 0x0c3e1d44 */
extern undefined1 legacy_io_meter_bits; /* 0x0c3e2b94 */
extern int io_service_buttons_2; /* 0x0c3f169c */
extern int io_service_buttons; /* 0x0c3f16a0 */
extern int coin_meter_timers[4]; /* 0x0c5caff0 */
extern int coins_pending[4]; /* 0x0c5cb000 */

void coin_state_clear(void)
{
    int i;

    io_service_buttons = 0;
    io_service_buttons_2 = 0;
    for (i = 0; i < 4; i++) {
        coins_pending[i] = 0;
        coin_meter_timers[i] = 0;
    }
}

void backup_flush(void)
{
  long busy;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  
  if (coin_input_held == 0) {
    if ((credit_backup_pending != 0) && (busy = rom_dma_busy(), busy == 0)) {
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      lvl = (sr & 0xf0) >> 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
      credit_backup_update();
      credit_backup_pending = 0;
      __asm__ volatile ("stc sr,%0" : "=r" (t));
      t = (t & 0xffffff0f) | (lvl << 4);
      __asm__ volatile ("ldc %0,sr" : : "r" (t));
    }
    uptime_save_tick = uptime_save_tick + 1;
    if ((uptime_save_tick & 0xf) == 0) {
      busy = rom_dma_busy();
      if (busy == 0) {
        uptime_save();
      }
      else {
        uptime_save_tick = 0xf;
      }
    }
  }
  return;
}

void frame_io_update(void)
{
  long state;
  int pending;
  int i;
  uint coins[4];
  uint coins_2[4];
  
  state = io_ready_check();
  if (state != 1) {
    for (i = 0; i < 4; i++) {
      coins[i] = 0;
      coins_2[i] = 0;
    }
    if (jvs_node_count == 0) {
      legacy_io_poll(0,&io_service_buttons,(int *)coins);
      coins_pending[0] = coins_pending[0] + coins[0];
    }
    else if (jvs_node_count == 1) {
      jvs_read_node(1,&io_service_buttons,(int *)coins);
      coins_pending[0] = coins_pending[0] + coins[0];
      coins_pending[1] = coins_pending[1] + coins[1];
    }
    else if (jvs_node_count == 2) {
      jvs_read_two_nodes(2,&io_service_buttons,(int *)coins,1,&io_service_buttons_2,(int *)coins_2);
      { int t = io_service_buttons & 0x10;
      io_service_buttons_2 = io_service_buttons_2 & 0x10;
      io_service_buttons = io_service_buttons_2 * 2 | t; }
      coins[1] = coins_2[0];
      coins[2] = 0;
      coins[3] = 0;
      coins_pending[0] = coins_pending[0] + coins[0];
      coins_pending[1] = coins_pending[1] + coins_2[0];
    }
    else {
      jvs_read_node(jvs_main_node,&io_service_buttons,(int *)coins);
      coins_pending[0] = coins_pending[0] + coins[0];
      coins_pending[1] = coins_pending[1] + coins[1];
    }
    if (coin_input_held == 0) {
      for (i = 0; i < 4; i++) {
        pending = coins_pending[i];
        if (pending != 0) {
          coins[i] = 1;
          coins_pending[i] = pending + -1;
        }
      }
      credit_backup_pending = credit_backup_pending + coin_input_update(io_service_buttons,coins);
      legacy_io_meters();
    }
    frame_io_done();
  }
}

void legacy_io_meters(void)
{
  uint *t;
  int i;

  t = coin_meter_table();
  legacy_io_meter_bits = 0;
  for (i = 0; i < 4; i++) {
    if (coin_meter_timers[i] > 6) {
      legacy_io_meter_bits |= 1 << i;
      coin_meter_timers[i]--;
    }
    else if (coin_meter_timers[i] > 0) {
      coin_meter_timers[i]--;
      t[i] = 0;
    }
    else if (t[i] != 0) {
      coin_meter_timers[i] = 12;
    }
  }
}
