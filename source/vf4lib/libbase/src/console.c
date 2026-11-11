/*
 * vf4lib/libbase/src/console.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_xy_colors_z_alpha
#define KRPROTO_sprite_put_flags_xyz
#define KRPROTO_sprite_put_xyz_scaled_alpha_angle_flags
#define KRPROTO_sprite_font_text_put
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float console_glow; /* 0x0c28fa8c */
extern uchar console_glow_down; /* 0x0c28fa90 */
extern card_icon_pos_rec card_icon_pos; /* 0x0c28ed38 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern int round_timer_digits[10]; /* 0x0c163470 */
extern int round_timer_low_digits[10]; /* 0x0c163498 */
extern int round_timer_small_digits[10]; /* 0x0c1634c0 */
extern int oldgauge_timer_digits[10]; /* 0x0c1634e8 */
extern int oldgauge_timer_small_digits[10]; /* 0x0c163510 */
extern int char_plate_sprites[19][2]; /* 0x0c163538 */
extern int quest_plate_sprites[19][2]; /* 0x0c1635d0 */
extern int judge_mes_group[5]; /* 0x0c163e9c */
extern int sprite_list_5f50[9]; /* 0x0c163eb4 */
extern short rank_char_sprites_adv[19][2]; /* 0x0c163ed8 */
extern short rank_char_sprites[19][2]; /* 0x0c163f24 */
extern int rank_char_name_sprites[19]; /* 0x0c163f70 */
extern float rank_time_digit_x; /* 0x0c163fbc */
extern short round_flash_sprites[18]; /* 0x0c163fe0 */
extern short char_side_textures[19][2]; /* 0x0c164004 */
extern short char_textures[19]; /* 0x0c164050 */
extern short char_name_sprites[19][2]; /* 0x0c164076 */
extern float rob_status_mark_x[2]; /* 0x0c1640c4 */
extern int rob_status_sprites_2[8]; /* 0x0c1640cc */
extern int rob_status_sprites_1[8]; /* 0x0c1640ec */
extern int win_streak_digit_sprites[10]; /* 0x0c16410c */
extern float level_badge_pos_2[2][2][2]; /* 0x0c164c5c */
extern float level_badge_pos_3[2][3][2]; /* 0x0c164c7c */
extern int card_digit_sprites[10]; /* 0x0c164cac */
extern int prize_banner_corners[18]; /* 0x0c164d28 */
extern int prize_anim_0[72]; /* 0x0c165230 */
extern int prize_anim_2[108]; /* 0x0c165350 */
extern int prize_anim_1[99]; /* 0x0c165500 */
extern int prize_anim_3[315]; /* 0x0c16568c */
extern int prize_banner_anim_g[180]; /* 0x0c165b78 */
extern char format_6d[4]; /* 0x0c21b3c4 */
extern undefined format_02d; /* 0x0c21b3c8 */
extern char quest_start_text[21]; /* 0x0c21b3d8 */
extern char quest_success_text[23]; /* 0x0c21b3f0 */
extern char quest_running_text[15]; /* 0x0c21b408 */
extern char quest_result_send_text[31]; /* 0x0c21b418 */
extern char quest_result_sending_text[27]; /* 0x0c21b438 */
extern char card_times_suffix[3]; /* 0x0c21b884 */
extern char card_no_name_text[15]; /* 0x0c21b888 */
extern char team_label_text[7]; /* 0x0c21b898 */
extern char drink_count_format[5]; /* 0x0c21b8b0 */
extern char format_03d[5]; /* 0x0c21b8b8 */
extern char prize_debug_1p_text[5]; /* 0x0c21b8f0 */
extern char prize_debug_item_format[16]; /* 0x0c21b8f8 */
extern char prize_debug_money_format[9]; /* 0x0c21b908 */
extern char prize_debug_2p_text[5]; /* 0x0c21b914 */
extern char prize_debug_focus_text[13]; /* 0x0c21b91c */
extern char prize_debug_none_text[9]; /* 0x0c21b92c */
extern char prize_debug_rank_format[10]; /* 0x0c21b938 */
extern uchar oldgauge_on; /* 0x0c28ed30 */
extern char prize_kind; /* 0x0c28ed31 */
extern char prize_fly_state; /* 0x0c28ed32 */
extern float card_panel_timer[2]; /* 0x0c28ed4c */
extern short mes_anim_timer[3]; /* 0x0c28ed58 */
extern short quest_mes_timer[2]; /* 0x0c28ed5e */
extern char quest_mes_text[2][35]; /* 0x0c28ed62 */
extern char emblem_browse_side; /* 0x0c28edb0 */
extern int sou_banks_loaded; /* 0x0c28edb4 */
extern int miss_bank_loaded; /* 0x0c28edb8 */
extern float mes_offset_y; /* 0x0c28edbc */
extern game_mes_rec game_mes_table[41]; /* 0x0c28edc0 */
extern int sega_disp_sprites[12]; /* 0x0c28f0f4 */
extern int sega_disp_sprites_2[9]; /* 0x0c28f124 */
extern int cri_disp_sprites[12]; /* 0x0c28f148 */
extern short rank_digits_adv[22]; /* 0x0c28f178 */
extern short rank_digits[22]; /* 0x0c28f1a4 */
extern float char_name_2p_x[19]; /* 0x0c28f1d0 */
extern float rob_debug_value_x; /* 0x0c28f21c */
extern float rob_debug_value_y; /* 0x0c28f220 */
extern float rob_debug_1p_x; /* 0x0c28f224 */
extern float rob_debug_1p_y; /* 0x0c28f228 */
extern float rob_debug_2p_x; /* 0x0c28f22c */
extern float rob_debug_2p_y; /* 0x0c28f230 */
extern win_streak_part_rec *win_streak_layouts[5][2]; /* 0x0c28f54c */
extern win_streak_part_rec *win_streak_layouts_overseas[5][2]; /* 0x0c28f6dc */
extern win_streak_part_rec *win_streak_other_layouts[4][2]; /* 0x0c28f80c */
extern win_streak_part_rec *win_streak_other_layouts_overseas[2]; /* 0x0c28f874 */
extern float win_streak_mark_x[2]; /* 0x0c28f87c */
extern float win_streak_player_mark_x[2]; /* 0x0c28f884 */
extern float *card_panel_row_sets[4]; /* 0x0c28f8ac */
extern float card_panel_text_x; /* 0x0c28f8bc */
extern char sel_arrow_dirs[2]; /* 0x0c28f8c0 */
extern int sel_arrow_objects[2]; /* 0x0c28f8c4 */
extern float card_type2_win_rate_dx; /* 0x0c28f8d8 */
extern float card_frame_mid_x; /* 0x0c28f8dc */
extern float card_frame_mid_unit; /* 0x0c28f8e0 */
extern float card_frame_mid_scale; /* 0x0c28f8e4 */
extern float card_frame_side_offsets[2][2]; /* 0x0c28f8e8 */
extern int card_frame_side_sprites[2]; /* 0x0c28f8f8 */
extern float card_rank_value_shift; /* 0x0c28f910 */
extern float card_rank_digits_x; /* 0x0c28f914 */
extern float card_rank_label_w; /* 0x0c28f918 */
extern float card_rank_title_x; /* 0x0c28f91c */
extern float card_name_underline_max; /* 0x0c28f920 */
extern float card_rank_title_char_w; /* 0x0c28f924 */
extern float card_team_name_x; /* 0x0c28f928 */
extern float card_count_06_digits_x; /* 0x0c28f92c */
extern float card_count_06_label_x; /* 0x0c28f930 */
extern float card_count_06_pad; /* 0x0c28f934 */
extern float card_count_08_digits_x; /* 0x0c28f938 */
extern float card_count_08_label_x; /* 0x0c28f93c */
extern float card_count_08_pad; /* 0x0c28f940 */
extern float card_win_rate_digits_x; /* 0x0c28f944 */
extern float card_win_rate_tenths_x; /* 0x0c28f948 */
extern float card_win_rate_percent_pos[2]; /* 0x0c28f94c */
extern float card_win_rate_point_pos[2]; /* 0x0c28f954 */
extern float card_win_rate_header_x; /* 0x0c28f95c */
extern float card_win_rate_header_w; /* 0x0c28f960 */
extern float card_win_rate_underline_dx; /* 0x0c28f964 */
extern float card_win_rate_pad; /* 0x0c28f968 */
extern float card_match_header_x; /* 0x0c28f96c */
extern float card_match_header_w; /* 0x0c28f970 */
extern float card_match_digits_x; /* 0x0c28f974 */
extern float card_match_suffix_x; /* 0x0c28f978 */
extern float card_match_pad; /* 0x0c28f97c */
extern float card_use_header_x; /* 0x0c28f980 */
extern float card_use_digits_x; /* 0x0c28f984 */
extern float card_use_suffix_x; /* 0x0c28f988 */
extern float card_use_pad; /* 0x0c28f98c */
extern float card_use_header_w; /* 0x0c28f990 */
extern float card_digit_step; /* 0x0c28f994 */
extern float card_digit_dy; /* 0x0c28f998 */
extern float card_panel_select_x[2]; /* 0x0c28f99c */
extern float card_panel_game_x[2]; /* 0x0c28f9a4 */
extern float card_panel_content_dx[2]; /* 0x0c28f9ac */
extern float card_panel_mission_dy; /* 0x0c28f9b4 */
extern float card_panel_select_y; /* 0x0c28f9b8 */
extern float card_panel_game_y; /* 0x0c28f9bc */
extern int level_notice_parts[27]; /* 0x0c28f9c8 */
extern short level_banner_frame; /* 0x0c28fa34 */
extern int level_up_parts[21]; /* 0x0c28fa38 */
extern int net_reply_timer; /* 0x0c28fa94 */
extern uchar net_messages_flag; /* 0x0c28fa98 */
extern int net_vs_parts[18]; /* 0x0c28fa9c */
extern float rob_flag_pair_x[2]; /* 0x0c28fae4 */
extern float rob_flag_pair_step; /* 0x0c28faec */
extern float rob_flag_pair_y; /* 0x0c28faf0 */
extern float rob_flag_base_sprites[2]; /* 0x0c28faf4 */
extern float rob_flag_slot_sprites[2]; /* 0x0c28fafc */
extern float rob_flag_state_sprites[2]; /* 0x0c28fb04 */
extern float rob_flag_slot_dx; /* 0x0c28fb0c */
extern float rob_flag_state_dx; /* 0x0c28fb10 */
extern float rob_flag_overlay_dy; /* 0x0c28fb14 */
extern float rob_flag_overlay_alpha; /* 0x0c28fb18 */
extern int mode_sel_choice_sprites[2]; /* 0x0c28fb1c */
extern float mode_sel_choice_x[2]; /* 0x0c28fb24 */
extern float mode_sel_choice_y[2]; /* 0x0c28fb2c */
extern float mode_sel_pulse_rate; /* 0x0c28fb34 */
extern float mode_sel_pulse_rate_fast; /* 0x0c28fb38 */
extern int mode_sel_highlight_color; /* 0x0c28fb3c */
extern float mode_sel_highlight_dx; /* 0x0c28fb40 */
extern float mode_sel_highlight_dy; /* 0x0c28fb44 */
extern float mode_sel_highlight_w; /* 0x0c28fb48 */
extern float mode_sel_highlight_h; /* 0x0c28fb4c */
extern float mode_sel_box_x; /* 0x0c28fb50 */
extern float mode_sel_box_y; /* 0x0c28fb54 */
extern float mode_sel_box_w; /* 0x0c28fb58 */
extern float mode_sel_box_h; /* 0x0c28fb5c */
extern float mode_sel_box_h_alt; /* 0x0c28fb60 */
extern float mode_sel_box_alpha; /* 0x0c28fb64 */
extern float mode_sel_box_alpha_alt; /* 0x0c28fb68 */
extern float mode_sel_timer_x; /* 0x0c28fb6c */
extern float mode_sel_timer_y; /* 0x0c28fb70 */
extern float mode_sel_timer_tens_dx; /* 0x0c28fb74 */
extern float mode_sel_timer_ones_dx; /* 0x0c28fb78 */
extern float mode_sel_timer_single_dx; /* 0x0c28fb7c */
extern float mode_sel_timer_dy; /* 0x0c28fb80 */
extern float mode_sel_depth_step; /* 0x0c28fb84 */
extern int vblank_count; /* 0x0c292544 */
extern int mode_timer; /* 0x0c292550 */
extern int intrude_timer; /* 0x0c292554 */
extern int round_time; /* 0x0c292558 */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern char round_end_kind; /* 0x0c2925cd */
extern char rounds_to_win; /* 0x0c2925ce */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern char round_number; /* 0x0c2925d8 */
extern int win_streak; /* 0x0c2925dc */
extern short judge_time; /* 0x0c2925e8 */
extern undefined4 course_time_total; /* 0x0c2925f8 */
extern undefined4 tex_vram_free; /* 0x0c292740 */
extern int rank_page; /* 0x0c3713b0 */
extern short rob_debug_value; /* 0x0c371a78 */
extern int char_textures_loaded; /* 0x0c3ad290 */
extern int sprite_set_vram_used[81]; /* 0x0c3b6298 */
extern float card_team_label_x; /* 0x0c41f094 */
extern short prize_debug_item; /* 0x0c41f0bc */
extern short prize_debug_item_2p; /* 0x0c41f0be */
extern short prize_debug_money; /* 0x0c41f0c0 */
extern short prize_debug_money_2p; /* 0x0c41f0c2 */
extern short prize_debug_rank; /* 0x0c41f0c4 */
extern player_message player_messages[2]; /* 0x0c41f0c8 */
extern char team_names[2][21]; /* 0x0c41f2e0 */
extern short quest_state_seen[2]; /* 0x0c41f30a */
extern int emblem_sprites[2]; /* 0x0c41f328 */
extern short winner_mes_group; /* 0x0c41f330 */
extern uchar continue_digit_on; /* 0x0c41f332 */
extern char continue_digit_shown; /* 0x0c41f333 */
extern int rank_usage_total; /* 0x0c41f3b4 */
extern int rank_row_count; /* 0x0c41f3b8 */
extern int rank_disp_timer; /* 0x0c41f3bc */
extern float rank_scroll_y; /* 0x0c41f3c0 */
extern short rank_scroll_hold; /* 0x0c41f3c4 */
extern short round_flash_timer; /* 0x0c41f3c6 */
extern life_gauge_rec life_gauge[2]; /* 0x0c41f3c8 */
extern mes_pop_rec mes_pop_state[3]; /* 0x0c41f3e8 */
extern float card_panel_row_y; /* 0x0c41f3f4 */
extern float card_panel_stats_row_y; /* 0x0c41f3f8 */
extern float card_panel_kyu_row_y; /* 0x0c41f3fc */
extern net_status_rec net_status[2]; /* 0x0c41f400 */
extern int mode_sel_blink; /* 0x0c41f448 */
extern mes_anim_slots mes_anim_state; /* 0x0c28ed54 */
extern quest_mes_phase_rec quest_mes_phase; /* 0x0c28eda8 */
extern card_panel_sel_rows_rec card_panel_sel_rows; /* 0x0c28f88c */
extern rounds_won_sides rounds_won; /* 0x0c2925d0 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern credit_copy_rec credit_copy; /* 0x0c370b80 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern char sprite_set_loaded[81]; /* 0x0c3b63dc */
extern char_side_tex_mask_rec char_side_tex_mask; /* 0x0c3b6430 */
extern agg_0c41f310 agg_0c41f310_0c41f310; /* 0x0c41f310 */
extern agg_0c41f31c agg_0c41f31c_0c41f31c; /* 0x0c41f31c */
extern prize_anim_rec prize_anim; /* 0x0c41f440 */
extern float card_underline_end_w; /* 0x0c28f900 */
extern float card_underline_dy; /* 0x0c28f904 */
extern float card_underline_unit; /* 0x0c28f908 */
extern float card_underline_mid_x; /* 0x0c28f90c */
extern float card_type1_label_x; /* 0x0c28f8cc */
extern float card_type1_count_x; /* 0x0c28f8d0 */
extern float card_type1_label_w; /* 0x0c28f8d4 */

static inline char *spot_state(void) /* an inline accessor, kept: written out by hand at each use, mes_jitter_in_draw gets an 8-byte frame (the game's is 4) and other changes, not just a register swap, so the original most likely had a helper like this here */
{
  return (char *)&mes_anim_state;
}

void game_mes_load(int group)
{
  int vram;
  int i;
  int sprite_no;
  uint bit;
  int set_no = 7;
  
  vram = tex_vram_free_total();
  for (i = 0; i < 4; i++) {
    sprite_no = game_mes_table[group].sprites[i];
    if (sprite_no == -1) break;
    bit = 1 << (sprite_no - 0x348);
    if ((bit & game_mes_loaded) == 0) {
      sprite_set_sprite_load(7,sprite_no);
      game_mes_loaded = game_mes_loaded | bit;
      sprite_set_loaded[set_no] = 1;
    }
  }
  tex_vram_free = tex_vram_free_total();
  vram -= tex_vram_free;
  sprite_set_vram_used[set_no] += vram;
}

int game_mes_is_loaded(int group)
{
  int sprite_no;
  char i;
  
  if (sprite_set_loaded[7] == '\0') {
    return 0;
  }
  for (i = 0; i < 4; i++) {
    sprite_no = game_mes_table[group].sprites[i];
    if (sprite_no == -1) {
      return 0;
    }
    if (((1 << (sprite_no - 0x348)) & game_mes_loaded) != 0) {
      return 1;
    }
  }
  return 0;
}

void game_mes_free(int group)
{
  int vram;
  int i;
  int sprite_no;
  uint bit;
  int set_no = 7;
  
  vram = tex_vram_free_total();
  for (i = 0; i < 4; i++) {
    sprite_no = game_mes_table[group].sprites[i];
    if (sprite_no == -1) break;
    bit = 1 << (sprite_no - 0x348);
    if ((bit & game_mes_loaded) != 0) {
      sprite_set_sprite_free(7,sprite_no);
      game_mes_loaded = ~bit & game_mes_loaded;
    }
  }
  tex_vram_free = tex_vram_free_total();
  vram -= tex_vram_free;
  sprite_set_vram_used[set_no] += vram;
}

void emblem_sprite_load(int emblem_sprite)
{
  int vram_before;
  int vram_after;
  int sprite_no;
  
  sprite_no = emblem_sprite - 32000;
  sprite_no += 9600;
  vram_before = tex_vram_free_total();
  sprite_set_sprite_load(0x50,sprite_no);
  vram_after = tex_vram_free_total();
  sprite_set_vram_used[0x50] = (vram_before - vram_after) + sprite_set_vram_used[0x50];
}

void emblem_sprite_free(int emblem_sprite)
{
  int vram_before;
  int vram_after;
  
  emblem_sprite -= 32000;
  emblem_sprite += 9600;
  vram_before = tex_vram_free_total();
  sprite_set_sprite_free(0x50,emblem_sprite);
  vram_after = tex_vram_free_total();
  sprite_set_vram_used[0x50] = (vram_before - vram_after) + sprite_set_vram_used[0x50];
}

void screen_fill_white(void)
{
  sprite_colors_set(0,0xffffff);
  sprite_put_xyz_scaled(0,0.0,0.0,0.006,80.0,60.0);
  sprite_colors_reset();
  return;
}

void screen_fill_black(void)
{
  sprite_colors_set(0,0);
  sprite_put_xyz_scaled(0,0.0,0.0,0.006,80.0,60.0);
  sprite_colors_reset();
  return;
}

void sega_disp_sprites_draw(void)
{
  sprite_list_put(sega_disp_sprites,0.05);
  sprite_list_put(sega_disp_sprites_2,0.04);
  return;
}

void cri_disp_sprites_draw(void)
{
  if (2 < (int)mode_timer) {
    sprite_list_put(cri_disp_sprites,0.05);
  }
}

void sprite_list_5f50_draw(void)
{
  sprite_list_put(sprite_list_5f50,0.07);
  return;
}

void rating_layout_draw(void)
{
  mes_layout_draw(0x25,0.04);
  return;
}

void rating_layout_draw_blend_9c(void)
{
  blend_pair_bits = 0x9c;
  mes_layout_draw(0x25,0.04);
  blend_pair_bits = 0x68;
  return;
}

void mes_group_23_load(void)
{
  game_mes_load(0x23);
  return;
}

void mes_group_23_free(void)
{
  game_mes_free(0x23);
  return;
}

void mes_group_23_draw_on_flag_800(void)
{
  if ((game_flags & 0x800) != 0) {
    game_mes_load(0x23);
    mes_layout_draw(0x23,0.04);
  }
  return;
}

void version_label_draw(void)
{
  sprite_colors_set(0x209020,0);
  sprite_font_text_put(&ascii_font,"(VER.B)",544.0,424.0,0,0.006);
  sprite_colors_reset();
  return;
}

void miss_bank_load(void)
{
  sprite_set_load(0x1e);
  miss_bank_loaded = 1;
  return;
}

void miss_bank_free(void)
{
  sprite_set_free(0x1e);
  miss_bank_loaded = 0;
  return;
}

void sou_bank_select(int kind)
{
  char bank = (char)kind;
  switch (bank) {
  case 3: prize_kind = 2; break;
  case 4: prize_kind = 3; break;
  default: prize_kind = 0; break;
  }
  if (bank == -1) prize_kind = 1;
}

void sou_banks_load(void)
{
  game_mes_load(0x26);
  sprite_set_load(0x1f);
  switch(prize_kind) {
  case 1:
    sprite_set_load(0x20);
    break;
  case 2:
    sprite_set_load(0x21);
    break;
  case 3:
    sprite_set_load(0x22);
    break;
  case 0:
    break;
  }
  sou_banks_loaded = 1;
  return;
}

void sou_banks_free(void)
{
  game_mes_free(0x26);
  sprite_set_free(0x1f);
  switch(prize_kind) {
  case 0:
    break;
  case 1:
    sprite_set_free(0x20);
    break;
  case 2:
    sprite_set_free(0x21);
    break;
  case 3:
    sprite_set_free(0x22);
    break;
  }
  sou_banks_loaded = 0;
  return;
}

void oldgauge_bank_load(void)
{
  sprite_set_load(0x24);
  return;
}

void oldgauge_bank_free(void)
{
  sprite_set_free(0x24);
  oldgauge_on = 0;
  return;
}

void hud_stage_load(void)
{
  emblem_sprites_load();
  if (oldgauge_on != 0) {
    oldgauge_bank_load();
  }
  sprite_set_file_load(6);
  char_hud_textures_load();
  level_notice_tex_load();
  if (oldgauge_on != 0) {
    item_pending_mark_init(0x972,320.0,45.0);
  }
  else {
    item_pending_mark_init(0x969,320.0,48.0);
  }
  quest_mes_phase.side1 = 0xff;
  quest_mes_phase.side0 = 0xff;
  return;
}

void hud_draw(void)
{
  int sub_mode;
  void (*timer_draw)(void);
  
  if (!(oldgauge_on == 0)) {
    oldgauge_frame_draw();
    oldgauge_life_gauges_draw();
    oldgauge_round_marks_draw();
    timer_draw = oldgauge_round_timer_draw;
  }
  else {
    hud_frame_draw();
    life_gauges_draw();
    round_marks_draw();
    timer_draw = round_timer_draw;
  }
  (*timer_draw)();
  char_names_draw();
  level_notice_draw();
  shun_drink_count_draw(0);
  shun_drink_count_draw(1);
  rob_debug_numbers_draw();
  rob_status_marks_draw();
  if (play_style == '\x01') {
    play_time_draw();
    rank_round_number_draw();
  }
  prize_hud_draw();
  quest_mes_draw_all();
  emblem_debug_pick((int)((char)emblem_browse_side));
  item_pending_mark_update();
  item_pending_mark_draw();
  sub_mode = mode_get_sub();
  if ((sub_mode != 0xd) || ((game_flags & 0x400) == 0)) {
    rob_flag_pair_draw();
  }
  return;
}

void hud_stage_free(void)
{
  char_hud_textures_free();
  emblem_sprites_free();
  return;
}

