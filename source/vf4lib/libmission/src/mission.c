/*
 * vf4lib/libmission/src/mission.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char prize_test_profiles[220][8]; /* 0x0c1977ec */
extern uchar *bonus_item_lists[19]; /* 0x0c19808c */
extern uchar *char_item_lists[19]; /* 0x0c198b30 */
extern uchar *char_item_lists_alt[19]; /* 0x0c198fbc */
extern uchar *item_rank_lists[4][19]; /* 0x0c1997c8 */
extern int mission_chance_by_round[14]; /* 0x0c1998f8 */
extern int mission_reward_money[10]; /* 0x0c199930 */
extern int mission_reward_items[10][19]; /* 0x0c199958 */
extern int mission_late_round_picks[16]; /* 0x0c199c50 */
extern uchar *prize_money_lists[3][4][6]; /* 0x0c199d80 */
extern int mission_table[30][6]; /* 0x0c19ba58 */
extern int lottery_bonus_odds[23]; /* 0x0c19bd28 */
extern int lottery_symbol_values[23]; /* 0x0c19bd84 */
extern uchar *quest_checks_0[16]; /* 0x0c19bf14 */
extern uchar *quest_checks_1[16]; /* 0x0c19bf54 */
extern uchar *quest_checks_2[6]; /* 0x0c19bf94 */
extern int quest_rank_targets[16]; /* 0x0c19bfac */
extern int quest_total_targets[16]; /* 0x0c19bfec */
extern int game_frame_count; /* 0x0c292540 */
extern int round_time; /* 0x0c292558 */
extern uint match_round_time; /* 0x0c29255c */
extern uint game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern int win_streak; /* 0x0c2925dc */
extern uint match_stage; /* 0x0c2926dc */
extern int prize_test_runs; /* 0x0c2a0660 */
extern int mission_count; /* 0x0c2a0680 */
extern int mission_result; /* 0x0c2a0694 */
extern short mission_last_health; /* 0x0c2a0698 */
extern int mission_air_combo_seen[2]; /* 0x0c2a069c */
extern int mission_jump_state[2]; /* 0x0c2a06a4 */
extern int mission_down_seen[2]; /* 0x0c2a06ac */
extern int mission_first_hit[2]; /* 0x0c2a06b4 */
extern int mission_combo_hits[2]; /* 0x0c2a06bc */
extern int mission_last_combo; /* 0x0c2a06c4 */
extern int bonus_time_flags; /* 0x0c2a06c8 */
extern int bonus_time_left; /* 0x0c2a06cc */
extern int lottery_state; /* 0x0c2a06d0 */
extern int lottery_symbol; /* 0x0c2a06d4 */
extern int lottery_spins; /* 0x0c2a06d8 */
extern int bonus_walls_left; /* 0x0c2a06dc */
extern int bonus_low_health_flag; /* 0x0c2a06e0 */
extern int bonus_round_time_flag; /* 0x0c2a06e4 */
extern int bonus_eight_chars_flag; /* 0x0c2a06e8 */
extern int bonus_all_chars_flag; /* 0x0c2a06ec */
extern uint bonus_beaten_chars; /* 0x0c2a06f0 */
extern int evade_latch; /* 0x0c2a06f4 */
extern int quest_streak_flag[2]; /* 0x0c2a06f8 */
extern int mission_combo_broken; /* 0x0c2a0700 */
extern int quest_meter_changed[2]; /* 0x0c2a0704 */
extern int prize_item_fallback[2]; /* 0x0c2a0714 */
extern int prize_got_item; /* 0x0c2a071c */
extern int prize_got_money; /* 0x0c2a0720 */
extern uchar *prize_loss_tiers[3][19]; /* 0x0c2a0724 */
extern uchar *prize_pace_tiers[3][19]; /* 0x0c2a0808 */
extern ushort quest_count[2]; /* 0x0c2a08fc */
extern int prize_item_pool[2][32]; /* 0x0c450e0c */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern int mission_tally[2]; /* 0x0c2a0684 */
extern int mission_of_side[2]; /* 0x0c2a068c */
extern int prize_item_chance[2]; /* 0x0c2a070c */
extern int prize_item[2]; /* 0x0c2a08ec */
extern int prize_money[2]; /* 0x0c2a08f4 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern int lottery_reels[3]; /* 0x0c450f0c */

void bonus_lottery_step(void)
{
  int i;
  int roll = 0;
  int odds;
  int sum;

  net_info_get();
  if (play_style != '\x02') {
    return;
  }
  if (net_ready_latched() == 0) {
    return;
  }
  if (net_free_play_is_on() != 0) {
    return;
  }
  if ((bonus_time_flags & 1) != 0) {
    lottery_state = -1;
    lottery_symbol = -1;
    lottery_spins = roll;
    for (i = 0; i < 3; i++) {
      lottery_reels[i] = 0xffffffff;
    }
    return;
  }
  if (lottery_state < 0) {
    lottery_symbol = -1;
    lottery_spins = 0;
    roll = lcg_next15();
    odds = net_syuchu_value1_get();
    if (odds == 0) {
      odds = 400;
    }
    if (roll < 0x7fff / odds) {
      lottery_state = 0x0;
      user_back_data.lottery_starts = user_back_data.lottery_starts + 1;
      for (i = 0; i < 3; i++) {
        lottery_reels[i] = 0xffffffff;
      }
    }
  }
  if (lottery_state == 0x0) {
    for (i = 0; i < 3; i++) {
      if ((int)lottery_reels[i] < 0) {
        lottery_symbol = lottery_symbol_draw();
        bonus_time_roll();
        lottery_reels[i] = lottery_symbol_values[lottery_symbol];
        if (i == 2) {
          lottery_state = 0x1;
        }
        break;
      }
    }
  }
  if ((bonus_time_flags & 1) != 0) {
    return;
  }
  if (lottery_state == 0x1) {
    sum = lottery_reels[0] + lottery_reels[1] +
            lottery_reels[2];
    if (sum < 8) {
      lottery_spins = (int)lcg_next15() % 6;
    }
    else if (sum >= 0x10) {
      lottery_spins = (int)lcg_next15() % 4;
    }
    else {
      lottery_spins = (int)lcg_next15() % 2;
    }
    if (lottery_spins < 1) goto stop;
    lottery_state = 0x2;
  }
  if (lottery_state != 0x2) {
    return;
  }
  user_back_data.lottery_spins = user_back_data.lottery_spins + 1;
  lottery_spins = lottery_spins + -1;
  if (-1 < lottery_spins) {
    lottery_symbol = lottery_symbol_draw();
    bonus_time_roll();
  }
  if (0 < lottery_spins) {
    return;
  }
  roll = (int)lcg_next15() % 10;
  lottery_spins = lottery_spin_lists[lottery_symbol_values[lottery_symbol]][roll];
  if (0 < lottery_spins) {
    return;
  }
stop:
  lottery_state = -1;
}

void bonus_time_roll(void)
{
  int roll;

  roll = lcg_next15();
  if (roll < 32767 / (400 / lottery_bonus_odds[lottery_symbol])) {
    bonus_time_flags = bonus_time_flags | 1;
    sound_state_bit6_set();
    user_back_data.bonus_time_starts = user_back_data.bonus_time_starts + 1;
    bonus_time_left = (int)lcg_next15() % 21 + 10;
    if (user_back_data.bonus_time_longest < bonus_time_left) {
      user_back_data.bonus_time_longest = bonus_time_left;
    }
    if (user_back_data.bonus_time_shortest == 0) {
      user_back_data.bonus_time_shortest = bonus_time_left;
    }
    else if (user_back_data.bonus_time_shortest > bonus_time_left) {
      user_back_data.bonus_time_shortest = bonus_time_left;
    }
  }
}

int lottery_symbol_draw(void)
{
  int symbol;

  symbol = (int)lcg_next15() % 12;
  switch (symbol) {
  case 1:
  case 2:
  case 7:
    if ((int)lcg_next15() < 0x3fff) {
      if (symbol == 1) symbol = 0xc;
      else if (symbol == 2) symbol = 0xd;
      else if (symbol == 7) symbol = 0x12;
    }
    break;
  case 5:
  case 10:
    if ((int)lcg_next15() < 0x3fff) {
      if (symbol == 5) symbol = 0x10;
      else if (symbol == 10) symbol = 0x15;
    }
    break;
  case 4:
  case 9:
    if ((int)lcg_next15() < 0x2aaa) {
      if (symbol == 4) symbol = 0xf;
      else if (symbol == 9) symbol = 0x14;
    }
    break;
  case 6:
  case 11:
    if ((int)lcg_next15() < 0x1999) {
      if (symbol == 6) symbol = 0x11;
      else if (symbol == 11) symbol = 0x16;
    }
    break;
  case 3:
  case 8:
    if ((int)lcg_next15() < 0xccc) {
      if (symbol == 3) symbol = 0xe;
      else if (symbol == 8) symbol = 0x13;
    }
    break;
  case 0:
  default:
    break;
  }
  return symbol;
}

int lottery_symbol_get(void)
{
  return lottery_symbol;
}

void mission_round_init(void)
{
  mission_round_flags_clear();
  return;
}

bool mission_blocked(void)
{
  int i;
  int blocked;
  card_record_rec *records[2];
  blocked = 0;
  mission_result = 0;
  records[0] = card_record_get(0);
  records[1] = card_record_get(1);
  if (play_style == 2) {
    if (jisaku_flag_get() != 0) blocked = 1;
    if ((records[0]->in_use == 0) || (records[1]->in_use == 0)) {
      blocked = 1;
    }
  }
  else {
    for (i = 0; i < 2; i++) {
      if ((side_is_player(i) != 0) && (records[i]->in_use == 0)) {
        blocked = 1;
      }
    }
  }
  if (net_ready_latched() == 0) blocked = 1;
  if (net_free_play_is_on() != 0) blocked = 1;
  for (i = 0; i < 2; i++) {
    if (level_change_flags_get(i) != 0) blocked = 1;
  }
  if (msnm_active() != 0) blocked = 1;
  return blocked;
}

int mission_wall_stage_fix(int mission)
{
  int result;
  int stage;
  result = mission;
  stage = match_stage;
  if (stage == 4 || stage == 0x10 || stage == 0) {
  }
  else if ((game_frame_count & 1U) != 0) {
    result = 0;
  }
  else {
    result = 7;
  }
  return result;
}

int mission_ring_out_stage_fix(int mission)
{
  if (((match_stage == 0x16 || match_stage == 0x14) || (match_stage == 0x13)) ||
     (match_stage == 3)) {
    int parity = game_frame_count & 1; mission = (1 ^ parity) + 0x1b;
  }
  return mission;
}

int ranks_adjacent(uint rank_a, uint rank_b)
{
  uint a;
  int adjacent;
  uint b;
  
  a = rank_a & 0xff;
  b = rank_b & 0xff;
  adjacent = 0;
  if ((a == 10) && ((b - 10 & 0xff) < 2)) {
    adjacent = 1;
  }
  if ((b == 10) && ((a - 10 & 0xff) < 2)) {
    adjacent = 1;
  }
  if ((a == 0x13) && ((b - 0x12 & 0xff) < 2)) {
    adjacent = 1;
  }
  if ((b == 0x13) && ((a - 0x12 & 0xff) < 2)) {
    adjacent = 1;
  }
  if ((a - 0xb & 0xff) < 8) {
    if (a - b == 1) {
      adjacent = 1;
    }
    if (b - a == 1) {
      adjacent = 1;
    }
    if (a == b) {
      adjacent = 1;
    }
  }
  return adjacent;
}

byte ranks_matched(uint rank_a, uint rank_b)
{
  uint a;
  uint b;
  byte matched;
  
  a = rank_a & 0xff;
  b = rank_b & 0xff;
  matched = 0;
  if ((a < 10) & (b < 10)) matched = 1;
  if (((a - 10 & 0xff) < 10) && ((b - 10 & 0xff) < 10) && (ranks_adjacent(a,b) != 0)) matched = 1;
  if (0x13 < a) {
    if (a == b) matched = 1;
  }
  if ((a == 0) | (b == 0)) matched = 0;
  if ((a > 29) | (b > 29)) matched = 0;
  return matched;
}

int prize_loss_tier(uint side)
{
  int tier;
  int owned;
  int losses;
  int item_count;
  int i;
  int *row;
  int cls;
  int j;
  int count;
  
  tier = 0;
  losses = card_image_get(side)->header.all_lose;
  item_count = char_item_count(side);
  if (item_count == 0) {
    item_count = 10;
  }
  owned = char_items_owned(side);
  if (owned > item_count) {
    owned = item_count;
  }
  i = net_struggle_revise_get(0);
  row = (int *)prize_loss_tiers[i][card_image_char_get(side & 0xff)];
  for (i = 0; i < owned; i++) {
    if (row[2] > 0) {
      row = row + 2;
    }
  }
  cls = player_rank_class(side);
  j = 0;
  count = 0;
  for (; cls >= 0; j = 0) {
    for (; ((int *)item_rank_lists[cls][card_image_char_get(side & 0xff)])[j] > 0; j++) {
      count++;
    }
    cls--;
  }
  if (owned < count && *row > 0) {
    if (*row <= losses) tier = 1;
    if (row[1] <= losses) tier = 2;
  }
  return tier;
}

int char_item_count(uint side)
{
  int n = 0;
  int *p;
  for (p = (int *)char_item_lists[card_image_char_get(side & 0xff)]; *p > 0; p += 2) n++;
  return n;
}

int char_items_owned(uint side)
{
  int n = 0;
  int owned;
  int *p;
  for (p = (int *)char_item_lists[card_image_char_get(side & 0xff)]; *p > 0; p += 2) {
    owned = item_has_unchecked(side, *p);
    n += (uint)(owned != 0);
  }
  return n;
}

