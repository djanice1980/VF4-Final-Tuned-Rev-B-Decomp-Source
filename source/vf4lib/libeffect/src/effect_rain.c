/*
 * vf4lib/libeffect/src/effect_rain.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int rain_extra_model_ids[16]; /* 0x0c1a10b4 */
extern int rain_sphere_splash_model_ids[16]; /* 0x0c1a10f4 */
extern int rain_limb_joint_ids[2]; /* 0x0c1a1134 */
extern int rain_sphere_ids[10]; /* 0x0c1a113c */
extern float rain_fan_uvs[30][4][2]; /* 0x0c1a11f4 */
extern char rain_drip_psys[17504]; /* 0x0c458c94 */
extern char rain_splash_psys[17504]; /* 0x0c45d0f4 */
extern int rain_drip_sets[2][801]; /* 0x0c461554 */
extern int rain_splash_sets[2][4801]; /* 0x0c462e5c */
extern void *rain_spare_models[10]; /* 0x0c489448 */
extern compiled_model_rec *rain_drip_models[5][30]; /* 0x0c489470 */
extern compiled_model_rec *rain_splash_models[2][30]; /* 0x0c4896c8 */
extern camera_state camera; /* 0x0c370ce0 */
extern int rain_anim_models[16]; /* 0x0c1a1074 */
extern float rain_anim_frame; /* 0x0c489444 */
extern int rain_last_word; /* 0x0c489840 */
extern rain_params_rec rain_params; /* 0x0c389cdc */
extern rain_streak rain_streaks[36]; /* 0x0c46c464 */

