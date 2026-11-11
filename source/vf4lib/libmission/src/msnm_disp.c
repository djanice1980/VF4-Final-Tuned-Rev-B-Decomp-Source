/*
 * vf4lib/libmission/src/msnm_disp.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_xyz
#define KRPROTO_aet_still_move_to
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern chlg_layer_desc chlg_result_layer_descs[5]; /* 0x0c1a0188 */
extern chlg_layer_desc chlg_coin_layer_descs[12]; /* 0x0c1a0228 */
extern int chlg_digits_large[17]; /* 0x0c1a0378 */
extern int chlg_digits_small[17]; /* 0x0c1a03bc */
extern int mode_timer; /* 0x0c292550 */
extern uint course_time_total; /* 0x0c2925f8 */
extern char * msn_texts_a[64]; /* 0x0c2a0e38 */
extern char * msn_texts_b[64]; /* 0x0c2a0f38 */
extern char * msn_hints[64]; /* 0x0c2a1038 */
extern char * msn_noname_text; /* 0x0c2a1168 */
extern char * msn_stance_finish_texts[22]; /* 0x0c2a15e0 */
extern char * msn_text_tag_names[19]; /* 0x0c2a1c6c */
extern int coin_viewer_prio_index; /* 0x0c2a1e78 */
extern chlg_total_column_xy chlg_total_columns; /* 0x0c1a0108 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern msn_text_box msn_hint_box; /* 0x0c2a1cb8 */
extern chlg_coin_get_pos chlg_coin_get_xy; /* 0x0c2a1e3c */
extern msnm_disp_rec msnm_disp_state; /* 0x0c450f24 */

static inline float part_width(int id)
{
  return chlg_rank_part_offset(id,1);
}

void msnm_disp_init(void)
{
  uchar i;
  
  aet_set_load(3);
  memset(&msnm_disp_state,0,0x164);
  for (i = 0; i < 0x1f; i++) {
    msnm_disp_state.objs[i] = -1;
  }
  for (i = 0; i < 0x31; i++) {
    msnm_disp_state.stills[i] = -1;
  }
  msnm_disp_state.plate_wait = 0;
  msnm_disp_state.flags = msnm_disp_state.flags | 2;
  msnm_disp_state.hidden = 0;
}

void msnm_disp_release(void)
{
  aet_set_free(3);
  return;
}

void msnm_disp_scenes_load(void)
{
  aet_scene_textures_load(0x30002);
  aet_scene_textures_load(0x30003);
  aet_scene_textures_load(0x30004);
  aet_scene_textures_load(0x30006);
  aet_scene_textures_load(0x30001);
  aet_scene_textures_load(0x30007);
  sprite_set_load(0x27);
  sprite_set_load(0x31);
  return;
}

void msnm_disp_scenes_free(void)
{
  aet_objs_stop_by_scene(0x30002);
  aet_objs_stop_by_scene(0x30003);
  aet_stills_stop_by_scene(0x30004);
  aet_objs_stop_by_scene(0x30006);
  aet_objs_stop_by_scene(0x30001);
  aet_stills_stop_by_scene(0x30007);
  aet_scene_textures_free(0x30002);
  aet_scene_textures_free(0x30003);
  aet_scene_textures_free(0x30004);
  aet_scene_textures_free(0x30006);
  aet_scene_textures_free(0x30001);
  aet_scene_textures_free(0x30007);
  sprite_set_free(0x31);
  sprite_set_free(0x27);
  return;
}

void msnm_disp_scene0_load(void)
{
  if (msnm_work->scene0_loaded == '\0') {
    aet_scene_textures_load(0x30000);
    sprite_set_load(0x27);
    msnm_work->scene0_loaded = 1;
  }
  return;
}

void msnm_disp_scene0_free(void)
{
  if (msnm_work->scene0_loaded != '\0') {
    aet_objs_stop_by_scene(0x30000);
    aet_scene_textures_free(0x30000);
    sprite_set_free(0x27);
    msnm_work->scene0_loaded = 0;
  }
  return;
}

void msnm_quit_confirm_draw(msnm_quit_rec *quit)
{
  uint col;
  float *box;
  float *time_pos;
  float x;
  float y;
  int frames;
  int tens;
  int ones;
  char buf[256];

  col = color_pulse_mix(quit->timer, 0xffffff, 0, 1.0f);
  if (quit->choice == 0) {
    msn_text_custom_color_set(6, col, 0);
    msn_text_custom_color_set(7, 0xff808080, 0);
  }
  else {
    msn_text_custom_color_set(6, 0xff808080, 0);
    msn_text_custom_color_set(7, col, 0);
  }
  msn_text_align_set(2);
  box = (float *)&msn_quit_box;
  msn_text_box_draw(box, msn_quit_text);
  msn_text_custom_colors_reset();
  time_pos = msn_quit_time_pos;
  x = box[0] + time_pos[0];
  y = box[1] + time_pos[1];
  sprite_text_put(&euc_text_font, x, y, box[8], 0, msn_quit_time_label);
  frames = quit->timer;
  tens = frames / 600;
  ones = frames / 60 - tens * 10;
  if (tens > 10) {
    tens = 9;
  }
  if (tens == 0) {
    sprintf(buf, "  %s", msn_wide_digits[ones]);
  }
  else {
    char **digits = msn_wide_digits;
    sprintf(buf, "%s%s", digits[tens], digits[ones]);
  }
  col = 0xffffff;
  if ((tens == 0) & (ones < 4)) {
    col = color_pulse_mix(quit->timer, col, 0, 3.0f);
  }
  sprite_colors_set(col, 0);
  x += msn_quit_digit_dx;
  sprite_text_put(&euc_text_font, x, y, ((float *)&msn_quit_box)[8], 0, buf);
  sprite_colors_reset();
}

void msnm_map_base_open(int unused, uchar show_logo)
{
  int stage;
  int shown;
  char *layer;
  aet_obj_param buf;
  aet_still_param still;
  
  stage = msnm_row_get();
  if ((show_logo != '\0') && (msnm_disp_state.stills[46] == -1)) {
    aet_still_param_init(&still);
    aet_still_param_scene_set(&still,0x30007);
    aet_still_param_layer_set(&still,"PARTS_CHLG_MODE_ROGO");
    aet_still_param_depth_range_set(&still,0.02,0.02);
    aet_still_param_alpha_set(&still,0.7);
    msnm_disp_state.stills[46] = aet_still_start(&still);
  }
  if (msnm_disp_state.objs[0] == -1) {
    if ((stage == 0) & (show_logo == '\0')) {
      layer = "MAP_BASE_OP_1_BG";
      sound_play_id(0x367,0);
    }
    else {
      layer = "MAP_BASE_OP_2";
    }
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30002);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,layer);
    aet_param_depth_range_set(&buf,0.07,0.07);
    if (msnm_disp_state.hidden == '\0') {
      msnm_disp_state.objs[0] = aet_obj_start(&buf);
    }
    if ((stage == 0) & (show_logo == '\0')) {
      aet_param_init(&buf);
      aet_param_scene_set(&buf,0x30002);
      aet_param_flags_set(&buf,4);
      aet_param_filter_set(&buf,0);
      aet_param_layer_set(&buf,"MAP_BASE_OP_1");
      aet_param_depth_range_set(&buf,0.05,0.05);
      aet_obj_start(&buf);
    }
  }
  shown = aet_obj_end_reached(msnm_disp_state.objs[0]);
  if (shown != 0) {
    msnm_map_pos_read_stub();
  }
  return;
}

void msnm_map_pos_read_stub(void)
{
  msnm_row_get();
  msnm_column_get();
  return;
}

