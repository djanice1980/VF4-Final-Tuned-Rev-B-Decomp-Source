/*
 * vf4lib/libbase/src/light.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float hit_light_colors[13][3]; /* 0x0c167fa0 */
extern float hit_light_gain; /* 0x0c293d90 */
extern float hit_light_level; /* 0x0c293d94 */
extern float hit_light_decay; /* 0x0c293d98 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern short cam_anim_kind; /* 0x0c370c94 */
extern short light_set_current; /* 0x0c37245e */
extern elan_light_rec light_send_buf[14]; /* 0x0c372460 */
extern short hit_light_timer; /* 0x0c3728a6 */
extern uint hit_light_flags; /* 0x0c3728a8 */
extern camera_state camera; /* 0x0c370ce0 */
extern adv_light_color adv_light_target; /* 0x0c371010 */
extern elan_light_rec extra_light; /* 0x0c372818 */
extern hit_light_pos_rec hit_light_pos; /* 0x0c37422c */
extern elan_light_masks_rec light_masks; /* 0x0c374238 */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void light_sets_init(void)
{
  int i;
  int j;
  float level;
  light_table_fill_defaults();
  lighting_path_select(1);
  for (i = 0; i < 0x10; i++) {
    for (j = 0; j < 6; j++) {
      light_entry_default(&light_sets[j][i]);
    }
  }
  for (i = 0; i < 6; i++) {
    if (i == 5) {
      level = 0.0;
      light_sets[5][0].type = 1;
    }
    else {
      level = 0.5;
      light_sets[i][0].type = 2;
    }
    light_set_ambient[i].color[0] = level;
    light_set_ambient[i].color[1] = level;
    light_set_ambient[i].color[2] = level;
  }
  light_set_current = 0xffff;
  hit_light_color = 0;
}

void light_entry_default(light_src_rec *light)
{
  light->type = 0;
  light->angle_min = 0x2000;
  light->angle_max = 0x4000;
  light->pos[0] = 0.0;
  light->pos[1] = 2.0;
  light->pos[2] = 0.0;
  light->dir[0] = 1.0;
  light->dir[1] = 0.0;
  light->dir[2] = 0.0;
  light->color[0] = 1.0;
  light->color[1] = 1.0;
  light->color[2] = 1.0;
  light->field_0x20[0] = 1.0;
  light->field_0x20[1] = 0.0;
  light->field_0x20[2] = 0.0;
  light->dist_min = 0.0001;
  light->dist_max = 10.0;
}

void adv_light_blend_step(void)
{
  float k;
  char i;
  char j;

  if (mode_get_main() != MODE_MAIN_ADVERTISE) {
    return;
  }
  if ((adv_light_blend_mode & 4) != 0) {
    stage_lights_load();
    adv_light_blend_mode = 0;
  }
  else if ((adv_light_blend_mode & 2) != 0) {
    k = 1.0 / (float)adv_light_blend_frames;
    for (i = 0; i < 3; i++) {
      light_set_ambient[0].color[i] +=
           ((&adv_light_target.r)[i] - light_set_ambient[0].color[i]) * k;
    }
    for (i = 0; i < 16; i++) {
      light_src_rec *e = &light_sets[0][i];
      for (j = 0; j < 3; j++) {
        e->color[j] += ((&adv_light_target.r)[j] * 2.0 - e->color[j]) * k;
      }
    }
    for (i = 0; i < 3; i++) {
      light_set_ambient[1].color[i] +=
           ((&adv_light_target.r)[i] - light_set_ambient[1].color[i]) * k;
    }
    for (i = 0; i < 16; i++) {
      light_src_rec *e = &light_sets[1][i];
      for (j = 0; j < 3; j++) {
        e->color[j] += ((&adv_light_target.r)[j] * 2.0 - e->color[j]) * k;
      }
    }
    adv_light_blend_frames--;
  }
  else if ((adv_light_blend_mode & 1) != 0) {
    adv_light_blend_frames--;
  }
  if (adv_light_blend_frames <= 0) {
    adv_light_blend_mode = 0;
  }
}

