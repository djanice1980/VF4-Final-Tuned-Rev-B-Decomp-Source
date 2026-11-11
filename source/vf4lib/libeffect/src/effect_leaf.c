/*
 * vf4lib/libeffect/src/effect_leaf.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int leaf_cfg_gdn[23]; /* 0x0c1a718c */
extern int leaf_cfg_noh[23]; /* 0x0c1a71e8 */
extern int leaf_cfg_other[23]; /* 0x0c1a7244 */
extern leaf_cfg_rec *leaf_cfg; /* 0x0c2b0aa8 */
extern int leaf_draw_frame; /* 0x0c2b0aac */
extern int leaf_warmup_frames; /* 0x0c2b0ab0 */
extern float leaf_kick_tuning; /* 0x0c2b0ab4 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern float leaf_fly_shapes[64][3][3]; /* 0x0c397b8c */
extern float leaf_ground_shapes[72]; /* 0x0c39848c */
extern leaf_rec *leaf_records; /* 0x0c48c6d0 */
extern int leaf_foot_frame; /* 0x0c48c6d4 */
extern char leaf_sources[180]; /* 0x0c48c6d8 */
extern int leaf_active_count; /* 0x0c48c78c */
extern uchar *leaf_ground_model; /* 0x0c48c794 */
extern short leaf_active_lists[1002]; /* 0x0c48c7a8 */
extern uchar leaf_active_side; /* 0x0c48cf7c */
extern float leaf_times[20]; /* 0x0c48cf80 */
extern char leaf_on; /* 0x0c48cfd0 */
extern ushort leaf_grid[10240]; /* 0x0c48cfd2 */
extern int leaf_rescan_pos; /* 0x0c491fd4 */
extern float leaf_foot_pos[18]; /* 0x0c491fd8 */
extern camera_state camera; /* 0x0c370ce0 */
extern uchar *leaf_fly_models[2]; /* 0x0c48c798 */
extern uchar *leaf_shadow_models[2]; /* 0x0c48c7a0 */

void leaf_timer_start(float *t0)
{
  int usec;
  
  usec = stopwatch_usec();
  *t0 = (float)usec;
  return;
}

void leaf_timer_lap(float *t0, float *last, float *max)
{
  float t;
  float dt;
  t = (float)(int)stopwatch_usec();
  if (leaf_warmup_frames == 0) {
    dt = t - *t0;
    *last = dt;
    if (*max < dt) {
      *max = dt;
    }
  }
  *t0 = t;
  return;
}

uchar leaf_enabled_update(void)
{
  int result;
  
  if ((((stage_cur_no == 0x15) && (play_style == '\x01')) && (result = msnm_active(), result == 0))
     && ((result = net_is_on(), result == 0 || (result = net_ready_and_idle(), result == 0)))) {
    leaf_on = 0;
  }
  else {
    leaf_on = 1;
  }
  return leaf_on;
}

uchar leaf_enabled(void)
{
  return leaf_on;
}

void leaf_init(void)
{
  typedef struct { int w[23]; } blk;
  typedef struct { int head; int type; int id; float radius; float strength; char pad14[8]; vec3 v[3]; } slot;
  vec3 zero = { 0.0, 0.0, 0.0 };
  leaf_rec *p;
  uint r;
  int i;
  uchar k;
  float a;
  float b;
  slot *q;
  float *h;
  leaf_cfg_rec *cf;
  short *hs;

  if (leaf_enabled_update() != 0) {
    leaf_records = (leaf_rec *)effect_malloc(560000);
    if (stage_cur_no == 0x11) {
      *(blk *)leaf_cfg = *(blk *)&leaf_cfg_gdn;
      k = 0x15;
    }
    else if (stage_cur_no == 0x15) {
      *(blk *)leaf_cfg = *(blk *)&leaf_cfg_noh;
      k = 0x1c;
    }
    else {
      *(blk *)leaf_cfg = *(blk *)&leaf_cfg_other;
      k = 0xb;
    }
    leaf_cfg->grid_half_width = leaf_cfg->half_width;
    leaf_cfg->cells_per_unit = 2.0;
    leaf_cfg->grid_cells = (int)((leaf_cfg->half_width + leaf_cfg->half_width) * leaf_cfg->cells_per_unit);
    leaf_cfg->spawn_timer = 0;
    leaf_cfg->in_use_count = leaf_cfg->ground_count;
    light_set_load(2,k);
    leaf_grid_clear();
    leaf_ground_shapes_build();
    leaf_fly_shapes_build();
    stage_fx_rand_reset();
    p = leaf_records;
    for (i = 0; i < leaf_cfg->leaf_count; i++, p++) {
      p->mode = 0;
      p->id = i;
      p->source = -1;
      p->cell = -1;
      p->shape = i & 3;
      r = stage_fx_rand_next();
      p->vel = zero;
      h = &p->floor_y;
      *h = leaf_cfg->floor_y + (float)(i % 20) * 0.0025;
      cf = leaf_cfg;
      if (i >= cf->ground_count) {
        a = (float)(r & 0xff) * 0.003921 * cf->ring_radius;
        p->pos[0] = cos_ang16(r & 0xffff) * a;
        p->pos[2] = sin_ang16(r & 0xffff) * a;
        p->pos[1] = *h - 5.0;
        p->ang[0] = r * 2;
        p->ang[1] = r << 2;
        p->ang[2] = r << 3;
      }
      else {
        float sc;
        a = (float)(r & 0xff) * (sc = 0.007843) - 1.0;
        b = (float)(((int)r >> 8) & 0xff) * sc - 1.0;
        p->pos[0] = leaf_cfg->grid_half_width * a;
        p->pos[2] = b * leaf_cfg->grid_half_width;
        p->pos[1] = p->floor_y;
        p->ang[0] = 0x4000;
        if (r & 1) {
          p->ang[0] = 0xc000;
        }
        p->ang[1] = r * 2;
        p->ang[2] = r << 2;
        leaf_grid_update(p);
      }
      p->spin_max = (float)(((int)r >> 4) & 0xff) * 0.35 + 300.0;
      p->spin = p->spin_max;
    }
    leaf_active_count = 0;
    leaf_active_count_prev = 0;
    leaf_active_side = 0;
    hs = leaf_active_lists;
    hs[0] = 0xffff;
    hs[0x1f5] = 0xffff;
    leaf_ground_model_build();
    leaf_fly_model_build();
    leaf_shadow_model_build();
    leaf_foot_frame = 0xffffff4c;
    for (i = 0; i < 3; i++) {
      char *sb = (char *)&leaf_foot_frame;
      q = (slot *)(sb + i * 60);
      q->type = 0;
      q->id = 0;
      q->strength = 0.0;
      q->radius = 0.0;
      q->v[0] = zero;
      q->v[1] = zero;
      q->v[2] = zero;
    }
    for (i = 0; i < 0x14; i++) {
      leaf_times[i] = 0.0;
    }
  }
}