void msnm_map_plates_draw(int slot, uint flag)
{
  typedef struct { float x; float y; } pos2;
  char sel;
  uchar s;
  uchar n;
  int stage;
  int branch;
  int type;
  char *name;
  int obj;
  float x;
  float y;
  float z;
  pos2 pos[2];
  aet_obj_param buf;
  aet_still_param buf2;

  sel = (char)slot;
  s = (uchar)flag;
  stage = msnm_row_get();
  branch = msnm_column_get();
  if (msnm_work->state > 3) {
    msnm_map_round_parts_draw(2,sel);
    msnm_map_course_draw(0);
    msnm_disp_state.sel = sel;
    x = (float)(stage + 1) * 50.0;
    y = (float)branch * 17.0;
    pos[0].x = x + y;
    pos[0].y = y;
    z = (float)(branch + 1) * 17.0;
    pos[1].x = x + z;
    pos[1].y = z;
    for (n = 0; n < 2; n++) {
      if (msnm_disp_state.objs[6] != -1 && msnm_disp_state.plate_wait == 0 &&
          msnm_disp_state.objs[n + 15] == -1 && aet_obj_end_reached(msnm_disp_state.objs[0]) != 0) {
        if ((msnm_disp_state.flags & 2) != 0) {
          name = e_chlg_map_plate_names[branch + n];
          type = 4;
          if (n == sel) {
            msnm_disp_state.flags |= 1;
          }
          if (n == 1) {
            msnm_disp_state.flags &= ~2;
          }
        }
        else if (n == sel) {
          if (s != 0) {
            name = e_chlg_map_plate_names[msnm_disp_state.plate];
          }
          else {
            name = e_platemove_names[msnm_disp_state.plate];
          }
          type = 4;
          msnm_disp_state.flags |= 1;
        }
        else {
          name = e_chlg_map_plate_0c2a1d24_names[branch + n];
          type = 8;
        }
        aet_param_init(&buf);
        aet_param_scene_set(&buf,0x30002);
        aet_param_layer_set(&buf,name);
        aet_param_depth_range_set(&buf,0.02,0.02);
        aet_param_flags_set(&buf,type);
        aet_param_pos_set(&buf,pos[n].x,pos[n].y);
        if (msnm_disp_state.hidden == 0) {
          msnm_disp_state.objs[n + 15] = aet_obj_start(&buf);
        }
        if (n == sel) {
          aet_param_init(&buf);
          aet_param_scene_set(&buf,0x30002);
          aet_param_layer_set(&buf,"MAP_HIKARI2");
          aet_param_depth_range_set(&buf,0.01,0.01);
          aet_param_flags_set(&buf,4);
          aet_param_color_set(&buf,chlg_branch_colors[msnm_disp_state.plate]);
          aet_param_pos_set(&buf,pos[n].x,pos[n].y);
          msnm_disp_state.objs[1] = aet_obj_start(&buf);
        }
        if ((n == sel) & (s == 0)) {
          aet_obj_flag_put(msnm_disp_state.objs[1],4,0);
          aet_obj_flag_put(msnm_disp_state.objs[1],0x10,1);
          aet_param_init(&buf);
          aet_param_scene_set(&buf,0x30002);
          aet_param_layer_set(&buf,"MAP_HIKARI");
          aet_param_depth_range_set(&buf,0.01,0.01);
          aet_param_flags_set(&buf,4);
          aet_param_color_set(&buf,chlg_branch_colors[msnm_disp_state.plate]);
          aet_param_pos_set(&buf,pos[n].x,pos[n].y);
          msnm_disp_state.objs[2] = aet_obj_start(&buf);
          aet_param_init(&buf);
          aet_param_scene_set(&buf,0x30002);
          aet_param_layer_set(&buf,"MAP_CROSS_V");
          aet_param_depth_range_set(&buf,0.03,0.03);
          aet_param_color_set(&buf,chlg_branch_colors[msnm_disp_state.plate]);
          aet_param_pos_set(&buf,pos[n].x,pos[n].y);
          aet_obj_start(&buf);
          aet_param_init(&buf);
          aet_param_scene_set(&buf,0x30002);
          aet_param_layer_set(&buf,"MAP_CROSS_H");
          aet_param_depth_range_set(&buf,0.02,0.02);
          aet_param_color_set(&buf,chlg_branch_colors[msnm_disp_state.plate]);
          aet_param_pos_set(&buf,pos[n].x,pos[n].y);
          aet_obj_start(&buf);
        }
      }
    }
    if (aet_obj_end_reached(msnm_disp_state.objs[sel + 15]) != 0 && (obj = msnm_disp_state.objs[sel + 15]) != -1 &&
        (msnm_disp_state.flags & 1) != 0) {
      msnm_disp_state.flags &= ~1;
      aet_obj_stop(obj);
      msnm_disp_state.objs[sel + 15] = -1;
      aet_param_init(&buf);
      aet_param_scene_set(&buf,0x30002);
      aet_param_layer_set(&buf,e_chlg_map_plate_0c2a1d24_names[msnm_disp_state.plate]);
      aet_param_depth_range_set(&buf,0.02,0.02);
      aet_param_flags_set(&buf,2);
      aet_param_pos_set(&buf,pos[sel].x,pos[sel].y);
      if (msnm_disp_state.hidden == 0) {
        msnm_disp_state.objs[sel + 15] = aet_obj_start(&buf);
      }
    }
    if (msnm_work->state > 3) {
      if (msnm_disp_state.objs[5] == -1) {
        aet_param_init(&buf);
        aet_param_scene_set(&buf,0x30001);
        aet_param_flags_set(&buf,2);
        aet_param_layer_set(&buf,"LABOR_WORK");
        aet_param_depth_range_set(&buf,0.01,0.01);
        msnm_disp_state.objs[5] = aet_obj_start(&buf);
      }
      if (msnm_disp_state.stills[43] == -1) {
        aet_still_param_init(&buf2);
        aet_still_param_scene_set(&buf2,0x30007);
        aet_still_param_layer_set(&buf2,"PARTS_LABOR_COME");
        aet_still_param_depth_range_set(&buf2,0.02,0.02);
        aet_still_param_pos_set(&buf2,25.0,220.0);
        aet_still_param_alpha_set(&buf2,0.7);
        msnm_disp_state.stills[43] = aet_still_start(&buf2);
      }
    }
    if (s == 0) {
      for (n = 0; n < 2; n++) {
        if (n != sel) {
          aet_obj_stop(msnm_disp_state.objs[n + 15]);
        }
      }
    }
  }
}

void msnm_map_round_parts_draw(mode, slot) char mode; char slot;
{
  int stage;
  int branch;
  aet_still_param buf;
  
  stage = msnm_row_get();
  branch = msnm_column_get();
  branch = branch + slot;
  if (mode == 2) {
    msn_panel_base_create(0,branch);
  }
  if (msnm_disp_state.stills[29] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_color_set(&buf,chlg_branch_colors[branch]);
    aet_still_param_depth_range_set(&buf,0.04,0.04);
    if (mode == 0) {
      aet_still_param_layer_set(&buf,e_parts_r_names[stage]);
    }
    else if (mode == 2) {
      aet_still_param_layer_set(&buf,e_parts_r_names[stage + 1]);
    }
    else {
      aet_still_param_layer_set(&buf,e_parts_r_num_names[stage]);
    }
    if (mode == 1) {
      aet_still_param_pos_set(&buf,330.0,397.0);
    }
    else {
      aet_still_param_pos_set(&buf,360.0,283.0);
    }
    msnm_disp_state.stills[29] = aet_still_start(&buf);
  }
  if (msnm_disp_state.stills[31] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_color_set(&buf,chlg_branch_colors[branch]);
    aet_still_param_depth_range_set(&buf,0.04,0.04);
    if (mode == 1) {
      aet_still_param_layer_set(&buf,"PARTS_R_NUM_BAR");
      aet_still_param_pos_set(&buf,355.0,402.0);
    }
    else {
      aet_still_param_layer_set(&buf,"PARTS_R_BAR");
      aet_still_param_pos_set(&buf,398.0,293.0);
    }
    msnm_disp_state.stills[31] = aet_still_start(&buf);
  }
  if (msnm_disp_state.stills[30] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_color_set(&buf,chlg_branch_colors[branch]);
    aet_still_param_depth_range_set(&buf,0.04,0.04);
    if (mode != 1) {
      aet_still_param_layer_set(&buf,e_parts_r_names[branch]);
    }
    else {
      aet_still_param_layer_set(&buf,e_parts_r_num_names[branch]);
    }
    if (mode == 1) {
      aet_still_param_pos_set(&buf,370.0,397.0);
    }
    else {
      aet_still_param_pos_set(&buf,420.0,283.0);
    }
    msnm_disp_state.stills[30] = aet_still_start(&buf);
  }
  return;
}

