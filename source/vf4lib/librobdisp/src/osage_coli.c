/*
 * vf4lib/librobdisp/src/osage_coli.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int cloth_normals_show; /* 0x0c2b0ff8 */
extern float osage_coli_draw_light; /* 0x0c2b1008 */

int osage_coli_push_out(float *point, float *mtx, osage_coli_rec *coli)
{
  int n;

  n = 0;
  mtx_push(0);
  for (; coli->type != 0; coli++) {
    mtx_set_current_wordwise(mtx + coli->slot * 0x10);
    switch(coli->type) {
    case 1:
      n += osage_coli_ball_push(point,&coli->shape);
      break;
    case 2:
      n += osage_coli_xy_push(point,&coli->shape);
      break;
    case 3:
      n += osage_coli_yz_push(point,&coli->shape);
      break;
    case 4:
      n += osage_coli_zx_push(point,&coli->shape);
      break;
    }
  }
  mtx_pop(1);
  return n;
}

bool osage_coli_ball_push(float *point, osage_coli_shape *shape)
{
  float v[3];
  float dx;
  float dy;
  float dz;
  float r;
  float d;
  float s;
  bool ret;

  mtx_xform_point(shape->pos,v);
  dx = point[0] - v[0];
  dy = point[1] - v[1];
  dz = point[2] - v[2];
  r = shape->radius;
  d = dx * dx + dy * dy + dz * dz;
  ret = 0;
  if (d < r * r) {
    s = r * rsqrt_float(d);
    point[0] = dx * s + v[0];
    point[1] = dy * s + v[1];
    point[2] = dz * s + v[2];
    ret = 1;
  }
  return ret;
}

int osage_coli_xy_push(float *point, osage_coli_shape *shape)
{
  int hit;
  vec_to_local_into(point,point);
  hit = osage_coli_capsule_push(*point,point[1],point[2],shape->pos[0],shape->pos[1],shape,point);
  mtx_xform_point(point,point);
  return hit;
}

int osage_coli_yz_push(float *point, osage_coli_shape *shape)
{
  int hit;
  float v[3];
  vec_to_local_into(point,point);
  hit = osage_coli_capsule_push(point[1],point[2],*point,shape->pos[1],shape->pos[2],shape,point);
  v[0] = point[2];
  v[1] = *point;
  v[2] = point[1];
  mtx_xform_point(v,point);
  return hit;
}

int osage_coli_zx_push(float *point, osage_coli_shape *shape)
{
  int hit;
  float v[3];
  vec_to_local_into(point,point);
  hit = osage_coli_capsule_push(point[0],point[2],point[1],shape->pos[0],shape->pos[2],shape,point);
  v[0] = point[0];
  v[1] = point[2];
  v[2] = point[1];
  mtx_xform_point(v,point);
  return hit;
}

int osage_coli_capsule_push(float u, float v, float w, float center_u, float center_v, osage_coli_shape *shape, float *out)
{
  int ret;
  float dy;
  float r;
  float lo;
  float hi;
  float dx;
  float dz;
  float z;
  float d;
  float s;
  ret = 0;
  r = shape->radius;
  lo = shape->lo;
  hi = shape->hi;
  dx = u - center_u;
  dy = v - center_v;
  if (w > lo && hi > w) {
    d = dx * dx + dy * dy;
    if (r * r > d) {
      s = r * (float)rsqrt_float(d);
      u = dx * s + center_u;
      v = dy * s + center_v;
      ret = 1;
    }
  }
  else {
    if (lo > w) z = lo;
    else z = hi;
    dz = w - z;
    d = dx * dx + dy * dy + dz * dz;
    if (r * r > d) {
      s = r * (float)rsqrt_float(d);
      u = dx * s + center_u;
      v = dy * s + center_v;
      w = dz * s + z;
      ret = 1;
    }
  }
  *out = u;
  out[1] = v;
  out[2] = w;
  return ret;
}