void emblem_sprites_load(void)
{
  short emblem;
  int *sprites;
  char i;
  
  if (emblem_browse_side >= 0) {
    sprite_set_load(0x50);
  }
  else {
    sprite_set_file_load(0x50);
    for (i = 0; i < 2; i++) {
      sprites = emblem_sprites;
      sprites[i] = -1;
      if (card_record_get(i)->in_use != 0) {
        emblem = card_body_emblem_get(&card_image_get(i)->body) - 1;
        if (emblem < 0) {
          emblem = card_body_emblem_get(&card_record_get(i)->body) - 1;
        }
        if ((uint)emblem < 0xae) {
          emblem_sprites[i] = emblem + 32000;
          emblem_sprite_load(emblem + 32000);
        }
      }
    }
  }
  return;
}

void emblem_sprites_free(void)
{
  int emblem_sprite;
  int i;
  
  if (!(((char)emblem_browse_side) < '\0')) {
    sprite_set_free(0x50);
  }
  else {
    i = 0;
    do {
      emblem_sprite = emblem_sprites[i];
      if (0 < emblem_sprite) {
        emblem_sprite_free(emblem_sprite);
        emblem_sprites[i] = -1;
      }
      i = i + 1;
    } while (i < 2);
  }
  return;
}

void round_intro_draw(void)
{
  if (mode_timer == 0x95) {
    if ((game_flags & 0x200) != 0) {
      game_mes_load(0x11);
      game_mes_load(0x24);
      mes_pop_slot_reset(0);
    }
    else {
      game_mes_load(0x11);
      game_mes_load(0x12);
      mes_pop_slot_reset(0);
      mes_pop_slot_reset(2);
    }
    mes_anim_state.slot0 = 1;
    mes_anim_state.slot1 = 1;
    mes_anim_state.slot2 = 1;
  }
  else if ((float)mode_timer < 149.0) {
    sprite_offset_set(0.0,mes_offset_y,0.0);
    if ((game_flags & 0x200) != 0) {
      mes_spin_in_draw(0x24,0,0.008);
    }
    else if ((char)mes_spin_in_draw(0x11,0,0.008) != 0) {
      if ((int)rounds_won.side0 + (int)rounds_won.side1 == rounds_to_win * 2 - 2) {
        mes_number_burst_draw(0.007,(round_number + 1) % 10 + 0x12,2,1);
      }
      else {
        mes_number_burst_draw(0.007,(round_number + 1) % 10 + 0x12,2,0);
      }
    }
    sprite_offset_reset();
  }
  hud_draw();
  if (mode_timer == 0x3c) {
    mes_pop_slot_reset(1);
  }
  else if (mode_timer < 0x3c) {
    sprite_offset_set(0.0,mes_offset_y,0.0);
    mes_slide_in_draw(0x21,1,0.008);
    sprite_offset_reset();
  }
  if (mode_timer == 0) {
    round_intro_free();
  }
}

void round_intro_free(void)
{
  game_mes_free(0x11);
  game_mes_free(0x12);
  game_mes_free(0x24);
  mes_anim_state.slot0 = 0;
  mes_anim_state.slot1 = 0;
  mes_anim_state.slot2 = 0;
  return;
}

void judge_mes_draw(void)
{

  if (mode_timer == 0) {
    game_mes_free(judge_mes_group[round_end_kind]);
    mes_anim_state.slot0 = 0;
  }
  else {
    if (mode_timer == judge_time + -1) {
      mes_pop_slot_reset(0);
      game_mes_load(judge_mes_group[round_end_kind]);
      mes_anim_state.slot0 = 1;
      if ((play_style == '\x02') && ((game_flags & 0x400) != 0)) {
        player_messages[0].set = card_record_get(0)->in_use;
        player_messages[1].set = card_record_get(1)->in_use;
      }
      else {
        player_messages[0].set = 0;
        player_messages[1].set = 0;
      }
    }
    if ((mode_timer < judge_time + -3) && (2 < mode_timer)) {
      sprite_offset_set(0.0,mes_offset_y,0.0);
      mes_squeeze_in_draw(0.008,judge_mes_group[round_end_kind],0);
      sprite_offset_reset();
    }
    hud_draw();
  }
}

void replay_hud_draw(void)
{
  if (mode_timer == 0) {
    game_mes_free(0x10);
  }
  else if (mode_timer == 0xef) {
    game_mes_load(0x10);
  }
  if (mode_timer - 3U < 0xea) {
    mes_layout_draw(0x10,0.04);
  }
  net_vs_parts_draw();
  level_notice_draw();
  prize_hud_draw();
  player_message_pick(0,&player_messages[0].sel);
  player_message_pick(1,&player_messages[1].sel);
  player_message_draw();
  quest_mes_draw_all();
}

void vars_0c41f310_clear(void)
{
  agg_0c41f310_0c41f310.field_0x0 = 0;
  agg_0c41f310_0c41f310.field_0x4 = 0;
  agg_0c41f310_0c41f310.field_0x8 = 0;
  agg_0c41f31c_0c41f31c.field_0x0 = 0.0;
  agg_0c41f31c_0c41f31c.field_0x4 = 0.0;
  agg_0c41f31c_0c41f31c.field_0x8 = 0.0;
  console_cleared_word = 0;
  return;
}

void winner_mes_start(void)
{
  int group;
  int side;
  
  emblem_sprites_free();
  if ((game_flags & 0x400) == 0) {
    if (play_style == '\x01') {
      if (side_entry_flags[0] & 4) side = 0; else side = 1;
      group = (round_winner == side) ? 0x1f : 0x1e;
    }
    else
      group = 0x1d;
  }
  else {
    if (play_style == '\x01')
      group = (game_flags & 0x800) ? 0x1f : 0x1e;
    else
      group = 0x1d;
  }
  mes_pop_slot_reset(0);
  game_mes_load(group);
  winner_mes_group = (short)group;
  level_up_tex_load();
  if (play_style == '\x02' && mission_active_get() != -1)
    prize_fly_state = '\x01';
  else
    prize_fly_state = '\0';
  mes_anim_state.slot0 = 1;
  cns_nn_winner_show();
  return;
}

void winner_mes_draw(void)
{
  if (2 < mode_timer) {
    mes_sweep_in_draw(0.008,(int)winner_mes_group,0);
    sprite_offset_reset();
    level_up_banner_draw();
    item_set_redeem_draw();
    prize_hud_draw();
    if (prize_fly_state != '\0') {
      prize_fly_draw();
    }
    else {
      item_award_mes_draw(0);
      item_award_mes_draw(1);
    }
    player_message_pick(0,&player_messages[0].sel);
    player_message_pick(1,&player_messages[1].sel);
    player_message_draw();
    quest_mes_draw_all();
    cns_nn_winner_draw();
  }
  if (mode_timer == 0) {
    winner_mes_end();
  }
  return;
}

void winner_mes_end(void)
{
  game_mes_free((int)((short)winner_mes_group));
  level_up_tex_free();
  miss_bank_free();
  if (sou_banks_loaded != 0) {
    sou_banks_free();
  }
  if (oldgauge_on != 0) {
    oldgauge_bank_free();
  }
  prize_fly_state = 0;
  card_icon_pos_set(36.0,446.0,601.0,446.0);
  mes_anim_state.slot0 = 0;
  cns_nn_winner_hide();
  return;
}

void continue_mes_start(void)
{
  continue_digit_on = 0;
  mes_pop_slot_reset(0);
  mes_pop_slot_reset(1);
  continue_digit_shown = 9;
  game_mes_load(4);
  game_mes_load(5);
  mes_anim_state.slot0 = 1;
  mes_anim_state.slot1 = 1;
  return;
}

void continue_mes_free(void)
{
  game_mes_free(4);
  game_mes_free(5);
  mes_anim_state.slot0 = 0;
  mes_anim_state.slot1 = 0;
  return;
}

void continue_mes_draw(void)
{
  int done;
  int timer;
  int digit;
  
  timer = mode_timer;
  digit = timer / 60;
  if (timer < 0) {
    digit = 0;
  }
  else if (9 < digit) {
    digit = 9;
  }
  if (continue_digit_on == 0) {
    done = mes_jitter_in_draw(4,0,0.008);
    if (done != 0) {
      mes_pop_slot_reset(0);
      continue_digit_on = 1;
    }
  }
  else {
    if (continue_digit_shown != digit) {
      mes_pop_slot_reset(0);
      mes_anim_state.slot1 = 1;
    }
    mes_jitter_in_draw(4,0,0.008);
    ((int (*)(float,int,int))mes_triple_zoom_draw)(0.007,digit + 5,1);
  }
  continue_digit_shown = (char)digit;
  return;
}

void gameover_mes_draw(void)
{
  short frame;
  float scale;
  float alpha;

  frame = 0x1a3 - (short)mode_timer;
  if (mode_timer <= 0) {
    game_mes_free(0xf);
    game_mes_free(0x28);
    sprite_colors_set(0,0xffffff);
    sprite_put_xyz_scaled(0,0.0,0.0,0.09,80.0,60.0);
    sprite_colors_reset();
    return;
  }
  if (mode_timer == 0x1a3) {
    mes_pop_slot_reset(0);
    game_mes_load(0xf);
    game_mes_load(0x28);
    return;
  }
  if (mode_timer > 0x1a2) {
    return;
  }
  if (frame < 0x6c) {
    mes_letter_pop_draw(0xf,0,0.04);
  }
  else if (frame < 0x88) {
    alpha = (136.0 - frame) / 28.0;
    sprite_alpha_set(alpha);
    mes_letter_pop_draw(0xf,0,0.04);
    sprite_alpha_reset();
  }
  if (frame >= 120 && frame <= 152) {
    if (frame <= 136) {
      scale = (136.0 - frame) * 0.0625 * 0.25 + 1.25;
    }
    else {
      scale = (150.0 - frame) * 0.0625 * 0.25 + 1.0;
    }
    if (frame <= 130) {
      alpha = (frame - 120.0) / 10.0 * 0.8;
    }
    else if (frame >= 131 && frame <= 139) {
      alpha = 0.8;
    }
    else {
      alpha = (152.0 - frame) / 12.0 * 0.8;
    }
    sprite_put_xyz_scaled_alpha_flags(0xb2a,328.0,236.0,0.02,scale,scale,alpha,0x100b);
    sprite_put_xyz_scaled_alpha_flags(0xb2b,328.0,236.0,0.02,scale,scale,alpha,0x1009);
  }
  if (frame >= 150 && frame <= 210) {
    if (frame <= 162) {
      scale = (162.0 - frame) / 12.0 * 0.35 + 1.15;
    }
    else {
      scale = (210.0 - frame) / 48.0 * 0.15 + 1.0;
    }
    if (frame <= 162) {
      alpha = (frame - 150.0) / 12.0 * 0.5;
    }
    else if (frame >= 163 && frame <= 199) {
      alpha = 0.5;
    }
    else {
      alpha = (210.0 - frame) / 10.0 * 0.5;
    }
    sprite_put_xyz_scaled_alpha_flags(0xb28,328.0,236.0,0.01,scale,scale,alpha,0x100b);
    sprite_put_xyz_scaled_alpha_flags(0xb29,328.0,236.0,0.01,scale,scale,alpha,0x1009);
  }
  if (frame > 179) {
    alpha = (frame - 180.0) / 30.0;
    if (alpha > 1.0) {
      alpha = 1.0;
    }
    sprite_put_xyz_alpha_flags(0xb23,328.0,236.0,0.03,alpha,0x100b);
    sprite_put_xyz_alpha_flags(0xb24,328.0,236.0,0.03,alpha,0x1009);
  }
  if (frame > 161) {
    alpha = (frame - 162.0) / 48.0;
    if (alpha > 1.0) {
      alpha = 1.0;
    }
    sprite_put_xyz_scaled_alpha_flags(0xb25,320.0,0.0,0.01,1.25,7.5,alpha,7);
    sprite_put_xyz_scaled_alpha_flags(0xb25,320.0,0.0,0.01,1.25,7.5,alpha,0x15);
    sprite_put_xyz_scaled_alpha_flags(0xb25,320.0,480.0,0.01,1.25,7.5,alpha,0xf);
    sprite_put_xyz_scaled_alpha_flags(0xb25,320.0,480.0,0.01,1.25,7.5,alpha,0x1d);
    sprite_colors_set(0,0xffffff);
    sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.05,80.0,60.0,alpha);
    sprite_colors_reset();
  }
  if (frame > 199) {
    alpha = (frame - 200.0) / 66.0;
    if (alpha > 1.0) {
      alpha = 1.0;
    }
    sprite_put_xyz_alpha(0xb26,84.0,264.0,0.01,alpha);
    sprite_put_xyz_alpha(0xb27,340.0,264.0,0.01,alpha);
  }
}

void challenger_mes_draw(void)
{
  if (intrude_timer == 0) {
    round_intro_free();
    winner_mes_end();
    game_mes_free(0x20);
  }
  else if (intrude_timer == 0xb3) {
    game_mes_load(0x20);
    mes_pop_slot_reset(0);
    mes_anim_state.slot0 = 1;
  }
  else if (intrude_timer - 3U < 0xb0) {
    mes_flip_in_draw(0x20,0,0.008);
  }
  item_set_redeem_draw();
  if (!(prize_fly_state == '\0')) {
    prize_fly_draw();
  }
  else {
    item_award_mes_draw(0);
    item_award_mes_draw(1);
  }
  return;
}

void rank_next_page_setup(void)
{
  char swap_chara;
  int count, next_count;
  int i, j;
  
  rank_page = rank_page + 1;
  if (3 < rank_page) {
    rank_page = 0;
  }
  switch(rank_page) {
  case 1:
    rank_row_count = 0xf;
    break;
  case 0:
  case 2:
    rank_row_count = 0x32;
    break;
  case 3:
    rank_row_count = 0x20;
  }
  rank_disp_timer = 0;
  rank_scroll_hold = 0;
  rank_scroll_y = 96.0;
  rank_usage_total = 0;
  for (i = 0; i < 0x10; i++) {
    if (i == 0xd) {
      rank_dural_usage = -1;
    }
    else {
      rank_usage_entry *e = &rank_char_usage[i];
      e->count = user_back_data.bookkeep.chara_play_counts[0][i] + user_back_data.bookkeep.chara_play_counts[1][i];
      rank_usage_total = rank_usage_total + e->count;
    }
    rank_char_usage[i].chara = i;
  }
  for (i = 0; i < 0x10; i++) {
    for (j = 0; j < 0xf; j++) {
      rank_usage_entry *e = &rank_char_usage[j];
      count = ((int *)e)[1];
      next_count = ((int *)e)[3];
      if (count < next_count) {
        swap_chara = e->chara;
        e->chara = (e + 1)->chara;
        ((int *)e)[1] = next_count;
        (e + 1)->chara = swap_chara;
        ((int *)e)[3] = count;
      }
    }
  }
  return;
}

void rank_page_header_draw(void)
{
  switch(rank_page) {
  case 0:
    sprite_put_xyz(0x642,68.0,44.0,0.009);
    sprite_put_xyz(0x64b,220.0,84.0,0.009);
    sprite_put_xyz(0x64d,516.0,84.0,0.009);
    break;
  case 1:
    sprite_put_xyz(0x640,68.0,44.0,0.009);
    sprite_put_xyz(0x641,324.0,44.0,0.009);
    sprite_put_xyz(0x649,212.0,84.0,0.009);
    sprite_put_xyz(0x64a,460.0,84.0,0.009);
    break;
  case 2:
    sprite_put_xyz(0x643,72.0,44.0,0.009);
    sprite_put_xyz(0x644,328.0,44.0,0.009);
    sprite_put_xyz(0x64b,220.0,84.0,0.009);
    sprite_put_xyz(0x64e,508.0,84.0,0.009);
    break;
  case 3:
    sprite_put_xyz(0x47e2,68.0,44.0,0.009);
    sprite_put_xyz(0x47e3,304.0,44.0,0.009);
    sprite_put_xyz(0x64b,220.0,84.0,0.009);
    sprite_put_xyz(0x47e0,510.0,86.0,0.009);
    break;
  }
  sprite_put_xyz_scaled(0x645,0.0,72.0,0.02,56.5,1.0);
  sprite_put_xyz(0x647,452.0,32.0,0.02);
  sprite_put_xyz_scaled(0x648,516.0,32.0,0.02,15.5,1.0);
  sprite_put_xyz(0x646,508.0,48.0,0.009);
  sprite_put_xyz(0x64c,52.0,84.0,0.009);
  sprite_put_xyz_scaled_alpha(0x655,0.0,0.0,0.07,80.0,10.0,0.5);
  sprite_put_xyz_scaled_alpha(0x654,0.0,383.0,0.04,80.0,1.0,1.0);
}

void rank_place_ordinal_draw(int place, float x, float y)
{
  typedef struct { short a; short b; } ent;
  int rank;
  ent *digits;
  short suffix;
  
  rank = (short)place;
  if (mode_get_main() == MODE_MAIN_ADVERTISE) digits = (ent *)rank_digits_adv;
  else digits = (ent *)rank_digits;
  if (rank < 9) {
    sprite_put_xy(digits[rank + 1].a,x,y);
  }
  else {
    sprite_put_xy(digits[(rank + 1) / 10].a,x - 20.0f,y);
    sprite_put_xy(digits[(rank + 1) % 10].a,x,y);
  }
  if ((rank + 1) / 10 == 1) suffix = digits[0].b;
  else suffix = digits[(rank + 1) % 10].b;
  sprite_put_xy(suffix,x + 36.0f,y + 12.0f);
}

void rank_row_name_count_draw(char row, float row_y)
{
  ranking_row_rec *p = &user_back_data.rankings.point_ranking_rows[row];
  int cells;
  float y;
  int value;

  sprite_put_xy(rank_char_sprites_adv[p->chara][(char)(row % 2)], 112.0, row_y + 4.0);
  cells = euc_text_cells(p->name);
  if (cells < 8) {
    cells = 8;
  }
  sprite_text_put(&euc_text_font, 272.0, y = row_y + 4.0, 0.04, 0x20, p->name);
  sprite_put_xyz_scaled_alpha(0, 264.0, y, 0.05, (float)(cells + 2), 2.0, 0.5);
  sprite_put_xyz_scaled(0x1b, 264.0, y + 10.0, 0.05, (float)(cells + 2), 1.0);
  sprite_put_xyz(0x19, 264.0, y + 18.0, 0.05);
  value = p->value;
  if (value > 999) {
    value = 999;
  }
  if (value / 100 != 0) {
    sprite_put_xy(rank_digits_adv[value / 100 * 2], 436.0, row_y);
  }
  if (value / 100 != 0 || value / 10 != 0) {
    sprite_put_xy(rank_digits_adv[value % 100 / 10 * 2], 456.0, row_y);
  }
  sprite_put_xy(rank_digits_adv[value % 10 * 2], 476.0, row_y);
  sprite_put_xy(0x663, 512.0, row_y);
}

void rank_row_time_draw(int row_index, float row_y)
{
  char row = (char)row_index;
  ranking_row_rec *p = &user_back_data.rankings.time_ranking_rows[row];
  ushort seconds;
  ushort hundredths;
  int cells;
  float y;
  uint time;
  ushort minutes;
  short *digits;
  char slot;
  float *digit_x;

  if (mode_get_main() == MODE_MAIN_ADVERTISE) {
    digits = rank_digits_adv;
    sprite_put_xy(rank_char_sprites_adv[p->chara][(char)(row % 2)], 112.0, row_y + 4.0);
  } else {
    digits = rank_digits;
    sprite_put_xy(rank_char_sprites[p->chara][(char)(row % 2)], 112.0, row_y + 4.0);
  }
  cells = euc_text_cells(p->name);
  if (cells < 8) {
    cells = 8;
  }
  sprite_text_put(&euc_text_font, 272.0, y = row_y + 4.0, 0.04, 0x20, p->name);
  sprite_put_xyz_scaled_alpha(0, 264.0, y, 0.05, (float)(cells + 2), 2.0, 0.5);
  sprite_put_xyz_scaled(0x1b, 264.0, y + 10.0, 0.05, (float)(cells + 2), 1.0);
  sprite_put_xyz(0x19, 264.0, y + 18.0, 0.05);
  time = p->value;
  minutes = time / 3600;
  seconds = time / 60 - time / 3600 * 60;
  hundredths = time % 60 * 100 / 60;
  slot = 0;
  if (minutes > 99) {
    minutes = 99;
  }
  if (minutes / 10 != 0) {
    sprite_put_xy(digits[minutes / 10 * 2], (&rank_time_digit_x)[slot++], row_y);
  } else {
    slot++;
  }
  sprite_put_xy(digits[minutes % 10 * 2], (digit_x = &rank_time_digit_x)[slot++], row_y);
  sprite_put_xy(digits[20], digit_x[slot++], row_y);
  sprite_put_xy(digits[seconds / 10 * 2], digit_x[slot++], row_y);
  sprite_put_xy(digits[seconds % 10 * 2], digit_x[slot++], row_y);
  sprite_put_xy(digits[20], digit_x[slot++], row_y);
  sprite_put_xy(digits[20], digit_x[slot++], row_y);
  sprite_put_xy(digits[hundredths / 10 * 2], digit_x[slot++], row_y);
  sprite_put_xy(digits[hundredths % 10 * 2], digit_x[slot++], row_y);
}

void rank_row_usage_percent_draw(char row, float row_y)
{
  short percent, hundreds, rest, tens, ones, tenths;
  float value;
  short *digits;
  
  if (rank_usage_total == 0) {
    sprite_put_xy(rank_digits_adv[0],470.0,row_y);
    sprite_put_xy(0x660,490.0,row_y + 16.0);
    sprite_put_xy(rank_digits_adv[0],500.0,row_y);
    sprite_put_xy(0x662,526.0,row_y);
  }
  else {
    value = ((float)rank_char_usage[row].count / (float)rank_usage_total) * 100.0;
    percent = (int)float_floor(value);
    value = value - float_floor(value);
    value = value * 10.0;
    tenths = (int)float_floor(value);
    hundreds = percent / 100;
    rest = percent % 100;
    tens = rest / 10;
    ones = percent % 10;
    if (hundreds != 0) {
      sprite_put_xy(rank_digits_adv[hundreds * 2],430.0,row_y);
    }
    if ((hundreds != 0) | (tens != 0)) {
      sprite_put_xy(rank_digits_adv[tens * 2],450.0,row_y);
    }
    digits = rank_digits_adv;
    sprite_put_xy(digits[ones * 2],470.0,row_y);
    sprite_put_xy(0x660,490.0,row_y + 16.0);
    sprite_put_xy(digits[tenths * 2],500.0,row_y);
    sprite_put_xy(0x662,526.0,row_y);
  }
  sprite_put_xy(rank_char_sprites_adv[rank_char_usage[row].chara][(char)(row % 2)],112.0,row_y + 4.0);
  sprite_put_xy(rank_char_name_sprites[rank_char_usage[row].chara],272.0,row_y + 4.0);
}

void rank_row_score_draw(char index, float row_y)
{
  float y = row_y;
  char row = index;
  short *digits;
  char course;
  char slot;
  int place;
  mission_score_rec *entry;
  int cells;
  int score;
  int i;
  float text_y;
  char buf[64];
  int digit;

  if (mode_get_main() == MODE_MAIN_ADVERTISE) digits = rank_digits_adv;
  else digits = rank_digits;
  course = row / 4;
  slot = row % 4;
  if (slot == 0) {
    sprite_put_xyz(0x47e1, 112.0, y, 0.04);
    sprite_put_xyz(digits[course * 2 + 2], 264.0, y, 0.04);
  } else {
    place = slot - 1;
    entry = &user_back_data.challenge.course_ranking[course][place];
    if (mode_get_main() == MODE_MAIN_ADVERTISE) {
      sprite_put_xy(rank_char_sprites_adv[entry->chara][row % 2], 112.0, y + 4.0);
    } else {
      sprite_put_xy(rank_char_sprites[entry->chara][row % 2], 112.0, y + 4.0);
    }
    cells = euc_text_cells(entry->name);
    if (cells < 8) cells = 8;
    sprite_text_put(&euc_text_font, 272.0, text_y = y + 4.0, 0.04, 0x20, (byte *)entry->name);
    sprite_put_xyz_scaled_alpha(0, 264.0, text_y, 0.05, (float)(cells + 2), 2.0, 0.5);
    sprite_put_xyz_scaled(0x1b, 264.0, text_y + 10.0, 0.05, (float)(cells + 2), 1.0);
    sprite_put_xyz(0x19, 264.0, text_y + 18.0, 0.05);
    score = entry->score;
    if (score > 999999) score = 999999;
    sprintf(buf, format_6d, score);
    for (i = 0; i < 6; i++) {
      if (buf[i] != ' ') {
        digit = buf[i] - 0x30;
        sprite_put_xyz(digits[digit * 2], (float)i * 20.0 + 456.0, y, 0.04);
      }
    }
  }
}