void msnm_map_course_draw(uint mode)
{
  typedef struct { float x; float y; } map_pos;
  aet_obj_param plate;
  aet_still_param rank;
  uint sel;
  int stage;
  map_pos pp = {0.0f, 0.0f};
  map_pos rp = {0.0f, 0.0f};
  msnm_course_results_rec *route;
  uchar i;
  int branch;
  int ok;
  char *shown;
  float r;

  sel = mode & 0xff;
  {
    map_pos ofs = {58.0f, 120.0f};
    float w = 50.0f;
    float h = 17.0f;
    route = &msnm_work->course;
    shown = (char *)aet_obj_end_reached(msnm_disp_state.objs[0]);
    if (shown != 0) {
      for (i = 0; i < 7; i++) {
        if (route->columns[i] != -1) {
          stage = i;
          branch = route->columns[i];
          pp.x = (float)stage * w + (float)branch * h;
          pp.y = (float)branch * h;
          ok = 0;
          if (i == 0) {
            if (msnm_disp_state.objs[7] == -1) ok = 1;
          }
          else {
            if (msnm_disp_state.objs[i + 7] == -1 && msnm_disp_state.plate_wait == 0) ok = 1;
          }
          if (ok) {
            aet_param_init(&plate);
            aet_param_scene_set(&plate,0x30002);
            aet_param_layer_set(&plate,e_chlg_map_plate_names[branch]);
            aet_param_depth_range_set(&plate,0.02,0.02);
            aet_param_flags_set(&plate,4);
            aet_param_pos_set(&plate,pp.x,pp.y);
            {
              if (msnm_disp_state.hidden == 0) {
                msnm_disp_state.objs[i + 7] = aet_obj_start(&plate);
              }
            }
            msnm_disp_state.plate_wait = 10;
          }
          r = chlg_rank_part_offset(route->grades[i],0);
          rp.x = (float)stage * w + ofs.x + (float)branch * h + r;
          rp.y = ((float)branch * h + ofs.y) - 4.0f;
          if (msnm_disp_state.stills[i + 34] == -1 &&
              aet_obj_end_reached(msnm_disp_state.objs[i + 7]) != 0) {
            aet_still_param_init(&rank);
            aet_still_param_scene_set(&rank,0x30004);
            aet_still_param_layer_set(&rank,e_chlg_map_rank_names[route->grades[i]]);
            aet_still_param_depth_range_set(&rank,0.01,0.01);
            aet_still_param_pos_set(&rank,rp.x,rp.y);
            if (msnm_disp_state.hidden == 0) {
              msnm_disp_state.stills[i + 34] = aet_still_start(&rank);
            }
          }
        }
      }
      stage = msnm_row_get();
      branch = msnm_column_get();
      r = chlg_rank_part_offset(msnm_work->grade,0);
      pp.x = (float)stage * w + (float)branch * h;
      pp.y = (float)branch * h;
      rp.x = (float)stage * w + ofs.x + (float)branch * h + r;
      rp.y = ((float)branch * h + ofs.y) - 4.0f;
      ok = 0;
      if (stage == 0 && msnm_disp_state.objs[6] == -1) {
        ok = 1;
      }
      else {
        if (msnm_disp_state.objs[6] == -1 && msnm_disp_state.objs[branch + 7] != 0 && msnm_disp_state.plate_wait == 0) ok = 1;
      }
      if (ok) {
        aet_param_init(&plate);
        aet_param_scene_set(&plate,0x30002);
        aet_param_layer_set(&plate,e_chlg_map_plate_names[branch]);
        aet_param_depth_range_set(&plate,0.02,0.02);
        aet_param_flags_set(&plate,4);
        aet_param_pos_set(&plate,pp.x,pp.y);
        if (msnm_disp_state.hidden == 0) {
          msnm_disp_state.objs[6] = aet_obj_start(&plate);
        }
        msnm_disp_state.flags = msnm_disp_state.flags | 1;
      }
      if (msnm_disp_state.objs[6] != -1 && (msnm_disp_state.flags & 1) != 0 &&
          aet_obj_end_reached(msnm_disp_state.objs[6]) != 0) {
        if (sel == 0) {
          msnm_disp_state.flags = msnm_disp_state.flags | 4;
        }
        msnm_disp_state.flags = msnm_disp_state.flags & 0xfffffffe;
        aet_obj_stop(msnm_disp_state.objs[6]);
        msnm_disp_state.objs[6] = -1;
        aet_param_init(&plate);
        aet_param_scene_set(&plate,0x30002);
        aet_param_layer_set(&plate,e_chlg_map_plate_0c2a1d24_names[branch]);
        aet_param_depth_range_set(&plate,0.02,0.02);
        aet_param_flags_set(&plate,2);
        aet_param_pos_set(&plate,pp.x,pp.y);
        if (msnm_disp_state.hidden == 0) {
          msnm_disp_state.objs[6] = aet_obj_start(&plate);
        }
      }
      if (msnm_disp_state.objs[6] != -1 && (msnm_disp_state.flags & 4) != 0 &&
          msnm_work->state != 10 &&
          ((msnm_work->state == 1 && aet_obj_end_reached(msnm_disp_state.objs[26]) != 0) ||
           msnm_work->state > 1)) {
        msnm_disp_state.flags = msnm_disp_state.flags & 0xfffffffb;
        aet_obj_flag_put(msnm_disp_state.objs[6],2,0);
        aet_obj_flag_put(msnm_disp_state.objs[6],4,1);
        aet_obj_pos_set(msnm_disp_state.objs[6],pp.x,pp.y - 5.0f);
      }
      if (msnm_disp_state.objs[6] != -1 && msnm_work->state != 10 &&
          msnm_work->state > 3) {
        aet_obj_stop(msnm_disp_state.objs[6]);
        msnm_disp_state.objs[6] = -1;
        aet_param_init(&plate);
        aet_param_scene_set(&plate,0x30002);
        aet_param_layer_set(&plate,e_chlg_map_plate_0c2a1d24_names[branch]);
        aet_param_depth_range_set(&plate,0.02,0.02);
        aet_param_flags_set(&plate,0x18);
        aet_param_pos_set(&plate,pp.x,pp.y);
        if (msnm_disp_state.hidden == 0) {
          msnm_disp_state.objs[6] = aet_obj_start(&plate);
        }
      }
      if (sel == 0 && msnm_disp_state.objs[6] != -1 && msnm_disp_state.stills[42] == -1 &&
          msnm_work->state != 10 &&
          ((msnm_work->state == 1 && aet_obj_end_reached(msnm_disp_state.objs[6]) != 0 &&
            aet_obj_end_reached(msnm_disp_state.objs[26]) != 0) ||
           (msnm_work->state == 2 && aet_obj_end_reached(msnm_disp_state.objs[6]) != 0) ||
           msnm_work->state > 3)) {
        aet_still_param_init(&rank);
        aet_still_param_scene_set(&rank,0x30004);
        aet_still_param_layer_set(&rank,e_chlg_map_rank_names[msnm_work->grade]);
        aet_still_param_depth_range_set(&rank,0.01,0.01);
        aet_still_param_pos_set(&rank,rp.x,rp.y);
        if (msnm_disp_state.hidden == 0) {
          msnm_disp_state.stills[42] = aet_still_start(&rank);
        }
      }
      if (stage == 0) {
        if (msnm_disp_state.objs[28] == -1) {
          aet_param_init(&plate);
          aet_param_scene_set(&plate,0x30002);
          aet_param_layer_set(&plate,"MAP_HIKARI");
          aet_param_depth_range_set(&plate,0.01,0.01);
          aet_param_color_set(&plate,chlg_branch_colors[0]);
          msnm_disp_state.objs[28] = aet_obj_start(&plate);
        }
        if (msnm_disp_state.objs[29] == -1) {
          aet_param_init(&plate);
          aet_param_scene_set(&plate,0x30002);
          aet_param_layer_set(&plate,"MAP_CROSS_V");
          aet_param_depth_range_set(&plate,0.03,0.03);
          aet_param_color_set(&plate,chlg_branch_colors[0]);
          msnm_disp_state.objs[29] = aet_obj_start(&plate);
        }
        if (msnm_disp_state.objs[30] == -1) {
          aet_param_scene_set(&plate,0x30002);
          aet_param_layer_set(&plate,"MAP_CROSS_H");
          aet_param_depth_range_set(&plate,0.02,0.02);
          aet_param_color_set(&plate,chlg_branch_colors[0]);
          msnm_disp_state.objs[30] = aet_obj_start(&plate);
        }
      }
      if (msnm_disp_state.plate_wait > 0) {
        msnm_disp_state.plate_wait = msnm_disp_state.plate_wait - 1;
      }
    }
  }
}

void msn_panel_draw(char side, msnm_try_rec *mission, msnm_try_setting_rec *setting, int status, int where)
{
  uchar during_fight;
  int branch;
  int stage;
  char *text;
  rob_rec *rob;
  uint color;
  msn_count_place_rec *place;
  int place_side;
  float place_y;
  float place_x;
  float bob;
  float fx;
  float fy;
  int o = 0x80;
  char *pb;

  sprite_colors_reset();
  place = &msn_count_places[where];
  stage = msnm_row_get();
  branch = msnm_column_get();
  place_side = side;
  if (place->by_side == 0) place_side = 0;
  { float *f = place->pos[place_side];
  place_x = *f++;
  place_y = *f; }
  during_fight = 1; if (where != 1) during_fight = 0;
  text = msn_text_build(mission,setting,status,where);
  if (text == 0) {
    return;
  }
  if (where == 0 || where == 2) {
    msn_text_mirror_set(0);
    if (msnm_work->side == '\x01') {
      msn_text_mirror_set(1);
    }
    msn_text_align_set(3);
    msn_text_box_draw((float *)&msn_panel_box,text);
  }
  else if (where == 1) {
    rob = rob_get((uint)msnm_work->side);
    msn_text_mirror_set(0);
    if ((rob->flags & 0x20) != 0) {
      msn_text_mirror_set(1);
    }
    msn_text_align_set(1);
    msn_text_box_draw((float *)&msn_panel_box_fight,text);
  }
  msn_hint_draw(where);
  if (during_fight != 0) {
    color = 0xffffffff;
    if (status == 2) {
      color = 0xffffff00;
    }
    fx = place_x + msn_count_x_offsets[place_side];
    fy = place_y + 14.0;
    msn_count_draw(fx,fy,color,side);
    bob = (float)(msnm_disp_state.selroute_frames <= 0x1d ? msnm_disp_state.selroute_frames : 0x3c - msnm_disp_state.selroute_frames) * 0.16666667;
    msnm_disp_state.selroute_frames = msnm_disp_state.selroute_frames + 1;
    if (0x3c < msnm_disp_state.selroute_frames) {
      msnm_disp_state.selroute_frames = 0;
    }
    pb = (char *)((int *)&msnm_disp_state + stage);
    aet_still_move_to((float)stage * 27.0 + 202.0,449.0 - bob,*(int *)(pb + o));
  }
  if (where != 2) {
    msn_panel_base_create(during_fight,branch);
  }
  return;
}

void msn_panel_base_create(uchar during_fight, int branch)
{
  float y;
  float x;
  aet_still_param buf;
  
  if (msnm_disp_state.stills[25] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_color_set(&buf,chlg_branch_colors[branch]);
    if (during_fight != 0) {
      aet_still_param_filter_set(&buf,0);
      aet_still_param_layer_set(&buf,"MATCH_BASE");
    }
    else {
      aet_still_param_layer_set(&buf,"ROUTE_MAP_BASE");
    }
    aet_still_param_depth_range_set(&buf,0.05,0.05);
    msnm_disp_state.stills[25] = aet_still_start(&buf);
  }
  if (msnm_disp_state.stills[28] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_color_set(&buf,chlg_branch_colors[branch]);
    aet_still_param_depth_range_set(&buf,0.04,0.04);
    if (during_fight != 0) {
      aet_still_param_layer_set(&buf,"PARTS_ROUTE_SMALL");
      x = 210.0;
      y = 397.0;
    }
    else {
      aet_still_param_layer_set(&buf,"PARTS_ROUTE_BIG");
      x = 180.0;
      y = 283.0;
    }
    aet_still_param_pos_set(&buf,x,y);
    msnm_disp_state.stills[28] = aet_still_start(&buf);
  }
}

void chlg_route_time_draw(hide_digits) uchar hide_digits;
{
  int seconds;
  aet_still_param buf;
  seconds = msnm_work->menu_timer / 60;
  if (hide_digits != 0) seconds = 0;
  if (msnm_work->state - 4U < 2) {
    if (msnm_disp_state.stills[26] == -1) {
      aet_still_param_init(&buf);
      aet_still_param_scene_set(&buf,0x30007);
      aet_still_param_layer_set(&buf,"PARTS_TIME");
      aet_still_param_depth_range_set(&buf,0.006,0.006);
      aet_still_param_alpha_set(&buf,0.7);
      aet_still_param_pos_set(&buf,170.0,220.0);
      msnm_disp_state.stills[26] = aet_still_start(&buf);
    }
    if (seconds < 5) {
      if ((msnm_work->menu_timer / 4) & 1) {
        return;
      }
    }
    chlg_number_draw(seconds,186.0,240.0,0.006,0,0);
  }
}