void light_set_masks_send(float *ambient, int set)
{
  int id = (short)set;
  elan_light_masks_rec *cmd = &light_masks;
  uint bits = 0;
  light_src_rec *lights = light_sets[(short)id];
  int i;
  float dim;
  for (i = 0; i < 16; i++) {
    if (lights[i].type > 1) bits |= 1 << i;
  }
  if (ambient != 0) {
    light_set_ambient[id].color[0] = ambient[0];
    light_set_ambient[id].color[1] = ambient[1];
    light_set_ambient[id].color[2] = ambient[2];
  }
  cmd->flags = bitflags_0c293dac;
  cmd->field_0x4 = 0;
  cmd->field_0x5 = 0;
  cmd->masks[0] = cmd->masks[1] = cmd->masks[2] = cmd->masks[3] = (short)bits;
  cmd->ambient0[0] = cmd->ambient1[0] = 1.0;
  if ((id == 1) | (id == 4)) {
    dim = 1.0 - hit_light_level;
    cmd->ambient0[1] = cmd->ambient1[1] = light_set_ambient[id].color[0] * dim;
    cmd->ambient0[2] = cmd->ambient1[2] = light_set_ambient[id].color[1] * dim;
    cmd->ambient0[3] = cmd->ambient1[3] = dim * light_set_ambient[id].color[2];
  }
  else {
    cmd->ambient0[1] = cmd->ambient1[1] = light_set_ambient[id].color[0];
    cmd->ambient0[2] = cmd->ambient1[2] = light_set_ambient[id].color[1];
    cmd->ambient0[3] = cmd->ambient1[3] = light_set_ambient[id].color[2];
  }
  elan_light_masks_queue(cmd);
}

int light_set_send(float *ambient, short set)
{
  light_src_rec *light;
  int i;
  int count;
  light_src_rec *lights = light_sets[set];

  light_set_masks_send(ambient,set);
  mtx_push(0);
  camera_view_load();
  count = 0;
  for (i = 0; i < 0x10; i++) {
    light = &lights[i];
    light_entry_build(i,light);
    if (1 < light->type) {
      count = i + 1;
    }
  }
  elan_lights_queue(count,light_send_buf);
  mtx_pop(1);
  light_set_current = set;
  return (short)light_set_current;
}

void light_entry_build(int id, light_src_rec *src)
{
  elan_light_rec *e;
  elan_light_rec *q;
  float pos[3];
  float dir[3];
  float d[3];
  float d2[3];
  float *n;
  float *m;
  int *p;
  e = &light_send_buf[id];
  e->id = id;
  e->field_0x4 = 0.0f;
  e->color[0] = src->color[0];
  e->color[1] = src->color[1];
  e->color[2] = src->color[2];
  e->diffuse = diffuse;
  e->specular = specular;
  e->routing = routing;
  e->spec_fade = spec_fade;
  e->field_0x30 = 0;
  mtx_xform_point(src->pos, pos);
  mtx_xform_dir(src->dir, dir);
  switch (src->type) {
  case 2:
    n = d;
    light_send_buf[id].type = 2;
    n[0] = dir[0];
    n[1] = dir[1];
    n[2] = dir[2];
    vec3_normalize_nolen(n);
    light_send_buf[id].dir[0] = n[0];
    light_send_buf[id].dir[1] = n[1];
    light_send_buf[id].dir[2] = n[2];
    break;
  case 3:
    light_send_buf[id].type = 3;
    light_send_buf[id].pos[0] = pos[0];
    light_send_buf[id].pos[1] = pos[1];
    light_send_buf[id].pos[2] = pos[2];
    light_send_buf[id].dist_min = src->dist_min;
    light_send_buf[id].dist_max = src->dist_max;
    break;
  case 4:
    m = d2;
    q = &light_send_buf[id];
    q->type = 4;
    q->pos[0] = pos[0];
    q->pos[1] = pos[1];
    q->pos[2] = pos[2];
    m[0] = dir[0];
    m[1] = dir[1];
    m[2] = dir[2];
    vec3_normalize_nolen(m);
    q->dir[0] = m[0];
    q->dir[1] = m[1];
    q->dir[2] = m[2];
    q->dist_min = src->dist_min;
    q->dist_max = src->dist_max;
    q->angle_min = src->angle_min;
    p = &q->angle_max;
    *p = src->angle_max;
    break;
  }
}

