/*
 * vf4lib/libmission/src/msnm.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_chlg_route_time_draw
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float msnm_last_random; /* 0x0c450f18 */
extern float msnm_item_chances[8][5]; /* 0x0c19c0b0 */
extern int msnm_money_rewards[8][5]; /* 0x0c19c150 */
extern int msnm_char_items[16][2]; /* 0x0c19c328 */
extern int msnm_char_item_counts[16]; /* 0x0c19c714 */
extern float msnm_stage_scales[24]; /* 0x0c19c760 */
extern short msnm_target_move_pairs[4][2]; /* 0x0c19c96a */
extern int mode_timer; /* 0x0c292550 */
extern int round_time; /* 0x0c292558 */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern undefined4 course_time_total; /* 0x0c2925f8 */
extern int msnm_hyper_counts[3]; /* 0x0c2a0e24 */
extern char * msn_noname_text; /* 0x0c2a1168 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern undefined4 hit_light_flags; /* 0x0c3728a8 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern camera_state camera; /* 0x0c370ce0 */

void msnm_boot_init(void)
{
  msnm_work = &msnm_work_area;
  msnm_work_reset();
  msnm_side_tallies_clear();
  msnm_side_trackers_clear();
  return;
}

void msnm_start(int side)
{
  msnm_try_rec next_try;
  msnm_try_setting_rec next_setting;

  msnm_work->active = 1;
  msnm_work->side = side;
  msnm_work->saved_side = msnm_work->side;
  msnm_work->saved_active = msnm_work->active;
  msnm_work->status = 0;
  msnm_try_record_reset(&msnm_work->cur_try);
  msnm_progress_clear(&msnm_work->progress,&msnm_work->disp);
  msnm_work->column = 0;
  msnm_work->row = 0;
  msnm_work->choice = 0;
  msnm_work->grade_average = -1;
  msnm_work->rank = -1;
  msnm_work->points = 0;
  msnm_work->time_total = 0;
  msnm_work->reward = 0;
  msnm_work->item_given = 0;
  msnm_work->item = -1;
  msnm_course_results_reset(&msnm_work->course);
  msnm_try_pick(0,0,&next_try,&next_setting);
  msnm_mission_set(&next_try,&next_setting);
  return;
}

void msnm_resume(void)
{
  msnm_work->active = 1;
  msnm_work->status = 1;
  msnm_progress_clear(&msnm_work->progress,&msnm_work->disp);
  return;
}

void msnm_end(void)
{
  msnm_mission_clear();
  msnm_textures_free();
  msnm_work_reset();
  return;
}

void msnm_intrude_suspend(void)
{
  if (msnm_work->active != '\0') {
    msnm_work->active = 0;
    msnm_work->status = 4;
    msnm_work->saved_stage = msnm_work->stage;
    msnm_work->saved_opponent = msnm_work->opponent;
  }
  chlg_map_disp_free_all();
  mode_sel_close();
  msnm_textures_free();
  return;
}

int msnm_vs_result_apply(char winner)
{
  int result;

  if ((int)(char)msnm_work->saved_side == (int)winner) {
    msnm_work->active = msnm_work->saved_active;
    if (!(msnm_work->active == '\0')) {
      msnm_work->stage = msnm_work->saved_stage;
      msnm_work->opponent = msnm_work->saved_opponent;
      msnm_resume();
    }
    else {
      msnm_side_keep((int)winner);
    }
    result = 0;
  }
  else {
    msnm_end();
    result = 1;
  }
  return result;
}

void msnm_side_keep(int side)
{
  msnm_work->active = 0;
  msnm_work->saved_active = msnm_work->active;
  msnm_work->saved_side = side;
  return;
}

uchar msnm_quit_chosen_get(void)
{
  uchar *p = (uchar *)offsetof(msnm_work_rec, quit.chosen);
  return p[(int)msnm_work];
}

void msnm_mission_set(msnm_try_rec *mission_try, msnm_try_setting_rec *setting)
{
  msnm_work->cur_try = *mission_try;
  msnm_work->status = 1;
  msnm_work->setting = *setting;
  msnm_work->stage = setting->stage;
  msnm_work->opponent = setting->opponent;
  msnm_work->disp.variant = setting->variant;
  msnm_progress_clear(&msnm_work->progress,&msnm_work->disp);
}

void msnm_mission_clear(void)
{
  msnm_try_record_reset(&msnm_work->cur_try);
  msnm_work->status = 0;
  return;
}

int msnm_mission_present(void)
{
  int r = msnm_work->cur_try.type == -1;
  return r == 0;
}

void msnm_stage_start(void)
{
  char chara;
  int i;
  int type;

  if (((msnm_work->active != '\0') && (msnm_mission_present() != 0)) &&
     (msnm_work->status != 0)) {
    msnm_work->round_count = 0;
    msnm_work->wins_ko = 0;
    msnm_work->wins_perfect = 0;
    msnm_work->wins_ring_out = 0;
    msnm_work->wins_time_up = 0;
    for (i = 0; i < 2; i++) {
      msnm_work->round_times[i] = 0;
      msnm_work->round_lives[i] = 0;
      msnm_work->round_opp_lives[i] = 0;
      msnm_work->round_results[i] = 0;
    }
    msnm_work->score = 0;
    msnm_work->grade = 0;
    msnm_progress_match_clear(&msnm_work->progress,&msnm_work->disp);
    type = msnm_work->cur_try.type;
    chara = *(char *)(&player_chara.side0 + msnm_work->side);
    msnm_cmd_list_setup(type,(int)chara);
    msnm_target_move_setup(type,(int)chara);
    course_time_total = 0;
    msn_text_init();
    msnm_work->stage_dim_level = msnm_stage_scales[stage_cur_no];
  }
  return;
}

void msnm_round_start(void)
{
  int present;

  if (((msnm_work->active != '\0') && (present = msnm_mission_present(), present != 0)) &&
     (msnm_work->status != 0)) {
    msnm_progress_round_clear(&msnm_work->progress,&msnm_work->disp);
    msnm_cmd_rows_progress_clear();
    msnm_round_sound_cmds();
  }
  return;
}

int msnm_opponent_char_get(void)
{
  int present;
  int chara;

  if ((msnm_work->active == '\0') || (present = msnm_mission_present(), present == 0)) {
    chara = 0;
  }
  else {
    chara = msnm_work->opponent;
  }
  return chara;
}

int msnm_stage_get(void)
{
  int present;
  int stage;

  if ((msnm_work->active == '\0') || (present = msnm_mission_present(), present == 0)) {
    stage = 0;
  }
  else {
    stage = msnm_work->stage;
  }
  return stage;
}

uint msnm_cpu_level_get(void)
{
  uint level;

  level = msnm_work->cur_try.cpu_level;
  if (level == 0xffffffff) {
    level = (msnm_work->row - msnm_work->column) + 5;
  }
  return level & 0xff;
}

int msnm_mission53_flag_get(void)
{
  msnm_work_rec *work;
  msnm_try_rec *mission_try;
  msnm_combo_variant_rec *variant;
  char c;

  work = msnm_work;
  mission_try = &work->cur_try;
  if (mission_try->type == 0x35) {
    c = *(char *)(&player_chara.side0 + work->side);
    variant = msnm_combo_variant_get(mission_try->param,c);
    if ((variant->flags & 1) != 0) {
      return 1;
    }
  }
  return 0;
}

int msnm_cpu_energy_get(char side)
{
  int *energy;
  if (msnm_work->side == side) return 0xc9;
  energy = &msnm_work->cur_try.match.energy;
  if (*energy == -1) return 0xc9;
  return *energy;
}

int msnm_match_energy_get(int side)
{
  char c = side;
  volatile int *energy;
  if (msnm_work->side == c) return 0xc9;
  energy = (volatile int *)&msnm_work->cur_try.match.energy;
  if (*energy == -1) return 0xc9;
  return *energy;
}

int msnm_round_time_get(void)
{
  msnm_match_settings_rec *match = &msnm_work->cur_try.match;

  if (match->round_time == -1) return 0x708;
  return match->round_time * 0x3c;
}