void cloth_draw(cloth_slot_rec *slot)
{
  if (slot->on == 1) {
    if (slot->obj->type == 9 || slot->obj->type == 0xf) {
      cloth_obj_draw(slot);
    }
    else {
      cloth_model_update(slot->obj_id,slot->points,slot->map_order);
      obj_draw_buffered(slot->obj_id);
    }
  }
}

void cloth_grid_lines_draw(cloth_slot_rec *slot)
{
  cloth_header_rec *header;
  cloth_point *e;
  int i;
  int j;
  header = slot->header;
  e = slot->points;
  for (i = 0; i < header->cols - 1; e++, i++)
    line3d_quads_opaque(e->pos, (e + 1)->pos, 0xff);
  e++;
  for (j = 1; j < header->rows; j++) {
    for (i = 0; i < header->cols - 1; e++, i++) {
      line3d_quads_opaque(e->pos, (e - header->cols)->pos, 0xff);
      line3d_quads_opaque(e->pos, (e + 1)->pos, 0xff);
    }
    line3d_quads_opaque(e->pos, (e - header->cols)->pos, 0xff);
    e++;
  }
}

void cloth_normal_lines_draw(int cols, int rows, float *points)
{
  int i;
  int j;
  float v[3];
  float k = 0.1;
  float *p = points;

  for (j = 0; j < cols; j++) {
    for (i = 0; i < rows; i++) {
      v[0] = p[0] + k * p[3];
      v[1] = p[1] + k * p[4];
      v[2] = p[2] + k * p[5];
      line3d_quads_opaque(p,v,0xffffffff);
      p = p + 6;
    }
  }
}

void cloth_model_write(obj_node *node, cloth_point *points, int map_order)
{
  int front;
  int back;
  compiled_model_rec *model;
  int base;

  if (node == 0) return;
  if (node->type == 9) cloth_header_ofs = (int *)&node->unknown_40;
  else if (node->type == 0xf) cloth_header_ofs = (int *)&node->list_80;
  else return;
  if (*cloth_header_ofs == 0) return;
  base = (int)node->file;
  cloth_model_header = (cloth_header_rec *)(base + *cloth_header_ofs);
  cloth_normals_build(cloth_model_header,points);
  if (node->type > 0 && frame_parity != 0) model = node->model2;
  else model = node->model;
  if (model == 0) return;
  if (cloth_normals_show != 0) {
    cloth_normal_lines_draw(cloth_model_header->cols,cloth_model_header->rows,(float *)points);
  }
  if (map_order != 0) {
    front = cloth_model_header->front_map;
    back = cloth_model_header->back_map;
  }
  else {
    front = cloth_model_header->back_map;
    back = cloth_model_header->front_map;
  }
  if (front != 0) cloth_front_vertices_write(model,(int *)(front + base),(uint *)points);
  if (back != 0) cloth_back_vertices_write(model,(int *)(back + base),(uint *)points);
}

void cloth_model_update(int obj_id, cloth_point *points, int map_order)
{
  obj_node *node;

  node = (obj_node *)obj_node_get(obj_id);
  cloth_model_write(node,points,map_order);
  return;
}

void cloth_front_vertices_write(compiled_model_rec *model, int *map, uint *points)
{
  uint packed;
  int off;
  uint *dst;
  uint flags;
  int i;
  int n;

  while (1) {
    n = *map++;
    if (n == 0) break;
    for (i = 0; i < n; i++) {
      off = *map++;
      flags = *map++;
      dst = (uint *)((char *)model + off);
      packed = normal_pack_bytes((float *)(points + 3));
      *dst = packed | flags;
      dst[1] = *points;
      dst[2] = points[1];
      dst[3] = points[2];
    }
    points = points + 6;
  }
}

