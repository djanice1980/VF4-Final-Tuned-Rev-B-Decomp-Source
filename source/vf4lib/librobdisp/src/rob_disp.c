/*
 * vf4lib/librobdisp/src/rob_disp.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_face_static_copy
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined format_02d; /* 0x0c21b3c8 */
extern float perf_render_usec; /* 0x0c292590 */
extern float perf_ta_usec; /* 0x0c292594 */
extern rob_rec *osage_rob; /* 0x0c2b100c */
extern undefined chara_item_table; /* 0x0c2dd3d8 */
extern char rob_item_flag8[2]; /* 0x0c2dd5bc */
extern morph_anim_slot morph_anim_slots[3]; /* 0x0c2dd77c */
extern float shadow_light_dir[2][3]; /* 0x0c2dd7b0 */
extern int osage_edit_coli_index[100]; /* 0x0c2ee04c */
extern osage_coli_edit_rec osage_coli_edit[8]; /* 0x0c2ee2bc */
extern int osage_edit_targets[5]; /* 0x0c2ee9e8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern undefined4 pause_flags; /* 0x0c360094 */
extern uint rob_draw_mode[2]; /* 0x0c39c434 */
extern obj_draw_handler shadow_draw_fn; /* 0x0c39c440 */
extern float shadow_height_scale; /* 0x0c39c444 */
extern obj_draw_handler shadow_volume_draw_fn; /* 0x0c39c448 */
extern osage_chain_slot osage_chains[2][64]; /* 0x0c39c450 */
extern char osage_param_text[8192]; /* 0x0c39eb24 */
extern char itmtst_log[28672]; /* 0x0c3a11e4 */
extern osage_node *osage_node_next; /* 0x0c3a8268 */
extern osage_node osage_nodes[2][100]; /* 0x0c496518 */
extern osage_node *osage_node_first; /* 0x0c498778 */
extern osage_node *osage_node_by_index[2][100]; /* 0x0c498780 */
extern int osage_chain_count; /* 0x0c498aa0 */
extern char osage_node_used[100]; /* 0x0c498aa4 */
extern zanzou_trail_rec rob_zanzou_trails[2][9]; /* 0x0c498b14 */
extern int rob_zanzou_slots[2][9][4]; /* 0x0c498e2c */
extern zanzou_trail_rec item_zanzou_trails[2][10]; /* 0x0c4a6794 */
extern zanzou_item item_zanzou_items[2][10]; /* 0x0c4a6b04 */
extern int item_zanzou_hold; /* 0x0c4c5004 */
extern osage_param osage_param_zero; /* 0x0c2dd850 */
extern item_zanzou_trail_id_pair item_zanzou_trail_ids; /* 0x0c2eea9c */
extern zanzou_fixed_point_8_vec zanzou_fixed_point_8; /* 0x0c2efa3c */
extern zanzou_fixed_point_9_vec zanzou_fixed_point_9; /* 0x0c2efa48 */
extern zanzou_fixed_dir_8_vec zanzou_fixed_dir_8; /* 0x0c2efa54 */
extern zanzou_fixed_dir_9_vec zanzou_fixed_dir_9; /* 0x0c2efa60 */
extern osage_wind_vec osage_wind; /* 0x0c2ff5bc */
extern camera_state camera; /* 0x0c370ce0 */
extern rob_usec_pair rob_chain_usec; /* 0x0c371b18 */
extern wind_rec stage_wind; /* 0x0c3ad21c */
extern int objbank_mem_sizes[21]; /* 0x0c3ba014 */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */
extern itmtst_rec itmtst_state; /* 0x0c4c52e4 */
extern itmtst_stats_rec itmtst_stats; /* 0x0c4c5304 */

static inline void clr_pair(char *p, int i, int j)
{
  ((struct { int a; int b; } *)(p + (j * 0x38 + i * 0x230)))->a = 0;
  ((struct { int a; int b; } *)(p + (j * 0x38 + i * 0x230)))->b = 0;
}

void rob_disp_init(void)
{
  return;
}

void rob_disp_exec(void)
{
  rob_rec *rob;
  int start;
  int t1;
  int i;
  float *q;
  uint *tab;
  float v;
  mtx_push(0);
  poly_modvol_bit_on();
  camera_view_load();
  if (mode_get_main() == MODE_MAIN_ADVERTISE && (demo_flags & 0x80U) != 0) {
    light_set_send(0,5);
  }
  else {
    light_set_send(0,1);
  }
  i = 0;
  tab = rob_draw_mode;
  for (; i < 2; i++) {
    rob = rob_get(i & 0xff);
    start = stopwatch_usec();
    if ((rob->flags & 1) != 0) {
      if (rob_disp == ON_OFF_ON) {
        if (disp_type > 1) {
          rob_coli_spheres_draw(rob);
        }
        else {
          if (disp_type != 0) {
            rob_coli_spheres_draw(rob);
          }
          rob_draw(rob);
          rob_items_draw(rob);
        }
      }
      if (tab[rob->side] == 0) {
        rob_shadow_draw(rob);
      }
    }
    q = &rob_draw_usec[i];
    *q = v = (float)(int)(stopwatch_usec() - start);
    rob_frame_usec[i] =
         v + rob_calc_usec[i] + ((float *)&rob_chain_usec)[i]
         + rob_cloth_usec[i];
  }
  if (mode_get_main() != MODE_MAIN_ADVERTISE || (demo_flags & 0x80U) == 0) {
    light_set_send(0,0);
  }
  mtx_pop(1);
  item_zanzou_disp();
  poly_modvol_bit_off();
}

void rob_draw(rob_rec *rob)
{
  if (((&disp_sw)[rob->side] & 1) == 0) {
    switch (rob_draw_mode[(uint)rob->side]) {
    case 1:
      rob_draw_alt_model(rob);
      break;
    case 2:
      rob_draw_single_object(rob);
      break;
    case 0:
      rob_draw_full(rob);
      break;
    default:
      rob_draw_full(rob);
      break;
    }
  }
  return;
}

void poly_or_word_bit31_toggle(int flag)
{
  uint x = poly_header_or_words.control_or;
  uint m = 0x80000000;
  if ((x & m) == m) {
    poly_header_or_words.control_or = x & 0x7fffffff;
  }
  else {
    poly_header_or_words.control_or = poly_header_or_words.control_or | 0x80000000;
  }
}

void rob_draw_full(rob_rec *rob)
{
  float m[16];
  int mtx;
  int item_flag;
  int objs;

  mtx = (int)rob->mtx;
  item_flag = rob_item_flag8[rob->side] != 0;
  mtx_push(0);
  if (rob->hands[0].shape >= 0) {
    mtx_mul_by((float *)(mtx + 0x1c0));
    mtx_scale_xyz(1.0, 1.0, -1.0);
    poly_or_word_bit31_toggle(1);
    if (mode_get_main() == MODE_MAIN_ENDING || stage_cur_no == 0x12) {
      objs = *(int *)(rob->chara_data + 0x48);
      obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x54));
    } else if (rob->hands[0].flags & 2) {
      if (rob->flags & 0x40000000) {
        objs = *(int *)(rob->chara_data + 0x48);
        obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x50));
      } else if (!item_flag) {
        objs = *(int *)(rob->chara_data + 0x48);
        obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x10));
      }
    } else if (!item_flag) {
      rob_hand_draw(rob, 0);
    }
    poly_or_word_bit31_toggle(0);
  }
  if (rob->hands[1].shape >= 0) {
    mtx_peek_pushed(0);
    mtx_mul_by((float *)(mtx + 0x300));
    if (mode_get_main() == MODE_MAIN_ENDING || stage_cur_no == 0x12) {
      objs = *(int *)(rob->chara_data + 0x48);
      obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x54));
    } else if (rob->hands[1].flags & 2) {
      if (rob->flags & 0x40000000) {
        objs = *(int *)(rob->chara_data + 0x48);
        obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x50));
      } else if (!item_flag) {
        objs = *(int *)(rob->chara_data + 0x48);
        obj_draw(*(uint *)(rob->costume * 0x20 + objs + 0x10));
      }
    } else if (!item_flag) {
      rob_hand_draw(rob, 1);
    }
  }
  mtx_store_wordwise(m);
  if (m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[1] * m[4] * m[10] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[2] * m[5] * m[8] > 0.0
      && (rob->flags & 0x40000000) == 0) {
    mtx_peek_pushed(0);
    mtx_mul_by((float *)(mtx + 0xc0));
    if (rob->face_motion != 0) {
      eyes_draw(rob);
      face_draw_skinned(rob);
    } else if (rob->rigid_face >= 0) {
      eyes_draw(rob);
      obj_draw(rob->rigid_face);
    }
  }
  mtx_peek_pushed(0);
  rob_model_draw_bones(rob, rob->body_model);
  mtx_peek_pushed(0);
  rob_extras_draw(rob);
  mtx_pop(1);
}

void rob_draw_alt_model(rob_rec *rob)
{
  if (rob_alt_models[rob->chara][rob->costume] != 0) {
    rob_model_draw_bones(rob,rob_alt_models[rob->chara][rob->costume]);
  }
  return;
}

void rob_draw_single_object(rob_rec *rob)
{
  int *objs;
  if (stage_cur_no == 0x12) {
    objs = rob_single_objects_stage18;
  }
  else {
    objs = rob_single_objects;
  }
  mtx_push(0);
  mtx_translate(rob->pos.x,rob->pos.y,rob->pos.z);
  mtx_reset_3x3();
  obj_draw(objs[rob->side]);
  mtx_pop(1);
}

void rob_draw_mode_set(int side, uint mode)
{
  rob_draw_mode[side] = mode;
  return;
}

void rob_part_list_draw(rob_rec *rob)
{
  int *p;
  int n;
  int i;

  p = rob_part_lists[rob->chara][rob->costume];
  n = *p++;
  for (i = 0; i < n; i++) {
    if ((rob_part_mask[rob->side] & 1 << i) != 0) {
      rob_model_draw_bones(rob,p[i]);
    }
  }
  return;
}

void rob_model_draw_bones(rob_rec *rob, int model)
{
  float *m;
  obj_node *node;
  obj_node **q;
  byte n;
  byte i;

  m = rob->mtx;
  node = obj_node_get(model);
  if (node != 0) {
    if (node->type == 0x13)
      n = 0x7a;
    else if (node->type == 0x15)
      n = 0xa2;
    else
      return;
    q = (obj_node **)(node->bones + (int)node->file);
    mtx_push(0);
    node_draw_buffered(*q++);
    m += 16;
    for (i = 1; i < n; i++, m += 16) {
      if (*q != 0) {
        mtx_peek_pushed(0);
        mtx_mul_by(m);
      }
      __asm__ volatile ("pref @%0" : : "r" (m + 32));
      node_draw(*q++);
    }
    mtx_pop(1);
  }
}

void rob_body_calc_all(void)
{
  rob_rec *rob;
  uint held;
  int i;
  float t;

  rob = rob_get(0);
  perf_color_mark(0xbf0000);
  mtx_push(0);
  for (i = 0; i < 2; i++, rob++) {
    t = (float)(int)stopwatch_usec();
    if ((rob->flags & 1) != 0) {
      held = pause_frame_held();
      if ((held == 0) && (rob->face_motion != 0)) {
        face_rig_offset(rob);
      }
      rob_model_envelope_calc(rob,rob->body_model);
      rob_items_update(rob);
    }
    {
      float *p = rob_calc_usec;
      p[i] = (float)(int)stopwatch_usec() - t;
    }
  }
  mtx_pop(1);
  return;
}

void rob_model_envelope_calc(rob_rec *rob, int model)
{
  obj_node *node;

  node = obj_node_get(model);
  if (node != 0) {
    if ((node->type == 0x15) && (node->pairs_90 != 0)) {
      rob_extra_joints_calc((int *)(node->pairs_90 + (int)node->file),rob->mtx);
    }
    envelope_bones = (obj_node **)(node->bones + (int)node->file);
    envelope_update(envelope_bones,rob->mtx);
  }
  return;
}

void rob_extra_joint_calc(int joint, float *mtx, int slot)
{
  uint word;
  int parent;
  float pos[3];

  word = *(uint *)(joint + 0x80);
  parent = (short)(word >> 0x10);
  word = word & 0xffff;
  mtx_set_current_wordwise(mtx + parent * 0x10);
  mtx_xform_point((float *)(joint + 4),pos);
  mtx_set_current_wordwise(mtx + word * 0x10);
  mtx_overwrite_translation(pos);
  mtx_store_wordwise(mtx + slot * 0x10);
  return;
}

void rob_extra_joints_calc(int *pairs, float *mtx)
{
  int joint;

  mtx_push(0);
  joint = *pairs;
  while (joint != 0) {
    if (*pairs != 0) {
      rob_extra_joint_calc(*pairs,mtx,pairs[1]);
    }
    pairs = pairs + 2;
    joint = *pairs;
  }
  mtx_pop(1);
  return;
}

void rob_coli_spheres_draw(rob_rec *rob)
{
  int i;
  int n;
  int f;
  int obj;
  vec3 *v;
  float *radius;

  v = rob->coli->sph_a;
  radius = rob->coli->rad_b;
  n = 26;
  for (i = 0; i < n; i++, v++) {
    obj = 0x46;
    f = 0;
    if ((uint)rob->coli->touch_mask & (1 << i)) {
      obj = 0x49;
      if (rob->state & 0x4000) {
        obj = 0x48;
        f = 1;
      }
    }
    if ((uint)rob->coli->touch_bits & (1 << i)) {
      if (f == 1) obj = 0x4a;
      else obj = 0x4d;
    }
    mtx_push(0);
    mtx_translate_by_vec3((float *)v);
    mtx_scale_xyz(*radius, *radius, *radius);
    radius++;
    obj_draw(obj);
    mtx_pop(1);
  }
}