void extra_light_parallel_set(float intensity)
{
  elan_light_rec *p = &extra_light;
  short mask = 0x4000;
  light_masks.masks[0] |= mask;
  light_masks.masks[1] |= mask;
  light_masks.masks[2] |= mask;
  light_masks.masks[3] |= mask;
  p->id = 14;
  p->field_0x4 = p->color[0] = p->color[1] = p->color[2] = intensity;
  p->type = 2;
  p->dir[0] = 1.0;
  p->dir[1] = p->dir[2] = 0.0;
  p->diffuse = 3;
  p->specular = 3;
  p->routing = 12;
  p->spec_fade = 0;
  elan_light_queue(p);
  elan_light_masks_queue(&light_masks);
}

void extra_light_spot_set(float intensity)
{
  float v0[3];
  float v1[3];
  float a[3];
  float b[3];
  elan_light_rec *p;
  float dist;
  float dist1;
  float g;
  int ang;
  int angle;
  short mask;

  p = &extra_light;
  mask = 0x4000;
  light_masks.masks[0] |= mask;
  light_masks.masks[1] |= mask;
  light_masks.masks[2] |= mask;
  light_masks.masks[3] |= mask;
  p->id = 14;
  p->field_0x4 = p->color[0] = p->color[1] = p->color[2] = intensity;
  p->type = 4;
  if (camera.mode == SOFTIMAGE && cam_anim_kind == 1 &&
      (stage_cur_no == 16 || stage_cur_no == 2)) {
    float *c = &camera.eye.x;
    float *q0 = v0;
    float *q1 = v1;
    float *pos0 = &(rob_get(0))->pos.x;
    float *pos1 = &(rob_get(1))->pos.x;
    vec3_sub(q0, pos0, c);
    vec3_sub(q1, pos1, c);
    dist = vec3_length(q0);
    dist1 = vec3_length(q1);
    if (dist > dist1) {
      dist = dist1;
    }
    dist = extra_light_spot_dist + (dist - extra_light_spot_dist) * 0.3f;
    extra_light_spot_dist = dist;
    vec3_normalize(q0);
    vec3_normalize(q1);
    ang = acos_ang16(vec3_dot(q0, q1));
    p->dir[0] = p->dir[1] = 0.0f;
    p->dir[2] = -1.0f;
    g = 2.5f;
    { float *q = p->pos; q[1] = q[0] = 0.0f; q[2] = g; }
    p->dist_min = dist + g;
    p->dist_max = p->dist_min + 0.25f;
    angle = (short)ang;
    if (angle < 0x600) {
      angle = 0x600;
    }
    p->angle_min = angle;
    extra_light_angle_min = angle;
    extra_light_angle_max = p->angle_max = (float)angle * 1.3f;
  }
  else {
    mtx_push(0);
    camera_view_load();
    mtx_xform_point(&camera.eye.x, b);
    mtx_xform_point(&camera.target.x, a);
    vec3_sub(a, a, b);
    vec3_normalize(a);
    mtx_pop(1);
    p->dir[0] = a[0];
    p->dir[1] = a[1] + pos_yofs;
    p->dir[2] = a[2];
    p->pos[0] = b[0];
    p->pos[1] = b[1] + pos_yofs;
    p->pos[2] = b[2] + pos_zofs;
    p->dist_min = dist_min;
    p->dist_max = dist_max;
    p->angle_min = angl_min;
    p->angle_max = angl_max;
  }
  p->diffuse = 3;
  p->specular = 3;
  p->routing = 12;
  p->spec_fade = 0;
  p->field_0x30 = 0;
  elan_light_queue(p);
  elan_light_masks_queue(&light_masks);
}

