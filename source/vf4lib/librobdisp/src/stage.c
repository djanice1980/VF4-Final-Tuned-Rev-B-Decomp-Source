/*
 * vf4lib/librobdisp/src/stage.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern fog_rec fog_base; /* 0x0c3742dc */
extern char demo_stage_select; /* 0x0c28c787 */
extern short demo_flags; /* 0x0c28c7a2 */
extern int game_frame_count; /* 0x0c292540 */
extern uint match_stage; /* 0x0c2926dc */
extern int *demo_stage_obj_lists[3]; /* 0x0c306108 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern stage_rec **stage_cur_entry; /* 0x0c306cec */
extern int stage_cur_no; /* 0x0c306cf0 */
extern char stage_vertex_hook_override; /* 0x0c306d00 */
extern float stage_spot_light; /* 0x0c306d14 */
extern wind_preset stage_wind_table[24]; /* 0x0c306d18 */
extern uint poly_shadow_scale; /* 0x0c3226a0 */
extern static_heap_rec heap_stage_obj; /* 0x0c3600e8 */
extern static_heap_rec heap_efcstg_obj; /* 0x0c36013c */
extern fog_rec *stage_fog_record; /* 0x0c3ad218 */
extern stage_rec *stage_table[24]; /* 0x0c306c88 */
extern static_heap_rec field_load; /* 0x0c360000 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern camera_state camera; /* 0x0c370ce0 */
extern wind_rec stage_wind; /* 0x0c3ad21c */
extern view_side_planes_rec view_side_planes; /* 0x0c41cab0 */

void empty_fn_0c0dca40(void)
{
  return;
}

void stage_load(int stage_no)
{
  int free_before;
  int free_after;
  stage_rec *stage;

  stage_cur_entry = &stage_table[stage_no];
  stage = *stage_cur_entry;
  stage_cur_no = stage_no;
  stage_cur = stage;
  txn_list_current_set(0);
  free_before = tex_vram_free_total();
  stage_obj_load(stage->obj_file);
  free_after = tex_vram_free_total();
  stage_obj_vram_used = free_before - free_after;
  free_before = free_after;
  stage_ef_obj_load(stage->ef_obj_file);
  free_after = tex_vram_free_total();
  stage_ef_obj_vram_used = free_before - free_after;
}

void stage_apply(void)
{
  int stage_no;
  stage_rec *stage;

  stage_no = stage_cur_no;
  stage = stage_table[stage_no];
  stg_light_num = stage->stg_light_num;
  rob_light_num = stage->rob_light_num;
  stage_lights_load();
  shadow_stage_setup(stage);
  stage_wind_set(stage_no);
  if (stage_no == 6) rope_stage_on = 1; else rope_stage_on = 0;
  poly_shadow_scale = stage->shadow_scale;
  bg_color_set(stage->bg_colors);
}

void stage_load_apply(int stage_no)
{
  stage_load(stage_no);
  stage_apply();
  return;
}

void stage_load_request(int stage_no)
{
  stage_rec **entry;
  stage_rec *stage;
  entry = &stage_table[stage_no];
  stage_cur_entry = entry;
  stage = *entry;
  stage_cur_no = stage_no;
  stage_cur = stage;
  stg_light_num = stage_cur->stg_light_num;
  rob_light_num = stage_cur->rob_light_num;
  txn_list_current_set(0);
  load_poly_req(3,stage_cur->obj_file,0,&heap_stage_obj,&heap_texdata,
               &heap_texdata,0x46d0);
  if (0 < stage_cur->ef_obj_file) {
    load_poly_req(4,stage_cur->ef_obj_file,0,&heap_efcstg_obj,&heap_texdata,
                 &heap_texdata,0x4d50);
  }
  return;
}

void stage_obj_load_request(int stage_no)
{
  stage_rec *stage;

  stage_cur_entry = &stage_table[stage_no];
  stage = *stage_cur_entry;
  stage_cur_no = stage_no;
  stage_cur = stage;
  stg_light_num = stage_cur->stg_light_num;
  rob_light_num = stage_cur->rob_light_num;
  txn_list_current_set(0);
  load_poly_req(3,stage_cur->obj_file,0,&heap_stage_obj,&heap_texdata,
               &heap_texdata,0x46d0);
}