void leaf_ground_model_build(void)
{
  float v[6][3];
  int *src;
  int six;
  int four;
  int i;
  int j;
  int k;
  leaf_rec *leaf;
  float *t;

  src = (int *)obj_model_get(leaf_cfg->ground_model_id);
  if (src == 0) {
    leaf_ground_model = (uchar *)0x0;
    return;
  }
  six = 6;
  four = 4;
  leaf_ground_model = (uchar *)elan_heap_alloc(leaf_cfg->leaf_count * six * 24 + 0xc0);
  if (leaf_ground_model == (uchar *)0x0) return;
  for (i = 0; i < 0x30; i++) {
    ((int *)leaf_ground_model)[i] = src[i];
  }
  *(float *)(leaf_ground_model + 0x14) = 20.0;
  *(int *)(leaf_ground_model + 0x18) = leaf_cfg->leaf_count * six * 24 + 0xc0;
  *(int *)(leaf_ground_model + 0x20) = leaf_cfg->leaf_count * four;
  *(int *)(leaf_ground_model + 0x24) = leaf_cfg->leaf_count * six;
  *(int *)(leaf_ground_model + 0x28) = 1;
  *(int *)(leaf_ground_model + 0x2c) = 1;
  *(int *)(leaf_ground_model + 0x44) = leaf_cfg->leaf_count * six * 24 + 0x60;
  *(int *)(leaf_ground_model + 0xbc) = leaf_cfg->leaf_count * six;
  leaf = leaf_records;
  mtx_push(0);
  i = 0;
  k = 0x30;
  for (; i < leaf_cfg->leaf_count; i++, leaf++) {
    t = leaf_ground_uvs[leaf->shape];
    leaf_ground_quad_get(leaf, v);
    for (j = 0; j < six; j++) {
      ((int *)leaf_ground_model)[k] = leaf_ground_vtx_words[j];
      ((uint *)leaf_ground_model)[k] &= 0xff000000;
      ((uint *)leaf_ground_model)[k] |= leaf->packed_nrm;
      ((float *)leaf_ground_model)[k + 1] = v[j][0];
      ((float *)leaf_ground_model)[k + 2] = v[j][1];
      ((float *)leaf_ground_model)[k + 3] = v[j][2];
      ((float *)leaf_ground_model)[k + 4] = t[j * 2];
      ((float *)leaf_ground_model)[k + 5] = t[j * 2 + 1];
      k += 6;
    }
  }
  mtx_pop(1);
}

void leaf_fly_model_build(void)
{
  typedef struct { float u; float v; } uvt;
  int *src;
  int *b;
  int i;
  int j;
  int k;
  int *a;
  int nv;
  int one;
  int size;
  leaf_rec *leaf;
  float v[3][3];
  float *uv;

  src = (int *)obj_model_get(leaf_cfg->fly_model_id);
  if (src == 0) {
    leaf_fly_models[0] = leaf_fly_models[1] = 0;
    return;
  }
  nv = 3;
  one = 1;
  leaf_fly_models[0] = (uchar *)elan_heap_alloc(leaf_cfg->fly_max * nv * 24 + 0xc0);
  leaf_fly_models[1] = (uchar *)elan_heap_alloc(leaf_cfg->fly_max * nv * 24 + 0xc0);
  if (leaf_fly_models[0] == 0 || leaf_fly_models[1] == 0) return;
  a = (int *)leaf_fly_models[0];
  b = (int *)leaf_fly_models[1];
  for (i = 0; i < 48; i++) a[i] = src[i];
  ((float *)a)[5] = 20.0f;
  a[6] = leaf_cfg->fly_max * nv * 24 + 0xc0;
  a[8] = leaf_cfg->fly_max * one;
  a[9] = leaf_cfg->fly_max * nv;
  a[10] = 1;
  a[11] = 1;
  a[19] = leaf_cfg->fly_max * nv * 24 + 0x60;
  a[47] = leaf_cfg->fly_max * nv;
  leaf = leaf_records;
  mtx_push(0);
  for (i = 0, k = 48; i < leaf_cfg->fly_max; i++, leaf++) {
    uvt *t = (uvt *)leaf_fly_uvs;
    leaf_fly_tri_get(leaf, v);
    for (j = 0; j < nv; k += 6, j++) {
      a[k] = leaf_fly_vtx_words[j];
      ((float *)a)[k + 1] = v[j][0];
      ((float *)a)[k + 2] = v[j][1];
      ((float *)a)[k + 3] = v[j][2];
      ((float *)a)[k + 4] = t[j].u;
      ((float *)a)[k + 5] = t[j].v;
    }
  }
  mtx_pop(1);
  size = a[6];
  for (i = 0; i < size / 4; i++) *b++ = *a++;
}