void extra_light_off(void)
{
  short mask = 0xbfff;
  light_masks.masks[0] &= mask;
  light_masks.masks[1] &= mask;
  light_masks.masks[2] &= mask;
  light_masks.masks[3] &= mask;
  elan_light_masks_queue(&light_masks);
}

void hit_lights_frame(void)
{
  hit_light_update();
  rob_fl_lights_update();
  return;
}

void hit_light_update(void)
{
  light_src_rec *light = &light_sets[1][13];
  rob_rec *rob;
  float power;
  float dx;
  float dz;
  float dist;
  float v;
  int ry;
  int rx;
  float *t;
  uchar i;
  float a[3];
  float b[3];

  if ((hit_light_flags & 0xaf) != 0) {
    hit_light_timer = 90;
    light->pos[0] = hit_light_pos.x;
    light->pos[1] = hit_light_pos.y;
    light->pos[2] = hit_light_pos.z;
    light->dist_min = 0.0001;
    light->dist_max = 1.2;
    light->type = 3;
    power = ((float)hit_light_damage / 30.0 * 0.3 + 0.7) * hit_light_gain;
    if (power > 1.0) {
      power = 1.0;
    }
    if (hit_light_flags & 0x80) {
      int k = hit_light_color;
      if ((uchar)k < 13) {
        light->color[0] = (t = hit_light_colors[(char)k])[0];
        light->color[1] = t[1];
        light->color[2] = t[2];
      }
    }
    else if (hit_light_flags & 8) {
      light->color[0] = power + power;
      light->color[1] = power * 0.9;
      light->color[2] = power * 0.9;
    }
    else if (hit_light_flags & 0x20) {
      light->color[0] = power + power;
      light->color[1] = power * 1.2;
      light->color[2] = power * 0.5;
    }
    else if (hit_light_flags & 4) {
      light->color[0] = power + power;
      light->color[1] = power + power;
      light->color[2] = power;
    }
    else if (hit_light_flags & 2) {
      light->color[0] = power * 1.2;
      light->color[1] = power * 1.2;
      light->color[2] = power * 1.5;
    }
    light_sets[4][13] = *light;
    if (hit_light_flags & 0x20) {
      hit_light_level = power;
    }
    hit_light_flags = hit_light_flags & 0xffffff50;
    hit_light_color = 0;
  }
  else {
    hit_light_timer = hit_light_timer - 1;
  }
  if (hit_light_timer < 0) {
    hit_light_reset();
  }
  else {
    for (i = 1; i < 4; i++) {
      v = light->color[i - 1] - hit_light_decay;
      if (v < 0.0) {
        v = 0.0;
      }
      light->color[i - 1] = v;
    }
    if (hit_light_flags & 0x40) {
      rob = rob_get(1);
    }
    else {
      rob = rob_get(0);
    }
    a[0] = rob->pos.x;
    a[1] = 0.0;
    a[2] = rob->pos.z;
    b[0] = hit_light_pos.x;
    b[1] = 0.0;
    b[2] = hit_light_pos.z;
    dist = vec3_distance(a,b);
    if (dist > 0.5) {
      dist = 0.5;
    }
    a[1] = rob->pos.y;
    dx = camera.eye.x - a[0];
    dz = camera.eye.z - a[2];
    ry = atan2_s16(dx,dz);
    dx = a[0] - camera.eye.x;
    dz = a[2] - camera.eye.z;
    rx = atan2_s16(a[1] - camera.eye.y,fsqrt_asm(dx * dx + dz * dz));
    a[1] = 0.0;
    mtx_push_identity();
    mtx_translate_by_vec3(a);
    mtx_rot_y_s16(ry);
    mtx_rot_x_s16(rx);
    mtx_translate(0.0,0.0,dist);
    mtx_store_translation(light->pos);
    light->pos[1] = hit_light_pos.y;
    mtx_pop(1);
    light_sets[4][13] = *light;
    hit_light_level = hit_light_level - 0.045;
    if (hit_light_level < 0.0) {
      hit_light_level = 0.0;
    }
  }
}

