/*
 * vf4lib/libbase/src/field.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void field_coli_set(int old_base, field_coli_rec *coli)
{
  field_coli = coli;
  coli->field_0x44 = 0;
  field_coli->field_0x48 = 0;
  field_coli_relocate(old_base,coli);
  field_coli->block = *field_coli->blocks;
  return;
}

void field_coli_relocate(int old_base, field_coli_rec *coli)
{
  int polys;
  int delta;
  
  if (old_base != (int)coli) {
    delta = (int)coli - old_base;
    coli->verts = (float *)((int)coli->verts + delta);
    polys = delta + (int)coli->polys;
    coli->polys = (coli_poly_rec *)polys;
    coli->grid = (coli_grid_rec *)((int)coli->grid + delta);
    coli->cells = (coli_poly_rec ***)((int)coli->cells + delta);
    coli_polys_relocate((coli_poly_rec *)polys,coli->poly_count,delta);
    coli_cell_table_relocate((int *)coli->cells,coli->cell_count,delta);
    if (coli->wall_root != 0) {
      coli->wall_root = (int *)(delta + (int)coli->wall_root);
      coli_list_relocate(coli->wall_root,delta);
    }
    if (coli->groups != 0) {
      coli->groups = (int *)(delta + (int)coli->groups);
      coli_groups_relocate(coli->groups,coli->entry_count,delta);
    }
    if (coli->blocks != 0) {
      coli->blocks = (int *)(delta + (int)coli->blocks);
      coli_ptrs_relocate(coli->blocks,coli->entry_count,delta);
    }
  }
  return;
}

void coli_polys_relocate(coli_poly_rec *poly, int count, int delta)
{
  int i;
  int *pp;
  int j;
  
  for (i = 0; i < count; i++, poly++) {
      if (poly->field_0x10 != 0) {
        poly->field_0x10 += delta;
      }
      if (poly->field_0x14 != 0) {
        poly->field_0x14 += delta;
      }
      pp = (int *)poly->verts;
      j = 0;
      do {
        *pp = *pp + delta;
        j = j + 1;
        pp = pp + 1;
      } while (j < 5);
  }
  return;
}

void coli_cell_table_relocate(int *cells, int count, int delta)
{
  int *base;
  int *lo;
  int i;
  int p;

  base = cells;
  lo = cells;
  for (i = 0; i < count; i++, cells++) {
    p = *cells;
    p += delta;
    *cells = p;
    if ((int)p < (int)lo) {
      lo = (int *)p;
    }
  }
  p = (int)lo;
  while (1) {
    if (!((int)p < (int)base)) break;
    if (*(int *)p != 0) {
      *(int *)p += delta;
    }
    p += 4;
  }
}

void coli_list_relocate(int *list, int delta)
{
  int *p;
  int *q;
  int e;
  int v;
  p = (int *)*list;
  p = (int *)((char *)p + delta);
  *list = (int)p;
  for (; (e = *p) != 0; p++) {
    e += delta;
    q = (int *)(delta + *(int *)(e + 4));
    *(int **)(e + 4) = q;
    *p = e;
    for (; (v = *q) != 0; q++) {
      *q = v + delta;
    }
  }
}

void coli_groups_relocate(int *groups, int count, int delta)
{
  int i;
  int *pp;
  int o;
  int *a;
  int *b;
  
  for (i = 0; i < count; i++, groups++) {
    pp = (int *)*groups;
    pp = (int *)((int)pp + delta);
    *groups = (int)pp;
    for (o = *pp; o != 0; o = *++pp) {
      o += delta;
      *pp = o;
      *(int *)(o + 0x10) += delta;
      *(int *)(o + 0x14) += delta;
      *(int *)(o + 0x18) += delta;
      *(int *)(o + 0x1c) += delta;
      *(int *)(o + 0x20) += delta;
      *(int *)(o + 0x24) += delta;
      a = *(int **)(o + 0x18);
      b = *(int **)(o + 0x1c);
      while (*a != 0) {
        *a = delta + *a;
        coli_polys_relocate((coli_poly_rec *)*a,1,delta);
        a++;
        *b = delta + *b;
        coli_polys_relocate((coli_poly_rec *)*b,1,delta);
        b++;
      }
    }
  }
  return;
}

void coli_ptrs_relocate(int *ptrs, int count, int delta)
{
  int i;
  
  for (i = 0; i < count; i = i + 1, ptrs = ptrs + 1) {
      *ptrs = *ptrs + delta;
  }
  return;
}

int coli_poly_point_inside(coli_poly_rec *poly, float *point)
{
  float **pa;
  float **pb;
  float *normal;
  float *edge;
  float *rel;
  uint n;
  float edge_buf[3];
  float rel_buf[3];

  pa = poly->verts;
  pb = &poly->verts[1];
  normal = poly->normal;
  edge = edge_buf;
  rel = rel_buf;
  for (n = poly->vertex_count; n != 0; n--) {
    vec3_sub(edge,*pb,*pa);
    vec3_sub(rel,point,*pa);
    vec3_cross(edge,rel,edge);
    if (vec3_dot(normal,edge) < 0.0) return 0;
    pa++;
    pb++;
  }
  return 1;
}

int coli_grid_cell_at(float *pos, coli_grid_rec *grid)
{
  int x;
  int y;
  int r;
  
  x = (int)((*pos - grid->x0) * grid->scale);
  y = (int)((pos[2] - grid->z0) * grid->scale);
  r = x + y * grid->width;
  if (r < 0 || r >= grid->cell_count) {
    r = -1;
  }
  return r;
}

int coli_grid_cells_along(ushort *cells, float *end, float *start)
{
  struct { float pos[3]; coli_grid_rec *grid; int prev; } walk;
  float step = 1.0;
  float dx, dz, len, inv, d;
  int steps, i, count, cell;
  float *walk_pos;

  dx = end[0] - start[0];
  dz = end[2] - start[2];
  len = dx * dx + dz * dz;
  if (len > step * step) {
    len = __builtin_fabsf(dx);
    if (__builtin_fabsf(dz) > len) {
      len = __builtin_fabsf(dz);
    }
    steps = (int)(len / step + 1.0);
    inv = 1.0 / (float)steps;
    dx *= inv;
    dz *= inv;
  }
  else {
    steps = 1;
  }
  walk_pos = walk.pos;
  walk.grid = field_coli->grid;
  walk.prev = -2;
  walk.pos[0] = start[0];
  walk.pos[1] = start[1];
  walk.pos[2] = start[2];
  count = 0;
  for (i = 0; i < steps; i++) {
    cell = coli_grid_cell_at(walk_pos, walk.grid);
    if (cell < 0) break;
    if (cell != walk.prev) {
      *cells++ = cell;
      count++;
    }
    walk_pos[0] += dx;
    walk_pos[2] += dz;
    walk.prev = cell;
  }
  *cells = 0xffff;
  return count;
}

coli_poly_rec * coli_segment_hit_nearest(float *out, float *end, float *start, uint mask)
{
  typedef struct { int w[3]; } blk;
  coli_poly_rec *best = 0;
  short *cell = (short *)coli_cells;
  float best_dist = 998001.0;
  float hit_buf[3];
  float dir_buf[3];
  float *dir = dir_buf;
  float *hit = hit_buf;
  int idx;
  coli_poly_rec **polys;
  coli_poly_rec *poly;
  uint flags;
  float *normal;
  float t;
  float end_dist;
  float dist;

  vec3_sub(dir, end, start);
  if (coli_grid_cells_along(cell, end, start) < 0) return 0;
  while (1) {
    idx = *cell++;
    if (idx < 0) break;
    if (idx < field_coli->cell_count) {
      polys = field_coli->cells[idx];
      for (poly = *polys++; poly != 0; poly = *polys++) {
        if (poly->field_0x0 != 0 && (mask & 2) == 0) continue;
        flags = poly->flags;
        if (flags & 0x80000400) {
          if ((mask & 0x40) == 0) continue;
        } else if (flags & 0x200) {
          if ((mask & 0x24) != 4) continue;
          if ((mask & 0x10) == 0 && poly->field_0xc == 0) continue;
        } else if (flags & 0x100) {
          if ((mask & 0x48) == 0 && poly->field_0xc == 0) continue;
        }
        normal = poly->normal;
        t = poly->dist;
        end_dist = vec3_dot(normal, end) - t;
        t = vec3_dot(normal, start) - t;
        if (end_dist < 0.0 && t >= 0.0) {
          t = -t / vec3_dot(normal, dir);
          hit[0] = start[0] + t * dir[0];
          hit[1] = start[1] + t * dir[1];
          hit[2] = start[2] + t * dir[2];
          if (coli_poly_point_inside(poly, hit) == 1) {
            dist = vec3_distance(hit, start);
            if (dist < best_dist) {
              *(blk *)out = *(blk *)hit;
              best_dist = dist;
              best = poly;
            }
          }
        }
      }
    }
  }
  return best;
}

coli_poly_rec * coli_sphere_hit_nearest(float *out, float *end, float *start, uint mask, float radius)
{
  typedef struct { float v[3]; } V;
  float hit_buf[3];
  float dir_buf[3];
  float seg[3];
  coli_poly_rec *best;
  short *cell;
  float best_dist;
  coli_poly_rec **polys;
  coli_poly_rec *poly;
  int idx;
  uint flags;
  float *normal;
  float t;
  float end_dist;
  float k;
  float dist;
  float *hit;
  float *dir;

  hit = hit_buf;
  dir = dir_buf;
  best = 0;
  cell = (short *)coli_cells;
  best_dist = 998001.0;
  vec3_sub(dir,end,start);
  if (coli_grid_cells_along(coli_cells,end,start) < 0) return 0;
  for (;;) {
    idx = *cell++;
    if (idx < 0) break;
    if (idx >= field_coli->cell_count) continue;
    polys = field_coli->cells[idx];
    while ((poly = *polys++) != 0) {
      if (poly->field_0x0 != 0 && (mask & 2) == 0) continue;
      flags = poly->flags;
      if (flags & 0x80000400) {
        if ((mask & 0x40) == 0) continue;
      }
      else if (flags & 0x200) {
        if ((mask & 0x24) != 4) continue;
        if ((mask & 0x10) == 0 && poly->field_0xc == 0) continue;
      }
      else if (flags & 0x100) {
        if ((mask & 0x48) == 0 && poly->field_0xc == 0) continue;
      }
      normal = poly->normal;
      t = poly->dist;
      end_dist = vec3_dot(normal,end) - t;
      t = vec3_dot(normal,start) - t;
      if (!(end_dist < radius)) continue;
      if (t < -radius) continue;
      t += radius;
      hit[0] = radius * normal[0] + start[0];
      hit[1] = radius * normal[1] + start[1];
      hit[2] = start[2] + radius * normal[2];
      k = -radius - radius;
      seg[0] = k * normal[0] + dir[0];
      seg[1] = k * normal[1] + dir[1];
      seg[2] = dir[2] + k * normal[2];
      t = -t / vec3_dot(normal,seg);
      hit[0] = t * seg[0] + hit[0];
      hit[1] = t * seg[1] + hit[1];
      hit[2] = t * seg[2] + hit[2];
      if (coli_poly_point_inside(poly,hit) == 1) {
        hit[0] = radius * normal[0] + hit[0];
        hit[1] = radius * normal[1] + hit[1];
        hit[2] = radius * normal[2] + hit[2];
        dist = vec3_distance(hit,start);
        if (dist < best_dist) {
          *(V *)out = *(V *)hit;
          best_dist = dist;
          best = poly;
        }
      }
    }
  }
  return best;
}

int coli_segment_hit_any(float *end, float *start, uint mask)
{
  coli_poly_rec **polys;
  coli_poly_rec *poly;
  float *normal;
  float hit_buf[3];
  float dir_buf[3];
  float t;
  float *hit = hit_buf;
  float *dir = dir_buf;
  float end_dist;

  polys = *field_coli->cells;
  if (polys == 0) return 0;
  if (field_coli->version != 0x3f2) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/field.c",0x28c,"Collision data version error!!");
    return 0;
  }
  if (end[1] < -1000.0 && start[1] < -1000.0) return 0;
  vec3_sub(dir,end,start);
  while ((poly = *polys++) != 0) {
    if (poly->field_0x0 != 0 && (mask & 2) == 0) continue;
    if ((int)poly->flags >= 0) {
      if (poly->flags & 0x200) {
        if (mask & 0x20) continue;
        if ((mask & 4) == 0) continue;
        if (poly->field_0xc == 0 && (mask & 0x10) == 0) continue;
      }
      if (poly->flags & 0x100) {
        if ((mask & 8) == 0) {
          if ((mask & 0x24) != 4) continue;
          if (poly->field_0xc <= 0x1000) continue;
        }
      }
    }
    normal = poly->normal;
    t = poly->dist;
    end_dist = vec3_dot(normal, end) - t;
    t = vec3_dot(normal, start) - t;
    if (((mask & 1) == 0 && end_dist * t < 0.0) || ((mask & 1) != 0 && end_dist < 0.0 && t >= 0.0)) {
      t = -t / vec3_dot(normal, dir);
      hit[0] = start[0] + t * dir[0];
      hit[1] = start[1] + t * dir[1];
      hit[2] = start[2] + t * dir[2];
      if (coli_poly_point_inside(poly, hit) == 1) return 1;
    }
  }
  return 0;
}

uint coli_flag_list_set(coli_wall_rec *wall)
{
  coli_poly_rec **pp;
  coli_poly_rec *poly;
  
  pp = wall->polys;
  while ((poly = *pp++) != 0) {
    poly->field_0x0 = 1;
  }
  return 1;
}

void coli_flag_list_mark(coli_wall_rec *wall)
{
  coli_poly_rec **pp;
  coli_poly_rec *poly;

  pp = wall->polys;
  while ((poly = *pp++) != 0) {
    poly->field_0x0 = 1;
  }
  return;
}

void coli_flag_list_clear(coli_wall_rec *wall)
{
  coli_poly_rec **pp;
  coli_poly_rec *poly;

  pp = wall->polys;
  while ((poly = *pp++) != 0) {
    poly->field_0x0 = 0;
  }
  return;
}

float field_floor_height(coli_poly_rec *poly_out, float *pos, int near, float rise)
{
  coli_poly_rec *poly;
  uint mask;
  float low[3];
  float high[3];
  float hit[3];
  float y;
  
  low[0] = high[0] = pos[0];
  low[2] = high[2] = pos[2];
  low[1] = -1000.01;
  high[1] = rise + pos[1];
  mask = 9;
  if (near != 0) {
    mask = 0x49;
  }
  poly = coli_segment_hit(hit,low,high,mask);
  if (poly != (coli_poly_rec *)0x0) {
    *poly_out = *poly;
    y = hit[1];
    coli_floor_poly = poly;
  }
  else {
    y = -1000.0;
    poly_out->flags = 0x400;
  }
  return y;
}
