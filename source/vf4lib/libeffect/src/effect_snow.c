/*
 * vf4lib/libeffect/src/effect_snow.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 snow_grid_dents; /* 0x0c1a1b88 */
extern undefined4 snow_body_joints; /* 0x0c1a5b74 */
extern undefined snow_body_points_a; /* 0x0c1a5bc4 */
extern undefined snow_body_points_b; /* 0x0c1a5c34 */
extern int snow_spray_model_ids[3][32]; /* 0x0c1a5cc4 */
extern int snow_fx_flags; /* 0x0c2b0250 */
extern float snow_cell_step; /* 0x0c2b0254 */
extern float snow_cell_step_inv; /* 0x0c2b0258 */
extern float snow_ground_top; /* 0x0c2b025c */
extern float snow_ground_y; /* 0x0c2b0260 */
extern float snow_spray_curve_leave[128]; /* 0x0c2b030c */
extern float snow_spray_curve_inside[128]; /* 0x0c2b050c */
extern float snow_press_max; /* 0x0c2b070c */
extern float snow_slump_step; /* 0x0c2b0710 */
extern int snow_spray_kinds; /* 0x0c2b0718 */
extern int snow_bodies; /* 0x0c389db4 */
extern uint stage_fx_rand_state; /* 0x0c38a844 */
extern short snow_prev_yaw; /* 0x0c38bfec */
extern float snow_flake_tris[32][3][3]; /* 0x0c38bff0 */
extern int snow_slump_count; /* 0x0c38c470 */
extern int snow_dirty_count; /* 0x0c38c474 */
extern short snow_yaw_delta; /* 0x0c38c478 */
extern snow_cell_rec *snow_grid; /* 0x0c38cd50 */
extern ushort snow_dirty_cells[10202]; /* 0x0c38cd54 */
extern int snow_sheet_count; /* 0x0c489878 */
extern int snow_djo_fixed_sheet_count; /* 0x0c48987c */
extern int snow_cas_timing_frames; /* 0x0c4898ac */
extern char snow_sprays[80][56]; /* 0x0c4898b0 */
extern int snow_spray_list_len; /* 0x0c48aad4 */
extern int snow_spray_cursor; /* 0x0c48aad8 */
extern uchar snow_spray_list_new; /* 0x0c48aadc */
extern uchar snow_spray_list_live; /* 0x0c48aadd */
extern uchar snow_djo_zone_far; /* 0x0c48aade */
extern uchar snow_djo_zone_deep; /* 0x0c48aadf */
extern camera_state camera; /* 0x0c370ce0 */
extern snow_flake snow_flakes[150]; /* 0x0c38a848 */
extern snow_wind_rec snow_wind; /* 0x0c38bfb8 */
extern snow_sway_circle snow_sway[16]; /* 0x0c38c47c */
extern snow_sheet snow_sheets[50]; /* 0x0c38c57c */
extern ground_frame_rec snow_ground_frame; /* 0x0c38cd4c */
extern float snow_cas_timings[10]; /* 0x0c489884 */
extern uchar snow_spray_lists[2][81]; /* 0x0c48aa30 */