int msnm_rounds_to_win_get(void)
{
  msnm_match_settings_rec *match = &msnm_work->cur_try.match;
  if (match->rounds_to_win == -1) return 2;
  return *(char *)&match->rounds_to_win;
}

void msnm_disp(void)
{
  char side;
  char other;
  int present;
  uint held;
  msnm_try_type_rec *entry;
  rob_rec *rob;
  rob_rec *opp;

  if (((msnm_work->active != '\0') && (present = msnm_mission_present(), present != 0)) &&
     (held = pause_frame_held(), held == 0)) {
    entry = msnm_entry_get(msnm_work->cur_try.type);
    side = msnm_work->side;
    other = side == 0;
    rob = rob_get((uchar)side);
    opp = rob_get((uchar)other);
    if (entry->frame != 0) {
      (*entry->frame)(rob,opp,&msnm_work->cur_try,&msnm_work->progress);
    }
  }
  return;
}

void msnm_judge_init(void)
{
  msnm_try_type_rec *entry;
  char side;
  int other;
  rob_rec *rob;
  rob_rec *opp;
  if (msnm_work->active == '\0') return;
  if (msnm_mission_present() == 0) return;
  if (pause_frame_held() != 0) return;
  entry = msnm_entry_get(msnm_work->cur_try.type);
  side = msnm_work->side;
  other = side == 0;
  rob = rob_get((uchar)side);
  opp = rob_get((uchar)other);
  msnm_work->round_results[msnm_work->round_count] = 2;
  if (round_winner == msnm_work->side) {
    msnm_work->round_results[msnm_work->round_count] = 1;
    if ((game_flags & 0x80) != 0) {
      msnm_work->wins_perfect++;
    }
    else if ((game_flags & 0x20) != 0) {
      msnm_work->wins_ko++;
    }
    if ((game_flags & 0x10) != 0) {
      msnm_work->wins_time_up++;
    }
    if ((game_flags & 0x40) != 0) {
      msnm_work->wins_ring_out++;
    }
  }
  msnm_work->round_times[msnm_work->round_count] = round_time;
  msnm_work->round_lives[msnm_work->round_count] = rob->life;
  msnm_work->round_opp_lives[msnm_work->round_count] = opp->life;
  msnm_work->round_count++;
  if (msnm_work->round_count >= msnm_rounds_to_win_get()) {
    game_flags = game_flags | 0xc00;
  }
  if (entry->round_end != (code *)0x0) {
    (*entry->round_end)(rob,opp,&msnm_work->cur_try,&msnm_work->progress);
  }
  if ((game_flags & 0x400) != 0) {
    msnm_result_judge();
  }
}

void msnm_judge_disp(void)
{
  return;
}

int msnm_mission60_full_life(void)
{
  int i;
  rob_rec *rob;
  if ((msnm_work->active == '\0') || (msnm_mission_present() == 0) || (pause_frame_held() != 0)) {
    return 0;
  }
  if ((msnm_work->cur_try.type != 0x3c) || ((game_flags & 0x20) == 0)) {
    return 0;
  }
  for (i = 0; i < 2; i++) {
    rob = rob_get(i & 0xff);
    if (rob->life == rob->life_max) {
      return 1;
    }
  }
  return 0;
}

uchar msnm_active(void)
{
  return msnm_work->active;
}

int msnm_player_side(void)
{
  return (int)(char)msnm_work->side;
}

int msnm_status_get(char side)
{
  int status;

  if ((msnm_work->active == '\0') || ((uint)msnm_work->side != (int)side)) {
    status = 0;
  }
  else {
    status = msnm_work->status;
  }
  return status;
}

void msnm_status_set(char side, int status)
{
  if ((msnm_work->active != '\0') && ((uint)msnm_work->side == (int)side)) {
    msnm_work->status = status;
  }
  return;
}

int msnm_cleared(char side)
{
  int r;
  r = 0;
  if (!(msnm_work->active != '\0' && msnm_work->side == side)) return r;
  if (msnm_work->status == 2) r = 1;
  return r;
}

int msnm_round_result_get(int round)
{
  return msnm_work->round_results[round];
}

msnm_try_rec * msnm_mission_record_get(char side)
{
  int present;
  msnm_try_rec *mission_try;

  if (((msnm_work->active == '\0') || (present = msnm_mission_present(), present == 0)) ||
     ((uint)msnm_work->side != (int)side)) {
    mission_try = (msnm_try_rec *)0x0;
  }
  else {
    mission_try = &msnm_work->cur_try;
  }
  return mission_try;
}

msnm_progress_rec * msnm_progress_get(void)
{
  return &msnm_work->progress;
}

msnm_try_disp_rec * msnm_work_b4_get(void)
{
  return &msnm_work->disp;
}

msnm_try_type_rec * msnm_entry_get(int type)
{
  return &msnm_try_types[type];
}

uchar ** msnm_entry_masks_get(int type)
{
  return (uchar **)&msnm_try_types[type].stage_mask;
}

void msnm_mission19_cpu_mark(char side)
{
  msnm_work_rec *work;
  msnm_progress_rec *progress;
  int a;
  int b;
  int other;
  int type;

  if (msnm_work->active != '\0') {
    if (msnm_mission_present() != 0) {
      work = msnm_work;
      type = work->cur_try.type;
      other = work->side == '\0';
      progress = &work->progress;
      a = type == 0x13;
      b = side == other;
      if (a & b) {
        progress->round_flag = 1;
      }
    }
  }
  return;
}

void msnm_mission27_mark(char guard_side)
{
  msnm_work_rec *work;
  msnm_progress_rec *progress;
  int type;

  if ((msnm_work->active != '\0') && (msnm_mission_present() != 0)) {
    work = msnm_work;
    type = work->cur_try.type;
    progress = &work->progress;
    if (type == 0x1b) {
      progress->round_flag = 1;
      progress->round_cnt = guard_side;
    }
  }
  return;
}

int msnm_column_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->column;
}

int msnm_row_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->row;
}

void msnm_row_column_set(int row, int column)
{
  if (row > 7 || column > row) return;
  msnm_work->row = row;
  msnm_work->column = column;
  return;
}

void msnm_textures_load(void)
{
  if (msnm_work->loaded == '\0') {
    msnm_vram_free_total = ((int (*)(void))tex_vram_free_total)();
    msnm_vram_free_second = ((int (*)(void))tex_vram_free_second)();
    msnm_disp_init();
    msnm_disp_scenes_load();
    msnm_work->loaded = 1;
    msnm_vram_free_total = ((int (*)(void))tex_vram_free_total)();
    msnm_vram_free_second = ((int (*)(void))tex_vram_free_second)();
  }
}

void msnm_textures_free(void)
{
  if (msnm_work->loaded != 0) {
    ((struct { uint a; } *)&msnm_vram_free_total)->a = tex_vram_free_total();
    ((struct { uint a; } *)&msnm_vram_free_second)->a = tex_vram_free_second();
    msnm_disp_scenes_free();
    msnm_disp_scene0_free();
    msnm_disp_release();
    msnm_work->loaded = 0;
    ((struct { uint a; } *)&msnm_vram_free_total)->a = tex_vram_free_total();
    ((struct { uint a; } *)&msnm_vram_free_second)->a = tex_vram_free_second();
  }
}

void msnm_course_chars_get(int *chara_now, int *chara_next_a, int *chara_next_b)
{
  int active;
  mission_score_rec *node;

  *chara_now = -1;
  *chara_next_a = -1;
  *chara_next_b = -1;
  active = msnm_active();
  if (active != 0) {
    node = chlg_map_node_get(msnm_work->column,msnm_work->row);
    *chara_now = (int)node->chara;
    node = chlg_map_node_get(msnm_work->column,msnm_work->row + 1);
    if (node != 0) {
      *chara_next_a = (int)node->chara;
    }
    node = chlg_map_node_get(msnm_work->column + 1,msnm_work->row + 1);
    if (node != 0) {
      *chara_next_b = (int)node->chara;
    }
  }
  return;
}

