/*
 * vf4lib/libbase/src/cns_nickname.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_xyz_scaled_alpha_angle_flags
#include "vf4ft.h"


/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char play_style; /* 0x0c2925cc */
extern static_heap_rec nick_comm_buf; /* 0x0c360018 */
extern static_heap_rec nick_game_buf; /* 0x0c3600dc */

void cns_nn_boot(void)
{
  cns_nn_state_clear();
  cns_nn_files_load();
  return;
}

void cns_nn_state_clear(void)
{
  int i;
  int j;

  for (i = 0; i < 2; i++) {
    cns_nn_state->shown[i] = 0;
  }
  cns_nn_state->shown[2] = 0;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      cns_nn_plate_clear(&cns_nn_state->plates[i]);
    }
    cns_nn_plate_clear(&cns_nn_state->plates[2]);
  }
  for (i = 0; i < 0x40; i++) {
    cns_nn_state->loaded_sprites[i] = -1;
  }
  cns_nn_state->offset_x = 0;
  cns_nn_state->offset_y = 0;
  cns_nn_state->bg_obj = -1;
  cns_nn_state->frame_count = 0;
  return;
}

void cns_nn_reset(void)
{
  cns_nn_obi_close();
  cns_nn_release();
  cns_nn_winner_hide();
  cns_nn_state_clear();
  return;
}

void cns_nn_obi_open(void)
{
  sprite_set_load(0x2c);
  return;
}

void cns_nn_obi_close(void)
{
  sprite_set_free(0x2c);
  return;
}

void cns_nn_players_show(void)
{
  int found;
  int side;
  uint shown;
  uint kind;
  uint level;

  for (side = 0; side < 2; side++) {
    shown = (uint)(byte)cns_nn_state->shown[side];
    if (shown == 0) {
      kind = shown;
      level = shown;
      found = nickname_current_get(side,&kind,&level) != 0;
      if (found && cns_nn_game_kind_valid(kind,level) != 0) {
        cns_nn_state->offset_x = 0.0f;
        cns_nn_state->offset_y = 0.0f;
        if ((int)mission_active_get() != -1 && play_style == '\x01') {
          cns_nn_state->offset_y = -20.0f;
        }
        cns_nn_plate_build(&cns_nn_state->plates[side],0,kind,level);
        cns_nn_state->shown[side] = 1;
      }
    }
  }
  return;
}

void cns_nn_release(void)
{
  cns_nn_tex_release_all();
  return;
}

void cns_nn_winner_show(void)
{
  int i;
  uint kind;
  uint level;
  char ok;

  for (i = 0; i < 2; i++) {
    kind = 0;
    level = 0;
    ok = nickname_levelup_get(i,&kind,&level) != 0;
    if (ok && cns_nn_winner_kind_valid(kind,level)) {
      cns_nn_state->offset_x = 0.0;
      cns_nn_state->offset_y = -20.0;
      sprite_set_file_load(0x2b);
      cns_nn_plate_build(&cns_nn_state->plates[2],1,kind,level);
      aet_set_load(0);
      aet_scene_textures_load(0);
      cns_nn_winner_bg_create();
      cns_nn_state->frame_count = 0;
      cns_nn_state->shown[2] = 1;
      return;
    }
  }
}

void cns_nn_winner_hide(void)
{
  if (cns_nn_state->shown[2] != '\0') {
    cns_nn_winner_bg_free();
    cns_nn_tex_release_all();
    aet_scene_textures_free(0);
    aet_set_free(0);
    cns_nn_state->shown[2] = 0;
  }
  return;
}

void cns_nn_players_draw(uchar with_parts)
{
  int i;
  cns_nn_plate_rec *plate;

  for (i = 0; i < 2; i++) {
    cns_nn_state_rec *st = cns_nn_state;
    if (st->shown[i] != '\0') {
      plate = &st->plates[i];
      cns_nn_plate_base_draw(i,plate,0);
      if (with_parts != 0) {
        cns_nn_plate_parts_draw(i,plate,0);
        cns_nn_obi_draw(i);
        cns_nn_level_draw(i,plate,0);
      }
      else {
        cns_nn_level_draw(i,plate,1);
      }
    }
  }
}

void cns_nn_winner_draw(void)
{
  int held;
  cns_nn_plate_rec *plate;

  if (cns_nn_state->shown[2] != '\0') {
    plate = &cns_nn_state->plates[2];
    cns_nn_plate_base_draw(0,plate,1);
    cns_nn_plate_parts_draw(0,plate,1);
    held = pause_frame_held();
    if (held == 0) {
      cns_nn_state->frame_count = cns_nn_state->frame_count + 1;
    }
  }
  return;
}

uchar cns_nn_winner_shown(void)
{
  return cns_nn_state->shown[2];
}

void cns_nn_files_load(void)
{
  char **name;
  perf_color_mark(0xffff);
  name = &sprite_sets[42].name;
  load_file(*name,nick_game_buf.addr,nick_game_buf.size);
  name -= 3;
  load_file(*name,nick_comm_buf.addr,nick_comm_buf.size);
  perf_color_mark(0);
  return;
}

