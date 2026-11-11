/*
 * vf4lib/libdebug/src/debug_menu.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_led_write_if_idle
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern camera_state camera; /* 0x0c370ce0 */
extern uchar led_blink_patterns; /* 0x0c1e2c38 */
extern float perf_render_usec; /* 0x0c292590 */
extern float perf_ta_usec; /* 0x0c292594 */
extern float perf_render_usec_avg; /* 0x0c292598 */
extern float perf_ta_usec_avg; /* 0x0c29259c */
extern float perf_render_usec_max; /* 0x0c2925a0 */
extern float perf_ta_usec_max; /* 0x0c2925a4 */
extern char champion_side; /* 0x0c2925d4 */
extern uchar *guard_motion_tables[86]; /* 0x0c29fcdc */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern float perf_frame_count; /* 0x0c32281c */
extern uchar *poly_test_bg_colors; /* 0x0c3228c0 */
extern uchar rob_pos_presets; /* 0x0c322910 */
extern uchar *level_item_counts; /* 0x0c322cb0 */
extern uchar *level_item_lists; /* 0x0c322cfc */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern uint perf_test_time; /* 0x0c3d77b0 */
extern uchar camera_mode_saved; /* 0x0c5555e4 */
extern cpu_false_stats_rec cpu_false_stats; /* 0x0c2925fc */
extern render_false_stats_rec render_false_stats; /* 0x0c292608 */
extern level_change_items_rec level_change_items; /* 0x0c322d4c */
extern camera_state poly_test_camera; /* 0x0c3d62c8 */
extern motion_test_rec motion_test; /* 0x0c3d77c4 */
extern frame_check_rec frame_check; /* 0x0c3d7828 */

void perf_color_mark(uint color)
{
  return;
}

int led_blink_pattern(void)
{
  return (int)(char)(&led_blink_patterns)[(vblank_count & 8) >> 3];
}

void led_write_if_idle(value) char value;
{
  long busy;

  busy = rom_dma_busy();
  if (busy == 0) {
    led_latch_byte((uchar)value);
  }
  return;
}

void led_blink_update(void)
{
  int pattern;

  pattern = led_blink_pattern();
  led_write_if_idle(pattern);
  return;
}

void perf_frame_stats_update(void)
{
  int usec;
  float *p = &perf_render_usec;

  usec = frame_stats_drawing_usec();
  *p = (float)usec;
  usec = frame_stats_vertex_input_usec();
  perf_ta_usec = (float)usec;
  frame_stats_capture();
  perf_frame_count = perf_frame_count + 1.0;
  if (*p > perf_render_usec_max) {
    perf_render_usec_max = *p;
  }
  if (perf_ta_usec > perf_ta_usec_max) {
    perf_ta_usec_max = perf_ta_usec;
  }
  perf_render_usec_sum = perf_render_usec_sum + perf_render_usec;
  perf_ta_usec_sum = perf_ta_usec_sum + perf_ta_usec;
  perf_render_usec_avg = perf_render_usec_sum / perf_frame_count;
  perf_ta_usec_avg = perf_ta_usec_sum / perf_frame_count;
  if (task_stats_frame == 0) {
    perf_render_usec_avg = 0.0;
    perf_render_usec_max = 0.0;
    perf_render_usec_sum = 0.0;
    perf_ta_usec_avg = 0.0;
    perf_ta_usec_max = 0.0;
    perf_ta_usec_sum = 0.0;
    perf_frame_count = 0.0;
  }
  return;
}

int win_hide_combo_held(void)
{
  player_input_rec *input;
  int held;

  held = 0;
  input = player_input_get(1);
  if (((input->now & 0x10) != 0) &&
     (input = player_input_get(1), (input->now & 7) == 7)) {
    held = 1;
  }
  return held;
}

void perf_false_count(void)
{
  int n;
  float usec;
  n = stopwatch_usec();
  usec = n;
  if (0 < ((int) (((float) usec) * 5.9999882e-05)))
  {
    cpu_false_stats.late = cpu_false_stats.late + 1;
    cpu_false_stats.frames = cpu_false_stats.frames + ((int) (((float) usec) * 5.9999882e-05));
  }
  usec = perf_render_usec;
  if (((int) (usec * 5.9999882e-05)) != 0)
  {
    if (((int) (usec * 5.9999882e-05)) == 1)
    {
      render_false_stats.one = render_false_stats.one + 1;
    }
    else
    {
      n = render_false_stats.more + 1;
      render_false_stats.more = n;
    }
  }
  return;
}

void debug_boot_stub(void)
{
  return;
}

void break_ab_clear(void)
{
  ((volatile short *)0xff200008)[0] = 0;
  ((volatile short *)0xff200008)[6] = 0;
}