static inline float star_fsqrt(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void rain_init(void)
{
  rain_sheet_model_clear();
  rain_params_init();
  rain_streaks_init();
  rain_init_tail();
  rain_anim_models_init();
  rain_water_fx_init();
  rain_last_init();
  return;
}

void rain_exec(void)
{
  stopwatch_start();
  rain_part_update_empty();
  rain_streaks_fall();
  rain_anim_advance();
  rain_water_fx_update();
  rain_last_update_empty();
  rain_params.exec_usec = stopwatch_usec();
  return;
}

void rain_draw(void)
{
  mtx_push(0);
  mtx_invert_rigid();
  mtx_store_wordwise(rain_inv_view_mtx);
  mtx_xform_point(rain_sheet0_offset,rain_sheet0_pos);
  mtx_xform_point(rain_sheet1_offset,rain_sheet1_pos);
  mtx_xform_point(rain_sheet2_offset,rain_sheet2_pos);
  mtx_xform_point(rain_sheet3_offset,rain_sheet3_pos);
  mtx_xform_point(rain_sheet4_offset,rain_sheet4_pos);
  mtx_store_translation(rain_eye_pos);
  mtx_pop(1);
  rain_sheet_model_write();
  rain_part_draw_empty();
  rain_sheets_draw();
  rain_anim_draw();
  rain_water_fx_draw();
  rain_last_draw_empty();
  return;
}

void rain_free(void)
{
  rain_part_free_empty();
  rain_water_fx_free();
  return;
}

void rain_streaks_reset(void)
{
  rain_params_init();
  rain_streaks_init();
  return;
}

void rain_streaks_scatter_only(void)
{
  rain_streaks_scatter();
  return;
}

void rain_params_init(void)
{
  int i;
  rain_params_rec *p;

  rain_params.box[0][0] = 1.9f;
  rain_params.box[0][1] = 0.4f;
  rain_params.box[0][2] = 1.3f;
  rain_params.vel[0].x = 0.0726f;
  rain_params.vel[0].y = -1.791f;
  rain_params.vel[0].z = -0.017f;
  rain_params.weight[0] = 0.7f;
  rain_params.box[1][0] = 5.1f;
  rain_params.box[1][1] = 4.9f;
  rain_params.box[1][2] = 5.9f;
  rain_params.vel[1].x = 0.0f;
  rain_params.vel[1].y = -0.12f;
  rain_params.vel[1].z = 0.0f;
  rain_params.weight[1] = 1.4f;
  for (i = 0; i < 3; i++) {
    rain_params.shape[0][i] = rain_streak_shape_init[i];
    rain_params.shape[1][i] = rain_streak_shape_init[i];
  }
  p = &rain_params;
  p->shape[0][0].y = 1.5f;
  p->shape[0][1].x = -0.0068f;
  p->shape[0][2].x = 0.0068f;
  p->shape[1][0].y = 0.91f;
  p->shape[1][1].x = -0.0088f;
  p->shape[1][2].x = 0.0088f;
  p->count[0] = 2;
  p->count[1] = 2;
  p->field_0x94 = 1;
  p->field_0x9c = 0.3f;
  p->field_0xb8 = 1;
  p->field_0xa0.x = 0.0f;
  p->field_0xa0.y = 0.0f;
  p->field_0xa0.z = -3.0f;
}

void rain_streaks_init(void)
{
  rain_streak *streak;
  int i;

  rain_streaks_scatter();
  streak = rain_streaks;
  i = 0;
  do {
    rain_streak_setup(streak);
    streak->pos.y = streak->base.y + 8.0;
    i = i + 1;
    streak = streak + 1;
  } while (i < 0x24);
  return;
}

void rain_streak_respawn(rain_streak *streak)
{
  typedef struct { int w[3]; } blk;
  float lo;
  float hi;
  float *s;
  float *d;
  float *t;

  s = &streak->base.x;
  lo = -0.1;
  hi = 0.1;
  streak->pos.x = rain_lerp(lo,hi,lcg_next_unit()) + *s;
  d = &streak->pos.z;
  t = s + 2;
  *d = rain_lerp(lo,hi,lcg_next_unit()) + *t;
  s = t - 7;
  lo = -3.0;
  hi = 1.0;
  *s = rain_lerp(lo,hi,lcg_next_unit()) + 8.0;
  *(blk *)&streak->vel = *(blk *)&streak->vel0;
}

void rain_streak_setup(rain_streak *streak)
{
  float h[3];
  float up[3];
  float axis[3];
  float *a;
  float *c;
  float *b;
  float *d;
  float *s;
  float q;
  int i;

  up[0] = 0.0f;
  up[1] = -1.0f;
  up[2] = 0.0f;
  {
    float *c0 = &streak->base.x;
    streak->pos.x = rain_lerp(-0.1f, 0.1f, lcg_next_unit()) + *c0;
  }
  {
    float *d0 = &streak->pos.z;
    float *c1 = &streak->base.z;
    *d0 = rain_lerp(-0.1f, 0.1f, lcg_next_unit()) + *c1;
  }
  c = &streak->pos.y;
  *c = rain_lerp(-3.0f, 1.0f, lcg_next_unit()) + 8.0f;
  if (streak->idx > 20) {
    streak->vel.x = rain_params.vel[1].x;
    streak->vel.y = rain_params.vel[1].y;
    streak->vel.z = rain_params.vel[1].z;
    streak->weight = rain_params.weight[1];
  }
  else {
    streak->vel.x = rain_params.vel[0].x;
    streak->vel.y = rain_params.vel[0].y;
    streak->vel.z = rain_params.vel[0].z;
    streak->weight = rain_params.weight[0];
  }
  a = &streak->vel.x;
  h[0] = *a * 0.5f;
  h[2] = streak->vel.z * 0.5f;
  b = &streak->vel.z;
  *a = rain_lerp(-h[0], h[0], lcg_next_unit()) + *a;
  *b = rain_lerp(-h[2], h[2], lcg_next_unit()) + *b;
  rain_params.shape[0][1].x = -rain_params.shape[0][2].x;
  rain_params.shape[1][1].x = -rain_params.shape[1][2].x;
  streak->vel0 = streak->vel;
  if (streak->idx > 20) {
    for (i = 0; i < 3; i++) {
      streak->shape[i] = rain_params.shape[1][i];
    }
  }
  else {
    struct vec3 *t = rain_params.shape[0];
    for (i = 0; i < 3; i++) {
      vec3_cross(up, &streak->vel.x, axis);
      q = vec3_dot(up, &streak->vel.x) / vec3_length(&streak->vel.x);
      if (q < 1.0f) {
        float ang = vec3_normalize(axis);
        mtx_push_identity();
        mtx_rot_unit_axis_sincos(axis, ang, q);
        mtx_xform_dir(&t[i].x, &streak->shape[i].x);
        mtx_pop(1);
      }
      else {
        streak->shape[i] = t[i];
      }
    }
  }
}

void rain_streaks_scatter(void)
{
  rain_streak *e;
  int i;
  float a;
  float b;
  float c;

  e = rain_streaks;
  a = rain_params.box[0][0];
  b = rain_params.box[0][1];
  c = rain_params.box[0][2];
  for (i = 0; i < 21; i++, e++) {
    e->base.x = rain_lerp(-a,a,lcg_next_unit());
    e->base.z = rain_lerp(-c,b,lcg_next_unit());
    e->base.y = rain_lerp(-3.0,8.0,lcg_next_unit());
    e->idx = i;
    e->dist = star_fsqrt(e->base.x * e->base.x + e->base.z * e->base.z);
  }
  a = rain_params.box[1][0];
  b = rain_params.box[1][1];
  c = rain_params.box[1][2];
  for (i = 21; i < 36; i++, e++) {
    e->base.x = rain_lerp(-a,a,lcg_next_unit());
    e->base.z = rain_lerp(-c,-b,lcg_next_unit());
    e->base.y = rain_lerp(-3.0,8.0,lcg_next_unit());
    e->idx = i;
    e->dist = star_fsqrt(e->base.x * e->base.x + e->base.z * e->base.z);
  }
}

void rain_streaks_fall(void)
{
  rain_streak *streak;
  int i;

  streak = rain_streaks;
  i = 0;
  do {
    if (!(-3.0 < streak->pos.y)) {
      rain_streak_respawn(streak);
    }
    else {
      streak->vel.y = streak->weight * -0.00351 + streak->vel.y;
      vec3_add_to(&streak->pos.x,&streak->vel.x);
    }
    i = i + 1;
    streak = streak + 1;
  } while (i < 0x24);
  return;
}

void rain_sheets_draw(void)
{
  mtx_push(0);
  mtx_translate_by_vec3(rain_sheet0_pos);
  mtx_rot_y_s16(camera.yaw);
  obj_draw(0x23001e);
  mtx_pop(1);
  mtx_push(0);
  mtx_translate_by_vec3(rain_sheet1_pos);
  mtx_rot_y_s16(camera.yaw);
  obj_draw(0x23001e);
  mtx_pop(1);
  mtx_push(0);
  mtx_translate_by_vec3(rain_sheet2_pos);
  mtx_rot_y_s16(camera.yaw);
  obj_draw(0x23001e);
  mtx_pop(1);
  mtx_push(0);
  mtx_translate_by_vec3(rain_sheet3_pos);
  mtx_rot_y_s16(camera.yaw);
  obj_draw(0x23001e);
  mtx_pop(1);
  mtx_push(0);
  mtx_translate_by_vec3(rain_sheet4_pos);
  mtx_rot_y_s16(camera.yaw);
  obj_draw(0x23001e);
  mtx_pop(1);
  return;
}

void rain_sheet_model_clear(void)
{
  typedef struct { float x, y, z; } v3;
  char *model;
  int word;
  int i;
  v3 z;

  model = (char *)obj_model_get(0x23001e);
  i = 0;
  word = 0x30;
  z.x = z.y = z.z = 0.0f;
  *(uint *)(model + 0x6c) = 0x64999999;
  *(uint *)(model + 0x74) = 0x64999999;
  for (; i < *(int *)(model + 0xbc); i++) {
    *(v3 *)(word * 4 + model + 4) = z;
    word = word + 6;
  }
  return;
}

void rain_sheet_model_write(void)
{
  char *model;
  int corner;
  int word;
  rain_streak *streak;
  int n;

  model = (char *)obj_model_get(0x23001e);
  streak = rain_streaks;
  n = 0;
  corner = 0;
  word = 0x30;
  do {
    vec3_add((float *)(model + word * 4 + 4),&streak->pos.x,&streak->shape[corner].x);
    corner = corner + 1;
    word = word + 6;
    if (2 < corner) {
      corner = 0;
      n = n + 1;
      streak = streak + 1;
    }
  } while (n < 0x24);
  return;
}

float rain_lerp(float lo, float hi, float t)
{
  float f;
  if ((lo < hi) || (hi < lo)) {
    f = lo / (hi - lo);
    return (hi * (t + f)) / (1.0 + f);
  }
  return 0.0;
}

bool rain_outside_7(float *pos)
{
  bool r = 0;
  if (pos[0] > 7.0f) r = 1;
  if (pos[0] < -7.0f) r = 1;
  if (pos[2] > 7.0f) r = 1;
  if (pos[2] < -7.0f) r = 1;
  return r;
}

void rain_anim_models_init(void)
{
  compiled_model_rec *model;
  int i;
  int j;
  int k;
  int n;

  rain_anim_frame = 0.0;
  for (i = 0; i < 0x10; i++) {
    model = obj_model_get(rain_anim_models[i]);
    k = 0x30;
    n = model->vertex_count;
    for (j = 0; j < n; j++) {
      *(float *)(k * 4 + (char *)model + 8) = 12.595f;
      k = k + 6;
    }
  }
  return;
}

void rain_anim_advance(void)
{
  rain_anim_frame = rain_anim_frame + 0.5;
  if (16.0 <= rain_anim_frame) {
    rain_anim_frame = 0.0;
  }
  return;
}

void rain_anim_draw(void)
{
  obj_draw(rain_anim_models[(int)rain_anim_frame]);
  return;
}

void rain_init_tail(void)
{
  return;
}

void rain_fan_records_clear(void)
{
  rain_fan_rec *fan;
  int i;
  int j;

  fan = rain_fan_records;
  for (i = 0; i < 22; i++) {
    *(float *)&fan->field_0x0 = 0.0f;
    fan->count = 6;
    fan->field_0x350 = 0;
    for (j = 0; j < 30; j++) {
      fan->models[j] = 0;
    }
    fan++;
  }
  return;
}

void rain_cycle_tables_randomize(void)
{
  int frame;
  int j;
  int *table;
  int i;
  rain_fan_rec *fan;
  int k;
  int base;
  float t;

  fan = rain_fan_records;
  for (k = 0; k < 0x16; k++) {
    if (fan->field_0x350 != 0) {
      for (i = 0; i < fan->count; i++) {
          table = fan->frames[i];
          t = (float)lcg_next_unit();
          t = (float)rain_lerp(0.0,29.0,t);
          base = (int)t;
          for (j = 0; j < 0x1e; j++) {
            frame = base + j;
            *table = frame;
            if (0x1d < frame) {
              *table = frame + -0x1e;
            }
            table = table + 1;
          }
      }
      fan = fan + 1;
    }
  }
}

void rain_fan_models_build(void)
{
  typedef struct { float a; float b; } tent;
  typedef struct { int w[3]; } v3;
  int *tmpl;
  rain_fan_rec *fan;
  int k;
  int j;
  int i;
  int c;
  int idx;
  uint n;
  int *p;
  int *w;
  int v;
  tent *t;
  int *q;
  int m;
  v3 buf[4];

  tmpl = (int *)obj_model_get(0x23002f);
  fan = rain_fan_records;
  for (k = 0; k < 22; k++) {
    if (fan->field_0x350 != 0 && (n = fan->count) != 0) {
      for (j = 0; j < 30; j++) {
        p = (int *)(fan->models[j] = elan_heap_alloc(n * 0x60 + 0xc0));
        if (p == 0) {
          fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect_rain.c",0x46d,
                   "assertion `%s\' failed.\n","0");
        }
        for (i = 0; i < 0x30; i++) {
          p[i] = tmpl[i];
        }
        *(float *)(p + 5) = 10.0;
        p[6] = n * 0x60 + 0xc0;
        p[8] = n * 2;
        p[9] = n << 2;
        p[0x13] = n * 0x60 + 0x60;
        p[0x1a] = 0x6ff;
        p[0x1b] = 0xb2aaaaaa;
        p[0x1d] = 0xb2aaaaaa;
        p[0x2f] = n << 2;
        for (i = 0; i < (int)n; i++) {
          rain_fan_quad_corners(n,i,fan->field_0x350[3],(float *)buf);
          idx = i * 0x18 + 0x30;
          for (c = 0; c < 4; c++) {
            switch(c) {
            case 0:
            case 1:
              p[idx] = 0;
              break;
            case 2:
              p[idx] = 0x60000000;
              break;
            case 3:
              p[idx] = 0xa0000000;
              break;
            }
            w = (int *)(idx * 4 + (int)p);
            *w = *w & 0xff000000 | 0x7f0000;
            q = w + 1;
            *(v3 *)q = buf[c];
            q = q + 3;
            t = (tent *)&rain_fan_uvs[0][0][0];
            *(float *)q = t[fan->frames[i][j] * 4 + c].a;
            *(float *)(w + 5) = t[fan->frames[i][j] * 4 + c].b;
            idx = idx + 6;
          }
        }
      }
      fan = fan + 1;
    }
  }
  return;
}