void chlg_progress_bar_draw(int side, msnm_course_results_rec *route)
{
  int stage;
  int branch;
  int result;
  char *layer;
  int i;
  float dx;
  aet_still_param buf;
  
  stage = msnm_row_get();
  branch = msnm_column_get();
  if (msnm_disp_state.stills[0] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_layer_set(&buf,"PARTS_ITA_GLAY");
    aet_still_param_depth_range_set(&buf,0.02,0.02);
    aet_still_param_pos_set(&buf,200.0,453.0);
    msnm_disp_state.stills[0] = aet_still_start(&buf);
  }
  for (i = 0; i < 8; i++) {
    if ((route->kinds[i] == -1 && stage != i) &&
       msnm_disp_state.stills[i + 17] == -1) {
      switch(i) {
      case 1:
        dx = 23.0;
        break;
      case 2:
      case 3:
        dx = 25.0;
        break;
      case 4:
      case 5:
      case 6:
        dx = 26.0;
        break;
      case 7:
        dx = 26.2;
        break;
      default:
        dx = 26.0;
      }
      aet_still_param_init(&buf);
      aet_still_param_scene_set(&buf,0x30004);
      aet_still_param_layer_set(&buf,e_chlg_map_suji_names[i]);
      aet_still_param_depth_range_set(&buf,0.01,0.01);
      aet_still_param_alpha_set(&buf,0.5);
      aet_still_param_pos_set(&buf,(float)i * dx + 215.0,455.0);
      msnm_disp_state.stills[i + 17] = aet_still_start(&buf);
    }
  }
  for (i = 0; i < 8; i++) {
    layer = "NULL";
    if (msnm_disp_state.stills[i + 1] == -1) {
      result = route->kinds[i];
      if (result != -1) {
        if (result >= -1) {
          if (result < 2) {
            layer = e_parts_ita_lv_names[route->columns[i]];
          }
        }
      }
      if (memcmp(layer,"NULL",5) == 0) break;
      if (msnm_disp_state.stills[i + 1] == -1) {
        aet_still_param_init(&buf);
        aet_still_param_scene_set(&buf,0x30004);
        aet_still_param_layer_set(&buf,layer);
        aet_still_param_depth_range_set(&buf,0.009,0.009);
        aet_still_param_pos_set(&buf,(float)i * 27.0 + 202.0,449.0);
        msnm_disp_state.stills[i + 1] = aet_still_start(&buf);
      }
      if (msnm_disp_state.stills[i + 9] == -1) {
        dx = part_width(route->grades[i]);
        aet_still_param_init(&buf);
        aet_still_param_scene_set(&buf,0x30004);
        aet_still_param_layer_set(&buf,e_parts_names[route->grades[i]]);
        aet_still_param_depth_range_set(&buf,0.008,0.008);
        aet_still_param_pos_set(&buf,(float)i * 27.0 + 210.0 + dx,447.0);
        msnm_disp_state.stills[i + 9] = aet_still_start(&buf);
      }
    }
  }
  if (msnm_disp_state.stills[stage + 1] == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_layer_set(&buf,e_parts_ita_lv_names[branch]);
    aet_still_param_depth_range_set(&buf,0.009,0.009);
    aet_still_param_pos_set(&buf,(float)branch * 28.0 + 202.0,449.0);
    msnm_disp_state.stills[stage + 1] = aet_still_start(&buf);
  }
  return;
}

void chlg_total_score_draw(msnm_course_results_rec *route, uint stage_score, uint stage_rank)
{
  typedef struct { float x; float y; } pos;
  int i;
  int j;
  uint v;
  uint w;
  uint sum;
  uchar n;
  uchar k;
  uchar c;
  float pulse;
  float t0;
  float x;
  float y;
  float off;
  char *name;
  chlg_chara_icon_rec *icon;
  char buf[64];
  aet_obj_param res;

  if (msnm_disp_state.objs[23] == -1) {
    aet_param_init(&res);
    aet_param_scene_set(&res,0x30006);
    aet_param_flags_set(&res,4);
    aet_param_layer_set(&res,"TOTAL_STAGE_SCORE_RANK");
    aet_param_depth_range_set(&res,0.01,0.01);
    aet_param_alpha_set(&res,0.7);
    msnm_disp_state.objs[23] = aet_obj_start(&res);
  }
  if (aet_obj_end_reached(msnm_disp_state.objs[23]) == 0) return;
  sum = 0;
  t0 = ((float)msnm_work->menu_timer * 6.2831855) / 60.0;
  pulse = sin_radians(t0 + t0) * 0.5 + 0.5;
  for (i = 0; i < 3; i++) {
    pos *t = &((pos *)&chlg_total_columns)[i];
    for (j = 0; j < 8; j++) {
      buf[0] = 0;
      x = t->x;
      y = (float)j * 22.0 + t->y;
      switch (i) {
      case 0:
        chlg_number_draw(j + 1,x,y,0.006,0,0);
        break;
      case 1:
        if (j == msnm_work->row) {
          v = stage_score;
        }
        else {
          v = route->scores[j];
        }
        chlg_number_draw(v,x,y,0.006,0,0);
        sum += v;
        break;
      case 2:
        if (j == msnm_work->row) {
          w = stage_rank;
        }
        else {
          w = route->grades[j];
        }
        if (w == 0xffffffff) w = 0;
        off = 0.0;
        if ((int)w <= 2) {
          n = 1;
          k = w;
        }
        else {
          n = w - 1;
          k = 2;
        }
        for (c = 0; c < n; c++) {
          int *marks = chlg_rank_mark_sprites;
          sprite_put_xyz_alpha(marks[k],x + off,y,0.006,0.7);
          off += 16.0;
        }
        break;
      }
      x = t->x;
      y = t->y + (float)j * 22.0;
      sprite_colors_set(0xffffffff,0);
      if (j == msnm_work->row) {
        sprite_alpha_set(pulse);
      }
      sprite_font_text_put(&font_12x16,buf,x,y,0x10,0.006);
      sprite_alpha_reset();
      if (i != 0 && j == msnm_work->row) break;
    }
  }
  x = ((pos *)&chlg_total_columns)[0].x + 200.0;
  y = ((pos *)&chlg_total_columns)[0].y + 132.0;
  sprite_colors_set(0xffffffff,0);
  if (msnm_disp_state.objs[24] == -1) {
    aet_param_init(&res);
    aet_param_scene_set(&res,0x30006);
    aet_param_flags_set(&res,4);
    aet_param_layer_set(&res,"TOTAL_TOTAL_SCORE_COLOR");
    aet_param_alpha_set(&res,0.7);
    aet_param_color_set(&res,0xffffb7);
    aet_param_depth_range_set(&res,0.01,0.01);
    msnm_disp_state.objs[24] = aet_obj_start(&res);
  }
  sprite_colors_set(0xffffffff,0xffffffff);
  sprite_put_xyz_scaled_alpha(0,x + 20.0,y + 15.0,0.09,50.0,0.1,1.0);
  x = chlg_total_columns.score_x + 460.0;
  y = *(float *)&chlg_total_columns.score_y + 132.0 - 12.0;
  if (aet_obj_end_reached(msnm_disp_state.objs[24]) != 0) {
    sprite_colors_set(0xffffffb7,0);
    chlg_number_draw(sum,x,y,0.006,1,0);
    sprite_colors_reset();
  }
  if (aet_obj_frame_get(msnm_disp_state.objs[24]) == 0x19) {
    sound_play_id(0x375,0);
  }
  if (aet_obj_end_reached(msnm_disp_state.objs[24]) != 0) {
    icon = &chlg_chara_icons[msnm_work->result.chlg.player.chara];
    sprite_put_xyz(531.0,385.0,0.006,icon->icon);
    name = msnm_work->result.chlg.player.name;
    if (*name == 0) {
      name = "NO-NAME";
    }
    sprite_text_put(&euc_text_font,255.0,385.0,0.006,1,(byte *)name);
  }
}

void chlg_node_hiscore_draw(mission_score_rec *rec, int place)
{
  float x;
  float dz;
  float y;
  char *name;
  chlg_chara_icon_rec *icon;
  aet_obj_param buf;
  float v[4];
  
  float *t = chlg_hiscore_places[place];
  x = t[0];
  y = t[1];
  if (place == 2) {
    v[0] = x - 130.0;
    v[1] = y + 109.0;
    v[2] = x - 280.0;
    v[3] = y + 127.0;
    chlg_hiscore_icon_name_draw(rec,v);
  }
  else {
    if (msnm_disp_state.objs[22] == -1) {
      aet_param_init(&buf);
      aet_param_scene_set(&buf,0x30006);
      aet_param_layer_set(&buf,"MAP_HISCORE");
      aet_param_depth_range_set(&buf,0.02,0.02);
      aet_param_flags_set(&buf,4);
      aet_param_pos_set(&buf,0.0,35.0);
      aet_param_alpha_set(&buf,0.7);
      msnm_disp_state.objs[22] = aet_obj_start(&buf);
    }
    dz = 0.0;
    if (place == 1) {
      dz = -0.001;
    }
    chlg_number_draw(rec->score,x + 30.0,y + 31.0,dz + 0.006,0,0);
    icon = &chlg_chara_icons[rec->chara];
    sprite_put_xyz(x + 80.0,
                 (y - 12.0) + 35.0,icon->icon,dz + 0.006);
    name = rec->name;
    if (rec->name[0] == 0) {
      name = msn_noname_text;
    }
    sprite_text_put(&euc_text_font,x + 180.0,y + 33.0,dz + 0.006,1,name);
  }
  return;
}

void chlg_result_old_hiscore_draw(mission_score_rec *rec)
{
  float x;
  float y;
  float dz;
  float pos[4];
  float *place;
  
  place = chlg_hiscore_places[2];
  x = place[0] + 16.0;
  y = place[1] + 3.0;
  dz = -0.001;
  pos[0] = x + 195.0;
  pos[1] = y + 106.0;
  pos[2] = x + 65.0;
  pos[3] = y + 125.0;
  chlg_hiscore_icon_name_draw(rec,pos);
  x = x + 0.0;
  x = x + 240.0;
  y = y + 0.0;
  y = y + 135.0;
  chlg_number_draw(rec->score,x,y,dz + 0.006,0,0);
  sprite_colors_reset();
}

void chlg_hiscore_icon_name_draw(mission_score_rec *rec, float *pos)
{
  float dz;
  float x;
  float y;
  char *name;

  dz = -0.001;
  x = pos[0] + 0.0;
  y = pos[1] + 48.0;
  sprite_put_xyz(chlg_chara_icons[rec->chara].icon,
               x,y,dz + 0.006);
  x = pos[2] + 0.0;
  y = pos[3] + 32.0;
  name = rec->name;
  if (rec->name[0] == 0) {
    name = msn_noname_text;
  }
  sprite_text_put(&euc_text_font,x,y,dz + 0.006,1,name);
  return;
}

void chlg_outcome_banner_show(void)
{
  int cleared = 0;
  if (msnm_work->status == 2) cleared = 1;
  chlg_outcome_layer_create(cleared);
  return;
}

void chlg_result_init(void)
{
  chlg_result_rec *res;
  mission_score_rec *node;
  res = &msnm_work->result.chlg;
  res->state = 0;
  res->mode = 0;
  res->timer = 0;
  node = chlg_map_node_get(msnm_work->column,
                   msnm_work->row);
  res->old_hiscore = *node;
  res->new_record = 0;
}