int cns_nn_part_sprite(int which, int kind, int level, int part)
{
  int r;
  uint t;

  if (which == 1) {
    level = level + 8;
  }
  r = -1;
  switch(kind) {
  case 0:
    r = cns_nn_parts_kind0[level][part];
    break;
  case 1:
    r = cns_nn_parts_kind1[level][part];
    break;
  case 2:
    r = cns_nn_parts_kind2[level][part];
    break;
  case 3:
    r = cns_nn_parts_kind3[level][part];
    break;
  case 4:
    r = cns_nn_parts_kind4[level][part];
    break;
  case 5:
    r = cns_nn_parts_kind5[level][part];
    break;
  case 6:
    r = cns_nn_parts_kind6[level][part];
    break;
  case 7:
    if (which == 0) {
      r = cns_nn_parts_kind7[level][part];
    }
    break;
  case 8:
    r = cns_nn_parts_kind8[part];
    if (which == 1) {
      r = cns_nn_parts_kind8_winner[part];
    }
    break;
  default:
    t = kind - 9;
    if (t <= 10) {
      r = cns_nn_parts_kinds_9_19[t][part];
    }
    break;
  }
  return r;
}

cns_nn_style_rec * cns_nn_style_get(int kind, uint level)
{
  cns_nn_style_rec *r;
  uint i;
  if (kind == 7) r = &cns_nn_style_kind7;
  else if (kind == 8) r = &cns_nn_style_kind8;
  else {
    i = kind - 9;
    if (i < 0xb) r = &cns_nn_style_kinds_9_19[i];
    else r = &cns_nn_styles[level];
  }
  return r;
}

int cns_nn_game_kind_valid(uint kind, int level)
{
  if (kind > 0x13) return 0;
  if ((int)kind <= 7) {
    if (level - 1U > 6) return 0;
    if (kind == 7 && level > 4) return 0;
  }
  return 1;
}

bool cns_nn_winner_kind_valid(uint kind, int level)
{
  int r;
  int t;

  if (kind == 8) {
    r = 1;
  }
  else if (level - 1U > 6) {
    r = 0;
  }
  else {
    t = kind > 6;
    r = t == 0;
  }
  return r;
}

void cns_nn_plate_build(cns_nn_plate_rec *plate, int which, int kind, int level)
{
  int i;
  int sprite;
  int *p;

  plate->kind = kind;
  plate->level = level;
  sprite = cns_nn_kind_sprites[kind];
  plate->kind_sprite = sprite;
  cns_nn_tex_load(sprite);
  for (i = 0; i < 4; i++) {
    plate->part_sprites[i] = cns_nn_part_sprite(which,kind,level,i);
    cns_nn_tex_load(plate->part_sprites[i]);
  }
  cns_nn_style_cur = cns_nn_style_get(kind,level);
  plate->style = cns_nn_style_cur;
  if (which == 0) {
    sprite = cns_nn_frame_sprites[cns_nn_style_cur->frame];
    plate->frame_sprite = sprite;
    cns_nn_tex_load(sprite);
  }
  p = cns_nn_frame_sprites + cns_nn_style_cur->frame;
  sprite = p[4];
  plate->inner_sprite = sprite;
  cns_nn_tex_load(sprite);
  return;
}

void cns_nn_plate_clear(cns_nn_plate_rec *plate)
{
  int i;

  plate->kind = -1;
  plate->level = -1;
  plate->kind_sprite = -1;
  i = 0;
  do {
    plate->part_sprites[i] = -1;
    i = i + 1;
  } while (i < 4);
  plate->inner_sprite = -1;
  plate->frame_sprite = -1;
  return;
}

