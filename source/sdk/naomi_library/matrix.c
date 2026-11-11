/*
 * sdk/naomi_library/matrix.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char mtx_stack_selected; /* 0x0c41c79c */
extern float mtx_proj_stack_slots[48]; /* 0x0c41c7a0 */
extern float mtx_view_stack_slots[48]; /* 0x0c41c890 */
extern mtx_stack_record_set mtx_stack_records; /* 0x0c41c860 */

void mtx_xform_points(float (*src)[3], float (*dst)[3], int count)
{
  while (--count >= 0) {
    mtx_xform_point(*src++,*dst++);
  }
}

void mtx_xform_dirs(float (*src)[3], float (*dst)[3], int count)
{
  while (--count >= 0) {
    mtx_xform_dir(*src++,*dst++);
  }
}

void mtx_pop_all(void)
{
  mtx_store_wordwise(0);
  mtx_stack_records.active.top = mtx_stack_records.active.base;
  mtx_stack_records.active.depth = 0;
  mtx_set_current_wordwise(0);
  return;
}

void mtx_copy_or_current(float *dst, float *src)
{
  int i;
  if (dst == 0) {
    if (src != 0) {
      mtx_set_current_wordwise(src);
    }
  }
  else {
    if (src == 0) {
      mtx_store_wordwise(dst);
    }
    else {
      for (i = 0; i < 0x10; i++) {
        *dst++ = *src++;
      }
    }
  }
}

void mtx_determinant(float *mtx)
{
  char order [4];
  float copy [16];
  
  mtx_copy_or_current(copy,mtx);
  mtx_lu_pivoted(copy,order);
  return;
}

float * mtx_store_wordwise(float *mtx)
{
  float *stored;
  
  if (mtx == 0) {
    mtx = mtx_stack_records.active.top;
  }
  stored = xmtrx_to_words(mtx);
  return stored;
}

float * mtx_store_3x3(float *dst)
{
  float *stored;
  
  stored = 0;
  if (dst != 0) {
    stored = xmtrx_to_3x3_packed(dst);
  }
  return stored;
}

float * mtx_store_3x3_stride4(float *dst)
{
  float *stored;
  
  stored = 0;
  if (dst != 0) {
    stored = xmtrx_to_3x3_in_4x4(dst);
  }
  return stored;
}

float * mtx_store_4x3(float *dst)
{
  float *stored;
  
  stored = 0;
  if (dst != 0) {
    stored = xmtrx_to_4x3(dst);
  }
  return stored;
}

float * mtx_store(float *mtx)
{
  if (mtx == 0) {
    mtx = mtx_stack_records.active.top;
  }
  return xmtrx_to_pairs(mtx);
}

void mtx_cofactor_rows(float *mtx)
{
  float *rows;
  float cof[16];
  
  rows = mtx;
  if (mtx == 0) {
    rows = mtx_store_wordwise(0);
    mtx_store_wordwise(cof);
  }
  else {
    mtx_copy_or_current(cof,rows);
  }
  vec3_cross(rows + 4,rows + 8,cof);
  vec3_cross(rows + 8,rows,cof + 4);
  vec3_cross(rows,rows + 4,cof + 8);
  if (mtx == 0) {
    mtx_set_current_wordwise(cof);
  }
  else {
    mtx_copy_or_current(rows,cof);
  }
  return;
}

void mtx_peek_pushed(float *dst)
{
  float *pushed = mtx_stack_records.active.top - 16;
  if (dst == 0) {
    mtx_set_current(pushed);
  }
  else {
    mtx_copy_or_current(dst,pushed);
  }
  return;
}

float * mtx_stack_setup(mtx_stack_rec *stack, float *buffer, int slots)
{
  uint base;
  int aligned;
  
  base = (uint)buffer + 0x1f & 0xffffffe0;
  aligned = (uint)buffer == base;
  slots -= aligned == 0;
  if (0x7fff < slots) {
    slots = 0x7fff;
  }
  stack->limit = (short)slots;
  stack->depth = 0;
  stack->top = (float *)base;
  stack->base = (float *)base;
  mtx_load_identity();
  mtx_store_wordwise(stack->top);
  return stack->base;
}

