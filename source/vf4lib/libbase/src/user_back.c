/*
 * vf4lib/libbase/src/user_back.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int user_back_defaults_fixed; /* 0x0c2943c4 */
extern user_back_data_rec user_back_defaults; /* 0x0c2943c8 */
extern int user_back_dirty; /* 0x0c3744e0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern uint user_back_area_size; /* 0x0c3777bc */

void user_back_defaults_once(void)
{
  if (user_back_defaults_fixed == 0) {
    user_back_defaults.card_use[0] = 1;
    user_back_defaults.card_use[1] = 1;
    user_back_defaults.net_connect = 1;
    user_back_defaults_fixed = 1;
  }
  return;
}

void user_back_init(void)
{
  user_back_defaults_once();
  g1_dma_spin_idle();
  user_back_dirty = 0;
  user_back_area_size = sram_save_size_limit();
  if (user_back_area_size < sizeof(user_back_data)) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/user_back.c",0x242,"backup ram size error!!");
  }
  user_back_load_status = sram_save_load_dma_safe(sizeof(user_back_data),&user_back_data);
  if (user_back_load_status == 3) {
    user_back_reset();
  }
  if (user_back_data.format_stamp != user_back_defaults.format_stamp) {
    user_back_reset();
  }
  return;
}

void user_back_reset(void)
{
  user_back_data = user_back_defaults;
  card_log_clear();
  g1_dma_spin_idle();
  sram_save_store_dma_safe();
}

void user_back_mark_dirty(void)
{
  user_back_dirty = user_back_dirty | 1;
  return;
}

void user_back_flush(void)
{
  if ((user_back_dirty != 0) && (user_back_dirty = 0, user_back_load_status != -1)) {
    g1_dma_spin_idle();
    sram_save_store_dma_safe();
  }
  return;
}

void user_back_save_now(void)
{
  g1_dma_spin_idle();
  sram_save_store_dma_safe();
  user_back_dirty = 0;
  return;
}

void point_ranking_insert(int chara, int score, char *name)
{
  char c = chara;
  int i;
  int j;
  ranking_row_rec *e;

  for (i = 0; i < 50; i++) {
    if (score > user_back_data.rankings.point_ranking_rows[i].value) break;
  }
  if (i > 49) return;
  for (j = 48; i <= j; j--) {
    user_back_data.rankings.point_ranking_rows[j + 1] = user_back_data.rankings.point_ranking_rows[j];
  }
  e = &user_back_data.rankings.point_ranking_rows[i];
  snprintf(e->name,0x15,"%s",name);
  e->chara = c;
  e->value = score;
}

void time_ranking_insert(signed char chara, uint time, char *name)
{
  int n;
  int i;
  ranking_row_rec *e;

  n = time_ranking_slot_find(time);
  if (n != -1) {
    e = &user_back_data.rankings.time_ranking_rows[n];
    for (i = 0x30; n <= i; i--) {
      user_back_data.rankings.time_ranking_rows[i + 1] = user_back_data.rankings.time_ranking_rows[i];
    }
    snprintf(e->name,0x15,"%s",name);
    e->chara = chara;
    e->value = time;
  }
  user_back_mark_dirty();
}

int time_ranking_slot_find(uint time)
{
  int i;
  
  i = 0;
  for (; i < 0x32; i++) {
    if (time < (uint)user_back_data.rankings.time_ranking_rows[i].value) return i;
  }
  return -1;
}

void card_log_clear(void)
{
  int i;
  item_pending_entry_rec *e;
  item_pending_table_rec *table;

  table = &user_back_data.item_pending_table;
  e = table->entries;
  table->count = 0;
  table->last_card_id = 0xffffffff;
  table->last_card_hits = 0;
  for (i = 0; i < 10; i++, e++) {
    e->time = 0;
    e->card_id = 0;
    e->count = 0;
  }
}
