/*
 * sdk/libspr/lines.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_prefetch_line_at
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float line3d_split_length; /* 0x0c3259b0 */
extern float line3d_width; /* 0x0c3259b4 */
extern float line3d_screen_mtx[16]; /* 0x0c3259c8 */
extern uint line3d_opaque_header[8]; /* 0x0c325a08 */
extern uint line3d_alpha_header[8]; /* 0x0c325a28 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern float proj_rect_px_left; /* 0x0c32a704 */
extern float proj_rect_px_top; /* 0x0c32a708 */
extern float line3d_screen_half_w; /* 0x0c5cab24 */
extern float line3d_screen_half_h; /* 0x0c5cab28 */
extern float line3d_clip_left; /* 0x0c5cab2c */
extern float line3d_clip_top; /* 0x0c5cab30 */
extern mtx_stack_record_set mtx_stack_records; /* 0x0c41c860 */
extern line3d_step_vec line3d_piece_step; /* 0x0c5cab00 */
extern line3d_piece_start_pt line3d_piece_start; /* 0x0c5cab0c */
extern line3d_piece_end_pt line3d_piece_end; /* 0x0c5cab18 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void line3d_quads_opaque(float *a, float *b, uint colour)
{
  line3d_quads_emit(a,b,colour,0);
  return;
}

void line3d_quads_opaque_world(float *a, float *b, uint colour)
{
  line3d_quads_emit(a,b,colour,2);
  return;
}

int line3d_quads_alpha(float *a, float *b, uint colour)
{
  return line3d_quads_emit(a,b,colour,1);
}

void line3d_quads_alpha_world(float *a, float *b, uint colour)
{
  line3d_quads_emit(a,b,colour,3);
  return;
}

void line3d_runs_opaque(uint *runs)
{
  line3d_runs_emit(runs,0);
  return;
}

void line3d_runs_opaque_world(uint *runs)
{
  line3d_runs_emit(runs,2);
  return;
}

void line3d_runs_alpha(uint *runs)
{
  line3d_runs_emit(runs,1);
  return;
}

void line3d_runs_alpha_world(uint *runs)
{
  line3d_runs_emit(runs,3);
  return;
}

void line3d_split_length_set(float length)
{
  line3d_split_length = length;
  return;
}

void line3d_width_store(float width)
{
  line3d_width = width;
  return;
}

void line3d_alpha_blend_set(int src, uint dst)
{
  line3d_alpha_header[2] &= 0x1fffffff;
  line3d_alpha_header[2] &= 0xe3ffffff;
  line3d_alpha_header[2] |= src << 29;
  line3d_alpha_header[2] |= (dst & 7) << 26;
  return;
}

void line3d_fog_set(int flag)
{
  if (flag == 1) {
    line3d_opaque_header[2] &= 0xff3fffff;
    line3d_alpha_header[2] &= 0xff3fffff;
    line3d_opaque_header[2] |= 0 << 22; /* ORIGINAL SHAPE (corroborated 2026-11-10: the else branch's four statements with 2 are forced in this order, and line3d_alpha_blend_set above has the same clear-then-set steps, forced; bits 22-23 are the fog mode and 0 is a real mode): the two zero ORs leave no code, but the game loads both headers before storing either, which needs them; the same field setter in both branches, value 0 here and 2 below */
    line3d_alpha_header[2] |= 0 << 22;
  }
  else {
    line3d_opaque_header[2] &= 0xff3fffff;
    line3d_alpha_header[2] &= 0xff3fffff;
    line3d_opaque_header[2] |= 2 << 22;
    line3d_alpha_header[2] |= 2 << 22;
  }
  return;
}