void leaf_shadow_model_build(void)
{
  typedef struct { float u; float v; } uv_pair;
  typedef struct { int idx; float x; float y; float z; float u; float v; } vtx_entry;
  int *src;
  int *buf;
  int nv;
  int i;
  int k;
  int *dst;
  int j;
  leaf_rec *leaf;
  int size;
  int one;
  vtx_entry *p;
  uv_pair *uv;
  float v[3][3];

  src = (int *)obj_model_get(leaf_cfg->shadow_model_id);
  if (src == 0) {
    leaf_shadow_models[1] = 0;
    leaf_shadow_models[0] = 0;
    return;
  }
  nv = 3;
  one = 1;
  leaf_shadow_models[0] = (uchar *)elan_heap_alloc(leaf_cfg->fly_max * nv * 24 + 0xc0);
  leaf_shadow_models[1] = (uchar *)elan_heap_alloc(leaf_cfg->fly_max * nv * 24 + 0xc0);
  if (leaf_shadow_models[0] == 0 || leaf_shadow_models[1] == 0) {
    return;
  }
  dst = (int *)leaf_shadow_models[0];
  buf = (int *)leaf_shadow_models[1];
  for (i = 0; i < 0x30; i++) {
    dst[i] = src[i];
  }
  *(float *)(dst + 5) = 20.0;
  dst[6] = nv * leaf_cfg->fly_max * 24 + 0xc0;
  dst[8] = leaf_cfg->fly_max * one;
  dst[9] = nv * leaf_cfg->fly_max;
  dst[10] = 1;
  dst[11] = 1;
  dst[0x1b] = 0x40808080;
  dst[0x1d] = 0x40808080;
  dst[0x13] = nv * leaf_cfg->fly_max * 24 + 0x60;
  dst[0x2f] = nv * leaf_cfg->fly_max;
  leaf = leaf_records;
  mtx_push(0);
  for (i = 0, k = 0x30; i < leaf_cfg->fly_max; i++, leaf++) {
    leaf_fly_tri_get(leaf,v);
    uv = (uv_pair *)leaf_shadow_uvs;
    for (j = 0; j < nv; j++) {
      p = (vtx_entry *)(k * 4 + (int)dst);
      p->idx = leaf_fly_vtx_words[j];
      p->x = v[j][0];
      p->y = v[j][1];
      p->z = v[j][2];
      p->u = uv[j].u;
      p->v = uv[j].v;
      k += 6;
    }
  }
  mtx_pop(1);
  size = dst[6];
  for (i = 0; i < size / 4; i++) {
    *buf++ = *dst++;
  }
}

void leaf_grid_clear(void)
{
  int i;
  int j;
  ushort *p;
  
  i = 0;
  p = leaf_grid;
  for (; i < 0x400; i++) {
    for (j = 0; j < 10; j++) {
      p[i * 10 + j] = 0xffff;
    }
  }
  leaf_rescan_pos = 0;
}

void leaf_grid_cell_add(int cell, leaf_rec *leaf)
{
  int i;
  int found;
  uint id;
  
  i = 0;
  found = -1;
  for (; i < 10; i++) {
    ushort *tbl = leaf_grid;
    id = tbl[cell * 10 + i];
    if (id == 0xffff) {
      tbl[cell * 10 + i] = leaf->id;
      return;
    }
    if (leaf_records[id].mode == 0) {
      found = i;
    }
  }
  if ((leaf->mode != 0) & (found >= 0)) {
    id = leaf_grid[cell * 10 + found];
    leaf_grid[cell * 10 + found] = leaf->id;
    leaf_records[id].cell = -1;
  }
  else {
    leaf->cell = -1;
  }
  return;
}

void leaf_grid_cell_remove(int cell, leaf_rec *leaf)
{
  int i;
  ushort *p;
  i = 0;
  p = leaf_grid;
  for (; i < 10; i++) {
    if (p[cell * 10 + i] == (ushort)leaf->id) {
      p[cell * 10 + i] = 0xffff;
      return;
    }
  }
  return;
}

void leaf_grid_rescan_step(void)
{
  int end;
  int i;
  leaf_rec *leaf;
  int start;
  
  start = leaf_rescan_pos;
  leaf_rescan_pos = leaf_rescan_pos + 0x32;
  if (leaf_rescan_pos >= leaf_cfg->in_use_count) {
    end = leaf_cfg->in_use_count;
    leaf_rescan_pos = 0;
  }
  else {
    end = leaf_rescan_pos;
  }
  for (leaf = &leaf_records[start], i = start; i < end; i++, leaf++) {
    if (leaf->cell < 0) {
      leaf_grid_update(leaf);
    }
  }
  return;
}

void leaf_grid_update(leaf_rec *leaf)
{
  int found;
  int old;
  short cell;
  
  old = leaf->cell;
  if (-3 < old) {
    found = leaf_grid_cell_at(leaf);
    leaf->cell = found;
    cell = (int)found;
    if (cell != old) {
      if (-1 < old) {
        leaf_grid_cell_remove(old,leaf);
      }
      if (-1 < cell) {
        leaf_grid_cell_add(cell,leaf);
      }
    }
  }
  return;
}