void msnm_intro_draw(void)
{
  int present;
  mission_score_rec *node;

  if (((msnm_work->active != '\0') && (present = msnm_mission_present(), present != 0)) &&
     (msnm_work->status != 0)) {
    msnm_disp_frame_stub();
    msnm_map_base_open(0,0);
    chlg_screen_dim_draw(msnm_work->stage_dim_level);
    msnm_map_course_draw(1);
    msnm_map_round_parts_draw(0,0);
    msn_panel_draw((int)(char)msnm_work->side,&msnm_work->cur_try,&msnm_work->setting,1,0)
    ;
    node = chlg_map_node_get(msnm_work->column,msnm_work->row);
    chlg_node_hiscore_draw(node,0);
    chlg_route_cursor_set(&msnm_work->next_tries[msnm_work->column],
                 &msnm_work->next_settings[msnm_work->column]);
    sprite_colors_reset();
  }
  return;
}

void msnm_fight_draw(void)
{
  int present;

  if ((msnm_work->active != '\0') && (present = msnm_mission_present(), present != 0)) {
    if (msnm_work->status != 0) {
      msn_panel_draw((int)(char)msnm_work->side,&msnm_work->cur_try,&msnm_work->setting,
                   msnm_work->status,1);
      msnm_map_round_parts_draw(1,0);
      sprite_colors_reset();
    }
  }
  return;
}

void msnm_round_init_hook(void)
{
  int present;

  if (((msnm_work->active != '\0') && (present = msnm_mission_present(), present != 0)) &&
     (msnm_work->status != 0)) {
    chlg_progress_bar_draw((int)(char)msnm_work->side,&msnm_work->course);
  }
  return;
}

void msnm_join_draw(void)
{
  switch (msnm_work->state) {
  case 0:
    winner_mes_draw();
    break;
  case 1:
    mode_sel_choices_draw(msnm_work->choice,0);
    mode_sel_timer_draw(msnm_work->menu_timer);
    mode_sel_box_draw(0);
    break;
  case 2:
  default:
    { msnm_work_rec *work = msnm_work; mode_sel_choices_draw(work->choice,1); }
    mode_sel_box_draw(0);
    break;
  }
  sprite_colors_reset();
}

void msnm_result_draw(void)
{
  int course_end;
  uint timed;
  int row;

  if (msnm_work->active == '\0') {
    return;
  }
  msnm_disp_frame_stub();
  chlg_bars_draw();
  timed = 1;
  switch (msnm_work->state) {
  case 0:
    chlg_outcome_banner_show();
    break;
  case 1:
    msnm_map_base_open(msnm_work->choice,1);
    msnm_map_course_draw(0);
    chlg_result_draw();
    chlg_map_layout_update(1);
    break;
  case 2:
    chlg_total_score_draw(&msnm_work->course,msnm_work->score,msnm_work->grade);
    msnm_map_course_draw(0);
    chlg_map_layout_update(1);
    break;
  case 3:
    chlg_selroute_layer_create();
  case 4:
    timed = 0;
  case 5:
    course_end = msnm_course_end();
    if (course_end == 0) {
      msnm_map_base_open(msnm_work->choice,1);
      msnm_map_plates_draw((char)msnm_work->choice,msnm_work->route_choosing);
      if (msnm_work->state != 3) {
        msn_panel_draw(msnm_work->side,&msnm_work->next_tries[msnm_work->choice],&msnm_work->next_settings[msnm_work->choice],1,2);
        chlg_route_time_draw(timed);
        row = msnm_work->row + 1;
        chlg_node_hiscore_draw(chlg_map_node_get(msnm_work->column + msnm_work->choice,row),0);
      }
    }
    break;
  case 7:
    chlg_coin_get_text_draw(msnm_work->reward);
    break;
  case 9:
    msnm_quit_confirm_draw(&msnm_work->quit);
    break;
  }
  sprite_colors_reset();
}

void msnm_prize_start(void)
{
  switch (msnm_work->status) {
  case 2:
    prize_anim_start(1);
    break;
  case 3:
    prize_anim_start(2);
    break;
  }
  sprite_colors_reset();
  return;
}

void msnm_join_offer(int side)
{
  msnm_work->join_side = side;
  msnm_work->result_kind = 1;
  msnm_work->state = 0;
  msnm_work->choice = 0;
}

int msnm_join_select(void)
{
  int r;
  int choice;

  r = 0;
  switch(msnm_work->state) {
  case 0:
    if (mode_timer > 2) break;
    msnm_menu_timers_set(600);
    msnm_work->state = 1;
    round_intro_free();
    winner_mes_end();
    item_award_mes_reset();
    mode_sel_open();
    break;
  case 1:
    choice = -1;
    if (msnm_menu_input_check(msnm_work->join_side,0x20) != 0) {
      choice = 0;
    }
    else if (msnm_menu_input_check(msnm_work->join_side,0x10) != 0) {
      choice = 1;
    }
    if (choice != -1) {
      if (msnm_work->choice != choice) {
        sound_play_id(0x3af,0);
      }
      msnm_work->choice = choice;
    }
    if (msnm_menu_input_check(msnm_work->join_side,0xf) != 0 || msnm_work->menu_timer < 1) {
      msnm_work->menu_timer = 0x5a;
      sound_play_id(0x3ae,0);
      msnm_work->state = 2;
    }
    break;
  case 2:
    if (msnm_work->menu_timer < 1) {
      if (msnm_work->choice == 1) {
        msnm_start(msnm_work->join_side);
        msnm_resume();
      }
      else {
        msnm_end();
        msnm_course_reset();
        msnm_side_keep(msnm_work->join_side);
      }
      msnm_work->state = 3;
    }
    break;
  case 3:
  default:
    r = msnm_work->choice == 0;
    r = 2 - r;
    mode_sel_close();
    break;
  }
  msnm_menu_timers_step();
  return r;
}

void msnm_result_start(void)
{
  int row;
  int i;

  msnm_work->result_kind = 2;
  msnm_work->state = 0;
  msnm_work->choice = 0;
  msnm_quit_rec_clear(&msnm_work->quit);
  msnm_menu_timers_set(300);
  if (msnm_course_end() == 0) {
    row = msnm_work->row + 1;
    for (i = 0; i < 2; i++) {
      msnm_try_pick(row,i + msnm_work->column,
                   &msnm_work->next_tries[i],&msnm_work->next_settings[i]);
    }
  }
  chlg_bars_reset();
  return;
}

