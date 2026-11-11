/*
 * vf4lib/libeffect/src/effect_flare.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern lens_flare_rec lens_flare_copy; /* 0x0c3985b0 */
extern float lens_flare_alpha; /* 0x0c2b0ae0 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern lens_flare_rec *lens_flare_src; /* 0x0c3985ac */
extern camera_state camera; /* 0x0c370ce0 */
extern fog_rec fog_offset; /* 0x0c374288 */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline void vec_from_to(float *d, float *from, float *to)
{
  d[0] = to[0] - from[0];
  d[1] = to[1] - from[1];
  d[2] = to[2] - from[2];
}

void flare_element_draw(int model, float dist, float scale, float intensity)
{
  float b[16];
  float m[16];
  
  memset(m,0,0x40);
  m[0] = 1.0;
  m[5] = 1.0;
  m[10] = 1.0;
  m[15] = 1.0;
  if (model != -1) {
    mtx_push(0);
    mtx_translate(0.0,0.0,dist);
    mtx_store_wordwise(b);
    m[12] = b[12];
    m[13] = b[13];
    m[14] = b[14];
    mtx_set_current_wordwise(m);
    mtx_scale_xyz(scale,scale,scale);
    extra_light_parallel_set(intensity);
    obj_draw_translucent(model);
    mtx_pop(1);
  }
  return;
}

void flare_dir_rotate(float *dir)
{
  float up[3];
  float v[3];

  up[0] = 0.0;
  up[1] = 0.0;
  up[2] = 1.0;
  v[0] = dir[0];
  v[1] = 0.0;
  v[2] = dir[2];
  vec3_normalize(v);
  mtx_rot_vec_to_vec((uint *)up,v);
  v[0] = dir[1];
  v[1] = 0.0;
  v[2] = fsqrt_asm(dir[0] * dir[0] + dir[2] * dir[2]);
  mtx_rot_z_s16(0x4000);
  vec3_normalize(v);
  mtx_rot_vec_to_vec((uint *)up,v);
  mtx_rot_z_s16(0xc000);
}

void flare_fog_glare_set(float cos_view)
{
  typedef struct { char p0[4]; short argb_a; ushort argb_r; ushort argb_g; ushort argb_b; float near_dist; float far_dist; float strength; char pos[0xc]; float angle; } flare_glare_src;
  float cos_angle;
  float t;
  cos_angle = cos_ang16((int)(((flare_glare_src *)lens_flare_src)->angle * 182.04445));
  if ((fog_flags & 2) != 0) {
    if (cos_angle < cos_view) {
      t = (cos_view - cos_angle) * (1.0 / (1.0 - cos_angle));
      fog_offset.strength = ((flare_glare_src *)lens_flare_src)->strength * t;
      fog_offset.far_dist = ((flare_glare_src *)lens_flare_src)->far_dist;
      fog_offset.near_dist = ((flare_glare_src *)lens_flare_src)->near_dist * t;
      fog_offset.argb[0] = ((flare_glare_src *)lens_flare_src)->argb_a;
      fog_offset.argb[1] = ((flare_glare_src *)lens_flare_src)->argb_r * t;
      fog_offset.argb[2] = ((flare_glare_src *)lens_flare_src)->argb_g * t;
      fog_offset.argb[3] = ((flare_glare_src *)lens_flare_src)->argb_b * t;
    }
    else {
      fog_offset.strength = 0.0;
      fog_offset.far_dist = 0.0;
      fog_offset.near_dist = 0.0;
      fog_offset.argb[0] = 0;
      fog_offset.argb[1] = 0;
      fog_offset.argb[2] = 0;
      fog_offset.argb[3] = 0;
    }
  }
}

void flare_fog_glare_clear(void)
{
  if ((fog_flags & 2) != 0) {
    fog_offset.strength = 0.0;
    fog_offset.far_dist = 0.0;
    fog_offset.near_dist = 0.0;
    fog_offset.argb[0] = 0;
    fog_offset.argb[1] = 0;
    fog_offset.argb[2] = 0;
    fog_offset.argb[3] = 0;
  }
  return;
}