int mtx_stacks_start(float *buffer, int depth)
{
  mtx_stack_rec *rec = &mtx_stack_records.view;
  mtx_stack_rec *first;
  mtx_stack_setup(rec,mtx_view_stack_slots,3);
  mtx_stack_setup(rec + 1,mtx_proj_stack_slots,3);
  first = rec - 1;
  mtx_stack_setup(first,buffer,depth + 1);
  mtx_stack_selected = '\x01';
  rec = rec - 2;
  rec[0] = first[0];
}

float mtx_inverse_lu(float *mtx)
{
  float *m;
  char order[4];
  float inv[16];
  char i;
  char j;
  char k;
  char row;
  float det;
  float sum;
  
  m = mtx;
  if (m == 0) {
    m = mtx_store_wordwise(0);
  }
  det = mtx_lu_pivoted(m,order);
  if (det != 0.0) {
    for (j = 0; j < 4; j++) {
      for (i = 0; i < 4; i++) {
        row = order[i];
        sum = 1.0;
        if (row != j) sum = 0.0;
        for (k = 0; k < i; k++) {
          sum = sum - m[row * 4 + k] * inv[k * 4 + j];
        }
        inv[i * 4 + j] = sum;
      }
      for (i = 3; i >= 0; i--) {
        sum = inv[i * 4 + j];
        row = order[i];
        for (k = i + 1; k < 4; k++) {
          sum = sum - m[row * 4 + k] * inv[k * 4 + j];
        }
        inv[i * 4 + j] = sum / m[row * 4 + i];
      }
    }
    mtx_copy_or_current(m,inv);
    if (mtx == 0) {
      mtx_set_current_wordwise(m);
    }
  }
  return det;
}

void mtx_set_rot_3x3(float *src)
{
  if (src != 0) {
    xmtrx_from_3x3_packed(src);
  }
  return;
}

void mtx_set_rot_3x3_stride4(float *src)
{
  if (src != 0) {
    xmtrx_from_3x3_of_4x4(src);
  }
  return;
}

void mtx_set_from_4x3(float *src)
{
  if (src != 0) {
    xmtrx_from_4x3(src);
  }
  return;
}

void mtx_mul_eye_to_target_roll(float *eye, float *target, uint roll)
{
  float dir[3];
  float h;
  float inv_xz;
  float s;

  if ((roll & 0xffff) != 0) {
    mtx_rot_z_s16(roll & 0xffff);
  }
  dir[0] = target[0] - eye[0];
  dir[1] = target[1] - eye[1];
  dir[2] = target[2] - eye[2];
  h = dir[0] * dir[0] + dir[2] * dir[2];
  inv_xz = rsqrt_float(h);
  h = rsqrt_float(h + dir[1] * dir[1]);
  mtx_rot_x_sincos(-dir[1] * h,h / inv_xz);
  mtx_rot_y_sincos(inv_xz * dir[0],-dir[2] * inv_xz);
  mtx_translate_by_minus_v(eye);
}

void mtx_mul_eye_target_up(float *eye, float *target, float *up)
{
  float m[16];
  float side[3];
  float new_up[3];
  float fwd[3];

  vec3_sub(fwd,target,eye);
  vec3_cross(up,fwd,side);
  vec3_cross(fwd,side,new_up);
  vec3_normalize(side);
  vec3_normalize(new_up);
  vec3_normalize(fwd);
  m[0] = side[0];
  m[4] = side[1];
  m[8] = side[2];
  m[12] = 0.0f;
  m[1] = new_up[0];
  m[5] = new_up[1];
  m[9] = new_up[2];
  m[13] = 0.0f;
  m[2] = fwd[0];
  m[6] = fwd[1];
  m[10] = fwd[2];
  m[14] = 0.0f;
  m[3] = 0.0f;
  m[7] = 0.0f;
  m[11] = 0.0f;
  m[15] = 1.0f;
  mtx_mul_by(m);
  mtx_translate_by_minus_v(eye);
}