int anim_key_index_find(float *keys, int count, float time)
{
  int i;
  int c;

  if (keys == 0) return 0;
  c = 3;
  for (i = 0; i < count; i++, keys += 4) {
    if (__builtin_fabsf(*keys - time) < 1.1920929e-07f) {
      c = 0;
      break;
    }
    if (*keys > time) {
      c = (i == 0) ? 1 : 2;
      break;
    }
  }
  switch (c) {
  case 0:
    break;
  case 1:
    break;
  case 2:
  case 3:
    i--;
    break;
  }
  return i;
}

void morph_anim_time_advance(morph_anim_rec *anim)
{
  morph_anim_key *keys = anim->keys;

  anim->time += anim->speed;
  if (keys[anim->key_count - 1].time < anim->time) {
    anim->time = 0.0;
  }
}

void morph_anim_apply(morph_anim_rec *anim)
{
  morph_anim_key *keys;
  float *time;
  float frac;
  float lo;
  float hi;
  int i;

  keys = anim->keys;
  time = &anim->time;
  frac = curve_sample((float *)keys,anim->key_count,*time);
  i = anim_key_index_find((float *)keys,anim->key_count,*time);
  lo = keys[i].value;
  hi = keys[i + 1].value;
  frac = (frac - lo) / (hi - lo);
  obj_morph_set(anim->obj,i,frac);
}

void morph_anim_init(int slot)
{
  morph_anim_slot *e = &morph_anim_slots[slot];
  morph_anim_rec *anim = e->anim;
  morph_anim_key *keys = anim->keys;
  float v = keys[anim->key_count - 1].time;
  e->scale = v / (float)e->frames;
  anim->time = 0;
}

void morph_anim_update(int slot, float rate)
{
  morph_anim_slot *e;
  morph_anim_rec *anim;

  e = &morph_anim_slots[slot];
  anim = e->anim;
  anim->speed = rate * e->scale;
  morph_anim_time_advance(anim);
  morph_anim_apply(anim);
}

void morph_anim_draw(int slot)
{
  obj_draw(morph_anim_slots[slot].anim->obj);
}

void empty_fn_0c0d1840(void)
{
  return;
}

void ray_hit_plane_y(float *out, float *point, float *dir, float height)
{
  float t;
  
  t = (height - point[1]) / dir[1];
  *out = t * *dir + *point;
  out[1] = height;
  out[2] = t * dir[2] + point[2];
  return;
}

void rob_shadow_draw(rob_rec *rob)
{
  typedef struct { int w[3]; } vblk;
  typedef struct { int obj; short bone; short pad; float len; } shd_ent;
  uint i;
  float *base;
  float light[3];
  shd_ent *tbl;
  float len;
  float scale;
  base = rob->mtx;
  if (shadow_erase == 0) return;
  mtx_save_proj_x_current(0.0,0.0);
  if (shadow_volume_on != 0) shadow_draw_fn = shadow_volume_draw_fn;
  else shadow_draw_fn = obj_draw;
  shadow_height_scale = shadow_height_scale_base;
  if ((demo_flags & 0x80) != 0) {
    shadow_draw_fn = obj_draw;
    shadow_height_scale = 0.01;
  }
  *(vblk *)light = ((vblk *)&shadow_light_dir[0][0])[rob->side];
  vec3_normalize(light);
  tbl = (shd_ent *)shadow_piece_tables[rob->chara * 2 + rob->costume];
  if ((rob->flags & 0x40000000) != 0) tbl = (shd_ent *)&shadow_pieces_alt;
  mtx_push(0);
  for (i = 0; i < 15; i++, tbl++) {
    float a[3] = { 0.0, 0.0, 0.0 };
    float wa[3];
    float pa[3];
    float wb[3];
    float pb[3];
    float dir[3];
    if (shadow_piece_on[i] == -1) break;
    if (shadow_piece_on[i] == 0) continue;
    scale = 1.0;
    mtx_set_current_wordwise(base + tbl->bone * 0x10);
    if (tbl->bone == 3) mtx_rot_zyx_s16(0,-0x4000,0);
    else if (tbl->bone == 14) mtx_rot_zyx_s16(0,0,-0x4000);
    else if (tbl->bone == 17) {
      mtx_translate_by_vec3(shadow_bone_offset);
      mtx_rot_zyx_s16(0,0,0x4000);
    }
    else if (tbl->bone == 21) {
      mtx_translate_by_vec3(shadow_bone_offset);
      mtx_rot_zyx_s16(0,0,0x4000);
    }
    a[0] = -shadow_piece_margin;
    mtx_xform_point(a,wa);
    a[0] = tbl->len + shadow_piece_margin;
    mtx_xform_point(a,wb);
    ray_hit_plane_y(pa,wa,light,rob->floor_y + shadow_floor_offset);
    ray_hit_plane_y(pb,wb,light,rob->floor_y + shadow_floor_offset);
    if (shadow_labels_on != 0) {
      debug_cross_label_draw(pa,(char *)&root_text,0xffffffff);
      debug_cross_label_draw(pb,leaf_text,0xffffffff);
    }
    len = vec3_distance(pa,pb);
    if ((tbl->bone == 17) | (tbl->bone == 21)) {
      if (len < 1.0) len = 1.0;
    }
    else if (tbl->bone == 16) {
      float c8[3];
      float bc[3];
      float b0[3];
      mtx_set_current_wordwise(base + 0x110);
      mtx_store_translation(bc);
      mtx_translate_by_vec3(shadow_bone_offset);
      mtx_rot_zyx_s16(0,0,0x4000);
      mtx_store_translation(c8);
      ray_hit_plane_y(b0,c8,light,rob->floor_y + shadow_floor_offset);
      *(vblk *)pb = *(vblk *)b0;
      len = vec3_distance(pa,b0);
      len = len / vec3_distance(wa,bc);
    }
    else if (tbl->bone == 20) {
      float a4[3];
      float p98[3];
      float p8c[3];
      mtx_set_current_wordwise(base + 0x150);
      mtx_store_translation(p98);
      mtx_translate_by_vec3(shadow_bone_offset);
      mtx_rot_zyx_s16(0,0,0x4000);
      mtx_store_translation(a4);
      ray_hit_plane_y(p8c,a4,light,rob->floor_y + shadow_floor_offset);
      *(vblk *)pb = *(vblk *)p8c;
      len = vec3_distance(pa,p8c);
      len = len / vec3_distance(wa,p98);
    }
    else if ((tbl->bone == 1) | (tbl->bone == 14)) {
      float up[3];
      float up2[3];
      float e68[3];
      float e5c[3];
      float e50[3];
      float one;
      float t1;
      float u;
      up[0] = 0.0;
      up[1] = one = 1.0;
      up[2] = 0.0;
      mtx_xform_dir(up,up);
      scale = fabsf(vec3_dot(up,light)) * (one - shadow_width_min) + shadow_width_min;
      mtx_set_current_wordwise(base + 0x50);
      mtx_store_translation(up2);
      ray_hit_plane_y(e68,up2,light,rob->floor_y + shadow_floor_offset);
      t1 = vec3_distance(pa,e68);
      mtx_set_current_wordwise(base + 0xa0);
      mtx_store_translation(e5c);
      ray_hit_plane_y(e50,e5c,light,rob->floor_y + shadow_floor_offset);
      len = vec3_distance(pa,e50);
      if (t1 > len) {
        u = vec3_distance(wa,up2);
        len = t1;
      }
      else {
        u = vec3_distance(wa,e5c);
      }
      len = len / u + shadow_stretch_add;
      if (len < 1.0) len = len * (1.0 - shadow_stretch_min) + shadow_stretch_min;
    }
    vec3_sub(dir,pb,pa);
    {
      ushort ang = atan2_s16(dir[0],dir[2]);
      mtx_peek_pushed(0);
      mtx_translate(0.0,shadow_lift,0.0);
      mtx_translate_by_vec3(pa);
      mtx_rot_y_s16(ang + shadow_angle_offset);
    }
    if (shadow_test_bones_17_21 != 0) {
      mtx_scale_xyz(len,shadow_height_scale,scale);
      if ((tbl->bone == 17) | (tbl->bone == 21)) obj_draw(shadow_test_obj);
    }
    else {
      mtx_scale_xyz(len,shadow_height_scale,scale);
      if (tbl->bone == 14) mtx_rot_zyx_s16(0,0,0x4000);
      else if (tbl->bone == 3) mtx_rot_zyx_s16(0x4000,0,0x4000);
      {
        int arg;
        if (shadow_test_obj_on != 0) arg = shadow_test_obj;
        else arg = tbl->obj;
        shadow_draw_fn(arg);
      }
    }
  }
  mtx_pop(1);
}

void shadow_volume_draw_both(uint obj)
{
  obj_volume_draw(obj);
  obj_volume_draw_trans(obj);
  return;
}

void shadow_stage_setup(stage_rec *stage)
{
  int i;
  vec3 *dir;
  vec3 dir_default;
  obj_draw_handler draw_fn;

  dir = (vec3 *)stage->light_dir;
  dir_default.x = 1.0;
  dir_default.y = -1.0;
  dir_default.z = 0.2;
  if (dir == 0) {
    dir = &dir_default;
  }
  for (i = 0; i < 2; i++) {
    vec3 *t = (vec3 *)&shadow_light_dir[0][0];
    t[i] = *dir;
  }
  if (memcmp("BAN STAGE",stage->name,3) == 0) {
    draw_fn = shadow_volume_draw_both;
  }
  else {
    draw_fn = obj_volume_draw;
  }
  shadow_volume_draw_fn = draw_fn;
}

void osage_chain_nodes_count(int *cmds, char *counts)
{
  int go;
  for (go = 1; go; cmds += 2) {
    osage_joint_desc *joint = (osage_joint_desc *)cmds[1];
    switch (*cmds) {
    case 3:
      counts[joint->param]++;
      break;
    case 0:
    case 4:
      counts[joint->param]++;
      go = 0;
      break;
    case 1:
    case 2:
      break;
    }
  }
}

void osage_chains_add_params(rob_rec *rob, int model, osage_param *params, int *coli_lists)
{
  typedef struct { int w[5]; } param_head;
  int i = 0;
  uint side = rob->side;
  obj_node *node;
  void **chains;
  char *used;
  char *used2;
  char *used3;
  char *used4;
  node = obj_node_get(model);
  if (node == 0) return;
  chains = node->list_80;
  if (chains == 0) return;
  for (; i < 100; i++) {
    used = osage_node_used;
    used[i] = 0;
  }
  osage_rob = rob;
  osage_model_first_node = (int)osage_node_next;
  while (*chains != 0) {
    osage_chain_nodes_count((int *)*chains,osage_node_used);
    osage_chain_build(rob,&osage_chains[side][osage_chain_count],(int *)*chains,j_ex_00[side]);
    chains++;
    osage_chains[side][osage_chain_count].model_id = model;
    osage_chain_count++;
  }
  if (params == 0) params = (osage_param *)&rot_type;
  for (i = 0; i < 100; i++) {
    int params_base;
    used2 = osage_node_used;
    params_base = (int)j_ex_00;
    if (used2[i] != 0) {
      osage_param *param = (osage_param *)(side * 0x960 + i * 24 + params_base);
      int coli = param->coli_list;
      *(param_head *)param = *(param_head *)(params + i);
      param->coli_list = coli;
    }
  }
  if (coli_lists != 0) {
    osage_coli_lists_last = coli_lists;
    while (*coli_lists >= 0) {
      int params_base;
      used3 = osage_node_used;
      params_base = (int)j_ex_00;
      if (used3[*coli_lists] != 0) {
        ((osage_param *)(*coli_lists * 24 + side * 0x960 + params_base))->coli_list = coli_lists[1];
      }
      coli_lists += 2;
    }
  }
  else {
    for (i = 0; i < 100; i++) {
      int params_base;
      used4 = osage_node_used;
      params_base = (int)j_ex_00;
      if (used4[i] != 0) {
        ((osage_param *)(i * 24 + side * 0x960 + params_base))->coli_list = 0;
      }
    }
  }
  return;
}

void osage_chains_add_list(rob_rec *rob, int model, int *list)
{
  osage_chain_slot (*slots)[64];
  uint side = rob->side;
  obj_node *node;
  void **chains;
  node = obj_node_get(model);
  if (node == 0)
  {
    return;
  }
  chains = node->list_80;
  if (chains == 0)
  {
    return;
  }
  osage_rob = rob;
  osage_model_first_node = (int)osage_node_next;
  while ((*chains) != 0)
  {
    osage_chain_nodes_count((int *) (*chains), osage_node_used);
    slots = osage_chains;
    osage_chain_build(rob, &slots[side][osage_chain_count], (int *) (*chains), j_ex_00[side]);
    chains++;
    slots[side][osage_chain_count].model_id = model;
    osage_chain_count++;
  }

  while ((*list) >= 0)
  {
    int params_base;
    params_base = (int) j_ex_00;
    *((osage_param *) ((((*list) * 24) + (side * 0x960)) + params_base)) = *((osage_param *) (list + 1));
    list += 7;
  }

}

void osage_param_list_copy(int *list, int *out)
{
  int n;

  n = 0;
  if (list == 0) return;
  if (out == 0) return;
  while (*(short *)list != 0) {
    out[0] = list[0];
    out[1] = list[1];
    out[2] = list[2];
    out[3] = list[3];
    out[4] = list[4];
    out[5] = list[5];
    out[6] = list[6];
    list += 7;
    out += 7;
    n++;
    if (n > 6) break;
  }
  *(short *)out = 0;
  return;
}

void osage_node_link_set(osage_node *nodes, int node_a, int node_b, float length)
{
  uint first = (uint)(osage_model_first_node - (int)osage_node_first) / 0x2c;
  nodes[node_a + first].link = (int)&nodes[node_b + first];
  nodes[node_a + first].link_len = length;
}