void rank_list_draw(void)
{
  float y;
  int flag;
  char i;
  char place;

  if (rank_disp_timer <= 899) y = 0.0;
  else y = -0.25;
  flag = 0;
  if ((player_input_get(0)->now | player_input_get(1)->now) & 0x80) {
    y = ((float)rank_scroll_hold / 30.0 + 2.0) * -1.0;
    if (y < -10.0) y = -10.0;
    rank_scroll_hold = rank_scroll_hold + 1;
    rank_disp_timer = 0x30c;
  }
  else {
    if ((player_input_get(0)->now | player_input_get(1)->now) & 0x40) {
      y = (float)rank_scroll_hold / 30.0 + 2.0;
      if (y > 10.0) y = 10.0;
      rank_scroll_hold = rank_scroll_hold + 1;
      rank_disp_timer = 0x30c;
    }
    else {
      rank_scroll_hold = 0;
    }
  }
  if ((player_input_get(0)->pressed | player_input_get(1)->pressed) & 0xf) {
    rank_page = rank_page + 1;
    rank_scroll_y = 96.0;
    rank_disp_timer = 0x30c;
    if (rank_page > 3) rank_page = 0;
    switch (rank_page) {
    case 0:
    case 2:
      rank_row_count = 0x32;
      break;
    case 1:
      rank_row_count = 0xf;
      break;
    case 3:
      rank_row_count = 0x20;
      break;
    }
  }
  rank_scroll_y = rank_scroll_y + y;
  if (rank_scroll_y > 96.0) rank_scroll_y = 96.0;
  y = float_floor(rank_scroll_y);
  sprite_clip_push(0,3,0x13,0xb,0,2);
  for (i = 0; i < rank_row_count; i++) {
    if (y >= 60.0 && y <= 384.0) {
      if (!flag) flag = 1;
      sprite_put_xyz_scaled_alpha(0x654,0.0,y + 36.0,0.04,80.0,1.0,1.0);
      place = i;
      if (rank_page == 3) place = place % 4 - 1;
      if (place != -1) rank_place_ordinal_draw(place,48.0,y + 4.0);
      switch (rank_page) {
      case 0:
        rank_row_time_draw(i,y + 4.0);
        break;
      case 1:
        rank_row_usage_percent_draw(i,y + 4.0);
        break;
      case 2:
        rank_row_name_count_draw(i,y + 4.0);
        break;
      case 3:
        rank_row_score_draw(i,y + 4.0);
        break;
      }
    }
    y = y + 36.0;
  }
  if (!flag) sprite_put_xyz_alpha(0x655,-8.0,-8.0,0.04,0.0);
  sprite_clip_push(0,0,0x13,0xe,2,0);
  if (flag) rank_page_header_draw();
  rank_disp_timer = rank_disp_timer + 1;
}

void round_mark_win_flash_draw(float mark_x, int side_arg, float mark_y)
{
  float x = mark_x;
  float y = mark_y;
  int side = (char)side_arg;
  uint paused;
  int frame;
  int timer;
  int since;
  int sprite;
  ushort rot;
  float alpha;
  float scale;
  
  if (mode_timer == judge_time + -1) {
    round_flash_timer = 0;
  }
  else {
    paused = pause_frame_held();
    if (paused == 0) {
      round_flash_timer = round_flash_timer + 1;
    }
  }
  if (round_flash_timer < 0x6a) {
    sprite_next_blend_set(0);
  }
  timer = (int)round_flash_timer;
  if (timer < 0x44) {
    if (timer < 0x32) {
      alpha = ((float)timer / 50.0) * 0.7 + 0.1;
    }
    else {
      alpha = 0.8 - ((float)(round_flash_timer + -0x32) / 18.0) * 0.8;
    }
    scale = 1.0;
    if (0x1d < round_flash_timer) {
      scale = 1.0 - (float)(round_flash_timer + -0x1e) / 100.0;
    }
    rot = (short)(int)((float)((int)round_flash_timer << 0xf) / 68.0);
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e6,x,y,0.0298,scale,scale,alpha,(short)rot,0xb);
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e6,x,y,0.0298,scale,scale,alpha,(int)(short)(rot + -0x8000),0xb);
  }
  if ((ushort)(round_flash_timer - 10U) < 0x50) {
    if (round_flash_timer < 0x3c) {
      alpha = ((float)(round_flash_timer + -10) / 50.0) * 0.7 * 0.015 + 0.1;
    }
    else {
      alpha = 0.8 - ((float)(round_flash_timer + -0x3c) / 30.0) * 0.8;
    }
    scale = 1.0;
    if (0x1d < round_flash_timer) {
      scale = 1.0 - (float)(round_flash_timer + -0x1e) / 100.0;
    }
    rot = (int)((((float)(round_flash_timer + -10) * 32768.0) / 6.0) / 80.0) + 0x4000;
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e8,x,y,0.0299,scale,scale,alpha,(short)rot,0xb);
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e8,x,y,0.0299,scale,scale,alpha,(short)(rot + -0x8000),0xb);
  }
  if ((ushort)(round_flash_timer - 0x2eU) < 0x3c) {
    if (round_flash_timer < 0x4c) {
      alpha = ((float)(round_flash_timer + -0x2e) / 30.0) * 0.7 + 0.3;
    }
    else {
      alpha = 1.0 - (float)(round_flash_timer + -0x4c) / 30.0;
    }
    if (round_flash_timer < 0x4c) {
      since = round_flash_timer + -0x2e;
      scale = ((float)(since * since) / 900.0) * 0.5 + 1.0;
    }
    else {
      scale = 1.5 - ((float)(round_flash_timer + -0x4c) / 30.0) * 1.3;
    }
    if (round_flash_timer < 0x4c) {
      since = round_flash_timer + -0x2e;
      rot = (short)(int)(((float)(since * since) / 900.0) * 65535.0);
    }
    else {
      rot = 0;
    }
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e7,x,y,0.03,scale,scale,(short)rot,alpha,0xe);
    sprite_put_xyz_scaled_alpha_angle_flags(0x9e7,x,y,0.03,scale,-scale,(short)rot,alpha,0xe);
  }
  if (round_flash_timer < 0x6a) {
    sprite_next_blend_normal();
  }
  frame = (int)round_flash_timer;
  if (frame <= 0x4b) {
    sprite = 0x99f;
  }
  else if (frame > 99) {
    if (frame <= 0x85) {
      sprite = round_flash_sprites[(frame - 100) / 2];
    }
    else {
      sprite = 0xa26;
    }
  }
  else {
    sprite = 0xa26;
  }
  sprite_put_xyz_scaled_flags(sprite,x,y,0.04,((float)side - 0.5) + ((float)side - 0.5),1.0,10);
  return;
}

void round_marks_draw(void)
{
  float p0[2] = {263.0, 378.0};
  float p1[2] = {304.0, 337.0};
  float p2[2] = {264.0, 377.0};
  float p3[2] = {264.0, 377.0};
  int sel;
  char i;
  char k;
  float d;
  char *p;
  float x;
  
  sel = -1;
  if (msnm_active() != 0) {
    sel = msnm_player_side();
  }
  for (i = 0; i < 2; i++) {
    if ((sel != -1) & (sel != i)) continue;
    if (i == 0) d = -1.0; else d = 1.0;
    sprite_put_xyz_scaled(0x9a1,p1[i],57.0,0.05,d,1.0);
    for (k = 0; k < rounds_to_win - 1; k++) {
      sprite_put_xyz_scaled(0x9a2,(float)k * 19.0 * d + p2[i],57.0,0.05,d,1.0);
    }
    sprite_put_xyz_scaled(0x99e,p3[i] + (float)k * 19.0 * d,57.0,0.05,d,1.0);
    for (k = 0; k < rounds_to_win; k++) {
      x = p0[i];
      if (i == 0) x += (float)k * -19.0; else x += (float)k * 19.0;
      if (msnm_active() == 0) {
        if (k == (p = &rounds_won.side0, p[i]) - 1 && round_winner == i && mode_get_sub() == 0xd) {
          round_mark_win_flash_draw(x,i,66.0);
        }
        else if (k == (p = &rounds_won.side0, p[i]) - 1 && mode_get_sub() == 0xd && (game_flags & 0xb00) == 0xb00) {
          sprite_put_xyz_scaled_flags(0x99f,x,66.0,0.04,d,1.0,10);
        }
        else {
          sprite_put_xyz_scaled_flags((p = &rounds_won.side0, p[i]) > k ? 0xa26 : 0x99f,x,66.0,0.04,d,1.0,10);
        }
      }
      else {
        switch ((int)msnm_round_result_get(k)) {
        case 1:
          if (mode_get_sub() == 0xd && (game_flags & 0xb00) != 0 && round_number == k) {
            round_mark_win_flash_draw(x,i,66.0);
          }
          else {
            sprite_put_xyz_scaled_flags(0xa26,x,66.0,0.04,d,1.0,10);
          }
          break;
        case 2:
          chlg_round_lamp_show(k,x,66.0);
          break;
        case 0:
        default:
          sprite_put_xyz_scaled_flags(0x99f,x,66.0,0.04,d,1.0,10);
        }
      }
    }
  }
}

void oldgauge_round_marks_draw(void)
{
  char side;
  char j;
  float fx;
  float dir;
  for (side = 0; side < 2; side++) {
    if (side == 0) {
      dir = -1.0;
    }
    else {
      dir = 1.0;
    }
    for (j = 0; j < rounds_to_win; j++) {
      if (side == 0) {
        fx = 0x110 - j * 20;
      }
      else {
        fx = j * 20 + 0x170;
      }
      if (j == ((struct { char c[2]; } *)&rounds_won)->c[side] - 1 && round_winner == side && mode_get_sub() == 0xd) {
        sprite_put_xyz_scaled((mode_timer & 8) ? 0x3845 : 0x3844,fx,60.0,0.04,dir,1.0);
      }
      else if (j == ((struct { char c[2]; } *)&rounds_won)->c[side] - 1 && mode_get_sub() == 0xd && (game_flags & 0xb00) == 0xb00) {
        sprite_put_xyz_scaled(0x3844,fx,60.0,0.04,dir,1.0);
      }
      else {
        sprite_put_xyz_scaled((((struct { char c[2]; } *)&rounds_won)->c[side] > j) ? 0x3845 : 0x3844,fx,60.0,0.04,dir,1.0);
      }
    }
  }
}

void char_hud_textures_load(void)
{
  int i;
  int course_chars [3];

  char_hud_textures_free();
  char_side_texture_load(0,(int)*(char *)&player_chara.side0);
  char_texture_load((int)*(char *)&player_chara.side0);
  if (msnm_active() != 0) {
    char_side_texture_load(0,(int)*(char *)(&player_chara.side0 + msnm_player_side()));
    char_texture_load((int)*(char *)(&player_chara.side0 + msnm_player_side()));
    msnm_course_chars_get(course_chars,course_chars + 1,course_chars + 2);
    for (i = 0; i < 3; i++) {
      char_side_texture_load(0,(int)(char)course_chars[i]);
      char_texture_load((int)(char)course_chars[i]);
    }
  }
  char_side_texture_load(1,(int)*(char *)&player_chara.side1);
  char_texture_load((int)*(char *)&player_chara.side1);
  return;
}

void char_side_texture_load(int side_arg, int chara_arg)
{
  char side = side_arg;
  char chara = chara_arg;
  int *mask;

  if (chara == -1) return;
  mask = (int *)&char_side_tex_mask + side;
  if (((*mask >> chara) & 1) == 0) {
    sprite_set_sprite_load(6,char_side_textures[chara][side]);
    *mask = (1 << chara) | *mask;
  }
}

void char_texture_load(int chara_arg)
{
  char chara = (char)chara_arg;
  int *mask;

  if (chara != -1) {
    mask = &char_textures_loaded;
    if (((*mask >> chara) & 1) == 0) {
      sprite_set_sprite_load(6,char_textures[chara]);
      *mask = *mask | 1 << chara;
    }
  }
  return;
}

void char_hud_textures_free(void)
{
  int i;
  int j;
  short *textures;
  int *masks;

  for (i = 0; i < 2; i++) {
    masks = (int *)&char_side_tex_mask;
    textures = &char_side_textures[0][0];
    for (j = 0; j < 0x13; j++) {
      if ((masks[i] >> j) & 1) {
        sprite_set_sprite_free(6, textures[j * 2 + i]);
        masks[i] &= ~(1 << j);
      }
    }
  }
  for (i = 0; i < 0x13; i++) {
    if ((char_textures_loaded >> i) & 1) {
      sprite_set_sprite_free(6, char_textures[i]);
      char_textures_loaded &= ~(1 << i);
    }
  }
}

void char_names_draw(void)
{
  char i;
  float plate_x[2] = {0.0, 512.0};
  float name_x[2] = {3.0, char_name_2p_x[player_chara.side1]};
  uint colors[2] = {0xff0000, 0xff};
  int chara;
  int *chara_p;

  for (i = 0; i < 2; i++) {
    { int *pb = (int *)&player_chara; chara_p = pb + i; }
    sprite_put_xyz(char_plate_sprites[*chara_p][i],plate_x[i],52.0,0.09);
    chara = *chara_p;
    sprite_put_xyz(char_name_sprites[chara][0],name_x[i],64.0,0.05);
    sprite_colors_set(colors[i],0);
    sprite_put_xyz(char_name_sprites[chara][1],name_x[i],64.0,0.06);
    sprite_colors_reset();
    quest_start_plate_flash(i);
  }
}

void oldgauge_flag_set(byte on)
{
  if ((byte)on < 2) {
    oldgauge_on = (byte)on;
  }
  return;
}

void life_gauges_reset(void)
{
  char i;
  life_gauge_rec *gauge;
  for (i = 0; i < 2; i++) {
    gauge = &life_gauge[i];
    gauge->hit_timer = 0;
    gauge->drain_timer = 0;
    gauge->life = (float)(rob_get((uchar)i))->life_max;
    gauge->damage = 0;
    gauge->combo_damage = 0;
  }
}

void life_gauges_draw(void)
{
  char i;
  rob_rec *rob;
  life_gauge_rec *gauge;
  float life_rate[2];
  float damage_rate[2];
  float combo_rate[2];
  float alpha;

  for (i = 0; i < 2; i++) {
    rob = rob_get((uchar)i);
    if ((float)rob->life < life_gauge[i].life) {
      life_gauge[i].hit_timer = 0;
      life_gauge[i].drain_timer = 0;
      life_gauge[i].damage = life_gauge[i].life - (float)rob->life;
      life_gauge[i].life = (float)rob->life;
      if (rob->combo[0].hits < 2)
        life_gauge[i].combo_damage = 0.0;
      life_gauge[i].combo_damage += life_gauge[i].damage;
    }
    life_rate[i] = (float)rob->life / (float)rob->life_max;
    gauge = &life_gauge[i];
    damage_rate[i] = gauge->damage / (float)rob->life_max;
    combo_rate[i] = gauge->combo_damage / (float)rob->life_max;
    if (pause_frame_held() == 0) {
      if (gauge->hit_timer < 30)
        gauge->hit_timer++;
      if (life_gauge[i].combo_damage > 0.0 && (rob->combo[0].hits == 0 || mode_get_sub() == 13)) {
        life_gauge[i].combo_damage -= (float)((life_gauge[i].drain_timer * life_gauge[i].drain_timer) / 100);
        if (life_gauge[i].combo_damage < 0.0)
          life_gauge[i].combo_damage = 0.0;
        if (life_gauge[i].damage > life_gauge[i].combo_damage)
          life_gauge[i].damage = life_gauge[i].combo_damage;
        life_gauge[i].drain_timer++;
      }
    }
  }
  sprite_colors_set(0xa0a0a0, 0x6020);
  sprite_put_xyz_scaled_flags(0xa27, 278.0, 36.0, 0.05, life_rate[0] * 32.0, 1.0, 7);
  sprite_put_xyz_scaled_flags(0xa27, 362.0, 36.0, 0.05, life_rate[1] * 32.0, 1.0, 5);
  sprite_colors_reset();
  sprite_colors_set(0, 0xffffff);
  alpha = 1.0 - ((float)life_gauge[0].hit_timer * 0.7) / 30.0;
  sprite_put_xyz_scaled_alpha_flags(0xa27, 278.0, 36.0, 0.06, (damage_rate[0] + life_rate[0]) * 32.0, 1.0, alpha, 7);
  alpha = 1.0 - ((float)life_gauge[1].hit_timer * 0.7) / 30.0;
  sprite_put_xyz_scaled_alpha_flags(0xa27, 362.0, 36.0, 0.06, (damage_rate[1] + life_rate[1]) * 32.0, 1.0, alpha, 5);
  sprite_colors_reset();
  sprite_colors_set(0x808080, 0x800000);
  sprite_put_xyz_scaled_flags(0xa39, 278.0, 36.0, 0.07, (combo_rate[0] + life_rate[0]) * 32.0, 1.0, 7);
  sprite_put_xyz_scaled_flags(0xa39, 362.0, 36.0, 0.07, (combo_rate[1] + life_rate[1]) * 32.0, 1.0, 5);
  sprite_colors_reset();
  sprite_put_xyz_scaled_flags(0x996, 278.0, 36.0, 0.08, 32.0, 1.0, 7);
  sprite_put_xyz_scaled_flags(0x996, 362.0, 36.0, 0.08, 32.0, 1.0, 5);
}

void oldgauge_life_gauges_draw(void)
{
  char i;
  rob_rec *rob;
  life_gauge_rec *gauge;
  life_gauge_rec *hit;
  life_gauge_rec *drain;
  short *life;
  float shown;
  float rate[2];
  float dmg[2];
  float red[2];

  for (i = 0; i < 2; i++) {
    rob = rob_get((uchar)i);
    life = &rob->life;
    shown = *life;
    hit = &life_gauge[i];
    if (shown < hit->life) {
      hit->hit_timer = 0;
      hit->drain_timer = 0;
      hit->damage = hit->life - *life;
      hit->life = *life;
      if (rob->combo[0].hits < 2) {
        hit->combo_damage = 0.0;
      }
      life_gauge[i].combo_damage = life_gauge[i].combo_damage + life_gauge[i].damage;
    }
    rate[i] = (float)rob->life / (float)rob->life_max;
    gauge = &life_gauge[i];
    dmg[i] = gauge->damage / (float)rob->life_max;
    red[i] = gauge->combo_damage / (float)rob->life_max;
    if (pause_frame_held() == 0) {
      if (gauge->hit_timer < 30) {
        gauge->hit_timer++;
      }
      if (life_gauge[i].combo_damage > 0.0) {
        if (rob->combo[0].hits == 0 || mode_get_sub() == 13) {
          drain = &life_gauge[i];
          drain->combo_damage = drain->combo_damage - (drain->drain_timer * drain->drain_timer) / 100;
          if (drain->combo_damage < 0.0) {
            drain->combo_damage = 0.0;
          }
          if (life_gauge[i].damage > life_gauge[i].combo_damage) {
            life_gauge[i].damage = life_gauge[i].combo_damage;
          }
          life_gauge[i].drain_timer++;
        }
      }
    }
  }
  sprite_put_xyz_scaled_flags(14400, 272.0, 36.0, 0.05, rate[0] * 32.0, 1.0, 7);
  sprite_put_xyz_scaled_flags(14400, 368.0, 36.0, 0.05, rate[1] * 32.0, 1.0, 5);
  sprite_colors_set(0, 0xffffff);
  if (mode_timer & 1) {
    sprite_put_xyz_scaled_flags(14400, 272.0, 36.0, 0.06, (dmg[0] + rate[0]) * 32.0, 1.0, 7);
  }
  else {
    sprite_put_xyz_scaled_flags(14400, 368.0, 36.0, 0.06, (dmg[1] + rate[1]) * 32.0, 1.0, 5);
  }
  if (mode_timer & 1) {
    sprite_put_xyz_scaled_alpha_flags(14400, 272.0, 36.0, 0.07, (red[0] + rate[0]) * 32.0, 1.0, 0.7, 7);
  }
  else {
    sprite_put_xyz_scaled_alpha_flags(14400, 368.0, 36.0, 0.07, (red[1] + rate[1]) * 32.0, 1.0, 0.7, 5);
  }
  sprite_colors_reset();
}

void hud_frame_draw(void)
{
  sprite_put_xyz(0x9a0,0.0,24.0,0.02);
  sprite_put_xyz(0x999,240.0,16.0,0.02);
  sprite_put_xyz(0x998,256.0,16.0,0.02);
  sprite_put_xyz(0x99a,384.0,16.0,0.02);
  sprite_put_xyz(0x99c,584.0,24.0,0.02);
  sprite_put_xyz_scaled(0x99b,240.0,24.0,0.02,-23.0,1.0);
  sprite_put_xyz_scaled(0x99d,400.0,24.0,0.02,23.0,1.0);
  sprite_put_xyz(0xa25,288.0,26.0,0.05);
  return;
}

void oldgauge_frame_draw(void)
{
  sprite_put_xyz_scaled_flags(0x3842,8.0,34.0,0.08,1.0,1.0,0x10);
  sprite_put_xyz_scaled(0x3841,16.0,34.0,0.08,32.0,1.0);
  sprite_put_xyz_scaled_flags(0x3843,272.0,34.0,0.08,1.0,1.0,0x10);
  sprite_put_xyz_scaled(0x3848,288.0,18.0,0.02,1.0,1.0);
  sprite_put_xyz_scaled(0x3846,296.0,18.0,0.02,6.0,1.0);
  sprite_put_xyz_scaled(0x3847,344.0,18.0,0.02,1.0,1.0);
  sprite_put_xyz_scaled(0x3849,300.0,18.0,0.01,1.0,1.0);
  sprite_put_xyz_scaled(0x3843,360.0,34.0,0.08,1.0,1.0);
  sprite_put_xyz_scaled(0x3841,368.0,34.0,0.08,32.0,1.0);
  sprite_put_xyz_scaled(0x3842,624.0,34.0,0.08,1.0,1.0);
}

void round_timer_draw(void)
{
  int tens;
  int ones;
  int tenths;
  int hundredths;
  float x;
  int *digits;

  if (round_time > 0x1733) {
    sprite_put_xyz(0xa1d,295.0,36.0,0.01);
    return;
  }
  if (round_time > 0) {
    tens = round_time / 600;
    ones = round_time / 60 % 10;
    tenths = round_time % 60 / 6;
    hundredths = round_time % 60 * 100 / 60 % 10;
  }
  else {
    tens = 0;
    ones = 0;
    tenths = 0;
    hundredths = 0;
  }
  sprite_offset_set(0.0,0.0,0.01);
  if (tens > 9) {
    tens = tens % 10;
  }
  if (tenths > 9) {
    tenths = tenths % 10;
  }
  digits = round_timer_small_digits;
  sprite_put_xy(digits[tenths],312.0,58.0);
  sprite_put_xy(digits[hundredths],321.0,58.0);
  sprite_put_xy_scaled(0xa1e,310.0,58.0,-1.0,1.0);
  sprite_put_xy_scaled(0xa1e,330.0,58.0,1.0,1.0);
  if (tens != 0) {
    sprite_put_xy(round_timer_digits[tens],296.0,26.0);
    x = 312.0;
  }
  else {
    x = 304.0;
  }
  if (tens == 0) {
    if (mode_get_sub() == 10) {
      if (ones < 4) {
        if (round_time & 4) sprite_put_xy(round_timer_low_digits[ones],x,26.0);
      }
      else if (round_time & 8) sprite_put_xy(round_timer_low_digits[ones],x,26.0);
    }
    else sprite_put_xy(round_timer_low_digits[ones],x,26.0);
  }
  else {
    sprite_put_xy(round_timer_digits[ones],x,26.0);
  }
  sprite_offset_reset();
}

