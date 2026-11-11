/*
 * vf4lib/libbase/src/system.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint game_main_arg0; /* 0x0c243000 */
extern uint game_main_arg1; /* 0x0c243004 */
extern uint render_done_seen; /* 0x0c24300c */
extern uint render_done_usec; /* 0x0c243010 */
extern uint test_exit_state; /* 0x0c243018 */
extern int test_exit_card_wait; /* 0x0c24301c */
extern int game_frame_count; /* 0x0c292540 */
extern int vblank_count; /* 0x0c292544 */
extern uint mode_frame_count; /* 0x0c29254c */
extern uint game_flags; /* 0x0c2925b8 */
extern uint poly_shadow_scale; /* 0x0c3226a0 */
extern static_heap_rec motcm_load_addr; /* 0x0c36000c */
extern uint pause_flags; /* 0x0c360094 */
extern uchar *board_config; /* 0x0c360178 */
extern static_heap_rec motep_load_addr; /* 0x0c360198 */
extern char card_unit_used[2]; /* 0x0c37e3dc */
extern system_param_value_rec system_param_value; /* 0x0c3601ec */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

int game_main(uint arg0, uint arg1)
{
  int again;

  game_main_arg0 = arg0;
  game_main_arg1 = arg1;
  hardware_boot();
  game_boot();
  do {
    again = game_loop_enter();
  } while (again != 0);
  game_main_tail_a();
  game_main_tail_b();
  return 0;
}

int pause_pad_step(player_input_rec *pad)
{
  uint flags = pause_flags;
  uint pressed = pad->pressed;
  uint now = pad->now;
  int acted = 0;
  if (!(flags & 1)) return 0;
  if (flags & 0x10) {
    if ((now & 0x8000) && (pressed & 1)) { flags &= 0xffffffeb; acted = 1; }
    else if ((pressed & 0x8000) && (flags & 0x40)) { flags &= 0xfffffffb; acted = 1; }
    else { flags |= 4; acted = 0; }
  }
  else {
    if ((now & 1) && (pressed & 0x8000)) { flags |= 0x10; acted = 1; }
  }
  pause_flags = flags;
  return acted;
}

void pause_pads_poll(void)
{
  player_input_rec *pad;
  int acted;

  if ((pause_flags & 0x41) == 0x41) {
    pad = player_input_get(0);
    acted = pause_pad_step(pad);
    if (acted == 0) {
      pad = player_input_get(1);
      pause_pad_step(pad);
    }
  }
  return;
}

uint pause_frame_held(void)
{
  return pause_flags & 4;
}

uint debug_info_enabled(void)
{
  return pause_flags & 2;
}

uint pause_is_on(void)
{
  return pause_flags & 0x10;
}

void hardware_boot(void)
{
  typedef struct { int w[5]; } blk;
  int lib_mode;
  unsigned int sr;

  memset(&system_param_value,0,0x58);
  system_param_value.field_0x0 = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  sys_bring_up_vectors_timer();
  board_prestart();
  board_config = (uchar *)board_config_ptr();
  ((struct { int pad[5]; blk b; } *)&system_param_value)->b = *(blk *)board_config;
  lib_mode = 0;
  if (((*(int *)((int)board_config + 0xc) == 0) && (*(char *)((int)board_config + 0x10) != '\0')) &&
     (*(char *)((int)board_config + 0x11) != '\0')) {
    lib_mode = 9;
  }
  naomi2_lib_start(lib_mode,0x80,1);
  { unsigned int e;
  __asm__ volatile ("stc sr,%0" : "=r" (e));
  e = e & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (e)); }
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  sys_bring_up_buses();
  board_start();
  dimm_request_hook_install();
  input_init();
  irq_slots_install_naomi();
  irq_slot_fn_install(1,(uint)game_vblank_in);
  irq_slot_fn_install(0,(uint)game_render_done);
  { unsigned int e;
  __asm__ volatile ("stc sr,%0" : "=r" (e));
  e = e & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (e)); }
  tmu0_restart_usec_scale();
  mtx_stacks_start(mtx_stack_buffer[0],0x20);
  cart_control_write(1,0);
  poly_system_init();
  text_glyphs_set_source(text_glyph_pixels);
  text_overlay_setup();
  text_newline_keeps_margin();
}