int line3d_quads_emit(float *point_a, float *point_b, uint colour, uint mode)
{
  float a[4];
  float b[4];
  float r;
  int sq_addr;
  uint *cursor;

  sq_addr = line3d_header_send(mode);
  mtx_push(line3d_screen_mtx);
  mtx_mul_by_pairs(mtx_stack_records.proj.top);
  if ((mode & 2) != 0) {
    mtx_mul_by_pairs(mtx_stack_records.view.top);
  }
  mtx_times_last_pushed();
  mtx_xform_point_xyzw(point_a,a);
  mtx_xform_point_xyzw(point_b,b);
  mtx_pop(1);
  if (seg_clip_near_z(a,b) == 0) {
    return 0;
  }
  if (seg_clip_to_screen(a,b) == 0) {
    return 0;
  }
  r = 1.0 / a[2];
  a[0] = r * a[0] + line3d_screen_half_w + proj_rect_px_left;
  a[1] = r * a[1] + line3d_screen_half_h + proj_rect_px_top;
  r = 1.0 / b[2];
  b[0] = r * b[0] + line3d_screen_half_w + proj_rect_px_left;
  b[1] = r * b[1] + line3d_screen_half_h + proj_rect_px_top;
  sq_addr = line3d_screen_quads(sq_addr,(line3d_piece_start_pt *)a,(line3d_piece_start_pt *)b,&colour);
  if ((mode & 1) != 0) {
    cursor = &list_write_ptrs[2];
  }
  else {
    cursor = list_write_ptrs;
  }
  *cursor = *cursor & 0xfc000000 | sq_addr & 0x3ffffff;
  return 1;
}

int line3d_runs_emit(uint *runs, uint mode)
{
  uint n;
  uint i;
  uint *next;
  int sq_addr;
  uint *cursor;
  float a[4];
  float b[4];
  uint colour;
  float inv;

  sq_addr = line3d_header_send(mode);
  mtx_push(line3d_screen_mtx);
  mtx_mul_by_pairs(mtx_stack_records.proj.top);
  if ((mode & 2) != 0) {
    mtx_mul_by_pairs(mtx_stack_records.view.top);
  }
  mtx_times_last_pushed();
  while (*runs != 0) {
    n = *runs++;
    colour = *runs++;
    for (i = 0; i < n; i++) {
      mtx_xform_point_xyzw((float *)runs,a);
      next = runs + 3;
      mtx_xform_point_xyzw((float *)next,b);
      if (seg_clip_near_z(a,b) == 0) {
        runs = next;
        continue;
      }
      if (seg_clip_to_screen(a,b) == 0) {
        runs += 3;
        continue;
      }
      {
        inv = 1.0 / a[2];
        a[0] = inv * a[0] + line3d_screen_half_w + proj_rect_px_left;
        a[1] = inv * a[1] + line3d_screen_half_h + proj_rect_px_top;
        inv = 1.0 / b[2];
        b[0] = inv * b[0] + line3d_screen_half_w + proj_rect_px_left;
        b[1] = inv * b[1] + line3d_screen_half_h + proj_rect_px_top;
        sq_addr = line3d_screen_quads(sq_addr,(line3d_piece_start_pt *)a,(line3d_piece_start_pt *)b,&colour);
      }
      runs += 3;
    }
    runs += 3;
  }
  mtx_pop(1);
  if ((mode & 1) != 0) {
    cursor = &list_write_ptrs[2];
  }
  else {
    cursor = list_write_ptrs;
  }
  *cursor = *cursor & 0xfc000000 | sq_addr & 0x3ffffff;
  return 1;
}

int line3d_header_send(uint mode)
{
  uint *src;
  uint addr;
  uint word;
  int i;
  volatile uint *qacr;

  line3d_screen_half_w = proj_rect_px_width * 0.5;
  line3d_screen_half_h = proj_rect_px_height * 0.5;
  line3d_clip_left = -1.0 - line3d_screen_half_w;
  line3d_clip_top = -1.0 - line3d_screen_half_h;
  line3d_screen_mtx[0] = line3d_screen_half_w;
  line3d_screen_mtx[5] = -line3d_screen_half_h;
  if (mode & 1) {
    src = line3d_alpha_header;
    addr = list_write_ptrs[2];
  }
  else {
    src = line3d_opaque_header;
    addr = list_write_ptrs[0];
  }
  word = *src & 0xfffcffff;
  *src = word;
  *src = poly_header_bits16 << 16 | word | 0x8c0000;
  qacr = (volatile uint *)0xff000038;
  qacr[0] = addr >> 24;
  qacr[1] = addr >> 24;
  addr = addr & 0x3ffffff | 0xe0000000;
  for (i = 0; i < 8; i++) {
    ((uint *)addr)[i] = *src++;
  }
  prefetch_line_at(addr);
  addr += 0x20;
  return addr;
}

