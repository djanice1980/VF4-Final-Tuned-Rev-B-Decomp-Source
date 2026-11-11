/*
 * vf4lib/libeffect/src/effect_sand.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint sand_grid_dents; /* 0x0c1a5e44 */
extern sand_joint_rec sand_body_joints[5]; /* 0x0c1a6cdc */
extern int game_frame_count; /* 0x0c292540 */
extern float sand_level; /* 0x0c2b0788 */
extern int sand_frame_count; /* 0x0c2b07a0 */
extern float sand_puff_curve[128]; /* 0x0c2b07bc */
extern int sand_puff_kinds; /* 0x0c2b09bc */
extern int sand_cloud_model_ids[32]; /* 0x0c2b09c0 */
extern uchar *sand_puff_config_ptr; /* 0x0c2b0a70 */
extern int sand_timing_skip_frames; /* 0x0c2b0a74 */
extern int sand_timing_last_frame; /* 0x0c2b0a78 */
extern sand_grain_rec sand_grains[100]; /* 0x0c396cd8 */
extern int sand_grains_baked; /* 0x0c397b18 */
extern float sand_update_times[2]; /* 0x0c397b1c */
extern float sand_puff_times[4]; /* 0x0c397b24 */
extern float sand_draw_times[2]; /* 0x0c397b34 */
extern int sand_bodies; /* 0x0c48ab64 */
extern char *sand_grid; /* 0x0c48b8b0 */
extern int sand_dirty_count; /* 0x0c48b920 */
extern float sand_cell_scale_x; /* 0x0c48b924 */
extern float sand_cell_scale_z; /* 0x0c48b928 */
extern int sand_puff_list_len; /* 0x0c48c650 */
extern int sand_cloud_count; /* 0x0c48c654 */
extern int sand_puff_cursor; /* 0x0c48c658 */
extern uchar sand_puff_list_new; /* 0x0c48c65c */
extern uchar sand_puff_list_live; /* 0x0c48c65d */
extern sand_puff_config_rec sand_puff_config; /* 0x0c2b0a40 */
extern camera_state camera; /* 0x0c370ce0 */
extern sand_colors_rec sand_colors; /* 0x0c396cc0 */
extern sand_puff_config_copy_rec sand_puff_config_copy; /* 0x0c397ae8 */
extern ground_frame_rec sand_ground_frame; /* 0x0c48ab60 */
extern uchar sand_puff_lists[2][81]; /* 0x0c48c5ac */

