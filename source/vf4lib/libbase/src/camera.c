/*
 * vf4lib/libbase/src/camera.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_rob_cam_eye_update
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern camera_proj_rec camera_proj_default; /* 0x0c1631b4 */
extern camera_state camera; /* 0x0c370ce0 */
extern void (*camera_mode_funcs[10])(void); /* 0x0c1631c4 */
extern float replay_cam_heights[8]; /* 0x0c1632ac */
extern int camera_proj_depth; /* 0x0c24316c */
extern vec3 cam_sat_offsets[2][2]; /* 0x0c2431b8 */
extern short *camera_shake_tables[4]; /* 0x0c2433f0 */
extern vec3 replay_cam_eyes_a[33]; /* 0x0c24340c */
extern vec3 replay_cam_eyes_b[33]; /* 0x0c243598 */
extern float stage_orbit_angle; /* 0x0c243728 */
extern int vblank_count; /* 0x0c292544 */
extern float mode_timer_rate; /* 0x0c292678 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern short cam_anim_index; /* 0x0c370d5c */
extern uint cam_anim_flags; /* 0x0c370dc4 */
extern camera_proj_rec camera_proj_stack[16]; /* 0x0c370dc8 */
extern cam_anim_start_eye_rec cam_anim_start_eye; /* 0x0c370c98 */
extern cam_anim_channels cam_anim_sample; /* 0x0c370cb0 */
extern cam_anim_eye_rec cam_anim_eye; /* 0x0c370cd0 */
extern cam_anim_target_rec cam_anim_target; /* 0x0c370d64 */
extern cam_anim_start_target_rec cam_anim_start_target; /* 0x0c370db0 */
extern rob_cam_history_head rob_cam_history; /* 0x0c41eccc */
extern rob_cam_hist_a_tail_rec rob_cam_hist_a_tail; /* 0x0c41edc8 */
extern rob_cam_hist_a_pos_rec rob_cam_hist_a_oldest_pos; /* 0x0c41eddc */
extern rob_cam_hist_b_pos_rec rob_cam_hist_b_oldest_pos; /* 0x0c41eefc */
extern rob_cam_history_end_rec rob_cam_history_end; /* 0x0c41ef08 */
extern replay_cam_rec replay_cam; /* 0x0c41ef20 */