int msnm_result_step(void)
{
  int result_state;
  int choice;
  int done;

  done = 0;
  chlg_bars_step();
  switch(msnm_work->state) {
  case 0:
    if (msnm_menu_input_check(msnm_work->side,0x8000) != 0) {
      if (msnm_work->menu_timer > 0x3c) {
        msnm_work->menu_timer = 0x3c;
      }
      else {
        msnm_work->menu_timer = 0;
      }
    }
    if (msnm_work->menu_timer <= 0) {
      chlg_outcome_layer_free();
      msnm_menu_timers_set(300);
      msnm_work->state = 1;
      msnm_result_camera_set();
      chlg_result_init();
      msnm_round_sound_cmds();
      sound_play_id(0x371,0);
    }
    break;
  case 1:
    chlg_result_step();
    result_state = chlg_result_state_get();
    if (msnm_menu_input_check(msnm_work->side,0x8000) != 0) {
      if (result_state <= 3) {
        chlg_result_tally_finish();
      }
      else {
        chlg_result_end();
      }
    }
    if (result_state == 5) {
      chlg_result_layers_free();
      msnm_menu_timers_set(300);
      msnm_work->state = 2;
      if (msnm_course_end() != 0) {
        msnm_course_result_store(msnm_work->row,
                     msnm_work->column,
                     msnm_work->status,
                     msnm_work->score,
                     msnm_work->grade,course_time_total);
        msnm_course_totals_compute(msnm_work->column);
        chlg_rank_layers_create(msnm_work->grade_average,1);
      }
      msnm_round_sound_cmds();
      sound_play_id(0x371,0);
    }
    break;
  case 2:
    if (msnm_menu_input_check(msnm_work->side,0x8000) != 0) {
      msnm_work->menu_timer = 0;
    }
    if (msnm_work->menu_timer <= 0) {
      chlg_rank_layers_free();
      msnm_round_sound_cmds();
      if (msnm_course_end() == 0) {
        msnm_menu_timers_set(0x4b0);
        chlg_route_preview_reset(msnm_work->next_tries,msnm_work->next_settings);
        msnm_work->state = 3;
        chlg_map_hidden_set(1);
        msnm_work->route_choosing = 1;
      }
      else if (msnm_work->reward == -1) {
        msnm_work->state = 10;
      }
      else {
        if (msnm_work->reward == 4) {
          msnm_menu_timers_set(0x4b0);
        }
        else {
          msnm_menu_timers_set(600);
        }
        msnm_work->state = 6;
      }
    }
    break;
  case 3:
    if (chlg_selroute_layer_done() != 0) {
      chlg_map_hidden_set(0);
      chlg_map_layout_update(0);
      msnm_work->state = 4;
    }
    break;
  case 4:
    choice = -1;
    if (msnm_menu_input_check(msnm_work->side,0x80) != 0) {
      choice = 0;
    }
    else {
      if (msnm_menu_input_check(msnm_work->side,0x40) != 0) {
        choice = 1;
      }
    }
    if (choice != -1) {
      if (msnm_work->choice != choice) {
        sound_play_id(0x3af,0);
      }
      msnm_work->choice = choice;
      chlg_route_preview_free(1);
      chlg_route_cursor_set(&msnm_work->next_tries[choice],&msnm_work->next_settings[choice]);
    }
    if (msnm_quit_hold_check(&msnm_work->quit) != 0) {
      msnm_quit_confirm_open(&msnm_work->quit);
      msnm_work->state = 9;
    }
    else {
      if (msnm_menu_input_check(msnm_work->side,0xf) != 0 || msnm_work->menu_timer <= 0) {
        msnm_menu_timers_set(0x5a);
        sound_play_id(0x3ae,0);
        chlg_route_preview_free(0);
        msnm_work->state = 5;
        msnm_work->route_choosing = 0;
      }
    }
    break;
  case 5:
    if (msnm_work->menu_timer < 1) {
      msnm_course_result_store(msnm_work->row,
                   msnm_work->column,
                   msnm_work->status,
                   msnm_work->score,
                   msnm_work->grade,course_time_total);
      msnm_work->row = msnm_work->row + 1;
      if (7 < msnm_work->row) {
        msnm_work->row = 7;
      }
      msnm_work->column =
           msnm_work->column + msnm_work->choice;
      if (msnm_work->status == 2) {
        card_money_add((uchar)msnm_work->side,0x32);
      }
      msnm_mission_set(&msnm_work->next_tries[msnm_work->choice],
                   &msnm_work->next_settings[msnm_work->choice]);
      msnm_work->state = 10;
    }
    break;
  case 6:
    ((struct { uint a; } *)&msnm_vram_free_total)->a = tex_vram_free_total();
    ((struct { uint a; } *)&msnm_vram_free_second)->a = tex_vram_free_second();
    msnm_disp_scenes_free();
    ((struct { uint a; } *)&msnm_vram_free_total)->a = tex_vram_free_total();
    ((struct { uint a; } *)&msnm_vram_free_second)->a = tex_vram_free_second();
    msnm_disp_scene0_load();
    chlg_coin_prize_start(msnm_work->reward);
    msnm_work->state = 7;
    break;
  case 7:
    if ((msnm_work->menu_timer < 0x169) &&
       (msnm_menu_input_check(msnm_work->side,0x8000) != 0)) {
      msnm_work->menu_timer = 0;
    }
    chlg_coin_prize_step(msnm_work->reward,msnm_work->item_given,0x3c0);
    if (msnm_work->menu_timer < 1) {
      msnm_work->state = 8;
    }
    break;
  case 8:
    chlg_coin_layers_release();
    msnm_disp_scene0_free();
    msnm_work->state = 10;
    break;
  case 9:
    msnm_work->state = msnm_quit_confirm_step(&msnm_work->quit);
    break;
  case 10:
  default:
    done = 1;
    break;
  }
  if (msnm_work->state != 9) {
    msnm_menu_timers_step();
  }
  return done;
}

void msnm_result_kind_clear(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  work->result_kind = 0;
  return;
}

bool msnm_course_end(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return 7 < work->row + 1;
}

int msnm_result_kind_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->result_kind;
}

void msnm_result_camera_set(void)
{
  camera.mode = 9; /* the stage orbit, camera_mode_funcs[9] = cam_mode_stage_orbit: the game's nine mode names stop at 8 */
  camera.pers_req = 45.0;
  camera.flags = 0x18;
  robs_flags_clear();
  return;
}

void msnm_side_tallies_clear(void)
{
  int i;
  int *tally;
  for (i = 0; i < 2; i++) {
    tally = msnm_work->input_stats[i].tally;
    tally[0] = 0;
    tally[1] = 0;
    tally[2] = 0;
    tally[3] = 0;
    tally[4] = 0;
    tally[5] = 0;
    tally[6] = 0;
  }
}

void msnm_side_trackers_clear(void)
{
  msnm_input_stats_rec *stats;
  int i;
  for (i = 0; i < 2; i++) {
    stats = &msnm_work->input_stats[i];
    stats->phase = 0;
    stats->release_found = 0;
    stats->field_0x5 = 0;
    stats->lever_dirs = 0;
    stats->lever_dir_mask = 0;
  }
}

void msnm_side_trackers_update(void)
{
  rob_rec *rob;
  rob_rec *opp;
  rob_rec *t;
  int i;
  rob = rob_get(0);
  opp = rob_get(1);
  for (i = 0; i < 2; i++) {
    if (i == 1) {
      t = rob;
      rob = opp;
      opp = t;
    }
    msnm_input_stats_track(rob,opp,&msnm_work->input_stats[i]);
  }
  msnm_input_track_stub();
}

int * msnm_side_tally_get(int side)
{
  return msnm_work->input_stats[(char)side].tally;
}

int msnm_rank_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->rank;
}

int msnm_points_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->points;
}

void msnm_ranking_name_set(char *name)
{
  mission_score_rec *dst;
  if (msnm_work->rank == -1) return;
  msnm_ranking_shift_down(msnm_work->column,msnm_work->rank);
  dst = &user_back_data.challenge.course_ranking[msnm_work->column][msnm_work->rank];
  msnm_ranking_record_fill(dst,msnm_work->points,msnm_work->time_total,name,0);
}

int msnm_rule_get(void)
{
  int present;
  int rule;
  msnm_try_type_rec *entry;

  if ((msnm_work->active == '\0') || (present = msnm_mission_present(), present == 0)) {
    rule = 0;
  }
  else {
    entry = msnm_entry_get(msnm_work->cur_try.type);
    rule = entry->rule;
  }
  return rule;
}

int msnm_rule10_cpu_check(char side)
{
  int rule;
  int r;

  rule = msnm_rule_get();
  if (!((rule != 10) || (r = 1, (uint)msnm_work->side == (int)side))) return r;
  r = 0;
  return r;
}

int msnm_rule11_cpu_check(char side)
{
  int rule;
  int r;

  rule = msnm_rule_get();
  if (!((rule != 0xb) || (r = 1, (uint)msnm_work->side == (int)side))) return r;
  r = 0;
  return r;
}

int msnm_rule12_cpu_check(char side)
{
  int rule;
  int r;

  rule = msnm_rule_get();
  if (!((rule != 0xc) || (r = 1, (uint)msnm_work->side == (int)side))) return r;
  r = 0;
  return r;
}

msnm_rule_params_rec * msnm_rule_params_get(void)
{
  int rule;

  rule = msnm_rule_get();
  if (!(rule != 8)) {
    return &msnm_rule_params_all;
  }
  return &msnm_rule_params;
}

