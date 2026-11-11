/*
 * vf4lib/libenemy/src/libenemy.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

void enemy_task_start(void)
{
  enemy_work = &enemy_work_1p;
  enemy_work_init(&enemy_work_1p);
  enemy_work = &enemy_work_2p;
  enemy_work_init(&enemy_work_2p);
  vp_work_init();
  return;
}

void enemy_task_stop(void)
{
  vp_work_disable();
  vp_mode_flag_clear();
  return;
}

void enemy_state_clear(void)
{
  enemy_work->situation = enemy_work->situation | 0x40000000;
  enemy_work->state = 0;
  enemy_work->step = 0;
  return;
}

void enemy_script_clear(void)
{
  enemy_work->script_flags = 0x80000000;
  enemy_work->output_input = 0;
  enemy_work->entry_index = 0;
  enemy_work->frames_left = 0;
  enemy_work->field_0x24 = 0;
  return;
}

uint enemy_pick_yoroke(uint situation)
{
  if ((enemy_opp_rob->state & 0x1000000) != 0) {
    situation = enemy_state_select(situation,9);
  }
  return situation;
}

uint enemy_pick_rise(uint situation)
{
  if (((enemy_self_rob->state & 0x800) != 0) && ((situation & 8) == 0)) {
    situation = enemy_state_select(situation,8);
  }
  return situation;
}

void enemy_state_sake_nagenuke(void)
{
  if ((int)enemy_work->script_flags < 0) {
    enemy_state_clear();
    enemy_script_start(enemy_script_up_gp,0);
  }
  return;
}

void enemy_state_revise(void)
{
  enemy_script_start(enemy_script_guard_tap,0);
  enemy_state_clear();
  return;
}

void enemy_yoroke_script_start(void)
{
  void **list;
  int count;
  
  list = enemy_data_table_get(7,enemy_chara);
  count = enemy_script_list_count(list);
  enemy_script_start_random(list,count);
  return;
}

int enemy_script_list_count(void **list)
{
  int count;
  
  count = 0;
  if ((int *)*list != enemy_script_stand) {
    do {
      list = list + 1;
      count = (int)(short)((short)count + 1);
    } while ((int *)*list != enemy_script_stand);
  }
  return count;
}

void enemy_script_start(void *script, void *tag)
{
  enemy_work->script_flags = 0x40000000;
  enemy_work->script = script;
  enemy_work->script_start = script;
  enemy_work->output_input = 0;
  enemy_work->entry_index = 0;
  enemy_work->frames_left = 0;
  enemy_work->script_tag = tag;
  enemy_work->field_0x24 = 0;
  return;
}

uchar enemy_rand(void)
{
  uchar r;
  
  r = lcg_next15();
  return r;
}

void enemy_mot_sya_guard(void)
{
  int acted;
  
  acted = enemy_mot_debug_override();
  if (acted == 0) {
    enemy_script_start(enemy_script_down_guard_10,0);
  }
  return;
}

void enemy_mot_stand(void)
{
  int acted;
  
  acted = enemy_mot_debug_override();
  if (acted == 0) {
    enemy_script_start(enemy_script_stand,0);
  }
  return;
}

void enemy_mot_ta_guard(void)
{
  int acted;
  
  acted = enemy_mot_debug_override();
  if (acted == 0) {
    enemy_script_start(enemy_script_guard_10,0);
  }
  return;
}

void enemy_mot_sya_punch(void)
{
  int acted;
  
  acted = enemy_mot_debug_override();
  if (acted == 0) {
    if (enemy_work->step == 0) {
      if (((enemy_work->situation & 0x80000000) == 0) &&
         ((enemy_work->script_flags & 0x80000000) != 0)) {
        enemy_script_start(enemy_script_down_punch,0);
        enemy_work->step = enemy_work->step + 1;
      }
    }
    else if (((enemy_work->script_flags & 0x80000000) != 0) &&
            ((enemy_work->situation & 0x80000000) == 0)) {
      enemy_script_clear();
      enemy_state_clear();
    }
  }
  return;
}

void enemy_mot_counter_s(void)
{
  enemy_counter_run(enemy_counter_s_scripts);
  return;
}

void enemy_mot_counter_m(void)
{
  enemy_counter_run(enemy_counter_m_scripts);
  return;
}

void enemy_mot_counter_b(void)
{
  enemy_counter_run(enemy_counter_b_scripts);
  return;
}

void enemy_think_ai_type_stub_a(void)
{
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0xfb9,"assertion `%s\' failed.\n","0")
  ;
  return;
}

int enemy_dead_stub_4195(void)
{
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x1063,"assertion `%s\' failed.\n","0"
          );
  return 0;
}

int enemy_dead_stub_4223(void)
{
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x107f,"assertion `%s\' failed.\n","0"
          );
  return 0;
}

void enemy_ai_attack_run(void)
{
  enemy_ai_attack_script_start();
  return;
}

void enemy_think_ai_type_stub_b(void)
{
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x1102,"assertion `%s\' failed.\n","0"
          );
  return;
}

void enemy_dead_stub_4479(void)
{
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x117f,"assertion `%s\' failed.\n","0"
          );
  return;
}

uint enemy_pick_gacha(uint situation)
{
  uchar rnd;
  uchar *rate;
  
  if (((0x2000 & situation) != 0) &&
     (rate = &enemy_work->mash_rate, rnd = enemy_rand(), rnd < *rate)) {
    situation = enemy_state_select(situation,7);
  }
  return situation;
}

uint enemy_pick_furimuki(uint situation)
{
  if ((situation & 0x10000000) != 0) {
    situation = enemy_state_select(situation,5);
  }
  return situation;
}

void enemy_followup_script_start(void)
{
  void **list;
  int count;
  
  enemy_followup_opp_land_frames = (int)enemy_opp_rob->frames_to_land;
  enemy_followup_opp_height = enemy_opp_rob->pos.y;
  enemy_followup_opp_dist = enemy_opp_rob->opp_dist;
  list = enemy_data_table_get(6,enemy_chara);
  count = enemy_script_list_count(list);
  enemy_script_start_random(list,count);
  return;
}

uint enemy_pick_sake_nagenuke(uint situation)
{
  if (((ushort)(enemy_self_rob->frame_disadvantage - 6U) < 0x19) &&
     ((int)enemy_self_rob->frame >= enemy_self_rob->recovery_end + -5)) {
    situation = enemy_state_select(situation,0x11);
  }
  return situation;
}

void enemy_script_output_set(uint input, int frames)
{
  enemy_work->output_input = input; enemy_work->frames_left = frames - 1;
}

void ** enemy_data_table_get(uint table_no, int chara)
{
  void **list;
  
  if (10 < table_no) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x1a1,"assertion `%s\' failed.\n",
             "tbl_no < ENEMY_DATA_TBL_MAX");
  }
  if (0x12 < chara) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x1a2,"assertion `%s\' failed.\n",
             "char_num < CN_MAX");
  }
  list = enemy_data_tables[table_no][chara];
  if (list != (void **)0x0) return list;
  return enemy_default_script_list;
}

uint enemy_pick_forward(uint situation)
{
  if ((situation & 0x80) != 0) return situation;
  if (2.1 < enemy_self_rob->opp_dist) {
    situation = enemy_state_select(situation,1);
  }
  return situation;
}

void enemy_state_yoroke(void)
{
  if (enemy_work->step == 0) {
    enemy_work->attack_cooldown = 0;
    if (-1 < (int)enemy_work->script_flags) {
      return;
    }
    enemy_work->step = 1;
    if (1.8 < enemy_self_rob->opp_dist) {
      enemy_script_start((game_frame_count & 1) ? enemy_script_forward_twice : enemy_script_down_forward_twice,0);
      return;
    }
    enemy_yoroke_script_start();
  }
  else {
    if (-1 < (int)enemy_work->script_flags) {
      return;
    }
    enemy_script_clear();
    enemy_state_clear();
  }
}

void enemy_state_gacha(void)
{
  enemy_state_clear();
  if ((game_frame_count & 1) != 0) enemy_script_start(enemy_script_forward_tap,0);
  else enemy_script_start(enemy_script_back_tap,0);
}

uint enemy_state_select(uint situation, int state)
{
  situation = situation & 0xbfffffff | 0x20000000;
  enemy_work->situation = situation;
  enemy_work->state = state;
  enemy_work->step = 0;
  return situation;
}

void enemy_mot_syagami(void)
{
  int acted;
  void *script;
  
  acted = enemy_mot_debug_override();
  if (acted == 0) {
    if ((enemy_self_rob->flags & 0x10) != 0) {
      script = enemy_script_down_11;
    }
    else {
      script = enemy_script_down_guard_10;
    }
    enemy_script_start(script,0);
  }
  return;
}

uint enemy_input_side_swap(uint input)
{
  if (enemy_self_rob->screen_xy[0] > enemy_opp_rob->screen_xy[0] && (input & 0x30)) input ^= 0x30;
  return input;
}

uint enemy_pick_emy_down(uint situation)
{
  if (mode_get_main() == MODE_MAIN_GAME && enemy_round <= '\x04') return situation;
  if ((enemy_opp_rob->state & 0x40000) != 0 &&
      enemy_opp_rob->frame > enemy_opp_rob->active_end) return situation;
  if ((enemy_opp_rob->state & 0x40800) != 0 &&
      (enemy_opp_rob->move_flags & 0x4000) == 0) {
    enemy_script_clear();
    situation = enemy_state_select(situation,0x10);
  }
  return situation;
}

void enemy_sidestep_pkg_start(int up)
{
  void *script;
  if (up == 0) {
    if (game_frame_count & 1) script = enemy_script_down_tap_gpk_12; else script = enemy_script_down_tap_gpk_16;
  }
  else {
    if (game_frame_count & 1) script = enemy_script_up_tap_gpk_12; else script = enemy_script_up_tap_gpk_16;
  }
  enemy_script_start(script,0);
}

void enemy_state_ai_attack(void)
{
  enemy_work_rec *work = enemy_work;
  if (work->step == 0) {
    if ((int)work->script_flags < 0) enemy_ai_attack_run();
  }
  else {
    if ((int)enemy_work->script_flags < 0) {
      enemy_script_clear();
      enemy_state_clear();
    }
  }
}

void enemy_situation_ai_type_stub(void)
{
  char buf[240];
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x465,"assertion `%s\' failed.\n","0");
}

uint enemy_pick_ukemi(uint situation)
{
  uint rnd;
  uchar *rate;
  
  if ((situation & 0x100) == 0) return situation;
  if (enemy_self_rob->move_no == 0x136) return situation;
  rate = &enemy_work->recovery_rate;
  rnd = enemy_rand();
  if (*rate > rnd) {
    situation = enemy_state_select(situation,0xb);
  }
  return situation;
}

void enemy_counter_run(void **scripts)
{
  rob_rec *opp;
  
  if ((enemy_mot_debug_override() == 0) && ((enemy_work->script_flags & 0x20000000) == 0)) {
    enemy_script_clear();
    enemy_state_clear();
    if (enemy_self_rob->opp_dist <= 1.8) {
      opp = enemy_opp_rob;
      if ((opp->move_flags & 0x4000) != 0) {
        if ((opp->frame < opp->active_start) &&
            ((enemy_work->situation & 0x80000000) == 0) &&
            ((enemy_work->script_flags & 0x80000000) != 0) &&
            (opp->frame > opp->active_start - 8)) {
          enemy_script_start(scripts[enemy_chara],0);
        }
      }
    }
  }
  return;
}

void enemy_script_entry_get(uint index, uint *pair)
{
  uint i;
  uint *p;
  uint input;
  int frames;
  
  p = enemy_work->script;
  if (index > 0x19) {
    enemy_work->script_flags = enemy_work->script_flags | 0x10000000;
    return;
  }
  for (i = 0; i < index; i++) {
    p = p + 2;
  }
  input = p[0];
  frames = p[1];
  if (frames == 0) {
    enemy_work->script_flags = enemy_work->script_flags | 0x10000000;
    return;
  }
  pair[0] = input;
  pair[1] = frames;
  return;
}

void enemy_repeated_throws_collect(void)
{
  int k;
  int f;
  int j;
  int i;
  ushort counts [16];
  ushort repeated [16];
  ushort found [16];
  
  for (i = 0; i < 0x10; i++) {
    repeated[i] = 0;
    counts[i] = 0;
    found[i] = 0;
  }
  for (i = 0; i < 0x10; i++) {
    if (enemy_work->throws[i] != 0) {
      for (j = i; j < 0x10; j++) {
        if ((i != j) &&
           (enemy_work->throws[i] ==
            enemy_work->throws[j])) {
          repeated[i] = enemy_work->throws[i];
          counts[i] = counts[i] + 1;
        }
      }
    }
  }
  for (i = 0; i < 0x10; i++) {
    if (1 < counts[i]) {
      found[i] = repeated[i];
    }
  }
  for (i = 0; i < 0x10; i++) {
    f = 0;
    for (j = 0; j < 8; j++) {
      if (found[i] != 0) {
        if (found[i] != enemy_work->repeated_throws[j]) {
          f = 1;
        }
        else {
          f = 0;
          break;
        }
      }
    }
    if (f) {
      for (k = 7; k >= 0; k--) {
        if (k != 7) {
          enemy_work->repeated_throws[k + 1] =
               enemy_work->repeated_throws[k];
        }
      }
      enemy_work->repeated_throws[0] = found[i];
    }
  }
}

uint enemy_pick_yoke_or_attack(uint situation)
{
  rob_rec *opp;
  int frame, start;
  if ((situation & 0x4000000) != 0) {
    opp = enemy_opp_rob;
    frame = opp->frame;
    start = opp->active_start;
    if (frame > start - 0xc) {
      if (frame < start) situation = enemy_state_select(situation,6);
    }
    else {
      situation = enemy_state_select(situation,2);
    }
  }
  return situation;
}

uint enemy_pick_nage_or_attack(uint situation)
{
  short *cooldown = &enemy_work->throw_cooldown;
  short v = *cooldown;

  if (v != 0) {
    *cooldown = v - 1;
  }
  else {
    if ((situation & 0x10000000) != 0) return situation;
    if ((situation & 0x100000) != 0) return situation;
    if (hit_wait_timer_get() != 0) return situation;
    if ((enemy_self_rob->state & 0x40000) != 0) return situation;
    if ((enemy_opp_rob->state & 0x40000) != 0) return situation;
    if ((enemy_opp_rob->move_flags & 0x40000) != 0) return situation;
    if (((situation & 0x8000) == 0) && ((game_frame_count & 1U) != 0)) return situation;
    if (enemy_self_rob->opp_dist > 1.5) return situation;
    if ((uint)enemy_rand() > enemy_work->throw_rate) {
      enemy_work->throw_cooldown = (uint)(int)enemy_work->throw_interval >> 2;
    }
    else if ((situation & 0x4000000) != 0) {
      return enemy_state_select(situation,2);
    }
    else {
      enemy_work->throw_cooldown = enemy_work->throw_interval;
      situation = enemy_state_select(situation,3);
    }
  }
  return situation;
}

void enemy_state_nage(void)
{
  void **list;
  int count;
  int table_no;
  
  if (enemy_work->step != 0) {
    if (((enemy_work->situation & 0x2000000) != 0) &&
       (enemy_opp_rob->opp_dist <= 1.5)) {
      enemy_script_clear();
      enemy_state_clear();
      list = enemy_data_table_get(8,enemy_chara);
      count = enemy_script_list_count(list);
      enemy_script_start_random(list,count);
      return;
    }
    if ((enemy_work->script_flags & 0x20000000) != 0) {
      return;
    }
    enemy_script_clear();
    enemy_state_clear();
    return;
  }
  if ((enemy_opp_rob->state & 0xc000) == 0) {
    if ((int)enemy_opp_rob->frame < enemy_opp_rob->recovery_end + -5) {
      return;
    }
    if (((int)enemy_opp_rob->frame <= (int)enemy_opp_rob->active_end) &&
       ((enemy_opp_rob->move_flags & 0x4000) != 0)) {
      return;
    }
  }
  if ((enemy_opp_rob->relation_flags & 1) != 0) {
    if ((enemy_opp_rob->flags & 0x10) != 0) table_no = 8;
    else table_no = 3;
  }
  else if ((enemy_opp_rob->flags & 0x10) != 0) {
    if ((enemy_self_rob->frame_disadvantage <= -14) ||
        ((hit_wait_timer_get() < 0xd && ((enemy_work->situation & 0x20000) == 0)))) {
      table_no = 4;
    }
    else {
      table_no = 9;
    }
  }
  else table_no = 3;
  list = enemy_data_table_get(table_no,enemy_chara);
  count = enemy_script_list_count(list);
  enemy_script_start_random(list,count);
  enemy_work->step = enemy_work->step + 1;
  enemy_work->throw_cooldown = enemy_work->throw_interval;
}

uint enemy_pick_counter(uint situation)
{
  char level;
  
  level = enemy_opp_rob->attack_level;
  if (level == '\b' || level == '\v' || level == '\t') return situation;
  if (((enemy_opp_rob->field_0x30 & 4) != 0) && ((enemy_opp_rob->flags24 & 0x8000) != 0)) {
    enemy_state_clear();
  }
  else if ((situation & 0x20000) != 0) {
    situation = enemy_state_select(situation,0xf);
  }
  return situation;
}

uint enemy_pick_revise(uint situation)
{
  int angle;
  int a;
  int b;
  
  if ((situation & 0x2000080) != 0) return situation;
  if ((enemy_self_rob->state & 0x40000) != 0) return situation;
  angle = (short)(enemy_self_rob->facing - enemy_self_rob->opp_dir);
  if ((ushort)(angle + 0x1553) > 0x2aa6) {
    a = angle < 0x6aa4;
    b = angle > -0x6aa4;
    if (a & b) {
      situation = enemy_state_select(situation,0xe);
    }
  }
  return situation;
}

void enemy_state_choose(void)
{
  uint situation;
  situation = enemy_work->situation;
  if (enemy_hit_interrupt_check() != 0) return;
  if ((int)situation < 0) {
    if (situation & 0x40000000) situation = enemy_pick_ukemi(situation);
    if (situation & 0x40000000) situation = enemy_pick_sake_nagenuke(situation);
    if (situation & 0x40000000) situation = enemy_pick_kaeshi(situation);
    if (enemy_round > 1 && (situation & 0x40000000)) situation = enemy_pick_hame(situation);
    if (situation & 0x40000000) situation = enemy_pick_rise(situation);
    situation = enemy_pick_nagekaeshi(situation);
    if (situation & 0x40000000) situation = enemy_pick_guard(situation);
    if (situation & 0x40000000) situation = enemy_pick_gacha(situation);
  }
  else {
    if (situation & 0x40000000) situation = enemy_pick_ukemi(situation);
    if (situation & 0x40000000) situation = enemy_pick_furimuki(situation);
    if (situation & 0x40000000) situation = enemy_pick_forward(situation);
    if (situation & 0x40000000) situation = enemy_pick_sake_nagenuke(situation);
    if (situation & 0x40000000) situation = enemy_pick_kaeshi(situation);
    if (situation & 0x40000000) situation = enemy_pick_rise(situation);
    if (enemy_round > 1 && (situation & 0x40000000)) situation = enemy_pick_hame(situation);
    if (enemy_round < 2) {
      if (enemy_round == 0) {
        if (round_time > 300) {
          if (situation & 0x40000000) situation = enemy_pick_counter(situation);
        }
      }
      else if (enemy_round == 1) {
        if ((int)(match_round_time - round_time) < 300) {
          if (situation & 0x40000000) situation = enemy_pick_counter(situation);
        }
      }
    }
    if (situation & 0x40000000) situation = enemy_pick_yoke_or_attack(situation);
    if (situation & 0x40000000) situation = enemy_pick_emy_down(situation);
    if (situation & 0x40000000) situation = enemy_pick_yoke(situation);
    if (situation & 0x40000000) situation = enemy_pick_nagekaeshi(situation);
    if (situation & 0x40000000) situation = enemy_pick_guard(situation);
    if (situation & 0x40000000) situation = enemy_pick_nage_or_attack(situation);
    if (situation & 0x40000000) situation = enemy_pick_yoroke(situation);
    if (situation & 0x40000000) situation = enemy_pick_attack(situation);
    if (situation & 0x40000000) situation = enemy_pick_revise(situation);
  }
  enemy_work->situation = situation;
}

uint enemy_pick_kaeshi(uint situation)
{
  uint rate;
  int frame;
  int frames_to_active;
  
  if ((situation & 0x4d) != 0) {
    return situation;
  }
  {
    if (enemy_chara == 0 || enemy_chara == 5 || enemy_chara == 9 || enemy_chara == 7 ||
        enemy_chara == 10 || enemy_chara == 0xc) {
      if ((situation & 0x20000) != 0) {
        if ((enemy_opp_rob->attack_mask & 0x180) == 0) {
          rate = enemy_work->reversal_rate >> 2;
          if (rate > enemy_rand()) {
            if (enemy_self_rob->opp_dist < 1.7) {
              frame = enemy_opp_rob->frame;
              if (frame < enemy_opp_rob->active_start) {
                frames_to_active = enemy_opp_rob->active_start - frame;
                if (frames_to_active < 0xd) {
                  if (frame != 1) {
                    situation = enemy_state_select(situation,10);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return situation;
}

void enemy_state_furimuki(void)
{
  uint situation;
  uint rnd;
  void **list;
  int count;
  
  situation = enemy_work->situation;
  if ((enemy_self_rob->opp_dist < 2.1) &&
      (((situation & 0x20000) == 0 ||
       ((int)enemy_opp_rob->active_end - (int)enemy_opp_rob->frame >= 0x10))) &&
      (rnd = enemy_rand(), enemy_work->attack_rate > rnd)) {
    list = enemy_data_table_get(5,enemy_chara);
    count = enemy_script_list_count(list);
    enemy_script_start_random(list,count);
    enemy_work->state = 2;
    enemy_work->step = 1;
  }
  else {
    enemy_script_start(enemy_script_forward_tap,0);
    enemy_state_clear();
  }
}

void enemy_records_clear(void)
{
  int i;
  
  for (i = 0; i < 10; i++) {
    enemy_records[i].field_0x14 = 0;
    enemy_records[i].field_0x16 = 0;
    enemy_records[i].field_0x18 = 0;
    enemy_records[i].field_0x1a = 0;
    enemy_records[i].field_0x1b = 0;
  }
}

uint enemy_opponent_pad_idle(void)
{
  player_input_rec *input;
  uint buttons;
  uint r;
  input = player_input_get((uint)enemy_opp_rob->side);
  buttons = input->pressed;
  r = 1;
  if (buttons & 1) r = 0;
  if (buttons & 2) r = 0;
  if (buttons & 4) r = 0;
  return r;
}

void enemy_state_ukemi(void)
{
  if ((enemy_work->situation & 0x100) == 0) {
    enemy_state_clear();
    enemy_script_clear();
  }
  else {
    enemy_script_start(enemy_ukemi_scripts[game_frame_count % 3],0);
  }
}

void enemy_timers_step(void)
{
  short t;
  uint situation;
  
  situation = enemy_work->situation;
  if ((int)situation < 0) {
    if (enemy_self_rob->stun != 0) {
      t = enemy_self_rob->stun;
    }
    else {
      t = enemy_self_rob->recovery_end - enemy_self_rob->frame;
    }
    enemy_work->recover_frames = t;
  }
  if ((situation & 1) != 0) {
    enemy_work->land_frames = enemy_self_rob->frames_to_land;
  }
  else {
    enemy_work->land_frames = 0;
  }
  t = enemy_work->start_wait;
  if (t != 0) {
    enemy_work->start_wait = t = t - 1;
    if (t == 0) {
      enemy_state_clear();
    }
    else {
      enemy_work->situation = enemy_work->situation & 0x9fffffff;
    }
  }
  t = enemy_work->timer_0x30;
  if (t != 0) {
    enemy_work->timer_0x30 = t + -1;
  }
  t = enemy_work->guard_cooldown;
  if (t != 0) {
    enemy_work->guard_cooldown = t + -1;
  }
  else {
    situation = situation | 0x10000;
  }
  t = enemy_work->timer_0x28;
  if (t != 0) {
    enemy_work->timer_0x28 = t + -1;
  }
  enemy_work->situation = situation;
  return;
}

uint enemy_pick_guard(uint situation)
{
  float dist;
  uint rnd;
  dist = enemy_self_rob->opp_dist;
  if ((situation & 0x10000) == 0) return situation;
  if ((situation & 0x20000) == 0) return situation;
  if ((situation & 0x4000000) != 0) return situation;
  if ((situation & 0x80000) != 0) return situation;
  if (dist > 3.0) return situation;
  rnd = enemy_rand();
  if (enemy_work->guard_rate > rnd) {
    situation = enemy_state_select(situation,4);
  }
  else {
    situation = situation & 0xfffeffff;
    enemy_work->situation = situation;
    enemy_work->guard_cooldown = enemy_work->guard_interval;
  }
  return situation;
}

uint enemy_pick_yoke(uint situation)
{
  if (enemy_self_rob->opp_dist > 2.6) return situation;
  if (--enemy_work->sidestep_count != 0xff) return situation;
  if ((enemy_work->situation & 0x20000) != 0) {
    if (enemy_rand() < enemy_work->sidestep_rate) {
      enemy_work->sidestep_count = enemy_work->base_sidestep_count;
      situation = enemy_state_select(situation,6);
    }
  }
  else {
    if (!(((2.0 < enemy_self_rob->bound_dist[2]) && (2.0 < enemy_self_rob->bound_dist[3])) &&
       (2.0 < enemy_self_rob->bound_dist[1]))) {
      if ((enemy_opp_rob->state & 0x40c00) != 0) {
        if (enemy_rand() < enemy_work->sidestep_rate) {
          situation = enemy_state_select(situation,6);
        }
      }
    }
  }
  return situation;
}

void enemy_state_forward(void)
{
  void *script;
  
  if (enemy_work->step != 0) {
    if ((enemy_self_rob->opp_dist <= 2.1) || ((int)enemy_work->script_flags < 0)) {
      enemy_script_clear();
      enemy_state_clear();
    }
  }
  else {
    if (enemy_chara == 0xb && (enemy_self_rob->flags24 & 0x400) != 0)
      script = enemy_script_guard_tap;
    else
      script = enemy_script_forward_twice_hold;
    enemy_script_start(script,0);
    enemy_work->step = 1;
  }
  return;
}

int enemy_hit_interrupt_check(void)
{
  if ((enemy_opp_rob->coli->field_0x1c[1] != 0) &&
     ((enemy_opp_rob->coli->field_0x94 & 0x23) != 0)) {
    if (enemy_self_rob->opp_dist > 2.1) return 0;
    enemy_work->attack_cooldown = 0;
    enemy_work->throw_cooldown = 0;
    enemy_work->guard_cooldown = 0;
    enemy_state_clear();
    enemy_script_clear();
    return 1;
  }
  if ((enemy_work->situation & 0x800) != 0) {
    enemy_work->attack_cooldown = 0;
    enemy_work->throw_cooldown = 0;
    enemy_work->guard_cooldown = 0;
    enemy_state_clear();
    enemy_script_clear();
  }
  return 0;
}

int enemy_mot_debug_override(void)
{
  int acted;
  void *script;
  
  acted = 0;
  if ((enemy_self_rob->state & 0x40) != 0) {
    return 1;
  }
  if ((ukemi_on_off != OFF_ON_OFF) && ((enemy_work->situation & 0x100) != 0)) {
    enemy_script_start(enemy_ukemi_scripts[game_frame_count % 3],0);
    acted = 1;
  }
  if ((gacha_on_off != OFF_ON_OFF) &&
     (((enemy_work->situation & 0x2000) != 0 ||
      ((enemy_self_rob->state & 0x80000) != 0)))) {
    if ((game_frame_count & 1U) != 0) {
      script = enemy_script_forward_tap;
    }
    else {
      script = enemy_script_back_tap;
    }
    enemy_script_start(script,0);
    acted = 1;
  }
  return acted;
}

void enemy_state_emy_down(void)
{
  int pick;
  void *script;

  if ((int)enemy_work->script_flags < 0) {
    if (enemy_self_chara_down_set() != 0 && enemy_opp_rob->stun >= 0x14) {
      if (enemy_self_rob->chara != '\n' && (game_frame_count & 1U) != 0)
        script = enemy_script_down_gp;
      else
        script = enemy_script_down_forward_gp;
    }
    else {
      pick = (int)lcg_next15() % 5;
      if (pick == 0)
        script = enemy_script_down_back_twice;
      else if (pick == 1)
        script = enemy_script_back_twice;
      else
        script = enemy_script_down_forward_twice;
    }
    enemy_script_start(script,0);
    if ((enemy_opp_rob->state & 0x800) == 0) {
      enemy_work->step = 1;
    }
    if ((enemy_work->step == 1) &&
       (((enemy_opp_rob->state & 0x40000) == 0 ||
        (enemy_opp_rob->frame >= enemy_opp_rob->active_end)))) {
      enemy_state_clear();
      enemy_script_clear();
    }
    if ((enemy_opp_rob->move_flags & 0x4000) != 0) {
      enemy_state_clear();
      enemy_script_clear();
    }
  }
}

void enemy_attack_mode_shuffle(uint situation)
{
  enemy_work_rec *work;

  enemy_shuffle_frames = enemy_shuffle_frames + 1;
  if (((situation & 0x200004) != 0) || ((int)enemy_shuffle_frames % 180 == 0)) {
    work = enemy_work;
    work->attack_mode = (int)lcg_next15() % 3;
  }
  if (enemy_work->attack_mode == 0) {
    enemy_work->attack_rate = enemy_difficulty_rates[user_back_data.rank_difficult];
    enemy_work->attack_cooldown = 0;
  }
  return;
}

uint enemy_pick_attack(uint situation)
{
  uint result = situation;
  float dist = enemy_self_rob->opp_dist;
  uint opp_state = enemy_opp_rob->state;
  short *cooldown = &enemy_work->attack_cooldown;
  short left = *cooldown;
  rob_rec *opp;
  if (enemy_ai_type == 0) {
    if (left != 0) {
      *cooldown = left - 1;
      return result;
    }
  }
  if ((opp_state & 0x40800) != 0) return result;
  if (dist > 2.1) return result;
  if ((enemy_self_rob->state & 0x40000) != 0) return result;
  if (enemy_ai_type != 0) {
    return enemy_state_select(result,0x12);
  }
  if (enemy_rand() < enemy_work->attack_rate) {
    opp = enemy_opp_rob;
    if ((opp->state & 0xc0) != 0 && opp->frame <= opp->active_end &&
        (opp->move_flags & 0x4000) != 0) {
      return result;
    }
    result = enemy_state_select(result,2);
  }
  else {
    cooldown = &enemy_work->attack_cooldown;
    *cooldown = (short)((uint)(int)enemy_work->attack_interval >> 2);
  }
  return result;
}

bool enemy_self_chara_down_set(void)
{
  int r = 0;
  if (enemy_self_rob->chara == 9) r = 1;
  if (enemy_self_rob->chara == 10) r = 1;
  if (enemy_self_rob->chara == 4) r = 1;
  if (enemy_self_rob->chara == 14) r = 1;
  return r;
}

void enemy_ai_attack_script_start(void)
{
  int i;
  int n = 0x5c;
  
  if ((enemy_ai_attack_wait != 0 || enemy_ai_attack_count < 1) && enemy_ai_attack_move != 0 &&
      ((ushort)enemy_ai_attack_frames >= (ushort)enemy_ai_attack_wait || enemy_ai_attack_count == 0)) {
    for (i = 0; i < n; i++) {
      ushort *k = &enemy_ai_attack_move;
      enemy_ai_attack_rec *e = enemy_ai_attack_table;
      if (*k == e[i].move) {
        enemy_script_start(e[i].script,0);
        enemy_work->step = enemy_work->step + 1;
        enemy_work->attack_cooldown = 0;
        enemy_ai_attack_count++;
        enemy_ai_attack_frames = 1;
        if (enemy_ai_attack_count > enemy_opp_rob->combo[0].hits + 1)
          enemy_ai_attack_count = enemy_opp_rob->combo[0].hits + 1;
        return;
      }
    }
  }
}

void enemy_situation_update(void)
{
  uint situation;
  uint self_state;
  uint opp_state;
  uint self_flags;
  uint v;

  situation = enemy_work->situation & 0x60010200;
  enemy_work->situation_prev = enemy_work->situation;
  self_state = enemy_self_rob->state;
  opp_state = enemy_opp_rob->state;
  self_flags = enemy_self_rob->flags;
  if ((self_flags & 8) != 0) {
    situation |= 1;
  }
  if ((self_flags & 0x80) != 0) {
    situation |= 2;
  }
  if ((self_state & 0xc00) != 0) {
    situation |= 4;
  }
  if ((self_state & 0x40000) != 0) {
    situation |= 8;
  }
  if ((self_state & 0x20) != 0) {
    situation |= 0x10;
  }
  if ((self_state & 0x4000) != 0) {
    situation |= 0x40;
  }
  if ((self_state & 1) == 0) {
    situation |= 0x1000;
  }
  if ((self_state & 0x1000000) != 0) {
    situation |= 0x2000;
  }
  if ((self_state & 0x80000) != 0) {
    situation |= 0x2000;
  }
  if ((enemy_self_rob->field_0x118 & 0x25) == 0x25) {
    situation |= 0x100;
  }
  else {
    situation &= ~0x100;
  }
  if ((enemy_self_rob->reach_flags & 0x107) != 0) {
    if ((enemy_opp_rob->move_flags & 0x4040) == 0) {
      situation |= 0x8000;
    }
    if ((enemy_opp_rob->move_flags & 0x4000) != 0 &&
        enemy_opp_rob->frame > enemy_opp_rob->active_end &&
        hit_wait_timer_get() < 8 &&
        enemy_self_rob->frame_disadvantage < -7 &&
        (enemy_opp_rob->move_flags & 0x40) == 0) {
      situation |= 0x8000;
    }
  }
  if (enemy_opp_rob->move_no != enemy_opp_rob->old_cur->move_no) {
    situation |= 0x10000;
    situation &= ~0x200;
  }
  if ((enemy_opp_rob->move_flags & 0x4000) != 0) {
    if ((enemy_opp_rob->state & 0xc000) != 0) situation |= 0x1000000;
    else if (enemy_opp_rob->frame > enemy_opp_rob->active_end + 1) situation |= 0x1000000;
    else situation |= 0x20000;
  }
  if ((enemy_opp_rob->flags24 & 0x100) != 0) {
    situation |= 0x4000000;
  }
  if ((opp_state & 0x100000) != 0) {
    situation |= 0x40000;
  }
  if ((opp_state & 0x100000) != 0 && enemy_opp_rob->frame <= 10 &&
      (enemy_self_rob->state & 0x200000) != 0) {
    situation |= 0x800;
  }
  if ((enemy_opp_rob->flags & 8) != 0) {
    situation |= 0x100000;
  }
  if ((opp_state & 0xc00) != 0) {
    situation |= 0x200000;
  }
  if ((opp_state & 0x20) != 0) {
    situation |= 0x400000;
  }
  if ((enemy_opp_rob->relation_flags & 1) != 0) {
    situation |= 0x2000000;
  }
  if ((enemy_self_rob->relation_flags & 1) != 0) {
    situation |= 0x10000000;
  }
  if ((opp_state & 0x8000) != 0) {
    situation |= 0x8000000;
  }
  if ((enemy_opp_rob->flags28 & 0x40000000) != 0 &&
      (enemy_opp_rob->move_flags & 0x104000) == 0 &&
      (enemy_opp_rob->frame <= enemy_opp_rob->active_start - 2 ||
       enemy_opp_rob->frame <= enemy_opp_rob->active_end - 2)) {
    situation |= 0x80000;
  }
  if (enemy_ai_type != 0) {
    enemy_situation_ai_type_stub();
  }
  else {
    if ((self_state & 1) != 0 && (situation & 0x80) == 0 &&
        enemy_self_rob->frame < enemy_self_rob->recovery_end - 5) {
      situation |= 0x80000000;
    }
  }
  enemy_work->situation = situation;
}

void enemy_rates_update(void)
{
  ushort m = 0xff;
  uint flags;
  ushort a;
  ushort b;

  enemy_work->attack_rate = enemy_work->base_attack_rate;
  enemy_work->throw_rate = enemy_work->base_throw_rate;
  enemy_work->guard_rate = enemy_work->base_guard_rate;
  enemy_work->sidestep_rate = enemy_work->base_sidestep_rate;
  enemy_work->reversal_rate = enemy_work->base_reversal_rate;
  enemy_work->sidestep_count = enemy_work->base_sidestep_count;
  enemy_work->mash_rate = enemy_work->base_mash_rate;
  enemy_work->recovery_rate = enemy_work->base_recovery_rate;
  flags = enemy_work->situation;
  if (enemy_round > 1) {
    enemy_attack_mode_shuffle(flags);
  }
  if (((flags & 0x80000000) == 0) && ((enemy_work->script_flags & 0x20000000) == 0)) {
    enemy_work->script_flags = enemy_work->script_flags | 0x80000000;
  }
  a = enemy_work->attack_rate;
  b = enemy_work->throw_rate;
  if (((flags & 0x4000000) != 0) && ((char)rank_round > 1)) {
    a = a + 0x30;
  }
  if ((flags & 0x1000000) != 0) {
    a = a + 0x20;
  }
  if ((flags & 0x800090) != 0) {
    a = a + 0x10;
  }
  if (enemy_rates_maxed != 0) {
    a = m;
  }
  if ((enemy_self_rob->life < enemy_opp_rob->life) &&
     (100 < enemy_opp_rob->life)) {
    a = a + 0x20;
    b = b + 0x20;
  }
  enemy_work->attack_rate = a < m ? a : m;
  enemy_work->throw_rate = b < m ? b : m;
  if (((flags & 0x4000000) != 0) && ((char)rank_round > 1)) {
    enemy_work->attack_cooldown = 0;
  }
  a = enemy_work->guard_rate;
  if (enemy_self_rob->combo[0].hits != 0) {
    a = enemy_self_rob->combo[0].hits * enemy_work->level * 8 + a;
  }
  if ((enemy_self_rob->life < enemy_opp_rob->life) &&
     (100 < enemy_opp_rob->life)) {
    a = a + 0x20;
  }
  enemy_work->guard_rate = a < m ? a : m;
}

void enemy_state_no_act(void)
{
  rob_rec *self;
  rob_rec *opp;
  uchar chara;
  int frame;

  self = enemy_self_rob;
  chara = self->chara;
  if (((chara == 6) | (chara == 12)) != 0) {
    opp = enemy_opp_rob;
    if ((opp->move_flags & 0x4000) != 0) {
      if (!(self->opp_dist > 1.8)) {
        frame = opp->frame;
        if (frame < opp->active_start - 4 || frame > opp->active_end) {
        }
        else if (enemy_round < 5) {
          if (game_frame_count % 10 != 0) {
            enemy_script_start(enemy_script_forward_10,0);
          }
        }
        else {
          switch (game_frame_count % 7) {
          case 0:
          case 1:
            enemy_script_start(enemy_script_forward_10,0);
            break;
          case 2:
          case 3:
            enemy_script_start(enemy_script_guard_7,0);
            break;
          default:
            enemy_script_start(enemy_script_wait_7,0);
            break;
          }
        }
      }
    }
  }
  enemy_state_clear();
  return;
}

void enemy_sidestep_pick(int up)
{
  if (up == 0) {
    switch (game_frame_count % 3) {
    case 0: enemy_sidestep_pkg_start(0); break;
    case 1: enemy_script_start(enemy_script_down_tap,0); break;
    case 2: enemy_script_start(enemy_script_down_tap_gp,0); break;
    }
  }
  else {
    switch (game_frame_count % 3) {
    case 0: enemy_sidestep_pkg_start(1); break;
    case 1: enemy_script_start(enemy_script_up_tap,0); break;
    case 2: enemy_script_start(enemy_script_up_tap_gp,0); break;
    }
  }
}

void enemy_task_frame(void)
{
  rob_rec *rob_1p;
  rob_rec *rob_2p;
  
  rob_1p = rob_get(0);
  rob_2p = rob_get(1);
  if (enemy_ai_type != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/libenemy.c",0x208,"assertion `%s\' failed.\n",
             "ai_type == 0");
  }
  if (cpu_mot_type == CPU_MOT_TYPE_OFF) {
    if (vp_both_enabled() != 0) {
      vp_frame();
      return;
    }
    if (rob_1p->chara != 13 && rob_2p->chara != 13) {
      return;
    }
  }
  enemy_self_rob = rob_1p;
  enemy_opp_rob = rob_2p;
  if (mode_get_main() == 1) enemy_round = 9;
  else enemy_round = rank_round;
  switch (cpu_on_off) {
  case BOTH_OFF:
    enemy_self_rob->flags |= 4;
    enemy_opp_rob->flags |= 4;
    break;
  case _1P_ON:
    enemy_self_rob->flags &= ~4;
    enemy_opp_rob->flags |= 4;
    break;
  case _2P_ON:
    enemy_self_rob->flags |= 4;
    enemy_opp_rob->flags &= ~4;
    break;
  case BOTH_ON:
    enemy_self_rob->flags &= ~4;
    enemy_opp_rob->flags &= ~4;
    break;
  }
  if ((enemy_self_rob->flags & 4) != 0 && (enemy_opp_rob->flags & 4) != 0) {
    return;
  }
  if ((enemy_self_rob->flags & 4) == 0 && (enemy_opp_rob->flags & 4) != 0) {
    enemy_self_rob = rob_1p;
    enemy_opp_rob = rob_2p;
    enemy_work = &enemy_work_1p;
    enemy_chara = (uchar)rob_1p->chara;
    enemy_side_frame();
    cpu_mode_1p = enemy_work->state;
  }
  else if ((enemy_self_rob->flags & 4) != 0 && (enemy_opp_rob->flags & 4) == 0) {
    enemy_self_rob = rob_2p;
    enemy_opp_rob = rob_1p;
    enemy_work = &enemy_work_2p;
    enemy_chara = (uchar)rob_2p->chara;
    enemy_side_frame();
    cpu_mode_2p = enemy_work->state;
  }
  else {
    enemy_self_rob = rob_1p;
    enemy_opp_rob = rob_2p;
    enemy_work = &enemy_work_1p;
    enemy_chara = (uchar)rob_1p->chara;
    enemy_side_frame();
    cpu_mode_1p = enemy_work->state;
    enemy_self_rob = rob_2p;
    enemy_opp_rob = rob_1p;
    enemy_work = &enemy_work_2p;
    enemy_chara = (uchar)rob_2p->chara;
    enemy_side_frame();
    cpu_mode_2p = enemy_work->state;
  }
}

void enemy_script_step(void)
{
  uint script_flags;
  int entry;
  uint input;
  int frames;
  uint pair[2];
  enemy_work_rec **pp;

  script_flags = enemy_work->script_flags;
  entry = enemy_work->entry_index;
  input = enemy_work->output_input;
  frames = enemy_work->frames_left;
  if ((script_flags & 0x60000000) == 0) {
    enemy_script_clear();
    return;
  }
  if ((script_flags & 0x04000000) != 0) {
    if ((enemy_ai_type == 0) && ((int)enemy_work->situation < 0)) {
      enemy_script_output_set(0,1);
      return;
    }
    script_flags = script_flags & 0xfbffffff | 0x40000000;
  }
  if ((script_flags & 0x40000000) != 0) {
    script_flags = script_flags & 0xbfffffff | 0x20000000;
    pp = &enemy_work;
    (*pp)->script_flags = script_flags;
  }
  else {
    if (frames != 0) {
      enemy_script_output_set(input,frames);
      return;
    }
    entry++;
    pp = &enemy_work;
    { enemy_work_rec *q = *pp; q->entry_index = entry; }
  }
  enemy_script_entry_get(entry,pair);
  if (((*pp)->script_flags & 0x10000000) != 0) {
    enemy_script_clear();
  }
  else {
    enemy_script_output_set(pair[0],pair[1]);
  }
}

void enemy_state_nagekaeshi(void)
{
  unsigned char chara;
  
  if ((int)enemy_work->script_flags < 0) {
    if ((enemy_work->situation & 0x80000) != 0) {
      enemy_state_clear();
      chara = enemy_opp_rob->chara;
      if (chara == 1 || chara == 7 || chara == 9) {
        enemy_script_start(enemy_script_down_punch,0);
      }
      else if (chara == 12) {
        enemy_script_start(enemy_script_down_punch,0);
      }
    }
    else {
      if ((enemy_opp_rob->flags28 & 0x10000000) != 0) {
        enemy_script_start(enemy_nagekaeshi_scripts_a[game_frame_count % 10],0);
      }
      else {
        enemy_script_start(enemy_nagekaeshi_scripts_b[game_frame_count % 12],0);
      }
      if (10 < enemy_opp_rob->frame) {
        enemy_state_clear();
        enemy_script_clear();
      }
    }
  }
  return;
}

void enemy_state_rise(void)
{
  void *script;
  
  if ((enemy_self_rob->action.part.type != '\t') && ((enemy_work->situation & 4) == 0)) {
    enemy_rise_phase = 0;
    enemy_state_clear();
    return;
  }
  if ((((ushort)(enemy_rise_phase - 2U) < 2) && ((enemy_work->situation & 0x20000) != 0)) &&
     ((int)enemy_work->script_flags < 0)) {
    if (enemy_rise_phase == 2) {
      enemy_rise_pick = enemy_rand() & 7;
      enemy_rise_phase = 3;
      script = enemy_rise_scripts[enemy_rise_pick];
    }
    else if ((game_frame_count & 1U) != 0) {
      script = enemy_script_guard_7;
    }
    else {
      script = enemy_script_down_guard_7;
    }
    enemy_script_start(script,0);
  }
  if (enemy_self_rob->stun > 0) {
    if ((enemy_rise_phase == 0) && ((enemy_self_rob->state & 0x1000) != 0)) {
      enemy_rise_pick = enemy_rand() & 3;
      enemy_script_start(enemy_rise_scripts[enemy_rise_pick],0);
      enemy_rise_phase = 1;
      return;
    }
    if ((int)enemy_work->script_flags < 0) {
      enemy_script_start(enemy_script_wait_guard,0);
    }
  }
  else {
    if (-1 < (int)enemy_work->script_flags) {
      return;
    }
    if (enemy_rise_phase != 1) {
      return;
    }
    if (enemy_self_rob->opp_dist < 2.1) {
      enemy_rise_pick = enemy_rand() & 7;
    }
    else {
      enemy_rise_pick = game_frame_count % 4;
    }
    enemy_rise_phase = 2;
    enemy_script_start(enemy_rise_scripts[enemy_rise_pick],0);
  }
}

void enemy_state_kaeshi(void)
{
  rob_rec **pp = &enemy_opp_rob;
  uint attack_mask;
  uint mask;
  void *script;

  attack_mask = (*pp)->attack_mask;
  enemy_state_clear();
  if ((int)enemy_work->script_flags >= 0) return;
  if ((enemy_chara == 0) | (enemy_chara == 10)) {
    if ((attack_mask & 0x50005) != 0) {
      if ((attack_mask & 0x50000) == 0 || (*pp)->active_start - (*pp)->frame < 13 || (game_frame_count & 1) == 0 || (*pp)->frame == 1) goto msg2a8;
      if (enemy_chara != 0) {
        script = enemy_script_back_gpk;
        goto call;
      }
    } else {
      if ((attack_mask & 0x12) == 0) goto bits;
      if ((attack_mask & 2) == 0) goto msg7b0;
      if (enemy_chara != 0) return;
    }
    script = enemy_script_back_gpk_forward_p;
    goto call;
  msg7b0:
    script = enemy_script_down_pk;
    goto call;
  bits:
    if ((attack_mask & 0x680068) == 0) {
      mask = 0x200;
      goto masktest;
    }
    goto msg2b8;
  }
  if (enemy_chara == 5) {
    if ((attack_mask & 0x50005) != 0) script = enemy_script_forward_pk;
    else if ((attack_mask & 0x680068) != 0) script = enemy_script_down_forward_pk;
    else return;
    goto call;
  }
  if (enemy_chara == 12) {
    mask = 0x50005;
  } else if (enemy_chara != 9) {
    mask = 0x10001;
  } else {
    if ((attack_mask & 0x40004) != 0) {
      if ((enemy_self_rob->flags & 0x10) != 0) {
        script = enemy_script_down_guard_7;
        goto call;
      }
      goto msg2a8;
    }
    mask = 0x80008;
  masktest:
    if ((attack_mask & mask) == 0) return;
  msg2b8:
    script = enemy_script_down_back_pk;
    goto call;
  }
  if ((attack_mask & mask) == 0) return;
msg2a8:
  script = enemy_script_back_pk;
call:
  enemy_script_start(script, 0);
}

void enemy_state_guard(void)
{
  uint situation;
  rob_rec *opp;
  int frame;
  int start;
  void *script;
  int m;
  void (*handler)(void);

  situation = enemy_work->situation;
  if (enemy_round > 2 && (situation & 0x04000000) != 0) {
    opp = enemy_opp_rob;
    frame = opp->frame;
    start = opp->active_start;
    if (frame > start - 12) {
      m = 6;
      if (frame < start) {
        enemy_state_select(situation, m);
        return;
      }
    }
    else {
      enemy_state_select(situation, 2);
      return;
    }
  }
  if ((situation & 0x80000) != 0) goto c1;
  if ((situation & 0x20000) == 0) goto c1;
  if ((situation & 0x10000) != 0) goto next;
c1:
  enemy_state_clear();
  return;
next:
  if ((enemy_opp_rob->field_0x30 & 4) != 0 && (enemy_opp_rob->flags24 & 0x8000) != 0) {
    enemy_state_clear();
    if (enemy_self_rob->opp_dist > 1.5f) {
      handler = enemy_state_attack;
    }
    else {
      handler = enemy_state_nage;
    }
    handler();
    return;
  }
  if (enemy_opp_rob->frame > enemy_opp_rob->active_end) {
    enemy_state_clear();
  }
  enemy_script_clear();
  switch (enemy_opp_rob->attack_level) {
  case 3:
  case 5:
  case 6:
  case 0xd:
    if ((int)lcg_next15() % 7 == 0) goto afd0;
    goto b1c0;
  case 1:
  case 7:
    if (enemy_round <= 3) {
      if (game_frame_count % 10 != 0) goto afd0;
      goto b1c0;
    }
    else {
      if (game_frame_count % 10 == 0) goto afd0;
      goto b1c0;
    }
  default:
    goto afd0;
  }
b1c0:
  script = enemy_script_down_guard_7;
  goto call;
afd0:
  script = enemy_script_guard_7;
call:
  enemy_script_start(script, 0);
  return;
}

void enemy_state_yoke(void)
{
  if ((enemy_work->situation & 0x20000) != 0) {
    if ((int)enemy_work->script_flags >= 0) {
      return;
    }
    if ((enemy_opp_rob->field_0x30 & 8) != 0 || (enemy_opp_rob->flags28 & 0x20) != 0) {
      enemy_work->situation = enemy_work->situation | 0x10000;
      enemy_state_select(enemy_work->situation,4);
      enemy_state_guard();
    }
    else if (enemy_opp_rob->active_start > enemy_opp_rob->frame &&
             (enemy_self_rob->state & 0x4000000) == 0) {
      enemy_state_clear();
      if ((uint)enemy_rand() < 0x30) {
        enemy_sidestep_against_attack();
      }
    }
    else if ((game_frame_count & 1) != 0) {
      enemy_state_clear();
    }
    return;
  }
  if ((enemy_opp_rob->state & 0x40) != 0) {
    return;
  }
  enemy_state_clear();
  if (enemy_self_rob->bound_dist[1] <= 2.0 || enemy_self_rob->bound_dist[2] <= 2.0 ||
      enemy_self_rob->bound_dist[3] <= 2.0) {
  if (enemy_self_rob->bound_dist[1] <= 2.0 && enemy_self_rob->bound_dist[2] > 2.0 &&
             enemy_self_rob->bound_dist[3] > 2.0) {
      if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) enemy_script_start(enemy_script_up_tap,0);
      else enemy_script_start(enemy_script_down_tap,0);
    }
    else if (enemy_self_rob->bound_dist[1] > 2.0 && enemy_self_rob->bound_dist[2] > 2.0 &&
             enemy_self_rob->bound_dist[3] <= 2.0) {
      if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) enemy_script_start(enemy_script_down_tap,0);
      else enemy_script_start(enemy_script_up_tap,0);
    }
    else if (enemy_self_rob->bound_dist[1] > 2.0 && enemy_self_rob->bound_dist[2] <= 2.0 &&
             enemy_self_rob->bound_dist[3] > 2.0) {
      if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) {
        if (enemy_self_rob->bound_dist[1] > enemy_self_rob->bound_dist[3]) enemy_script_start(enemy_script_down_tap,0);
        else enemy_script_start(enemy_script_up_tap,0);
      }
      else {
        if (enemy_self_rob->bound_dist[1] > enemy_self_rob->bound_dist[3]) enemy_script_start(enemy_script_up_tap,0);
        else enemy_script_start(enemy_script_down_tap,0);
      }
    }
    else if (enemy_self_rob->bound_dist[1] <= 2.0 && enemy_self_rob->bound_dist[2] <= 2.0 &&
             enemy_self_rob->bound_dist[3] > 2.0) {
      if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) enemy_script_start(enemy_script_up_tap,0);
      else enemy_script_start(enemy_script_down_tap,0);
    }
    else if (enemy_self_rob->bound_dist[1] > 2.0 && enemy_self_rob->bound_dist[2] <= 2.0 &&
             enemy_self_rob->bound_dist[3] <= 2.0) {
      if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) enemy_script_start(enemy_script_down_tap,0);
      else enemy_script_start(enemy_script_up_tap,0);
    }
    else {
      switch (game_frame_count % 4) {
      case 0:
        enemy_script_start(enemy_script_up_tap,0);
        break;
      case 2:
        enemy_script_start(enemy_script_forward_twice,0);
        break;
      case 3:
        enemy_script_start(enemy_script_up_forward_gk,0);
        break;
      default:
        return;
      }
    }
  }
  else {
    if (enemy_self_rob->screen_xy[0] < enemy_opp_rob->screen_xy[0]) {
      switch (game_frame_count % 5) {
      case 0: case 1: case 3: case 4:
        enemy_script_start(enemy_script_down_tap,0);
        break;
      case 2:
        enemy_script_start(enemy_script_back_twice,0);
        break;
      default:
        return;
      }
    }
    else {
      switch (game_frame_count % 5) {
      case 0: case 1: case 3: case 4:
        enemy_script_start(enemy_script_up_tap,0);
        break;
      case 2:
        enemy_script_start(enemy_script_back_twice,0);
        break;
      default:
        return;
      }
    }
  }
}

void enemy_sidestep_against_attack(void)
{
  uchar pick;
  
  if ((enemy_opp_rob->field_0x328 & 0x100) != 0) {
    if (enemy_opp_rob->screen_xy[0] < enemy_self_rob->screen_xy[0]) {
      if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) != 0) enemy_sidestep_pick(0);
      else if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) == 0) enemy_sidestep_pick(0);
      else enemy_sidestep_pick(1);
    }
    else {
      if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) != 0) enemy_sidestep_pick(1);
      else if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) == 0) enemy_sidestep_pick(1);
      else enemy_sidestep_pick(0);
    }
  }
  else if ((enemy_opp_rob->field_0x328 & 0x200) != 0) {
    if (enemy_opp_rob->screen_xy[0] < enemy_self_rob->screen_xy[0]) {
      if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) != 0) enemy_sidestep_pick(1);
      else if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) == 0) enemy_sidestep_pick(1);
      else enemy_sidestep_pick(0);
    }
    else {
      if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) != 0) enemy_sidestep_pick(0);
      else if ((enemy_opp_rob->side_flags & 0x10) != 0 && (enemy_self_rob->side_flags & 0x10) == 0) enemy_sidestep_pick(0);
      else enemy_sidestep_pick(1);
    }
  }
  else {
    pick = enemy_rand() % 11;
    if (pick < 6) {
      enemy_script_start((game_frame_count & 1) != 0 ? enemy_script_up_tap : enemy_script_down_tap,0);
    }
    else {
      if ((game_frame_count & 1) != 0) enemy_sidestep_pkg_start(1);
      else enemy_sidestep_pkg_start(0);
    }
  }
}

void enemy_work_init(enemy_work_rec *work)
{
  char level;
  char *t;
  int i;
  level = *(char *)&enemy_round_levels[0][0];
  for (i = 0; i < 0x10; i++) {
    work->attacks[i].move = 0;
    work->attacks[i].dist = 0.0;
    work->throws[i] = 0;
  }
  for (i = 0; i < 8; i++) {
    work->repeated_attacks[i].move = 0;
    work->repeated_attacks[i].dist = 0.0;
    work->repeated_throws[i] = 0;
  }
  enemy_hame_found = 0;
  enemy_cleared_flag = 0;
  enemy_records_clear();
  work->level = level;
  work->situation = 0;
  work->situation_prev = 0;
  work->state = 0;
  work->attack_mode = 0;
  work->recover_frames = 0;
  work->land_frames = 0;
  work->start_wait = 0;
  work->timer_0x30 = 0;
  work->step = 0;
  work->output_input = 0;
  work->entry_index = 0;
  work->frames_left = 0;
  work->attack_cooldown = 0;
  work->throw_cooldown = 0;
  work->guard_cooldown = 0;
  work->timer_0x28 = 0;
  {
    short *d = &work->attack_interval;
    t = (char *)&enemy_level_params[0][0];
    *d = ((enemy_level_params_rec *)t)[level].attack_interval;
  }
  work->throw_interval = ((short *)&enemy_level_params[0][0])[level * 7 + 1];
  work->guard_interval = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].guard_interval;
  work->base_attack_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].attack_rate;
  work->base_throw_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].throw_rate;
  work->base_guard_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].guard_rate;
  work->base_sidestep_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].sidestep_rate;
  work->base_reversal_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].reversal_rate;
  work->base_sidestep_count = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].sidestep_count;
  work->base_mash_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].mash_rate;
  work->base_recovery_rate = ((enemy_level_params_rec *)&enemy_level_params[0][0])[level].recovery_rate;
  work->attack_cooldown = 2;
  work->throw_cooldown = 2;
  work->guard_cooldown = 2;
  work->start_wait = 2;
  enemy_state_clear();
  enemy_script_clear();
}

void enemy_side_frame(void)
{
  if (pause_frame_held() != 0) return;
  if ((((enemy_opp_rob->life < 1) || (enemy_self_rob->life < 1)) ||
      ((mode_get_main() == MODE_MAIN_GAME && (round_time < 1)))) ||
     ((game_flags & 0x40) != 0)) {
    enemy_self_rob->ctrl->cpu_buttons = 0;
  }
  else if (cpu_mot_type != CPU_MOT_TYPE_OFF && mode_get_main() == MODE_MAIN_GAME) {
    enemy_situation_update();
    (*enemy_mot_type_funcs[cpu_mot_type])();
    enemy_script_step();
    {
      char *p = (char *)enemy_work;
      int o = 0x134;
      ushort v;
      o = *(int *)(p + o);
      v = enemy_input_side_swap(o);
      enemy_self_rob->ctrl->cpu_buttons = v;
    }
  }
  else {
    if (enemy_ai_attack_frames != 0) {
      enemy_ai_attack_frames = enemy_ai_attack_frames + 1;
    }
    enemy_opponent_moves_record();
    enemy_level_update();
    enemy_situation_update();
    enemy_timers_step();
    enemy_rates_update();
    enemy_think();
  }
}

void enemy_think(void)
{
  uint input;

  if (enemy_ai_type != 0) {
    enemy_think_ai_type_stub_a();
    enemy_think_ai_type_stub_b();
  }
  enemy_prev_state = enemy_work->state;
  enemy_state_choose();
  if (enemy_prev_state == enemy_work->state) {
    if ((enemy_work->script_flags & 0x20000000) == 0) {
      enemy_state_frames = enemy_state_frames + 1;
    }
  }
  else {
    enemy_state_frames = 0;
  }
  if ((enemy_work->situation & 0x20000000) != 0) {
    (*enemy_state_funcs[enemy_work->state])();
  }
  enemy_script_step();
  input = enemy_work->output_input;
  input = enemy_input_side_swap(input);
  enemy_self_rob->ctrl->cpu_buttons = input;
  if (0xb3 < enemy_state_frames) {
    enemy_state_clear();
  }
}

void enemy_script_start_random(void **list, int count)
{
  short n = count;
  short v;
  void *script;
  v = enemy_rand() % n;
  if (v >= n) v = 0;
  script = list[v];
  enemy_script_start(script,script);
}

void enemy_opponent_moves_record(void)
{
  int i;
  int ok;
  int t;
  int c;
  if (enemy_opp_rob->coli->field_0x1c[1] != 0) {
    c = enemy_opp_rob->coli->field_0x94 - 1;
    t = (uint)c > 4;
    ok = t == 0;
    if ((enemy_opp_rob->side_flags & 0x2000) != 0) ok = 0;
    if (ok == 0) {
      return;
    }
    for (i = 0xf; i >= 0; i--) {
      if (i != 0xf) {
        enemy_work->attacks[i + 1].move =
             enemy_work->attacks[i].move;
        enemy_work->attacks[i + 1].dist =
             enemy_work->attacks[i].dist;
      }
    }
    enemy_work->attacks[0].move = enemy_opp_rob->move_no;
    enemy_work->attacks[0].dist = enemy_opp_rob->opp_dist;
    enemy_repeated_attacks_collect();
  }
  if ((((enemy_opp_rob->state & 0x100000) != 0) &&
      ((enemy_self_rob->state & 0x200000) != 0)) && (enemy_opp_rob->frame == 1)) {
    for (i = 0xf; i >= 0; i--) {
      if (i != 0xf) {
        enemy_work->throws[i + 1] =
             enemy_work->throws[i];
      }
    }
    enemy_work->throws[0] = enemy_opp_rob->move_no;
    enemy_repeated_throws_collect();
  }
}

void enemy_repeated_attacks_collect(void)
{
  int i;
  int j;
  int f;
  ushort cnt[16];
  enemy_move_log_rec dup[16];
  enemy_move_log_rec out[16];
  
  for (i = 0; i < 16; i++) {
    cnt[i] = 0;
    dup[i].move = 0;
    dup[i].dist = 0.0;
    out[i].move = 0;
    out[i].dist = 0.0;
  }
  for (i = 0; i < 16; i++) {
    if (enemy_work->attacks[i].move != 0) {
      for (j = i; j < 16; j++) {
        if (i != j && enemy_work->attacks[i].move == enemy_work->attacks[j].move) {
          dup[i].move = enemy_work->attacks[i].move;
          dup[i].dist = enemy_work->attacks[i].dist;
          cnt[i]++;
        }
      }
    }
  }
  for (i = 0; i < 16; i++) {
    if (cnt[i] > 1) {
      out[i].move = dup[i].move;
      out[i].dist = dup[i].dist;
    }
  }
  for (i = 0; i < 16; i++) {
    f = 0;
    for (j = 0; j < 8; j++) {
      if (out[i].move != 0) {
        if (out[i].move != enemy_work->repeated_attacks[j].move) {
          f = 1;
        }
        else {
          f = 0;
          break;
        }
      }
    }
    if (f) {
      int n;
      for (n = 7; n >= 0; n--) {
        if (n != 7) {
          enemy_work->repeated_attacks[n + 1].move = enemy_work->repeated_attacks[n].move;
          enemy_work->repeated_attacks[n + 1].dist = enemy_work->repeated_attacks[n].dist;
        }
      }
      enemy_work->repeated_attacks[0].move = out[i].move;
      enemy_work->repeated_attacks[0].dist = out[i].dist;
    }
  }
}

uint enemy_pick_hame(uint situation)
{
  int i;
  
  if ((situation & 0x04000000) != 0) return situation;
  for (i = 0; i < 8; i++) {
    if (enemy_opp_rob->move_no == enemy_work->repeated_attacks[i].move) {
      enemy_hame_found = 1;
      enemy_work->attack_mode = 4;
      enemy_work->attack_rate = ((struct { char t[8]; } *)enemy_difficulty_rates)->t[user_back_data.rank_difficult];
      enemy_work->sidestep_rate = (uchar)((struct { char t[8]; } *)enemy_difficulty_rates)->t[user_back_data.rank_difficult] >> 1;
      enemy_work->guard_rate = ((struct { char t[8]; } *)enemy_difficulty_rates)->t[user_back_data.rank_difficult];
      enemy_work->guard_cooldown = 0;
      situation = enemy_state_select(situation,0xd);
    }
  }
  return situation;
}

uint enemy_pick_nagekaeshi(uint situation)
{
  int i;
  uchar *rate;
  
  if ((situation & 0x800) != 0) {
    if (enemy_round > 2) {
      for (i = 0; i < 8; i++) {
        if (enemy_opp_rob->move_no == enemy_work->repeated_throws[i]) {
          return enemy_state_select(situation,0xc);
        }
      }
    }
    rate = (uchar *)enemy_work;
    rate += 0x14d;
    if (*rate > (uint)enemy_rand()) {
      situation = enemy_state_select(situation,0xc);
    }
  }
  else if ((situation & 0x80000) != 0 && enemy_round >= 4) {
    rate = (uchar *)enemy_work;
    rate += 0x14d;
    if (*rate > (uint)enemy_rand()) {
      situation = enemy_state_select(situation,0xc);
    }
  }
  return situation;
}

void enemy_state_hame(void)
{
  short frames_to_active;
  uint table_no;
  void **list;
  uchar pick;
  uint t;
  enemy_work_rec *work;

  frames_to_active = enemy_opp_rob->active_start - enemy_opp_rob->frame;
  if (enemy_opp_rob->frame == 1) frames_to_active = 0;
  enemy_state_clear();
  if (((enemy_work->situation & 0x04000000) != 0 && !(enemy_self_rob->opp_dist > 1.8)) ||
      frames_to_active > 17 || (enemy_opp_rob->state & 0xc0) != 0 ||
      enemy_self_rob->frame_disadvantage <= -10) {
    work = enemy_work;
    if (work->step == 0) {
      if ((work->situation & 0x80000000) != 0) return;
      enemy_data_table_get(2,0);
      if ((enemy_work->script_flags & 0x80000000) == 0) return;
      if ((enemy_opp_rob->state & 0xc0) != 0 &&
          !(enemy_opp_rob->frame > enemy_opp_rob->active_end)) {
        if ((game_frame_count & 1) != 0) {
          enemy_script_start(enemy_script_guard_10,0);
        }
        else {
          enemy_sidestep_against_attack();
        }
      }
      else {
        if ((enemy_opp_rob->flags & 0x10) != 0 &&
            (enemy_work->situation & 0x20000) == 0 &&
            (enemy_opp_rob->state & 0x40) == 0) table_no = 4;
        else if (hit_wait_timer_get() <= 12 &&
                 (enemy_work->situation & 0x20000) == 0 &&
                 (enemy_opp_rob->state & 0x40) == 0) table_no = 10;
        else table_no = 2;
        list = enemy_data_table_get(table_no,enemy_chara);
        enemy_script_start_random(list,enemy_script_list_count(list));
      }
      enemy_work->step = enemy_work->step + 1;
      return;
    }
  }
  else if (enemy_work->step == 0) {
    if ((enemy_work->situation & 0x80000000) != 0) return;
    if ((enemy_work->script_flags & 0x80000000) == 0) return;
    if (enemy_opp_rob->active_end < enemy_opp_rob->frame) return;
    enemy_work->step = 1;
    pick = enemy_rand() % 13;
    switch (pick) {
    case 0:
    case 1:
      if ((enemy_self_rob->state & 0x04000000) != 0) {
        enemy_script_start(enemy_script_down_punch,0);
        return;
      }
      if ((enemy_opp_rob->field_0x30 & 8) == 0 && (enemy_opp_rob->flags28 & 0x20) == 0) {
        enemy_sidestep_against_attack();
        return;
      }
      break;
    case 5:
    case 6:
    case 7:
      switch (enemy_opp_rob->attack_level) {
      case 1:
      case 7:
        enemy_script_start(enemy_script_down_punch,0);
        return;
      }
      if (enemy_opp_rob->active_start - enemy_opp_rob->frame > 13) {
        enemy_script_start(enemy_script_down_punch,0);
        return;
      }
      break;
    case 8:
      if ((enemy_opp_rob->field_0x30 & 8) == 0 && (enemy_opp_rob->flags28 & 0x20) == 0) {
        enemy_sidestep_pkg_start(0);
        return;
      }
      break;
    case 9:
      if ((enemy_opp_rob->field_0x30 & 8) == 0 && (enemy_opp_rob->flags28 & 0x20) == 0) {
        enemy_sidestep_pkg_start(1);
        return;
      }
      break;
    }
    enemy_work->situation = enemy_work->situation | 0x10000;
    enemy_state_select(enemy_work->situation,4);
    enemy_state_guard();
    return;
  }
  if ((enemy_work->script_flags & 0x80000000) == 0) return;
  t = enemy_work->situation & 0x80000000;
  if (t != 0) return;
  enemy_rates_maxed = t;
  enemy_script_clear();
  enemy_state_clear();
}

void enemy_level_update(void)
{
  int level;
  int o;
  uchar *e;
  uchar *t;
  { char *i = (char *)(int)enemy_round; int row = (int)(&enemy_round_levels[0][0] + user_back_data.rank_difficult * 16); level = i[row]; }
  if ((game_flags & 0x40000) != 0) {
    level = (char)(level - 5);
    if (level < 0) level = 0;
  }
  if (enemy_opponent_pad_idle() != 0) enemy_pad_idle_frames++;
  else enemy_pad_idle_frames = 0;
  if (enemy_pad_idle_frames > 0x1df) level = 30;
  if (mode_get_main() == MODE_MAIN_ADVERTISE) {
    if (enemy_self_rob->side == 0) level = 30;
    else level = 15;
  }
  if (level > 30) level = 30;
  if (enemy_work->level != level) enemy_work->level = level;
  enemy_rank = enemy_work->level;
  enemy_work->attack_interval = *(short *)(e = (t = &enemy_level_params[0][0]) + (o = level * 14));
  enemy_work->throw_interval = *(short *)((t += 2) + o);
  enemy_work->guard_interval = ((enemy_level_params_rec *)e)->guard_interval;
  enemy_work->base_attack_rate = ((enemy_level_params_rec *)e)->attack_rate;
  enemy_work->base_throw_rate = ((enemy_level_params_rec *)e)->throw_rate;
  enemy_work->base_guard_rate = ((enemy_level_params_rec *)e)->guard_rate;
  enemy_work->base_sidestep_rate = ((enemy_level_params_rec *)e)->sidestep_rate;
  enemy_work->base_reversal_rate = ((enemy_level_params_rec *)e)->reversal_rate;
  enemy_work->base_sidestep_count = ((enemy_level_params_rec *)e)->sidestep_count;
  enemy_work->base_mash_rate = ((enemy_level_params_rec *)e)->mash_rate;
  enemy_work->base_recovery_rate = ((enemy_level_params_rec *)e)->recovery_rate;
}

void enemy_state_attack(void)
{
  typedef struct { float x; float lo; float hi; } range_ent;
  void **list;
  void **list2;
  uint situation;
  short frames_left;
  float lo;
  float hi;
  float off;
  uint table_no;
  int chara;
  situation = enemy_work->situation;
  if (enemy_work->step == 0) {
    if ((situation & 0x80000000) != 0) return;
    frames_left = enemy_opp_rob->recovery_end - enemy_opp_rob->frame - enemy_self_rob->stun;
    chara = enemy_chara;
    lo = ((range_ent *)enemy_chara_ranges)[chara].lo;
    hi = ((range_ent *)enemy_chara_ranges)[chara].hi;
    off = stage_cur->center.y;
    lo += off;
    hi += off;
    if ((enemy_work->script_flags & 0x80000000) == 0) return;
    if ((situation & 0x8020000) == 0x20000) {
      if ((situation & 0x4000000) == 0 || enemy_self_rob->opp_dist > 1.8) {
        enemy_state_yoke();
        return;
      }
      table_no = 2;
    }
    else {
      if (enemy_round <= 4) {
        list = enemy_data_table_get(1,enemy_chara);
        goto second;
      }
      table_no = 0;
      chara = enemy_chara; /* WORKAROUND (kept as possibly original; no game instruction of its own): reloads a global nothing has changed since it was read. The game loads it here either way; without this line only the branch layout changes (an extra bra). Kept as plausible original code. */
    }
    list = enemy_data_table_get(table_no,chara);
  second:
    if ((enemy_opp_rob->flags & 0x10) != 0) table_no = 9;
    else {
      if ((enemy_self_rob->flags & 0x10) == 0) goto skip;
      table_no = 10;
    }
    chara = enemy_chara;
    list = enemy_data_table_get(table_no,chara);
  skip:
    if ((situation >> 24 & 1) & (frames_left <= 5)) {
      if ((game_frame_count & 1) != 0 && !(enemy_self_rob->opp_dist > 1.5)) {
        if ((enemy_opp_rob->flags & 0x10) == 0) table_no = 3;
        else goto low;
      }
      else table_no = 2;
    }
    else if ((situation >> 24 & 1) & (frames_left > 5)) {
      if ((enemy_opp_rob->flags & 0x10) != 0) table_no = 9;
      else if ((enemy_self_rob->flags & 0x10) != 0) table_no = 10;
      else {
      low:
        if (enemy_round <= 4) {
          list2 = enemy_data_table_get(1,enemy_chara);
          enemy_script_start_random(list2,enemy_script_list_count(list2));
          goto tail;
        }
        table_no = 0;
      }
    }
    else {
      if ((situation & 0x2000000) == 0) goto done;
      if ((uint)enemy_rand() > 0x1f) goto done;
      table_no = 8;
    }
    chara = enemy_chara;
    list2 = enemy_data_table_get(table_no,chara);
    enemy_script_start_random(list2,enemy_script_list_count(list2));
    goto tail;
  done:
    enemy_script_start_random(list,enemy_script_list_count(list));
  tail:
    enemy_followup_opp_land_frames = 0;
    enemy_followup_opp_height = 0.0;
    enemy_followup_opp_dist = 0.0;
    enemy_work->attack_cooldown = enemy_work->attack_interval;
    if ((situation & 0x200000) != 0 && (enemy_opp_rob->state & 0x1000) == 0 && lo <= enemy_opp_rob->pos.y && enemy_opp_rob->pos.y <= hi) {
      enemy_rates_maxed = 1;
      enemy_work->attack_cooldown = 0;
      enemy_followup_script_start();
    }
    if (enemy_opp_rob->pos.y > hi) enemy_script_clear();
    enemy_work->step = enemy_work->step + 1;
  }
  else {
    if ((enemy_work->script_flags & 0x80000000) == 0) return;
    if ((situation & 0x80000000) != 0) return;
    enemy_rates_maxed = situation & 0x80000000;
    enemy_script_clear();
    enemy_state_clear();
  }
  return;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void enemy_record_fill(int table,int group,int row)
{
  vec3 delta;
  float dist_sq;
  enemy_record_rec *rec;
  delta.x = enemy_opp_rob->pos.x - enemy_origin.x;
  delta.z = enemy_opp_rob->pos.z - enemy_origin.z;
  dist_sq = delta.x * delta.x + delta.z * delta.z;
  rec = (enemy_record_rec *)((group * 10 + row) * 0x20 + table);
  rec->dist = fsqrt_asm(dist_sq);
  rec->active_end = enemy_opp_rob->active_end;
  rec->recovery_end = enemy_opp_rob->recovery_end;
  rec->active_start = enemy_opp_rob->active_start;
  rec->attack_damage = enemy_opp_rob->attack_damage;
  rec->marker[0] = 0x11;
  rec->marker[1] = 0x22;
  rec->marker[2] = 0x33;
  rec->marker[3] = 0x44;
}