static inline float fsqrt_inl(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void sand_umi_init(void)
{
  sand_grid = effect_malloc(560000);
  stage_fx_rand_reset();
  sand_ground_init();
  sand_puffs_reset();
  sand_puff_init();
  return;
}

void sand_umi_update(void)
{
  float buf[1];
  float *p;
  float *q;
  
  sand_time_mark(buf);
  sand_ground_update();
  p = sand_update_times;
  sand_time_lap(buf,p,p + 1);
  if (mode_get_main() == MODE_MAIN_GAME && mode_get_sub() == 9) {
    sand_puffs_reset();
  }
  else {
    sand_puffs_update();
  }
  sand_frame_count = sand_frame_count + 1;
  q = sand_puff_times;
  sand_time_lap(buf,q,q + 1);
  { float *r = q - 2; q[2] = *r + *q; }
  if (q[3] < q[2]) {
    q[3] = q[2];
  }
}

void sand_umi_draw(void)
{
  float buf;
  
  sand_time_mark(&buf);
  sand_ground_draw();
  sand_puffs_draw();
  sand_time_lap(&buf,sand_draw_times,sand_draw_times + 1);
  sand_umi_draw_tail();
  return;
}

void sand_umi_free(void)
{
  int i;

  if (sand_ground_model != 0) {
    elan_heap_free(sand_ground_model);
  }
  i = 0;
  do {
    if (sand_grain_models[i] != 0) {
      elan_heap_free(sand_grain_models[i]);
    }
    i = i + 1;
  } while (i < 0x20);
  effect_free(sand_grid);
  sand_grid = 0;
  return;
}

void sand_ground_init(void)
{
  sand_ground_model_build();
  sand_grid_build();
  sand_dirty_list_clear();
  sand_ground_vertices_write_all();
  sand_cell_scale_x = 7.8020344;
  sand_cell_scale_z = 9.009009;
  return;
}

void sand_ground_model_build(void)
{
  int *src;
  int i, j, idx, n;

  src = (int *)obj_model_get(0x1d008f);
  if (src == 0) {
    sand_ground_model = 0;
    return;
  }
  n = 0x4ee8;
  sand_ground_model = elan_heap_alloc(n * 24 + 192);
  if (sand_ground_model == 0) return;
  for (j = 0; j < 48; j++) ((int *)sand_ground_model)[j] = src[j];
  sand_ground_model->sphere[3] = 20.0f;
  sand_ground_model->size = n * 24 + 192;
  sand_ground_model->triangle_count = 20000;
  sand_ground_model->vertex_count = n;
  sand_ground_model->field_0x28 = 1;
  sand_ground_model->field_0x2c = 1;
  sand_ground_model->parts[0].size = n * 24 + 96;
  *(int *)((char *)sand_ground_model + 0x6c) = 0xfff0f0d0;
  *(int *)((char *)sand_ground_model + 0x74) = 0xfff0f0d0;
  *(uint *)((char *)sand_ground_model + 0xa4) &= 0xe7ffffff;
  *(uint *)((char *)sand_ground_model + 0xa4) |= 0x18000000;
  *(uint *)((char *)sand_ground_model + 0xa8) &= 0xfffe7fff;
  *(uint *)((char *)sand_ground_model + 0xb0) &= 0xfffe7fff;
  *(int *)((char *)sand_ground_model + 0xbc) = n;
  for (j = 0, idx = 48; j < 100; j++) {
    for (i = 0; i < 202; i++) {
      switch (i) {
      case 0: case 1: ((int *)sand_ground_model)[idx] = 0; break;
      case 2: ((int *)sand_ground_model)[idx] = 0x60000000; break;
      case 201: ((int *)sand_ground_model)[idx] = 0xa0000000; break;
      default: ((int *)sand_ground_model)[idx] = 0x20000000; break;
      }
      ((float *)sand_ground_model)[idx + 4] = sand_ground_u_step * (float)(i / 2) + (float)(i % 2) * (0.5f * sand_ground_u_step);
      ((float *)sand_ground_model)[idx + 5] = sand_ground_v_step * (float)(i % 2 + j);
      idx += 6;
    }
  }
}

void sand_grid_height_table_apply(void)
{
  struct { int i; float k; } *p;
  float t;

  p = (void *)&sand_grid_dents;
  if (0.0 < sand_dent_depth) {
    for (; p->i != -1; p++) {
      t = p->k * sand_dent_depth;
      ((sand_cell_rec *)sand_grid)[p->i].pos.y -= t;
    }
  }
}

void sand_grid_build(void)
{
  sand_cell_rec *e;
  vec3 tmp;
  float s;
  float a;
  float xo;
  float fa;
  float fb;
  float c;
  int i;
  int j;
  int m;
  int n;
  int v;
  int y;

  sand_ground_frame.frame = 0;
  sand_ground_frame.field_0x0 = 0;
  c = 0.4714715;
  for (i = 0; i < 103; i++) {
    tmp.z = (float)(i - 51) * 0.111;
    s = sin_radians(c * tmp.z);
    a = s * 0.05 + 0.15;
    xo = (float)(i % 2) * -0.06408585;
    if (i <= 50) {
      fa = (float)(i - sand_edge_rows) * sand_edge_slope_z;
    }
    else {
      fa = (float)((-sand_edge_rows - i) + 102) * sand_edge_slope_z;
    }
    if (fa < 0.0) fa = 0.0;
    if (fa > 1.0) fa = 1.0;
    for (j = 0; j < 104; j++) {
      e = (sand_cell_rec *)(sand_grid + (j + i * 104) * 36);
      if (j <= 51) {
        fb = (float)(j - sand_edge_cols) * sand_edge_slope_x;
      }
      else {
        fb = (float)((-sand_edge_cols - j) + 103) * sand_edge_slope_x;
      }
      if (fb < 0.0) fb = 0.0;
      if (fb > 1.0) fb = 1.0;
      if (fb > fa) fb = fa;
      tmp.y = sand_grid_base_y + a * fb;
      tmp.x = (float)(j - 51) * 0.1281717 + xo;
      e->pos = tmp;
      e->base_y = tmp.y;
      if (e->pos.y > sand_grid_top_y) {
        sand_grid_top_y = e->pos.y;
      }
      e->press_frame = -100;
      e->flags = 0;
      e->vtx[0] = 0;
      e->vtx[1] = 0;
    }
  }
  sand_grid_top_y = sand_grid_top_y + 0.1;
  sand_grid_height_table_apply();
  for (y = 0; y < 100; y++) {
    v = y * 0x4bc + 0x30;
    for (m = 0; m < 202; m++) {
      if (y & 1) {
        j = (m + 1) / 2 + 1;
      }
      else {
        j = m / 2 + 1;
      }
      i = m % 2 + y + 1;
      e = (sand_cell_rec *)(sand_grid + (j + i * 104) * 36);
      n = 0;
      if (e->vtx[0] > 0) n = 1;
      e->vtx[n] = v;
      v += 6;
    }
  }
}

void sand_ground_update(void)
{
  sand_ground_frame.frame = sand_ground_frame.frame + 1;
  sand_dirty_list_clear();
  sand_bodies_collect((int)sand_ground_frame.frame);
  sand_bodies_press_all();
  sand_ground_vertices_write_dirty();
  return;
}

void sand_ground_vertices_write_all(void)
{
  uchar *d;
  sand_cell_rec *e;
  uint c;
  int i;
  int j;
  d = (uchar *)sand_ground_model;
  if (d == 0) return;
  for (i = 1; i < 0x66; i++) {
    for (j = 1; j < 0x67; j++) {
      e = (sand_cell_rec *)sand_grid + (j + i * 0x68);
      c = sand_cell_normal_packed(j,i);
      if (e->vtx[0] > 0) {
        *(uint *)(d + e->vtx[0] * 4) &= 0xff000000;
        *(uint *)(d + e->vtx[0] * 4) |= c;
        *(float *)(d + e->vtx[0] * 4 + 4) = e->pos.x;
        *(float *)(d + e->vtx[0] * 4 + 8) = e->pos.y;
        *(float *)(d + e->vtx[0] * 4 + 0xc) = e->pos.z;
        if (e->vtx[1] > 0) {
          *(uint *)(d + e->vtx[1] * 4) &= 0xff000000;
          *(uint *)(d + e->vtx[1] * 4) |= c;
          *(float *)(d + e->vtx[1] * 4 + 4) = e->pos.x;
          *(float *)(d + e->vtx[1] * 4 + 8) = e->pos.y;
          *(float *)(d + e->vtx[1] * 4 + 0xc) = e->pos.z;
        }
      }
    }
  }
}

void sand_ground_vertices_write_dirty(void)
{
  uchar *buf;
  ushort *p;
  ushort v;
  sand_cell_rec *e;
  uint c;

  buf = (uchar *)sand_ground_model;
  if (buf != (uchar *)0x0) {
    p = sand_dirty_cells;
    for (v = *p++; v != 0xffff; v = *p++) {
      e = (sand_cell_rec *)sand_grid + v;
      c = sand_cell_normal_packed((ushort)(v % 0x68), (ushort)(v / 0x68));
      if (0 < e->vtx[0]) {
        {
          uint t = *(uint *)(buf + e->vtx[0] * 4);
          *(uint *)(buf + e->vtx[0] * 4) = t & 0xff000000;
        }
        *(uint *)(buf + e->vtx[0] * 4) |= c;
        *(float *)(buf + e->vtx[0] * 4 + 8) = e->pos.y;
        if (0 < e->vtx[1]) {
          {
            uint t = *(uint *)(buf + e->vtx[1] * 4);
            *(uint *)(buf + e->vtx[1] * 4) = t & 0xff000000;
          }
          *(uint *)(buf + e->vtx[1] * 4) |= c;
          *(float *)(buf + e->vtx[1] * 4 + 8) = e->pos.y;
        }
      }
      *(ushort *)&e->flags &= 0xfffe;
    }
    sand_dirty_cells[0] = 0xffff;
  }
}

uint sand_cell_normal_packed(int col, int row)
{
  int i;
  int k;
  float *a;
  float *b;
  float *q;
  float p[3][3];
  float n[3];
  typedef struct { int a; int b; } ent;

  col += row * 0x68;
  for (i = 0; i < 3; i++) {
    q = (float *)(i * 12 + (int)p);
    ent *t = (ent *)sand_normal_pairs;
    k = (row % 2) * 3 + i;
    int d = (int)sand_grid;
    a = (float *)((t[k].a + col) * 0x24 + d);
    b = (float *)((t[k].b + col) * 0x24 + d);
    q[0] = (a[0] + b[0]) * 0.5;
    q[1] = (a[1] + b[1]) * 0.5;
    q[2] = (a[2] + b[2]) * 0.5;
  }
  vec3_tri_normal(p[0], p[1], p[2], n);
  return (uchar)(int)(n[0] * 127.0) | (uchar)(int)(n[1] * 127.0) << 8 |
         (uchar)(int)(n[2] * 127.0) << 16;
}

void sand_motion_lengths(vec3 a, float *len, float *hlen, float *ratio)
{
  float r;
  float zero;
  uint b[3];

  b[0] = *(uint *)&a.x;
  b[2] = *(uint *)&a.z;
  zero = 0.0;
  *(float *)&b[1] = zero;
  *len = vec3_length(&a.x);
  *hlen = vec3_length((float *)b);
  if ((*len <= zero) || (*hlen <= zero)) {
    *ratio = 1.0;
  }
  else {
    vec3_normalize((float *)b);
    r = vec3_dot(&a.x,(float *)b);
    r = r / *len;
    if (1.0 < r) {
      r = 1.0;
    }
    if (r < 0.0) {
      r = 0.0;
    }
    *ratio = r;
  }
}

void sand_heading_sector(vec3 dir, uint *heading, uint *sector, float hlen)
{
  float r = hlen;
  uint *p4 = heading;
  uint *p5 = sector;
  float axis[3];
  float c;
  int a;

  { float *q = axis; float z; *q = 1.0; z = 0.0; q++; *q = z; q++; *q = z; }

  if (r <= 0.0) {
    *p4 = 0;
  }
  else {
    dir.y = 0.0;
    c = vec3_dot((float *)&dir,axis) / r;
    if (c > 1.0) c = 1.0;
    if (c < -1.0) c = -1.0;
    if (dir.z > 0.0) a = acos_ang16(c);
    else a = -acos_ang16(c);
    a = (ushort)a; *p4 = a;
    *p5 = *(volatile uint *)p4 - 0x1555;
    if ((int)*p5 >= 0) {
      *p5 = (int)*(volatile uint *)p5 / 0x2aaa + 1;
      if ((int)*p5 > 5) *p5 = 0;
    }
    else {
      *p5 = 0;
    }
  }
}

void sand_bodies_collect(uint frame)
{
  vec3 d;
  float p[3];
  float q[3];
  int idx[4];
  vec3 a;
  vec3 b;
  vec3 v;
  float hlen;
  float ratio;
  float len;
  sand_joint_rec *e;
  int j;
  int i;
  char *m0;
  char *m1;
  float dist;
  float seg;
  float s;
  int num;
  int cnt;
  sand_joint_rec *tbl;
  sand_body_table *tb;

  tb = (sand_body_table *)&sand_bodies;
  tb->count = 0;
  tbl = sand_body_joints;
  for (j = 0; j < 2; j++) {
    m0 = (char *)rob_get(j & 0xff);
    m1 = (char *)((rob_rec *)m0)->bone_mtx_prev;
    m0 = (char *)((rob_rec *)m0)->bone_mtx;
    for (i = 0; i < 5; i++) {
      tb->bodies[tb->count].prev_dir = tb->bodies[tb->count].dir;
      p[0] = *(float *)(m0 + tbl[i].bone * 0x40 + 0x30);
      p[1] = *(float *)(m0 + tbl[i].bone * 0x40 + 0x34) - tbl[i].y_offset;
      p[2] = *(float *)(m0 + tbl[i].bone * 0x40 + 0x38);
      q[0] = *(float *)(m1 + tbl[i].bone * 0x40 + 0x30);
      q[1] = *(float *)(m1 + tbl[i].bone * 0x40 + 0x34) - tbl[i].y_offset;
      q[2] = *(float *)(m1 + tbl[i].bone * 0x40 + 0x38);
      d.x = p[0] - q[0];
      d.y = p[1] - q[1];
      d.z = p[2] - q[2];
      sand_motion_lengths(d, &len, &hlen, &ratio);
      tb->bodies[tb->count].pos = *(vec3 *)p;
      tb->bodies[tb->count].dir = d;
      tb->bodies[tb->count].len = len;
      tb->bodies[tb->count].hlen = hlen;
      tb->bodies[tb->count].ratio = ratio;
      tb->bodies[tb->count].radius = tbl[i].radius;
      tb->bodies[tb->count].kind = tbl[i].kind;
      tb->bodies[tb->count].flags |= 4;
      tb->bodies[tb->count].flags &= ~8;
      if (tbl[i].kind == 0 && hlen > 0.05 && ratio > 0.8) {
        tb->bodies[tb->count].flags |= 8;
        sand_heading_sector(d, &tb->bodies[tb->count].heading, &tb->bodies[tb->count].sector, hlen);
      }
      tb->count++;
    }
  }
  m0 = (char *)rob_get(frame & 1);
  m0 = (char *)((rob_rec *)m0)->bone_mtx;
  if (((int)frame >> 1 & 1) == 0) {
    tbl = sand_extra_joints_a;
  }
  else {
    tbl = sand_extra_joints_b;
  }
  for (j = 0; j < 7; j++) {
    p[0] = *(float *)(m0 + tbl[j].bone * 0x40 + 0x30);
    p[1] = *(float *)(m0 + tbl[j].bone * 0x40 + 0x34);
    p[2] = *(float *)(m0 + tbl[j].bone * 0x40 + 0x38);
    if (tbl[j].bone2 >= 0) {
      p[0] = (*(float *)(m0 + tbl[j].bone2 * 0x40 + 0x30) + p[0]) * 0.5;
      p[1] = (*(float *)(m0 + tbl[j].bone2 * 0x40 + 0x34) + p[1]) * 0.5;
      p[2] = (*(float *)(m0 + tbl[j].bone2 * 0x40 + 0x38) + p[2]) * 0.5;
    }
    tb->bodies[tb->count].pos.x = p[0];
    tb->bodies[tb->count].pos.y = p[1] - tbl[j].y_offset;
    tb->bodies[tb->count].pos.z = p[2];
    tb->bodies[tb->count].radius = tbl[j].radius;
    tb->bodies[tb->count].kind = tbl[j].kind;
    tb->bodies[tb->count].flags &= ~4;
    tb->count++;
  }
  idx[0] = 0;
  idx[1] = 1;
  idx[2] = 5;
  idx[3] = 6;
  for (j = 0, cnt = 0; j < 4 && cnt < 8; j++) {
    a = tb->bodies[idx[j]].pos;
    v.x = -tb->bodies[idx[j]].dir.x;
    v.y = -tb->bodies[idx[j]].dir.y;
    v.z = -tb->bodies[idx[j]].dir.z;
    dist = signed_sqrt(v.x * v.x + v.z * v.z);
    seg = tb->bodies[idx[j]].radius;
    num = dist / seg;
    for (i = 1; i < num && cnt < 8; i++) {
      s = ((float)i * seg) / dist;
      b.x = s * v.x + a.x;
      b.y = s * v.y + a.y;
      b.z = s * v.z + a.z;
      tb->bodies[tb->count].pos = b;
      tb->bodies[tb->count].radius = seg;
      tb->bodies[tb->count].kind = 0;
      tb->bodies[tb->count].flags &= ~4;
      tb->count++;
      cnt++;
    }
  }
}

void sand_bodies_press_all(void)
{
  sand_body_table *table;
  sand_body_rec *p;
  int i;

  table = (sand_body_table *)&sand_bodies;
  p = table->bodies;
  for (i = 0; i < table->count; i++, p++) {
    sand_body_press(p);
    if ((p->flags & 4) != 0) {
      sand_body_state_update(p);
    }
  }
}

float sand_cell_neighbour_height(uint cell, int highest)
{
  float best;
  float v;
  int k;
  int idx;
  int m;

  best = -110.0;
  m = (int)cell / 0x68 % 2;
  for (k = 0; k < 6; k++) {
    int (*tab)[6] = sand_cell_neighbours;
    idx = tab[m][k] + cell;
    if (((sand_cell_rec *)sand_grid)[idx].vtx[0] != 0) {
      if (!(best > -100.0)) {
        best = ((sand_cell_rec *)sand_grid)[idx].pos.y;
      }
      else if (highest != 0) {
        v = ((sand_cell_rec *)sand_grid)[idx].pos.y;
        if (v > best) best = v;
      }
      else {
        v = ((sand_cell_rec *)sand_grid)[idx].pos.y;
        if (v < best) best = v;
      }
    }
  }
  if (best <= -100.0) best = ((sand_cell_rec *)sand_grid)[cell].pos.y;
  return best;
}

void sand_body_press(sand_body_rec *body)
{
  typedef struct { int dx; int dy; } step;
  vec3 pos;
  int gx;
  int gy;
  int now;
  int old;
  uint low;
  float rr;
  float sc;
  float d;
  float h;
  float lim;
  int mode;
  int i;
  int idx;
  int nx;
  int ny;
  int m;
  int k;
  sand_cell_rec *e;

  old = body->flags & 1;
  low = 0;
  body->flags &= ~3;
  pos = body->pos;
  body->pressed = 0.0;
  if (sand_world_to_cell(pos.x,pos.z,&gx,&gy) == 0) {
    now = sand_ground_frame.frame;
    rr = body->radius * body->radius;
    if (pos.y < sand_level + 0.2) low = 1;
    sc = 0.01;
    mode = 0;
    if (body->kind == 0) {
      mode = 1;
      if ((body->flags & 8) != 0) mode = 2;
    }
    for (i = 0; i < 7; i++) {
      if ((mode == 1) & (i > 0)) break;
      if ((mode == 2) & (i > 0)) {
        float fx = gx;
        float *t = sand_press_dirs[0];
        m = gy % 2;
        k = body->sector + i;
        nx = (int)(fx + t[k * 5 + m * 2 - 4]);
        if ((uint)(nx - 2) > 99) continue;
        ny = (int)((float)gy + t[k * 5 + m * 2 - 3]);
        if ((uint)(ny - 2) > 98) continue;
        idx = nx + ny * 0x68;
        if (((sand_cell_rec *)sand_grid)[idx].vtx[0] == 0) continue;
        d = sc * t[k * 5 - 5];
      }
      else {
        float dx;
        float dz;
        int o;
        step *s;
        ny = gy;
        o = i + ny % 2 * 7;
        o <<= 3;
        s = (step *)sand_press_steps;
        s = (step *)((char *)s + o);
        nx = gx + s->dx;
        if ((uint)nx > 103) continue;
        ny += s->dy;
        if ((uint)ny > 102) continue;
        idx = nx + ny * 0x68;
        e = (sand_cell_rec *)sand_grid;
        e += idx;
        if (e->vtx[0] == 0) continue;
        dx = e->pos.x - pos.x;
        dz = e->pos.z - pos.z;
        if (dx * dx + dz * dz > rr) continue;
        d = e->pos.y - (pos.y - fsqrt_inl(rr - (dx * dx + dz * dz)));
        if (d < 0.0) d = 0.0;
        if (mode == 2) {
          sc = d;
          if (d < 0.02) sc = 0.02;
          if (sc > 0.1) sc = 0.1;
        }
      }
      if (d > 0.0 || d < 0.0) {
        int t;
        e = (sand_cell_rec *)sand_grid;
        e += idx;
        t = e->press_frame;
        e->press_frame = now;
        if (now >= t + 6) {
          h = ((sand_cell_rec *)sand_grid)[idx].pos.y;
          if (d > 0.0) {
            body->pressed += d;
            lim = sand_cell_neighbour_height(idx,1) - sand_dig_limit;
            if (lim < sand_level - 0.05) lim = sand_level - 0.05;
            h -= d;
            if (h < lim) h = lim;
          }
          else {
            lim = sand_cell_neighbour_height(idx,0) + sand_dig_limit * sand_raise_limit_scale;
            h -= sand_raise_scale * d;
            if (h > lim) h = lim;
          }
          sand_cell_set_height(idx,h);
        }
      }
    }
  }
  body->flags = low | body->flags;
  if (old != low) body->flags |= 2;
}

void sand_body_state_update(sand_body_rec *body)
{
  float tmp;
  float v;
  int n;
  float *d;

  body->puff_power = 0.0;
  body->state = 0xff;
  if (body->flags & 2) {
    body->state_frames = 0;
    body->start_pos.x = body->pos.x - body->dir.x;
    body->start_pos.y = body->pos.y - body->dir.y;
    body->start_pos.z = body->pos.z - body->dir.z;
    body->move_len = body->len;
    body->start_pressed = body->pressed;
    body->travel = 1.0;
    body->puff_amount = 0.0;
  }
  else {
    n = body->state_frames;
    if (n < 300) {
      body->state_frames = n + 1;
    }
    if (body->state_frames < 2) {
      d = &body->move.x;
      d[0] = body->pos.x - body->start_pos.x;
      d[1] = body->pos.y - body->start_pos.y;
      d[2] = body->pos.z - body->start_pos.z;
      sand_motion_lengths(*(vec3 *)d,d + 3,&tmp,&v);
      body->move_ratio = v;
      body->curve = sand_puff_curve[(int)(v * 127.0)];
      if (body->flags & 1) {
        body->state = 2;
        sand_body_state_enter(body);
      }
      else {
        body->state = 0;
        sand_body_state_leave(body);
      }
    }
    else if (body->flags & 1) {
      body->state = 3;
      sand_body_state_inside(body);
    }
    else {
      body->state = 1;
      sand_body_state_above(body);
    }
  }
}

void sand_body_state_enter(sand_body_rec *body)
{
  return;
}

void sand_body_state_inside(sand_body_rec *body)
{
  return;
}

void sand_body_state_above(sand_body_rec *body)
{
float f; f = (&sand_kind_params[0][1])[(uint)body->kind * 4]; f *= body->puff_amount; body->puff_power = f;
}

void sand_body_state_leave(sand_body_rec *body)
{
  float s;
  float v;
  float t;
  float r;

  s = body->curve;
  v = body->move_len - 0.15;
  t = sand_kind_params[0][(uint)body->kind * 4];
  if (v < 0.0) v = 0.0;
  if (0.3 < v) v = 0.3;
  r = v * 3.33 * s * t;
  body->leave_power = r;
  if (body->state_frames == 1) {
    if (r < 0.2) r = 0.0;
    body->puff_power = r;
    body->puff_amount = r + 0.5;
    if (1.0 < r + 0.5) body->puff_amount = 1.0;
  }
}

void sand_dirty_list_clear(void)
{
  sand_dirty_count = 0;
  sand_dirty_cells[0] = 0xffff;
  return;
}

void sand_cell_set_height(int cell, float height)
{
  char *new_var;
  sand_cell_rec *e;
  int q;
  int j;
  int k;
  if (sand_dirty_count < 0x28)
  {
    ((sand_cell_rec *) sand_grid)[cell].pos.y = height;
    e = &((sand_cell_rec *) sand_grid)[cell];
    if ((e->flags & 1) == 0)
    {
      e->flags |= 1;
      sand_dirty_cells[sand_dirty_count] = cell;
      sand_dirty_count++;
    }
    q = (cell / 0x68) % 2;
    for (j = 0; j < 6; j++)
    {
      int *t = sand_cell_neighbours[0];
      k = cell + t[(q * 6) + j];
      {
        char *b = (char *) sand_grid;
        new_var = b;
        ;
      }
      if (((sand_cell_rec *) (new_var + (k * 36)))->vtx[0] != 0)
      {
        if ((((sand_cell_rec *) (new_var + (k * 36)))->flags & 1) == 0)
        {
          ushort *l = sand_dirty_cells;
          ((sand_cell_rec *) (new_var + (k * 36)))->flags |= 1;
          l[sand_dirty_count] = k;
          sand_dirty_count++;
        }
      }
    }

    sand_dirty_cells[sand_dirty_count] = 0xffff;
  }
}

int sand_world_to_cell(float x_pos, float z_pos, int *col, int *row)
{
  int bad;
  float y;
  float x;

  bad = 0;
  if (x_pos > 5.0 || x_pos < -5.0 || z_pos > 5.0 || z_pos < -5.0) {
    bad = 1;
  }
  if (bad) {
    return -1;
  }
  y = (z_pos + 5.6610003) * sand_cell_scale_z;
  if ((int)(y * 10.0) % 10 > 5) {
    y = y + 1.0;
  }
  if ((uint)(int)y > 102) {
    return -1;
  }
  x = x_pos + 6.5367565;
  if ((int)y & 1) {
    x = x + 0.06408585;
  }
  x = x * sand_cell_scale_x;
  if ((int)(x * 10.0) % 10 > 5) {
    x = x + 1.0;
  }
  if ((uint)(int)x > 103) {
    return -1;
  }
  *col = (int)x;
  *row = (int)y;
  return (((sand_cell_rec *)sand_grid)[(int)y * 0x68 + *col].vtx[0] != 0) - 1;
}

void sand_ground_draw(void)
{
  if (sand_ground_model != 0) {
    mtx_push(0);
    model_draw(sand_ground_model);
    mtx_pop(1);
  }
  return;
}

void sand_puffs_reset(void)
{
  int i;
  sand_puff_rec *p;

  sand_puff_lists[0][0] = 0xff;
  sand_puff_lists[1][0] = 0xff;
  sand_puff_cursor = 0;
  sand_puff_list_len = 0;
  sand_cloud_count = 0;
  sand_puff_list_new = 0;
  sand_puff_list_live = 0;
  p = sand_puffs;
  for (i = 0; i < 0x50; i++) {
    p[i].kind = 0xff;
  }
  return;
}

void sand_puffs_update(void)
{
  int i;
  sand_body_table *table = (sand_body_table *)&sand_bodies;

  sand_puff_list_new = sand_puff_list_new ^ 1;
  sand_puff_list_live = sand_puff_list_new + 1 & 1;
  sand_puff_cursor = 0;
  for (i = 0; i < 10; i++) {
    if ((i == 2) | (i == 7)) continue;
    if ((i == 3) | (i == 8)) continue;
    if ((i == 4) | (i == 9)) continue;
    sand_body_puffs_spawn(&table->bodies[i]);
  }
  sand_puffs_move();
  return;
}

sand_puff_rec * sand_puff_alloc(void)
{
  int p;
  uchar *n;
  int i;
  uint c;
  while (sand_puff_cursor < 0x50 && sand_puff_list_len < 0x50) {
    sand_puff_rec *t = sand_puffs;
    i = sand_puff_cursor;
    if (t[i].kind == 0xff) {
      c = sand_puff_list_new;
      n = (uchar *)sand_puff_list_len;
      p = (int)sand_puff_lists[c];
      n[p] = sand_puff_cursor;
      sand_puff_list_len = (int)++n;
      n[p] = 0xff;
      return &sand_puffs[(sand_puff_cursor = i + 1) - 1];
    }
    sand_puff_cursor++;
  }
  return (sand_puff_rec *)0x0;
}

void sand_puffs_move(void)
{
  uchar *src;
  uchar *dst;
  uint i;
  sand_puff_rec *e;
  sand_puff_list_len = 0;
  src = &sand_puff_lists[0][0] + sand_puff_list_new * 0x51;
  dst = &sand_puff_lists[0][0] + sand_puff_list_live * 0x51;
  for (i = *src++; i != 0xff; i = *src++) {
    e = sand_puffs + i;
    e->vel.y += e->gravity;
    e->pos.x += e->vel.x;
    e->pos.y += e->vel.y;
    e->pos.z += e->vel.z;
    if (e->kind < 4) {
      e->life--;
      if (e->cloud > 0.0) e->spin += 0x100;
      if (e->pos.y < sand_level || e->life < 0) {
        e->life = 0;
        e->kind = 0xff;
        if (e->cloud > 0.0) sand_cloud_count--;
      }
    }
    if (e->kind != 0xff) {
      dst[sand_puff_list_len] = i;
      sand_puff_list_len++;
    }
  }
  dst[sand_puff_list_len] = 0xff;
}

void sand_body_puffs_spawn(sand_body_rec *body)
{
  switch(body->state) {
  case 2: if (0.0 < body->puff_power) sand_puffs_spawn_enter(); break;
  case 3: if (0.0 < body->puff_power) sand_puffs_spawn_inside(); break;
  case 0: if (0.0 < body->puff_power) ((void (*)(void))sand_puffs_spawn_leave)(); break;
  case 1: if (0.0 < body->puff_power) ((void (*)(void))sand_puffs_spawn_above)(); break;
  default:
    break;
  }
  return;
}

void sand_vec_clamp_len_up(float *v, float len, float max_len)
{
  if (max_len < len) {
    float s = max_len / len;
    v[0] *= s;
    v[1] *= s;
    v[2] = s * v[2];
  }
  {
    float y = v[1];
    if (y < 0.0) {
      v[1] = y * -1.0;
    }
  }
}

void sand_puffs_emit(int kind, int count, vec3 pos, vec3 spread, float cloud_chance)
{
  vec3 z = {0.0, 0.0, 0.0};
  sand_puff_rec *puff;
  int r;
  float t;
  float a;
  float c;
  int i;

  for (i = 0; i < count; i++) {
    puff = sand_puff_alloc();
    if (puff == 0) return;
    puff->kind = kind;
    puff->cloud = 0.0;
    r = stage_fx_rand_next();
    puff->spin = r;
    puff->life = (r & 0xf) + 0x14;
    if (puff->life > 0x1f) {
      puff->life = 0x1f;
    }
    a = (float)(r & 0xff) * 0.0039216;
    if (a < cloud_chance && sand_cloud_count < 8) {
      puff->cloud = 1.0;
      sand_cloud_count = sand_cloud_count + 1;
      cloud_chance = cloud_chance - 0.1;
    }
    t = (float)(r >> 8 & 0xff) * 0.0039216;
    puff->pos.x = t * spread.x + pos.x;
    puff->pos.y = pos.y + t * spread.y;
    puff->pos.z = t * spread.z + pos.z;
    if (puff->cloud > 0.0) {
      puff->vel = z;
      puff->gravity = 0.0;
    }
    else {
      r = stage_fx_rand_next();
      a = (float)(r & 0xff) * 0.0003921;
      t = (float)(r >> 4 & 0xff) * 0.0003921;
      c = (float)(r >> 8 & 0xff) * 0.0003921;
      puff->vel.x = spread.x * a;
      puff->vel.y = spread.y * t;
      puff->vel.z = c * spread.z;
      puff->gravity = -0.001;
    }
  }
}

void sand_puffs_spawn_leave(sand_body_rec *body)
{
  int n;
  vec3 b;
  vec3 a;

  if ((sand_puff_kinds & 1) == 0) {
    return;
  }
  n = (int)(body->puff_power * 20.0);
  if (n > 20) {
    n = 20;
  }
  a = body->start_pos;
  b = body->move;
  sand_vec_clamp_len_up(&b.x,body->move_len,0.03);
  sand_puffs_emit(0,n,a,b,0.7);
}

void sand_puffs_spawn_above(sand_body_rec *body, uint unused1, uint unused2, uint unused3)
{
  ray3 a;
  float f;
  float g;
  int n;

  if (((sand_puff_kinds & 2) != 0) && (0.0 < (g = body->puff_amount))) {
    body->travel = body->travel + body->len;
    if (body->travel >= 0.15) {
      body->travel = 0.0;
      n = (int)(g * 15.0);
      if (10 < n) {
        n = 10;
      }
      body->puff_amount = body->puff_amount - 0.1;
      if (body->puff_amount < 0.0) {
        body->puff_amount = 0.0;
      }
      a.d.x = body->pos.x - body->dir.x;
      a.d.y = body->pos.y - body->dir.y;
      a.d.z = body->pos.z - body->dir.z;
      a.p = body->dir;
      f = a.d.y - sand_level;
      if (f < 0.25) {
        f = 0.6;
      }
      else {
        f = 0.5 - (f - 0.25);
        if (f < 0.1) {
          f = 0.1;
        }
      }
      sand_puffs_emit(1,n,a.d,a.p,f);
      return;
    }
  }
  return;
}

void sand_puffs_spawn_enter(void)
{
  return;
}

void sand_puffs_spawn_inside(void)
{
  return;
}

void sand_puffs_draw(void)
{
  uchar *pb;
  uchar b;

  pb = &sand_puff_lists[0][0] + (uint)sand_puff_list_live * 0x51;
  while ((b = *pb++) != 0xff) {
    sand_puff_rec *p = &(sand_puffs)[b];
    mtx_push(0);
    if (p->kind < 4) {
      mtx_translate_by_vec3(&p->pos.x);
      if (p->cloud > 0.0) {
        mtx_rot_y_s16(camera.yaw + 0x8000);
        mtx_rot_z_s16(p->spin);
        mtx_scale_xyz(1.5,1.5,1.5);
        int *ta = sand_cloud_model_ids;
        obj_draw(ta[~(int)p->life & 0x1f]);
      }
      else {
        mtx_rot_y_s16(p->spin);
        compiled_model_rec **tb = sand_grain_models;
        model_draw(tb[~(int)p->life & 0x1f]);
      }
    }
    mtx_pop(1);
  }
}

void sand_cloud_models_color_set(void)
{
  char *model;
  int i;

  i = 0;
  do {
    model = (char *)obj_model_get(sand_cloud_model_ids[i]);
    if (model != 0) {
      *(uint *)(model + 0x6c) = *(uint *)(sand_puff_config_ptr + 8);
    }
    i = i + 1;
  } while (i < 0x20);
  return;
}

void sand_puff_init(void)
{
  sand_puff_config_copy.grain_count = sand_puff_config.grain_count;
  sand_puff_config_copy.grain_color = sand_puff_config.grain_color;
  sand_puff_config_copy.cloud_color = sand_puff_config.cloud_color;
  sand_puff_config_copy.shrink = sand_puff_config.shrink;
  sand_puff_config_copy.gravity = sand_puff_config.gravity;
  sand_puff_config_copy.turn_scale = sand_puff_config.turn_scale;
  sand_puff_config_copy.size_min = sand_puff_config.size_min;
  sand_puff_config_copy.size_max = sand_puff_config.size_max;
  sand_puff_config_copy.dist_min = sand_puff_config.dist_min;
  sand_puff_config_copy.dist_max = sand_puff_config.dist_max;
  sand_puff_config_copy.speed_min = sand_puff_config.speed_min;
  sand_puff_config_copy.speed_max = sand_puff_config.speed_max;
  sand_puff_config_ptr = (uchar *)&sand_puff_config;
  sand_grain_models_bake();
  sand_cloud_models_color_set();
  return;
}

void sand_grains_seed(void)
{
  typedef struct { float w[3]; } vec3_blk;
  sand_grain_rec *e;
  vec3_blk zero = { { 0 } };
  vec3_blk d;
  float x0, dx, y0, dy, z0, dz;
  int i;
  int r;

  e = sand_grains;
  x0 = *(float *)(sand_puff_config_ptr + 0x28);
  dx = (*(float *)(sand_puff_config_ptr + 0x2c) - x0) / 255.0;
  y0 = *(float *)(sand_puff_config_ptr + 0x18);
  dy = (*(float *)(sand_puff_config_ptr + 0x1c) - y0) / 255.0;
  z0 = *(float *)(sand_puff_config_ptr + 0x20);
  dz = (*(float *)(sand_puff_config_ptr + 0x24) - z0) / 255.0;
  mtx_push(0);
  for (i = 0; i < *(int *)sand_puff_config_ptr; i++, e++) {
    int hi = stage_fx_rand_next() << 15;
    r = hi | stage_fx_rand_next();
    float *pd = d.w;
    d = zero;
    pd[0] = (float)(r & 0xff) * dx + x0;
    short *px = &e->spin_x;
    short *py;
    float f;
    *px = r;
    f = (float)((uint)(r << 2) >> 16) * *(float *)(sand_puff_config_ptr + 0x14);
    py = &e->spin_y;
    *py = f;
    e->spin_x_step = (r >> 2) & 0x7ff;
    e->spin_y_step = (r >> 4) & 0x7ff;
    mtx_load_identity();
    mtx_rot_x_s16(*px);
    mtx_rot_y_s16(*py);
    mtx_xform_point(pd,e->vel);
    f = (float)((r >> 8) & 0xff) * dz + z0;
    vec3_scale(e->pos,e->vel,f);
    f = (float)((r >> 16) & 0xff) * dy + y0;
    e->size = f;
  }
  mtx_pop(1);
}

void sand_grains_step(void)
{
  sand_grain_rec *p;
  int i;
  float t;
  p = sand_grains;
  for (i = 0; i < *(int *)sand_puff_config_ptr; i++, p++) {
    p->pos[0] = p->pos[0] + p->vel[0];
    p->pos[1] = p->pos[1] + p->vel[1];
    p->pos[2] = p->pos[2] + p->vel[2];
    t = p->size - *(float *)(sand_puff_config_ptr + 0xc);
    p->size = t;
    if (t < 0.0) {
      p->size = 0.0;
    }
    p->spin_x = p->spin_x + p->spin_x_step;
    p->spin_y = p->spin_y + p->spin_y_step;
    p->vel[1] = p->vel[1] - *(float *)(sand_puff_config_ptr + 0x10);
  }
}

void sand_grain_tri_build(sand_grain_rec *grain, float *verts, uint *normal)
{
  typedef struct { float x; float y; float z; } corner;
  corner m[3] = {{-0.01, 0.0, -0.0087}, {0.0, 0.0, 0.0087}, {0.01, 0.0, -0.0087}};
  float n[3];

  mtx_load_identity();
  mtx_rot_y_s16((int)grain->spin_y);
  mtx_rot_x_s16((int)grain->spin_x);
  vec3_tri_normal(&m[0].x, &m[1].x, &m[2].x, n);
  mtx_overwrite_translation(grain->pos);
  mtx_scale_xyz(grain->size,grain->size,grain->size);
  mtx_xform_points((float (*)[3])m,(float (*)[3])verts,3);
  *normal = ((int)(n[0] * 127.0) & 0xffU) | ((int)(n[1] * 127.0) & 0xffU) << 8 | ((int)(n[2] * 127.0) & 0xffU) << 0x10;
}

void sand_grain_models_bake(void)
{
  float verts[9];
  float uv[6];
  uint normal;
  int *res;
  int i;
  int n3;
  int cnt;
  int a;
  int b;
  compiled_model_rec *r;
  int k;
  int idx;
  int n;
  sand_grain_rec *src;
  compiled_model_rec **p1;
  compiled_model_rec **p2;
  compiled_model_rec **p3;

  sand_grains_seed();
  sand_grains_baked = 1;
  i = 0;
  do {
    sand_grain_models[i] = 0;
    i = i + 1;
  } while (i < 0x20);
  res = (int *)obj_model_get(0x1d008f);
  if (res == 0) {
    sand_grains_baked = 0;
  }
  else {
    n3 = *(int *)sand_puff_config_ptr * 3;
    cnt = *(int *)sand_puff_config_ptr;
    mtx_push(0);
    i = 0;
    a = n3 * 0x18;
    b = a + 0x60;
    do {
      p1 = sand_grain_models + i;
      r = elan_heap_alloc(n3 * 0x18 + 0xc0);
      *p1 = r;
      if (r == 0) {
        sand_grains_baked = 0;
        break;
      }
      n = 0;
      do {
        ((int *)sand_grain_models[i])[n] = res[n];
        n = n + 1;
      } while (n < 0x30);
      p2 = sand_grain_models + i;
      (*p2)->sphere[3] = 1.0f;
      (*p2)->size = a + 0xc0;
      (*p2)->triangle_count = cnt;
      (*p2)->vertex_count = n3;
      (*p2)->field_0x28 = 1;
      (*p2)->field_0x2c = 1;
      (*p2)->parts[0].size = b;
      *(int *)((char *)*p2 + 0x6c) = *(int *)(sand_puff_config_ptr + 4);
      *(int *)((char *)*p2 + 0x74) = *(int *)(sand_puff_config_ptr + 4);
      *(uint *)((char *)*p2 + 0xa0) &= 0xffffff3f;
      *(uint *)((char *)*p2 + 0xa4) &= 0xe7ffffff;
      *(uint *)((char *)*p2 + 0xa4) |= 0x08000000;
      *(uint *)((char *)*p2 + 0xa8) &= 0xfffe7fff;
      *(uint *)((char *)*p2 + 0xb0) &= 0xfffe7fff;
      *(int *)((char *)*p2 + 0xbc) = n3;
      src = sand_grains;
      n = 0;
      idx = 0x30;
      for (; n < *(int *)sand_puff_config_ptr; n++, src++) {
        uv[0] = 0.25f;
        uv[1] = 0.5f;
        uv[2] = 0.5f;
        uv[3] = 0.0f;
        uv[4] = 0.75f;
        uv[5] = 0.5f;
        sand_grain_tri_build(src, verts, &normal);
        k = 0;
        do {
          if (k == 2)
            ((uint *)sand_grain_models[i])[idx] = normal | 0xe0000000;
          else
            ((uint *)sand_grain_models[i])[idx] = normal;
          p3 = sand_grain_models + i;
          ((float *)*p3)[idx + 1] = verts[k * 3];
          ((float *)*p3)[idx + 2] = verts[k * 3 + 1];
          ((float *)*p3)[idx + 3] = verts[k * 3 + 2];
          ((float *)*p3)[idx + 4] = uv[k * 2];
          ((float *)*p3)[idx + 5] = uv[k * 2 + 1];
          k = k + 1;
          idx = idx + 6;
        } while (k < 3);
      }
      sand_grains_step();
      i = i + 1;
    } while (i < 0x20);
    mtx_pop(1);
  }
}

void sand_time_mark(float *start)
{
  int now;

  now = stopwatch_usec();
  *start = (float)now;
  return;
}

void sand_time_lap(float *start, float *lap, float *peak)
{
  float t;
  float d;

  if (sand_timing_skip_frames != 0) {
    if (sand_timing_last_frame != game_frame_count) {
      sand_timing_last_frame = game_frame_count;
      sand_timing_skip_frames = sand_timing_skip_frames + -1;
    }
  }
  else {
    t = (float)(int)stopwatch_usec();
    d = t - *start;
    *lap = d;
    if (*peak < d) {
      *peak = d;
    }
    *start = t;
  }
}

void sand_umi_draw_tail(void)
{
  return;
}

void sand_colors_default_set(void)
{
  sand_colors.grain_r = 0xd2;
  sand_colors.grain_g = 200;
  sand_colors.grain_b = 200;
  sand_colors.cloud_r = 0xd2;
  sand_colors.cloud_g = 0xb4;
  sand_colors.cloud_b = 0xa4;
  sand_grain_models_color_apply();
  sand_model_color_set();
  return;
}

void sand_grain_models_color_apply(void)
{
  uint color;
  int i;
  color = 0xff000000;
  color |= sand_colors.grain_r << 0x10 | sand_colors.grain_g << 8 | sand_colors.grain_b;
  for (i = 0; i < 0x20; i++) {
    if (sand_grain_models[i] != 0) {
      *(uint *)((char *)sand_grain_models[i] + 0x6c) = color;
    }
  }
  return;
}

void sand_model_color_set(void)
{
  char *p; uint c; int i;
  c = 0xff000000;
  c |= sand_colors.cloud_r << 0x10 | sand_colors.cloud_g << 8 | sand_colors.cloud_b;
  for (i = 0; i < 0x20; i++) {
    p = (char *)obj_model_get(sand_cloud_model_ids[i]);
    if (p) *(uint *)(p + 0x6c) = c;
  }
}