void mtx_mul_eye_target_up_flip(float *eye, float *target, float *up)
{
  float m[16];
  float side[3];
  float new_up[3];
  float back[3];

  back[0] = eye[0] - target[0];
  back[1] = eye[1] - target[1];
  back[2] = eye[2] - target[2];
  vec3_cross(up, back, side);
  vec3_cross(back, side, new_up);
  vec3_normalize(side);
  vec3_normalize(new_up);
  vec3_normalize(back);
  m[0] = side[0];
  m[4] = side[1];
  m[8] = side[2];
  m[12] = 0.0;
  m[1] = new_up[0];
  m[5] = new_up[1];
  m[9] = new_up[2];
  m[13] = 0.0;
  m[2] = back[0];
  m[6] = back[1];
  m[10] = back[2];
  m[14] = 0.0;
  m[3] = 0.0;
  m[7] = 0.0;
  m[11] = 0.0;
  m[15] = 1.0;
  mtx_mul_by(m);
  mtx_translate_by_minus_v(eye);
}

void mtx_set_current_wordwise(float *mtx)
{
  if (mtx == 0) {
    mtx = mtx_stack_records.active.top;
  }
  xmtrx_from_words(mtx);
  return;
}

void mtx_set_current(float *mtx)
{
  if (mtx == 0) {
    mtx = mtx_stack_records.active.top;
  }
  xmtrx_from_pairs(mtx);
  return;
}

float mtx_lu_pivoted(float *mtx, char *order)
{
  float scale[4];
  float *given;
  int i;
  int j;
  int k;
  int row;
  char pivot_row;
  float big;
  float tmp;
  float det;
  float pivot;

  given = mtx;
  if (mtx == 0) {
    mtx = mtx_store_wordwise(0);
  }
  det = 0.0;
  for (i = 0; i < 4; i++) {
    order[i] = i;
    big = 0.0;
    for (k = 0; k < 4; k++) {
      tmp = fabs(((float (*)[4])mtx)[i][k]);
      if (tmp > big) big = tmp;
    }
    if (big == 0.0) goto out;
    scale[i] = 1.0 / big;
  }
  det = 1.0;
  for (i = 0; i < 4; i++) {
    big = -1.0;
    for (j = i; j < 4; j++) {
      row = order[j];
      tmp = fabs(((float (*)[4])mtx)[row][i]) * scale[row];
      if (tmp > big) {
        big = tmp;
        k = j;
      }
    }
    pivot_row = order[k];
    if (k != i) {
      order[k] = order[i];
      order[i] = pivot_row;
      det = -det;
    }
    pivot = ((float (*)[4])mtx)[pivot_row][i];
    det *= pivot;
    if (pivot == 0.0) goto out;
    for (j = i + 1; j < 4; j++) {
      row = order[j];
      tmp = ((float (*)[4])mtx)[row][i] /= pivot;
      for (k = i + 1; k < 4; k++) {
        ((float (*)[4])mtx)[row][k] -= tmp * ((float (*)[4])mtx)[pivot_row][k];
      }
    }
  }
  if (given == 0) {
    mtx_set_current_wordwise(mtx);
  }
out:
  return det;
}

void mtx_mul_plane_reflect(float *plane)
{
  float m[16];
  float *n;
  float x, y, z, d;

  n = plane + 3;
  x = n[0];
  m[0] = 1.0 - (x + x) * x;
  y = n[1];
  m[5] = 1.0 - (y + y) * y;
  z = n[2];
  m[10] = 1.0 - (z + z) * z;
  m[1] = m[4] = x * -2.0 * y;
  m[2] = m[8] = x * -2.0 * z;
  m[6] = m[9] = y * -2.0 * z;
  d = vec3_dot(plane, n);
  d = d + d;
  m[12] = n[0] * d;
  m[13] = n[1] * d;
  m[14] = d * n[2];
  m[3] = 0.0;
  m[7] = 0.0;
  m[11] = 0.0;
  m[15] = 1.0;
  mtx_mul_by(m);
}

