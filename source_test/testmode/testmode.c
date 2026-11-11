/*
 * testmode/testmode.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void menu_items_draw(menu_item_rec *items, uint n)
{
  uint i;

  for (i = 0; i < n; i++, items++) {
    text_cursor_to_cell(items->x, items->y);
    text_draw_formatted(" %s\n", items->text);
  }
}

void text_put_at(int x, int y, char *s)
{
  text_cursor_to_cell(x, y);
  text_draw_string(s);
}

void test_frame_finish(void)
{
  elan_frame_finish();
  frame_io_update();
  card_units_poll();
}

void text_size_16(void)
{
  text_cell_size_set(0x10, 0x10);
}

void text_size_8(void)
{
  text_cell_size_set(8, 8);
}

void test_menu_start(void)
{
  test_cursor = 0xb;
  bg_plane_corner_colors(0, 0, 0);
  coin_input_hold();
  text_cell_size_set(0x10, 0x10);
  text_color_set(0xffffffff);
  test_mode = 0;
  test_phase = 0;
}

void test_screen_run(void)
{
  test_screen_call(test_mode, test_phase);
}

int menu_cursor_step(int count)
{
  if (input_merged_pressed(0x20101008)) {
    if (test_cursor < count - 1)
      test_cursor++;
    else
      test_cursor = 0;
  }
  if (input_merged_pressed(0x00202000)) {
    if (test_cursor > 0)
      test_cursor--;
    else
      test_cursor = count - 1;
  }
  return test_cursor;
}

void menu_draw_with_cursor(uint cursor, menu_item_rec *items, uint n)
{
  uint i;

  for (i = 0; i < n; i++) {
    if (cursor == i) {
      menu_item_rec *p = &items[i];
      text_cursor_to_cell(p->x - 1, p->y);
      text_draw_formatted("%c", 4);
    }
  }
  menu_items_draw(items, n);
}

void test_screen_call(int mode, int phase)
{
  ((void (*)(void))test_screen_table[mode][phase])();
}

void input_test_rows_draw(player_input_rec *input_1p, player_input_rec *input_2p, input_test_row_rec *rows, uint n)
{
  uint i;

  for (i = 0; i < n; i++, rows++) {
    text_draw_formatted(rows->format,
                        on_off_text((1 << rows->bit) & input_1p->now),
                        on_off_text((1 << rows->bit) & input_2p->now));
  }
}

void game_assignments(void)
{
  uint cur;
  int n = 17;
  char v;

  text_cursor_to_cell(0xc,1);
  text_draw_formatted("GAME ASSIGNMENTS");
  menu_hint_draw();
  if (net_log_unlocked == 1) n = 17; /* ORIGINAL SHAPE (corroborated 2026-11-10: network_test in this file chooses its item count the same way, n = 2 or 4 when unlocked, and there the form is forced): the item count chosen by net_log_unlocked; here both counts are 17, which keeps 17 in r8 across both menu calls as the game does */
  cur = menu_cursor_step(n);
  menu_draw_with_cursor(cur,assignment_menu_items,n);
  if (input_merged_pressed(0x100f0f04) != 0) {
    switch (cur) {
    case 0:
      v = ++enemy_level;
      if (v > 4) enemy_level = 0;
      break;
    case 1:
      v = ++energy_max_1p;
      if (v > 3) energy_max_1p = 0;
      break;
    case 2:
      v = ++energy_max_vs;
      if (v > 3) energy_max_vs = 0;
      break;
    case 3:
      v = ++match_count_1p;
      if (v > 5) match_count_1p = 2;
      break;
    case 4:
      v = ++match_count_vs;
      if (v > 5) match_count_vs = 2;
      break;
    case 5:
      v = ++time_limit;
      if (v > 2) time_limit = 0;
      break;
    case 6:
      v = ++stage_select;
      if (v > 1) stage_select = 0;
      break;
    case 7:
      v = ++continue_setting;
      if (v > 1) continue_setting = 0;
      break;
    case 8:
      v = ++vs_finish;
      if (v > 10) vs_finish = 0;
      break;
    case 9:
      v = ++network_on;
      if (v > 1) network_on = 0;
      break;
    case 10:
      v = ++advertise_connect;
      if (v > 1) advertise_connect = 0;
      break;
    case 11:
      v = ++card_use[0];
      if (v > 1) card_use[0] = 0;
      break;
    case 12:
      v = ++card_use[1];
      if (v > 1) card_use[1] = 0;
      break;
    case 13:
      v = ++card_start;
      if (v > 1) card_start = 0;
      break;
    case 14:
      if (++effect_skip > 1) effect_skip = 0;
      break;
    case 15:
      enemy_level = 2;
      energy_max_1p = 1;
      energy_max_vs = 1;
      match_count_1p = 2;
      match_count_vs = 2;
      time_limit = 0;
      stage_select = 0;
      continue_setting = 0;
      vs_finish = 0;
      challenge_only = 0;
      network_on = 1;
      advertise_connect = 1;
      card_use[0] = 1;
      card_use[1] = 1;
      card_start = 0;
      effect_skip = 1;
      break;
    case 16:
      test_phase = 2;
      break;
    }
  }
  user_back_data.rank_difficult = enemy_level;
  user_back_data.energy_1p = energy_max_1p;
  user_back_data.energy_vs = energy_max_vs;
  user_back_data._1p_match_point = match_count_1p;
  user_back_data.vs_match_point = match_count_vs;
  user_back_data.game_time_1p = time_limit;
  user_back_data.game_time_vs = time_limit;
  user_back_data.stage_select = stage_select;
  user_back_data.no_continue = continue_setting;
  user_back_data.win_streak_limit = vs_finish;
  user_back_data.challenge_only = challenge_only;
  user_back_data.button_type = button_type;
  user_back_data.net_connect = network_on;
  user_back_data.card_use[0] = card_use[0];
  user_back_data.card_use[1] = card_use[1];
  user_back_data.card_start = card_start;
  user_back_data.advertise_connect = advertise_connect;
  user_back_data.effect_skip = effect_skip;
  difficulty_text();
}

void game_assignments_exit(void)
{
  sram_save_store_dma_safe();
  test_cursor = 1;
  test_mode = 0;
  test_phase = 1;
}

void backup_clear_done(void)
{
  menu_items_draw(backup_clear_done_items,3);
  if (input_merged_pressed(0x100f0f04) != 0) {
    test_phase = 2;
  }
}

void test_menu_exit_to_bios(void)
{
  net_shutdown();
  g1_bus_hold();
  elan_regs_quiesce();
  bios_helper_44();
}

int frames_to_time_part(uint frames, int part)
{
  int r;

  r = 0;
  switch (part) {
  case 0:
    r = (uint)(frames / 5166690.0f);
    break;
  case 1:
    r = (uint)(frames / 215278.75f) % 24;
    break;
  case 2:
    r = (uint)(frames / 3587.979f) % 60;
    break;
  case 3:
    r = (uint)(frames / 59.799652f) % 60;
    break;
  case 4:
    r = (uint)((frames % 59 * 100) / 59.799652f);
    break;
  }
  return r;
}

void game_bookkeeping(void)
{
  text_cursor_to_cell(10,1);
  text_draw_formatted("GAME BOOKKEEPING %d/%d",test_page + 1,bookkeep_page_count);
  text_cursor_to_cell(7,27);
  if (test_page == bookkeep_page_count - 1) {
    text_draw_formatted("PRESS TEST BUTTON TO EXIT\n");
  }
  else {
    text_draw_formatted("PRESS TEST BUTTON TO CONTINUE");
  }
  if (input_merged_pressed(0x100f0f04)) {
    if (++test_page >= bookkeep_page_count) {
      test_page = 0;
      test_cursor = 9;
      test_mode = 0;
      test_phase = 1;
      return;
    }
  }
  switch (test_page) {
  case 0:
    bookkeep_play_totals();
    break;
  case 1:
    character_usage();
    break;
  case 2:
    stage_usage();
    break;
  case 3:
    bookkeep_daily_coins();
    break;
  case 4:
    bookkeep_billing_info();
    break;
  case 5:
    challenge_hiscore();
    break;
  case 6:
    challenge_total_bookkeep();
    break;
  case 7:
    stage_ringout_ratio();
    break;
  case 8:
    other_data();
    break;
  case 9:
    mission_data();
    break;
  }
}