uchar prize_pace_tier(uint side)
{
  int n;
  int owned;
  int chara;
  int *row;
  uint matches;
  int i;
  uchar tier;
  card_image_rec *card;
  
  tier = 0;
  card = card_image_get(side);
  matches = (ushort)(card->header.all_lose + card->header.all_win);
  n = char_item_count(side);
  if (n == 0) {
    n = 10;
  }
  owned = char_items_owned(side);
  if (owned > n) {
    owned = n;
  }
  n = net_struggle_revise_get(1);
  chara = card_image_char_get(side & 0xff);
  row = (int *)(&prize_pace_tiers[0][0])[n * 0x13 + chara];
  for (i = 0; i < owned; i++) {
    if (0 < row[2]) {
      row = row + 2;
    }
  }
  if (0 < *row) {
    if (*row >= (int)matches) {
      tier = 1;
    }
    if (row[1] >= (int)matches) {
      tier = 2;
    }
  }
  return tier;
}

int prize_rate_get(int revise_no, int percent)
{
  if ((int)net_struggle_revise_get(revise_no) <= 0)
    return (int)((float)percent * 327.67);
  if ((int)net_struggle_revise_get(revise_no) >= 100)
    return (int)((float)percent * 327.67);
  return (int)((float)(int)net_struggle_revise_get(revise_no) * 327.67);
}

void prize_chances_set(uint rank_a, uint rank_b)
{
  typedef struct { int m[2]; } chance_pair;
  int i;
  uint a = rank_a & 0xff;
  uint b = rank_b & 0xff;
  card_image_rec *cards[2];

  for (i = 0; i < 2; i++) {
    cards[i] = card_image_get(i);
  }
  if (win_streak >= 30) {
    if ((uchar)champion_side == 0) {
      prize_item_chance[0] = prize_rate_get(9,10);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(9,10);
    }
  }
  else if (win_streak >= 20 && win_streak < 30) {
    if ((uchar)champion_side == 0) {
      prize_item_chance[0] = prize_rate_get(8,8);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(8,8);
    }
  }
  else if (win_streak >= 10 && win_streak < 20) {
    if ((uchar)champion_side == 0) {
      prize_item_chance[0] = prize_rate_get(7,6);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(7,6);
    }
  }
  else {
    prize_item_chance[0] = prize_rate_get(3,4);
    prize_item_chance[1] = prize_rate_get(3,4);
  }
  if (ranks_matched(a,b) != 0) {
    for (i = 0; i < 2; i++) {
      if (((chance_pair *)&prize_item_chance)->m[i] < prize_rate_get(4,8)) {
        ((chance_pair *)&prize_item_chance)->m[i] = prize_rate_get(4,8);
      }
    }
    for (i = 0; i < 2; i++) {
      if (prize_loss_tier(i) == 1) {
        ((chance_pair *)&prize_item_chance)->m[i] = prize_rate_get(5,0xc);
      }
      else if (prize_loss_tier(i) == 2) {
        ((chance_pair *)&prize_item_chance)->m[i] = prize_rate_get(6,0x19);
      }
    }
  }
  for (i = 0; i < 2; i++) {
    if (card_body_prize_misses_get(&cards[i]->body) > 29) {
      ((chance_pair *)&prize_item_chance)->m[i] = prize_rate_get(0x10,0x50);
    }
  }
}

void mission_choose(void)
{
  int rnd;
  int i;
  int odds;
  int t;
  short *counter;
  tm now;
  card_image_rec *cards[2];
  int side;
  int kind;

  rnd = lcg_next15();
  side = 0;
  kind = 0;
  get_now_tm(&now);
  mission_result = side;
  cards[0] = card_image_get(0);
  cards[1] = card_image_get(1);
  if (mission_blocked() != 0) {
    mission_clear();
    return;
  }
  if (play_style == 1) {
    {
      int player = side_is_player(0);
      int is_player = player == 1;
      side = is_player == 0;
    }
    if (rnd > mission_chance_by_round[(char)rank_round]) {
      mission_clear();
      return;
    }
    mission_of_side[(side & 1) ^ 1] = -1;
    rnd = rnd % 30;
    if (rnd == 3) {
      rnd = mission_wall_stage_fix(3);
    }
    else if (rnd == 11) {
      rnd = mission_ring_out_stage_fix(11);
    }
    else if (rnd == 25) {
      if (((int *)&player_chara)[side] == 9) {
        t = game_frame_count & 1;
        rnd = (t ^ 1) + 27;
      }
    }
    else if (rnd == 2) {
      t = ((int *)&player_chara)[(side & 1) ^ 1];
      if (((t == 4) | (t == 9)) || t == 13) {
        t = game_frame_count & 1;
        rnd = (t ^ 1) + 27;
      }
    }
    if ((char)rank_round > 4 && cards[side]->header.level < 13) {
      rnd = mission_late_round_picks[(int)lcg_next15() % 16];
    }
    if (mission_select != OFF_ON_OFF && mission_no < 30) {
      rnd = mission_no;
    }
    mission_of_side[side] = rnd;
    if ((((mission_rec *)&mission_table)[rnd].flags & 0x800) != 0) {
      mission_count = (user_back_data._1p_match_point * ((mission_rec *)&mission_table)[rnd].value);
    }
    else {
      mission_count = ((mission_rec *)&mission_table)[mission_of_side[side]].value;
    }
    user_back_data.mission_offered[rnd]++;
  }
  else {
    prize_item_chance[0] = 1;
    prize_item_chance[1] = 1;
    prize_chances_set(cards[0]->header.level,cards[1]->header.level);
    for (i = 0; i < 2; i++) {
      if (card_body_syu_low5_get(&cards[i]->body) != now.tm_mday) {
        card_body_syu_low5_set(&cards[i]->body,0);
      }
    }
    for (i = 0; i < 2; i++) {
      if (card_body_syu_low5_get(&cards[i]->body) == 0) {
        odds = net_syuchu_value0_get();
        if (odds == 0) {
          odds = 800;
        }
        if ((int)lcg_next15() < 0x7fff / odds) {
          user_back_data.syuchu_marks++;
          card_body_syu_low5_set(&cards[i]->body,(ushort)now.tm_mday);
        }
      }
    }
    if ((int)bonus_time_left < 1) {
      bonus_time_flags &= ~1;
    }
    else {
      bonus_time_left--;
      user_back_data.bonus_time_games++;
    }
    for (i = 0; i < 2; i++) {
      if (card_body_syu_low5_get(&cards[i]->body) != 0 && (int)lcg_next15() < 0xccc) {
        card_body_syu_low5_set(&cards[i]->body,0);
      }
    }
    if (bonus_time_flags != 0) {
      kind = 1;
      for (i = 0; i < 2; i++) {
        if (prize_item_chance[i] < 0x4cc8) {
          prize_item_chance[i] = 0x4cc8;
        }
      }
    }
    for (i = 0; i < 2; i++) {
      if (card_body_syu_low5_get(&cards[i]->body) != 0) {
        kind = 2;
        if (prize_item_chance[i] < 0x4cc8) {
          prize_item_chance[i] = 0x4cc8;
        }
      }
    }
    for (i = 0; i < 2; i++) {
      if ((int)lcg_next15() >= prize_item_chance[i]) {
        mission_of_side[i] = -1;
      }
      else {
        {
          int *missions = mission_of_side;
          missions[i] = 29;
        }
        side = i;
      }
    }
    if (mission_of_side[side] >= 0) {
      user_back_data.prize_matches++;
      if (kind == 0) {
        user_back_data.prize_matches_plain++;
      }
      else {
        if (kind == 1) {
          counter = &user_back_data.prize_matches_bonus_time;
        }
        else if (kind == 2) {
          counter = &user_back_data.prize_matches_syuchu;
        }
        else if (kind == 3) {
          counter = &user_back_data.prize_matches_kind3;
        }
        else {
          goto skip;
        }
        (*counter)++;
      }
    skip:
      mission_count = ((mission_rec *)&mission_table)[mission_of_side[side]].value;
    }
  }
  mission_result = 0;
  mission_tally[0] = 0;
  mission_tally[1] = 0;
}

void mission_frame_check(void)
{
  int i;
  uint flags;

  if ((mode_get_main() == MODE_MAIN_GAME) && (mode_get_sub() == 10)) {
    for (i = 0; i < 2; i++) {
      mission_rec *B;
      mission_rec *A = (mission_rec *)&mission_table;
      if (mission_of_side[i] == -1) continue;
      flags = A[mission_of_side[i]].flags;
      if ((flags & 4) != 0) continue;
      if ((flags & 0x20) != 0) {
        if ((flags & 0x40) == 0) continue;
      }
      B = (mission_rec *)&mission_table;
      B[mission_of_side[i]].check(i);
      if ((B[mission_of_side[i]].flags & 1) != 0) {
        if (mission_count < 0) mission_result = 0xffffffff;
      }
      else {
        mission_rec *T = (mission_rec *)&mission_table;
        if ((T[mission_of_side[i]].flags & 2) != 0) {
          if (mission_count <= 0) mission_result = 0xffffffff;
        }
        else if ((((mission_rec *)&mission_table)[mission_of_side[i]].flags & 0x40) == 0) {
          if (mission_count == 0) mission_result = 1;
          else if (mission_count < 0) mission_result = 0xffffffff;
        }
      }
    }
  }
}

void mission_judge(void)
{
#define TBL ((mission_rec *)&mission_table)
#define ST (mission_of_side)
#define RK (mission_tally)
  int i;

  for (i = 0; i < 2; i++) {
    if (ST[i] == -1) continue;
    if (ST[i] == 4) {
      mission_quick_win_check(i);
      if (mission_count == 0) mission_result = 1;
    }
    else if (TBL[ST[i]].flags & 4) {
      TBL[ST[i]].check(i);
    }
    if (TBL[ST[i]].flags & 8) {
      if (TBL[ST[i]].flags & 0x10) {
        if (mission_count == 0) mission_result = -1;
      }
      else {
        if (mission_count == 0) mission_result = 1;
      }
    }
    if ((game_flags & 0x400) == 0) return;
    if (mission_result != 0) {
      if (!(TBL[ST[i]].flags & 0x20)) {
        if (!(TBL[ST[i]].flags & 0x80)) goto next;
      }
    }
    if (TBL[ST[i]].flags & 1) {
      if (champion_side != i) mission_result = -1;
      else if (mission_count >= 0) mission_result = 1;
    }
    else {
     mission_rec *table = TBL;
     int *missions = ST;
     uint f = table[missions[i]].flags;
     if (f & 2) {
      if (f & 0x100) {
        if (mission_sides_get() == 2) {
          if (i != 0) goto next;
          table[missions[0]].check(0);
        }
        else TBL[ST[i]].check(i);
      }
      else {
        if (TBL[ST[i]].flags & 0x80) {
          if (champion_side == i) {
            if (mission_count > 0) mission_result = 1;
          }
          else mission_result = -1;
          goto next;
        }
        TBL[ST[i]].check(i);
      }
      if (mission_count > 0) mission_result = 1;
      else mission_result = -1;
    }
    else if (TBL[ST[i]].flags & 0x10) {
      if (mission_count != 0) mission_result = 1;
    }
    else if (TBL[ST[i]].flags & 0x20) {
      if (RK[i] > RK[(i & 1) ^ 1]) mission_result = 1;
      else mission_result = -1;
    }
    else if (mission_count > 0) mission_result = -1;
    else if (mission_count == 0) mission_result = 1;
    }
  next:
    prize_got_item = -1;
    prize_got_money = -1;
    if (play_style == 1) mission_reward_give(i);
  }
  if ((game_flags & 0x400) && play_style != 1 && (int)mission_active_get() != -1) {
    prize_got_item = -1;
    prize_got_money = -1;
    prize_give(champion_side);
  }
#undef TBL
#undef ST
#undef RK
}

void mission_hud_draw(void)
{
  float x_2p, y, x_1p;
  int alt_layout;
  int i;

  if (mode_get_sub() == 4 || mode_get_sub() == 5 || mode_get_sub() == 7) {
    x_2p = 492.0f;
    y = 360.0f;
    x_1p = 140.0f;
    alt_layout = 1;
  }
  else {
    x_2p = 496.0f;
    y = 424.0f;
    x_1p = 144.0f;
    alt_layout = 0;
  }
  if (mission_of_side[0] == -1 && mission_of_side[1] == -1) return;
  for (i = 0; i < 2; i++) {
    if (i == 0) {
      if (side_is_player(0) != 0 && play_style == 1) {
        if (alt_layout == 0) {
          prize_bar_1p_draw();
        }
        if (mode_get_sub() < 10) {
          prize_plate_1p_draw(alt_layout);
        }
      }
      else if (play_style == 2) {
        if (alt_layout == 0) {
          prize_bar_1p_draw();
        }
        if (mode_get_sub() < 10) {
          prize_plate_1p_draw(alt_layout);
        }
      }
    }
    else {
      if (side_is_player(1) != 0 && play_style == 1) {
        if (alt_layout == 0) {
          prize_bar_2p_draw();
        }
        if (mode_get_sub() < 10) {
          prize_plate_2p_draw(alt_layout);
        }
      }
      else if (play_style == 2) {
        if (alt_layout == 0) {
          prize_bar_2p_draw();
        }
        if (mode_get_sub() < 10) {
          prize_plate_2p_draw(alt_layout);
        }
      }
    }
    if (mission_of_side[i] == -1 && play_style == 2) {
      mission_result_draw(x_2p,y,x_1p,i);
    }
    else if (mission_of_side[i] != -1) {
      if (mission_sides_get() == 2) {
        if (i == 0) {
          mission_text_draw(x_2p,y,x_1p,0,alt_layout);
        }
        else {
          mission_result_draw(x_2p,y,x_1p,i);
        }
      }
      else {
        mission_text_draw(x_2p,y,x_1p,i,alt_layout);
      }
    }
  }
}