void rain_fan_quad_corners(int count, int index, float radius, float *out)
{
  float v[3];
  typedef struct { float x; float y; float z; } corner;
  corner c[4] = {{0.05, 0.05, 0.0}, {0.05, -0.05, 0.0}, {-0.05, 0.05, 0.0}, {-0.05, -0.05, 0.0}};
  float a;
  float s;
  float k;
  int i;

  mtx_push_identity();
  a = (float)(count - 1) * 10.0;
  mtx_rot_z_s16((int)((((90.0 - a) + (float)index * 20.0) * 65536.0) / 360.0 + 0.5) & 0xffff);
  mtx_xform_dir(rain_fan_dir,v);
  mtx_pop(1);
  s = lcg_next_unit();
  s = rain_lerp(0.95,1.1,s);
  vec3_scale(v,v,radius * s);
  mtx_push_identity();
  mtx_translate_by_vec3(v);
  mtx_rot_z_s16((int)((((float)index * 20.0 - a) * 65536.0) / 360.0 + 0.5) & 0xffff);
  s = lcg_next_unit();
  k = rain_lerp(0.9,1.3,s);
  for (i = 0; i < 4; i++) {
    vec3_scale(&c[i].x,&c[i].x,k);
    mtx_xform_point(&c[i].x,out + i * 3);
  }
  mtx_pop(1);
}