void chlg_result_draw(void)
{
  chlg_result_rec *res;

  res = &msnm_work->result.chlg;
  chlg_result_old_hiscore_draw(&res->old_hiscore);
  msnm_ranking_record_fill(&res->player,res->tally[4].value,course_time_total,msn_noname_text,0);
  chlg_node_hiscore_draw(&res->player,2);
  chlg_result_numbers_draw();
  if (msnm_disp_state.objs[17] == -1) {
    msnm_disp_state.objs[17] = chlg_layer_from_desc(0x30006,chlg_result_layer_descs);
  }
  if (msnm_disp_state.objs[18] == -1) {
    msnm_disp_state.objs[18] = chlg_layer_from_desc(0x30006,&chlg_result_layer_descs[1]);
  }
  if (msnm_disp_state.objs[19] == -1) {
    msnm_disp_state.objs[19] = chlg_layer_from_desc(0x30006,&chlg_result_layer_descs[2]);
  }
  if (msnm_disp_state.objs[20] == -1) {
    msnm_disp_state.objs[20] = chlg_layer_from_desc(0x30006,&chlg_result_layer_descs[3]);
  }
  sprite_colors_set(0xffffffff,0xffffffff);
  sprite_put_xyz_scaled_alpha(0,25.0,376.0,0.01,44.0,0.1,1.0);
  if (msnm_disp_state.objs[21] == -1) {
    msnm_disp_state.objs[21] = chlg_layer_from_desc(0x30006,&chlg_result_layer_descs[4]);
  }
  sprite_colors_set(0xffffffff,0xffffffff);
  sprite_put_xyz_scaled_alpha(0,400.0,376.0,0.01,30.0,0.1,1.0);
}

void chlg_result_step(void)
{
  chlg_result_rec *res;

  res = &msnm_work->result.chlg;
  switch(chlg_result_state_get()) {
  case 0:
    if (chlg_bars_state_get() == 1) chlg_result_state_set(1);
    break;
  case 1:
  case 2:
  case 3:
    chlg_result_count_up();
    break;
  case 4:
    if (--res->timer <= 0) chlg_result_end();
    break;
  }
}

int chlg_result_state_get(void)
{
  msnm_work_rec *work;
  work = msnm_work;
  return work->result.chlg.state;
}

void chlg_result_state_set(int state)
{
  msnm_work_rec *work;
  work = msnm_work;
  work->result.chlg.state = state;
  return;
}

void chlg_result_tally_finish(void)
{
  chlg_result_rec *res;
  int *dst;
  int *src;
  int score;

  res = &msnm_work->result.chlg;
  res->mode = 2;
  res->tally[0].value = res->tally[0].target;
  res->tally[2].value = res->tally[2].target;
  res->tally[3].value = res->tally[3].target;
  res->tally[4].value = res->tally[4].target;
  chlg_rank_layers_create(msnm_work->grade,0);
  chlg_result_state_set(4);
  res->timer = 300;
  msnm_ranking_record_fill(&res->player,res->tally[4].value,course_time_total,msn_noname_text,0);
  src = (int *)&res->player;
  if (res->old_hiscore.score < (uint)src[7]) {
    score = src[7];
    res->new_record = 1;
    dst = (int *)&res->old_hiscore;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = score;
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[11] = src[11];
    dst[12] = src[12];
    dst[13] = src[13];
  }
  return;
}

void chlg_result_end(void)
{
  chlg_rank_layers_free();
  chlg_result_state_set(5);
  return;
}

mission_score_rec * chlg_map_node_get(int branch, uint stage)
{
  mission_score_rec *rec;
  int index;
  mission_score_rec *tbl;

  if (!((stage < 8 && -1 < branch) && (branch <= (int)stage))) {
    rec = (mission_score_rec *)0x0;
  }
  else {
    index = chlg_map_node_index(stage,branch);
    tbl = user_back_data.challenge.chlg_map_hiscores;
    rec = &tbl[index];
  }
  return rec;
}

int chlg_map_node_index(int stage, int branch)
{
  int index;
  int n;

  index = 0;
  n = 1;
  for (; n <= stage; index = index + n, n = n + 1) {

  }
  return index + branch;
}

void msn_text_tags_expand(char *text, char *out, msnm_try_rec *mission, msnm_try_setting_rec *setting)
{
  int i;
  int j;
  int k;
  int n;
  int inside_tag;
  int start;
  int len;
  char name[32];
  char buf[36];

  inside_tag = 0;
  start = 0;
  i = 0;
  j = 0;
  for (n = 0; n < 0x100; n++) {
    out[n] = 0;
  }
  while (text[i] != 0) {
    if (j > 0xfe) {
      out[j] = 0;
      break;
    }
    if (text[i] == '<') {
      inside_tag = 1;
      start = i + 1;
    }
    if (inside_tag && text[i] == '>') {
      len = i - start;
      if (len <= 0) break;
      strncpy(name,text + start,len);
      name[len] = 0;
      for (k = 0; k < 0x13; k++) {
        if (strcmp(name,msn_text_tag_names[k]) == 0) break;
      }
      if (k != 0x13) {
        j += msn_text_tag_format(out + j,k,mission,setting);
      }
      else {
        sprintf(buf,"<%s>",name);
        strcat(out,buf);
        j += len + 2;
      }
      inside_tag = 0;
    }
    else if (!inside_tag) {
      out[j++] = text[i];
    }
    i++;
  }
  out[j] = 0;
}

int msn_text_tag_format(char *out, int tag, msnm_try_rec *mission, msnm_try_setting_rec *setting)
{
  msnm_work_rec *work = msnm_work;
  uint *progress = (uint *)&work->progress;
  int chara = ((int *)&player_chara)[work->side];
  int len;
  int para;

  msnm_work_b4_get();
  len = 0;
  switch (tag) {
  case 0:
    len = sprintf(out, "%d", mission->target_count);
    break;
  case 1:
    len = sprintf(out, "%d", mission->param);
    break;
  case 2:
    len = sprintf(out, "%d", progress[0]);
    break;
  case 3:
    len = sprintf(out, "%d", progress[1]);
    break;
  case 4:
    len = sprintf(out, "%d", progress[2]);
    break;
  case 5:
    len = sprintf(out, "%d", *(uchar *)(progress + 4));
    break;
  case 6:
    len = sprintf(out, "%d", progress[5]);
    break;
  case 7:
    len = sprintf(out, "%d", progress[6]);
    break;
  case 8:
    len = sprintf(out, "%d", *(uchar *)(progress + 7));
    break;
  case 9:
    len = sprintf(out, "%d", progress[8]);
    break;
  case 10:
    len = sprintf(out, "%d", msnm_bits9_count(progress[9]));
    break;
  case 11:
    para = mission->param;
    len = sprintf(out, "%s", msn_combo_cmd_get(para, (char)chara));
    break;
  case 12:
    len = chara * 2;
    len = sprintf(out, "%s", msn_waza_cmds[len + setting->variant]);
    break;
  case 13:
    len = sprintf(out, "%s", msn_nage_cmds[chara * 10 + setting->variant]);
    break;
  case 14:
    para = mission->param;
    len = sprintf(out, "%s", msn_hyper_cmd_get(0, (char)chara, para, setting));
    break;
  case 15:
    para = mission->param;
    len = sprintf(out, "%s", msn_hyper_cmd_get(1, (char)chara, para, setting));
    break;
  case 16:
    para = mission->param;
    len = sprintf(out, "%s", msn_hyper_cmd_get(2, (char)chara, para, setting));
    break;
  case 17:
    para = mission->param;
    if (chara == 12) {
      len = sprintf(out, "%s", msn_stance_finish_texts[para]);
    }
    break;
  case 18:
    len = sprintf(out, "%s", (&msn_stance_finish_texts[3])[chara]);
    break;
  }
  return len;
}

char * msn_text_build(msnm_try_rec *mission, msnm_try_setting_rec *setting, int status, int where)
{
  char *text;
  
  if (where == 1) {
    text = msn_texts_a[mission->type];
    if (mission->special != -1) text = msn_texts_a_special[mission->special];
  }
  else {
    text = msn_texts_b[mission->type];
    if (mission->special != -1) text = msn_texts_b_special[mission->special];
  }
  msn_text_tags_expand(text,msnm_work->text,mission,setting);
  return msnm_work->text;
}

char * msn_combo_cmd_get(int list, char chara)
{
  if (list == 0) {
    return msn_combo_cmds_0[chara];
  }
  if (list == 1) {
    return msn_combo_cmds_1[chara];
  }
  if (list == 2) {
    return msn_combo_cmds_2[chara];
  }
  return 0;
}

char * msn_hyper_cmd_get(int slot, int chara_no, int para, msnm_try_setting_rec *setting)
{
  int chara;
  int kind;
  int idx;
  char **tbl;

  chara = (char)chara_no;
  kind = setting->hyper_kinds[slot];
  idx = setting->hyper_entries[slot];
  if (chara == 0xc && para == 0) {
    tbl = msn_hyper_cmds_stance[kind];
  }
  else {
    tbl = msn_hyper_cmds_0[chara];
    if (kind == 0) return tbl[idx];
    if (kind == 2) {
      tbl = msn_hyper_cmds_2[chara];
    }
    else if (kind == 1) {
      tbl = msn_hyper_cmds_1[chara];
    }
  }
  return tbl[idx];
}