void stage_field_init(void)
{
  stage_rec *stage;
  int hook;
  
  stage = stage_cur;
  if (((char)stage_vertex_hook_override) < '\0') {
    hook = stage_cur->vertex_hook;
  }
  else {
    hook = (int)((char)stage_vertex_hook_override);
  }
  obj_vertex_hook_select(hook);
  effect_list_start(stage->effect_list);
  stage_coli_load();
  return;
}

void stage_coli_load(void)
{
  load_file(stage_cur->coli_file,field_load.addr,
            field_load.size);
  field_coli = field_load.addr;
  field_coli_set(0,field_load.addr);
  return;
}

void stage_task_init(void)
{
  task_start(&task_stg_fog);
  return;
}

void stage_task_exec(void)
{
  stage_rec *stage = stage_cur;
  int *list;
  stage_placed_obj *placed;
  int obj;
  int modvol_on;
  int translucent;
  int i;
  float ambient[3];
  if (mode_get_main() == MODE_MAIN_ADVERTISE && (demo_flags & 0x800) != 0) return;
  camera_view_load();
  list = *stage->draw_lists;
  light_set_send(0, 0);
  if (list != 0) {
    while (*list >= 0) obj_draw_culled(*list++);
  } else if (stage_cur_no == 7) {
    list = demo_stage_obj_lists[demo_stage_select];
    while (*list >= 0) {
      obj = *list;
      list++;
      obj_draw_culled(obj);
    }
  }
  if (stage_cur_no == 0x15) {
    mtx_push(0);
    mtx_rot_y_s16(game_frame_count & 0xffff);
    obj_draw_culled(0x130007);
    mtx_pop(1);
  }
  if (stage_placed_objs_on != 0 && stage->objs_on != 0 && (placed = (stage_placed_obj *)stage->objs) != 0) {
    modvol_on = 0;
    if (memcmp("CAV STAGE", stage->name, 3) == 0 || memcmp("HBR STAGE", stage->name, 3) == 0) {
      poly_modvol_bit_on();
      modvol_on = 1;
    }
    for (; placed->obj != 0; placed++) {
      if (((placed->flags & 2) == 0 || demo_stage_select == 1) && (placed->flags & 1) != 0) {
        mtx_push(0);
        mtx_translate_by_vec3(placed->pos);
        obj_draw_culled(placed->obj);
        mtx_pop(1);
      }
    }
    if (modvol_on) poly_modvol_bit_off();
  }
  stage_far_objs_draw(stage->draw_lists[1]);
  list = stage->draw_lists[2];
  if (list != 0 && list != (int *)-1) {
    if (stage_cur_no == 1 || stage_cur_no == 0x10) {
      float d = 0.2;
      ambient[0] = light_set_ambient[0].color[0] + d;
      ambient[1] = light_set_ambient[0].color[1] + d;
      ambient[2] = light_set_ambient[0].color[2] + d;
      light_set_send(ambient, 0);
    }
    if (stage_cur_no == 3 || stage_cur_no == 0x13) translucent = 1;
    else if ((mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 5 || mode_get_sub() == 7 || mode_get_sub() == 0xf)) ||
             (mode_get_main() == MODE_MAIN_OVER && (mode_get_sub() == 1 || mode_get_sub() == 3))) translucent = 0;
    else translucent = 1;
    if (translucent) {
      extra_light_spot_set(stage_spot_light);
      for (i = 0; list[i] != -1; i++) obj_draw_translucent(list[i]);
      extra_light_off();
    } else {
      for (i = 0; list[i] != -1; i++) obj_draw(list[i]);
    }
    if (stage_cur_no == 1 || stage_cur_no == 0x10) {
      float d = 0.2;
      ambient[0] = light_set_ambient[0].color[0] - d;
      ambient[1] = light_set_ambient[0].color[1] - d;
      ambient[2] = light_set_ambient[0].color[2] - d;
      light_set_send(ambient, 0);
    }
  }
  stage_late_objs_draw();
}

void stage_late_objs_draw(void)
{
  int *p;
  p = stage_cur->late_objs;
  mtx_push(0);
  if (match_stage == 6) {
    mtx_translate(0.0,4.0,0.0);
  }
  while (*p != -1) {
    obj_draw(*p++);
  }
  mtx_pop(1);
}

