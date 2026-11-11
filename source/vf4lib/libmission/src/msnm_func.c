/*
 * vf4lib/libmission/src/msnm_func.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_msnm_try_33_step
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int msnm_hyper_counts[3]; /* 0x0c2a0e24 */
extern int msnm_try01_follow_moves[20]; /* 0x0c19fe88 */
extern int round_time; /* 0x0c292558 */
extern undefined4 match_round_time; /* 0x0c29255c */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern int win_streak; /* 0x0c2925dc */
extern int msnm_listed_states[6]; /* 0x0c2a0df8 */
extern ushort msnm_lever_dirs[10]; /* 0x0c2a0e10 */

void msnm_try_hyper_pick(msnm_try_setting_rec *setup, int set, int chara)
{
  char char_num;
  int j;
  int n;
  int mot;
  int type;
  int m;
  int first;
  int pick;
  int count;
  int type_max[3];

  char_num = (char)chara;
  for (j = 0; j < 4; j++) {
    setup->hyper_kinds[j] = -1;
    setup->hyper_entries[j] = -1;
  }
  count = msnm_hyper_counts[set];
  for (j = 0; j < 3; j++) {
    type_max[j] = 0;
    for (m = 0; msnm_hyper_mot_get(char_num,j,m,set) != 0; m++) {
    }
    type_max[j] = m;
  }
  if (type_max[0] <= 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmission/src/msnm_func.c",0x2ef,"assertion `%s\' failed.\n"
             ,"type_max[MSNM_TRY_HYPER_TYPE_A] > 0");
  }
  setup->hyper_kinds[0] = 0;
  setup->hyper_entries[0] = (int)(msnm_random_unit() * (float)type_max[0]);
  for (n = 1; n < count; ) {
    first = (int)(msnm_random_unit() * 3.0);
    for (j = 0; j < 3; j++) {
      type = first + j;
      if (type > 2) {
        type -= 3;
      }
      if (type_max[type] != 0) {
        pick = (int)(msnm_random_unit() * (float)type_max[type]);
        mot = msnm_hyper_mot_get(char_num,type,pick,set);
        for (m = 0; m < n; m++) {
          if (setup->hyper_kinds[m] != -1 &&
              mot == msnm_hyper_mot_get(char_num,setup->hyper_kinds[m],setup->hyper_entries[m],set)) break;
        }
        if (m == n) {
          setup->hyper_kinds[n] = type;
          setup->hyper_entries[n] = pick;
          n++;
          break;
        }
      }
    }
  }
}

ushort *msnm_hyper_mot_table_get(char char_num, int type, int set)
{
  if (char_num == 0xc && set == 0) {
    return msnm_hyper_mots_chara12[type];
  }
  if (type == 0) {
    return msnm_hyper_mots_type0[char_num];
  }
  if (type == 2) {
    return msnm_hyper_mots_type2[char_num];
  }
  if (type == 1) {
    return msnm_hyper_mots_type1[char_num];
  }
  return 0;
}

ushort msnm_hyper_mot_get(char char_num, int type, int index, int set)
{
  ushort *mot_tbl;
  
  if (0xf < (uint)(int)(char)char_num) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmission/src/msnm_func.c",0x337,"assertion `%s\' failed.\n"
             ,"char_num >= 0 && char_num <= 15");
  }
  mot_tbl = msnm_hyper_mot_table_get((int)(char)char_num,type,set);
  if (mot_tbl == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmission/src/msnm_func.c",0x339,"assertion `%s\' failed.\n"
             ,"mot_tbl");
  }
  return mot_tbl[index];
}

void msnm_try_00_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int wait;
  int stun;
  int left;
  int adv;

  wait = hit_wait_timer_get();
  stun = rob->stun;
  left = opp->recovery_end - opp->frame;
  if (left < wait) left = wait;
  adv = stun - left;
  msnm_try_00_debug_stub(adv, rob->stun, wait, stun, left);
  switch (*(uint *)(progress + 0x18)) {
  case 0:
    if (msnm_rob_8001_vs_attack_check(rob,opp) == 0) return;
    if (adv > -8) return;
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    if ((opp->state & 2) != 0) {
      *(int *)(progress + 0x18) = 0;
      break;
    }
    if (stun > 0) return;
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 2:
    if (left <= 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      break;
    }
    if ((rob->flags28 & 0x20000000) != 0) {
      *(int *)(progress + 0x18) = 4;
      break;
    }
    if ((opp->state & 2) != 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case 3:
    if ((rob->flags28 & 0x20000000) != 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    else {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case 4:
    if ((rob->flags28 & 0x20000000) == 0) {
      if ((opp->flags28 & 0x08000000) == 0) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 0;
    }
    if (opp->life == 0) {
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
}

void msnm_try_00_debug_stub(int adv, int stun, int wait, int stun_copy, int left)
{
  return;
}

void msnm_try_01_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  typedef struct { uint w[20]; } blk;
  int i;
  int starters[5];
  blk follows;
  
  starters[0] = 0x19f;
  starters[1] = 0x1e5;
  starters[2] = 0x2fe;
  starters[3] = 0xa5f;
  starters[4] = 0x227;
  follows = *(blk *)&msnm_try01_follow_moves;
  if (*(char *)(progress + 0x10) == '\0') {
    switch (*(int *)(progress + 0x18)) {
    case 0:
      if (msnm_rob_move_in_list(rob,starters,5) != 0) {
        *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      }
      break;
    case 1:
      if (msnm_rob_move_in_list(rob,starters,5) == 0) {
        for (i = 0; follows.w[i] != 0xffffffff; i++) {
          if ((uint)rob->move_no == follows.w[i]) {
            *(int *)(progress + 0x18) = 0;
            break;
          }
        }
        if (follows.w[i] == 0xffffffff) {
          *(char *)(progress + 0x10) = 1;
        }
      }
      break;
    }
  }
}

void msnm_try_03_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint flags;
  
  flags = msnm_try_flags_get((int *)&msnm_work->cur_try);
  if (*(char *)(progress + 0x1c) == '\0') {
    if (msnm_rob_state_in_list(rob,msnm_listed_states,6) != 0) {
      *(char *)((flags & 2) ? progress + 0x10 : progress + 0x1c) = 1;
    }
  }
}

void msnm_try_03_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint flags;
  
  flags = msnm_try_flags_get((int *)&msnm_work->cur_try);
  if (!((flags & 2) == 0)) {
    msnm_try_match_win_count(rob,opp,try_rec,progress);
  }
  else {
    if (((uint)rob->side == (int)(round_winner)) && (*(char *)(progress + 0x1c) == '\0')) {
      msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
    }
  }
  return;
}

void msnm_try_05_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if ((opp->flags28 & 0x20000000) != 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if ((opp->flags28 & 0x20000000) == 0) {
      if ((rob->flags28 & 0x8000000) != 0) {
        msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
}

void msnm_try_06_round_ringout_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && ((game_flags & 0x40) != 0)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_07_round_win_no_ringout(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && ((game_flags & 0x40) == 0)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_08_round_ringout_fails(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && ((game_flags & 0x40) != 0)) {
    *(char *)(progress + 0x10) = 1;
  }
  msnm_try_match_win_count(rob,opp,try_rec,progress);
  return;
}

void msnm_try_10_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  short stun;
  int left;
  int adv;
  
  stun = rob->stun;
  left = (int)opp->recovery_end - (int)opp->frame;
  adv = stun - left;
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (msnm_rob_8001_vs_attack_check(rob,opp) != 0) {
      if (adv <= -8) *(int *)(progress + 0x18) += 1;
    }
    break;
  case 1:
    if ((opp->state & 2) != 0) {
      *(int *)(progress + 0x18) = 0;
    }
    else if (stun <= 0) {
      *(int *)(progress + 0x18) += 1;
    }
    break;
  case 2:
    if ((opp->state & 2) != 0) {
      *(int *)(progress + 0x18) = 0;
    }
    else if (msnm_rob_hit_record_check(rob) != 0) {
      msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = 0;
    }
    else if (left <= 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  return;
}

void msnm_try_11_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;

  switch (*(int *)(progress + 0x18)) {
  case 0:
    if ((rob->coli->field_0x1c[1] != 0) &&
       (rob->coli->field_0x94 == '\r')) {
      msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    result = msnm_rob_flags_1000201_check(opp);
    if (result == 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
}

void msnm_try_12_round_loss_fails(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if ((int)(round_winner) != (uint)rob->side) {
    *(char *)(progress + 0x10) = 1;
  }
  msnm_try_match_win_count(rob,opp,try_rec,progress);
  return;
}

void msnm_try_14_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if ((rob->coli->field_0x3c[0] & 2) != 0) {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_15_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  result = msnm_rob_phase_watch(opp,progress);
  if (result != 0) {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_16_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  switch (*(int *)(progress + 0x18)) {
  case 0:
    result = msnm_rob_state_in_list(rob,msnm_listed_states,2);
    if (result == 0) {
      return;
    }
    if (rob->frame >= rob->recovery_end) {
      return;
    }
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    if (((((rob->state & 2) == 0) || ((rob->move_flags & 0x4000) == 0)) &&
        (result = msnm_rob_hit_record_check(opp), result == 0)) &&
       (((rob->flags28 & 0x20000000) == 0 &&
        (rob->frame < rob->recovery_end)))) {
      return;
    }
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
    *(int *)(progress + 0x18) = 0;
    break;
  }
}

void msnm_try_17_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uchar chara;
  chara = rob->chara;
  switch(*(uint *)(progress + 0x18)) {
  case 0:
    if (msnm_rob_state_in_list(rob,(uint *)&msnm_listed_states,4) != 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if ((chara == 9 || (chara == 12 && rob->stance == 1)) &&
        (rob->flags & 0x04000000) != 0) {
      *(int *)(progress + 0x18) = 0;
    }
    else if ((rob->state & 2) != 0 && (rob->move_flags & 0x4000) != 0) {
      if ((chara == 9 || (chara == 12 && rob->stance == 1)) &&
          (rob->state & 0x04000000) == 0) {
        *(int *)(progress + 0x18) = 0;
      }
      else {
        *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      }
    }
    else {
      if (rob->frame >= rob->recovery_end) {
        *(int *)(progress + 0x18) = 0;
      }
    }
    break;
  case 2:
    if (chara == 9 || (chara == 12 && rob->stance == 1)) {
      if ((rob->state & 0x100000) != 0) {
        *(int *)(progress + 0x18) = 4;
      }
    }
    else {
      if (msnm_rob_hit_record_check(rob) != 0) {
        if ((int)rob->flags28 < 0) {
          msnm_try_count_up(rob->side,try_rec,(int *)progress);
        }
        *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      }
    }
  case 3:
    if (rob->frame >= rob->recovery_end) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case 4:
    if ((rob->state & 0x100000) == 0) {
      if ((opp->state & 0x100000) == 0) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 0;
    }
    if (opp->life == 0) {
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = 0;
    }
  }
  return;
}

void msnm_try_18_frame_damage_score(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (msnm_combo_damage_track(rob,opp,progress,0) != 0) {
    if (*(int *)(progress + 0x20) < 0xb) {
      *(int *)(progress + 0xc) += 10;
    }
    else if (*(int *)(progress + 0x20) < 0x1f) {
      *(int *)(progress + 0xc) += 0x32;
    }
    else if (*(int *)(progress + 0x20) < 0x3d) {
      *(int *)(progress + 0xc) += 100;
    }
    else {
      *(int *)(progress + 0xc) += 200;
    }
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
}

void msnm_try_19_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (*(char *)(progress + 0x1c) != '\0') {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  *(char *)(progress + 0x1c) = 0;
  return;
}

void msnm_try_20_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  if ((*(char *)(progress + 0x10) == '\0') && (result = msnm_rob_8001_vs_attack_check(rob,opp), result != 0)) {
    *(char *)(progress + 0x10) = '\x01';
  }
  return;
}

void msnm_try_22_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  char level;
  switch (*(int *)(progress + 0x18)) {
  case 0:
    level = opp->attack_level;
    if (((level == 12) | (level == 14)) && (opp->state & 2) != 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if (opp->frame >= opp->recovery_end) {
      msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = 0;
    }
    else if (opp->coli->field_0x1c[1] != 0 && opp->coli->field_0x94 != 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
  case 2:
    if (opp->frame >= opp->recovery_end) *(int *)(progress + 0x18) = 0;
    break;
  }
}

void msnm_try_23_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((rob->coli->flags & 0x2000) != 0) ||
     ((opp->coli->flags & 0x2000) != 0)) {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_24_round_fast_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int secs = *(int *)(try_rec + 8);

  if (round_winner == rob->side &&
      (int)match_round_time - round_time <= secs * 0x3c && (game_flags & 0xa0) != 0) {
    msnm_try_count_up((char)rob->side,try_rec,(int *)progress);
  }
}

void msnm_try_25_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint flags;
  int limit;
  int next;
  rob_rec *who;
  uint state;
  int damage;
  
  flags = msnm_try_flags_get((int *)&msnm_work->cur_try);
  limit = *(int *)(try_rec + 8);
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (msnm_rob_hit_record_check(rob) != 0) {
      if (rob->attack_damage >= limit) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
        next = -2;
        break;
      }
      goto fail;
    }
    if (msnm_rob_hit_record_check(opp) != 0) {
      goto fail;
    }
    if ((rob->state & 0x100000) != 0) {
      *(char *)(progress + 0x1c) = 1;
    }
    else if ((opp->state & 0x100000) != 0) {
      *(char *)(progress + 0x1c) = 0;
    }
    else {
      return;
    }
    next = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    who = *(char *)(progress + 0x1c) != 0 ? rob : opp;
    state = who->state;
    if ((state & 0x100000) != 0) {
      return;
    }
    damage = opp->life_max - opp->life;
    if (((int)state >> 21 & 1 & (damage == 0)) != 0) {
      next = *(int *)(progress + 0x18) + 1;
      break;
    }
    if (*(char *)(progress + 0x1c) == 0) {
      goto fail;
    }
    if (damage >= limit) {
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = -2;
    }
    if (damage == 0) {
      *(int *)(progress + 0x18) = damage;
      return;
    }
  fail:
    next = -1;
    break;
  case 2:
    if ((rob->state & 0x300000) != 0) {
      return;
    }
    next = opp->state & 0x300000;
    if (next != 0) {
      return;
    }
    break;
  case -1:
    if ((flags & 2) != 0) {
      msnm_try_count_clear((int *)progress);
    }
    next = -2;
    break;
  case -2:
  default:
    return;
  }
  *(int *)(progress + 0x18) = next;
  return;
}

void msnm_try_27_frame_tally(rob_rec *rob, rob_rec *opp, int try_rec, int *progress)
{
  int target = *((int *) (try_rec + 4));
  int *p;
  if (((char) progress[7]) != 0)
  {
    p = progress;
    if (p[8] == (rob->side))
    {
      if ((++(*p)) >= target)
      {
        msnm_status_set(rob->side, 2);
      }
    }
    else
    {
      int floor;
      --(*p);
      floor = -target;
      if ((*p) <= floor)
      {
        *p = floor;
      }
    }
    *((char *) (progress + 7)) = 0;
  }
}

void msnm_try_28_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  if (((*(char *)(progress + 0x1c) == '\0') && (result = msnm_rob_8001_vs_attack_check(rob,opp), result != 0))
     && (opp->attack_level == '\x01' || opp->attack_level == '\a')) {
    *(char *)(progress + 0x1c) = '\x01';
  }
  return;
}

void msnm_try_28_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && (*(char *)(progress + 0x1c) == '\0')) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_29_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if ((opp->combo[2].hits == 1) && ((rob->flags28 & 0x20000000) == 0)) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if ((int)opp->combo[0].damage >= *(int *)(try_rec + 8)) {
      msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0xc) =
           ((int)opp->combo[0].damage - *(int *)(try_rec + 8)) + *(int *)(progress + 0xc);
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    else {
      if (opp->combo[2].hits == 0) {
        *(int *)(progress + 0x18) = 0;
      }
    }
    break;
  case 2:
    if (opp->combo[2].hits == 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  return;
}

void msnm_try_30_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (opp->combo[2].hits == 1 && (rob->flags28 & 0x20000000) == 0) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if (((int)opp->combo[2].hits >= *(int *)(try_rec + 8)) &&
       ((opp->state & 0x100000) == 0)) {
      msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0xc) =
           ((int)opp->combo[2].hits - *(int *)(try_rec + 8)) + *(int *)(progress + 0xc);
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      break;
    }
  case 2:
    if (opp->combo[2].hits == 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  if (*(int *)(progress + 0x14) < (int)opp->combo[2].hits) {
    *(int *)(progress + 0x14) = (int)opp->combo[2].hits;
  }
}

void msnm_try_31_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  result = msnm_rob_hit_record_check(rob);
  if (result != 0) {
    *(int *)(progress + 0x14) = *(int *)(progress + 0x14) + 1;
  }
  return;
}

void msnm_try_31_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if ((((game_flags & 0x400) != 0) && ((int)(champion_side) == (uint)rob->side)) &&
     (*(int *)(progress + 0x14) < *(int *)(try_rec + 4))) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_32_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (opp->frame < opp->active_start - 1) return;
    if (opp->frame >= opp->recovery_end) return;
    if ((opp->move_flags & 0x4000) == 0) return;
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    if (msnm_rob_8001_vs_attack_check(rob,opp) != 0 || msnm_rob_hit_record_check(opp) != 0 ||
        (rob->flags28 & 0x2000000) != 0 || (opp->flags28 & 0x20000000) != 0) {
      msnm_try_count_clear((int *)progress);
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    else {
      if ((opp->state & 2) == 0 && opp->frame < opp->recovery_end) return;
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case 2:
    if ((opp->state & 2) == 0 && opp->frame < opp->recovery_end) return;
    *(int *)(progress + 0x18) = 0;
    break;
  }
}

void msnm_try_33_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint flags;
  int result;
  char flag_4;
  
  flags = msnm_try_flags_get((int *)&msnm_work->cur_try);
  flag_4 = 0;
  if ((flags & 4) != 0) flag_4 = 1;
  result = msnm_try_33_step(rob,opp,try_rec,progress,flag_4);
  if (result != 0) {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  return;
}

int msnm_try_33_step(rob, opp, try_rec, progress, flag_4) rob_rec *rob; rob_rec *opp; int try_rec; int progress; uchar flag_4;
{
  uint press;
  uint press_low;
  int limit;
  int count;
  int ret;
  press = msnm_input_hist_press_get((char)rob->side,0);
  press_low = press & 7;
  limit = *(int *)(try_rec + 8);
  count = msnm_try_33_dir_count(rob,opp,progress);
  ret = 0;
  switch (*(int *)(progress + 0x18)) {
  case 0:
    *(int *)(progress + 0x20) = 0;
    *(char *)(progress + 0x1c) = 0;
    if (msnm_rob_listed_state_check(rob,2) != 0) {
      if (count >= limit) {
        if (flag_4 != 0) {
          *(int *)(progress + 0x18) = 4;
        }
        else {
          ret = 1;
          *(int *)(progress + 0xc) = count - limit + *(int *)(progress + 0xc);
          *(int *)(progress + 0x18) = -1;
        }
      }
      else {
        *(int *)(progress + 0x20) = rob->move_length - rob->frame;
        *(int *)(progress + 0x18) = 1;
      }
    }
    else if (msnm_rob_listed_state_check(rob,4) != 0) {
      *(int *)(progress + 0x20) = rob->move_length - rob->frame;
      *(int *)(progress + 0x18) = 2;
    }
    else if ((opp->flags28 & 0x20000000) != 0) {
      if (msnm_input_c0_release_find(rob) != -1) {
        *(int *)(progress + 0x18) = 3;
      }
      else {
        *(int *)(progress + 0x18) = -2;
      }
    }
    break;
  case 1:
    if (count >= limit) {
      if (flag_4 != 0) {
        *(int *)(progress + 0x18) = 4;
      }
      else {
        ret = 1;
        *(int *)(progress + 0xc) = count - limit + *(int *)(progress + 0xc);
        *(int *)(progress + 0x18) = -1;
      }
    }
    else if (*(int *)(progress + 0x20) <= 0) {
      *(int *)(progress + 0x18) = 0;
    }
    *(int *)(progress + 0x20) = *(int *)(progress + 0x20) - 1;
    break;
  case 2:
    if (((int)opp->flags28 >> 0x1d & 1 & (count >= limit)) != 0) {
      *(int *)(progress + 0x18) = 3;
    }
    else if (*(int *)(progress + 0x20) <= 0) {
      *(int *)(progress + 0x18) = 0;
    }
    *(int *)(progress + 0x20) = *(int *)(progress + 0x20) - 1;
    break;
  case 3:
    if (press_low == 1) {
      *(char *)(progress + 0x1c) = press_low;
    }
    if (((int)rob->flags28 >> 0x1b & 1 & (count >= limit)) != 0) {
      if (flag_4 == 0 || *(char *)(progress + 0x1c) != 0) {
        ret = 1;
        *(int *)(progress + 0xc) = count - limit + *(int *)(progress + 0xc);
        *(int *)(progress + 0x18) = -1;
      }
    }
    else if ((opp->flags28 & 0x20000000) == 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case 4:
    if ((rob->state & 0x8000) != 0) {
      ret = 1;
      *(int *)(progress + 0xc) = count - limit + *(int *)(progress + 0xc);
      *(int *)(progress + 0x18) = 0;
    }
    else if ((rob->state & 0x1002) != 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case -1:
    if (rob->frame >= rob->recovery_end) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  case -2:
    if ((opp->flags28 & 0x20000000) == 0) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  msnm_try_33_debug_stub(rob,count);
  return ret;
}

int msnm_rob_listed_state_check(rob_rec *rob, int end)
{
  int i;
  
  if (rob->frame < rob->recovery_end) {
    i = end + -2;
    do {
      if (((int)rob->action.part.move & 0x7fffU) == msnm_listed_states[i]) {
        return 1;
      }
      i = i + 1;
    } while (i < end);
  }
  return 0;
}

int msnm_try_33_dir_count(rob_rec *rob, rob_rec *opp, int progress)
{
  int count;
  
  switch(*(int *)(progress + 0x18)) {
  case 0:
    {
      int z = 0;
      *(int *)(progress + 0x28) = z;
      if (2 < (byte)(opp->attack_level - 8U)) {
        *(int *)(progress + 0x24) = z;
      }
    }
  case 1:
  case 2:
    count = msnm_lever_dirs_used_count(rob,(uint *)(progress + 0x24));
    break;
  case 3:
    if (((opp->flags28 & 0x20000000) != 0) &&
       (opp->frame <= opp->active_end)) {
      count = msnm_lever_dirs_used_count(rob,(uint *)(progress + 0x24));
      *(int *)(progress + 0x28) = count;
    }
    count = *(int *)(progress + 0x28);
    break;
  case -2:
  case -1:
  default:
    count = 0;
  }
  return count;
}

void msnm_try_33_debug_stub(rob_rec *rob, int count)
{
  return;
}

void msnm_try_35_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (msnm_rob_hit_record_check(rob) != 0) {
      if (rob->attack_level == '\f' || rob->attack_level == '\x0e') {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      (*(int *)(progress + 0x18))++;
    }
  case 1:
    if (rob->frame >= rob->recovery_end) {
      *(int *)(progress + 0x18) = 0;
    }
    break;
  default:
    return;
  }
}

void msnm_try_36_round_win_streak(rob_rec *rob, rob_rec *opp, int try_rec)
{
  int target = *(int *)(try_rec + 4);
  if ((game_flags & 0x400) != 0 && round_winner == rob->side &&
      champion_side == round_winner && win_streak >= target - 1) {
    msnm_status_set(rob->side,2);
  }
}

void msnm_try_37_round_low_energy_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int limit = *(int *)(try_rec + 8);
  if (((int)rob->life <= limit) &&
     ((int)round_winner == (uint)rob->side)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_38_frame_timed_hit(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int secs;
  int secs_left;
  
  secs = *(int *)(try_rec + 8);
  secs_left = round_time / 0x3c;
  if (*(char *)(progress + 0x1c) == '\0') {
    if (secs_left == secs) {
      if (msnm_rob_hit_record_check(rob) != 0) {
        msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
        *(char *)(progress + 0x1c) = '\x01';
      }
    }
    else if (secs_left < secs) {
      *(char *)(progress + 0x1c) = 1;
    }
  }
}

void msnm_try_39_round_perfect_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && ((game_flags & 0x80) != 0)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_40_frame_hit_score(rob_rec *rob, rob_rec *opp, int try_rec, int *progress)
{
  if (msnm_rob_hit_record_check(rob) != 0) {
    if (rob->attack_damage < 0xb) {
      progress[3] += 0x14;
    }
    else if (rob->attack_damage < 0x1a) {
      progress[3] += 0x32;
    }
    else {
      progress[3] += 100;
    }
    msnm_try_count_up(rob->side,try_rec,(int *)progress);
  }
}

void msnm_try_41_round_ko_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && ((game_flags & 0x20) != 0)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_round_win(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if ((int)(round_winner) == (uint)rob->side) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_43_frame_limit(rob_rec *rob, rob_rec *opp, int try_rec, int *progress)
{
  if (msnm_rob_phase_watch(rob,(int)progress) != 0) {
    if (++*progress >= *(int *)(try_rec + 4)) {
      msnm_status_set((int)rob->side,3);
    }
  }
}

void msnm_try_43_round(rob_rec *rob)
{
  int status;
  
  if (((game_flags & 0x400) != 0) && (status = msnm_status_get((int)rob->side), status == 1)) {
    msnm_status_set((int)rob->side,2);
  }
  return;
}

void msnm_try_44_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((rob->side_flags & 0x20) != 0) && ((rob->move_flags & 0x10000) != 0))
  {
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_45_frame_energy_floor(rob_rec *rob, rob_rec *opp, int try_rec)
{
  if ((int)rob->life < *(int *)(try_rec + 4)) {
    msnm_status_set((int)rob->side,3);
  }
  return;
}

void msnm_try_45_round_energy_kept(rob_rec *rob, rob_rec *opp, int try_rec)
{
  if (((game_flags & 0x400) != 0) && ((int)rob->life >= *(int *)(try_rec + 8))) {
    msnm_status_set((int)rob->side,2);
  }
  return;
}

void msnm_try_46_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  result = msnm_combo_damage_track(rob,opp,progress,0);
  if (result != 0) {
    *(int *)(progress + 0x14) = *(int *)(progress + 0x14) + 1;
  }
  return;
}

void msnm_try_46_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if ((((game_flags & 0x400) != 0) && ((int)(champion_side) == (uint)rob->side)) &&
     (*(int *)(progress + 0x14) < *(int *)(try_rec + 4))) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_47_frame_new_moves(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  ushort *moves;
  
  moves = (ushort *)msnm_work_b4_get();
  switch (*(int *)(progress + 0x18)) {
  case 0:
    *moves = 0;
    if ((rob->flags28 & 0x20000000) != 0) {
      *moves = rob->action.part.move & 0x7fff;
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    }
    break;
  case 1:
    if ((rob->flags28 & 0x20000000) == 0) {
      if (((opp->flags28 & 0x08000000) == 0) &&
         (msnm_move_list_add_new((uint)*moves,moves) != 0)) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 0;
    }
    else if (*moves != ((struct { ushort v : 15; } *)&rob->action)->v) {
      if (msnm_move_list_add_new((uint)*moves,moves) != 0) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      *moves = rob->action.part.move & 0x7fff;
    }
    if (opp->life == 0) {
      if (msnm_move_list_add_new((uint)*moves,moves) != 0) {
        msnm_try_count_up(rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  msnm_try_47_debug_stub();
}

int msnm_move_list_add_new(uint move_no, ushort *list)
{
  ushort move = move_no;
  ushort *slot;
  int i;

  for (i = 0; i < 10; i++) {
    slot = list + i + 1;
    if (*slot == move) break;
    if (*slot == 0) {
      *slot = move;
      return 1;
    }
  }
  return 0;
}

void msnm_try_47_debug_stub(void)
{
  return;
}

void msnm_try_cmd_rows_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  char *work;
  char *row;
  int i;
  int j;
  int done;
  
  work = (char *)msnm_work_b4_get();
  for (i = 0; i < 6; i++) {
    row = work + i * 0xa8 + 0x38;
    if (msnm_cmd_entry_is_end(row,0) != 0) break;
    done = 0;
    switch (*(int *)(work + i * 4 + 0x20)) {
    case 0:
      done = msnm_cmd_row_step(rob,opp,work,row,i);
      break;
    case 1:
      done = msnm_cmd_row_step_kind1(rob,opp,work,row,i);
      break;
    case 2:
      done = msnm_cmd_row_step_kind2(rob,opp,work,row,i);
      break;
    }
    if (done != 0) {
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      for (j = 0; j < 6; j++) {
        *(int *)(work + j * 4 + 0x428) = 0;
      }
      break;
    }
  }
  msnm_cmd_rows_debug_stub(rob,work);
}

int msnm_cmd_entry_is_end(char *row, int i)
{
  typedef struct { int a; ushort b; } ent8;
  ent8 *p = (ent8 *)(row + i * 8);
  ent8 *z = (ent8 *)(msnm_try01_follow_moves + 20);
  if (p->a == z->a && p->b == z->b) return 1;
  return 0;
}

int msnm_move_matches(uint cur_move, uint row_move, char *links)
{
  typedef struct { ushort v[3]; } link_ent;
  link_ent *t = (link_ent *)links;
  ushort a = cur_move;
  ushort b = row_move;
  link_ent z = {{0}};
  link_ent *e;
  int i;
  int j;
  
  if (a == b) return 1;
  if (t != 0) {
    for (i = 0; i < 0x10; i++) {
      e = (link_ent *)(i * 6 + (int)t);
      if (e->v[0] == z.v[0] && e->v[1] == z.v[1] && e->v[2] == z.v[2]) break;
      if (t[i].v[0] == b) {
        for (j = 1; j < 3; j++) {
          if (t[i].v[j] == a) return 1;
        }
      }
    }
  }
  return 0;
}

void msnm_cmd_rows_debug_stub(rob_rec *rob, char *work)
{
  return;
}

int msnm_cmd_row_step(rob_rec *rob, rob_rec *opp, char *work, char *row, int idx)
{
  uint *ent;
  int *step;
  if (opp->combo[0].hits > 0) {
    { int o = idx * 4 + (int)work; step = (int *)(o + 0x428); }
    ent = (uint *)(row + *step * 8);
    if (((rob->coli->field_0x3c[0] & *ent) != 0) &&
       (msnm_move_matches((uint)rob->move_no,(uint)(ushort)ent[1],
                             *(char **)(work + 0x1c)) != 0)) {
      if (msnm_cmd_entry_is_end(row,++*step) != 0) {
        return 1;
      }
    }
  }
  else {
    *(int *)(idx * 4 + work + 0x428) = 0;
  }
  return 0;
}

int msnm_cmd_row_step_kind1(rob_rec *rob, rob_rec *opp, char *work, char *row, int idx)
{
  int *step;
  char *ent;
  int done;
  int ret;
  { int t = idx * 4 + (int)work; step = (int *)(t + 0x428); }
  ent = row + *step * 8;
  if (*step == 0) {
    if ((rob->state & 0x100000) == 0) goto fail;
    if (msnm_move_matches((uint)rob->move_no,(uint)*(ushort *)(ent + 4),*(char **)(work + 0x1c)) == 0) goto fail;
    *step = *step + 1;
    goto fail;
  }
  { int t = idx * 4 + (int)work; step = (int *)(t + 0x428); }
  if (*step == 1) {
    if ((opp->state & 0x200000) != 0) goto fail;
    if (msnm_move_matches((uint)rob->move_no,(uint)*(ushort *)(ent + 4),*(char **)(work + 0x1c)) != 0) {
      done = msnm_cmd_entry_is_end(row,++*step);
    }
    else {
      *(int *)(idx * 4 + work + 0x428) = 0;
      goto fail;
    }
  }
  else {
    done = msnm_cmd_row_step(rob,opp,work,row,idx);
  }
  if (done != 0) { ret = 1; goto out; }
fail:
  ret = 0;
out:
  return ret;
}

int msnm_cmd_row_step_kind2(rob_rec *rob, rob_rec *opp, char *work, char *row, int idx)
{
  char *ent;
  int done;
  
  ent = row + ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] * 8;
  if (((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] == 0) {
    if ((rob->coli->field_0x3c[0] & 0xc0) == 0) {
      goto fail;
    }
    if (msnm_move_matches(rob->move_no,*(ushort *)(ent + 4),*(char **)(work + 0x1c)) == 0) {
      goto fail;
    }
    ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] = ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] + 1;
    goto fail;
  }
  if (((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] == 1) {
    if ((opp->state & 0x1000000) != 0) {
      goto fail;
    }
    if (msnm_move_matches(rob->move_no,*(ushort *)(ent + 4),*(char **)(work + 0x1c)) != 0) {
      ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] = ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] + 1;
      done = msnm_cmd_entry_is_end(row,((struct { char pad[0x428]; int a[4]; } *)work)->a[idx]);
    }
    else {
      ((struct { char pad[0x428]; int a[4]; } *)work)->a[idx] = 0;
      goto fail;
    }
  }
  else {
    done = msnm_cmd_row_step(rob,opp,work,row,idx);
  }
  if (done != 0) {
    return 1;
  }
fail:
  return 0;
}

void msnm_try_55_frame_combo_starter(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  ushort move;
  char *work;
  int i;
  
  work = (char *)msnm_work_b4_get();
  i = msnm_combo_damage_track(rob,opp,progress,work);
  if (i != 0) {
    i = 0;
    do {
      move = *(short *)(i * 2 + work + 0x440);
      if (move == 0) {
        return;
      }
      if (*(ushort *)(work + 0x446) == move) {
        msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
        return;
      }
      i = i + 1;
    } while (i < 3);
  }
  return;
}

void msnm_try_56_frame(rob_rec *rob, rob_rec *opp, int try_rec, int *progress)
{
  switch (progress[6]) {
  case 0:
    if (rob->coli->field_0x1c[1] != 0 &&
        rob->coli->field_0x94 == '\r') {
      progress[6] = progress[6] + 1;
    }
    break;
  case 1:
    if (msnm_rob_flags_1000201_check(opp) == 0) {
      progress[6] = 0;
    }
    else if (opp->combo[2].hits >= *(int *)(try_rec + 8) &&
             (rob->state & 0x100000) == 0) {
      progress[6] = progress[6] + 1;
      *progress = *progress + 1;
      progress[8] = progress[8] + 1;
    }
    break;
  case 2:
    if (opp->combo[2].hits == 0) {
      progress[6] = 0;
    }
    break;
  }
}

void msnm_try_56_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && (*(int *)(progress + 0x20) >= *(int *)(try_rec + 4)))
  {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_57_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int result;
  
  result = msnm_rob_phase_watch(opp,progress);
  if (result != 0) {
    *(int *)(progress + 0x20) = 1;
  }
  return;
}

void msnm_try_57_round(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  if (((int)(round_winner) == (uint)rob->side) && (*(int *)(progress + 0x20) == 0)) {
    msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
  }
  return;
}

void msnm_try_62_frame_all_entries(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  char *work;
  char *rows;
  int n;
  int i;
  int hits;
  int t;
  uint *ent;

  work = (char *)msnm_work_b4_get();
  rows = work + 0x38;
  if (0 < opp->combo[0].hits) {
    n = 0;
    for (i = 0; i < 0x15; i++) {
      if (msnm_cmd_entry_is_end(rows,i) != 0) break;
      n = n + 1;
      ent = (uint *)(rows + i * 8);
      if (((rob->coli->field_0x3c[0] & *ent) != 0) &&
         (msnm_move_matches((uint)rob->move_no,(uint)*(ushort *)(ent + 1),
                       *(char **)(work + 0x1c)) != 0)) {
        *(uint *)(work + 0x428) |= 1 << i;
      }
    }
    hits = 0;
    for (i = 0; i < 0x15; i++) {
      t = ((1 << i) & *(uint *)(work + 0x428)) == 0;
      hits += t == 0;
    }
    if (hits != n) {
      return;
    }
    msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
  }
  *(int *)(work + 0x428) = 0;
  return;
}

void msnm_try_61_frame(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  char *work;
  int *entry;

  work = (char *)msnm_work_b4_get();
  entry = *(int **)(work + 0x448);
  switch (*entry) {
  case 0:
    msnm_try_61_kind0_step(work,(char *)entry,rob,opp,try_rec,progress);
    break;
  case 1:
    msnm_try_61_kind1_step(work,(char *)entry,rob,opp,try_rec,progress);
    break;
  case 2:
    msnm_try_61_kind2_step(work,(char *)entry,rob,opp,try_rec,progress);
    break;
  }
}

void msnm_try_61_kind0_step(char *work, char *entry, rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  int i;
  uint move;

  switch (*(int *)(progress + 0x18)) {
  case 0:
    for (i = 0; i < 4; i++) {
      move = *(ushort *)(i * 2 + entry + 4);
      if (msnm_move_matches((uint)rob->move_no,move,
                       *(char **)(work + 0x1c)) != 0) {
        *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
        break;
      }
    }
    if (i == 4) break;
  case 1:
    if (msnm_rob_hit_record_check(rob) != 0) {
      if (opp->life == 0) {
        msnm_try_count_up((int)rob->side,try_rec,(int *)progress);
      }
      *(int *)(progress + 0x18) = 2;
    }
    for (i = 0; i < 4; i++) {
      move = *(ushort *)(i * 2 + entry + 4);
      if (msnm_move_matches((uint)rob->move_no,move,
                       *(char **)(work + 0x1c)) != 0) break;
    }
    if (i == 4) {
      *(int *)(progress + 0x18) = 2;
    }
    if (rob->frame >= rob->active_end) {
      *(int *)(progress + 0x18) = 2;
    }
    break;
  case 2:
    break;
  }
}

void msnm_try_61_kind1_step(char *work, char *entry, rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint move;

  switch (*(int *)(progress + 0x18)) {
  case 0:
    move = *(ushort *)(entry + 4);
    if (msnm_move_matches(rob->move_no,move,*(char **)(work + 0x1c)) == 0) {
      return;
    }
    *(int *)(progress + 0x20) = 0;
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    if (msnm_rob_hit_record_check(rob) != 0 && opp->life == 0) {
      msnm_try_count_up(rob->side,try_rec,(int *)progress);
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      break;
    }
    move = *(ushort *)(entry + *(int *)(progress + 0x20) * 2 + 4);
    if (msnm_move_matches(rob->move_no,move,*(char **)(work + 0x1c)) != 0) {
      return;
    }
    *(int *)(progress + 0x20) = *(int *)(progress + 0x20) + 1;
    if (*(int *)(progress + 0x20) >= 4) {
      *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
      return;
    }
    move = *(ushort *)(entry + *(int *)(progress + 0x20) * 2 + 4);
    if (msnm_move_matches(rob->move_no,move,*(char **)(work + 0x1c)) != 0) {
      return;
    }
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 2:
  default:
    break;
  }
}

void msnm_try_61_kind2_step(char *work, char *entry, rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  ushort a = *(ushort *)(entry + 4);
  ushort b = *(ushort *)(entry + 6);
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if ((game_flags & 8) != 0) return;
    if (msnm_move_matches(rob->move_no, a, *(char **)(work + 0x1c)) == 0) return;
    *(int *)(progress + 0x18) += 1;
  case 1:
    if (msnm_rob_hit_record_check(rob) == 0) goto chk;
    *(int *)(progress + 0x18) += 1;
  case 2:
    if ((rob->state & 0x100000) != 0) {
      *(int *)(progress + 0x18) += 1;
      return;
    }
  chk:
    if (rob->frame > rob->recovery_end) {
      *(int *)(progress + 0x18) = 4;
    }
    return;
  case 3:
    if ((rob->state & 0x100000) != 0) {
      if (rob->move_no != b) return;
      if (opp->life == 0 || (game_flags & 8) != 0) {
        msnm_try_count_up(rob->side, try_rec, (int *)progress);
      }
    }
    *(int *)(progress + 0x18) = 4;
  }
}

uint msnm_try_flags_get(int *try_rec)
{
  uchar **masks;
  uint flags;
  
  if ((msnm_work->active == '\0') || msnm_mission_present() == 0) {
    flags = 0;
  }
  else {
    masks = msnm_entry_masks_get(*try_rec);
    flags = (uint)masks[2];
  }
  return flags;
}

void msnm_try_match_win_count(rob_rec *rob, rob_rec *opp, int try_rec, int progress)
{
  uint flags;
  int wins;
  int target;
  
  flags = msnm_try_flags_get((int *)&msnm_work->cur_try);
  target = *(int *)(try_rec + 4);
  if ((game_flags & 0x400) != 0) {
    if ((*(char *)(progress + 0x10) == '\0') && ((int)(champion_side) == (uint)rob->side)) {
      if (!((flags & 2) == 0)) {
        wins = *(int *)(progress + 8) + 1;
        *(int *)(progress + 8) = wins;
        if (wins == target) {
          msnm_status_set((int)(char)rob->side,2);
        }
      }
      else {
        msnm_try_count_up((int)(char)rob->side,try_rec,(int *)progress);
      }
    }
    else if ((flags & 2) != 0) {
      *(int *)(progress + 8) = 0;
    }
  }
  return;
}

int msnm_rob_move_in_list(rob_rec *rob, int *moves, int count)
{
  int i;
  for (i = 0; i < count; i++) {
    if ((uint)rob->move_no == moves[i]) {
      return 1;
    }
  }
  return 0;
}

int msnm_rob_state_in_list(rob_rec *rob, int *states, int count)
{
  int i;
  
  for (i = 0; i < count; i++) {
    if (((int)rob->action.part.move & 0x7fffU) == states[i]) {
      return 1;
    }
  }
  return 0;
}

void msnm_try_count_up(char side, int try_rec, int *progress)
{
  int count;
  
  count = *progress;
  *progress = count + 1;
  if (count + 1 == *(int *)(try_rec + 4)) {
    msnm_status_set((int)(char)side,2);
  }
  return;
}

void msnm_try_count_clear(int *progress)
{
  *progress = 0;
  return;
}

int msnm_rob_8001_vs_attack_check(rob_rec *rob, rob_rec *opp)
{
  int r;
  
  if (!(((((rob->state & 0x8001) != 0x8001) || (rob->stun < 1)) ||
      (opp->frame < opp->active_start)) ||
     (((opp->move_flags & 0x4000) == 0 ||
      (r = 1, (rob->move_flags & 0x4000) != 0))))) return r;
  r = 0;
  return r;
}

int msnm_rob_hit_record_check(rob_rec *rob)
{
  char c;
  rob_coli_rec *coli;
  
  coli = rob->coli;
  if (coli->field_0x1c[1] == 0) goto no;
  c = coli->field_0x94;
  if (c == 0) goto no;
  if (c == 10) goto no;
  c = rob->attack_level;
  if (c == 8) goto no;
  if (c == 9) goto no;
  if (c == 10) goto no;
  return 1;
no:
  return 0;
}

int msnm_rob_phase_watch(rob_rec *rob, int progress)
{
  rob_rec *p = rob;
  int next;
  int r;

  r = 0;
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if ((p->state & 0x800) == 0) {
      goto out;
    }
    if (((p->flags & 0x200000) == 0) &&
       ((p->flags28 & 0x20000000) == 0)) {
      if ((p->field_0x118 & 1) != 0) goto next;
      r = 1;
    }
    next = 2;
    break;
  case 1:
    if ((p->field_0x118 & 3) != 0) {
      if ((int)p->frame != p->field_0x300[1] + 1) {
        goto out;
      }
      r = 1;
    next:
      next = *(int *)(progress + 0x18) + 1;
    }
    else {
      next = 0;
    }
    break;
  case 2:
    next = p->state & 0x40400;
    if (next != 0) {
      goto out;
    }
    break;
  default:
    goto out;
  }
  *(int *)(progress + 0x18) = next;
out:
  return r;
}

bool msnm_combo_damage_track(rob_rec *rob, rob_rec *opp, int progress, char *work)
{
  bool r;
  
  r = false;
  switch (*(int *)(progress + 0x18)) {
  case 0:
    if (work != 0) {
      *(ushort *)(work + 0x446) = 0;
    }
    if ((rob->flags28 & 0x20000000) == 0) break;
    if (work != 0) {
      *(ushort *)(work + 0x446) = rob->move_no;
    }
    *(int *)(progress + 0x20) = (int)opp->life;
    *(int *)(progress + 0x18) = *(int *)(progress + 0x18) + 1;
    break;
  case 1:
    if ((rob->flags28 & 0x20000000) == 0) {
      if ((opp->flags28 & 0x08000000) == 0) r = true;
      *(int *)(progress + 0x20) = *(int *)(progress + 0x20) - (int)opp->life;
      *(int *)(progress + 0x18) = 0;
    }
    if (opp->life == 0) {
      r = true;
      *(int *)(progress + 0x18) = 0;
    }
    if ((opp->flags & 0x100) != 0) {
      r = true;
      *(int *)(progress + 0x20) = *(int *)(progress + 0x20) - (int)opp->life;
      *(int *)(progress + 0x18) = 0;
    }
    break;
  }
  return r;
}

int msnm_input_c0_release_find(rob_rec *rob)
{
  int i;
  int flag;
  int cnt;
  uint r;
  
  flag = 0;
  cnt = 0;
  for (i = 0xc; i >= 0; i--) {
    r = msnm_input_hist_dir_get((char)rob->side,-i);
    if (flag == 0) {
      if ((r & 0xc0) != 0) {
        flag = 1;
        cnt = 0;
      }
    }
    else {
      cnt++;
      if ((r & 0xc0) != 0) {
        r = 0;
      }
      else {
        if (cnt < 9) return i + 1;
        flag = 0;
        cnt = 0;
      }
    }
  }
  return -1;
}

int msnm_lever_dirs_used_count(rob_rec *rob, uint *used)
{
  int i;
  int j;
  uint dir;
  uint lever;
  uint cur;
  int hold;
  
  i = msnm_input_hist_len(rob);
  hold = 0;
  cur = 0;
  for (; i >= 0; i--) {
    dir = msnm_input_hist_dir_get((int)(char)rob->side,-i) & 7;
    lever = msnm_input_hist_press_get((int)(char)rob->side,-i) & 0x3c0;
    if (lever != 0) {
      cur = lever;
      hold = 3;
    }
    else if (--hold < 0) {
      cur = 0;
      hold = 0;
    }
    for (j = 0; j < 9; j++) {
      if (dir == 3 && (ushort)msnm_lever_dirs[j] == cur) {
        *used |= 1 << j;
        cur = 0;
        hold = 0;
        break;
      }
    }
  }
  return msnm_bits9_count(*used);
}

uint msnm_input_hist_len(rob_rec *rob)
{
  int len;
  
  len = (int)rob->ctrl->history_idx -
          (int)rob->ctrl->mark & 0x3f;
  if (0xc < len) {
    len = 0xc;
  }
  return len;
}

uint msnm_input_hist_dir_get(int side, int back)
{
  uint v;
  
  v = input_log_dir_get((int)(char)side,back);
  return v;
}

uint msnm_input_hist_press_get(int side, int back)
{
  uint v;
  
  v = input_log_press_get((int)(char)side,back);
  return v;
}

uint msnm_input_hist_held_get(int side, int back)
{
  uint v;
  
  v = input_log_held_get((int)(char)side,back);
  return v;
}

void msnm_input_stats_add(msnm_input_stats_rec *track)
{
  int *c = track->tally;
  c[0] = c[0] + 1;
  if (track->release_found != 0) c[1]++;
  if (track->field_0x5 != 0) c[2]++;
  if (1 < track->lever_dirs) c[3]++;
  if (2 < track->lever_dirs) c[4]++;
  if ((*(uint *)&track->release_found & 0xffff) != 0) {
    c[5]++;
    if (1 < track->lever_dirs) c[6]++;
  }
}

int msnm_rob_flags_1000201_check(rob_rec *rob)
{
  int r;
  
  if (!((((rob->state & 0x1000200) == 0) || (rob->stun == 0)) ||
     (r = 1, (rob->state & 1) == 0))) return r;
  r = 0;
  return r;
}