void frames_to_min_sec_centi(uint frames, uint *min, int *sec, uint *centi)
{
  *min = frames / 3600;
  *sec = frames / 60 % 60;
  *centi = frames % 60 * 100 / 60;
}

void bookkeep_billing_info(void)
{
  alab_billing_rec *b = &user_back_data.billing;

  text_cursor_to_cell(8, 8);
  text_draw_formatted("BILLING INFO\n");
  text_draw_formatted("\n");
  text_draw_formatted("1P START    %d\n", b->starts[0]);
  text_draw_formatted("   CONTINUE %d\n", b->continues[0]);
  text_draw_formatted("   END      %d\n", b->ends[0]);
  text_draw_formatted("\n");
  text_draw_formatted("2P START    %d\n", b->starts[1]);
  text_draw_formatted("   CONTINUE %d\n", b->continues[1]);
  text_draw_formatted("   END      %d\n", b->ends[1]);
}

void mission_data(void)
{
  int i;

  text_cursor_to_cell(0xf, 4);
  text_draw_formatted("MISSION DATA");
  for (i = 0; i < 15; i++) {
    text_cursor_to_cell(3, i + 6);
    text_draw_formatted("NO %2d = %d/%d\n", i, user_back_data.mission_cleared[i], user_back_data.mission_offered[i]);
  }
  for (i = 15; i < 30; i++) {
    text_cursor_to_cell(0x16, i - 9);
    text_draw_formatted("NO %2d = %d/%d\n", i, user_back_data.mission_cleared[i], user_back_data.mission_offered[i]);
  }
}

void other_data(void)
{
  int i;

  text_cursor_to_cell(15, 4);
  text_draw_formatted("OTHER DATA");
  text_cursor_to_cell(3, 6);
  text_draw_formatted("TOUCH  %d LV10  %d\n", user_back_data.jisaku_no_touch_matches, user_back_data.jisaku_no_touch_high_matches);
  text_draw_formatted("LEVER  %d LV10  %d\n", user_back_data.jisaku_no_lever_matches, user_back_data.jisaku_no_lever_high_matches);
  text_draw_formatted("BUTTON %d LV10  %d\n", user_back_data.jisaku_no_g_matches, user_back_data.jisaku_no_g_high_matches);
  text_draw_formatted("\n");
  text_draw_formatted("PRE_HIT    %d\n", user_back_data.lottery_starts);
  text_draw_formatted("UWANOSE    %d\n", user_back_data.lottery_spins);
  text_draw_formatted("\n");
  text_draw_formatted("MACINE_HIT %d\n", user_back_data.bonus_time_starts);
  text_draw_formatted("MACINE_CNT %d\n", user_back_data.bonus_time_games);
  text_draw_formatted("MACINE_MIN %d\n", user_back_data.bonus_time_shortest);
  text_draw_formatted("MACINE_MAX %d\n", user_back_data.bonus_time_longest);
  text_draw_formatted("\n");
  text_draw_formatted("USER_HIT   %d\n", user_back_data.syuchu_marks);
  text_draw_formatted("\n");
  text_draw_formatted("ALL_SDT_CNT  %d\n", user_back_data.prize_matches);
  text_draw_formatted("SDT_ITEM_GET %d\n", user_back_data.prize_items_given);
  text_draw_formatted("SDT_MACINE   %d\n", user_back_data.prize_matches_bonus_time);
  text_draw_formatted("SDT_USER     %d\n", user_back_data.prize_matches_syuchu);
  text_draw_formatted("SDT_ITEM     %d\n", user_back_data.prize_matches_kind3);
  text_draw_formatted("SDT_NORMAL   %d\n", user_back_data.prize_matches_plain);
  text_cursor_to_cell(23, 10);
  for (i = 0; i < 8; i++) {
    text_draw_formatted("ITEM_%d  %d\n", i, user_back_data.prize_item_counts[i]);
  }
}

void character_usage(void)
{
  uint i;

  text_cursor_to_cell(0xb,4);
  text_draw_formatted("USAGE OF CHARACTERS");
  text_cursor_to_cell(4,6);
  text_draw_formatted("1P CHARACTERS\n\n");
  for (i = 0; i < 19; i++) {
    if (chara_shown[i] != 0) {
      text_draw_formatted("%-7s %6d\n",chara_names[i],
                          user_back_data.bookkeep.chara_play_counts[0][i]);
    }
  }
  text_cursor_to_cell(0x16,6);
  text_draw_formatted("2P CHARACTERS\n\n");
  for (i = 0; i < 19; i++) {
    if (chara_shown[i] != 0) {
      text_draw_formatted("%-7s %6d\n",chara_names[i],
                          user_back_data.bookkeep.chara_play_counts[1][i]);
    }
  }
}

void stage_records_1p(void)
{
  bookkeep_rec *r = &user_back_data.bookkeep;
  uint i;
  menu_item_rec *it;
  char **t;

  text_cursor_to_cell(10, 1);
  text_draw_formatted("  ARRIVAL STAGE %d/5", test_page + 1);
  text_cursor_to_cell(10, 5);
  text_draw_formatted("1P GAME STAGE RECORDS");
  text_cursor_to_cell(5, 0x1c);
  if (test_page < 4) {
    text_draw_formatted("PRESS TEST BUTTON TO CONTINUE");
  }
  else {
    text_draw_formatted("PRESS TEST BUTTON TO EXIT");
  }
  text_cursor_to_cell(7, 9);
  it = &enemy_level_texts[test_page];
  t = &it->text;
  text_draw_formatted(" %s\n\n", *t);
  text_draw_formatted("        TOTAL        1COIN\n");
  for (i = 0; i < 11; i++) {
    text_draw_formatted("#%-2u    %6d       %6d\n", i + 1, r->course_results[test_page][i], r->course_results_no_continue[test_page][i]);
  }
  if (input_merged_pressed(0x100f0f04)) {
    if (++test_page > 4) {
      test_page = 0;
      test_cursor = 2;
      test_mode = 0;
      test_phase = 1;
    }
  }
}

void chara_data(void)
{
  int n;
  uint u;
  int i;
  int k;

  n = 0;
  for (u = 0; u < 19; u++) {
    if (chara_shown[u] != 0) n++;
  }
  if (input_merged_pressed(0x100f0f04)) {
    test_page++;
  }
  if (test_page >= n) {
    test_page = 0;
  }
  else {
    text_cursor_to_cell(10, 1);
    text_draw_formatted("    CHARA DATA %d/%d", test_page + 1, n);
    text_cursor_to_cell(5, 27);
    text_draw_formatted("PRESS TEST BUTTON TO CONTINUE\n");
    text_draw_formatted("PRESS SERVICE+TEST BUTTON TO EXIT");
    k = 0;
    for (i = 0; i < 19; i++) {
      if (chara_shown[i] != 0) {
        if (k == test_page) break;
        k++;
      }
    }
    chara_data_page_draw(i);
    if (!(input_merged_held(4) && input_merged_held(8))) {
      if (!input_merged_held(0x10000000)) return;
      if (!input_merged_held(0x20000000)) return;
    }
  }
  test_cursor = 3;
  test_mode = 0;
  test_phase = 1;
}

void chara_data_page_draw(int chara)
{
  text_cursor_to_cell(4,7);
  text_draw_formatted("%s\n",chara_names[chara]);
  text_cursor_to_cell(4,10);
  text_draw_formatted("MAX COMBO  %d\n",user_back_data.bookkeep.chara_max_combo[chara]);
  text_draw_formatted("MAX DAMAGE %d\n\n",user_back_data.bookkeep.chara_max_damage[chara]);
  text_draw_formatted("MAX WALL COMBO  %d\n",user_back_data.bookkeep.chara_max_wall_combo[chara]);
  text_draw_formatted("MAX WALL DAMAGE %d\n",user_back_data.bookkeep.chara_max_wall_damage[chara]);
}