static inline float ball_fsqrt(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

typedef struct { int head; vec3 pos; vec3 dir; float len; float rad; char pad24[12]; uint flags; char pad34; char tag; char pad36[54]; } snow_body_slot; typedef struct { int joint; int joint2; float rad; char tag; char pad[3]; } snow_body_src; typedef struct { float m[12]; vec3 t; float w; } snow_bone_mtx; static inline float trail_neg(float v)
{
  return -v;
}

void stage_fx_rand_reset(void)
{
  stage_fx_rand_state = 1;
  return;
}

uint stage_fx_rand_next(void)
{
  uint r;
  r = 0x41c64e6d;
  stage_fx_rand_state = (r = (stage_fx_rand_state * r) + 0x3039);
  r = r >> 0x10;
  return r;
}

void snow_cas_init(void)
{
  snow_grid = effect_malloc(560000);
  stage_fx_rand_reset();
  snow_flakes_sheets_init(0x1a0000,snow_cas_sheet_spots);
  snow_ground_init();
  snow_spray_models_bits_clear();
  return;
}

void snow_cas_update(void)
{
  float f0;

  f0 = (float)(int)stopwatch_usec();
  snow_dirty_lists_clear();
  snow_wind_update();
  snow_sway_update();
  snow_flakes_update();
  if ((snow_fx_flags & 4) != 0) {
    snow_ground_update();
  }
  f0 = (float)(int)stopwatch_usec() - f0;
  snow_cas_timings[0] = f0;
  if (snow_cas_timing_frames > 0x3c) {
    if (f0 > snow_cas_timings[1]) {
      snow_cas_timings[1] = f0;
    }
  }
  else {
    snow_cas_timing_frames = snow_cas_timing_frames + 1;
  }
}

void snow_cas_draw(void)
{
  float t0;
  float *a;
  
  t0 = (float)(int)stopwatch_usec();
  if ((snow_fx_flags & 1) != 0) {
    snow_flakes_draw();
  }
  if ((snow_fx_flags & 2) != 0) {
    snow_sheets_draw();
  }
  if ((snow_fx_flags & 4) != 0) {
    snow_ground_draw();
  }
  t0 = (float)(int)stopwatch_usec() - t0;
  a = snow_cas_timings;
  a[2] = t0;
  if ((snow_cas_timing_frames > 0x3c) && (t0 > a[3])) {
    a[3] = t0;
  }
  snow_cas_draw_tail();
}

void snow_cas_free(void)
{
  if (snow_ground_model != 0) {
    elan_heap_free(snow_ground_model);
  }
  effect_free(snow_grid);
  snow_grid = 0;
  return;
}

void snow_flakes_sheets_init(int model, snow_sheet_spot *spots)
{
  snow_flake *p;
  snow_sheet *q;
  uint r;
  float prev;
  float f;
  float fb;
  float k;
  float lim;
  int i;
  r = lcg_next15();
  p = snow_flakes;
  i = 0;
  for (; i < 0x96; i++, p++)
  {
    k = 7.629627e-05;
    prev = ((float) ((int) r)) * k;
    r = lcg_next15();
    p->pos.x = (p->home.x = (cos_ang16(r * 2) * prev) + (lim = 2.0));
    p->pos.z = (p->home.z = sin_ang16(r * 2) * prev);
    p->pos.y = ((fb = (float) (r & 0xff)) * 0.0392) + snow_ground_y;
    p->home.y = (fb * k) + (snow_ground_y + 8.0);
    p->weight = (((float) ((((int) r) >> 4) & 0xff)) * 0.00392) + 1.0;
    if (p->weight > lim)
    {
      p->weight = lim;
    }
    p->sway = (r >> 8) & 0xf;
    p->spin = stage_fx_rand_next();
    p->tri = 0;
  }

  if (model > 0)
  {
    snow_flakes_bind_model();
  }
  snow_sheet_count = 0;
  q = snow_sheets;
  for (i = 0; i < 0x32; i++, q++)
  {
    q->x = 0.0;
    q->z = 0.0;
    q->y[0] = snow_ground_y;
    q->y[1] = snow_ground_y + 5.0;
    q->y[2] = (snow_ground_y + 5.0) + 5.0;
    q->spin = stage_fx_rand_next();
    if (spots->r > 0.0)
    {
      q->r = spots->r;
      q->spd = spots->speed;
      *(int *)&q->place = spots->angle;
      q->on = 1;
      snow_sheet_count++;
      spots++;
    }
    else
    {
      q->r = 0.0;
      q->spd = 0.0;
      *(int *)&q->place = 0;
      q->on = 0;
    }
  }

  snow_sway_init();
  snow_wind_init();
  snow_flake_tris_build();
  for (i = 0; i < 10; i++)
  {
    snow_cas_timings[i] = 0.0;
  }

  snow_cas_timing_frames = 0;
}

void snow_flakes_bind_model(void)
{
  int *buf;
  int k;
  int i;
  int j;
  int n;
  snow_flake *p;

  buf = (int *)obj_model_get(0x1a0000);
  if (buf != 0) {
    p = snow_flakes;
    k = 0;
    i = 0x2f;
    while (k < 0x96) {
      n = buf[i] / 3;
      for (j = 0; j < n && k < 0x96; j++, k++) {
        p->tri = (float *)((char *)&buf[i + 2] + j * 0x48);
        p++;
      }
      i = n * 0x12 + i + 0x18;
    }
  }
}

void snow_wind_init(void)
{
  snow_wind.base.x = -0.7;
  snow_wind.base.y = 0.0;
  snow_wind.base.z = 0.2;
  snow_wind.dir.x = 0.3;
  snow_wind.dir.y = 0.0;
  snow_wind.dir.z = 0.3;
  snow_wind.wind.x = 0.0;
  snow_wind.wind.y = 0.0;
  snow_wind.wind.z = 0.0;
  snow_wind.phase = 0;
  snow_wind.phase2 = 0;
  snow_wind.phase_step = 0x60;
  snow_wind.phase2_step = 0x130;
  snow_wind.amp = 1.0;
  snow_wind.amp2 = 30000.0;
  return;
}

void snow_sway_init(void)
{
  snow_sway_circle *p;
  int i;
  short y;
  uint r;
  p = snow_sway;
  i = 0;
  y = 0xc0;
  do {
    r = lcg_next15();
    p->ang = r * 2;
    p->spd = y;
    p->x = 0.0;
    p->y = 0.0;
    p->r = (float)((int)r >> 4 & 0xff) * 7.84e-05 + 0.02;
    i++;
    y += 0x20;
    p++;
  } while (i < 0x10);
}

void snow_flake_tris_build(void)
{
  int i;
  float v[3][3];
  v[0][0] = 0.05;
  v[0][1] = 0.0375;
  v[0][2] = 0.0;
  { float *q = v[1]; *q = -0.05; q++; *q = 0.0375; q++; *q = 0.0; }
  v[2][0] = 0.0;
  v[2][1] = -0.0375;
  v[2][2] = 0.0;
  mtx_push(0);
  for (i = 0; i < 0x20; i++) {
    mtx_load_identity();
    mtx_rot_y_s16(i << 0xb);
    mtx_xform_points(v, snow_flake_tris[i], 3);
  }
  mtx_pop(1);
}

void snow_wind_update(void)
{
  snow_wind_rec *p = &snow_wind;
  float f;
  p->phase = p->phase + p->phase_step;
  p->phase2 = p->phase2 + p->phase2_step;
  f = sin_ang16(p->phase2);
  f = f * p->amp2;
  f = sin_ang16((short)((short)(int)f + p->phase));
  f = f * p->amp;
  p->wind.x = p->base.x + f * p->dir.x;
  p->wind.y = p->base.y + f * p->dir.y;
  p->wind.z = p->base.z + f * p->dir.z;
}

void snow_sway_update(void)
{
  snow_sway_circle *p;
  int i;

  p = snow_sway;
  for (i = 0; i < 16; i++, p++) {
    p->ang = p->ang + p->spd * 2;
    p->x = sin_ang16(p->ang) * p->r;
    p->y = cos_ang16(p->ang) * p->r;
  }
}

void snow_sheets_update(float dist)
{
  snow_sheet *p;
  int i;
  int j;
  int k;

  p = snow_sheets;
  snow_yaw_delta = camera.yaw - snow_prev_yaw;
  snow_prev_yaw = camera.yaw;
  for (i = 0; i < snow_sheet_count; i++, p++) {
    if (p->on != '\0') {
      p->x = camera.eye.x + sin_ang16(camera.yaw + p->place) * (dist + p->r);
      p->z = cos_ang16(camera.yaw + p->place) * (dist + p->r) + camera.eye.z;
      p->spin = p->spin - snow_yaw_delta + 0x10;
      for (j = 0; j < 3; j++) {
        p->y[j] -= p->spd;
        if (p->y[j] < snow_ground_y - 2.5) {
          k = j - 1;
          if (k < 0) k = 2;
          p->y[j] = p->y[k] + 5.0;
        }
      }
    }
  }
}

void snow_flakes_update(void)
{
  float k;
  snow_wind_rec *wd;
  snow_flake *p;
  int i;
  uint r;
  float v[3];
  float f;
  float *pos;
  float *pz;
  camera_state *cam;
  short *ang;

  k = 0.018;
  wd = &snow_wind;
  p = snow_flakes;
  cam = &camera;
  pos = &cam->eye.x;
  ang = &cam->yaw;
  v[0] = sin_ang16(*ang + 0x8000) * 6.0 + *pos + 2.0;
  pz = pos + 2;
  v[2] = cos_ang16(*ang + 0x8000) * 6.0 + *pz;
  v[1] = cam->eye.y + 4.0;
  for (i = 0; i < 0x96; i++, p++) {
    r = stage_fx_rand_next();
    p->pos.y -= p->weight * 0.0016 + k;
    if (p->pos.y < snow_ground_y) {
      p->pos.x = (v[0] - 5.0) + (float)(uchar)r * 0.0392;
      p->pos.y = v[1];
      p->pos.z = (v[2] - 5.0) + (float)(int)((r & 0xff00) >> 8) * 0.0392;
    }
    else if (p->pos.y < snow_ground_y + 5.0) {
      p->pos.x = wd->wind.x * 0.012 + p->pos.x;
      p->pos.z = wd->wind.z * 0.012 + p->pos.z;
    }
  }
  f = fsqrt_asm(camera.eye.x * camera.eye.x + camera.eye.z * camera.eye.z) * 0.16;
  if (f > 1.0) f = 1.0;
  snow_sheets_update(f);
}

void snow_flakes_draw(void)
{
  compiled_model_rec *h;
  int i;
  int idx;
  float *s;
  float *d;
  snow_flake *p;
  float m[4][3];
  float (*tb)[3][3];

  h = obj_model_get(0x1a0000);
  if (h != 0) {
    idx = (camera.yaw + 0x8000 >> 11) & 0x1f;
    for (i = 0; i < 3; i++) {
      d = (float *)(i * 0xc + (int)m);
      tb = snow_flake_tris;
      s = tb[idx][i];
      d[0] = s[0];
      d[1] = s[1];
      d[2] = s[2];
    }
    h->sphere[3] = 20.0;
    p = snow_flakes;
    for (i = 0; i < 0x96; i++, p++) {
      p->tri[0] = p->pos.x + snow_sway[p->sway].x + m[0][0];
      p->tri[1] = p->pos.y + snow_sway[p->sway].x + m[0][1];
      p->tri[2] = p->pos.z + snow_sway[p->sway].x + m[0][2];
      p->tri[6] = p->pos.x + snow_sway[p->sway].x + m[1][0];
      p->tri[7] = p->pos.y + snow_sway[p->sway].x + m[1][1];
      p->tri[8] = p->pos.z + snow_sway[p->sway].x + m[1][2];
      p->tri[12] = p->pos.x + snow_sway[p->sway].x + m[2][0];
      p->tri[13] = p->pos.y + snow_sway[p->sway].x + m[2][1];
      p->tri[14] = p->pos.z + snow_sway[p->sway].x + m[2][2];
    }
    model_draw(h);
  }
}

void snow_sheets_draw(void)
{
  int j;
  snow_sheet *p;
  int i;
  
  p = snow_sheets;
  i = 0;
  do {
    if (p->on != '\0') {
      j = 0;
      do {
        mtx_push(0);
        mtx_translate(p->x,p->y[j],p->z);
        mtx_rot_y_s16((int)camera.yaw + p->spin);
        obj_draw(0x1a0001);
        mtx_pop(1);
        j = j + 1;
      } while (j < 3);
    }
    i = i + 1;
    p = p + 1;
  } while (i < 0x32);
  return;
}

void snow_ground_init(void)
{
  snow_ground_model = 0;
  snow_ground_model_build();
  snow_grid_build();
  snow_dirty_lists_clear();
  snow_ground_vertices_write_all();
  snow_sprays_reset();
  light_set_load(2,9);
  return;
}

void snow_ground_model_build(void)
{
  int *src;
  int n;
  int i, j, idx, m;
  float s1, s2;
  src = (int *)obj_model_get(0x1a0042);
  if (src == 0) {
    snow_ground_model = 0;
    return;
  }
  n = 20200;
  snow_ground_model = elan_heap_alloc(n * 24 + 0xc0);
  if (snow_ground_model == 0) return;
  for (j = 0; j < 0x30; j++) ((int *)snow_ground_model)[j] = src[j];
  snow_ground_model->sphere[3] = 20.0;
  snow_ground_model->size = n * 24 + 0xc0;
  snow_ground_model->triangle_count = 20000;
  snow_ground_model->vertex_count = n;
  snow_ground_model->field_0x28 = 1;
  snow_ground_model->field_0x2c = 1;
  snow_ground_model->parts[0].size = n * 24 + 0x60;
  *(int *)((char *)snow_ground_model + 0x6c) = 0xffc0c0c0;
  *(int *)((char *)snow_ground_model + 0x74) = 0xffc0c0c0;
  *(uint *)((char *)snow_ground_model + 0xa4) &= 0xe7ffffff;
  *(uint *)((char *)snow_ground_model + 0xa4) |= 0x18000000;
  *(uint *)((char *)snow_ground_model + 0xa8) &= 0xfffe7fff;
  *(uint *)((char *)snow_ground_model + 0xb0) &= 0xfffe7fff;
  *(int *)((char *)snow_ground_model + 0xbc) = n;
  m = 202;
  s1 = 0.04;
  s2 = 0.04;
  for (j = 0, idx = 48; j < 100; j++) {
    for (i = 0; i < m; i++) {
      switch (i) {
      case 0:
      case 1:
        ((int *)snow_ground_model)[idx] = 0;
        break;
      case 2:
        ((int *)snow_ground_model)[idx] = 0x60000000;
        break;
      case 201:
        ((int *)snow_ground_model)[idx] = 0xa0000000;
        break;
      default:
        ((int *)snow_ground_model)[idx] = 0x20000000;
        break;
      }
      ((float *)snow_ground_model)[idx + 4] = (float)(i / 2) * s1;
      ((float *)snow_ground_model)[idx + 5] = (float)(i - i / 2 * 2 + j) * s2;
      idx += 6;
    }
  }
}

void snow_grid_height_table_apply(void)
{
  struct snow_dent { int cell; float factor; } *p;
  float *v;
  float t;
  int k;
  int b;
  
  for (p = (struct snow_dent *)&snow_grid_dents; p->cell != -1; p++) {
    t = p->factor / 2.8;
    k = p->cell;
    b = (int)snow_grid;
    v = (float *)(k * 0x2c + b + 4);
    *v = (*v - snow_ground_y) * t + snow_ground_y;
  }
}

void snow_grid_build(void)
{
  snow_cell_rec *e;
  float x;
  float z;
  float k;
  float h;
  int i;
  int j;
  int t;
  int v;

  snow_ground_frame.frame = 0;
  snow_ground_frame.field_0x0 = 0;
  e = snow_grid;
  x = snow_cell_step * -50.0;
  z = snow_cell_step * -50.0;
  k = 3.14 / (snow_cell_step * 33.0);
  for (i = 0; i < 10201; i++, e++) {
    if (i > 0 && i % 101 == 0) {
      x = snow_cell_step * -50.0;
      z += snow_cell_step;
    }
    e->x = x;
    e->z = z;
    e->y = sin_radians(k * z) * 0.08 + 4.21;
    if (e->y > snow_ground_top) {
      snow_ground_top = e->y;
    }
    e->press_frame = -100;
    e->state = 0;
    e->links = 0;
    for (j = 0; j < 4; j++) {
      e->link[j] = -1;
    }
    if (i % 101 != 0) {
      e->link[0] = i - 1;
      e->links |= 1;
    }
    if ((i + 1) % 101 != 0) {
      e->link[1] = i + 1;
      e->links |= 2;
    }
    if (i - 101 >= 0) {
      e->link[2] = i - 101;
      e->links |= 4;
    }
    if (i + 101 < 10201) {
      e->link[3] = i + 101;
      e->links |= 8;
    }
    e->vert[1] = 0;
    e->vert[0] = 0;
    x += snow_cell_step;
  }
  snow_ground_top = snow_ground_top + 0.1;
  snow_grid_height_table_apply();
  for (i = 0; i < 100; i++) {
    v = i * 0x4bc + 0x30;
    for (j = 0; j < 202; j++) {
      int n = j / 2;
      e = snow_grid + (n + (j % 2 + i) * 101);
      t = 0;
      if (e->vert[0] > 0) t = 1;
      e->vert[t] = v;
      v += 6;
    }
  }
}

void snow_ground_vertices_write_all(void)
{
  uchar *base;
  snow_cell_rec *e;
  uint c;
  int i;

  base = (uchar *)snow_ground_model;
  if (base != 0) {
    e = snow_grid;
    for (i = 0; i < 0x27d9; i++, e++) {
      c = snow_cell_normal_packed(e);
      if (e->vert[0] > 0) {
        ((float *)(base + e->vert[0] * 4))[1] = e->x;
        ((float *)(base + e->vert[0] * 4))[2] = e->y;
        ((float *)(base + e->vert[0] * 4))[3] = e->z;
        *(uint *)(base + e->vert[0] * 4) &= 0xff000000;
        *(uint *)(base + e->vert[0] * 4) |= c;
        if (e->vert[1] > 0) {
          ((float *)(base + e->vert[1] * 4))[1] = e->x;
          ((float *)(base + e->vert[1] * 4))[2] = e->y;
          ((float *)(base + e->vert[1] * 4))[3] = e->z;
          *(uint *)(base + e->vert[1] * 4) &= 0xff000000;
          *(uint *)(base + e->vert[1] * 4) |= c;
        }
      }
    }
  }
}

void snow_ground_vertices_write_dirty(void)
{
  ushort v;
  ushort *p;
  uchar *t;
  uint m;
  snow_cell_rec *e;
  
  t = (uchar *)snow_ground_model;
  if (t != (uchar *)0x0) {
    p = snow_dirty_cells;
    while ((v = *p++) != 0xffff) {
      e = &snow_grid[v];
      m = snow_cell_normal_packed(e);
      if (0 < e->vert[0]) {
        *(float *)(t + e->vert[0] * 4 + 8) = e->y;
        *(uint *)(t + e->vert[0] * 4) &= 0xff000000;
        *(uint *)(t + e->vert[0] * 4) |= m;
        if (0 < e->vert[1]) {
          *(float *)(t + e->vert[1] * 4 + 8) = e->y;
          *(uint *)(t + e->vert[1] * 4) &= 0xff000000;
          *(uint *)(t + e->vert[1] * 4) |= m;
        }
      }
      e->state &= 0xfffe;
    }
    snow_dirty_cells[0] = 0xffff;
  }
  return;
}

uint snow_cell_normal_packed(snow_cell_rec *cell)
{
  typedef struct { int w[3]; } vec3_words;
  float m[4][3];
  float *a;
  float *b;
  int q;
  int k;

  switch (cell->links) {
  case 10:
    *(vec3_words *)m[0] = *(vec3_words *)cell;
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[3]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[1]];
    break;
  case 11:
    *(vec3_words *)m[0] = *(vec3_words *)&snow_grid[cell->link[0]];
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[3]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[1]];
    break;
  case 9:
    *(vec3_words *)m[0] = *(vec3_words *)cell;
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[0]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[3]];
    break;
  case 14:
    *(vec3_words *)m[0] = *(vec3_words *)&snow_grid[cell->link[3]];
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[1]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[2]];
    break;
  case 15:
    m[0][0] = (snow_grid[cell->link[0]].x + snow_grid[cell->link[2]].x) * 0.5;
    m[0][1] = (snow_grid[cell->link[0]].y + snow_grid[cell->link[2]].y) * 0.5;
    m[0][2] = (snow_grid[cell->link[0]].z + snow_grid[cell->link[2]].z) * 0.5;
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[3]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[1]];
    break;
  case 13:
    *(vec3_words *)m[0] = *(vec3_words *)&snow_grid[cell->link[2]];
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[0]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[3]];
    break;
  case 6:
    *(vec3_words *)m[0] = *(vec3_words *)cell;
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[1]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[2]];
    break;
  case 7:
    *(vec3_words *)m[0] = *(vec3_words *)&snow_grid[cell->link[0]];
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[1]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[2]];
    break;
  case 5:
    *(vec3_words *)m[0] = *(vec3_words *)cell;
    *(vec3_words *)m[1] = *(vec3_words *)&snow_grid[cell->link[2]];
    *(vec3_words *)m[2] = *(vec3_words *)&snow_grid[cell->link[0]];
    break;
  default:
    return 0x7f00;
  }
  vec3_tri_normal(m[0], m[1], m[2], m[3]);
  return ((int)(m[3][0] * 127.0) & 0xffU) | ((int)(m[3][1] * 127.0) & 0xffU) << 8 | ((int)(m[3][2] * 127.0) & 0xffU) << 0x10;
}

