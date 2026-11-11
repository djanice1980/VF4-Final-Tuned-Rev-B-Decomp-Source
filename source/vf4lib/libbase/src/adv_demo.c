/*
 * vf4lib/libbase/src/adv_demo.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_xyz_scaled_alpha_angle_flags
#define KRPROTO_sprite_put_xy_scaled_flags_alpha
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern camera_state camera; /* 0x0c370ce0 */
extern adv_light_color adv_light_target; /* 0x0c371010 */
extern void (*demo_cmd_table[21])(void); /* 0x0c1632f4 */
extern void (*demo_scene_draw_table[37])(void); /* 0x0c163348 */
extern void (*demo_scene_init_table[37])(void); /* 0x0c1633dc */
extern char demo_type; /* 0x0c28c784 */
extern char demo_cmd_state; /* 0x0c28c786 */
extern char demo_stage_select; /* 0x0c28c787 */
extern char demo_scene; /* 0x0c28c788 */
extern float demo_clock; /* 0x0c28c78c */
extern float demo_clock_step; /* 0x0c28c790 */
extern short demo_total_count; /* 0x0c28c794 */
extern short demo_cmd_frame; /* 0x0c28c7a0 */
extern short demo_flags; /* 0x0c28c7a2 */
extern short demo_scene_frame; /* 0x0c28c7a4 */
extern short demo_phase_end; /* 0x0c28c7a6 */
extern short demo_phase_start; /* 0x0c28c7a8 */
extern int demo_model_slot; /* 0x0c28c7ac */
extern float demo_white_in; /* 0x0c28c7b0 */
extern float demo_flash_alpha; /* 0x0c28c7b4 */
extern float demo_flash_step; /* 0x0c28c7b8 */
extern char demo_chara_pool[15]; /* 0x0c28c7dc */
extern demo_aet_layer_rec demo_aet_layers[25]; /* 0x0c28c7ec */
extern demo_script_rec demo_script_adva[2]; /* 0x0c28dffc */
extern demo_script_rec demo_script_advb[2]; /* 0x0c28e41c */
extern demo_script_rec demo_script_advc[2]; /* 0x0c28e99c */
extern int mode_timer; /* 0x0c292550 */
extern float mode_timer_float; /* 0x0c292578 */
extern float mode_timer_rate; /* 0x0c292678 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern static_heap_rec rob_obj_ofs[3]; /* 0x0c3600ac */
extern undefined *board_config; /* 0x0c360178 */
extern static_heap_rec objmem_heap_chr[3]; /* 0x0c3601bc */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern short cam_anim_index; /* 0x0c370d5c */
extern uint cam_anim_flags; /* 0x0c370dc4 */
extern demo_rob_request demo_rob_req[2]; /* 0x0c370ff0 */
extern short *demo_phase_list; /* 0x0c37101c */
extern char demo_pick_chara; /* 0x0c371020 */
extern char demo_pick_costume; /* 0x0c371021 */
extern demo_script_rec *demo_script_pos; /* 0x0c371028 */
extern demo_cmd_rec *demo_cmd_pos; /* 0x0c37102c */
extern char demo_chara_req[3]; /* 0x0c41f088 */
extern char demo_phase; /* 0x0c41f090 */
extern stage_rec *stage_table[24]; /* 0x0c306c88 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern agg_0c41f08c agg_0c41f08c_0c41f08c; /* 0x0c41f08c */

void demo_start(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  rob_rec *rob;
  uchar i;
  float f;
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  agg_0c41f08c_0c41f08c.field_0x0 = 0;
  agg_0c41f08c_0c41f08c.field_0x2 = 0;
  demo_total_count = 0;
  demo_clock = 0.0;
  demo_cmd_frame = 0;
  adv_light_blend_frames = 0;
  adv_light_target.r = adv_light_target.g = adv_light_target.b = 0.0;
  rob_round_reset(rob0);
  rob_round_reset(rob1);
  mode_timer = 0x708;
  mode_timer_float = 1800.0;
  mode_timer_rate = 1.0;
  for (i = 0; i < 2; i++) {
    demo_rob_request *e;
    rob = rob_get(i);
    rob->pos.x = 0.0;
    rob->ground_pos.x = 0.0;
    rob->place_pos.x = 0.0;
    f = stage_cur->center.y;
    rob->pos.y = f;
    rob->ground_pos.y = f;
    rob->place_pos.y = f;
    rob->pos.z = 0.0;
    rob->ground_pos.z = 0.0;
    rob->place_pos.z = 0.0;
    rob->heading = 0;
    rob->flags &= ~8;
    rob->state |= 0x10;
    rob->side_flags &= 0xffe7;
    rob_move_set(rob,0x1b6);
    rob->frame = 1;
    rob->frame_pos = 1.0;
    rob->action.word = rob->move_no;
    rob->action_step = 1;
    rob->flags = (rob->flags | 0x100000) & ~1;
    rob->field_0x574 = 0;
    e = &demo_rob_req[i];
    e->flags = 0;
    e->and_mask = 0;
    e->or_mask = 0;
    e->set = 0;
    e->motion = 0;
  }
  camera.mode = FIX;
  demo_cmd_pos = (demo_cmd_rec *)0x0;
  switch (demo_type) {
  case 1:
    demo_script_pos = demo_script_advb;
    break;
  case 2:
    demo_script_pos = demo_script_advc;
    break;
  case 0:
  default:
    demo_script_pos = demo_script_adva;
    break;
  }
  demo_model_slot = 2;
  rob0->model_slot = 0;
  rob1->model_slot = 1;
  demo_pick_chara = demo_chara_pool[(int)lcg_next15() % 15];
  demo_pick_costume = (int)lcg_next15() % 2;
  demo_stage_select = '\x02';
  demo_scene = '\0';
  demo_scene_frame = 0;
  demo_flags = 1;
  demo_cmd_state = 0;
  demo_phase = '\0';
  objbank_free(7);
  demo_start_hook();
  demo_aet_open();
  demo_flash_alpha = 0.0;
  demo_flash_step = 0.0;
  demo_white_in = 0.0;
  rob_light_num = 0x2f;
  stage_lights_load();
  return;
}