void msnm_rule1_damage_set(rob_rec *rob, int value_a, int value_b, int value_c)
{
  short a = value_a;
  short b = value_b;
  short c = value_c;
  msnm_rule_params_rec *params;

  if (msnm_rule_is(1) != 0) {
    params = msnm_rule_params_get();
    if ((a == c + 1) & (b >= 0)) {
      rob->pending_damage = params->rule1_damage;
      if (rob->pending_damage > rob->life) {
        rob->pending_damage = rob->life + -1;
      }
      rob_damage_apply(rob,0);
      hit_light_flags = hit_light_flags | 4;
      sound_request(rob,0x330);
      sound_request(rob,0x30a);
    }
  }
}

bool msnm_rule_is(int rule)
{
  int i = msnm_rule_get();
  int t = i == rule || i == 8; int u = t == 0; return u == 0;
}

void msnm_rule2_scale(rob_rec *rob, short *values, float *out_scale, short *out_value)
{
  int on;
  msnm_rule_params_rec *params;

  on = msnm_rule_is(2);
  if ((on != 0) &&
     ((params = msnm_rule_params_get(), (byte)(rob->hit_param_row - 2U) < 2 ||
      (0xe < rob->attack_damage)))) {
    *out_scale = *(float *)(values + 10) * params->rule2_scale;
    *out_value = *values + params->rule2_add;
    if (0x4000 < *out_value) {
      *out_value = 0x4000;
    }
  }
  return;
}

void msnm_rule5_damage_set(rob_rec *rob)
{
  int on;
  msnm_rule_params_rec *params;

  on = msnm_rule_is(5);
  if (on != 0) {
    params = msnm_rule_params_get();
    rob->pending_damage = params->rule5_damage;
    if (rob->pending_damage > rob->life) {
      rob->pending_damage = rob->life + -1;
    }
    rob->combo[0].hits = rob->combo[0].hits + 1;
    rob_damage_apply(rob,2);
    hit_light_flags = hit_light_flags | 4;
    sound_request(rob,0x330);
    sound_request(rob,0x30a);
  }
  return;
}

void msnm_rule6_life_add(rob_rec *rob, short amount)
{
  int rule;
  msnm_rule_params_rec *params;
  rob_rec *target;
  short *max;
  short *life;

  rule = msnm_rule_get();
  if (rule == 6) {
    params = msnm_rule_params_get();
    target = rob->opp;
    life = &target->life;
    *life = (short)(int)((float)amount * params->rule6_life_rate) + *life;
    max = &target->life_max;
    if (*life > *max) {
      *life = *max;
    }
  }
  return;
}

void msnm_combo_move_start(rob_rec *rob)
{
  msnm_combo_rec *combo;
  int d;
  uint f;
  short *h;

  combo = &msnm_work->combos[rob->side];
  combo->leave_frame = rob->recovery_end;
  if ((rob->move_flags & 0x4000) && (uchar)rob->attack_level != 12)
    (combo->flag4000_moves)++;
  if (combo->new_move && ((f = rob->move_flags) & 0x4000) && (uchar)rob->attack_level != 12) {
    if (f & 0x40) {
      d = rob->recovery_end - rob->active_end;
      d = (float)d * 0.4;
      if (d < 1) d = 1;
      rob->recovery_end = rob->active_end + d;
    }
    else {
      rob->recovery_end = rob->active_end + 1;
    }
    if (combo->count > 1) {
      h = &rob->attack_damage;
      *h = (float)*h * 0.5;
      combo->damage_sum = *h + combo->damage_sum;
    }
    combo->cut_short = 1;
  }
  else {
    combo->cut_short = 0;
  }
  combo->new_move = 0;
}

void msnm_combo_reset(int side)
{
  msnm_combo_rec *combo;
  int i;

  combo = &msnm_work->combos[side];
  combo->count = 0;
  for (i = 0; i < 100; i++) combo->moves[i] = 0;
  combo->new_move = 0;
  combo->broken = 0;
  combo->leave_frame = 0;
  combo->cut_short = 0;
  combo->damage_sum = 0;
  combo->flag4000_moves = 0;
}

uchar msnm_combo_cut_short(int side)
{
  msnm_combo_rec *combo = &msnm_work->combos[side];
  return combo->cut_short;
}

uchar msnm_combo_active(int side)
{
  msnm_combo_rec *combo = &msnm_work->combos[side];
  return combo->cut_short;
}

uchar msnm_combo_broken(int side)
{
  msnm_combo_rec *combo = &msnm_work->combos[side];
  return combo->broken;
}

int msnm_combo_count(int side)
{
  msnm_combo_rec *combo = &msnm_work->combos[side];
  return combo->count;
}

int msnm_combo_d0_get(int side)
{
  int o = side * sizeof(msnm_combo_rec);
  return ((msnm_combo_rec *)(char *)(o + (int)msnm_work + offsetof(msnm_work_rec, combos)))->leave_frame;
}

void msnm_combo_d0_set(int side, int frame)
{
  char *p = (char *)msnm_work + side * sizeof(msnm_combo_rec);
  ((msnm_combo_rec *)(p + offsetof(msnm_work_rec, combos)))->leave_frame = frame;
}

int msnm_combo_d4_get(int side)
{
  int o = side * sizeof(msnm_combo_rec);
  return ((msnm_combo_rec *)(char *)(o + (int)msnm_work + offsetof(msnm_work_rec, combos)))->damage_sum;
}

void msnm_combo_d4_add(int side, int damage)
{
  int o = side * sizeof(msnm_combo_rec);
  uchar *p = (uchar *)msnm_work + o;
  ((msnm_combo_rec *)(p + offsetof(msnm_work_rec, combos)))->damage_sum += damage;
}

int msnm_combo_d8_get(int side)
{
  int o = side * sizeof(msnm_combo_rec);
  return ((msnm_combo_rec *)(char *)(o + (int)msnm_work + offsetof(msnm_work_rec, combos)))->flag4000_moves;
}

void msnm_combo_move_add(int side, uint code)
{
  msnm_combo_rec *s;
  int ok;
  int j;
  int k;
  int n;
  char c;

  s = &msnm_work->combos[side];
  if (s->broken == 0 && ((c = (char)((int)code >> 0x18)) == 0 | c == 3)) {
    ok = 1;
    n = s->count;
    s->moves[n] = (ushort)code;
    n++;
    s->count = n;
    if (n > 99) {
      s->count = 0;
      s->new_move = 0;
      s->broken = 1;
      ok = 0;
    }
    for (j = 0; j < s->count - 1; j++) {
      for (k = j + 1; k < s->count; k++) {
        if (s->moves[j] == s->moves[k]) {
          s->broken = 1;
          ok = 0;
          break;
        }
      }
      if (!ok) break;
    }
  }
  else {
    ok = 0;
  }
  s->new_move = ok;
}

int msnm_rule9_combo_check(rob_rec *rob)
{
  int r;

  r = msnm_rule_get();
  if (((r == 9) && (r = msnm_combo_active((uint)rob->side), r != 0)) &&
     (r = msnm_combo_count((uint)rob->side), 1 < r)) {
    return 1;
  }
  return 0;
}

void msnm_work_reset(void)
{
  msnm_work->active = 0;
  msnm_work->saved_side = 0xff;
  msnm_work->loaded = 0;
  msnm_work->scene0_loaded = 0;
  msnm_work->saved_active = 0;
  msnm_work->saved_stage = 0;
  msnm_work->saved_opponent = 0;
  msnm_work->result_kind = 0;
  msnm_quit_rec_clear(&msnm_work->quit);
  return;
}

void msnm_result_judge(void)
{
  rob_rec *rob;

  rob = rob_get((uint)msnm_work->side);
  champion_side = msnm_work->side;
  if (msnm_work->status == 1) {
    msnm_status_set(rob->side,3);
  }
  msnm_work->grade = msnm_result_grade();
  msnm_work->score = msnm_score_calc(msnm_work->grade);
}