void stage_far_objs_draw(int *objs)
{
  compiled_model_rec *model;
  float *center;
  int i;
  int visible;
  float target_dist;
  float radius;
  float view_pos[3];
  float *plane;

  if (objs != (int *)0x0 && objs != (int *)-1) {
    target_dist = vec3_distance(&camera.target.x,&camera.eye.x);
    for (; *objs >= 0; objs++) {
      model = (compiled_model_rec *)obj_model_get(*objs);
      if (model != (compiled_model_rec *)0x0) {
        center = model->sphere;
        radius = model->sphere[3];
        if ((target_dist < vec3_distance(center,&camera.eye.x)) ||
           (mode_get_main() == MODE_MAIN_GAME && mode_get_sub() == 5)) {
          mtx_xform_point(center,view_pos);
          i = 0;
          visible = 1;
          for (; i < 4; i++) {
            plane = (float *)&view_side_planes + i * 4;
            if (plane[0] * view_pos[0] + plane[1] * view_pos[1] + plane[2] * view_pos[2] > radius) {
              visible = 0;
              break;
            }
          }
          if (visible) {
            if (model->format == 0x100) model_draw(model);
            else obj_put_typed((int *)model);
          }
        }
      }
    }
  }
}

void stg_fog_task_init(void)
{
  fog_rec *fog = &fog_base;
  fog_rec *src;

  src = stage_cur->fog;
  stage_fog_record = src;
  if (src != 0) {
    if ((~fog_flags & 2) == 0) {
      fog->argb[1] = src->argb[1];
      fog->argb[2] = src->argb[2];
      fog->argb[3] = src->argb[3];
      fog->near_dist = src->near_dist;
      fog->far_dist = src->far_dist;
      fog->strength = src->strength;
    }
  }
  else {
    fog->argb[1] = 0;
    fog->argb[2] = 0;
    fog->argb[3] = 0;
    fog->near_dist = 0.0;
    fog->far_dist = 0.0;
    fog->strength = 0.0;
  }
}

void stg_fog_task_exec(void)
{
  fog_rec *fog = &fog_base;
  fog_rec *src;

  if (pause_frame_held() == 0 && (~fog_flags & 2) == 0) {
    src = stage_fog_record;
    if (src != 0) {
      fog->argb[1] = src->argb[1];
      fog->argb[2] = src->argb[2];
      fog->argb[3] = src->argb[3];
      fog->near_dist = src->near_dist;
      fog->far_dist = src->far_dist;
      fog->strength = src->strength;
    }
    else {
      fog->argb[1] = 0;
      fog->argb[2] = 0;
      fog->argb[3] = 0;
      fog->near_dist = 0.0f;
      fog->far_dist = 0.0f;
      fog->strength = 0.0f;
    }
  }
}

void stage_wind_set(int stage_no)
{
  wind_preset *preset;
  int i;
  
  preset = stage_wind_table + stage_no;
  stage_wind.bias = preset->bias;
  stage_wind.cycle[0] = preset->cycle0;
  stage_wind.ratio[0] = preset->ratio0;
  stage_wind.cycle[1] = preset->cycle1;
  stage_wind.ratio[1] = preset->ratio1;
  stage_wind.cycle[2] = preset->cycle2;
  stage_wind.ratio[2] = preset->ratio2;
  stage_wind.xang = preset->xang;
  stage_wind.yang = preset->yang;
  for (i = 2; i < 8; i++) {
    stage_wind.cycle[i] = 0.0;
    stage_wind.ratio[i] = 0.0;
  }
  return;
}

void wind_update(wind_rec *wind)
{
  float vec[3];
  int i;
  float angle;

  ((int *)vec)[0] = 0;
  ((int *)vec)[1] = 0;
  vec[2] = wind->bias;
  for (i = 0; i < 8; i++) {
    angle = wind->phase[i] + wind->cycle[i] * 0.104719736;
    vec[2] += wind->ratio[i] * (sin_s16((short)(int)((angle * 65536.0) / 6.2831855 + 0.5)) + 1.0);
    wind->phase[i] = angle;
  }
  mtx_push_identity();
  mtx_rot_y_s16(wind->yang);
  mtx_rot_x_s16(wind->xang);
  mtx_xform_dir(vec,&wind->vec.x);
  mtx_pop(1);
}
