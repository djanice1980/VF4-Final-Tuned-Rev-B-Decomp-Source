/*
 * vf4lib/libbase/src/mode.c
 * Game mode requests and the per-frame mode dispatch.
 * Reconstructed C: every function here matches the game byte for byte
 * (python3 -I tools/match/match.py checks the whole tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar demo_stage_cycle; /* 0x0c162ae4 */
extern uchar demo_charas; /* 0x0c162af0 */
extern uchar demo_charas_random; /* 0x0c163178 */
extern uchar demo_stages_random; /* 0x0c163188 */
extern uchar cpu_order; /* 0x0c163196 */
extern char rep_repeat_mode; /* 0x0c243078 */
extern uchar game_session_inited; /* 0x0c243122 */
extern uint title_day_of_month; /* 0x0c243128 */
extern uint credit_blink_count; /* 0x0c243168 */
extern char demo_type; /* 0x0c28c784 */
extern int game_frame_count; /* 0x0c292540 */
extern uint mode_frame_count; /* 0x0c29254c */
extern int mode_timer; /* 0x0c292550 */
extern int intrude_timer; /* 0x0c292554 */
extern int round_time; /* 0x0c292558 */
extern uint match_round_time; /* 0x0c29255c */
extern int continue_timer; /* 0x0c292570 */
extern float round_time_float; /* 0x0c292574 */
extern float mode_timer_float; /* 0x0c292578 */
extern uint game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern char round_end_kind; /* 0x0c2925cd */
extern char rounds_to_win; /* 0x0c2925ce */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern char round_number; /* 0x0c2925d8 */
extern char demo_kind; /* 0x0c2925db */
extern int win_streak; /* 0x0c2925dc */
extern short match_energy; /* 0x0c2925e6 */
extern short judge_time; /* 0x0c2925e8 */
extern uchar cpu_next_chara; /* 0x0c2925ea */
extern uchar cpu_next_stage; /* 0x0c2925eb */
extern uint course_time_total; /* 0x0c2925f8 */
extern uint p1_play_frames_max; /* 0x0c292614 */
extern uint p1_play_frames_min; /* 0x0c292618 */
extern uint vs_round_time; /* 0x0c29261c */
extern uint p1_round_time; /* 0x0c292620 */
extern uint vs_play_frames; /* 0x0c292624 */
extern float mode_timer_rate; /* 0x0c292678 */
extern uint match_stage; /* 0x0c2926dc */
extern uint stage_intro_frames; /* 0x0c292724 */
extern uint round_intro_frames; /* 0x0c292728 */
extern uint replay_frames; /* 0x0c29272c */
extern uint winner_frames; /* 0x0c292730 */
extern uint appose_frames; /* 0x0c292734 */
extern uint mask_load_frames; /* 0x0c292738 */
extern uint judge_frames; /* 0x0c29273c */
extern short rep_ring_write; /* 0x0c360248 */
extern short rep_mark; /* 0x0c360250 */
extern char demo_stage_no; /* 0x0c370c87 */
extern char demo_chara_a; /* 0x0c370c88 */
extern char demo_chara_b; /* 0x0c370c89 */
extern coin_added_sides coin_added; /* 0x0c24312c */
extern coin_count_prev_sides coin_count_prev; /* 0x0c243130 */
extern credit_count_prev_sides credit_count_prev; /* 0x0c243134 */
extern rounds_won_sides rounds_won; /* 0x0c2925d0 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern side_costume_sides side_costume; /* 0x0c2926e8 */
extern credit_copy_rec credit_copy; /* 0x0c370b80 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

/* the game's assert: file, line, format and the expression text */
#define assert(e) ((e) ? (void)0 : fatal_at(__FILE__, __LINE__, "assertion `%s' failed.\n", #e))
#define MODE_MAIN_MAX 8

void mode_req_main(int req)
{
#line 451 "/p71/prj71p/vf4e2/vf4lib/libbase/src/mode.c"
    assert((req >= 0)&&(req<MODE_MAIN_MAX));
    main_mode_req = req;
}

void mode_req_sub(int req)
{
    sub_mode_req = req;
}

int mode_get_main(void)
{
    return main_mode_no;
}

int mode_get_sub(void)
{
    return sub_mode_no;
}

void mode_exec(void)
{
    mode_handler main_func = mode_data[main_mode_no].main_func.func;

    mode_data[main_mode_no].sub_func[sub_mode_no].func();
    main_func();
}

void entry_flags_reset(void *flags)
{
  memcpy(flags,entry_flags_default,3);
  return;
}

void mode_warning(void)
{
  return;
}

void australia_notice_draw(void)
{
  int country;
  
  country = system_param_country_get();
  if (country == 4) {
    sprite_font_text_put(&ascii_font,"AUSTRALIAN VERSION",16.0,36.0,0,0.008);
  }
  return;
}

void mode_selector(void)
{
  return;
}

void sel_init(void)
{
  uint *sound_state;
  
  demo_cleanup();
  mission_clear();
  sound_state = (uint *)sound_state_get();
  task_tree_stop();
  loads_cancel_all();
  bg_color_set(bg_color_presets[0]);
  bgm_sound_off();
  voice_slots_off();
  sound_slots_1_3_5_off();
  stage_se_slot_off();
  stage_sound_stop_hook_run();
  other_bgm_load(0,1);
  *sound_state = *sound_state | 0x10000;
  aet_objs_stop_by_set(0x1b);
  mode_req_sub(1);
  vs_play_frames = 0;
  sel_state_reset();
  return;
}

void stage_tasks_start(void)
{
  task_start(&task_stage);
  task_start(&task_fog);
  task_start(&task_effect);
  task_start(&task_rendtx);
  return;
}

void rob_tasks_start(void)
{
  task_start(&task_rob_ctrl);
  task_start(&task_rob_disp);
  task_start(&task_rep_save);
  return;
}

void game_tasks_start(void)
{
  stage_tasks_start();
  rob_tasks_start();
  return;
}

void game_round_disp_nop(void)
{
  return;
}

void mode_ending(void)
{
  return;
}

void mode_over(void)
{
  return;
}

void card_state_idle_reset(void)
{
  uchar *state;
  int result;
  
  state = card_service_state_get();
  result = card_service_mode_get();
  if (((result == 2) && (result = card_close_phase_get(), result == 2)) && (result = card_write_mode_busy(), result == 0)
     ) {
    card_close_phase_set(0);
    *(uint *)(state + 4) = 0xffffffff;
  }
  return;
}

void mode_intrude(void)
{
  return;
}

void mode_test(void)
{
  return;
}

void test_init(void)
{
  coin_input_hold();
  bg_plane_corner_colors(0,0,0);
  input_per_frame();
  sys_frame_end();
  test_disp();
  return;
}

void test_disp(void)
{
  return;
}

int ending_skip_requested(void)
{
  return start_skip_pressed();
}

int loads_busy(void)
{
  int busy;
  int result;
  
  result = 0;
  busy = load_pending_get();
  if ((busy != 0) || (busy = poly_queue_busy_get(), busy != 0)) {
    result = 1;
  }
  return result;
}

int return_zero_0c02a520(void)
{
  return 0;
}

int loads_busy_b(void)
{
  int busy;
  int result;
  
  result = 0;
  busy = load_pending_get();
  if ((busy != 0) || (busy = poly_queue_busy_get(), busy != 0)) {
    result = 1;
  }
  return result;
}

void tasks_mark_on_request(void)
{
  if (tasks_resume_request != '\0') {
    tasks_resume_request = '\0';
    task_tree_resume();
  }
  return;
}

void game_banks_free(void)
{
  stage_screens_free();
  objbank_free(3);
  objbank_free(4);
  objbank_free(5);
  objbank_free(6);
  objbank_free(7);
  objbank_free(8);
  objbank_free(9);
  objbank_free(10);
  objbank_free(0xb);
  objbank_free(0xc);
  objbank_free(0xd);
  objbank_free(0xe);
  objbank_free(0xf);
  objbank_free(0x10);
  objbank_free(0x11);
  objbank_free(0x12);
  objbank_free(0x13);
  objbank_free(0x14);
  rob_hand_models_clear(0);
  rob_hand_models_clear(1);
  effect_slots_free_all();
  return;
}

void loads_cancel_all(void)
{
  load_reset();
  poly_queue_reset();
  return;
}

char *cpu_order_entry(uint index)
{
  return (char *)&cpu_order + (index % 0xf) * 2;
}

void mode_init(void)
{
  mode_req_main(MODE_MAIN_WARNING);
  mode_req_sub(0);
  mode_update();
  mode_frame_count = 0;
  task_start(&task_sprite);
  task_start(&task_ev_aet);
  card_mode_init();
  return;
}

void game_load(void)
{
  stage_apply();
  user_back_data.bookkeep.load_time = user_back_data.bookkeep.load_time + mask_load_frames;
  user_back_data.bookkeep.load_runs = user_back_data.bookkeep.load_runs + 1;
  mode_req_sub(4);
  return;
}

void alt_mode_match_end(void)
{
  int result;
  rob_rec *rob;
  
  course_time_total = match_time_frames + round_time_frames + course_time_total;
  match_time_frames = 0;
  round_time_frames = 0;
  result = msnm_course_end();
  if (!(result == 0)) {
    rob = rob_get(0);
    if ((rob->flags & 4) == 0) {
      rob = rob_get(1);
    }
    game_flags = game_flags | 0x20000;
    alt_mode_clear_stats_add(rob);
  }
  else {
    result = card_close_phase_get();
    if (result == 1) {
      card_cpu_play_end();
      card_close_phase_set(2);
      result = net_is_on();
      if (result != 0) {
        net_game_end_send_cpu();
      }
    }
  }
  return;
}

int effect_skip_requested(void)
{
  if (effect_skip != OFF_ON_OFF) {
    return start_skip_pressed();
  }
  return 0;
}

void round_next(void)
{
  camera.mode = MODE_ROB;
  camera.proj.pers = 45.0;
  round_number = round_number + '\x01';
  game_flags = game_flags & 0xffefffff;
  match_time_frames = match_time_frames + round_time_frames;
  robs_start();
  camera_view_reset();
  task_resume(&task_rep_save);
  task_start(&task_rep_save);
  mode_req_sub(8);
  return;
}

void sega_init(void)
{
  tm now;
  
  get_now_tm(&now);
  title_day_of_month = now.tm_mday;
  screen_fill_white();
  jisaku_reset();
  task_tree_stop();
  mode_req_sub(1);
  mode_timer = 3;
  mode_timer_float = 3.0;
  mode_timer_rate = 1.0;
  game_flags = game_flags & 1;
  input_repeat_set(0x14,1);
  bg_color_set(bg_color_presets[4]);
  bgm_sound_off();
  dural_alt_active = 0;
  return;
}

void rank_init(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  logo_sprite_set_free();
  rank_sprite_sets_load();
  game_tasks_start();
  task_stop(&task_rep_save);
  stage_field_init();
  robs_start();
  task_start(&task_camera);
  *sound_state = *sound_state | 0x1000;
  camera.mode = REPLAY;
  camera.mode_return = 7;
  camera_shake_request(0xffffffff,0);
  camera.pers_req = 45.0;
  camera.flags = camera.flags | 0x10;
  game_flags = game_flags | 4;
  mode_timer = 0xe10;
  mode_timer_float = 3600.0;
  mode_timer_rate = 1.0;
  mode_req_sub(10);
  rank_next_page_setup();
  vp_level_set(0x1e);
  vp_round_init();
  return;
}

void sel_load(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  if ((*sound_state & 0x10000) == 0) {
    *sound_state = *sound_state | 2;
    game_banks_free();
    game_sprite_sets_free();
    rank_sprite_sets_free();
    sprite_sets_noop_d();
    logo_sprite_set_free();
    ftttl_scenes_free();
    sel_scenes_load();
    mode_timer_rate = 1.0;
    mode_req_sub(2);
    game_session_inited = 0;
  }
  return;
}