void mission_result_draw(float x_2p, float y, float x_1p, int side)
{
  size_t len;
  char *text;
  
  if (*(volatile int *)&mission_result == -1) text = mission_success_banner;
  else if (*(volatile int *)&mission_result == 1) text = mission_failure_banner;
  else text = mission_win_match_banner;
  len = strlen(text);
  sprite_text_put(&euc_text_font,((float)side * (x_2p - x_1p) + x_1p) - (float)(len * 8 / 2),y,0.006,0,text);
  return;
}

void mission_text_draw(float x_2p, float y, float x_1p, uint side, int alt_layout)
{
  mission_rec *m;
  char *p;
  char *q;
  int i;
  int count;
  int lead;
  size_t len;
  char text[64];
  char count_text[64];
  char target_text[64];
  int count_width;

  count_width = 0;
  p = text;
  q = count_text;
  m = &((mission_rec *)&mission_table)[mission_of_side[side]];
  for (i = 0; i < 0x40; i++) {
    text[i] = 0;
    count_text[i] = 0;
    target_text[i] = 0;
  }
  if (*(volatile int *)&mission_result == -1) {
    snprintf(text, 0x40, mission_failure_label);
  }
  else if (*(volatile int *)&mission_result == 1) {
    snprintf(text, 0x40, mission_success_label);
  }
  else {
    p += sprintf(p, "%s", m->name);
    if (*m->suffix != '\0') {
      if (m->flags & 0x800) {
        p += sprintf(p, "%d", user_back_data._1p_match_point * m->value);
      }
      else {
        p += sprintf(p, "%d", m->value);
      }
      p += sprintf(p, "%s", m->suffix);
      if (alt_layout == 0 && (m->flags & 0x600)) {
        if (m->flags & 0x800) {
          if (m->flags & 0x400) {
            count = m->value * user_back_data._1p_match_point - mission_count;
          }
          else {
            count = mission_count;
          }
          sprintf(q, mission_count_format, count);
        }
        else {
          if (m->flags & 0x400) {
            count = m->value - mission_count;
          }
          else {
            count = mission_count;
          }
          sprintf(q, mission_count_format, count);
        }
        if (m->flags & 1) {
          sprintf(p, mission_seconds_padded_text);
          sprintf(target_text, mission_seconds_text);
        }
        else if (m->flags & 0x800) {
          sprintf(p, "     /%d", user_back_data._1p_match_point * m->value);
          sprintf(target_text, slash_number_format, user_back_data._1p_match_point * m->value);
        }
        else {
          sprintf(p, "     /%d", m->value);
          sprintf(target_text, slash_number_format, m->value);
        }
        count_width = (char)(strlen(target_text) * 8);
      }
    }
    else if (m->flags & 0x40) {
      if (alt_layout == 0) {
      {
        int *tally = mission_tally;
        lead = tally[side] - tally[(side & 1) ^ 1];
      }
      sprintf(p, "      ");
      if (lead > 0) {
        q += sprintf(q, plus_sign_text);
      }
      else if (lead < 0) {
        q += sprintf(q, mission_minus_text);
        lead = -lead;
      }
      sprintf(q, two_digit_format, lead);
      }
    }
  }
  len = strlen(text);
  if (m->flags & 0x800) {
    sprintf(target_text, slash_number_format, user_back_data._1p_match_point * m->value);
  }
  else {
    sprintf(target_text, slash_number_format, m->value);
  }
  sprite_text_put(&euc_text_font, ((float)(int)side * (x_2p - x_1p) + x_1p) - (float)(len * 8 / 2), y, 0.006, 0, (byte *)text);
  sprite_colors_set(0x00ffff60, 0);
  sprite_text_put(&euc_text_font, ((float)(int)side * (x_2p - x_1p) + x_1p + (float)(len * 8 / 2)) - (float)count_width, y, 0.006, 2, (byte *)count_text);
  sprite_colors_reset();
}

void mission_clear(void)
{
  mission_of_side[0] = -1;
  mission_of_side[1] = -1;
  return;
}

int mission_active_get(void)
{
  int i;
  int m;
  
  m = -1;
  i = 0;
  do {
    if (mission_of_side[i] != -1) {
      m = mission_of_side[i];
    }
    i = i + 1;
  } while (i < 2);
  return m;
}

uint mission_sides_get(void)
{
  if (mission_of_side[0] != -1 &&
      mission_of_side[1] != -1) return 2;
  if (mission_of_side[0] != -1) return 0;
  return -(mission_of_side[1] == -1) | 1;
}

int mission_result_get(void)
{
  return mission_result;
}

void mission_reward_give(uint side)
{
  int mission;
  int reward;
  int chara;
  mission = mission_of_side[side];
  if (mission != -1 && mission_result == 1) {
    reward = ((mission_rec *)&mission_table)[mission].reward;
    if (reward <= 6) {
      card_money_add(side, mission_reward_money[reward]);
      prize_got_money = mission_reward_money[reward];
    }
    else {
      chara = card_image_char_get(side & 0xff);
      prize_got_item = mission_reward_items[reward][chara];
      if (item_has_unchecked(side, prize_got_item) != 0) {
        card_money_add(side, mission_reward_money[reward]);
      prize_got_money = mission_reward_money[reward];
      }
      else if (prize_got_item >= 0)
        item_give_carded(side, prize_got_item);
    }
  }
}

void prize_choose(void)
{
  int rank;
  int side;
  int *item;
  char bank;
  
  if ((mission_of_side[0] != -1) ||
     (mission_of_side[1] != -1)) {
    {
    int *items;
    int *moneys;
    items = prize_item;
    items[1] = -1;
    items[0] = -1;
    moneys = prize_money;
    moneys[1] = -1;
    moneys[0] = -1;
    if ((mission_of_side[0] != -1) &&
       (mission_of_side[1] != -1)) {
      prize_item_choose(0);
      prize_item_choose(1);
      if (items[0] >= 0 && items[1] >= 0) {
        sou_bank_select(0xffffffff);
        prize_debug_value_set(0,0,*(short *)&items[0]);
        prize_debug_value_set(0,1,*(short *)&moneys[0]);
        prize_debug_value_set(1,0,*(short *)&items[1]);
        prize_debug_value_set(1,1,*(short *)&moneys[1]);
        prize_debug_value_set(2,2,0xfffffffe);
        return;
      }
      side = (uint)prize_item[0] >> 0x1f;
    }
    else {
      side = mission_of_side[0] <= 0;
      prize_item_choose(mission_of_side[0] <= 0);
    }
    }
    item = &prize_item[side];
    if (*item >= 0) {
      rank = item_rank_list_find(card_image_char_get((uchar)side),*item);
      if (rank != -1) {
        bank = rank + 2;
      }
      else {
        bank = 2;
      }
      sou_bank_select(bank);
    }
    else {
      rank = 0;
      sou_bank_select(0);
      prize_money_choose(side);
    }
    prize_money_choose((char)(side ^ 1));
    prize_debug_value_set(0,0,(int)(short)prize_item[0]);
    prize_debug_value_set(0,1,(int)(short)prize_money[0]);
    prize_debug_value_set(1,0,(int)(short)prize_item[1]);
    prize_debug_value_set(1,1,(int)(short)prize_money[1]);
    prize_debug_value_set(2,2,rank);
  }
  return;
}

void prize_miss_count_update(void)
{
  int i;
  uint misses;
  uint loser;
  card_image_rec *cards[2];
  
  loser = (int)(uchar)champion_side & 1U ^ 1;
  for (i = 0; i < 2; i++) {
    cards[i] = card_image_get(i);
  }
  if (play_style == '\x02') {
    misses = card_body_prize_misses_get(&cards[loser]->body);
    if (misses < 0x1e) {
      misses = (ushort)(misses + 1);
    }
    card_body_prize_misses_set(&cards[loser]->body,misses);
    if (-1 < (int)mission_sides_get()) {
      for (i = 0; i < 2; i++) {
        card_body_prize_misses_set(&cards[i]->body,0);
      }
    }
  }
  return;
}

uint prize_item_chance_get(char side)
{
  uint chance;
  uint self;
  uint other;
  
  self = (uint)side;
  other = (side & 1) ^ 1;
  chance = prize_rate_get(10,0x3c);
  if (prize_pace_tier(self) == 1) {
    chance = prize_rate_get(0xb,5);
  }
  else if (prize_pace_tier(self) == 2) {
    chance = prize_rate_get(0xc,2);
  }
  if ((bonus_time_flags != 0) && (70.0 <= (float)player_win_rate(self))) {
    chance = prize_rate_get(0xf,3);
  }
  if (prize_loss_tier(self) == 1) {
    chance = prize_rate_get(0xd,0x46);
  }
  else if (prize_loss_tier(self) == 2) {
    chance = prize_rate_get(0xe,0x5a);
  }
  if ((prize_loss_tier(self) == 0) || (prize_loss_tier(other) == 0)) {
    chance = chance & -(uint)(prize_loss_tier(other) == 0);
  }
  return chance;
}

void prize_item_choose(char side)
{
  int value;

  value = prize_item_chance_get(side);
  if ((int)lcg_next15() < value) {
    value = prize_item_pick(side);
    if (prize_item_roll(value,side) != 0) {
      prize_item[side] = value;
      return;
    }
  }
  prize_item[side] = -1;
}

void prize_money_choose(int side)
{
  char self;
  uint other;
  int band;
  int row;
  int *entry;
  card_image_rec *cards[2];
  
  self = (char)side;
  other = (self & 1) ^ 1;
  cards[self] = card_image_get(self);
  band = win_rate_band(player_win_rate(self));
  row = card_body_syu_b11_get(&cards[self]->body);
  switch ((int)net_struggle_revise_get(2)) {
  case 1:
    entry = (int *)(&prize_money_lists[1][0][0])[row * 6 + band];
    break;
  case 2:
    entry = (int *)(&prize_money_lists[2][0][0])[row * 6 + band];
    break;
  case 0:
  default:
    entry = (int *)(&prize_money_lists[0][0][0])[row * 6 + band];
    break;
  }
  prize_money[self] = 0;
  for (; entry[1] > 0; entry += 2) {
    if ((int)lcg_next15() < entry[1] * 0x147) {
      prize_money[self] = entry[0];
      break;
    }
  }
  if (prize_money[self] == 0) {
    prize_money[self] = entry[0];
  }
  if (prize_loss_tier(other) != 0 && mission_of_side[other] > 0) {
    prize_money[self] = 100;
  }
}

float player_win_rate(int side)
{
  card_image_rec *cards [2];
  card_image_rec *card;
  uint wins;
  int games;
  float rate;
  char self = side;
  
  cards[self] = card = card_image_get(self);
  wins = card->header.all_win;
  games = wins + card->header.all_lose;
  if (games != 0) {
    rate = (float)wins / (float)games;
  }
  else {
    rate = 0.0;
  }
  return rate * 100.0;
}

int win_rate_band(float rate)
{
  int band = 0;
  if (rate >= 80.0) band = 0;
  else if (rate >= 70.0 && rate < 80.0) band = 1;
  else if (rate >= 60.0 && rate < 70.0) band = 2;
  else if (rate >= 45.0 && rate < 60.0) band = 3;
  else if (rate >= 30.0 && rate < 45.0) band = 4;
  else band = 5;
  return band;
}

int item_rank_list_find(int chara, int item)
{
  char i, j;
  for (j = 2; j >= 0; j--) {
    for (i = 0; ((int *)item_rank_lists[j][chara])[i] != -1; i++) {
      if (item == ((int *)item_rank_lists[j][chara])[i]) return j;
    }
  }
  return -1;
}

void prize_give(int side)
{
  int *slot;
  
  slot = prize_item + side;
  if (*slot >= 0) {
    item_give_carded(side,*slot);
    prize_got_item = *slot;
    user_back_data.prize_items_given = user_back_data.prize_items_given + 1;
    *slot = -1;
  }
  slot = prize_money + side;
  if (*slot >= 0) {
    card_money_add(side,*slot);
    prize_got_money = *slot;
    *slot = -1;
  }
  return;
}

int prize_item_roll(int item, uint side)
{
  int *entry;
  int won;
  
  won = 0;
  if (prize_item_fallback[side] != 0) {
    entry = (int *)char_item_lists_alt[card_image_char_get(side & 0xff)];
  }
  else {
    entry = (int *)char_item_lists[card_image_char_get(side & 0xff)];
  }
  if (item < 0) return 0;
  for (; *entry >= 0; entry += 2) {
    if (*entry == item) {
      if ((int)lcg_next15() < entry[1] * 0x147) won = 1;
      break;
    }
  }
  return won;
}

int prize_item_pick(uint side)
{
  int chara;
  int cls;
  int item = 0;
  
  chara = card_image_char_get(side & 0xff);
  cls = player_rank_class(side);
  prize_item_fallback[side] = item;
  if (cls < 2) {
    item = prize_item_pick_unowned(side,cls,chara);
  }
  else {
    item = prize_item_pick_high(side,cls,chara);
  }
  return item;
}

int prize_item_pick_high(int side, int cls, int chara)
{
  int i;
  int n;
  int *entry;
  int item_no;
  int *pool;
  int item;
  
  if (prize_loss_tier(side) != 0) {
    item = prize_item_pick_unowned(side,1,chara);
    if (item != -1) return item;
  }
  item = prize_item_pick_unowned(side,cls,chara);
  if (item != -1) return item;
  i = 0;
  pool = &prize_item_pool[0][0];
  for (; i < 32; i++) {
    pool[side * 32 + i] = -1;
  }
  n = 0;
  {
    for (entry = (int *)item_rank_lists[3][chara]; (item_no = *entry) >= 0; entry++) {
      if (item_has_unchecked(side, item_no) == 0) {
        (&prize_item_pool[0][0])[side * 32 + n] = item_no;
        n++;
      }
    }
  }
  i = 0;
  pool = &prize_item_pool[0][0];
  for (; i < 32; i++) {
    if (pool[side * 32 + i] < 0) break;
  }
  if (i > 0) {
    item = (&prize_item_pool[0][0])[side * 32 + (int)lcg_next15() % i];
    prize_item_fallback[side] = 1;
  }
  else {
    item = -1;
  }
  return item;
}