void snow_ground_update(void)
{
  bool skip;
  e_mode_main mode;
  int sub;
  
  snow_ground_frame.frame = snow_ground_frame.frame + 1;
  snow_bodies_collect((int)snow_ground_frame.frame);
  skip = false;
  mode = mode_get_main();
  if ((mode == MODE_MAIN_GAME) &&
     ((sub = mode_get_sub(), sub == 4 || (sub = mode_get_sub(), sub == 5)))) {
    skip = true;
  }
  if (!skip) {
    snow_bodies_press_all();
  }
  skip = false;
  mode = mode_get_main();
  if (mode == MODE_MAIN_GAME) {
    sub = mode_get_sub();
    if ((((sub == 4) || (sub = mode_get_sub(), sub == 5)) ||
        (sub = mode_get_sub(), sub == 7)) ||
       ((sub = mode_get_sub(), sub == 8 || (sub = mode_get_sub(), sub == 9)))) {
      skip = true;
    }
    sub = mode_get_sub();
    if (sub == 8) {
      snow_sprays_reset();
    }
  }
  if (!skip) {
    snow_sprays_update();
  }
  snow_slump_apply();
  snow_ground_vertices_write_dirty();
  return;
}

void snow_bodies_collect(uint frame)
{
  vec3 p;
  vec3 q;
  vec3 d;
  int list[4];
  vec3 s;
  vec3 pt;
  vec3 nd;
  int i;
  snow_bone_mtx *mb;
  rob_rec *rob;
  snow_body_src *tbl;
  int nlist;
  snow_bone_mtx *mq;
  int k;
  int cnt;
  int n;
  float len;
  float rad;
  int *tab;

  nlist = 0;
  tab = &snow_bodies;
  *tab = 0;
  tbl = (snow_body_src *)&snow_body_joints;
  for (i = 0; i < 2; i++) {
    rob = rob_get((uchar)i);
    mq = (snow_bone_mtx *)rob->bone_mtx_prev;
    mb = (snow_bone_mtx *)rob->bone_mtx;
    for (k = 0; k < 5; k++) {
      snow_body_src *t = (snow_body_src *)(k * 16 + (int)tbl);
      p.x = mb[t->joint].t.x;
      p.y = mb[t->joint].t.y;
      p.z = mb[t->joint].t.z;
      q.x = mq[t->joint].t.x;
      q.y = mq[t->joint].t.y;
      q.z = mq[t->joint].t.z;
      d.x = p.x - q.x;
      d.y = p.y - q.y;
      d.z = p.z - q.z;
      ((snow_body_slot *)tab)[*tab].pos = p;
      ((snow_body_slot *)tab)[*tab].dir = d;
      ((snow_body_slot *)tab)[*tab].len = vec3_length((float *)&d);
      ((snow_body_slot *)tab)[*tab].rad = t->rad;
      ((snow_body_slot *)tab)[*tab].tag = t->tag;
      ((snow_body_slot *)tab)[*tab].flags |= 2;
      if ((rob->flags & 1) != 0) {
        ((snow_body_slot *)tab)[*tab].flags &= ~4;
        if (t->tag == 0 && nlist < 4) {
          list[nlist] = *tab;
          nlist++;
        }
      } else {
        ((snow_body_slot *)tab)[*tab].flags |= 4;
      }
      (*tab)++;
    }
  }
  rob = rob_get(frame & 1);
  if ((rob->flags & 1) != 0) {
    mb = (snow_bone_mtx *)rob->bone_mtx;
    if (((int)frame >> 1 & 1) == 0) {
      tbl = (snow_body_src *)&snow_body_points_a;
    } else {
      tbl = (snow_body_src *)&snow_body_points_b;
    }
    for (i = 0; i < 7; i++) {
      snow_body_src *t = &tbl[i];
      p.x = mb[t->joint].t.x;
      p.y = mb[t->joint].t.y;
      p.z = mb[t->joint].t.z;
      if (t->joint2 > -1) {
        p.x = (mb[t->joint2].t.x + p.x) * 0.5f;
        p.y = (mb[t->joint2].t.y + p.y) * 0.5f;
        p.z = (mb[t->joint2].t.z + p.z) * 0.5f;
      }
      ((snow_body_slot *)tab)[*tab].pos.x = p.x;
      ((snow_body_slot *)tab)[*tab].pos.y = p.y;
      ((snow_body_slot *)tab)[*tab].pos.z = p.z;
      ((snow_body_slot *)tab)[*tab].rad = tbl[i].rad;
      ((snow_body_slot *)tab)[*tab].tag = tbl[i].tag;
      ((snow_body_slot *)tab)[*tab].flags &= ~2;
      ((snow_body_slot *)tab)[*tab].flags &= ~4;
      (*tab)++;
    }
  }
  i = 0;
  cnt = 0;
  for (; i < nlist && cnt < 8; i++) {
    int *pi = (int *)(i * 4 + (int)list);
    snow_body_slot *e = (snow_body_slot *)(*pi * 108 + (int)tab);
    s = e->pos;
    nd.x = trail_neg(e->dir.x);
    nd.y = trail_neg(e->dir.y);
    nd.z = trail_neg(e->dir.z);
    len = signed_sqrt(nd.x * nd.x + nd.z * nd.z);
    rad = ((snow_body_slot *)tab)[*pi].rad;
    n = (int)(len / rad);
    for (k = 1; k < n && cnt < 8; k++) {
      float f = ((float)k * rad) / len;
      pt.x = f * nd.x + s.x;
      pt.y = f * nd.y + s.y;
      pt.z = f * nd.z + s.z;
      ((snow_body_slot *)tab)[*tab].pos = pt;
      ((snow_body_slot *)tab)[*tab].rad = rad;
      ((snow_body_slot *)tab)[*tab].tag = 0;
      ((snow_body_slot *)tab)[*tab].flags &= ~2;
      ((snow_body_slot *)tab)[*tab].flags &= ~4;
      (*tab)++;
      cnt++;
    }
  }
}