void lens_flare_draw(camera_state *cam,float *light_pos,float size,float intensity)
{
  float proj[3];
  float a[3];
  float b[3];
  float d[3];
  float e[3];
  float t[3];
  float scr[2];
  float *pp = proj;
  float *sc = scr;
  float *ap;
  float *bp;
  float *dp;
  float *ep;
  float *tp;
  lens_flare_element *el;
  float edge;
  float fade;
  float dot;
  float s;
  float len;
  int i;

  el = (lens_flare_element *)lens_flare_src->elements;
  mtx_xform_point(light_pos, proj);
  if (proj[2] < 0.0) {
    mtx_save_proj_x_current(640.0, 480.0);
    proj_point_to_pixel_xy(light_pos, sc);
    if (sc[0] > -200.0 && sc[0] < 840.0 && sc[1] > -200.0 && sc[1] < 680.0) {
      ap = a;
      bp = b;
      ep = e;
      dp = d;
      vec_from_to(ap, &cam->eye.x, &cam->target.x);
      vec_from_to(bp, &cam->eye.x, light_pos);
      t[0] = 0.0;
      t[1] = 0.0;
      t[2] = -0.4;
      tp = t;
      edge = 0.99;
      fade = 0.99;
      if (sc[0] < -100.0) {
        edge = (sc[0] + 200.0) / 100.0;
      }
      else if (sc[0] > 740.0) {
        edge = (840.0 - sc[0]) / 100.0;
      }
      if (sc[1] < -100.0) {
        edge *= (sc[1] + 200.0) / 100.0;
      }
      else if (sc[1] > 580.0) {
        edge *= (680.0 - sc[1]) / 100.0;
      }
      if (sc[0] > -20.0 && sc[0] < 660.0 && sc[1] > -20.0 && sc[1] < 500.0) {
        if (sc[0] < 20.0) {
          fade = (sc[0] + 20.0) / 40.0;
        }
        else if (sc[0] > 620.0) {
          fade = (660.0 - sc[0]) / 40.0;
        }
        if (sc[1] < 20.0) {
          fade *= (sc[1] + 20.0) / 40.0;
        }
        else if (sc[1] > 460.0) {
          fade *= (500.0 - sc[1]) / 40.0;
        }
      }
      else {
        fade = 0.0;
      }
      intensity = intensity * edge;
      vec3_normalize(ap);
      vec3_normalize(bp);
      dot = vec3_dot(ap, bp);
      flare_fog_glare_set(dot);
      ep[0] = (pp[0] * -1.0) / pp[2];
      ep[1] = (pp[1] * -1.0) / pp[2];
      ep[2] = -1.0;
      dp[0] = tp[0] - ep[0];
      dp[1] = tp[1] - ep[1];
      dp[2] = tp[2] - ep[2];
      if ((lens_flare_src->on != 0) & (el != 0)) {
        mtx_push(0);
        mtx_load_identity();
        if (size > 0.0) {
          mtx_push(0);
          mtx_translate_by_vec3(ep);
          if (el[1].model != -1) {
            mtx_push(0);
            s = size * el[1].scale;
            mtx_scale_xyz(s, s, s);
            extra_light_parallel_set(fade * 0.8);
            obj_draw_translucent(el[1].model);
            mtx_pop(1);
          }
          if (el[2].model != -1) {
            mtx_push(0);
            s = el[2].scale;
            mtx_scale_xyz(s, s, s);
            extra_light_parallel_set(intensity);
            obj_draw_translucent(el[2].model);
            mtx_pop(1);
          }
          if (el[3].model != -1) {
            mtx_push(0);
            s = dot * el[3].scale;
            mtx_scale_xyz(s, s, s);
            extra_light_parallel_set(fade * 0.05);
            obj_draw_translucent(el[3].model);
            mtx_pop(1);
          }
          mtx_pop(1);
        }
        mtx_translate_by_vec3(tp);
        if (el[0].model != -1) {
          mtx_push(0);
          s = el[0].scale;
          mtx_scale_xyz(s, s, s);
          extra_light_parallel_set(edge);
          obj_draw_translucent(el[0].model);
          mtx_pop(1);
        }
        flare_dir_rotate(dp);
        len = fsqrt_asm(dp[0] * dp[0] + dp[1] * dp[1] + dp[2] * dp[2]);
        for (i = 0; i < 15; i++) {
          if (el[i + 4].model == -1) break;
          flare_element_draw(el[i + 4].model, len * el[i + 4].fraction, el[i + 4].scale, intensity);
        }
        mtx_pop(1);
      }
    }
    else {
      flare_fog_glare_clear();
    }
  }
  else {
    flare_fog_glare_clear();
  }
}

void lens_flare_src_load(void)
{
  lens_flare_rec *s;
  
  s = stage_cur->flare;
  if (s != 0) {
    lens_flare_copy = *s;
    lens_flare_src = &lens_flare_copy;
  }
  else {
    lens_flare_src = 0;
  }
}

void lens_flare_effect_draw(void)
{
  if (lens_flare_src != (lens_flare_rec *)0x0) {
    lens_flare_draw(&camera,(float *)&lens_flare_src->pos_x,0.4,lens_flare_alpha);
    extra_light_off();
  }
  return;
}