void rob_osage_init(rob_rec *rob)
{
  int model;
  obj_node *node;
  int i;
  model = rob->body_model;
  node = obj_node_get(model);
  for (i = 0; i < 64; i++) {
    osage_chains[rob->side][i].on = 0;
  }
  for (i = 0; i < 100; i++) {
    char *t = (char *)j_ex_00;
    ((osage_param *)(t + (i * 24 + rob->side * 0x960)))->coli_list = 0;
    *(osage_param *)(t + (i * 24 + rob->side * 0x960)) = osage_param_zero;
  }
  osage_chain_count = 0;
  for (i = 0; i < 100; i++) {
    osage_nodes[rob->side][i].mtx = (float *)0xdeaddead;
    osage_nodes[rob->side][i].link = 0xdeaddead;
  }
  osage_node_next = osage_nodes[rob->side];
  osage_node_first = osage_node_next;
  if (node == 0) return;
  if ((rob->flags & 0x40000000) != 0) {
    osage_setup_alt_tables(rob);
    return;
  }
  switch (rob->chara) {
  case 0:
    if (rob->costume == 0) osage_chains_add_list(rob, model, (int *)&osage_list_aki_0);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_aki_1, (int *)&osage_coli_none);
    break;
  case 2:
    if (rob->costume == 0) osage_chains_add_list(rob, model, (int *)&osage_list_lau_0);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_lau_1, (int *)&osage_coli_none);
    break;
  case 4:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_jef_0, (int *)&osage_coli_jef_0);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_jef_1, (int *)&osage_coli_jef_1);
    break;
  case 13:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_dur_0, (int *)&osage_coli_none);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_dur_1, (int *)&osage_coli_none);
    break;
  case 5:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_pai_0, (int *)&osage_coli_none);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_pai_1, (int *)&osage_coli_pai_1);
    break;
  case 11:
    if (rob->costume == 0) {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_boz_0, (int *)&osage_coli_boz_0);
      if (model == 0x2e0001) {
        osage_node_link_set(osage_nodes[rob->side], 0x12, 0x1a, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 0x13, 0x1a, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 0x14, 0x1b, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 0x16, 0x1a, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 0x17, 0x1b, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 0x18, 0x1c, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 5, 9, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 6, 10, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 7, 0xb, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 1, 9, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 2, 9, osage_link_len_boz_0);
        osage_node_link_set(osage_nodes[rob->side], 3, 10, osage_link_len_boz_0);
      }
    }
    else {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_boz_1, (int *)&osage_coli_boz_1);
      if (model == 0x2f0001) {
        osage_node_link_set(osage_nodes[rob->side], 0xb, 0x10, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 0xc, 0x11, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 0xd, 0x12, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 0xe, 0x13, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 6, 1, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 7, 2, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 8, 3, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 9, 4, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 0x10, 1, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 2, 0x11, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 0x12, 3, osage_link_len_boz_1);
        osage_node_link_set(osage_nodes[rob->side], 4, 0x13, osage_link_len_boz_1);
      }
    }
    break;
  case 9:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_wol_0, (int *)&osage_coli_wol_0);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_wol_1, (int *)&osage_coli_wol_1);
    break;
  case 1:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_sar_0, (int *)&osage_coli_sar_0);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_sar_1, (int *)&osage_coli_sar_1);
    break;
  case 10:
    if (rob->costume == 0) {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_aoi_0, (int *)&osage_coli_aoi_0);
    }
    else {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_aoi_1, (int *)&osage_coli_aoi_1);
      if (model == 0x2b0000) {
        osage_node_link_set(osage_nodes[rob->side], 0xd, 1, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0xe, 2, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0xf, 3, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x10, 4, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x11, 5, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 7, 1, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 8, 2, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 9, 3, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 10, 4, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0xb, 5, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x1f, 0x13, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x20, 0x14, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x21, 0x15, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x22, 0x16, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x23, 0x17, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x19, 0x13, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x1a, 0x14, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x1b, 0x15, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x1c, 0x16, osage_link_len_aoi_1);
        osage_node_link_set(osage_nodes[rob->side], 0x1d, 0x17, osage_link_len_aoi_1);
      }
    }
    break;
  case 3:
    if (rob->costume == 0) {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_shun_0, (int *)&osage_coli_shun_0);
      if (model == 0x580000) {
        osage_node_link_set(osage_nodes[rob->side], 0x1b, 0x19, osage_link_len_shun_0);
        osage_node_link_set(osage_nodes[rob->side], 0x15, 0x17, osage_link_len_shun_0);
        osage_node_link_set(osage_nodes[rob->side], 0x15, 0x1b, osage_link_len_shun_0);
      }
    }
    else {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_shun_1, (int *)&osage_coli_shun_1);
      if (model == 0x590000) {
        osage_node_link_set(osage_nodes[rob->side], 0x21, 0x23, osage_link_len_shun_1);
        osage_node_link_set(osage_nodes[rob->side], 0x27, 0x25, osage_link_len_shun_1);
        osage_node_link_set(osage_nodes[rob->side], 0x27, 0x21, osage_link_len_shun_1);
      }
    }
    break;
  case 8:
    if (rob->costume == 0) {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_lio_0, (int *)&osage_coli_lio_0);
      if (model == 0x420000) {
        osage_node_link_set(osage_nodes[rob->side], 4, 1, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 5, 2, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 7, 1, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 8, 2, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 10, 4, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 0xb, 5, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 0xd, 7, osage_link_len_lio_0);
        osage_node_link_set(osage_nodes[rob->side], 0xe, 8, osage_link_len_lio_0);
      }
    }
    else {
      osage_chains_add_params(rob, model, (osage_param *)&osage_params_lio_1, (int *)&osage_coli_none);
    }
    break;
  case 7:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_kag_0, (int *)&osage_coli_none);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_kag_1, (int *)&osage_coli_none);
    break;
  case 6:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_jak_0, (int *)&osage_coli_none);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_jak_1, (int *)&osage_coli_none);
    break;
  case 12:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_blk_0, (int *)&osage_coli_none);
    else osage_chains_add_params(rob, model, (osage_param *)&osage_params_blk_1, (int *)&osage_coli_blk_1);
    break;
  case 14:
    if (rob->costume == 0) osage_chains_add_list(rob, model, (int *)&osage_list_jdk_0);
    else osage_chains_add_list(rob, model, (int *)&osage_list_jdk_1);
    break;
  case 15:
    if (rob->costume == 0) osage_chains_add_params(rob, model, (osage_param *)&osage_params_mue_0, (int *)&osage_coli_mue_0);
    else osage_chains_add_list(rob, model, (int *)&osage_list_mue_1);
    break;
  }
  rob_osage_items_init(rob);
  for (i = 0; i < 64; i++) {
    osage_node_by_index[rob->side][i] = 0;
  }
  for (i = 0; i < 100; i++) {
    char *nodes = (char *)osage_nodes;
    osage_node *n = (osage_node *)(nodes + (rob->side * 0x1130 + i * 44));
    if (n->num > 0) {
      osage_node_by_index[rob->side][n->num] = n;
    }
  }
}

void rob_osage_items_init(rob_rec *rob)
{
  int model;
  byte *item;
  rob_item_slot *slots;
  int slot;
  
  slots = (rob_item_slot *)rob->items;
  for (slot = 0; slot < 4; slot++, slots++) {
    model = rob_item_value_get(rob,slot,1,0);
    if (-1 < model) {
      item = (byte *)slots->data;
      if ((*item & 2) != 0 || *(int *)(item + 8) == 0) {
        osage_chains_add_params(rob,model,*(osage_param **)(item + 8),*(int **)(item + 0xc));
      }
      else {
        osage_chains_add_list(rob,model,*(int **)(item + 8));
      }
      osage_item_links_set(rob,model);
    }
    model = rob_item_value_get(rob,slot,1,1);
    if (-1 < model) {
      item = (byte *)slots->card_data;
      if ((*item & 2) != 0 || *(int *)(item + 8) == 0) {
        osage_chains_add_params(rob,model,*(osage_param **)(item + 8),*(int **)(item + 0xc));
      }
      else {
        osage_chains_add_list(rob,model,*(int **)(item + 8));
      }
      osage_item_links_set(rob,model);
    }
  }
}

void osage_item_links_set(rob_rec *rob, int model)
{
  if (obj_node_get(model) != 0) {
    if (model == 0x650004) {
      osage_node_link_set(osage_nodes[rob->side],26,22,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],27,23,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],30,26,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],31,27,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],34,30,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],35,31,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],38,34,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],39,35,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],42,38,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],43,39,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],18,22,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],19,23,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],14,18,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],15,19,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],10,14,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],11,15,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],6,10,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],7,11,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],2,6,osage_link_len_item_650004);
      osage_node_link_set(osage_nodes[rob->side],3,7,osage_link_len_item_650004);
    }
    else if (model == 0x500008) {
      osage_node_link_set(osage_nodes[rob->side],16,10,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],17,11,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],13,10,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],14,11,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],4,7,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],5,8,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],1,7,osage_link_len_item_500008);
      osage_node_link_set(osage_nodes[rob->side],2,8,osage_link_len_item_500008);
    }
  }
}

void rob_osage_chains_clear(rob_rec *rob)
{
  int i;
  uint side = rob->side;
  for (i = 0; i < 0x40; i++) {
    osage_chains[side][i].on = 0;
  }
}

void osage_chains_clear_all(void)
{
  int i;
  int j;
  int *chains;
  rob_get(0);
  i = 0;
  chains = &osage_chains[0][0].on;
  for (; i < 2; i++) {
    for (j = 0; j < 0x40; j++) {
      chains[j * 7 + i * 0x1c0] = 0;
    }
  }
}

void osage_chain_root_matrix(rob_rec *rob, float *out, int chain)
{
  obj_node *node;
  float *mtx;
  chain_root_desc *root;
  void **chains;
  float pos[3];
  
  node = obj_node_get(rob->body_model);
  chains = node->list_80;
  if (chains != 0) {
    osage_root_model = node;
    root = (chain_root_desc *)((int *)chains[chain])[1];
    mtx = rob->mtx;
    mtx_push(0);
    mtx_set_current_wordwise(mtx + root->slot * 0x10);
    mtx_xform_point(root->offset,pos);
    mtx_peek_pushed(0);
    mtx_overwrite_translation(pos);
    mtx_store_wordwise(out);
    mtx_pop(1);
  }
  return;
}

void osage_calc_all(void)
{
  rob_rec *rob;
  int i;
  int start;
  int j;
  float *mtx;
  int side;
  char *o;
  ushort angle;
  osage_node *node;
  int base;

  rob = rob_get(0);
  if ((demo_flags & 2) == 0 || mode_get_main() != MODE_MAIN_ADVERTISE || mode_get_sub() != 8) {
    wind_update(&stage_wind);
    osage_wind.x = *(uint *)&stage_wind.vec.x;
    osage_wind.y = *(uint *)&stage_wind.vec.y;
    osage_wind.z = *(uint *)&stage_wind.vec.z;
    mtx_push(0);
    if (pause_frame_held() == 0 || deb_sw != 0) {
      for (i = 0; i < 2; i++, rob++) {
        start = stopwatch_usec();
        if (obj_node_get(rob->body_model) == 0) {
          rob_osage_chains_clear(rob);
        }
        else {
          osage_rob = rob;
          for (j = 0; j < 0x40; j++) {
            if (osage_chains[i][j].on != 0) {
              mtx = rob->mtx;
              mtx_set_current_wordwise(mtx + osage_chains[i][j].root_slot * 0x10);
              if ((rob->flags & 0x100000) != 0) {
                osage_chain_reset(&osage_chains[i][j]);
              }
              osage_chain_calc(&osage_chains[i][j]);
            }
          }
          if ((rob->flags & 0x40000000) == 0) {
            switch (rob->chara) {
            case 10:
              if (rob->costume == 0 && rob->body_model == 0x2a0000) {
                mtx_set_current_wordwise(rob->mtx + 0x130);
                osage_chain_root_matrix(rob, rob->mtx + 0x1a0, 7);
                mtx_set_current_wordwise(rob->mtx + 0xf0);
                osage_chain_root_matrix(rob, rob->mtx + 0x190, 6);
              }
              break;
            case 8:
              if (rob->costume == 1 && rob->body_model == 0x430000) {
                mtx_set_current_wordwise(rob->mtx + 0x130);
                osage_chain_root_matrix(rob, rob->mtx + 0x1a0, 1);
                mtx_set_current_wordwise(rob->mtx + 0xf0);
                osage_chain_root_matrix(rob, rob->mtx + 0x190, 0);
              }
              break;
            case 9:
              if (rob->costume == 1 && rob->body_model == 0x510000) {
                mtx_set_current_wordwise(rob->mtx + 0x30);
                osage_chain_root_matrix(rob, rob->mtx + 0x220, 2);
                mtx_set_current_wordwise(rob->mtx + 0x130);
                osage_chain_root_matrix(rob, rob->mtx + 0x210, 8);
                mtx_set_current_wordwise(rob->mtx + 0xf0);
                osage_chain_root_matrix(rob, rob->mtx + 0x200, 7);
              }
              break;
            case 0xe:
              side = rob->side;
              base = side * 400 + (int)&osage_node_by_index;
              o = (char *)128;
              node = *(osage_node **)(o + base);
              if (node != 0) {
                angle = node->ang[1];
                if (angle > 0x4000 && angle < 0xc000) {
                  base = side * 2400 + (int)j_ex_00;
                  ((osage_param *)base)[32].coli_list = (uint)&osage_coli_jdk_node32;
                }
              }
              break;
            }
          }
          (&rob_chain_usec.rob0)[i] = (float)((int)stopwatch_usec() - start) + (&rob_chain_usec.rob0)[i];
        }
      }
    }
    mtx_pop(1);
  }
}