void oldgauge_round_timer_draw(void)
{
  int tens;
  int ones;
  int tenths;
  int hundredths;
  int time;
  int *digits;

  if (round_time > 5940) {
    tens = 8;
    ones = 8;
    tenths = 8;
    hundredths = 8;
  }
  else if (round_time > 0) {
    time = round_time;
    tens = time / 600;
    ones = time / 60 % 10;
    tenths = (time % 60) / 6;
    hundredths = ((time % 60) * 100 / 60) % 10;
  }
  else {
    tens = 0;
    ones = 0;
    tenths = 0;
    hundredths = 0;
  }
  tens = tens % 10;
  tenths = tenths % 10;
  digits = oldgauge_timer_small_digits;
  sprite_put_xyz(digits[tenths],325.0,47.0,0.008);
  sprite_put_xyz(digits[hundredths],334.0,47.0,0.008);
  sprite_put_xyz(0x3854,323.0,33.0,0.008);
  digits = oldgauge_timer_digits;
  sprite_put_xyz(digits[tens],294.0,33.0,0.008);
  sprite_put_xyz(digits[ones],309.0,33.0,0.008);
}

void play_time_draw(void)
{
  int time;
  int minutes;
  int seconds;
  int hundredths;
  float x;
  char buf[12];

  time = course_time_total + match_time_frames + round_time_frames;
  if (time > 359999) {
    time = 359999;
  }
  if (time > 0) {
    minutes = time / 3600;
    seconds = time / 60 - minutes * 60;
    hundredths = time % 60 * 100 / 60;
  }
  else {
    minutes = 0;
    seconds = 0;
    hundredths = 0;
  }
  if (msnm_active() != 0) {
    x = (side_entry_flags[0] & 4) ? 534.0 : -8.0;
  }
  else {
    x = (side_entry_flags[0] & 4) ? 520.0 : 8.0;
    sprite_put_xyz(0xae8,x,396.0,0.05);
  }
  sprintf(buf,&format_02d,minutes);
  sprite_font_text_put(&ascii_font,buf,x + 14.0,402.0,0.04,0x10);
  sprite_font_text_put(&ascii_font,"\'",x + 14.0 + 24.0,402.0,0.04,0);
  sprintf(buf,&format_02d,seconds);
  sprite_font_text_put(&ascii_font,buf,x + 14.0 + 24.0 + 5.0,402.0,0.04,0x10);
  sprite_font_text_put(&ascii_font,"\"",x + 14.0 + 48.0 + 5.0,402.0,0.04,0);
  sprintf(buf,&format_02d,hundredths);
  sprite_font_text_put(&ascii_font,buf,x + 14.0 + 48.0 + 5.0 + 8.0,402.0,0.04,0x10);
}

void rank_round_number_draw(void)
{
  int round_no;
  float x;
  char buf[16];
  
  if ((game_flags & 0x800) != 0) {
    round_no = (char)rank_round;
  }
  else {
    round_no = (char)(rank_round + 1);
  }
  if ((side_entry_flags[0] & 4U) != 0) x = 520.0;
  else x = 8.0;
  if (msnm_active() == 0) {
    if (round_no < 10) {
      sprintf(buf,&format_02d,round_no);
      sprite_font_text_put(&ascii_font,buf,(x + 82.0) - 4.0,421.0,0x10,0.03);
      sprite_put_xy(0xae9,x + 10.0 + 4.0,421.0);
    }
    else if (round_no == 10) {
      sprite_put_xy(0xaea,x + 9.0,421.0);
    }
    else {
      sprite_put_xy(0xaec,x + 9.0,421.0);
    }
  }
}

void rob_debug_numbers_draw(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if (rob0->field_0x544 != 0) {
    text_cell_size_set(1,1);
    text_cursor_to_cell((int)rob_debug_value_x,(int)rob_debug_value_y);
    text_cell_size_set(0x10,0x20);
    text_draw_formatted("%d",(int)rob_debug_value);
    text_cell_size_set(1,1);
    text_cursor_to_cell((int)rob_debug_1p_x,(int)rob_debug_1p_y);
    text_cell_size_set(0x10,0x10);
    text_draw_formatted("%d",rob0->press_count);
    text_cell_size_set(1,1);
    text_cursor_to_cell((int)rob_debug_2p_x,(int)rob_debug_2p_y);
    text_cell_size_set(0x10,0x10);
    text_draw_formatted("%d",rob1->press_count);
    text_cell_size_set(8,8);
  }
  return;
}

void quest_mes_clear(int side_arg)
{
  uchar *phase;
  int side;
  
  phase = &quest_mes_phase.side0;
  side = (int)(char)side_arg;
  quest_mes_timer[side] = 0;
  phase[side] = 0xff;
  quest_state_seen[side] = (short)(quest_state_get(side));
  return;
}

void quest_mes_set(int side_arg, char *text)
{
  int side;
  char *dst;
  
  side = (int)(char)side_arg;
  dst = quest_mes_text[side];
  strncpy(dst,text,0x23);
  dst[0x22] = 0;
  quest_mes_timer[side] = 1;
  quest_state_seen[side] = quest_state_get(side);
  return;
}

void quest_mes_update(int side_arg)
{
#define TIMER(side) (quest_mes_timer[(side)])
#define MODE(side) (*(ushort *)&quest_state_seen[side])
#define STATE(side) (st[side])
  int side = (char)side_arg;
  uint state;
  char phase;

  state = quest_state_get(side);
  if (MODE(side) != state) {
    switch (quest_state_get(side)) {
    case 0:
      if (MODE(side) == 2) {
        char *st = (char *)&quest_mes_phase;
        phase = 0;
        STATE(side) = phase;
      }
      break;
    case 1:
      if (TIMER(side) == 0) {
        char *st = (char *)&quest_mes_phase;
        quest_mes_set(side, quest_start_text);
        STATE(side) = 0;
      }
      break;
    case 2:
      if (TIMER(side) > 0) {
        char *st = (char *)&quest_mes_phase;
        quest_mes_set(side, quest_success_text);
        phase = 1;
        STATE(side) = phase;
      }
      break;
    case 3:
      if (TIMER(side) > 0) quest_mes_clear(side);
      break;
    }
  } else {
    switch ((int)quest_state_get(side)) {
    case 0:
      break;
    case 1: {
      char *st = (char *)&quest_mes_phase;
      if (STATE(side) == -1) {
        quest_mes_set(side, quest_running_text);
        STATE(side) = 0;
      }
      break;
    }
    case 2: {
      char *st = (char *)&quest_mes_phase;
      if (STATE(side) == 1) {
        if (TIMER(side) > 0xe2) {
          quest_mes_set(side, quest_result_send_text);
          STATE(side) = 0;
        }
      } else {
        char *st = (char *)&quest_mes_phase;
        if (STATE(side) == -1) {
          quest_mes_set(side, quest_result_sending_text);
          STATE(side) = 0;
        }
      }
      break;
    }
    }
  }
  {
    short *p = &TIMER(side);
    if (*p != 0) {
      short *q;
      if (pause_frame_held() == 0) (*p)++;
      q = &TIMER(side);
      if (*q > 0x1aa) {
        short *w = q;
        short t;
        if (MODE(side) == 1) t = 0x146; else t = 0;
        *w = t;
      }
    }
  }
}

bool quest_mes_visible(int side)
{
  return (ushort)(quest_mes_timer[(char)side] - 1U) < 0xe1;
}

void quest_mes_draw(int side_arg)
{
  char side;
  float layout[6];
  char buf[36];
  float reveal;
  float half_len;
  float width;
  float *e;
  int timer;
  char *text;
  
  side = (char)side_arg;
  layout[0] = 8.0;
  layout[1] = 24.0;
  layout[2] = 1.0;
  layout[3] = 608.0;
  layout[4] = 616.0;
  layout[5] = -1.0;
  timer = quest_mes_timer[side];
  if (13 < timer) {
    if (timer < 30) {
      reveal = (float)(timer - 14) * 0.0625;
    }
    else {
      timer = quest_mes_timer[side];
      if (0xd1 < timer) {
        if (timer < 0xe2) reveal = 1.0 - (float)(timer - 0xd2) * 0.0625;
        else reveal = 0.0;
      }
      else reveal = 1.0;
    }
  }
  else reveal = 0.0;
  half_len = (float)strlen(quest_mes_text[side]) * 0.5;
  if (net_ready_latched() == 0 || card_record_get(side)->in_use == 0) {
    return;
  }
  if (side == 0) {
    strncpy(buf,quest_mes_text[0],0x23);
    buf[(uchar)(int)(reveal * half_len) * 2] = side;
    sprite_text_put(&euc_text_font,40.0,82.0,0.008,0,(byte *)buf);
    width = (float)(strlen(buf) + 4);
  }
  else {
    text = quest_mes_text[side];
    text += ((uchar)(int)half_len - (uchar)(int)(reveal * half_len)) * 2;
    sprite_text_put(&euc_text_font,600.0,82.0,0.008,2,(byte *)text);
    width = (float)(strlen(text) + 4) * -1.0;
  }
  sprite_put_xyz(0x21,layout[side * 3],80.0,0.008);
  sprite_put_xyz_scaled(0x12,layout[side * 3 + 1],80.0,0.009,width,1.0);
  sprite_colors_set(0xff8000,0);
  sprite_put_xyz_scaled(0x13,width * 8.0 + layout[side * 3 + 1],80.0,0.009,layout[side * 3 + 2],1.0);
  sprite_colors_reset();
}

void quest_mes_draw_all(void)
{
  int visible;
  int side;
  
  side = 0;
  do {
    visible = quest_mes_visible(side);
    if (visible != 0) {
      quest_mes_draw(side);
    }
    quest_mes_update(side);
    side = (int)(char)((char)side + '\x01');
  } while (side < 2);
  return;
}

void quest_start_plate_flash(int side_arg)
{
  int ready;
  char *entry_flags = (char *)(int)side_entry_flags;
  int side;
  float scale_x;
  float fade_alpha;
  float alpha;
  float scale_y;
  
  side = (int)(char)side_arg;
  if ((((quest_state_seen[side] == 1) &&
       (((uint)(int)*(char *)(entry_flags + side) >> 2 & 1) != 0)) &&
      (ready = net_ready_latched(), ready != 0)) && (0x3b < quest_mes_timer[side])) {
    if (quest_mes_visible(side) != 0) {
      if (quest_mes_timer[side] < 0xa0) {
        alpha = (float)(quest_mes_timer[side] + -0x3c) / 100.0;
      }
      else {
        alpha = 1.0;
      }
      if (quest_mes_timer[side] < 100) {
        scale_x = ((float)(quest_mes_timer[side] + -0x3c) / 40.0) * 4.0;
      }
      else {
        scale_x = 4.0;
      }
      if (quest_mes_timer[side] < 0x82) {
        scale_y = 3.0 - ((float)(quest_mes_timer[side] + -0x3c) / 70.0) * 3.0;
      }
      else {
        scale_y = 0.0;
      }
      if (side == 0) {
        sprite_put_xyz_alpha(quest_plate_sprites[player_chara.side0][0],0.0,52.0,0.08,alpha);
      }
      else {
        sprite_put_xyz_alpha(quest_plate_sprites[(&player_chara.side0)[side]][side],512.0,52.0,0.08,alpha);
      }
      sprite_next_blend_set(0);
      if (side == 0) {
        sprite_put_xyz_scaled_alpha_flags(0x9cd,72.0,60.0,0.03,scale_x,scale_y,alpha,0xb);
        sprite_put_xyz_scaled_alpha_flags(0x9ce,72.0,60.0,0.03,scale_x,scale_y,alpha,9);
      }
      else {
        sprite_put_xyz_scaled_alpha_flags(0x9cd,568.0,60.0,0.03,scale_x,scale_y,alpha,0x19);
        sprite_put_xyz_scaled_alpha_flags(0x9ce,568.0,60.0,0.03,scale_x,scale_y,alpha,0x1b);
      }
      sprite_next_blend_normal();
    }
    else {
      if (quest_mes_timer[side] < 0x178) {
        fade_alpha = 1.0 - (float)(quest_mes_timer[side] + -0x146) / 50.0;
      }
      else {
        fade_alpha = (float)(quest_mes_timer[side] + -0x178) / 50.0;
      }
      if (side == 0) {
        sprite_put_xyz_alpha(quest_plate_sprites[player_chara.side0][0],0.0,52.0,0.08,fade_alpha);
      }
      else {
        sprite_put_xyz_alpha(quest_plate_sprites[(&player_chara.side0)[side]][side],512.0,52.0,0.08,fade_alpha);
      }
    }
  }
  return;
}

void mes_layout_draw(int group, float z)
{
  mes_label_rec *label = game_mes_table[group].labels;
  for (; label->sprite != -1; label++) {
    sprite_put_xyz(label->sprite, label->x, label->y, z);
  }
}

void mes_pop_slot_reset(int slot_arg)
{
  int slot = (char)slot_arg;
  mes_pop_state[slot].state = 0;
  mes_pop_state[slot].timer = 0;
}

int mes_letter_pop_draw(int group_arg, int slot_arg, float z_arg)
{
  float z = z_arg;
  int group = group_arg;
  char slot = slot_arg;
  mes_label_rec *label = game_mes_table[group].labels;
  int count;
  int i;
  short *state;
  short *timer;
  int t;
  float step;
  float scale;
  float alpha;
  float offset;
  int done;

  count = 0;
  i = 0;
  offset = 0.0f;
  state = &mes_pop_state[slot].state;
  timer = &mes_pop_state[slot].timer;
  done = 0;
  while (label->sprite != -1) {
    label++;
    count++;
  }
  if (*state == 0) {
    *timer = count * 10;
    *state = 1;
  }
  else if (*state == 1 && pause_frame_held() == 0) {
    (*timer)--;
  }
  if (*timer == 0) {
    *state = -1;
  }
  for (label = game_mes_table[group].labels; label->sprite != -1; label++, i++) {
    if ((count * 10 - *timer) / 2 < count) if ((count * 10 - *timer) / 2 < i) break;
    t = *timer + i * 2;
    if (count * 10 - 10 < t) {
      t = t % 10;
      step = (float)t / 10.0f;
      if (t == 0) step = 1.0f;
      scale = step + step + 1.0f;
      alpha = 1.0f - step;
      if (alpha < 0.0f) alpha = 0.0f;
      done = 0;
    }
    else {
      scale = 1.0f;
      alpha = 1.0f;
      done = 1;
    }
    sprite_next_blend_set(0);
    sprite_colors_set(0xffffff, 0x40);
    sprite_put_xyz_scaled_alpha_angle_flags(label->sprite, label->x + offset, label->y, z, scale, scale, 0, alpha, 0x100a);
    sprite_put_xyz_scaled_alpha_angle_flags(label->sprite, label->x + offset * 0.5f, label->y, z + 1e-05f, scale, scale, 0, alpha * 0.5f, 0x100a);
    sprite_colors_reset();
    sprite_next_blend_normal();
    if (alpha > 0.99f) {
      sprite_put_xyz_scaled_alpha_flags(label->sprite, label->x, label->y, z - 0.0001f, scale, scale, 0.3f, 0x100a);
    }
  }
  return done;
}

void mes_sprite_draw(sprite, x, y, z, scale_x, scale_y, rot, alpha) int sprite; float x; float y; float z; float scale_x; float scale_y; ushort rot; float alpha;
{
  sprite_colors_set(0xffffff,0x555555);
  ((void (*)(int,float,float,float,float,float,int,float,int))sprite_put_xyz_scaled_alpha_angle_flags)(sprite,x,y,z,scale_x,scale_y,(short)rot,alpha,0x100a);
  sprite_colors_reset();
}