void seg_cut_at_near_z(float *a, float *b, float *out)
{
  float z;
  float t;
  z = graphics_work.near_z;
  t = (z - a[2]) / (b[2] - a[2]);
  out[2] = z;
  *out = (*b - *a) * t + *a;
  out[1] = (b[1] - a[1]) * t + a[1];
  return;
}

uint point_screen_outcode(float *point)
{
  uint r;
  float x;
  float y;
  float w;
  float a;
  float b;
  float c;
  float d;
  float k1;
  float k2;
  float k3;
  
  r = 0;
  x = point[0];
  y = point[1];
  w = point[2];
  a = line3d_screen_half_w;
  a *= w;
  k1 = line3d_screen_half_h;
  b = k1 * w;
  k2 = line3d_clip_left;
  c = k2 * w;
  k3 = line3d_clip_top;
  d = k3 * w;
  if (x > a) r = 2;
  else if (x < c) r |= 1;
  if (y > b) r |= 8;
  else if (y < d) r |= 4;
  return r;
}

void seg_cut_at_x_edge(float *a, float *b, float *out, float edge)
{
  float cut[3];
  float a0;
  float a1;
  float a2;
  float dx;
  float dy;
  float dz;
  float t;

  a0 = a[0];
  a1 = a[1];
  a2 = a[2];
  dx = b[0] - a0;
  dy = b[1] - a1;
  dz = b[2] - a2;
  if (dz == 0.0) {
    cut[2] = a2;
    cut[0] = edge * a2;
    if (dx == 0.0) cut[1] = out[1];
    else cut[1] = ((cut[0] - a0) * dy) / dx + a1;
  }
  else {
    t = (a0 * dz - dx * a2) / (dz * edge - dx);
    cut[2] = t;
    cut[0] = edge * t;
    cut[1] = (dy * (t - a2)) / dz + a1;
  }
  out[0] = cut[0];
  out[1] = cut[1];
  out[2] = cut[2];
}

void seg_cut_at_y_edge(float *a, float *b, float *out, float edge)
{
  float o[3];
  float x = a[0];
  float y = a[1];
  float z = a[2];
  float dx = b[0] - x;
  float dy = b[1] - y;
  float dz = b[2] - z;
  if (dz == 0.0) {
    o[2] = z;
    o[1] = edge * z;
    if (dy == 0.0) {
      o[0] = *out;
    }
    else {
      o[0] = ((o[1] - y) * dx) / dy + x;
    }
  }
  else {
    o[2] = (y * dz - dy * z) / (dz * edge - dy);
    o[1] = edge * o[2];
    o[0] = (dx * (o[2] - z)) / dz + x;
  }
  out[0] = o[0];
  out[1] = o[1];
  out[2] = o[2];
}

int seg_clip_to_screen(float *a, float *b)
{
  uint c1;
  uint c2;
  uint c;
  float *q;

  for (;;) {
    c1 = point_screen_outcode(a);
    c2 = point_screen_outcode(b);
    if ((c1 & c2) != 0) return 0;
    if ((c1 == 0) & (c2 == 0)) return 1;
    c = c1 ? c1 : c2;
    q = c1 ? a : b;
    if (c & 1) seg_cut_at_x_edge(a, b, q, line3d_clip_left);
    else if (c & 2) seg_cut_at_x_edge(a, b, q, line3d_screen_half_w);
    else if (c & 4) seg_cut_at_y_edge(a, b, q, line3d_clip_top);
    else if (c & 8) seg_cut_at_y_edge(a, b, q, line3d_screen_half_h);
  }
}