void ending_init(void)
{
  screen_fill_black();
  task_tree_stop();
  mode_req_sub(1);
  mode_timer = 2;
  mode_timer_float = 2.0;
  mode_timer_rate = 1.0;
  dural_alt_active = 0;
  return;
}

void name_entry_init(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  mode_timer_float = 1800.0;
  mode_timer = 0x708;
  mode_timer_rate = 1.0;
  name_entry_setup();
  name_entry_sprite_sets_load();
  other_bgm_load(2,0);
  *sound_state = *sound_state | 8;
  mode_req_sub(5);
  return;
}

void continue_init(void)
{
  sound_slots_1_3_5_off();
  voice_slots_off();
  stage_sound_stop_hook_run();
  if ((user_back_data.option_bits & 0x4000000) == 0) {
    sound_play_id(0x35b,0);
  }
  stage_anim_reset();
  fighter_fx_clear();
  continue_pose_start();
  mode_timer_float = 599.0;
  mode_timer = 599;
  mode_timer_rate = 1.0;
  mode_req_sub(1);
  continue_mes_start();
  return;
}

void fukkatu_init(void)
{
  stage_anim_reset();
  fukkatu_pose_start();
  mode_timer_rate = 1.0;
  mode_req_sub(6);
  return;
}

void intrude_init(void)
{
  int result;
  uint *sound_state;
  
  mode_sel_close();
  msnm_intrude_suspend();
  cns_nn_reset();
  result = card_close_phase_get();
  if ((result == 1) && (result = card_write_mode_busy(), result == 0)) {
    card_cpu_play_end();
    card_close_phase_set(2);
  }
  sound_state = (uint *)sound_state_get();
  mode_timer_float = 180.0;
  mode_timer = 0xb4;
  intrude_timer = 0xb4;
  mode_timer_rate = 1.0;
  mode_req_sub(1);
  streak_challenge_state = 2;
  bgm_sound_off();
  sound_slots_1_3_5_off();
  if ((user_back_data.option_bits & 0x4000000) == 0) {
    sound_play_id(0x364,0);
  }
  stage_sound_stop_hook_run();
  *sound_state = *sound_state | 0x20;
  fog_flags = fog_flags & 0xffffffbf | 7;
  return;
}

void warning_init(void)
{
  int bios_result;
  int country;
  uint debug_on;
  void (*call)();
  uint arg;
  
  bios_result = bios_helper_08();
  console_disp = ON_OFF_ON;
  country = system_param_country_get();
  if ((country == 0) && (warning_shown == '\0' || bios_result < 1)) {
    debug_on = debug_info_enabled();
    if (!(debug_on == 0)) {
      mode_timer = 10;
      mode_timer_float = 10.0;
    }
    else {
      mode_timer = 600;
      mode_timer_float = 600.0;
    }
    mode_timer_rate = 1.0;
    mode_req_sub(1);
    warning_shown = '\x01';
    match_settings_reset(0);
    arg = 0xffe0e0e0;
    call = text_color_set;
  }
  else {
    country = system_param_country_get();
    if (country == 1) {
      advertise_sub6_skip = 1;
    }
    mode_req_main(MODE_MAIN_ADVERTISE);
    arg = 0;
    call = mode_req_sub;
  }
  (*call)(arg);
  rank_page = 0;
  demo_stage_index = '\0';
  demo_kind = '\x01';
  return;
}

void intrude_disp(void)
{
  int result;
  
  if (0 < intrude_timer) {
    intrude_timer = intrude_timer + -1;
  }
  challenger_mes_draw();
  rep_load_finished();
  result = card_wait_check();
  if (((result == 0) && (intrude_timer < 1)) && (result = card_write_mode_busy(), result == 0)) {
    user_back_data.bookkeep.challengers = user_back_data.bookkeep.challengers + 1;
    hud_stage_free();
    item_award_mes_reset();
    mode_req_main(MODE_MAIN_SELECTOR);
    mode_req_sub(0);
  }
  return;
}

void mode_game(void)
{
  int i;
  for (i = 0; i < 2; i++) player_plate_draw(i);
  if (play_style != 2 && (mode_get_sub() == 5 || mode_get_sub() == 7 || mode_get_sub() == 8 || mode_get_sub() == 9 || mode_get_sub() == 10 || mode_get_sub() == 0xc || mode_get_sub() == 0xd || mode_get_sub() == 0xe)) mission_hud_draw();
}

void match_or_round_next(void)
{
  ((game_flags & 0x400) ? match_end_next : round_next)();
}

void game_appose_init(void)
{
  uint *sound_state;
  int mission;
  
  match_settings_reset(0);
  mode_timer_rate = 1.0;
  game_flags = game_flags & 0xfff7ffff;
  appose_frames = 1;
  stage_tasks_start();
  stage_tasks_start();
  stage_field_init();
  sound_state = (uint *)sound_state_get();
  *sound_state = *sound_state | 0x1001;
  level_change_flags_compute();
  hud_stage_load();
  if (!(play_style != '\x02')) item_lottery_stage_roll(); else item_lottery_clear();
  mission = mission_active_get();
  if (mission != -1) {
    prize_anim_tick();
  }
  rob_tasks_start();
  task_start(&task_camera);
  robs_start();
  enemy_task_start();
  appose_intro_start();
  mode_req_sub(7);
  game_appose_disp();
  return;
}

void game_appose_disp(void)
{
  int result;
  
  result = msnm_active();
  if (result != 0) {
    msnm_intro_draw();
  }
  result = mission_active_get();
  if (result != -1) {
    if (!(play_style != '\x02')) prize_banner_appose_draw(); else prize_anim_draw();
  }
  result = effect_skip_requested();
  if (result != 0) {
    mode_timer = 0;
    game_flags = game_flags | 0x80000;
  }
  appose_frames = appose_frames + 1;
  level_notice_draw();
  sprite_put_xy_scaled(0,0.0,0.0,80.0,7.5);
  sprite_put_xy_scaled(0,0.0,420.0,80.0,7.5);
  appose_intro_step();
  return;
}

void gameover_rating_disp(void)
{
  if ((int)mode_timer < 1) {
    mode_req_main(MODE_MAIN_ADVERTISE);
    mode_req_sub(0);
  }
}

int card_wait_check(void)
{
  uchar *state;
  int busy;
  
  busy = 0;
  state = card_service_state_get();
  if (*(int *)(state + 4) == 3) {
    if (card_write_mode_busy() != 0) {
      busy = 1;
    }
    else {
      if (card_close_phase_get() == 2) {
        card_close_phase_set(0);
        *(uint *)(state + 4) = 0xffffffff;
      }
      busy = 0;
    }
  }
  return busy;
}

void fukkatu_disp(void)
{
  if ((int)mode_timer <= 0) {
    mode_req_main(MODE_MAIN_GAME);
    mode_req_sub(4);
  }
}

void sega_disp(void)
{
  sega_disp_sprites_draw();
  if (*(int *)&mode_timer <= 0) {
    mode_timer = 0xb4;
    mode_timer_float = 180.0;
    mode_req_sub(3);
  }
}

void title_disp(void)
{
  australia_notice_draw();
  if ((int)mode_timer < 1) {
    if (demo_kind != '\0') {
      task_stop(&task_camera);
      task_stop(&task_title_disp);
    }
    mode_req_sub(0);
  }
  return;
}

void rating_disp(void)
{
  rating_layout_draw();
  if ((coin_added.side0 != 0) || (coin_added.side1 != 0)) {
    mode_timer = 0;
  }
  if ((int)mode_timer < 1) {
    mode_req_sub(2);
    mode_timer = 0xba;
    mode_timer_float = 186.0;
    bg_color_set(bg_color_presets[4]);
  }
}

void demo_init(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  logo_sprite_set_free();
  switch (demo_type) {
  case 0:
    sprite_sets_noop_a();
    break;
  case 1:
    sprite_sets_noop_b();
    break;
  case 2:
    sprite_sets_noop_c();
    break;
  default:
    sprite_sets_noop_c();
    break;
  }
  game_tasks_start();
  task_stop(&task_rep_save);
  stage_field_init();
  robs_start();
  task_start(&task_camera);
  mode_timer = 900;
  mode_timer_float = 900.0;
  mode_timer_rate = 1.0;
  demo_start();
  *sound_state = *sound_state | 4;
  mode_req_sub(8);
  demo_frames = 0;
  return;
}

void demo_disp(void)
{
  uint held;
  
  sound_state_get();
  demo_frame();
  demo_scene_draw();
  held = pause_frame_held();
  if (held == 0) {
    demo_frames = demo_frames + 1;
  }
  if (*(int *)&mode_timer <= 0) {
    demo_cleanup();
    task_tree_stop();
    sprite_sets_noop_d();
    mode_req_sub(0);
  }
  return;
}

void rank_disp(void)
{
  rank_list_draw();
  if ((int)mode_timer <= 0) {
    screen_fill_black();
    stage_sound_stop_hook_run();
    vp_work_disable();
    mode_req_sub(4);
    task_tree_stop();
    game_flags = game_flags & 0xfffffffb;
  }
  else if ((game_flags & 8) != 0) {
    game_flags = game_flags & 0xffffffb7;
    robs_start();
    camera.flags = camera.flags | 8;
  }
  return;
}

void ending_free(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  if (*(int *)&mode_timer <= 0) {
    mode_timer_float = 6000.0;
    mode_timer = 6000;
    mode_timer_rate = 1.0;
    mode_req_sub(2);
    game_sprite_sets_free();
    game_banks_free();
    staff_sprite_set_load();
    ending_roll_reset();
    stage_sound_stop_hook_run();
    sound_slots_1_3_5_off();
    voice_slots_off();
    other_bgm_load(3,0);
    *sound_state = *sound_state | 0x10;
    stage_extras_setup(0xffffffff,0xffffffff,0xffffffff,0xffffffff,0x12);
    stage_load_request(0x12);
    ending_rep_reset();
  }
  return;
}

void name_entry_disp(void)
{
  name_entry_frame();
  if ((int)mode_timer < 1) {
    mode_req_main(MODE_MAIN_OVER);
    mode_req_sub(2);
    name_entry_sprite_sets_free();
  }
  return;
}

void mode_update(void)
{
  main_mode_prev = main_mode_no;
  sub_mode_prev = sub_mode_no;
  if (main_mode_req != MODE_MAIN_NONE) {
    main_mode_no = main_mode_req;
    if (sub_mode_req == -1) {
      sub_mode_req = 0;
    }
    if (mode_data[main_mode_no].main_func.mode_asg != main_mode_no) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/mode.c",0x1e4,"assertion `%s\' failed.\n",
               "mode_data[main_mode_no].main_func.mode_asg == main_mode_no");
    }
  }
  if ((sub_mode_req != -1) &&
     (sub_mode_no = sub_mode_req,
     mode_data[main_mode_no].sub_func[sub_mode_req].mode_asg != sub_mode_req)) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/mode.c",0x1ec,"assertion `%s\' failed.\n",
             "mode_data[main_mode_no].sub_func[sub_mode_no].mode_asg == sub_mode_no");
  }
  main_mode_name = mode_data[main_mode_no].main_func.name;
  sub_mode_name = mode_data[main_mode_no].sub_func[sub_mode_no].name;
  snprintf(mode_name_text,0x80,"%s.%s",main_mode_name,sub_mode_name);
  main_mode_req = MODE_MAIN_NONE;
  sub_mode_req = -1;
  return;
}

void sega_free(void)
{
  int country;
  uint *colors;
  
  screen_fill_white();
  if ((int)mode_timer < 1) {
    game_sprite_sets_free();
    game_banks_free();
    loads_cancel_all();
    rep_highlights_free();
    ftttl_scenes_free();
    logo_sprite_set_load();
    cns_nn_reset();
    msnm_end();
    aet_all_free();
    ev_aet_task_stop();
    demo_match_pick();
    game_session_init_once();
    country = system_param_country_get();
    if ((country == 1) && (advertise_sub6_skip == '\0')) {
      mode_timer = 0xb4;
      mode_timer_float = 180.0;
      mode_req_sub(6);
      colors = bg_color_presets[0];
    }
    else {
      advertise_sub6_skip = '\0';
      mode_req_sub(2);
      mode_timer = 0xba;
      mode_timer_float = 186.0;
      colors = bg_color_presets[4];
    }
    bg_color_set(colors);
    user_back_mark_dirty();
  }
  return;
}