void hit_light_reset(void)
{
  light_src_rec *p = &light_sets[1][13];
  hit_light_flags = hit_light_flags & 0xffffff10;
  hit_light_color = 0;
  hit_light_timer = 0;
  *p = light_off;
  light_sets[4][13] = *p;
  hit_light_level = 0.0;
}

void rob_fl_lights_update(void)
{
  light_src_rec l;
  light_src_rec *e = &l;
  float tv[3] = { 0.65, 0.0, 0.0 };
  float *t = tv;
  uint k;
  int j;
  rob_rec *rob;
  float fl_rate;
  float base;
  float *dir;

  l.angle_min = 0;
  l.angle_max = 0xc00;
  l.field_0x20[0] = 1.0;
  l.field_0x20[1] = 0.0;
  l.field_0x20[2] = 0.0;
  l.color[2] = 0.0;
  l.color[1] = 0.0;
  l.color[0] = 0.0;
  l.field_0x2c = 0.0;
  l.dist_min = 1.0;
  l.dist_max = 1.01;
  mtx_push(0);
  dir = l.dir;
  for (k = 0; (int)k < 2; k++) {
    rob = rob_get(k & 0xff);
    fl_rate = rob->fl_rate;
    base = stage_fl_base;
    if ((rob->flags & 1) != 0 && !(fl_rate < 0.01) && (hit_light_flags & 0x10) != 0) {
      mtx_set_current_wordwise(rob->mtx + 0x30);
      mtx_translate_by_vec3(t);
      mtx_store_translation(l.pos);
      dir[0] = rob->mtx[0x3c];
      dir[1] = rob->mtx[0x3d];
      dir[2] = rob->mtx[0x3e];
      vec3_sub_from(dir,l.pos);
      mtx_translate(0.0,0.0,0.07);
      mtx_store_translation(l.pos);
      e->type = 4;
      for (j = 1; j < 4; j++) e->color[j - 1] = base * fl_rate;
    }
    else {
      e->type = 1;
    }
    if (k == 0) {
      light_sets[1][11] = *e;
      light_sets[4][11] = *e;
    }
    else {
      light_sets[1][12] = *e;
      light_sets[4][12] = *e;
    }
  }
  mtx_pop(1);
}

void light_set_load(uchar set, uchar table_no)
{
  stage_light_table_rec *e = &stage_light_table[table_no];
  light_src_rec **pp = e->lights;
  uchar i;

  light_set_ambient[set].color[0] = e->ambient[0];
  light_set_ambient[set].color[1] = e->ambient[1];
  light_set_ambient[set].color[2] = e->ambient[2];
  for (i = 0; i < 16; i++) {
    if (*pp == 0) {
      light_sets[set][i] = light_off;
    }
    else {
      light_sets[set][i] = **pp++;
    }
  }
}

void stage_lights_load(void)
{
  float *fp;

  light_set_load(0,(uchar)stg_light_num);
  if (rob_light_num >= 0) {
    light_set_load(1,(uchar)rob_light_num);
    light_set_load(4,(uchar)rob_light_num);
  }
  else {
    light_set_load(1,(uchar)stg_light_num);
    light_set_load(4,(uchar)stg_light_num);
  }
  light_set_load(2,(uchar)stg_light_num);
  light_set_load(3,(uchar)stg_light_num);
  hit_light_flags = 0;
  fp = stage_cur->fl_base;
  if (fp != (float *)0x0) {
    stage_fl_base = *fp;
    ((struct { unsigned int : 4; unsigned int f : 1; } *)&hit_light_flags)->f = 1;
  }
  else {
    stage_fl_base = 0.0;
    ((struct { unsigned int : 4; unsigned int f : 1; } *)&hit_light_flags)->f = 0;
  }
  hit_light_timer = 0;
}