void rain_part_update_empty(void)
{
  return;
}

void rain_part_draw_empty(void)
{
  return;
}

void rain_part_free_empty(void)
{
  return;
}

void rain_water_fx_init(void)
{
  rain_drip_models_bake();
  rain_splash_models_bake();
  rain_drip_init();
  rain_splash_init();
  return;
}

void rain_water_fx_update(void)
{
  rain_drip_update();
  rain_splash_update();
  return;
}

void rain_water_fx_draw(void)
{
  rain_drip_draw();
  rain_splash_draw();
  return;
}

void rain_water_fx_free(void)
{
  int i;
  int j;
  for (j = 0; j < 10; j++) {
    if (rain_spare_models[j] != 0) {
      elan_heap_free((void *)rain_spare_models[j]);
      rain_spare_models[j] = 0;
    }
  }
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 0x1e; j++) {
      if (rain_drip_models[i][j] != 0) {
        elan_heap_free(rain_drip_models[i][j]);
        rain_drip_models[i][j] = 0;
      }
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 0x1e; j++) {
      if (rain_splash_models[i][j] != 0) {
        elan_heap_free(rain_splash_models[i][j]);
        rain_splash_models[i][j] = 0;
      }
    }
  }
}

void rain_drip_init(void)
{
  rob_rec *rob;
  rain_emitter_rec *emitter;
  int j;
  uint side;
  rain_drip_set *set;

  set = (rain_drip_set *)rain_drip_sets;
  side = 0;
  do {
    rob = rob_get(side & 0xff);
    set->rob = rob;
    emitter = set->limbs;
    j = 0;
    do {
      emitter->particle_count = 0x1e;
      rain_emitter_reset(emitter);
      j = j + 1;
      emitter = emitter + 1;
    } while (j < 2);
    side = side + 1;
    set = set + 1;
  } while ((int)side < 2);
  return;
}

void rain_drip_update(void)
{
  typedef struct { char pad[0x30]; vec3 p; char r[4]; } J;
  vec3 v1;
  vec3 v2;
  float tmp[3];
  int k;
  rain_drip_set *set;
  char *a;
  char *b;
  rain_emitter_rec *s;
  rain_particle_rec *el;
  int j;
  int i;
  int n;
  float d;
  float t;
  float u;
  char **pp;
  rob_rec *rob;

  set = (rain_drip_set *)rain_drip_sets;
  for (k = 0; k < 2; k++, set++) {
    pp = (char **)set;
    rob = (rob_rec *)*pp++;
    s = (rain_emitter_rec *)pp;
    a = (char *)rob->bone_mtx;
    b = (char *)rob->bone_mtx_prev;
    for (j = 0; j < 2; j++, s++) {
      v1.x = ((J *)a)[rain_limb_joint_ids[j]].p.x;
      v1.y = ((J *)a)[rain_limb_joint_ids[j]].p.y;
      v1.z = ((J *)a)[rain_limb_joint_ids[j]].p.z;
      v2.x = ((J *)b)[rain_limb_joint_ids[j]].p.x;
      v2.y = ((J *)b)[rain_limb_joint_ids[j]].p.y;
      v2.z = ((J *)b)[rain_limb_joint_ids[j]].p.z;
      vec3_sub(tmp,&v1.x,&v2.x);
      d = vec3_normalize(tmp);
      u = d * 0.15;
      vec3_scale(&s->vel.x,tmp,u >= 0.02 ? 0.02 : u);
      s->pos = v1;
      for (el = s->particles, i = 0; i < s->particle_count; i++, el++) {
        if (el->active != 0) {
          el->life = 30;
          el->frame_step = 1.0;
          rain_particle_step(el);
        }
      }
      switch (s->state) {
      case 0:
        if (v1.y > 12.690001 && v2.y <= 12.690001) s->state = 1;
        break;
      case 1:
        if (v1.y > 12.690001) s->state = 2;
        if (v1.y <= 12.690001) s->state = 0;
        break;
      case 2:
        if (s->phase == 0) {
          if (set->rob->action.part.type != 1 && d >= 0.01) {
            n = s->count / 2;
            el = &s->particles[n];
            el->active = 1;
            el->pos = s->pos;
            el->vel = s->vel;
            t = lcg_next_unit();
            el->kind = (int)rain_lerp(0.0,4.0,t);
          }
          if (++s->count > 19) s->phase++;
        }
        if (v1.y <= 12.690001) {
          s->phase = 0;
          s->count = 0;
          s->state = 0;
        }
        break;
      }
    }
  }
}

