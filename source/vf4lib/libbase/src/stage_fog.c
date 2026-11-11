/*
 * vf4lib/libbase/src/stage_fog.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern stage_intro_cam_rec stage_intro_cams[15][4]; /* 0x0c29411c */
extern short stage_intro_frame; /* 0x0c2942fc */
extern fog_event_rec *stage_intro_fog_events; /* 0x0c294300 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern fog_rec fog_fade; /* 0x0c3742b4 */
extern fog_rec fog_record_zero; /* 0x0c168068 */
extern camera_state camera; /* 0x0c370ce0 */
extern fog_rec fog_offset; /* 0x0c374288 */
extern fog_rec fog_target; /* 0x0c37429c */
extern fog_rec fog_cur; /* 0x0c3742c8 */
extern fog_rec fog_base; /* 0x0c3742dc */

void stage_intro_cam_pick(void)
{
  short cam_no;
  int intro;
  stage_intro_cam_rec *cam;
  
  intro = (int)stage_cur->intro_cam;
  stage_intro_frame = 0;
  if (intro < 0) {
    cam_no = 0x10;
  }
  else {
    cam = stage_intro_cams[intro];
    if (stg_cam_no != 0) {
      cam += stg_cam_no - 1;
    }
    else {
      cam += (int)lcg_next15() % 4;
    }
    stage_intro_fog_events = cam->fog_events;
    cam_no = cam->cam_no;
  }
  cam_anim_index_camera = cam_no;
  camera.mode = SOFTIMAGE;
  cam_anim_time = 0.0;
  cam_anim_kind = 0;
  return;
}

void stage_intro_fog_step(void)
{
  stage_intro_frame = stage_intro_frame + 1;
  if (stage_intro_fog_events != 0) {
    fog_event_run(stage_intro_fog_events);
  }
  return;
}

void stage_intro_fog_end(void)
{
  fog_flags = fog_flags & 3;
  if (stage_intro_fog_events != 0) {
    fog_flags = fog_flags | 0x10;
    fog_timer = 0x28;
  }
  return;
}

void fog_event_run(fog_event_rec *events)
{
  fog_event_rec *p;
  for (p = events; p->flags != 0; p++) {
    if (stage_intro_frame == p->start) {
      fog_timer = p->end - p->start;
      fog_flags &= 3;
      fog_flags |= p->flags;
      fade_color[0] = p->r;
      fade_color[1] = p->g;
      fade_color[2] = p->b;
      break;
    }
  }
}

void fog_task_start(void)
{
  fog_flags = fog_flags | 8;
  task_start(&task_fog);
  fog_flags = (fog_flags | 1) & 0xfffffff7;
}

void fog_task_stop(void)
{
  task_stop(&task_fog);
  fog_table_zero_fill();
  fog_flags = fog_flags & 0xfffffffe;
  return;
}

void fog_task_init(void)
{
  if ((fog_flags & 8) == 0) {
    fog_defaults_reset();
    fog_flags = 3;
  }
  return;
}

void fog_defaults_reset(void)
{
  fog_rec *p = &fog_offset;
  fog_cur.argb[0] = 0;
  fog_cur.argb[1] = 0xff;
  fog_cur.argb[2] = 0xff;
  fog_cur.argb[3] = 0xff;
  fog_cur.near_dist = 0.0;
  fog_cur.far_dist = 10.0;
  fog_cur.strength = 0.0;
  *p = fog_record_zero;
}

void fog_task_sync(void)
{
  if (fog_flags & 1) fog_task_start(); else fog_task_stop();
}