void game_mask(void)
{
  int result;
  
  sound_state_get();
  mes_group_23_draw_on_flag_800();
  if ((((int)mode_timer < 1) && (result = poly_queue_busy_get(), result == 0)) &&
     (result = load_pending_get(), result == 0)) {
    mes_group_23_free();
    mode_req_sub(3);
    game_sprite_sets_load();
    stopwatch_usec();
    rob_chara_setup((int)(char)player_chara.side0,
                 (int)(char)side_costume.side0,0);
    stopwatch_usec();
    rob_chara_setup((int)(char)player_chara.side1,
                 (int)(char)side_costume.side1,1);
    stopwatch_usec();
    stage_load_apply(match_stage);
    stopwatch_usec();
    stage_sound_banks_load_own(match_stage,0);
    chara_sound_banks_load((int)(char)player_chara.side0,
                 (int)(char)player_chara.side1,0);
    result = msnm_active();
    if (result != 0) {
      msnm_textures_load();
    }
  }
  return;
}

int start_skip_pressed(void)
{
  if (pause_is_on() == 0) {
    if ((player_input_get(0)->pressed & 0x8000) != 0 && (side_entry_flags[0] & 4) != 0) return 1;
    if ((player_input_get(1)->pressed & 0x8000) != 0 && (side_entry_flags[1] & 4) != 0) return 1;
  }
  return 0;
}

void rob_card_level_apply(int pick_mode)
{
  rob_rec *rob;
  int mark;
  int i;
  for (i = 0; i < 2; i++) {
    rob = rob_get(i & 0xff);
    mark = rob_card_level_pick(i,pick_mode);
    if (mark >= 0) {
      rob_card_mark_set(rob,mark);
    }
  }
}

void game_stage_disp(void)
{
  int result;
  
  level_notice_draw();
  sprite_put_xy_scaled(0,0.0,0.0,80.0,7.5);
  sprite_put_xy_scaled(0,0.0,420.0,80.0,7.5);
  stage_intro_fog_step();
  result = msnm_active();
  if (result != 0) {
    msnm_intro_draw();
  }
  result = mission_active_get();
  if (result != -1) {
    prize_anim_tick();
    if (!(play_style != '\x02')) prize_banner_appose_draw(); else prize_anim_draw();
  }
  result = mission_active_get();
  if ((((((result == -1) && (result = level_change_flags_get(0), result == 0)) &&
        (result = level_change_flags_get(1), result == 0)) && (result = msnm_active(), result == 0)) ||
      (mode_timer < 0x78)) &&
     ((result = effect_skip_requested(), result != 0 &&
      ((result = poly_queue_busy_get(), result == 0 || (result = load_pending_get(), result != 0)))))) {
    mode_timer = 0;
    game_flags = game_flags | 0x80000;
  }
  if (((mode_timer < 1) && (result = poly_queue_busy_get(), result == 0)) &&
     (result = load_pending_get(), result == 0)) {
    user_back_data.bookkeep.stage_intro_time = user_back_data.bookkeep.stage_intro_time + stage_intro_frames;
    user_back_data.bookkeep.stage_intro_runs = user_back_data.bookkeep.stage_intro_runs + 1;
    appose_frames = 1;
    stage_intro_fog_end();
    rob_tasks_start();
    robs_start();
    enemy_task_start();
    mode_req_sub(7);
    appose_intro_start();
  }
  return;
}

void game_judge_disp(void)
{
  int result;
  short frames;
  
  judge_mes_draw();
  result = msnm_active();
  if (!(result == 0)) {
    msnm_fight_draw();
  }
  else {
    cns_nn_players_draw(0);
  }
  if ((task_rep_save.flags & 2) != 0) {
    frames = rep_ring_write - rep_mark;
    if (frames < 0) {
      frames = frames + 0x168;
    }
    if (rep_judge_pre_frames < frames) {
      task_stop(&task_rep_save);
    }
  }
  result = msnm_active();
  if (result != 0) {
    msnm_judge_disp();
  }
  if (mode_timer < 1) {
    task_stop(&task_rep_save);
    vp_work_disable();
    user_back_data.bookkeep.judge_time = user_back_data.bookkeep.judge_time + judge_frames;
    user_back_data.bookkeep.judge_runs = user_back_data.bookkeep.judge_runs + 1;
    game_flags = game_flags & 0xfffffffb;
    judge_end_next();
  }
  return;
}

void warning_disp(void)
{
  text_cell_size_set(0x10,0x10);
  text_cursor_to_cell(1,4);
  text_draw_string(warning_text);
  if ((pause_frame_held() == 0) && (mode_timer < 0)) {
    text_cell_size_set(8,8);
    mode_req_main(MODE_MAIN_ADVERTISE);
    mode_req_sub(0);
  }
  return;
}

void game_stage_init(void)
{
  uint *sound_state;
  int result;
  
  net_ready_latch();
  sound_state = (uint *)sound_state_get();
  level_change_flags_compute();
  mission_choose();
  bonus_item_init();
  quest_end_check();
  net_nushi_ranks_validate();
  result = net_is_on();
  if (result != 0) {
    if (play_style == '\x01') {
      net_game_start_send_cpu();
    }
    if (play_style == '\x02') {
      net_game_start_send_vs();
    }
  }
  rob_card_level_apply(0);
  match_settings_reset(0);
  result = mission_active_get();
  if (result != -1) {
    if (play_style != '\x02') {
      miss_bank_load();
    }
    else {
      prize_choose();
      sou_banks_load();
    }
    prize_anim_start(0);
  }
  stage_tasks_start();
  stage_field_init();
  *sound_state = *sound_state | 0x1001;
  stage_intro_cam_pick();
  task_start(&task_camera);
  mode_timer_rate = 1.0;
  mode_timer = 0xf0;
  mode_timer_float = 240.0;
  mode_req_sub(5);
  game_flags = game_flags & 0xfff7ffff | 0x100000;
  if (play_style == '\x02') {
    item_lottery_stage_roll();
  }
  else {
    item_lottery_clear();
  }
  vs_stats_round_clear();
  hud_stage_load();
  result = msnm_active();
  if (result != 0) {
    msnm_stage_start();
  }
  else {
    cns_nn_obi_open();
  }
  cns_nn_state_clear();
  msnm_side_tallies_clear();
  nickname_stage_init();
  return;
}

