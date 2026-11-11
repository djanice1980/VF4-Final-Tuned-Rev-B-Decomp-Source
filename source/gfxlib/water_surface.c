/*
 * gfxlib/water_surface.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern water_state_rec water_state; /* 0x0c41347c */

static inline float tab_fsqrt(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void water_surface_init(float spacing)
{
  water_state_rec *w = &water_state;

  fft16_twiddles_build();
  w->flags = 0;
  *(float *)&w->grid_spacing = spacing;
  w->look[0] = 16.0;
  w->look[1] = 1.0;
  w->look[2] = 0.0;
  w->look[3] = 0.0;
  w->look[4] = 0.0;
  w->look[5] = 0.0;
  water_phases_randomize(w->waves);
}

void water_phases_randomize(water_wave *waves)
{
  int i;
  int j;
  float *p = (float *)waves;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      p[1] = (float)lcg_next_unit() * 6.2831855;
      p[3] = (float)lcg_next_unit() * 6.2831855;
      p[2] = 0;
      p[4] = 0;
      p += 5;
    }
  }
}

float * water_amplitude_table_build(float exponent, float *out)
{
  float s = exponent;
  float *d = out;
  float *p;
  int i;
  int j;

  *d = 0.0;
  p = d + 1;
  j = 0;
  i = 1;
  for (; j < 8; j++) {
    for (; i < 8; i++) {
      *p++ = 1.0 / pow_float(fsqrt_asm((float)(i * i + j * j)),s);
    }
    i = 0;
  }
  return d;
}

float * water_dispersion_table_build(float exponent, float *out)
{
  float s = exponent;
  float *ret = out;
  int i;
  int j;
  float *p;

  p = ret;
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 8; i++) {
      *p++ = pow_float(tab_fsqrt((float)(i * i + j * j)),s);
    }
  }
  return ret;
}

float * water_table_scale(float factor, float *table)
{
  float *p;
  int i;
  int j;
  float v;

  *table = 0.0f;
  p = table + 1;
  j = 0;
  i = 1;
  for (; j < 8; j++) {
    for (; i < 8; i++) {
      v = *p * factor;
      *p = v;
      if ((v < 1e-05) && (-1e-05 < v)) {
        *p = 1e-05;
      }
      p = p + 1;
    }
    i = 0;
  }
  return table;
}

void water_amplitudes_set(float *amplitudes)
{
  int i;
  water_wave *wave;
  int j;
  float a;

  wave = water_state.waves;
  j = 0;
  do {
    i = 0;
    do {
      a = *amplitudes;
      amplitudes = amplitudes + 1;
      wave->amplitude = a;
      wave = wave + 1;
      i = i + 1;
    } while (i < 8);
    j = j + 1;
  } while (j < 8);
  return;
}

void water_speeds_set(float *speeds1, float *speeds2)
{
  int i;
  water_wave *wave;
  int j;

  wave = water_state.waves;
  j = 0;
  do {
    i = 0;
    do {
      wave->speed1 = (*speeds1);
      speeds1 = 1 + speeds1;
      wave->speed2 = *speeds2;
      speeds2 = 1 + speeds2;
      wave = wave + 1;
      i = i + 1;
    } while (i < 8);
    j = 1 + j;
  } while (j < 8);
  return;
}

void water_look_pair_set(float a, float b)
{
  water_state.look[4] = a;
  water_state.look[5] = b;
  return;
}

void water_grid_spare_set(float value, uint *vec)
{
  water_grid_spare_value = value;
  water_grid_spare_vec.x = vec[0];
  water_grid_spare_vec.y = vec[1];
  water_grid_spare_vec.z = vec[2];
  return;
}

void water_colors_set(uint flags, float value, float unused, float color1_r, float color1_g, float color1_b, float color2_r, float color2_g, float color2_b, float look0, float look1)
{
  water_state_rec *w = &water_state;

  w->flags = flags;
  w->field_0x8 = value;
  w->color1_r = color1_r;
  w->color1_g = color1_g;
  w->color1_b = color1_b;
  if (flags & 1) {
    w->color2_r = color2_r;
    w->color2_g = color2_g;
    w->color2_b = color2_b;
  }
  else {
    w->color2_r = 0.0;
    w->color2_g = 0.0;
    w->color2_b = 0.0;
  }
  w->look[0] = look0;
  w->look[1] = look1;
}

void water_step(water_state_rec *water)
{
  typedef struct { float amplitude; float phase1; float speed1; float phase2; float speed2; } wave_rec;
  wave_rec *e;
  float (*re)[17];
  float (*im)[17];
  float buf[16][2];
  int i;
  int j;

  e = (wave_rec *)water->waves;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      e->phase1 += e->speed1;
      if (e->phase1 > 3.1415927) e->phase1 -= 6.2831855;
      else if (e->phase1 < -3.1415927) e->phase1 += 6.2831855;
      e->phase2 += e->speed2;
      if (e->phase2 > 3.1415927) e->phase2 -= 6.2831855;
      else if (e->phase2 < -3.1415927) e->phase2 += 6.2831855;
      e++;
    }
  }
  re = water->grid_re;
  im = water->grid_im;
  water_spectrum_build((float *)re,(float *)im,(float *)water->waves,(uint)e);
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      buf[j][0] = re[i][j];
      buf[j][1] = im[i][j];
    }
    fft16_inverse(buf[0]);
    for (j = 0; j < 16; j++) {
      re[i][j] = buf[j][0];
      im[i][j] = buf[j][1];
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      buf[j][0] = re[j][i];
      buf[j][1] = im[j][i];
    }
    fft16_inverse(buf[0]);
    for (j = 0; j < 16; j++) {
      re[j][i] = buf[j][0];
      im[j][i] = buf[j][1];
    }
  }
  for (i = 0; i < 17; i++) {
    re[i][16] = re[i][0];
    re[16][i] = re[0][i];
  }
}