void rain_drip_draw(void)
{
  int i;
  int k;
  int j;
  rain_drip_set *set;
  rain_emitter_rec *emitter;
  rain_particle_rec *e;
  for (set = (rain_drip_set *)rain_drip_sets, i = 0; i < 2; i++, set++) {
    emitter = set->limbs;
    for (k = 0; k < 2; k++, emitter++) {
      e = emitter->particles;
      for (j = 0; j < emitter->particle_count; j++, e++) {
        if (e->active != 0) {
          mtx_push(0);
          mtx_translate_by_vec3(&e->pos.x);
          mtx_rot_y_s16((int)camera.yaw);
          model_draw(rain_drip_models[e->kind][(int)e->frame]);
          mtx_pop(1);
        }
      }
    }
  }
}

void rain_emitter_reset(rain_emitter_rec *emitter)
{
  int i;
  rain_particle_rec *p;
  float *f = &emitter->pos.x;
  p = emitter->particles;
  f[0] = f[1] = f[2] = 0.0f;
  f[3] = f[4] = f[5] = 0.0f;
  emitter->count = 0;
  emitter->state = 0;
  emitter->phase = 0;
  for (i = 0; i < emitter->particle_count; i++, p++) {
    rain_particle_clear(p);
  }
}

void rain_particle_clear(rain_particle_rec *particle)
{
  float *f = &particle->pos.x;
  f[0] = f[1] = f[2] = 0.0f;
  f[3] = f[4] = f[5] = 0.0f;
  particle->frame = 0.0f;
  particle->field_0x28 = 0;
}

void rain_particle_step(rain_particle_rec *particle)
{
  float *frame;
  float next;

  vec3_add_to(&particle->pos.x,&particle->vel.x);
  frame = &particle->frame;
  next = *frame + particle->frame_step;
  *frame = next;
  if ((float)particle->life <= next) {
    *frame = 0.0;
    particle->active = 0;
  }
  return;
}

void rain_particle_spawn(rain_emitter_rec *emitter, float speed)
{
  typedef struct { int w[3]; } v3;
  int i;
  rain_particle_rec *e;
  int t;

  e = emitter->particles;
  for (i = 0; i < emitter->particle_count; i++, e++) {
    if (e->active == 0) {
      e->active = 1;
      *(v3 *)&e->pos = *(v3 *)&emitter->pos;
      e->vel.x = e->vel.y = e->vel.z = 0.0;
      e->frame = 0.0;
      t = speed > 0.1;
      e->kind = t == 0;
      e->spawn_speed = speed;
      return;
    }
  }
}

void rain_splash_init(void)
{
  uint i;
  int j;
  rain_emitter_rec *limb;
  rain_emitter_rec *sphere;
  rain_splash_set *set;
  char *model;
  set = (rain_splash_set *)rain_splash_sets;
  for (i = 0; (int)i < 2; i++, set++) {
    set->rob = rob_get(i & 0xff);
    limb = set->limbs;
    sphere = set->spheres;
    for (j = 0; j < 2; j++, limb++) {
      limb->particle_count = 3;
      rain_emitter_reset(limb);
    }
    for (j = 0; j < 10; j++, sphere++) {
      sphere->particle_count = 1;
      rain_emitter_reset(sphere);
    }
  }
  for (i = 0; (int)i < 0x10; i++) {
    model = (char *)obj_model_get(rain_extra_model_ids[i]);
    if (model != 0) *(uint *)(model + 0x68) = 0x6ff;
  }
  for (i = 0; (int)i < 0x10; i++) {
    model = (char *)obj_model_get(rain_sphere_splash_model_ids[i]);
    if (model != 0) *(uint *)(model + 0x68) = 0x6ff;
  }
}