void msn_count_draw(float x, float y, uint color, uchar side)
{
  int target;
  int progress;
  int len;
  float cx;
  float off;
  float px;
  char buf[64];
  aet_still_param spr;

  target = msnm_work->cur_try.target_count;
  progress = msnm_work->progress.cnt_num;
  len = sprintf(buf,"/%d",target);
  cx = x + (float)len * 16.0;
  sprite_colors_set(color,0);
  chlg_number_draw(progress,cx - (float)len * 16.0,y - 1.0,0.006,1,0x11);
  sprite_colors_set(0xffffffff,0);
  if (target >= 10) off = -32.0;
  else off = -16.0;
  sprite_put_xyz_alpha(0x4c9f,cx + off,y + 1.0,0.006,1.0);
  chlg_number_draw(target,cx,y + 6.0,0.006,0,0x11);
  if (msnm_disp_state.flash_frames == 0 && (msnm_disp_state.flags & 8) != 0
      && msnm_disp_state.stills[27] != -1) {
    aet_still_stop(msnm_disp_state.stills[27]);
    msnm_disp_state.stills[27] = -1;
    msnm_disp_state.flags = msnm_disp_state.flags & 0xfffffff7 | 0x10;
  }
  if ((msnm_disp_state.flags & 8) != 0 && 0 < (int)msnm_disp_state.flash_frames) {
    msnm_disp_state.flash_frames = msnm_disp_state.flash_frames - 1;
  }
  if ((msnm_disp_state.flags & 8) != 0 && msnm_disp_state.stills[27] == -1) {
    aet_still_param_init(&spr);
    aet_still_param_scene_set(&spr,0x30004);
    aet_still_param_layer_set(&spr,"MATCH_BASE_FLASH");
    aet_still_param_depth_range_set(&spr,0.006,0.006);
    msnm_disp_state.stills[27] = aet_still_start(&spr);
    msnm_disp_state.flash_frames = 0xc;
    sound_play_id(0x36b,0);
  }
  if (progress == target && (msnm_disp_state.flags & 0x10) == 0) {
    msnm_disp_state.flags = msnm_disp_state.flags | 8;
  }
  if ((progress >= target || (msnm_disp_state.flags & 8) != 0) && msnm_disp_state.stills[45] == -1) {
    px = 10.0;
    if (side != 0) px = 532.0;
    aet_still_param_init(&spr);
    aet_still_param_scene_set(&spr,0x30004);
    aet_still_param_layer_set(&spr,"PARTS_SUCCESS");
    aet_still_param_depth_range_set(&spr,0.006,0.006);
    aet_still_param_pos_set(&spr,px,379.0);
    msnm_disp_state.stills[45] = aet_still_start(&spr);
  }
}

void msn_hint_draw(int where)
{
  typedef struct { float x; float y; float width; float height; float pad_x; int rest[6]; } box;
  uint col;
  char *text;
  msnm_try_rec *mission;
  float text_width;
  box hint_box;
  
  if (where == 1) {
    mission = &msnm_work->cur_try;
    text = msn_hints[mission->type];
    if (mission->special != -1) {
      text = msn_hints_special[mission->special];
    }
    if (*text != '\0') {
      col = color_pulse_mix(mode_timer,0xffffff,0xff0000,1.0);
      hint_box = *(box *)&msn_hint_box;
      text_width = msn_text_width(text);
      hint_box.width = text_width + (hint_box.pad_x + hint_box.pad_x);
      if (msnm_work->side == '\0') {
        hint_box.x = (578.0 - text_width) - (hint_box.pad_x + hint_box.pad_x);
      }
      else {
        hint_box.x = 62.0;
      }
      msn_text_custom_color_set(6,col,0);
      msn_text_box_draw(&hint_box.x,text);
    }
  }
}

uint color_pulse_mix(int frame, uint color_a, uint color_b, float rate)
{
  int i;
  int sh;
  uint mask;
  float t;
  float u;
  uchar a[4];
  uchar b[4];
  uchar c[4];
  
  t = sin_radians(((float)frame * 6.2831855 / 60.0) * 2.0 * rate);
  t = t * 0.5 + 0.5;
  u = 1.0 - t;
  for (i = 0; i < 3; i++) {
    sh = i * 8;
    mask = 0xff << sh;
    a[i] = (color_a & mask) >> sh;
    b[i] = (mask & color_b) >> sh;
    c[i] = (int)(t * (float)a[i] + (float)b[i] * u);
  }
  return (uint)c[2] << 0x10 | (uint)c[1] << 8 | (uint)c[0];
}

void chlg_bars_reset(void)
{
  chlg_result_rec *res;
  res = &msnm_work->result.chlg;
  res->bar_state = 0;
  res->bar_count = 0;
}

void chlg_bars_step(void)
{
  chlg_result_rec *res;
  int count;
  int limit;
  
  res = &msnm_work->result.chlg;
  if (res->bar_state != 1) {
    res->bar_count++;
  }
  if (res->bar_state == 0) {
    count = res->bar_count;
    limit = 0x3b;
  }
  else {
    if (res->bar_state != 2) {
      return;
    }
    count = res->bar_count;
    limit = 0x77;
  }
  if (count > limit) {
    res->bar_state = 1;
    res->bar_count = limit;
  }
}

void chlg_bars_draw(void)
{
  int count;
  float height;
  float dim;
  float y;
  chlg_result_rec *res;
  res = &msnm_work->result.chlg;
  count = res->bar_count;
  if (count < 60) {
    height = ((float)count / 60.0f) * 7.5f;
    dim = ((float)count / 60.0f) * msnm_work->stage_dim_level;
  }
  else {
    height = ((float)count / 60.0f) * 7.5f;
    dim = ((float)count / 60.0f) * msnm_work->stage_dim_level;
  }
  sprite_put_xyz_scaled(0,0.0f,0.0f,0.04f,80.0f,height);
  y = 7.5f - height;
  y = y * 8.0f + 420.0f;
  sprite_put_xyz_scaled(0,0.0f,y,0.04f,80.0f,height);
  chlg_screen_dim_draw(dim);
  return;
}

void chlg_screen_dim_draw(float level)
{
  sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.09,80.0,60.0,level);
  return;
}

void chlg_bars_state_set(int state)
{
  chlg_result_rec *res = &msnm_work->result.chlg;
  res->bar_state = state;
  return;
}

int chlg_bars_state_get(void)
{
  chlg_result_rec *res = &msnm_work->result.chlg;
  return res->bar_state;
}

void chlg_result_count_up(void)
{
  chlg_result_rec *res = &msnm_work->result.chlg;

  switch (res->state) {
  case 1:
    res->mode = 0;
    if (value_step_to_limit(&res->tally[0].value, res->tally[0].target, res->step)) res->state++;
    break;
  case 2:
    res->mode = 1;
    if (value_step_to_limit(&res->tally[2].value, res->tally[2].target, res->step)) res->state++;
    break;
  case 3:
    res->mode = 2;
    if (value_step_to_limit(&res->tally[3].value, res->tally[3].target, res->step)) chlg_result_tally_finish();
    break;
  }
  value_step_to_limit(&res->tally[4].value, res->tally[4].target, res->step);
}

bool value_step_to_limit(int *value, int limit, int step)
{
  *value += step;
  if (*value > limit) {
    *value = limit;
    return 1;
  }
  return 0;
}

void chlg_result_numbers_draw(void)
{
  chlg_result_rec *res;
  int i;
  int j;
  uint value;
  int big;
  float x;
  msnm_work_rec *work;
  float *row;
  float *origin;
  float y;
  float t;
  float digits;
  uint seconds;
  uint hundredths;
  uint c;

  value = 0;
  big = 0;
  res = &msnm_work->result.chlg;
  for (i = 0; i < 4; i++) {
    row = chlg_result_row_offsets[i];
    origin = chlg_result_numbers_origin;
    x = origin[0] + row[0];
    y = origin[1] + row[1];
    for (j = 0; j < 4; j++) {
      if ((j > res->mode) & (j != 3)) continue;
      if (i == 1) {
        if (j == 1 && msnm_work->result.chlg.tally[1].value != 0) {
          t = (float)msnm_work->result.chlg.tally[1].value / 60.0;
          seconds = (int)t;
          hundredths = (int)((t - (float)(int)seconds) * 100.0);
          c = msnm_work->result.chlg.tally[1].target;
          digits = 5.0;
          if ((int)hundredths > 9) digits = 6.0;
          chlg_number_draw(seconds,x - digits * 16.0,y,0.006,0,0x10);
          chlg_number_draw(hundredths,x - 48.0,y,0.006,0,0xe);
          digits = 2.0;
          if ((int)c > 9) digits = 1.0;
          chlg_number_draw(c,x - digits * 16.0,y,0.006,0,0);
        }
      }
      else if (i == 2) {
        if (j == 1 && (work = msnm_work, work->result.chlg.tally[2].value) != 0)
          sprite_put_xyz_alpha(0x4cb0,x,y,0.006,0.7);
      }
      else if (i != 0) {
        switch (j) {
        case 0:
          work = msnm_work;
          value = work->result.chlg.tally[0].value;
          big = 0;
          break;
        case 1:
          work = msnm_work;
          value = work->result.chlg.tally[2].value;
          big = 0;
          break;
        case 2:
          work = msnm_work;
          value = work->result.chlg.tally[3].value;
          big = 0;
          break;
        case 3:
          work = msnm_work;
          value = work->result.chlg.tally[4].value;
          big = 1;
          x = x - 10.0;
          y = chlg_result_numbers_origin[1] + 80.0 + 15.0;
          sprite_colors_set(-0x49,0);
          break;
        }
        chlg_number_draw(value,x,y,0.006,big,0);
        sprite_colors_reset();
      }
      y = y + 18.0;
    }
  }
}

void chlg_rank_layers_create(int rank, uchar total)
{
  char *base_layer;
  char *rank_layer;
  aet_obj_param buf;
  
  if (total) {
    rank_layer = e_total_rank_names[rank];
    base_layer = "TOTAL_TOTAL_RANK";
    msnm_round_sound_cmds();
    sound_play_id((int)*(short *)&chlg_total_rank_sounds[rank],0);
  }
  else {
    rank_layer = e_result_rank_names[rank];
    base_layer = "RESULT_RANK";
  }
  if (msnm_disp_state.objs[25] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30006);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,base_layer);
    aet_param_depth_range_set(&buf,0.009,0.009);
    aet_param_alpha_set(&buf,0.7);
    msnm_disp_state.objs[25] = aet_obj_start(&buf);
  }
  if (msnm_disp_state.objs[26] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30006);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,rank_layer);
    aet_param_depth_range_set(&buf,0.009,0.009);
    msnm_disp_state.objs[26] = aet_obj_start(&buf);
  }
  return;
}

void chlg_selroute_layer_create(void)
{
  aet_obj_param buf;
  
  if (msnm_disp_state.objs[27] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30006);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,"MAP_SELROUTE");
    aet_param_depth_range_set(&buf,0.006,0.006);
    msnm_disp_state.objs[27] = aet_obj_start(&buf);
    aet_param_alpha_set(&buf,0.7);
    msnm_disp_state.selroute_frames = aet_obj_end_frame_get(msnm_disp_state.objs[27]);
  }
  return;
}