void snow_bodies_press_all(void)
{
  int i;
  char *p = (char *)&snow_bodies;
  for (i = 0; i < *(int *)p; i++) {
    if ((*(uint *)(p + i * 108 + 48) & 4) == 0) {
      snow_body_press((snow_body_rec *)(p + i * 108 + 4));
    }
  }
}

void snow_body_press(snow_body_rec *body)
{
  vec3 pos;
  vec3 a;
  vec3 b;
  int lx;
  int lz;
  int now;
  uint was;
  uint hit;
  float rr;
  float dx;
  float dz;
  float d;
  float h;
  float t;
  int i;
  int idx;
  int old;
  snow_cell_rec *c;
  snow_press_ofs_rec *tab;

  was = body->flags & 1;
  hit = 0;
  pos = body->pos;
  body->press = 0.0;
  body->press_total = body->press_total * 0.7;
  if (snow_world_to_cell(pos.x,pos.z,&lx,&lz) == 0) {
    now = snow_ground_frame.frame;
    rr = body->rad * body->rad;
    for (i = 0, tab = snow_press_offsets; i < 9; i++) {
      if ((uint)(lx + tab[i].dx) <= 100 && (uint)(tab[i].dz + lz) <= 100) {
        idx = lx + tab[i].dx + (tab[i].dz + lz) * 101;
        c = snow_grid;
        c += idx;
        dx = c->x - pos.x;
        dz = c->z - pos.z;
        d = dx * dx + dz * dz;
        if (d <= rr) {
          h = c->y - (pos.y - ball_fsqrt(rr - d));
          if (h > 0.0) {
            hit |= 1;
            body->press_total = body->press_total + h;
            old = snow_grid[idx].press_frame;
            snow_grid[idx].press_frame = now;
            if (now >= old + 6) {
              if (snow_grid[idx].links == 15) {
                snow_cell_neighbours_queue_slump(idx);
              }
              if (h > snow_press_max) {
                h = snow_press_max;
              }
              body->press = body->press + h;
              { int cb = (int)snow_grid; float *py = (float *)(idx * 0x2c + cb + 4); *py -= h; } /* ORIGINAL SHAPE (corroborated 2026-11-10: snow_grid_height_table_apply in this file reaches a cell's height through the same int base, b = (int)snow_grid, and loop.c and expr.c give the game's address only for an integer base): an int cell base; a pointer base does not match */
              snow_cell_mark_dirty(idx);
            }
          }
        }
      }
    }
  }
  if (body->flags & 2) {
    body->flags = (body->flags & ~1) | hit;
    body->spray = 0.0;
    body->state = -1;
    if (body->press_total > 1.0) {
      body->press_total = 1.0;
    }
    if (hit != was) {
      body->frames = 0;
      body->start.x = body->pos.x - body->dir.x;
      body->start.y = body->pos.y - body->dir.y;
      body->start.z = body->pos.z - body->dir.z;
      body->move_len = body->len;
      body->press_start = body->press;
      body->press_total_start = body->press_total;
    }
    else {
      if (body->frames < 300) {
        body->frames++;
      }
      if (body->frames < 3) {
        body->move.x = body->pos.x - body->start.x;
        body->move.y = body->pos.y - body->start.y;
        body->move.z = body->pos.z - body->start.z;
        body->move_len = vec3_length(&body->move.x);
        b = body->move;
        a = b;
        b.y = 0.0;
        vec3_normalize(&b.x);
        t = vec3_dot(&a.x,&b.x) / body->move_len;
        if (t > 1.0) {
          t = 1.0;
        }
        if (t < 0.0) {
          t = 0.0;
        }
        body->ratio = t;
        if (hit != 0) {
          body->state = 2;
          snow_body_state_enter(body);
        }
        else {
          body->state = 0;
          snow_body_state_leave(body);
        }
      }
      else if (hit != 0) {
        body->state = 3;
        snow_body_state_inside(body);
      }
      else {
        body->state = 1;
        snow_body_state_above(body);
      }
    }
  }
}