uint msnm_score_calc(int n)
{
  msnm_progress_rec *progress = &msnm_work->progress;
  int *conds = &msnm_work->cur_try.score_kind;
  msnm_score_table_rec *table;
  msnm_score_rates_rec *e;
  msnm_score_rec *s;
  int i;
  uint v;
  int d;

  table = msnm_score_table_get(msnm_work->row, msnm_work->column);
  e = 0;
  if (n > 1) e = &table->rates[n - 2];
  s = &msnm_work->result.score_parts;
  s->field_0x2c = 0;
  s->field_0x3c = 0;
  s->field_0x44 = 0;
  s->field_0x4c = 0;
  s->cnt_num = progress->cnt_num;
  s->field_0x28 = 0;
  s->base = (int)table->base[n];
  s->win_time = 0;
  s->time_rate = 0;
  s->time_score = 0;
  if (e) {
    for (i = 0; i < 2; i++) {
      if (msnm_work->round_results[i] == 1) {
        s->win_time += msnm_work->round_times[i];
        s->time_rate = (int)e->time_rate;
      }
    }
  }
  s->time_score = (int)((float)s->win_time / 60.0f * (float)s->time_rate);
  s->bonus = 0;
  if (e) {
    s->bonus = (int)((float)s->bonus + (float)msnm_work->wins_ko * e->ko_bonus);
    s->bonus = (int)((float)s->bonus + (float)msnm_work->wins_perfect * e->perfect_bonus);
    for (i = 0; i < 2; i++) {
      if (msnm_work->round_results[i] == 1) {
        if (msnm_work->round_times[i] % 60 == 0) {
          s->bonus = (int)((float)s->bonus + e->even_time_bonus);
        }
        if (msnm_work->round_lives[i] <= 10) {
          s->bonus = (int)((float)s->bonus + e->low_life_bonus);
        }
      }
    }
    if (e) {
      if (*conds == 0) {
        d = progress->cnt_num - conds[1];
        s->bonus = (int)((float)s->bonus + (float)d * e->count_kind_rate);
      } else if (*conds == 1) {
        s->bonus = (int)((float)s->win_time / 60.0f * e->time_kind_rate) + s->bonus;
      } else if ((uint)(*conds - 2) < 2) {
        s->bonus = s->bonus + progress->score;
      }
    }
  }
  v = s->base + s->time_score + s->bonus;
  s->total = v;
  s->total_step = (int)v / 180 + 1;
  return v;
}

msnm_score_table_rec * msnm_score_table_get(uint row, int column)
{
  int i;
  uint t;
  i = 0;
  if (2 < row) {
    t = row - 3;
    if (t < 3) i = 1;
    else {
      t = row - 6;
      if (t < 2) i = 2;
    }
  }
  return &msnm_score_tables[i];
}

int msnm_course_grade_average(int unused)
{
  int i;
  int sum;

  sum = 0;
  for (i = 0; i < 8; i++) {
    sum += msnm_work->course.grades[i];
  }
  sum = sum / 8; return sum;
}

int msnm_reward_roll(int grade, int column)
{
  uint n;
  float lim;
  uint r;

  n = net_ticket_get_value();
  if (((struct { char pad[16]; int ticket; } *)&stage)->ticket == 0 && card_record_get(msnm_work->side)->in_use == 0) {
    return 0xffffffff;
  }
  lim = (&msnm_item_chances[0][0])[column * 5 + grade];
  lim = lim / 100.0 * (float)n;
  if (net_free_play_is_on()) {
    card_money_add(msnm_work->side,50);
  }
  else if (msnm_random_unit() <= lim) {
    if (msnm_item_award(&r)) {
      msnm_work->item_given = 1;
    }
    return r;
  }
  else {
    int v = msnm_money_rewards[column][grade];
    card_money_add(msnm_work->side,v);
  }
  return 0;
}

bool msnm_item_award(uint *count)
{
  card_image_rec *card;
  int next;
  uint ticket;
  int chara;
  bool given;
  int list_len;
  card = card_image_get((uint) (msnm_work->side));
  ticket = card_hdr_ftf_ticket_get(&card->header);
  next = ticket + 1;
  ticket = next & 0xff;
  *count = ticket;
  given = 0;
  if (3 < ticket)
  {
    chara = (int) (*((char *) ((&player_chara.side0) + (msnm_work->side))));
    if (msnm_get_item_pick(msnm_char_items[chara], 2) != 0)
    {
      given = 1;
    }
    if (!given)
    {
      list_len = msnm_char_item_counts[chara];
      if (msnm_get_item_pick(msnm_char_item_lists[chara], list_len) != 0)
      {
        given = 1;
      }
      if (!given)
      {
        card_money_add((uint) (msnm_work->side), 1000);
      }
    }
    ticket = 0;
  }
  card_hdr_ftf_ticket_set(&card->header, ticket);
  return given;
}

bool msnm_get_item_pick(int *items, int item_max)
{
  int i;
  int n;
  int k;
  uint list[30];

  if (item_max > 30) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmission/src/msnm.c",0xb93,"assertion `%s\' failed.\n",
             "item_max <= MSNM_GET_ITEM_MAX");
  }
  for (i = 0; i < 30; i++) {
    list[i] = 0xffffffff;
  }
  n = 0;
  for (i = 0; i < item_max; i++) {
    if (item_has_unchecked(msnm_work->side,items[i]) == 0) {
      list[n] = items[i];
      n++;
    }
  }
  if (n == 0) {
    return 0;
  }
  k = (int)(msnm_random_unit() * (float)n);
  msnm_work->item = list[k];
  item_give_carded(msnm_work->side,msnm_work->item);
  return 1;
}

void msnm_try_record_reset(msnm_try_rec *mission_try)
{
  msnm_try_rec t;
  t = msnm_try_default;
  *mission_try = t;
}

void msnm_progress_clear(msnm_progress_rec *progress, msnm_try_disp_rec *disp)
{
  msnm_progress_match_clear(progress,disp);
  msnm_progress_round_clear(progress,disp);
  return;
}

void msnm_progress_match_clear(msnm_progress_rec *progress, msnm_try_disp_rec *disp)
{
  int i;

  progress->cnt_num = 0;
  progress->win = 0;
  progress->win_streak = 0;
  progress->score = 0;
  progress->flag = 0;
  progress->count = 0;
  i = 0;
  do {
    disp->moves[i] = 0;
    i = i + 1;
  } while (i < 10);
  return;
}

void msnm_progress_round_clear(msnm_progress_rec *progress, msnm_try_disp_rec *disp)
{
  progress->mode = 0;
  progress->round_flag = 0;
  progress->round_cnt = 0;
  progress->nuke_dir_flags = 0;
  progress->field_0x28 = 0;
  return;
}

void msnm_course_results_reset(msnm_course_results_rec *course)
{
  int i;

  for (i = 0; i < 8; i++) {
    course->columns[i] = -1;
    course->kinds[i] = -1;
    course->scores[i] = 0;
    course->grades[i] = -1;
    course->times[i] = 0;
  }
}

float msnm_random_unit(void)
{
  return msnm_last_random = (float)lcg_next_unit();
}

uint msnm_mask_bit_pick(uint mask)
{
  int tbl[32];
  int tbl_num;
  int i;

  tbl_num = 0;
  for (i = 0; i < 32; i++) {
    if (mask & (1 << i)) {
      tbl[tbl_num] = i;
      tbl_num++;
    }
  }
  if (!(tbl_num > 0 && tbl_num < 32)) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmission/src/msnm.c",0xc15,"assertion `%s\' failed.\n","tbl_num > 0 && tbl_num < 32");
  }
  return tbl[(int)((float)tbl_num * msnm_random_unit())];
}

void msnm_course_reset(void)
{
  rank_round = 0;
  course_time_total = 0;
  cpu_opponent_pick(0);
  return;
}

void msnm_course_result_store(int step, int column, int status, uint score, int grade, int time)
{
  int i;
  msnm_progress_rec *progress;
  mission_score_rec *e;
  msnm_course_results_rec *s;

  progress = &msnm_work->progress;
  s = &msnm_work->course;
  s->columns[step] = column;
  s->kinds[step] = msnm_status_kinds[status];
  s->scores[step] = score;
  s->grades[step] = grade;
  s->times[step] = time;
  i = chlg_map_node_index(step,column);
  e = &user_back_data.challenge.chlg_map_hiscores[i];
  if (e->score < score) {
    msnm_ranking_record_fill(e,score,time,msn_noname_text,progress->cnt_num);
  }
}