void game_boot(void)
{
  int wait_frames;
  int i;
  int wait;
  net_info_t *info;
  int net_on;
  uint log_word;
  tm now;
  int *times;
  unsigned int sr;
  unsigned int sr2;
  
  system_param_country_init();
  pause_flags = 0;
  game_frame_count = 0;
  mode_frame_count = 0;
  game_boot_word = 0;
  game_flags = 0;
  rope_cloth_gravity = 0.0029944447;
  world_gravity = 0.0029944447;
  energy_1p_setting = energy_table_1p[(char)user_back_data.energy_1p];
  energy_vs_setting = energy_table_vs[(char)user_back_data.energy_vs];
  times = round_time_table;
  round_time_1p_setting = times[(char)user_back_data.game_time_1p];
  round_time_vs_setting = times[(char)user_back_data.game_time_vs];
  static_heaps_alloc_all();
  effect_heap_init();
  debug_boot_stub();
  stage_screens_boot();
  user_back_init();
  player_input_init();
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  file_system_init();
  __asm__ volatile ("stc sr,%0" : "=r" (sr2));
  sr2 = sr2 & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr2));
  sprite_sets_init();
  objbanks_save();
  objbanks_restore();
  obj_file_sizes_read();
  rep_highlights_clear();
  camera_task_start();
  media_probe_and_ids();
  card_boot_nop();
  boot_motion_load();
  common_obj_load();
  win_sys_init();
  task_tree_init();
  fft16_twiddles_build();
  stage_screens_clear();
  mode_init();
  robs_boot_init();
  robs_match_clear();
  rob_item_slots_init();
  vp_boot_init();
  light_sets_init();
  make_old_task_start();
  net_status_clear();
  card_logs_settle();
  aet_init();
  msnm_boot_init();
  nickname_boot_init();
  cns_nn_boot();
  get_now_tm(&now);
  lcg_seed_set(now.tm_mon * now.tm_min + now.tm_mday * now.tm_sec +
               now.tm_hour * now.tm_wday);
  input_enabled = 1;
  bg_plane_corner_colors(0,0,0);
  ledger_daily_baseline();
  led_write_if_idle(0x55);
  if (net_init((int)(char)user_back_data.net_connect) == 0) {
    dimm_firmware_error_halt();
  }
  wait_frames = user_back_data.net_timeout;
  if (net_is_on() == 0) {
    wait_frames = 2;
  }
  for (i = 0; i < wait_frames; i++) {
    info = net_info_get();
    if (info->auth_stat != 0) break;
    ledger_daily_update();
    text_cursor_to_cell(5,0xf);
    text_draw_formatted("% 4d / %d",i,wait_frames);
    text_cursor_to_cell(10,0xc);
    text_draw_formatted("%s","NOW NETWORK CHECKING");
    input_per_frame();
    io_board_error_check();
    net_frame_service();
    test_switch_exit_to_bios();
    sys_frame_end();
  }
  net_on = net_is_on();
  log_word = system_param_byte0_get();
  net_log_status_write(net_on,log_word,i,wait_frames);
  net_select_pending_clear();
  wait = 0x708;
  do {
    ledger_daily_update();
    input_per_frame();
    io_board_error_check();
    test_switch_exit_to_bios();
    net_frame_service();
    net_log_current_draw();
    text_cursor_to_cell(0x1b,0x36);
    text_draw_formatted("PLEASE WAIT 30 SECONDS\n");
    text_draw_formatted("OR START BUTTON TO EXIT");
    if ((player_input_get(0)->pressed & 0x8000) != 0) break;
    if ((player_input_get(1)->pressed & 0x8000) != 0) break;
    if (wait-- <= 0) break;
    sys_frame_end();
  } while (wait != 0);
  if (alab_enable == OFF_ON_OFF) {
    alab_step_disable();
  }
  return;
}

int game_loop_enter(void)
{
  ((int (*)(void))game_frame_loop)();
  return 0;
}

void game_main_tail_a(void)
{
  return;
}

void game_main_tail_b(void)
{
  return;
}

void game_frame_loop(void)
{
  for (;;) {
    game_flags = game_flags | 1;
    *(float *)((char *)&graphics_work + 0x20) = game_far_z;
    user_back_flush();
    mtx_load_identity();
    clip_rect_store(0,0,0x13,0xe);
    clip_rect_apply(0);
    input_per_frame();
    io_board_error_check();
    test_switch_exit_to_bios();
    pause_pads_poll();
    win_sys_update();
    rope_stage_on = 0;
    mode_frame();
    adv_light_blend_step();
    task_tree_frame();
    sound_frame_update();
    play_time_account();
    perf_color_mark(0x9090c0);
    net_frame_service();
    perf_color_mark(0xffffff);
    win_sys_draw();
    clip_rect_apply(0);
    load_service();
    load_poly_bg_service();
    sound_bank_queue_poll();
    cpu_usage_capture();
    perf_false_count();
    perf_color_mark(0x3f3f3f);
    render_done_wait();
    perf_color_mark(0);
    clip_rect_clear();
    frame_parity = frame_parity ^ 1;
    sys_frame_end();
    perf_frame_stats_update();
    ((int (*)())stopwatch_usec_dropped)();
    stopwatch_start();
    backup_flush();
    gfx_counters_snapshot();
    frame_hook_empty();
    game_frame_count = game_frame_count + 1;
  }
}

void error_screen_frame(void)
{
  input_per_frame();
  sys_frame_end();
  return;
}

int game_vblank_in(void)
{
  vblank_count = vblank_count + 1;
  if (input_enabled != '\0') {
    frame_io_update();
    card_units_poll();
    input_read();
  }
  coin_insert_watch();
  return 0;
}