int mes_spin_in_draw(int group, int slot_arg, float z)
{
  mes_label_rec *label;
  int done;
  float step;
  float dx;
  float scale;
  ushort rot;
  int t;
  char index;
  int slot = (char)slot_arg;
  char *st;

  t = 0;
  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 3:
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      step = (float)tm[slot] * 0.25;
      dx = step * (label->x - 320.0) * 0.5;
      scale = (1.0 - step) * 0.2 + 0.6;
      mes_sprite_draw(label->sprite, dx + label->x, label->y, z, scale, scale, 0, 1.0);
    }
    if (mes_anim_timer[slot] > 3) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 4:
    done = 1;
    index = 0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      t = (short)(mes_anim_timer[slot] - index * 4);
      if (t < 0) dx = (label->x - 320.0) * 0.5;
      else if (t < 12) dx = ((label->x - 320.0) * 0.5 * (float)(12 - t)) / 12.0;
      else dx = 0.0;
      if (t < 0) rot = 0;
      else if (t < 3) rot = (float)t * 16384.0 / 3.0;
      else if (t < 12) rot = 16384.0 - (float)(t - 3) * 16384.0 / 9.0;
      else rot = 0;
      if (t <= 2) scale = 0.6;
      else if (t < 12) scale = ((float)(t - 3) / 9.0) * 1.4 + 0.6;
      else scale = 1.0;
      mes_sprite_draw(label->sprite, dx + label->x, label->y, z, scale, scale, rot, 1.0);
      index++;
    }
    if (t >= 12) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 5:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_sweep_in_draw(float z, int group, int slot_arg)
{
  mes_label_rec *label;
  int done;
  float edge_x;
  float alpha;
  float weight;
  int slot = (char)slot_arg;
  char *st;

  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    edge_x = (float)mes_anim_timer[slot] / 45.0 * 640.0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      if (edge_x > label->x) {
        mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
        alpha = 0.5 - (edge_x - label->x) / 1280.0;
        if (alpha < 0.0) alpha = 0.0;
      }
      else {
        mes_sprite_draw(label->sprite, edge_x, label->y, z + 0.0001, 1.0, 1.0, 0, 1.0);
        break;
      }
      mes_sprite_draw(label->sprite, ((weight = 4.0) * label->x + edge_x) / 5.0, label->y, z, 1.0, 1.0, 0, alpha);
    }
    if (mes_anim_timer[slot] > 0xb3) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_squeeze_in_draw(float z, int group, int slot_arg)
{
  mes_label_rec *label;
  int done;
  float scale_x;
  float scale_y;
  float dx;
  short step;
  char count;
  char i;
  int slot = (char)slot_arg;
  char *st;

  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    count = 0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      count++;
    }
    i = 0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      short t = tm[slot];
      if (i < (char)(count / 2)) {
        step = t - i * 4;
      }
      else {
        step = t - (count - i) * 4 + 2;
      }
      if (step >= 0) {
        if (step < 16) scale_x = step * 0.0625 * 0.4 + 0.6;
        else scale_x = 1.0;
      }
      else scale_x = 0.6;
      if (step >= 0) {
        if (step < 16) scale_y = 6.0 - step * 0.0625 * 5.0;
        else scale_y = 1.0;
      }
      else scale_y = 6.0;
      if (step < 1) dx = 320.0 - label->x;
      else if (step < 16) dx = (320.0 - label->x) / step;
      else dx = 0.0;
      mes_sprite_draw(label->sprite, dx + label->x, label->y, z, scale_x, scale_y, 0, 1.0);
      i++;
    }
    if (mes_anim_timer[slot] > 39) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 4:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_slide_in_draw(int group, char slot_arg, float z)
{
  mes_label_rec *label;
  int done;
  float dx;
  short t;
  int slot = slot_arg;
  char *st;

  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      t = tm[slot];
      dx = (1.0 - (float)(t * t) * 0.00390625) * 640.0 - 20.0;
      mes_sprite_draw(label->sprite, dx + label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    if (mes_anim_timer[slot] > 15) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      dx = ((float)tm[slot] / 60.0) * 40.0 - 20.0;
      mes_sprite_draw(label->sprite, dx + label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_jitter_in_draw(int group, char slot_arg, float z)
{
  mes_label_rec *label;
  int done;
  float noise;
  float fade;
  float jitter_x;
  float jitter_y;
  float dist;
  float dist_in;
  float out_x;
  float out_y;
  char i;
  char j;
  int slot = slot_arg;

  done = 0;
  switch (spot_state()[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    spot_state()[slot]++;
  case 3:
  case 5:
  case 7:
  case 9:
  case 11:
    done = 1;
    i = 0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      noise = 0.1;
      for (j = 0; j < spot_state()[slot] * i + 5; j++) {
        noise = noise * 3.66 * (1.0 - noise);
      }
      jitter_x = (float)((int)(noise * 100.0) % 3 - 1);
      fade = (float)mes_anim_timer[slot] / 60.0;
      dist = fade * 60.0;
      dist_in = dist - 60.0;
      jitter_y = (float)((int)(noise * 1000.0) % 3 - 1);
      jitter_y *= dist_in;
      out_x = (float)((int)(noise * 10000.0) % 3 - 1) * dist;
      out_y = (float)((int)(noise * 100000.0) % 3 - 1) * dist;
      mes_sprite_draw(label->sprite, jitter_x * dist_in + label->x, jitter_y + label->y, z, 1.0, 1.0, 0, fade);
      mes_sprite_draw(label->sprite, out_x + label->x, out_y + label->y, z, 1.0, 1.0, 0, 1.0 - fade);
      i++;
    }
    if (mes_anim_timer[slot] > 59) {
      mes_anim_timer[slot] = 0;
      spot_state()[slot]++;
    }
    break;
  case 2:
  case 4:
  case 6:
  case 8:
  case 10:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    if ((float)mes_anim_timer[slot] >= 60.0) {
      mes_anim_timer[slot] = 0;
      spot_state()[slot]++;
    }
    break;
  case 12:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_triple_zoom_draw(float z, int group, char slot_arg)
{
  mes_label_rec *label;
  int done;
  short times[3];
  char i;
  float step;
  float scale;
  float alpha;
  int slot = slot_arg;
  char *st;

  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      times[0] = tm[slot];
      times[1] = times[0] - 8;
      times[2] = times[0] - 16;
      for (i = 0; i < 3; i++) {
        if (times[i] < 0) {
          times[i] = 0;
        }
        else if (times[i] > 25) {
          times[i] = 25;
        }
        step = (float)times[i] / 25.0;
        alpha = step * 0.7 + 0.3;
        scale = 3.0 - (step + step);
        mes_sprite_draw(label->sprite, label->x, label->y, z, scale, scale, 0, alpha);
      }
    }
    if (mes_anim_timer[slot] > 40) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_number_burst_draw(float z, int group, int slot_arg, int flash_arg)
{
  int slot = (char)slot_arg;
  char flash = flash_arg;
  int done = 0;
  mes_label_rec *label;
  float step;
  float scale;
  float alpha;
  uint gray;
  char *st;

  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    if (mes_anim_timer[slot] > 22) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      step = (float)tm[slot] / 20.0;
      scale = step * 3.0 + 1.0;
      alpha = 1.0 - step;
      mes_sprite_draw(label->sprite, label->x, label->y, z - 0.0001, scale, scale, 0, alpha);
    }
    if (mes_anim_timer[slot] > 19) {
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
  case 4:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      if (flash != 0 && tm[slot] <= 29) {
        gray = (tm[slot] * tm[slot] * 255) / 900 & 0xff;
        sprite_colors_set(0xffffff, gray << 16 | gray << 8 | gray);
        sprite_put_flags_xyz(label->sprite, label->x, label->y, z, 0x100a);
        sprite_colors_reset();
      }
      else {
        mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
      }
    }
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

int mes_flip_in_draw(int group, int slot_arg, float z)
{
  mes_label_rec *label;
  int done;
  float scale_y;
  int t;
  char index;
  int slot = (char)slot_arg;
  char *st;

  t = 0;
  done = 0;
  {
    char *sp = (char *)&mes_anim_state;
    switch (sp[slot]) {
  case 1:
    mes_anim_timer[slot] = 0;
    st = (char *)&mes_anim_state;
    st[slot] = st[slot] + 1;
  case 2:
    index = 0;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      short *tm = mes_anim_timer;
      t = (short)(tm[slot] - index * 2);
      if (t < 0) scale_y = 0.0;
      else if (t < 40) scale_y = sin_radians(((float)t * -4.7124) / 40.0);
      else scale_y = 1.0;
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, scale_y, 0, 1.0);
      index++;
    }
    if (t > 40) {
      mes_anim_timer[slot] = 0;
      st = (char *)&mes_anim_state;
      st[slot] = st[slot] + 1;
    }
    break;
  case 3:
    done = 1;
    for (label = game_mes_table[group].labels; label->sprite != -1; label++) {
      mes_sprite_draw(label->sprite, label->x, label->y, z, 1.0, 1.0, 0, 1.0);
    }
    break;
  }
  }
  if (pause_frame_held() == 0) {
    mes_anim_timer[slot]++;
  }
  return done;
}

void rob_status_marks_draw(void)
{
  rob_rec *rob;
  uint i;
  float y = 150.0;
  int frame;

  for (i = 0; (int)i < 2; i++) {
    rob = rob_get(i & 0xff);
    if (rob->flags & 4) {
      if (rob->state & 0x80000) {
        frame = vblank_count % 8;
        sprite_put_xyz(rob_status_sprites_1[frame],rob_status_mark_x[i],y,0.03);
      }
      else if (rob->flags24 & 0x1000000) {
        frame = vblank_count % 8;
        sprite_put_xyz(rob_status_sprites_2[frame],rob_status_mark_x[i],y,0.03);
      }
    }
  }
}

void win_streak_draw(int side_arg, uint blink)
{
  int places[3];
  char buf[12];
  int side;
  int tier;
  int j;
  int digit_no;
  win_streak_part_rec *layout;
  win_streak_part_rec *part;
  int *credits;
  int *digits;

  side = (char)side_arg;
  places[2] = win_streak / 100;
  places[1] = (win_streak - places[2] * 100) / 10;
  places[0] = win_streak % 10;
  if (win_streak > 999) {
    places[0] = places[1] = places[2] = 9;
  }
  if (champion_side == side) {
    if (win_streak == 1) {
      sprite_put_xy(0x98e, win_streak_mark_x[side], 16.0f);
    }
    else if (win_streak > 1) {
      if (blink & 0x40) {
        sprite_put_xy(0x98e, win_streak_mark_x[side], 16.0f);
      }
      else {
        sprite_colors_set(0xffff00, 0);
        if (win_streak <= 9) tier = 0;
        else if (win_streak <= 49) tier = 1;
        else if (win_streak <= 69) tier = 2;
        else { tier = win_streak > 99; tier += 3; }
        if (!system_param_country_get()) layout = win_streak_layouts[tier][side];
        else layout = win_streak_layouts_overseas[tier][side];
        j = 0;
        digit_no = 0;
        for (; layout[j].sprite != -1; j = (char)(j + 1)) {
          part = (win_streak_part_rec *)(j * 12 + (int)layout);
          if (part->sprite == 0x98c) {
            sprite_colors_set(0xff0000, 0);
            if (!system_param_country_get()) {
              digits = win_streak_digit_sprites;
              sprite_put_xy(digits[places[digit_no]], part->x, part->y - 1.0f);
            }
            else {
              sprintf(buf, "%d", places[digit_no]);
              sprite_font_text_put(&ascii_font, buf, layout[j].x, layout[j].y, 0, 0.04f);
            }
            digit_no = (char)(digit_no + 1);
          }
          else {
            sprite_colors_set(0xffff00, 0);
            sprite_put_xy(layout[j].sprite, layout[j].x, layout[j].y);
          }
        }
        sprite_colors_reset();
      }
    }
  }
  else if (!((({ char *b = (char *)side_entry_flags; b[side]; }) >> 2) & 1)) {
    if (credit_copy.separate == 0) credits = (int *)&credit_copy;
    else credits = (int *)((char *)&credit_copy + side * 16);
    if (credit_copy.credit_mode != 1 && credits[0] + credits[1] != 0) return;
    if (credit_copy.credit_mode == 1) return;
    if (blink & 0x40) return;
    if (win_streak <= 4) tier = 0;
    else if (win_streak - 5U <= 4) tier = 1;
    else if (win_streak - 10U <= 9) tier = 2;
    else tier = 3;
    if (!system_param_country_get()) layout = win_streak_other_layouts[tier][side];
    else layout = win_streak_other_layouts_overseas[side];
    for (j = 0; layout[j].sprite != -1; j = (char)(j + 1)) {
      if (layout[j].sprite == 0x97e) sprite_colors_set(0xff0000, 0);
      else sprite_colors_set(0xffff00, 0);
      sprite_put_xy(layout[j].sprite, layout[j].x, layout[j].y);
    }
    sprite_colors_reset();
  }
  else {
    if (win_streak < 1) return;
    sprite_put_xy(0x98d, win_streak_player_mark_x[side], 16.0f);
  }
}

void text_right_align_draw(char *text, uint max_len, float x, float y, float z)
{
  uchar count;
  uchar i;
  uchar limit = max_len;
  byte cell[2] = {0};

  for (count = 0; count < limit && text[count] != 0; count++)
    ;
  for (i = 0; i < count; i++) {
    cell[0] = text[i];
    sprite_text_put(&euc_text_font,x - (float)(count - i) * 12.0,y,z,0x80,cell);
  }
}

void card_times_draw(int side, float x, float y, float z)
{
  card_image_rec *img;
  uint color;
  int count;
  char buf [12];
  
  img = card_image_get((int)(char)side);
  count = img->header.use_count;
  if (count < 0) count = 0;
  if (count < 4) color = 0xff0000;
  else if (count < 10) color = 0xe0e000;
  else color = 0xffffff;
  sprite_colors_set(color,0);
  sprintf(buf,"%d",count);
  text_right_align_draw(buf,3,x - 19.0,y,z);
  sprite_colors_reset();
  sprite_text_put(&euc_text_font,x,y,z,2,card_times_suffix);
  return;
}

void card_panel_type0_draw(card_image_rec *img, char side, float x, float y, float z)
{
  float z_line;
  float z_sprite;
  
  z_sprite = z - 3e-05;
  z_line = z - 2e-05;
  card_panel_base_draw(img,(int)(char)side,x,y,z_sprite,z_line);
  card_win_count_draw((uint)img->header.all_win,x,y + card_panel_stats_row_y,z_sprite,z_line);
  card_lose_count_draw((uint)img->header.all_lose,x,y + card_panel_stats_row_y,z_sprite,z_line);
  card_win_rate_draw(img,x,y + card_panel_stats_row_y,z_sprite,z_line);
  return;
}

void card_panel_type1_draw(card_image_rec *img, char side, float x, float y, float z)
{
  float z_sprite;
  float z_line;
  
  z_sprite = z - 3e-05;
  z_line = z - 2e-05;
  card_panel_base_draw(img,(int)(char)side,x,y,z_sprite,z_line);
  sprite_put_xyz(0x5dc2,x + card_type1_label_x,y + card_panel_stats_row_y,z_sprite);
  card_panel_underline_draw(x + card_type1_label_x,y + card_panel_stats_row_y,z_line,card_type1_label_w);
  card_win_count_draw((uint)img->header.all_win,x + card_type1_count_x,y + card_panel_stats_row_y,z_sprite,
               z_line);
  return;
}

void card_panel_type2_draw(card_image_rec *img, char side, float x, float y, float z)
{
  card_panel_base_draw(img,(int)(char)side,x,y,z - 3e-05,z - 2e-05);
  card_win_rate_draw(img,x + card_type2_win_rate_dx,y + card_panel_stats_row_y,z - 3e-05,z - 2e-05
              );
  return;
}

void card_panel_type3_draw(card_image_rec *img, char side, float x, float y, float z)
{
  card_panel_base_draw(img,(int)(char)side,x,y,z - 3e-05,z - 2e-05);
  card_match_count_draw(img,x,y + card_panel_stats_row_y,z - 3e-05,z - 2e-05);
  return;
}

void card_panel_type4_draw(card_image_rec *img, char side, float x, float y, float z)
{
  card_panel_base_draw(img,(int)(char)side,x,y,z - 3e-05,z - 2e-05);
  return;
}

void card_panel_side_frame_draw(int side_arg, float x_arg, float y_arg, float z_arg)
{
  float x = x_arg;
  float y = y_arg;
  float z = z_arg;
  char side = side_arg;
  float t;
  float *offset;
  
  sprite_put_xyz(0x5dc4,x,y,z);
  t = x + card_frame_mid_x;
  sprite_put_xyz_scaled(0x5dc5,t,y,z,card_frame_mid_scale,1.0);
  t += card_frame_mid_scale * card_frame_mid_unit;
  sprite_put_xyz(0x5dc6,t,y,z);
  offset = card_frame_side_offsets[side];
  t = x + offset[0];
  y += offset[1];
  sprite_put_xyz(card_frame_side_sprites[side],t,y,z);
}

void card_panel_underline_draw(float x_arg, float y_arg, float z, float width)
{
  float mid_scale;
  float x;
  float y;
  if (mode_get_main() == MODE_MAIN_SELECTOR) {
    mid_scale = (width - (card_underline_end_w + card_underline_end_w)) / card_underline_unit;
    x = x_arg - card_underline_end_w;
    y = y_arg + card_underline_dy;
    sprite_put_xyz(0x5dc7, x, y, z);
    x = x + card_underline_mid_x;
    if (mid_scale > 0.0) {
      sprite_put_xyz_scaled(0x5dc8, x, y, z, mid_scale, 1.0);
    }
    else {
      mid_scale = 0.0;
    }
    x = mid_scale * card_underline_unit + x;
    sprite_put_flags_xyz(0x5dc7, x, y, z, 0x10);
  }
}

void card_panel_base_draw(card_image_rec *img, char side, float x, float y, float z_sprite, float z_line)
{
  e_mode_main mode;
  uint level;
  float shift;
  int is_none;
  uint valid;

  level = img->header.level;
  is_none = level == 0xff;
  valid = is_none == 0;
  level = level & -valid;
  if (0xed < level) {
    level = 0xed;
  }
  mode = mode_get_main();
  if (mode == MODE_MAIN_SELECTOR) {
    card_name_rank_draw(&img->body,level,x,y + card_panel_row_y,z_sprite,z_line);
    card_use_count_draw((int)img->header.use_count,x,y + card_panel_kyu_row_y,z_sprite,z_line);
    shift = 0.0;
  }
  else {
    card_team_name_draw(side,x,y + card_panel_row_y,z_sprite);
    shift = card_rank_value_shift;
  }
  if (level < 10) {
    card_rank_value_draw(img,shift + x,y + card_panel_kyu_row_y,z_sprite,z_line);
  }
}

void card_rank_value_draw(card_image_rec *img, float x, float y, float z_sprite, float z_line)
{
  int digits;
  int value;
  float label_x;
  
  label_x = x + card_panel_text_x;
  sprite_put_xyz(0x5dce,label_x,y,z_sprite);
  value = (uint)img->header.level * 100 + (uint)img->body.exp;
  card_panel_underline_draw(label_x,y,z_line,card_rank_label_w);
  if (999 < value) {
    value = 999;
  }
  digits = (int)(float)(int)digits5_draw(value,x + card_rank_digits_x,y,z_sprite);
  card_panel_underline_draw((x + card_rank_digits_x) - ((float)digits - 1.0) * 9.0,y,z_line,
               (float)digits * 9.0);
  return;
}

void card_name_rank_draw(card_body_rec *body, uint level_arg, float panel_x, float y, float z_sprite, float z_line)
{
  uchar level;
  float text_x;
  float name_x;
  float width;
  float max_w;
  char buf [100];
  
  level = level_arg;
  text_x = card_panel_text_x;
  if (card_body_name_empty(body) != 0) {
    snprintf(buf,100,card_no_name_text);
  }
  else {
    card_body_name_get(body,buf,100);
  }
  name_x = panel_x + text_x;
  sprite_text_put(&euc_text_font,name_x,y,z_sprite,0x20,buf);
  width = sprite_text_end_x() - name_x;
  if (width < 0.0) width = 0.0;
  else if (width > (max_w = card_name_underline_max)) width = max_w;
  card_panel_underline_draw(panel_x + text_x,y,z_line,width);
  sprintf(buf,"%s",level_name_get(level));
  sprite_text_put(&euc_text_font,panel_x + card_rank_title_x,y,z_sprite,2,buf);
  {float half_w; half_w = (float)(int)strlen(buf) * card_rank_title_char_w * 0.5;
  card_panel_underline_draw((panel_x + card_rank_title_x) - half_w,y,z_line,half_w);}
  return;
}

void card_team_name_draw(char side, float x, float y, float z)
{
  byte buf [100];
  
  card_team_label_x = card_panel_text_x;
  if (team_names[side][0] != '\0') {
    sprintf(buf,team_label_text);
    sprite_text_put(&euc_text_font,x + card_team_label_x,y,z,0,buf);
    sprintf(buf,"%s",team_names[side]);
    sprite_text_put(&euc_text_font,x + card_team_name_x,y,z,2,buf);
  }
}

void card_win_count_draw(int count, float x, float y, float z_sprite, float z_line)
{
  float digits;
  if (99999 < count) {
    count = 99999;
  }
  digits = (float)(int)digits5_draw(count,x + card_count_06_digits_x,y,z_sprite);
  sprite_put_xyz(0x5dcd,x + card_count_06_label_x,y,z_sprite);
  card_panel_underline_draw((x + card_count_06_label_x) - digits * 9.0,y,z_line,digits * 9.0 + card_count_06_pad);
}

void card_lose_count_draw(int count, float x, float y, float z_sprite, float z_line)
{
  float digits;
  
  if (99999 < count) {
    count = 99999;
  }
  digits = (float)(int)digits5_draw(count,x + card_count_08_digits_x,y,z_sprite);
  sprite_put_xyz(0x5dcf,x + card_count_08_label_x,y,z_sprite);
  card_panel_underline_draw((x + card_count_08_label_x) - digits * 9.0,y,z_line,digits * 9.0 + card_count_08_pad);
  return;
}

void card_win_rate_draw(card_image_rec *img, float x, float y, float z_sprite, float z_line)
{
  int matches;
  float rate;
  float digits;

  rate = 0.0;
  matches = (uint)img->header.all_win + (uint)img->header.all_lose;
  if (matches != 0) {
    rate = (float)img->header.all_win / (float)matches;
  }
  if (0.99999 < rate) {
    rate = 1.0;
  }
  sprite_put_xyz(0x5ddb,x + card_win_rate_header_x,y,z_sprite);
  card_panel_underline_draw(x + card_win_rate_header_x,y,z_line,card_win_rate_header_w);
  digits = (float)(int)digits5_draw((ushort)(int)(rate * 100.0),x + card_win_rate_digits_x,y,z_sprite);
  sprite_text_put(&euc_text_font,x + card_win_rate_point_pos[0],y + card_win_rate_point_pos[1],z_sprite,0x80,
               (byte *)0xc24237c);
  digits5_draw((ushort)((ushort)(int)(rate * 1000.0) % 10u),x + card_win_rate_tenths_x,y,z_sprite);
  sprite_put_xyz(0x5dc3,x + card_win_rate_percent_pos[0],y + card_win_rate_percent_pos[1],z_sprite);
  digits = digits + 1.0;
  card_panel_underline_draw(((x + card_win_rate_percent_pos[0]) - digits * 9.0) + card_win_rate_underline_dx,y,z_line,digits * 9.0 + card_win_rate_pad);
}

void card_match_count_draw(card_image_rec *img, float x, float y, float z_sprite, float z_line)
{
  float digits;
  int matches;
  sprite_put_xyz(24000,x + card_match_header_x,y,z_sprite);
  card_panel_underline_draw(x + card_match_header_x,y,z_line,card_match_header_w);
  matches = img->header.all_win + img->header.all_lose;
  if (matches > 99999) {
    matches = 99999;
  }
  digits = (int)digits5_draw(matches,x + card_match_digits_x,y,z_sprite);
  sprite_put_xyz(0x5dc1,x + card_match_suffix_x,y,z_sprite);
  card_panel_underline_draw((x + card_match_suffix_x) - digits * 9.0,y,z_line,digits * 9.0 + card_match_pad);
  return;
}

void card_use_count_draw(uint count_arg, float x, float y, float z_sprite, float z_line)
{
  int digits;
  uint color;
  uint count;
  int positive;
  
  sprite_put_xyz(0x5dc9,x + card_use_header_x,y,z_sprite);
  card_panel_underline_draw(x + card_use_header_x,y,z_line,card_use_header_w);
  if ((int)count_arg < 0) positive = 0; else positive = 1;
  count = count_arg & -positive;
  if (999 < (int)count) {
    count = 999;
  }
  color = ((int)count <= 3) ? 0xff0000 : (((int)count <= 9) ? 0xe0e000 : 0xffffff);
  sprite_colors_set(color,0);
  digits = digits5_draw(count,x + card_use_digits_x,y,z_sprite);
  sprite_colors_reset();
  sprite_put_xyz(0x5dcc,x + card_use_suffix_x,y,z_sprite);
  card_panel_underline_draw((x + card_use_suffix_x) - (float)digits * 9.0,y,z_line,
               (float)digits * 9.0 + card_use_pad);
  return;
}

int digits5_draw(uint value, float x, float y_arg, float z)
{
  int places[5];
  int sprites[5];
  int digits;
  float y;

  if ((int)value > 99999) {
    value = 99999;
  }
  y = y_arg + card_digit_dy;
  places[4] = (int)value / 10000;
  places[3] = (int)value / 1000 % 10;
  places[2] = (int)value / 100 % 10;
  places[1] = (int)value / 10 % 10;
  places[0] = (int)value % 10;
  {
  float one = 1.0;
  sprites[0] = card_digit_sprites[places[0]];
  sprite_put_xyz_alpha(sprites[0],x,y,z,one);
  digits = 1;
  if (places[1] != 0 || ((places[4] != 0 || places[3] != 0 || places[2] != 0) && places[1] == 0)) {
    sprites[1] = card_digit_sprites[places[1]];
    sprite_put_xyz_alpha(sprites[1],x - card_digit_step,y,z,one);
    digits = 2;
  }
  if (places[2] != 0 || ((places[4] != 0 || places[3] != 0) && places[2] == 0)) {
    sprites[2] = card_digit_sprites[places[2]];
    sprite_put_xyz_alpha(sprites[2],x - (card_digit_step + card_digit_step),y,z,one);
    digits = 3;
  }
  if (places[3] != 0 || (places[4] != 0 && places[3] == 0)) {
    sprites[3] = card_digit_sprites[places[3]];
    sprite_put_xyz_alpha(sprites[3],x - card_digit_step * 3.0,y,z,one);
    digits = 4;
  }
  if (places[4] != 0) {
    sprites[4] = card_digit_sprites[places[4]];
    sprite_put_xyz_alpha(sprites[4],x - card_digit_step * 4.0,y,z,one);
    digits = 5;
  }
  }
  return digits;
}

void console_side_value_set(char side, uchar value)
{
  if (value < 5) {
    console_side_value[side] = value;
  }
  return;
}

uchar console_side_value_get(char side)
{
  return console_side_value[side];
}

void team_name_set(char side, char *name)
{
  char *p;
  if (name != (char *)0x0) {
    p = team_names[side];
    strncpy(p,name,0x14);
    p[0x14] = 0;
  }
}

void card_panel_draw(int side_arg, float fade)
{
  card_image_rec *img;
  int placed;
  int lines;
  int level;
  uint kind;
  char side;
  float dy;
  float *rows;
  void (*draw[5])(card_image_rec *img, char side, float x, float y, float z);
  
  side = (char)side_arg;
  draw[0] = card_panel_type0_draw;
  draw[1] = card_panel_type1_draw;
  draw[2] = card_panel_type2_draw;
  draw[3] = card_panel_type3_draw;
  draw[4] = card_panel_type4_draw;
  img = card_image_get(side);
  level = img->header.level;
  if (level == 0xff) {
    level = 0;
  }
  if (level > 0xed) {
    level = 0xed;
  }
  kind = 7;
  kind &= img->body.misc.w >> 0xd;
  if (kind > 4) {
    kind = 0;
  }
  if (mode_get_main() == MODE_MAIN_SELECTOR) {
    rows = (float *)&card_panel_sel_rows;
    card_panel_row_y = rows[0];
    card_panel_stats_row_y = rows[1];
    card_panel_kyu_row_y = rows[2];
    if (sel_data_layer_done(side) != 0) {
      (*draw[kind])(img,side,card_panel_select_x[side],card_panel_select_y,0.008);
    }
  }
  else {
    lines = card_panel_line_count(side,level,kind);
    placed = card_panel_rows_place(side,level,kind,lines);
    if (placed != 0) {
      dy = 0.0;
      if (((int)mission_active_get() != -1) && (play_style == '\x01')) {
        dy = dy + card_panel_mission_dy;
      }
      (*draw[kind])(img,side,card_panel_game_x[side] + card_panel_content_dx[side],
                         dy + card_panel_game_y,0.008);
      card_panel_side_frame_draw(side,card_panel_game_x[side],dy + card_panel_game_y,0.008);
    }
  }
  mode_get_main();
  return;
}

int card_panel_line_count(int side_arg, int level, uint kind)
{
  int count, flag;
  flag = team_names[(char)side_arg][0] == 0;
  count = flag == 0;
  flag = kind > 3;
  count += flag == 0;
  flag = level > 9;
  count += flag == 0;
  return count;
}

int card_panel_rows_place(int side_arg, int level, uint kind, int lines)
{
  int side = (char)side_arg;
  int placed;
  uint row;
  float *rows;
  int base;
  
  if (lines <= 0) placed = 0;
  else {
    base = (int)card_panel_row_sets;
    rows = *(float **)(base + lines * 4);
    card_panel_row_y = 0.0;
    card_panel_stats_row_y = 0.0;
    card_panel_kyu_row_y = 0.0;
    row = 0;
    if (team_names[side][0] != '\0') {
      card_panel_row_y = *rows;
      row = 1;
    }
    if (kind < 4) {
      card_panel_stats_row_y = rows[row];
      row = row + 1;
    }
    if (level < 10) {
      card_panel_kyu_row_y = rows[row];
    }
    placed = 1;
  }
  return placed;
}

void sel_arrow_show(int slot, char dir, float x, float y, float z)
{
  int obj;
  aet_obj_param buf;
  char *dirs;
  dirs = sel_arrow_dirs;
  
  if (dirs[slot] != dir) {
    dirs[slot] = dir;
    obj = sel_arrow_objects[slot];
    if (obj != -1) {
      aet_obj_stop(obj);
      sel_arrow_objects[slot] = -1;
    }
  }
  if (sel_arrow_objects[slot] == -1) {
    x = x - 320.0;
    y = y - 231.0;
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,"SEL_KSN_YAJI");
    aet_param_depth_range_set(&buf,z,z);
    aet_param_pos_set(&buf,x,y);
    obj = aet_obj_start(&buf);
    sel_arrow_objects[slot] = obj;
  }
  return;
}

void sel_arrow_hide(int slot)
{
  int obj;
  
  obj = sel_arrow_objects[slot];
  if (obj != -1) {
    aet_obj_stop(obj);
    sel_arrow_objects[slot] = -1;
  }
  return;
}

void name_plate_draw(int side_arg)
{
  int side;
  int timer;
  int cells;
  float x;
  float y;
  uchar buf[24];
  side = (char)side_arg;
  timer = quest_mes_timer[side];
  if (timer != 0) {
  if (timer < 14) x = 8.0 - (float)timer * 4.0;
  else {
    timer = quest_mes_timer[side];
    if (timer >= 0xe2) {
      x = 8.0;
      if (timer < 0xf0) x -= (float)(0xf0 - timer) * 4.0;
    }
    else x = -200.0;
  }
  }
  else x = 8.0;
  card_body_name_get(&card_image_get(side)->body,buf,0x15);
  cells = euc_text_cells((char *)buf);
  if (buf[0] != 0) {
    if (cells < 8) cells = 8;
    if (side == 0) {
      sprite_text_put(&euc_text_font,x + 8.0,80.0,0.04,0x20,buf);
      sprite_put_xyz_scaled_alpha(0,x,80.0,0.05,(float)(cells + 2),2.0,0.3);
      sprite_put_xyz_scaled(0x1b,x,90.0,0.05,(float)(cells + 2),1.0);
      sprite_put_xyz(0x19,x,98.0,0.05);
    }
    else {
      sprite_text_put(&euc_text_font,632.0 - x,80.0,0.04,0x22,buf);
      y = 640.0 - x;
      sprite_put_xyz_scaled_alpha_flags(0,y,80.0,0.05,(float)(cells + 2),2.0,0.3,7);
      sprite_put_xyz_scaled_flags(0x1b,y,90.0,0.05,(float)(cells + 2),1.0,7);
      sprite_put_flags_xyz(0x19,y,98.0,0.05,7);
    }
  }
}

bool level_over_31(int level)
{
  return 0x1f < (byte)level;
}

void level_badge_draw(int side_arg)
{
  int side;
  uchar level;
  uint mask;
  float shadow_alpha;
  char *pos;
  char i;
  short sprite;
  float *xy;
  uint color;

  side = (char)side_arg;
  level = card_image_get(side)->header.level;
  if (level == 0xff) level = 0;
  if (level > 0xed) level = 0xed;
  mask = 0;
  shadow_alpha = 0.4f;
  if (level_table[level].sprites[2] != -1) pos = (char *)level_badge_pos_3[side];
  else pos = (char *)level_badge_pos_2[side];
  if (level_over_31(level) == 1 || level == 31 || level == 30 || level == 29) {
    mask = 0xffffff;
    shadow_alpha = 0.8f;
  }
  i = 0;
  while (i < 3 && level_table[level].sprites[i] != -1) {
    sprite = level_table[level].sprites[i];
    sprite_colors_set(0,mask ^ 0xffffff);
    xy = (float *)((i << 3) + (int)pos);
    sprite_put_xyz_scaled_flags(sprite,xy[0],xy[1],0.04f,0.5f,0.5f,0x100a);
    sprite_colors_reset();
    sprite_colors_set(0,mask);
    sprite_put_xyz_scaled_alpha_flags(sprite,xy[0] + 1.0f,xy[1] + 1.0f,0.05f,0.5f,0.5f,shadow_alpha,0x100a);
    sprite_put_xyz_scaled_alpha_flags(sprite,xy[0] - 1.0f,xy[1] + 1.0f,0.05f,0.5f,0.5f,shadow_alpha,0x100a);
    sprite_put_xyz_scaled_alpha_flags(sprite,xy[0] - 1.0f,xy[1] - 1.0f,0.05f,0.5f,0.5f,shadow_alpha,0x100a);
    sprite_put_xyz_scaled_alpha_flags(sprite,xy[0] + 1.0f,xy[1] - 1.0f,0.05f,0.5f,0.5f,shadow_alpha,0x100a);
    sprite_colors_reset();
    i++;
  }
  if (level_over_31(level) == 1) {
    sprite_colors_set(0xb0f0ff,0);
    if (side == 0) sprite_put_xyz(0xa3b,80.0f,56.0f,0.06f);
    else sprite_put_flags_xyz(0xa3b,0x10,476.0f,56.0f,0.06f);
    sprite_colors_reset();
  }
  else if (level == 30 || level == 31) {
    if (side == 0) sprite_put_xyz(0xa3a,80.0f,56.0f,0.06f);
    else sprite_put_flags_xyz(0xa3a,0x10,476.0f,56.0f,0.06f);
  }
  else if (level == 29) {
    if (side == 0) sprite_put_xyz(0xa3b,80.0f,56.0f,0.06f);
    else sprite_put_flags_xyz(0xa3b,0x10,476.0f,56.0f,0.06f);
  }
  else {
    switch (level) {
    case 28: color = 0; break;
    case 27: color = 0x505050; break;
    case 26: color = 0x808080; break;
    case 25: color = 0xaa7722; break;
    case 24: color = 0xcc8844; break;
    case 23: color = 0xffaa66; break;
    case 22: color = 0xcccc33; break;
    case 21: color = 0xccff44; break;
    case 20: color = 0x99ff55; break;
    default:
      if (side == 0) color = 0xff0000;
      else color = 0xff;
    }
    sprite_colors_set(color,0);
    if (side == 0) sprite_put_xyz(0xaeb,80.0f,60.0f,0.06f);
    else sprite_put_flags_xyz(0xaeb,0x10,476.0f,60.0f,0.06f);
    sprite_colors_reset();
  }
}

void mark_1to3_draw(uint side, uint count_arg)
{
  int marks[4];
  uint count;
  char chara;
  float dy;
  float x;
  float y;
  
  count = count_arg & 0xff;
  marks[0] = -1;
  marks[1] = 0xaed;
  marks[2] = 0xaef;
  marks[3] = 0xaee;
  chara = (rob_get(side & 0xff))->chara;
  if (count == 0) return;
  if (count > 3) count = 3;
  if (count == 0) dy = 0.0; else dy = -8.0;
  if (side == 0) x = 48.0; else x = 552.0;
  if (card_body_name_empty(&card_image_get(side)->body) == 0) {
    if ((chara == 3) | (chara == 13)) y = 133.0; else y = 115.0;
  }
  else {
    if ((chara == 3) | (chara == 13)) y = 107.0; else y = 89.0;
  }
  if (count == 1) y = y + 8.0;
  sprite_put_xyz(marks[count],x,y + dy,0.03);
}

void emblem_draw(int side_arg)
{
  char side;
  char chara;
  card_image_rec *img;
  float x;
  float y;

  side = (char)side_arg;
  chara = (rob_get((uchar)side))->chara;
  img = card_image_get(side);
  if (side == 0) x = 16.0; else x = 600.0;
  if (card_body_name_empty(&img->body) == 0) {
    if ((chara == 3) | (chara == 13)) y = 130.0; else y = 112.0;
  }
  else {
    if ((chara == 3) | (chara == 13)) y = 104.0; else y = 86.0;
  }
  if (card_emblem_rank_is_set(side) != 0) {
    uchar rank = card_body_emblem_dan_get(&img->body) - 1;
    emblem_rank_gauge_draw(side, rank);
  }
  else {
    int *emblem = &emblem_sprites[side];
    if (0 < *emblem && card_body_emblem_get(&img->body) != 0) {
      sprite_put_xyz(*emblem,x,y,0.02);
    }
  }
}

void shun_drink_count_draw(char side)
{
  rob_rec *rob;
  card_image_rec *img;
  float x;
  float y;
  char buf[8];
  
  rob = rob_get((uchar)side);
  if (rob->chara != 3 && rob->chara != 13) return;
  img = card_image_get(side);
  if (side == 0) x = 16.0; else x = 584.0;
  if (!card_body_name_empty(&img->body)) y = 110.0; else y = 84.0;
  sprite_put_xyz(0x991,x,y,0.04);
  sprintf(buf,drink_count_format,(int)rob->attack_rate);
  sprite_text_put(&euc_text_font,x + 16.0,y,0.04,0,(byte *)buf);
  sprite_colors_set(0,0);
  sprite_text_put(&euc_text_font,x + 18.0,y + 2.0,0.05,0,(byte *)buf);
  sprite_colors_reset();
}

void emblem_debug_pick(int side_arg)
{
  char chara;
  int side;
  float x;
  float y;
  byte buf[100];
  
  side = (char)side_arg;
  if (emblem_browse_side >= 0) {
    if ((player_input_get(side)->pressed & 1) != 0) {
      if (++emblem_sprites[side] > 0x7dae) emblem_sprites[side] = 32000;
    }
    chara = (rob_get((byte)side))->chara;
    if (side == 0) x = 16.0; else x = 576.0;
    if (card_body_name_empty(&card_image_get(side)->body) == 0) {
      if ((chara == 3) | (chara == 13)) y = 130.0; else y = 112.0;
    }
    else {
      if ((chara == 3) | (chara == 13)) y = 104.0; else y = 86.0;
    }
    sprintf(buf,format_03d,emblem_sprites[side] - 0x7cff);
    sprite_text_put(&euc_text_font,x,y + 40.0,0.006,0x20,buf);
  }
}

void emblem_browse_side_set(char side)
{
  int *sprite;

  if ((uint)side < 2) {
    emblem_browse_side = side;
  }
  sprite = &emblem_sprites[side];
  if (*sprite < 32000) {
    *sprite = 32000;
  }
  return;
}

void emblem_debug_store(int side_arg)
{
  card_image_rec *img;
  
  img = card_image_get((int)(char)side_arg);
  emblem_browse_side = 0xff;
  card_body_emblem_set(&img->body,(int)*(char *)&emblem_sprites[(char)side_arg] + 1U & 0xff);
  return;
}

void emblem_debug_clear(int side_arg)
{
  card_image_rec *img;
  
  img = card_image_get((int)(char)side_arg);
  emblem_browse_side = 0xff;
  card_body_emblem_set(&img->body,0);
  return;
}

void emblem_rank_gauge_draw(uint side, uint rank_arg)
{
  int tier_sprites[4];
  int pip_sprites[4];
  int cap_sprites[3];
  float pos[7][2];
  uchar pip;
  uchar rank;
  char chara;
  char i;
  float y;
  
  rank = rank_arg;
  tier_sprites[0] = 0xa3d;
  tier_sprites[1] = 0xa3e;
  tier_sprites[2] = 0xa3f;
  tier_sprites[3] = 0xa40;
  pip_sprites[0] = 0xa41;
  pip_sprites[1] = 0xa42;
  pip_sprites[2] = 0xa43;
  pip_sprites[3] = 0xa44;
  cap_sprites[0] = 0xa45;
  cap_sprites[1] = 0xa46;
  cap_sprites[2] = 0xa47;
  pos[0][0] = 5.0;
  pos[0][1] = 112.0;
  pos[1][0] = 21.0;
  pos[1][1] = 112.0;
  pos[2][0] = 26.0;
  pos[2][1] = 135.0;
  pos[3][0] = 16.0;
  pos[3][1] = 132.0;
  pos[4][0] = 10.0;
  pos[4][1] = 123.0;
  pos[5][0] = 11.0;
  pos[5][1] = 113.0;
  pos[6][0] = 21.0;
  pos[6][1] = 112.0;
  chara = (rob_get((uchar)side))->chara;
  if (rank < 15) {
    y = 0.0;
    if (card_body_name_empty(&card_image_get(side)->body) != 0) {
      y += -26.0;
    }
    if ((chara == 3) | (chara == 13)) {
      y += 18.0;
    }
    if (side == 0) {
      sprite_put_xyz_scaled(0xa3c,pos[0][0],y + pos[0][1],0.07,1.0,1.0);
      sprite_put_xyz_scaled(tier_sprites[(uchar)(rank / 5) + 1],pos[1][0],y + pos[1][1],0.06,1.0,1.0);
      for (i = 2; i < 6; i++) {
        if (i - 2 < (uchar)(rank % 5)) pip = rank / 5 + 1;
        else pip = 0;
        sprite_put_xyz_scaled(pip_sprites[pip],pos[i][0],y + pos[i][1],0.06,1.0,1.0);
      }
      sprite_put_xyz_scaled(cap_sprites[(uchar)(rank / 5)],pos[6][0],y + pos[6][1],0.05,1.0,1.0);
    }
    else {
      sprite_put_xyz_scaled(0xa3c,640.0 - pos[0][0],y + pos[0][1],0.07,-1.0,1.0);
      sprite_put_xyz_scaled(tier_sprites[(uchar)(rank / 5) + 1],640.0 - pos[1][0],y + pos[1][1],0.06,-1.0,1.0);
      for (i = 2; i < 6; i++) {
        if (i - 2 < (uchar)(rank % 5)) pip = rank / 5 + 1;
        else pip = 0;
        sprite_put_xyz_scaled(pip_sprites[pip],640.0 - pos[i][0],y + pos[i][1],0.06,-1.0,1.0);
      }
      sprite_put_xyz_scaled(cap_sprites[(uchar)(rank / 5)],618.0 - pos[6][0],y + pos[6][1],0.05,1.0,1.0);
    }
  }
}

void player_plate_draw(uint side)
{
  card_image_rec *img;
  int side_no;
  card_record_rec *card;
  
  card = card_record_get(side);
  img = card_image_get(side);
  if ((card->in_use == 1) && (img->body.play_id != 0)) {
    if (mode_get_sub() == 9 || mode_get_sub() == 0xd || mode_get_sub() == 10) {
      side_no = (int)(char)side;
      level_badge_draw(side_no);
      name_plate_draw(side_no);
      emblem_draw(side_no);
      if ((net_card_place_matches(side & 0xff) != 0) && ((img->body.misc.w & 0x300) != 0)) {
        mark_1to3_draw(side,4 - img->body.misc.f.tenpo_value2 & 0xff);
      }
      if ((msnm_active() == 0) && (mode_get_sub() == 9)) {
        if (pause_frame_held() == 0) {
          card_panel_timer[side] = card_panel_timer[side] + 0.05;
        }
        card_panel_draw((int)(char)side,card_panel_timer[side]);
      }
      player_plate_item_icons_draw(side);
    }
    else {
      card_panel_timer[side] = 0.0;
    }
  }
  return;
}

void banner_shine_draw(int frame_arg, float x, float y, uint style_arg)
{
  typedef struct { shine_style e[4]; } blk;
  short frame;
  uint style;
  float scale;
  float dx;
  blk styles;

  frame = frame_arg;
  style = style_arg & 0xff;
  styles = *(blk *)shine_styles;
  if ((ushort)frame > 59) return;
  if ((ushort)frame < 14) {
    scale = (float)frame;
    scale = scale / 14.0;
  }
  else if (frame > 45) {
    scale = 60.0 - (float)frame;
    scale = scale / 14.0;
  }
  else {
    scale = 1.0;
  }
  dx = ((float)frame * 320.0) / 60.0;
  sprite_next_blend_set(0);
  sprite_put_xyz_scaled_alpha_angle_flags(styles.e[style].sprite,dx + x,y,0.01,scale,scale,0,0.9,styles.e[style].flags);
  sprite_put_xyz_scaled_alpha_angle_flags(styles.e[style].sprite,dx + x,y,0.01,scale,scale,styles.e[style].rot,0.9,styles.e[style].flags2);
  sprite_next_blend_normal();
}

void prize_trail_sparks_draw(int count)
{
  float seed = 0.123f;
  ushort i;
  float x, y, dist, scale, alpha, px;

  if (item_set_redeem_active()) return;
  for (i = 0; i < count; i++) {
    seed = seed * 3.66f * (1.0f - seed);
    x = (pow_float((float)i, 3.0f) / 20000.0f + pow_float((float)i, 2.0f) / 30000.0f * (dist = (float)(count - i))) * 230.0f;
    y = (float)((int)(seed * 1000.0f) % 100 - 50) / 25.0f * dist;
    scale = (float)((int)(seed * 100.0f) % 100) / 100.0f * 0.7f - dist * 0.02f + 0.3f;
    if (scale < 0.0f) scale = 0.0f;
    alpha = scale;
    if (scale > 1.0f) alpha = 1.0f;
    sprite_next_blend_set(0);
    if ((uchar)champion_side == 0) px = 337.0f - x;
    else px = x + 337.0f;
    sprite_put_xyz_scaled_alpha_flags(0x3076, px, y + 422.0f, 0.03f, scale, scale, alpha, 0x100a);
    sprite_next_blend_normal();
  }
}

void prize_burst_draw(uint frame_arg)
{
  int frame;
  ushort rot;
  float x;
  float scale;
  float half;
  float alpha;
  
  frame = frame_arg;
  if (item_set_redeem_active() != 0) {
    return;
  }
  if ((uchar)champion_side == 0) {
    x = 107.0;
  }
  else {
    x = 567.0;
  }
  if (frame < 100) {
    alpha = (float)frame / 100.0;
  }
  else if (frame < 200) {
    alpha = 1.0 - (float)frame / 100.0;
  }
  else {
    alpha = 0.0;
  }
  if (frame < 30) {
    scale = ((float)frame / 30.0) * 1.5 + 0.5;
  }
  else if (frame < 60) {
    scale = 2.0 - (float)(frame - 30) / 30.0;
  }
  else if (frame < 200) {
    scale = ((float)(frame - 60) / 140.0) * 3.0 + 1.0;
  }
  else {
    scale = 0.0;
  }
  if (frame % 3 == 2) {
    scale = scale / 3.0;
  }
  rot = (frame * frame * 0xffff) / 0x1324;
  sprite_next_blend_set(0);
  sprite_put_xyz_scaled_alpha_angle_flags(0x3077,x,422.0,0.06,scale,scale,(short)rot,alpha,0xb);
  half = scale * 0.5;
  sprite_put_xyz_scaled_alpha_angle_flags(0x3077,x,422.0,0.06,half,half,(short)-rot,alpha,0xb);
  sprite_put_xyz_scaled_alpha_angle_flags(0x3077,x,422.0,0.06,scale,scale,(short)(rot + -0x8000),alpha,0xb);
  sprite_put_xyz_scaled_alpha_angle_flags(0x3077,x,422.0,0.06,half,half,(short)(-0x8000 - rot),alpha,0xb);
  sprite_next_blend_normal();
  return;
}

int prize_fly_state_get(void)
{
  return (int)((char)prize_fly_state);
}

void prize_fly_draw(void)
{
/* ORIGINAL SHAPE (corroborated 2026-11-10: move_tcmd_33_slot_effect in libaction/rob_ctrl.c needs its hit-light block repeated per case by the same mechanism, copies merged by flow2's cross-jumping; rob_ctrl.c's SET and HIT_LIGHT are in-function macros like this one): the clamp and the side step are repeated at the end of every break path, and case 6 falls into default; GCC cross-jumps the copies into the game's one. Macro or pasted copies is not in the bytes. The global order of x, h and y is an exact priority tie: rerun the match after any edit here */
#define PRIZE_FLY_SIDE() \
if (y < 0.0) { \
  y = 0.0; \
} \
if ((uchar)champion_side == 0) { \
  x -= y; \
  card_icon_pos_set(z + 36.0,446.0,601.0,446.0); \
} \
else { \
  x += y; \
  card_icon_pos_set(36.0,446.0,601.0 - z,446.0); \
}
  int id[4];
  int trail;
  char k;
  int d;
  float x;
  float h;
  float y;
  float z;
  float lim;
  float s;
  float w;
  float u;

  id[0] = 0x3070;
  id[1] = 0x3071;
  id[2] = 0x3074;
  id[3] = 0x3072;
  trail = 0;
  y = 0.0;
  z = y;
  h = y;
  x = 337.0;
  switch (prize_fly_state) {
  case 1:
    prize_fly_frame = 0;
    prize_fly_state++;
    for (k = 0; k < 9; k++) {
      prize_trail_x[k] = 0.0;
    }
  case 2:
    h = (float)prize_fly_frame;
    if (h > 30.0) {
      h = 30.0;
      prize_fly_state++;
      prize_fly_launch_frame = prize_fly_frame;
      PRIZE_FLY_SIDE();
      break;
    }
    PRIZE_FLY_SIDE();
    break;
  case 3:
    trail = 1;
    h = 30.0;
    y = pow_float((float)(prize_fly_frame - prize_fly_launch_frame),3.0);
    y = y / 20000.0 * 230.0;
    prize_trail_sparks_draw(prize_fly_frame - prize_fly_launch_frame);
    if (y > 230.0) {
      prize_fly_state++;
      prize_fly_land_frame = prize_fly_frame;
      PRIZE_FLY_SIDE();
      break;
    }
    PRIZE_FLY_SIDE();
    break;
  case 4:
    z = sin_radians((float)(prize_fly_frame - prize_fly_land_frame) / 13.0); /* ORIGINAL SHAPE (assumed): the game calls sin_radians here and drops the result; read as z once animated, then switched off by the z = 0.0 below */
    d = prize_fly_frame - prize_fly_land_frame;
    h = 30.0;
    z = 0.0;
    lim = 6.0; /* ORIGINAL SHAPE (assumed): two assignments that leave no code sit between the sin calls (they set case 4's register order); z = 0.0 is one, and the limit held in a local is the form found for the other; the name is ours */
    y = sin_radians((float)d / 13.0) * h * 10.0;
    d = prize_fly_frame - prize_fly_land_frame;
    y = y / (float)(d + 10) + 230.0;
    s = pow_float(1.1,(float)d);
    w = 2.0 / (float)(prize_fly_frame - prize_fly_land_frame + 3);
    sprite_put_xyz_scaled_alpha_flags(id[prize_kind],(uchar)champion_side == 0 ? x - y : x + y,452.0 - h,0.03,s,s,w,0x100a);
    if (s > lim) {
      prize_fly_state++;
      prize_trail_sparks_draw(prize_fly_frame - prize_fly_launch_frame);
      prize_burst_draw(prize_fly_frame - prize_fly_land_frame);
      PRIZE_FLY_SIDE();
      break;
    }
    prize_trail_sparks_draw(prize_fly_frame - prize_fly_launch_frame);
    prize_burst_draw(prize_fly_frame - prize_fly_land_frame);
    PRIZE_FLY_SIDE();
    break;
  case 5:
    y = sin_radians((float)(prize_fly_frame - prize_fly_land_frame) / 13.0) * 30.0 * 10.0 / (float)(prize_fly_frame - prize_fly_land_frame + 10) + 230.0;
    h = 30.0;
    prize_trail_sparks_draw(prize_fly_frame - prize_fly_launch_frame);
    prize_burst_draw(prize_fly_frame - prize_fly_land_frame);
    if (prize_fly_frame - prize_fly_land_frame > 200) {
      prize_fly_state++;
      PRIZE_FLY_SIDE();
      break;
    }
    PRIZE_FLY_SIDE();
    break;
  case 6:
    y = sin_radians((float)(prize_fly_frame - prize_fly_land_frame) / 13.0) * 30.0 * 10.0 / (float)(prize_fly_frame - prize_fly_land_frame + 10) + 230.0;
    h = 30.0;
  default:
    PRIZE_FLY_SIDE();
    break;
  }
  for (k = 8; k > 0; k--) {
    prize_trail_x[k] = prize_trail_x[k - 1];
  }
  prize_trail_x[0] = x;
  sprite_put_flags_xyz(id[prize_kind],x,452.0 - h,0.03,0x100a);
  if (trail) {
    for (k = 0; k < 9; k++) {
      u = 1.0 - (float)k * 0.1;
      sprite_put_xyz_alpha_flags(id[prize_kind],prize_trail_x[k],452.0 - h,0.03,u,0x100a);
    }
  }
  if (pause_frame_held() == 0) {
    prize_fly_frame++;
  }
}

void card_icon_pos_set(float p1_x, float p1_y, float p2_x, float p2_y)
{
  card_icon_pos.p1_x = p1_x;
  card_icon_pos.p1_y = p1_y;
  card_icon_pos.p2_x = p2_x;
  card_icon_pos.p2_y = p2_y;
  return;
}

void level_notice_tex_load(void)
{
  uchar flags[2];
  
  flags[0] = level_change_flags_get(0);
  flags[1] = level_change_flags_get(1);
  if ((flags[0] != '\0') || (flags[1] != 0)) {
    level_banner_frame = 0;
    game_mes_load(0x26);
  }
  return;
}

void level_notice_tex_free(void)
{
  char flags[2];
  int flag1;
  
  flags[0] = ((int (*)(int))level_change_flags_get)(0);
  flag1 = ((int (*)(int))level_change_flags_get)(1);
  flags[1] = flag1;
  if (flags[0] != 0) {
    game_mes_free(0x26);
  }
  else if (flag1 != 0) {
    game_mes_free(0x26);
  }
}

void level_notice_draw(void)
{
  char i;
  uchar flags[2];
  card_record_rec *cards[2];
  uint card_flags[2];

  flags[0] = level_change_flags_get(0);
  flags[1] = level_change_flags_get(1);
  cards[0] = card_record_get(0);
  cards[1] = card_record_get(1);
  card_flags[0] = card_flags_get(0);
  card_flags[1] = card_flags_get(1);
  if (mode_get_main() == MODE_MAIN_GAME) {
    for (i = 0; i < 2; i++) {
      if (flags[i] != 0 && cards[i]->in_use != 0 && card_message_active(i) == 0 && cards[i]->in_use == 1 && (card_flags[i] & 1) != 0 && (card_flags[i] & 0x80) != 0) {
        if (flags[i] & 2) {
          card_message_post(i,0x1b,0xffffffff,0);
        }
        else if ((flags[i] & 1) && (flags[i] & 4)) {
          card_message_post(i,0x1d,0xffffffff,0);
        }
        else if (flags[i] & 1) {
          card_message_post(i,0x1a,0xffffffff,0);
        }
        else if (flags[i] & 4) {
          card_message_post(i,0x1c,0xffffffff,0);
        }
      }
    }
  }
  if (mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 5 || mode_get_sub() == 7) && ((flags[0] != 0 && cards[0]->in_use != 0) || (flags[1] != 0 && cards[1]->in_use != 0))) {
    sprite_list_put(level_notice_parts,0.03f);
    sprite_put_xy_scaled(0xb08,136.0f,188.0f,1.0f,12.0f);
    sprite_put_xy_scaled_flags(0xb08,440.0f,188.0f,1.0f,12.0f,0x10);
    sprite_put_xy_scaled(0xb03,200.0f,188.0f,30.0f,12.0f);
    sprite_put_xy_scaled_flags(0xb09,320.0f,180.0f,2.75f,1.0f,7);
    sprite_put_xy_scaled_flags(0xb09,320.0f,180.0f,2.75f,1.0f,0x10);
    sprite_put_xy_scaled_flags(0xb09,320.0f,278.0f,2.75f,1.0f,7);
    sprite_put_xy_scaled_flags(0xb09,320.0f,278.0f,2.75f,1.0f,0x10);
    sprite_put_flags_xyz(0xb04,448.0f,248.0f,0.02f,0x10);
    sprite_put_flags_xyz(0xb05,384.0f,256.0f,0.02f,0x10);
    sprite_put_flags_xyz(0xb06,360.0f,256.0f,0.02f,0x10);
    sprite_put_flags_xyz(0xb07,360.0f,272.0f,0.02f,0x10);
    banner_shine_draw((int)level_banner_frame,160.0f,240.0f,0);
    level_banner_frame = level_banner_frame + 1;
  }
}

void level_up_tex_load(void)
{
  uint flags[2];
  flags[0] = level_change_flags_get(0);
  flags[1] = level_change_flags_get(1);
  if (((flags[0] | flags[1]) & 8) != 0)
  {
    game_mes_load(0x27);
  }
  level_banner_frame = 0;
  return;
}

void level_up_tex_free(void)
{
  uint flags[2];
  flags[0] = level_change_flags_get(0);
  flags[1] = level_change_flags_get(1);
  if ((flags[0] | flags[1]) & 8) {
    game_mes_free(0x27);
    streak_end_flag_set(0);
  }
}

void level_up_banner_draw(void)
{
  int due;
  uint level;
  level_entry *entry;
  
  due = level_up_banner_due();
  if (due != 0) {
    sprite_list_put(level_up_parts,0.03);
    sprite_put_xy_scaled(0xb08,136.0,188.0,1.0,12.0);
    sprite_put_xy_scaled_flags(0xb08,440.0,188.0,1.0,12.0,0x10);
    sprite_put_xy_scaled(0xb03,200.0,188.0,30.0,12.0);
    sprite_put_xy_scaled_flags(0xb09,320.0,180.0,2.75,1.0,7);
    sprite_put_xy_scaled_flags(0xb09,320.0,180.0,2.75,1.0,0x10);
    sprite_put_xy_scaled_flags(0xb09,320.0,278.0,2.75,1.0,7);
    sprite_put_xy_scaled_flags(0xb09,320.0,278.0,2.75,1.0,0x10);
    sprite_put_flags_xyz(0xb04,448.0,248.0,0.02,0x10);
    sprite_put_flags_xyz(0xb05,384.0,256.0,0.02,0x10);
    sprite_put_flags_xyz(0xb06,360.0,256.0,0.02,0x10);
    sprite_put_flags_xyz(0xb07,360.0,272.0,0.02,0x10);
    level = card_image_get(champion_side)->header.level;
    if (0xed < level) {
      level = 0xed;
    }
    entry = &level_table[level];
    sprite_put_xyz(entry->sprites[0],196.0,192.0,0.03);
    sprite_put_xyz(entry->sprites[1],252.0,192.0,0.03);
    sprite_put_xyz(0xb0e,312.0,192.0,0.03);
    banner_shine_draw(level_banner_frame,160.0,240.0,0);
    level_banner_frame = level_banner_frame + 1;
  }
  return;
}

int level_up_banner_due(void)
{
  uint flags;
  card_record_rec *card;
  int has_card;
  int due;
  flags = level_change_flags_get(champion_side);
  card = card_record_get(champion_side);
  if (streak_limit_hit != 0) {
    int has = 0;
    if ((card->in_use != 0) || (streak_end_flag_get() != 0)) {
      has = 1;
    }
    has_card = has;
  }
  else {
    has_card = card->in_use != 0;
  }
  due = 0;
  if ((((flags >> 3) & 1) & (has_card != 0)) && (mode_get_main() == MODE_MAIN_GAME) && (mode_get_sub() == 0xf)) {
    due = 1;
  }
  return due;
}

void console_frame_draw(void)
{
  int ok[2];
  int rec0;
  int rec1;
  int net_sprite;
  uint flags0;
  uint flags1;
  net_info_t *net_info;
  card_service_rec *service;
  int icon;
  int i;
  float x;
  float y;

  ok[0] = 1;
  ok[1] = 1;
  rec0 = (int)card_record_get(0);
  rec1 = (int)card_record_get(1);
  flags0 = card_flags_get(0);
  flags1 = card_flags_get(1);
  net_info = net_info_get();
  service = (card_service_rec *)card_service_state_get();
  if (net_is_on() != 0) {
    if (net_ready_and_idle() == 0) goto none;
    {
      switch (net_info->error_level) {
      case 0:
        net_sprite = 0x1d;
        break;
      case 1:
        net_sprite = 0x1e;
        break;
      case 2:
        net_sprite = 0x1f;
        break;
      default:
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/console.c",0x1c85,"assertion `%s\' failed.\n","0");
      case 3:
      none:
        net_sprite = 0x20;
        break;
      }
    }
    x = 6.0;
    y = 445.0;
    sprite_put_xyz(net_sprite,x,y,0.008);
  }
  if (card_service_mode_get() == 3) {
    for (i = 0; i < 2; i++) {
      int *dst = &ok[i];
      int v = 0;
      char *b = &side_entry_flags[0];
      char c = b[i];
      if ((c >> 2) & 1) {
        if (c & 1) {
          v = 1;
        }
      }
      *dst = v;
    }
  }
  x = card_icon_pos.p1_x;
  y = card_icon_pos.p1_y;
  if (user_back_data.card_use[0] == OFF_ON_ON) {
    if (card_unit_ok(0) == 0 || service->mode == -1 || ok[0] == 0) {
      icon = 0xf;
      sprite_put_xyz(10,x - 7.0,y - 2.0,0.007);
    }
    else if (((mode_get_main() == MODE_MAIN_SELECTOR && (side_entry_flags[0] & 4) != 0) ||
              ((card_record_rec *)rec0)->in_use == 1) && (flags0 & 0x81) == 0x81) {
      icon = 8;
      if (((card_record_rec *)rec0)->in_use == 1) {
        sprite_next_blend_set(0);
        if (item_lottery_pending(0) != 0) {
          sprite_put_xy_colors_z_alpha(0x18,32.0,442.0,0.007,console_glow,0xffff00,0xff000000);
        }
        else {
          sprite_put_xyz_alpha(0x18,32.0,442.0,0.007,console_glow);
        }
        sprite_next_blend_normal();
      }
    }
    else {
      icon = 0xf;
    }
    sprite_put_xyz(icon,x,y,0.008);
    sprite_put_xyz(0x11,35.0,445.0,0.009);
    if (card_unit_ok(0) != 0 && (rec0 = mode_get_main()) == MODE_MAIN_SELECTOR && card_service_mode_get() != -1 &&
        (side_entry_flags[0] & 2) != 0 && (flags0 & 1) == 0 && *(int *)(card_service_state_get() + 4) != -1) {
      sprite_put_xyz(0xb,71.0,447.0,0.008);
      sprite_put_xyz_scaled(0x12,43.0,445.0,0.009,5.75,1.0);
      sprite_colors_set(0xff0000,0);
      sprite_put_xyz(0x13,89.0,445.0,0.008);
      sprite_colors_reset();
    }
    else {
      sprite_put_xyz_scaled(0x12,43.0,445.0,0.009,2.125,1.0);
      sprite_put_flags_xyz(0x11,60.0,445.0,0.009,0x10);
    }
  }
  if ((rec0 = user_back_data.card_use[1]) == OFF_ON_ON) {
    x = card_icon_pos.p2_x;
    y = card_icon_pos.p2_y;
    if ((flags0 = card_unit_ok(1)) == 0 || service->mode == -1 || ok[1] == 0) {
      icon = 0x10;
      sprite_put_xyz(10,x - 7.0,y - 2.0,0.007);
    }
    else if (((mode_get_main() == MODE_MAIN_SELECTOR && (side_entry_flags[1] & 4) != 0) ||
              ((card_record_rec *)rec1)->in_use == 1) && (flags1 & 0x81) == 0x81) {
      icon = 9;
      if (((card_record_rec *)rec1)->in_use == 1) {
        sprite_next_blend_set(0);
        if (item_lottery_pending(1) != 0) {
          sprite_put_xy_colors_z_alpha(0x18,597.0,442.0,0.007,console_glow,0xffff00,0xff000000);
        }
        else {
          sprite_put_xyz_alpha(0x18,597.0,442.0,0.007,console_glow);
        }
        sprite_next_blend_normal();
      }
    }
    else {
      icon = 0x10;
    }
    sprite_put_xyz(icon,x,y,0.008);
    sprite_put_flags_xyz(0x11,625.0,445.0,0.009,0x10);
    if (card_unit_ok(1) != 0 && card_service_mode_get() != -1 && mode_get_main() == MODE_MAIN_SELECTOR &&
        (side_entry_flags[1] & 2) != 0 && (flags1 & 1) == 0 && *(int *)(((uchar *)(flags1 = (int)card_service_state_get())) + 4) != -1) {
      sprite_put_xyz(0xc,582.0,447.0,0.008);
      sprite_put_xyz_scaled(0x12,576.0,445.0,0.009,6.125,1.0);
      sprite_colors_set(0xff,0);
      sprite_put_flags_xyz(0x13,568.0,445.0,0.008,0x10);
      sprite_colors_reset();
    }
    else {
      sprite_put_xyz(0x11,600.0,445.0,0.009);
      sprite_put_xyz_scaled(0x12,608.0,445.0,0.009,2.125,1.0);
    }
  }
  if ((flags1 = console_glow_down) == 0) {
    console_glow = console_glow + 0.02;
    if (0.7 <= console_glow) {
      console_glow_down = 1;
      console_glow = 0.7;
    }
  }
  else {
    console_glow = console_glow - 0.02;
    if (console_glow <= 0.0) {
      console_glow_down = 0;
      console_glow = 0.0;
    }
  }
  if ((rec1 = mode_get_main()) == MODE_MAIN_ADVERTISE) {
    version_label_draw();
  }
}

void net_reply_timer_set(int frames)
{
  net_reply_timer = frames;
  return;
}

void net_status_clear(void)
{
  net_status_rec *p;
  int i;
  
  p = net_status;
  i = 0;
  do {
    p->id[0] = 0;
    p->command[0] = 0;
    p->status = 0;
    p->active = 0;
    p->timer = 0;
    i = i + 1;
    p = p + 1;
  } while (i < 2);
  return;
}

void net_status_request_note(int side, char *id, char *command)
{
  strncpy(net_status[side].id,id,0x11);
  strncpy(net_status[side].command,command,6);
  return;
}

void net_status_reply_note(int side, uint timer, uint status, char *id)
{
  int diff;
  
  diff = strcmp(net_status[side].id,id);
  if ((diff == 0) && (net_status[side].active == 0)) {
    net_status[side].active = 1;
    net_status[side].timer = timer;
    net_status[side].status = (uchar)status;
  }
  return;
}

void net_status_tick(void)
{
  int ok;
  net_status_rec *p;
  int i;

  p = net_status;
  ok = 1;
  for (i = 0; i < 2; i++, p++) {
    if (p->active != 0) {
      if (pause_frame_held() == 0) {
        p->timer = p->timer - 1;
      }
      switch (mode_get_main()) {
      case MODE_MAIN_GAME:
        switch (mode_get_sub()) {
        case 1:
        case 2:
        case 4:
        case 10:
          ok = 0;
          break;
        }
        break;
      case MODE_MAIN_OVER:
      case MODE_MAIN_INTRUDE:
        break;
      default:
        ok = 0;
        break;
      }
      if ((int)p->timer < 0) ok = 0;
      if (!ok) {
        p->timer = 0;
        p->active = 0;
      }
    }
  }
}

void net_status_icons_draw(void)
{
  int i;
  net_status_rec *p;
  
  p = net_status;
  for (i = 0; i < 2; i++, p++) {
    if (p->active != 0) {
      net_status_icon_draw((int)(char)i,p->timer,(float)(i * 0x228),421.0,0.009,
                   (uint)(byte)p->status);
    }
  }
}

void net_messages_flag_set(void)
{
  net_messages_flag = 1;
  return;
}

void net_messages_draw(void)
{
  byte *pr_text0;
  byte *pr_text1;
  byte *hours_text;
  byte *message;
  int value;
  uint paused;
  e_mode_main mode;
  
  pr_text0 = (byte *)net_pr_text_get(0);
  pr_text1 = (byte *)net_pr_text_get(1);
  hours_text = net_hours_message_get();
  message = net_message_text_get();
  value = net_is_on();
  if (value == 0) {
    return;
  }
  if (hours_text != (byte *)0x0) {
    message = hours_text;
  }
  if (((0 < net_reply_timer) && (paused = pause_frame_held(), paused == 0)) &&
     (net_reply_timer = net_reply_timer + -1, net_reply_timer < 1)) {
    net_reply_timer = 0;
  }
  net_status_tick();
  net_status_icons_draw();
  mode = mode_get_main();
  if (((mode == MODE_MAIN_GAME) && (value = mode_get_sub(), value == 5)) ||
     ((mode = mode_get_main(), mode == MODE_MAIN_GAME && (value = mode_get_sub(), value == 7)))) {
    if (play_style == '\x02') {
      if ((message != (byte *)0x0) && (*message != 0)) {
        sprite_text_put(&euc_text_font,74.0,24.0,1e-05,0,message);
      }
    }
    else if ((message != (byte *)0x0) && (*message != 0)) {
      sprite_text_put(&euc_text_font,76.0,424.0,1e-05,0,message);
      goto done;
    }
    sprite_text_put(&euc_text_font,74.0,424.0,1e-05,0,pr_text0);
    sprite_text_put(&euc_text_font,320.0,424.0,1e-05,0,pr_text1);
  }
done:
  mode = mode_get_main();
  if ((((mode == MODE_MAIN_ADVERTISE) && (value = mode_get_sub(), value != 6)) &&
      (message != (byte *)0x0)) && (*message != 0)) {
    sprite_put_xyz_scaled(0,0.0,420.0,0.009,80.0,7.5);
    sprite_text_put(&euc_text_font,76.0,440.0,1e-05,0,message);
  }
  net_messages_flag = 0;
  return;
}

void net_vs_parts_draw(void)
{
  int value0;
  int value1;
  int value2;
  value0 = net_is_on();
  if (((value0 != 0) && (play_style != '\x01')) && ((value0 = card_user_count_get(), 1 < value0)))
  {
    value0 = (int)card_record_get(0);
    value1 = (int)card_record_get(1);
    net_info_get();
    if ((((game_flags & 0x400) != 0) && ((value2 = net_ready_and_idle(), value2 != 0))) && (((value2 = card_unit_used_get(0), (value2 != 0) && (((card_record_rec *)value0)->in_use == 1))) || ((value0 = card_unit_used_get(1), (value0 != 0) && (((card_record_rec *)value1)->in_use == 1)))))
    {
      if (value2)
      {
        sprite_list_put(net_vs_parts, 0.008);
      }
      else
      {
        sprite_list_put(net_vs_parts, 0.008);
      }
    }
  }
  return;
}

int euc_text_cells(char *text)
{
  char cells;
  char i;
  
  cells = strlen(text);
  for (i = 0; i < 0x15; i++) {
    if (((unsigned char *)text)[i] == 0x8e) {
      cells--;
    }
    else if (text[i] == '\0') {
      break;
    }
  }
  return cells;
}

void prize_plate_1p_draw(int visible)
{
  if (!(visible == 0)) {
    prize_plate_draw(0,10.0,332.0,0.008);
  }
  else {
    prize_plate_hide(0);
  }
  return;
}

void prize_plate_2p_draw(int visible)
{
  if (!(visible == 0)) {
    prize_plate_draw(1,334.0,332.0,0.008);
  }
  else {
    prize_plate_hide(1);
  }
  return;
}

void prize_bar_1p_draw(void)
{
  prize_bar_draw(0,256.0,422.0,0.01);
  return;
}

void prize_bar_2p_draw(void)
{
  prize_bar_draw(1,256.0,422.0,0.01);
  return;
}

void prize_numbers_1p_stub(int upper_value, int lower_value)
{
  prize_number_stub(upper_value,56.0,406.0,0.007);
  prize_number_stub(lower_value,26.0,418.0,0.007);
  return;
}

void prize_numbers_2p_stub(int upper_value, int lower_value)
{
  prize_number_stub(upper_value,563.0,406.0,0.007);
  prize_number_stub(lower_value,593.0,418.0,0.007);
  return;
}

void prize_number_stub(int value, float x, float y, float z)
{
  volatile int unused;
}

void prize_bar_draw(int side, float x, float y, float z)
{
  uchar i;
  
  switch (side) {
  case 0:
    for (i = 0; i < 4; i++) {
      sprite_put_xyz_scaled(12000,(x + (float)(i << 6)) - 256.0,y,z,1.0,1.0);
    }
    sprite_put_xyz_scaled(0x2ee1,x,y,z,1.0,1.0);
    break;
  case 1:
    for (i = 0; i < 4; i++) {
      sprite_put_xyz_scaled(12000,((640.0 - x) - (float)(i << 6)) + 256.0,y,z,-1.0,1.0);
    }
    sprite_put_xyz_scaled(0x2ee1,640.0 - x,y,z,-1.0,1.0);
    break;
  }
}

void prize_plate_draw(int side, float x, float y, float z)
{
  uchar i;
  int sprites[8];
  sprites[0] = 0x2ee2;
  sprites[1] = 0x2ee3;
  sprites[2] = 0x2ee4;
  sprites[3] = 0x2ee5;
  sprites[4] = 0x2ee6;
  sprites[5] = 0x2ee7;
  sprites[6] = 0x2ee8;
  sprites[7] = 0x2ee9;
  switch (side) {
  case 0:
    sprite_put_xyz(sprites[0],x,y,z);
    for (i = 1; i < 8; i++) {
      sprite_put_xyz_scaled(sprites[i],(x - 16.0) + (float)(i * 0x28),y,z,1.0,1.0);
    }
    sprite_put_xyz(0x2ef6,x + 47.0,y + 3.0,z - 0.001);
    break;
  case 1:
    sprite_put_xyz(sprites[0],x + 320.0,y,z);
    for (i = 1; i < 8; i++) {
      sprite_put_xyz_scaled(sprites[i],(x + 320.0) - (float)(i * 0x28),y,z,-1.0,1.0);
    }
    sprite_put_xyz(0x2ef6,x + 65.0,y + 3.0,z - 0.001);
    break;
  }
}

void prize_plate_hide(int side)
{
  return;
}

void prize_banner_draw(char kind, char winner, int frame_arg)
{
  typedef struct { int w[18]; } corner_list;
  int loaded;
  float *cy;
  float scale_x;
  float *cx;
  float alpha;
  int *corner;
  float scale_y;
  uint fade;
  int corners [18];
  int frame;
  uint gray;
  
  frame = (int)(short)frame_arg;
  loaded = game_mes_is_loaded(0x26);
  if (loaded == 0) {
    return;
  }
  if (0x59 < frame) {
    if (frame < 0x6e) {
      gray = (uchar)(frame * 8 + 0x30);
    }
    else if (frame < 0x7a) {
      gray = (uchar)(0xa0 - ((short)frame * 0xa0 - 17600) / 12);
    }
    else {
      gray = 0;
    }
  }
  else {
    gray = 0;
  }
  sprite_put_xyz_scaled(0xb08,136.0,188.0,0.09,1.0,12.0);
  sprite_put_xyz_scaled(0xb08,504.0,188.0,0.09,-1.0,12.0);
  sprite_put_xyz_scaled(0xb03,200.0,188.0,0.09,30.0,12.0);
  sprite_put_xyz_scaled(0xb09,496.0,180.0,0.07,-2.75,1.0);
  sprite_put_xyz_scaled(0xb09,144.0,180.0,0.07,2.75,1.0);
  sprite_put_xyz_scaled(0xb09,496.0,278.0,0.07,-2.75,1.0);
  sprite_put_xyz_scaled(0xb09,144.0,278.0,0.07,2.75,1.0);
  if (winner == '\x01') {
    sprite_put_xyz(0x307a,275.0,253.0,0.03);
    if ((uchar)champion_side == '\0') {
      sprite_put_xyz(0x307b,351.0,253.0,0.03);
      sprite_put_xyz(0x3081,192.0,240.0,0.03);
      sprite_put_xyz(0x3083,320.0,240.0,0.03);
    }
    else {
      sprite_put_xyz(0x307c,351.0,253.0,0.03);
      sprite_put_xyz(0x3082,192.0,240.0,0.03);
      sprite_put_xyz(0x3083,320.0,240.0,0.03);
    }
  }
  switch(kind) {
  case 0:
    switch(winner) {
    case 0:
      sprite_put_xyz_scaled(0xb04,184.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb05,192.0,256.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb06,256.0,256.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb07,256.0,272.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb04,456.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb05,448.0,256.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb06,384.0,256.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb07,384.0,272.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x307d,248.0,192.0,0.03);
      sprite_put_xyz(0x307e,376.0,192.0,0.03);
      sprite_put_xyz(0x307f,192.0,240.0,0.03);
      sprite_put_xyz(0x3080,320.0,240.0,0.03);
      sprite_colors_set(0xffffff,gray << 0x10 | gray << 8 | gray);
      sprite_put_xyz(0x3070,300.0,253.0,0.03);
      sprite_colors_reset();
      return;
    case 1:
      sprite_put_xyz_scaled(0xb04,167.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb05,175.0,256.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb06,239.0,256.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb07,239.0,272.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0xb04,473.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb05,465.0,256.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb06,401.0,256.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0xb07,401.0,272.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3078,229.0,192.0,0.03);
      sprite_put_xyz(0x3079,357.0,192.0,0.03);
      break;
    }
    return;
  case 1:
    switch(winner) {
    case 0:
      sprite_put_xyz_scaled(0x3202,176.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3203,192.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3204,232.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3205,272.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3202,464.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3203,448.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3204,408.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3205,368.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3209,248.0,184.0,0.03);
      sprite_put_xyz(0x320a,376.0,184.0,0.03);
      sprite_put_xyz(0x307f,192.0,240.0,0.03);
      sprite_put_xyz(0x3080,320.0,240.0,0.03);
      sprite_colors_set(0xffffff,gray << 0x10 | gray << 8 | gray);
      sprite_put_xyz(0x3071,300.0,253.0,0.03);
      sprite_colors_reset();
      break;
    case 1:
      sprite_put_xyz_scaled(0x3202,160.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3203,176.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3204,216.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3205,256.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3202,480.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3203,464.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3204,424.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3205,384.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3206,229.0,184.0,0.03);
      sprite_put_xyz(0x3207,357.0,184.0,0.03);
      break;
    }
    sprite_next_blend_set(0);
    if (0x13 < frame) {
      if (frame < 0x20) {
        alpha = (float)(frame + -0x14) / 12.0;
      }
      else if (frame < 0x3c) {
        alpha = 1.0 - (float)(frame + -0x20) / 28.0;
      }
      else {
        alpha = 0.0;
      }
    }
    else {
      alpha = 0.0;
    }
    sprite_put_xyz_alpha_flags(0x3200,320.0,218.0,0.05,alpha,0x100b);
    sprite_put_xyz_alpha_flags(0x3201,320.0,218.0,0.05,alpha,0x1009);
    if (0x1f < frame) {
      if (frame < 0x3c) {
        scale_x = (float)(frame + -0x20) / 28.0 + 1.0;
      }
      else {
        scale_x = 0.0;
      }
    }
    else {
      scale_x = 1.0;
    }
    if (0x1f < frame) {
      if (frame < 0x3c) {
        scale_y = ((float)(frame + -0x20) / 28.0) * 0.2 + 1.0;
      }
      else {
        scale_y = 0.0;
      }
    }
    else {
      scale_y = 1.0;
    }
    sprite_put_xyz_scaled_alpha_flags(0x3200,320.0,218.0,0.05,scale_x,scale_y,alpha,0x100b);
    sprite_put_xyz_scaled_alpha_flags(0x3201,320.0,218.0,0.05,scale_x,scale_y,alpha,0x1009);
    if (0x13 < frame) {
      if (frame < 0x20) {
        alpha = (float)(frame + -0x14) / 12.0;
      }
      else if (frame < 0x56) {
        alpha = 1.0 - ((float)(frame + -0x20) / 54.0) * 0.8;
      }
      else {
        alpha = 0.0;
      }
    }
    else {
      alpha = 0.0;
    }
    if (0x1f < frame) {
      if (frame < 0x56) {
        scale_y = ((float)(frame + -0x20) / 54.0) * 0.2 + 1.0;
      }
      else {
        scale_y = 0.0;
      }
    }
    else {
      scale_y = 1.0;
    }
    scale_x = scale_y;
    if (winner == '\0') {
      sprite_put_xyz_scaled_alpha_flags(0x320d,376.0,216.0,0.02,scale_x,scale_y,alpha,0x100b);
      sprite_put_xyz_scaled_alpha_flags(0x320e,376.0,216.0,0.02,scale_x,scale_y,alpha,0x1009);
    }
    else {
      sprite_put_xyz_scaled_alpha_flags(0x320b,357.0,216.0,0.02,scale_x,scale_y,alpha,0x100b);
      sprite_put_xyz_scaled_alpha_flags(0x320c,357.0,216.0,0.02,scale_x,scale_y,alpha,0x1009);
    }
    sprite_next_blend_normal();
    return;
  case 2:
    switch(winner) {
    case 0:
      sprite_put_xyz_scaled(0x3394,176.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3395,192.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3396,232.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3397,272.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3394,464.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3395,448.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3396,408.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3397,368.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3399,248.0,192.0,0.03);
      sprite_put_xyz(0x339b,376.0,192.0,0.03);
      sprite_put_xyz(0x307f,192.0,240.0,0.03);
      sprite_put_xyz(0x3080,320.0,240.0,0.03);
      sprite_put_xyz_scaled(0x3398,185.0,197.0,0.03,1.0,1.0);
      sprite_put_xyz_scaled(0x3398,455.0,197.0,0.03,-1.0,1.0);
      sprite_colors_set(0xffffff,gray << 0x10 | gray << 8 | gray);
      sprite_put_xyz(0x3074,300.0,253.0,0.03);
      sprite_colors_reset();
      break;
    case 1:
      sprite_put_xyz_scaled(0x3394,160.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3395,176.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3396,216.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3397,256.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3394,480.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3395,464.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3396,424.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3397,384.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3390,229.0,192.0,0.03);
      sprite_put_xyz(0x3392,357.0,192.0,0.03);
      sprite_put_xyz_scaled(0x3398,167.0,197.0,0.03,1.0,1.0);
      sprite_put_xyz_scaled(0x3398,473.0,197.0,0.03,-1.0,1.0);
      break;
    }
    break;
  case 3:
    switch(winner) {
    case 0:
      sprite_put_xyz_scaled(0x3528,176.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3529,192.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x352a,232.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x352b,272.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3528,464.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3529,448.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x352a,408.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x352b,368.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3523,248.0,192.0,0.03);
      sprite_put_xyz(0x3524,376.0,192.0,0.03);
      sprite_put_xyz(0x307f,192.0,240.0,0.03);
      sprite_put_xyz(0x3080,320.0,240.0,0.03);
      sprite_colors_set(0xffffff,gray << 0x10 | gray << 8 | gray);
      sprite_put_xyz(0x3072,300.0,253.0,0.03);
      sprite_colors_reset();
      break;
    case 1:
      sprite_put_xyz_scaled(0x3528,160.0,240.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3529,176.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x352a,216.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x352b,256.0,248.0,0.07,1.0,1.0);
      sprite_put_xyz_scaled(0x3528,480.0,240.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x3529,464.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x352a,424.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz_scaled(0x352b,384.0,248.0,0.07,-1.0,1.0);
      sprite_put_xyz(0x3520,229.0,192.0,0.03);
      sprite_put_xyz(0x3521,357.0,192.0,0.03);
      break;
    }
    *(corner_list *)corners = *(corner_list *)prize_banner_corners;
    corner = corners;
    if (frame < 0x3c) {
      fade = (uchar)((short)frame * 0xff / 0x3c);
    }
    else if (frame < 0x78) {
      fade = (uchar)(0xff - ((short)frame * 0xff - 15300) / 0x3c);
    }
    else {
      fade = 0;
    }
    sprite_colors_set(0xffffff,fade << 0x10 | fade << 8 | fade);
    while (*corner != -1) {
      cx = (float *)(corner + 1);
      cy = (float *)(corner + 2);
      sprite_put_xyz_scaled(*corner,*cx,*cy,0.03,1.0,1.0);
      sprite_put_xyz_scaled(*corner,640.0 - *cx,*cy,0.03,-1.0,1.0);
      sprite_put_xyz_scaled(*corner,640.0 - *cx,480.0 - *cy,0.03,-1.0,-1.0);
      sprite_put_xyz_scaled(*corner,*cx,480.0 - *cy,0.03,1.0,-1.0);
      corner = corner + 3;
    }
    sprite_colors_reset();
    break;
  default:
    return;
  }
  sprite_anim_play(prize_banner_anim_g,(float)frame);
  return;
}

void prize_anim_start(int kind)
{
  prize_anim.kind = kind;
  prize_anim.frame = 0.0;
  return;
}

void prize_anim_tick(void)
{
  prize_anim_rec *p = &prize_anim;

  if (pause_frame_held() == 0) {
    p->frame = p->frame + 1.0;
  }
  if (180.0 <= p->frame) {
    p->frame = 180.0;
  }
}

void prize_anim_draw(void)
{
  float frame;
  prize_anim_rec *anim = &prize_anim;
  
  switch(anim->kind) {
  case 0:
    frame = anim->frame;
    sprite_anim_play(prize_anim_0,frame);
    break;
  case 1:
    frame = anim->frame;
    sprite_anim_play(prize_anim_1,frame);
    break;
  case 2:
    frame = anim->frame;
    sprite_anim_play(prize_anim_2,frame);
    break;
  case 3:
    frame = anim->frame;
    sprite_anim_play(prize_anim_3,frame);
    break;
  }
  return;
}

void prize_banner_appose_draw(void)
{
  int mission;
  
  mission = mission_active_get();
  if ((mission != -1) && (play_style == '\x02')) {
    prize_banner_draw((int)prize_kind,0,(int)(short)(int)prize_anim.frame);
    banner_shine_draw((short)(int)prize_anim.frame,160.0,240.0,(byte)prize_kind);
    card_message_post(0,0x1e,0xffffffff,0);
    card_message_post(1,0x1e,0xffffffff,0);
  }
}

void prize_banner_winner_draw(void)
{
  prize_anim_rec *p;
  int value;
  
  value = mission_active_get();
  p = &prize_anim;
  if (((value != -1) && (play_style == '\x02')) && (value = game_mes_is_loaded(0x26), value != 0)) {
    prize_banner_draw((int)prize_kind,1,(int)(short)(int)p->frame);
    banner_shine_draw((short)(int)p->frame,160.0,240.0,(byte)prize_kind);
  }
}

void prize_hud_draw(void)
{
  typedef struct { int sprite; float x; float y; } prize_mark;
  prize_mark marks[5] = {
    { 0x3070, 314.0f, 436.0f },
    { 0x3071, 314.0f, 436.0f },
    { 0x3074, 314.0f, 436.0f },
    { 0x3072, 314.0f, 436.0f },
    { -1, 0.0f, 0.0f },
  };
  char buf[52];
  char *p;
  float alpha;

  if ((int)mission_active_get() == -1) return;
  if (play_style != 2) return;
  alpha = sin_radians((float)mode_timer / 20.0f) * 0.3f + 0.7f;
  if (prize_fly_state == 0) {
    sprite_put_xyz_alpha(marks[prize_kind].sprite, marks[prize_kind].x, marks[prize_kind].y, 0.03f, alpha);
  }
  if (debug_info_enabled() == 0) return;
  p = buf + sprintf(buf, prize_debug_1p_text);
  if (prize_debug_item >= 0) {
    p += sprintf(p, prize_debug_item_format, prize_debug_item);
  }
  if (prize_debug_money >= 0) {
    sprintf(p, prize_debug_money_format, prize_debug_money);
  }
  if (mission_sides_get() == 2 || mission_sides_get() == 0) {
    sprite_colors_set(0xffff00, 0);
  }
  sprite_text_put(&euc_text_font, 70.0f, 412.0f, 0.006f, 0, (byte *)buf);
  sprite_colors_reset();
  p = buf + sprintf(buf, prize_debug_2p_text);
  if (prize_debug_item_2p >= 0) {
    p += sprintf(p, prize_debug_item_format, prize_debug_item_2p);
  }
  if (prize_debug_money_2p >= 0) {
    sprintf(p, prize_debug_money_format, prize_debug_money_2p);
  }
  if (mission_sides_get() == 2 || mission_sides_get() == 1) {
    sprite_colors_set(0xffff00, 0);
  }
  sprite_text_put(&euc_text_font, 570.0f, 412.0f, 0.006f, 2, (byte *)(p = buf));
  sprite_colors_reset();
  if (prize_debug_rank == -1) {
    sprintf(p, prize_debug_focus_text);
  }
  else if (prize_debug_rank == -2) {
    sprintf(p, prize_debug_none_text);
  }
  else {
    sprintf(p, prize_debug_rank_format, prize_debug_rank);
  }
  sprite_text_put(&euc_text_font, 280.0f, 422.0f, 0.006f, 0, (byte *)buf);
}

void prize_debug_value_set(int side_arg, int kind_arg, int value_arg)
{
  char side = side_arg;
  char kind = kind_arg;
  short value = value_arg;
  switch (kind) {
  case 0:
    (&prize_debug_item)[side] = value;
    break;
  case 1:
    (&prize_debug_money)[side] = value;
    break;
  case 2:
    prize_debug_rank = value;
    break;
  }
}

void net_status_icon_draw(int side_arg, uint timer, float x, float y, float z, int state)
{
  char side = (char)side_arg;
  float offset;
  float alpha;
  
  if (state == 1) {
    sprite_put_xy_colors_z_alpha(0xa53,x,y,0,side == 0 ? 0xff0000 : 0xff,z + 1e-05,1.0);
    sprite_put_xyz(0xa52,x,y,z);
    if ((int)timer >= 0x3d && (int)timer <= 0x77) {
      offset = ((float)(int)(0x78 - timer) / 60.0) * 68.0;
      if ((int)timer < 0x5a) {
        alpha = (float)(int)(timer - 0x3c) / 30.0;
      }
      else {
        alpha = 1.0 - (float)(int)(timer - 0x5a) / 30.0;
      }
      sprite_put_xyz_alpha(0xa51,(x + offset) - 38.0,y - 4.0,z - 1e-05,alpha);
      sprite_put_xyz_scaled_alpha(0xa51,(x + offset) + 58.0,y - 4.0,z - 1e-05,-1.0,1.0,alpha);
    }
  }
  else if ((((int)timer / 36) & 1) == 0) {
    sprite_put_xyz(0xa50,x,y,z + 1e-05);
  }
}

void prize_layer_2efb_draw(float x, float y, float z)
{
  sprite_flags_override_set(0x100a);
  sprite_put_xyz(0x2efb,x,y,z);
  sprite_flags_override_reset();
  return;
}

void prize_layer_2efb_parts_draw(float x, float y, float z)
{
  typedef struct { int sprite; float x; float y; } part_ent;
  part_ent parts[4] = {{0x2f01, 82.0, 194.0}, {0x2f02, 98.0, 250.0}, {0x2f03, 170.0, 274.0}, {-1, 0.0, 0.0}};
  char i;

  prize_layer_2efb_draw(x,y,z);
  for (i = 0; parts[i].sprite != -1; i++) {
    sprite_put_xyz_scaled(parts[i].sprite,(x + parts[i].x) - 320.0,(y + parts[i].y) - 236.0,z,1.0,1.0);
    sprite_put_xyz_scaled(parts[i].sprite,640.0 - ((parts[i].x + x) - 320.0),(y + parts[i].y) - 236.0,z,-1.0,1.0);
  }
}

void prize_layer_2efa_draw(float x, float y, float z)
{
  prize_layer_pick_draw(0,x,y,z);
  return;
}

void prize_layer_2ef8_draw(float x, float y, float z)
{
  prize_layer_pick_draw(1,x,y,z);
  return;
}

void prize_layer_2ef9_draw(float x, float y, float z)
{
  prize_layer_pick_draw(2,x,y,z);
  return;
}

void prize_layer_2efc_draw(float x, float y, float z)
{
  sprite_flags_override_set(0x100b);
  sprite_put_xyz(0x2efc,x,y,z - 0.001);
  sprite_flags_override_reset();
  return;
}

void prize_layer_2efd_draw(float x, float y, float z)
{
  sprite_flags_override_set(0x1009);
  sprite_put_xyz(0x2efd,x,y,z - 0.001);
  sprite_flags_override_reset();
  return;
}

void prize_layer_2efe_draw(float x, float y, float z)
{
  sprite_next_blend_set(0);
  sprite_flags_override_set(0x100b);
  sprite_put_xyz(0x2efe,x,y,z);
  sprite_flags_override_reset();
  sprite_flags_override_set(0x1009);
  sprite_put_xyz(0x2eff,x,y,z);
  sprite_flags_override_reset();
  sprite_next_blend_normal();
  return;
}

void prize_layer_2f00_draw(float x, float y, float z)
{
  sprite_next_blend_set(0);
  sprite_flags_override_set(0x100e);
  sprite_put_xyz(0x2f00,x,y,z - 0.001);
  sprite_flags_override_reset();
  ((void (*)(int,float,float,float,int))sprite_put_flags_xyz)(0x2f00,x,y,z - 0.001,0x1026);
  sprite_next_blend_normal();
}

void prize_layer_3075_draw(float x, float y, float z)
{
  sprite_next_blend_set(0);
  sprite_flags_override_set(0x100e);
  sprite_put_xyz(0x3075,x,y,z - 0.001);
  sprite_flags_override_reset();
  ((void (*)(int,float,float,float,int))sprite_put_flags_xyz)(0x3075,x,y,z - 0.001,0x1026);
  sprite_next_blend_normal();
  return;
}

void prize_layer_pick_draw(int kind, float x, float y, float z)
{
  int sprites [3];
  
  sprites[0] = 0x2efa;
  sprites[1] = 0x2ef8;
  sprites[2] = 0x2ef9;
  if (kind < 3) {
    sprite_flags_override_set(0x100a);
    sprite_next_blend_set(0);
    sprite_put_xyz(sprites[kind],x,y,z - 0.001);
    sprite_next_blend_normal();
    sprite_flags_override_reset();
  }
  else if (kind == 3) {
    sprite_flags_override_set(0x100b);
    sprite_put_xyz(0x2efc,x,y,z - 0.001);
    sprite_flags_override_reset();
    sprite_flags_override_set(0x1009);
    sprite_put_xyz(0x2efd,x,y,z - 0.001);
    sprite_flags_override_reset();
  }
  return;
}

void player_message_pick(int side_arg, char *choice)
{
  char side = (char)side_arg;
  player_input_rec *input;
  player_input_rec *input2;
  uint buttons;
  int pressed;
  char i;
  
  pressed = 0;
  input = player_input_get(side);
  input2 = player_input_get(side);
  buttons = input->pressed | input2->now;
  if ((buttons & 7) == 1) {
    if ((buttons & 0x80) != 0) {
      pressed = 1;
    }
    if ((buttons & 0x10) != 0) {
      pressed = pressed | 2;
    }
    if ((buttons & 0x40) != 0) {
      pressed = pressed | 4;
    }
    if ((buttons & 0x20) != 0) {
      pressed = pressed | 8;
    }
  }
  if (((pressed >> *choice) & 1) == 0) {
    for (i = 0; i < 4; i++) {
      if ((pressed >> i) & 1) {
        *choice = i;
        return;
      }
    }
    *choice = -1;
  }
  return;
}

void player_message_draw(void)
{
  char n;
  char i;
  char *s;
  float f;
  float v[2];
  n = 0;
  for (i = 0; i < 2; i++) {
    if (player_messages[i].set != 0 && player_messages[i].sel > -1) {
      s = player_messages[i].texts[player_messages[i].sel];
      f = player_messages[i].width;
      v[i] = player_messages[i].heights[player_messages[i].sel];
      if (*s != 0) {
        sprite_text_put(&euc_text_font,i == 0 ? 50.0 : 540.0 - f,92.0,0.008,0,(byte *)s);
        n++;
      }
    }
    else {
      v[i] = 0.0;
    }
  }
  if (n != 0) {
    sprite_put_xyz_scaled_alpha(0,0.0,90.0,0.009,80.0,v[0] > v[1] ? v[0] : v[1],0.5);
  }
}

void player_message_text_set(int side_arg, int slot_arg, char *text, int width, int lines)
{
  int side;
  int slot;
  side = (int)(char)side_arg;
  slot = (int)(char)slot_arg;
  snprintf(player_messages[side].texts[slot],0x3d,"%s",text);
  if (slot == 0 || (float)width > player_messages[side].width) {
    player_messages[side].width = (float)width * 8.0;
  }
  player_messages[side].heights[slot] = (float)lines + (float)lines + 1.0;
  player_messages[side].sel = 0xff;
  return;
}

void rob_flag_pair_draw(void)
{
  int side;
  int slot;
  int slot_sprite;
  int state_sprite;
  int base_sprite;
  float y_offset;
  float x;
  float base_y;
  float overlay_y;
  float overlay_x;
  int state[2];
  int change_flags;
  
  if ((game_flags & 4) != 0) {
  for (side = 0; side < 2; side++) {
    if (((rob_get((uchar)side))->flags & 4) != 0) {
      change_flags = level_change_flags_get(side);
      y_offset = 0.0;
      if ((change_flags & 7) != 0 || ((int)mission_active_get() != -1 && play_style == 2)) {
        y_offset = -21.0;
      }
      for (slot = 0; slot < 2; slot++) {
        state[slot] = rob_card_flag_test((char)side,(char)slot) == 1;
        slot_sprite = (int)rob_flag_slot_sprites[slot];
        state_sprite = (int)rob_flag_state_sprites[state[slot]];
        base_sprite = (int)rob_flag_base_sprites[state[slot]];
        x = rob_flag_pair_x[side] + (float)slot * rob_flag_pair_step;
        base_y = y_offset + rob_flag_pair_y;
        overlay_y = base_y + rob_flag_overlay_dy;
        sprite_put_xyz(base_sprite,x,base_y,0.02);
        overlay_x = x + rob_flag_slot_dx;
        sprite_put_xyz_alpha(slot_sprite,overlay_x,overlay_y,0.01,rob_flag_overlay_alpha);
        overlay_x = x + rob_flag_state_dx;
        sprite_put_xyz_alpha(state_sprite,overlay_x,overlay_y,0.01,rob_flag_overlay_alpha);
      }
    }
  }
  }
}

void mode_sel_open(void)
{
  mode_sel_blink = 0;
  sprite_set_load(0x28);
  return;
}

void mode_sel_choices_draw(int choice, uchar fast)
{
  int i;
  float x;
  float y;
  float z;
  float alpha;
  float pulse;
  float rate;
  
  rate = mode_sel_pulse_rate;
  if (fast != 0) {
    rate = mode_sel_pulse_rate_fast;
  }
  pulse = sin_radians((((float)mode_sel_blink * 6.2831855) / 60.0) * rate);
  pulse = pulse * 0.5 + 0.5;
  for (i = 0; i < 2; i++) {
    x = mode_sel_choice_x[i];
    y = mode_sel_choice_y[i];
    z = 0.007 - mode_sel_depth_step;
    alpha = 1.0;
    sprite_put_xyz_alpha(0x3e81,x,y,z,alpha);
    z = 0.007 - mode_sel_depth_step * 3.0;
    sprite_put_xyz_alpha(mode_sel_choice_sprites[i],x,y,z,alpha);
    if (i == choice) {
      x = mode_sel_choice_x[i] + mode_sel_highlight_dx;
      y = mode_sel_choice_y[i] + mode_sel_highlight_dy;
      z = 0.007 - (mode_sel_depth_step + mode_sel_depth_step);
      sprite_next_blend_set(0);
      sprite_colors_set(0,mode_sel_highlight_color);
      sprite_put_xyz_scaled_alpha(0,x,y,z,mode_sel_highlight_w,mode_sel_highlight_h,pulse);
      sprite_colors_reset();
      sprite_next_blend_normal();
    }
  }
  mode_sel_blink = mode_sel_blink + 1;
}

void mode_sel_timer_draw(int timer)
{
  int seconds;
  int tens;
  int ones;
  float y;
  float x1;
  int *digits;
  float x2;

  if (timer != 0) {
    seconds = timer / 60;
    if (seconds > 99) seconds = 99;
    tens = seconds / 10;
    ones = seconds - tens * 10;
    y = mode_sel_timer_y + mode_sel_timer_dy;
    if (tens != 0) {
      x1 = mode_sel_timer_x + mode_sel_timer_tens_dx;
      x2 = mode_sel_timer_x + mode_sel_timer_ones_dx;
      digits = round_timer_digits;
      sprite_put_xyz(digits[tens], x1, y, 0.007 - mode_sel_depth_step);
      sprite_put_xyz(digits[ones], x2, y, 0.007 - mode_sel_depth_step);
    }
    else {
      x2 = mode_sel_timer_x + mode_sel_timer_single_dx;
      if (ones > 3 || (mode_sel_blink & 4) == 0) {
        sprite_put_xyz(round_timer_low_digits[ones], x2, y, 0.007 - mode_sel_depth_step);
      }
    }
  }
}

void mode_sel_box_draw(uchar alt)
{
  float h;
  float alpha;
  h = mode_sel_box_h;
  alpha = mode_sel_box_alpha;
  if (alt) {
    h = mode_sel_box_h_alt;
    alpha = mode_sel_box_alpha_alt;
  }
  sprite_put_xyz_scaled_alpha(0,mode_sel_box_x,mode_sel_box_y,0.007,mode_sel_box_w,h,alpha);
}

void mode_sel_close(void)
{
  sprite_set_free(0x28);
  return;
}

char * level_name_get(int level)
{
  return level_table[level].name;
}