void demo_frame(void)
{
  uint held;

  held = pause_frame_held();
  if (held == 0) {
    if (*(int *)(board_config + 0xc) == 0) {
      demo_clock = demo_clock + demo_clock_step;
      demo_total_count = (short)(int)demo_clock;
    }
    else {
      demo_total_count = demo_total_count + 1;
    }
    demo_script_step();
    demo_cmd_list_run();
    demo_rob_update();
  }
  return;
}

int demo_script_step(void)
{
  if (demo_script_pos->frame <= demo_total_count) {
    if (demo_script_pos->cmds == 0) {
      demo_cmd_pos = (demo_cmd_rec *)0x0;
      mode_timer = 0;
      mode_timer_float = 0.0;
      demo_cmd_frame = 0;
    }
    else {
      demo_cmd_pos = demo_script_pos->cmds;
      mode_timer = 0x1c20;
      mode_timer_float = 7200.0;
      demo_cmd_frame = 0;
      demo_script_pos = demo_script_pos + 1;
    }
  }
  return 0;
}

int demo_cmd_list_run(void)
{
  if (demo_cmd_pos == (demo_cmd_rec *)0x0) {
    return 0;
  }
  demo_cmd_frame = demo_cmd_frame + 1;
  while (demo_cmd_pos->frame <= demo_cmd_frame) {
    (*demo_cmd_table[demo_cmd_pos->cmd])();
    if (demo_cmd_pos->frame == 0) {
      return 1;
    }
    demo_cmd_pos = demo_cmd_pos + 1;
  }
  return 0;
}

void demo_cmd_00_cam_anim(void)
{
  char *s;

  s = demo_cmd_pos->arg;
  sic_rob_id = *s;
  cam_anim_index = *(short *)(s + 2);
  cam_anim_flags = 1;
  cam_anim_time = 0.0;
  cam_anim_kind = 4;
  camera.mode = SOFTIMAGE;
  return;
}

void demo_cmd_01_rob_flags(void)
{
  byte *s;
  demo_rob_request *e;

  s = demo_cmd_pos->arg;
  e = &demo_rob_req[*s];
  switch (s[1]) {
  case 0:
    e->flags = e->flags | 1;
    e->and_mask = e->and_mask | *(uint *)(s + 4);
    break;
  case 1:
    e->flags = e->flags | 2;
    e->or_mask = e->or_mask | *(uint *)(s + 4);
    break;
  case 2:
    e->flags = e->flags | 4;
    e->set = *(uint *)(s + 4);
    break;
  }
  return;
}

void demo_cmd_02_rob_motion(void)
{
  byte *s;
  demo_rob_request *e;

  s = demo_cmd_pos->arg;
  e = &demo_rob_req[*s];
  e->motion = *(ushort *)(s + 2);
  e->flags = e->flags | 8;
  return;
}

void demo_cmd_03_chara_load(void)
{
  char *d = demo_chara_req;
  char *s = demo_cmd_pos->arg;

  demo_flags |= 4;
  d[0] = s[0];
  if (s[2] == -1) {
    d[2] = (int)lcg_next15() % 2;
  } else if (s[2] == -2) {
    d[2] = demo_pick_costume;
  } else {
    d[2] = chara_costume_swap[s[1]] ^ s[2];
  }
  if (s[1] == -1) {
    d[1] = demo_chara_pool[(int)lcg_next15() % 15];
  } else if (s[1] == -2) {
    d[1] = demo_pick_chara;
  } else {
    d[1] = s[1];
  }
}

void demo_cmd_04_robs_bit0_clear(void)
{
  rob_rec *rob;

  demo_cmd_state = 1;
  rob = rob_get(0);
  rob->flags = rob->flags & 0xfffffffe;
  rob = rob_get(1);
  rob->flags = rob->flags & 0xfffffffe;
  return;
}

void demo_cmd_05_state_clear(void)
{
  demo_cmd_state = 0;
  return;
}

void demo_cmd_07_state_clear(void)
{
  demo_cmd_state = 0;
  return;
}

void demo_cmd_06_state_2(void)
{
  demo_cmd_state = 2;
  return;
}

void demo_cmd_08_flag2_set(void)
{
  demo_flags = demo_flags | 2;
  return;
}

void demo_cmd_09_flag2_clear(void)
{
  demo_flags = demo_flags & 0xfffd;
  return;
}

void demo_cmd_10_chara_apply(void)
{
  char *s;

  s = demo_cmd_pos->arg;
  demo_flags = demo_flags | 0x10;
  demo_apply_side = *s;
  return;
}

void demo_cmd_11_stage_lights(void)
{
  int stage_no;
  char *s;

  s = demo_cmd_pos->arg;
  stage_no = 7;
  demo_stage_select = *s;
  switch (demo_stage_select) {
  case 1:
    stage_no = 0x15;
    break;
  case 2:
    stage_no = 0;
    break;
  case 0:
    stage_no = 0x11;
    break;
  }
  stg_light_num = stage_table[stage_no]->stg_light_num;
  rob_light_num = stage_table[stage_no]->rob_light_num;
  stage_lights_load();
  return;
}

void demo_cmd_12_mode_flags(void)
{
  short *s;

  s = demo_cmd_pos->arg;
  if (*(char *)(s + 1) == 3) {
    demo_flags = demo_flags | 0x200;
  }
  else {
    switch (*(byte *)(s + 1)) {
    case 0:
      fog_flags = fog_flags | 0x10;
      break;
    case 1:
      fog_flags = fog_flags | 0x20;
      demo_flags = demo_flags | 0x2000;
      break;
    case 2:
      fog_flags = fog_flags & 0xffffff3b;
      break;
    }
    fog_timer = *s;
    fade_color[0] = s[2];
    fade_color[1] = s[3];
    fade_color[2] = s[4];
  }
  return;
}