static inline int padhit(int m)
{
  player_input_rec *p0 = player_input_get(0);
  player_input_rec *p1 = player_input_get(1);
  return (p0->pressed | p1->pressed) & m;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void camera_task_start(void)
{
  camera_reset();
  camera_proj_set(&camera.proj.near_clip);
  mtx_push_identity();
  mtx_store_wordwise(camera_view_mtx);
  mtx_pop(1);
  return;
}

void camera_reset(void)
{
  camera_state *a = &camera;
  float f = 0.0;
  int z = 0;
  a->field_0x0 = f;
  a->field_0x4 = f;
  a->field_0x8 = f;
  a->eye.x = f;
  a->eye.y = f;
  a->eye.z = f;
  a->target.x = f;
  a->target.y = f;
  a->target.z = f;
  a->goal.x = f;
  a->goal.y = f;
  a->goal.z = f;
  a->field_0x58 = f;
  a->field_0x5c = f;
  a->field_0x60 = f;
  a->field_0x64 = f;
  a->pitch = z;
  a->yaw = z;
  a->roll = z;
  a->cam_rob_no = z;
  a->sat_row = z;
  a->hold_frames = z;
  a->flags = 0x18;
  a->mode_return = a->mode;
  a->mode_last = a->mode;
  a->proj = camera_proj_default;
  a->pers_req = a->proj.pers;
  a->shake.state = 0;
  a->shake.req = 0;
  a->shake.x = 0;
  a->shake.y = 0;
  a->shake.field_0x4 = 0;
  return;
}

void camera_task_frame(void)
{
  float up[3];
  camera_state *cam = &camera;

  mtx_load_identity();
  camera_mode_update();
  camera_proj_set(&cam->proj.near_clip);
  up[0] = 0.0;
  up[1] = 1.0;
  up[2] = 0.0;
  mtx_rot_x_s16(-cam->shake.x);
  mtx_rot_y_s16(-cam->shake.y);
  mtx_rot_z_s16(-cam->roll);
  mtx_mul_eye_target_up_flip(&cam->eye.x,&cam->target.x,up);
  mtx_store_wordwise(camera_view_mtx);
  lights_xform_by_current();
  cam->flags = cam->flags & 0xffffffe7;
  return;
}

void camera_view_load(void)
{
  mtx_set_current_wordwise(camera_view_mtx);
  return;
}

void camera_proj_reapply(void)
{
  camera_proj_set(&camera.proj.near_clip);
  return;
}

void camera_proj_push(float *proj)
{
  int i;
  
  i = camera_proj_depth;
  camera_proj_stack[i] = camera.proj;
  camera_proj_depth = i + 1;
  camera_proj_set(proj);
  if (0xf < camera_proj_depth) {
    camera_proj_depth = 0xf;
  }
  return;
}

void camera_proj_pop(void)
{
  if (camera_proj_depth < 1) {
    camera_proj_depth = 0;
  }
  else {
    camera_proj_depth = camera_proj_depth + -1;
    camera_proj_set(&camera_proj_stack[camera_proj_depth].near_clip);
  }
}

void camera_proj_set(float *proj)
{
  float *c = &camera.proj.near_clip;
  *(camera_proj_rec *)c = *(camera_proj_rec *)proj;
  if (pause_frame_held() == 0) {
    if ((camera.flags & 0x10) != 0) {
      camera.proj.pers = camera.pers_req;
    }
    else {
      c[2] = c[2] * 0.8 + camera.pers_req * 0.2;
    }
  }
  mtx_stack_select(3);
  mtx_load_identity();
  proj_from_vfov((int)((c[2] * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,
               c[0],c[1]);
  proj_cmds_send();
  mtx_stack_select(1);
}

void camera_mode_update(void)
{
  camera_state *p = &camera;
  camera_state *q;
  if ((char)p->mode != p->mode_last) {
    p->mode_last = p->mode;
  }
  camera_shake_take_request(&camera.shake);
  if ((char)camera.mode < 10) {
    (*camera_mode_funcs[(char)camera.mode])();
  }
  q = &camera;
  if (q->mode != SIC_TEST) {
    camera_shake_step(&q->shake);
  }
  return;
}

void camera_view_reset(void)
{
  camera_state *c = &camera;
  float *d = &c->proj.near_clip;
  float z = 0.0;
  short *s = &c->roll;

  view_reset_word = 0;
  *s = 0;
  view_reset_value_b = z;
  view_reset_value_a = z;
  c->proj = camera_proj_default;
  c->pers_req = c->proj.pers;
  camera_proj_set(d);
  c->flags = c->flags | 0x18;
  return;
}

void rob_cam_history_fill(rob_rec *rob_a, rob_rec *rob_b)
{
  typedef struct { float x[3]; } vec3;
  typedef struct { float x[2]; } vec2;
  typedef struct { int b; vec3 v; vec2 w; } ent;
  typedef struct { float f; ent a[12]; ent b[12]; int n; } all;
#define P ((all *)&rob_cam_history)
  int i;

  for (i = 0; i < 12; i++) {
    P->a[i].b = rob_a->side;
    P->b[i].b = rob_b->side;
    P->a[i].v = *(vec3 *)&rob_a->ground_pos;
    P->b[i].v = *(vec3 *)&rob_b->ground_pos;
    P->a[i].w = *(vec2 *)rob_a->screen_xy;
    P->b[i].w = *(vec2 *)rob_b->screen_xy;
  }
  P->f = vec3_distance(&rob_a->ground_pos.x,&rob_b->ground_pos.x);
  P->n = 0x3c;
}

void rob_cam_history_push(rob_rec *rob_a, rob_rec *rob_b)
{
  typedef struct { float x; float y; float z; } vec;
  typedef struct { int x; int y; } pair;
  typedef struct { int a; vec v; pair w; } ent;
  typedef struct { int pad; ent e[2][12]; } hist;
  int i;

  for (i = 11; i > 0; i--) {
    ((hist *)&rob_cam_history)->e[0][i].a = ((hist *)&rob_cam_history)->e[0][i - 1].a;
    ((hist *)&rob_cam_history)->e[0][i].v = ((hist *)&rob_cam_history)->e[0][i - 1].v;
    ((hist *)&rob_cam_history)->e[0][i].w = ((hist *)&rob_cam_history)->e[0][i - 1].w;
    ((hist *)&rob_cam_history)->e[1][i].a = ((hist *)&rob_cam_history)->e[1][i - 1].a;
    ((hist *)&rob_cam_history)->e[1][i].v = ((hist *)&rob_cam_history)->e[1][i - 1].v;
    ((hist *)&rob_cam_history)->e[1][i].w = ((hist *)&rob_cam_history)->e[1][i - 1].w;
  }
  ((hist *)&rob_cam_history)->e[0][0].a = rob_a->side;
  ((hist *)&rob_cam_history)->e[0][0].v = *(vec *)&rob_a->ground_pos;
  ((hist *)&rob_cam_history)->e[0][0].w = *(pair *)rob_a->screen_xy;
  ((hist *)&rob_cam_history)->e[1][0].a = rob_b->side;
  ((hist *)&rob_cam_history)->e[1][0].v = *(vec *)&rob_b->ground_pos;
  ((hist *)&rob_cam_history)->e[1][0].w = *(pair *)rob_b->screen_xy;
}

void rob_cam_target_update(camera_state *cam, rob_rec *rob_a, rob_rec *rob_b, float dist, float rate)
{
  typedef struct { int w[3]; } vec_blk;
  vec_blk old;
  float d[3];
  float base;
  float h;
  float lim;
  base = stage_cur->center.y;
  old = *(vec_blk *)&cam->target;
  vec3_weighted_sum(&rob_cam_hist_a_oldest_pos.x, &rob_cam_hist_b_oldest_pos.x, 0.5, &cam->goal.x, 0.5);
  if (__builtin_fabsf(rate - 1.0) < 1.1920929e-07) {
    *(vec_blk *)&cam->target = *(vec_blk *)&cam->goal;
  }
  else {
    vec3_sub(d, &cam->target.x, &cam->goal.x);
    if ((rob_a->state & 0x9000) != 0 && (rob_a->state & 0x400) == 0 &&
        (rob_b->state & 0x9000) != 0 && (rob_b->state & 0x400) == 0 &&
        (__builtin_fabsf(d[0]) < 0.2 || __builtin_fabsf(d[2]) < 0.2)) {
      rob_cam_history_end.hold_frames++;
    }
    else {
      rob_cam_history_end.hold_frames = 0;
    }
    if (rob_cam_history_end.hold_frames < 0x3c) {
      vec3_weighted_sum(&cam->goal.x, &cam->target.x, rate, &cam->target.x, 1.0 - rate);
    }
  }
  h = dist * 0.28572524 + 0.009461561 + base;
  if (__builtin_fabsf(cam->target.y - h) > 0.005) {
    cam->target.y = h;
  }
  lim = base + 1.0;
  if (cam->target.y < lim) {
    cam->target.y = lim;
  }
}

void rob_cam_eye_update(cam, rob_a, rob_b, dist, rate, follow) camera_state *cam; rob_rec *rob_a; rob_rec *rob_b; float dist; float rate; char follow;
{
  typedef struct { uint w[3]; } pos3;
  float v[3];
  float x;
  float len;
  float a;
  float b;
  float t;
  float d;
  short base;
  short ang;
  short diff;
  short step;
  rob_rec *rob;

  float *from = &rob_cam_hist_a_oldest_pos.x;
  float *to = &rob_cam_hist_b_oldest_pos.x;
  x = stage_cur->center.y;
  v[0] = to[0] - from[0];
  v[1] = 0.0;
  v[2] = to[2] - from[2];
  len = vec3_length(v);
  if (fabs(len - rob_cam_history.rob_gap) < 0.05) {
    len = rob_cam_history.rob_gap;
  }
  else {
    rob_cam_history.rob_gap = len;
  }
  a = rob_a->pos.y;
  b = rob_b->pos.y;
  if (a < x) a = x;
  if (b < x) b = x;
  d = b - a;
  if (d < 0.0) d = -d;
  x = d * h_rate + h_add;
  if (x < len) x = len;
  x *= dist_rate;
  x = min_dist > x ? min_dist : x;
  rob_cam_eye_dist = x;
  base = -0x8000 - cam->yaw;
  if ((((rob_a->state | rob_b->state) & 0x300000) != 0) & (follow != 0)) {
    if (rob_a->recovery_end > rob_b->recovery_end) rob = rob_b;
    else rob = rob_a;
    if (rob->recovery_end - 20 > rob->frame) {
      ang = base;
    }
    else {
      ang = atan2_s16(v[0],-v[2]) + 0x4000;
    }
  }
  else {
    ang = atan2_s16(v[0],-v[2]) + 0x4000;
    diff = ang - base;
    if ((ushort)(diff + 0x7ff) < 0xfff) {
      ang = (int)(diff * 0.05) + base;
    }
    else if (diff > 0) {
      ang = base + diff + -0x800;
    }
    else {
      ang = base + diff + 0x800;
    }
    step = ang - base;
    t = fabs(dist - x) * angrate;
    if (t > 1.0) t = 1.0;
    step = (int)((diff - step) * t) + step;
    if (rob_cam_recenter != 0) {
      if (step > 0x1000) step = 0x1000;
      else if (step < -0x1000) step = -0x1000;
    }
    ang = base + step;
  }
  rob_cam_yaw_step = ang - base;
  vec3_move_on_heading(ang,&cam->target.x,&cam->field_0x0,x);
  if (fabs(rate - 1.0) < 1.1920929e-07) {
    *(pos3 *)&cam->eye = *(pos3 *)&cam->field_0x0;
  }
  else if (rob_cam_history_end.hold_frames < 0x3c) {
    vec3_weighted_sum(&cam->field_0x0,&cam->eye.x,rate,&cam->eye.x,1.0 - rate);
  }
  cam->eye.y = cam->target.y;
}

void rob_cam_place(camera_state *cam)
{
  typedef struct { float x, y, z; } vec;
  rob_rec *rob_a;
  rob_rec *rob_b;
  float dz;
  float dx;
  vec v;

  rob_a = rob_get(0);
  rob_b = rob_get(1);
  dx = 0.0;
  dz = min_dist;
  if (mode_get_sub() == 0xd) {
    if (rob_a->screen_xy[0] > rob_b->screen_xy[0]) {
      rob_a = rob_get(1);
      rob_b = rob_get(0);
    }
  }
  else if (stage_cur_no == 1) {
    dz = dz * -1.0;
  }
  else if (stage_cur_no == 3) {
    dx = dz;
    dz = 0.0;
  }
  v = *(vec *)&stage_cur->center.x;
  v.y = v.y + 1.0;
  *(vec *)&cam->target = v;
  *(vec *)&cam->goal = v;
  *(vec *)&cam->field_0x0 = v;
  cam->field_0x0 = cam->field_0x0 + dx;
  cam->field_0x8 = cam->field_0x8 + dz;
  *(vec *)&cam->eye = *(vec *)&cam->field_0x0;
  cam->pitch = 0;
  cam->yaw = 0;
  cam->roll = 0;
  rob_cam_recenter = 0;
  rob_cam_history_fill(rob_a,rob_b);
  if (mode_get_sub() == 0xd) {
    rob_cam_target_update(cam,rob_a,rob_b,0.0,1.0);
    rob_cam_eye_update(cam,rob_a,rob_b,0.0,1.0,0);
  }
}

void cam_mode_rob(void)
{
  camera_state *cam = &camera;
  int follow = 1;
  int busy = 0;
  float k;
  rob_rec *rob_b;
  rob_rec *rob_a;
  rob_rec *rob;
  float d;

  if (pause_frame_held() != 0) return;
  if ((cam->flags & 0x20) != 0) {
    if (--*(int *)&cam->hold_frames != -1) {
      busy = 1;
    }
    else {
      cam->flags &= ~0x20;
      *(int *)&cam->hold_frames = 0;
    }
  }
  if (busy) return;
  if ((cam->flags & 8) != 0) {
    rob_cam_place(cam);
    goto done;
  }
  if ((cam->flags & 0x40) != 0) {
    rob_cam_place(cam);
    cam->flags &= ~0x40;
    k = 1.0;
  }
  else {
    k = 0.1;
  }
  if (mode_get_main() == 3 && mode_get_sub() == 9) return;
  rob_a = rob_get(0);
  rob_b = rob_get(1);
  if (rob_a->screen_xy[0] > rob_b->screen_xy[0]) {
    rob_b = rob_get(0);
    rob_a = rob_get(1);
  }
  vec3_distance(&rob_a->ground_pos.x, &rob_b->ground_pos.x);
  if (((rob_a->state | rob_b->state) & 0x04000000) != 0) {
    if (rob_cam_recenter_frames > 6) {
      rob_cam_history_fill(rob_a, rob_b);
      k = 0.1;
    }
    rob_cam_recenter_frames++;
  }
  else if (((rob_a->cam_bits | rob_b->cam_bits) & 1) != 0) {
    if (rob_cam_recenter_frames == 0) rob_cam_recenter_side = rob_b->side;
    if (rob_cam_recenter_frames > 0x14) {
      rob_cam_history_fill(rob_a, rob_b);
      follow = 0;
      rob_cam_recenter = 1;
      k = 0.3;
    }
    rob_cam_recenter_frames++;
  }
  else if (((rob_a->cam_bits | rob_b->cam_bits) & 2) != 0) {
    if (rob_cam_recenter_frames == 0) rob_cam_recenter_side = rob_b->side;
    if (rob_cam_recenter_frames > rob_cam_recenter_delay) {
      rob_cam_history_fill(rob_a, rob_b);
      follow = 0;
      rob_cam_recenter = 1;
      k = 0.18;
    }
    rob_cam_recenter_frames++;
  }
  else if (((rob_a->cam_bits | rob_b->cam_bits) & 4) != 0) {
    if (rob_cam_recenter_frames == 0) rob_cam_recenter_side = rob_b->side;
    if (rob_cam_recenter_frames > rob_cam_recenter_delay) {
      rob_cam_history_fill(rob_a, rob_b);
      follow = 0;
      rob_cam_recenter = 1;
      k = 0.07;
    }
    rob_cam_recenter_frames++;
  }
  else {
    rob_cam_recenter_frames = 0;
  }
  if (rob_cam_recenter != 0) if (rob_cam_recenter == 1) {
    rob = rob_get(rob_cam_recenter_side);
    if (rob_a->side == rob->side) {
      rob_cam_recenter = 0;
    }
    else {
      rob_a = rob;
      rob_b = rob->opp;
    }
    rob_cam_history_end.hold_frames = 0;
  }
  rob_cam_history_push(rob_a, rob_b);
  if (rob_a->side != ((struct { int p[4]; int f; } *)&rob_cam_hist_a_tail)->f) {
    rob_cam_history_fill(rob_a, rob_b);
  }
  d = vec3_distance(&cam->eye.x, &cam->target.x);
  rob_cam_target_update(cam, rob_a, rob_b, d, k);
  rob_cam_eye_update(cam, rob_a, rob_b, d, k, follow);
done:
  camera_angles_update(cam);
}

void cam_mode_fix(void)
{
  return;
}

void cam_mode_test(void)
{
  float step;
  step = vec3_distance(&camera.target.x,&camera.eye.x);
  if (step <= 0.1) {
    step = 0.1;
  }
  step = step * 0.012;
  test_cam_move_eye(&camera,step);
  test_cam_move_target(&camera,step);
  camera_angles_update(&camera);
  test_cam_ball_draw();
  cam_mode_test_nop();
  return;
}

void cam_mode_stay(void)
{
  rob_rec *rob;
  float k;
  rob_rec *rob_a;
  rob_rec *rob_b;

  if ((char)camera.cam_rob_no >= 0) {
    k = 0.02;
    rob = rob_get((uint)camera.cam_rob_no);
    if (vec3_distance(&rob->pos.x,&camera.target.x) > 0.8) {
      camera.target.x =
           (rob->pos.x - camera.target.x) * k +
           camera.target.x;
      camera.target.y =
           (rob->pos.y - camera.target.y) * k +
           camera.target.y;
      camera.target.z =
           (rob->pos.z - camera.target.z) * k +
           camera.target.z;
    }
  }
  else {
    rob_a = rob_get(0);
    rob_b = rob_get(1);
    vec3_weighted_sum(&rob_a->pos.x,&rob_b->pos.x,0.5,&camera.target.x,0.5);
  }
}

void cam_mode_sat(void)
{
  typedef struct { float x, y, z; } v3;
  camera_state *a = &camera;
  v3 *t = (v3 *)cam_sat_offsets[a->sat_row];
  rob_rec *rob;
  float *pos;
  float *h;
  v3 l;
  rob = rob_get((uint)a->cam_rob_no);
  vec3_add(&a->target.x,pos = &rob->pos.x,(float *)t++);
  l = *t;
  h = &a->target.y;
  *h = rob->ground_pos.y + 1.0;
  mtx_push(0);
  mtx_load_identity();
  mtx_translate_by_vec3(pos);
  mtx_rot_y_s16(-rob->heading);
  mtx_xform_point((float *)&l,(float *)&l);
  l.y = *h;
  *(v3 *)&a->eye.x = l;
  mtx_pop(1);
}

void cam_mode_softimage(void)
{
  uint flags;
  cam_clip_rec *clip;
  float t, a, b, spd;
  cam_curve_rec **pp;
  ushort *ph;
  float *dst;
  short i;
  int e;
  float *s = (float *)&cam_anim_sample;
  camera_state *cam;
  int ry, rx;
  float dx, dz;

  switch (cam_anim_kind) {
  case 1:
    clip = cam_clips_nagerare[cam_anim_index];
    nagerare = cam_anim_index;
    break;
  case 2:
    clip = cam_clips_chara[cam_anim_index];
    break;
  case 4:
    clip = cam_clips_adv[cam_anim_index];
    break;
  case 5:
    clip = cam_clips_kind5[cam_anim_index_kind5];
    break;
  case 0:
  default:
    clip = cam_clips_camera[cam_anim_index_camera];
    break;
  }
  a = *clip->time_range;
  b = clip->time_range[1];
  t = cam_anim_time;
  flags = cam_anim_flags;
  if (flags & 1) {
    flags &= ~0x25;
    if (cam_anim_kind != 1) {
      cam_anim_time = 0.0;
    }
    if (flags & 2) {
      typedef struct { char pad[12]; cam_anim_start_eye_rec pos; cam_anim_start_target_rec at; } cam_vecs;
      flags &= ~2;
      flags |= 4;
      cam_anim_start_eye = ((cam_vecs *)&camera)->pos;
      cam_anim_start_target = ((cam_vecs *)&camera)->at;
      cam_anim_start_pers = camera.proj.pers;
      cam_anim_start_roll = (float)camera.roll * 360.0 * 1.5258789e-05;
    }
    cam_anim_ended = 0;
  }
  else if ((cam_anim_kind == 1 || cam_anim_kind == 2) &&
           ((rob_get(sic_rob_id))->side_flags & 0x20)) {
    if (mode_get_main() == MODE_MAIN_INTRUDE && cam_anim_kind == 2) {
      return;
    }
    cam_anim_flags = 0;
    camera.pers_req = 45.0;
    camera.mode = camera.mode_return;
    if ((char)camera.mode == MODE_ROB) {
      cam_mode_rob();
    }
    return;
  }
  if (flags & 0x10) {
    flags &= ~0x18;
    flags |= 0x20;
  }
  if (flags & 8) {
    if (b < t + a + (float)cam_anim_end_offset) {
      flags &= ~8;
      flags |= 0x20;
    }
  }
  if ((flags & 0x20) == 0) {
    pp = clip->channels;
    ph = clip->channel_heads;
    dst = (float *)&cam_anim_sample;
    if (pause_frame_held() == 0) {
      if (cam_anim_kind == 1 || cam_anim_kind == 2) {
        spd = (rob_get(sic_rob_id))->frame_speed;
      }
      else {
        spd = mode_timer_rate;
      }
      t = spd * 0.016666668 + t;
    }
    if (b < t + a) {
      if (cam_anim_flags & 0x80) {
        cam_anim_flags = 0;
        camera.mode = camera.mode_return;
        camera.pers_req = 45.0;
        if ((char)camera.mode == MODE_ROB) {
          cam_mode_rob();
        }
        return;
      }
      if (clip->flags & 0x40) {
        t = t - (b - a);
      }
      else {
        t = b - a;
      }
      cam_anim_ended = 1;
    }
    for (i = 0; i < 8; i++, ph++, pp++, dst++) {
      e = (int)*pp;
      if (e != 0) {
        *dst = curve_half_sample(*(short **)(e + 4), *(ushort *)e, a + cam_anim_time);
      }
      else {
        *dst = half_to_float(*ph);
      }
    }
    cam_anim_time = t;
    switch (cam_anim_kind) {
    case 1: {
      rob_rec *rob1;
      float *v1, *w1;
      rob1 = rob_get(sic_rob_id);
      v1 = s;
      w1 = v1 + 3;
      if (rob1->side_flags & 0x10) {
        *v1 *= -1.0;
        *w1 *= -1.0;
      }
      mtx_push_identity();
      mtx_set_current_wordwise(rob1->place_mtx);
      mtx_xform_point(w1, w1);
      if (w1[1] < stage_cur->center.y) {
        w1[1] = stage_cur->center.y;
      }
      mtx_xform_point(v1, v1);
      if (v1[1] < stage_cur->center.y + 0.5) {
        v1[1] = stage_cur->center.y + 0.5;
      }
      if (stage_cur_no == 0x15 && v1[1] < 5.2) {
        v1[1] = 5.2;
      }
      mtx_pop(1);
      break;
    }
    case 2:
    case 4: {
      rob_rec *rob2;
      float *v2, *w2;
      rob2 = rob_get(sic_rob_id);
      v2 = s;
      w2 = v2 + 3;
      if (rob2->side_flags & 0x10) {
        *v2 *= -1.0;
        *w2 *= -1.0;
      }
      mtx_push_identity();
      mtx_set_current_wordwise(rob2->place_mtx);
      mtx_xform_point(v2, v2);
      mtx_xform_point(w2, w2);
      mtx_pop(1);
      break;
    }
    case 0:
    case 3:
    case 5:
      break;
    }
    cam = &camera;
    cam_anim_eye.x = s[0];
    cam_anim_eye.y = s[1];
    cam_anim_eye.z = s[2];
    cam_anim_target.x = s[3];
    cam_anim_target.y = s[4];
    cam_anim_target.z = s[5];
    cam_anim_pers = s[7];
    cam_anim_roll = (short)(int)(s[6] * 65536.0 / 360.0 + 0.5);
    if (sic_disp_sw != 0) {
      dx = s[0] - s[3];
      dz = s[2] - s[5];
      ry = atan2_s16(dx, dz);
      dx = s[3] - s[0];
      dz = s[5] - s[2];
      rx = atan2_s16(s[4] - s[1], fsqrt_asm(dx * dx + dz * dz));
      mtx_push(0);
      mtx_set_current_wordwise(camera_view_mtx);
      mtx_push(0);
      mtx_translate(s[0], s[1], s[2]);
      mtx_rot_z_s16((int)cam_anim_pers);
      mtx_rot_y_s16(ry);
      mtx_rot_x_s16(rx);
      mtx_rot_y_s16(0x4000);
      mtx_scale_xyz(0.5, 0.5, 0.5);
      obj_draw(0x24);
      mtx_pop(1);
      mtx_translate(s[3], s[4], s[5]);
      mtx_scale_xyz(0.1, 0.1, 0.1);
      obj_draw(0x46);
      mtx_pop(1);
      cam_mode_test();
    }
    else {
      cam->eye.x = s[0];
      cam->eye.y = s[1];
      cam->eye.z = s[2];
      cam->target.x = s[3];
      cam->target.y = s[4];
      cam->target.z = s[5];
      cam->roll = (short)(int)(s[6] * 65536.0 / 360.0 + 0.5);
      cam->proj.pers = cam->pers_req = s[7];
      camera_angles_update(cam);
    }
  }
  cam_anim_flags = flags;
  rob_cam_history_push(rob_get(0), rob_get(1));
}

void cam_throw_anim_start(rob_rec *rob)
{
  rob_rec *opp;
  uchar flip;

  opp = rob->opp;
  sic_rob_id = rob->side;
  cam_anim_kind = 1;
  cam_anim_index = ((int)lcg_next15() % 120) * 2;
  flip = 0;
  if ((rob->side_flags & 0x10) != 0) flip = 1;
  if ((rob->state & 0x400000) != 0) flip++;
  if (rob->screen_xy[0] > opp->screen_xy[0]) {
    if (!(flip & 1)) cam_anim_index++;
  }
  else if (flip & 1) cam_anim_index++;
  cam_anim_time = (rob->frame_pos - 1.0) * 0.016666668;
  cam_anim_flags = cam_anim_flags | 0x81;
  camera.mode = SOFTIMAGE;
}

void camera_angles_update(camera_state *cam)
{
  float dx;
  float dz;
  float d;
  float dy;

  dx = cam->eye.x - cam->target.x;
  dz = cam->eye.z - cam->target.z;
  cam->yaw = atan2_s16(dx, dz);
  dx = cam->target.x - cam->eye.x;
  dz = cam->target.z - cam->eye.z;
  d = fsqrt_asm(dx * dx + dz * dz);
  dy = cam->target.y - cam->eye.y;
  cam->pitch = atan2_s16(dy, d);
}

void test_cam_pad_vector(int pad, float step, float *vec)
{
  float f = step;
  float *d = vec;
  uint u;

  u = player_input_get(pad)->now;
  if ((u & 0x20) != 0) {
    *d = -f;
  }
  if ((u & 0x10) != 0) {
    *d = f;
  }
  if ((u & 1) != 0) {
    if ((u & 0x80) != 0) {
      d[2] = -f;
    }
    if ((u & 0x40) != 0) {
      d[2] = f;
    }
  }
  else {
    if ((u & 0x80) != 0) {
      d[1] = f;
    }
    if ((u & 0x40) != 0) {
      d[1] = -f;
    }
  }
  return;
}

void test_cam_move_eye(camera_state *cam, float step)
{
  test_cam_move(0,cam,step);
  return;
}

void test_cam_move_target(camera_state *cam, float step)
{
  test_cam_move(1,cam,step);
  return;
}

void test_cam_move(int which, camera_state *cam, float step)
{
  float v[3];

  v[0] = v[1] = v[2] = 0.0;
  mtx_push_identity();
  mtx_rot_y_s16(cam->yaw);
  mtx_rot_x_s16(cam->pitch);
  test_cam_pad_vector(which,step,v);
  mtx_xform_point(v,v);
  if (which == 0) {
    cam->eye.x = cam->eye.x + v[0];
    cam->eye.y = cam->eye.y + v[1];
    cam->eye.z = cam->eye.z + v[2];
  }
  else {
    cam->target.x = cam->target.x + v[0];
    cam->target.y = cam->target.y + v[1];
    cam->target.z = cam->target.z + v[2];
  }
  mtx_pop(1);
}

void test_cam_ball_draw(void)
{
  if ((int_ball_disp == ON_OFF_ON) && (camera.mode == MODE_TEST)) {
    mtx_push(0);
    mtx_set_current_wordwise(camera_view_mtx);
    mtx_translate(camera.target.x,camera.target.y,
                  camera.target.z);
    mtx_scale_xyz(0.1,0.1,0.1);
    obj_draw(0x46);
    mtx_pop(1);
  }
  return;
}

void cam_mode_test_nop(void)
{
  return;
}

void camera_shake_request(uint req, uint kind)
{
  camera.shake.req = (uchar)req;
  camera.shake.req_kind = (uchar)kind;
  return;
}

void camera_shake_request_clear(void)
{
  camera.shake.req = 0;
  return;
}

void camera_shake_take_request(cam_shake *shake)
{
  char *req;

  req = (char *)&shake->req;
  if (*req != '\0') {
    shake->state = *req;
    shake->kind = shake->req_kind;
    *req = '\0';
  }
  return;
}

void camera_shake_step(cam_shake *shake)
{
  if (camera.mode == REPLAY) {
    shake->state = 0;
  }
  switch ((char)shake->state) {
  case 1:
    camera_shake_begin(shake);
  case 2:
    camera_shake_next_offset(shake);
    break;
  default:
    shake->state = 0;
    shake->x = 0;
    shake->y = 0;
    shake->field_0x4 = 0;
  }
}

void camera_shake_begin(cam_shake *shake)
{
  shake->step = 0;
  shake->state = shake->state + '\x01';
  return;
}

void camera_shake_next_offset(cam_shake *shake)
{
  int idx;
  short *ps;
  int a;
  int b;

  idx = (char)shake->kind & 3;
  if (idx > 3) idx = 3;
  ps = camera_shake_tables[idx];
  if (((char)shake->kind & 4) == 0) {
    a = 0;
    b = 1;
  }
  else {
    a = 1;
    b = 0;
  }
  ps = ps + shake->step * 2;
  if (*ps == 0x7fff) {
    shake->state = 0;
    shake->x = 0;
    shake->y = 0;
  }
  else {
    shake->x = ps[a];
    shake->y = ps[b];
    shake->step = shake->step + 1;
  }
}

void cam_mode_sic_test(void)
{
  uint flags;
  cam_clip_rec *clip;
  float t, a, b;
  cam_curve_rec **pp;
  ushort *ph;
  float *dst;
  short i;
  int e;
  rob_rec *rob;
  float *v, *w, *s;

  flags = cam_anim_flags;
  clip = cam_clips_nagerare[(short)nagerare];
  a = *clip->time_range;
  b = clip->time_range[1];
  t = cam_anim_time;
  if ((flags & 1) || padhit(1)) {
    flags &= ~1;
    cam_anim_time = 0.0;
    t = 0.0;
  } else {
    if (padhit(2)) {
      sic_test_paused ^= 1;
    }
  }
  if (sic_test_paused) {
    if (padhit(4) == 0) {
      return;
    }
  }
  pp = clip->channels;
  ph = clip->channel_heads;
  dst = (float *)&cam_anim_sample;
  if (pause_frame_held() == 0) {
    t = (rob_get(sic_rob_id))->frame_speed * 0.016666668 + t;
  }
  if (b < t + a) {
    if (clip->flags & 0x40) {
      t = t - (b - a);
    } else {
      t = b - a;
    }
  }
  cam_anim_time = t;
  t += a;
  for (i = 0; i < 8; i++, ph++, pp++, dst++) {
    e = (int)*pp;
    if (e != 0) {
      *dst = curve_half_sample(*(short **)(e + 4), *(ushort *)e, t);
    } else {
      *dst = half_to_float(*ph);
    }
  }
  rob = rob_get(sic_rob_id);
  v = (float *)&cam_anim_sample;
  w = v + 3;
  if (rob->side_flags & 0x10) {
    *v *= -1.0;
    *w *= -1.0;
  }
  mtx_push_identity();
  mtx_set_current_wordwise(rob->place_mtx);
  mtx_xform_point(w, w);
  mtx_xform_point(v, v);
  mtx_pop(1);
  s = (float *)&cam_anim_sample;
  camera.eye.x = s[0];
  camera.eye.y = s[1];
  camera.eye.z = s[2];
  camera.target.x = s[3];
  camera.target.y = s[4];
  camera.target.z = s[5];
  camera.roll = (short)(int)(s[6] * 65536.0 / 360.0 + 0.5);
  camera.proj.pers = camera.pers_req = s[7];
  camera_angles_update(&camera);
  cam_anim_flags = flags;
}

void replay_cam_reset(void)
{
  rob_rec *rob_a;
  rob_rec *rob_b;
  float d;
  char i;
  camera_state *g = &camera;

  rob_a = rob_get(0);
  rob_b = rob_get(1);
  vec3_weighted_sum(&rob_a->pos.x, &rob_b->pos.x, 0.5, &g->target.x, 0.5);
  g->goal.x = g->target.x;
  g->goal.y = g->target.y;
  g->goal.z = g->target.z;
  replay_cam.shot_frames = 0;
  replay_cam.last_eye = 0xff;
  replay_cam.last_height = -1;
  replay_cam.history_pos = 0;
  replay_cam.pers_phase = 1;
  replay_cam.pers_frames = 0;
  replay_cam.pers_row = 0;
  replay_cam.pers_row_last = 0;
  replay_cam.line_frames = 0;
  replay_cam.range_frames = 0;
  replay_cam_eye_pick(g, rob_a, rob_b);
  g->pitch = 0;
  g->yaw = 0;
  g->roll = 0;
  d = vec3_distance(&rob_a->ground_pos.x, &rob_b->ground_pos.x);
  for (i = 0; i < 20; i++) {
    replay_cam.history[i].mid = g->target;
    replay_cam.history[i].dist = d;
  }
}

float replay_cam_pers_from_dist(float dist, float pers)
{
  pers_range *t;
  char i;
  float r;
  if (mode_get_main() == MODE_MAIN_ENDING)
  {
    t = cam_pers_dist_table_ending;
  }
  else
  {
    t = cam_pers_dist_table;
  }
  r = pers;
  switch (replay_cam.pers_phase)
  {
    case 1:
      replay_cam.pers_row_last = replay_cam.pers_row;
      for (i = 0; t->tag != (-1); i++, t++)
    {
      if ((!(dist < t->lo)) && (dist < t->hi))
      {
        replay_cam.pers_row = i;
        break;
      }
    }

      if (replay_cam.pers_row_last != replay_cam.pers_row)
    {
      replay_cam.pers_phase = 2;
      replay_cam.pers_frames = 0;
    }
      break;

    case 2:
 { char row = replay_cam.pers_row; r = r + (((t[row].val - pers) * ((float) replay_cam.pers_frames)) / 30.0f); } if (replay_cam.pers_frames == 30) { r = pers;
      replay_cam.pers_phase = 0;
      replay_cam.pers_frames = 0;
    }
      if (dist || cam_pers_dist_table)
    {
      replay_cam.pers_frames = replay_cam.pers_frames + 1;
      break;
    }
    else
    {
      replay_cam.pers_frames = replay_cam.pers_frames + 1;
      break;
    }

    case 0:
      replay_cam.pers_frames = replay_cam.pers_frames + 1;
      if (replay_cam.pers_frames > 60)
    {
      replay_cam.pers_phase = 1;
    }
      break;

  }

  return r;
}

void mtx_origin_to_screen(float *mtx, float *xy)
{
  float pos [3];
  mtx_push(0);
  mtx_set_current_wordwise(mtx);
  mtx_store_translation(pos);
  camera_view_load();
  ((void (*)(float,float))mtx_save_proj_x_current)(640.0,480.0);
  proj_point_to_pixel_xy(pos,xy);
  mtx_pop(1);
}

char rob_parts_offscreen_count(rob_rec *rob)
{
  typedef struct { int w[16]; } blk;
  blk b;
  float v[2];
  char n;

  n = 0;
  b = *(blk *)(rob->mtx + 0x30);
  mtx_origin_to_screen((float *)&b,v);
  if (540.0 < v[0] || v[0] < 100.0 || 400.0 < v[1] || v[1] < 80.0) {
    n++;
  }
  b = *(blk *)(rob->mtx + 0x60);
  mtx_origin_to_screen((float *)&b,v);
  if (540.0 < v[0] || v[0] < 100.0 || 400.0 < v[1] || v[1] < 80.0) {
    n++;
  }
  b = *(blk *)(rob->mtx + 0xb0);
  mtx_origin_to_screen((float *)&b,v);
  if (540.0 < v[0] || v[0] < 100.0 || 400.0 < v[1] || v[1] < 80.0) {
    n++;
  }
  b = *(blk *)(rob->mtx + 0x120);
  mtx_origin_to_screen((float *)&b,v);
  if (540.0 < v[0] || v[0] < 100.0 || 400.0 < v[1] || v[1] < 80.0) {
    n++;
  }
  b = *(blk *)(rob->mtx + 0x160);
  mtx_origin_to_screen((float *)&b,v);
  if (540.0 < v[0] || v[0] < 100.0 || 400.0 < v[1] || v[1] < 80.0) {
    n++;
  }
  return n;
}

float cam_dir_dot_rob_axis(rob_rec *rob_a, rob_rec *rob_b)
{
  float *p;
  float a [3];
  float b [3];
  
  p = &camera.eye.x;
  vec3_sub(a,p,p + 3);
  vec3_normalize(a);
  vec3_sub(b,&rob_a->ground_pos.x,&rob_b->ground_pos.x);
  vec3_normalize(b);
  return vec3_dot(a,b);
}

void replay_cam_eye_pick(camera_state *cam, rob_rec *rob_a, rob_rec *rob_b)
{
  char cur;
  char idx;
  char col;
  vec3 *tbl;
  uchar o;
  uchar j;
  float d;

  cur = replay_cam.last_eye;
  replay_cam_pick_word = 0;
  if (stage_cur_no == 2 || stage_cur_no == 4 || stage_cur_no == 0x10 || stage_cur_no == 0 ||
      stage_cur_no == 0x14 || stage_cur_no == 1) {
    tbl = replay_cam_eyes_a;
  }
  else {
    tbl = replay_cam_eyes_b;
  }
  idx = (int)lcg_next15() % 33;
  col = vblank_count % 8;
  if (replay_cam.last_height == col) {
    col = (col + 2) % 8;
  }
  for (o = 0; o < 8; o++) {
    for (j = 0; j < 33; j++) {
      if (idx != cur) {
        cam->eye = tbl[idx];
        cam->eye.y = replay_cam_heights[col];
        cam->eye.y = cam->eye.y + stage_cur->center.y;
        if (stage_cur_no == 2) {
          cam->eye.y = cam->eye.y + 0.5;
        }
        else if ((stage_cur_no == 4 || stage_cur_no == 5) &&
                 cam->eye.y > stage_cur->center.y + 4.0) {
          cam->eye.y = stage_cur->center.y + 2.0;
        }
        d = vec3_distance(&cam->eye.x,&cam->target.x);
        if (d > 4.0 && d < 8.0) {
          if (__builtin_fabsf(cam_dir_dot_rob_axis(rob_a,rob_b)) < 0.8) {
            camera_angles_update(cam);
            if (__builtin_fabsf(__builtin_fabsf(cam->pitch * 360.0 * 1.5258789e-05) - 90.0) > 15.0)
              goto done;
          }
        }
      }
      idx = (idx + 1) % 33;
    }
    col = (col + 1) % 8;
  }
done:
  replay_cam.last_eye = idx;
  replay_cam.last_height = col;
}

void cam_mode_replay(void)
{
  typedef struct { float x; float y; float z; } v3;
  camera_state *cam = &camera;
  char *n = &replay_cam.history_pos;
  rob_rec *rob_a;
  rob_rec *rob_b;
  float *a;
  float *b;
  char old;
  replay_cam_hist_rec *e;
  float k;
  float f;
  float r;
  int flag;
  int lim;
  char c0;
  char c1;
  float diff[3];
  rob_a = rob_get(0);
  rob_b = rob_get(1);
  if (pause_frame_held() == 0) {
    if ((cam->flags & 8) != 0) {
      replay_cam_reset();
    }
    else {
      a = &rob_a->pos.x;
      b = &rob_b->pos.x;
      vec3_weighted_sum(a, b, 0.5, &replay_cam.history[*n].mid.x, 0.5);
      replay_cam.history[*n].dist = vec3_distance(a, b);
      old = *n;
      *n = *n + 1;
      if (*n > 19) *n = 0;
      e = &replay_cam.history[*n];
      k = 0.1;
      if (replay_cam_goal_frames == 20) {
        cam->goal = replay_cam.history[old].mid;
        replay_cam_goal_frames = 0;
      }
      else {
        replay_cam_goal_frames++;
      }
      vec3_sub(diff, &cam->target.x, &cam->goal.x);
      vec3_weighted_sum(&cam->goal.x, &cam->target.x, k, &cam->target.x, 1.0 - k);
      if (fabsf(rob_a->ground_pos.y - rob_b->ground_pos.y) > 0.4) {
        f = stage_cur->center.y;
      }
      else {
        f = stage_cur->center.y + 1.0;
      }
      if (f > cam->target.y) {
        cam->target.y = f;
      }
      flag = 0;
      if (mode_get_main() == MODE_MAIN_GAME) {
        lim = 180;
      }
      else {
        lim = 1800;
      }
      r = vec3_distance(&camera.eye.x, &camera.target.x);
      if (r < 4.0 || r > 8.0) {
        replay_cam.range_frames++;
      }
      else {
        replay_cam.range_frames = 0;
      }
      if (fabsf(cam_dir_dot_rob_axis(rob_a, rob_b)) > 0.9) {
        replay_cam.line_frames++;
      }
      else {
        replay_cam.line_frames = 0;
      }
      camera_angles_update(cam);
      if (fabsf(fabsf((float)cam->pitch * 360.0 * 1.5258789e-05) - 90.0) < 15.0) {
        flag = 1;
      }
      if (replay_cam.shot_frames >= lim || replay_cam.range_frames > 60 || replay_cam.line_frames > 60) {
        flag = 1;
      }
      if (flag) {
        replay_cam.shot_frames = 0;
        replay_cam.pers_phase = 1;
        replay_cam.pers_frames = 0;
        replay_cam.line_frames = 0;
        replay_cam_eye_pick(cam, rob_a, rob_b);
      }
      camera.flags |= 0x10;
      r = replay_cam_pers_from_dist(e->dist, camera.proj.pers);
      if (r < 12.0) r = 12.0;
      else if (r > 50.0) r = 50.0;
      camera.pers_req = r;
      c0 = rob_parts_offscreen_count(rob_a);
      c1 = rob_parts_offscreen_count(rob_b);
      if ((char)(c0 | c1) != 0) {
        camera.pers_req += replay_cam_offscreen_pers;
      }
    }
    replay_cam.shot_frames++;
    camera_angles_update(cam);
  }
}

void cam_mode_pointstage(void)
{
  float f = 0.0; /* ORIGINAL SHAPE (corroborated 2026-11-10: the zero locals f and z are forced, literal zeros in the stores do not match, and camera_reset above resets the same record through the same two locals) */
  int z = 0;

  if ((camera.flags & 8) != 0) {
    camera.field_0x0 = f;
    camera.field_0x4 = f;
    camera.field_0x8 = 5.0;
    camera.eye.x = f;
    camera.eye.y = f;
    camera.eye.z = 5.0;
    camera.target.x = f;
    camera.target.y = f;
    camera.target.z = f;
    camera.goal.x = f;
    camera.goal.y = f;
    camera.goal.z = f;
    camera.field_0x58 = f;
    camera.field_0x5c = f;
    camera.field_0x60 = f;
    camera.field_0x64 = f;
    camera.pitch = z;
    camera.yaw = z;
    camera.roll = z;
    camera.cam_rob_no = z;
    camera.sat_row = z;
    camera.hold_frames = z;
    camera.flags = 0;
    camera.mode_return = camera.mode;
    camera.mode_last = camera.mode;
    camera.proj = camera_proj_default;
    camera.pers_req = camera.proj.pers;
    camera.shake.state = 0;
    camera.shake.req = 0;
    camera.shake.x = 0;
    camera.shake.y = 0;
    camera.shake.field_0x4 = 0;
  }
  return;
}

void cam_mode_stage_orbit(void)
{
  typedef struct { float x; float y; float z; } v3;
  camera_state *c;
  float v[3];

  c = &camera;
  if ((c->flags & 8) != 0) {
    c->pers_req = 45.0;
    c->flags = c->flags | 0x10;
    *(v3 *)&c->eye.x = *(v3 *)&stage_cur->center.x;
    *(v3 *)&c->target.x = *(v3 *)&stage_cur->center.x;
    c->shake.x = 0;
    c->shake.y = 0;
    c->shake.field_0x4 = 0;
    stage_orbit_angle = 0.0;
    if (stage_cur_no == 1) {
      stage_orbit_angle = 180.0;
    }
  }
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = 3.5;
  mtx_push(0);
  mtx_load_identity();
  mtx_rot_y_s16((ushort)(int)((stage_orbit_angle * 65536.0) / 360.0 + 0.5));
  mtx_xform_point(v,v);
  mtx_pop(0);
  *(v3 *)&c->target.x = *(v3 *)&stage_cur->center.x;
  c->target.y = c->target.y + 0.7;
  *(v3 *)&c->eye.x = *(v3 *)&stage_cur->center.x;
  c->eye.x = c->eye.x + v[0];
  c->eye.y = v[1] + 0.5 + c->eye.y;
  c->eye.z = c->eye.z + v[2];
  stage_orbit_angle = stage_orbit_angle + 0.1;
  if (stage_orbit_angle > 360.0) {
    stage_orbit_angle = stage_orbit_angle - 360.0;
  }
  c->roll = 0;
  camera_angles_update(c);
}