void msnm_course_totals_compute(int column)
{
  msnm_course_results_rec *course;
  msnm_work_rec *q;
  msnm_work_rec *r;
  msnm_work_rec *s;
  int i;

  course = &msnm_work->course;
  msnm_work->points = 0;
  for (i = 0; i < 8; i++) {
    msnm_work->points += course->scores[i];
  }
  s = msnm_work;
  s->time_total = 0;
  for (i = 0; i < 8; i++) {
    msnm_work->time_total += course->times[i];
  }
  for (i = 0; i < 3; i++) {
    if ((user_back_data.challenge.course_ranking[0] + i + column * 3)->score < (uint)msnm_work->points) break;
  }
  msnm_work->rank = i;
  if (i == 3) {
    msnm_work->rank = -1;
  }
  q = msnm_work;
  q->grade_average = msnm_course_grade_average(q->points);
  r = msnm_work;
  r->reward = msnm_reward_roll(r->grade_average,r->column);
}

void msnm_ranking_shift_down(int column, int place)
{
  mission_score_rec *pa;
  mission_score_rec *pb;
  mission_score_rec *pc;
  pa = &user_back_data.challenge.course_ranking[column][0];
  pb = &user_back_data.challenge.course_ranking[column][1];
  pc = &user_back_data.challenge.course_ranking[column][2];
  if (place < 2) {
    *pc = *pb;
  }
  if (place < 1) {
    *pb = *pa;
  }
}

void msnm_ranking_record_fill(mission_score_rec *record, uint score, int time, char *default_name, int extra)
{
  card_image_rec *card;
  int i;

  card = card_image_get((uint)msnm_work->side);
  if (card_body_name_empty(&card->body) == 0) {
    for (i = 0; i < 0x14; i++) {
      record->name[i] = card->body.name[i];
    }
    record->name[i] = 0;
  }
  else {
    strcpy(record->name,default_name);
  }
  record->chara = *(uchar *)(&player_chara.side0 + msnm_work->side);
  record->time = time;
  record->score = score;
  record->extra = extra;
}

void msnm_try_pick(int level, int slot, msnm_try_rec *mission_try, msnm_try_setting_rec *setting)
{
  msnm_try_group_rec *g;
  float sum;
  float r;
  int i;
  char c;

  if ((level > 7) | (slot > level)) return;
  g = msnm_try_groups[level] + slot;
  sum = 0.0;
  for (i = 0; i < 3; i++) {
    if (g->tries[i].type != -1) {
      sum += g->weights[i];
    }
  }
  if (sum > 0.0) {
    r = msnm_random_unit() * sum;
    sum = 0.0;
    for (i = 0; i < 3; i++) {
      if (g->tries[i].type != -1) {
        sum += g->weights[i];
        if (r <= sum) break;
      }
    }
    if (i == 3) i = 0;
    c = (&player_chara.side0)[(uchar)msnm_work->side];
    *mission_try = g->tries[i];
    setting->stage = msnm_try_stage_pick(mission_try);
    setting->opponent = msnm_try_opponent_pick(mission_try,c);
    setting->variant = msnm_try_variant_pick(mission_try,c);
    if (mission_try->type == 0x3e) {
      msnm_try_hyper_pick(setting,mission_try->param,c);
    }
  }
}

uint msnm_try_opponent_pick(msnm_try_rec *mission_try, int chara)
{
  char c = (char)chara;
  uint pick;
  uint *masks;
  msnm_combo_variant_rec *variant;
  uint mask;
  int type;
  int ok;

  pick = mission_try->opponent;
  if (pick == 0xffffffff) {
    type = mission_try->type;
    masks = (uint *)msnm_entry_masks_get(type);
    mask = masks[1];
    if (type == 0x35) {
      variant = msnm_combo_variant_get(mission_try->param,c);
      mask = variant->opponent_mask;
    }
    pick = msnm_mask_bit_pick(mask);
    ok = pick > 15; ok = ok == 0; return pick & -ok;
  }
  return pick;
}

uint msnm_try_stage_pick(msnm_try_rec *mission_try)
{
  uint pick;
  uint *masks;
  int t;

  pick = mission_try->stage;
  if (pick == 0xffffffff) {
    masks = (uint *)msnm_entry_masks_get(mission_try->type);
    pick = msnm_mask_bit_pick(*masks);
    t = pick > 0x17;
    t = t == 0;
    pick = pick & -t;
  }
  return pick;
}

int msnm_try_variant_pick(msnm_try_rec *mission_try, char chara)
{
  short *p;
  int i;
  int t;
  int variant [1];

  t = mission_try->type;
  variant[0] = 0;
  if (t == 0x36) {
    variant[0] = msnm_random_unit() * 2.0f;
  }
  else if (t == 0x37) {
    p = (short *)msnm_target_moves_get(chara);
    if (p != 0) {
      for (i = 0; i < 10; i++) {
        if (p[i] == 0) break;
      }
      variant[0] = (float)i * msnm_random_unit();
      msnm_variant_debug_stub(variant,10,msnm_variant_debug_value);
    }
  }
  return variant[0];
}

void msnm_variant_debug_stub(int *variant, int max, int value)
{
  return;
}

void msnm_input_track_stub(void)
{
  return;
}

int msnm_result_grade(void)
{
  int *conds;
  int i;
  int s;
  int grade;

  conds = &msnm_work->cur_try.score_kind;
  s = 0;
  for (i = 0; i < 2; i++) {
    s += msnm_work->round_times[i];
  }
  grade = 0;
  if (msnm_work->status == 2) {
    grade = 2;
    if (msnm_grade_cond_check(conds[1],s) != 0) grade = 4;
    else if (msnm_grade_cond_check(conds[2],s) != 0) grade = 3;
  }
  else if (msnm_grade_cond_check(conds[3],s) != 0) grade = 1;
  return grade;
}

int msnm_grade_cond_check(int cond, int time_sum)
{
  msnm_work_rec *work = msnm_work;
  int other = work->side == 0;
  msnm_progress_rec *progress = &work->progress;
  int i;

  switch(cond) {
  case -1:
    if (msnm_round_wins_total() == 2) return 1;
    break;
  case -2:
    if (msnm_round_wins_total() == 1) return 1;
    break;
  case -3:
    for (i = 0; i < 2; i++) {
      if (msnm_work->round_opp_lives[i] <= (int)msnm_match_energy_get((char)other) / 2) {
        return 1;
      }
    }
    break;
  case -4:
    if (msnm_round_wins_total() != 2) {
      break;
    }
    for (i = 0; i < 2; i++) {
      if (msnm_work->round_lives[i] <= (int)msnm_match_energy_get((char)msnm_work->side) / 2) break;
    }
    if (i == 2) return 1;
    break;
  case -5:
    if (msnm_round_wins_total() != 2) {
      break;
    }
    if (time_sum > 0x833) return 1;
    break;
  default:
    if (progress->cnt_num >= cond) return 1;
    break;
  }
  return 0;
}

int msnm_round_wins_total(void)
{
  return msnm_work->wins_ko + msnm_work->wins_perfect +
         msnm_work->wins_ring_out + msnm_work->wins_time_up;
}

int msnm_menu_input_check(char side, uint mask)
{
  int move;
  int wait;
  if ((mask & 0xf0) != 0) {
    move = 1;
    wait = msnm_work->input_wait_move;
  }
  else {
    move = 0;
    wait = msnm_work->input_wait;
  }
  if (((mask & player_input_get(side)->pressed) != 0) & (wait < 1)) {
    if (!move) {
      msnm_work->input_wait = 0x1e;
    }
    return 1;
  }
  return 0;
}

void msnm_menu_timers_set(int frames)
{
  msnm_work->input_wait_move = 0x1e;
  msnm_work->input_wait = 0x1e;
  msnm_work->menu_timer = frames;
  return;
}

void msnm_menu_timers_step(void)
{
  msnm_work->input_wait_move = msnm_work->input_wait_move + -1;
  msnm_work->input_wait = msnm_work->input_wait + -1;
  msnm_work->menu_timer = msnm_work->menu_timer + -1;
  if (msnm_work->input_wait_move < 0) {
    msnm_work->input_wait_move = 0;
  }
  if (msnm_work->input_wait < 0) {
    msnm_work->input_wait = 0;
  }
  if (msnm_work->menu_timer < 1) {
    msnm_work->menu_timer = 0;
  }
  return;
}