void water_normals_build(water_state_rec *water)
{
  typedef struct { float x; float y; float z; } vec3;
  typedef struct { float height; vec3 normal; float pad; } cell;
  float (*h)[17] = water->grid_re;
  cell (*o)[17] = (cell (*)[17])water->cells;
  float s = *(float *)&water->grid_spacing;
  float p0[3];
  float p1[3];
  float p2[3];
  float d1[3];
  float d2[3];
  float n[3];
  int i;
  int j;
  int k;
  float x;
  float y;

  for (j = 0; j < 16; j++) {
    for (i = 0; i < 16; i++) {
      y = (float)j * s;
      x = (float)i * s;
      p0[0] = x;
      p0[1] = h[j][i];
      p0[2] = y;
      p1[0] = s + x;
      p1[1] = h[j][i + 1];
      p1[2] = y;
      p2[0] = x;
      p2[1] = h[j + 1][i];
      p2[2] = s + y;
      d1[0] = p1[0] - p0[0];
      d1[1] = p1[1] - p0[1];
      d1[2] = p1[2] - p0[2];
      d2[0] = p2[0] - p0[0];
      d2[1] = p2[1] - p0[1];
      d2[2] = p2[2] - p0[2];
      vec3_cross(d2, d1, n);
      vec3_normalize(n);
      o[j][i].height = p0[1];
      o[j][i].normal = *(vec3 *)n;
    }
    o[j][16].height = o[j][0].height;
    o[j][16].normal = o[j][0].normal;
  }
  for (k = 0; k < 17; k++) {
    o[16][k].height = o[0][k].height;
    o[16][k].normal = o[0][k].normal;
  }
}

void water_strip_heights_fill(water_state_rec *water, uchar *block)
{
  typedef struct { float height; int rest[4]; } cell_height;
  typedef struct { int head[2]; float height; int rest[3]; } strip_vtx;
  cell_height (*g)[17];
  strip_vtx *d;
  int r;
  int c;

  g = (cell_height (*)[17])water->cells;
  if (block != 0) {
    d = (strip_vtx *)(block + 0x7c);
    d = (strip_vtx *)((char *)d + 0x44);
    for (c = 0; c < 17; c++) {
      d->height = g[7][16 - c].height;
      d++;
      d->height = g[8][16 - c].height;
      d++;
    }
    for (r = 1; r < 7; r++) {
      for (c = 0; c < 17; c++) {
        d->height = g[r + 8][c].height;
        d++;
        d->height = g[r + 7][c].height;
        d++;
      }
      for (c = 0; c < 17; c++) {
        d->height = g[7 - r][16 - c].height;
        d++;
        d->height = g[8 - r][16 - c].height;
        d++;
      }
    }
    for (r = 15; r < 17; r++) {
      for (c = 0; c < 17; c++) {
        d->height = g[r][c].height;
        d++;
        d->height = g[r - 1][c].height;
        d++;
      }
    }
    for (c = 0; c < 17; c++) {
      d->height = g[1][c].height;
      d++;
      d->height = g[0][c].height;
      d++;
    }
  }
}

void water_update(uchar *block)
{
  water_step(&water_state);
  water_normals_build(&water_state);
  water_strip_heights_fill(&water_state,block);
  return;
}

void water_spectrum_build(float *re, float *im, float *waves, uint unused)
{
  float *a = re;
  float *b = im;
  int i;
  int j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      float r = waves[0];
      float t1 = waves[1];
      float t2 = waves[3];
      float x;
      float y;
      register float s __asm__("fr4");
      register float c __asm__("fr5");
      __asm__ ("fsca fpul,dr4" : "+f"(s), "+f"(c) : "y"((int)(t1 * 65536.0f / 6.2831855f)));
      c *= r;
      s *= r;
      a[i * 17 + j] = c;
      b[i * 17 + j] = s;
      a[(16 - i) * 17 - j + 16] = c;
      b[(16 - i) * 17 - j + 16] = -s;
      __asm__ ("fsca fpul,dr4" : "+f"(s), "+f"(c) : "y"((int)(t2 * 65536.0f / 6.2831855f)));
      c *= r;
      s *= r;
      a[i * 17 - j + 16] = c;
      b[i * 17 - j + 16] = s;
      a[(16 - i) * 17 + j] = c;
      b[(16 - i) * 17 + j] = -s;
      waves += 5;
    }
  }
  a[0] = 0.0f;
  b[0] = 0.0f;
  a[0x10] = 0.0f;
  b[0x10] = 0.0f;
  a[0x110] = 0.0f;
  b[0x110] = 0.0f;
  a[0x120] = 0.0f;
  b[0x120] = 0.0f;
}