void rob_shun_item_draw(rob_rec *rob)
{
  byte side;
  int obj;
  uint costume;
  int idx;
  int mtx;
  int bones [2];
  osage_chain_slot *chain;
  uint mirror;

  side = rob->side;
  if (osage_sticks_on != 0) {
    for (idx = 0; idx < 0x40; idx++) {
      char *b = (char *)osage_chains;
      chain = (osage_chain_slot *)(b + side * 0x700 + idx * 0x1c);
      if (chain->on != 0) {
        osage_chain_sticks_draw(chain);
      }
    }
  }
  if (rob->chara != 3) {
    return;
  }
  bones[0] = 0x34;
  bones[1] = 0x34;
  mtx = (int)rob->mtx;
  obj = chara3_item_value_get((uint)rob->costume);
  costume = (uint)rob->costume;
  idx = bones[costume];
  if ((rob->flags & 0x40000000) != 0) {
    idx = 0x18;
    if (costume == 0) {
      obj = 0x4e0003;
    }
    else {
      obj = 0x4f0003;
    }
  }
  mtx_push(0);
  if ((rob->flags & 0x1800000) != 0) {
    mirror = (uint)(int)(short)rob->side_flags >> 4 & 1;
    if (((int)rob->flags >> 0x17 & 1U) != mirror) {
      mtx_mul_by((float *)(mtx + 0x1c0));
      mtx_scale_xyz(1.0,1.0,-1.0);
      mtx_translate_by_vec3(shun_item_hand_offset);
      mtx_rot_z_s16((ushort)shun_item_hand_rz);
      mtx_rot_y_s16(shun_item_hand_ry);
      mtx_rot_x_s16(shun_item_hand_rx);
      hdr_or_top_bit(1);
      obj_draw(obj);
      hdr_or_top_bit(0);
    }
    else {
      mtx_mul_by((float *)(mtx + 0x300));
      mtx_translate_by_vec3(shun_item_hand_offset);
      mtx_rot_z_s16((ushort)shun_item_hand_rz);
      mtx_rot_y_s16(shun_item_hand_ry);
      mtx_rot_x_s16(shun_item_hand_rx);
      obj_draw(obj);
    }
  }
  else {
    mtx_mul_by((float *)(mtx + idx * 0x40));
    mtx_rot_x_s16(*(ushort *)(&shun_item_rx + (uint)rob->costume * 2));
    obj_draw(obj);
  }
  mtx_pop(1);
  if ((rob->flags & 0x80000) != 0) {
    rob->flags = rob->flags & 0xfff7ffff;
    shun_drink_tint_apply(rob);
  }
}

void osage_node_stick_draw(float *length, osage_node *node)
{
  mtx_push(0);
  ((void (*)(float *))mtx_mul_by)((float *)node->mtx);
  mtx_scale_xyz(*length,osage_stick_width,osage_stick_width);
  coord_scale_store(*length);
  obj_draw(osage_stick_obj);
  mtx_pop(1);
  return;
}

void osage_chain_sticks_draw(osage_chain_slot *chain)
{
  int *cmd;
  osage_node *node;
  node = (osage_node *)chain->nodes;
  node++;
  for (cmd = chain->cmds; ; cmd += 2) {
    switch (*cmd) {
    case 3:
      osage_node_stick_draw((float *)cmd[1], node++);
      break;
    case 0:
    case 4:
      osage_node_stick_draw((float *)cmd[1], node);
      return;
    }
  }
}

void j_ex_coli_lists_clear(void)
{
  int i;

  for (i = 0; i < 100; i++) {
    j_ex_00[0][i].coli_list = 0;
  }
  return;
}

void face_draw_skinned(rob_rec *rob)
{
  int objs;
  int face_obj;

  objs = *(int *)(rob->chara_data + 0x48);
  face_obj = *(int *)(objs + (uint)rob->costume * 4 + 0x80);
  if (-1 < face_obj) {
    mtx_push(0);
    face_skin_update(rob,face_obj);
    camera_view_load();
    mtx_mul_by((float *)((int)rob->mtx + 0xc0));
    obj_draw(face_obj);
    mtx_pop(1);
  }
}

void face_skin_update(rob_rec *rob, int model)
{
  obj_node *node;
  int usec;
  int skin;
  float *verts;

  node = obj_node_get(model);
  if (node != 0 && (skin = (int)node->model) != 0 && node->type == 0xd) {
    verts = (float *)((int)node->list_80 + (int)node->file);
    usec = stopwatch_usec();
    verts = face_skin(verts,(int)rob->face_mtx,(compiled_model_rec *)skin);
    if (node->type == 0xd) {
      face_static_copy(verts,skin);
    }
    usec = stopwatch_usec() - usec;
    face_skin_usec[rob->side] = usec;
  }
}

void rob_disp_setup(rob_rec *rob)
{
  bone_chains_setup(rob);
  rob_osage_init(rob);
  rob_cloth_init(rob);
  rob_zanzou_slots_clear();
  item_zanzou_setup();
  item_anim_snap_start();
  return;
}

void rob_disp_release(rob_rec *rob)
{
  bone_chains_clear(rob);
  rob_osage_chains_clear(rob);
  rob_cloth_clear(rob);
  return;
}

void item_zanzou_restart(void)
{
  item_zanzou_setup();
  item_anim_snap_start();
  return;
}

void rob_effect_init(void)
{
  bone_chains_init();
  osage_chains_clear_all();
  rope_init();
  cloth_clear_all();
  return;
}

void rob_effect_exec(void)
{
  int now;
  
  rob_chain_usec.rob0 = 0.0;
  rob_chain_usec.rob1 = 0.0;
  perf_color_mark(0x7f00);
  item_anims_update();
  bone_chains_update();
  osage_calc_all();
  perf_color_mark(0x7f);
  cloth_calc_all();
  perf_color_mark(0x7f0000);
  rob_body_calc_all();
  perf_color_mark(0x3f3f3f);
  rob_trails_usec = stopwatch_usec();
  rob_zanzou_calc_all();
  item_zanzou_update();
  rope_update();
  now = stopwatch_usec();
  rob_trails_usec = now - rob_trails_usec;
  zanzou_cam_mode_last = (int)camera.mode_last;
  perf_color_mark(0);
  return;
}

void rob_extras_draw(rob_rec *rob)
{
  perf_color_mark(0x7f00);
  bone_chains_draw(rob);
  rob_shun_item_draw(rob);
  perf_color_mark(0x7f);
  rob_cloth_draw(rob);
  perf_color_mark(0x3f3f3f);
  rope_draw();
  rob_zanzou_disp(rob);
  bone_tree_debug_draw(rob);
  osage_debug_draw(rob);
  perf_color_mark(0);
  return;
}

void bone_tree_debug_draw(rob_rec *rob)
{
  if (bone_tree_debug_on != 0) {
    mtx_push(0);
    camera_view_load();
    ((void (*)(float,float))mtx_save_proj_x_current)(0.0,0.0);
    bone_tree_debug_draw_node(rob->bones);
    mtx_pop(1);
  }
  return;
}

void bone_tree_debug_draw_node(bone_work_rec *bone)
{
  bone_work_rec *child;
  bone_work_rec **children;
  int i;
  float a[2]; float b[2];

  children = bone->children;
  for (i = 0; i < bone->def->nchildren; i++, children++) {
      child = *children;
      if (((bone->def->flags & 0x40) != 0) &&
         ((child->def->flags & 0x80) != 0)) {
        debug_cross_label_draw(child->mtx + 12,bone->def->name,0xffffffff);
      }
      if ((child->def->flags & 0x1900) == 0x1900) {
        debug_cross_label_draw(child->mtx + 12,child->def->name,0xffffffff);
      }
      if (((bone->def->flags & 0x80) != 0) &&
         ((child->def->flags & 0x3100) != 0)) {
        proj_point_to_pixel_xy(bone->mtx + 12,a);
        proj_point_to_pixel_xy(child->mtx + 12,b);
        screen_line_draw(a[0],a[1],b[0],b[1],debug_line_depth,0xff00);
        text_cursor_put((int)((a[0] + b[0]) * 0.5),(int)((a[1] + b[1]) * 0.5));
        text_color_set(0xff00ff00);
        text_draw_string(bone->def->name);
        text_color_set(0xffffffff);
      }
      bone_tree_debug_draw_node(child);
  }
  return;
}

void debug_cross_label_draw(float *point, char *text, uint color)
{
  float v[2];
  
  proj_point_to_pixel_xy(point,v);
  screen_line_draw(v[0] - 6.0,v[1],v[0] + 6.0,v[1],debug_line_depth,color);
  screen_line_draw(v[0],v[1] - 6.0,v[0],v[1] + 6.0,debug_line_depth,color);
  text_cursor_put((int)(v[0] + 8.0),(int)(v[1] - 4.0));
  if (text != 0) {
    text_draw_string(text);
  }
}

void osage_coli_0_open(void)
{
  return;
}

void ex_osage_new_open(void)
{
  part_0c2edba8 = 0;
  osage_edit_target_last = 1;
  c_root = 0;
  osage_edit_c_root_last = 1;
  deb_sw = 1;
  bone_sw = 1;
  return;
}

void osage_edit_targets_update(void)
{
  rob_rec *rob;
  int i;
  int model;
  rob = rob_get(0);
  if ((osage_edit_parts & 1U) != 0) {
    osage_edit_targets[0] = rob->body_model;
  }
  for (i = 0; i < 4; i++) {
    if ((osage_edit_parts >> (i + 1)) & 1) {
      
      model = rob_item_value_get(rob,i,1,1);
      if (model < 0) {
        model = rob_item_value_get(rob,i,1,0);
      }
      { int *a = osage_edit_targets; a[i + 1] = model; }
    }
  }
}

void ex_osage_new_update(void)
{
  int count;
  int i;
  int side;

  count = 0;
  side = 0;
  osage_edit_targets_update();
  if (osage_edit_restore_request != 0) {
    osage_edit_restore_selected();
  }
  for (i = 0; i < 0x40; i++) {
    if (osage_chains[side][i].on != 0) {
      if (osage_chains[side][i].model_id == osage_edit_targets[part_0c2edba8]) {
      if (count == c_root) {
        if (osage_edit_c_root_last != count || osage_edit_target_last != part_0c2edba8) {
          osage_edit_chain_exchange(&osage_chains[side][i],1);
        }
        osage_edit_chain_exchange(&osage_chains[side][i],0);
      }
      if (coli_flag != 0) {
        osage_chain_coli_attach(&osage_chains[side][i]);
      }
      count++;
      }
    }
  }
  osage_edit_target_last = part_0c2edba8;
  osage_edit_c_root_last = c_root;
}

void osage_edit_chain_exchange(osage_chain_slot *chain, int dir)
{
  int i;
  int *cmd;
  osage_param *rot;
  osage_param *params;
  int *coli_index;
  int op;
  osage_joint_desc *joint;
  int node;

  i = 0;
  cmd = chain->cmds;
  rot = (osage_param *)&rot_type;
  params = (osage_param *)chain->params;
  coli_index = osage_edit_coli_index;
  do {
    op = cmd[0];
    joint = (osage_joint_desc *)cmd[1];
    if (op == 3 || op == 0) {
      node = joint->param;
      if (dir == 0) {
        params[node] = rot[i];
        coli_index[node] = ((struct { char c[1]; } *)&coli_num)->c[i];
      }
      else {
        rot[i] = params[node];
        ((struct { char c[1]; } *)&coli_num)->c[i] = coli_index[node];
      }
      i++;
    }
    cmd += 2;
  } while (op != 0);
}

void osage_debug_draw(rob_rec *rob)
{
  uchar side;
  int i;
  int count;
  int labelled;
  uint color;
  obj_node *node;
  char *pc;

  side = rob->side;
  count = 0;
  if (side == 0) {
  mtx_push(0);
  for (i = 0; i < 8; i++) {
    if (osage_coli_edit[i].on != 0) {
      osage_coli_draw(osage_coli_edit[i].recs, rob);
    }
  }
  mtx_pop(1);
  if (bone_sw != 0) {
  mtx_push(0);
  mtx_save_proj_x_current(0.0f, 0.0f);
  osage_edit_targets_update();
  osage_debug_model_name[0] = 0;
  for (i = 0; i < 0x10; i++) {
    osage_debug_labels[i][0] = 0;
  }
  for (i = 0; i < 0x40; i++) {
    int *tbl = &osage_chains[0][0].on;
    if (tbl[i * 7 + side * 0x1c0] != 0 && tbl[i * 7 + side * 0x1c0 + 6] == osage_edit_targets[part_0c2edba8]) {
      labelled = -1;
      color = osage_chain_color;
      node = obj_node_get(tbl[i * 7 + side * 0x1c0 + 6]);
      screen_line_width_set(1.0f);
      if (count == c_root) {
        labelled = 0;
        color = osage_chain_color_selected;
        screen_line_width_set(osage_chain_width_selected);
        sprintf(osage_debug_model_name, "%s", node->name);
        pc = strstr(osage_debug_model_name, "_o_body");
        if (pc != 0) {
          *pc = 0;
        }
        sprintf(osage_debug_file_line, "osage_%s.txt\n", osage_debug_model_name);
      }
      osage_debug_chain_draw(&osage_chains[side][i], color, labelled);
      count++;
    }
  }
  mtx_pop(1);
  osage_coli_off = 0;
  }
  }
}