void fog_frame_update(void)
{
  uint col = 0;
  fog_rec *cur = &fog_cur;
  fog_rec *base = &fog_base;
  fog_rec *out = &fog_target;
  fog_rec *ofs = &fog_offset;
  ushort *fade = fog_fade.argb;
  uint fl;
  float a;
  float b;
  float t;
  int v;

  out->argb[0] = base->argb[0] + ofs->argb[0];
  out->argb[1] = base->argb[1] + ofs->argb[1];
  out->argb[2] = base->argb[2] + ofs->argb[2];
  out->argb[3] = base->argb[3] + ofs->argb[3];
  out->near_dist = base->near_dist + ofs->near_dist;
  out->far_dist = base->far_dist + ofs->far_dist;
  out->strength = base->strength + ofs->strength;
  if (fog_flags & 0x10) {
    fade[1] = fade_color[0];
    fade[2] = fade_color[1];
    fade[3] = fade_color[2];
    if (fog_flags & 0x100)
      fog_flags &= ~0x100;
    else
      fog_cnt = 0;
    fog_flags = (fog_flags | 4) & ~0x80;
  }
  else if (fog_flags & 0x20) {
    fade[1] = fade_color[0];
    fade[2] = fade_color[1];
    fade[3] = fade_color[2];
    if (fog_flags & 0x100)
      fog_flags &= ~0x100;
    else
      fog_cnt = 0;
    fog_flags |= 0x84;
  }
  fl = fog_flags;
  fog_flags = fl & ~0x30;
  if ((fl & 4) && fog_cnt <= fog_timer) {
    t = fog_timer;
    b = (t - fog_cnt) / t;
    if (fl & 0x80) {
      a = 1.0 - b;
    }
    else {
      a = b;
      b = 1.0 - a;
    }
    cur->argb[0] = out->argb[0] * b + fade[0] * a;
    cur->argb[1] = out->argb[1] * b + fade[1] * a;
    cur->argb[2] = out->argb[2] * b + fade[2] * a;
    cur->argb[3] = out->argb[3] * b + fade[3] * a;
    if (pause_frame_held() == 0 && (fog_flags & 0x40) == 0)
      fog_cnt++;
    if (fog_cnt > fog_timer) {
      if (fog_flags & 0x80)
        fog_flags |= 0x40;
      else
        fog_flags &= ~0x44;
      fog_cnt = fog_timer;
    }
  }
  else {
    cur->argb[0] = out->argb[0];
    cur->argb[1] = out->argb[1];
    cur->argb[2] = out->argb[2];
    cur->argb[3] = out->argb[3];
  }
  cur->near_dist = out->near_dist;
  cur->far_dist = out->far_dist;
  cur->strength = out->strength;
  v = cur->argb[0];
  if (v > 255) v = 255;
  if (v < 0) v = 0;
  col |= v << 24;
  v = cur->argb[1];
  if (v > 255) v = 255;
  if (v < 0) v = 0;
  col |= v << 16;
  v = cur->argb[2];
  if (v > 255) v = 255;
  if (v < 0) v = 0;
  col |= v << 8;
  v = cur->argb[3];
  if (v > 255) v = 255;
  if (v < 0) v = 0;
  col |= v;
  fog_col_ram_write_bytes(col);
  fog_density_pack_float(cur->far_dist);
  fog_table_build(cur->near_dist, cur->far_dist, cur->strength);
}

void fog_table_build(float near, float far, float strength)
{
  float fade_floor;
  float ratio;
  int n;
  int exponent;
  int mantissa;
  int last;
  int i;

  fade_floor = 0.0;
  if (near < fade_floor) near = fade_floor;
  if (far < 0.1) far = 0.1;
  if (strength > 1.0) strength = 1.0;
  ratio = far / near;
  n = (int)ratio;
  exponent = 0;
  while (n > 1) {
    n >>= 1;
    exponent++;
    if (exponent >= 7) break;
  }
  mantissa = (int)((ratio / pow_float(2.0,(float)exponent) - 1.0) * 16.0);
  if (mantissa > 15) mantissa = 15;
  last = exponent << 4 | mantissa;
  if ((fog_flags & 4) != 0) {
    float ft = (float)fog_timer;
    float t = (ft - (float)fog_cnt) / ft;
    if ((fog_flags & 0x80) != 0) fade_floor = 1.0 - t;
    else fade_floor = t;
  }
  for (i = 0; i < 0x80; i++) {
    if (i <= last) fog_table[i] = 1.0 - (float)i / (float)last;
    else fog_table[i] = 0.0;
    fog_table[i] = fog_table[i] * strength;
    if ((fog_flags & 4) != 0 && fog_table[i] < fade_floor) {
      fog_table[i] = fade_floor;
      if (fade_floor > 1.0) fog_table[i] = 1.0;
    }
  }
}