int leaf_grid_cell_at(leaf_rec *leaf)
{
  int cell;
  float gz;
  float gx;
  
  gx = (leaf->pos[0] + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
  gz = (leaf->pos[2] + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
  if ((((gx < 0.0) || ((float)leaf_cfg->grid_cells <= gx)) || (gz < 0.0)) ||
     ((float)leaf_cfg->grid_cells <= gz)) {
    cell = -2;
  }
  else { int x = (int)gx; int y = (int)gz; cell = x + y * leaf_cfg->grid_cells; }
  return cell;
}

void leaf_move(void)
{
  int i;
  player_input_rec *input;
  float *p;
  float t0 [1];
  i = leaf_enabled();
  if (i != 0) {
    leaf_timer_start(t0);
    leaf_feet_track();
    leaf_physics_step();
    if (leaf_warmup_frames != 0) {
      leaf_warmup_frames = leaf_warmup_frames + -1;
    }
    else {
      p = leaf_times;
      leaf_timer_lap(t0,p,p + 1);
    }
    input = player_input_get(0);
    if ((input->pressed & 0x8000) != 0) {
      for (i = 0; i < 10; i++) {
        leaf_times[i] = 0.0;
      }
    }
  }
  return;
}

void leaf_feet_track(void)
{
  typedef struct { float x; float y; float z; } vec;
  int i;
  int ok;
  int sel;
  int k;
  int off;
  vec *c;
  int m;
  float *t;
  vec n;
  vec o;

  for (i = 0; i < 3; i++) {
    { int b = (int)&leaf_foot_frame; *(float *)(b + i * 60 + 16) = 0.0; }
  }
  if ((int)++leaf_foot_frame < 0) ok = 0;
  else {
    ok = 1;
    leaf_foot_frame &= 1;
  }
  sel = leaf_foot_frame & 1;
  m = (int)(rob_get((uchar)sel))->bone_mtx;
  for (i = 0; i < 3; i++) {
    k = sel * 3 + i;
    { vec *c = (vec *)&leaf_foot_pos; o = c[k]; }
    t = (float *)(leaf_foot_bones[i] * 0x40 + m);
    n.x = t[12];
    n.y = t[13];
    n.z = t[14];
    ((vec *)leaf_foot_prev)[k] = o;
    ((vec *)&leaf_foot_pos)[k] = n;
    ((vec *)leaf_foot_move)[k].x = n.x - o.x;
    ((vec *)leaf_foot_move)[k].y = n.y - o.y;
    ((vec *)leaf_foot_move)[k].z = n.z - o.z;
    if (ok) leaf_kick_from_foot(k);
  }
  if (ok) {
    leaf_source1_type_set();
    leaf_source2_type_set();
  }
}

void leaf_source1_type_set(void)
{
  *(int *)&leaf_sources[60] = 1;
  return;
}

void leaf_source2_type_set(void)
{
  *(int *)&leaf_sources[120] = 2;
  return;
}

void leaf_kick_from_foot(int foot)
{
  leaf_source_rec *s = (leaf_source_rec *)&leaf_sources;
  vec3 d;
  vec3 b;
  vec3 a;
  float h;
  float f;
  int n;
  a = ((vec3 *)leaf_foot_prev)[foot];
  b = ((vec3 *)&leaf_foot_pos)[foot];
  if (b.y < a.y) h = b.y - leaf_cfg->floor_y;
  else h = a.y - leaf_cfg->floor_y;
  if (h < 0.0) h = 0.0;
  if (h > 0.7) return;
  h = (0.7 - h) * 10.0;
  if (h > 1.0) h = 1.0;
  d.x = b.x - a.x;
  d.y = b.y - a.y;
  d.z = b.z - a.z;
  f = signed_sqrt(d.x * d.x + d.z * d.z) * h;
  if (f < 0.1) return;
  if (s->strength > f) return;
  s->id = foot;
  s->strength = f;
  s->radius = 0.49;
  n = (f - 0.1) * 317.5;
  if (n > 127) n = 127;
  if ((rob_get(*(char *)&leaf_foot_frame & 1))->state & 1) {
    h = leaf_kick_speed_curve[n] * leaf_kick_tuning * leaf_cfg->kick_push_alt;
    s->limit_len = leaf_cfg->kick_limit_len_alt;
    s->limit_y = leaf_cfg->kick_limit_y_alt;
  } else {
    h = leaf_kick_speed_curve[n] * leaf_kick_tuning * leaf_cfg->kick_push;
    s->limit_len = leaf_cfg->kick_limit_len;
    s->limit_y = leaf_cfg->kick_limit_y;
  }
  d.y = 0.0;
  vec3_normalize(&d.x);
  s->dir = d;
  d.x *= h;
  d.y = h;
  d.z *= h;
  s->push = d;
  s->pos = a;
}

void leaf_physics_step(void)
{
  float t[1];
  float *p;
  
  leaf_timer_start(t);
  leaf_grid_rescan_step();
  leaf_spawn_step();
  leaf_sources_apply();
  p = &leaf_times[2];
  leaf_timer_lap(t,p,p + 1);
  leaf_active_update();
  leaf_timer_lap(t,p + 2,p + 3);
  return;
}

void leaf_spawn_step(void)
{
  short *row;
  leaf_cfg->spawn_timer += 1;
  if (leaf_cfg->in_use_count < leaf_cfg->leaf_count &&
      leaf_active_count < leaf_cfg->fly_max &&
      leaf_cfg->spawn_timer >= leaf_cfg->spawn_interval) {
    leaf_cfg->spawn_timer = 0;
    row = ((short (*)[0x1f5])leaf_active_lists)[leaf_active_side];
    row[leaf_active_count] = leaf_cfg->in_use_count;
    leaf_active_count++;
    row[leaf_active_count] = -1;
    leaf_records[leaf_cfg->in_use_count].mode = 2;
    leaf_records[leaf_cfg->in_use_count].pos[1] = 12.0;
    leaf_cfg->in_use_count += 1;
  }
}

void leaf_sources_apply(void)
{
  leaf_source_rec *e;
  int i;
  int idx;
  float x0;
  float y0;
  float x1;
  float y1;
  float x;
  float y;

  for (i = 0; i < 3; i++) {
    e = &((leaf_source_rec *)&leaf_sources)[i];
    if (e->strength > 0.0) {
      x0 = ((e->pos.x - e->radius) + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
      y0 = ((e->pos.z - e->radius) + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
      x1 = (e->pos.x + e->radius + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
      y1 = (e->pos.z + e->radius + leaf_cfg->grid_half_width) * leaf_cfg->cells_per_unit;
      if (x0 >= 0.0 && y0 >= 0.0 && x1 <= (float)leaf_cfg->grid_cells &&
          y1 <= (float)leaf_cfg->grid_cells) {
        for (x = x0; x <= x1; x = x + 1.0) {
          if (x >= 0.0 && x < (float)leaf_cfg->grid_cells) {
            for (y = y0; y <= y1; y = y + 1.0) {
              if (y >= 0.0 && y < (float)leaf_cfg->grid_cells) {
                idx = (int)x + (int)y * leaf_cfg->grid_cells;
                switch (e->type) {
                case 0:
                  leaf_kick_apply(i,idx);
                  break;
                case 1:
                  leaf_source_nop(i,idx);
                  break;
                case 2:
                  leaf_swirl_apply(i,idx);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}

void leaf_swirl_apply(int source, int cell)
{
  leaf_source_rec *s;
  leaf_rec *e;
  vec3 pos;
  vec3 v;
  vec3 d;
  int list;
  short *out;
  float inv;
  float c;
  float best;
  float k2;
  float bs;
  float k;
  float h;
  float dist;
  float f;
  int i;
  int n;
  int idx;
  out = (short *)((char *)leaf_active_lists + leaf_active_side * 1002);
  s = (leaf_source_rec *)&leaf_sources + source;
  pos = s->pos;
  best = s->strength;
  k = s->radius;
  k2 = k * k;
  inv = 1.0 / k;
  bs = best * leaf_swirl_tangential;
  c = leaf_swirl_inward * bs;
  d.y = 0.0;
  for (i = 0, n = 0; i < 10 && (leaf_active_count < leaf_cfg->fly_max) & (n < 10); i++) {
    idx = ((struct { ushort a[1]; } *)leaf_grid)->a[cell * 10 + i];
    if (idx != 0xffff) {
      e = &leaf_records[idx];
      n++;
      h = e->pos[1] - (e->floor_y + 1.0);
      if (h <= 1.0) {
        if (h < 0.0) h = 0.0;
        d.x = e->pos[0] - pos.x;
        d.z = e->pos[2] - pos.z;
        dist = d.x * d.x + d.z * d.z;
        if (dist <= k2) {
          vec3_normalize(&d.x);
          idx = inv * signed_sqrt(dist) * 127.0;
          if (idx > 127) idx = 127;
          f = 1.0 - leaf_swirl_falloff[idx];
          v.x = f * (-d.z * bs) + -d.x * c * f;
          v.z = f * (bs * d.x) + -d.z * c * f;
          v.y = leaf_swirl_upward * best * f * (1.0 - h);
          e->spin = (1.0 - f) * 700.0 + 500.0;
          if (e->mode == 0) {
            leaf_ground_vertices_hide(e);
            out[leaf_active_count] = e->id;
            leaf_active_count++;
          }
          e->mode = 2;
          e->vel.x += v.x;
          e->vel.y += v.y;
          e->vel.z += v.z;
          f = vec3_length(&e->vel.x);
          f = leaf_swirl_speed_max / f;
          if (f < 1.0) {
            e->vel.x *= f;
            e->vel.y *= f;
            e->vel.z *= f;
          }
        }
      }
    }
  }
  out[leaf_active_count] = -1;
}

void leaf_source_nop(int source, int cell)
{
  return;
}

void leaf_kick_apply(int source, int cell)
{
  leaf_source_rec *e;
  short *row;
  vec3 pos;
  vec3 k;
  vec3 dir;
  vec3 d;
  float r2;
  float s;
  float f;
  float t;
  float v;
  float a;
  float b;
  float c;
  float q;
  float sc;
  float off;
  int i;
  int n;
  uint idx;
  uint rnd;
  leaf_rec *leaf;

  row = ((short (*)[0x1f5])leaf_active_lists)[leaf_active_side];
  e = (leaf_source_rec *)(source * 60 + (int)&leaf_sources);
  pos = e->pos;
  k = e->push;
  dir = e->dir;
  r2 = e->radius * e->radius;
  for (i = 0; i < 10 && leaf_active_count < leaf_cfg->fly_max; i++) {
    idx = leaf_grid[cell * 10 + i];
    if (idx == 0xffff) continue;
    leaf = &leaf_records[idx];
    d.x = leaf->pos[0] - pos.x;
    d.y = 0.0;
    d.z = leaf->pos[2] - pos.z;
    t = d.x * d.x + d.z * d.z;
    if (t > r2) continue;
    t = signed_sqrt(t);
    s = 1.0 - (t + t);
    if (t > 0.0) {
      f = vec3_dot(&d.x, &dir.x) / t;
    }
    else {
      f = 1.0;
    }
    if (f < 0.0) continue;
    t = (leaf_cfg->floor_y + 0.5) - leaf->pos[1];
    if (t < 0.0) continue;
    n = t * 254.0;
    if (n > 0x7f) {
      n = 0x7f;
    }
    t = leaf_kick_height_gain[n];
    if (leaf->mode == 0) {
      leaf_ground_vertices_hide(leaf);
      row[leaf_active_count] = leaf->id;
      leaf_active_count++;
    }
    leaf->mode = 2;
    leaf->source = e->id;
    rnd = stage_fx_rand_next();
    a = (float)(rnd & 0xff);
    sc = 0.00274;
    off = 0.3;
    a = a * sc + off;
    b = (float)((rnd & 0xff0) >> 4) * 0.00392 + 0.5;
    c = (float)((rnd & 0xff00) >> 8) * sc + off;
    leaf->vel.x += s * k.x * t * f * a;
    v = leaf->vel.y + k.y * s * t * f * b;
    leaf->vel.y = v;
    leaf->vel.z += s * k.z * t * f * c;
    if (v > e->limit_y) {
      leaf->vel.y = e->limit_y;
    }
    q = e->limit_len / vec3_length(&leaf->vel.x);
    if (q < 1.0) {
      leaf->vel.x *= q;
      leaf->vel.y *= q;
      leaf->vel.z *= q;
    }
    leaf->spin = s * 500.0 + 700.0;
  }
  row[leaf_active_count] = -1;
}

void leaf_active_update(void)
{
  vec3 zero = { 0.0, 0.0, 0.0 };
  ushort *src;
  ushort *dst;
  ushort id;
  leaf_rec *p;
  float len;
  float d;
  float k;
  float spin;
  float y;
  float a;
  src = ((ushort (*)[501])leaf_active_lists)[leaf_active_side];
  leaf_active_side ^= 1;
  dst = ((ushort (*)[501])leaf_active_lists)[leaf_active_side];
  leaf_active_count_prev = leaf_active_count;
  leaf_active_count = 0;
  mtx_push(0);
  while ((id = *src++) != 0xffff) {
    p = &leaf_records[id];
    switch (p->mode) {
    case 1:
      p->vel.x *= 0.5;
      p->vel.z *= 0.5;
      p->pos[0] += p->vel.x;
      p->pos[2] += p->vel.z;
      if (leaf_settle_step(p)) {
        p->mode = 0;
        p->vel = zero;
      }
      break;
    case 2:
      p->vel.y += leaf_cfg->gravity;
      len = vec3_length(&p->vel.x);
      d = 0.0;
      if (len > d) d = vec3_dot(&p->vel.x,p->nrm) / len;
      k = len * leaf_cfg->drag * d;
      p->vel.x -= k * p->nrm[0];
      p->vel.y -= k * p->nrm[1];
      p->vel.z -= k * p->nrm[2];
      a = p->spin * leaf_cfg->spin * len;
      p->ang[1] += (short)(int)(a * 0.25);
      p->ang[0] += (short)(int)a;
      p->ang[2] += (short)(int)(a * 0.5);
      if (p->spin > p->spin_max) p->spin = p->spin - 1.0;
      p->pos[0] += p->vel.x;
      y = p->pos[1] + p->vel.y;
      p->pos[1] = y;
      p->pos[2] += p->vel.z;
      if (y <= p->floor_y) {
        switch (p->cell) {
        case -2:
          if (leaf_cfg->drop_floor_y > 0.0) {
            p->floor_y = leaf_cfg->drop_floor_y;
            p->cell = -3;
            break;
          }
          goto fall;
        case -3:
          if (fabsf(p->pos[0]) < leaf_cfg->drop_half_width &&
              fabsf(p->pos[2]) < leaf_cfg->drop_half_width) {
            p->pos[1] = p->floor_y;
            p->vel.y = 0.0;
            p->mode = 1;
            break;
          }
        fall:
          p->floor_y = leaf_cfg->fall_floor_y;
          p->cell = -4;
          break;
        default:
          p->pos[1] = p->floor_y;
          p->vel.y = 0.0;
          p->mode = 1;
          break;
        case -4:
          p->mode = 0;
          break;
        }
      }
      break;
    }
    if (p->mode != 0) {
      dst[leaf_active_count] = id;
      leaf_active_count++;
    }
    else {
      leaf_ground_vertices_set(p);
    }
    leaf_grid_update(p);
  }
  dst[leaf_active_count] = 0xffff;
  mtx_pop(1);
}

void leaf_fly_shapes_build(void)
{
  char *model;
  int k;
  int i;
  float f;
  float b[18];

  model = (char *)obj_model_get(leaf_cfg->fly_model_id);
  if (model != 0) {
    i = 0;
    k = 0x30;
    do {
      b[i * 3] = f = *(float *)(k * 4 + model + 4);
      b[i * 3 + 1] = *(float *)(k * 4 + model + 8);
      if (0 < i) {
        if (f < b[9+0]) {
          b[9+0] = f;
        }
        if (b[i * 3] > b[15+0]) {
          b[15+0] = b[i * 3];
        }
        if (b[i * 3 + 1] < b[9+1]) {
          b[9+1] = b[i * 3 + 1];
        }
        if (b[i * 3 + 1] > b[15+1]) {
          b[15+1] = b[i * 3 + 1];
        }
      }
      else {
        b[9+0] = b[15+0] = b[i * 3];
        b[9+1] = b[15+1] = b[i * 3 + 1];
      }
      i = i + 1;
      k = k + 6;
    } while (i < 3);
    b[12+0] = (b[9+0] + b[15+0]) * 0.5;
    b[12+1] = (b[9+1] + b[15+1]) * 0.5;
    i = 0;
    do {
      b[i * 3] = b[i * 3] - b[12+0];
      b[i * 3 + 1] = b[i * 3 + 1] - b[12+1];
      b[i * 3 + 2] = 0.0;
      i = i + 1;
    } while (i < 3);
    mtx_push(0);
    i = 0;
    do {
      mtx_load_identity();
      mtx_rot_z_s16(i << 10);
      mtx_xform_points((float (*)[3])b,leaf_fly_shapes[i],3);
      i = i + 1;
    } while (i < 0x40);
    mtx_pop(1);
  }
  return;
}

void leaf_fly_tri_get(leaf_rec *leaf, float (*tri)[3])
{
  typedef struct { float x; float y; float z; } V;
  V in;
  V out;
  
  in.x = 0.0;
  in.y = 0.0;
  in.z = 1.0;
  mtx_load_identity();
  mtx_rot_y_s16((int)leaf->ang[1]);
  mtx_rot_x_s16((int)leaf->ang[0]);
  mtx_xform_point(&in.x,&out.x);
  mtx_overwrite_translation(leaf->pos);
  mtx_xform_points(leaf_fly_shapes[(short)((ushort)((uint)(int)leaf->ang[2] >> 10) & 0x3f)],tri,
               3);
  if (out.y < 0.0) {
    out.x = -out.x;
    out.y = -out.y;
    out.z = -out.z;
  }
  *(V *)leaf->nrm = out;
  leaf->packed_nrm =
       (int)(out.x * 127.0) & 0xffU | ((int)(out.y * 127.0) & 0xffU) << 8 |
       ((int)(out.z * 127.0) & 0xffU) << 0x10;
}

void leaf_ground_shapes_build(void)
{
  float pts[6][3];
  float mn[3];
  float c[3];
  float mx[3];
  char *base;
  int i;
  int k;
  int n;
  float x;

  base = (char *)obj_model_get(leaf_cfg->ground_model_id);
  if (base == 0) return;
  for (i = 0, n = 0x30; i < 4; i++) {
    for (k = 0; k < 6; k++, n += 6) {
      pts[k][0] = x = *(float *)(base + n * 4 + 4);
      pts[k][1] = *(float *)(base + n * 4 + 8);
      if (k > 0) {
        if (mn[0] > x) mn[0] = x;
        if (pts[k][0] > mx[0]) mx[0] = pts[k][0];
        if (pts[k][1] < mn[1]) mn[1] = pts[k][1];
        if (pts[k][1] > mx[1]) mx[1] = pts[k][1];
      }
      else {
        mn[0] = mx[0] = pts[k][0];
        mn[1] = mx[1] = pts[k][1];
      }
    }
    c[0] = (mn[0] + mx[0]) * 0.5;
    c[1] = (mn[1] + mx[1]) * 0.5;
    for (k = 0; k < 6; k++) {
      float *t = leaf_ground_shapes;
      t[k * 3 + i * 18] = pts[k][0] - c[0];
      t[k * 3 + i * 18 + 1] = pts[k][1] - c[1];
      t[k * 3 + i * 18 + 2] = 0.0;
    }
  }
}

void leaf_ground_quad_get(leaf_rec *leaf, float (*quad)[3])
{
  typedef struct { int w[3]; } v3;
  float a[3];
  float b[3];

  a[0] = 0.0;
  a[1] = 0.0;
  a[2] = 1.0;
  mtx_load_identity();
  mtx_rot_y_s16((int)leaf->ang[1]);
  mtx_rot_x_s16((int)leaf->ang[0]);
  mtx_rot_z_s16((int)leaf->ang[2]);
  mtx_xform_point(a,b);
  mtx_overwrite_translation(leaf->pos);
  mtx_xform_points((float (*)[3])(leaf_ground_shapes + (uint)leaf->shape * 0x12),quad,6);
  if (b[1] < 0.0) {
    b[0] = -b[0];
    b[1] = -b[1];
    b[2] = -b[2];
  }
  *(v3 *)leaf->nrm = *(v3 *)b;
  leaf->packed_nrm =
       (int)(b[0] * 127.0) & 0xffU | ((int)(b[1] * 127.0) & 0xffU) << 8 |
       ((int)(b[2] * 127.0) & 0xffU) << 0x10;
}

bool leaf_settle_step(leaf_rec *leaf)
{
  uint q;
  short *p;
  short a[4];
  short b[4];
  
  memcpy(a,leaf_settle_steps,8);
  memcpy(b,leaf_settle_angles,8);
  p = &leaf->ang[0];
  q = (int)*p >> 0xe & 3;
  *p = *p + a[q];
  if (q != ((int)*p >> 0xe & 3U)) {
    *p = b[q];
    return 1;
  }
  return 0;
}

void leaf_draw(void)
{
  int on;
  float buf[1];
  float *p;
  
  on = leaf_enabled();
  if ((on != 0) && ((demo_flags & 0x800U) == 0)) {
    leaf_timer_start(buf);
    light_set_send(0,2);
    leaf_ground_draw();
    leaf_fly_draw();
    light_set_send(0,0);
    p = &leaf_times[6];
    leaf_timer_lap(buf,p,p + 1);
    leaf_draw_end_stub();
  }
  return;
}

void leaf_ground_draw(void)
{
  if (leaf_ground_model != 0) {
    model_draw((compiled_model_rec *)leaf_ground_model);
  }
  return;
}

void leaf_ground_vertices_set(leaf_rec *leaf)
{
  int i;
  int k;
  float v[6][3];

  if (leaf_ground_model != (uchar *)0x0) {
    leaf_ground_quad_get(leaf,v);
    k = (uint)(ushort)leaf->id * 0x24 + 0x30;
    for (i = 0; i < 6; i++) {
      *(uint *)(leaf_ground_model + k * 4) &= 0xff000000;
      *(uint *)(leaf_ground_model + k * 4) |= leaf->packed_nrm;
      *(float *)(leaf_ground_model + k * 4 + 4) = v[i][0];
      *(float *)(leaf_ground_model + k * 4 + 8) = v[i][1];
      *(float *)(leaf_ground_model + k * 4 + 0xc) = v[i][2];
      k += 6;
    }
  }
}

void leaf_ground_vertices_hide(leaf_rec *leaf)
{
  int k;
  int i;
  
  if (leaf_ground_model != 0) {
    k = (uint)(ushort)leaf->id * 0x24 + 0x30;
    i = 0;
    do {
      *(float *)(k * 4 + leaf_ground_model + 8) = leaf_cfg->floor_y - 1.0;
      k = k + 6;
      i = i + 1;
    } while (i < 6);
  }
  return;
}

void leaf_fly_draw(void)
{
  float v[9];
  int i;
  int j;
  uint fr;
  ushort *ent;
  int k;
  ushort n;
  leaf_rec *leaf;
  int lvl;
  float y;
  float save;
  float *c;
  float *t1;
  float *t2;

  if (leaf_active_count == 0 || leaf_fly_models[0] == 0 || leaf_fly_models[1] == 0 ||
      leaf_shadow_models[0] == 0 || leaf_shadow_models[1] == 0) {
    return;
  }
  fr = ++leaf_draw_frame & 1;
  y = leaf_cfg->floor_y;
  ent = leaf_active_lists + leaf_active_side * 0x1f5;
  mtx_push(0);
  for (i = 0, k = 0x30; i < leaf_cfg->fly_max; i++) {
    n = *ent;
    if (n != 0xffff) {
      leaf = &leaf_records[n];
      leaf_fly_tri_get(leaf,(float (*)[3])v);
      t1 = leaf_fly_uvs[leaf->shape];
      lvl = (int)((float)(int)(leaf->pos[1] - y) * 3.333 + 4.0);
      if (lvl < 4) {
        lvl = 4;
      }
      if (lvl > 7) {
        lvl = 7;
      }
      t2 = leaf_shadow_uvs[lvl];
      for (j = 0; j < 3; j++) {
        int *pa = (int *)&leaf_fly_models + fr;
        int *pb = (int *)&leaf_shadow_models + fr;
        *(uint *)(*pa + k * 4) = *(uint *)(*pa + k * 4) & 0xff000000;
        *(uint *)(*pa + k * 4) = *(uint *)(*pa + k * 4) | leaf->packed_nrm;
        *(float *)(*pa + k * 4 + 4) = v[j * 3];
        *(float *)(*pa + k * 4 + 8) = v[j * 3 + 1];
        *(float *)(*pa + k * 4 + 0xc) = v[j * 3 + 2];
        *(float *)(*pa + k * 4 + 0x10) = t1[j * 2];
        *(float *)(*pa + k * 4 + 0x14) = t1[j * 2 + 1];
        *(float *)(*pb + k * 4 + 4) = v[j * 3];
        if (v[j * 3 + 1] < y + 0.05) {
          *(float *)(*pb + k * 4 + 8) = v[j * 3 + 1] - 0.01;
        }
        else {
          int *qb = (int *)&leaf_shadow_models + fr;
          *(float *)(*qb + k * 4 + 8) = y + 0.05;
        }
        {
          int *rb = (int *)&leaf_shadow_models + fr;
          *(float *)(*rb + k * 4 + 0xc) = v[j * 3 + 2];
          *(float *)(*rb + k * 4 + 0x10) = t2[j * 2];
          *(float *)(*rb + k * 4 + 0x14) = t2[j * 2 + 1];
        }
        k += 6;
      }
      ent++;
    }
    else {
      for (j = 0; j < 3; j++) {
        int *pa = (int *)&leaf_fly_models + fr;
        *(float *)(*pa + k * 4 + 8) = y - 50.0;
        {
          int *pb = (int *)&leaf_shadow_models + fr;
          *(float *)(*pb + k * 4 + 8) = y - 50.0;
        }
        k += 6;
      }
    }
  }
  mtx_pop(1);
  c = &camera_proj.near_clip;
  save = *c;
  *c = 1.0;
  mtx_push(0);
  mtx_stack_select(3);
  mtx_load_identity();
  proj_from_vfov((int)((pers * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,*c,camera.proj.far_clip);
  proj_cmds_send();
  mtx_stack_select(1);
  model_draw(((compiled_model_rec **)&leaf_fly_models)[fr]);
  model_draw(((compiled_model_rec **)&leaf_shadow_models)[fr]);
  *c = save;
  mtx_stack_select(3);
  mtx_load_identity();
  proj_from_vfov((int)((pers * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,*c,camera.proj.far_clip);
  proj_cmds_send();
  mtx_stack_select(1);
  mtx_pop(1);
}

void leaf_exit(void)
{
  int on;
  
  on = leaf_enabled();
  if (on != 0) {
    if (leaf_ground_model != 0) {
      elan_heap_free(leaf_ground_model);
    }
    if (leaf_fly_models[0] != 0) {
      elan_heap_free(leaf_fly_models[0]);
    }
    if (leaf_fly_models[1] != 0) {
      elan_heap_free(leaf_fly_models[1]);
    }
    if (leaf_shadow_models[0] != 0) {
      elan_heap_free(leaf_shadow_models[0]);
    }
    if (leaf_shadow_models[1] != 0) {
      elan_heap_free(leaf_shadow_models[1]);
    }
    effect_free(leaf_records);
    leaf_records = 0;
  }
  return;
}

void leaf_draw_end_stub(void)
{
  return;
}

void leaf_grid_cell_draw(int cell, uint color)
{
  typedef struct { float x; float y; float z; } v3;
  v3 pos;
  v3 next;
  v3 tbl[4] = { {0.0, 0.0, 0.5}, {0.5, 0.0, 0.0}, {0.0, 0.0, -0.5}, {-0.5, 0.0, 0.0} };
  leaf_cfg_rec *g;
  int q;
  int i;

  if (cell < 0) return;
  g = leaf_cfg;
  q = cell / g->grid_cells;
  pos.x = (float)(cell % g->grid_cells) * 0.5 - g->grid_half_width;
  pos.z = (float)q * 0.5 - g->grid_half_width;
  pos.y = next.y = g->floor_y;
  line3d_width_store(2.0);
  for (i = 0; i < 4; i++) {
    next.x = pos.x + tbl[i].x;
    next.z = pos.z + tbl[i].z;
    line3d_quads_opaque(&pos.x,&next.x,color);
    pos = next;
  }
}