int seg_clip_near_z(float *a, float *b)
{
  float t = graphics_work.near_z;
  if (t > a[2]) {
    if (t > b[2]) return 0;
    seg_cut_at_near_z(a,b,a);
  }
  else if (t > b[2]) {
    seg_cut_at_near_z(a,b,b);
  }
  return 1;
}

void quad_vertices_send(uint *dst, uint *src, uint *colour)
{
  extern void prefetch_line_at(); /* the file leaves it undeclared (0c10ff20 reads r0) */
  int i;
  uint colour_word;

  colour_word = *colour;
  for (i = 0; i < 4; i++) {
    if (i == 3) *dst = 0xf0000000; else *dst = 0xe0000000;
    dst++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = colour_word;
    *dst++ = 0;
    *dst++ = colour_word;
    *dst = 0;
    prefetch_line_at(dst);
    dst++;
  }
}

void seg_quad_ext_both(int sq_addr, float *a, float *b, uint *colour, float ox, float oy)
{
  float v[12];

  v[0] = oy + a[0] + ox;
  v[1] = (a[1] - ox) + oy;
  v[2] = a[2];
  v[3] = (a[0] - oy) + ox;
  v[4] = ox + a[1] + oy;
  v[5] = a[2];
  v[6] = (oy + b[0]) - ox;
  v[7] = (b[1] - ox) - oy;
  v[8] = b[2];
  v[9] = (b[0] - oy) - ox;
  v[10] = (ox + b[1]) - oy;
  v[11] = b[2];
  quad_vertices_send((uint *)sq_addr,(uint *)v,colour);
}

void seg_quad_ext_first(int sq_addr, float *a, float *b, uint *colour, float ox, float oy)
{
  float v [12];

  v[0] = oy + a[0] + ox;
  v[1] = (a[1] - ox) + oy;
  v[2] = a[2];
  v[3] = (a[0] - oy) + ox;
  v[4] = ox + a[1] + oy;
  v[5] = a[2];
  v[6] = oy + b[0];
  v[7] = b[1] - ox;
  v[8] = b[2];
  v[9] = b[0] - oy;
  v[10] = ox + b[1];
  v[11] = b[2];
  quad_vertices_send((uint *)sq_addr,(uint *)v,colour);
}

void seg_quad_plain(int sq_addr, float *a, float *b, uint *colour, float ox, float oy)
{
  float v[12];
  
  v[0] = oy + a[0];
  v[1] = a[1] - ox;
  v[2] = a[2];
  v[3] = a[0] - oy;
  v[4] = ox + a[1];
  v[5] = a[2];
  v[6] = oy + b[0];
  v[7] = b[1] - ox;
  v[8] = b[2];
  v[9] = b[0] - oy;
  v[10] = ox + b[1];
  v[11] = b[2];
  quad_vertices_send((uint *)sq_addr,(uint *)v,colour);
}

void seg_quad_ext_second(int sq_addr, float *a, float *b, uint *colour, float ox, float oy)
{
  float v[12];

  v[0] = oy + a[0];
  v[1] = a[1] - ox;
  v[2] = a[2];
  v[3] = a[0] - oy;
  v[4] = ox + a[1];
  v[5] = a[2];
  v[6] = (oy + b[0]) - ox;
  v[7] = (b[1] - ox) - oy;
  v[8] = b[2];
  v[9] = (b[0] - oy) - ox;
  v[10] = (ox + b[1]) - oy;
  v[11] = b[2];
  quad_vertices_send((uint *)sq_addr,(uint *)v,colour);
}