void osage_debug_chain_draw(osage_chain_slot *chain, uint color, int labelled)
{
  typedef struct { int w[3]; } vec;
  int *cmd;
  osage_joint_desc *joint;
  osage_node *node;
  vec prev;
  int go;
  int count;
  int named;

  cmd = chain->cmds;
  go = 1;
  node = (osage_node *)chain->nodes;
  count = 0;
  osage_debug_color = color;
  do {
    joint = (osage_joint_desc *)cmd[1];
    named = 0;
    switch (*cmd) {
    case 1:
      if (labelled != -1) {
        debug_cross_label_draw(node->pos,e_names_0c1e31cc_names[chain->root_slot],0xffffffff);
      }
      else {
        debug_cross_label_draw(node->pos,0,0xff7f7f7f);
      }
      prev = *(vec *)node->pos;
      node++;
      break;
    case 3:
      if (labelled != -1) {
        named = 1;
        sprintf(osage_debug_labels[count],"j_ex_%02d",joint->param);
        count++;
      }
      osage_debug_link_draw(joint,node,(float *)&prev,named);
      node++;
      break;
    case 0:
      if (labelled != -1) {
        named = 1;
        sprintf(osage_debug_labels[count],"j_ex_%02d",joint->param);
      }
      osage_debug_link_draw(joint,node,(float *)&prev,named);
      go = 0;
      break;
    }
    cmd += 2;
  } while (go);
  text_color_set(0xffffffff);
}

void osage_debug_link_draw(osage_joint_desc *joint, osage_node *node, float *prev, int labelled)
{
  typedef struct { int w[3]; } blk;
  float a[2];
  float b[2];

  proj_point_to_pixel_xy(prev,a);
  proj_point_to_pixel_xy(node->pos,b);
  screen_line_draw(a[0],a[1],b[0],b[1],debug_line_depth,osage_debug_color);
  screen_diamond_draw(b[0],b[1],2.0,debug_line_depth,0xffff0000);
  if (labelled != 0) {
    text_cursor_put((int)((a[0] + b[0]) * 0.5),(int)((a[1] + b[1]) * 0.5));
    text_color_set(osage_debug_color);
    text_draw_formatted("j_ex_%02d",joint->param);
  }
  *(blk *)prev = *(blk *)node->pos;
}

void osage_chain_coli_attach(osage_chain_slot *chain)
{
  int go;
  int node;
  int coli;
  int *cmd;
  osage_param *params;
  osage_coli_edit_rec *tables;
  int *coli_index;

  cmd = chain->cmds;
  params = (osage_param *)chain->params;
  go = 1;
  tables = osage_coli_edit;
  coli_index = osage_edit_coli_index;
  do {
    node = ((osage_joint_desc *)cmd[1])->param;
    switch (*cmd) {
    case 3:
      coli = coli_index[node];
      if (coli != -1) {
        params[node].coli_list = (uint)tables[coli].recs;
      }
      else {
        params[node].coli_list = 0;
      }
      break;
    case 0:
      coli = coli_index[node];
      if (coli != -1) {
        params[node].coli_list = (uint)tables[coli].recs;
      }
      else {
        params[node].coli_list = 0;
      }
      go = 0;
      break;
    }
    cmd += 2;
  } while (go);
}

void screen_diamond_draw(float x, float y, float size, float depth, uint color)
{
  struct { float x, y, z; uint c; } v[4];
  depth = 1.0 / depth;
  v[0].x = x - size;
  v[0].y = y;
  v[1].x = x;
  v[1].y = y - size;
  v[2].x = x;
  v[2].y = y + size;
  v[3].x = x + size;
  v[3].y = y;
  v[0].z = v[1].z = v[2].z = v[3].z = depth;
  v[0].c = color;
  v[1].c = color;
  v[2].c = color;
  v[3].c = color;
  polystrip_plain_opaque(4,(uint *)v);
}

void osage_edit_restore_selected(void)
{
  int i;
  int side;

  side = 0; /* ORIGINAL SHAPE (corroborated 2026-11-10: forced: a literal [0] folds the row away, and the game keeps the multiply by the zero; its caller ex_osage_new_update writes the same row index): a row index that stays 0 */
  for (i = 0; i < 0x40; i++) {
    if (osage_chains[side][i].on != 0) {
      if (osage_chains[side][i].model_id == osage_edit_targets[part_0c2edba8]) {
        osage_edit_chain_restore(&osage_chains[side][i]);
      }
    }
  }
  osage_edit_restore_request = 0;
}

void osage_edit_chain_restore(osage_chain_slot *chain)
{
  int *cmd;
  osage_param *params;
  int op;
  int node;
  osage_joint_desc *joint;
  osage_param *rot;
  int *coli_index;

  cmd = chain->cmds;
  rot = (osage_param *)&rot_type;
  params = (osage_param *)chain->params;
  coli_index = osage_edit_coli_index;
  do {
    op = *cmd;
    joint = (osage_joint_desc *)cmd[1];
    if (op == 3 || op == 0) {
      node = joint->param;
      params[node] = rot[node];
      {
        int c = (int)&coli_num;
        coli_index[node] = *(char *)(c + node);
      }
    }
    cmd = cmd + 2;
  } while (op != 0);
}

void osage_chain_nodes_mark(osage_chain_slot *chain, int *marks)
{
  int op;
  osage_joint_desc *joint;
  int *cmd;

  for (cmd = chain->cmds; ; cmd += 2) {
    op = cmd[0];
    joint = (osage_joint_desc *)cmd[1];
    if (op == 3 || op == 0) {
      marks[joint->param] = 1;
    }
    if (op == 0) break;
  }
}

void make_osage_param_text(void)
{
  strbuf_rec sb;
  strbuf_rec *bp;
  char name[64];
  char *p;
  int model;
  obj_node *node;
  osage_coli_rec *e;
  osage_param *j;
  int i;
  int idx;
  int pl = 0; /* ORIGINAL SHAPE (corroborated 2026-11-10: forced: a literal [0] folds the row away; the game multiplies 0x700 by a zero it re-reads from r10, and ex_osage_new_update above has the same zero row index): a row index that stays 0 */

  bp = &sb;
  model = osage_edit_targets[part_0c2edba8];
  node = obj_node_get(model);
  if (node == 0) return;
  strcpy(osage_param_model_name,node->name);
  p = strstr(osage_param_model_name,"_o_body");
  if (p != 0) *p = 0;
  p = osage_param_model_name;
  strbuf_init(bp,osage_param_text,0x2000,0,0);
  for (i = 0; i < 8; i++) {
    e = osage_coli_edit[i].recs;
    if (e->type != 0) {
      strbuf_printf(bp,"static CLOTH_COLI coli_%s_%d[] = \n{\n",p,i);
      for (; e->type != 0; e++) {
        strbuf_printf(bp,"    {CLOTH_COLI_%s,",osage_coli_type_names[e->type]);
        strbuf_printf(bp,"VFX_DISP_MAT_%s,\t",e_names_0c1e31cc_names[e->slot]);
        strbuf_printf(bp,"{%.2f,%.2f,%.2f,\t%.2f,\t%.2f,%.2f} },\n",e->shape.pos[0],e->shape.pos[1],e->shape.pos[2],e->shape.radius,e->shape.lo,e->shape.hi);
      }
      strbuf_printf(bp,"    {CLOTH_COLI_END, 0, {0,0,0,0,0,0} }\n};\n");
    }
  }
  for (i = 0; i < 100; i++) {
    osage_param_node_marks[i] = 0;
  }
  for (i = 0; i < 0x40; i++) {
    if (osage_chains[pl][i].on != 0 && osage_chains[pl][i].model_id == model) {
      osage_chain_nodes_mark(&osage_chains[pl][i],osage_param_node_marks);
    }
  }
  j = j_ex_00[0];
  strbuf_printf(bp,"OSAGE_COLI_TBL2 osp_%s[] = \n{\n",p);
  for (i = 0; i < 100; j++, i++) {
    if (osage_param_node_marks[i] != 0) {
      int b = (int)osage_edit_coli_index; /* ORIGINAL SHAPE (corroborated 2026-11-10: regclass: held in an int the table address gets the register class the game's code shows; a pointer local does not): block-local table address */
      idx = *(int *)(b + i * 4);
      if (idx >= 0 && osage_coli_edit[idx].recs[0].type != 0) {
        sprintf(name,"coli_%s_%d",p,idx);
      }
      else {
        sprintf(name,"NULL");
      }
      strbuf_printf(bp,"{%d, {%s, 0x%04x, %f, %f, %f, 0x%04x, 0x%04x, %s} },\n",i,e_rottype_names[(short)j->rot_type],j->limit,j->stiffness,j->damping,j->gravity,j->rest_ry,j->rest_rz,name);
    }
  }
  strbuf_printf(bp,"{-1, {ROTTYPE_ZY, 0x0000, 0.010000, 0.000000, 0.000000, 0x0000, 0x0000, NULL} },\n");
  strbuf_printf(bp,"};\n\n");
  osage_param_text_len = strlen(osage_param_text);
  strbuf_release_nop(bp);
  sprintf(osage_cmd_save_text,"shell mv tmp.txt osage2_%s.txt\n",p);
  osage_cmd_save_text_len = strlen(osage_cmd_save_text);
  sprintf(osage_cmd_save_coli,"shell mv tmp.bin ../data/param/osage_coli_%s.bin\n",p);
  osage_cmd_save_coli_len = strlen(osage_cmd_save_coli);
  sprintf(osage_cmd_load_coli,"shell cp ../data/param/osage_coli_%s.bin tmp.bin\n",p);
  osage_cmd_load_coli_len = strlen(osage_cmd_load_coli);
  sprintf(osage_cmd_save_param_text,"shell mv tmp.txt osage_param_%s.txt\n",p);
  osage_cmd_save_param_text_len = strlen(osage_cmd_save_param_text);
  sprintf(osage_cmd_save_param,"shell mv tmp.bin ../data/param/osage_param_%s.bin\n",p);
  osage_cmd_save_param_len = strlen(osage_cmd_save_param);
  sprintf(osage_cmd_load_param,"shell cp ../data/param/osage_param_%s.bin tmp.bin\n",p);
  osage_cmd_load_param_len = strlen(osage_cmd_load_param);
  sprintf(osage_cmd_save_coli_idx,"shell mv tmp.bin ../data/param/osage_coli_idx_%s.bin\n",p);
  osage_cmd_save_coli_idx_len = strlen(osage_cmd_save_coli_idx);
  sprintf(osage_cmd_load_coli_idx,"shell cp ../data/param/osage_coli_idx_%s.bin tmp.bin\n",p);
  osage_cmd_load_coli_idx_len = strlen(osage_cmd_load_coli_idx);
  window_trap();
}

void rob_zanzou_heaps_alloc(void)
{
  int j;
  int i;
  
  i = 0;
  do {
    j = 0;
    do {
      rob_zanzou_elan_bufs[i][j] = elan_heap_alloc(0x1000);
      j = j + 1;
    } while (j < 9);
    i = i + 1;
  } while (i < 2);
  return;
}

void rob_zanzou_trails_init(void)
{
  int j;
  int i;
  void **t;
  
  i = 0;
  t = rob_zanzou_elan_bufs[0];
  for (; i < 2; i++) {
    for (j = 0; j < 9; j++) {
      zanzou_trail_init(&rob_zanzou_trails[i][j],0x10012,
                   t[i * 9 + j],&rob_zanzou_work + i * 0x6c00 + j * 0xc00);
    }
  }
  item_zanzou_trails_init();
}

void rob_zanzou_slots_clear(void)
{
  int i, j; for (i = 0; i < 2; i++) { int *p = &rob_zanzou_slots[0][0][0]; for (j = 0; j < 9; j++) p[(i * 9 + j) * 4] = 0; }
}

void rob_zanzou_calc_all(void)
{
  rob_rec *rob;
  uint held;
  int i;
  
  rob = rob_get(0);
  held = pause_frame_held();
  if ((held == 0) && (rob_zanzou_on != 0)) {
    mtx_push(0);
    i = 0;
    do {
      rob_zanzou_calc(rob);
      i = i + 1;
      rob = rob + 1;
    } while (i < 2);
    mtx_pop(1);
  }
  rob_zanzou_make_all();
  return;
}