int enemy_ai_attack_kind_is_1(ushort move_id)
{
  int i;
  int result = 0;
  int n = 0x5c;
  
  for (i = 0; i < n; i++) {
    enemy_ai_attack_rec *e = enemy_ai_attack_table;
    if (move_id == e[i].move) {
      if (e[i].kind == 1) result = 1;
      break;
    }
  }
  return result;
}

int enemy_ai_attack_kind_is_2(ushort move_id)
{
  int i;
  int result = 0;
  int n = 0x5c;
  
  for (i = 0; i < n; i++) {
    enemy_ai_attack_rec *e = enemy_ai_attack_table;
    if (move_id == e[i].move) {
      if (e[i].kind == 2) result = 1;
      break;
    }
  }
  return result;
}

ushort enemy_ai_attack_frames_spare(ushort move_id)
{
  short frames = 0;
  int i;
  int n = 0x5c;
  
  for (i = 0; i < n; i++) {
    enemy_ai_attack_rec *e = enemy_ai_attack_table;
    if (move_id == e[i].move) {
      enemy_script_step_rec *q;
      for (q = e[i].script; q->frames != 0; q++) frames += q->frames;
      break;
    }
  }
  frames = enemy_ai_attack_frame_base - enemy_opp_rob->active_start - frames;
  if (frames < 1) frames = 2;
  return frames;
}
