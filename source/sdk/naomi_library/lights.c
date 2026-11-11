/*
 * sdk/naomi_library/lights.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern light_table_entry light_record_default; /* 0x0c32a5dc */
extern float light_color_black[4]; /* 0x0c32a698 */
extern int light_stash_flags[16]; /* 0x0c41c65c */
extern light_color_rec light_stash_colors[16]; /* 0x0c41c69c */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void light_table_fill_defaults(void)
{
  int i;
  for (i = 0; i < 16; i++) {
    light_table[i] = light_record_default;
  }
  lights_on_mask = 0;
}

void lighting_path_select(int path)
{
  lighting_path = path;
  return;
}

void light_unit_axis_set(int entry, float x, float y, float z)
{
  light_table_entry *light;
  light_color_rec *stash;
  light = &light_table[entry];
  light->dir[0] = x;
  light->dir[1] = y;
  light->dir[2] = z;
  vec3_normalize(light->dir);
}

void light_pos_set(int light, float x, float y, float z)
{
  light_table[light].pos[0] = x;
  light_table[light].pos[1] = y;
  light_table[light].pos[2] = z;
  return;
}

void light_atten_set(int light, float constant, float linear, float quadratic)
{
  light_table[light].atten[0] = constant;
  light_table[light].atten[1] = linear;
  light_table[light].atten[2] = quadratic;
  return;
}

void light_range_set(int light, float range)
{
  light_table[light].range = range;
  light_table[light].range_sq = range * range;
  light_table[light].range_inv = 1.0f / range;
  return;
}

void spot_cone_set_cos(int entry,float cone_cos)
{
  spot_cone_set_sincos(entry,fsqrt_asm(1.0 - cone_cos * cone_cos),cone_cos);
}

void spot_cone_set_sincos(int entry, float cone_sin, float cone_cos)
{
  light_table_entry *light;
  light_color_rec *stash;
  light = &light_table[entry];
  light->cone_sin = cone_sin;
  light->cone_cos = cone_cos;
  light->cone_scale = 1.0f / (1.0f - cone_cos);
  return;
}

void spot_cone_set_ang16(int entry, int angle)
{
  float cone_sin;
  float cone_cos;

  cone_sin = (float)sin_ang16(angle);
  cone_cos = (float)cos_ang16(angle);
  spot_cone_set_sincos(entry,cone_sin,cone_cos);
  return;
}

void spot_cone_set_radians(int entry, float angle)
{
  float cone_sin;
  float cone_cos;

  cone_sin = (float)sin_radians(angle);
  cone_cos = cos_radians(angle);
  spot_cone_set_sincos(entry,cone_sin,cone_cos);
  return;
}

void light_type_set(int light, short type)
{
  light_table[light].type = type;
  return;
}

void light_ambient_from_rgb(float *in)
{
  light_ambient_color.field_0x0 = *in++;
  light_ambient_color.red = *in++;
  light_ambient_color.green = *in++;
  light_ambient_color.blue = *in;
  light_ambient_color.field_0x0 = light_ambient_color.red * 0.3f + light_ambient_color.green * 0.59f + light_ambient_color.blue * 0.11f;
}

void light_put_color4(int entry, float *color)
{
  float *s = color;
  light_table_entry *light = &light_table[entry];
  float *d = &light->color.field_0x0;
  *d++ = *s++;
  *d++ = *s++;
  *d = *s++;
  light->color.blue = *s;
}

void lights_by_inverse_matrix(void)
{
  light_table_entry *light;
  light_color_rec *stash;
  int i;

  mtx_push(0);
  i = 0;
  mtx_invert_rigid();
  light = light_table;
  do {
    if (light->on != 0) {
      mtx_xform_dir(light->view_dir,light->model_dir);
      mtx_xform_point(light->view_pos,light->model_pos);
    }
    i = i + 1;
    light = light + 1;
  } while (i < 0x10);
  mtx_pop(1);
  return;
}

void light0_by_inverse_rotation(void)
{
  mtx_transpose();
  mtx_xform_dir(light_table[0].view_dir,light_table[0].model_dir);
  mtx_xform_dir(view_unit_z,model_unit_z);
  mtx_transpose();
}

void light_on_set(int light, uchar on)
{
  light_table[light].on = on;
  light = 1 << light;
  lights_on_mask |= light;
  if (on == 0) {
    lights_on_mask ^= light;
  }
}

void lights_stash_and_blackout(void)
{
  int i;
  int *flags;
  light_table_entry *light;
  light_color_rec *stash;

  i = 0;
  flags = light_stash_flags;
  light = light_table;
  for (; i < 0x10; i++) {
    flags[i] = light[i].on;
    stash = light_stash_colors + i;
    stash->field_0x0 = light[i].color.field_0x0;
    stash->red = light[i].color.red;
    stash->green = light[i].color.green;
    stash->blue = light[i].color.blue;
    light_put_color4(i,light_color_black);
    light[i].on = 0;
  }
  lights_on_mask = 0;
}

void lights_unstash(void)
{
  int i;
  uint bit;

  lights_on_mask = 0;
  bit = 1;
  for (i = 0; i < 16; i++) {
    light_table[i].color.field_0x0 = light_stash_colors[i].field_0x0;
    light_table[i].color.red = light_stash_colors[i].red;
    light_table[i].color.green = light_stash_colors[i].green;
    light_table[i].color.blue = light_stash_colors[i].blue;
    light_table[i].on = light_stash_flags[i];
    if (light_table[i].on == 1) {
      lights_on_mask |= bit;
    }
    bit <<= 1;
  }
}

void light_ambient_color_get(float *out)
{
  *out++ = light_ambient_color.field_0x0;
  *out++ = light_ambient_color.red;
  *out++ = light_ambient_color.green;
  *out = light_ambient_color.blue;
  return;
}

void light_ambient_color_set(float *in)
{
  light_ambient_color.field_0x0 = *in++;
  light_ambient_color.red = *in++;
  light_ambient_color.green = *in++;
  light_ambient_color.blue = *in;
  return;
}