void point_square_quad(int sq_addr, float *point, uint *colour, float half)
{
  float v[12];
  
  v[0] = point[0] - half;
  v[1] = point[1] - half;
  v[2] = point[2];
  v[3] = point[0] + half;
  v[4] = v[1];
  v[5] = v[2];
  v[6] = v[0];
  v[7] = point[1] + half;
  v[8] = v[2];
  v[9] = v[3];
  v[10] = v[7];
  v[11] = v[2];
  quad_vertices_send((uint *)sq_addr,(uint *)v,colour);
}

int line3d_screen_quads(int sq_addr, line3d_piece_start_pt *a, line3d_piece_start_pt *b, uint *colour)
{
  float dx;
  float dy;
  float d2;
  float hw;
  float len;
  float r;
  float sx;
  float sy;
  float inv;
  float bz;
  float *az;
  float *pbz;
  int ix;
  int iy;
  int i;
  int n;
  float inv2;
  
  dx = a->x - b->x;
  dy = a->y - b->y;
  d2 = dx * dx + dy * dy;
  hw = line3d_width * 0.5f;
  if (d2 == 0.0f) {
    if (a->inv_z <= b->inv_z) {
      point_square_quad(sq_addr, (float *)a, colour, hw);
    }
    else {
      point_square_quad(sq_addr, (float *)b, colour, hw);
    }
    return sq_addr + 0x80;
  }
  __asm__ ("fsqrt %0" : "=f"(len) : "0"(d2));
  r = 1.0f / len * hw;
  sx = dx * r;
  sy = dy * r;
  inv = 1.0f / line3d_split_length;
  ix = (int)(dx * inv);
  iy = (int)(dy * inv);
  if (ix < 0) ix = -ix;
  if (iy < 0) iy = -iy;
  n = iy;
  if (iy > ix) n = ix;
  az = &a->inv_z;
  *az = 1.0f / *az;
  pbz = &b->inv_z;
  bz = 1.0f / *pbz;
  *pbz = bz;
  if (n > 1) {
    inv2 = 1.0f / (float)n;
    line3d_piece_step.x = dx * inv2;
    line3d_piece_step.y = dy * inv2;
    line3d_piece_step.inv_z = (*az - bz) * inv2;
    line3d_piece_start.x = b->x;
    line3d_piece_end.x = line3d_piece_start.x + line3d_piece_step.x;
    line3d_piece_start.y = b->y;
    line3d_piece_end.y = line3d_piece_start.y + line3d_piece_step.y;
    line3d_piece_start.inv_z = *pbz;
    line3d_piece_end.inv_z = line3d_piece_start.inv_z + line3d_piece_step.inv_z;
    seg_quad_ext_second(sq_addr, (float *)&line3d_piece_end, (float *)&line3d_piece_start, colour, sx, sy);
    sq_addr += 0x80;
    for (i = 1; i < n - 1; i++) {
      line3d_piece_start.x += line3d_piece_step.x;
      line3d_piece_end.x += line3d_piece_step.x;
      line3d_piece_start.y += line3d_piece_step.y;
      line3d_piece_end.y += line3d_piece_step.y;
      line3d_piece_start.inv_z += line3d_piece_step.inv_z;
      line3d_piece_end.inv_z += line3d_piece_step.inv_z;
      seg_quad_plain(sq_addr, (float *)&line3d_piece_end, (float *)&line3d_piece_start, colour, sx, sy);
      sq_addr += 0x80;
    }
    line3d_piece_start.x += line3d_piece_step.x;
    line3d_piece_end.x += line3d_piece_step.x;
    line3d_piece_start.y += line3d_piece_step.y;
    line3d_piece_end.y += line3d_piece_step.y;
    line3d_piece_start.inv_z += line3d_piece_step.inv_z;
    line3d_piece_end.inv_z += line3d_piece_step.inv_z;
    seg_quad_ext_first(sq_addr, (float *)&line3d_piece_end, (float *)&line3d_piece_start, colour, sx, sy);
    sq_addr += 0x80;
  }
  else {
    seg_quad_ext_both(sq_addr, (float *)a, (float *)b, colour, sx, sy);
    sq_addr += 0x80;
  }
  return sq_addr;
}