void break_a_read_set(uint address, int address_mask)
{
  *(volatile uint *)0xff200000 = address;
  *(volatile uchar *)0xff200004 = address_mask;
  *(volatile short *)0xff200008 = 0x34;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_b_read_set(uint address, int address_mask)
{
  *(volatile uint *)0xff20000c = address;
  *(volatile uchar *)0xff200010 = address_mask;
  *(volatile short *)0xff200014 = 0x34;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_a_access_set(uint address, int address_mask)
{
  *(volatile uint *)0xff200000 = address;
  *(volatile uchar *)0xff200004 = address_mask;
  *(volatile short *)0xff200008 = 0x3c;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_b_access_set(uint address, int address_mask)
{
  *(volatile uint *)0xff20000c = address;
  *(volatile uchar *)0xff200010 = address_mask;
  *(volatile short *)0xff200014 = 0x3c;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_a_write_set(uint address, int address_mask)
{
  *(volatile uint *)0xff200000 = address;
  *(volatile uchar *)0xff200004 = address_mask;
  *(volatile short *)0xff200008 = 0x38;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_b_write_set(uint address, int address_mask)
{
  *(volatile uint *)0xff20000c = address;
  *(volatile uchar *)0xff200010 = address_mask;
  *(volatile short *)0xff200014 = 0x38;
  *(volatile short *)0xff200020 = 0;
  return;
}

void break_b_write_value_set(uint address, int address_mask, ushort size, uint value)
{
  *(volatile uint *)0xff20000c = address;
  *(volatile uchar *)0xff200010 = address_mask;
  *(volatile short *)0xff200014 = 0x38 | size;
  if (size == 1) {
    value |= value << 8;
  }
  *(volatile uint *)0xff200018 = value;
  *(volatile uint *)0xff20001c = 0;
  *(volatile short *)0xff200020 = 0x80;
  return;
}

void break_b_write_value_mask_set(uint address, int address_mask, ushort size, uint value, uint value_mask)
{
  *(volatile uint *)0xff20000c = address;
  *(volatile uchar *)0xff200010 = address_mask;
  *(volatile short *)0xff200014 = 0x38 | size;
  if (size == 1) {
    value |= value << 8;
  }
  *(volatile uint *)0xff200018 = value;
  *(volatile uint *)0xff20001c = value_mask;
  *(volatile short *)0xff200020 = 0x80;
  return;
}

void stats_322804_clear(void)
{
  int i;

  for (i = 0; i < 2; i++) {
    stats_322804[i] = 0.0f;
  }
  stats_32280c = 0.0f;
  return;
}

void switch_29_latch(void)
{
  int held;

  held = input_merged_bit_held(0x1d);
  if (held != 0) {
    switch_29_latched = 1;
  }
  return;
}

void empty_fn_0c0edd20(void)
{
  return;
}

void polygon_test_open(void)
{
  task_tree_pause();
  poly_test_saved_mode = mode_get_main();
  mode_req_main(MODE_MAIN_TEST);
  poly_test_saved_sub_mode = mode_get_sub();
  mode_req_sub(2);
  return;
}

void polygon_test_close(void)
{
  bg_color_set(stage_cur->bg_colors);
  task_tree_resume();
  mode_req_main(poly_test_saved_mode);
  mode_req_sub(poly_test_saved_sub_mode);
  return;
}

void poly_test_init(void)
{
  float z = 0.0;
  bank_num = 1;
  obj_num = 0;
  poly_test_bank_loaded = 1;
  poly_test_camera.eye.x = z;
  poly_test_camera.eye.y = z;
  poly_test_camera.eye.z = -5.0;
  poly_test_camera.target.x = z;
  poly_test_camera.target.y = z;
  poly_test_camera.target.z = z;
  poly_test_camera.pitch = poly_test_camera.yaw = 0;
  poly_test_bg_color = '\n';
  mode_req_sub(3);
}

void poly_test_bank_reload(void)
{
  if (bank_num != poly_test_bank_loaded) {
    poly_test_bank_loaded = (int)bank_num;
    objbank_free(0xf);
    obj_tex_base_set(0x5dc);
  }
  return;
}

void poly_test_ctrl(void)
{
  uint a;
  uint b;
  int n;
  int i;
  float f;
  float *c;
  camera_state *cam;
  int v;
  int t;
  float up[3];
  float m[3];

  bg_color_set((uint *)(&poly_test_bg_colors)[poly_test_bg_color]);
  n = 0;
  for (i = 0; i < 21; i++) {
    if (objbanks[i].nodes != 0) {
      poly_test_banks[n] = i;
      n++;
    }
  }
  poly_test_obj_names = objbanks[poly_test_banks[bank_num]].names;
  mtx_push_identity();
  a = player_input_get(0)->now & 2;
  b = player_input_get(1)->now & 2;
  f = vec3_distance(&poly_test_camera.target.x,&poly_test_camera.eye.x);
  if (f <= 0.1) f = 0.1;
  f = f * 0.012;
  if ((a == 0) & (b == 0)) {
    test_cam_move(0,&poly_test_camera,f);
    test_cam_move(1,&poly_test_camera,f);
  }
  cam = &poly_test_camera;
  camera_angles_update(cam);
  c = &camera.proj.near_clip;
  mtx_stack_select(3);
  mtx_load_identity();
  proj_from_vfov((int)((c[2] * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,c[0],c[1]);
  proj_cmds_send();
  mtx_stack_select(1);
  up[0] = 0.0;
  up[1] = 1.0;
  up[2] = 0.0;
  mtx_mul_eye_target_up_flip(&cam->eye.x,&cam->target.x,up);
  lights_xform_by_current();
  light_unit_axis_set(0,light_sets[0][0].dir[0],light_sets[0][0].dir[1],light_sets[0][0].dir[2]);
  mtx_xform_dir(light_sets[0][0].dir,m);
  obj_light_dir_put_v(m);
  light_set_send(0,0);
  if (poly_test_blend_mode == 1) blend_pair_bits = 0x78;
  lights_xform_by_current();
  obj_vertex_hook_select(poly_test_vertex_hook);
  v = objbanks[poly_test_banks[bank_num]].set;
  t = v << 16 | obj_num;
  poly_test_set = v;
  obj_draw(t);
  if (poly_test_blend_mode == 1) blend_pair_bits = 0x68;
  mtx_pop(1);
}

void camera_menu_open(void)
{
  camera_mode_saved = camera.mode;
  camera_mode_return_saved = camera.mode_return;
  return;
}

void camera_menu_close(void)
{
  camera_reset();
  camera.mode = camera_mode_saved;
  camera.mode_return = camera_mode_return_saved;
  return;
}

void texture_test_draw(win_rec *win)
{
  typedef struct { float x; float y; float z; float u; float v; int col; int spec; } tex_vtx;
  tex_vtx vtx[4];
  int i;
  int n;
  int k;
  int tex;
  float x0, y0, w, h, u, v;
  float du, dv;
  n = 0;
  for (i = 0; i < 0x5250; i++) {
    if (texidx_map[i].use_count != 0) {
      tex_test_list[n] = i;
      n++;
    }
  }
  k = tex_test_list[deb_idx];
  tex_slot_current = tex_slot_table + texidx_map[k].slot_no;
  tex_test_tex_no = k;
  x0 = (float)(win->x + 1) * 8.0;
  y0 = (float)(win->y + 4) * 8.0;
  w = (float)win->width * 8.0;
  h = (float)(win->height - 2) * 8.0;
  u = w / (float)(int)tex_slot_current->surface.width;
  v = h / (float)(int)tex_slot_current->surface.height;
  du = (float)(0x1000 / (int)tex_slot_current->surface.width);
  dv = (float)(0x1000 / (int)tex_slot_current->surface.height);
  if ((player_input_get(0)->now & 0x20) != 0) {
    tex_test_scroll_u = du + du + tex_test_scroll_u;
    if ((1.0 - u) * 4096.0 < tex_test_scroll_u) {
      tex_test_scroll_u = (1.0 - u) * 4096.0;
    }
  }
  if ((player_input_get(0)->now & 0x10) != 0) {
    tex_test_scroll_u = tex_test_scroll_u - (du + du);
    if (tex_test_scroll_u < 0.0) {
      tex_test_scroll_u = 0.0;
    }
  }
  if ((player_input_get(0)->now & 0x80) != 0) {
    tex_test_scroll_v = tex_test_scroll_v - (dv + dv);
    if (tex_test_scroll_v < 0.0) {
      tex_test_scroll_v = 0.0;
    }
  }
  if ((player_input_get(0)->now & 0x40) != 0) {
    tex_test_scroll_v = dv + dv + tex_test_scroll_v;
    if ((1.0 - v) * 4096.0 < tex_test_scroll_v) {
      tex_test_scroll_v = (1.0 - v) * 4096.0;
    }
  }
  if (1.0 < u) {
    tex_test_scroll_u = 0.0;
  }
  if (1.0 < v) {
    tex_test_scroll_v = 0.0;
  }
  for (i = 0; i < 4; i++) {
    vtx[i].z = 1000.0;
    vtx[i].col = -1;
    vtx[i].spec = 0;
  }
  vtx[0].x = x0;
  vtx[0].y = y0;
  vtx[0].u = tex_test_scroll_u * 0.00024414062 + 0.0;
  vtx[0].v = v + tex_test_scroll_v * 0.00024414062;
  vtx[1].x = x0;
  vtx[1].y = h + y0;
  vtx[1].u = vtx[0].u;
  vtx[1].v = tex_test_scroll_v * 0.00024414062 + 0.0;
  vtx[2].x = w + x0;
  vtx[2].y = y0;
  vtx[2].u = tex_test_scroll_u * 0.00024414062 + u;
  vtx[2].v = vtx[0].v;
  vtx[3].x = vtx[2].x;
  vtx[3].y = vtx[1].y;
  vtx[3].u = vtx[2].u;
  vtx[3].v = vtx[1].v;
  tex = texidx_map[tex_test_list[deb_idx]].slot_no;
  polystrip_tex_blended(4, (uint *)vtx, tex);
}

void stage_reload_selected(void)
{
  stage_load_apply(debug_stage_no);
  return;
}

void stage_slot_select(void)
{
  debug_stage_entry_ptr = &stage_table[debug_stage_no];
  debug_stage_entry = *debug_stage_entry_ptr;
  return;
}

void fog_menu_update(void)
{
  fog_task_sync();
  return;
}

void mtx_aim_along(float *dir)
{
  float x;
  float y;
  float z;
  float d;
  float r;
  float r2;

  x = dir[0];
  y = dir[1];
  z = dir[2];
  d = x * x + y * y;
  r = d;
  __asm__ ("fsrra %0" : "+f" (r));
  mtx_rot_z_sc(y * r, x * r);
  r = r * d;
  r2 = z * z + d;
  __asm__ ("fsrra %0" : "+f" (r2));
  mtx_rot_y_sc(-z * r2, r * r2);
}

void light_marker_draw(set, light) short set; int light;
{
  uint obj;

  if (light_disp == ON_OFF_ON) {
    mtx_push(0);
    camera_view_load();
    if (light_sets[set][light].type == 2) {
      mtx_translate_by_vec3(&stage_cur->center.x);
    }
    mtx_translate_by_vec3(light_sets[set][light].pos);
    if (light_sets[set][light].type == 3) {
      mtx_scale_xyz(0.15,0.15,0.15);
      obj = 0x46;
    }
    else {
      mtx_aim_along(light_sets[set][light].dir);
      mtx_scale_xyz(0.3,0.3,0.3);
      obj = 0x24;
    }
    obj_draw(obj);
    mtx_pop(1);
  }
  return;
}

void light_test_draw(void)
{
  int i;
  i = 0;
  do
  {
    if (1 < light_sets[light_tag][i].type)
    {
      light_marker_draw((uint) light_tag, i);
    }
    i = i + 1;
  }
  while (i < 0x10);
  light_selected = &light_sets[light_tag][light_num];
  ambient_selected = &light_set_ambient[light_tag];
  return;
}

void ambient_draw(void)
{
  ambient_selected = &light_set_ambient[light_tag];
  return;
}

void dbg_stg_light_set(void)
{
  short *p = &stg_light_num;
  if (*p >= 0) {
    light_set_load(0,*(unsigned char *)p);
  }
}

void dbg_rob_light_set(void)
{
  float *d;
  float *s;
  light_src_rec *bd;
  light_src_rec *bs;
  uint i;
  
  if (rob_light_num < 0) {
    d = (float *)&light_set_ambient[1];
    s = d - 4;
    bd = light_sets[1];
    bs = light_sets[0];
    for (i = 0; i < 4; i++) {
      *d++ = *s++;
    }
    for (i = 0; i < 0x10; i++) {
      *bd++ = *bs++;
    }
  }
  else {
    light_set_load(1,(uchar)rob_light_num);
  }
}

void window_trap(void)
{
  __asm__ volatile ("trapa #0x50");
  return;
}

void gravity_preset_apply(char index)
{
  float gravity;

  if ((uint)index < 7) {
    gravity = gravity_presets[index];
  }
  else {
    gravity = 0.0029944447f;
  }
  rope_cloth_gravity = gravity;
  world_gravity = gravity;
  return;
}

void rob_pos_preset_apply(void)
{
  typedef struct { float x0; float z0; short heading0; short pad; float x1; float z1; short heading1; short pad2; } ent;
  rob_rec *rob0;
  rob_rec *rob1;
  ent *e;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  e = (ent *)&rob_pos_presets + rob_pos_preset_no;
  rob0->place_pos.y = stage_cur->center.y;
  rob1->place_pos.y = stage_cur->center.y;
  rob0->place_pos.x = e->x0;
  rob0->pos.x = e->x0;
  rob0->place_pos.z = e->z0;
  rob0->pos.z = e->z0;
  rob0->heading = e->heading0;
  rob1->place_pos.x = e->x1;
  rob1->pos.x = e->x1;
  rob1->place_pos.z = e->z1;
  rob1->pos.z = e->z1;
  rob1->heading = e->heading1;
  rob_move_set(rob0,*rob0->move_table);
  rob_move_set(rob1,*rob1->move_table);
}

void mothead_sound_init(void)
{
  int i;

  rob = 0;
  for (i = 0; i < 8; i++) {
    cv_type[i] = 0;
    cv_coma[i] = 0;
    snd_type[i] = 0;
    snd_coma[i] = 0;
  }
  return;
}

void mothead_sound_play(void)
{
  rob_rec *r;
  int i;

  r = rob_get(rob);
  if ((r->side_flags & 0x20) != 0) {
    mothead_voice_played = 0;
    mothead_sound_played = 0;
  }
  for (i = 0; i < 8; i++) {
    if (cv_coma[i] > 0) {
      byte *f = &mothead_voice_played;
      char v = *f;
      if ((((byte)v >> i) & 1) == 0 && cv_coma[i] == r->frame &&
          (ushort)(cv_type[i] - CV_WAZA_1) <= 0x5c) {
        *f = (1 << i) | v;
        rob_voice_request(r,cv_type_voices[cv_type[i]]);
      }
    }
    if (snd_coma[i] > 0) {
      byte *f = &mothead_sound_played;
      char v = *f;
      if ((((byte)v >> i) & 1) == 0 && snd_coma[i] == r->frame &&
          (ushort)(snd_type[i] - SD_GUARD) <= 0x2c) {
        *f = (1 << i) | v;
        sound_request(r,snd_type_sounds[snd_type[i]]);
      }
    }
  }
  return;
}

void rob_start_pos_apply(void)
{
  rob_rec *rob;
  int s;
  int i;
  float f;
  float v;

  s = stage_cur->start_pos;
  rob = rob_get(0);
  for (i = 0; i < 2; i++, s += 0x10, rob++) {
    switch (rob_start_pos_mode) {
    case 0:
      f = 0.0;
      rob->side_flags &= 0xffe7;
      break;
    case 1:
      f = 0.0;
      rob->side_flags |= 0x18;
      break;
    case 2:
      f = 4.0;
      rob->side_flags &= 0xffe7;
      break;
    case 3:
    default:
      f = 4.0;
      rob->side_flags |= 0x18;
      break;
    }
    if (*(float *)(s + 4) < 0.0) {
      f = f * -1.0;
    }
    v = *(float *)(s + 4) + f;
    rob->pos.x = v;
    rob->ground_pos.x = v;
    rob->place_pos.x = v;
    v = *(float *)(s + 8);
    rob->pos.y = v;
    rob->ground_pos.y = v;
    rob->place_pos.y = v;
    v = *(float *)(s + 0xc);
    rob->pos.z = v;
    rob->ground_pos.z = v;
    rob->place_pos.z = v;
    rob->heading = *(short *)(s + 2);
    rob->flags &= 0xfffffff7;
    rob->state |= 0x10;
  }
  camera.flags = camera.flags | 0x40;
}

void rob_start_pos_reset(void)
{
  rob_start_pos_mode = 0;
  rob_start_pos_apply();
  return;
}

void rob_start_pos_cycle(void)
{
  player_input_rec *input;

  input = player_input_get(0);
  if (((input->now & 1) != 0) &&
     (input = player_input_get(1), (input->pressed & 1) != 0)) {
    rob_start_pos_mode = rob_start_pos_mode + 1;
    if (3 < rob_start_pos_mode) {
      rob_start_pos_mode = 0;
    }
    rob_start_pos_apply();
  }
  return;
}

void debug_malloc_update(win_rec *w)
{
  main_heap_stats_get((struct mallinfo *)(w->value_base + 0x40));
  return;
}

void level_change_open(void)
{
  uint f;
  card_record_rec *q;
  uchar i;
  uchar k;
  uchar *lim;
  uchar **t;
  uchar *s;
  int m;
  uchar n;
  int a;
  int e;

  debug_dan_i = 0;
  level_change_items.c0_slot0 = 0;
  level_change_items.c0_slot1 = 0;
  level_change_items.c0_slot2 = 0;
  level_change_items.c0_slot3 = 0;
  level_change_items.c1_slot0 = 0;
  level_change_items.c1_slot1 = 0;
  (&level_change_items.c1_slot0)[2] = 0;
  (&level_change_items.c1_slot0)[3] = 0;
  f = card_flags_get(0);
  q = card_record_get(0);
  if ((f & 1) != 0) {
    if ((f & 0x80) != 0) {
      debug_dan_i = q->header.level;
      level_change_items.c0_slot0 = item_slot_get(0,0,0);
      level_change_items.c0_slot1 = item_slot_get(0,0,1);
      level_change_items.c0_slot2 = item_slot_get(0,0,2);
      level_change_items.c0_slot3 = item_slot_get(0,0,3);
      level_change_items.c1_slot0 = item_slot_get(0,1,0);
      level_change_items.c1_slot1 = item_slot_get(0,1,1);
      (&level_change_items.c1_slot0)[2] = item_slot_get(0,1,2);
      (&level_change_items.c1_slot0)[3] = item_slot_get(0,1,3);
      (&entry_0)[0] = 0;
      (&entry_0)[1] = 0;
      (&entry_0)[2] = 0;
      (&entry_0)[3] = 0;
      (&entry_0)[4] = 0;
      (&entry_0)[5] = 0;
      (&entry_0)[6] = 0;
      (&entry_0)[7] = 0;
      m = card_record_char_get(0);
      t = ((uchar ***)&level_item_lists)[m];
      lim = ((uchar **)&level_item_counts)[m];
      for (i = 0; i < 4; i++, lim++) {
        s = t[i];
        n = *lim;
        for (k = 0; k < n; k++) {
          uchar *ab = &level_change_items.c0_slot0;
          if (s[k] == ab[i]) {
            e = (int)&entry_0;
            *(uchar *)(e + i) = k;
          }
          if (s[k] == (uchar)(&level_change_items.c1_slot0)[i]) (&entry_0_0c322d58)[i] = k;
        }
      }
    }
  }
}

void level_change_update(void)
{
  uint f;
  rob_rec *rob;
  int i;
  uchar *lim;
  char **t;
  uchar a, b, c, d;
  uchar *e;
  char *o;
  char *s;

  f = card_flags_get(0);
  card_image_get(0);
  card_record_get(0);
  rob = rob_get(0);
  if ((f & 1) != 0) {
    if ((f & 0x80) != 0) {
      if ((rob->flags & 1) != 0) {
        i = card_record_char_get(0);
        lim = ((uchar **)&level_item_counts)[i];
        t = ((char ***)&level_item_lists)[i];
        a = lim[0];
        b = lim[1];
        c = lim[2];
        d = lim[3];
        if (entry_0 >= a) entry_0 = a - 1;
        if (entry_0_0c322d55 >= b) entry_0_0c322d55 = b - 1;
        if (entry_0_0c322d56 >= c) entry_0_0c322d56 = c - 1;
        if (entry_0_0c322d57 >= d) entry_0_0c322d57 = d - 1;
        if (entry_0_0c322d58 >= a) entry_0_0c322d58 = a - 1;
        if (entry_0_0c322d59 >= b) entry_0_0c322d59 = b - 1;
        if (entry_0_0c322d5a >= c) entry_0_0c322d5a = c - 1;
        if (entry_0_0c322d5b >= d) entry_0_0c322d5b = d - 1;
        s = t[0];
        o = (char *)&level_change_items;
        e = &entry_0;
        o[0] = s[e[0]];
        o[4] = s[e[4]];
        s = t[1];
        o[1] = s[e[1]];
        o[5] = s[e[5]];
        s = t[2];
        o[2] = s[e[2]];
        o[6] = s[e[6]];
        s = t[3];
        o[3] = s[e[3]];
        o[7] = s[e[7]];
      }
    }
  }
}

void item_change(void)
{
  uint f;
  card_image_rec *p;
  card_record_rec *q;
  int i;
  int s;

  f = card_flags_get(0);
  p = card_image_get(0);
  q = card_record_get(0);
  if ((f & 1) != 0) {
    if ((f & 0x80) != 0) {
      p->header.level = debug_dan_i;
      q->header.level = debug_dan_i;
      for (i = 0; i < 4; i++) {
        s = (int)&level_change_items.c0_slot0;
        p->body.equip_1p[i] = *(uchar *)(i + s);
        p->body.equip_2p[i] = *(uchar *)(i + s + 4);
        q->body.equip_1p[i] = *(uchar *)(i + s);
        q->body.equip_2p[i] = *(uchar *)(i + s + 4);
      }
      champion_side = 0;
      card_mode_start_result(0);
      task_tree_stop();
    }
  }
}

void performance_update(void)
{
  int start;
  int i;

  start = stopwatch_usec();
  for (i = 0; i < 1000; i++) {
    switch (type_0c322d5c) {
    case 1:
      asin_ang16((float)i * 0.001);
      break;
    case 2:
      asinf((float)i * 0.001);
      break;
    }
  }
  perf_test_time = stopwatch_usec() - start;
}

void debug_console_clear(uint *sb)
{
  sb[2] = *sb;
  sb[3] = *sb;
  return;
}

int console_vprintf(int sb, const char *format, va_list ap)
{
  int n;
  int write;

  write = *(int *)(sb + 8);
  n = strbuf_vprintf((void *)sb, format, ap);
  if ((uint)(write + n) >= *(uint *)(sb + 4)) {
    *(int *)(sb + 8) = write;
    strbuf_drop_line((strbuf_rec *)sb, n + 1);
    strbuf_vprintf((void *)sb, format, ap);
  }
}

void console_printf(int sb, const char *format, ...)
{
  va_list ap;
  __builtin_stdarg_start(ap, format);
  console_vprintf(sb, format, ap);
  __builtin_va_end(ap);
}

int console_tail_start(int text, int len, int lines, int cols)
{
  int w = cols;
  char *s = (char *)text;
  int n = lines;
  int i;
  char *p;
  int t;

  if (len == 0) return (int)s;
  if (lines < 1) return (int)s;
  i = len - 1;
  p = s + i;
  t = *p == '\n';
  p += t == 0;
  for (; i != 0; i--) {
    if (s[i - 1] == '\n') {
      if (wraparound != 0) {
        n -= (p - (s + i) + w - 1) / w;
        if (n < 1) {
          for (; n < 0; n++) i += w;
          break;
        }
      }
      else {
        if (--n == 0) break;
      }
      p = s + i - 1;
    }
  }
  return (int)(s + i);
}

void * console_line_end(void *line, size_t len, size_t cols)
{
  void *end;
  size_t n;
  n = len;
  if (wraparound != 0) {
    n = cols;
    if ((int)cols > (int)len) n = len;
  }
  end = memchr(line,10,n);
  if (end == (void *)0x0) {
    end = (void *)((int)line + n);
  }
  return end;
}

void console_text_draw(win_rec *w, int text, int len)
{
  int cols;
  char *s;
  char *e;
  char *end;
  int n;
  int i;
  int r;

  cols = win_text_cols(w);
  end = (char *)(text + len);
  n = w->height - w->count;
  if (n > 0) {
    s = (char *)console_tail_start(text,len,n,cols);
    for (i = w->count; i < w->height; i++) {
      r = end - s;
      if (r <= 0) return;
      e = (char *)console_line_end(s,r,cols);
      win_text_put(w,0,i,s,e - s);
      s = e;
      s += (*e == '\n') & (e < end);
    }
  }
}

void console_draw(win_rec *w, int *sb)
{
  console_text_draw(w,*sb,sb[2] - *sb);
  return;
}

void empty_fn_0c0ef640(void)
{
  return;
}

void sscanf_test(void)
{
  sscanf("12345","%d",&sscanf_test_value);
  return;
}

void motion_test_open(void)
{
  typedef struct { int w[3]; } v3;
  motion_test_rec *t;
  rob_rec *rob0;
  rob_rec *rob1;

  t = &motion_test;
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  *(v3 *)&t->side[0].saved_x = *(v3 *)&t->side[0].start_x = *(v3 *)&rob0->place_pos;
  t->side[0].prev_motion = 0;
  t->side[0].motion = rob0->move_no;
  t->side[0].frame = t->side[0].length = 1.0;
  if ((t->flags & 1) == 0) {
    t->side[1].field_0x0 = rob1->flags;
    t->flags = t->flags | 1;
  }
  rob1->flags = rob1->flags & 0xfffffffe;
  t->play_mode = 0;
  t->frame_step = 1.0;
  t->saved_blend_switch = motion_blend_switch;
  t->saved_deb_sw = *(char *)&deb_sw;
  motion_blend_switch = 0;
  deb_sw = 1;
  rob0->motion_flags = 0;
  rob0->rsmooth_weight = 0.0;
  rob0->blend_weight = 0.0;
  rob0->reach.flags = rob0->reach.flags & 0xf4;
  t->saved_camera_mode = camera.mode;
  camera.mode = FIX;
}

void motion_test_update(void)
{
  motion_test_rec *t = &motion_test;
  motion_test_side *side = &t->side[0];
  rob_rec *rob;

  rob = rob_get(0);
  if (t->side[0].motion != 0) {
    motion_test_step(rob);
    rob_skeleton_update(rob);
    face_rigid_pick(rob);
    rob_hands_step(rob);
  }
  else {
    side->frame = 1.0;
    side->length = 0;
  }
  robs_coli_prepare();
  rob_coli_spheres_update(rob);
}

void motion_test_draw(void)
{
  typedef struct { float x; float y; float z; } V;
  typedef struct { char pad[12]; V v; } ent;
  char *s;
  ent *e;
  V a;
  V b;
  V *pa;
  V *pb;
  float *ya;
  float *yb;
  short i;
  rob_rec *rob;

  s = (char *)&motion_test;
  e = (ent *)(s + 0xc);
  pa = &a;
  pb = &b;
  a = b = *(V *)(s + 0x18);
  mtx_push_identity();
  mtx_set_current_wordwise(camera_view_mtx);
  a.y = b.y = stage_cur->center.y + 0.005;
  a.z = *(float *)(s + 0x20) + 3.0;
  b.z = *(float *)(s + 0x20) - 3.0;
  for (i = -3; i < 4; i++) {
    {
      float t = e->v.x;
      pb->x = pa->x = i + t;
    }
    if (i != 0) {
      line3d_width_store(4.0);
      line3d_quads_alpha(&pa->x,&pb->x,0xff2040f0);
    }
    else {
      line3d_width_store(6.0);
      line3d_quads_alpha(&pa->x,&pb->x,0xffe0e0e0);
    }
  }
  pa->x = e->v.x + 3.0;
  pb->x = e->v.x - 3.0;
  for (i = -3; i < 4; i++) {
    {
      float *za = &pa->z;
      float *zb = &pb->z;
      float t = e->v.z;
      *zb = *za = i + t;
    }
    if (i != 0) {
      line3d_width_store(4.0);
      line3d_quads_alpha(&pa->x,&pb->x,0xff2040f0);
    }
    else {
      line3d_width_store(6.0);
      line3d_quads_alpha(&pa->x,&pb->x,0xffe0e0e0);
    }
  }
  rob = rob_get(0);
  *pa = *pb = *(V *)&rob->pos;
  ya = &pa->y;
  yb = &pb->y;
  *ya = *yb = stage_cur->center.y + 0.01;
  pa->x += 0.2;
  pb->x -= 0.2;
  line3d_width_store(10.0);
  line3d_quads_alpha(&pa->x,&pb->x,0xffff2020);
  *pa = *pb = *(V *)&rob->pos;
  *ya = *yb = stage_cur->center.y + 0.01;
  pa->z += 0.2;
  pb->z -= 0.2;
  line3d_quads_alpha(&pa->x,&pb->x,0xffff2020);
  mtx_pop(1);
}

void motion_test_close(void)
{
  typedef struct { uint w[3]; } v3;
  motion_test_rec *t = &motion_test;
  rob_rec *rob0;
  rob_rec *rob1;

  rob0 = rob_get(0);
  *(v3 *)&rob0->place_pos = *(v3 *)&t->side[0].saved_x;
  rob0->state = rob0->state | 0x10;
  rob1 = rob_get(1);
  rob1->flags = t->side[1].field_0x0;
  motion_blend_switch = t->saved_blend_switch;
  deb_sw = (int)(char)t->saved_deb_sw;
  camera.mode = t->saved_camera_mode;
  t->flags = t->flags & 0xfffffffe;
}

void motion_test_step(rob_rec *rob)
{
  char *s = (char *)&motion_test;
  char *side = s + 12;
  float d;
  float e;
  
  switch ((uchar)s[6]) {
  case 0:
    if ((player_input_get(0)->pressed & 1) || *(short *)(side + 10) != *(short *)(side + 8)) {
      motion_test_restart(rob);
    }
    else if (*(float *)(side + 40) < *(float *)(side + 36)) {
      *(float *)(side + 40) = *(float *)(s + 8) + *(float *)(side + 40);
      rob->side_flags &= ~2;
    }
    break;
  case 1:
    if ((player_input_get(0)->pressed & 1) || *(short *)(side + 10) != *(short *)(side + 8)) {
      motion_test_restart(rob);
    }
    else {
      if (*(float *)(side + 40) < *(float *)(side + 36)) {
        *(float *)(side + 40) = *(float *)(s + 8) + *(float *)(side + 40);
      }
      else {
        *(float *)(side + 40) = 1.0;
      }
      rob->side_flags &= ~2;
    }
    break;
  case 2:
    if (*(short *)(side + 10) != *(short *)(side + 8)) {
      motion_test_restart(rob);
    }
    else {
      if (player_input_get(0)->now & 1) {
        if (player_input_get(0)->now & 0x10) {
          e = *(float *)(s + 8);
        }
        else if (player_input_get(0)->now & 0x20) {
          e = -*(float *)(s + 8);
        }
        else {
          e = 0.0;
        }
        *(float *)(side + 40) = *(float *)(side + 40) + e;
      }
    }
    break;
  case 3:
    if (*(short *)(side + 10) != *(short *)(side + 8)) {
      motion_test_restart(rob);
    }
    else {
      if (player_input_get(0)->now & 1) {
        if (player_input_get(0)->pressed & 0x10) {
          e = *(float *)(s + 8);
        }
        else if (player_input_get(0)->pressed & 0x20) {
          e = -*(float *)(s + 8);
        }
        else {
          e = 0.0;
        }
        *(float *)(side + 40) = *(float *)(side + 40) + e;
      }
    }
    break;
  case 4:
    *(float *)(s + 8) = 1.0;
    if (*(short *)(side + 10) != *(short *)(side + 8)) {
      motion_test_restart(rob);
    }
    if (*(int *)s & 2) {
      if (player_input_get(1)->now & 1) {
        d = 0.0;
        if (player_input_get(1)->pressed & 0x10) {
          d = *(float *)(s + 8);
        }
        else if (player_input_get(1)->pressed & 0x20) {
          d = -*(float *)(s + 8);
        }
        *(float *)(side + 40) = *(float *)(side + 40) + d;
      }
    }
    else {
      if (*(float *)(side + 40) < *(float *)(side + 36)) {
        *(float *)(side + 40) = *(float *)(s + 8) + *(float *)(side + 40);
      }
      else {
        *(float *)(side + 40) = 1.0;
      }
      rob->side_flags &= ~2;
    }
    break;
  }
  if (*(float *)(side + 40) < 1.0) {
    *(float *)(side + 40) = 1.0;
  }
  else if (*(float *)(side + 40) > *(float *)(side + 36)) {
    *(float *)(side + 40) = *(float *)(side + 36);
  }
  rob->frame_pos = *(float *)(side + 40);
  rob->frame = (int)*(float *)(side + 40);
  attack_active_flag(rob);
  if (player_input_get(0)->pressed & 1) {
    *(int *)s &= ~2;
  }
  else if (s[6] == 4 && (rob->state & 0x4000)) {
    *(int *)s |= 2;
  }
  if (rob->timed_list != 0) {
    move_timed_run(rob);
  }
  rob_root_offset_apply(rob);
}

void motion_test_restart(rob_rec *rob)
{
  motion_test_side *e;
  uint i;
  uint ofs;
  ushort *move_no;
  int *src;
  int *dst;
  int *dst2;
  int x;
  int y;
  int z;

  i = rob->side;
  ofs = i * sizeof(motion_test_side);
  e = &motion_test.side[0];
  e = (motion_test_side *)((char *)e + ofs); /* ORIGINAL SHAPE (assumed; the 2026-11-10 audit narrowed it: the offset is i * sizeof(motion_test_side), not a bare 0x2c): the side reached by a byte offset from side[0]; &motion_test.side[i] compiles differently */
  e->motion &= 0x7fff;
  e->motion = mot_id_skip_unloaded(e->motion,e->prev_motion);
  move_no = &rob->move_no;
  *move_no = e->motion;
  rob->frame_speed = e->frame = 1.0;
  rob->frame_pos = 1.0;
  rob->frame = 1;
  rob_move_set(rob,*move_no);
  e->length = (float)rob->move_length;
  dst2 = (int *)&rob->place_pos;
  dst = (int *)&rob->pos;
  src = &e->start_x;
  x = src[0];
  dst[0] = x;
  y = src[1];
  dst[1] = y;
  z = src[2];
  dst[2] = z;
  dst2[0] = x;
  dst2[1] = y;
  dst2[2] = z;
  e->prev_motion = e->motion;
}

void attack_active_flag(rob_rec *rob)
{
  if ((rob->move_flags & 0x4000) != 0) {
    if ((float)rob->active_start <= rob->frame_pos &&
        rob->frame_pos < (float)rob->active_end) {
      rob->state |= 0x4000;
    }
    else {
      rob->state &= ~0x4000;
    }
  }
}

void frame_check_open(void)
{
  typedef struct { int w[3]; } vec;
  typedef struct { short motion; short prev_motion; vec start; vec saved; float length; float frame; int pad[2]; } ent;
  typedef struct { uint flags; char saved_blend_switch; char saved_deb_sw; char play_mode; char reaction_row; char reaction_row_run; char hit_case; char hit_case_run; char saved_camera_mode; float frame_step; int pad[2]; ent side[2]; } hdr;
  hdr *h = (hdr *)&frame_check;
  rob_rec *rob0;
  rob_rec *rob1;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  h->side[0].prev_motion = h->side[0].motion = rob0->move_no;
  h->side[1].prev_motion = h->side[1].motion = rob1->move_no;
  h->side[0].saved = h->side[0].start = *(vec *)&rob0->place_pos;
  h->side[1].saved = h->side[1].start = *(vec *)&rob1->place_pos;
  h->side[0].frame = h->side[0].length = 1.0;
  h->side[1].frame = h->side[1].length = 1.0;
  if ((h->flags & 1) == 0) {
    h->flags |= 1;
  }
  h->play_mode = 0;
  h->reaction_row = 0;
  h->reaction_row_run = 0;
  h->hit_case = 0;
  h->hit_case_run = 0;
  h->frame_step = 1.0;
  h->saved_blend_switch = motion_blend_switch;
  h->saved_deb_sw = deb_sw;
  motion_blend_switch = 0;
  deb_sw = 1;
  h->saved_camera_mode = camera.mode;
  camera.mode = MODE_ROB;
}

void frame_check_update(void)
{
  char *g = (char *)&frame_check;
  char *atk_side = g + 16;
  char *def_side = g + 60;
  rob_rec *attacker;
  rob_rec *defender;
  char *guard_moves;
  uint ofs;
  
  attacker = rob_get(0);
  defender = rob_get(1);
  if (*(short *)(g + 24) == 0) {
    *(float *)(g + 56) = 1.0f;
    *(float *)(g + 100) = 1.0f;
    *(float *)(g + 52) = 0.0f;
    *(float *)(g + 96) = 0.0f;
    return;
  }
  if (player_input_get(1)->pressed & 1) {
    g[9] = 0;
    g[7] = 0;
  }
  if ((player_input_get(0)->pressed & 1) == 0 && *(short *)(atk_side +8) == *(short *)(atk_side +10) &&
      (uchar)g[7] == (uchar)g[8] && (uchar)g[9] == (uchar)g[10]) {
    switch ((uchar)g[6]) {
    case 0:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      break;
    case 1:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      else if (defender->frame > defender->move_length) {
        *(float *)(atk_side +40) = 1.0f;
        attacker->side_flags &= 0xfffd;
        frame_advantage(attacker, defender, atk_side, def_side);
      }
      break;
    case 2:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      else if (defender->frame > defender->move_length && (uchar)g[9] <= 2) {
        g[9]++;
        frame_advantage(attacker, defender, atk_side, def_side);
      }
      break;
    case 3:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      else if (defender->frame > defender->move_length) {
        if ((uchar)g[9] <= 2) {
          g[9]++;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
        else {
          g[9] = 0;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
      }
      break;
    case 4:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      else if (defender->frame > defender->move_length) {
        if ((uchar)g[9] <= 2) {
          g[9]++;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
        else if ((uchar)g[7] <= 18) {
          g[9] = 0;
          g[7]++;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
      }
      break;
    case 5:
      if (*(float *)(atk_side +40) < *(float *)(atk_side +36)) {
        *(float *)(atk_side +40) = *(float *)(g + 12) + *(float *)(atk_side +40);
        attacker->side_flags &= 0xfffd;
      }
      else if (defender->frame > defender->move_length) {
        if ((uchar)g[9] <= 2) {
          g[9]++;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
        else if ((uchar)g[7] <= 18) {
          g[9] = 0;
          g[7]++;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
        else {
          g[9] = 0;
          g[7] = 0;
          frame_advantage(attacker, defender, atk_side, def_side);
        }
      }
      break;
    }
  }
  else {
    frame_advantage(attacker, defender, atk_side, def_side);
  }
  if (*(float *)(atk_side +40) < 1.0f) {
    *(float *)(atk_side +40) = 1.0f;
  }
  else if (*(float *)(atk_side +40) > *(float *)(atk_side +36)) {
    *(float *)(atk_side +40) = *(float *)(atk_side +36);
  }
  attacker->frame_pos = *(float *)(atk_side +40);
  attacker->frame = (int)*(float *)(atk_side +40);
  *(float *)(def_side +40) = *(float *)(def_side +40) + *(float *)(g + 12);
  if (*(float *)(def_side +40) < 1.0f) {
    *(float *)(def_side +40) = 1.0f;
  }
  defender->frame_pos = *(float *)(def_side +40);
  defender->frame = (int)*(float *)(def_side +40);
  if ((attacker->move_flags & 0x4000) != 0 && attacker->frame >= attacker->active_start &&
      (*(uint *)g & 4) == 0) {
    guard_moves = guard_motion_tables[attacker->field_0x328 & 0xff];
    guard_moves += (uchar)g[7] * 16;
    ofs = (uchar)g[9] * 4;
    *(float *)(def_side +40) = 1.0f;
    defender->frame_pos = 1.0f;
    defender->frame = 1;
    rob_move_set(defender,*(ushort *)(guard_moves + ofs));
    *(float *)(def_side +36) = (float)defender->move_length;
    *(uint *)g |= 4;
  }
  else if ((*(uint *)g & 4) == 0 && (((uchar)g[7] == 17) | ((uchar)g[7] == 19))) {
    int o = 0x584;
    rob_old_rec *old;
    defender->velocity.y = 0.0f;
    old = *(rob_old_rec **)((char *)defender + o);
    defender->place_pos.y = old->place_pos.y;
    defender->pos.y = old->pos.y;
  }
  rob_skeleton_update(attacker);
  face_rigid_pick(attacker);
  rob_hands_step(attacker);
  rob_skeleton_update(defender);
  face_rigid_pick(defender);
  rob_hands_step(defender);
}

void frame_check_close(void)
{
  typedef struct { int w[3]; } v3;
  char *g = (char *)&frame_check;
  rob_rec *rob0;
  rob_rec *rob1;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  *(v3 *)&rob0->place_pos = *(v3 *)(g + 0x28);
  rob0->state = rob0->state | 0x10;
  *(v3 *)&rob1->place_pos = *(v3 *)(g + 0x54);
  rob1->state = rob1->state | 0x10;
  motion_blend_switch = g[4];
  deb_sw = g[5];
  camera.mode = g[0xb];
  *(uint *)g = *(uint *)g & 0xfffffffe;
}

void frame_advantage(rob_rec *attacker, rob_rec *defender, char *atk_side, char *def_side)
{
  byte mirror;
  ushort *motion;
  char *g = (char *)&frame_check;
  
  g[8] = g[7];
  g[10] = g[9];
  *(uint *)g = *(uint *)g & 0xfffffffb;
  motion = (ushort *)(atk_side + 8);
  *motion &= 0x7fff;
  *motion = attack_move_find(*motion,*(short *)(atk_side + 10));
  attacker->move_no = *motion;
  attacker->side_flags = attacker->side_flags & 0xffef;
  defender->side_flags = defender->side_flags & 0xffef;
  rob_pos_reset();
  attacker->frame_speed = *(float *)(atk_side + 0x28) = 1.0f;
  attacker->frame_pos = 1.0f;
  attacker->frame = 1;
  rob_move_set(attacker,attacker->move_no);
  *(float *)(atk_side + 0x24) = (float)attacker->move_length;
  *(ushort *)(atk_side + 10) = *motion;
  defender->frame_speed = *(float *)(def_side + 0x28) = 1.0f;
  defender->frame_pos = 1.0f;
  defender->frame = 1;
  if (1 < (byte)frame_check.hit_case) {
    defender->heading = defender->heading + -0x8000;
  }
  rob_move_set(defender,defender->move_table[(char)frame_check_react_moves[(byte)g[7]]]);
  *(float *)(def_side + 0x24) = (float)defender->move_length;
  {
    int saved_case;
    int start;
    short damage;
    short d;
    short *pending;
    saved_case = attacker->coli->field_0x3c[0];
    attacker->frame = start = attacker->active_start;
    damage = attacker->attack_damage;
    defender->pending_damage = attacker->attack_damage;
    if (attacker->react_recovery[0] != -1) {
      d = attacker->react_recovery[0] - start;
    }
    else {
      d = attacker->recovery_end - attacker->active_start;
    }
    attacker->frame_adv_guard = guard_stun(attacker,defender,0) - d;
    if (attacker->react_recovery[1] != -1) {
      d = attacker->react_recovery[1] - attacker->active_start;
    }
    else {
      d = attacker->recovery_end - attacker->active_start;
    }
    attacker->coli->field_0x3c[0] = 1;
    attacker->frame_adv_hit = hit_stun(attacker,defender) - d;
    if (attacker->react_recovery[2] != -1) {
      d = attacker->react_recovery[2] - attacker->active_start;
    }
    else {
      d = attacker->recovery_end - attacker->active_start;
    }
    attacker->coli->field_0x3c[0] = 2;
    pending = &defender->pending_damage;
    *pending = *pending + (attacker->attack_damage >> 1);
    attacker->frame_adv_hit_2 = hit_stun(attacker,defender) - d;
    attacker->frame = 1;
    attacker->coli->field_0x3c[0] = saved_case;
    *pending = damage;
  }
  mirror = g[9] & 1;
  if ((attacker->field_0x328 & 0x100) != 0) {
    mirror = mirror ^ 1;
  }
  if (1 < (byte)g[9]) {
    mirror = mirror ^ 1;
  }
  if (mirror != 0) {
    attacker->side_flags = attacker->side_flags | 0x10;
  }
}

uint move_flags_get(short motion)
{
  if (motion == 0) return 0;
  return *(uint *)move_record((short)(motion - 1));
}

int attack_move_find(short motion, short prev)
{
  short r;

  if (motion == prev) return prev;
  r = mot_id_skip_unloaded(motion, prev);
  if (r > 0x167b) return prev;
  if (r == 0) return prev;
  if (r == prev) return prev;
  if (move_flags_get(r) & 0x4000) return r;
  if (motion > prev) motion = motion + 1;
  else motion = motion - 1;
  return attack_move_find(motion, prev);
}