void game_replay_disp(void)
{
  int i;
  int result;
  player_input_rec *pad;
  
  if (((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
      (result = net_is_on(), result != 0)) && (result = card_user_count_get(), 1 < result)) {
    i = 0;
    do {
      pad = player_input_get(i);
      if ((pad->now & 7) == 7) {
        net_bookmark_set(i);
      }
      i = i + 1;
    } while (i < 2);
  }
  result = effect_skip_requested();
  if (((result != 0) && (2 < mode_timer)) &&
     ((play_style != '\x02' || ((game_flags & 0x400) == 0)))) {
    mode_timer_float = 2.0;
    mode_timer = 2;
  }
  replay_hud_draw();
  if (mode_timer < 1) {
    user_back_data.bookkeep.replay_time = user_back_data.bookkeep.replay_time + replay_frames;
    user_back_data.bookkeep.replay_runs = user_back_data.bookkeep.replay_runs + 1;
    field_coli_saved[0] = *field_coli;
    rep_load_end();
    field_coli_saved[1] = *field_coli;
    replay_end_next();
  }
  return;
}

void ending_end(void)
{
  int req;
  
  if (mode_timer < 1) {
    staff_sprite_set_free();
    game_banks_free();
    rob_obj_alt_req[0] = 0;
    rob_obj_alt_req[1] = 0;
    name_entry_rank = time_ranking_slot_find(course_time_total);
    bgm_fade_out_cancel();
    if (name_entry_rank != -1) req = 4; else { mode_req_main(MODE_MAIN_OVER); req = 2; } mode_req_sub(req);
  }
  return;
}

int card_entry_allowed(int side)
{
  char *state;
  int used;
  
  if (user_back_data.card_start != OFF_ON_OFF) {
    state = card_service_state_get();
    used = card_unit_used_get(side);
    if (used == 0 || *state == '\0' || (&card_status)[side * 8] == '\0') {
      return 0;
    }
    return 1;
  }
  return 1;
}

void match_settings_reset(int keep_tasks)
{
  int result;
  short v;
  
  if (play_style == '\x02') {
    match_energy = energy_vs_setting;
    match_round_time = round_time_vs_setting;
    rounds_to_win = user_back_data.vs_match_point;
    jisaku_match_start();
  }
  else if ((play_style == '\x01') && (result = msnm_active(), result != 0)) {
    match_energy = msnm_match_energy_get(0);
    match_round_time = msnm_round_time_get();
    result = msnm_rounds_to_win_get();
    rounds_to_win = (char)result;
  }
  else {
    if (play_style == '\x01') {
      v = energy_1p_setting;
    } else v = 9999;
    match_energy = v;
    match_round_time = round_time_1p_setting;
    rounds_to_win = user_back_data._1p_match_point;
  }
  round_number = '\0';
  rounds_won.side0 = '\0';
  rounds_won.side1 = '\0';
  match_time_frames = 0;
  camera.mode = MODE_ROB;
  camera.mode_return = 0;
  camera.proj.pers = 45.0;
  game_flags = game_flags & 0x40001;
  if (keep_tasks == 0) {
    task_tree_stop();
  }
  bgm_sound_off();
  voice_slots_off();
  sound_slots_1_3_5_off();
  stage_sound_stop_hook_run();
  robs_flags_clear();
  camera_view_reset();
  robs_match_clear();
  card_images_clear_unused();
  both_pose_flags = 0;
  return;
}

void match_end_notify(void)
{
  cns_nn_obi_close();
  cns_nn_release();
  if (play_style == '\x01') {
    if (msnm_active() != 0) {
      msnm_prize_start();
      champion_side = (char)msnm_player_side();
      round_winner = (char)msnm_player_side();
      msnm_result_start();
    }
    else {
      msnm_result_kind_clear();
    }
  }
  else if (streak_limit_hit != '\0' || msnm_vs_result_apply(champion_side) == 0) {
    msnm_result_kind_clear();
  }
  else {
    msnm_join_offer(champion_side);
  }
}

void game_session_init_once(void)
{
  if (*(char *)&game_session_inited == 0) {
    game_session_inited = 1;
    play_style = 0;
    camera.mode = FIX;
    game_init_value = 1.0;
    champion_side = 0xff;
    rank_round = 0;
    course_time_total = 0;
    entry_flags_reset(side_entry_flags);
    player_chara.side0 = 0;
    player_chara.side1 = 0;
    side_costume.side0 = 0;
    side_costume.side1 = 1;
    board_id_code_step = 0;
    board_id_code_timer = 0;
    card_record_release(0);
    card_record_release(1);
    win_streak = 0;
    match_settings_reset(0);
  }
}

void game_round_disp(void)
{
  int result;
  
  game_round_disp_nop();
  round_intro_draw();
  result = msnm_active();
  if (result != 0) {
    msnm_fight_draw();
  }
  else {
    cns_nn_players_show();
    cns_nn_players_draw(1);
  }
  if (((game_flags & 0x200) == 0) && (mode_timer == 0x78)) {
    sound_play_id(round_number + 0x3bc,3);
  }
  if (mode_timer == 0x3c) {
    sound_play_id(0x3ce,3);
  }
  if (mode_timer < 1) {
    user_back_data.bookkeep.round_intro_time = user_back_data.bookkeep.round_intro_time + round_intro_frames;
    user_back_data.bookkeep.round_intro_runs = user_back_data.bookkeep.round_intro_runs + 1;
    vs_round_time = 0;
    p1_round_time = 0;
    sound_play_id(0x3cb,3);
    stage_ban_rain_round_off();
    vp_round_init();
    robs_card_flags_refresh();
    mode_req_sub(10);
    game_flags = (game_flags | 4) & 0xffdfffff;
  }
  return;
}

void gameover_disp(void)
{
  int pressed;
  int r;
  
  gameover_mes_draw();
  if (card_wait_check() != 0) return;
  r = card_write_mode_busy();
  if (r != 0) return;
  pressed = start_pressed_mask();
  if (pressed != 0) {
    mode_timer = r;
  }
  if (mode_timer < 1) {
    game_sprite_sets_free();
    if (play_style == '\x01') {
      if (((user_back_data.bookkeep.shortest_1p >= p1_play_frames_min) ||
          (user_back_data.bookkeep.shortest_1p == 0)) && (p1_play_frames_min != 0)) {
        user_back_data.bookkeep.shortest_1p = p1_play_frames_min;
      }
      if (user_back_data.bookkeep.longest_1p < p1_play_frames_max) {
        user_back_data.bookkeep.longest_1p = p1_play_frames_max;
      }
      net_player_end((uint)(uchar)champion_side);
    }
    user_back_mark_dirty();
    streak_limit_hit = 0;
    mode_req_main(MODE_MAIN_ADVERTISE);
    mode_req_sub(0);
  }
}

void gameover_init(void)
{
  uchar *state;
  int result;
  short *fade;
  card_record_rec *p[2];
  
  mode_timer_float = 420.0;
  mode_timer = 0x1a4;
  mode_timer_rate = 1.0;
  mode_req_sub(3);
  sound_slots_1_3_5_off();
  bgm_sound_off();
  voice_slots_off();
  stage_sound_stop_hook_run();
  sound_play_id(0x3ca,3);
  if ((user_back_data.option_bits & 0x4000000) == 0) {
    sound_play_id(0x35e,0);
  }
  if (((char)rank_round < '\n') && (play_style == '\x01')) {
    state = card_service_state_get();
    if (*(int *)(state + 4) == 1) {
      card_cpu_play_end();
      result = net_is_on();
      if ((result != 0) && (play_style == '\x01')) {
        net_game_end_send_cpu();
      }
    }
    else {
      result = card_close_phase_get();
      if (result == 1) {
        card_cpu_play_end();
        card_close_phase_set(2);
      }
      else {
        result = net_is_on();
        if ((result != 0) && (play_style == '\x01')) {
          net_game_end_send_cpu();
        }
        p[0] = card_record_get(0);
        p[1] = card_record_get(1);
        p[0]->in_use = 0;
        p[1]->in_use = 0;
      }
    }
  }
  fog_flags = fog_flags | 0x20;
  fog_timer = 200;
  fade = fade_color;
  fade[0] = 0xff;
  fade[1] = 0xff;
  fade[2] = 0xff;
  stage_anim_reset();
  rob_alt_model_card[1] = 0;
  rob_alt_model_card[0] = 0;
  bg_color_set(bg_color_presets[4]);
  result = msnm_active();
  if (result != 0) {
    alt_mode_gameover();
  }
}

void alt_mode_match_setup(void)
{
  int cpu_side;
  uint level;
  int result;

  cpu_side = msnm_player_side() == 0;
  match_stage = msnm_stage_get();
  ((struct { uint a[2]; } *)&player_chara)->a[cpu_side] = msnm_opponent_char_get();
  level = msnm_cpu_level_get();
  ((void (*)(int))vp_level_set)(level);
  result = msnm_mission53_flag_get();
  if (result != 0) {
    vp_mode_flag_set(1);
  }
  return;
}

void mode_frame(void)
{
  int start;
  int end;
  uint held;
  
  tasks_mark_on_request();
  player_join_frame();
  credit_display_frame();
  ledger_daily_update();
  if (main_mode_req == MODE_MAIN_ADVERTISE) {
    board_id_code_step = '\0';
    board_id_code_index = '\0';
    board_id_code_timer = 0;
  }
  mode_update();
  start = stopwatch_usec();
  card_modes_frame();
  console_frame_draw();
  net_messages_draw();
  mode_exec();
  end = stopwatch_usec();
  mode_exec_usec = (float)(end - start);
  held = pause_frame_held();
  if (held == 0) {
    int old = mode_timer;
    mode_frame_count = mode_frame_count + 1;
    mode_timer_float = mode_timer_float - mode_timer_rate;
    mode_timer = (int)mode_timer_float;
    if (old != mode_timer) {
      game_flags = game_flags | 0x10000;
    }
    else {
      game_flags = game_flags & 0xfffeffff;
    }
  }
}

void game_srep_disp(void)
{
  if (mode_timer < 1) {
    rep_load_end();
    task_resume(&task_rep_save);
    camera.mode = MODE_ROB;
    camera.pers_req = 45.0;
    camera.flags = camera.flags | 0x10;
    mode_req_main((int)srep_next_main);
    mode_req_sub((int)srep_next_sub);
    mode_timer = srep_next_timer;
    mode_timer_float = (float)(int)srep_next_timer;
    mode_timer_rate = 1.0;
  }

}

void round_win_add(int side)
{
  char c;
  char *p;

  c = (char)side;
  round_winner = c;
  p = &rounds_won.side0;
  p[c]++;
  if (p[c] >= rounds_to_win) {
    champion_side = c;
    game_flags = game_flags | 0x400;
    if (((rob_get((byte)c))->flags & 4) != 0) {
      game_flags = game_flags | 0x800;
    }
  }
}

void replay_end_next(void)
{
  uint sides;
  int result;
  int i;
  player_input_rec *pad;
  
  card_state_idle_reset();
  if ((game_flags & 0x400) != 0) {
    match_end_notify();
  }
  if (((game_flags & 0x400) != 0) && (play_style == '\x01')) {
    sides = mission_sides_get();
    if ((play_style == '\x01') && ((-1 < (int)sides && (result = mission_result_get(), result == 1)))) {
      user_back_data.mission_cleared[(int)mission_active_get()]++;
    }
    result = mission_result_get();
    if (result == 1) {
      if (sides != 0xffffffff) {
        item_award_mes_start(sides,0);
      }
      prize_anim_start(1);
    }
    else {
      result = mission_result_get();
      if (result == -1) {
        if (play_style == '\x02') prize_anim_start(1);
        else prize_anim_start(2);
      }
    }
  }
  camera_shake_request(0xffffffff,0);
  stage_anim_reset();
  fighter_fx_clear();
  audience_mood_set(0);
  if ((((game_flags & 0x400) != 0) && (play_style == '\x02')) &&
     (result = net_is_on(), result != 0)) {
    result = card_user_count_get();
    if (1 < result) {
      for (i = 0; i < 2; i++) {
        pad = player_input_get(i);
        if ((pad->now & 7) == 7) {
          net_bookmark_set(i);
        }
      }
    }
    net_game_end_send_vs((int)champion_side,win_streak);
  }
  if ((round_winner == -1) ||
     (((user_back_data.option_bits & 0x400000) == 0 && ((game_flags & 0x400) == 0))))
  {
    match_or_round_next();
  }
  else {
    if (((game_flags & 0x400) != 0) &&
       (bgm_sound_off(), (user_back_data.option_bits & 0x4000000) == 0)) {
      sound_play_id(0x361,0);
    }
    winner_frames = 1;
    mode_req_sub(0xf);
    rob_start_mirror_request();
    winner_pose_start();
    winner_mes_start();
  }
  return;
}

void game_init(void)
{
  int *renew;
  card_record_rec *r;
  uint side;
  int *times;
  
  item_pending_expire();
  side = 0;
  do {
    renew = card_renew_times_get(side);
    r = card_record_get(side);
    if (renew[2] == 3) {
      user_back_data.discarded_codes[user_back_data.discarded_code_next++] = *renew;
      if (9 < user_back_data.discarded_code_next) {
        user_back_data.discarded_code_next = 0;
      }
      card_renew_times_clear(side);
      user_back_mark_dirty();
    }
    if ((renew[2] == 1) && ((r->header.flags & 4) != 0)) {
      card_renew_times_reset(side & 0xff);
    }
    side = side + 1;
  } while ((int)side < 2);
  input_repeat_set(0x14,1);
  perf_color_mark(0xffff0000);
  bg_color_set(bg_color_presets[0]);
  energy_1p_setting = energy_table_1p[(char)user_back_data.energy_1p];
  energy_vs_setting = energy_table_vs[(char)user_back_data.energy_vs];
  times = round_time_table;
  round_time_1p_setting = times[(char)user_back_data.game_time_1p];
  round_time_vs_setting = times[(char)user_back_data.game_time_vs];
  game_init_value = 1.0;
  mode_req_sub(1);
  mode_timer_rate = 1.0;
  rep_highlights_alloc();
  cpu_opponent_pick((char)rank_round);
  version_label_draw();
}

void game_winner_disp(void)
{
  rob_rec *rob;
  uint w;
  void (*f)(void);

  rob = rob_get((uint)winner_disp_side);
  if (rob->frame >= rob->move_length) {
    mode_timer = 0;
  }
  card_state_idle_reset();
  if (effect_skip_requested() != 0 && card_write_mode_busy() == 0) {
    if ((item_set_redeem_active() | item_award_mes_active(0) | item_award_mes_active(1)) == 0 &&
        (prize_fly_state_get() == 0 || prize_fly_state_get() > 4) &&
        (level_up_banner_due() == 0 || mode_timer < 0xdb) &&
        2 < mode_timer && cns_nn_winner_shown() == 0) {
      net_reply_timer_set(0);
      mode_timer_float = 2.0;
      mode_timer = 2;
    }
  }
  prize_debug_text_draw();
  if (2 < mode_timer && mission_active_get() != -1) {
    prize_anim_tick();
    if (play_style == '\x02')
      f = prize_banner_winner_draw;
    else
      f = prize_anim_draw;
    (*f)();
  }
  w = winner_disp_done();
  if (card_wait_check() == 0 && card_write_mode_busy() == 0 && w != 0) {
    user_back_data.bookkeep.winner_time = user_back_data.bookkeep.winner_time + winner_frames;
    user_back_data.bookkeep.winner_runs = user_back_data.bookkeep.winner_runs + 1;
    item_award_mes_reset();
    camera.pers_req = 45.0;
    camera.flags = camera.flags | 0x10;
    match_or_round_next();
  }
}

void match_end_next(void)
{
  int result;
  int s;
  
  hud_stage_free();
  if ((play_style == '\x01') && (result = msnm_active(), result != 0)) {
    alt_mode_match_next();
    return;
  }
  if (play_style == '\x01') {
    if (((rob_get(0))->flags & 4) == 0) {
      rob_get(1);
    }
    if ((9 < *(signed char *)&rank_round) && ((game_flags & 0x800) == 0)) {
      mode_req_main(MODE_MAIN_ENDING);
      mode_req_sub(0);
      champion_side = (uchar)champion_side ^ 1;
      ranking_chara = (rob_get((uint)(uchar)champion_side))->chara;
      return;
    }
    if ((game_flags & 0x800) == 0) {
      mode_req_main(MODE_MAIN_OVER);
      if (user_back_data.no_continue == 0) mode_req_sub(0); else mode_req_sub(2);
      champion_side = (uchar)champion_side ^ 1;
      return;
    }
    if ((game_flags & 0x20000) != 0) {
      mode_req_main(MODE_MAIN_ENDING);
      s = 0;
      goto LAB_0c0285f6;
    }
  }
  if (streak_limit_hit != '\0') {
    mode_req_main(MODE_MAIN_OVER);
    s = 2;
  }
  else {
    play_style = '\x01';
    bg_color_set(bg_color_presets[0]);
    mes_group_23_load();
    mes_group_23_draw_on_flag_800();
    task_tree_stop();
    mode_timer = 2;
    mode_timer_float = 2.0;
    mode_timer_rate = 1.0;
    s = 1;
  }
LAB_0c0285f6:
  mode_req_sub(s);
  return;
}

byte start_pressed_mask(void)
{
  byte r;

  r = 0;
  if (pause_is_on() == 0) {
    if ((player_input_get(0)->pressed & 0x8000) != 0) r = 1;
    if ((player_input_get(1)->pressed & 0x8000) != 0) r |= 2;
  }
  return r;
}

void cpu_opponent_pick(int round_no)
{
  char *entry;
  char *next;
  int index;
  
  char c = round_no;
  
  if (c > 9) {
    cpu_next_chara = '\r';
    cpu_next_stage = '\v';
  }
  else {
    entry = cpu_order_entry(c + title_day_of_month);
    cpu_next_chara = *entry;
    cpu_next_stage = entry[1];
    if (entry[1] < '\0') {
      index = 0;
      switch (*entry) {
      case 0xf:
        break;
      case 0xe:
        index = 2;
        break;
      default:
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/mode.c",0x130d,"assertion `%s\' failed.\n","0");
      }
      index = index + title_day_of_month + 10;
      next = cpu_order_entry(index);
      if (next[1] < '\0') {
        next = cpu_order_entry(index + 1);
      }
      cpu_next_stage = next[1];
      if ((char)cpu_next_stage < '\0') {
        cpu_next_stage = '\x17';
      }
    }
  }
}

void alt_mode_gameover(void)
{
  uchar *state;
  int side;
  
  state = card_service_state_get();
  if (*(int *)(state + 4) == 1) {
    card_cpu_play_end();
    if (net_is_on() != 0) {
      net_game_end_send_cpu();
    }
  }
  else if (card_close_phase_get() == 1) {
    card_cpu_play_end();
    card_close_phase_set(2);
    if (net_is_on() != 0) {
      net_game_end_send_cpu();
    }
  }
  else {
    if (net_is_on() != 0) {
      net_game_end_send_cpu();
    }
    for (side = 0; side < 2; side++) {
      card_record_get(side)->in_use = 0;
    }
  }
  msnm_end();
}

void game_round_init(void)
{
  uint *sound_state;
  int result;
  uint *f;
  
  rob_card_level_apply(0);
  sound_state = (uint *)sound_state_get();
  user_back_data.bookkeep.appose_time = user_back_data.bookkeep.appose_time + appose_frames;
  user_back_data.bookkeep.appose_runs = user_back_data.bookkeep.appose_runs + 1;
  stage_anim_reset();
  fighter_fx_clear();
  djo_light_cycle_next();
  audience_mood_set(0);
  f = (uint *)&game_flags;
  *f = *f & 0x140201 | 0x200000;
  round_winner = 0xff;
  round_time = match_round_time;
  round_time_float = (float)(int)match_round_time;
  mode_timer = 0x96;
  mode_timer_float = 150.0;
  mode_timer_rate = 1.0;
  mode_req_sub(9);
  round_time_frames = 0;
  voice_slots_off();
  camera.mode = MODE_ROB;
  camera.mode_return = 0;
  camera.flags = 0x18;
  camera.pers_req = 45.0;
  if ((*f & 0x200) != 0) {
    sound_play_id(0x3d0,3);
  }
  else {
    sound_play_id(0x3c9,3);
    *sound_state = *sound_state | 0x400;
  }
  life_gauges_reset();
  level_notice_tex_free();
  robs_round_init();
  mission_round_init();
  jisaku_round_reset();
  result = msnm_active();
  if (result != 0) {
    msnm_round_start();
    chlg_map_layers_release();
    chlg_result_layers_free();
    msnm_round_init_hook();
  }
  msnm_side_trackers_clear();
  nickname_round_init();
  return;
}

void judge_end_next(void)
{
  int result;
  rob_rec *rob;
  player_input_rec *pad;

  result = msnm_active();
  if (result != 0) {
    chlg_panel_parts_free();
    chlg_result_layers_free();
  }
  if ((game_flags & 0x400) != 0) {
    bonus_lottery_step();
    if (((play_style == '\x01') && (result = msnm_active(), result == 0)) &&
       (rob = rob_get((uint)(uchar)round_winner), (rob->flags & 4) != 0)) {
      rep_highlight_store();
    }
    stage_ban_rain_judge_on();
    if ((game_flags & 0x110) != 0) {
      replay_end_next();
      return;
    }
  }
  else {
    if ((game_flags & 0x10) != 0) {
      replay_end_next();
      return;
    }
    if ((game_flags & 0x100) != 0) {
      round_next();
      return;
    }
  }
  mode_req_sub(0xe);
  mode_timer = 0xf0;
  mode_timer_float = 240.0;
  mode_timer_rate = 1.0;
  vars_0c41f310_clear();
  camera_view_reset();
  stage_anim_reset();
  fighter_fx_clear();
  audience_mood_set(0);
  item_zanzou_restart();
  rob = rob_get(0);
  if ((((rob->flags & 4) != 0) &&
       (pad = player_input_get(0), (pad->now & 7) == 3)) ||
      ((rob = rob_get(1), (rob->flags & 4) != 0 &&
       (pad = player_input_get(1), (pad->now & 7) == 3)))) {
    camera.mode = MODE_ROB;
  }
  else {
    camera.mode = REPLAY;
  }
  camera.mode_return = camera.mode;
  camera_shake_request(0xffffffff,0);
  replay_frames = 1;
  rep_repeat_mode = 0;
  rep_repeat_count = 0;
  task_start(&task_rep_load);
}

uint winner_disp_done(void)
{
  uint r;

  r = 0;
  switch ((int)msnm_result_kind_get()) {
  case 1:
    if (msnm_join_select() != 0) r = 1;
    if (r == 0) msnm_join_draw();
    break;
  case 2:
    r = msnm_result_step();
    if (r == 0) msnm_result_draw();
    break;
  case 0:
  default:
    if (mode_timer <= 0) r = 1;
    winner_mes_draw();
    break;
  }
  return r;
}

void continue_disp(void)
{
  if ((&coin_added.side0)[champion_side] != '\0') {
    mode_timer_float = 599.0;
    mode_timer = 599;
  }
  else if ((mode_timer < 0x1a4) &&
     ((player_input_get(champion_side)->pressed & 0xf) != 0)) {
    mode_timer_float = mode_timer_float - 60.0;
    mode_timer = (int)mode_timer_float;
  }
  if (mode_timer < 0) {
    mode_timer_float = 0.0;
    mode_timer = 0;
  }
  if ((0 < mode_timer) && (mode_timer % 60 == 0x30)) {
    sound_play_id(0x3b3,0);
  }
  continue_mes_draw();
  if (mode_timer < 1) {
    mode_req_sub(2);
    continue_mes_free();
  }
  return;
}

void mode_advertise(void)
{
  player_input_rec *pad;
  uchar *id;
  
  pad = player_input_get(0);
  switch (board_id_code_step) {
  case 0:
    if (pad->now == 0x22) {
      board_id_code_step++;
      board_id_code_index = 0;
      board_id_code_timer = 300;
    }
    break;
  case 1:
    if (--board_id_code_timer != -1) {
      if (board_id_code[board_id_code_index] == -1) {
        board_id_code_step++;
        board_id_code_index = 0;
        board_id_code_timer = 600;
      }
      else if (pad->now == board_id_code[board_id_code_index]) {
        board_id_code_index++;
      }
    }
    else {
      board_id_code_step = 0;
    }
    break;
  case 2:
    if (--board_id_code_timer != -1) {
      text_cell_size_set(8,8);
      text_cursor_to_cell(0x28,0x38);
      text_color_set(0xffe0e0e0);
      id = main_board_id_get();
      text_draw_formatted("MAIN BOARD ID %s\n",id);
      id = dimm_board_id_get();
      text_draw_formatted("DIMM BOARD ID %s\n",id);
      sprite_put_xyz_scaled(0,316.0,444.0,0.006,32.0,3.0);
    }
    else {
      board_id_code_step = 0;
      board_id_code_index = 0;
      board_id_code_timer = 0;
    }
    break;
  }
  return;
}

void game_disp(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  uint *sound_state;
  int old;
  
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  jisaku_flag_update();
  mission_frame_check();
  bonus_wall_watch();
  if (msnm_active() != 0) {
    msnm_disp();
    msnm_fight_draw();
  }
  else {
    cns_nn_players_draw(0);
  }
  msnm_side_trackers_update();
  sound_state = (uint *)sound_state_get();
  hud_draw();
  card_item_log_state_update();
  if (pause_frame_held() == 0) {
    nickname_disp();
    if ((rob0->flags & 4) != 0) {
      if (user_back_data.bookkeep.chara_max_combo[rob0->chara] < rob1->max_combo) {
        user_back_data.bookkeep.chara_max_combo[rob0->chara] = rob1->max_combo;
      }
      if (user_back_data.bookkeep.chara_max_damage[rob0->chara] < rob1->max_damage) {
        user_back_data.bookkeep.chara_max_damage[rob0->chara] = rob1->max_damage;
      }
      if (user_back_data.bookkeep.chara_max_wall_combo[rob0->chara] < rob1->max_wall_combo) {
        user_back_data.bookkeep.chara_max_wall_combo[rob0->chara] = rob1->max_wall_combo;
      }
      if (user_back_data.bookkeep.chara_max_wall_damage[rob0->chara] < rob1->max_wall_damage) {
        user_back_data.bookkeep.chara_max_wall_damage[rob0->chara] = rob1->max_wall_damage;
      }
    }
    if ((rob1->flags & 4) != 0) {
      if (user_back_data.bookkeep.chara_max_combo[rob1->chara] < rob0->max_combo) {
        user_back_data.bookkeep.chara_max_combo[rob1->chara] = rob0->max_combo;
      }
      if (user_back_data.bookkeep.chara_max_damage[rob1->chara] < rob0->max_damage) {
        user_back_data.bookkeep.chara_max_damage[rob1->chara] = rob0->max_damage;
      }
      if (user_back_data.bookkeep.chara_max_wall_combo[rob1->chara] < rob0->max_wall_combo) {
        user_back_data.bookkeep.chara_max_wall_combo[rob1->chara] = rob0->max_wall_combo;
      }
      if (user_back_data.bookkeep.chara_max_wall_damage[rob1->chara] < rob0->max_wall_damage) {
        user_back_data.bookkeep.chara_max_wall_damage[rob1->chara] = rob0->max_wall_damage;
      }
    }
    if ((game_flags & 8) == 0) {
      old = round_time;
      round_time_float = round_time_float - mode_timer_rate;
      round_time = (int)round_time_float;
      if (old != round_time) {
        if (*(char *)&rank_round < 10) {
          round_time_frames = round_time_frames + 1;
        }
        game_flags = game_flags | 0x10000;
      }
      else {
        game_flags = game_flags & 0xfffeffff;
      }
      if ((round_time < 600) && ((*sound_state & 0x300) == 0)) {
        *sound_state = *sound_state | 0x100;
      }
      if (0 < round_time) {
        return;
      }
      game_flags = game_flags | 0x18;
    }
    mode_req_sub(0xc);
  }
}

void alt_mode_match_next(void)
{
  rob_rec *rob;
  card_image_rec *image;
  int rank;
  char buf[24];

  rob = rob_get(msnm_player_side() & 0xff);
  image = card_image_get(msnm_player_side());
  if (card_body_name_empty(&image->body) != 0) {
    snprintf(buf,0x15,"NO-NAME");
  }
  else {
    card_body_name_get(&image->body,buf,0x15);
  }
  msnm_textures_free();
  if (streak_limit_hit != 0) {
    mode_req_main(MODE_MAIN_OVER);
    mode_req_sub(2);
    return;
  }
  if ((game_flags & 0x20000) != 0) {
    rank = msnm_rank_get();
    if (rank != -1) {
      name_entry_rank = rank + msnm_column_get() * 4 + 1;
      mode_req_main(MODE_MAIN_ENDING);
      mode_req_sub(4);
      point_ranking_insert((char)rob->chara,win_streak,buf);
      return;
    }
  }
  else if (msnm_quit_chosen_get() == 0) goto ok;
  mode_req_main(MODE_MAIN_OVER);
  mode_req_sub(2);
  point_ranking_insert((char)rob->chara,win_streak,buf);
  return;
ok:
  play_style = 1;
  bg_color_set(bg_color_presets[0]);
  mes_group_23_load();
  mes_group_23_draw_on_flag_800();
  task_tree_stop();
  mode_timer = 2;
  mode_timer_float = 2.0;
  mode_timer_rate = 1.0;
  mode_req_sub(1);
}

void credit_text_draw(int side)
{
  typedef struct { int credits; int coins; int coins_per_credit; int status; } cr;
  int pl = (char)side;
  cr *p = &((cr *)&credit_copy)[pl];
  char *str;
  uint m;
  float x;
  int t;
  char buf[64];

  if (credit_copy.credit_mode == 1) {
    str = "FREE PLAY";
  }
  else {
    if (p->credits + p->coins == 0) return;
    if (p->coins_per_credit == 1 || p->coins == 0) {
      snprintf(buf,0x40,"CREDIT(S) %d",p->credits);
    }
    else {
      snprintf(buf,0x40,"CREDIT(S) %d %d/%d",p->credits,p->coins,p->coins_per_credit);
    }
    str = buf;
  }
  if (credit_copy.separate == 0) {
    if ((uint)(main_mode_no - 2) <= 1) {
      if ((*(char *)&side_entry_flags[0] & 4) == 0) {
        if (play_style == 1 && (game_flags & 0xc00) == 0x400) {
          x = 621.0f;
          m = 2;
        }
        else {
          x = 18.0f;
          m = 0;
        }
      }
      else {
        if ((*(char *)&side_entry_flags[1] & 4) != 0) return;
        if (play_style == 1 && (game_flags & 0xc00) == 0x400) {
          x = 18.0f;
          m = 0;
        }
        else {
          x = 621.0f;
          m = 2;
        }
      }
    }
    else {
      if (main_mode_no == MODE_MAIN_OVER) {
        if ((*(char *)&side_entry_flags[0] & 4) != 0) {
          x = 18.0f;
          m = 0;
        }
        else if ((*(char *)&side_entry_flags[1] & 4) != 0) {
          x = 621.0f;
          m = 2;
        }
        else return;
      }
      else {
        x = 32.0f;
        m = 0;
      }
    }
  }
  else {
    if (main_mode_no == MODE_MAIN_OVER || (game_flags & 0xc00) == 0x400) {
      if (((uint)(int)*(char *)&side_entry_flags[0] >> 2 & 1 & (pl == 0)) != 0) x = 18.0f;
      else if (((uint)(int)*(char *)&side_entry_flags[1] >> 2 & 1 & (pl == 1)) != 0) x = 621.0f;
      else return;
    }
    else {
      if (((((int)*(char *)&side_entry_flags[0] >> 2) & 1) == 0) & (pl == 0)) x = 18.0f;
      else if (((((int)*(char *)&side_entry_flags[1] >> 2) & 1) == 0) & (pl == 1)) x = 621.0f;
      else return;
    }
    t = pl == 0;
    m = t == 0;
    m = m + m;
  }
  sprite_font_text_put(&ascii_font,str,x,16.0f,m,1e-05f);
  return;
}

void round_draw_add(void)
{
#define GO(x) \
  game_flags = game_flags | 0x400; \
  rob = rob_get(x); \
  if ((rob->flags & 4) != 0) { \
    game_flags = game_flags | 0x800; \
  }
  char c;
  rob_rec *rob;

  rounds_won.side0 += 1;
  rounds_won.side1 += 1;
  if ((game_flags & 0x200) != 0) {
    c = play_style;
    if (c == 1) {
      rob = rob_get(0);
      if ((rob->flags & 4) != 0) {
        round_winner = c;
      } else {
        round_winner = 0;
      }
    } else {
      if (champion_side == -1) {
        round_winner = game_frame_count & 1;
      } else {
        round_winner = (uchar)champion_side;
      }
    }
    champion_side = (uchar)round_winner;
    GO((uchar)round_winner)
  } else {
    if (rounds_won.side0 >= rounds_to_win) {
      if (rounds_won.side1 >= rounds_to_win) {
        game_flags = game_flags | 0x200;
        rounds_won.side0 -= 1;
        rounds_won.side1 -= 1;
        return;
      }
      round_winner = 0;
      champion_side = 0;
      GO(0)
    } else {
      if (rounds_won.side1 < rounds_to_win) {
        return;
      }
      round_winner = 1;
      champion_side = 1;
      GO(1)
    }
  }
}

void insert_coin_text_draw(int side)
{
  typedef struct { int credits; int coins; int coins_per_credit; int status; } cr;
  int pl = (char)side;
  cr *p;
  char *str;
  uint m;
  float x;
  float y;
  int t;

  if (credit_copy.credit_mode == 1) {
    str = "PRESS START BUTTON";
  }
  else {
    p = &((cr *)&credit_copy)[pl];
    switch ((uint)p->status) {
    case 1:
      str = "INSERT MORE COIN(S)";
      break;
    case 0:
      str = "INSERT COIN(S)";
      break;
    case 2:
      str = "PRESS START BUTTON";
      break;
    default:
      str = "";
    }
  }
  if (credit_copy.separate == 0) {
    if ((uint)(main_mode_no - 2) <= 1) {
      if ((*(char *)&side_entry_flags[0] & 4) == 0) {
        x = 18.0f;
        m = 0;
      }
      else {
        if ((*(char *)&side_entry_flags[1] & 4) != 0) return;
        x = 621.0f;
        m = 2;
      }
      y = 16.0f;
    }
    else {
      if (main_mode_no == MODE_MAIN_OVER) {
        if ((*(char *)&side_entry_flags[0] & 4) != 0) {
          x = 18.0f;
          m = 0;
        }
        else if ((*(char *)&side_entry_flags[1] & 4) != 0) {
          x = 621.0f;
          m = 2;
        }
        else return;
        y = 16.0f;
      }
      else {
        x = 320.0f;
        y = 424.0f;
        m = 4;
      }
    }
  }
  else {
    if (main_mode_no == MODE_MAIN_OVER) {
      if (((uint)(int)*(char *)&side_entry_flags[0] >> 2 & 1 & (pl == 0)) != 0) x = 18.0f;
      else if (((uint)(int)*(char *)&side_entry_flags[1] >> 2 & 1 & (pl == 1)) != 0) x = 621.0f;
      else return;
    }
    else {
      if (((((int)*(char *)&side_entry_flags[0] >> 2) & 1) == 0) & (pl == 0)) x = 18.0f;
      else if (((((int)*(char *)&side_entry_flags[1] >> 2) & 1) == 0) & (pl == 1)) x = 621.0f;
      else return;
    }
    y = 16.0f;
    t = pl == 0;
    m = t == 0;
    m = m + m;
  }
  sprite_font_text_put(&ascii_font,str,x,y,m,1e-05f);
  return;
}

void cri_disp(void)
{
  uint *sound_state;
  sound_state = (uint *)sound_state_get();
  cri_disp_sprites_draw();
  if (mode_timer < 1 && poly_queue_busy_get() == 0 && load_pending_get() == 0 && (*sound_state & 0x10000) == 0) {
    rob_costume_colors_send(0);
    rob_costume_colors_send(1);
    if (demo_kind == '\0') {
      switch (demo_type) {
      case 1:
        mothead_relocate(1,0x12);
        break;
      case 2:
        mothead_relocate(1,0x13);
        break;
      case 0:
      default:
        mothead_relocate(1,0x11);
        break;
      }
    }
    else {
      mothead_relocate(1,*((uchar *)chara_data_get((int)demo_chara_a) + 0x32));
      mothead_relocate(2,*((uchar *)chara_data_get((int)demo_chara_b) + 0x32));
    }
    stage_apply();
    if ((demo_kind & 1U) != 0) {
      mode_req_sub(9);
    }
    else {
      mode_req_sub(7);
    }
  }
  return;
}

void game_mask_init(void)
{
  int v;
  uint other;
  uint *chara;
  uint *costumes;
  
  sound_state_get();
  if (play_style == '\x01') {
    other = (int)champion_side & 1U ^ 1;
    costumes = &side_costume.side0;
    costumes[other] = costumes[champion_side] ^ 1;
    chara = &(&player_chara.side0)[other];
    *chara = ((char)cpu_next_chara == -1) ? 0 : (char)cpu_next_chara;
    v = (char)cpu_next_stage;
    if (v == -1) {
      v = 4;
    }
    match_stage = v;
  }
  if (msnm_active() != 0) {
    alt_mode_match_setup();
  }
  else {
    vp_level_set_for_round((uint)rank_round);
  }
  cpu_next_chara = 0xff;
  cpu_next_stage = 0xff;
  dural_alt_active = 0;
  game_banks_free();
  sel_scenes_release();
  stage_extras_setup(player_chara.side0,side_costume.side0,
               player_chara.side1,side_costume.side1,match_stage);
  voice_slots_off();
  stage_sound_stop_hook_run();
  mode_timer = 2;
  mode_timer_float = 2.0;
  mode_req_sub(2);
  mes_group_23_draw_on_flag_800();
  return;
}

int game_coin_prompt_state(void)
{
  e_mode_main main_mode;
  int sub_mode;
  main_mode = mode_get_main();
  sub_mode = mode_get_sub();
  if (main_mode != MODE_MAIN_GAME) return -1;
  if (sub_mode == 10) return 1;
  if ((uint)(sub_mode - 13) < 2) return 0;
  if ((game_flags & 0xc00) == 0x400) return 0;
  if (msnm_active() != 0 && (game_flags & 0x20000) != 0) return 0;
  return 1;
}

void ending_disp(void)
{
  int skip;
  
  ending_rep_run();
  ending_roll_draw(1);
  skip = ending_skip_requested();
  if (skip != 0) {
    loads_cancel_all();
    mode_timer = 0;
  }
  if (mode_timer == 0x150) {
    bgm_fade_out_start();
  }
  if (mode_timer < 1) {
    screen_fill_black();
    task_tree_stop();
    mode_req_sub(3);
    mode_timer = 2;
    mode_timer_float = 2.0;
    mode_timer_rate = 1.0;
  }
  return;
}

void demo_match_pick(void)
{
  typedef struct { char row_tail[4]; char enabled; char next_row_stage; } demo_ent;
  uint *sound_state;
  char *e;
  char i;
  char x1;
  char x2;
  uchar a;
  uchar b;
  uint u1;
  uint u2;
  int n;

  sound_state = (uint *)sound_state_get();
  demo_kind += 1;
  demo_kind %= 2;
  switch ((char)adv_demo_sw) {
  case 1:
    demo_kind = 1;
    break;
  case 2:
    demo_kind = 0;
    break;
  }
  if (demo_kind & 1) {
    n = 15;
    a = (int)lcg_next15() % n;
    b = (int)lcg_next15() % n;
    if (a == b) {
      b = b + 1;
    }
    { char *tb = (char *)&demo_charas;
    demo_chara_a = tb[a];
    demo_chara_b = tb[b]; }
    x1 = (int)lcg_next15() % 2;
    x2 = (int)lcg_next15() % 2;
    {
      char *t;
      demo_stage_no = (t = (char *)&demo_stage_cycle)[demo_stage_index++];
      if (demo_stage_no == -1) {
        demo_stage_index = 1;
        demo_stage_no = t[0];
      }
    }
    stage_sound_banks_load_own(demo_stage_no,1);
  }
  else {
    for (i = 0; i < 3; i++) {
      demo_type += 1;
      demo_type %= 3;
      demo_ent *t = (demo_ent *)((char *)&adva_stage + 1);
      if (t[demo_type].enabled != 0) break;
    }
    e = (char *)&adva_stage + demo_type * 6;
    u1 = lcg_next15();
    u2 = lcg_next15();
    if (e[0] < 0) {
      demo_stage_no = (&demo_stages_random)[u1 % 14];
    }
    else {
      demo_stage_no = e[0];
    }
    if (e[1] < 0) {
      demo_chara_a = (&demo_charas_random)[u1 % 15];
    }
    else {
      demo_chara_a = e[1];
    }
    if (e[3] < 0) {
      demo_chara_b = (&demo_charas_random)[u2 % 15];
    }
    else {
      demo_chara_b = e[3];
    }
    if (e[2] < 0) {
      x1 = (int)u1 % 2;
    }
    else {
      x1 = chara_costume_swap[demo_chara_a] ^ e[2];
    }
    if (e[4] < 0) {
      x2 = (int)u2 % 2;
    }
    else {
      x2 = chara_costume_swap[demo_chara_b] ^ e[4];
    }
    if ((demo_chara_a == demo_chara_b) & (x1 == x2)) {
      x2 = (x1 + 1) % 2;
    }
    stage_sound_banks_load_attract(demo_stage_no,1);
  }
  stage_extras_setup(demo_chara_a,x1,demo_chara_b,x2,demo_stage_no);
  chara_sound_banks_load(demo_chara_a,demo_chara_b,1);
  *sound_state = *sound_state | 0x10000;
  loads_cancel_all();
  rob_chara_load_queued(demo_chara_a,x1,0);
  rob_chara_load_queued(demo_chara_b,x2,1);
  if (demo_kind & 1) {
    stage_load_request(demo_stage_no);
  }
  else {
    stage_load_request(demo_stage_no);
  }
  demo_match_word = 0;
}

void title_init(void)
{
  uint *sound_state;
  
  sound_state = (uint *)sound_state_get();
  if (demo_kind == '\0') {
    mode_timer_float = 240.0;
    mode_timer = 0xf0;
    mode_req_sub(5);
  }
  else if (title_load_started == '\0') {
    screen_fill_black();
    task_tree_stop();
    bgm_sound_off();
    title_load_started = 1;
    mode_timer_float = 2.0;
    mode_timer = 2;
  }
  else if (mode_timer <= 0) {
    if (mode_timer == 0) {
      stage_screens_free();
      objbank_free(3);
      rank_sprite_sets_free();
      attract_stage_se_load(1);
      *sound_state = *sound_state | 0x10000;
    }
    else if (poly_queue_busy_get() == 0 && load_pending_get() == 0 && (*sound_state & 0x10000) == 0) {
      title_load_started = '\0';
      bg_color_set(bg_color_presets[0]);
      ftttl_scenes_load();
      attract_sound_play(0x379);
      task_start(&task_camera);
      task_start(&task_title_disp);
      mode_timer_float = 960.0;
      mode_timer = 0x3c0;
      mode_req_sub(5);
    }
  }
}

void credit_display_frame(void)
{
  typedef struct { int credits; int coins; int r[2]; } pl_ent;
  char i;
  credit_copy_rec *p;
  pl_ent *e;

  if (!((mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() != 6) ||
        (mode_get_main() == MODE_MAIN_SELECTOR && mode_get_sub() == 3) ||
        mode_get_main() == MODE_MAIN_GAME ||
        (mode_get_main() == MODE_MAIN_OVER && mode_get_sub() == 1))) return;
  sprite_colors_set(0xffff00,0);
  p = &credit_copy;
  if (p->separate == 0) {
    e = (pl_ent *)p;
    if (mode_get_main() == MODE_MAIN_SELECTOR || mode_get_main() == MODE_MAIN_GAME) {
      if (credit_copy.credit_mode != 1 && e->credits + e->coins == 0) {
        if (mode_get_main() == MODE_MAIN_GAME) {
          if (mode_get_sub() == 10) {
            if (credit_blink_count & 0x40) insert_coin_text_draw(0);
          }
          else if ((int)game_coin_prompt_state() > 0) {
            if (credit_blink_count & 0x18) insert_coin_text_draw(0);
          }
        }
        else if (credit_blink_count & 0x18) insert_coin_text_draw(0);
      }
      else if (mode_get_main() == MODE_MAIN_GAME) {
        if ((int)game_coin_prompt_state() > 0) {
          if (credit_blink_count & 0x40) insert_coin_text_draw(0);
          else credit_text_draw(0);
        }
        else credit_text_draw(0);
      }
      else if (credit_blink_count & 0x40) insert_coin_text_draw(0);
      else credit_text_draw(0);
    }
    else if (mode_get_main() == MODE_MAIN_ADVERTISE) {
      credit_text_draw(0);
      if (credit_blink_count & 0x18) insert_coin_text_draw(0);
    }
    else if (mode_get_main() == MODE_MAIN_OVER) {
      if (credit_blink_count & 0x40) insert_coin_text_draw(0);
      else credit_text_draw(0);
    }
    else credit_text_draw(0);
  }
  else {
    for (i = 0; i < 2; i++) {
      e = &((pl_ent *)&credit_copy)[i];
      if (credit_copy.credit_mode != 1 && e->credits + e->coins == 0) {
        if (mode_get_main() == MODE_MAIN_GAME) {
          if (mode_get_sub() == 10) {
            if (credit_blink_count & 0x40) insert_coin_text_draw(i);
          }
          else if ((int)game_coin_prompt_state() > 0) {
            if (credit_blink_count & 0x18) insert_coin_text_draw(i);
          }
        }
        else if (credit_blink_count & 0x18) insert_coin_text_draw(i);
      }
      else if (mode_get_main() == MODE_MAIN_GAME) {
        if ((int)game_coin_prompt_state() > 0 && (credit_blink_count & 0x40)) insert_coin_text_draw(i);
        else credit_text_draw(i);
      }
      else if (credit_blink_count & 0x40) {
        if (credit_blink_count & 0x18) insert_coin_text_draw(i);
      }
      else credit_text_draw(i);
    }
  }
  sprite_colors_reset();
  if (mode_get_main() == MODE_MAIN_GAME && mode_get_sub() == 10) {
    win_streak_draw(0,credit_blink_count);
    win_streak_draw(1,credit_blink_count);
  }
  credit_blink_count = credit_blink_count + 1;
  if (credit_event_pop() != 0) {
    credit_blink_count = 0;
  }
}

void player_join_frame(void)
{
  char flags[4];
  char str[20];
  int on[2];
  char cur_main;
  char cur_sub;
  char cnt;
  uint mask;
  int bits;
  int i;
  char *p;
  float x;
  float y;
  int align;

  memcpy(flags, side_entry_flags, 3);
  *(uint *)flags &= ~0x800;
  *(uint *)flags &= ~8;
  if (main_mode_req >= 0) cur_main = main_mode_req; else cur_main = main_mode_no;
  if (sub_mode_req >= 0) cur_sub = sub_mode_req; else cur_sub = mode_get_sub();
  on[0] = on[1] = 0;
  if (play_style == 2 && (game_flags & 0x400) == 0) {
    credit_select_price(0, on[0]);
    credit_select_price(1, on[1]);
    credit_snapshot((int)&credit_copy);
    memcpy(side_entry_flags, flags, 3);
    return;
  }
  if (streak_limit_hit != 0) return;
  switch (cur_main) {
  case 1:
    if (cur_sub <= 1) return;
    *(uint *)flags &= ~4;
    *(uint *)flags &= ~0x400;
    *(uint *)flags |= 0x808;
    break;
  case 3:
    if ((game_flags & 0xc00) == 0x400) return;
    if (game_flags & 0x20000) return;
    if (cur_sub <= 4) return;
    if (cur_sub == 12 || cur_sub == 13) return;
    if ((play_style == 2) & (cur_sub == 14)) return;
    if (continue_timer > 0) {
      on[1] = 1;
      on[0] = 1;
      if (cur_sub != 12 && cur_sub != 13 && cur_sub != 14) {
        if (((char *)&coin_added)[champion_side ^ 1] != 0) continue_timer = 599;
        else continue_timer--;
        if (msnm_active() != 0 && (cur_sub == 9 || cur_sub == 10)) {
          if (champion_side == 1) { x = 60.0; align = 0; } else { x = 590.0; align = 2; }
          y = 92.0;
        }
        else {
          if (champion_side == 1) { x = 18.0; align = 0; } else { x = 621.0; align = 2; }
          y = 72.0;
        }
        sprintf(str, "CONTINUE? %d", continue_timer / 60);
        sprite_colors_set(0xffff00, 0);
        sprite_font_text_put(&ascii_font, str, x, y, align, 0.0001);
        sprite_colors_reset();
      }
      else if (play_style == 2) return;
    }
  case 2:
    if (cur_sub == 11) return;
    if (champion_side == 0) {
      *(uint *)flags &= ~0x400;
      *(uint *)flags |= 0x800;
    }
    else {
      *(uint *)flags &= ~4;
      *(uint *)flags |= 8;
    }
    break;
  case 5:
    if (cur_sub != 0 && cur_sub != 1) return;
    if (champion_side == 0) {
      *(uint *)flags &= ~0x400;
      *(uint *)flags |= 8;
      on[0] = 1;
    }
    else {
      *(uint *)flags &= ~4;
      *(uint *)flags |= 0x800;
      on[1] = 1;
    }
    break;
  }
  credit_select_price(0, on[0]);
  credit_select_price(1, on[1]);
  mask = 0;
  cnt = 0;
  bits = start_pressed_mask();
  for (i = 0; i < 2; i++) {
    if ((flags[i] >> 3 & 1) && (bits >> i & 1) && card_entry_allowed(i) && credit_try_spend(i, on[i])) {
      flags[i] |= 4;
      mask = (uchar)(mask | 1 << i);
      cnt++;
    }
  }
  credit_snapshot((int)&credit_copy);
  memcpy(side_entry_flags, flags, 3);
  switch (cnt) {
  case 1:
    if ((play_style == 0) | (cur_main == 1)) {
      champion_side = (mask & 1) == 0;
      ((struct { char a[3]; } *)side_entry_flags)->a[(uchar)champion_side] |= 0x10;
      play_style = 1;
      mode_req_main(MODE_MAIN_SELECTOR);
      mode_req_sub(0);
    }
    else if (cur_main == 5) {
      uint pl;
      uint t = ~mask;
      pl = t & 1;
      user_back_data.bookkeep.continues++;
      if ((user_back_data.bookkeep.shortest_1p >= p1_play_frames_min || user_back_data.bookkeep.shortest_1p == 0) && p1_play_frames_min != 0)
        user_back_data.bookkeep.shortest_1p = p1_play_frames_min;
      if (user_back_data.bookkeep.longest_1p < p1_play_frames_max)
        user_back_data.bookkeep.longest_1p = p1_play_frames_max;
      p1_play_frames_max = 0;
      p1_play_frames_min = 0;
      game_flags |= 0x40000;
      play_style = 1;
      mode_req_main(MODE_MAIN_OVER);
      mode_req_sub(5);
      continue_mes_free();
      net_player_continue(pl);
    }
    else {
      if (cur_main == 2) {
        play_style = cur_main;
        mode_req_main(MODE_MAIN_SELECTOR);
        if (mode_get_sub() > 1) mode_req_sub(2);
      }
      else {
        if (cur_main == 3 && mode_get_sub() <= 3) mode_req_main(MODE_MAIN_SELECTOR);
        else mode_req_main(MODE_MAIN_INTRUDE);
        mode_req_sub(0);
        play_style = 2;
        game_flags &= ~0xc00;
      }
      if (mask & 1) p = &side_entry_flags[0];
      else p = &side_entry_flags[1];
      *p |= 0x10;
    }
    game_flags &= ~4;
    break;
  case 2:
    play_style = 2;
    champion_side = game_frame_count & 1;
    mode_req_main(MODE_MAIN_SELECTOR);
    mode_req_sub(0);
    game_flags &= ~4;
    side_entry_flags[0] |= 0x10;
    side_entry_flags[1] |= 0x10;
    break;
  }
}

void game_judge_init(void)
{
  char buf[24];
  card_image_rec *pa[2];
  card_record_rec *pb[2];
  rob_rec *rob0;
  rob_rec *rob1;
  rob_rec *win;
  rob_rec *lose;
  int prev_champion;
  net_info_t *net_info;
  uchar *card_state;
  struct { card_record_rec *a; card_record_rec *b; } pw = { 0, 0 };
  short life0;
  short life1;
  rob_rec *p;
  int i;
  rob_rec *champ;
  rob_rec *champ_opp;
  int win_side;
  int lose_side;
  int lottery;
  int pending;
  int redeem;
  int entry_flags;
  int other;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  life0 = rob0->life;
  life1 = rob1->life;
  win = rob0;
  lose = rob1;
  card_state = card_service_state_get();
  pa[0] = card_image_get(0);
  pa[1] = card_image_get(1);
  pb[0] = card_record_get(0);
  pb[1] = card_record_get(1);
  net_info = net_info_get();
  streak_limit_hit = 0;
  prev_champion = champion_side;
  item_plate_hidden_clear();
  if (life0 < life1) {
    win = rob1;
    lose = rob0;
  }
  rep_mark_set();
  round_stats_record();
  if (game_flags & 0x20) {
    if (life0 > 0) {
      if (match_energy == life0) game_flags |= 0x80;
      round_win_add(0);
    }
    else if (life1 > 0) {
      if (match_energy == life1) game_flags |= 0x80;
      round_win_add(1);
    }
    else {
      round_draw_add();
      game_flags |= 0x100;
    }
  }
  else if (game_flags & 0x40) {
    if (rob0->move_no == 0x8b5) round_win_add(0);
    else if (rob1->move_no == 0x8b5) round_win_add(1);
    else if (rob0->flags & 0x100) {
      if (rob1->flags & 0x100) {
        round_draw_add();
        game_flags |= 0x100;
      }
      else round_win_add(1);
    }
    else round_win_add(0);
  }
  else {
    if (life0 > life1) round_win_add(0);
    else if (life0 < life1) round_win_add(1);
    else {
      round_draw_add();
      game_flags |= 0x100;
    }
  }
  if (msnm_active() && msnm_mission60_full_life()) game_flags |= 0x80;
  vp_judge_init(game_flags & 0x400);
  if (game_flags & 0x100) user_back_data.bookkeep.draw_rounds++;
  if (game_flags & 0x80) round_end_kind = 2;
  else if (game_flags & 0x40) round_end_kind = 4;
  else if (game_flags & 0x10) round_end_kind = 3;
  else if (game_flags & 0x100) round_end_kind = 0;
  else round_end_kind = 1;
  sound_play_id(judge_sound_ids[round_end_kind * 2], 3);
  if (game_flags & 0x10) audience_mood_set(3);
  else if (game_flags & 0x80) audience_mood_set(2);
  else audience_mood_set(1);
  judge_time = 180;
  if (game_flags & 0x40) judge_time += 60;
  if (game_flags & 0x40) judge_rep_fraction = 0.7f;
  else if ((game_flags & 0x110) == 0 && ((rob_get((uchar)round_winner == 0))->state & 0x200000)) judge_rep_fraction = 0.7f;
  else judge_rep_fraction = 0.62f;
  rep_judge_pre_frames = (short)(judge_rep_fraction * 240.0f);
  rep_judge_post_frames = 240 - rep_judge_pre_frames;
  mode_timer = judge_time;
  mode_timer_float = (float)mode_timer;
  mode_req_sub(0xd);
  vs_stats_round_end();
  mission_judge();
  jisaku_round_end();
  quest_judge();
  bonus_item_judge();
  if (msnm_active()) {
    msnm_judge_init();
    msnm_fight_draw();
  }
  else {
    cns_nn_players_draw(0);
  }
  rob_card_level_apply(2);
  if (game_flags & 0x400) {
    nickname_judge();
    if (play_style == 1 && msnm_active()) {
      alt_mode_match_end();
    }
    else if (play_style == 1) {
      p = rob0;
      if ((rob0->flags & 4) == 0) p = rob1;
      course_time_total = match_time_frames + round_time_frames + course_time_total;
      match_time_frames = 0;
      round_time_frames = 0;
      if ((char)rank_round > 9) {
        if (*(int *)(card_state + 4) == 1) {
          card_cpu_play_end();
          if (net_is_on() && play_style == 1) net_game_end_send_cpu();
        }
        else if (card_close_phase_get() == 1) {
          card_cpu_play_end();
          card_close_phase_set(2);
          if (net_is_on() && play_style == 1) net_game_end_send_cpu();
        }
        else {
          if (net_is_on() && play_style == 1) net_game_end_send_cpu();
          pw.a = card_record_get(0);
          pw.b = card_record_get(1);
          pw.a->in_use = 0;
          pw.b->in_use = 0;
        }
      }
      else {
        if (card_close_phase_get() == 1) {
          card_cpu_play_end();
          card_close_phase_set(2);
          if (net_is_on() && play_style == 1) net_game_end_send_cpu();
        }
      }
      if (game_flags & 0x800) {
        rank_round++;
        item_set_redeem_start(champion_side);
        if ((char)rank_round > 10) {
          game_flags |= 0x20000;
          ranking_chara = p->chara;
          clear_stats_add(p);
          if (card_body_name_empty(&pa[p->side]->body)) snprintf(buf, 21, no_name_wide);
          else card_body_name_get(&pa[p->side]->body, buf, 21);
          point_ranking_insert((char)p->chara, win_streak, buf);
          win_streak = 0;
        }
        else {
          cpu_opponent_pick((char)rank_round);
        }
      }
      else {
        lose_stats_add(p);
        if (card_body_name_empty(&pa[p->side]->body)) snprintf(buf, 21, "NO-NAME");
        else card_body_name_get(&pa[p->side]->body, buf, 21);
        point_ranking_insert((char)p->chara, win_streak, buf);
        win_streak = 0;
      }
    }
    else {
      prize_miss_count_update();
      if (net_is_on() && net_service_active() && net_info->nam_state == 0) {
        for (i = 0; i < 2; i++) {
          if (pb[i]->in_use) {
            pa[i]->header.flags |= 8;
            pa[i]->body.misc.f.equip_flag0 = 1;
            pa[i]->body.misc.f.equip_flag1 = 1;
            pa[i]->body.misc.f.color_flag0 = 1;
            pa[i]->body.misc.f.color_flag1 = 1;
          }
        }
      }
      vs_time_stats_add();
      streak_challenge_state = 1;
      other = (uchar)champion_side == 0;
      entry_flags = (int)side_entry_flags;
      *(char *)(entry_flags + other) &= 0xf8;
      cpu_opponent_pick((char)rank_round);
      champ = rob_get((uchar)champion_side);
      champ_opp = champ->opp;
      win_side = 0;
      lose_side = 1;
      if ((uchar)champion_side == 1) {
        win_side = 1;
        lose_side = 0;
      }
      if (!net_free_play_is_on()) {
        item_set_steal(win_side, lose_side);
        redeem = item_set_redeem_start(win_side);
        item_lottery_judge(champion_side);
        lottery = item_lottery_result_get(champion_side);
        pending = item_pending_redeem(champion_side);
        item_pending_save_mark();
        item_award_rules_check(0, (rob_get(0))->field_0x3);
        item_award_rules_check(1, (rob_get(1))->field_0x3);
        if (redeem == 0) {
          if (lottery != 0 || pending != 0) {
            if (lottery != 0) item_plate_hidden_set(win_side, lottery);
            if (pending != 0) item_plate_hidden_set(win_side, pending);
            item_award_mes_start(win_side, 0);
          }
          card_item_log_lottery();
        }
      }
      card_match_result_record(rob0, rob1, win_side, lose_side);
      for (i = 0; i < 2; i++) {
        if (card_unit_used_get(i) && pb[i]->in_use) {
          card_mode_start_result(i);
          if (card_service_mode_get() == 2) card_close_phase_set(2);
          card_item_log_drop_player((char)i);
        }
      }
      card_logs_settle();
      user_back_data.bookkeep.chara_play_counts[champ_opp->field_0x3][champ_opp->chara]++;
      user_back_data.bookkeep.games_total++;
      user_back_data.bookkeep.games_vs++;
      user_back_data.bookkeep.vs_chara_counts[champ->chara][champ_opp->chara]++;
      if (champion_side == prev_champion) {
        win_streak++;
        if (win->chara == 2 && lose->chara == 2) lau_vs_lau_wins[champion_side]++;
      }
      else {
        if (card_body_name_empty(&pa[lose->side]->body)) snprintf(buf, 21, "NO-NAME");
        else card_body_name_get(&pa[lose->side]->body, buf, 21);
        point_ranking_insert((char)lose->chara, win_streak, buf);
        win_streak = 1;
      }
      if (user_back_data.rankings.best_win_streak[win->chara] < win_streak)
        user_back_data.rankings.best_win_streak[win->chara] = win_streak;
      continue_timer = user_back_data.no_continue ? 0 : 599;
      if (win_streak >= user_back_data.win_streak_limit && user_back_data.win_streak_limit > 0) {
        streak_limit_hit = 1;
        continue_timer = 0;
      }
      if (streak_limit_hit) {
        user_back_data.bookkeep.chara_play_counts[win->field_0x3][win->chara]++;
        if (card_body_name_empty(&pa[win->side]->body)) snprintf(buf, 21, "NO-NAME");
        else card_body_name_get(&pa[win->side]->body, buf, 21);
        point_ranking_insert((char)win->chara, win_streak, buf);
        win_streak = 0;
        user_back_data.bookkeep.games_total++;
        user_back_data.bookkeep.games_vs++;
        net_player_end(0);
        net_player_end(1);
        for (i = 0; i < 2; i++) {
          if (card_unit_used_get(i) && pb[i]->in_use) {
            char *q = (char *)&card_status;
            if (q[i * 8]) {
              card_mode_start_result(i);
              if (card_service_mode_get() == 2) card_close_phase_set(2);
            }
            else {
              pb[i]->in_use = 0;
              card_message_post((char)i, 5, 0x3c, 0);
            }
          }
        }
      }
      else {
        net_player_end((uchar)champion_side == 0);
      }
    }
  }
  user_back_mark_dirty();
  judge_mes_draw();
}

void coin_insert_watch(void)
{
  typedef struct { char credits; char c1; char c2; char c3; char coins; char pad[11]; } plent;
  typedef struct { plent pl[4]; int credit_mode; int separate; } credit_snap;
  int event;
  int row0;
  int row1;

  credit_snapshot((int)&credit_copy);
  event = credit_event_pop();
  if (event == 1) {
    sound_play_id(0x3ab,0);
  }
  else if (event == 2) {
    sound_play_id(0x3ac,0);
  }
  coin_added.side1 = 0;
  coin_added.side0 = 0;
  if (((credit_snap *)&credit_copy)->credit_mode != 1) {
    if (((credit_snap *)&credit_copy)->separate == 0) {
      row0 = 0;
      row1 = 0;
    }
    else {
      row0 = 0;
      row1 = 1;
    }
    if (coin_count_prev.side0 < ((credit_snap *)&credit_copy)->pl[row0].coins) {
      coin_added.side0++;
    }
    if (coin_count_prev.side1 < ((credit_snap *)&credit_copy)->pl[row1].coins) {
      coin_added.side1++;
    }
    if (credit_count_prev.side0 < ((credit_snap *)&credit_copy)->pl[row0].credits) {
      coin_added.side0++;
    }
    if (credit_count_prev.side1 < ((credit_snap *)&credit_copy)->pl[row1].credits) {
      coin_added.side1++;
    }
    coin_count_prev.side0 = ((credit_snap *)&credit_copy)->pl[row0].coins;
    coin_count_prev.side1 = ((credit_snap *)&credit_copy)->pl[row1].coins;
    credit_count_prev.side0 = ((credit_snap *)&credit_copy)->pl[row0].credits;
    credit_count_prev.side1 = ((credit_snap *)&credit_copy)->pl[row1].credits;
  }
  if (((coin_added.side0 != 0) || (coin_added.side1 != 0)) &&
     (0 < continue_timer)) {
    continue_timer = 599;
  }
  return;
}