void rob_zanzou_calc(rob_rec *rob)
{
  vec3 a;
  vec3 b;
  vec3 buf[2][8];
  vec3 cd[2][10];
  float start;
  float *mtx;
  uint side;
  zanzou_trail_rec *trail;
  int i;
  int j;
  int m;
  int *bones;

  start = (int)stopwatch_usec();
  mtx = rob->mtx;
  trail = &rob_zanzou_trails[rob->side][0];
  side = rob->side;
  for (i = 0; i < 9; i++, trail++) {
    if (trail->on != 0) {
      bones = rob_zanzou_bones[i];
      mtx_set_current_wordwise(mtx + bones[0] * 0x10);
      mtx_xform_point((float *)&rob_zanzou_edge_point,(float *)&a);
      mtx_set_current_wordwise(mtx + bones[1] * 0x10);
      mtx_xform_point((float *)&rob_zanzou_edge_point,(float *)&b);
      if (zanzou_cam_mode_last != camera.mode_last) {
        if ((&rob_zanzou_slots[0][0][0])[(side * 9 + i) * 4] != 0) {
          zanzou_trail_restart(trail,&a,&b,1);
          rob_zanzou_history[side][i].edge_a[2] = rob_zanzou_history[side][i].edge_a[1] = rob_zanzou_history[side][i].edge_a[0] = a;
          rob_zanzou_history[side][i].edge_b[2] = rob_zanzou_history[side][i].edge_b[1] = rob_zanzou_history[side][i].edge_b[0] = b;
        }
        else {
          trail->on = 0;
        }
      }
      else if (rob_zanzou_spline_mode == 1) {
        rob_zanzou_history[side][i].edge_a[3] = rob_zanzou_history[side][i].edge_a[2];
        rob_zanzou_history[side][i].edge_a[2] = rob_zanzou_history[side][i].edge_a[1];
        rob_zanzou_history[side][i].edge_a[1] = rob_zanzou_history[side][i].edge_a[0];
        rob_zanzou_history[side][i].edge_a[0] = a;
        rob_zanzou_history[side][i].edge_b[3] = rob_zanzou_history[side][i].edge_b[2];
        rob_zanzou_history[side][i].edge_b[2] = rob_zanzou_history[side][i].edge_b[1];
        rob_zanzou_history[side][i].edge_b[1] = rob_zanzou_history[side][i].edge_b[0];
        rob_zanzou_history[side][i].edge_b[0] = b;
        zanzou_spline_points4(&buf[0][0].x,rob_zanzou_spline_points,(float *)rob_zanzou_history[side][i].edge_a);
        zanzou_spline_points4(&buf[1][0].x,rob_zanzou_spline_points,(float *)rob_zanzou_history[side][i].edge_b);
        zanzou_trail_cur_shift(trail,1);
        for (j = 0; j < (int)rob_zanzou_spline_points; j++) {
          zanzou_trail_push(trail,&buf[0][j],&buf[1][j]);
        }
        zanzou_trail_push(trail,&rob_zanzou_history[side][i].edge_a[1],&rob_zanzou_history[side][i].edge_b[1]);
        zanzou_trail_push(trail,&a,&b);
      }
      else if (rob_zanzou_spline_mode == 2) {
        uint k = rob->side;
        rob_zanzou_history[k][i].edge_a[2] = rob_zanzou_history[k][i].edge_a[1];
        rob_zanzou_history[k][i].edge_a[1] = rob_zanzou_history[k][i].edge_a[0];
        rob_zanzou_history[k][i].edge_a[0] = a;
        rob_zanzou_history[k][i].edge_b[2] = rob_zanzou_history[k][i].edge_b[1];
        rob_zanzou_history[k][i].edge_b[1] = rob_zanzou_history[k][i].edge_b[0];
        rob_zanzou_history[k][i].edge_b[0] = b;
        zanzou_spline_points3(&cd[0][1].x,rob_zanzou_spline_points,(float *)rob_zanzou_history[k][i].edge_a);
        zanzou_spline_points3(&cd[1][1].x,rob_zanzou_spline_points,(float *)rob_zanzou_history[k][i].edge_b);
        cd[0][rob_zanzou_spline_points + 1] = a;
        cd[1][rob_zanzou_spline_points + 1] = b;
        for (m = 1; m <= (int)(rob_zanzou_spline_points + 1); m++) {
          zanzou_trail_push(trail,&cd[0][m],&cd[1][m]);
        }
      }
    }
  }
  if (rob_zanzou_timing_on != 0) {
    text_cursor_to_cell(2,rob->side + 0x2b);
    start = (float)(int)stopwatch_usec() - start;
    text_draw_formatted("%f rob_zanzou\n",start);
  }
}

void rob_zanzou_make_all(void)
{
  int j;
  int i;
  
  i = 0;
  do {
    j = 0;
    do {
      zanzou_trail_make(&rob_zanzou_trails[i][j]);
      j = j + 1;
    } while (j < 9);
    i = i + 1;
  } while (i < 2);
  return;
}

void rob_zanzou_start(rob_rec *rob, int trail, int value)
{
  typedef struct { int on; int field_0x4; int field_0x8; int value; } zanzou_slot;
  zanzou_trail_rec *rec;
  float *mtx;
  int *bones;
  zanzou_slot *slots;
  vec3 v0;
  vec3 v1;
  rob_rec *lrob;
  int ltrail;
  int lvalue;
  lrob = rob;
  ltrail = trail;
  lvalue = value;
  rec = &rob_zanzou_trails[(uint)rob->side][trail];
  mtx = rob->mtx;
  bones = rob_zanzou_bones[trail];
  mtx_set_current_wordwise(mtx + bones[0] * 0x10);
  mtx_xform_point((float *)&rob_zanzou_edge_point,&v0.x);
  mtx_set_current_wordwise(mtx + bones[1] * 0x10);
  mtx_xform_point((float *)&rob_zanzou_edge_point,&v1.x);
  zanzou_trail_restart(rec,&v0,&v1,1);
  slots = (zanzou_slot *)&rob_zanzou_slots[0][0][0];
  slots[(uint)lrob->side * 9 + ltrail].on = 1;
  slots[(uint)lrob->side * 9 + ltrail].field_0x8 = 0;
  slots[(uint)lrob->side * 9 + ltrail].value = lvalue;
}

void rob_zanzou_motion_nop(rob_rec *rob)
{
  int i;
  
  i = 0;
  do {
    i = i + 1;
  } while (i < 9);
  return;
}

void rob_zanzou_disp(rob_rec *rob)
{
  float t;
  int i;
  
  t = (float)(int)stopwatch_usec();
  if (rob_zanzou_on != 0) {
    for (i = 0; i < 9; i++) {
      zanzou_trail_draw(&rob_zanzou_trails[(uint)rob->side][i]);
    }
    if (rob_zanzou_timing_on != 0) {
      t = (float)(int)stopwatch_usec() - t;
      text_cursor_to_cell(2,rob->side + 0x2d);
      text_draw_formatted("%f disp_zanzou\n",t);
    }
  }
}

void item_zanzou_heaps_alloc(void)
{
  char *elan;
  char *model;
  int i;
  int j;
  elan = elan_heap_alloc(0xf000);
  model = elan_heap_alloc(0x1e00);
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 10; j++) {
      item_zanzou_elan_bufs[i][j] = (compiled_model_rec *)elan;
      item_zanzou_model_bufs[i][j] = (compiled_model_rec *)model;
      elan += 0xc00;
      model += 0x180;
    }
  }
}

void item_zanzou_trails_init(void)
{
  obj_node *obj0;
  obj_node *obj1;
  int i;
  int j;
  uint size;

  obj0 = obj_node_get(item_zanzou_objs[0]);
  obj1 = obj_node_get(item_zanzou_objs[1]);
  size = model_size((int *)obj0->model) + 0x1fU & 0xffffffe0;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 8; j++) {
      zanzou_trail_init(&item_zanzou_trails[i][j],item_zanzou_trail_ids.item_trail,
                   item_zanzou_elan_bufs[i][j],&item_zanzou_work + i * 0xf000 + j * 0x1800);
      memcpy(item_zanzou_model_bufs[i][j],obj0->model,size);
    }
    for (j = 8; j < 10; j++) {
      zanzou_trail_init(&item_zanzou_trails[i][j],item_zanzou_trail_ids.fixed_trail,
                   item_zanzou_elan_bufs[i][j],&item_zanzou_work + i * 0xf000 + j * 0x1800);
      memcpy(item_zanzou_model_bufs[i][j],obj1->model,size);
    }
  }
}

void item_zanzou_setup(void)
{
  typedef struct { int v[3]; } V3;
  int zero[3];
  int i;
  rob_rec *rob;
  int j;
  uint *entry;
  uint *dir;
  uint *item;
  card_record_rec *card;
  char *items;
  int *trails;
  int o2;
  int sa = 0x1b8;
  int sb = 0x230;

  rob = rob_get(0);
  zero[0] = 0;
  zero[1] = 0;
  zero[2] = 0;
  item_zanzou_hold = 8;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 10; j++) {
      zanzou_trail_restart(&item_zanzou_trails[i][j],(vec3 *)zero,(vec3 *)zero,1);
      item_zanzou_trails[i][j].on = 0;
      clr_pair((char *)item_zanzou_items, i, j);
    }
  }
  for (i = 0; i < 2; i++, rob++) {
  if ((rob->flags & 0x40000000) == 0) {
    entry = 0;
    for (j = 0; j < 30; j++) {
      char *table = &item_zanzou_table;
      uint *ent = (uint *)(table + rob->costume * 0x7bc + j * 0x84);
      if (*ent == rob->chara) {
        entry = ent;
        break;
      }
    }
    if (entry != 0) {
      for (j = 0; j < 4; j++) {
        item = &entry[j * 8 + 1];
        if (rob_card_item_is_50_or_64(rob,j) != 0 || item_zanzou_force != 0) {
          if (j == 3) dir = (uint *)&item_zanzou_dir_3;
          else dir = (uint *)&item_zanzou_dir;
          if ((int)*item >= 0) {
            char *as = (char *)&item_zanzou_trails[0][0];
            char *bs = (char *)item_zanzou_items;
            *(int *)(as + (j * 0x58 + i * 0x1b8)) = 1;
            ((zanzou_item *)(bs + (j * 0x70 + i * 0x230)))->on = 1;
            ((zanzou_item *)(bs + (j * 0x70 + i * 0x230)))->bone = *item;
            *(V3 *)((zanzou_item *)(bs + (j * 0x70 + i * 0x230)))->point = *(V3 *)(item + 1);
            *(V3 *)((zanzou_item *)(bs + (j * 0x70 + i * 0x230)))->dir = *(V3 *)dir;
          }
        }
      }
    }
    if (rob->chara == 0xd) {
      card = card_record_get((uchar)(rob->side ^ 1));
      if (item_zanzou_force != 0 || (card->in_use != 0 && card->header.all_win % 10 == 9)) {
        trails = (int *)((char *)&item_zanzou_trails[0][0] + i * sa);
        o2 = 8 * 0x2c;
        *(int *)((char *)trails + o2) = 1;
        items = (char *)item_zanzou_items + i * sb;
        o2 = 8 * 0x38;
        *(int *)(items + o2) = 1;
        o2 = 8 * 0x38 + 0xc;
        *(int *)(items + o2) = 3;
        *(V3 *)(items + 8 * 0x38 + 0x14) = *(V3 *)&zanzou_fixed_point_8;
        *(V3 *)(items + 8 * 0x38 + 0x20) = *(V3 *)&zanzou_fixed_dir_8;
        o2 = 9 * 0x2c;
        *(int *)((char *)trails + o2) = 1;
        o2 = 9 * 0x38;
        *(int *)(items + o2) = 1;
        o2 = 9 * 0x38 + 0xc;
        *(int *)(items + o2) = 3;
        *(V3 *)(items + 9 * 0x38 + 0x14) = *(V3 *)&zanzou_fixed_point_9;
        *(V3 *)(items + 9 * 0x38 + 0x20) = *(V3 *)&zanzou_fixed_dir_9;
      }
    }
  }
  }
  zanzou_cam_mode_last = 0xffffffff;
}

void item_zanzou_update(void)
{
  int held;
  
  held = pause_frame_held();
  if ((held == 0) || (item_zanzou_force != 0)) {
    item_zanzou_calc();
  }
  item_zanzou_make();
  return;
}

void item_zanzou_calc(void)
{
  float pos[3];
  float screen[3];
  float dir[3];
  float pts[10][3];
  float (*q)[3];
  float vd[3];
  float ve[3];
  float vf[3];
  rob_rec *rob;
  float *mtx;
  int side;
  int i;
  int j;
  int restart;
  int off;
  zanzou_trail_rec *trail_rec;
  zanzou_item *it;
  item_zanzou_hist_rec *hist;
  float start;
  float fade;
  float dot;
  float len;
  float tmp;

  rob = rob_get(0);
  mtx_push(0);
  camera_view_load();
  mtx_save_proj_x_current(0.0,0.0);
  for (side = 0; side < 2; side++, rob++) {
    start = (float)(int)stopwatch_usec();
    mtx = rob->mtx;
    if (rob->chara == 13) {
      item_zanzou_dural_nop(rob);
    }
    for (i = 0; i < 10; i++) {
      trail_rec = &item_zanzou_trails[side][i];
      restart = 0;
      if (trail_rec->on != 0) {
        it = &item_zanzou_items[side][i];
        it->vis = 1;
        mtx_set_current_wordwise(mtx + it->bone * 0x10);
        mtx_xform_point(it->point,pos);
        mtx_xform_dir(it->dir,dir);
        vec3_sub((float *)&item_zanzou_eye_dir,&camera.eye.x,pos);
        dot = vec3_dot((float *)&item_zanzou_eye_dir,dir);
        len = vec3_length((float *)&item_zanzou_eye_dir);
        dot = dot / (len * vec3_length(dir));
        fade = ramp_ratio(dot,item_zanzou_fade_top,item_zanzou_fade_bottom);
        if (fade < 0.0) restart = 1;
        if (zanzou_cam_mode_last != camera.mode_last) restart = 1;
        mtx_set_current_wordwise(proj_x_model_matrix);
        zanzou_point_project(pos,screen);
        fade = fade * item_zanzou_width / screen[2];
        screen[2] = zanzou_screen_z;
        vec3_scale(screen,screen,zanzou_screen_scale);
        *(vec3 *)it->screen = *(vec3 *)screen;
        it->size = item_zanzou_size * fade;
        if (restart) {
          if (it->on != 0) {
            zanzou_trail_restart(trail_rec,(vec3 *)screen,(vec3 *)screen,1);
            item_zanzou_history[side][i].pos[2] = item_zanzou_history[side][i].pos[1] = item_zanzou_history[side][i].pos[0] = *(vec3 *)screen;
          }
          else {
            trail_rec->on = 0;
          }
          it->vis = 0;
        }
        else {
          switch (item_zanzou_spline_mode) {
          case 1:
            break;
          case 2:
            off = side * 360 + i * 36;
            hist = (item_zanzou_hist_rec *)((char *)item_zanzou_history + off);
            hist->pos[2] = hist->pos[1];
            hist->pos[1] = hist->pos[0];
            hist->pos[0] = *(vec3 *)screen;
            if (vec3_distance(screen,&((item_zanzou_hist_rec *)((char *)item_zanzou_history + off))->pos[1].x) > item_zanzou_min_dist) {
              zanzou_spline_points3(pts[1],item_zanzou_spline_points,(float *)hist);
              *(vec3 *)pts[0] = hist->pos[1];
              q = pts;
              *(vec3 *)q[item_zanzou_spline_points + 1] = hist->pos[0];
              for (j = 1; j <= item_zanzou_spline_points + 1; j++) {
                vec3_sub(vd,pts[j],pts[j - 1]);
                vec3_normalize(vd);
                tmp = fade * vd[0];
                vd[0] = -(vd[1] * fade);
                vd[1] = tmp;
                vec3_add(ve,pts[j],vd);
                vec3_sub(vf,pts[j],vd);
                zanzou_trail_push(trail_rec,(vec3 *)ve,(vec3 *)vf);
              }
            }
            else {
              zanzou_trail_push(trail_rec,(vec3 *)screen,(vec3 *)screen);
            }
            break;
          }
        }
      }
    }
    if (item_zanzou_timing_on != 0) {
      text_cursor_to_cell(2,side + 0x30);
      start = (float)(int)stopwatch_usec() - start;
      text_draw_formatted("%f calc_item_zanzou\n",start);
    }
  }
  mtx_pop(1);
}