void mtx_stack_select(int stack)
{
  mtx_stack_rec *stacks;
  mtx_store_wordwise(0);
  stacks = (mtx_stack_rec *)&mtx_stack_records;
  stacks[mtx_stack_selected] = stacks[0];
  mtx_stack_selected = (char)stack;
  stacks[0] = stacks[(char)stack];
  mtx_set_current_wordwise(stacks[0].top);
  return;
}

void mtx_times_last_pushed(void)
{
  mtx_mul_by_pairs(mtx_stack_records.active.top - 16);
  return;
}

void mtx_premul_view_top(void)
{
  mtx_premul_by(mtx_stack_records.view.top);
  return;
}

void mtx_push_view_times_current(void)
{
  mtx_push(mtx_stack_records.view.top);
  mtx_times_last_pushed();
  return;
}

void mtx_push_then_view(void)
{
  mtx_push(mtx_stack_records.view.top);
  return;
}

void mtx_mul_plane_flatten(float *plane)
{
  float m[16];
  float *n;
  float d;

  n = plane + 3;
  m[0] = 1.0 - n[0] * n[0];
  m[5] = 1.0 - n[1] * n[1];
  m[10] = 1.0 - n[2] * n[2];
  m[1] = m[4] = -n[0] * n[1];
  m[2] = m[8] = -n[0] * n[2];
  m[6] = m[9] = -n[1] * n[2];
  d = vec3_dot(plane,n);
  m[12] = n[0] * d;
  m[13] = n[1] * d;
  m[14] = n[2] * d;
  m[3] = 0;
  m[7] = 0;
  m[11] = 0;
  m[15] = 1.0;
  mtx_mul_by(m);
}

void mtx_mul_plane_flatten_along(float *plane, float *from, float *to)
{
  float m[4][4];
  float v[3];
  float *n;
  float inv;
  float s;

  v[0] = to[0] - from[0];
  v[1] = to[1] - from[1];
  v[2] = to[2] - from[2];
  n = plane + 3;
  inv = 1.0 / (n[0] * v[0] + n[1] * v[1] + n[2] * v[2]);
  s = inv * n[0];
  m[0][0] = 1.0 - s * v[0];
  m[0][1] = -v[1] * s;
  m[0][2] = -v[2] * s;
  s = inv * n[1];
  m[1][0] = -v[0] * s;
  m[1][1] = 1.0 - s * v[1];
  m[1][2] = -v[2] * s;
  s = inv * n[2];
  m[2][0] = -v[0] * s;
  m[2][1] = -v[1] * s;
  m[2][2] = 1.0 - s * v[2];
  s = vec3_dot(plane, n) * inv;
  m[3][0] = v[0] * s;
  m[3][1] = v[1] * s;
  m[3][2] = v[2] * s;
  m[0][3] = 0.0;
  m[1][3] = 0.0;
  m[2][3] = 0.0;
  m[3][3] = 1.0;
  mtx_mul_by((float *)m);
  mtx_translate_by_vec3(from);
}

void mtx_stack_drop(int count)
{
  mtx_stack_drop_in(count,&mtx_stack_records.active);
  return;
}

bool mtx_push_identity(void)
{
  bool had_room;
  
  had_room = mtx_push(0);
  mtx_load_identity();
  return had_room != 0;
}

void mtx_rot_axis_s16(float *axis, uint angle)
{
  float unit[3];
  unit[0] = axis[0];
  unit[1] = axis[1];
  unit[2] = axis[2];
  vec3_normalize(unit);
  mtx_rot_unit_axis_s16(unit,angle);
}

void mtx_rot_unit_axis_s16(float *axis, uint angle)
{
  float sine;
  float cosine [1];
  
  fsca_sincos16(angle,&sine,cosine);
  mtx_rot_unit_axis_sincos(axis,sine,cosine[0]);
  return;
}