int prize_item_pick_unowned(int side, int cls, int chara)
{
  int i;
  int n;
  int *entry;
  int item_no;
  int *pool;
  int item;
  
  i = 0;
  pool = &prize_item_pool[0][0];
  for (; i < 32; i++) {
    pool[side * 32 + i] = -1;
  }
  n = 0;
  for (; cls >= 0; cls--) {
    for (entry = (int *)item_rank_lists[cls][chara]; (item_no = *entry) >= 0; entry++) {
      if (item_has_unchecked(side, item_no) == 0) {
        (&prize_item_pool[0][0])[side * 32 + n] = item_no;
        n++;
      }
    }
  }
  i = 0;
  pool = &prize_item_pool[0][0];
  for (; i < 32; i++) {
    if (pool[side * 32 + i] < 0) break;
  }
  if (i > 0) {
    item = (&prize_item_pool[0][0])[side * 32 + (int)lcg_next15() % i];
  }
  else {
    item = -1;
  }
  return item;
}

int rank_class(byte rank)
{
  int cls = 0;
  if (rank > 14) {
    cls = (byte)(rank - 15) > 4; cls += 1;
  }
  return cls;
}

void prize_debug_text_draw(void)
{
  int enabled;
  
  enabled = debug_info_enabled();
  if (enabled != 0) {
    if (-1 < prize_got_item) {
      text_cursor_to_cell(0x1e,0x32);
      text_draw_formatted("GET ITEM %d",prize_got_item);
    }
    if (-1 < prize_got_money) {
      text_cursor_to_cell(0x1e,0x33);
      text_draw_formatted("GET MONEY %d",prize_got_money);
    }
  }
  return;
}