int game_render_done(void)
{
  int min;
  render_done_seen = 1;
  render_done_usec = stopwatch_usec();
  hdr_or_color_clamp(color_clamp_on);
  modvol_shadow_scale_put(poly_shadow_scale);
  min = (int)fog_clamp_min_a << 0x18 | (uint)fog_clamp_min_r << 0x10 | (uint)fog_clamp_min_g << 8 |
               (uint)fog_clamp_min_b;
  fog_clamp_regs_write((int)fog_clamp_max_a << 0x18 | (uint)fog_clamp_max_r << 0x10 | (uint)fog_clamp_max_g << 8 |
               (uint)fog_clamp_max_b, min);
  pt_alpha_ref_write_both((uint)pt_alpha_ref);
  return 0;
}

void render_done_wait(void)
{
  int *counters;
  while (render_done_seen == 0) {
    if ((int)stopwatch_usec() > 0x3e80) break;
  }
  render_done_seen = 0;
  counters = gfx_frame_counters;
  gfx_counters_since_snapshot(counters);
  counters[5] = counters[0] + counters[1] + counters[2] + counters[3] + counters[4];
  ta_param_area_size = frame_stats_ta_param_area_size();
  ta_param_used = frame_stats_ta_param_used();
  if ((task_fog.flags & 3) == 3) {
    fog_table_regs_write(fog_table);
  }
}

void io_board_error_check(void)
{
  uint errors;
  char *message;

  errors = input_error_flags_get();
  if (errors != 0) {
    sound_all_off();
    message = "unknown error";
    if (!((errors & 1) == 0)) {
      message = "I/O BD IS NOT CONNECTED TO NAOMI BD.\n\n";
    }
    else {
      if (!((errors & 6) == 0)) {
        message = "COM. ERROR OCCURRED\n\nBETWEEN NAOMI BD AND I/O BD.\n\n";
      }
      else {
        if ((errors & 8) != 0) {
          message = "I/O BD CONNECTED TO NAOMI BD\n\nDOES NOT FULFILL THE GAME SPECS.\n\n";
        }
      }
    }
    input_enabled = 1;
    bg_color_set(bg_color_presets[0]);
    obj_light_colour_put(1.0,1.0,1.0);
    do {
      if ((vblank_count & 0x18) != 0) {
        text_color_set(0xffe0e0e0);
        text_cell_size_set(0x10,0x10);
        text_cursor_to_cell(7,0xd);
        text_draw_formatted("%s",message);
        text_cell_size_set(8,8);
      }
      led_blink_update();
      test_switch_exit_to_bios();
      error_screen_frame();
    } while( true );
  }
  return;
}

void dimm_firmware_error_halt(void)
{
  sound_all_off();
  input_enabled = 1;
  bg_color_set(bg_color_presets[0]);
  obj_light_colour_put(1.0,1.0,1.0);
  do {
    if ((vblank_count & 0x18) != 0) {
      text_color_set(0xffe0e0e0);
      text_cell_size_set(0x10,0x10);
      text_cursor_to_cell(3,0xd);
      text_draw_formatted("%s",
                   "ERROR 14\nDIMM BD FIRMWARE VERSION DOES NOT\nFULFILL THE GAME SPEC.\nREQUIRED VERSION 3.17.\n\n"
                  );
      text_cell_size_set(8,8);
    }
    led_blink_update();
    test_switch_exit_to_bios();
    error_screen_frame();
  } while( true );
}

void test_switch_exit_to_bios(void)
{
  typedef struct { char a; uchar b; char p[6]; } ent;
  int i;

  switch (test_exit_state) {
  case 0:
    if (input_merged_pressed(0x10000004) == 0) break;
    test_exit_state = 1;
    for (i = 0; i < 2; i++) {
      if (card_unit_ok(i)) {
        char *c = card_unit_used;
        char z = 0;
        int *a = &test_exit_card_wait;
        c[i] = z;
        a[i] = 300;
      }
      else {
        int *a = &test_exit_card_wait;
        a[i] = 0;
      }
    }
    break;
  case 1:
    for (i = 0; i < 2; i++) {
      int *a = &test_exit_card_wait;
      int *q = &a[i];
      if (*q > 0) {
        if (card_unit_result(i) != 1) {
          ent *e = (ent *)&card_status;
          if (e[i].b == 0) { *q = 0; continue; }
        }
        {
          int *d = &test_exit_card_wait;
          d[i]--;
        }
      }
    }
    if ((&test_exit_card_wait)[0] <= 0 && (&test_exit_card_wait)[1] <= 0)
      test_exit_state = 2;
    break;
  }
  if (test_exit_state == 2) {
    test_exit_state = 0;
    net_shutdown();
    break_ab_clear();
    sound_all_off();
    g1_bus_hold();
    elan_regs_quiesce();
    bios_helper_44();
  }
}

void boot_motion_load(void)
{
  sound_boot();
  sound_tables_install();
  mothead_table_set((uchar **)motcm_load_addr.addr);
  mot_bank_load(3,'\x14');
  mot_bank_load(0,'\0');
  mothead_load(0,0);
  mot_dir_set((uchar *)motep_load_addr.addr + 8);
  mot_table2_set((int *)((char *)motep_load_addr.addr + *(int *)((char *)motep_load_addr.addr + 4)));
  return;
}