void rain_splash_update(void)
{
  typedef struct { char pad[0x30]; vec3 p; char r[4]; } J;
  typedef struct { char pad[0xc]; float h; char r[4]; } H;
  vec3 v1;
  vec3 v2;
  float tmp[3];
  int k;
  rain_splash_set *set;
  vec3 *a2;
  vec3 *b2;
  char *a;
  char *b;
  char *c;
  char **pp;
  rob_rec *rob;
  rain_emitter_rec *s;
  rain_particle_rec *el;
  int j;
  int i;
  float d;
  float h;

  set = (rain_splash_set *)rain_splash_sets;
  mtx_push(0);
  for (k = 0; k < 2; k++, set++) {
    pp = (char **)set;
    rob = (rob_rec *)*pp++;
    s = (rain_emitter_rec *)pp;
    a = (char *)rob->bone_mtx;
    b = (char *)rob->bone_mtx_prev;
    a2 = rob->coli->sph_a;
    b2 = rob->coli->sph_b;
    c = *(char **)(rob->chara_data + 0x3c);
    for (j = 0; j < 2; j++, s++) {
      v1.x = ((J *)a)[rain_limb_joint_ids[j]].p.x;
      v1.y = ((J *)a)[rain_limb_joint_ids[j]].p.y;
      v1.z = ((J *)a)[rain_limb_joint_ids[j]].p.z;
      v2.x = ((J *)b)[rain_limb_joint_ids[j]].p.x;
      v2.y = ((J *)b)[rain_limb_joint_ids[j]].p.y;
      v2.z = ((J *)b)[rain_limb_joint_ids[j]].p.z;
      s->pos = v1;
      s->pos.y = 12.65f;
      for (el = s->particles, i = 0; i < s->particle_count; i++, el++) {
        if (el->active != 0) {
          el->life = 30;
          el->frame_step = 1.0f;
          rain_particle_step(el);
        }
      }
      switch (s->state) {
      default:
        if (v1.y + 0.05f <= 12.84f && v2.y + 0.05f > 12.84f) s->state = 3;
        break;
      case 3:
        switch (s->phase) {
        case 0:
          if (rain_outside_7(&s->pos.x) == 0) {
            vec3_sub(tmp, &v1.x, &v2.x);
            if ((unsigned char)(set->rob->action.part.type - 8) > 1 && (d = vec3_length(tmp)) >= 0.01f) {
              rain_particle_spawn(s, d);
            }
          }
          s->phase++;
          break;
        case 1:
          s->phase = 0;
          s->state = 0;
          break;
        }
        break;
      }
    }
    s = set->spheres;
    for (j = 0; j < 10; j++, s++) {
      v1 = a2[rain_sphere_ids[j]];
      v2 = b2[rain_sphere_ids[j]];
      h = ((H *)c)[rain_sphere_ids[j]].h;
      s->pos = v1;
      s->pos.y = 12.690001f;
      for (el = s->particles, i = 0; i < s->particle_count; i++, el++) {
        if (el->active != 0) {
          el->life = 16;
          el->frame_step = 0.5f;
          rain_particle_step(el);
        }
      }
      switch (s->state) {
      default:
        if (v1.y - h <= 12.690001f && v2.y - h > 12.690001f) s->state = 3;
        break;
      case 3:
        switch (s->phase) {
        case 0:
          if (rain_outside_7(&s->pos.x) == 0) {
            rain_particle_spawn(s, 0.0f);
          }
          s->phase++;
          break;
        case 1:
          if ((unsigned char)(set->rob->action.part.type - 8) > 1) {
            s->phase = 0;
            s->state = 0;
          }
          break;
        }
        break;
      }
    }
  }
  mtx_pop(1);
}

void rain_splash_draw(void)
{
  float v[3];
  int i;
  rain_splash_set *set;
  int n;
  int j;
  int k;
  rain_emitter_rec *limb;
  rain_emitter_rec *sphere;
  rain_particle_rec *p1;

  set = (rain_splash_set *)rain_splash_sets;
  n = 0;
  i = 0;
  do {
    limb = set->limbs;
    sphere = set->spheres;
    for (j = 0; j < 2; j++, limb++) {
      p1 = limb->particles;
      for (k = 0; k < limb->particle_count; k++, p1++) {
        if (p1->active != 0) {
          if (n < 4) {
            vec3_sub(v, rain_eye_pos, &p1->pos.x);
            vec3_normalize_nolen(v);
            vec3_scale(v, v, 0.2);
            mtx_push(0);
            mtx_translate_by_vec3(&p1->pos.x);
            mtx_translate(v[0], 0.0, v[2]);
            mtx_rot_y_s16((int)camera.yaw);
            model_draw(rain_splash_models[p1->kind][(int)p1->frame]);
            mtx_pop(1);
            n++;
          }
        }
      }
    }
    for (j = 0; j < 10; j++, sphere++) {
      p1 = sphere->particles;
      for (k = 0; k < sphere->particle_count; k++, p1++) {
        int *t;
        if (p1->active != 0) {
          if (n < 4) {
            vec3_sub(v, rain_eye_pos, &p1->pos.x);
            vec3_normalize_nolen(v);
            vec3_scale(v, v, 0.2);
            mtx_push(0);
            mtx_translate_by_vec3(&p1->pos.x);
            mtx_translate(v[0], 0.2, v[2]);
            mtx_reset_3x3();
            t = rain_sphere_splash_model_ids;
            obj_draw(t[(int)p1->frame]);
            mtx_pop(1);
            n++;
          }
        }
      }
    }
    i++;
    set = set + 1;
  } while (i < 2);
}