void demo_cmd_13_robs_models_free(void)
{
  int i;
  (rob_get(0))->flags &= 0xfffffffe;
  (rob_get(1))->flags &= 0xfffffffe;
  for (i = 0; i < 3; i++) {
    objbank_free_req(i + 5);
    objbank_free_req(i + 9);
  }
}

void demo_cmd_14_bg_color(void)
{
  int *s;
  int color;

  s = demo_cmd_pos->arg;
  if (*(char *)(s + 1) == 2) {
    demo_flags |= 0x880;
    color = *s;
    if (color != 0) bg_color_set((uint *)color);
  }
  else if (*(char *)(s + 1) == 1) {
    demo_flags |= 0x800;
    demo_flags &= 0xff7f;
    color = *s;
    if (color != 0) bg_color_set((uint *)color);
  }
  else {
    demo_flags &= 0xf77f;
    bg_color_set(stage_cur->bg_colors);
  }
}

void demo_cmd_15_vec3_set(void)
{
  short *s;

  s = demo_cmd_pos->arg;
  switch (*(byte *)(s + 1)) {
  case 0:
    adv_light_target.r = *(float *)(s + 2);
    adv_light_target.g = *(float *)(s + 4);
    adv_light_target.b = *(float *)(s + 6);
    adv_light_blend_mode = 1;
    adv_light_blend_frames = *s;
    break;
  case 1:
    adv_light_target.r = *(float *)(s + 2);
    adv_light_target.g = *(float *)(s + 4);
    adv_light_target.b = *(float *)(s + 6);
    adv_light_blend_mode = 2;
    adv_light_blend_frames = *s;
    break;
  default:
    adv_light_blend_mode = 4;
    break;
  }
  return;
}

void demo_cmd_16_scene_set(void)
{
  demo_scene = *(char *)demo_cmd_pos->arg;
  demo_scene_frame = 0;
  demo_phase = 0;
  if (demo_scene_init_table[demo_scene] != 0) {
    (*demo_scene_init_table[demo_scene])();
  }
  return;
}

void demo_cmd_17_aet_play(void)
{
  int s;
  aet_obj_param param;

  s = (int)demo_cmd_pos->arg;
  aet_param_init(&param);
  aet_param_scene_set(&param,0x1c0001);
  aet_param_flags_set(&param,*(int *)(s + 4));
  aet_param_depth_range_set(&param,0.03,0.03);
  aet_obj_start(&param);
  return;
}

void demo_cmd_18_flash_set(void)
{
  byte *s;

  s = demo_cmd_pos->arg;
  switch (*s) {
  case 0:
    demo_flash_alpha = 0.0;
    demo_flash_step = *(float *)(s + 4);
    demo_flags = demo_flags | 0x4000;
    break;
  case 1:
    demo_flash_alpha = 1.0;
    demo_flash_step = -*(float *)(s + 4);
    demo_flags = demo_flags | 0x4000;
    break;
  case 2:
    demo_flash_alpha = 0.0;
    demo_flash_step = 0.0;
    demo_flags = (demo_flags & 0x9fff) | 0x1000;
    break;
  }
  return;
}

void demo_cmd_19_aet_layer(void)
{
  char *p;
  aet_obj_param buf;
  demo_aet_layer_rec *t;

  p = demo_cmd_pos->arg;
  if (*p == '\0') {
    aet_param_init(&buf);
    if (p[1] == '\x18') {
      aet_param_scene_set(&buf,0x1c0001);
    }
    else {
      aet_param_scene_set(&buf,0x1c0000);
    }
    t = demo_aet_layers;
    aet_param_flags_set(&buf,(&t->play_flags)[(byte)p[1] * 3]);
    aet_param_depth_range_set(&buf,0.03,0.03);
    if ((byte)p[1] != 0x18) {
      aet_param_layer_set(&buf,t[(byte)p[1]].name);
    }
    demo_aet_layers[(byte)p[1]].handle = aet_obj_start(&buf);
    if (p[1] == 0x18) {
      demo_flags = demo_flags & 0x9fff;
    }
  }
  else if (*p == '\x01') {
    if (demo_aet_layers[(byte)p[1]].handle != 0xffffffff) {
      aet_obj_stop(demo_aet_layers[(byte)p[1]].handle);
    }
  }
}

void demo_cmd_20_end(void)
{
  return;
}

void demo_scene_00_init(void)
{
  return;
}

void demo_scene_00_draw(void)
{
  return;
}

void demo_scene_01_init(void)
{
  short *p = demo_scene_01_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_01_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_01_layouts,(int)((char)demo_phase),progress);
  demo_band_draw(5,(int)((char)demo_phase),progress);
  demo_band_draw(6,(int)((char)demo_phase),progress);
  return;
}

void demo_scene_02_init(void)
{
  short *p = demo_scene_02_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_02_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_02_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_02_layouts_b,(int)demo_phase,progress);
  demo_band_draw(5,(int)demo_phase,progress);
  demo_band_draw(3,(int)demo_phase,progress);
  return;
}

void demo_scene_03_init(void)
{
  short *p = demo_scene_03_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_03_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_03_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_03_layouts_b,(int)demo_phase,progress);
  demo_band_draw(7,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_04_init(void)
{
  short *p = demo_scene_04_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_04_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_04_layouts,(int)((char)demo_phase),progress);
  demo_band_draw(1,(int)((char)demo_phase),progress);
  demo_band_draw(2,(int)((char)demo_phase),progress);
  return;
}