void item_zanzou_make(void)
{
  float start;
  int i;
  int side;

  for (side = 0; side < 2; side++) {
    start = (float)(int)stopwatch_usec();
    for (i = 0; i < 10; i++) {
      zanzou_trail_make(&item_zanzou_trails[side][i]);
    }
    if (item_zanzou_timing_on != 0) {
      text_cursor_to_cell(2,side + 0x32);
      start = (float)(int)stopwatch_usec() - start;
      text_draw_formatted("%f make_item_zanzou\n",start);
    }
  }
}

void item_zanzou_disp(void)
{
  float start = (float) ((int) stopwatch_usec());
  rob_rec *rob;
  zanzou_item *item;
  int i;
  int j;
  if (item_zanzou_hold > 0)
  {
    item_zanzou_hold--;
  }
  zanzou_proj_begin();
  for (i = 0; i < 2; i++)
  {
    rob = rob_get((byte) i);
    if ((rob->flags & 1) != 0)
    {
      for (j = 0; j < 10; j++)
      {
        item = &item_zanzou_items[i][j];
        if (item_zanzou_hold <= 0)
        {
          zanzou_trail_draw(&item_zanzou_trails[rob->side][j]);
        }
        if (item->vis != 0)
        {
          mtx_push(0);
          mtx_translate_by_vec3(item->screen);
          mtx_scale_xyz(item->size * item_zanzou_draw_scale, item->size * item_zanzou_draw_scale, zanzou_screen_scale);
          model_draw(item_zanzou_model_bufs[i][j]);
          mtx_pop(1);
        }
      }

    }
  }

  zanzou_proj_end();
  if (spline_test_on != 0)
  {
    spline_test_draw();
  }
  if (item_zanzou_timing_on != 0)
  {
    start = ((float) ((int) stopwatch_usec())) - start;
    text_cursor_to_cell(2, 0x34);
    text_draw_formatted("%f disp_item_zanzou\n", start);
  }
}

void zanzou_spline_points4(float *out, int count, float *hist)
{
  int i;
  int j;
  float step;
  float t;
  float m0[3];
  float m1[3];
  float keys[8];
  float *q;
  float *p0;
  float *p1;
  float *p2;
  float *p3;

  step = 1.0 / (float)(count + 1);
  q = hist + 3;
  vec3_sub(m0,q,hist + 9);
  vec3_sub(m1,hist,hist + 6);
  keys[0] = 0.0;
  keys[4] = 1.0;
  p0 = m0;
  p1 = m1;
  p2 = hist + 6;
  p3 = q;
  for (j = 0; j < 3; j++, p0++, p1++, p2++, p3++, out++) {
    keys[1] = keys[2] = *p0;
    keys[5] = keys[6] = *p1;
    keys[3] = *p2;
    keys[7] = *p3;
    t = step;
    for (i = 0; i < count; i++) {
      out[i * 3] = curve_sample(keys,2,t);
      t = t + step;
    }
  }
  return;
}

void zanzou_spline_points3(float *out, int count, float *hist)
{
  float *prev;
  int i;
  int k;
  float step;
  float t;
  float a[3];
  float b[3];
  float keys[8];
  float *pa;
  float *pb;
  float *pc;
  float *pd;

  step = 1.0 / (float)(count + 1);
  prev = hist + 3;
  vec3_sub(a,prev,hist + 6);
  vec3_sub(b,hist,prev);
  keys[0] = 0.0;
  keys[4] = 1.0;
  for (pa = a, pb = b, pc = prev, pd = hist, i = 0; i < 3; i++, pa++, pb++, pc++, pd++, out++) {
    keys[1] = keys[2] = *pa;
    keys[5] = keys[6] = *pb;
    keys[3] = *pc;
    keys[7] = *pd;
    t = step;
    for (k = 0; k < count; k++) {
      out[k * 3] = curve_sample(keys,2,t);
      t = t + step;
    }
  }
}

void spline_point_eval(float *out_point, float *points, int index, int count, float t)
{
  float keys[8];
  float *p;
  float *out;
  int i;
  float d1;
  float d2;

  out = out_point;
  p = points + index * 3;
  for (i = 0; i < 3; i++, out++) {
    if (index > 1) d1 = p[0] - p[-6];
    else d1 = 0.0;
    if (index < count) d2 = p[3] - p[-3];
    else d2 = 0.0;
    keys[0] = 0.0;
    keys[4] = 1.0;
    keys[3] = p[-3];
    keys[7] = *p++;
    keys[1] = keys[2] = d1;
    keys[5] = keys[6] = d2;
    *out = curve_sample(keys,2,t);
  }
}

void spline_test_draw(void)
{
  int i;
  int next;
  float pt_c[3];
  float pt_a[3];
  float pt_b[3];

  line3d_width_store(spline_test_width);
  for (i = 1; i < 4; i++) {
    line3d_quads_opaque(spline_test_points[i - 1],spline_test_points[i],spline_test_color);
  }
  i = 0;
  do {
    next = i + 1;
    spline_point_eval(pt_a,spline_test_points[0],next,4,0.25);
    spline_point_eval(pt_b,spline_test_points[0],next,4,0.5);
    spline_point_eval(pt_c,spline_test_points[0],next,4,0.75);
    line3d_width_store(spline_test_curve_width);
    line3d_quads_opaque(spline_test_points[i],pt_a,spline_test_curve_color);
    line3d_quads_opaque(pt_a,pt_b,spline_test_curve_color);
    line3d_quads_opaque(pt_b,pt_c,spline_test_curve_color);
    line3d_quads_opaque(pt_c,spline_test_points[i + 1],spline_test_curve_color);
    i = next;
  } while (next < 4);
}

float ramp_ratio(float x, float top, float bottom)
{
  if (!(x <= top)) {
    return 1.0;
  }
  return (x - bottom) / (top - bottom);
}

void item_zanzou_dural_nop(rob_rec *rob)
{
  return;
}

void model_color_list_tint(obj_node *node, char *buf, uint color)
{
  uint *p;
  uint offset;
  uint rec_color;
  char *d;
  uint tint;
  if (buf != 0) {
    p = (uint *)(node->tex_refs_offset + (int)node->file);
    while (*p != 0) {
      offset = *p;
      rec_color = p[1];
      tint = argb_mul(rec_color,color);
      d = buf + (offset >> 2) * 4;
      *(uint *)(d - 0x54) = tint;
      tint = argb_mul(rec_color,color);
      *(uint *)(d - 0x4c) = tint;
      p = p + 4;
    }
  }
}

int itmtst_item_set(int item)
{
  int items = itmtst_state.items;
  int count = itmtst_state.item_count;
  if (item < 0) return 0xffffffff;
  if (item >= count) return 0xffffffff;
  if (*(char *)(item * 0xc + items) == -1) return 0xffffffff;
  { int ent = item * 0xc + items; ent += itmtst_state.costume; if (*(char *)(ent + 1) == -1) return 0xffffffff; }
  (&item_1p_0)[*(char *)(item * 0xc + items)] = (char)item + 1;
  return 0;
}

void itmtst_slot_clear(int slot)
{
  char *slots;
  slots = (char *)&item_1p_0;
  slots[slot] = 0;
  return;
}

void itmtst_pause_set(byte on)
{
  if (!((char)on == '\0')) {
    pause_flags = pause_flags | 0x14;
  }
  else {
    pause_flags = pause_flags & 0xffffffeb;
  }
  return;
}

void itmtst_nop(int flag)
{
  return;
}

void itmtst_display_setup(void)
{
  itmtst_state.saved_pause_flags = pause_flags;
  rob_item_flags = rob_item_flags | 7;
  rob_color_flags = rob_color_flags | 3;
  shadow_erase = 0;
  console_disp = ON_OFF_OFF;
  stage_disp = ON_OFF_OFF;
  disp_sw = 0;
  (&disp_sw)[1] = 3;
  return;
}

void itmtst_display_restore(void)
{
  uint *p;
  pause_flags = itmtst_state.saved_pause_flags;
  shadow_erase = 1;
  console_disp = ON_OFF_ON;
  stage_disp = ON_OFF_ON;
  p = &disp_sw;
  p[0] = 0;
  p[1] = 0;
}

int itmtst_next_chara(void)
{
  rob_rec *rob;
  
  rob = rob_get(0);
  if ((*(uint *)rob & 0x00ffff00) == 0x10f00) return 0;
  itmtst_pause_set(1);
  disp_sw = 3;
  (&disp_sw)[1] = 3;
  robs_flags_clear();
  objbank_free(5);
  objbank_free(6);
  objbank_free(7);
  objbank_free(8);
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
  robs_match_clear();
  rob_reload_costume[0] = (char)rob->costume + 1;
  if (rob_reload_costume[0] > 1) {
    rob_reload_costume[0] = 0;
    rob_reload_chara[0] = (char)rob->chara + 1;
    if (rob_reload_chara[0] == 13) {
      rob_reload_chara[0]++;
    }
  }
  rob0_reload();
  rob1_reload();
  return 1;
}

void itmtst_start(void)
{
  typedef struct { short s; short pad; uchar *p; int r[2]; } ent16;
  int i;

  if (itmtst_state.step == 0) {
    itmtst_state.rob0 = 0;
    itmtst_state.rob1 = 0;
    itmtst_state.chara = 0;
    itmtst_state.costume = 0;
    itmtst_state.items = 0;
    itmtst_state.item_count = 0;
    itmtst_state.saved_pause_flags = 0;
    *(int *)&itmtst_state.step = 0;
    itmtst_state.rob0 = rob_get(0);
    itmtst_state.rob1 = rob_get(1);
    itmtst_state.chara = itmtst_state.rob0->chara;
    itmtst_state.costume = itmtst_state.rob0->costume;
    itmtst_state.items = (uint)((ent16 *)&chara_item_table)[itmtst_state.chara].p;
    itmtst_state.item_count = ((ent16 *)&chara_item_table)[itmtst_state.chara].s;
    memset(&itmtst_stats,0,0x1a4);
    itmtst_stats.item = -1;
    for (i = 0; i < 4; i++) {
      itmtst_stats.best[0].who[i] = -1;
      itmtst_stats.best[1].who[i] = -1;
      itmtst_stats.best[2].who[i] = -1;
      itmtst_stats.best[3].who[i] = -1;
      itmtst_stats.best[4].who[i] = -1;
      itmtst_stats.best[5].who[i] = -1;
      itmtst_stats.best[6].who[i] = -1;
      itmtst_stats.best[7].who[i] = -1;
    }
    memset(itmtst_log,0,0x7000);
    itmtst_log_len = 0;
    itmtst_stats.log = itmtst_log;
    sprintf(itmtst_log,
                 "item_name,         cpu, render,     [ta],tex(KB),altex(KB),    obj\n");
    itmtst_display_setup();
    itmtst_state.step = 1;
  }
}

int itmtst_step(void)
{
  int items;
  int ent;
  int ret = 1;
  int i;
  char buf[128];

  switch (itmtst_state.step) {
  case 0:
    ret = 0;
    break;
  case 1:
    for (i = 0; i < 4; i++) itmtst_slot_clear(i);
    for (;;) {
      switch (++itmtst_stats.item) {
      case 0xfa:
        itmtst_stats.mode = 1;
        break;
      case 0xfb:
        itmtst_stats.mode = 2;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[0].who[i]);
        break;
      case 0xfc:
        itmtst_stats.mode = 3;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[2].who[i]);
        break;
      case 0xfd:
        itmtst_stats.mode = 4;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[4].who[i]);
        break;
      case 0xfe:
        itmtst_stats.mode = 5;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[6].who[i]);
        break;
      case 0xff:
        itmtst_stats.mode = 6;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[1].who[i]);
        break;
      case 0x100:
        itmtst_stats.mode = 7;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[3].who[i]);
        break;
      case 0x101:
        itmtst_stats.mode = 8;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[5].who[i]);
        break;
      case 0x102:
        itmtst_stats.mode = 9;
        for (i = 0; i < 4; i++) itmtst_item_set(itmtst_stats.best[7].who[i]);
        break;
      case 0x103:
        itmtst_state.step = 5;
        return ret;
      default:
        if (itmtst_stats.item >= itmtst_state.item_count) continue;
        if ((items = itmtst_state.items, ent = itmtst_stats.item * 12 + items, *(char *)(ent + itmtst_state.costume + 1) == -1)) continue;
        if (itmtst_item_set(itmtst_stats.item) != 0) continue;
        break;
      }
      break;
    }
    itmtst_state.step++;
    break;
  case 2:
    itmtst_obj_counters_clear();
    rob_pol_send();
    itmtst_stats.frame = 0;
    itmtst_stats.sample = 0;
    itmtst_robs_reset();
    itmtst_pause_set(0);
    itmtst_nop(0);
    itmtst_state.step++;
    break;
  case 3:
    itmtst_camera_set((int)itmtst_stats.sample / 2);
    itmtst_stats.frame = itmtst_stats.frame + 1;
    if ((int)itmtst_stats.frame <= 120) break;
    itmtst_sample_store(itmtst_stats.sample);
    itmtst_stats.frame = itmtst_stats.frame - 30;
    itmtst_stats.sample = itmtst_stats.sample + 1;
    if ((int)itmtst_stats.sample <= 11) break;
    itmtst_state.step++;
    break;
  case 4:
    itmtst_pause_set(1);
    itmtst_nop(1);
    if (itmtst_stats.mode != 0) itmtst_summary_write();
    else itmtst_item_record();
    itmtst_state.step = 1;
    break;
  case 5:
    sprintf(buf, "itmtst_");
    strcat(buf, item_chara_names[itmtst_state.chara * 2 + itmtst_state.costume]);
    strcat(buf, ".txt");
    sprintf(itmtst_cmd_save_log, "shell mv tmp.txt %s\n", buf);
    itmtst_cmd_save_log_len = strlen(itmtst_cmd_save_log);
    itmtst_log_len = strlen(itmtst_stats.log);
    itmtst_state.step++;
    break;
  case 6:
    for (i = 0; i < 4; i++) itmtst_slot_clear(i);
    itmtst_display_restore();
    if (itmtst_all_charas != 0) {
      if (itmtst_next_chara() != 0) {
        itmtst_state.step = 0;
        itmtst_start();
      } else {
        itmtst_all_charas = 0;
        itmtst_state.step = 0;
      }
      break;
    }
    itmtst_state.step++;
    break;
  case 7:
    window_trap();
  default:
    itmtst_state.step = 0;
  }
  return ret;
}