void rain_drip_models_bake(void)
{
  typedef struct { int w[3]; } v3;
  typedef struct { float a; float b; } tent;
  float v[9];
  v3 out;
  int *tmpl;
  int k;
  int j;
  int i;
  int idx;
  int *p;
  rain_psys_particle *e;
  int c;
  int *w;
  float f;
  uint col;
  tent *t;
  float *sc;
  float *q;
  int o;

  tmpl = (int *)obj_model_get(0x23001e);
  for (i = 0; i < 5; i++) {
    rain_psys_reset((rain_psys_rec *)rain_drip_psys, rain_drip_seed);
    for (j = 0; j < 30; j++) {
      p = (int *)(*(rain_drip_models[i] + j) = elan_heap_alloc(0x2af0));
      if (p == 0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect_rain.c",0x7aa,
                 "assertion `%s\' failed.\n","0");
      }
      for (k = 0; k < 0x30; k++) {
        p[k] = tmpl[k];
      }
      *(float *)(p + 5) = 20.0;
      p[6] = 0x2af0;
      p[8] = 150;
      p[9] = 450;
      p[0x13] = 0x2a90;
      p[0x1a] = 0x6ff;
      p[0x1b] = 0x99999999;
      p[0x1d] = 0x99999999;
      p[0x2f] = 450;
      f = 0.6;
      if (j > 19) {
        f = (1.0 - (float)(j - 20) / 10.0) * 0.6;
      }
      col = 0x999999;
      col |= (int)(f * 255.0) << 24;
      p[0x1b] = col;
      p[0x1d] = col;
      for (k = 0; k < 150; k++) {
        idx = k * 18 + 0x30;
        e = &((rain_psys_rec *)rain_drip_psys)->particles[k];
        c = 0;
      vtx:
          w = (int *)(idx * 4 + (int)p);
          *w = rain_drop_vtx_cmds[c];
          *(float *)(w + 4) = ((tent *)rain_drop_uvs)[c].a;
          *(float *)(w + 5) = ((tent *)rain_drop_uvs)[c].b;
          *w = *w & 0xff000000 | 0x7f0000;
          o = c * 12;
          q = (float *)(o + (int)v);
          sc = rain_drop_scales[0] + c * 3;
          q[0] = ({ float r = lcg_next_unit(); rain_lerp(0.01014,0.0078f * 1.8f,r); }) * sc[0];
          q[1] = ({ float r = lcg_next_unit(); rain_lerp(0.01014,0.0078f * 1.8f,r); }) * sc[1];
          q[2] = sc[2] * ({ float r = lcg_next_unit(); rain_lerp(0.01014,0.0078f * 1.8f,r); });
          vec3_add((float *)&out, &e->pos.x, q);
          *(v3 *)(w + 1) = out;
          idx = idx + 6;
          c++;
          if (c < 3) goto vtx;
      }
      rain_psys_step((rain_psys_rec *)rain_drip_psys);
    }
  }
}

void rain_splash_seed_tall(rain_psys_particle *particle)
{
  float *p;
  float *q;
  float *c;
  float *d;
  float t;
  float v[3];

  p = &particle->pos.x;
  c = &particle->owner->origin.x;
  q = p;
  t = lcg_next_unit();
  *q++ = rain_lerp(-0.05,0.05,t);
  t = lcg_next_unit();
  *q++ = rain_lerp(0.0,0.02,t);
  t = lcg_next_unit();
  *q = rain_lerp(-0.05,0.05,t);
  t = lcg_next_unit();
  d = c;
  v[0] = rain_lerp(-0.04,0.04,t) + *d++;
  t = lcg_next_unit();
  v[1] = rain_lerp(0.0,0.68,t) + *d;
  c += 2;
  t = lcg_next_unit();
  v[2] = rain_lerp(-0.04,0.04,t) + *c;
  vec3_normalize_nolen(v);
  q = p + 3;
  t = lcg_next_unit();
  t = rain_lerp(0.01,0.027,t);
  vec3_scale(q,v,t);
  particle->field_0x64 = 0;
  particle->field_0x68 = 0;
  q += 0x18;
  t = lcg_next_unit();
  *q = rain_lerp(0.018,0.021,t);
  p += 7;
  t = lcg_next_unit();
  *p = rain_lerp(0.21,0.59,t) * -0.0027;
  return;
}

void rain_splash_seed_low(rain_psys_particle *particle)
{
  float *p = &particle->pos.x;
  float *q;
  float v[3];
  float *d;

  q = &particle->owner->origin.x;
  p[0] = ({ float r = lcg_next_unit(); rain_lerp(-0.05,0.05,r); });
  p[1] = ({ float r = lcg_next_unit(); rain_lerp(0.0,0.02,r); });
  p[2] = ({ float r = lcg_next_unit(); rain_lerp(-0.05,0.05,r); });
  v[0] = q[0] + ({ float r = lcg_next_unit(); rain_lerp(-0.11,0.11,r); });
  v[1] = q[1] + ({ float r = lcg_next_unit(); rain_lerp(0.0,0.08,r); });
  v[2] = q[2] + ({ float r = lcg_next_unit(); rain_lerp(-0.11,0.11,r); });
  vec3_normalize_nolen(v);
  d = p + 3;
  vec3_scale(d,v,({ float r = lcg_next_unit(); rain_lerp(0.005,0.016,r); }));
  particle->field_0x64 = 0;
  particle->field_0x68 = 0;
  particle->field_0x6c = ({ float r = lcg_next_unit(); rain_lerp(0.018,0.021,r); });
  particle->gravity = ({ float r = lcg_next_unit(); rain_lerp(0.21,0.6,r); }) * -0.0027;
}