void snow_body_state_enter(snow_body_rec *body)
{
  float spray;

  spray = snow_body_spray_gain[body->tag][0];
  spray = spray * body->press_total;
  body->spray_enter = spray;
  if (body->frames == 1) {
    if (spray < 0.1) {
      spray = 0.0;
    }
    body->spray = spray;
  }
  return;
}

void snow_body_state_inside(snow_body_rec *body)
{
  typedef struct { float x; float y; float z; } vec;
  float len;
  float t;
  float k;
  float r;
  float s;
  vec a;
  vec b;
  
  b = *(vec *)&body->dir;
  a = b;
  b.y = 0.0;
  len = vec3_length(&b.x);
  vec3_normalize(&b.x);
  t = vec3_dot(&a.x,&b.x) / body->len;
  if (t > 1.0) {
    t = 1.0;
  }
  if (t < 0.0) {
    t = 0.0;
  }
  body->ratio = t;
  body->curve = t = snow_spray_curve_inside[(int)(t * 127.0)];
  s = body->press;
  len = len - 0.1;
  if (len < 0.0) {
    len = 0.0;
  }
  if (len > 0.1) {
    len = 0.1;
  }
  if (s > 0.1) {
    s = 0.1;
  }
  k = snow_body_spray_gain[body->tag][1];
  r = s * len * 100.0 * t * k;
  if (r > 1.0) {
    r = 1.0;
  }
  body->spray = r;
}

void snow_body_state_above(snow_body_rec *body)
{
  float a = snow_body_spray_gain[body->tag][3];
  a = a * body->press_total;
  body->spray = a;
}

