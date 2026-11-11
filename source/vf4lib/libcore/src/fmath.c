/*
 * vf4lib/libcore/src/fmath.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float atan_consts[33]; /* 0x0c3253dc */
extern float trig_bench_inputs[41]; /* 0x0c325460 */
extern float vec_to_local_mtx[16]; /* 0x0c55dc28 */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

float point_segment_distance(float *point,float *closest,float *seg_a,float *seg_b)
{
  float ox, oy, oz;
  float cx, cy, cz;
  float dx, dy, dz;
  float ex, ey, ez;
  float t, r, s;
  float ux, uy, uz, vx, vy, vz;
  float px, py, pz, qx, qy, qz;
  float a[3];
  float b[3];
  ox = point[0];
  oy = point[1];
  oz = point[2];
  cx = seg_a[0];
  dx = cx - ox;
  cy = seg_a[1];
  dy = cy - oy;
  cz = seg_a[2];
  dz = cz - oz;
  ex = seg_b[0] - cx;
  ey = seg_b[1] - cy;
  ez = seg_b[2] - cz;
  if (__builtin_fabsf(ex) + __builtin_fabsf(ey) + __builtin_fabsf(ez) <= 1.1920929e-07) {
    if (closest != 0) {
      closest[0] = cx;
      closest[1] = cy;
      closest[2] = cz;
    }
    return fsqrt_asm(dx * dx + dy * dy + dz * dz);
  }
  t = -(ex * dx + ey * dy + ez * dz) / (ex * ex + ey * ey + ez * ez);
  dx = ex * t + dx;
  dy = ey * t + dy;
  dz = ez * t + dz;
  a[0] = (seg_a[0] - ox) - dx;
  a[1] = (seg_a[1] - oy) - dy;
  a[2] = (seg_a[2] - oz) - dz;
  b[0] = (seg_b[0] - ox) - dx;
  b[1] = (seg_b[1] - oy) - dy;
  b[2] = (seg_b[2] - oz) - dz;
  if (vec3_dot(a, b) < 0.0) {
    if (closest != 0) {
      closest[0] = dx + ox;
      closest[1] = dy + oy;
      closest[2] = dz + oz;
    }
    return fsqrt_asm(dx * dx + dy * dy + dz * dz);
  }
  ux = seg_a[0] - ox;
  uy = seg_a[1] - oy;
  uz = seg_a[2] - oz;
  r = ux * ux + uy * uy + uz * uz;
  ux = seg_b[0] - ox;
  uy = seg_b[1] - oy;
  uz = seg_b[2] - oz;
  s = ux * ux + uy * uy + uz * uz;
  if (s < r) {
    r = s;
    seg_a = seg_b;
  }
  if (closest != 0) {
    closest[0] = seg_a[0];
    closest[1] = seg_a[1];
    closest[2] = seg_a[2];
  }
  return fsqrt_asm(r);
}

short atan2_s16(float y, float x)
{
  float *b = atan_consts;
  float pi = b[0];
  float half = b[1];
  float r;

  if (y <= x) {
    if (-y <= x) {
      if (-1.1920929e-07f <= x && x <= 1.1920929e-07f) r = 0.0f;
      else r = atan_kernel(y / x);
    }
    else {
      if (-1.1920929e-07f <= y && y <= 1.1920929e-07f) r = -half;
      else r = -half - atan_kernel(x / y);
    }
  }
  else {
    if (-y <= x) {
      if (-1.1920929e-07f <= y && y <= 1.1920929e-07f) r = half;
      else r = half - atan_kernel(x / y);
    }
    else {
      if (-1.1920929e-07f <= x && x <= 1.1920929e-07f) r = pi;
      else if (y > 0.0f) r = pi + atan_kernel(y / x);
      else r = atan_kernel(y / x) - pi;
    }
  }
  return (int)(b[3] * r);
}

short atan_s16(float x)
{
  float *b = atan_consts;
  float *p;
  float r;
  if (x > 1.0f) {
    p = b + 1;
    r = atan_kernel(1.0f / x);
    r = *p - r;
  }
  else if (x < -1.0f) {
    p = b + 27;
    r = atan_kernel(1.0f / x);
    r = *p - r;
  }
  else {
    r = atan_kernel(x);
  }
  return (int)(b[3] * r);
}

float atan_kernel(float x)
{
  float x2;
  float n;
  float d;
  x2 = x * x;
  n = x2 * 0.17441538 + 5.731022;
  d = x2 + 11.554501;
  n = n * x2 + 22.939714;
  d = d * x2 + 29.776684;
  n = n * x2 + 20.51093;
  d = d * x2 + 20.51093;
  return (x * n) / d;
}

