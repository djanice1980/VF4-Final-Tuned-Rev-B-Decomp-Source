/*
 * vf4lib/libpoly/src/tune_texture.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int tune_texture_texels_per_step; /* 0x0c322798 */


static inline float hue_fsqrt(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void color_tune_yuv(float *pr, float *pg, float *pb, float y_scale, float diff_scale, float offset)
{
  float r;
  float g;
  float b;
  float y;
  float u;
  float v;

  r = *pr;
  g = *pg;
  b = *pb;
  y = (r * 0.299 + g * 0.587 + b * 0.114) * y_scale;
  u = (r * -0.147 - g * 0.289 + b * 0.436) * diff_scale;
  v = (r * 0.615 - g * 0.515 - b * 0.1) * diff_scale;
  r = y + u * 0.0 + v * 1.14 + offset;
  g = y - u * 0.396 - v * 0.581 + offset;
  b = y + u * 2.029 + v * 0.0 + offset;
  if (r > 1.0) r = 1.0;
  else if (r < 0.0) r = 0.0;
  if (g > 1.0) g = 1.0;
  else if (g < 0.0) g = 0.0;
  if (b > 1.0) b = 1.0;
  else if (b < 0.0) b = 0.0;
  *pr = r;
  *pg = g;
  *pb = b;
}

void color_tune_hsb(float *pr,float *pg,float *pb,ushort hue,float brightness,float saturation)
{
  float y;
  float base;
  float dr;
  float db;
  float len;
  float r;
  float g;
  float b;
  short a;

  r = *pr;
  g = *pg;
  b = *pb;
  y = r * 0.299 + g * 0.587 + b * 0.114;
  dr = r - y;
  db = b - y;
  base = y + brightness;
  len = hue_fsqrt(dr * dr + db * db) * (saturation + 1.0);
  a = atan2_s16(dr,db) + hue;
  r = sin_s16(a) * len + base;
  b = cos_s16(a) * len + base;
  if (r > 1.0) r = 1.0;
  else if (r < 0.0) r = 0.0;
  if (b > 1.0) b = 1.0;
  else if (b < 0.0) b = 0.0;
  g = base * 1.7041581 - r * 0.509292 - b * 0.194956;
  if (g > 1.0) g = 1.0;
  else if (g < 0.0) g = 0.0;
  *pr = r;
  *pg = g;
  *pb = b;
}

int tex_desc_tune_all(tex_desc_rec *descs)
{
  int done;

  tex_desc_tune_start(descs);
  do {
    done = tex_desc_tune_step();
  } while (done == 0);
  return 0;
}

int tex_desc_tune_start(tex_desc_rec *descs)
{
  tex_desc_tune_cursor = descs;
  tex_desc_tune_word = 0;
  tex_desc_tune_running = 0;
  return 0;
}

int tex_desc_tune_step(void)
{
  if (tex_desc_tune_cursor == 0 || tex_desc_tune_cursor->width == 0) {
    return 1;
  }
  if (tex_desc_tune_running == 0) {
    tex_tune_start(tex_desc_tune_cursor);
    tex_desc_tune_running = 1;
    tex_desc_tune_cursor = tex_desc_tune_cursor + 1;
  }
  else {
    tex_desc_tune_running = tune_texture_step();
  }
  return 0;
}

void empty_fn_0c0eac20(void)
{
  return;
}

int tex_tune_start(tex_desc_rec *desc)
{
  uint fmt;
  uint type;
  int w;

  tex_tune_desc = desc;
  fmt = desc->format & 3;
  tex_tune_texel = (short *)desc->data;
  type = desc->format & 0xf00;
  switch ((int)type) {
  case 0x300:
  case 0x400:
    tex_tune_texel_end = (int)desc->data + 0x800;
    break;
  case 0x100:
  case 0x900:
  case 0xd00:
    tex_tune_texel_end = (int)desc->width * (int)desc->height * 2 + (int)desc->data;
    break;
  case 0x200:
    tex_tune_texel_end = (int)desc->data;
    for (w = desc->width; w > 0; w >>= 1) {
      tex_tune_texel_end = w * w * 2 + tex_tune_texel_end;
    }
    break;
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/tune_texture.c", 0xab, "assertion `%s' failed.\n", "0");
    return -1;
  }
  switch ((int)fmt) {
  case 0:
    tex_tune_max_r = 31.0f;
    tex_tune_max_g = 31.0f;
    tex_tune_max_b = 31.0f;
    tex_tune_shift_r = 10;
    tex_tune_shift_g = 5;
    tex_tune_shift_b = 0;
    tex_tune_keep_mask = -0x8000;
    tex_tune_mask_r = 31;
    tex_tune_mask_g = 31;
    tex_tune_mask_b = 31;
    break;
  case 1:
    tex_tune_max_r = 31.0f;
    tex_tune_max_g = 63.0f;
    tex_tune_max_b = 31.0f;
    tex_tune_shift_r = 11;
    tex_tune_shift_g = 5;
    tex_tune_shift_b = 0;
    tex_tune_keep_mask = 0;
    tex_tune_mask_r = 31;
    tex_tune_mask_g = 63;
    tex_tune_mask_b = 31;
    break;
  case 2:
    tex_tune_max_r = 15.0f;
    tex_tune_max_g = 15.0f;
    tex_tune_max_b = 15.0f;
    tex_tune_shift_r = 8;
    tex_tune_shift_g = 4;
    tex_tune_shift_b = 0;
    tex_tune_keep_mask = -0x1000;
    tex_tune_mask_r = 15;
    tex_tune_mask_g = 15;
    tex_tune_mask_b = 15;
    break;
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/tune_texture.c", 0xc0, "assertion `%s' failed.\n", "0");
    return -2;
  }
  return 0;
}

int tune_texture_step(void)
{
  float r;
  float g;
  float b;
  float k0;
  float k1;
  float k2;
  int i;
  short *p;

  k0 = 1.0 / tex_tune_max_r;
  k1 = 1.0 / tex_tune_max_g;
  k2 = 1.0 / tex_tune_max_b;
  i = 0;
  while (i < tune_texture_texels_per_step) {
    r = (*tex_tune_texel >> tex_tune_shift_r) & tex_tune_mask_r;
    g = (*tex_tune_texel >> tex_tune_shift_g) & tex_tune_mask_g;
    b = (*tex_tune_texel >> tex_tune_shift_b) & tex_tune_mask_b;
    r *= k0;
    g *= k1;
    b *= k2;
    color_tune_hsb(&r,&g,&b,tex_tune_hue,tex_tune_brightness,tex_tune_saturation);
    r *= tex_tune_max_r;
    g *= tex_tune_max_g;
    b *= tex_tune_max_b;
    p = tex_tune_texel;
    *p = *p & tex_tune_keep_mask | ((short)r & tex_tune_mask_r) << tex_tune_shift_r
         | ((short)g & tex_tune_mask_g) << tex_tune_shift_g
         | ((short)b & tex_tune_mask_b) << tex_tune_shift_b;
    if ((uint)tex_tune_texel >= tex_tune_texel_end) {
      return 0;
    }
    i++;
    tex_tune_texel = tex_tune_texel + 1;
  }
  return 1;
}

void empty_fn_0c0eb120(void)
{
  return;
}