void cloth_back_vertices_write(compiled_model_rec *model, int *map, uint *points)
{
  int n;
  int i;
  int off;
  uint fl;
  float *d;
  float v[3];

  while (1) {
    n = *map++;
    if (n == 0) break;
    for (i = 0; i < n; i++) {
      off = *map++;
      fl = *map++;
      d = (float *)((char *)model + off);
      v[0] = -((float *)points)[3];
      v[1] = -((float *)points)[4];
      v[2] = -((float *)points)[5];
      *(uint *)d = normal_pack_bytes(v) | fl;
      d[1] = ((float *)points)[0];
      d[2] = ((float *)points)[1];
      d[3] = ((float *)points)[2];
    }
    points = points + 6;
  }
}

void cloth_normals_build(cloth_header_rec *header, cloth_point *points)
{
  typedef struct { int w[3]; } v3;
  int n;
  int j;
  int rows;
  int i;

  n = header->cols;
  rows = header->rows;
  cloth_nrm_up = points;
  cloth_nrm_cur = points + n;
  cloth_nrm_right = cloth_nrm_cur + 1;
  for (j = 1; j < rows; j++) {
    for (i = 0; i < n - 1; i++) {
      vec3_tri_normal_flipped(cloth_nrm_cur->pos,cloth_nrm_up->pos,cloth_nrm_right->pos,cloth_nrm_cur->normal);
      cloth_nrm_cur = cloth_nrm_cur + 1;
      cloth_nrm_right = cloth_nrm_right + 1;
      cloth_nrm_up = cloth_nrm_up + 1;
    }
    *(v3 *)cloth_nrm_cur->normal = *(v3 *)cloth_nrm_cur[-1].normal;
    cloth_nrm_cur = cloth_nrm_cur + 1;
    cloth_nrm_right = cloth_nrm_right + 1;
    cloth_nrm_up = cloth_nrm_up + 1;
  }
}

void osage_coli_ball_draw(osage_coli_shape *shape)
{
  float *p = shape->pos;

  mtx_push(0);
  mtx_translate(p[0],p[1],p[2]);
  mtx_scale_xyz(shape->radius,shape->radius,shape->radius);
  obj_draw_translucent(0x46);
  mtx_pop(1);
}

void osage_coli_yz_draw(osage_coli_shape *shape)
{
  float len;

  len = shape->hi - shape->lo;
  mtx_push(0);
  mtx_translate(shape->lo,shape->pos[1],shape->pos[2]);
  mtx_scale_xyz(len,shape->radius,shape->radius);
  obj_draw_translucent(0x44);
  mtx_pop(1);
}

void osage_coli_zx_draw(osage_coli_shape *shape)
{
  float *p = shape->pos;
  float d;

  d = shape->hi - shape->lo;
  mtx_push(0);
  mtx_translate(p[0],shape->lo,p[2]);
  mtx_scale_xyz(shape->radius,d,shape->radius);
  mtx_rot_z_s16(0x4000);
  obj_draw_translucent(0x44);
  mtx_pop(1);
}

void osage_coli_xy_draw(osage_coli_shape *shape)
{
  float d;

  d = shape->hi - shape->lo;
  mtx_push(0);
  mtx_translate(shape->pos[0],shape->pos[1],shape->lo);
  mtx_scale_xyz(shape->radius,shape->radius,d);
  mtx_rot_y_s16(0xffffc000);
  obj_draw_translucent(0x44);
  mtx_pop(1);
}

void osage_coli_draw(osage_coli_rec *coli, rob_rec *rob)
{
  int mtx;

  extra_light_parallel_set(osage_coli_draw_light);
  mtx = (int)rob->mtx;
  while (coli->type != 0) {
    mtx_push(0);
    mtx_mul_by((float *)(mtx + coli->slot * 0x40));
    switch(coli->type) {
    case 1:
      osage_coli_ball_draw(&coli->shape);
      break;
    case 2:
      osage_coli_xy_draw(&coli->shape);
      break;
    case 3:
      osage_coli_yz_draw(&coli->shape);
      break;
    case 4:
      osage_coli_zx_draw(&coli->shape);
      break;
    }
    coli = coli + 1;
    mtx_pop(1);
  }
  extra_light_off();
}