void mtx_rot_unit_axis_sincos(float *axis, float sine, float cosine)
{
  float m[16];
  float t;
  float x;
  float y;
  float z;
  float xy;
  float yz;
  float zx;
  float xs;
  float ys;
  float zs;
  float xx;
  float yy;
  float zz;

  t = 1.0f - cosine;
  x = axis[0];
  y = axis[1];
  z = axis[2];
  xy = x * y * t;
  yz = y * z * t;
  zx = z * x * t;
  xs = x * sine;
  ys = y * sine;
  zs = sine * z;
  xx = x * x;
  yy = y * y;
  zz = z * z;
  m[0] = (1.0f - xx) * cosine + xx;
  m[1] = xy + zs;
  m[2] = zx - ys;
  m[4] = xy - zs;
  m[5] = (1.0f - yy) * cosine + yy;
  m[6] = yz + xs;
  m[8] = zx + ys;
  m[9] = yz - xs;
  m[10] = (1.0f - zz) * cosine + zz;
  m[14] = 0.0f;
  m[13] = 0.0f;
  m[12] = 0.0f;
  m[11] = 0.0f;
  m[7] = 0.0f;
  m[3] = 0.0f;
  m[15] = 1.0f;
  mtx_mul_by(m);
}

void mtx_rot_x_rad_reversed(float angle)
{
  float sine;
  float cosine;
  
  sine = (float)sin_radians(angle);
  cosine = cos_radians(angle);
  mtx_rot_x_sincos_opposite(sine,cosine);
  return;
}

void mtx_rot_y_rad_reversed(float angle)
{
  float sine;
  float cosine;
  
  sine = (float)sin_radians(angle);
  cosine = cos_radians(angle);
  mtx_rot_y_sincos_opposite(sine,cosine);
  return;
}

void mtx_rot_z_rad_reversed(float angle)
{
  float sine;
  float cosine;
  
  sine = (float)sin_radians(angle);
  cosine = cos_radians(angle);
  mtx_rot_z_sincos_opposite(sine,cosine);
  return;
}

void mtx_rot_vec_to_vec(uint *from, float *to)
{
  float sine;
  float cosine;
  float axis[3];
  
  vec3_cross((float *)from,to,axis);
  sine = vec3_normalize(axis);
  cosine = vec3_dot((float *)from,to);
  if (sine != 0.0f) {
    mtx_rot_unit_axis_sincos(axis,sine,cosine);
  }
  else if (cosine < 0.0f) {
    axis[0] = ((float *)from)[1];
    axis[1] = ((float *)from)[2];
    axis[2] = ((float *)from)[0];
    mtx_rot_unit_axis_sincos(axis,0.0f,-1.0f);
  }
  return;
}

void mtx_translate_by_minus_v(float *v)
{
  mtx_translate(-*v,-v[1],-v[2]);
  return;
}

void mtx_translate_by_vec3(float *v)
{
  mtx_translate(v[0],v[1],v[2]);
}

bool vec3_tri_normal(float *p0, float *p1, float *p2, float *out)
{
  float mag;
  float a[3];
  float b[3];

  a[0] = p1[0] - p0[0];
  a[1] = p1[1] - p0[1];
  a[2] = p1[2] - p0[2];
  b[0] = p2[0] - p1[0];
  b[1] = p2[1] - p1[1];
  b[2] = p2[2] - p1[2];
  vec3_cross(a,b,out);
  mag = (float)vec3_dot(out,out);
  if (mag == 0.0) return 0;
  mag = rsqrt_float(mag);
  out[0] = out[0] * mag;
  out[1] = out[1] * mag;
  out[2] = out[2] * mag;
  return 1;
}

bool vec3_tri_normal_flipped(float *p0, float *p1, float *p2, float *out)
{
  float mag;
  float a[3];
  float b[3];
  a[0] = p1[0] - p0[0];
  a[1] = p1[1] - p0[1];
  a[2] = p1[2] - p0[2];
  b[0] = p2[0] - p1[0];
  b[1] = p2[1] - p1[1];
  b[2] = p2[2] - p1[2];
  vec3_cross(b,a,out);
  mag = vec3_dot(out,out);
  if (mag == 0.0) return 0;
  mag = rsqrt_float(mag);
  out[0] = out[0] * mag;
  out[1] = out[1] * mag;
  out[2] = out[2] * mag;
  return 1;
}