void total_data(void)
{
  text_cursor_to_cell(10,1);
  text_draw_formatted("    TOTAL DATA %d/%d",test_page + 1,1);
  text_cursor_to_cell(7,0x1b);
  if (test_page < 0) {
    text_draw_formatted("PRESS TEST BUTTON TO CONTINUE");
  }
  else {
    text_draw_formatted("PRESS TEST BUTTON TO EXIT\n");
  }
  if (input_merged_pressed(0x100f0f04) != 0) {
    if (++test_page > 0) {
      test_page = 0;
      test_cursor = 4;
      test_mode = 0;
      test_phase = 1;
      return;
    }
  }
  if (test_page == 0) {
    total_data_page_draw();
  }
}

void total_data_page_draw(void)
{
  ranking_row_rec *r;
  uint i;

  text_cursor_to_cell(1, 4);
  text_draw_formatted("SERIES OF WINS\n\n");
  for (i = 0; i < 10; i++) {
    r = &user_back_data.rankings.point_ranking_rows[i];
    text_draw_formatted("%4s %4d  %s\n", rank_names[i], r->value, chara_names[r->chara]);
  }
  text_cursor_to_cell(0x16, 4);
  text_draw_formatted("WINS OF CHARA\n\n");
  for (i = 0; i < 16; i++) {
    if (chara_shown[i] != 0)
      text_draw_formatted("%-7s %6d\n", chara_names[i], user_back_data.rankings.best_win_streak[i]);
  }
}

void bookkeep_daily_coins(void)
{
  ledger_day_rec *rec;
  uint i;
  int x;
  int y;

  x = 2;
  y = 10;
  text_size_8();
  text_cursor_to_cell(x,8);
  text_draw_formatted("           | 1P      | 2P      | TOTAL   | COIN    | SERVICE | TOTAL\n");
  text_draw_formatted("    DAY    | COIN    | COIN    | COIN    | CREDIT  | CREDIT  | CREDIT\n");
  for (i = 0; i < 30; i++) {
    rec = &user_back_data.ledger_daily_records[i];
    text_cursor_to_cell(x,y);
    if (i % 10 == 0) {
      text_draw_formatted("-----------------------------------------------------------------------------\n");
      y++;
    }
    if (rec->year == 0) {
      text_draw_formatted("---------- ");
    }
    else {
      text_draw_formatted("%04d-%02d-%02d ",rec->year + 1900,rec->month + 1,rec->day);
    }
    if (rec->coin_1p == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->coin_1p);
    }
    if (rec->coin_2p == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->coin_2p);
    }
    if (rec->coin_total == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->coin_total);
    }
    if (rec->credit_coin == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->credit_coin);
    }
    if (rec->credit_service == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->credit_service);
    }
    if (rec->credit_total == 0) {
      text_draw_formatted("        - ");
    }
    else {
      text_draw_formatted(" % 8d ",rec->credit_total);
    }
    y++;
  }
  text_size_16();
}