int cns_nn_tex_load(int sprite)
{
  int loaded;
  int i;
  int set;

  set = sprite / 400;
  sprite = sprite % 400;
  loaded = set * 120 + sprite;
  for (i = 0; i < 0x40; i++) {
    if (cns_nn_state->loaded_sprites[i] == loaded) {
      return 0;
    }
  }
  for (i = 0; i < 0x40; i++) {
    if (cns_nn_state->loaded_sprites[i] == -1) break;
  }
  if (i == 0x40) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/cns_nickname.c",0x408,"assertion `%s\' failed.\n"
             ,"i != CNS_NN_LOAD_TEX_MAX");
  }
  sprite_set_sprite_load(set,loaded);
  cns_nn_state->loaded_sprites[i] = loaded;
  return 1;
}

void cns_nn_tex_release_all(void)
{
  int loaded;
  int i;

  for (i = 0; i < 0x40; i++) {
    loaded = cns_nn_state->loaded_sprites[i];
    if (loaded != -1) {
      sprite_set_sprite_free(loaded / 120, loaded);
      cns_nn_state->loaded_sprites[i] = -1;
    }
  }
}

void cns_nn_plate_base_draw(int side, cns_nn_plate_rec *plate, int which)
{
  cns_nn_pos_rec *pos;
  float x;
  float y;

  if (which == 0) {
    cns_nn_pos_rec *side_pos = &cns_nn_plate_pos[side];
    x = side_pos->x;
    y = side_pos->y;
  }
  else {
    pos = &cns_nn_winner_plate_pos;
    x = pos->x;
    y = pos->y;
  }
  x += cns_nn_state->offset_x;
  y += cns_nn_state->offset_y;
  sprite_put_xyz_alpha(plate->kind_sprite,x,y,0.008,1.0);
  cns_nn_color_apply(plate->style);
  sprite_put_xyz_alpha(plate->inner_sprite,x,y,0.009,1.0);
  sprite_colors_reset();
  return;
}

void cns_nn_plate_parts_draw(int side, cns_nn_plate_rec *plate, int which)
{
  int n;
  int i;
  int none;
  float ox;
  float oy;
  int back_sprite;
  float width;
  float gap;
  float x;
  float y;
  cns_nn_pos_rec *pos;

  n = 0;
  for (i = 0; i < 4; i++) {
    none = plate->part_sprites[i] == -1;
    n = n + !none;
  }
  width = cns_nn_part_width[which];
  gap = (width * 4.0 - (float)n * width) / (float)(n + 1);
  if (which == 0) {
    pos = &cns_nn_parts_pos[side];
    x = gap + pos->x;
    y = pos->y;
    back_sprite = plate->frame_sprite;
  }
  else {
    pos = &cns_nn_winner_parts_pos;
    x = gap + pos->x;
    y = pos->y;
    back_sprite = plate->inner_sprite;
  }
  x = x + cns_nn_state->offset_x;
  y = y + cns_nn_state->offset_y;
  {
    cns_nn_pos_rec *offset = &cns_nn_part_sprite_offset[which];
    ox = offset->x;
    oy = offset->y;
  }
  for (i = 0; i < n; i++) {
    sprite_put_xyz_alpha(plate->part_sprites[i],ox + x,oy + y,0.008,1.0);
    cns_nn_color_apply(plate->style);
    sprite_put_xyz_alpha(back_sprite,x,y,0.009,1.0);
    sprite_colors_reset();
    x = x + (width + gap);
  }
  return;
}

void cns_nn_obi_draw(int side)
{
  cns_nn_pos_rec *pos = cns_nn_obi_pos;
  float x;
  float y;
  float px;
  float py;
  float oy;
  float oy2;
  x = pos[side].x + 200.0;
  if (side == 1) x = pos[1].x - 200.0;
  px = cns_nn_state->offset_x + cns_nn_obi_pos[side].x;
  oy = cns_nn_state->offset_y;
  y = cns_nn_obi_pos[side].y;
  py = oy + y;
  sprite_put_xyz_scaled_alpha_angle_flags(0x44c9, px, py, 0.05, cns_nn_obi_scale_x[side], 25.0, cns_nn_obi_angle[side], 1.0, cns_nn_obi_flags[side]);
  px = x + cns_nn_state->offset_x;
  oy2 = cns_nn_state->offset_y;
  py = oy2 + y;
  sprite_put_xyz_scaled_alpha_flags(0x44ca, px, py, 0.05, 1.0, 1.0, 1.0, cns_nn_obi_second_flags[side]);
}

void cns_nn_level_draw(int side, cns_nn_plate_rec *plate, uchar no_parts)
{
  cns_nn_pos_rec *pos;
  float x;
  float y;
  int kind;
  kind = plate->kind;
  if (((kind - 9U <= 10) | (kind == 7)) == 0) {
    pos = &cns_nn_level_pos[side];
    x = pos->x;
    y = pos->y;
    if (no_parts != 0) {
      pos = &cns_nn_level_pos_no_parts[side];
      x = pos->x;
      y = pos->y;
    }
    x = x + cns_nn_state->offset_x;
    y = y + cns_nn_state->offset_y;
    if (plate->kind == 8) {
      sprite_put_xyz_scaled_alpha_angle_flags(0x44c8,x,y,0.008,1.0,1.0,1.0,0x4000,0);
    }
    else {
      sprite_put_xyz_alpha(0x44c1,x,y,0.008,1.0);
      sprite_put_xyz_alpha(cns_nn_level_sprites[plate->level],x + 23.0,y,0.008,1.0);
    }
  }
  return;
}

void cns_nn_color_apply(cns_nn_style_rec *style)
{
  if ((style->color != 0) || (style->second_color != 0)) {
    sprite_colors_set(style->color,style->second_color);
  }
  return;
}

void cns_nn_winner_bg_create(void)
{
  aet_obj_param param;

  aet_param_init(&param);
  aet_param_scene_set(&param,0);
  aet_param_pos_set(&param,cns_nn_state->offset_x + cns_nn_winner_bg_pos.x,
               cns_nn_state->offset_y + cns_nn_winner_bg_pos.y);
  aet_param_flags_set(&param,4);
  aet_param_layer_set(&param,"SK_HAIKEI");
  aet_param_depth_range_set(&param,0.03,0.03);
  cns_nn_state->bg_obj = aet_obj_start(&param);
  sound_play_id(0x370,0);
}

void cns_nn_winner_bg_free(void)
{
  aet_objs_stop_by_scene(0);
  return;
}