void snow_body_state_leave(snow_body_rec *body)
{
  float move;
  float spray;
  float curve;
  float k;
  
  body->curve = curve = snow_spray_curve_leave[(int)(body->ratio * 127.0)];
  move = body->move_len - 0.15;
  spray = body->press_total_start - 0.2;
  k = snow_body_spray_gain[body->tag][2];
  if (move < 0.0) {
    move = 0.0;
  }
  if (0.3 < move) {
    move = 0.3;
  }
  if (spray < 0.0) {
    spray = 0.0;
  }
  if (0.3 < spray) {
    spray = 0.3;
  }
  spray = (spray * 2.31 + move) * curve * k;
  body->spray_leave = spray;
  if (body->frames == 2) {
    if (spray < 0.2) {
      spray = 0.0;
    }
    body->spray = spray;
  }
}

void snow_slump_apply(void)
{
  ushort *p;
  uint i;
  float c, l, r, u, d;

  if (snow_slump_step > 0.0) {
    p = snow_slump_cells;
    while ((i = *p++) != 0xffff) {
      snow_cell_rec *t = snow_grid;
      int k;
      c = t[i].y;
      k = i - 1; l = t[k].y;
      k = i + 1; r = t[k].y;
      k = i - 101; u = t[k].y;
      k = i + 101; d = t[k].y;
      if (r > l) l = r;
      if (d > u) u = d;
      if (l > u) l = u;
      if (c - l > snow_slump_step) {
        snow_grid[i].y = l + snow_slump_step;
        snow_cell_mark_dirty(i);
      }
      snow_grid[i].state &= 0xfffd;
    }
  }
}

void snow_dirty_lists_clear(void)
{
  snow_dirty_count = 0;
  snow_dirty_cells[0] = 0xffff;
  snow_slump_count = 0;
  snow_slump_cells[0] = 0xffff;
  return;
}

void snow_cell_mark_dirty(int idx)
{
  int i;
  int j;
  
  if (snow_dirty_count < 0x27da) {
    if (!(snow_grid[idx].state & 1)) {
      snow_grid[idx].state |= 1;
      snow_dirty_cells[snow_dirty_count] = (short)idx;
      snow_dirty_count = snow_dirty_count + 1;
    }
    for (i = 0; i < 4; i++) {
      j = snow_grid[idx].link[i];
      if (j >= 0) {
        if (!(snow_grid[j].state & 1)) {
          short *list = (short *)snow_dirty_cells;
          snow_grid[j].state |= 1;
          list[snow_dirty_count] = (short)j;
          snow_dirty_count = snow_dirty_count + 1;
        }
      }
    }
    snow_dirty_cells[snow_dirty_count] = 0xffff;
  }
}

void snow_cell_neighbours_queue_slump(int idx)
{
  int k;
  snow_cell_rec *p;
  char *q;
  int i, j;
  ushort *s;
  for (i = 0; i < 4; i++) {
    k = idx + snow_cell_neighbour_steps[i];
    q = (char *)snow_grid; p = (snow_cell_rec *)(k * 0x2c + q);
    if ((*(uint *)&p->links & 0xffff00ff) == 0xf) {
      p->state |= 2;
      s = snow_slump_cells; s[snow_slump_count] = (short)k; snow_slump_count = snow_slump_count + 1;
    }
  }
  snow_slump_cells[snow_slump_count] = 0xffff;
}

void snow_ground_draw(void)
{
  light_set_send(0,2);
  if (snow_ground_model != 0) {
    mtx_push(0);
    model_draw(snow_ground_model);
    mtx_pop(1);
  }
  snow_sprays_draw();
  light_set_send(0,0);
  return;
}

int snow_world_to_cell(float x, float z, int *cell_x, int *cell_z)
{
  float t;
  
  t = snow_cell_step * 50.0;
  x = x + t;
  z = z + t;
  x = x * snow_cell_step_inv;
  z = z * snow_cell_step_inv;
  if ((int)(x * 10.0) % 10 > 5) x = x + 1.0;
  if ((int)(z * 10.0) % 10 > 5) z = z + 1.0;
  if ((int)x < 0 || (int)z < 0) return -1;
  if ((int)x > 100 || (int)z > 100) return -1;
  *cell_x = (int)x;
  *cell_z = (int)z;
  return 0;
}

void snow_sprays_reset(void)
{
  int i;
  snow_spray_rec *tab;
  
  snow_spray_lists[0][0] = 0xff;
  snow_spray_lists[1][0] = 0xff;
  i = 0;
  snow_spray_cursor = 0;
  snow_spray_list_len = 0;
  snow_spray_list_new = 0;
  snow_spray_list_live = 0;
  tab = (snow_spray_rec *)&snow_sprays[0][0];
  for (; i < 0x50; i++) {
    tab[i].kind = 0xff;
  }
}

void snow_sprays_update(void)
{
  typedef struct { int head; char body[0x2c]; uint flags; char r[56]; } ent;
  ent *base = (ent *)&snow_bodies;
  int i;
  snow_spray_list_new = snow_spray_list_new ^ 1;
  snow_spray_list_live = snow_spray_list_new + 1 & 1;
  snow_spray_cursor = 0;
  for (i = 0; i < 10; i++) {
    if ((base[i].flags & 4) == 0) {
      snow_body_sprays_spawn((snow_body_rec *)base[i].body);
    }
  }
  snow_sprays_move();
}

snow_spray_rec * snow_spray_alloc(void)
{
  typedef struct { char pad[32]; unsigned char f; char r[23]; } ent;
  unsigned char c;
  char (*a)[0x51];

  while (snow_spray_cursor < 0x50 && snow_spray_list_len < 0x50) {
    ent *e = (ent *)&snow_sheet_count + 1;
    if (e[snow_spray_cursor].f == 0xff) {
      c = snow_spray_list_new;
      a = (char (*)[0x51])&snow_spray_lists[0][0];
      a[c][snow_spray_list_len] = snow_spray_cursor;
      snow_spray_list_len++;
      a[c][snow_spray_list_len] = 0xff;
      snow_spray_cursor++;
      return (snow_spray_rec *)&((ent *)&snow_sheet_count)[snow_spray_cursor];
    }
    snow_spray_cursor++;
  }
  return 0;
}

void snow_sprays_move(void)
{
  uchar id;
  uchar *src;
  uchar *dst;
  snow_spray_rec *e;
  float y;

  snow_spray_list_len = 0;
  src = &snow_spray_lists[0][0] + (uint)snow_spray_list_new * 0x51;
  dst = &snow_spray_lists[0][0] + (uint)snow_spray_list_live * 0x51;
  while ((id = *src++) != 0xff) {
    e = (snow_spray_rec *)&snow_sprays[0][0] + id;
    e->vel.y = e->vel.y + e->gravity;
    if (e->vel.y < e->vel_y_min) {
      e->vel.y = e->vel_y_min;
    }
    e->pos[0] = e->pos[0] + e->vel.x;
    y = e->pos[1] + e->vel.y;
    e->pos[1] = y;
    e->pos[2] = e->pos[2] + e->vel.z;
    if (e->kind < 4) {
      e->life--;
      if ((y < snow_ground_y) || (e->life < 0)) {
        e->kind = 0xff;
      }
    }
    if (e->kind != 0xff) {
      dst[snow_spray_list_len++] = id;
    }
  }
  dst[snow_spray_list_len] = 0xff;
}