void itmtst_robs_reset(void)
{
  itmtst_state.rob0->face_motion = 0;
  itmtst_state.rob1->face_motion = 0;
  itmtst_state.rob0->flags =
       itmtst_state.rob0->flags | 1;
  itmtst_state.rob1->flags =
       itmtst_state.rob1->flags | 1;
  rob_pos_reset();
  return;
}

int itmtst_item_slot_get(int item)
{
  return ((struct { uchar *a; char *p; char r[8]; } *)&chara_item_table)[itmtst_state.chara].p[item * 0xc];
}

void itmtst_item_info_get(int item, uint *number, char *flags)
{
  uint chara;
  uint costume;
  char *row;
  uchar *entry;
  uint num;
  char flag_bits;
  chara = itmtst_state.chara;
  costume = itmtst_state.costume;
  if (item < 0) {
    num = 0;
    flag_bits = 0;
  }
  else {
    chara = (uint)((struct { uchar *a; char *p; char r[8]; } *)&chara_item_table)[chara].p;
    row = (char *)(item * 0xc + chara);
    entry = *(uchar **)(row + costume * 4 + 4);
    if (row[costume + 1] == 1) { num = entry[0x14]; flag_bits = *entry; }
    else { num = entry[0x10]; flag_bits = *entry; }
  }
  if (number != 0) *number = num;
  if (flags != 0) *flags = flag_bits;
}

void itmtst_item_name_make(int slot, int number, char *out)
{
  char num_text [512];
  int chara = itmtst_state.chara; int costume = itmtst_state.costume;
  strcpy(out,item_chara_names[chara * 2 + costume]);
  strcat(out,"_");
  strcat(out,item_slot_names[slot]);
  strcat(out,"_");
  sprintf(num_text,&format_02d,number);
  strcat(out,num_text);
  return;
}

void itmtst_item_label_make(int item, char *out)
{
  int chara = itmtst_state.chara;
  int costume = itmtst_state.costume;
  char *row;
  int rows;
  char slot;
  byte *entry;
  byte num;
  char flag_bits;
  if (item < 0) {
    memcpy(out,itmtst_no_item_text,5);
  }
  else {
    rows = (int)((struct { uchar *a; char *p; char r[8]; } *)&chara_item_table)[chara].p;
    row = (char *)(item * 0xc + rows);
    slot = *row;
    entry = *(byte **)(row + costume * 4 + 4);
    if (row[costume + 1] == 1) { num = entry[0x14]; flag_bits = *entry; }
    else { num = entry[0x10]; flag_bits = *entry; }
    itmtst_item_name_make(slot,num,out);
    if (flag_bits & 1) {
      strcat(out,"(*)");
    }
    else {
      strcat(out,"   ");
    }
  }
}

void itmtst_camera_set(int view)
{
  vec3 target;
  vec3 eye;
  float dist;
  int angle;

  target.y = itmtst_state.rob0->pos.y - 0.2;
  target.x = itmtst_state.rob0->pos.x;
  target.z = itmtst_state.rob0->pos.z;
  dist = 3.0;
  switch(view) {
  case 0:
    angle = 0x2000;
    break;
  case 1:
    angle = 0x4000;
    break;
  case 2:
    angle = 0x6000;
    break;
  case 3:
    angle = -0x6000;
    break;
  case 4:
    angle = -0x4000;
    break;
  case 5:
    angle = -0x2000;
    break;
  default:
    angle = 0;
  }
  eye.x = -cos_ang16(angle) * dist;
  eye.y = -0.2;
  eye.z = sin_ang16(angle) * dist;
  vec3_add_to((float *)&eye,(float *)&target);
  camera.target = target;
  camera.eye = eye;
  camera.mode = FIX;
  camera.mode_return = 2;
  camera.flags = 0x18;
  camera.pers_req = 45.0;
}

void itmtst_sample_store(uint sample)
{
  if (sample < 0xc) {
    float cpu = rob_frame_usec[0];
    float render = perf_render_usec;
    float ta = perf_ta_usec;
    itmtst_stats.cpu[sample] = cpu;
    itmtst_stats.render[sample] = render;
    itmtst_stats.ta[sample] = ta;
  }
}

float itmtst_altex_kb(void)
{
  return (float)(uint)(rob_body_vram_used.side0 + rob_items_vram_used.side0) * 0.0009765625;
}

float itmtst_tex_kb(void)
{
  return (float)rob_items_vram_used.side0 * 0.0009765625;
}

float itmtst_obj_kb(void)
{
  return (float)(int)(objbank_mem_sizes[13] + objbank_mem_sizes[15] +
                 objbank_mem_sizes[17] + objbank_mem_sizes[19]) * 0.0009765625;
}

void itmtst_obj_counters_clear(void)
{
  objbank_mem_sizes[13] = 0;
  objbank_mem_sizes[15] = 0;
  objbank_mem_sizes[17] = 0;
  objbank_mem_sizes[19] = 0;
  return;
}

void itmtst_samples_average(float *cpu, float *render, float *ta)
{
  float cpu_sum;
  float ta_sum;
  float render_max;
  int i;

  cpu_sum = 0.0f;
  ta_sum = 0.0f;
  render_max = 0.0f;
  for (i = 0; i < 12; i++) {
    cpu_sum = cpu_sum + itmtst_stats.cpu[i];
    ta_sum = ta_sum + itmtst_stats.ta[i];
    if (render_max < itmtst_stats.render[i]) {
      render_max = itmtst_stats.render[i];
    }
  }
  *cpu = cpu_sum / 12.0f;
  *ta = ta_sum / 12.0f;
  *render = render_max;
}

void itmtst_default_record(void)
{
  float cpu;
  float render;
  float ta;
  float altex_kb;
  float obj_kb;
  uint i;
  int none;
  itmtst_samples_average(&cpu, &render, &ta);
  altex_kb = itmtst_altex_kb();
  obj_kb = itmtst_obj_kb();
  none = -1;
  for (i = 0; (int)i < 4; i++) {
    if (itmtst_stats.best[0].val[i] < cpu) {
      itmtst_stats.best[0].val[i] = cpu;
      itmtst_stats.best[0].who[i] = none;
      if (i > 1) itmtst_stats.best[1].who[i] = none;
    }
    if (itmtst_stats.best[2].val[i] < render) {
      itmtst_stats.best[2].val[i] = render;
      itmtst_stats.best[2].who[i] = none;
      if (i > 1) itmtst_stats.best[3].who[i] = none;
    }
    if (itmtst_stats.best[4].val[i] < altex_kb) {
      itmtst_stats.best[4].val[i] = altex_kb;
      itmtst_stats.best[4].who[i] = none;
      if (i > 1) itmtst_stats.best[5].who[i] = none;
    }
    if (itmtst_stats.best[6].val[i] < obj_kb) {
      itmtst_stats.best[6].val[i] = obj_kb;
      itmtst_stats.best[6].who[i] = none;
      if (i > 1) itmtst_stats.best[7].who[i] = none;
    }
  }
}

void itmtst_item_record(void)
{
  char line[512];
  char name[512];
  byte flags[4];
  uint number;
  float cpu;
  float render;
  float ta;
  int id;
  uint slot;
  float tex_kb;
  float altex_kb;
  float obj_kb;

  id = itmtst_stats.item;
  slot = itmtst_item_slot_get(id);
  itmtst_item_info_get(id,&number,flags);
  itmtst_samples_average(&cpu,&render,&ta);
  tex_kb = itmtst_tex_kb();
  altex_kb = itmtst_altex_kb();
  obj_kb = itmtst_obj_kb();
  if (slot == 0 && (flags[0] & 4) != 0) {
    if (itmtst_stats.best[1].val[slot] < cpu) {
      itmtst_stats.best[1].val[slot] = cpu;
      itmtst_stats.best[1].who[slot] = id;
    }
    if (itmtst_stats.best[3].val[slot] < render) {
      itmtst_stats.best[3].val[slot] = render;
      itmtst_stats.best[3].who[slot] = id;
    }
    if (altex_kb > itmtst_stats.best[5].val[slot]) {
      itmtst_stats.best[5].val[slot] = altex_kb;
      itmtst_stats.best[5].who[slot] = id;
    }
    if (obj_kb > itmtst_stats.best[7].val[slot]) {
      itmtst_stats.best[7].val[slot] = obj_kb;
      itmtst_stats.best[7].who[slot] = id;
    }
  }
  else {
    if (itmtst_stats.best[0].val[slot] < cpu) {
      itmtst_stats.best[0].val[slot] = cpu;
      itmtst_stats.best[0].who[slot] = id;
      if (slot > 1) itmtst_stats.best[1].who[slot] = id;
    }
    if (itmtst_stats.best[2].val[slot] < render) {
      itmtst_stats.best[2].val[slot] = render;
      itmtst_stats.best[2].who[slot] = id;
      if (slot > 1) itmtst_stats.best[3].who[slot] = id;
    }
    if (altex_kb > itmtst_stats.best[4].val[slot]) {
      itmtst_stats.best[4].val[slot] = altex_kb;
      itmtst_stats.best[4].who[slot] = id;
      if (slot > 1) itmtst_stats.best[5].who[slot] = id;
    }
    if (obj_kb > itmtst_stats.best[6].val[slot]) {
      itmtst_stats.best[6].val[slot] = obj_kb;
      itmtst_stats.best[6].who[slot] = id;
      if (slot > 1) itmtst_stats.best[7].who[slot] = id;
    }
  }
  memset(name,0,0x200);
  itmtst_item_label_make(id,name);
  memset(line,0,0x200);
  sprintf(line,"%s, %6.1f, %6.1f, [%6.1f], %6.1f, [%6.1f], %6.1f\n",name,cpu,render,ta,tex_kb,altex_kb,obj_kb);
  strcat(itmtst_stats.log,line);
}

void itmtst_summary_write(void)
{
  char line[512];
  char tail[512];
  char name[512];
  char *headings[10];
  float cpu;
  float render;
  float ta;
  float obj_kb;
  float altex_kb;
  float tex_kb;
  int i;
  int j;
  headings[0] = empty_text;
  headings[1] = itmtst_default_heading_text;
  headings[2] = itmtst_cpu_max_heading_text;
  headings[3] = itmtst_render_max_heading_text;
  headings[4] = itmtst_tex_max_heading_text;
  headings[5] = itmtst_obj_max_heading_text;
  headings[6] = itmtst_cpu_max_parts_heading_text;
  headings[7] = itmtst_render_max_parts_heading_text;
  headings[8] = itmtst_tex_max_parts_heading_text;
  headings[9] = itmtst_obj_max_parts_heading_text;
  itmtst_samples_average(&cpu,&render,&ta);
  altex_kb = itmtst_altex_kb();
  obj_kb = itmtst_obj_kb();
  memset(line,0,0x200);
  memset(tail,0,0x200);
  strcat(itmtst_stats.log,"\n");
  strcat(itmtst_stats.log,headings[itmtst_stats.mode]);
  for (i = 0; i < 4; i++) {
    if (itmtst_stats.mode == 1) {
      itmtst_default_record();
      for (j = 0; j < 4; j++) {
        tex_kb = itmtst_tex_kb();
        if ((int)tex_kb > 0) {
          itmtst_item_name_make(j,0,name);
          sprintf(line,"%s   ,      ,        ,      ,        , %6.1f\n",name,tex_kb);
          strcat(itmtst_stats.log,line);
        }
      }
      break;
    }
    switch (itmtst_stats.mode) {
    case 2:
      itmtst_item_label_make(itmtst_stats.best[0].who[i],name);
      break;
    case 3:
      itmtst_item_label_make(itmtst_stats.best[2].who[i],name);
      break;
    case 4:
      itmtst_item_label_make(itmtst_stats.best[4].who[i],name);
      break;
    case 5:
      itmtst_item_label_make(itmtst_stats.best[6].who[i],name);
      break;
    case 6:
      itmtst_item_label_make(itmtst_stats.best[1].who[i],name);
      break;
    case 7:
      itmtst_item_label_make(itmtst_stats.best[3].who[i],name);
      break;
    case 8:
      itmtst_item_label_make(itmtst_stats.best[5].who[i],name);
      break;
    case 9:
      itmtst_item_label_make(itmtst_stats.best[7].who[i],name);
      break;
    }
    sprintf(line,&nam_state_log_format[4],name);
    strcat(itmtst_stats.log,line);
  }
  sprintf(line,"              , %6.1f, %6.1f, [%6.1f], %6.1f",cpu,render,ta,altex_kb);
  sprintf(tail,", %6.1f\n",obj_kb);
  strcat(line,tail);
  strcat(itmtst_stats.log,line);
}