int chlg_selroute_layer_done(void)
{
  int r;
  r = 0;
  msnm_disp_state.selroute_frames = msnm_disp_state.selroute_frames - 1;
  if (msnm_disp_state.objs[27] == -1) r = 1;
  if (msnm_disp_state.selroute_frames < 0) r = 1;
  return r;
}

void chlg_rank_layers_free(void)
{
  aet_obj_stop(msnm_disp_state.objs[25]);
  msnm_disp_state.objs[25] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[26]);
  msnm_disp_state.objs[26] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[23]);
  msnm_disp_state.objs[25] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[24]);
  msnm_disp_state.objs[25] = 0xffffffff;
  return;
}

void chlg_map_layers_release(void)
{
  uchar i;
  
  aet_objs_stop_by_scene(0x30002);
  aet_objs_stop_by_scene(0x30006);
  aet_stills_stop_by_scene(0x30004);
  aet_stills_stop_by_scene(0x30007);
  aet_stills_stop_by_scene(0x30006);
  msnm_disp_state.objs[0] = -1;
  msnm_disp_state.objs[6] = -1;
  msnm_disp_state.stills[42] = -1;
  for (i = 0; i < 8; i++) {
    msnm_disp_state.objs[i + 7] = -1;
    msnm_disp_state.stills[i + 34] = -1;
  }
  msnm_disp_state.stills[25] = 0xffffffff;
  msnm_disp_state.stills[28] = 0xffffffff;
  msnm_disp_state.stills[29] = 0xffffffff;
  msnm_disp_state.stills[30] = 0xffffffff;
  msnm_disp_state.stills[31] = 0xffffffff;
  msnm_disp_state.stills[46] = 0xffffffff;
  msnm_disp_state.objs[22] = 0xffffffff;
}

void chlg_route_preview_free(int keep)
{
  if (((uchar)keep == '\0') ||
     (msnm_work->choice != msnm_disp_state.sel)) {
    if ((msnm_disp_state.flags & 1) != 0) {
      msnm_disp_state.flags = msnm_disp_state.flags & 0xfffffffe;
    }
    aet_still_stop(msnm_disp_state.stills[25]);
    msnm_disp_state.stills[25] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[28]);
    msnm_disp_state.stills[28] = 0xffffffff;
    aet_obj_stop(msnm_disp_state.objs[15]);
    msnm_disp_state.objs[15] = 0xffffffff;
    aet_obj_stop(msnm_disp_state.objs[16]);
    msnm_disp_state.objs[16] = 0xffffffff;
    aet_obj_stop(msnm_disp_state.objs[1]);
    msnm_disp_state.objs[1] = 0xffffffff;
    aet_obj_stop(msnm_disp_state.objs[2]);
    msnm_disp_state.objs[2] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[29]);
    msnm_disp_state.stills[29] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[30]);
    msnm_disp_state.stills[30] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[31]);
    msnm_disp_state.stills[31] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[26]);
    msnm_disp_state.stills[26] = 0xffffffff;
    aet_still_stop(msnm_disp_state.stills[43]);
    msnm_disp_state.stills[43] = 0xffffffff;
  }
}

void chlg_panel_parts_free(void)
{
  unsigned char i;
  int *p;
  aet_still_stop(msnm_disp_state.stills[25]);
  msnm_disp_state.stills[25] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[28]);
  msnm_disp_state.stills[28] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[29]);
  msnm_disp_state.stills[29] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[30]);
  msnm_disp_state.stills[30] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[31]);
  msnm_disp_state.stills[31] = 0xffffffff;
  for (i = 0; i < 25; i++) {
    aet_still_stop(msnm_disp_state.stills[i]);
    msnm_disp_state.stills[i] = 0xffffffff;
  }
  aet_still_stop(msnm_disp_state.stills[45]);
  msnm_disp_state.stills[45] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[27]);
  msnm_disp_state.stills[27] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[47]);
  msnm_disp_state.stills[47] = 0xffffffff;
  aet_still_stop(msnm_disp_state.stills[48]);
  msnm_disp_state.stills[48] = 0xffffffff;
}

void chlg_result_layers_free(void)
{
  aet_obj_stop(msnm_disp_state.objs[17]);
  msnm_disp_state.objs[17] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[18]);
  msnm_disp_state.objs[18] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[19]);
  msnm_disp_state.objs[19] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[20]);
  msnm_disp_state.objs[20] = 0xffffffff;
  aet_obj_stop(msnm_disp_state.objs[21]);
  msnm_disp_state.objs[21] = 0xffffffff;
  return;
}

void chlg_map_disp_free_all(void)
{
  chlg_map_layers_release();
  chlg_route_preview_free(0);
  chlg_panel_parts_free();
  chlg_result_layers_free();
  chlg_outcome_layer_free();
  chlg_rank_layers_free();
  return;
}

void chlg_outcome_layer_create(uint cleared)
{
  aet_obj_param buf;
  char *names [2];
  uchar n = cleared;
  
  names[0] = "OUT_FAIL";
  names[1] = "CHLG_SUCCESS";
  if (msnm_disp_state.objs[4] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30003);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,names[n]);
    aet_param_depth_range_set(&buf,0.01,0.01);
    msnm_disp_state.objs[4] = aet_obj_start(&buf);
    if (n == 1) {
      sound_play_id(0x369,0);
    }
  }
  return;
}

void chlg_outcome_layer_free(void)
{
  aet_obj_stop(msnm_disp_state.objs[4]);
  msnm_disp_state.objs[4] = 0xffffffff;
  return;
}

void chlg_round_lamp_show(int slot, float x, float y)
{
  int *base;
  int *handle;
  aet_still_param buf;
  
  base = &msnm_disp_state.objs[(char)slot];
  handle = &((msnm_disp_rec *)base)->stills[47];
  if (*handle == -1) {
    aet_still_param_init(&buf);
    aet_still_param_scene_set(&buf,0x30004);
    aet_still_param_layer_set(&buf,"PARTS_KURO_LAMP");
    aet_still_param_depth_range_set(&buf,0.04,0.04);
    aet_still_param_pos_set(&buf,x - 8.0,y - 8.0);
    *handle = aet_still_start(&buf);
  }
}

void chlg_coin_prize_start(int prize)
{
  chlg_result_rec *res;
  chlg_layer_desc desc;
  chlg_layer_desc *coins;
  int i;
  int n;
  float fx;
  float fy;
  int layer;
  int offset;
  chlg_layer_desc *coin_base;

  res = &msnm_work->result.chlg;
  for (i = 0; i < 10; i++) {
    res->coin_layers[i] = -1;
  }
  n = 0;
  if (prize != -1) {
    if (prize == 0) {
      chlg_layer_from_desc(0x30000, &chlg_coin_layer_descs[8]);
    }
    else {
      offset = prize * 28;
      coins = &chlg_coin_layer_descs[4];
      coin_base = coins - 1;
      layer = chlg_layer_from_desc(0x30000, (chlg_layer_desc *)((char *)coin_base + offset));
      res->coin_layers[n] = layer;
      n++;
      fx = coins[prize - 1].x;
      fy = coins[prize - 1].y;
      for (i = 0; i < 4; i++) {
        desc = chlg_coin_layer_descs[i];
        if (i != 0) {
          desc.x = fx;
          desc.y = fy;
        }
        layer = chlg_layer_from_desc(0x30000, &desc);
        if (i == 3) {
          msnm_disp_state.coin_frames = aet_obj_end_frame_get(layer);
        }
        res->coin_layers[n] = layer;
        n++;
      }
      for (i = 0; i < prize - 1; i++) {
        desc = (&chlg_coin_layer_descs[4])[i];
        desc.flags = 8;
        layer = chlg_layer_from_desc(0x30000, &desc);
        res->coin_layers[n] = layer;
        n++;
      }
      sound_play_id(0x36c, 0);
    }
  }
}

void chlg_coin_prize_step(int prize, byte with_box, int start_time)
{
  int layer;
  int i;
  msnm_work_rec *work;
  
  if (1 < prize + 1U) {
    if (prize == 4 && (work = msnm_work, work->menu_timer == start_time)) {
      if (with_box != 0) {
        for (i = 0; i < 3; i++) {
          layer = chlg_layer_from_desc(0x30000,&chlg_coin_layer_descs[9 + i]);
          if (i == 0) {
            msnm_disp_state.box_frames = aet_obj_end_frame_get(layer);
          }
        }
        sound_play_id(0x36f,0);
      }
      else {
        chlg_layer_from_desc(0x30000,&chlg_coin_layer_descs[8]);
      }
    }
    msnm_disp_state.coin_frames = msnm_disp_state.coin_frames + -1;
    if (msnm_disp_state.coin_frames == 0) {
      sound_play_id(0x36d,0);
    }
    work = msnm_work;
    if ((work->menu_timer < start_time) &&
       (msnm_disp_state.box_frames = msnm_disp_state.box_frames + -1,
       msnm_disp_state.box_frames == chlg_box_stop_frame)) {
      for (i = 0; i < 10; i++) {
        if (msnm_work->result.chlg.coin_layers[i] != -1) {
          aet_obj_stop(msnm_work->result.chlg.coin_layers[i]);
        }
      }
    }
  }
}

void chlg_coin_get_text_draw(int prize)
{
  uint color;
  msnm_work_rec *work;
  
  if (prize - 1U < 3) {
    work = msnm_work;
    color = color_pulse_mix(work->menu_timer,0xffffff,0xffff00,1.0);
    sprite_colors_set(color,0);
    sprite_font_text_put(&font_16x24,"C O I N   G E T !",chlg_coin_get_xy.x,
                 chlg_coin_get_xy.y,0,0.006);
    sprite_colors_reset();
  }
}

void chlg_coin_layers_release(void)
{
  aet_objs_stop_by_scene(0x30000);
  return;
}