void snow_body_sprays_spawn(snow_body_rec *body)
{
  switch(body->state) {
  case 2:
    if (body->spray > 0.0) snow_sprays_spawn_enter(body);
    break;
  case 3:
    if (body->spray > 0.0) snow_sprays_spawn_inside(body);
    break;
  case 0:
    if (body->spray > 0.0) snow_sprays_spawn_leave(body);
    break;
  case 1:
    if (body->spray > 0.0) snow_sprays_spawn_above(body);
    break;
  }
  return;
}

void snow_vec_clamp_len_up(float *v, float len, float max_len)
{
  float f;
  
  if (max_len < len) {
    f = max_len / len;
    v[0] = v[0] * f;
    v[1] = v[1] * f;
    v[2] = f * v[2];
  }
  if (v[1] < 0.0) {
    v[1] = v[1] * -1.0;
  }
}

void snow_sprays_spawn_leave(snow_body_rec *body)
{
  typedef struct { float x; float y; float z; } vec_t;
  vec_t v;
  int n;
  int i;
  snow_spray_rec *e;
  int r;
  float f;
  int t;
  float *px;
  float *py;
  float *pz;

  if ((snow_spray_kinds & 1) != 0) {
    v = *(vec_t *)&body->move;
    snow_vec_clamp_len_up(&v.x, body->move_len, 0.05f);
    n = (int)(body->spray * 10.0f);
    for (i = 0; i < n; i++) {
      px = &body->start.x;
      py = &body->start.y;
      pz = &body->start.z;
      e = snow_spray_alloc();
      if (e == 0) return;
      e->kind = 0;
      r = stage_fx_rand_next();
      f = (float)(r & 0xff) * 0.0019607f;
      e->variant = r % 3;
      t = (r & 0xf) + 0x14;
      e->life = t;
      if (t > 0x1f) e->life = 0x1f;
      e->angle = r;
      e->pos[0] = f * body->move.x + *px;
      e->pos[1] = f * body->move.y + *py;
      e->pos[2] = f * body->move.z + *pz;
      f = (float)(r >> 2 & 0xff) * 0.0019607f + 0.5f;
      e->vel.x = v.x * f;
      f = (float)(r >> 4 & 0xff) * 0.0019607f + 0.5f;
      e->vel.y = v.y * f;
      f = (float)(r >> 6 & 0xff) * 0.0019607f + 0.5f;
      e->vel.z = v.z * f;
      e->gravity = -0.0018f;
      e->vel_y_min = -0.006f;
    }
  }
}

void snow_sprays_spawn_above(snow_body_rec *body)
{
  vec3 zero = { 0.0, 0.0, 0.0 };
  snow_spray_rec *e;
  int r;
  float f;
  int n;
  int i;
  if ((snow_spray_kinds & 2) != 0 && body->len >= 0.1) {
    n = (int)(body->press_total * 100.0);
    if (n > 4) n = 4;
    for (i = 0; i < n; i++) {
      e = snow_spray_alloc();
      if (e == 0) return;
      e->kind = 1;
      r = stage_fx_rand_next();
      f = (float)r * 1.5e-05;
      e->pos[0] = body->pos.x - f * body->dir.x;
      e->pos[1] = body->pos.y - f * body->dir.y;
      e->pos[2] = body->pos.z - f * body->dir.z;
      e->vel = zero;
      e->gravity = e->vel_y_min = 0.0;
      e->life = (r & 15) + 16;
      if (e->life > 31) e->life = 31;
      e->angle = r;
      e->variant = r & 1;
    }
  }
}

void snow_sprays_spawn_enter(snow_body_rec *body)
{
  typedef struct { int w[3]; } v3;
  float v[3];
  int n;
  snow_spray_rec *e;
  int r;
  int i;
  float t;

  if ((snow_spray_kinds & 2) != 0) {
    *(v3 *)v = *(v3 *)&body->move;
    v[1] = v[1] * -1.0;
    snow_vec_clamp_len_up(v,body->move_len,0.05);
    n = (int)(body->spray * 30.0);
    for (i = 0; i < n; i++) {
      e = snow_spray_alloc();
      if (e == 0) return;
      e->kind = 2;
      r = stage_fx_rand_next();
      e->variant = r % 3;
      e->life = (r >> 8 & 15) + 20;
      if (e->life > 31) e->life = 31;
      e->angle = r;
      t = (float)(r & 0xff) * 0.0019607 - 0.25;
      e->pos[0] = body->pos.x + t;
      t = (float)(r >> 2 & 0xff) * 0.0019607 - 0.25;
      e->pos[2] = body->pos.z + t;
      e->pos[1] = snow_ground_y + 0.05;
      t = (float)(r >> 2 & 0xff) * 0.0019607 + 0.8;
      e->vel.x = v[0] * t;
      t = (float)(r >> 4 & 0xff) * 0.0019607 + 0.1;
      e->vel.y = v[1] * t;
      t = (float)(r >> 6 & 0xff) * 0.0019607 + 0.8;
      e->vel.z = v[2] * t;
      e->gravity = -0.0018;
      e->vel_y_min = -0.006;
    }
  }
}

void snow_sprays_spawn_inside(snow_body_rec *body)
{
  vec3 v;
  snow_spray_rec *e;
  int r;
  float f;
  int n;
  int i;
  if ((snow_spray_kinds & 3) != 0) {
    v = body->dir;
    snow_vec_clamp_len_up((float *)&v,body->len,0.02);
    n = (int)(body->spray * 7.0);
    for (i = 0; i < n; i++) {
      e = snow_spray_alloc();
      if (e == 0) return;
      e->kind = 3;
      r = stage_fx_rand_next();
      f = (float)(uchar)r * 0.0039215;
      e->variant = r % 3;
      e->life = (r & 15) + 20;
      if (e->life > 31) e->life = 31;
      e->angle = r;
      e->pos[0] = body->pos.x - f * body->dir.x;
      e->pos[1] = body->pos.y - f * body->dir.y - 0.05;
      e->pos[2] = body->pos.z - f * body->dir.z;
      f = (float)((r >> 2) & 0xff) * 0.0031372 + 0.2;
      e->vel.x = v.x * f;
      f = (float)((r >> 4) & 0xff) * 0.0031372 + 0.2;
      e->vel.y = v.y * f;
      f = (float)((r >> 6) & 0xff) * 0.0032372 + 0.2;
      e->vel.z = v.z * f;
      e->gravity = -0.0018;
      e->vel_y_min = -0.006;
    }
  }
}

void snow_sprays_draw(void)
{
  byte b;
  byte *p;
  p = &snow_spray_lists[0][0] + (uint)snow_spray_list_live * 0x51;
  while ((b = *p++) != 0xff) {
    int (*t)[32];
    snow_spray_rec *e = (snow_spray_rec *)snow_sprays[b];
    mtx_push(0);
    if (e->kind < 4) {
      mtx_translate_by_vec3(e->pos);
      mtx_rot_y_s16(e->angle);
      t = snow_spray_model_ids;
      obj_draw(t[(ushort)e->variant][31 - (e->life & 0x1f)]);
    }
    mtx_pop(1);
  }
}