void mission_strikes_check(int side)
{
  int landed;

  landed = strike_landed(side);
  if ((landed != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_throws_check(int side)
{
  int landed;

  landed = throw_landed(side);
  if ((landed != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_combo_damage_check(uint side)
{
  rob_rec *rob;
  rob_rec *foe;
  int target;

  target = ((mission_rec *)&mission_table)[mission_of_side[side]].value;
  rob = rob_get(side & 0xff);
  foe = rob_get(side & 1 ^ 1);
  if (foe->combo[0].hits == 0) {
    mission_combo_broken = 0;
  }
  if (((rob->state & 0x100000) != 0) && ((foe->state & 0x200000) != 0)) {
    mission_combo_broken = 1;
  }
  if (((0 < foe->combo[0].hits) && (mission_combo_broken == 0)) &&
     (foe->combo[0].damage >= target)) {
    mission_count = mission_combo_broken;
  }
  return;
}

void mission_wall_break_check(uint side)
{
  rob_rec *rob;
  rob_rec *foe;
  rob_coli_rec *coli;
  rob_coli_rec *foe_coli;

  rob = rob_get(side & 0xff);
  foe = rob_get(side & 1 ^ 1);
  coli = rob->coli;
  foe_coli = foe->coli;
  if ((((coli->flags & 0x2000) != 0) ||
      ((foe_coli->flags & 0x2000) != 0)) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_quick_win_check(int side)
{
  if ((round_winner == side) && ((int)match_round_time - round_time < 0x259)) {
    mission_count = 0x0;
  }
  return;
}

void mission_timer_tick(void)
{
  if (round_time % 60 == 0 && pause_frame_held() == 0) { if (--mission_count < 0) mission_count = -1; }
}

void mission_first_hit_check(uint side)
{
  int *first_hit;
  uint other;
  
  first_hit = mission_first_hit;
  if ((first_hit[0] == 0) && (first_hit[1] == 0)) {
    if ((strike_landed(side) != 0) || (throw_landed(side) != 0)) {
      mission_first_hit[side] = 1;
    }
    other = 1 - (side & 1);
    if ((strike_landed(other) != 0) || (throw_landed(other) != 0)) {
      mission_first_hit[(side & 1) ^ 1] = 1;
    }
    if ((mission_first_hit[side] != 0) && (0 < mission_count)) {
      mission_count = mission_count + -1;
    }
  }
  return;
}

void mission_ukemi_check(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  if ((((rob->side_flags & 0x20) != 0) && ((rob->move_flags & 0x10000) != 0)) &&
     (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_counter_check(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  if ((((uint)rob->coli->field_0x3c[0] & 2) != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_health_check(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  if (rob->life < mission_last_health) {
    mission_count -= mission_last_health - rob->life;
  }
  mission_last_health = rob->life;
  if (mission_count < 0) {
    mission_count = 0x0;
  }
  return;
}

void mission_ko_win_check(int side)
{
  if (round_winner == side && (game_flags & 0x20) != 0 && 0 < mission_count) {
    mission_count = mission_count + -1;
  }
}

void mission_ring_out_win_check(int side)
{
  if (round_winner == side && (game_flags & 0x40) != 0 && 0 < mission_count) {
    mission_count = mission_count + -1;
  }
}

void mission_excellent_win_check(int side)
{
  if (round_winner == side && (game_flags & 0x80) != 0 && 0 < mission_count) {
    mission_count = mission_count + -1;
  }
}

void mission_no_round_lost_check(int side)
{
  if (round_winner != side) {
    mission_count = 0x0;
  }
}

void mission_down_attack_check(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  if (((rob->attack_level == '\f') && (rob->coli->field_0x1c[1] != 0)) &&
     (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_air_combo_check(uint side)
{
  rob_rec *foe;

  foe = rob_get(side & 1 ^ 1);
  if (((foe->state & 0x40) != 0) && (1 < foe->combo[0].hits)) {
    if ((mission_air_combo_seen[side] == 0) && (0 < mission_count)) {
      mission_count = mission_count + -1;
    }
    mission_air_combo_seen[side] = 1;
  }
  if ((mission_air_combo_seen[side] == 1) && (foe->combo[0].hits == 0)) {
    mission_air_combo_seen[side] = 0;
  }
  return;
}

void mission_four_hit_combo_check(uint side)
{
  rob_rec *foe;
  rob_rec *rob;

  foe = rob_get(side & 1 ^ 1);
  rob = rob_get(side & 0xff);
  if (((foe->combo[0].hits != mission_last_combo) && ((rob->state & 0x100000) == 0))
     && (0 < foe->combo[0].hits)) {
    mission_combo_hits[side]++;
  }
  if (((mission_rec *)&mission_table)[mission_of_side[side]].value <=
      mission_combo_hits[side]) {
    mission_count = 0x0;
  }
  mission_last_combo = (int)foe->combo[0].hits;
  if (mission_last_combo == 0) {
    mission_combo_hits[side] = 0;
  }
  return;
}

void mission_empty_jump_check(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  if ((rob->move_flags & 0x14480) != 0x80) {
    mission_jump_state[side] = 0;
    return;
  }
  if (mission_jump_state[side] == 1) {
    if ((rob->flags & 8) != 0) {
      return;
    }
    if ((rob->old_cur->flags & 8) == 0) {
      return;
    }
    mission_jump_state[side] = 0;
    if (mission_count < 1) {
      return;
    }
    mission_count = mission_count + -1;
    return;
  }
  if ((rob->flags & 8) == 0) {
    return;
  }
  if ((rob->old_cur->flags & 8) != 0) {
    return;
  }
  mission_jump_state[side] = 1;
  return;
}

void mission_knocked_down_check(uint side)
{
  rob_rec *rob;
  
  rob = rob_get(side & 0xff);
  if ((rob->state & 0x1400) == 0x1400) {
    mission_down_seen[side] = 1;
  }
  else {
    if (mission_down_seen[side] != 1) {
      return;
    }
    if (0 < mission_count) {
      mission_count = mission_count + -1;
    }
    mission_down_seen[side] = 0;
  }
}

void mission_guarded_check(int side)
{
  int guarded;
  guarded = 0;
  if (attack_guarded_high(side)) guarded = 1;
  if (attack_guarded_mid(side)) guarded = 1;
  if (attack_guarded_low(side)) guarded = 1;
  if (guarded && 0 < mission_count) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_knockdown_check(uint side)
{
  rob_rec *foe;
  uint other;
  
  other = side & 1 ^ 1;
  foe = rob_get((uchar)other);
  if ((foe->state & 0x800) != 0) {
    if ((mission_down_seen[other] == 0) && (0 < mission_count)) {
      mission_count = mission_count - 1;
    }
    mission_down_seen[other] = 1;
  }
  if (mission_down_seen[other] == 1) {
    if ((foe->state & 0x400) == 0) {
      if (((foe->state & 0x40000) == 0) && (foe->combo[2].hits == 0)) {
        mission_down_seen[other] = 0;
      }
    }
  }
  return;
}

void mission_strike_limit_check(int side)
{
  int landed;
  
  landed = strike_landed(side);
  if ((landed != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_throw_limit_check(int side)
{
  int landed;
  
  landed = throw_landed(side);
  if ((landed != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_guard_check(uint side)
{
  int guarded = 0;
  uint other;
  
  other = side & 1 ^ 1;
  if (attack_guarded_high(other) != 0) guarded = 1;
  if (attack_guarded_mid(other) != 0) guarded = 1;
  if (attack_guarded_low(other) != 0) guarded = 1;
  if (guarded && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
}

void mission_evade_check(uint side)
{
  int evaded;
  uint other;
  
  evaded = 0;
  other = 1 & side ^ 1;
  if (attack_evaded_high(other) != 0) evaded = 1;
  if (attack_evaded_mid(other) != 0) evaded = 1;
  if (attack_evaded_low(other) != 0) evaded = 1;
  if (evaded && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_evade_attack_check(int side)
{
  int landed;
  
  landed = evade_attack_landed(side);
  if ((landed != 0) && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
  return;
}

void mission_evaded_check(int side)
{
  int evaded = 0;
  if (attack_evaded_high(side) != 0) evaded = 1;
  if (attack_evaded_mid(side) != 0) evaded = 1;
  if (attack_evaded_low(side) != 0) evaded = 1;
  if (evaded && (0 < mission_count)) {
    mission_count = mission_count + -1;
  }
}

void mission_strike_race_check(uint side)
{
  int landed;
  uint other;
  
  other = side & 1 ^ 1;
  landed = strike_landed(side);
  if (landed != 0) {
    mission_tally[side] = mission_tally[side] + 1;
  }
  landed = strike_landed(other);
  if (landed != 0) {
    mission_tally[other] = mission_tally[other] + 1;
  }
  return;
}

void mission_guard_race_check(uint side)
{
  uint other;
  int other_guarded;
  int self_guarded;
  
  other = side & 1 ^ 1;
  other_guarded = 0;
  self_guarded = 0;
  if (attack_guarded_high(other) != 0) other_guarded = 1;
  if (attack_guarded_mid(other) != 0) other_guarded = 1;
  if (attack_guarded_low(other) != 0) other_guarded = 1;
  if (other_guarded) {
    mission_tally[side] = mission_tally[side] + 1;
  }
  if (attack_guarded_high(side) != 0) self_guarded = 1;
  if (attack_guarded_mid(side) != 0) self_guarded = 1;
  if (attack_guarded_low(side) != 0) self_guarded = 1;
  if (self_guarded) {
    mission_tally[other] = mission_tally[other] + 1;
  }
  return;
}

void mission_match_win_check(int side)
{
  if (champion_side == side) {
    mission_count = 1;
  }
  else {
    mission_count = 0;
  }
}

void bonus_item_init(void)
{
  bonus_walls_left = 6;
  bonus_low_health_flag = 1;
  bonus_round_time_flag = 1;
  if (win_streak < 2) {
    bonus_eight_chars_flag = 1;
    bonus_all_chars_flag = 1;
    bonus_beaten_chars = 0;
  }
  return;
}

void prize_rate_test_run(void)
{
  typedef struct { char chara; char level; short wins; short losses; short field_6; } profile_ent;
  struct { int pad; card_image_rec *card; } slot;
  int i;
  int j;
  int k;
  slot.card = card_image_get(1);
  for (i = 0; i < 2; i++) {
    prize_test_side_prizes[i] = 0;
    prize_test_loss_tier2_count[i] = 0;
    prize_test_count_2[i] = 0;
    prize_test_loss_tier1_count[i] = 0;
    prize_test_count_4[i] = 0;
    prize_test_miss_capped_count[i] = 0;
    prize_test_count_8[i] = 0;
    prize_test_count_9[i] = 0;
    prize_test_count_6[i] = 0;
    prize_test_count_7[i] = 0;
    prize_test_count_10[i] = 0;
    for (j = 0; j < 1000; j++) {
      k = i * 1000 + j;
      prize_test_table_0[k] = 0;
      prize_test_table_1[k] = 0;
      prize_test_table_2[k] = 0;
      prize_test_table_3[k] = 0;
      prize_test_table_4[k] = 0;
      {
      ushort *table5 = prize_test_table_5;
      ushort *table6 = prize_test_table_6;
      ushort *table7 = prize_test_table_7;
      ushort *table8 = prize_test_table_8;
      ushort *table9 = prize_test_table_9;
      table5[k] = 0;
      table6[k] = 0;
      table7[k] = 0;
      table8[k] = 0;
      table9[k] = 0;
      }
    }
    for (j = 0; j < 5; j++) {
      ushort *table10 = prize_test_table_10;
      table10[i * 5 + j] = 0;
    }
  }
  bonus_time_flags = 0;
  prize_test_prize_matches = 0;
  prize_test_syuchu_matches = 0;
  prize_test_syuchu_marks = 0;
  prize_test_bonus_time_matches = 0;
  prize_test_lottery_starts = 0;
  prize_test_bonus_time_starts = 0;
  for (i = 0; i < prize_test_runs; i++) {
    prize_test_profile_no = lcg_next15() % 220;
    slot.card->header.char_num = ((profile_ent *)&prize_test_profiles[0][0])[(ushort)prize_test_profile_no].chara;
    slot.card->header.level = ((profile_ent *)&prize_test_profiles[0][0])[(ushort)prize_test_profile_no].level;
    { ushort n = prize_test_profile_no; short *wins = (short *)(&prize_test_profiles[0][0] + 2); slot.card->header.all_win = wins[n * 4]; }
    slot.card->header.all_lose = ((profile_ent *)&prize_test_profiles[0][0])[(ushort)prize_test_profile_no].losses;
    slot.card->body.exp = 0;
    slot.card->body.point = 0;
    mission_clear();
    mission_sim_vs_draw();
    prize_choose();
    mission_sim_item_roll();
    mission_sim_match_result();
    prize_miss_count_update();
  }
}

void bonus_wall_watch(void)
{
  if (play_style == '\x02') {
    bonus_wall_break_count(0);
  }
  return;
}

void bonus_item_judge(void)
{
  int i;
  
  i = 0;
  if (play_style == '\x02') {
    bonus_round_time_check();
    do {
      bonus_beaten_chars_update(i);
      bonus_low_health_check(i);
      i = i + 1;
    } while (i < 2);
    bonus_item_award();
  }
  return;
}

void bonus_item_award(void)
{
  uint unowned[5];
  uint earned[5];
  card_image_rec *cards[2];
  card_record_rec *records[2];
  int side;
  int i;
  int sum;
  int found;
  int *list;
  uint item;

  for (i = 0; i < 5; i++) {
    unowned[i] = 0;
    earned[i] = 0;
  }
  side = game_flags & 0x400;
  if (side != 0) {
    if (champion_side == 0) {
      side = 0;
    }
    else if (champion_side == 1) {
      side = 1;
    }
    else {
      return;
    }
    cards[side] = card_image_get(side);
    list = (int *)bonus_item_lists[card_image_char_get((uchar)side)];
    for (i = 0; i < 5; i++) {
      int item_no = list[i];
      if (item_has_unchecked(side,item_no) != 0) {
        unowned[i] = 0;
      }
      else {
        unowned[i] = 1;
      }
    }
    if (bonus_walls_left == 0) earned[0] = 1;
    if (bonus_round_time_flag == 0) earned[1] = 1;
    if (bonus_low_health_flag == 0) earned[2] = 1;
    if (bonus_eight_chars_flag == 0) earned[3] = 1;
    if (bonus_all_chars_flag == 0) earned[4] = 1;
    sum = 0;
    for (i = 0; i < 5; i++) sum += unowned[i];
    if (sum != 0) {
      sum = 0;
      for (i = 0; i < 5; i++) sum += earned[i];
      if (sum != 0) {
        found = 0;
        for (i = 4; i >= 0; i--) {
          if ((unowned[i] & earned[i]) != 0) {
            found = 1;
            break;
          }
        }
        if (found) {
          item = 0;
          records[side] = card_record_get(side);
          bonus_walls_left = 6;
          bonus_low_health_flag = 1;
          bonus_round_time_flag = 1;
          bonus_eight_chars_flag = 1;
          bonus_all_chars_flag = 1;
          if (i == 4) {
            bonus_beaten_chars = item;
          }
          item = *(ushort *)(bonus_item_lists[card_image_char_get((uchar)side)] + i * 4);
          item_give_carded(side,item);
          if (records[side]->in_use != 0) {
            item_award_mes_start(side,1);
          }
        }
      }
    }
  }
}

void bonus_beaten_chars_update(uint side)
{
  rob_rec *foe;
  int i;
  int n;
  foe = rob_get(side & 1 ^ 1);
  n = 0;
  if ((game_flags & 0x400) == 0) return;
  if (champion_side == side) {
    for (i = 0; i < 0x10; i++) {
      if (foe->chara != 0xd && foe->chara == i) bonus_beaten_chars |= 1 << i;
    }
  }
  for (i = 0; i < 0x10; i++) {
    if (bonus_beaten_chars & 1 << i) n++;
  }
  if (n == 8) bonus_eight_chars_flag = 0;
  if (n == 0xf) bonus_all_chars_flag = 0;
}

void bonus_low_health_check(uint side)
{
  rob_rec *rob;
  
  rob = rob_get(side & 0xff);
  if ((((game_flags & 0x400) != 0) && ((int)(champion_side) == side)) &&
     (rob->life == 2)) {
    bonus_low_health_flag = 0;
  }
  return;
}

void bonus_wall_break_count(uint side)
{
  rob_rec *rob;
  rob_rec *foe;
  rob_coli_rec *coli;
  rob_coli_rec *foe_coli;
  rob = rob_get(side & 0xff);
  foe = rob_get(side & 1 ^ 1);
  coli = rob->coli;
  foe_coli = foe->coli;
  if ((((coli->flags & 0x2000) != 0) ||
      ((foe_coli->flags & 0x2000) != 0)) && (0 < bonus_walls_left)) {
    bonus_walls_left = bonus_walls_left + -1;
  }
}

void bonus_round_time_check(void)
{
  if (round_time == 900) {
    bonus_round_time_flag = 0;
  }
  return;
}

void mission_round_flags_clear(void)
{
  int i;
  
  i = 0;
  do {
    mission_first_hit[i] = 0;
    mission_jump_state[i] = 0;
    i = i + 1;
  } while (i < 2);
  prize_got_item = -1;
  prize_got_money = -1;
  return;
}

uint side_is_player(int side)
{
  return (uint)(int)(side_entry_flags)[side] >> 2 & 1;
}

int strike_landed(uint side)
{
  char hit_result;
  rob_rec *rob;
  int landed;

  rob = rob_get(side & 0xff);
  landed = 0;
  if ((((rob->coli->field_0x1c[1] != 0) &&
       (hit_result = rob->coli->field_0x94, hit_result != '\0')) && (hit_result != '\n')) &&
     (3 < (int)rob->attack_level - 8U)) {
    landed = 1;
  }
  return landed;
}

int throw_landed(uint side)
{
  rob_rec *rob;
  rob_rec *foe;
  int thrown;
  uint move_no;
  uint move_no_2;

  rob = rob_get(side & 0xff);
  foe = rob_get(side & 1 ^ 1);
  thrown = 0;
  if ((rob->flags28 & 0x08000000) != 0) return 0;
  if ((foe->flags28 & 0x08000000) != 0) return 0;
  if ((foe->state & 0x200000) == 0) return 0;
  {
    if (foe->frame == 0xf) thrown = 1;
    move_no = foe->move_no;
    if (move_no == 0xe8d || move_no == 0xeca || move_no == 0x4a3) thrown = foe->frame == 1;
    if (foe->move_no == 0xdc3) thrown = foe->frame == 0xb;
    if (thrown == 1 || (foe->frame == 0xb && (((move_no_2 = foe->move_no) == 0x149c) | (move_no_2 == 0x1360)))) {
      if (control_branch_pending_get(foe) != 0) return 0;
      if ((rob->flags2c & 0x80000) != 0) return 1;
      if ((rob->flags2c & 0x100000) != 0) return 1;
      if ((rob->flags2c & 0x200000) != 0) return 1;
      if ((rob->flags2c & 0x400000) != 0) return 1;
      if ((rob->flags2c & 0x800000) != 0) return 1;
      if ((rob->flags2c & 0x1000000) != 0) return 1;
      if ((rob->flags2c & 0x2000000) != 0) return 1;
      if ((rob->flags2c & 0x4000000) != 0) return 1;
      if ((rob->flags2c & 0x8000000) != 0) return 1;
      if ((rob->flags2c & 0x10000000) != 0) return 1;
      if ((rob->flags2c & 0x20000000) != 0) return 1;
      if ((rob->flags2c & 0x40000000) != 0) return 1;
    }
  }
  return 0;
}

int attack_guarded_high(uint side)
{
  char hit_result;
  char level;
  rob_rec *rob;
  int guarded;

  rob = rob_get(side & 0xff);
  guarded = 0;
  if (rob->coli->field_0x1c[1] != 0) {
    hit_result = rob->coli->field_0x94;
    if (hit_result != 0) if (hit_result == 10) {
      level = rob->attack_level;
      if (level == 1) guarded = 1;
      else if (level == 7) guarded = 1;
    }
  }
  return guarded;
}

int attack_guarded_mid(uint side)
{
  char hit_result;
  rob_rec *rob;
  int guarded;

  rob = rob_get(side & 0xff);
  guarded = 0;
  if (rob->coli->field_0x1c[1] != 0) {
    hit_result = rob->coli->field_0x94;
    if (hit_result != 0) if (hit_result == 10) {
      switch (rob->attack_level) {
      case 2: case 4: guarded = 1;
      }
    }
  }
  return guarded;
}

int attack_guarded_low(uint side)
{
  char hit_result;
  rob_rec *rob;
  int guarded;

  rob = rob_get(side & 0xff);
  guarded = 0;
  if (((rob->coli->field_0x1c[1] != 0) &&
      (hit_result = rob->coli->field_0x94, hit_result != '\0')) && (hit_result == '\n')) {
    switch(rob->attack_level) {
    case 3:
    case 5:
    case 6:
    case 0xd:
      guarded = 1;
    }
  }
  return guarded;
}

int attack_evaded_high(uint side)
{
  rob_rec *rob;
  int evaded;

  rob = rob_get(side & 0xff);
  evaded = 0;
  if ((rob->side_flags & 0x5000) != 0) {
    if (evade_latch != 2) {
      evade_latch = 1;
    }
  }
  else {
    evade_latch = 0;
  }
  if (evade_latch == 1) {
    switch (rob->attack_level) {
    case 1:
    case 7:
      evaded = 1;
      evade_latch = 2;
    }
  }
  return evaded;
}

int attack_evaded_mid(uint side)
{
  rob_rec *rob;
  int evaded;
  char level;

  rob = rob_get(side & 0xff);
  evaded = 0;
  if ((rob->side_flags & 0x5000) != 0) {
    if (evade_latch != 2) evade_latch = 1;
  }
  else {
    evade_latch = 0;
  }
  if (evade_latch == 1) {
    level = rob->attack_level;
    switch (level) {
    case 2:
    case 4:
      evaded = 1;
      evade_latch = 2;
    }
  }
  return evaded;
}

int attack_evaded_low(uint side)
{
  rob_rec *rob;
  int evaded;
  rob = rob_get(side & 0xff);
  evaded = 0;
  if ((rob->side_flags & 0x5000) != 0) {
    if (evade_latch != 2) evade_latch = 1;
  }
  else {
    evade_latch = 0;
  }
  if (evade_latch == 1) {
    switch(rob->attack_level) {
    case 3:
    case 5:
    case 6:
    case 0xd:
      evaded = 1;
      evade_latch = 2;
    }
  }
  return evaded;
}

int evade_attack_landed(uint side)
{
  rob_rec *rob;
  int landed;
  int result;

  rob = rob_get(side & 0xff);
  result = 0;
  landed = strike_landed(side);
  if ((landed != 0) && ((int)rob->flags28 < 0)) {
    result = 1;
  }
  return result;
}

int player_rank_class(int side)
{
  int cls;
  card_image_rec *cards [2];

  cards[side] = card_image_get(side);
  cls = rank_class((uint)cards[side]->header.level);
  return cls;
}

void quest_judge(void)
{
  int i;
  card_body_rec *body;
  card_image_rec *card;
  int category;
  int kind;
  
  for (i = 0; i < 2; i++) {
    body = &card_image_get(i)->body;
    card = card_image_get(i);
    if (quest_state_get(i) != 1) continue;
    category = quest_category_get(i);
    kind = quest_kind_get(i);
    quest_count[i] = card_hdr_quest_count_get(&card->header);
    if (net_ready_latched() != 0) {
      if (category == 0) {
        (*(void (**)(int))(quest_checks_0 + kind))(i);
      }
      else if (category == 1) {
        (*(void (**)(int))(quest_checks_1 + kind))(i);
      }
      else if (category == 2) {
        (*(void (**)(int))(quest_checks_2 + kind))(i);
      }
      if (card_body_quest_category_get(body) != 1 || (quest_kind_get(i) != 0xc && quest_kind_get(i) != 0xd)) {
        card_hdr_quest_count_set(&card->header,quest_count[i]);
      }
      if (card_body_quest_category_get(body) == 1) {
        if (quest_kind_get(i) == 0xc || quest_kind_get(i) == 0xd) {
          if (card_hdr_quest_count_get(&card->header) == 0 && quest_param_a_get(i) == 0 && quest_param_b_get(i) == 0) {
            if (quest_param_c_get(i) == 0) {
              card_body_quest0_b0_set(body,2);
              card_body_quest_param_c_set(body,0);
              card_body_quest_param_b_set(body,0);
              card_body_quest_param_a_set(body,0);
            }
          }
          goto done;
        }
      }
      else if (card_body_quest_category_get(body) == 2 && quest_kind_get(i) == 2) {
        if (card_hdr_quest_count_get(&card->header) == 0) {
          card_body_quest0_b0_set(body,2);
          card_body_quest_param_c_set(body,0);
          card_body_quest_param_a_set(body,0);
        }
        goto done;
      }
      if (card_hdr_quest_count_get(&card->header) == 0) {
        card_body_quest0_b0_set(body,2);
        card_body_quest_param_c_set(body,0);
        card_body_quest_param_b_set(body,0);
        card_body_quest_param_a_set(body,0);
      }
    }
  done:
    if (card_body_quest0_b0_get(body) == 1 && quest_expired(i)) {
      card_body_quest0_b0_set(body,3);
      card_body_quest_param_c_set(body,0);
      card_body_quest_param_b_set(body,0);
      card_body_quest_param_a_set(body,0);
      card_hdr_quest_count_set(&card->header,0);
    }
  }
}

void quest_end_check(void)
{
  card_body_rec *body;
  card_image_rec *card;
  int state;
  int i;
  
  i = 0;
  do {
    body = &card_image_get(i)->body;
    card = card_image_get(i);
    state = card_body_quest0_b0_get(body);
    if ((state == 1) && (state = quest_expired(i), state != 0)) {
      card_body_quest0_b0_set(body,3);
      card_body_quest_param_c_set(body,0);
      card_body_quest_param_b_set(body,0);
      card_body_quest_param_a_set(body,0);
      card_hdr_quest_count_set(&card->header,0);
    }
    i = i + 1;
  } while (i < 2);
  return;
}

void quest_fight_rank_char_check(uint side)
{
  card_image_rec *card;
  uint param;
  uint other;
  card_image_rec *cards [2];
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (param = quest_param_a_get(side), cards[other]->header.level == param)) {
    if (quest_param_b_get(side) == (ushort)card_image_char_get((uchar)other)) {
      ushort *counts = quest_count;
      counts[side] = card_hdr_quest_count_get(&card->header);
      if (counts[side] != 0) {
        counts[side]--;
      }
    }
  }
  return;
}

void quest_fight_rank_min_char_check(uint side)
{
  ushort *count;
  card_image_rec *card;
  uint param;
  uint other;
  card_image_rec *cards[2];

  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == '\x02') {
      param = quest_param_a_get(side);
      if (cards[other]->header.level >= param) {
        param = quest_param_b_get(side);
        if (param == (card_image_char_get((uchar)other) & 0xffff)) {
          count = &quest_count[side];
          *count = card_hdr_quest_count_get(&card->header);
          if (*count != 0) {
            *count = *count - 1;
          }
        }
      }
    }
  }
}

void quest_win_rank_char_check(uint side)
{
  card_image_rec *card;
  card_image_rec *cards[2];
  uint other;
  char winner;
  uint param;
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
      ((param = quest_param_a_get(side)), cards[other]->header.level == param)) {
    param = quest_param_b_get(side);
    if (param == (ushort)card_image_char_get((uchar)other)) {
      winner = (uchar)champion_side;
      if (winner == side) {
        quest_count[winner] = card_hdr_quest_count_get(&card->header);
        if (quest_count[winner] != 0) {
          quest_count[winner] = quest_count[winner] - 1;
        }
      }
    }
  }
}

void quest_win_rank_min_char_check(uint side)
{
  card_image_rec *card;
  uint other;
  char winner;
  card_image_rec *cards[2];
  uint param;
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == 2) {
      param = quest_param_a_get(side);
      if (cards[other]->header.level >= param) {
        if (quest_param_b_get(side) == (ushort)card_image_char_get((uchar)other)) {
          winner = champion_side;
          if (winner == side) {
            quest_count[winner] = card_hdr_quest_count_get(&card->header);
            if (quest_count[winner] != 0) {
              quest_count[winner] = quest_count[winner] - 1;
            }
          }
        }
      }
    }
  }
  return;
}

void quest_excellent_rank_char_check(uint side)
{
  ushort *count;
  card_image_rec *card;
  uint other;
  int param;
  card_image_rec *cards [2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if (play_style == 2) {
    param = quest_param_a_get(side);
    if (cards[other]->header.level == param) {
      param = quest_param_b_get(side);
      if (param == (ushort)card_image_char_get((uchar)other)) {
        if (round_winner == side && (game_flags & 0x80) != 0) {
          count = quest_count + round_winner;
          *count = card_hdr_quest_count_get(&card->header);
          if (*count != 0) {
            *count = *count - 1;
          }
        }
      }
    }
  }
  return;
}

void quest_excellent_rank_min_char_check(uint side)
{
  card_image_rec *card;
  uint param;
  uint other;
  int winner;
  card_image_rec *cards [2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if (play_style == '\x02') {
    param = quest_param_a_get(side);
    if (cards[other]->header.level >= param) {
      param = quest_param_b_get(side);
      if (param == (ushort)card_image_char_get((uchar)other)) {
        winner = round_winner;
        if (winner == side && (game_flags & 0x80) != 0) {
          quest_count[winner] = card_hdr_quest_count_get(&card->header);
          if (quest_count[winner] != 0) {
            quest_count[winner]--;
          }
        }
      }
    }
  }
}

void quest_fight_rank_check(uint side)
{
  card_image_rec *card;
  uint other;
  card_image_rec *cards[2];
  ushort *counts;
  uint param;

  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (param = quest_param_a_get(side), cards[other]->header.level == param)) {
    counts = quest_count;
    counts[side] = card_hdr_quest_count_get(&card->header);
    if (counts[side] != 0) {
      counts[side] = counts[side] - 1;
    }
  }
}

void quest_win_rank_check(uint side)
{
  card_image_rec *card;
  uint other;
  uint winner;
  uint param;
  card_image_rec *cards[2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == '\x02') {
      param = quest_param_a_get(side);
      if (cards[other]->header.level == param) {
        winner = champion_side;
        if (winner == side) {
          if ((quest_count[winner] = card_hdr_quest_count_get(&card->header)) != 0) {
            quest_count[winner]--;
          }
        }
      }
    }
  }
  return;
}

void quest_excellent_rank_check(uint side)
{
  uint n;
  card_image_rec *card;
  uint param;
  uint other;
  int winner;
  card_image_rec *cards[2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if (play_style == '\x02') {
    param = quest_param_a_get(side);
    if (cards[other]->header.level == param) {
      winner = round_winner;
      if (winner == side) {
        if ((game_flags & 0x80) != 0) {
          quest_count[winner] = n = card_hdr_quest_count_get(&card->header);
          if ((ushort)n != 0) {
            quest_count[winner] = n - 1;
          }
        }
      }
    }
  }
  return;
}

void quest_fight_rank_min_check(uint side)
{
  card_image_rec *card;
  ushort *count;
  ushort s;
  uint other;
  card_image_rec *cards[2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == '\x02') {
      uint param = quest_param_a_get(side);
      if (cards[other]->header.level >= param) {
        count = &quest_count[side];
        *count = card_hdr_quest_count_get(&card->header);
        if (*count != 0) *count = *count - 1;
      }
    }
  }
  return;
}

void quest_win_rank_min_check(uint side)
{
  card_image_rec *cards[2];
  uint other;
  card_image_rec *card;
  char winner;
  uint param;
  ushort *count;

  other = (side & 1) ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) == 0) return;
  if (play_style != 2) return;
  param = quest_param_a_get(side);
  if (cards[other]->header.level < param) return;
  if ((winner = (uchar)champion_side) != side) return;
  count = quest_count + winner;
  *count = card_hdr_quest_count_get(&card->header);
  if (*count != 0) {
    *count = *count - 1;
  }
}

void quest_excellent_rank_min_check(uint side)
{
  uint n;
  card_image_rec *card;
  int winner;
  uint param;
  uint other;
  card_image_rec *cards [2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if (play_style == '\x02') {
    param = quest_param_a_get(side);
    if (cards[other]->header.level >= param) {
      winner = round_winner;
      if (winner == side && (game_flags & 0x80) != 0) {
        quest_count[winner] = n = card_hdr_quest_count_get(&card->header);
        if ((ushort)n != 0) {
          quest_count[winner] = n + -1;
        }
      }
    }
  }
}

void quest_fight_char_check(uint side)
{
  uint other;
  card_image_rec *card;
  card_image_rec *cards[2];
  uint n;
  short *count;
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (quest_param_b_get(side) == (&player_chara.side0)[other])) {
    count = (short *)(&quest_count[side]);
    n = card_hdr_quest_count_get(&card->header); *count = n;
    if ((ushort)n != 0) {
      *count = n - 1;
    }
  }
}

void quest_win_char_check(uint side)
{
  uint n;
  card_image_rec *card;
  int winner;
  uint other;
  card_image_rec *cards [2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0 && play_style == '\x02') {
    if (quest_param_b_get(side) == (&player_chara.side0)[other]) {
      winner = champion_side;
      if (winner == side) {
        quest_count[winner] = n = card_hdr_quest_count_get(&card->header);
        if ((ushort)n != 0) {
          quest_count[winner] = n + -1;
        }
      }
    }
  }
}

void quest_excellent_char_check(uint side)
{
  card_image_rec *card;
  uint other;
  int winner;
  card_image_rec **other_card;
  ushort *count;
  card_image_rec *cards [2];
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  other_card = &cards[other];
  *other_card = card_image_get(other);
  if (play_style == '\x02') {
    if (quest_param_b_get(side) == (&player_chara.side0)[other]) {
      winner = round_winner;
      if (winner == side) {
        if ((game_flags & 0x80) != 0) {
          count = quest_count + winner;
          *count = card_hdr_quest_count_get(&card->header);
          if (*count != 0) {
            *count = *count + -1;
          }
        }
      }
    }
  }
}

void quest_win_check(int side)
{
  card_image_rec *card;
  int winner;
  
  card = card_image_get(side);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (winner = (int)(champion_side), winner == side)) {
    quest_count[winner] = card_hdr_quest_count_get(&card->header);
    if (quest_count[winner] != 0) {
      quest_count[winner] = quest_count[winner] + -1;
    }
  }
  return;
}

void quest_win_streak_check(int side)
{
  card_image_rec *card;
  
  card = card_image_get(side);
  if (((game_flags & 0x400) != 0) && (play_style == '\x02')) {
    if (champion_side == side) {
      quest_count[side] = card_hdr_quest_count_get(&card->header);
      if (quest_count[side] != 0) {
        quest_count[side]--;
      }
    }
    else {
      quest_count[side] = quest_param_c_get(side);
    }
  }
}

void quest_excellent_check(int side)
{
  card_image_rec *card;
  int winner;
  
  card = card_image_get(side);
  if (((play_style == '\x02') && (winner = (int)(round_winner), winner == side)) &&
     ((game_flags & 0x80) != 0)) {
    quest_count[winner] = card_hdr_quest_count_get(&card->header);
    if (quest_count[winner] != 0) {
      quest_count[winner] = quest_count[winner] + -1;
    }
  }
  return;
}

void quest_cond_1_00_mission_match_end(int side)
{
  card_image_rec *card;
  int mission;
  
  card = card_image_get(side);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (mission = mission_active_get(), mission != -1)) {
    quest_count[side] = card_hdr_quest_count_get(&card->header);
    if (quest_count[side] != 0) {
      quest_count[side] = quest_count[side] + -1;
    }
  }
  return;
}

void quest_cond_1_01_mission_match_win(int side)
{
  ushort *count;
  card_image_rec *card;
  int winner;
  
  card = card_image_get(side);
  if (((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
      (mission_active_get() != -1)) && (winner = champion_side, winner == side)) {
    count = quest_count + winner;
    *count = card_hdr_quest_count_get(&card->header);
    if (*count != 0) {
      *count = *count - 1;
    }
  }
}

void quest_cond_1_02_1p_mission_result(int side)
{
  card_image_rec *card;
  int value;
  
  card = card_image_get(side);
  if (((((game_flags & 0x400) != 0) && (play_style == '\x01')) &&
      (value = mission_active_get(), value != -1)) && (value = mission_result_get(), value == 1)) {
    quest_count[side] = card_hdr_quest_count_get(&card->header);
    if (quest_count[side] != 0) {
      quest_count[side] = quest_count[side] + -1;
    }
  }
  return;
}

void quest_cond_1_03_rank_char_streak(uint side)
{
  uint n;
  card_image_rec *card;
  card_image_rec **other_card;
  uint param;
  uint other;
  short *count;
  card_image_rec *cards[2];
  
  card = card_image_get(side);
  other = side & 1 ^ 1;
  other_card = &cards[other];
  *other_card = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (param = quest_param_a_get(side), cards[other]->header.level == param)) {
    param = quest_param_b_get(side);
    if (param == (card_image_char_get((uchar)other) & 0xffff)) {
      if (champion_side == side) {
        count = (short *)(&quest_count[side]);
        n = card_hdr_quest_count_get(&card->header);
        *count = n;
        if ((ushort)n != 0) {
          *count = n + -1;
        }
      }
      else {
        ((short *)&quest_count)[side] = quest_param_c_get(side);
      }
    }
  }
  return;
}

void quest_cond_1_04_min_rank_char_streak(uint side)
{
  card_image_rec *card;
  uint param;
  uint other;
  card_image_rec **other_card;
  ushort *count;
  card_image_rec *cards[2];
  
  card = card_image_get(side);
  other = (side & 1) ^ 1;
  other_card = &cards[other];
  *other_card = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == '\x02') {
      param = quest_param_a_get(side);
      if (cards[other]->header.level >= param) {
        param = quest_param_b_get(side);
        if (param == (ushort)card_image_char_get((uchar)other)) {
          if (champion_side == side) {
            count = quest_count + side;
            *count = card_hdr_quest_count_get(&card->header);
            if (*count != 0) {
              *count = *count - 1;
            }
          }
          else {
            ((short *)&quest_count)[side] = quest_param_c_get(side);
          }
        }
      }
    }
  }
}

void quest_cond_1_05_own_rank_reach(int side)
{
  ushort n;
  card_image_rec *card;
  card_image_rec *cards [2];
  
  card = card_image_get(side);
  cards[side] = card_image_get(side);
  quest_count[side] = n = card_hdr_quest_count_get(&card->header);
  if ((int)(uint)cards[side]->header.level >= quest_rank_targets[n]) {
    quest_count[side] = 0;
  }
}

void quest_cond_1_06_card_total_reach(int side)
{
  uint n;
  card_image_rec *card;
  card_image_rec *cards [2];
  card_image_rec **own_card;
  ushort *count;

  card = card_image_get(side);
  own_card = &cards[side];
  *own_card = card_image_get(side);
  count = &quest_count[side];
  *count = n = card_hdr_quest_count_get(&card->header);
  if (((game_flags & 0x400) != 0) && (play_style == '\x02')) {
    if (champion_side == side) {
      if ((int)((*own_card)->header.all_win + 1) >= quest_total_targets[(uint)(ushort)n]) {
        *count = 0;
      }
    }
    else if ((int)(uint)cards[side]->header.all_win >=
             quest_total_targets[(uint)quest_count[side]]) {
      quest_count[side] = 0;
    }
  }
}

void quest_cond_1_09_rank_streak(uint side)
{
  card_image_rec *card;
  int other;
  card_image_rec **other_card;
  ushort *count;
  uint param;
  card_image_rec *cards[2];
  
  card = card_image_get(side);
  other = side & 1 ^ 1;
  other_card = &cards[other];
  *other_card = card_image_get(other);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (param = quest_param_a_get(side), (*other_card)->header.level == param)) {
    if (champion_side == side) {
      count = quest_count + side;
      *count = card_hdr_quest_count_get(&card->header);
      if (*count != 0) {
        *count = *count - 1;
      }
    }
    else {
      quest_count[side] = quest_param_c_get(side);
    }
  }
}

void quest_cond_1_10_min_rank_streak(uint side)
{
  card_image_rec *card;
  int other;
  card_image_rec *cards[2];
  ushort *count;
  uint param;
  card = card_image_get(side);
  other = side & 1 ^ 1;
  cards[other] = card_image_get(other);
  if ((game_flags & 0x400) != 0) {
    if (play_style == 2) {
      param = quest_param_a_get(side);
      if (cards[other]->header.level >= param) {
        if (champion_side == side) {
          count = &quest_count[side];
          *count = card_hdr_quest_count_get(&card->header);
          if (*count != 0) {
            *count = *count - 1;
          }
        }
        else {
          quest_count[side] = quest_param_c_get(side);
        }
      }
    }
  }
  return;
}

void quest_cond_1_11_char_streak(uint side)
{
  ushort *count;
  card_image_rec *card = card_image_get(side);
  int other = (side & 1) ^ 1;
  if (((game_flags & 0x400) != 0) && (play_style == '\x02')) {
    uint param = quest_param_b_get(side);
    if (param == (ushort)card_image_char_get((uchar)other)) {
      if ((int)champion_side == side) {
        count = quest_count + side;
        *count = card_hdr_quest_count_get(&card->header);
        if (*count != 0) {
          *count = *count - 1;
        }
      }
      else {
        quest_count[side] = quest_param_c_get(side);
      }
    }
  }
}

void quest_char_bit_clear(uint side)
{
  card_image_rec *card;
  card_body_rec *body;
  int chara;
  uint mask_hdr;
  uint mask_a;
  uint mask_b;
  uint mask_c;
  
  card = card_image_get(side);
  body = &card_image_get(side)->body;
  chara = card_image_char_get(side & 1 ^ 1);
  mask_hdr = card_hdr_quest_count_get(&card->header);
  mask_a = quest_param_a_get(side);
  mask_b = quest_param_b_get(side);
  mask_c = quest_param_c_get(side);
  if (chara == 0) mask_hdr &= 0xfffe;
  else if (chara == 1) mask_hdr &= 0xfffd;
  else if (chara == 2) mask_hdr &= 0xfffb;
  else if (chara == 3) mask_hdr &= 0xfff7;
  else if (chara == 4) mask_a &= 0xfffe;
  else if (chara == 5) mask_a &= 0xfffd;
  else if (chara == 6) mask_a &= 0xfffb;
  else if (chara == 7) mask_a &= 0xfff7;
  else if (chara == 8) mask_a &= 0xffef;
  else if (chara == 9) mask_a &= 0xffdf;
  else if (chara == 10) mask_b &= 0xfffe;
  else if (chara == 11) mask_b &= 0xfffd;
  else if (chara == 12) mask_b &= 0xfffb;
  else if (chara == 14) mask_b &= 0xfff7;
  else if (chara == 15) mask_c &= 0xfffe;
  card_hdr_quest_count_set(&card->header,mask_hdr & 0xff);
  card_body_quest_param_a_set(body,mask_a);
  card_body_quest_param_b_set(body,mask_b);
  card_body_quest_param_c_set(body,mask_c);
  return;
}

void quest_cond_1_12_char_set_match(int side)
{
  if (((game_flags & 0x400) != 0) && (play_style == '\x02')) {
    quest_char_bit_clear(side);
  }
  return;
}

void quest_cond_1_13_char_set_win(int side)
{
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) && ((champion_side) == side)) {
    quest_char_bit_clear((int)(champion_side));
  }
  return;
}

void quest_cond_1_14_1p_rank_round_clear(int side)
{
  card_image_rec *card;
  
  card = card_image_get(side);
  if ((((play_style == '\x01') && ((game_flags & 0x400) != 0)) && ('\t' < (char)rank_round)) &&
     ((game_flags & 0x800) != 0)) {
    quest_count[side] = card_hdr_quest_count_get(&card->header);
    if (quest_count[side] != 0) {
      quest_count[side]--;
    }
  }
}

void quest_cond_1_15_own_shop_streak(uint side)
{
  card_image_rec *card;
  
  card = card_image_get(side);
  if ((game_flags & 0x400) != 0 && play_style == 2 && net_card_place_matches(side & 0xff)) {
    if (champion_side == side) {
      if ((quest_count[side] = card_hdr_quest_count_get(&card->header)) != 0)
        quest_count[side]--;
    }
    else {
      quest_count[side] = quest_param_c_get(side);
    }
  }
}

void quest_cond_2_02_paired_quest_win(uint side)
{
  card_image_rec *card;
  int param;
  uint other;
  
  other = side & 1 ^ 1;
  card = card_image_get(side);
  if ((game_flags & 0x400) != 0 && play_style == 2 && quest_category_get(other) == 2 &&
      quest_kind_get(other) == 2 && net_ryuko_is_on() != 0) {
    param = quest_param_b_get(side);
    if (param != quest_param_b_get(side & 1 ^ 1) && champion_side == side) {
      ushort *count = quest_count + champion_side;
      *count = card_hdr_quest_count_get(&card->header);
      if (*count != 0) {
        *count = *count - 1;
      }
    }
  }
  return;
}

void quest_cond_2_03_rank_meter(uint side)
{
  card_image_rec *cards[3];
  card_image_rec *volatile *own_slot;
  card_image_rec *volatile *other_slot;
  uint other;

  cards[2] = card_image_get(side);
  other = side & 1 ^ 1;
  own_slot = (card_image_rec *volatile *)(side * 4 + (int)cards);
  *own_slot = card_image_get(side);
  other_slot = (card_image_rec *volatile *)(other * 4 + (int)cards);
  *other_slot = card_image_get(other);
  quest_meter_changed[side] = 0;
  if ((game_flags & 0x400) == 0) return;
  if (play_style != 2) return;
  if ((int)champion_side == side) {
    if (cards[side]->header.level > 0x13) {
      if (cards[other]->header.level < 0x14) return;
      ushort *count = &quest_count[side];
      uint n = card_hdr_quest_count_get(&cards[2]->header);
      *count = n;
      if ((ushort)n == 0) return;
      *count = n - 1;
      quest_meter_changed[side] = 1;
      return;
    }
    if (cards[side]->header.level > cards[other]->header.level) return;
    { ushort *count = &quest_count[side];
    uint n = card_hdr_quest_count_get(&cards[2]->header);
    *count = n;
    if ((ushort)n == 0) return;
    *count = n - 1;
    quest_meter_changed[side] = 1;
    return; }
  }
  if (cards[side]->header.level > 0x13) {
    if (0x13 < cards[other]->header.level) {
      ushort *count = &quest_count[side];
      uint n = card_hdr_quest_count_get(&cards[2]->header);
      *count = n;
      if (8 < (ushort)n) return;
      *count = n + 1;
      quest_meter_changed[side] = 1;
    }
  }
  else {
    if (cards[side]->header.level >= cards[other]->header.level) {
      ushort *count = &quest_count[side];
      uint n = card_hdr_quest_count_get(&cards[2]->header);
      *count = n;
      if (8 < (ushort)n) return;
      *count = n + 1;
      quest_meter_changed[side] = 1;
    }
  }
}

void quest_cond_2_04_streak_flag_opponent_win(uint side)
{
  int on;
  int other = (side & 1) ^ 1;
  char winner;
  
  if ((game_flags & 0x400) != 0) {
    if (play_style == '\x02') {
      on = net_masa_is_on();
      if (on != 0) {
        winner = champion_side;
        if (winner == side) {
          if (quest_streak_flag[other] == 1) {
            ((short *)&quest_count)[winner] = 0;
            quest_streak_flag[other] = 0;
          }
        }
      }
    }
  }
  return;
}

void quest_cond_2_05_marked_opponent_win(uint side)
{
  int other;
  card_image_rec *card;
  int winner;
  ushort *count;
  
  other = (side & 1) ^ 1;
  card = card_image_get(side);
  if (((game_flags & 0x400) != 0) && (play_style == 2)) {
    winner = champion_side;
    if (winner == side && net_nushi_rank_get(other) != 0) {
      count = quest_count + winner;
      *count = card_hdr_quest_count_get(&card->header);
      if (*count != 0) *count = *count - 1;
    }
  }
}

void quest_cond_2_01_opponent_flag300_win(uint side)
{
  card_image_rec *card;
  card_image_rec *other_card;
  uint other;
  int winner;
  ushort *count;
  card_image_rec **slot;
  card_image_rec *cards [2];
  
  card = card_image_get(side);
  other = side & 1 ^ 1;
  if (((game_flags & 0x400) != 0) && (play_style == '\x02')) {
    slot = &cards[other];
    *slot = (other_card = card_image_get(other));
    if (((other_card->body.misc.w & 0x300) != 0) &&
       (net_card_place_matches((uchar)other) != 0)) {
      winner = champion_side;
      if (winner == side) {
        count = quest_count + winner;
        *count = card_hdr_quest_count_get(&card->header);
        if (*count != 0) {
          *count = *count + -1;
        }
      }
    }
  }
}

void quest_cond_2_00_listed_opponent_win(int side)
{
  int listed;
  char winner;
  
  if (((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
      (winner = (int)(uchar)champion_side, winner == side)) && (listed = quest_opponent_listed(winner), 0 != listed)) {
    quest_count[winner] = 0;
  }
  return;
}

uint quest_opponent_listed(uint side)
{
  uint listed;
  
  listed = net_kumite_card_listed(side & 1 ^ 1);
  return listed;
}

int quest_in_progress(int side)
{
  int value;
  int active;
  
  active = 0;
  value = net_ready_latched();
  if ((value != 0) && (value = quest_state_get(side), value == 1)) {
    active = 1;
  }
  return active;
}

uint quest_state_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest0_b0_get(&cards[side]->body);
  return value;
}

uint quest_kind_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest0_b3_get(&cards[side]->body);
  return value;
}

uint quest_limit_month_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_limit_month_get(&cards[side]->body);
  return value;
}

uint quest_limit_day_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_limit_day_get(&cards[side]->body);
  return value;
}

uint quest_category_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_category_get(&cards[side]->body);
  return value;
}

uint quest_param_a_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_param_a_get(&cards[side]->body);
  return value;
}

uint quest_param_b_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_param_b_get(&cards[side]->body);
  return value;
}

uint quest_param_c_get(int side)
{
  uint value;
  card_image_rec *cards [2];
  
  cards[side] = card_image_get(side);
  value = card_body_quest_param_c_get(&cards[side]->body);
  return value;
}

bool quest_expired(int side)
{
  int month;
  int day;
  int hour;
  int minute;
  int now_minutes;
  int limit_minutes;
  tm now;
  int year;
  bool expired;
  int y;
  int m;
  
  year = card_quest_year_get(side);
  month = quest_limit_month_get(side);
  day = quest_limit_day_get(side);
  hour = card_quest_hour_get(side);
  minute = card_quest_minute_get(side);
  net_clock_now(&net_check_log,&now);
  y = now.tm_year - 100;
  m = now.tm_mon + 1;
  now_minutes = date_to_minutes(y,m,now.tm_mday,now.tm_hour,now.tm_min);
  limit_minutes = date_to_minutes(year,month,day,hour,minute);
  expired = 0;
  if (now_minutes > limit_minutes) expired = 1;
  return expired;
}

int rob_card_level_pick(int side, int pick_mode)
{
  card_image_rec *card;
  int level;
  char flags; char *entry_flags;

  card = card_image_get(side);
  level = -1;
  if (net_ready_latched() == 0) {
    return -1;
  }
  entry_flags = (char *)side_entry_flags; flags = entry_flags[side];
  if (((flags >> 2) & 1) == 0) {
    return level;
  }
  if (net_masa_is_on()) {
    if (champion_side == side) {
      if (win_streak > 4) {
        if (pick_mode != 2) goto set;
      }
    }
    if ((game_flags & 0x400) != 0) {
      if (pick_mode != 2) goto check;
      if (champion_side != side) {
        quest_streak_flag[side] = 0;
      }
    }
    if (pick_mode == 2) goto main;
  check:
    if (win_streak % 5 == 0 && win_streak != 0) {
      if (champion_side == side) goto main;
    set:
      quest_streak_flag[side] = 1;
      return 13;
    }
    if (quest_streak_flag[side] != 0) {
      return 13;
    }
    quest_streak_flag[side] = 0;
    return level;
  }
  quest_streak_flag[side] = 0;
main:
  if (net_nushi_rank_get(side) != 0) {
    return 10;
  }
  if (quest_expired(side)) {
    return level;
  }
  if (quest_category_get(side) == 2) {
  if (quest_kind_get(side) == 2) {
    if (net_ryuko_is_on()) {
      if (quest_param_b_get(side) == 0) {
        level = 12;
      }
      else if (quest_param_b_get(side) == 1) {
        level = 14;
      }
    }
  }
  else if (quest_kind_get(side) == 3) {
    if (quest_state_get(side) != 1 && pick_mode == 2 && quest_meter_changed[side] == 0) {
      return -1;
    }
    if (quest_state_get(side) != 1 && pick_mode <= 1) {
      quest_meter_changed[side] = 0;
      return -1;
    }
    switch (card_hdr_quest_count_get(&card->header)) {
    case 9:
      level = 0;
      break;
    case 8:
      level = 1;
      break;
    case 7:
      level = 2;
      break;
    case 6:
      level = 3;
      break;
    case 5:
      level = 4;
      break;
    case 4:
      level = 5;
      break;
    case 3:
      level = 6;
      break;
    case 2:
      level = 7;
      break;
    case 1:
      level = 8;
      break;
    case 0:
      level = 9;
      break;
    default:
      level = -1;
      break;
    }
  }
  }
  return level;
}

bool quest_kind_valid(int side)
{
  card_body_rec *body;
  int valid = 1;
  int limit;
  body = &card_image_get(side)->body;
  switch (card_body_quest_category_get(body)) {
  case 0:
    limit = 16;
    if ((int)card_body_quest0_b3_get(body) >= limit) valid = 0;
    break;
  case 1:
    limit = 16;
    if ((int)card_body_quest0_b3_get(body) >= limit) valid = 0;
    break;
  case 2:
    limit = 6;
    if ((int)card_body_quest0_b3_get(body) >= limit) valid = 0;
    break;
  }
  return valid;
}

void mission_sim_vs_draw(void)
{
  tm t;
  card_image_rec *cards[2];
  int side[1];
  int syuchu[1];
  int i;
  int odds;
  int mission;

  side[0] = 0;
  get_now_tm(&t);
  mission_result = side[0];
  cards[0] = card_image_get(0);
  cards[1] = card_image_get(1);
  prize_item_chance[0] = 1;
  prize_item_chance[1] = 1;
  mission_sim_chances_set(cards[0]->header.level,cards[1]->header.level);
  for (i = 0; i < 2; i++) {
    if (card_body_syu_low5_get(&cards[i]->body) != t.tm_mday) {
      card_body_syu_low5_set(&cards[i]->body,0);
    }
  }
  for (i = 0; i < 2; i++) {
    if (i != 1 && card_body_syu_low5_get(&cards[i]->body) == 0) {
      odds = net_syuchu_value0_get();
      if (odds == 0) odds = 800;
      if ((int)lcg_next15() < 0x7fff / odds) {
        card_body_syu_low5_set(&cards[i]->body,(ushort)t.tm_mday);
        prize_test_syuchu_marks = prize_test_syuchu_marks + 1;
      }
    }
  }
  if ((int)bonus_time_left < 1) {
    bonus_time_flags = bonus_time_flags & 0xfffffffe;
  }
  else {
    bonus_time_left = bonus_time_left - 1;
  }
  for (i = 0; i < 2; i++) {
    if (card_body_syu_low5_get(&cards[i]->body) != 0 && (int)lcg_next15() < 0xccc) {
      card_body_syu_low5_set(&cards[i]->body,0);
    }
  }
  if (bonus_time_flags != 0) {
    for (i = 0; i < 2; i++) {
      if ((int)prize_item_chance[i] < 0x4cc8) {
        prize_item_chance[i] = 0x4cc8;
      }
    }
    prize_test_bonus_time_matches = prize_test_bonus_time_matches + 1;
  }
  syuchu[0] = 0;
  for (i = 0; i < 2; i++) {
    if (card_body_syu_low5_get(&cards[i]->body) != 0) {
      syuchu[0] = 1;
      if ((int)prize_item_chance[i] < 0x4cc8) {
        prize_item_chance[i] = 0x4cc8;
      }
    }
  }
  if (syuchu[0] != 0) {
    prize_test_syuchu_matches = prize_test_syuchu_matches + 1;
  }
  for (i = 0; i < 2; i++) {
    if ((int)lcg_next15() >= (int)prize_item_chance[i]) {
      mission_of_side[i] = -1;
    }
    else {
      mission_of_side[i] = 0x1d;
      side[0] = i;
      prize_test_side_prizes[i] = prize_test_side_prizes[i] + 1;
    }
  }
  mission = mission_of_side[side[0]];
  if (mission >= 0) {
    prize_test_prize_matches = prize_test_prize_matches + 1;
    mission_count = ((mission_rec *)&mission_table)[mission].value;
  }
  mission_result = 0;
  mission_tally[0] = 0;
  mission_tally[1] = 0;
}

void mission_sim_item_roll(void)
{
  int roll = 0;
  int i;
  int odds;
  int sum;

  net_info_get();
  if (play_style != 2) return;
  if (net_ready_latched() == 0) return;
  if (net_free_play_is_on() != 0) return;
  if ((bonus_time_flags & 1) != 0) {
    lottery_state = -1;
    lottery_symbol = -1;
    lottery_spins = roll;
    for (i = 0; i < 3; i++) lottery_reels[i] = -1;
    return;
  }
  if (lottery_state < 0) {
    lottery_symbol = -1;
    lottery_spins = 0;
    roll = lcg_next15();
    odds = net_syuchu_value1_get();
    if (odds == 0) odds = 400;
    if (roll < 0x7fff / odds) {
      lottery_state = 0;
      prize_test_lottery_starts = prize_test_lottery_starts + 1;
        for (i = 0; i < 3; i++) lottery_reels[i] = -1;
    }
  }
  if (lottery_state == 0) {
    for (i = 0; i < 3; i++) {
      if ((int)lottery_reels[i] < 0) {
        lottery_symbol = lottery_symbol_draw();
        mission_sim_bonus_roll();
        lottery_reels[i] = lottery_symbol_values[lottery_symbol];
        if (i == 2) lottery_state = 1;
        break;
      }
    }
  }
  if ((bonus_time_flags & 1) != 0) return;
  if (lottery_state == 1) {
    sum = lottery_reels[0] + lottery_reels[1] + lottery_reels[2];
    if (sum < 8) lottery_spins = (int)lcg_next15() % 6;
    else if (sum > 15) lottery_spins = (int)lcg_next15() % 4;
    else lottery_spins = (int)lcg_next15() % 2;
    if (lottery_spins <= 0) goto stop;
    lottery_state = 2;
  }
  if (lottery_state != 2) return;
  if (--lottery_spins >= 0) {
    lottery_symbol = lottery_symbol_draw();
    mission_sim_bonus_roll();
  }
  if (lottery_spins > 0) return;
  roll = (int)lcg_next15() % 10;
  lottery_spins = lottery_spin_lists[lottery_symbol_values[lottery_symbol]][roll];
  if (lottery_spins > 0) return;
stop:
  lottery_state = -1;
}

void mission_sim_bonus_roll(void)
{
  int roll;
  roll = lcg_next15();
  if (roll < 0x7fff / (400 / lottery_bonus_odds[lottery_symbol])) {
    bonus_time_flags = bonus_time_flags | 1;
    sound_state_bit6_set();
    prize_test_bonus_time_starts = prize_test_bonus_time_starts + 1;
    bonus_time_left = (int)lcg_next15() % 21 + 10;
  }
}

void mission_sim_match_result(void)
{
  char prev_champion;
  float rate_0;
  rob_rec *rob0;
  rob_rec *rob1;
  int winner;
  int loser;
  float rate_1;
  
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  winner = 0;
  loser = 1;
  rate_0 = prize_test_win_rate;
  rate_1 = player_win_rate(1);
  prev_champion = champion_side;
  if ((int)lcg_next15() < (ushort)rate_0 * (ushort)0x147) {
    champion_side = 0;
  }
  else if ((int)lcg_next15() < (ushort)rate_1 * (ushort)0x147) {
    champion_side = 1;
  }
  else if ((ushort)(int)rate_0 == 0) {
    champion_side = 1;
  }
  else if ((ushort)(int)rate_0 == 0 && (ushort)(int)rate_1 == 0) {
    champion_side = lcg_next15() & 1;
  }
  if (champion_side == prev_champion) {
    win_streak = win_streak + 1;
  }
  else {
    win_streak = 1;
  }
  if (champion_side == 1) {
    winner = 1;
    loser = 0;
  }
  card_match_result_record(rob0,rob1,winner,loser);
  if ((int)mission_active_get() != -1) {
    prize_give((int)champion_side);
  }
  return;
}

void mission_sim_chances_set(uchar rank_a, uchar rank_b)
{
  int i;
  card_image_rec *cards[2];
  for (i = 0; i < 2; i++) {
    cards[i] = card_image_get(i);
  }
  if (win_streak >= 30) {
    if ((uchar)champion_side == '\0') {
      prize_item_chance[0] = prize_rate_get(9,10);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(9,10);
    }
  }
  else if ((uint)(win_streak - 20) < 10) {
    if ((uchar)champion_side == '\0') {
      prize_item_chance[0] = prize_rate_get(8,8);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(8,8);
    }
  }
  else if ((uint)(win_streak - 10) < 10) {
    if ((uchar)champion_side == '\0') {
      prize_item_chance[0] = prize_rate_get(7,6);
      prize_item_chance[1] = prize_rate_get(3,4);
    }
    else {
      prize_item_chance[0] = prize_rate_get(3,4);
      prize_item_chance[1] = prize_rate_get(7,6);
    }
  }
  else {
    prize_item_chance[0] = prize_rate_get(3,4);
    prize_item_chance[1] = prize_rate_get(3,4);
  }
  if (ranks_matched(rank_a,rank_b) != 0) {
    for (i = 0; i < 2; i++) {
      if (prize_item_chance[i] < prize_rate_get(4,8)) {
        prize_item_chance[i] = prize_rate_get(4,8);
      }
    }
    for (i = 0; i < 2; i++) {
      if (prize_loss_tier(i) == 1) {
        ((struct { int a[2]; } *)&prize_item_chance)->a[i] = prize_rate_get(5,0xc);
        prize_test_loss_tier1_count[i]++;
      }
      else if (prize_loss_tier(i) == 2) {
        ((struct { int a[2]; } *)&prize_item_chance)->a[i] = prize_rate_get(6,0x19);
        prize_test_loss_tier2_count[i]++;
      }
    }
  }
  for (i = 0; i < 2; i++) {
    if (card_body_prize_misses_get(&cards[i]->body) > 29) {
      prize_item_chance[i] = 0x6660;
      prize_test_miss_capped_count[i]++;
    }
  }
}

void card_money_debug_print(void)
{
  card_image_rec *card;
  ushort *money;
  int i;
  
  for (i = 0; i < 2; i++) {
    card = card_image_get(i);
    if (i == 0) {
      text_cursor_to_cell(10,0x14);
      text_draw_formatted(card_money_1p_title);
    }
    else {
      text_cursor_to_cell(0x28,0x14);
      text_draw_formatted(card_money_2p_title);
    }
    money = &card->body.money;
    text_draw_formatted("SEND_DATA  : %d\n",*money);
    text_draw_formatted("ID         : %d\n",(ushort)((*money & 0xf000) >> 0xc));
    text_draw_formatted("SEND_MONEY : %d\n",(ushort)((*money & 0xfc0) >> 6));
    text_draw_formatted("TEMP_MONEY : %d\n",*money & 0x3f);
  }
}

int card_money_send_pending(uint side)
{
  card_image_rec *card;
  int pending;
  
  pending = 0;
  card = card_image_get(side);
  if ((card->body.money & 0xfc0) != 0) pending = 1;
  else if ((card->body.money & 0x3f) != 0) {
      card_money_add(side,0);
      pending = 1;
  }
  return pending;
}

void card_money_add(uint side, int amount)
{
  card_image_rec *card;
  int id;
  int send;
  int temp;
  int units;
  uint word;

  card = card_image_get(side);
  word = (short)card->body.money;
  id = (ushort)((word & 0xf000) >> 12);
  send = (ushort)((word & 0xfc0) >> 6);
  temp = word & 0x3f;
  if (send == 0) {
    id++;
    if (id > 7) id = 0;
    units = amount / 50; send = units + temp;
    temp = 0;
  }
  else {
    temp += amount / 50;
  }
  if (send > 0x3e) send = 0x3f;
  if (temp > 0x3e) temp = 0x3f;
  card->body.money = (id << 12) | (send << 6) | temp;
}

void card_money_reply_apply(uint side, char *reply)
{
  card_image_rec *card;
  int value;
  int id;
  int card_id;
  int send;
  int temp;
  uint word;

  card = card_image_get(side);
  if (reply != 0) {
    value = text_to_int(reply);
    id = (value & 0xf000) >> 12;
    word = (short)card->body.money;
    card_id = (ushort)(word & 0xf000) >> 12;
    if (id == card_id) {
      temp = word & 0x3f;
      send = value & 0xfff;
      if (send != 0) {
        temp = 0;
        send = 0; /* WORKAROUND (kept as possibly original; no game instruction of its own): b = 0 is never read. With it GCC computes the cleared field without a branch (the game's tst / mov #-1 / negc); without it the if becomes a branch. Kept as plausible original code. */
      }
      card->body.money = (id << 12) | temp;
    }
  }
}

int text_to_int(char *text)
{
  int len;
  int i;
  int sum;
  int neg;
  int digit;
  
  sum = 0;
  neg = 0;
  if (strchr(text,0x2d) != (char *)0x0) neg = 1;
  len = strlen(text);
  for (i = neg; i < len; i++) {
    digit = net_hex_digit_value((int)text[i]);
    sum = sum + digit * int_power(10,(len - i) + -1);
  }
  if (neg != 0) {
    sum = -sum;
  }
  return sum;
}

int int_power(int base, int exponent)
{
  int result;
  int i;
  
  result = 1;
  for (i = 0; i < exponent; i = i + 1) {
      result = result * base;
  }
  return result;
}