float atan_kernel_copy(float x)
{
  float x2;
  float n;
  float d;
  
  x2 = x * x;
  n = x2 * 0.17441538f + 5.731022f;
  d = x2 + 11.554501f;
  n = n * x2 + 22.939714f;
  d = d * x2 + 29.776684f;
  n = n * x2 + 20.51093f;
  d = d * x2 + 20.51093f;
  return (x * n) / d;
}

float atan2_radians(float y, float x)
{
  if (y <= x) {
    if (-y <= x) {
      if ((-1.1920929e-07 <= x) && (x <= 1.1920929e-07)) {
        return 0.0;
      }
      return atan_kernel_copy(y / x);
    }
    if ((-1.1920929e-07 <= y) && (y <= 1.1920929e-07)) {
      return -1.5707964;
    }
    return -1.5707964 - atan_kernel_copy(x / y);
  }
  else {
    if (-y <= x) {
      if ((-1.1920929e-07 <= y) && (y <= 1.1920929e-07)) {
        return 1.5707964;
      }
      return 1.5707964 - atan_kernel_copy(x / y);
    }
    if ((-1.1920929e-07 <= x) && (x <= 1.1920929e-07)) {
      return 3.1415927;
    }
    if (0.0 < y) {
      return atan_kernel_copy(y / x) + 3.1415927;
    }
    return atan_kernel_copy(y / x) + -3.1415927;
  }
}

uint atan2_radians_to_s16(float y, float x)
{
  float angle;
  
  angle = (float)atan2_radians(y,x);
  return (int)(angle * 10430.378) & 0xffff;
}

void trig_benchmark_draw(void)
{
  int i;
  int j;
  int n;
  int a;
  float t;
  float *p;

  n = 41;
  text_cursor_to_cell(0,2);
  for (i = 0; i < n; i++) {
    p = &trig_bench_inputs[i];
    text_draw_formatted("atan:%+11.1f=%04X/%04X/%04X\n",*p,(ushort)atan2_s16(1.0f,*p),(ushort)atan2_radians_to_s16(1.0f,*p),(ushort)atan2_ang16(1.0f,*p));
  }
  text_cursor_to_cell(0x24,2);
  for (a = 0, i = 0; i < 32; i++, a += 0x800) {
    text_draw_formatted("sin:%04X=%+10.8f/%+10.8f\n",(ushort)a,sin_s16((short)a),sin_ang16(a));
  }
  text_cursor_to_cell(0x40,0x30);
  t = (int)stopwatch_usec();
  n = 41;
  for (i = 0; i < 100; i++)
    for (j = 0; j < n; j++) { float *tb = trig_bench_inputs; atan2_s16(tb[i],1.0f); }
  text_draw_formatted("dkAtan2:%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  n = 41;
  for (i = 0; i < 100; i++)
    for (j = 0; j < n; j++) { float *tb = trig_bench_inputs; atan2_radians_to_s16(tb[i],1.0f); }
  text_draw_formatted("f3Atan2:%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  n = 41;
  for (i = 0; i < 100; i++)
    for (j = 0; j < n; j++) { float *tb = trig_bench_inputs; atan2_ang16(tb[i],1.0f); }
  text_draw_formatted("nlAtan2:%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) sin_s16((short)j);
  text_draw_formatted("dkSin  :%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) sin_ang16(j);
  text_draw_formatted("nlSin  :%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) cos_s16((short)j);
  text_draw_formatted("dkCos  :%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) cos_ang16(j);
  text_draw_formatted("nlCOs  :%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) tan_s16((short)j);
  text_draw_formatted("dkTan  :%7.1f\n",(float)(int)stopwatch_usec() - t);
  t = (int)stopwatch_usec();
  for (j = 0, n = 0x10000; j < n; j += 0x10) tan_ang16(j);
  text_draw_formatted("nlTan  :%7.1f\n",(float)(int)stopwatch_usec() - t);
}

void vec_to_local(float *v)
{
  float *m;
  float dx;
  float dy;
  float dz;
  m = vec_to_local_mtx;
  mtx_store(m);
  dx = v[0] - m[12];
  dy = v[1] - m[13];
  dz = v[2] - m[14];
  v[0] = m[0] * dx + m[1] * dy + m[2] * dz;
  v[1] = m[4] * dx + m[5] * dy + m[6] * dz;
  v[2] = m[8] * dx + m[9] * dy + m[10] * dz;
}
