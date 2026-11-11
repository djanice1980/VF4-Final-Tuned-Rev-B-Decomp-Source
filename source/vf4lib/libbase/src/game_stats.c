/*
 * vf4lib/libbase/src/game_stats.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int vblank_count; /* 0x0c292544 */
extern uint game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern int win_streak; /* 0x0c2925dc */
extern uint p1_play_frames_max; /* 0x0c292614 */
extern uint p1_play_frames_min; /* 0x0c292618 */
extern uint vs_round_time; /* 0x0c29261c */
extern uint p1_round_time; /* 0x0c292620 */
extern uint vs_play_frames; /* 0x0c292624 */
extern uint match_stage; /* 0x0c2926dc */
extern uint stage_intro_frames; /* 0x0c292724 */
extern uint round_intro_frames; /* 0x0c292728 */
extern uint replay_frames; /* 0x0c29272c */
extern uint winner_frames; /* 0x0c292730 */
extern uint mask_load_frames; /* 0x0c292738 */
extern uint judge_frames; /* 0x0c29273c */
extern ledger_snapshot_rec ledger_snapshot; /* 0x0c370ecc */
extern int play_time_last_vblank; /* 0x0c370fc4 */
extern ledger_baseline_rec ledger_baseline; /* 0x0c28c214 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

void round_stats_record(void)
{
  if (play_style == '\x01') {
    user_back_data.bookkeep.p1_rounds = user_back_data.bookkeep.p1_rounds + 1;
  }
  else {
    user_back_data.bookkeep.vs_rounds = user_back_data.bookkeep.vs_rounds + 1;
    user_back_data.bookkeep.stage_vs_rounds[match_stage]++;
    if ((game_flags & 0x40) != 0) {
      user_back_data.bookkeep.stage_vs_rounds_flag40[match_stage]++;
    }
  }
  user_back_data.bookkeep.p1_round_time_sum = user_back_data.bookkeep.p1_round_time_sum + p1_round_time;
  user_back_data.bookkeep.vs_round_time_sum = user_back_data.bookkeep.vs_round_time_sum + vs_round_time;
  if (((game_flags & 0x200) == 0) &&
     (((user_back_data.bookkeep.best_vs_round >= vs_round_time || (user_back_data.bookkeep.best_vs_round == 0))
      && (vs_round_time != 0)))) {
    user_back_data.bookkeep.best_vs_round = vs_round_time;
  }
  if (((user_back_data.bookkeep.best_1p_round >= p1_round_time) || (user_back_data.bookkeep.best_1p_round == 0)
      ) && (p1_round_time != 0)) {
    user_back_data.bookkeep.best_1p_round = p1_round_time;
  }
  return;
}

void clear_stats_add(rob_rec *rob)
{
  user_back_data_rec *p;
  char difficulty;
  
  p = &user_back_data;
  p->bookkeep.chara_play_counts[rob->field_0x3][rob->chara]++;
  difficulty = p->rank_difficult;
  p->bookkeep.course_results[difficulty][10]++;
  if ((game_flags & 0x40000) == 0) {
    p->bookkeep.course_results_no_continue[difficulty][10]++;
  }
  user_back_data.bookkeep.games_total = user_back_data.bookkeep.games_total + 1;
  user_back_data.bookkeep.games_1p = user_back_data.bookkeep.games_1p + 1;
  return;
}

void lose_stats_add(rob_rec *rob)
{
  user_back_data_rec *p;
  char difficulty;
  char round;
  
  p = &user_back_data;
  p->bookkeep.chara_play_counts[rob->field_0x3][rob->chara]++;
  difficulty = p->rank_difficult;
  round = rank_round;
  p->bookkeep.course_results[difficulty][round]++;
  if ((game_flags & 0x40000) == 0) {
    p->bookkeep.course_results_no_continue[difficulty][round]++;
  }
  user_back_data.bookkeep.games_total = user_back_data.bookkeep.games_total + 1;
  user_back_data.bookkeep.games_1p = user_back_data.bookkeep.games_1p + 1;
}

void alt_mode_clear_stats_add(rob_rec *rob)
{
  bookkeep_rec *p = &user_back_data.bookkeep;
  p->chara_play_counts[rob->field_0x3][rob->chara]++;
  p->games_total++;
  p->games_1p++;
}

void vs_time_stats_add(void)
{
  user_back_data.bookkeep.vs_time = user_back_data.bookkeep.vs_time + vs_play_frames;
  if (((user_back_data.bookkeep.shortest_vs >= vs_play_frames) || (user_back_data.bookkeep.shortest_vs == 0)
      ) && (vs_play_frames != 0)) {
    user_back_data.bookkeep.shortest_vs = vs_play_frames;
  }
  if (user_back_data.bookkeep.longest_vs < vs_play_frames) {
    user_back_data.bookkeep.longest_vs = vs_play_frames;
  }
  return;
}

void play_time_account(void)
{
  e_mode_main main_mode;
  int sub_mode;
  int frames;
  bookkeep_rec *p;
  
  frames = 0;
  p = &user_back_data.bookkeep;
  if (vblank_count > play_time_last_vblank) {
    frames = vblank_count - play_time_last_vblank;
  }
  play_time_last_vblank = vblank_count;
  p->total_time = p->total_time + frames;
  if ((byte)(play_style - 1U) < 2) {
    p->play_time = p->play_time + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_SELECTOR) && (sub_mode = mode_get_sub(), sub_mode == 2)) {
    select_frames = 1;
  }
  main_mode = mode_get_main();
  if ((((main_mode == MODE_MAIN_SELECTOR) && (sub_mode = mode_get_sub(), sub_mode == 3)) ||
      ((main_mode = mode_get_main(), main_mode == MODE_MAIN_SELECTOR && (sub_mode = mode_get_sub(), sub_mode == 5)
       ))) || ((main_mode = mode_get_main(), main_mode == MODE_MAIN_SELECTOR &&
               (sub_mode = mode_get_sub(), sub_mode == 6)))) {
    select_frames = select_frames + frames;
  }
  main_mode = mode_get_main();
  if (((main_mode == MODE_MAIN_SELECTOR) && (sub_mode = mode_get_sub(), sub_mode == 6)) ||
     ((main_mode = mode_get_main(), main_mode == MODE_MAIN_SELECTOR && (sub_mode = mode_get_sub(), sub_mode == 5))
     )) {
    p->field_0xb80 = p->field_0xb80 + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 0)) {
    mask_load_frames = 1;
  }
  main_mode = mode_get_main();
  if (((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 2)) ||
     ((main_mode = mode_get_main(), main_mode == MODE_MAIN_GAME && (sub_mode = mode_get_sub(), sub_mode == 3)))) {
    mask_load_frames = mask_load_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 4)) {
    stage_intro_frames = 1;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 5)) {
    stage_intro_frames = stage_intro_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 8)) {
    round_intro_frames = 1;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 9)) {
    round_intro_frames = round_intro_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 0xc)) {
    judge_frames = 1;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 0xd)) {
    judge_frames = judge_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 0xe)) {
    replay_frames = replay_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_GAME) && (sub_mode = mode_get_sub(), sub_mode == 0xf)) {
    winner_frames = winner_frames + frames;
  }
  if (streak_challenge_state == 1) {
    streak_challenge_pending_frames = streak_challenge_pending_frames + frames;
  }
  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_ADVERTISE) || (win_streak == 0)) {
    streak_challenge_state = 0;
    streak_challenge_pending_frames = 0;
  }
  if ((streak_challenge_state == 2) && (win_streak != 0)) {
    user_back_data.streak_challenge_frames = user_back_data.streak_challenge_frames + streak_challenge_pending_frames;
    user_back_data.streak_challenge_count = user_back_data.streak_challenge_count + 1;
    
    streak_challenge_state = 0;
    streak_challenge_pending_frames = 0;
  }
  if (play_style == '\x02') {
    p->vs_play_time = p->vs_play_time + frames;
    vs_play_frames = vs_play_frames + frames;
  }
  if (play_style == '\x01') {
    p->p1_time = p->p1_time + frames;
    p1_play_frames_max = p1_play_frames_max + frames;
    p1_play_frames_min = p1_play_frames_min + frames;
  }
  else {
    main_mode = mode_get_main();
    if (main_mode == MODE_MAIN_ADVERTISE) {
      p1_play_frames_max = 0;
      p1_play_frames_min = 0;
    }
  }
  if (((play_style == '\x02') && (main_mode = mode_get_main(), main_mode == MODE_MAIN_GAME)) &&
     (sub_mode = mode_get_sub(), sub_mode == 10)) {
    vs_round_time = vs_round_time + frames;
  }
  if (((play_style == '\x01') && (main_mode = mode_get_main(), main_mode == MODE_MAIN_GAME)) &&
     (sub_mode = mode_get_sub(), sub_mode == 10)) {
    p1_round_time = p1_round_time + frames;
  }
  return;
}

void ledger_daily_baseline(void)
{
  ledger_snapshot_rec *p = &ledger_snapshot;
  g1_dma_spin_idle();
  sram_bank_snapshot((uint *)p);
  ledger_baseline_day = user_back_data.ledger_day;
  ledger_baseline.ledger_28 = p->coin_1p;
  ledger_baseline.ledger_2c = p->coin_2p;
  ledger_baseline_coin_total = p->coin_total;
  ledger_baseline_credit_coin = p->credit_coin;
  ledger_baseline_credit_service = p->credit_service;
  ledger_baseline_credit_total = p->credit_total;
}

void ledger_daily_update(void)
{
  ledger_snapshot_rec *p = &ledger_snapshot;
  tm t;
  ledger_day_rec *r;

  g1_dma_spin_idle();
  sram_bank_snapshot((uint *)p);
  get_now_tm(&t);
  user_back_data.ledger_day = t.tm_mday;
  if (ledger_baseline_day != 0 && ledger_baseline_day != t.tm_mday) {
    user_back_data.ledger_record++;
    if (user_back_data.ledger_record > 29) user_back_data.ledger_record = 0;
    ledger_day_rec *q = &user_back_data.ledger_daily_records[user_back_data.ledger_record];
    q->coin_1p = 0;
    q->coin_2p = 0;
    q->coin_total = 0;
    q->credit_coin = 0;
    q->credit_service = 0;
    q->credit_total = 0;
    q->year = 0;
    q->month = 0;
    q->day = 0;
  }
  if (user_back_data.ledger_record > 29) user_back_data.ledger_record = 0;
  r = &user_back_data.ledger_daily_records[user_back_data.ledger_record];
  r->year = t.tm_year;
  r->month = t.tm_mon;
  r->day = t.tm_mday;
  if (ledger_baseline.ledger_28 < p->coin_1p) r->coin_1p += p->coin_1p - ledger_baseline.ledger_28;
  if (ledger_baseline.ledger_2c < p->coin_2p) r->coin_2p += p->coin_2p - ledger_baseline.ledger_2c;
  if (ledger_baseline_coin_total < p->coin_total) r->coin_total += p->coin_total - ledger_baseline_coin_total;
  if (ledger_baseline_credit_coin < p->credit_coin) r->credit_coin += p->credit_coin - ledger_baseline_credit_coin;
  if (ledger_baseline_credit_service < p->credit_service) r->credit_service += p->credit_service - ledger_baseline_credit_service;
  if (ledger_baseline_credit_total < p->credit_total) r->credit_total += p->credit_total - ledger_baseline_credit_total;
  ledger_baseline_day = t.tm_mday;
  ledger_baseline.ledger_28 = p->coin_1p;
  ledger_baseline.ledger_2c = p->coin_2p;
  ledger_baseline_coin_total = p->coin_total;
  ledger_baseline_credit_coin = p->credit_coin;
  ledger_baseline_credit_service = p->credit_service;
  ledger_baseline_credit_total = p->credit_total;
}