void rain_splash_models_bake(void)
{
  typedef struct { int w[3]; } v3;
  typedef struct { float a; float b; } tent;
  float v[9];
  v3 out;
  int *tmpl;
  int k;
  int j;
  int i;
  int idx;
  int *p;
  rain_psys_particle *e;
  int c;
  int *w;
  float f;
  uint col;
  tent *t;
  float *sc;
  float *q;

  tmpl = (int *)obj_model_get(0x23001e);
  for (i = 0; i < 2; i++) {
    if (i == 0) rain_psys_reset((rain_psys_rec *)rain_splash_psys, rain_splash_seed_tall);
    else rain_psys_reset((rain_psys_rec *)rain_splash_psys, rain_splash_seed_low);
    for (j = 0; j < 30; j++) {
      p = (int *)(*(rain_splash_models[i] + j) = elan_heap_alloc(0x2af0));
      if (p == 0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect_rain.c",0x827,
                 "assertion `%s\' failed.\n","0");
      }
      for (k = 0; k < 0x30; k++) {
        p[k] = tmpl[k];
      }
      *(float *)(p + 5) = 20.0;
      p[6] = 0x2af0;
      p[8] = 150;
      p[9] = 450;
      p[0x13] = 0x2a90;
      p[0x1a] = 0x6ff;
      p[0x1b] = 0x99999999; /* redundant, but game code: this and p[0x1d] are overwritten with col below, but the game performs both stores. */
      p[0x1d] = 0x99999999;
      p[0x2f] = 450;
      f = 0.6;
      if (j > 19) {
        f = (1.0 - (float)(j - 20) / 10.0) * 0.6;
      }
      col = 0x999999;
      col |= (int)(f * 255.0) << 24;
      p[0x1b] = col;
      p[0x1d] = col;
      for (k = 0; k < 150; k++) {
        idx = k * 18 + 0x30;
        e = &((rain_psys_rec *)rain_splash_psys)->particles[k];
        c = 0;
      next_c:
          w = (int *)(idx * 4 + (int)p); *w = rain_drop_vtx_cmds[c]; *(float *)(w + 4) = ((tent *)rain_drop_uvs)[c].a; *(float *)(w + 5) = ((tent *)rain_drop_uvs)[c].b; *w = *w & 0xff000000 | 0x7f0000;
          q = (float *)(c * 12 + (int)v);
          sc = rain_drop_scales[0] + c * 3;
          q[0] = ({ float r = lcg_next_unit(); rain_lerp(0.011960001,0.01656,r); }) * sc[0];
          q[1] = ({ float r = lcg_next_unit(); rain_lerp(0.011960001,0.01656,r); }) * sc[1];
          q[2] = sc[2] * ({ float r = lcg_next_unit(); rain_lerp(0.011960001,0.01656,r); });
          vec3_add((float *)&out, &e->pos.x, q);
          *(v3 *)(w + 1) = out;
          idx = idx + 6;
        c++;
        if (c < 3) goto next_c;
      }
      rain_psys_step((rain_psys_rec *)rain_splash_psys);
    }
  }
}

void rain_psys_reset(rain_psys_rec *psys, void (*seed)(rain_psys_particle *))
{
  rain_psys_particle *particle;
  int i;
  void (*fn)(rain_psys_particle *);

  psys->origin.x = 0.0f;
  psys->origin.y = 0.0f;
  psys->origin.z = 0.0f;
  psys->vel_scale.x = 0.0f;
  psys->vel_scale.y = 0.0f;
  psys->vel_scale.z = 0.0f;
  psys->frame = 0;
  psys->field_0x60 = 0;
  psys->mode = 1;
  psys->seed = seed;
  fn = seed;
  particle = psys->particles;
  for (i = 0; i < 0x96; i++, particle++) {
    particle->owner = psys;
    (*fn)(particle);
  }
}

void rain_drip_seed(rain_psys_particle *particle)
{
  typedef struct { int w[3]; } v3;
  float *q;
  float *s;
  float *d;
  float v[3];
  float t;

  q = &particle->owner->origin.x;
  t = ({ float r = lcg_next_unit(); rain_lerp(-0.11,0.11,r); });
  s = q;
  particle->pos.x = t + *s++;
  d = &particle->pos.y;
  *d = ({ float r = lcg_next_unit(); rain_lerp(-0.08,0.08,r); }) + *s;
  d++;
  {
    float *s2 = q + 2;
    *d = ({ float r = lcg_next_unit(); rain_lerp(-0.11,0.11,r); }) + *s2;
  }
  switch (((rain_psys_rec *)q)->mode) {
  case 0: {
    float *cs;
    d = &particle->vel.x;
    cs = q + 3;
    *d = ({ float r = lcg_next_unit(); rain_lerp(0.9,2.5,r); }) * *cs;
    d++;
    cs++;
    *d = ({ float r = lcg_next_unit(); rain_lerp(1.0,1.5,r); }) * *cs;
    d++;
    cs++;
    *d = ({ float r = lcg_next_unit(); rain_lerp(0.9,2.5,r); }) * *cs;
    break;
  }
  case 1: {
    float *m;
    *(v3 *)v = *(v3 *)&particle->pos;
    vec3_normalize_nolen(v);
    m = &particle->vel.x;
    vec3_scale(m,v,({ float r = lcg_next_unit(); rain_lerp(0.005,0.016,r); }));
    break;
  }
  }
  particle->field_0x64 = 0;
  particle->field_0x68 = 0;
  {
    float *e = &particle->field_0x6c;
    *e = ({ float r = lcg_next_unit(); rain_lerp(0.018,0.021,r); });
    e -= 20;
    *e = ({ float r = lcg_next_unit(); rain_lerp(0.21,0.6,r); }) * -0.0027;
  }
}

void rain_psys_step(rain_psys_rec *psys)
{
  rain_psys_particle *p;
  int i;

  p = psys->particles;
  if (++psys->frame > 29) {
    rain_psys_reset(psys,psys->seed);
  }
  else {
    for (i = 0; i < 0x96; i++, p++) {
      rain_psys_particle_step(p);
    }
  }
  return;
}

void rain_psys_particle_step(rain_psys_particle *particle)
{
  particle->vel.y = particle->vel.y + particle->gravity;
  vec3_add_to(&particle->pos.x,&particle->vel.x);
  return;
}

void rain_last_init(void)
{
  rain_last_word = 0;
  return;
}

void rain_last_update_empty(void)
{
  return;
}

void rain_last_draw_empty(void)
{
  return;
}