void vs_diagram(void)
{
  uint i;
  uint j;
  int x;
  int y;
  uint t;
  int gap;
  int left;
  float s;
  int m[16][16];
  float wins[16];
  float losses[16];
  float rate[16];
  int rank[16];

  left = 2;
  for (i = 0; i < 16; i++) {
    wins[i] = 0.0f;
    losses[i] = 0.0f;
    rate[i] = 0.0f;
    rank[i] = 0;
    for (j = 0; j < 16; j++) {
      m[i][j] = 0;
    }
  }
  text_size_8();
  x = left + 4;
  y = 12;
  for (i = 0; i < 16; i++) {
    if (chara_shown[i] != 0) {
      for (j = 0; j < 16; j++) {
        if (chara_shown[j] != 0) {
          text_cursor_to_cell(x, y);
          if (i != j) {
            t = user_back_data.bookkeep.vs_chara_counts[i][j] + user_back_data.bookkeep.vs_chara_counts[j][i];
            if (t == 0) {
              text_draw_formatted(" %c", '-');
            }
            else if (user_back_data.bookkeep.vs_chara_counts[i][j] == 0) {
              text_draw_formatted(" 0");
            }
            else {
              text_draw_formatted("%2.0f", ((float)user_back_data.bookkeep.vs_chara_counts[i][j] / (float)t) * 100.0f);
            }
          }
          x += 4;
        }
      }
      x = left + 4;
      y += 2;
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      wins[i] += (float)user_back_data.bookkeep.vs_chara_counts[i][j];
      losses[i] += (float)user_back_data.bookkeep.vs_chara_counts[j][i];
    }
  }
  for (i = 0; i < 16; i++) {
    s = wins[i] + losses[i];
    if (s > 0.0f) {
      rate[i] = wins[i] / s;
    }
    else {
      rate[i] = 0.0f;
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      if (i != j && rate[i] < rate[j]) {
        rank[i]++;
      }
    }
  }
  gap = 0;
  for (i = 0; i < 16; i++) {
    if (chara_shown[i] == 0) gap = 1;
    else {
      if (gap == 0) text_cursor_to_cell(67, i * 2 + 12);
      else text_cursor_to_cell(67, i * 2 + 10);
      text_draw_formatted("%2.2f%%", rate[i] * 100.0f);
    }
  }
  text_color_set(0xffbbbbff);
  gap = 0;
  for (i = 0; i < 16; i++) {
    if (chara_shown[i] == 0) {
      gap = 1;
    }
    else {
      if (gap == 0) {
        text_cursor_to_cell(74, i * 2 + 12);
      }
      else {
        text_cursor_to_cell(74, i * 2 + 10);
      }
      text_draw_formatted("%2d", rank[i] + 1);
    }
  }
  text_color_set(-1);
  text_size_16();
  text_cursor_to_cell(15, 1);
  text_draw_formatted("VS DIAGRAM");
  text_cursor_to_cell(7, 28);
  text_draw_formatted("PRESS TEST BUTTON TO EXIT");
  text_cell_size_set(8, 8);
  text_cursor_to_cell(2, 9);
  text_draw_formatted("---------------------------------------------------------------\n");
  text_draw_formatted("   |AKI|SAR|LAU|SHN|JEF|PAI|JAK|KAG|LIO|WOL|AOI|LEI|VAN|GOH|BRD|\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---\n");
  text_draw_formatted("AKI|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("SAR|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("LAU|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("SHN|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("JEF|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("PAI|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("JAK|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("KAG|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("LIO|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("WOL|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("AOI|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("LEI|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("VAN|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("GOH|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---|\n");
  text_draw_formatted("BRD|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |\n");
  text_draw_formatted("---------------------------------------------------------------\n");
  text_cell_size_set(16, 16);
  if (input_merged_pressed(0x100f0f04) != 0) {
    test_cursor = 5;
    test_mode = 0;
    test_phase = 1;
  }
}

void network_test_log(void)
{
  text_cursor_to_cell(10, 1);
  text_draw_formatted("NETWORK TEST LOG %d/%d", net_log_page + 1, net_log_slot_count_get());
  text_cursor_to_cell(5, 0x1b);
  text_draw_formatted("PRESS TEST BUTTON TO CONTINUE\n");
  text_draw_formatted("PRESS SERVICE+TEST BUTTON TO EXIT");
  if (input_merged_pressed(0x10010104))
    net_log_page++;
  if (input_merged_pressed(0x00040400))
    net_log_page += 10;
  if (input_merged_pressed(0x20020208))
    net_log_page--;
  net_log_page = net_log_slot_wrap(net_log_page);
  network_test_log_draw(net_log_slot_addr(net_log_page));
  if ((input_merged_held(4) && input_merged_held(8)) ||
      (input_merged_held(0x10000000) && input_merged_held(0x20000000))) {
    test_cursor = 3;
    test_mode = 8;
    test_phase = 1;
  }
}

void network_test_log_draw(char *slot_addr)
{
  char buf[20];
  net_check_log_rec rec;
  net_check_log_rec *p;

  p = &rec;
  net_log_read(p, slot_addr);
  text_cursor_to_cell(2, 3);
  text_draw_formatted("%s\n", network_mode_text(p->mode, p->system_param));
  text_draw_formatted("\n");
  text_draw_formatted("SEND\n");
  text_draw_formatted(" IP         = %s\n", p->ip_text);
  text_draw_formatted(" SERIAL     = %s\n", p->serial_text);
  strftime((int)buf, 20, "%Y-%m-%d %H:%M:%S", (int *)&p->request_time);
  text_draw_formatted(" DATE       = %s\n", buf);
  text_draw_formatted("\n");
  text_draw_formatted("RECEIVE\n");
  text_draw_formatted(" STATUS     = %s\n", network_result_text(atol(p->auth_stat_text)));
  text_draw_formatted(" PLACE ID   = %s\n", p->place_text);
  strftime((int)buf, 20, "%Y-%m-%d %H:%M:%S", (int *)&p->server_time);
  text_draw_formatted("SERVER DATE = %s\n", buf);
  strftime((int)buf, 20, "%Y-%m-%d %H:%M:%S", (int *)&p->local_time);
  text_draw_formatted(" DATE       = %s\n", buf);
  text_draw_formatted("RESPONSE    = %d FRAMES (%d SEC)\n", p->response_frames, p->response_frames / 60);
  text_draw_formatted("TIME OUT    = %d SEC\n", p->timeout_frames / 60);
  text_draw_formatted("\n");
  text_draw_formatted("IP ADR      = %3d.%3d.%3d.%3d\n", ((uchar *)p->board_info)[0], ((uchar *)p->board_info)[1], ((uchar *)p->board_info)[2], ((uchar *)p->board_info)[3]);
  text_draw_formatted("NET MASK    = %3d.%3d.%3d.%3d\n", ((uchar *)p->board_info)[4], ((uchar *)p->board_info)[5], ((uchar *)p->board_info)[6], ((uchar *)p->board_info)[7]);
  text_draw_formatted("GATEWAY     = %3d.%3d.%3d.%3d\n", ((uchar *)p->board_info)[8], ((uchar *)p->board_info)[9], ((uchar *)p->board_info)[10], ((uchar *)p->board_info)[11]);
  text_draw_formatted("DNS 1       = %3d.%3d.%3d.%3d\n", ((uchar *)p->board_info)[12], ((uchar *)p->board_info)[13], ((uchar *)p->board_info)[14], ((uchar *)p->board_info)[15]);
  text_draw_formatted("DNS 2       = %3d.%3d.%3d.%3d\n", ((uchar *)p->board_info)[16], ((uchar *)p->board_info)[17], ((uchar *)p->board_info)[18], ((uchar *)p->board_info)[19]);
  text_draw_formatted("\n");
  text_draw_formatted("BUILD       = %s\n", p->date_text);
  text_draw_formatted("\n");
}

int net_check_step(void)
{
  int r;
  net_info_t *info;

  net_check_wait++;
  r = alab_auth_result_get();
  if (r == 0) {
    if (net_check_wait >= user_back_data.net_test_timeout || !net_is_on()) {
      info = net_info_get();
      net_log_record_time_stamp(&net_check_log);
      net_log_save(&net_check_log);
      info->auth_stat = 2;
    }
    else {
      return 0;
    }
  }
  else {
    net_auth_result_store(r, &net_check_log);
    net_log_save(&net_check_log);
  }
  return 1;
}

void net_test_reply_poll(void)
{
  form_map map;

  net_test_wait++;
  if (net_test_wait > 600) {
    net_test_stat = 2;
  }
  if (http_reply_ready() != 0) {
    if (net_reply_cmd_find(http_reply_text()) == 4) {
      if (form_map_parse(&map, http_header_end_find(http_reply_text())) == 0) {
        net_test_stat = atol(form_map_find(&map, "stat"));
        form_map_clear(&map);
      }
      http_reply_release();
    }
  }
}

void net_test_reply_poll_pending(void)
{
  if (net_test_stat == 0) {
    net_test_reply_poll();
  }
}

void close_time_hour_step(short day)
{
  close_hours[day]++;
  if (close_hours[day] == 9) {
    close_hours[day] = 19;
  }
  if (close_hours[day] > 23) {
    close_hours[day] = 0;
  }
  if (close_hours[day] == 7 || close_hours[day] == 8) {
    close_minutes[day] = 0;
  }
}

void close_time_minute_step(short day)
{
  close_minutes[day] += 15;
  if (close_minutes[day] > 59) {
    close_minutes[day] = 0;
  }
  if ((uchar)(close_hours[day] - 7) < 2) {
    close_minutes[day] = 0;
  }
}

void menu_hint_draw(void)
{
  text_cursor_to_cell(7,0x1b);
  text_draw_formatted("SELECT WITH SERVICE BUTTON\n");
  text_draw_formatted("  AND PRESS TEST BUTTON");
}

char *on_off_text(int on)
{
  return on_off_texts[on != 0];
}

void display_init(void)
{
  bg_plane_corner_colors(0, 0, 0);
  vo_border_write_opaque(0);
  render_pipeline_three_frames();
  render_config_texmem_size_put(0x280000);
  elan_lists_start(0x80000, 0x10000, 0x20000, 0x10000, 0x10000);
  elan_list_enable_mask_put(15);
}

void card_test_message_set(int unit, uint message)
{
  if (message < 13) {
    card_menu_state.unit[unit].message = message;
    card_menu_state.unit[unit].message_timer = 120;
  }
}

void card_menu_init(void)
{
  int i;

  card_crc_tables_init();
  card_menu_state.frame = 0;
  card_menu_state.cursor = 4;
  for (i = 0; i < 5; i++) {
    card_menu_state.item_enabled[i] = 0;
  }
  card_menu_state.item_enabled[4] = 1;
  for (i = 0; i < 2; i++) {
    card_menu_state.unit[i].kind = 0;
    card_menu_state.unit[i].step = 0;
    card_menu_state.unit[i].sub_step = 0;
    card_menu_state.unit[i].flags = 0;
    card_menu_state.unit[i].result = 0;
    card_menu_state.unit[i].value = 0;
    card_menu_state.unit[i].message_timer = 0;
    card_menu_state.unit[i].word_0x24 = 0;
    card_menu_state.unit[i].word_0x28 = 0;
    if (card_unit_ok(i)) {
      card_menu_state.unit[i].usable = 1;
      card_menu_state.unit[i].message = 0;
    }
    else {
      card_menu_state.unit[i].usable = 0;
      card_menu_state.unit[i].message = 1;
    }
  }
  test_cursor = 4;
  test_phase = 1;
}

void card_menu_frame(void)
{
  card_menu_state.frame++;
  card_test_units_step();
  card_menu_input();
  card_menu();
  card_menu_end_hook();
}

void card_test_units_step(void)
{
  card_test_rec *u;
  int i;

  for (i = 0; i < 2; i++) {
    u = &card_menu_state.unit[i];
    if (u->usable != 0) {
      card_test_insert_poll(i);
      switch (u->kind) {
      case 1:
        card_test_read_step(i);
        break;
      case 2:
        card_test_write_step(i);
        break;
      case 3:
        card_test_format_step(i);
        break;
      case 0:
      default:
        break;
      }
    }
  }
}

void card_test_insert_poll(int unit)
{
  card_test_rec *r = &card_menu_state.unit[unit];
  if ((&card_status)[unit * 8] != 0) {
    if ((r->flags & 1) == 0) {
      r->flags |= 0x80003;
    }
    else {
      r->flags &= ~2;
    }
  }
  else {
    if (r->flags & 1) {
      r->flags = 4;
    }
    else {
      r->flags &= ~4;
    }
    r->result = 0;
    card_test_message_set(unit,2);
  }
  if ((r->flags & 0x80000) && card_test_start(unit,1)) {
    r->flags &= ~0x80000;
  }
}

void card_menu_input(void)
{
  int d = 0;
  int i;
  card_menu_rec *menu = &card_menu_state;

  for (i = 0; i < 2; i++) {
    card_test_rec *u = &menu->unit[i];
    menu->item_enabled[i * 2] = 0;
    menu->item_enabled[i * 2 + 1] = 0;
    if (u->usable != 0 && u->result == 1 && u->kind == 0) {
      menu->item_enabled[i * 2] = 1;
      menu->item_enabled[i * 2 + 1] = 1;
    }
  }
  if (input_merged_pressed(0x20101008)) d++;
  if (input_merged_pressed(0x00202000)) d--;
  menu->cursor += d;
  if (menu->cursor < 0) menu->cursor = 4;
  if (menu->cursor > 4) menu->cursor = 0;
  if (input_merged_pressed(0x100f0f04) == 0) return;
  if (menu->item_enabled[menu->cursor] == 0) return;
  switch (menu->cursor) {
  case 0:
    card_test_start(0,2);
    break;
  case 1:
    if ((&card_status)[1] == 0) card_lock_start(0);
    else card_unlock_start(0);
    break;
  case 2:
    card_test_start(1,2);
    break;
  case 3:
    if ((&card_status)[9] == 0) card_lock_start(1);
    else card_unlock_start(1);
    break;
  case 4:
  default:
    test_cursor = 6;
    test_mode = 0;
    test_phase = 1;
    break;
  }
}

void card_menu(void)
{
  typedef struct { char sw; char lock; char pad[6]; } card_status_t;
  card_menu_rec *menu = &card_menu_state;
  card_test_rec *rec;
  card_status_t *cs;
  int unit;
  int i;
  int sw;
  int lock;
  int msg;
  int kind;
  int hide;
  int row;

  text_color_set(-1);
  text_cursor_to_cell(8, card_menu_rows[menu->cursor]);
  text_draw_formatted("%c", 4);
  text_cursor_to_cell(0xe, 1);
  text_draw_formatted("CARD MENU");
  for (unit = 0; unit < 2; unit++) {
    rec = &card_menu_state.unit[unit];
    cs = &((card_status_t *)&card_status)[unit];
    sw = cs->sw & 1;
    lock = cs->lock & 1;
    msg = rec->message;
    text_cursor_to_cell(2, unit * 8 + 6);
    text_draw_formatted("CARD %dP : SW %s : LOCK %s\n", unit + 1, card_off_on_texts[sw], card_off_on_texts[lock]);
    kind = card_test_messages[msg].kind;
    hide = 1;
    if (menu->frame & 0x10)
      hide = 0;
    if (kind == 0)
      hide = 0;
    if (kind == 1) {
      if (rec->message_timer <= 0)
        hide = 0;
    }
    if (hide)
      msg = 0;
    text_draw_formatted("        : %s", card_test_messages[msg].text);
    if (rec->message_timer > 0)
      rec->message_timer--;
    for (i = 0, row = unit * 2; i < 2; i++) {
      int k = row + i;
      int item = menu->item_enabled[k];
      text_cursor_to_cell(10, card_menu_rows[k]);
      text_color_set(card_menu_colors[item]);
      text_draw_formatted("%s", card_item_names[i]);
    }
    text_color_set(-1);
  }
  text_cursor_to_cell(10, 0x16);
  text_draw_formatted("EXIT");
  menu_hint_draw();
}

void card_menu_end_hook(void)
{
  return;
}

void difficulty_text(void)
{
  text_cursor_to_cell(0x1b,assignment_menu_items[0].y);
  text_draw_formatted("%s",enemy_level_texts[enemy_level].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[1].y);
  text_draw_formatted("%s",energy_texts[energy_max_1p].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[2].y);
  text_draw_formatted("%s",energy_texts[energy_max_vs].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[3].y);
  text_draw_formatted("%d",match_count_1p);
  text_cursor_to_cell(0x1b,assignment_menu_items[4].y);
  text_draw_formatted("%d",match_count_vs);
  text_cursor_to_cell(0x1b,assignment_menu_items[5].y);
  text_draw_formatted("%s",time_limit_texts[time_limit].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[6].y);
  text_draw_formatted("%s",stage_select_texts[stage_select].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[7].y);
  text_draw_formatted("%s",continue_texts[continue_setting].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[8].y);
  text_draw_formatted("%s",vs_finish_texts[vs_finish].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[9].y);
  text_draw_formatted("%s",off_on_items[network_on].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[10].y);
  text_draw_formatted("%s",off_on_items[advertise_connect].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[11].y);
  text_draw_formatted("%s",off_on_items[card_use[0]].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[12].y);
  text_draw_formatted("%s",off_on_items[card_use[1]].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[13].y);
  text_draw_formatted("%s",off_on_items[card_start].text);
  text_cursor_to_cell(0x1b,assignment_menu_items[14].y);
  text_draw_formatted("%s",off_on_items[effect_skip].text);
}

void backup_data_clear(void)
{
  int sel;

  text_cursor_to_cell(0xc, 1);
  text_draw_formatted("BACKUP DATA CLEAR");
  menu_hint_draw();
  sel = menu_cursor_step(2);
  menu_draw_with_cursor(sel, backup_clear_items, 2);
  if (sel < 1) {
    if (input_merged_pressed(0x100f0f04)) {
      backup_data_clear_run();
      sram_save_store_dma_safe();
      test_phase = 3;
    }
  }
  else if (sel == 1) {
    if (input_merged_pressed(0x100f0f04)) {
      test_phase = 2;
    }
  }
}

void challenge_total_bookkeep(void)
{
  uint min;
  int sec;
  uint centi;
  int i;
  int j;

  text_size_8();
  text_cursor_to_cell(2,4);
  text_draw_formatted("CHALLENGE TOTAL-BOOKKEEP");
  text_cursor_to_cell(2,6);
  text_draw_formatted("TOTAL CLEAR NUM:%d\n",user_back_data.challenge.chlg_clear_num);
  frames_to_min_sec_centi(user_back_data.challenge.chlg_clear_time_max,&min,&sec,&centi);
  text_draw_formatted("CLEAR TIME MAX:%02d'%02d''%02d\n",min,sec,centi);
  frames_to_min_sec_centi(user_back_data.challenge.chlg_clear_time_min,&min,&sec,&centi);
  text_draw_formatted("CLEAR TIME MIN:%02d'%02d''%02d\n",min,sec,centi);
  frames_to_min_sec_centi(user_back_data.challenge.chlg_clear_time_avg,&min,&sec,&centi);
  text_draw_formatted("CLEAR TIME AVG:%02d'%02d''%02d\n",min,sec,centi);
  for (i = 0; i < 8; i++) {
    text_cursor_to_cell(2,i * total_row_height + 11);
    text_draw_formatted(" C B A2A3A\n");
    text_draw_formatted("%2d%2d%2d%2d%2d\n",user_back_data.challenge.chlg_course_ranks[i][0],
                        user_back_data.challenge.chlg_course_ranks[i][1],
                        user_back_data.challenge.chlg_course_ranks[i][2],
                        user_back_data.challenge.chlg_course_ranks[i][3],
                        user_back_data.challenge.chlg_course_ranks[i][4]);
    text_draw_formatted("GET:%d\n",user_back_data.challenge.chlg_course_gets[i]);
    for (j = 0; j < 3; j++) {
      mission_score_rec *r = &user_back_data.challenge.course_ranking[i][j];
      text_cursor_to_cell(j * page_col_width + 14,
                          i * total_row_height + 11);
      text_draw_formatted("%d:\n",j + 1);
      text_draw_formatted("%s\n",chara_names[r->chara]);
      frames_to_min_sec_centi(r->time,&min,&sec,&centi);
      text_draw_formatted("%2d'%02d''%02d\n",min,sec,centi);
      text_draw_formatted("%d PT\n",r->score);
    }
  }
  text_size_16();
}

void stage_ringout_ratio(void)
{
  bookkeep_rec *b = &user_back_data.bookkeep;
  float ratio[30];
  uint i;
  text_cursor_to_cell(9,5);
  text_draw_formatted("RINGOUT RATIO OF STAGE");
  for (i = 0; i < 30; i++) {
    ratio[i] = 0.0f;
  }
  for (i = 0; i < 30; i++) {
    if (b->stage_vs_rounds[i] + b->stage_vs_rounds_flag40[i] != 0) {
      ratio[i] = (float)b->stage_vs_rounds_flag40[i] / (float)b->stage_vs_rounds[i];
    }
    else {
      ratio[i] = 0.0f;
    }
    ratio[i] = ratio[i] * 100.0f;
  }
  text_cursor_to_cell(2,7);
  text_draw_formatted("           RING OUT   ROUND  RATIO\n");
  for (i = 0; i < 24; i++) {
    if (stage_shown[i] != 0) {
      text_draw_formatted("%-10s  %6d  %6d   %5.2f%%\n",stage_names[i],b->stage_vs_rounds_flag40[i],b->stage_vs_rounds[i],ratio[i]);
    }
  }
}

void bookkeep_play_totals(void)
{
  uint total;
  uint play;
  float ratio;
  uint all_avg;
  uint p1_avg;
  uint vs_avg;
  uint p1_long;
  uint p1_short;
  uint vs_long;
  uint vs_short;
  int pd, ph, pm, ps;
  int td, th, tm, ts;
  int ah, am, as;
  int oh, om, os;
  int lh, lm, ls;
  int sh, sm, ss;
  int vh, vm, vs;
  int wh, wm, ws;
  int xh, xm, xs;

  total = user_back_data.bookkeep.total_time;
  play = user_back_data.bookkeep.play_time;
  if (total == 0) {
    ratio = 0.0f;
  }
  else {
    ratio = ((float)play / (float)total) * 100.0f;
  }
  if (user_back_data.bookkeep.games_total == 0) {
    all_avg = 0;
  }
  else {
    all_avg = user_back_data.bookkeep.play_time / user_back_data.bookkeep.games_total;
  }
  if (user_back_data.bookkeep.games_1p == 0) {
    p1_avg = 0;
  }
  else {
    p1_avg = user_back_data.bookkeep.p1_time / user_back_data.bookkeep.games_1p;
  }
  p1_long = user_back_data.bookkeep.longest_1p;
  p1_short = user_back_data.bookkeep.shortest_1p;
  if (user_back_data.bookkeep.games_vs == 0) {
    vs_avg = 0;
  }
  else {
    vs_avg = user_back_data.bookkeep.vs_time / user_back_data.bookkeep.games_vs;
  }
  vs_long = user_back_data.bookkeep.longest_vs;
  vs_short = user_back_data.bookkeep.shortest_vs;
  pd = frames_to_time_part(play, 0);
  ph = frames_to_time_part(play, 1);
  pm = frames_to_time_part(play, 2);
  ps = frames_to_time_part(play, 3);
  td = frames_to_time_part(total, 0);
  th = frames_to_time_part(total, 1);
  tm = frames_to_time_part(total, 2);
  ts = frames_to_time_part(total, 3);
  ah = frames_to_time_part(all_avg, 1);
  am = frames_to_time_part(all_avg, 2);
  as = frames_to_time_part(all_avg, 3);
  oh = frames_to_time_part(p1_avg, 1);
  om = frames_to_time_part(p1_avg, 2);
  os = frames_to_time_part(p1_avg, 3);
  lh = frames_to_time_part(p1_long, 1);
  lm = frames_to_time_part(p1_long, 2);
  ls = frames_to_time_part(p1_long, 3);
  sh = frames_to_time_part(p1_short, 1);
  sm = frames_to_time_part(p1_short, 2);
  ss = frames_to_time_part(p1_short, 3);
  vh = frames_to_time_part(vs_avg, 1);
  vm = frames_to_time_part(vs_avg, 2);
  vs = frames_to_time_part(vs_avg, 3);
  wh = frames_to_time_part(vs_long, 1);
  wm = frames_to_time_part(vs_long, 2);
  ws = frames_to_time_part(vs_long, 3);
  xh = frames_to_time_part(vs_short, 1);
  xm = frames_to_time_part(vs_short, 2);
  xs = frames_to_time_part(vs_short, 3);
  text_cursor_to_cell(1, 4);
  text_draw_formatted("\n\n");
  text_draw_formatted("        NUMBER OF GAMES  %6d\n", user_back_data.bookkeep.games_total);
  text_draw_formatted("               1P GAMES  %6d\n", user_back_data.bookkeep.games_1p);
  text_draw_formatted("               VS GAMES  %6d\n", user_back_data.bookkeep.games_vs);
  text_draw_formatted("\n");
  text_draw_formatted("     NUMBER OF CONTINUE  %6d\n", user_back_data.bookkeep.continues);
  text_draw_formatted("\n");
  text_draw_formatted("CHALLENGE/PARTICIPATION  %6d\n", user_back_data.bookkeep.challengers);
  text_draw_formatted("\n");
  text_draw_formatted("      TOTAL TIME  %2dD %2dH %2dM %2dS\n", td, th, tm, ts);
  text_draw_formatted(" TOTAL PLAY TIME  %2dD %2dH %2dM %2dS\n", pd, ph, pm, ps);
  text_draw_formatted("\n");
  text_draw_formatted("ALL AVERAGE PLAY TIME  %2dH %2dM %2dS\n", ah, am, as);
  text_draw_formatted("1P  AVERAGE PLAY TIME  %2dH %2dM %2dS\n", oh, om, os);
  text_draw_formatted("1P  LONGEST PLAY TIME  %2dH %2dM %2dS\n", lh, lm, ls);
  text_draw_formatted("1P SHORTEST PLAY TIME  %2dH %2dM %2dS\n", sh, sm, ss);
  text_draw_formatted("VS  AVERAGE PLAY TIME  %2dH %2dM %2dS\n", vh, vm, vs);
  text_draw_formatted("VS  LONGEST PLAY TIME  %2dH %2dM %2dS\n", wh, wm, ws);
  text_draw_formatted("VS SHORTEST PLAY TIME  %2dH %2dM %2dS\n", xh, xm, xs);
  text_draw_formatted("\n");
  text_draw_formatted("  OPERATING RATIO     %3.1f%%\n", ratio);
}

void stage_usage(void)
{
  uint i;

  text_cursor_to_cell(0xd,5);
  text_draw_formatted("USAGE OF STAGE");
  text_cursor_to_cell(10,7);
  for (i = 0; i < 24; i++) {
    if (stage_shown[i] != 0) {
      text_draw_formatted("%-10s  %6d\n",stage_names[i],
                          user_back_data.bookkeep.stage_select_counts[i]);
    }
  }
}

void network_test(void)
{
  int n;
  menu_item_rec *items;
  int sel;

  n = 2;
  items = network_menu_items_short;
  if (net_log_unlocked == 1) {
    n = 4;
    items = network_menu_items;
  }
  text_cursor_to_cell(10,1);
  text_draw_formatted("    NETWORK TEST");
  menu_hint_draw();
  sel = menu_cursor_step(n);
  menu_draw_with_cursor(sel,items,n);
  if (net_log_unlocked == 1) {
    switch (sel) {
    case 0:
      if (input_merged_pressed(0x100f0f04)) {
        char *p = net_test_request;
        net_test_wait = 0;
        net_test_stat = 0;
        p += sprintf(p,"cmd=test&");
        p += sprintf(p,"serial=%s&",net_serial_get());
        sprintf(p,"cnt=%d",net_test_count++);
        http_post_queue(net_upload_url_build(),net_test_request,strlen(net_test_request));
        test_phase = 2;
      }
      break;
    case 1:
      if (input_merged_pressed(0x100f0f04)) {
        timeout_screen_flag = 0;
        test_cursor = 2;
        test_phase = 3;
      }
      break;
    case 2:
      if (input_merged_pressed(0x100f0f04)) {
        net_log_page = net_log_slot_wrap(user_back_data.net_log_slot_next - 1);
        test_phase = 4;
      }
      break;
    case 3:
      if (input_merged_pressed(0x100f0f04)) {
        net_log_unlocked = 0;
        unlock_code_step = 0;
        test_cursor = 7;
        test_mode = 0;
        test_phase = 1;
      }
      break;
    }
  }
  else {
    switch (sel) {
    case 0:
      if (input_merged_pressed(0x100f0f04)) {
        char *p = net_test_request;
        net_test_wait = 0;
        net_test_stat = 0;
        p += sprintf(p,"cmd=test&");
        p += sprintf(p,"serial=%s&",net_serial_get());
        sprintf(p,"cnt=%d",net_test_count++);
        if (http_post_queue(net_upload_url_build(),net_test_request,strlen(net_test_request))) {
          net_test_stat = 0;
        }
        test_phase = 2;
      }
      break;
    case 1:
      if (input_merged_pressed(0x100f0f04)) {
        net_log_unlocked = 0;
        unlock_code_step = 0;
        test_cursor = 7;
        test_mode = 0;
        test_phase = 1;
      }
      break;
    }
  }
}

void network_timeout_setting(void)
{
  int c;

  text_cursor_to_cell(6,1);
  text_draw_formatted("NETWORK TEST TIME OUT SETTING");
  text_cursor_to_cell(5,0x1b);
  text_draw_formatted("   SELECT LEVER TO SETTING\n");
  text_draw_formatted("   AND PRESS TEST BUTTON");
  c = menu_cursor_step(3);
  menu_draw_with_cursor(c,timeout_menu_items,3);
  switch (c) {
  case 0:
    if (input_merged_pressed(0x104f4f04) != 0) {
      user_back_data.net_timeout += 600;
      if (user_back_data.net_timeout > 18000)
        user_back_data.net_timeout = 600;
    }
    if (input_merged_pressed(0x00808000) != 0) {
      user_back_data.net_timeout -= 600;
      if (user_back_data.net_timeout < 600)
        user_back_data.net_timeout = 18000;
    }
    if (user_back_data.net_timeout < 600)
      user_back_data.net_timeout = 600;
    if (user_back_data.net_timeout > 18000)
      user_back_data.net_timeout = 18000;
    break;
  case 1:
    if (input_merged_pressed(0x104f4f04) != 0) {
      user_back_data.net_test_timeout += 600;
      if (user_back_data.net_test_timeout > 18000)
        user_back_data.net_test_timeout = 600;
    }
    if (input_merged_pressed(0x00808000) != 0) {
      user_back_data.net_test_timeout -= 600;
      if (user_back_data.net_test_timeout < 600)
        user_back_data.net_test_timeout = 18000;
    }
    if (user_back_data.net_test_timeout < 600)
      user_back_data.net_test_timeout = 600;
    if (user_back_data.net_test_timeout > 18000)
      user_back_data.net_test_timeout = 18000;
    break;
  case 2:
    if (input_merged_pressed(0x100f0f04) != 0) {
      test_cursor = 3;
      sram_save_store_dma_safe();
      test_mode = 8;
      test_phase = 1;
    }
    break;
  }
  text_cursor_to_cell(0x1c,10);
  text_draw_formatted("%d SECONDS",user_back_data.net_timeout / 60);
  text_cursor_to_cell(0x1c,0xc);
  text_draw_formatted("%d SECONDS",user_back_data.net_test_timeout / 60);
}

void network_test_run(void)
{
  int stat;
  char *text;

  stat = net_test_stat;
  text_cursor_to_cell(10,1);
  text_draw_formatted("    NETWORK TEST");
  text_cursor_to_cell(7,0x1c);
  if (stat == 0) {
    text_draw_formatted("          WAIT");
  }
  else {
    text_draw_formatted("PRESS TEST BUTTON TO EXIT");
  }
  text = network_result_text(stat);
  text_cursor_to_cell(10,0xc);
  text_draw_formatted("%s\n",text);
  text_cursor_to_cell(10,0xf);
  text_draw_formatted("% 3d / %d\n",net_test_wait,600);
  if (input_merged_pressed(0x100f0f04) != 0 && net_test_stat != 0) {
    if (net_log_unlocked == 1) {
      test_cursor = 3;
    }
    else {
      test_cursor = 1;
    }
    test_mode = 8;
    test_phase = 1;
  }
}

void backup_data_clear_run(void)
{
  ranking_block_rec *rb;
  alab_billing_rec *bill;
  uint i;

  rb = &user_back_data.rankings;
  for (i = 0; i < 50; i++)
    rb->time_ranking_rows[i] = user_back_defaults.rankings.time_ranking_rows[i];
  for (i = 0; i < 50; i++)
    rb->point_ranking_rows[i] = user_back_defaults.rankings.point_ranking_rows[i];
  for (i = 0; i < 16; i++)
    rb->best_win_streak[i] = user_back_defaults.rankings.best_win_streak[i];
  user_back_data.bookkeep = user_back_defaults.bookkeep;
  user_back_data.option_bits = user_back_defaults.option_bits;
  user_back_data.decided_no_card = 0;
  user_back_data.decided_one_card = 0;
  user_back_data.decided_more_cards = 0;
  user_back_data.streak_challenge_count = 0;
  user_back_data.streak_challenge_frames = 0;
  user_back_data.jisaku_no_touch_matches = 0;
  user_back_data.jisaku_no_lever_matches = 0;
  user_back_data.jisaku_no_g_matches = 0;
  user_back_data.jisaku_no_touch_high_matches = 0;
  user_back_data.jisaku_no_lever_high_matches = 0;
  user_back_data.jisaku_no_g_high_matches = 0;
  user_back_data.lottery_starts = 0;
  user_back_data.lottery_spins = 0;
  user_back_data.bonus_time_starts = 0;
  user_back_data.bonus_time_games = 0;
  user_back_data.bonus_time_shortest = 0;
  user_back_data.bonus_time_longest = 0;
  user_back_data.syuchu_marks = 0;
  user_back_data.prize_matches = 0;
  user_back_data.prize_items_given = 0;
  user_back_data.prize_matches_bonus_time = 0;
  user_back_data.prize_matches_syuchu = 0;
  user_back_data.prize_matches_kind3 = 0;
  user_back_data.prize_matches_plain = 0;
  for (i = 0; i < 8; i++)
    user_back_data.prize_item_counts[i] = 0;
  for (i = 0; i < 30; i++)
    user_back_data.mission_offered[i] = 0;
  for (i = 0; i < 30; i++)
    user_back_data.mission_offered[i] = 0;
  for (i = 0; i < 30; i++)
    user_back_data.ledger_daily_records[i] = user_back_defaults.ledger_daily_records[i];
  user_back_data.ledger_day = user_back_defaults.ledger_day;
  user_back_data.ledger_record = user_back_defaults.ledger_record;
  bill = &user_back_data.billing;
  bill->starts[0] = 0;
  bill->starts[1] = 0;
  bill->continues[0] = 0;
  bill->continues[1] = 0;
  bill->ends[0] = 0;
  bill->ends[1] = 0;
  user_back_data.challenge = user_back_defaults.challenge;
}

void test_menu(void)
{
  int c;

  text_color_set(-1);
  text_cursor_to_cell(5, 1);
  text_draw_formatted("VF4 FINAL TUNED GAME TEST MENU");
  menu_hint_draw();
  c = menu_cursor_step(12);
  menu_draw_with_cursor(c, test_menu_items, 12);
  if (net_log_unlocked == 0) {
    if ((unlock_code_step == 0 || unlock_code_timer > 0)
        && input_merged_held(unlock_code_buttons[unlock_code_step]) != 0) {
      unlock_code_step++;
    }
    if (unlock_code_step == 1) {
      unlock_code_timer = 600;
    }
    if (unlock_code_timer-- <= 0) {
      unlock_code_step = 0;
      unlock_code_timer = 0;
    }
    if (unlock_code_buttons[unlock_code_step] == -1) {
      net_log_unlocked = 1;
    }
  }
  else if (net_log_unlocked > 1 || unlock_code_step > 12) {
    net_log_unlocked = 0;
    unlock_code_step = 0;
  }
  if (net_log_unlocked == 1) {
    text_cursor_to_cell(23, 17);
    text_draw_formatted("S");
  }
  if (input_merged_pressed(0x100f0f04) != 0) {
    test_mode = test_menu_targets[c][0];
    test_phase = test_menu_targets[c][1];
  }
}

void input_test(void)
{
  player_input_rec *p1;
  player_input_rec *p2;

  p1 = player_input_get(0);
  p2 = player_input_get(1);
  text_put_at(15,1,"INPUT TEST");
  text_put_at(5,5,"PLAYER     1P       2P");
  text_cursor_to_cell(5,7);
  input_test_rows_draw(p1,p2,input_test_dir_rows,5);
  text_draw_formatted("\n");
  input_test_rows_draw(p1,p2,input_test_button_rows,3);
  text_draw_formatted("\n");
  text_draw_formatted("TEST       %s\n",on_off_text(input_merged_held(0x10000004)));
  text_draw_formatted("SERVICE    %s\n",on_off_text(input_merged_held(0x20000008)));
  text_put_at(3,0x1c,"PRESS SERVICE+TEST BUTTON TO EXIT");
  if ((input_merged_held(4) != 0 && input_merged_held(8) != 0) ||
      (input_merged_held(0x10000000) != 0 && input_merged_held(0x20000000) != 0)) {
    test_phase = 2;
  }
}

void challenge_hiscore(void)
{
  uint min;
  int sec;
  uint centi;
  int i;
  int j;
  int k;
  int x;
  int y;

  text_size_8();
  text_cursor_to_cell(2,4);
  text_draw_formatted("CHALLENGE HI-SCORE");
  j = 0;
  k = 0;
  for (i = 0; i < 36; i++) {
    mission_score_rec *r = &user_back_data.challenge.chlg_map_hiscores[i];
    x = i % 6;
    y = i / 6;
    text_cursor_to_cell(x * page_col_width + 2,y * hiscore_row_height + 6);
    text_draw_formatted("%d-%d:\n",j + 1,k + 1);
    text_draw_formatted("%s\n",chara_names[r->chara]);
    frames_to_min_sec_centi(r->time,&min,&sec,&centi);
    text_draw_formatted("%2d'%02d''%02d\n",min,sec,centi);
    text_draw_formatted("%d PT\n",r->score);
    text_draw_formatted("NUM:%d\n",r->extra);
    text_draw_formatted(" C B A2A3A\n");
    text_draw_formatted("%2d%2d%2d%2d%2d\n",r->ranks[0],r->ranks[1],r->ranks[2],r->ranks[3],r->ranks[4]);
    if (++k > j) {
      j++;
      k = 0;
    }
  }
  text_size_16();
}

void close_setting_card(void)
{
  int n = 4;
  menu_item_rec *menu = close_day_items;
  int cur;
  int i;
  short d;
  int h;
  text_cursor_to_cell(10,1);
  text_draw_formatted(" CLOSE SETTING (CARD)");
  menu_hint_draw();
  if (close_by_week != 0) {
    n = 16;
    menu = close_week_items;
  }
  cur = menu_cursor_step(n);
  menu_draw_with_cursor(cur,menu,n);
  if (cur == 0 && input_merged_pressed(0x100f0f04) != 0) {
    close_by_week = (close_by_week ^ 1) & 1;
    user_back_data.close_hour = close_day_hour;
    user_back_data.close_minute = close_day_minute;
    for (i = 0; i < 7; i++) {
      user_back_data.close_weekday_hour[i] = close_hours[i];
      user_back_data.close_weekday_minute[i] = close_minutes[i];
    }
    sram_save_store_dma_safe();
  }
  if (close_by_week == 0) {
    switch (cur) {
    case 1:
      if (input_merged_pressed(0x100f0f04) != 0) {
        close_day_hour++;
        if (close_day_hour == 9) {
          close_day_hour = 19;
        }
        if (close_day_hour > 23) {
          close_day_hour = 0;
        }
        if (close_day_hour == 7 || close_day_hour == 8) {
          close_day_minute = 0;
        }
      }
      break;
    case 2:
      if (input_merged_pressed(0x100f0f04) != 0) {
        close_day_minute += 15;
        if (close_day_minute > 59) {
          close_day_minute = 0;
        }
        if (close_day_hour == 7 || close_day_hour == 8) {
          close_day_minute = 0;
        }
      }
      break;
    case 3:
      if (input_merged_pressed(0x100f0f04) != 0) {
        user_back_data.close_hour = close_day_hour;
        user_back_data.close_minute = close_day_minute;
        user_back_data.close_by_weekday = close_by_week;
        sram_save_store_dma_safe();
        test_cursor = 8;
        test_mode = 0;
        test_phase = 1;
      }
      break;
    }
    text_cursor_to_cell(26,close_day_items[0].y);
    text_draw_formatted("DAY / ");
    text_color_set(0xff808080);
    text_draw_formatted("WEEK");
    text_color_set(-1);
    h = close_day_hour;
    if (h == 8) {
      text_cursor_to_cell(23,close_day_items[1].y);
      text_draw_formatted("ALL DAY");
      text_cursor_to_cell(23,close_day_items[2].y);
      text_draw_formatted("(24 HOURS)");
    }
    else {
      if ((uchar)close_day_hour < 8) {
        h = (char)(close_day_hour + 24);
      }
      text_cursor_to_cell(23,close_day_items[1].y);
      text_draw_formatted("%02d",h);
      text_cursor_to_cell(23,close_day_items[2].y);
      text_draw_formatted("%02d",close_day_minute);
    }
  }
  else {
    switch (cur) {
    case 1:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(0);
      break;
    case 2:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(0);
      break;
    case 3:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(1);
      break;
    case 4:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(1);
      break;
    case 5:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(2);
      break;
    case 6:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(2);
      break;
    case 7:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(3);
      break;
    case 8:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(3);
      break;
    case 9:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(4);
      break;
    case 10:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(4);
      break;
    case 11:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(5);
      break;
    case 12:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(5);
      break;
    case 13:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_hour_step(6);
      break;
    case 14:
      if (input_merged_pressed(0x100f0f04) != 0) close_time_minute_step(6);
      break;
    case 15:
      if (input_merged_pressed(0x100f0f04) != 0) {
        for (i = 0; i < 7; i++) {
          user_back_data.close_weekday_hour[i] = close_hours[i];
          user_back_data.close_weekday_minute[i] = close_minutes[i];
        }
        user_back_data.close_by_weekday = close_by_week;
        sram_save_store_dma_safe();
        test_cursor = 8;
        test_mode = 0;
        test_phase = 1;
      }
      break;
    }
    text_cursor_to_cell(26,close_week_items[0].y);
    text_color_set(0xff808080);
    text_draw_formatted("DAY");
    text_color_set(-1);
    text_draw_formatted(" / WEEK");
    for (d = 0; d < 7; d++) {
      h = close_hours[d];
      if (h == 8) {
        text_cursor_to_cell(27,close_week_items[d * 2 + 1].y);
        text_draw_formatted("ALL DAY");
        text_cursor_to_cell(27,close_week_items[d * 2 + 2].y);
        text_draw_formatted("(24 HOURS)");
      }
      else {
        if ((uchar)close_hours[d] < 8) {
          h = (char)(close_hours[d] + 24);
        }
        text_cursor_to_cell(27,close_week_items[d * 2 + 1].y);
        text_draw_formatted("%02d",h);
        text_cursor_to_cell(27,close_week_items[d * 2 + 2].y);
        text_draw_formatted("%02d",close_minutes[d]);
      }
    }
  }
}