int chlg_layer_from_desc(int group, chlg_layer_desc *desc)
{
  aet_obj_param buf;

  aet_param_init(&buf);
  aet_param_scene_set(&buf, group);
  aet_param_layer_set(&buf, desc->layer);
  aet_param_flags_set(&buf, desc->flags);
  aet_param_depth_range_set(&buf, desc->depth, desc->depth);
  aet_param_pos_set(&buf, desc->x, desc->y);
  aet_param_alpha_set(&buf, desc->alpha);
  if (desc->color != 0xffffff) {
    aet_param_color_set(&buf, desc->color);
  }
  return aet_obj_start(&buf);
}

void chlg_number_draw(uint value, float x0, float y0, float depth, int large, uint prefix_no)
{
  float z = depth;
  uchar big = large;
  uchar prefix = prefix_no;
  int negative = 0;
  float w = 16.0;
  float x = x0;
  float y = y0;
  int n;
  int digits[5];
  volatile int sprites[5];
  float alpha;
  
  n = value;
  digits[4] = n / 10000;
  digits[3] = n / 1000 % 10;
  digits[2] = n / 100 % 10;
  digits[1] = n / 10 % 10;
  digits[0] = n % 10;
  if (n < 0) negative = 1;
  if (big != 0) w = 24.0;
  alpha = 0.7;
  if (prefix == 0x11) alpha = 1.0;
  if ((uchar)(prefix - 1) < 0x10) {
    if (big != 0) sprite_put_xyz_alpha(chlg_digits_large[prefix],x,y,z,alpha);
    else sprite_put_xyz_alpha(chlg_digits_small[prefix],x,y,z,alpha);
    x = x - w;
  }
  if (big != 0) sprites[0] = chlg_digits_large[digits[0]];
  else sprites[0] = chlg_digits_small[digits[0]];
  sprite_put_xyz_alpha(sprites[0],x,y,z,alpha);
  x = x - w;
  if (digits[1] != 0 || ((digits[4] != 0 || digits[3] != 0 || digits[2] != 0) && digits[1] == 0)) {
    if (big != 0) sprites[1] = chlg_digits_large[digits[1]];
    else sprites[1] = chlg_digits_small[digits[1]];
    sprite_put_xyz_alpha(sprites[1],x,y,z,alpha);
    x = x - w;
  }
  if (digits[2] != 0 || ((digits[4] != 0 || digits[3] != 0) && digits[2] == 0)) {
    if (big != 0) sprites[2] = chlg_digits_large[digits[2]];
    else sprites[2] = chlg_digits_small[digits[2]];
    sprite_put_xyz_alpha(sprites[2],x,y,z,alpha);
    x = x - w;
  }
  if (digits[3] != 0 || digits[4] != 0) {
    if (big != 0) sprites[3] = chlg_digits_large[digits[3]];
    else sprites[3] = chlg_digits_small[digits[3]];
    sprite_put_xyz_alpha(sprites[3],x,y,z,alpha);
    x = x - w;
  }
  if (digits[4] != 0) {
    if (big != 0) sprites[4] = chlg_digits_large[digits[4]];
    else sprites[4] = chlg_digits_small[digits[4]];
    sprite_put_xyz_alpha(sprites[4],x,y,z,alpha);
    x = x - w;
  }
  if (negative != 0) {
    sprite_put_xyz_alpha(big != 0 ? 0x4c9e : 0x4cb2,x,y,z,alpha);
  }
}

float chlg_rank_part_offset(uint rank, uchar small)
{
  float offset = 0.0f;
  switch(rank) {
  case 0:
  case 1:
  case 2:
    if (small == 0) offset = 20.0f; else offset = 5.0f;
    break;
  case 3:
    if (small == 0) offset = 14.0f; else offset = 2.0f;
    break;
  case 4:
    if (small == 0) offset = 10.0f; else offset = 0.0f;
    break;
  }
  return offset;
}

void chlg_map_layout_update(uchar raised)
{
  typedef struct { float x; float y; } v2;
  uchar i;
  v2 p;
  v2 q = {58.0, 120.0};
  float cw = 50.0;
  float ch = 17.0;
  float y0;
  msnm_course_results_rec *route = &msnm_work->course;
  int stage;
  int branch;
  float w;
  float w0;
  float x;
  float px;
  float dy;
  v2 r;
  int *branch_p;

  y0 = 0.0;
  if (raised != 0) {
    y0 = -40.0;
  }
  stage = msnm_row_get();
  branch = msnm_column_get();
  w0 = chlg_rank_part_offset(msnm_work->grade,0);
  p.x = (float)stage * cw + (float)branch * ch;
  p.y = (float)branch * ch;
  r = q;
  r.x = (float)stage * cw + r.x + (float)branch * ch + w0;
  r.y = ((float)branch * ch + r.y) - 4.0;
  aet_obj_pos_set(msnm_disp_state.objs[0],0.0,y0);
  aet_obj_pos_set(msnm_disp_state.objs[6],p.x,y0 + p.y);
  aet_still_move_to(msnm_disp_state.stills[42],r.x,y0 + r.y);
  for (i = 0; i < 8; i++) {
    branch_p = &route->columns[i];
    if (*branch_p != -1) {
      int *objs;
      float rx;
      stage = i;
      branch = *branch_p;
      x = (float)stage * cw;
      dy = (float)branch * ch;
      px = x + dy;
      p.x = px;
      p.y = dy;
      rx = x + q.x + dy;
      r.y = (dy + q.y) - 4.0; /* redundant, but game code: r.y is stored again before it is read, but the game performs this store (its frame keeps the slot). */
      w = chlg_rank_part_offset(route->grades[i],0);
      objs = &msnm_disp_state.objs[0] + i;
      {
        int *obj = &objs[7];
        aet_obj_pos_set(*obj,p.x,y0 + p.y);
      }
      r.x = rx + w;
      r.y = ((float)*branch_p * ch + q.y) - 4.0;
      aet_still_move_to(r.x,y0 + r.y,objs[0x41]);
    }
  }
}

void chlg_map_hidden_set(byte hidden)
{
  uint i;
  
  msnm_disp_state.hidden = hidden;
  if (msnm_disp_state.objs[0] != -1) {
    aet_obj_hide_set(msnm_disp_state.objs[0],hidden);
  }
  if (msnm_disp_state.objs[6] != -1) {
    aet_obj_hide_set(msnm_disp_state.objs[6],hidden);
  }
  if (msnm_disp_state.objs[15] != -1) {
    aet_obj_hide_set(msnm_disp_state.objs[15],hidden);
  }
  if (msnm_disp_state.objs[16] != -1) {
    aet_obj_hide_set(msnm_disp_state.objs[16],hidden);
  }
  if (msnm_disp_state.stills[42] != -1) {
    aet_still_hide_set(msnm_disp_state.stills[42],hidden);
  }
  i = 0;
  do {
    if (msnm_disp_state.objs[i + 7] != -1) {
      aet_obj_hide_set(msnm_disp_state.objs[i + 7],hidden);
    }
    if (msnm_disp_state.stills[i + 34] != -1) {
      aet_still_hide_set(msnm_disp_state.stills[i + 34],hidden);
    }
    i = i + 1 & 0xff;
  } while (i < 8);
  return;
}

void chlg_route_preview_reset(msnm_try_rec *next_mission, msnm_try_setting_rec *next_settings)
{
  msnm_disp_state.stills[32] = 0xffffffff;
  msnm_disp_state.stills[33] = 0xffffffff;
  msnm_disp_state.stills[44] = 0xffffffff;
  chlg_route_cursor_set(next_mission,next_settings);
  return;
}

void chlg_chara_icon_draw(int chara)
{
  float x = 485.0, y = 320.0;
  chlg_chara_icon_rec *icon = &chlg_chara_icons[chara];
  float z = 0.01;
  sprite_put_xyz(icon->icon,x,y,z);
}

void chlg_route_cursor_set(msnm_try_rec *next_mission, msnm_try_setting_rec *next_setting)
{
  int branch;
  
  branch = msnm_column_get();
  msnm_disp_state.plate = branch + msnm_work->choice;
  return;
}

void chlg_route_preview_parts_free(void)
{
  if (msnm_disp_state.stills[32] != -1) {
    aet_still_stop(msnm_disp_state.stills[32]);
  }
  if (msnm_disp_state.stills[33] != -1) {
    aet_still_stop(msnm_disp_state.stills[33]);
  }
  if (msnm_disp_state.stills[44] != -1) {
    aet_still_stop(msnm_disp_state.stills[44]);
  }
  return;
}

void chlg_coin_layer_viewer(void)
{
  int changed;
  player_input_rec *input;
  int prio;
  char *layer;
  aet_obj_param buf;
  
  text_cursor_to_cell(0x10,0x14);
  text_draw_formatted("%s",chlg_coin_layer_names[coin_viewer_name_index]);
  text_cursor_to_cell(0x10,0x15);
  text_draw_formatted("%d",coin_viewer_prio_values[coin_viewer_prio_index]);
  input = player_input_get(0);
  changed = 0;
  if ((input->pressed & 0x80) != 0) {
    changed = 1;
    coin_viewer_name_index = coin_viewer_name_index + -1;
    if (coin_viewer_name_index < 0) {
      coin_viewer_name_index = 0xb;
    }
  }
  else if ((input->pressed & 0x40) != 0) {
    changed = 1;
    coin_viewer_name_index = coin_viewer_name_index + 1;
    if (0xb < coin_viewer_name_index) {
      coin_viewer_name_index = 0;
    }
  }
  else if ((input->pressed & 0x10) != 0) {
    changed = 1;
    coin_viewer_prio_index = coin_viewer_prio_index + -1;
    if ((int)coin_viewer_prio_index < 0) {
      coin_viewer_prio_index = 6;
    }
  }
  else if ((input->pressed & 0x20) != 0) {
    coin_viewer_prio_index = coin_viewer_prio_index + 1;
    if (6 < (int)coin_viewer_prio_index) {
      coin_viewer_prio_index = 0;
    }
    changed = 1;
  }
  if (changed) {
    aet_objs_stop_by_scene(0x30000);
    layer = chlg_coin_layer_names[coin_viewer_name_index];
    prio = coin_viewer_prio_values[coin_viewer_prio_index];
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x30000);
    aet_param_layer_set(&buf,layer);
    aet_param_flags_set(&buf,prio);
    aet_param_depth_range_set(&buf,0.006,0.006);
    aet_obj_start(&buf);
  }
  return;
}