void msnm_quit_rec_clear(msnm_quit_rec *quit)
{
  quit->hold = 0;
  quit->chosen = 0;
  quit->open = 0;
  return;
}

void msnm_quit_confirm_open(msnm_quit_rec *quit)
{
  quit->choice = 1;
  quit->timer = 0x4b0;
  quit->open = 1;
  return;
}

int msnm_quit_hold_check(msnm_quit_rec *quit)
{
  player_input_rec *input;

  if (quit->open != '\0') return 0;
  input = player_input_get((uint)msnm_work->side);
  if ((input->now & 0x8000) != 0) {
    if (++quit->hold > 0xb4) return 1;
  }
  else {
    quit->hold = 0;
  }
  return 0;
}

int msnm_quit_confirm_step(msnm_quit_rec *quit)
{
  int choice;
  int state;

  state = 9;
  choice = -1;
  if (msnm_menu_input_check((int)(char)msnm_work->side,0x20) != 0) {
    choice = 0;
  }
  else if (msnm_menu_input_check((int)(char)msnm_work->side,0x10) != 0) {
    choice = 1;
  }
  if (choice != -1) {
    if (quit->choice != choice) {
      sound_play_id(0x3af,0);
    }
    quit->choice = choice;
  }
  if (msnm_menu_input_check((int)(char)msnm_work->side,0xf) != 0) {
    if (quit->choice == 1) {
      state = 4;
    }
    else {
      if (msnm_work->status == 2) {
        card_money_add((uint)msnm_work->side,0x32);
      }
      sound_play_id(0x3ae,0);
      quit->chosen = 1;
      state = 10;
    }
  }
  else if (quit->timer < 1) {
    state = 4;
  }
  quit->timer = quit->timer + -1;
  return state;
}

void msnm_round_sound_cmds(void)
{
  voice_slots_off();
  sound_slots_1_3_5_off();
  return;
}

void msnm_disp_frame_stub(void)
{
  return;
}

int msnm_bits9_count(uint mask)
{
  int n;
  int i;
  n = 0;
  for (i = 0; i < 9; i++) {
    int t = (mask & (1 << i)) == 0;
    n += t == 0;
  }
  return n;
}

bool msnm_input_stats_track(rob_rec *rob, rob_rec *opp, msnm_input_stats_rec *stats)
{
  bool done;

  done = 0;
  switch (stats->phase) {
  case 0:
    stats->release_found = 0;
    stats->field_0x5 = 0;
    stats->lever_dirs = 0;
    stats->lever_dir_mask = 0;
    if ((opp->flags28 & 0x20000000) != 0) {
      if (msnm_input_c0_release_find(rob) != -1) {
        stats->release_found = 1;
      }
      stats->lever_dirs = msnm_lever_dirs_used_count(rob,&stats->lever_dir_mask);
      stats->phase = stats->phase + 1;
      if ((((rob->old_cur->flags28 & 0x40000000) == 0) ||
          ((rob->old_cur->state & 4) != 0)) &&
         ((msnm_input_hist_held_get((int)(char)rob->side,0) & 7) == 1)) {
        stats->field_0x5 = 1;
      }
    }
    break;
  case 1:
    if (((opp->flags28 & 0x20000000) != 0) &&
       ((rob->flags & 0x30000000) != 0)) {
      stats->lever_dirs = msnm_lever_dirs_used_count(rob,&stats->lever_dir_mask);
    }
    if ((opp->flags28 & 0x20000000) == 0) {
      if ((rob->flags28 & 0x8000000) != 0) {
        done = 1;
        msnm_input_stats_add(stats);
      }
      stats->phase = 0;
    }
    break;
  }
  return done;
}

void msnm_cmd_list_setup(int type, int chara)
{
  char c = (char)chara;
  msnm_cmd_entry z = {0, 0};
  msnm_try_disp_rec *o;
  int i;
  int j;
  int variant;
  int k;
  short *s;
  msnm_cmd_entry **lst;
  msnm_cmd_entry *src;
  msnm_combo_variant_rec *cnt;
  int n;
  int m;
  int *q;
  msnm_try_setting_rec *setting;
  ushort v;
  msnm_cmd_entry *row;

  o = msnm_work_b4_get();
  o->alias_table = 0;
  for (i = 0; i < 6; i++) {
    row = o->rows[i];
    for (j = 0; j < 21; j++) {
      row[j] = z;
    }
    o->row_kinds[i] = 0;
  }
  variant = o->variant;
  if (type == 0x36) {
    s = msnm_combo3_moves[c];
    if (s != 0) {
      for (i = 0; i < 3; i++) {
        if (s[variant * 3 + i] == 0) {
          return;
        }
        o->rows[i][0].mask = 0x81;
        o->rows[i][0].move = s[variant * 3 + i];
      }
    }
  }
  else if (type == 0x35) {
    k = msnm_work->cur_try.param;
    lst = (msnm_cmd_entry **)msnm_combo_lists_get(k,c);
    o->alias_table = msnm_move_alias_tables[c];
    if (lst != 0) {
      for (i = 0; i < 6; i++) {
        src = lst[i];
        if (src == 0) break;
        row = o->rows[i];
        for (j = 0; j < 21; j++) {
          row[j] = src[j];
          if (src[j].mask == z.mask && src[j].move == z.move) break;
        }
      }
      cnt = msnm_combo_variant_get(k,c);
      for (i = 0; i < 6; i++) {
        o->row_kinds[i] = cnt->row_kinds[i];
      }
    }
  }
  else if (type == 0x3e) {
    m = msnm_work->cur_try.param;
    o->alias_table = msnm_move_alias_tables[c];
    n = msnm_hyper_counts[m];
    row = o->rows[0];
    for (i = 0; i < n; i++) {
      setting = &msnm_work->setting;
      q = (int *)(i * 4 + (int)setting);
      v = msnm_hyper_mot_get(c,q[3],q[7],m);
      row[i].mask = 0x81;
      row[i].move = v;
    }
  }
  else if (type == 0x3d) {
    o->pattern = msnm_pattern_recs[c];
  }
}

void msnm_cmd_rows_progress_clear(void)
{
  msnm_try_disp_rec *disp;
  int i;

  disp = msnm_work_b4_get();
  i = 0;
  do {
    disp->row_steps[i] = 0;
    i = i + 1;
  } while (i < 6);
  return;
}

void msnm_target_move_setup(int type, char chara)
{
  typedef struct { short move; short partner; } pair;
  msnm_try_disp_rec *o;
  int i;
  uchar *p;

  if (type == 0x37) {
    o = msnm_work_b4_get();
    for (i = 0; i < 3; i++) {
      o->target_moves[i] = 0;
    }
  }
  if (type == 0x37 && (p = msnm_target_moves[chara]) != 0) {
    o->target_moves[0] = *(short *)(p + o->variant * 2);
    for (i = 0; i < 0x10; i++) {
      if (((pair *)&msnm_target_move_pairs[0][0])[i].move == 0 && ((pair *)&msnm_target_move_pairs[0][0])[i].partner == 0) return;
      if ((ushort)o->target_moves[0] == (ushort)((pair *)&msnm_target_move_pairs[0][0])[i].move) {
        o->target_moves[1] = ((pair *)&msnm_target_move_pairs[0][0])[i].partner;
        return;
      }
    }
  }
}

uchar * msnm_combo_lists_get(int variant, char chara)
{
  if (variant == 0) {
    return msnm_combo_lists_a[chara];
  }
  if (variant == 1) {
    return msnm_combo_lists_b[chara];
  }
  if (variant == 2) {
    return msnm_combo_lists_c[chara];
  }
  return (uchar *)0x0;
}

msnm_combo_variant_rec * msnm_combo_variant_get(int variant, char chara)
{
  int i = chara;
  return &msnm_combo_variants[(i + i * 2) + variant];
}

uchar * msnm_target_moves_get(int chara)
{
  return msnm_target_moves[(char)chara];
}