void demo_scene_05_init(void)
{
  short *p = demo_scene_05_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_05_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_05_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_05_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_06_init(void)
{
  short *p = demo_scene_06_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_06_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_06_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_06_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_07_init(void)
{
  short *p = demo_scene_07_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_07_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_07_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_07_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_08_init(void)
{
  return;
}

void demo_scene_08_draw(void)
{
  if (demo_scene_frame > 0x20) {
    sprite_put_xy(0x824,0.0,64.0);
    sprite_put_xy(0x825,512.0,64.0);
    sprite_put_xy(0x826,512.0,216.0);
    sprite_put_xy(0x827,512.0,368.0);
  }
  else if (demo_scene_frame > 0x12) {
    sprite_put_xy(0x820,0.0,64.0);
    sprite_put_xy(0x821,512.0,64.0);
    sprite_put_xy(0x822,512.0,216.0);
    sprite_put_xy(0x823,512.0,368.0);
  }
  else {
    sprite_put_xy(0x81c,0.0,64.0);
    sprite_put_xy(0x81d,512.0,64.0);
    sprite_put_xy(0x81e,512.0,216.0);
    sprite_put_xy(0x81f,512.0,368.0);
  }
}

void demo_scene_09_init(void)
{
  return;
}

void demo_scene_09_draw(void)
{
  return;
}

void demo_scene_10_init(void)
{
  short *p = demo_scene_10_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_10_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_10_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_10_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_11_init(void)
{
  demo_phase_end = 0x14;
  return;
}

void demo_scene_11_draw(void)
{
  demo_layout_fade_out(&demo_scene_11_layout);
  return;
}

void demo_scene_12_init(void)
{
  demo_phase_end = 0x14;
  return;
}

void demo_scene_12_draw(void)
{
  demo_layout_fade_out(&demo_scene_12_layout);
  return;
}

void demo_scene_13_init(void)
{
  demo_phase_end = 0x14;
  return;
}

void demo_scene_13_draw(void)
{
  demo_layout_fade_out(&demo_scene_13_layout);
  return;
}

void demo_scene_14_init(void)
{
  demo_phase_end = 0x14;
  return;
}

void demo_scene_14_draw(void)
{
  demo_layout_fade_out(&demo_scene_14_layout);
  return;
}

void demo_scene_15_init(void)
{
  return;
}

void demo_scene_15_draw(void)
{
  float y = -4.0;

  if (demo_scene_frame > 0x11) {
    sprite_put_xy(0x7f2,0.0,y + 64.0);
    sprite_put_xy(0x7f3,512.0,y + 64.0);
    sprite_put_xy(0x7f4,512.0,y + 216.0);
    sprite_put_xy(0x7f5,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x7ee,0.0,y + 64.0);
    sprite_put_xy(0x7ef,512.0,y + 64.0);
    sprite_put_xy(0x7f0,512.0,y + 216.0);
    sprite_put_xy(0x7f1,512.0,y + 368.0);
  }
}

void demo_scene_16_init(void)
{
  return;
}

void demo_scene_16_draw(void)
{
  float y = -4.0;
  if (demo_scene_frame > 0x10) {
    sprite_put_xy(0x7fa,0.0,y + 64.0);
    sprite_put_xy(0x7fb,512.0,y + 64.0);
    sprite_put_xy(0x7fc,512.0,y + 216.0);
    sprite_put_xy(0x7fd,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x7f6,0.0,y + 64.0);
    sprite_put_xy(0x7f7,512.0,y + 64.0);
    sprite_put_xy(0x7f8,512.0,y + 216.0);
    sprite_put_xy(0x7f9,512.0,y + 368.0);
  }
}

void demo_scene_17_init(void)
{
  return;
}

void demo_scene_17_draw(void)
{
  float o = -4.0;

  if (demo_scene_frame > 14) {
    sprite_put_xy(0x802,0.0,o + 64.0);
    sprite_put_xy(0x803,512.0,o + 64.0);
    sprite_put_xy(0x804,512.0,o + 216.0);
    sprite_put_xy(0x805,512.0,o + 368.0);
  }
  else {
    sprite_put_xy(0x7fe,0.0,o + 64.0);
    sprite_put_xy(0x7ff,512.0,o + 64.0);
    sprite_put_xy(0x800,512.0,o + 216.0);
    sprite_put_xy(0x801,512.0,o + 368.0);
  }
  return;
}

void demo_scene_18_init(void)
{
  return;
}

void demo_scene_18_draw(void)
{
  float y = -4.0;
  float z = 0.0;
  float x;
  float a = y + 64.0;
  sprite_put_xy(0x806,z,a);
  x = 512.0;
  sprite_put_xy(0x807,x,a);
  sprite_put_xy(0x808,x,y + 216.0);
  y += 368.0;
  sprite_put_xy(0x809,x,y);
}

void demo_scene_19_init(void)
{
  return;
}

void demo_scene_19_draw(void)
{
  float y = 0.0;
  if (demo_scene_frame > 30) {
    sprite_put_xyz(0x861,0.0,y + 0.0,0.05);
    sprite_put_xyz(0x862,512.0,y + 0.0,0.05);
    sprite_put_xyz(0x863,512.0,y + 256.0,0.05);
  }
  else if (demo_scene_frame > 16) {
    sprite_put_xyz(0x86f,32.0,y + 200.0,0.05);
    sprite_put_xyz(0x870,160.0,y + 200.0,0.05);
    sprite_put_xyz(0x871,288.0,y + 200.0,0.05);
    sprite_put_xyz(0x872,416.0,y + 200.0,0.05);
    sprite_put_xyz(0x873,544.0,y + 200.0,0.05);
  }
  else {
    sprite_put_xyz(0x86c,88.0,y + 168.0,0.05);
    sprite_put_xyz(0x86d,88.0,y + 296.0,0.05);
    sprite_put_xyz(0x86e,344.0,y + 168.0,0.05);
  }
}

void demo_scene_20_init(void)
{
  demo_phase_end = 0x1e;
  return;
}

void demo_scene_20_draw(void)
{
  float x;
  float y;
  float dy;
  float w;
  x = -5.0;
  y = 480.0;
  dy = -(float)demo_scene_frame * 36.0;
  w = 208.0;
  sprite_put_xyz_scaled_alpha_angle_flags(0x7e6, w + x, dy + y, 0.05, 1.0, 1.0, -0x4000, 1.0, 5);
  x += 408.0;
  x -= 208.0;
  x += 8.0;
  y += 512.0;
  sprite_put_xyz_scaled_alpha_angle_flags(0x7e7, x, dy + y, 0.05, 1.0, 1.0, -0x4000, 1.0, 5);
}

void demo_scene_21_init(void)
{
  bg_color_set(bg_color_presets[4]);
  demo_flags = demo_flags & 0xfeff;
  return;
}

void demo_scene_21_draw(void)
{
  float b;

  b = 0.0;
  net_messages_flag_set();
  sprite_put_xy_alpha_flags(0x861,b,b * 2,1.0,0x805);
  sprite_put_xy_alpha_flags(0x862,512.0,b * 2,1.0,0x805);
  sprite_put_xy_alpha_flags(0x863,512.0,b + 256.0,1.0,0x805);
}

void demo_scene_22_init(void)
{
  return;
}

void demo_scene_22_draw(void)
{
  float y = -4.0;

  if (demo_scene_frame > 0x22) {
    sprite_put_xy(0x39d8,0.0,y + 64.0);
    sprite_put_xy(0x39d9,512.0,y + 64.0);
    sprite_put_xy(0x39da,512.0,y + 216.0);
    sprite_put_xy(0x39db,512.0,y + 368.0);
  }
  else if (demo_scene_frame > 0x11) {
    sprite_put_xy(0x39d4,0.0,y + 64.0);
    sprite_put_xy(0x39d5,512.0,y + 64.0);
    sprite_put_xy(0x39d6,512.0,y + 216.0);
    sprite_put_xy(0x39d7,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x39d0,0.0,y + 64.0);
    sprite_put_xy(0x39d1,512.0,y + 64.0);
    sprite_put_xy(0x39d2,512.0,y + 216.0);
    sprite_put_xy(0x39d3,512.0,y + 368.0);
  }
}

void demo_scene_23_init(void)
{
  return;
}

void demo_scene_23_draw(void)
{
  float y = -4.0;

  if (demo_scene_frame >= 36) {
    sprite_put_xy(0x39e4,0.0,y + 64.0);
    sprite_put_xy(0x39e5,512.0,y + 64.0);
    sprite_put_xy(0x39e6,512.0,y + 216.0);
    sprite_put_xy(0x39e7,512.0,y + 368.0);
  }
  else if (demo_scene_frame >= 18) {
    sprite_put_xy(0x39e0,0.0,y + 64.0);
    sprite_put_xy(0x39e1,512.0,y + 64.0);
    sprite_put_xy(0x39e2,512.0,y + 216.0);
    sprite_put_xy(0x39e3,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x39dc,0.0,y + 64.0);
    sprite_put_xy(0x39dd,512.0,y + 64.0);
    sprite_put_xy(0x39de,512.0,y + 216.0);
    sprite_put_xy(0x39df,512.0,y + 368.0);
  }
}

void demo_scene_24_init(void)
{
  return;
}

void demo_scene_24_draw(void)
{
  float y = -4.0;

  if (demo_scene_frame > 33) {
    sprite_put_xy(0x39f4,0.0,y + 64.0);
    sprite_put_xy(0x39f5,512.0,y + 64.0);
    sprite_put_xy(0x39f6,512.0,y + 216.0);
    sprite_put_xy(0x39f7,512.0,y + 368.0);
  }
  else if (demo_scene_frame > 16) {
    sprite_put_xy(0x39f0,0.0,y + 64.0);
    sprite_put_xy(0x39f1,512.0,y + 64.0);
    sprite_put_xy(0x39f2,512.0,y + 216.0);
    sprite_put_xy(0x39f3,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x39ec,0.0,y + 64.0);
    sprite_put_xy(0x39ed,512.0,y + 64.0);
    sprite_put_xy(0x39ee,512.0,y + 216.0);
    sprite_put_xy(0x39ef,512.0,y + 368.0);
  }
}

void demo_scene_25_init(void)
{
  return;
}

void demo_scene_25_draw(void)
{
  sprite_put_xyz(0x3a69,32.0,200.0,0.05);
  sprite_put_xyz(0x3a6a,160.0,200.0,0.05);
  sprite_put_xyz(0x3a6b,288.0,200.0,0.05);
  sprite_put_xyz(0x3a6c,416.0,200.0,0.05);
  sprite_put_xyz(0x3a6d,544.0,200.0,0.05);
  return;
}

void demo_scene_26_init(void)
{
  bg_color_set(bg_color_presets[4]);
  demo_flags = demo_flags & 0xfeff;
  return;
}

void demo_scene_26_draw(void)
{
  float y = 0.0;
  net_messages_flag_set();
  sprite_put_xy_alpha_flags(0x3a5d,y,y * 2.0,1.0,0x805);
  sprite_put_xy_alpha_flags(0x3a5e,512.0,y * 2.0,1.0,0x805);
  sprite_put_xy_alpha_flags(0x3a5f,512.0,y + 256.0,1.0,0x805);
}

void demo_scene_27_init(void)
{
  return;
}

void demo_scene_27_draw(void)
{
  float y = -4.0;

  if (demo_scene_frame > 39) {
    sprite_put_xy(0x3a00, 0.0, y + 64.0);
    sprite_put_xy(0x3a01, 512.0, y + 64.0);
    sprite_put_xy(0x3a02, 512.0, y + 216.0);
    sprite_put_xy(0x3a03, 512.0, y + 368.0);
  }
  else if (demo_scene_frame > 20) {
    sprite_put_xy(0x39fc, 0.0, y + 64.0);
    sprite_put_xy(0x39fd, 512.0, y + 64.0);
    sprite_put_xy(0x39fe, 512.0, y + 216.0);
    sprite_put_xy(0x39ff, 512.0, y + 368.0);
  }
  else {
    sprite_put_xy(0x39f8, 0.0, y + 64.0);
    sprite_put_xy(0x39f9, 512.0, y + 64.0);
    sprite_put_xy(0x39fa, 512.0, y + 216.0);
    sprite_put_xy(0x39fb, 512.0, y + 368.0);
  }
}

void demo_scene_28_init(void)
{
  return;
}

void demo_scene_28_draw(void)
{
  float y = -4.0;
  if (demo_scene_frame > 37) {
    sprite_put_xy(0x3a0c,0.0,y + 64.0);
    sprite_put_xy(0x3a0d,512.0,y + 64.0);
    sprite_put_xy(0x3a0e,512.0,y + 216.0);
    sprite_put_xy(0x3a0f,512.0,y + 368.0);
  }
  else if (demo_scene_frame > 19) {
    sprite_put_xy(0x3a08,0.0,y + 64.0);
    sprite_put_xy(0x3a09,512.0,y + 64.0);
    sprite_put_xy(0x3a0a,512.0,y + 216.0);
    sprite_put_xy(0x3a0b,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x3a04,0.0,y + 64.0);
    sprite_put_xy(0x3a05,512.0,y + 64.0);
    sprite_put_xy(0x3a06,512.0,y + 216.0);
    sprite_put_xy(0x3a07,512.0,y + 368.0);
  }
}

void demo_scene_29_init(void)
{
  return;
}

void demo_scene_29_draw(void)
{
  float y = -4.0;
  
  if (demo_scene_frame > 37) {
    sprite_put_xy(0x3a18,0.0,y + 64.0);
    sprite_put_xy(0x3a19,512.0,y + 64.0);
    sprite_put_xy(0x3a1a,512.0,y + 216.0);
    sprite_put_xy(0x3a1b,512.0,y + 368.0);
  }
  else if (demo_scene_frame > 19) {
    sprite_put_xy(0x3a14,0.0,y + 64.0);
    sprite_put_xy(0x3a15,512.0,y + 64.0);
    sprite_put_xy(0x3a16,512.0,y + 216.0);
    sprite_put_xy(0x3a17,512.0,y + 368.0);
  }
  else {
    sprite_put_xy(0x3a10,0.0,y + 64.0);
    sprite_put_xy(0x3a11,512.0,y + 64.0);
    sprite_put_xy(0x3a12,512.0,y + 216.0);
    sprite_put_xy(0x3a13,512.0,y + 368.0);
  }
  return;
}

void demo_scene_30_init(void)
{
  short *p = demo_scene_30_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_30_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_30_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_30_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_31_init(void)
{
  short *p = demo_scene_31_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_31_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_31_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_31_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_32_init(void)
{
  short *p = demo_scene_32_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_32_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_32_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_32_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_33_init(void)
{
  short *p = demo_scene_33_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_33_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_33_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_33_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_34_init(void)
{
  short *p = demo_scene_34_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_34_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_34_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_34_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_35_init(void)
{
  short *p = demo_scene_35_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_35_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_33_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_35_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_scene_36_init(void)
{
  short *p = demo_scene_36_phases;

  demo_phase = 0;
  demo_phase_end = *p++;
  demo_phase_start = 0;
  demo_phase_list = p;
}

void demo_scene_36_draw(void)
{
  float progress;
  
  progress = (float)demo_phase_progress();
  demo_layout_draw(demo_scene_36_layouts_a,(int)demo_phase,progress);
  demo_layout_draw(demo_scene_36_layouts_b,(int)demo_phase,progress);
  demo_band_draw(1,(int)demo_phase,progress);
  demo_band_draw(2,(int)demo_phase,progress);
  return;
}

void demo_rob_update(void)
{
  uchar side;
  uchar slot;
  int costumes;
  rob_rec *rob;
  int chara_data;
  uint flags;
  uchar *chara;
  uchar i;
  uchar *model_slot;
  uchar *costume;
  char *s;
  demo_rob_request *e;
  float y;

  if ((((demo_flags & 0x20U) != 0) && (poly_queue_busy_get() == 0)) && (load_pending_get() == 0)) {
    stage_apply();
    stage_field_init();
    demo_flags = demo_flags & 0xffdf;
  }
  if ((((demo_flags & 4U) != 0) && (poly_queue_busy_get() == 0)) &&
      ((load_pending_get() == 0 && ((demo_flags & 0x50U) == 0)))) {
    s = demo_chara_req;
    rob = rob_get((uchar)s[0]);
    rob_color_build_plain(s[1],s[2]);
    demo_rob_model_load(s[1],s[2],demo_model_slot);
    rob->field_0x570 = s[1];
    rob->field_0x571 = s[2];
    rob->field_0x573 = demo_model_slot;
    demo_flags = demo_flags & 0xfffbU | 8;
  }
  if ((((demo_flags & 0x10U) != 0) && (poly_queue_busy_get() == 0)) && (load_pending_get() == 0)) {
    side = demo_apply_side;
    rob = rob_get((uint)side);
    chara = &rob->chara;
    *chara = rob->field_0x570;
    costume = &rob->costume;
    *costume = rob->field_0x571;
    model_slot = &rob->model_slot;
    demo_model_slot = *model_slot;
    *model_slot = rob->field_0x573;
    rob_items_setup((int)(char)side,(int)(char)*chara,(int)(char)*costume);
    rob_chara_bind(rob,*chara);
    rob_round_reset(rob);
    rob_skeleton_init(rob);
    rob_disp_setup(rob);
    rob->flags = rob->flags & 0xfffffffe;
    slot = *model_slot;
    chara_data = (int)chara_data_get((uint)*chara);
    costumes = *(int *)(chara_data + 0x48);
    rob_hand_build_start((uint)slot,*(uint *)(costumes + (uint)*costume * 0x20 + 0xc));
    demo_flags = demo_flags | 0x40;
    rob->field_0x574 = rob->field_0x574 | 0x40;
    demo_flags = demo_flags & 0xffe7;
  }
  if (((demo_flags & 0x40U) != 0) && (rob_hand_build_step() != 0)) {
    rob = rob_get((uint)demo_apply_side);
    demo_flags = demo_flags & ~0x40;
    rob->field_0x574 = rob->field_0x574 & ~0x40;
  }
  i = 0;
  do {
    rob = rob_get(i);
    e = &demo_rob_req[i];
    if ((e->flags & 8) != 0) {
      rob->pos.x = 0.0;
      rob->ground_pos.x = 0.0;
      rob->place_pos.x = 0.0;
      y = stage_cur->center.y;
      rob->pos.y = y;
      rob->ground_pos.y = y;
      rob->place_pos.y = y;
      rob->pos.z = 0.0;
      rob->ground_pos.z = 0.0;
      rob->place_pos.z = 0.0;
      rob->heading = 0;
      rob_move_set(rob,(uint)e->motion);
      rob->frame = 1;
      rob->frame_pos = 1.0;
      rob->action.word = (uint)rob->move_no;
      rob->action_step = 1;
    }
    flags = rob->flags;
    if ((e->flags & 1) != 0) {
      flags = flags & e->and_mask;
    }
    if (((e->flags & 2) != 0) && (flags = flags | e->or_mask, (e->or_mask & 1) != 0)) {
      flags = flags | 0x100000;
    }
    if ((e->flags & 4) != 0) {
      flags = e->set;
    }
    rob->flags = flags;
    e->flags = 0;
    i++;
  } while (i < 2);
}

void demo_cleanup(void)
{
  demo_cleanup_hook();
  demo_aet_close();
  demo_flags = 0;
  return;
}

void demo_rob_model_load(int chara, int costume, int slot)
{
  objbank_free_req(slot + 5);
  objbank_free_req(slot + 9);
  load_poly_req(slot + 5,*(int *)(costume * 0x20 + *(int *)((char *)chara_data_get(chara) + 0x48) + 4),
               &objmem_heap_chr[slot],&rob_obj_ofs[slot],&heap_texdata,
               &heap_texdata,rob_tex_bases[slot]);
}

void demo_start_hook(void)
{
  return;
}

void demo_cleanup_hook(void)
{
  return;
}

void demo_scene_draw(void)
{
  if ((demo_flags & 0x100U) != 0) {
    if ((demo_flags & 0x200U) != 0) {
      sprite_put_xyz_scaled_flags(0,0.0,0.0,0.03,80.0,7.5,0x805);
      sprite_put_xyz_scaled_flags(0,0.0,420.0,0.03,80.0,7.5,0x805);
    }
    else {
      sprite_put_xyz_scaled(0,0.0,0.0,0.03,80.0,7.5);
      sprite_put_xyz_scaled(0,0.0,420.0,0.03,80.0,7.5);
    }
  }
  if (pause_frame_held() == 0) {
    demo_scene_frame = demo_scene_frame + 1;
  }
  if (demo_scene_draw_table[demo_scene] != 0) {
    (*demo_scene_draw_table[demo_scene])();
  }
  if ((demo_flags & 0x4000U) != 0) {
    if (pause_frame_held() == 0) {
      demo_flash_alpha = demo_flash_alpha + demo_flash_step;
    }
    if (demo_flash_alpha > 1.0) {
      demo_flash_alpha = 1.0;
    }
    else if (demo_flash_alpha < 0.0) {
      demo_flash_step = 0.0;
      demo_flash_alpha = 0.0;
      demo_flags = demo_flags & 0xbfff;
    }
    sprite_white_rect_draw(0.0,0.0,80.0,60.0,demo_flash_alpha);
  }
  if ((demo_flags & 0x2000U) != 0) {
    sprite_white_rect_draw(0.0,0.0,80.0,60.0,demo_white_in);
    if (pause_frame_held() == 0) {
      demo_white_in = demo_white_in + 0.01;
    }
    if (1.0 < demo_white_in) {
      demo_white_in = 1.0;
    }
  }
}

void demo_layout_draw(demo_layout_rec *layouts, char phase, float progress)
{
  demo_layout_rec *l;
  demo_sprite_rec *e;
  float scale;
  float one;
  float alpha;
  float mx;
  float my;
  float off;
  float ox;
  float oy;

  scale = (1.0 - progress) * 2.0 + 1.0;
  one = 1.0;
  alpha = 0.5;
  l = layouts + phase;
  e = l->list;
  switch (l->type) {
  case 1:
    mx = 640.0;
    my = 480.0;
    for (; e->id >= 0; e++) {
      if (e->x < mx)
        mx = e->x;
      if (e->y < my)
        my = e->y;
    }
    off = (1.0 - progress) * 640.0;
    e = l->list;
    ox = l->x - mx;
    oy = l->y - my;
    switch (phase) {
    case 1:
      for (; e->id >= 0; e++)
        sprite_put_xy_scaled(e->id, ox + e->x + off, oy + e->y, scale, one);
      break;
    case 3:
      alpha = (1.0 - progress) * 0.5;
    case 2:
      for (; e->id >= 0; e++)
        sprite_put_xy_alpha(e->id, ox + e->x, oy + e->y, alpha);
      break;
    }
    break;
  case 2:
    mx = 0.0;
    my = 0.0;
    for (; e->id >= 0; e++) {
      if (e->x > mx)
        mx = e->x;
      if (e->y > my)
        my = e->y;
    }
    off = (1.0 - progress) * -640.0;
    e = l->list;
    ox = 640.0 - mx - l->x;
    oy = 480.0 - my - l->y;
    switch (phase) {
    case 1:
      for (; e->id >= 0; e++)
        sprite_put_xy_scaled_flags(e->id, ox + e->x + off, oy + e->y, scale, one, 15);
      break;
    case 3:
      alpha = (1.0 - progress) * 0.5;
    case 2:
      for (; e->id >= 0; e++)
        sprite_put_xy_alpha_flags(e->id, ox + e->x, oy + e->y, alpha, 15);
      break;
    }
    break;
  case 3:
    mx = 640.0;
    my = 0.0;
    for (; e->id >= 0; e++) {
      if (e->x < mx)
        mx = e->x;
      if (e->y > my)
        my = e->y;
    }
    off = (1.0 - progress) * 640.0;
    e = l->list;
    ox = l->x - mx;
    oy = 480.0 - my - l->y;
    switch (phase) {
    case 1:
      for (; e->id >= 0; e++)
        sprite_put_xy_scaled_flags(e->id, ox + e->x + off, oy + e->y, scale, one, 13);
      break;
    case 3:
      alpha = (1.0 - progress) * 0.5;
    case 2:
      for (; e->id >= 0; e++)
        sprite_put_xy_alpha_flags(e->id, ox + e->x, oy + e->y, alpha, 13);
      break;
    }
    break;
  case 4:
    mx = 0.0;
    my = 480.0;
    for (; e->id >= 0; e++) {
      if (e->x > mx)
        mx = e->x;
      if (e->y < my)
        my = e->y;
    }
    off = (1.0 - progress) * -640.0;
    e = l->list;
    ox = 640.0 - mx - l->x;
    oy = l->y - my;
    switch (phase) {
    case 1:
      for (; e->id >= 0; e++)
        sprite_put_xy_scaled_flags(e->id, ox + e->x + off, oy + e->y, scale, one, 7);
      break;
    case 3:
      alpha = (1.0 - progress) * 0.5;
    case 2:
      for (; e->id >= 0; e++)
        sprite_put_xy_alpha_flags(e->id, ox + e->x, oy + e->y, alpha, 7);
      break;
    }
    break;
  }
}

void demo_band_draw(char band, char phase, float progress)
{
  float half = 0.5;
  int u = 0x860;
  float f = progress;
  float x;
  float y;

  if (demo_type == 1) u = 0x3a54;
  if (phase != 0) f = 1.0;
  if (phase == 3) half = (1.0 - progress) * 0.5;
  switch (band) {
  case 1:
    x = -288.0 - (1.0 - f) * 640.0;
    sprite_put_xy_scaled_alpha(u, x + 288.0, 90.0, 10.0, 1.0, half);
    break;
  case 2:
    x = (1.0 - f) * 640.0 + 289.0;
    sprite_put_xy_scaled_flags_alpha(u, x + 351.0, 390.0, 10.0, 1.0, half, 0xf);
    break;
  case 3:
    x = -288.0 - (1.0 - f) * 640.0;
    sprite_put_xy_scaled_alpha(u, x + 288.0, 390.0, 10.0, 1.0, half);
    break;
  case 4:
    x = (1.0 - f) * 640.0 + 289.0;
    sprite_put_xy_scaled_alpha(u, x + 351.0, 90.0, 10.0, 1.0, half);
    break;
  case 5:
    y = f * 480.0 + -232.0;
    sprite_put_xyz_scaled_alpha_angle_flags(u, 610.0, y + 232.0, 0.05, 7.5, 1.0, half, 0x4000, 5);
    break;
  case 6:
    y = (1.0 - f) * 480.0 + 248.0;
    sprite_put_xyz_scaled_alpha_angle_flags(u, 30.0, y + 232.0, 0.05, 7.5, 1.0, half, 0x4000, 5);
    break;
  case 7:
    y = f * 480.0 + -232.0;
    sprite_put_xyz_scaled_alpha_angle_flags(u, 30.0, y + 232.0, 0.05, 7.5, 1.0, half, 0x4000, 5);
    break;
  }
}

float demo_phase_progress(void)
{
  short *p;

  if (demo_phase_end <= demo_scene_frame) {
    if (*demo_phase_list > 0) {
      demo_phase_start = demo_phase_end;
      p = demo_phase_list;
      demo_phase_end = demo_phase_end + *p++;
      demo_phase_list = p;
      demo_phase = demo_phase + '\x01';
    }
    else if (*demo_phase_list == -2) {
      return 1.0;
    }
    return 0.0;
  }
  return (float)(demo_scene_frame - demo_phase_start) / (float)(demo_phase_end - demo_phase_start);
}

void demo_layout_fade_out(demo_layout_rec *layout)
{
  demo_sprite_rec *e;
  float r;
  float k;
  float mx;
  float my;
  float ox;
  float oy;
  e = layout->list;
  k = 1.0;
  r = (float)demo_scene_frame / (float)demo_phase_end;
  if (r < k) {
    k = k - r;
    mx = 640.0;
    my = 480.0;
    for (; e->id >= 0; e++) {
      if (e->x < mx) mx = e->x;
      if (e->y < my) my = e->y;
    }
    e = layout->list;
    ox = layout->x - mx;
    oy = layout->y - my;
    for (; e->id >= 0; e++) {
      sprite_put_xy_alpha_flags(e->id, ox + e->x, oy + e->y, k, 5);
    }
  }
}

void demo_aet_open(void)
{
  int i;
  aet_set_load(0x1c);
  aet_scene_textures_load(0x1c0001);
  aet_scene_textures_load(0x1c0000);
  demo_flags = demo_flags | 0x400;
  for (i = 0; i < 0x19; i++) {
    demo_aet_layers[i].handle = -1;
  }
}

void demo_aet_close(void)
{
  aet_objs_stop_by_set(0x1c);
  if ((demo_flags & 0x400) != 0) {
    aet_scene_textures_free(0x1c0001);
    aet_scene_textures_free(0x1c0000);
    aet_set_free(0x1c);
    demo_flags = demo_flags & 0xfbff;
  }
  return;
}