void snow_spray_models_bits_clear(void)
{
  compiled_model_rec *m;
  uint *p;
  uint c;
  int k, n, j, i;
  int *tab;
  
  i = 0;
  tab = &snow_spray_model_ids[0][0];
  for (; i < 2; i++) {
    for (j = 0; j < 0x20; j++) {
      m = obj_model_get(tab[i * 0x20 + j]);
      if (m != 0) {
        n = m->vertex_count;
        for (k = 0, p = (uint *)(m + 1); k < n; ) {
          if (*p == 0x08000500) {
            ((struct { char pad[64]; uint a; char x[24]; uint n; } *)p)->a &= 0xffffff3f;
            c = ((struct { char pad[64]; uint a; char x[24]; uint n; } *)p)->n;
            k += c;
            p = (uint *)((char *)p + c * 24 + 0x60);
          }
          else {
            *p &= 0xffffff3f;
            c = p[7];
            k += c;
            p = (uint *)((char *)p + c * 24 + 0x20);
          }
        }
      }
    }
  }
}

void snow_cas_draw_tail(void)
{
  return;
}

void snow_djo_big_flakes_init(void)
{
  snow_flake *p;
  int i;
  int r; int a;
  p = snow_flakes;
  for (i = 0; i < 6; i++, p++) {
    r = lcg_next15();
    a = r * 2;
    p->pos.x = cos_ang16(a) * 35.0;
    p->pos.z = sin_ang16(a) * 35.0;
    p->pos.y = snow_ground_y + (float)(r & 0xff) * 0.0392;
  }
}

void snow_djo_fixed_sheets_init(void)
{
  uint a;
  uint b;
  snow_sheet *d;
  int i;
  float *s;

  snow_djo_fixed_sheet_count = 0;
  d = snow_sheets + snow_sheet_count;
  s = snow_djo_fixed_sheet_spots;
  for (i = snow_sheet_count; i < 0x32; i++, d++) {
    if (*s < 0.0) break;
    a = lcg_next15();
    b = lcg_next15();
    d->x = ((float)(int)a * 3.051851e-05 + *s) - 13.0;
    d->z = (float)(int)b * 4.5777764e-05 - 22.0;
    d->r = 0.0;
    d->spd = s[1];
    *(int *)&d->place = ((int *)s)[2];
    d->on = 1;
    snow_djo_fixed_sheet_count = snow_djo_fixed_sheet_count + 1;
    s = s + 3;
  }
}

void snow_djo_init(void)
{
  stage_fx_rand_reset();
  snow_flakes_sheets_init(0xffffffff,snow_djo_sheet_spots);
  snow_djo_big_flakes_init();
  snow_djo_fixed_sheets_init();
  return;
}

void snow_djo_fixed_sheets_update(void)
{
  int i;
  int j;
  int k;
  snow_sheet *p;
  float d;
  float v;

  p = snow_sheets + snow_sheet_count;
  for (i = 0; i < snow_djo_fixed_sheet_count; i++, p++) {
    if (p->on != '\0') {
      p->spin = p->spin + 0x10;
      j = 0;
      d = p->spd;

      do {
        v = p->y[j] - d;
        p->y[j] = v;
        if (v < snow_ground_y - 2.5) {
          k = j - 1;
          if (k < 0) k = 2;
          p->y[j] = p->y[k] + 5.0;
        }
        j++;
      } while (j < 3);
    }
  }
}

void snow_djo_sheets_draw(void)
{
  snow_sheet *p;
  float s;
  int i;
  int j;

  if (snow_djo_zone_deep != '\0') s = 1.0;
  else s = 1.8;
  p = snow_sheets;
  for (i = 0; i < snow_sheet_count; i++, p++) {
    if (p->on != '\0') {
      for (j = 0; j < 3; j++) {
        mtx_push(0);
        mtx_translate(p->x,p->y[j],p->z);
        mtx_rot_y_s16(camera.yaw + p->spin);
        mtx_scale_xyz(s,s,s);
        obj_draw(0x1e0001);
        mtx_pop(1);
      }
    }
  }
}

void snow_djo_fixed_sheets_draw(void)
{
  snow_sheet *p;
  int i;
  int j;

  p = snow_sheets + snow_sheet_count;
  for (i = 0; i < snow_djo_fixed_sheet_count; i++, p++) {
    if (p->on != '\0') {
      for (j = 0; j < 3; j++) {
        mtx_push(0);
        mtx_translate(p->x,p->y[j],p->z);
        mtx_rot_y_s16(p->spin);
        mtx_scale_xyz(1.3,1.3,1.3);
        obj_draw(0x1e0001);
        mtx_pop(1);
      }
    }
  }
}

void snow_djo_step(void)
{
  float x;
  float z;
  float d;
  float r;
  float y;
  int i;
  uint rnd;
  int a;
  snow_flake *p;
  float k = 0.032399997;

  if (camera.eye.z < -10.0) {
    snow_djo_zone_far = 1;
    if (camera.eye.z < -25.0) {
      snow_djo_zone_deep = 1;
    }
    else {
      snow_djo_zone_deep = 0;
    }
  }
  else {
    snow_djo_zone_far = 0;
    snow_djo_zone_deep = 0;
  }
  x = camera.eye.x;
  z = camera.eye.z;
  d = x * (camera.target.x - x) + (camera.target.z - z) * z;
  snow_yaw_delta = camera.yaw - snow_prev_yaw;
  if (d > 0.0) {
    r = 35.0 - fsqrt_asm(x * x + z * z);
  }
  else {
    r = fsqrt_asm(x * x + z * z) + 35.0;
  }
  p = snow_flakes;
  y = camera.eye.y + 6.0;
  for (i = 0; i < 6; i++, p++) {
    rnd = stage_fx_rand_next();
    p->pos.y = p->pos.y - (p->weight * 0.0016 + k);
    p->spin = p->spin - snow_yaw_delta + 0x10;
    if (p->pos.y < snow_ground_y + 2.0) {
      a = (short)(camera.yaw + (rnd & 0x1fff) + 0x7000);
      p->pos.x = camera.eye.x + sin_ang16(a) * r;
      p->pos.z = cos_ang16(a) * r + camera.eye.z;
      p->pos.y = y;
    }
  }
  if (snow_djo_zone_deep != 0) {
    r = fsqrt_asm(x * x + z * z) * 0.16;
    if (r > 1.0) {
      r = 1.0;
    }
  }
  else if (d > 0.0) {
    r = 20.0 - fsqrt_asm(x * x + z * z);
  }
  else {
    r = fsqrt_asm(x * x + z * z) + 20.0;
  }
  snow_sheets_update(r);
  snow_djo_fixed_sheets_update();
}

void snow_djo_update(void)
{
  snow_djo_step();
  return;
}

void snow_djo_big_flakes_draw(void)
{
  snow_flake *p;
  int i;
  
  p = snow_flakes;
  for (i = 0; i < 6; i++, p++) {
    mtx_push(0);
    mtx_translate(p->pos.x,p->pos.y,p->pos.z);
    mtx_rot_y_s16(camera.yaw + p->spin);
    mtx_scale_xyz(2.3,2.3,2.3);
    obj_draw(0x1e0001);
    mtx_pop(1);
  }
}

void snow_djo_draw(void)
{
  if (((snow_fx_flags & 1) != 0) && (snow_djo_zone_far == '\0')) {
    snow_djo_big_flakes_draw();
  }
  if (((snow_fx_flags & 2) != 0) && ((snow_djo_zone_far != '\x01' || (snow_djo_zone_deep != '\0')))) {
    snow_djo_sheets_draw();
  }
  if (((snow_fx_flags & 8) != 0) && (snow_djo_zone_far == '\0')) {
    snow_djo_fixed_sheets_draw();
  }
  return;
}

void snow_djo_free(void)
{
  return;
}
