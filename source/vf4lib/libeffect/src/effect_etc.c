/*
 * vf4lib/libeffect/src/effect_etc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_line3d_quads_opaque
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern loop64_model_rec loop64_model; /* 0x0c454a10 */
extern int game_frame_count; /* 0x0c292540 */
extern float mode_timer_rate; /* 0x0c292678 */
extern int effect_rand_seed; /* 0x0c29278c */
extern short break_floor_request_tile; /* 0x0c292806 */
extern uchar *light_anim_table; /* 0x0c29e0c0 */
extern float light_anim_speed; /* 0x0c2a2d1c */
extern break_piece rome_fence_row0[25]; /* 0x0c2a3360 */
extern break_piece rome_fence_row1[25]; /* 0x0c2a36e4 */
extern break_piece *rome_fence_rows[3]; /* 0x0c2a3a68 */
extern break_piece yam_fence_row0[25]; /* 0x0c2a3fe8 */
extern break_piece yam_fence_row1[25]; /* 0x0c2a436c */
extern break_piece *yam_fence_rows[3]; /* 0x0c2a46f0 */
extern char rob_fx_flags; /* 0x0c2a4a8c */
extern char djo_light_cycle_index; /* 0x0c2a4d19 */
extern char audience_flags; /* 0x0c2a8d20 */
extern char audience_mood; /* 0x0c2a8d21 */
extern char audience_mood_target; /* 0x0c2a8d22 */
extern char audience_hold; /* 0x0c2a8d23 */
extern float audience_rate_target; /* 0x0c2a8d28 */
extern float audience_rate; /* 0x0c2a8d2c */
extern short fish_density_div; /* 0x0c2a988a */
extern short bird_density_div; /* 0x0c2a988c */
extern bird_model_set bird_model_sets[4]; /* 0x0c2a9890 */
extern short fish_seeker_turn_table[64]; /* 0x0c2a9fec */
extern char light_mot_choice; /* 0x0c2b0aa0 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern int nyc_light_flags; /* 0x0c386de0 */
extern float stage_light_anim_time; /* 0x0c386df4 */
extern djo_fence_fall_rec djo_fence_fall[24]; /* 0x0c386df8 */
extern float step_fx_floor_y; /* 0x0c3870cc */
extern char breath_phase[2]; /* 0x0c3870d0 */
extern uchar *break_fall_list[9]; /* 0x0c3870e0 */
extern uchar *oneshot_anim_pool; /* 0x0c387104 */
extern int break_floor_tile_info[400][6]; /* 0x0c387108 */
extern uchar **break_fall_list_end; /* 0x0c389688 */
extern char audience_volume_target; /* 0x0c38968d */
extern short fish_path_count; /* 0x0c38968e */
extern short fish_school_count; /* 0x0c389690 */
extern short bird_path_count; /* 0x0c389692 */
extern short fish_school_near_count; /* 0x0c389696 */
extern short bird_flock_count; /* 0x0c389698 */
extern mot_path_slot mot_path_slots[3]; /* 0x0c397b6c */
extern uchar * mot_paths_block; /* 0x0c397b84 */
extern aquarium_bubble *aquarium_bubbles; /* 0x0c454a20 */
extern fx_rec rob_fx_pool[16]; /* 0x0c454a24 */
extern short step_fx_wait[2]; /* 0x0c45565c */
extern float fx_saved_mtx[16]; /* 0x0c455660 */
extern short hbr_clock_hands; /* 0x0c4557a0 */
extern short cav_candle_frames[4]; /* 0x0c4557be */
extern short ban_flag_frames[50]; /* 0x0c4557c6 */
extern float dural_stage_scroll[2]; /* 0x0c455834 */
extern char dust_on; /* 0x0c45862c */
extern int dust_count; /* 0x0c458630 */
extern uchar *break_floor_model; /* 0x0c458638 */
extern int break_floor_hits; /* 0x0c45863c */
extern int break_floor_queue_len; /* 0x0c458640 */
extern int break_floor_queue[20]; /* 0x0c458644 */
extern uchar *break_floor_variants; /* 0x0c458694 */
extern audience_member *audience_members; /* 0x0c4586e8 */
extern bird_flock_member *bird_flock; /* 0x0c4586f0 */
extern bird_path_member *bird_paths; /* 0x0c4586f4 */
extern fish_school_member *fish_school; /* 0x0c458754 */
extern fish_path_rec *fish_paths; /* 0x0c458758 */
extern float stage_fx_base_mtx[16]; /* 0x0c458850 */
extern break_piece djo_fence_segs[25]; /* 0x0c2a46fc */
extern noh_etc_obj noh_etc_objs[4]; /* 0x0c2a4db8 */
extern break_piece break_floor_tiles[401]; /* 0x0c2a54b0 */
extern audience_anim_set audience_anim_sets[21]; /* 0x0c2a9750 */
extern fish_model_set fish_model_sets[5]; /* 0x0c2a9d30 */
extern fish_seeker_prm fish_seeker_params; /* 0x0c2aa06c */
extern short fish_mot_list[2]; /* 0x0c2b0aa2 */
extern stage_rec *stage_table[24]; /* 0x0c306c88 */
extern camera_state camera; /* 0x0c370ce0 */
extern break_floor_impact_pos break_floor_impact; /* 0x0c371a7c */
extern stage_anim_rec stage_anim_objs[2]; /* 0x0c382438 */
extern stage_anim_rec stage_anim_obj2[2]; /* 0x0c382498 */
extern nyc_heli nyc_helis[4]; /* 0x0c38651c */
extern nyc_searchlight nyc_searchlights[2]; /* 0x0c386aac */
extern nyc_searchlight nyc_searchlights_fixed[2]; /* 0x0c386b8c */
extern nyc_searchlight nyc_heli_light0; /* 0x0c386c6c */
extern nyc_searchlight nyc_heli_light1; /* 0x0c386cdc */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern fx_rec down_fx[2]; /* 0x0c454de4 */
extern step_fx_rec step_fx[64]; /* 0x0c454e5c */
extern garden_other_obj garden_other_objs; /* 0x0c4556a0 */
extern hbr_float_obj hbr_floats; /* 0x0c4556d8 */
extern umi_float_rec umi_float; /* 0x0c4557a4 */
extern dust_rec dust_records[20]; /* 0x0c45583c */
extern fish_seeker_rec fish_seeker; /* 0x0c4586f8 */
extern fish_path_rec fish_solo1; /* 0x0c45875c */
extern fish_path_rec fish_solo2; /* 0x0c4587ac */
extern fish_path_rec fish_solo3; /* 0x0c4587fc */
extern bird_spawn_box bird_spare_box; /* 0x0c2a99b8 */
extern uint game_flags; /* 0x0c2925b8 */

static inline float fsqrt_inline(float x)
{
  float r;
  __asm__ ("fsqrt %0" : "=f" (r) : "0" (x));
  return r;
}

static inline float dir_fsqrt(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsqrt_inl(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void aquarium_aqua_init(void)
{
  stage_light_anim_time = 0.0;
  aquarium_aqua_value = 0.0;
  light_anim_speed = 1.0;
  aquarium_frame_a = 0;
  aquarium_frame_b = 0;
  aquarium_frame_c = 0;
  aquarium_model_a = 0xffffffff;
  aquarium_model_b = 0xffffffff;
  aquarium_model_c = 0xffffffff;
  return;
}

void aquarium_aqua_update(void)
{
  short i;
  int n = 3;
  
  if (light_anim_sw == ON_OFF_ON) {
    char *lb = (char *)&light_sets;
    int sz = 0x44;
    stage_light_anim_time = light_anim_play(stage_light_anim_time,0);
    for (i = 0; i < 16 - n; i++) {
      char *sb = (char *)light_anim_lights;
      int k = i;
      int o = k * sz;
      light_src_rec *l = (light_src_rec *)(lb + o);
      light_src_rec *s;
      l[2] = *(s = (light_src_rec *)(sb + o));
      if ((unsigned)(i - 8) > 4) {
        light_src_rec *p = &light_sets[1][i + 3];
        float h = 0.5;
        l[19] = *s;
        p->color[0] *= h;
        if (p->color[0] > 2.0) p->color[0] = 2.0;
        p->color[1] *= h;
        if (p->color[1] > 2.0) p->color[1] = 2.0;
        p->color[2] *= h;
        if (p->color[2] > 2.0) p->color[2] = 2.0;
        p->angle_max = (ushort)(int)((float)p->angle_max * 1.5);
      }
    }
  }
  aquarium_aqua_frames_step();
}

void aquarium_aqua_frames_step(void)
{
  int held;
  
  held = pause_frame_held();
  if (held == 0) {
    if (aquarium_frame_b < 0x7e) {
      aquarium_frame_b = aquarium_frame_b + 1;
    }
    else {
      aquarium_frame_b = 0;
    }
    if (aquarium_frame_a < 0x7e) {
      aquarium_frame_a = aquarium_frame_a + 1;
    }
    else {
      aquarium_frame_a = 0;
    }
    if (aquarium_frame_c < 0x30) {
      aquarium_frame_c = aquarium_frame_c + 1;
    }
    else {
      aquarium_frame_c = 0;
    }
    aquarium_model_a = aquarium_models_a[(int)aquarium_frame_a >> 1];
    aquarium_model_b = aquarium_models_b[(int)aquarium_frame_b >> 1];
    aquarium_model_c = aquarium_models_c[aquarium_frame_c];
  }
  return;
}

void aquarium_aqua_draw(void)
{
  float near_clip;
  float *p;
  p = &camera.proj.near_clip;
  near_clip = *p;
  *p = 1.6;
  mtx_push(0);
  obj_draw(aquarium_model_c);
  obj_draw(aquarium_model_a);
  if (aquarium_model_b_on != '\0') {
    mtx_stack_select(3);
    mtx_load_identity();
    proj_from_vfov((int)((p[2] * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,p[0],p[1]);
    proj_cmds_send();
    mtx_stack_select(1);
    obj_draw(aquarium_model_b);
    *p = near_clip;
    mtx_stack_select(3);
    mtx_load_identity();
    proj_from_vfov((int)((p[2] * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,p[0],p[1]);
    proj_cmds_send();
    mtx_stack_select(1);
  }
  mtx_pop(1);
  return;
}

float light_anim_play(time, anim_no) float time; short anim_no;
{
  int *anim;
  float t;
  float start;
  float end;
  uint flags;
  int n;
  
  anim = (int *)(&light_anim_table)[anim_no];
  start = *(float *)*anim;
  end = ((float *)*anim)[1];
  t = time;
  flags = anim[1];
  n = 0;
  if (pause_frame_held() == 0 &&
      ((demo_flags & 2U) == 0 || mode_get_main() != MODE_MAIN_ADVERTISE || mode_get_sub() != 8)) {
    t = mode_timer_rate * 0.016666668 * light_anim_speed + t;
    if (end < t + start) {
      if (flags & 1) {
        t = t - (end - start);
      }
      else {
        t = end - start;
      }
    }
    time = t;
  }
  t = t + start;
  n = light_anim_parallel_eval(anim + 2,t,n);
  n = light_anim_point_eval(anim + 5,t,n);
  for (n = light_anim_spot_eval(anim + 8,t,n); n < 0x10; n = (short)(n + 1)) {
    light_anim_lights[n].type = 0;
  }
  return time;
}

int light_anim_parallel_eval(int *group, float time, int first)
{
  short n;
  short j;
  int *curves;
  float *consts;
  int *s;
  float *d;
  float *o;
  ushort *e;
  light_src_rec *dst;
  int v[3];
  short i;

  n = (short)first;
  curves = (int *)group[1];
  consts = (float *)group[2];
  dst = &light_anim_lights[n];
  for (i = 0; i < *(ushort *)*group && n < 0x10; i++, n++, curves += 6, consts += 6, dst++) {
    o = light_key_values;
    s = curves;
    d = consts;
    for (j = 0; j < 6; j++, s++, d++, o++) {
      e = (ushort *)*s;
      if (e != 0) {
        *o = curve_sample(*(float **)(e + 2),*e,time);
      }
      else {
        *o = *d;
      }
    }
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    vec3_sub(light_key_values,(float *)v,light_key_values);
    dst->type = 2;
    dst->dir[0] = light_key_values[0];
    dst->dir[1] = light_key_values[1];
    dst->dir[2] = light_key_values[2];
    dst->color[0] = light_key_values[3];
    dst->color[1] = light_key_values[4];
    dst->color[2] = light_key_values[5];
  }
  return n;
}

int light_anim_point_eval(int *group, float time, int first)
{
  int idx = (short)first;
  int *curves = (int *)group[1];
  float *consts = (float *)group[2];
  light_src_rec *d = &light_anim_lights[(short)idx];
  int cnt = 0;
  float *v;
  float *o;
  int *p;
  float *f;
  short j;
  ushort *q;

  while ((cnt < (int)*(ushort *)*group) & (idx <= 15)) {
    o = light_key_values;
    p = curves;
    f = consts;
    for (j = 0; j < 8; j++, p++, f++, o++) {
      q = (ushort *)*p;
      if (q != 0) *o = curve_sample(*(float **)(q + 2), *q, time);
      else *o = *f;
    }
    v = light_key_values;
    d->type = 3;
    d->pos[0] = v[0];
    d->pos[1] = v[1];
    d->pos[2] = v[2];
    d->color[0] = v[3];
    d->color[1] = v[4];
    d->color[2] = v[5];
    d->dist_min = v[6] * 0.1;
    d->dist_max = v[7] * 0.1;
    cnt = (short)(cnt + 1);
    idx = (short)(idx + 1);
    curves += 8;
    consts += 8;
    d++;
  }
  return idx;
}

int light_anim_spot_eval(int *group, float time, int first)
{
  typedef struct { int w[3]; } vec;
  vec zero;
  int idx = (short)first;
  int *curves = (int *)group[1];
  float *consts = (float *)group[2];
  light_src_rec *d = &light_anim_lights[(short)idx];
  int cnt = 0;
  float *v;
  float *o;
  int *p;
  float *f;
  short j;
  ushort *q;

  while ((cnt < (int)*(ushort *)*group) & (idx <= 15)) {
    v = light_key_spot;
    o = light_key_values;
    p = curves;
    f = consts;
    for (j = 0; j <= 12; j++, p++, f++, o++) {
      q = (ushort *)*p;
      if (q != 0) *o = curve_sample(*(float **)(q + 2), *q, time);
      else *o = *f;
    }
    zero.w[0] = 0;
    zero.w[1] = 0;
    zero.w[2] = 0;
    d->type = 4;
    vec3_sub_from(v, light_key_values);
    vec3_sub(v, v, (float *)&zero);
    d->pos[0] = light_key_values[0];
    d->pos[1] = v[-2];
    d->pos[2] = v[-1];
    d->dir[0] = v[0];
    d->dir[1] = v[1];
    d->dir[2] = v[2];
    d->color[0] = v[3];
    d->color[1] = v[4];
    d->color[2] = v[5];
    d->dist_min = v[6];
    d->dist_max = v[7];
    d->angle_min = (short)(int)(v[8] * 0.5f * 65536.0f / 360.0f + 0.5f);
    d->angle_max = (short)(int)((v[9] + v[8]) * 0.5f * 65536.0f / 360.0f + 0.5f);
    cnt = (short)(cnt + 1);
    idx = (short)(idx + 1);
    curves += 13;
    consts += 13;
    d++;
  }
  return idx;
}

void nyc_light_init(void)
{
  typedef struct { short v; short pad; } tent;
  nyc_heli *e;
  nyc_searchlight *p;
  nyc_searchlight *q;
  nyc_searchlight *r;
  nyc_searchlight *t;
  light_src_rec *d;
  light_src_rec *s;
  int i;
  float v;

  nyc_glow_anim_time = 0.0;
  nyc_spot_anim_time = 0.0;
  light_anim_speed = 1.0;
  p = nyc_searchlights + 1;
  q = &nyc_heli_light0;
  r = &nyc_heli_light1;
  p[0].light.field_0x2c = p[-1].light.field_0x2c = q->light.field_0x2c = r->light.field_0x2c = 0.0;
  p[0].light.color[0] = 1.0;
  p[0].light.color[1] = 1.1;
  p[0].light.color[2] = 1.4;
  p[-1].light.color[0] = 1.0;
  p[-1].light.color[1] = 1.1;
  p[-1].light.color[2] = 1.4;
  q->light.color[0] = r->light.color[0] = 1.2;
  q->light.color[1] = r->light.color[1] = 1.4;
  q->light.color[2] = r->light.color[2] = 1.6;
  p[0].light.dist_min = p[-1].light.dist_min = q->light.dist_min = r->light.dist_min = 100.0;
  p[0].light.dist_max = p[-1].light.dist_max = q->light.dist_max = r->light.dist_max = 100.0;
  p[0].light.pos[0] = 18.0;
  p[0].light.pos[1] = 20.0;
  p[0].light.pos[2] = 10.0;
  p[0].aim_radius_outer = p[-1].aim_radius_outer = 1.3;
  p[0].aim_radius_inner = p[-1].aim_radius_inner = 0.4;
  q->aim_radius_outer = r->aim_radius_outer = 4.0;
  q->aim_radius_inner = r->aim_radius_inner = 0.1;
  p[-1].light.pos[0] = -20.0;
  p[-1].light.pos[1] = 20.0;
  p[-1].light.pos[2] = 9.0;
  p[0].flags = p[-1].flags = 0;
  s = light_anim_lights;
  d = &light_sets[2][1];
  light_anim_play(1.0,2);
  *d = *s;
  s++;
  d++;
  *d = *s;
  s++;
  d++;
  *d = *s;
  e = nyc_helis;
  light_mot_paths_bind();
  for (i = 0; i < 4; i++, e++) {
    switch (i) {
    case 0:
    case 2:
      e->id = 0;
      break;
    default:
      e->id = 1;
      break;
    }
    e->tmax = ((tent *)&light_mot_pairs[0][1])[light_mot_choice * 2 + e->id].v;
    e->trst = 300.0;
    switch (i) {
    case 0:
      e->t0 = e->t = 1.0;
      break;
    case 1:
      e->t0 = e->t = e->tmax - e->trst;
      break;
    case 2:
      switch (light_mot_choice) {
      case 0:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 - 60.0;
        break;
      case 1:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 - 240.0;
        break;
      case 2:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 + 60.0;
        break;
      case 3:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 - 150.0;
        break;
      default:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5;
        break;
      }
      break;
    case 3:
      switch (light_mot_choice) {
      case 0:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 + 120.0;
        break;
      case 1:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 - 390.0;
        break;
      case 2:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 - 60.0;
        break;
      case 3:
      default:
        e->t0 = e->t = (e->tmax - e->trst) * 0.5 + 250.0;
        break;
      }
      break;
    default:
      e->t0 = e->t = (e->tmax - e->trst) * 0.5;
      break;
    }
    e->rotor0 = e->rotor1 = e->field_0x40 = 0;
    e->rotor_t = 0.0;
    e->rotor_tmax = 90.0;
  }
  t = nyc_searchlights_fixed;
  t->pos.x = 0.0;
  t->pos.y = -6.0;
  t->pos.z = -11.0;
  t->model = 1;
  t++;
  t->pos.x = 0.0;
  t->pos.y = -6.0;
  t->pos.z = 11.0;
  t->model = 1;
  t = nyc_searchlights;
  t->pitch = 0x2000;
  t->yaw = 0x8000;
  t->roll = 0;
  t->model = 0;
  t++;
  t->pitch = 0x2000;
  t->yaw = 0;
  t->roll = 0;
  t->model = 0;
  nyc_light_flags = 0;
  if (mode_get_main() == MODE_MAIN_ADVERTISE) {
    nyc_light_flags = nyc_light_flags | 1;
  }
}

void nyc_heli_update(void)
{
  typedef struct { int w[3]; } v3;
  nyc_heli *e;
  int i;
  float t;
  bone_work_rec *s;
  float *up;
  float f;
  float g;
  v3 *c;
  short *a;
  short *b;
  mot_path_slot *ms;

  e = nyc_helis;
  for (i = 0; i < 4; i++, e++) {
    t = e->t;
    ms = &mot_path_slots[e->id];
    s = ms->bone;
    c = (v3 *)s->pos;
    light_mot_path_eval(e->id,t);
    *(v3 *)&e->pos = *c;
    if (i == 3) e->pos.y = e->pos.y + 10.0;
    e->rot.x = s->rot[0];
    e->rot.y = s->rot[1];
    e->rot.z = s->rot[2];
    t = t + mode_timer_rate;
    if (t >= e->tmax) e->t = t - e->tmax + e->trst;
    else e->t = t;
    up = &e->rotor_t;
    g = *up;
    f = mode_timer_rate + g;
    *up = f;
    if (e->rotor_tmax < f) *up = f - e->rotor_tmax;
    a = &e->rotor0;
    *a = *a + -0x20b6;
    b = &e->rotor1;
    *b = *b + -0x216c;
    if (game_frame_count % 3 != 0) {
      *a = *a + -0x2000;
      *b = *b + -0x2000;
    }
  }
}

void nyc_searchlights_update(void)
{
  typedef struct { int w[3]; } pos_blk;
  light_src_rec l[2];
  light_src_rec s1[2];
  light_src_rec p[2][2];
  char q[2][16];
  rob_rec *rob0;
  rob_rec *rob1;
  light_src_rec *d;
  light_src_rec *d1;
  nyc_searchlight *w;
  nyc_searchlight *w0;
  nyc_searchlight *w1;
  int i;
  float x, y, z, t;
  light_src_rec *v;
  float *f;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if (light_anim_sw != ON_OFF_ON) return;
  nyc_glow_anim_time = light_anim_play(nyc_glow_anim_time,3);
  nyc_beam_glows[0] = nyc_beam_glows[1] = light_anim_lights[0];
  d = nyc_beam_glows;
  d1 = d + 1;
  if (nyc_glow_anim_time > 1.6166668) {
    d[0].pos[0] = -5.9;
    d[0].pos[1] = 5.7;
    d[0].pos[2] = -3.9;
    d1[0].pos[0] = 5.9;
    d1[0].pos[1] = 5.7;
    d1[0].pos[2] = 3.9;
  }
  else {
    d[0].pos[0] = 5.9;
    d[0].pos[1] = 5.7;
    d[0].pos[2] = -3.9;
    d1[0].pos[0] = -5.9;
    d1[0].pos[1] = 5.7;
    d1[0].pos[2] = 3.9;
  }
  nyc_spot_anim_time = light_anim_play(nyc_spot_anim_time,4);
  l[0] = light_anim_lights[0];
  l[1] = light_anim_lights[1];
  w0 = nyc_searchlights;
  if ((rob0->flags & 1) == 0 && (rob1->flags & 1) != 0) {
    s1[0].type = p[0][0].type = p[0][1].type = 0;
    *(pos_blk *)&w0[0].aim = *(pos_blk *)&stage_cur->center.x;
  }
  else if ((rob0->flags & 1) != 0) {
    *(pos_blk *)&w0[0].aim = *(pos_blk *)&rob0->pos;
  }
  else {
    *(pos_blk *)&w0[0].aim = *(pos_blk *)&stage_cur->center.x;
  }
  nyc_searchlight_spot_build(&w0[0],&s1[0]);
  nyc_searchlight_spot_pair_build(&w0[0],&p[0][0],&p[0][1],(float *)q[0],rob0);
  if ((w0[0].flags & 1) == 0) w0[0].light.type = 0;
  w1 = nyc_searchlights + 1;
  v = p[1]; /* WORKAROUND (kept as possibly original; no game instruction of its own): v is overwritten by both loops below before it is read. Compiled without this line the game loses no instruction; only r3 and r7 swap in one block. Kept because it may be original code whose reader was compiled out of this build: this function ends in an empty loop the game has, a sign of compiled-out debug code. */
  if ((rob1->flags & 1) == 0 && (rob0->flags & 1) != 0) {
    s1[1].type = p[1][0].type = p[1][1].type = 0;
    *(pos_blk *)&w1[0].aim = *(pos_blk *)&stage_cur->center.x;
  }
  else if ((rob1->flags & 1) != 0) {
    *(pos_blk *)&w1[0].aim = *(pos_blk *)&rob1->pos;
  }
  else {
    *(pos_blk *)&w1[0].aim = *(pos_blk *)&stage_cur->center.x;
  }
  nyc_searchlight_spot_build(&w1[0],&s1[1]);
  nyc_searchlight_spot_pair_build(&w1[0],&p[1][0],&p[1][1],(float *)q[1],rob1);
  if ((w1[0].flags & 1) == 0) w1[0].light.type = 0;
  for (i = 0; i < 2; i++) {
    w = nyc_searchlights_fixed + i;
    v = &l[i];
    x = v->dir[0];
    y = v->dir[1];
    z = v->dir[2];
    w->yaw = atan2_s16(x,z);
    t = fsqrt_asm(x * x + z * z);
    w->pitch = atan2_s16(y,t);
  }
  for (i = 0; i < 2; i++) {
    w = nyc_searchlights + i;
    v = &s1[i];
    x = v->dir[0];
    y = v->dir[1];
    z = v->dir[2];
    w->yaw = atan2_s16(x,z);
    t = fsqrt_asm(x * x + z * z);
    w->pitch = atan2_s16(y,t);
  }
  for (i = 0; i < 2; i++) { /* empty, as in the game: an empty counting loop at 0x0C0B24BE (mov #0 / add #1 / cmp/gt / bf), most likely debug code compiled out of this build. */
  }
}

void nyc_light_update(void)
{

  if ((mode_get_main() == MODE_MAIN_GAME) && (mode_get_sub() == 8)) {
    nyc_light_flags = nyc_light_flags | 1;
    nyc_searchlights[0].flags |= 1;
    nyc_searchlights[1].flags |= 1;
  }
  if ((nyc_light_flags & 1) != 0) {
    nyc_heli_update();
  }
  return;
}

void nyc_heli_draw(nyc_heli *heli, int light)
{
  float v[3];
  
  v[0] = 0.0f;
  v[1] = 2.35f;
  v[2] = 0.25f;
  mtx_push(0);
  mtx_translate_by_vec3(v);
  mtx_rot_y_s16(0x8000);
  obj_draw(0x1c0070);
  mtx_peek_pushed(0);
  mtx_translate_by_vec3(v);
  mtx_rot_y_s16(heli->rotor0);
  obj_draw(0x1c0071);
  mtx_peek_pushed(0);
  v[0] = 0.0f;
  v[1] = 2.77f;
  v[2] = 14.3f;
  mtx_translate_by_vec3(v);
  mtx_rot_x_s16(heli->rotor1);
  obj_draw(0x1c0075);
  mtx_pop(1);
  return;
}

void nyc_searchlight_draw(nyc_searchlight *light)
{
  float v[3];
  
  mtx_push(0);
  mtx_translate_by_vec3(&light->pos.x);
  mtx_rot_y_s16(light->yaw);
  mtx_rot_z_s16(0x8000);
  v[0] = 0.0;
  v[1] = -0.9;
  v[2] = 0.0;
  mtx_translate_by_vec3(v);
  mtx_rot_x_s16(light->pitch);
  mtx_rot_z_s16((int)light->roll);
  if (light->model == '\0') {
    obj_draw(0x1c006d);
  }
  mtx_pop(1);
  return;
}

void nyc_light_draw(void)
{
  typedef struct { float v[3]; } vec3_t;
  nyc_heli *e;
  light_src_rec *d;
  nyc_searchlight *q;
  float v1[3];
  vec3_t v2;
  float x;
  float y;
  float z;
  float s;
  float o1;
  float o2;
  int i;
  int rx;
  int ry;
  int m;

  nyc_searchlights_update();
  mtx_push(0);
  camera_view_load();
  light_set_send(0,0);
  if ((nyc_light_flags & 1) != 0) {
    e = nyc_helis;
    v1[0] = 0.0;
    v1[1] = -2.55;
    v1[2] = 7.65;
    mtx_push(0);
    for (i = 0; i < 4; i++, e++) {
      float *mp = nyc_heli_mtx;
      mtx_load_identity();
      v2 = *(vec3_t *)&e->dir;
      mtx_translate_by_vec3(&e->pos.x);
      mtx_rot_zyx_s16((int)e->rot.x,(int)e->rot.y,(int)e->rot.z);
      mtx_rot_y_s16(0x8000);
      mtx_store_wordwise(mp);
      mtx_rot_y_s16(0x8000);
      mtx_translate_by_vec3(v1);
      vec_to_local(v2.v);
      x = v2.v[0];
      y = v2.v[1];
      z = v2.v[2];
      e->rz = atan2_s16(x,z);
      z = fsqrt_asm(x * x + z * z);
      e->ry = atan2_s16(y,z);
      camera_view_load();
      mtx_mul_by((float *)mp);
      nyc_heli_draw(e,(int)&nyc_heli_light0 + i * 0x70);
    }
    mtx_pop(1);
  }
  obj_draw(0xd0000);
  obj_draw(0xd0001);
  obj_draw(0xd0002);
  obj_draw(0xd0003);
  obj_draw(0xd0004);
  rx = camera.pitch;
  ry = camera.yaw;
  d = nyc_beam_glows;
  s = d->color[0];
  mtx_push(0);
  { int t = game_frame_count % 4; m = t > 1; }
  m += 0x1c0069;
  if (d->pos[0] > 0.0) o1 = 0.1;
  else o1 = -0.1;
  if (d->pos[2] > 0.0) o2 = 0.1;
  else o2 = -0.1;
  mtx_translate(o1 + d->pos[0],d->pos[1],o2 + d->pos[2]);
  mtx_rot_y_s16(ry);
  mtx_rot_x_s16(rx);
  mtx_scale_xyz(s,s,s);
  obj_draw(m);
  d++;
  mtx_peek_pushed(0);
  if (d->pos[0] > 0.0) o1 = 0.1;
  else o1 = -0.1;
  if (d->pos[2] > 0.0) o2 = 0.1;
  else o2 = -0.1;
  mtx_translate(o1 + d->pos[0],d->pos[1],o2 + d->pos[2]);
  mtx_rot_y_s16(ry);
  mtx_rot_x_s16(rx);
  mtx_scale_xyz(s,s,s);
  obj_draw(m);
  mtx_pop(1);
  q = nyc_searchlights;
  q->pos.x = q->light.pos[0];
  q->pos.y = q->light.pos[1] - 0.9;
  q->pos.z = q->light.pos[2];
  nyc_searchlights[1].pos.x = nyc_searchlights[1].light.pos[0];
  nyc_searchlights[1].pos.y = nyc_searchlights[1].light.pos[1] - 0.9;
  nyc_searchlights[1].pos.z = nyc_searchlights[1].light.pos[2];
  nyc_searchlight_draw(&nyc_searchlights[0]);
  nyc_searchlight_draw(&nyc_searchlights[1]);
  if ((nyc_fixed_lights_on != '\0') || ((mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 5)))) {
    nyc_searchlight_draw(&nyc_searchlights_fixed[0]);
    nyc_searchlight_draw(&nyc_searchlights_fixed[1]);
  }
  mtx_pop(1);
}

void nyc_searchlight_spot_build(nyc_searchlight *light, light_src_rec *spot)
{
  light_src_rec *s = &light->light;
  float *v = light->light.pos;
  float d;
  d = vec3_distance(&light->aim.x,v);
  s->angle_min = atan2_s16(light->aim_radius_inner,d);
  s->angle_max = atan2_s16(light->aim_radius_outer,d);
  s->type = 4;
  if (nyc_beam_debug_lines_on != '\0') {
    line3d_width_store(2.0);
    line3d_quads_opaque(v,light,-1);
  }
  vec3_sub(s->dir,&light->aim.x,s->pos);
  *spot = *s;
  light->aim_dist = d;
}

void nyc_searchlight_spot_pair_build(nyc_searchlight *light,light_src_rec *spot_a,light_src_rec *spot_b,float *color,rob_rec *rob)
{
  float *p = (float *)light;
  float pos[3];
  float sc[4];
  float *q;
  float *c;
  float *row;
  float *pa;
  float *pb;
  short d;
  short s;
  short m = -1; /* a variable, not the literal: the game multiplies by a register holding -1 (muls.w), where -s compiles to neg. */
  float f;
  float ff;
  float a2;
  float dx;
  float dy;
  float dz;
  float dist;
  int off;
  int yaw;
  int t;
  int i;
  float k;
  float max;

  q = p + 10;
  c = p + 8;
  yaw = atan2_s16(q[0] - p[0],c[4] - p[2]);
  d = yaw - camera.yaw;
  if (d < 0) d = -d;
  f = (float)d * 360.0 * 1.5258789e-05 / 180.0;
  off = (short)(int)(f * 75.0 * 65536.0 / 360.0 + 0.5);
  s = rob->body_pitch;
  dx = q[0] - p[0];
  dz = q[2] - p[2];
  yaw = atan2_s16(dx,dz);
  dist = fsqrt_asm(dx * dx + dz * dz);
  dy = q[1] - p[1];
  t = atan2_s16(dy,dist);
  dist = fsqrt_asm(dy * dy + dist * dist);
  if (s < 0) s *= m;
  if (s > 0x4000) s = 0x4000;
  a2 = (float)s * 360.0 * 1.5258789e-05 / 90.0;
  t = (short)(int)((float)t * (1.0 - a2 * f));
  *spot_b = *(light_src_rec *)c;
  *spot_a = *spot_b;
  vec3_sub(pos,q,p);
  mtx_push_identity();
  mtx_translate_by_vec3(p);
  mtx_push(0);
  mtx_rot_y_s16(off + yaw);
  mtx_rot_x_s16(-t);
  mtx_translate(0.0,0.0,dist);
  pa = spot_a->pos;
  mtx_store_translation(pa);
  vec3_sub(spot_a->dir,p,pa);
  mtx_peek_pushed(0);
  mtx_rot_y_s16(yaw - off);
  mtx_rot_x_s16(-t);
  mtx_translate(0.0,0.0,dist);
  pb = spot_b->pos;
  mtx_store_translation(pb);
  vec3_sub(spot_b->dir,p,pb);
  mtx_pop(2);
  if (nyc_beam_debug_lines_on != 0) {
    line3d_width_store(2.0);
    line3d_quads_opaque((uint)pa,(uint)p,0xffff0000);
    line3d_quads_opaque((uint)pb,(uint)p,0xff00ff00);
  }
  ff = f * f;
  row = stage_light_table[rob_light_num >= 0 ? rob_light_num : stg_light_num].ambient;
  *color++ = 0.0;
  k = (1.0 - ff) * 0.5 + 0.5;
  for (i = 1; i < 4; i++) {
    *color++ = row[i - 1] * k;
  }
  max = 0.0;
  for (i = 1; i < 4; i++) {
    sc[i] = spot_a->color[i - 1] * 0.5;
    if (sc[i] > max) max = sc[i];
  }
  k = 2.0 - max;
  for (i = 1; i < 4; i++) {
    spot_b->color[i - 1] = spot_a->color[i - 1] = k * ff + sc[i];
  }
}

void are_light_init(void)
{
  stage_light_anim_time = 0.0;
  light_anim_speed = 1.0;
  return;
}

void are_light_update(void)
{
  float time;
  uchar i;
  
  light_anim_speed = (float)(int)audience_mood / 25.0;
  time = light_anim_play(stage_light_anim_time,9);
  if ((mode_get_main() != MODE_MAIN_ADVERTISE) || (mode_get_sub() != 8)) {
    stage_light_anim_time = time;
  }
  for (i = 0; i < 0xb; i++) {
    char *p = (char *)&light_sets + i * 0x44;
    *(light_src_rec *)(p + 0x154) = light_anim_lights[i];
    *(float *)(p + 400) *= 0.1;
    *(float *)(p + 0x194) *= 0.1;
  }
}

void are_light_draw(void)
{
  light_src_rec *e;
  uchar i;
  float x, y, z;
  int ry, rx;
  mtx_push(0);
  for (i = 5; i < 8; i++) {
    e = &light_sets[0][i];
    x = e->dir[0];
    y = e->dir[1];
    z = e->dir[2];
    ry = atan2_s16(x, z);
    rx = atan2_s16(y, fsqrt_asm(x * x + z * z));
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(e->pos);
    mtx_rot_y_s16(ry);
    mtx_rot_x_s16(-rx);
    obj_draw(0x210082);
    mtx_scale_xyz(1.5,1.5,0.8);
    obj_draw(0x210084);
  }
  mtx_pop(1);
}

void nyc_heli_pos_get(float *pos, int heli_no)
{
  typedef struct { int w[3]; } blk;
  
  nyc_heli *e = &nyc_helis[heli_no];
  *(blk *)pos = *(blk *)&e->pos;
  return;
}

void rome_fence_init(void)
{
  short buf[2];
  short *s;
  stage_anim_rec *p;
  int i;
  break_piece **pp;
  break_piece *q;
  memcpy(buf, rome_fence_anim_ids, 4);
  s = buf;
  p = stage_anim_objs;
  for (i = 0; i < 2; i++, p++) {
    p->flags = 8;
    p->frame_time = 1.0;
    p->anim_id = *s++;
    stage_anim_obj_bind(p);
  }
  oneshot_anim_pool_alloc();
  for (pp = rome_fence_rows; *pp != 0; pp++) {
    for (q = *pp; q->model != -1; q++) {
      q->flags = 0;
    }
  }
}

void rome_fence_update(void)
{
  fence_segment_break_update(rome_fence_row0,rome_fence_row1);
  stage_anim_objs_step(0,2);
  return;
}

void rome_fence_draw(void)
{
  mtx_push(0);
  camera_view_load();
  mtx_translate(0.0,stage_cur->center.y,0.0);
  fence_rows_draw(rome_fence_rows);
  fence_break_pieces_draw(rome_fence_piece_lists);
  oneshot_anim_draw_all();
  mtx_pop(1);
  return;
}

void fence_rows_draw(break_piece **rows)
{
  break_piece *a;
  break_piece *b;
  break_piece *p;
  break_piece *q;
  int has;
  int show;
  int i;
  float v[3];
  float d;
  
  a = *rows++;
  b = 0;
  has = 0;
  if (*rows != 0) {
    b = *rows;
    has = 1;
  }
  mtx_push(0);
  p = a;
  q = 0;
  if (has) q = b;
  if (mode_get_main() == 1 && mode_get_sub() == 8) show = 0;
  else show = 1;
  if ((stage_cur_no == 0) | (stage_cur_no == 4)) {
    if (show) {
      d = 0.3;
      v[0] = light_set_ambient[0].color[0] + d;
      v[1] = light_set_ambient[0].color[1] + d;
      v[2] = light_set_ambient[0].color[2] + d;
      light_set_masks_send(v,0);
    }
  }
  extra_light_spot_set(show ? 1.0 : 0.0);
  for (i = 0; i < 24; i++) {
    if (p->model < 0) break;
    if ((p->flags & 0x10) == 0) {
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(&p->pos.x);
      mtx_rot_y_s16(p->turn);
      obj_draw_translucent(p->model);
    }
    if (has && (q->flags & 0x10) == 0) {
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(&q->pos.x);
      mtx_rot_y_s16(q->turn);
      obj_draw_translucent(q->model);
      if (p == a) {
        if (a[23].flags & 0x10) {
          if (q->turn2 != 0) mtx_rot_y_s16(q->turn2);
          obj_draw_translucent(q->model2);
        }
      }
      else {
        if (p[-1].flags & 0x10) {
          if (q->turn2 != 0) mtx_rot_y_s16(q->turn2);
          obj_draw_translucent(q->model2);
        }
      }
      if (p->flags & 0x10) {
        mtx_rot_y_s16(0x8000);
        obj_draw_translucent(q->model2);
      }
    }
    p++;
    if (has) q++;
  }
  extra_light_off();
  if ((stage_cur_no == 0) | (stage_cur_no == 4)) {
    if (show) {
      d = 0.3;
      v[0] = light_set_ambient[0].color[0] - d;
      v[1] = light_set_ambient[0].color[1] - d;
      v[2] = light_set_ambient[0].color[2] - d;
      light_set_masks_send(v,0);
    }
  }
  mtx_pop(1);
}

void fence_break_pieces_draw(int **piece_lists)
{
  stage_anim_rec *e;
  int *list;
  typedef struct { float f[16]; } mat;
  mat *m;
  short s;
  short *p;
  int i;

  e = stage_anim_objs;
  mtx_push(0);
  for (i = 0; i < 1; i++, e++) {
    if (e->anim_id == 0x166f) {
      anim_skip_list = anim_skip_nodes_166f;
    }
    else if (e->anim_id == 0x1671) {
      anim_skip_list = anim_skip_nodes_1671;
    }
    else {
      anim_skip_list = 0;
    }
    if ((e->flags & 2) != 0) {
      m = (mat *)e->mtx;
      list = piece_lists[i];
      if (e->anim_id != 0x166f || (float)e->anim_frames > e->frame_time) {
        mtx_push(0);
        mtx_translate_by_vec3(e->pos);
        mtx_rot_y_s16(e->turn);
        mtx_push(0);
        m = m + 1;
        if (anim_skip_list == 0) {
          while (*list != -1) {
            mtx_peek_pushed(0);
            mtx_mul_by((float *)m++);
            obj_draw(*list++);
          }
        }
        else {
          p = anim_skip_list;
          s = 0;
          while (*list != -1) {
            if (*p == s) {
              if (s > 0) p++;
              m++;
              list++;
            }
            else if (*list == 0x18022e) {
              m++;
              list++;
            }
            else {
              mtx_peek_pushed(0);
              mtx_mul_by((float *)m++);
              obj_draw(*list++);
            }
            s++;
          }
        }
        mtx_pop(2);
      }
    }
  }
  mtx_pop(1);
}

void yam_fence_init(void)
{
  break_piece *p;
  break_piece **pp;
  yam_break_objs_init();
  for (pp = yam_fence_rows; *pp != 0; pp++) {
    for (p = *pp; p->model != -1; p++) {
      p->flags = 0;
    }
  }
}

void yam_fence_update(void)
{
  fence_segment_break_update(yam_fence_row0,yam_fence_row1);
  stage_anim_objs_step(0,2);
  return;
}

void fence_segment_break_update(break_piece *row, break_piece *row2)
{
  int i;
  break_piece *e;
  int *p;
  int prev;
  int next;
  int has;
  break_piece *q;
  break_piece *n;

  e = row;
  has = 0;
  if (row2 != 0) has = 1;
  fence_break_segment = 0;
  for (i = 0; i < 0x18; i++, e++) {
    if (i == fence_hit_segment && (stage_break_flags & 8) != 0) {
      p = e->anims;
      if ((e->flags & 0x10) == 0) {
        if (p != 0) {
          while (*p != 0) {
            oneshot_anim_spawn((uint *)*p,(uint *)e,e->turn);
            p++;
          }
        }
        fence_break_obj_start(e);
        fence_break_segment = i + 1;
        e->flags = 0x10;
        if (has) {
          prev = i - 1;
          next = i + 1;
          if (i == 0) prev = 0x17;
          else if (i > 0x16) next = 0;
          n = row + prev;
          if ((n->flags & 0x10) != 0) {
            q = row2 + i;
            fence_break_obj_start(q);
            q->flags = 0x10;
          }
          n = row + next;
          if ((n->flags & 0x10) != 0) {
            q = row2 + next;
            fence_break_obj_start(q);
            q->flags = 0x10;
          }
        }
      }
      e->flags &= ~8;
      stage_break_flags &= ~8;
    }
  }
}

void yam_fence_draw(void)
{
  mtx_push(0);
  camera_view_load();
  mtx_translate(0.0,stage_cur->center.y,0.0);
  fence_rows_draw(yam_fence_rows);
  fence_break_pieces_draw(yam_fence_piece_lists);
  oneshot_anim_draw_all();
  mtx_pop(1);
  return;
}

void fence_break_obj_start(break_piece *seg)
{
  short k;
  stage_anim_rec *e;
  uint *v;

  k = 0x30;
  e = (stage_anim_rec *)((char *)stage_anim_objs + seg->idx * k);
  if (seg->idx >= 0) {
    v = (uint *)e->pos;
    v[0] = *(uint *)&seg->pos.x;
    v[1] = *(uint *)&seg->pos.y;
    v[2] = *(uint *)&seg->pos.z;
    e->frame_time = 0.0;
    e->flags = 3;
    e->turn = seg->turn;
  }
}

void fence_segment_unbreak(int segment_no)
{
  char i = segment_no - 1;
  break_piece *p;
  djo_fence_fall_rec *e;
  switch ((int)stage_cur_no) {
  case 0: p = rome_fence_row0; break;
  case 4: p = yam_fence_row0; break;
  case 0x10: p = djo_fence_segs; break;
  default: return;
  }
  p += i;
  p->flags = 0;
  if (stage_cur_no == 0x10) {
    e = djo_fence_fall;
    e += i;
    e->flags = 0;
    e->time = 1.0;
  }
}

void fence_row2_unbreak_sync(void)
{
  break_piece *p;
  break_piece *q;
  break_piece *o;
  short i;
  int a;
  int b;
  switch (stage_cur_no) {
  case 0:
    p = rome_fence_row0;
    q = p;
    o = rome_fence_row1;
    break;
  case 4:
    p = yam_fence_row0;
    q = p;
    o = yam_fence_row1;
    break;
  default:
    return;
  }
  for (i = 0; i < 24; i++, p++, o++) {
    a = 0;
    b = 0;
    if (i == 0) {
      if (p[23].flags & 0x10) a = 1;
    }
    else {
      if (q->flags & 0x10) a = 1;
      q++;
    }
    if (p->flags & 0x10) b = 1;
    a &= b;
    if (!a) o->flags = 0;
  }
}

void fence_segment_break_set(int segment_no)
{
  char i = segment_no - 1;
  break_piece *p;
  switch ((int)stage_cur_no) {
  case 0: p = rome_fence_row0; break;
  case 4: p = yam_fence_row0; break;
  case 0x10: p = djo_fence_segs; break;
  default: return;
  }
  p += i; p->flags = 0x10;
  if (stage_cur_no == 0x10) {
    (djo_fence_fall + i)->time = 1000.0; /* ORIGINAL SHAPE (corroborated 2026-11-10: the line p += i; p->flags = 0x10; above in this function is forced the same way, and regclass shows why: only a pointer step marks the register as a pointer): the record by pointer step; djo_fence_fall[i].time swaps two registers */
  }
}

void fence_row2_break_sync(void)
{
  break_piece *a;
  break_piece *b;
  break_piece *d;
  short i;
  int f1;
  int f2;
  
  switch (stage_cur_no) {
  case 0:
    a = rome_fence_row0;
    b = a;
    d = rome_fence_row1;
    break;
  case 4:
    a = yam_fence_row0;
    b = a;
    d = yam_fence_row1;
    break;
  default:
    return;
  }
  for (i = 0; i < 0x18; i++, a++, d++) {
    f1 = 0;
    f2 = 0;
    if (i == 0) {
      if ((a[23].flags & 0x10) != 0) f1 = 1;
    }
    else {
      if ((b->flags & 0x10) != 0) f1 = 1;
      b++;
    }
    if ((a->flags & 0x10) != 0) f2 = 1;
    if (f1 & f2) d->flags = 0x10;
  }
  return;
}

void djo_fence_init(void)
{
  break_piece *p;
  djo_fence_fall_rec *q;
  mot_paths_block = mot_paths_bind(0,3,&djo_fence_mot_list[0][0]);
  p = djo_fence_segs;
  q = djo_fence_fall;
  for (; p->model != -1; p++, q++) {
    p->flags = 0;
    q->flags = 0;
    q->time = 0.0f;
  }
}

void djo_fence_update(void)
{
  typedef struct { float v[3]; } vec3blk;
  bone_work_rec *s;
  break_piece *p;
  djo_fence_fall_rec *q;
  mot_path_slot *t;
  int b;
  p = djo_fence_segs;
  q = djo_fence_fall;
  djo_fence_break_check(p);
  for (; p->model != -1; p++, q++) {
    if ((q->flags & 1) != 0 && (q->flags & 2) == 0) {
      t = &mot_path_slots[p->idx];
      mot_path_eval(t,q->time);
      s = t->bone;
      *(vec3blk *)&q->pos = *(vec3blk *)s->pos;
      q->rot[0] = s->rot[0];
      q->rot[1] = s->rot[1];
      q->rot[2] = s->rot[2];
      if (pause_frame_held() == 0) {
        b = (int)&djo_fence_mot_list[0][1];
        q->time = q->time + 1.0;
        if ((float)*(short *)(b + p->idx * 4) < q->time) {
          q->flags = q->flags | 2;
        }
      }
    }
  }
}

void djo_fence_break_check(break_piece *seg)
{
  int i;

  fence_break_segment = '\0';
  i = 0;
  do {
    if ((i == fence_hit_segment) && ((stage_break_flags & 8) != 0)) {
      if ((seg->flags & 0x10) == 0) {
        djo_fence_fall_start(seg,i);
        fence_break_segment = (char)i + '\x01';
        seg->flags = 0x10;
      }
      seg->flags = seg->flags & 0xf7;
      stage_break_flags = stage_break_flags & 0xf7;
    }
    i = i + 1;
    seg = seg + 1;
  } while (i < 0x18);
  return;
}

void djo_fence_fall_start(break_piece *seg, int index)
{
  djo_fence_fall_rec *p = djo_fence_fall + index;
  if (-1 < seg->idx) {
    p->time = 1.0;
    p->flags = p->flags | 1;
  }
}

void djo_fence_segments_draw(break_piece *seg, djo_fence_fall_rec *fall)
{
  float v[3];
  float f;

  mtx_push(0);
  {
    float d = 0.2;
    v[0] = light_set_ambient[0].color[0] + d;
    v[1] = light_set_ambient[0].color[1] + d;
    v[2] = light_set_ambient[0].color[2] + d;
    light_set_send(v,0);
  }
  if (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) {
    f = 0.0f;
  }
  else {
    f = 1.0f;
  }
  extra_light_spot_set(f);
  while (seg->model != -1) {
    mtx_peek_pushed(0);
    if ((seg->flags & 0x10) != 0) {
      mtx_translate(seg->pos.x,0.0f,seg->pos.z);
      mtx_rot_y_s16(seg->turn);
      mtx_translate_by_vec3(&fall->pos.x);
      mtx_rot_zyx_s16(fall->rot[0],fall->rot[1],fall->rot[2]);
    }
    else {
      mtx_translate_by_vec3(&seg->pos.x);
      mtx_rot_y_s16(seg->turn);
    }
    obj_draw_translucent(seg->model);
    seg = seg + 1;
    fall = fall + 1;
  }
  extra_light_off();
  {
    float d = 0.2;
    v[0] = light_set_ambient[0].color[0] - d;
    v[1] = light_set_ambient[0].color[1] - d;
    v[2] = light_set_ambient[0].color[2] - d;
    light_set_send(v,0);
  }
  mtx_pop(1);
}

void djo_fence_draw(void)
{
  mtx_push(0);
  camera_view_load();
  mtx_translate(0.0,stage_cur->center.y,0.0);
  djo_fence_segments_draw(djo_fence_segs,djo_fence_fall);
  mtx_pop(1);
  return;
}

void empty_fn_0c0b4160(void)
{
  return;
}

void empty_fn_0c0b4180(void)
{
  return;
}

void empty_fn_0c0b41a0(void)
{
  return;
}

void empty_fn_0c0b41c0(void)
{
  return;
}

void empty_fn_0c0b41e0(void)
{
  return;
}

void empty_fn_0c0b4200(void)
{
  return;
}

void empty_fn_0c0b4220(void)
{
  return;
}

uint effect_rand15(void)
{
  uint m;
  m = 0x41c64e6d;
  effect_rand_seed = (m = effect_rand_seed * m + 0x3039);
  m = (m >> 16) & 0x7fff;
  return m;
}

void model_clump_frames_build(uint x_range, uint y_range, int count, int *frame_models, int first_model, short frames)
{
  short sel[8];
  float pos[24];
  model_xform_pair list[8];
  float mtx[128];
  int i;
  int j;
  int total;
  int dest;

  mtx_push(0);
  for (i = 0; i < count; i++) {
    sel[i] = (int)effect_rand15() % frames;
    pos[i * 3] = (float)((short)((int)effect_rand15() % (int)x_range) - (int)x_range / 2) * 0.1;
    pos[i * 3 + 1] = (float)((short)((int)effect_rand15() % (int)y_range) - (int)y_range / 2) * 0.1;
    pos[i * 3 + 2] = (float)((short)((int)effect_rand15() % (int)x_range) - (int)x_range / 2) * 0.1;
  }
  total = 0;
  for (i = 0; i < frames; i++) {
    for (j = 0; j < count; j++) {
      int id = first_model + sel[j];
      list[j].model = obj_model_get(id);
      total += model_size((int *)list[j].model) + 31 & ~31;
      if (++sel[j] >= frames) {
        sel[j] = 0;
      }
    }
  }
  dest = (int)elan_heap_alloc(total);
  for (i = 0; i < frames; i++) {
    total = 0;
    for (j = 0; j < count; j++) {
      mtx_load_identity();
      mtx_translate_by_vec3(&pos[j * 3]);
      mtx_store_wordwise(mtx + j * 16);
      list[j].mtx = mtx + j * 16;
      list[j].model = obj_model_get(first_model + sel[j]);
      total += model_size((int *)list[j].model) + 31 & ~31;
      if (++sel[j] >= frames) {
        sel[j] = 0;
      }
    }
    models_merge_xformed_into(dest,count,list);
    frame_models[i] = dest;
    dest += total;
  }
  mtx_pop(1);
}

void model_clump_frames_free(int *frame_models, int count)
{
  int model;
  int i;
  short n = count;
  int *p = frame_models;
  int a, b;
  
  for (i = 0; i < n; i++) {
    model = p[i];
    a = model != 0;
    b = model != -1;
    if (a & b) {
      elan_heap_free((void *)model);
      return;
    }
  }
}

void loop64_model_init(void)
{
  loop64_model.x = 0.0;
  loop64_model.y = 0.0;
  loop64_model.z = 0.0;
  loop64_model.frame = -1;
  return;
}

void loop64_model_step(void)
{
  if (++loop64_model.frame == 0x40) {
    loop64_model.frame = 0;
  }
}

void loop64_model_draw(void)
{
  short *p = &loop64_model.frame;
  e_mode_main main_mode;
  int sub_mode;
  
  main_mode = mode_get_main();
  if ((main_mode != MODE_MAIN_ADVERTISE) || (sub_mode = mode_get_sub(), sub_mode != 8)) {
    mtx_push(0);
    mtx_translate_by_vec3((float *)(p + 2));
    obj_draw(*p + 0x180264);
    mtx_pop(1);
  }
  return;
}

void aquarium_bubble_init(void)
{
  aquarium_bubble *p;
  int i;
  
  aquarium_bubbles = (aquarium_bubble *)effect_malloc(1000);
  if (aquarium_bubbles == (aquarium_bubble *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  p = aquarium_bubbles;
  for (i = 0; i < 0x32; i++, p++) {
    p->flags = 0;
    p->frames = 0x20;
  }
  return;
}

void aquarium_bubble_update(void)
{
  typedef struct { float x; float y; float z; } vec_f3;
  aquarium_bubble *p;
  int i;
  int x;
  int z;
  vec_f3 v;

  p = aquarium_bubbles;
  for (i = 0; i < 0x32; i++, p++) {
    if ((p->flags & 1) != 0) {
      p->pos.y = p->pos.y + aquarium_bubble_rise;
      p->turn = p->turn + aquarium_bubble_spin;
      if (++p->frame == p->frames) {
        p->frame = 0;
      }
      if (p->pos.y >= 16.0) {
        p->flags = 0;
      }
    }
  }
  if ((int)effect_rand15() % aquarium_bubble_rate == 0) {
    p = aquarium_bubbles;
    for (i = 0; i < 0x32; i++, p++) {
      if ((p->flags & 1) == 0) {
        if ((int)lcg_next15() % 0x164 < 0xc4) {
          v.x = (float)((int)lcg_next15() % 0xf5 - 0x37) * 0.1;
          v.z = (float)((int)lcg_next15() % 0x50 - 200) * 0.1;
        }
        else {
          v.x = (float)((int)lcg_next15() % 0x32 + 0x8c) * 0.1;
          v.z = (float)((int)lcg_next15() % 0x140 - 0x78) * 0.1;
        }
        v.y = 0.0;
        p->flags = 3;
        *(vec_f3 *)&p->pos = v;
        p->turn = effect_rand15();
        p->frame = (int)effect_rand15() % p->frames;
        return;
      }
    }
  }
}

void aquarium_bubble_draw(void)
{
  aquarium_bubble *p;
  uint *models;
  int i;
  p = aquarium_bubbles;
  mtx_push(0);
  mtx_store(fx_saved_mtx);
  i = 0;
  do
  {
    if (((p->flags) & 2) != 0)
    {
      mtx_set_current(fx_saved_mtx);
      mtx_translate_by_vec3(&p->pos.x);
      mtx_rot_y_s16((int) (p->turn));
      models = aquarium_bubble_models;
      obj_draw(models[p->frame]);
    }
    i = i + 1;
    p = p + 1;
  }
  while (i < 0x32);
  mtx_pop(1);
  return;
}

void aquarium_bubble_end(void)
{
  effect_free(aquarium_bubbles);
  aquarium_bubbles = 0;
  return;
}

void rob_fx_enable(void)
{
  fx_rec *p = rob_fx_pool;
  int i;

  for (i = 0; i < 16; i++, p++) {
    p->flags = 0;
    p->wait = 0;
    p->time = 0.0;
    p->frame = 0;
    p->speed = 1.0;
    p->frame_table = 0;
    p->scale = 1.0;
  }
  rob_fx_flags |= 1;
}

void rob_fx_disable(void)
{
  rob_fx_flags = rob_fx_flags & 0xfe;
  return;
}

void rob_fx_update(void)
{
  fx_rec *p = rob_fx_pool;
  rob_rec *rob;
  int i;
  char c;
  int k;
  rob_coli_rec *coli;
  int bad;

  rob = rob_get(0);
  if (effect_play_allowed() == 0) {
    return;
  }
  for (i = 0; i < 16; i++, p++) {
    if (p->flags & 1) {
      fx_anim_step(p);
    } else {
      c = p->flags;
      if (c & 8) {
        p->wait++;
        if ((p->slot == 3) | (p->slot == 12)) {
          p->flags = (c & ~8) | 3;
        } else {
          if (p->wait != rob_fx_wait_frames) continue;
          rob_slot_pos_get((rob_rec *)p->rob, &p->pos.x);
          p->flags = (p->flags & ~8) | 3;
        }
      }
    }
  }
  for (i = 0; i < 2; i++, rob++) {
    coli = rob->coli;
    if (coli->field_0x1c[1] == 0) continue;
    k = coli->field_0x94;
    if (((uchar)(k - 11) <= 2) | (k == 0)) continue;
    if (k != 10) {
      if (rob->attack_damage >= 30) {
        p = rob_fx_spawn(-1, 5, rob);
        p->flags |= 0x20;
        p->turn = -0x8000 - rob->opp_dir;
        p = rob_fx_spawn(-1, 100, rob);
        p->turn = effect_rand15();
        p = rob_fx_spawn(-1, 32, rob);
        p->scale = 2.0;
        p = rob_fx_spawn(-1, 80, rob);
        p->flags |= 0x10;
        p->dir.x = rob->opp->knockback.x;
        p->dir.y = rob->opp->knockback.y;
        p->dir.z = rob->opp->knockback.z;
        vec3_normalize_nolen(&p->dir.x);
      } else {
        p = rob_fx_spawn(-1, 5, rob);
        p->flags |= 0x20;
        p->turn = -0x8000 - rob->opp_dir;
        rob_fx_spawn(-1, 32, rob);
      }
    } else {
      rob_fx_spawn(-1, 32, rob);
    }
  }
}

fx_rec * rob_fx_spawn(int model, int frames, rob_rec *rob)
{
  short s2 = (short)frames;
  char c;
  int i;
  fx_rec *p;

  p = rob_fx_pool;
  for (i = 0; i < 0x10; i++, p++) {
    if (p->flags == 0) break;
  }
  if (i == 0x10) {
    p = rob_fx_pool;
    rob_fx_pool[0].flags = 0;
  }
  fx_anim_reset(p);
  p->frames = s2;
  p->model = model;
  p->scale = 1.0;
  p->rob = (int)rob;
  if (rob == 0) {
    p->flags = 3;
    p->turn = 0;
  }
  else {
    p->flags |= 8;
    p->wait = 0;
    p->slot = *(char *)&rob->field_0x328;
    if (p->slot == 3 || p->slot == 0xc) {
      rob_slot_pos_get(rob,&p->pos.x);
    }
    if ((rob->flags & 0x20) != 0) {
      p->turn = 0x8000;
    }
    else {
      p->turn = 0;
    }
  }
  return p;
}

void rob_fx_draw(void)
{
  float (*up_ref)[3];
  fx_rec *p;
  float *m;
  float *scale_ref;
  int i;
  float s;
  float v[3];
  p = rob_fx_pool;
  m = fx_saved_mtx;
  mtx_push(0);
  mtx_store(m);
  for (i = 0; i < 0x10; i++, p++)
  {
    if ((p->flags & 2) != 0)
    {
      scale_ref = &s;
      mtx_set_current(m);
      mtx_translate_by_vec3(&p->pos.x);
      up_ref = &v;
      s = p->scale;
      if ((*scale_ref) > 1.0)
      {
        mtx_scale_xyz(s, s, 1.0);
      }
      if ((p->flags & 0x10) != 0)
      {
        v[0] = 0.0;
        v[1] = 0.0;
        v[2] = 1.0;
        mtx_rot_vec_to_vec((uint *) (*up_ref), &p->dir.x);
      }
      else
      {
        if ((p->flags & 0x20) == 0)
        {
          mtx_reset_3x3();
        }
        mtx_rot_y_s16(p->turn);
      }
      obj_draw(p->frame + p->model);
    }
  }

  mtx_pop(1);
}

void rob_slot_pos_get(rob_rec *rob, float *pos)
{
  float *f;
  int c;
  
  c = rob_slot_mtx_index[(int)rob->slot];
  f = rob->mtx + c * 16;
  pos[0] = f[12];
  pos[1] = f[13];
  pos[2] = f[14];
  return;
}

void pitch_to_point_unused(float *from,float *to)
{
  float dx = to[0] - from[0];
  float dz = to[2] - from[2];
  atan2_s16(to[1] - from[1], fsqrt_inline(dx * dx + dz * dz));
}

void rob_fx_slot_spawn(char kind, float *bone_mtx)
{
  vec3 v;
  fx_rec *fx;

  if ((rob_fx_flags & 1) != 0) {
    v.x = bone_mtx[12];
    v.y = bone_mtx[13];
    v.z = bone_mtx[14];
    switch (kind) {
    case 1:
      fx = rob_fx_spawn(0xffffffff,100,0);
      fx->turn = effect_rand15();
      fx->pos = v;
      fx = rob_fx_spawn(0xffffffff,0x20,0);
      fx->scale = 2.0;
      fx->pos = v;
      break;
    case 0:
      fx = rob_fx_spawn(0xffffffff,0x20,0);
      fx->pos = v;
      break;
    }
  }
}

void down_init_bank18(void)
{
  down_fx_init(0x18010e,0x40);
  return;
}

void down_init_bank19(void)
{
  down_fx_init(0x190143,0x40);
  return;
}

void rome_down_init(void)
{
  down_fx_init(0x1b0385,0x40);
  return;
}

void nyc_down_init(void)
{
  down_fx_init(0x1c0001,0x40);
  return;
}

void down_fx_init(int model, short frames)
{
  fx_rec *p;
  int i;

  p = down_fx;
  i = 0;
  do {
    p->flags = 0;
    p->time = 0.0;
    p->frame = 0;
    p->speed = 1.0;
    p->frame_table = 0;
    p->scale = 1.0;
    p->frames = frames;
    p->model = model;
    i = i + 1;
    p = p + 1;
  } while (i < 2);
  return;
}

void down_fx_update(void)
{
  typedef struct { int w[3]; } blk;
  rob_rec *rob;
  int i;
  fx_rec *p;

  p = down_fx;
  rob = rob_get(0);
  if (effect_play_allowed() != 0) {
    for (i = 0; i < 2; i++, p++, rob++) {
      if ((p->flags & 1) != 0) {
        fx_anim_step(p);
      }
      if ((p->flags & 4) != 0 && rob->action.part.type == 8 && rob->action_step == 4) {
        p->flags = 3;
        *(blk *)&p->pos = *(blk *)&rob->ground_pos;
        fx_anim_reset(p);
        p->flags &= 0xfb;
      }
      else if (rob->action.part.type == 8 && rob->action_step == 2) {
        p->flags |= 4;
      }
    }
  }
}

void down_fx_draw(void)
{
  int i;
  fx_rec *p;
  float *m;

  p = down_fx;
  m = fx_saved_mtx;
  mtx_push(0);
  mtx_store(m);
  for (i = 0; i < 2; i++, p++) {
    if ((p->flags & 2) != 0) {
      mtx_set_current(m);
      mtx_translate_by_vec3(&p->pos.x);
      mtx_rot_y_s16(camera.yaw);
      if (p->model != -1) {
        obj_draw(p->frame + p->model);
      }
    }
  }
  mtx_pop(1);
}

void fx_anim_step(fx_rec *fx)
{
  short *t;
  float f;

  t = fx->frame_table;
  f = fx->time + fx->speed;
  fx->time = f;
  if (t == 0) {
    fx->frame = (short)(int)f;
    if (fx->frame >= fx->frames) {
      fx->flags = 0;
    }
  }
  else {
    fx->frame = t[(short)(int)fx->time];
    if (fx->frame == -1) {
      fx->flags = 0;
    }
  }
}

void fx_anim_reset(fx_rec *fx)
{
  fx->time = 0.0f;
  if (fx->frame_table == 0) {
    fx->frame = 0;
    return;
  }
  fx->frame = *fx->frame_table;
}

void step_init_bank18(void)
{
  step_fx_init(0x18014e,0x15);
  return;
}

void rome_step_init(void)
{
  step_fx_init(0x1b03f9,0x15);
  return;
}

void nyc_step_init(void)
{
  step_fx_init(0x1c0041,0x15);
  return;
}

void step_fx_init(int model, short frames)
{
  int i;
  step_fx_rec *p;

  p = step_fx;
  i = 0;
  do {
    p->flags = 0;
    p->model = model;
    p->frames = frames;
    i = i + 1;
    p = p + 1;
  } while (i < 0x40);
  i = 0;
  do {
    step_fx_wait[i] = 0;
    i = i + 1;
  } while (i < 2);
  step_fx_floor_y = stage_table[stage_cur_no]->center.y;
  return;
}

void step_fx_update(void)
{
  typedef struct { char pad[0x4f0]; float q[3]; char pad2[0xf4]; float p[3]; } obj_t;
  rob_rec *rob;
  int i;
  short *cnt;
  step_fx_rec *e;
  float d;
  float a[3];
  float b[3];
  e = step_fx;
  rob = rob_get(0);
  if (effect_play_allowed() != 0) {
    for (i = 0; i < 0x40; i++, e++) {
      if ((e->flags & 1) != 0) {
        e->time = e->time + 1.0;
        e->frame = (short)e->time;
        if (e->frame >= e->frames) e->flags = 0;
      }
    }
    for (i = 0; i < 2; i++, rob++) {
      cnt = step_fx_wait + i;
      if (*cnt > 0) {
        *cnt = *cnt - 1;
      }
      else if ((rob->flags & 1) != 0) {
        a[0] = ((obj_t *)rob->bone_mtx)->p[0];
        a[1] = ((obj_t *)rob->bone_mtx)->p[1];
        a[2] = ((obj_t *)rob->bone_mtx)->p[2];
        b[0] = ((obj_t *)rob->bone_mtx_prev)->p[0];
        b[1] = ((obj_t *)rob->bone_mtx_prev)->p[1];
        b[2] = ((obj_t *)rob->bone_mtx_prev)->p[2];
        d = vec3_distance(a,b);
        if (b[1] - step_fx_floor_y >= 0.0 && b[1] - step_fx_floor_y < step_fx_floor_band && d > step_fx_min_move) {
          e = step_fx_spawn(a);
          step_fx_wait[i] = step_fx_wait_frames;
          if (d > step_fx_big_move) e->scale = 1.0; else e->scale = 0.5;
        }
        a[0] = ((obj_t *)rob->bone_mtx)->q[0];
        a[1] = ((obj_t *)rob->bone_mtx)->q[1];
        a[2] = ((obj_t *)rob->bone_mtx)->q[2];
        b[0] = ((obj_t *)rob->bone_mtx_prev)->q[0];
        b[1] = ((obj_t *)rob->bone_mtx_prev)->q[1];
        b[2] = ((obj_t *)rob->bone_mtx_prev)->q[2];
        d = vec3_distance(a,b);
        if (b[1] - step_fx_floor_y >= 0.0 && b[1] - step_fx_floor_y < step_fx_floor_band && d > step_fx_min_move) {
          e = step_fx_spawn(a);
          step_fx_wait[i] = step_fx_wait_frames;
          if (d > step_fx_big_move) e->scale = 1.0; else e->scale = 0.5;
        }
      }
    }
  }
}

step_fx_rec * step_fx_spawn(float *pos)
{
  int i;
  step_fx_rec *p;

  p = step_fx;
  i = 0;
  do {
    if (p->flags == '\0') break;
    i = i + 1;
    p = p + 1;
  } while (i < 0x40);
  if (i == 0x40) {
    p = step_fx;
  }
  p->flags = p->flags | 3;
  p->time = 0.0;
  p->frame = 0;
  p->pos.x = *pos;
  p->pos.z = pos[2];
  p->pos.y = step_fx_floor_y;
  return p;
}

void step_fx_draw(void)
{
  step_fx_rec *p;
  int i;
  float *m;

  p = step_fx;
  m = fx_saved_mtx;
  mtx_push(0);
  mtx_store(m);
  for (i = 0; i < 0x40; i++, p++) {
    if ((p->flags & 2) != 0) {
      mtx_set_current(m);
      mtx_translate_by_vec3(&p->pos.x);
      mtx_rot_y_s16(camera.yaw + 0x8000);
      mtx_scale_xyz(p->scale,p->scale,1.0);
      obj_draw(p->frame + p->model);
    }
  }
  mtx_pop(1);
}

int effect_play_allowed(void)
{
  if (mode_get_main() == MODE_MAIN_GAME) {
    if (mode_get_sub() < 9) return 0;
  }
  else if (mode_get_main() == MODE_MAIN_ADVERTISE) {
    switch (mode_get_sub()) {
    case 7:
    case 9:
      return 0;
    }
  }
  return 1;
}

void fighter_fx_clear(void)
{
  int i;
  uchar *flags;
  
  flags = (uchar *)rob_fx_pool;
  i = 0;
  do {
    *flags = 0;
    i = i + 1;
    flags = flags + 0x3c;
  } while (i < 0x10);
  flags = (uchar *)down_fx;
  i = 0;
  do {
    *flags = 0;
    i = i + 1;
    flags = flags + 0x3c;
  } while (i < 2);
  flags = (uchar *)step_fx;
  i = 0;
  do {
    *flags = 0;
    i = i + 1;
    flags = flags + 0x20;
  } while (i < 0x40);
  return;
}

void garden_other_init(void)
{
  garden_other_rand_init();
  garden_frame_b = 0;
  garden_frame_a = 0;
  return;
}

void garden_other_update(void)
{
  garden_other_frames_step();
  if (++garden_frame_a > 0x1d) {
    garden_frame_a = 0;
  }
  if (++garden_frame_b > 0x7f) {
    garden_frame_b = 0;
  }
}

void garden_other_draw(void)
{
  mtx_push(0);
  garden_other_objs_draw();
  mtx_peek_pushed(0);
  obj_draw(((short)garden_frame_a) + 0x1f0051);
  obj_draw(((int)((short)garden_frame_b) >> 1) + 0x1f0072);
  mtx_pop(1);
  return;
}

void garden_other_rand_init(void)
{
  int i;
  garden_other_obj *p;
  
  p = &garden_other_objs;
  for (i = 0; i < 6; i++, p++) {
    p->time = (float)((int)lcg_next15() % 80);
  }
}

void garden_other_frames_step(void)
{
  struct { short a; short pad; float f; } *p;
  int i;
  
  p = (void *)&garden_other_objs;
  for (i = 0; i < 6; i++, p++) {
    p->f += garden_other_speed;
    p->a = p->f;
    if (p->a > 0x4f) {
      p->f = 0.0;
      p->a = 0;
    }
  }
}

void garden_other_objs_draw(void)
{
  int i;
  short *frame;
  
  frame = &garden_other_objs.frame;
  i = 0;
  do {
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(garden_other_positions[i]);
    obj_draw(*frame + 0x1f0001);
    i = i + 1;
    frame = frame + 4;
  } while (i < 6);
  return;
}

void hbr_other_init(void)
{
  hbr_float_init();
  hbr_clock_init();
  hbr_turn_angle = lcg_next15();
  return;
}

void hbr_other_update(void)
{
  hbr_float_update();
  hbr_clock_update();
  hbr_turn_angle = hbr_turn_angle + hbr_turn_speed;
  return;
}

void hbr_other_draw(void)
{
  mtx_push(0);
  hbr_float_draw();
  hbr_clock_draw();
  mtx_peek_pushed(0);
  mtx_translate_by_vec3(hbr_turn_pos);
  mtx_rot_y_s16(hbr_turn_angle);
  obj_draw(0x20000b);
  obj_draw(0x20000c);
  mtx_pop(1);
  return;
}

void hbr_float_init(void)
{
  hbr_float_obj *e;
  char *q;
  int i;

  e = &hbr_floats;
  q = hbr_float_rows[0];
  for (i = 0; i < 10; i++, e++, q += 0x28) {
    e->bob_phase = (int)lcg_next15() % 0x10000;
    e->roll_phase = (int)lcg_next15() % 0x10000;
    e->pitch_phase = (int)lcg_next15() % 0x10000;
    e->row = q;
  }
}

void hbr_float_update(void)
{
  typedef struct { short v[3]; short pad[5]; float s[3]; } speed;
  hbr_float_obj *e;
  speed *d;
  int i;

  e = &hbr_floats;
  for (i = 0; i < 10; i++, e++) {
    d = (speed *)e->row;
    e->bob_phase += d->v[0];
    e->roll_phase += d->v[1];
    e->pitch_phase += d->v[2];
    *(float *)&e->bob = sin_s16(e->bob_phase) * d->s[0];
    *(short *)&e->roll = sin_s16(e->roll_phase) * d->s[1];
    *(short *)&e->pitch = sin_s16(e->pitch_phase) * d->s[2];
  }
}

void hbr_float_draw(void)
{
  typedef struct { float x; float y; float z; } vec;
  vec v;
  hbr_float_obj *e;
  char *p;
  int i;

  e = &hbr_floats;
  for (i = 0; i < 10; i++, e++) {
    mtx_peek_pushed(0);
    p = e->row;
    v = *(vec *)(p + 0x1c);
    v.y = v.y + *(float *)&e->bob;
    mtx_translate_by_vec3((float *)&v);
    mtx_rot_y_s16(*(short *)(p + 8));
    mtx_rot_x_s16(*(short *)&e->pitch);
    mtx_rot_z_s16((int)*(short *)&e->roll);
    obj_draw(*(uint *)(p + 0xc));
  }
}

void hbr_clock_init(void)
{
  return;
}

void hbr_clock_update(void)
{
  tm t;
  short a;
  short b;

  get_now_tm(&t);
  a = ((t.tm_hour % 12) << 16) / 12 + (t.tm_min * 0x1555) / 60;
  b = (t.tm_min << 16) / 60;
  hbr_clock_hands = -a;
  hbr_clock_minute_hand = -b;
}

void hbr_clock_draw(void)
{
  mtx_peek_pushed(0);
  mtx_translate(13.9,20.85,53.45);
  mtx_rot_y_s16(0x8000);
  mtx_push(0);
  mtx_rot_z_s16((int)hbr_clock_hands);
  obj_draw(0x20000e);
  mtx_pop(1);
  mtx_rot_z_s16((int)hbr_clock_minute_hand);
  obj_draw(0x20000d);
  return;
}

void umi_etc_init(void)
{
  umi_anim_init();
  umi_float_init();
  return;
}

void umi_etc_update(void)
{
  umi_anim_step();
  umi_float_update();
  return;
}

void umi_etc_draw(void)
{
  mtx_push(0);
  umi_anim_draw();
  umi_float_draw();
  mtx_pop(1);
  return;
}

void umi_float_init(void)
{
  umi_float_rec *s = &umi_float;
  s->bob_phase = (int)lcg_next15() % 0x10000;
  s->roll_phase = (int)lcg_next15() % 0x10000;
  s->pitch_phase = (int)lcg_next15() % 0x10000;
  s->spin_a = (int)lcg_next15() % 0x10000;
  s->spin_b = (int)lcg_next15() % 0x10000;
  s->counter = 0;
  s->row = umi_float_row;
}

void umi_float_update(void)
{
  umi_float_rec *s;
  short *p;

  s = &umi_float;
  p = (short *)s->row;
  s->bob_phase += p[0];
  s->roll_phase += p[1];
  s->pitch_phase += p[2];
  s->bob = sin_s16(s->bob_phase) * *(float *)(p + 8);
  s->roll = sin_s16(s->roll_phase) * *(float *)(p + 10);
  s->pitch = sin_s16(s->pitch_phase) * *(float *)(p + 12);
  s->spin_a += p[3];
  s->spin_b += p[3] + 0x40;
  if (++s->counter > 47) {
    s->counter = 0;
  }
}

void umi_float_draw(void)
{
  typedef struct { uint w[3]; } v3;
  umi_float_rec *a = &umi_float;
  char *p = a->row;
  v3 v;
  mtx_peek_pushed(0);
  v = *(v3 *)(p + 0x1c);
  *(float *)&v.w[1] = *(float *)&v.w[1] + a->bob;
  mtx_translate_by_vec3((float *)v.w);
  mtx_rot_y_s16(*(short *)(p + 10));
  mtx_rot_x_s16(a->pitch);
  mtx_rot_z_s16((int)a->roll);
  obj_draw(0x1d0020);
  mtx_push(0);
  mtx_translate_by_vec3((float *)(p + 0x28));
  mtx_rot_z_s16((int)a->spin_a);
  obj_draw(0x1d0021);
  mtx_pop(1);
  mtx_translate_by_vec3((float *)(p + 0x34));
  mtx_rot_z_s16((int)a->spin_b);
  obj_draw(0x1d0021);
  return;
}

void umi_anim_init(void)
{
  umi_anim_frame = 0;
  return;
}

void umi_anim_step(void)
{
  if (++umi_anim_frame > 0x77) {
    umi_anim_frame = 0;
  }
}

void umi_anim_draw(void)
{
  mtx_peek_pushed(0);
  obj_draw(((int)((short)umi_anim_frame) >> 1) + 0x1d0053);
  return;
}

void djo_light_cycle_init(void)
{
  djo_light_cycle_index = 0;
  djo_light_cycle_next();
  djo_light_cycle_hold = 1;
  return;
}

void empty_fn_0c0b65c0(void)
{
  return;
}

void mtx_push_pop_only(void)
{
  mtx_push(0);
  mtx_pop(1);
  return;
}

void djo_light_cycle_hold_clear(void)
{
  djo_light_cycle_hold = 0;
  return;
}

void djo_light_cycle_next(void)
{
  uchar *t;
  if (djo_light_cycle_hold == 0) {
    if (++djo_light_cycle_index > 3) djo_light_cycle_index = 0;
    if (stage_cur_no == 0x10) {
      light_set_load(0,djo_light_set0_nos[djo_light_cycle_index]);
      t = djo_light_set1_nos;
      light_set_load(1,t[djo_light_cycle_index]);
      light_set_load(4,t[djo_light_cycle_index]);
      djo_light_row = djo_light_rows[djo_light_cycle_index];
    }
  }
  else {
    djo_light_cycle_hold = 0;
  }
}

void are_etc_init(void)
{
  int rnd;
  int rnd_up;
  int i;
  are_billboard *b;
  
  b = are_billboards;
  i = 0;
  do {
    rnd = lcg_next15();
    rnd_up = rnd;
    if (rnd < 0) {
      rnd_up = rnd + 0x7f;
    }
    b->frame = (short)rnd - (short)((rnd_up >> 7) << 7);
    i = i + 1;
    b = b + 1;
  } while (i < 4);
  return;
}

void are_etc_update(void)
{
  int i;
  are_billboard *b;
  b = are_billboards;
  if (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) return;
  for (i = 0; i < 4; i++, b++) {
    b->frame = b->frame + 1;
    if (b->frame > 0x7f) b->frame = 0;
  }
}

void are_etc_draw(void)
{
  int i;
  are_billboard *b;
  
  b = are_billboards;
  if (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) return;
  mtx_push(0);
  for (i = 0; i < 4; i++, b++) {
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(&b->pos.x);
    mtx_rot_y_s16(camera.yaw);
    obj_draw(b->frame + 0x210000);
  }
  mtx_pop(1);
}

void noh_etc_init(void)
{
  return;
}

void noh_etc_update(void)
{
  short yaw;
  noh_etc_obj *p;
  int i;
  
  p = noh_etc_objs;
  i = 0;
  do {
    yaw = atan2_s16(camera.eye.x - p->x,
                         camera.eye.z - p->z);
    p->turn = yaw + -0x8000;
    i = i + 1;
    p = p + 1;
  } while (i < 4);
  return;
}

void noh_etc_draw(void)
{
  noh_etc_obj *p;
  int i;
  p = noh_etc_objs;
  mtx_push(0);
  for (i = 0; i < 4; i++, p++) {
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(&p->x);
    mtx_rot_y_s16(p->turn);
    mtx_scale_xyz(p->size, p->size, p->size);
    obj_draw(0x130006);
  }
  mtx_pop(1);
}

void cav_candle_init(void)
{
  int i;
  
  i = 0;
  do {
    cav_candle_frames[i] = (short)(i << 3);
    i = i + 1;
  } while (i < 4);
  return;
}

void cav_candle_update(void)
{
  short frame;
  short *p;
  int i;
  
  i = 0;
  do {
    p = cav_candle_frames + i;
    frame = *p;
    *p = frame + 1;
    if (0x1f < (short)(frame + 1)) {
      *p = 0;
    }
    i = i + 1;
  } while (i < 4);
  return;
}

void cav_candle_draw(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    obj_draw(cav_candle_models[i] + (short)cav_candle_frames[i]);
  }
}

void ban_flag_init(void)
{
  int i;
  
  for (i = 0; i < 0x32; i++) {
    ban_flag_frames[i] = (int)lcg_next15() % 64;
  }
}

void ban_flag_update(void)
{
  short frame;
  short *p;
  int i;
  
  i = 0;
  do {
    p = ban_flag_frames + i;
    frame = *p;
    *p = frame + 1;
    if (0x3f < (short)(frame + 1)) {
      *p = 0;
    }
    i = i + 1;
  } while (i < 0x32);
  return;
}

void ban_flag_draw(void)
{
  ban_flag *flag;
  int i;
  float f;
  flag = ban_flags;
  mtx_push(0);
  for (i = 0; i < 0x32; i++, flag++) {
    f = flag->scale;
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(&flag->pos.x); mtx_rot_y_s16(flag->turn); mtx_scale_xyz(f,f,f);
    short *t = ban_flag_frames; obj_draw(t[i] + 0x150000);
  }
  mtx_pop(1);
}

void dural_stage_init(void)
{
  dural_stage_scroll[0] = 250.0;
  dural_stage_scroll[1] = -250.0;
  dural_scroll_rate = 0.002;
  dural_scroll_speed = 0.0;
}

void dural_stage_update(void)
{
  short s;
  float f;
  int i;

  if ((mode_get_main() == MODE_MAIN_GAME) &&
     ((mode_get_sub() == 10 || (mode_get_sub() == 9)))) {
    s = (rob_get(0))->life;
    dural_scroll_speed = (float)(s - (rob_get(1))->life) * dural_scroll_rate;
  }
  for (i = 0; i < 2; i++) {
    f = dural_stage_scroll[i] + dural_scroll_speed;
    dural_stage_scroll[i] = f;
    if (f >= 500.0) dural_stage_scroll[i] = f - 1000.0;
    else if (dural_stage_scroll[i] <= -500.0) dural_stage_scroll[i] += 1000.0;
  }
}

void dural_stage_draw(void)
{
  int i;
  
  mtx_push(0);
  i = 0;
  do {
    mtx_peek_pushed(0);
    mtx_translate(0.0,0.0,dural_stage_scroll[i]);
    obj_draw(0x160001);
    i = i + 1;
  } while (i < 2);
  mtx_pop(1);
  return;
}

float dural_stage_scroll_get(int index)
{
  return dural_stage_scroll[index];
}

void are_fan_init(void)
{
  int v;
  are_fan *fan;
  int i;
  
  fan = are_fans;
  for (i = 0; i < 2; i++, fan++) {
    v = (int)lcg_next15() % 0x10000;
    fan->angle = v;
    fan->spin = v % 128 + 128;
    fan->spin_set = v % 16 + 128;
  }
  return;
}

void are_fan_update(void)
{
  are_fan *fan;
  int i;
  
  fan = are_fans;
  for (i = 0; i < 2; i++, fan++) {
    fan->spin = fan->spin_set;
    if (fan->spin > 0x200) fan->spin = 0x200;
    else if (fan->spin < -0x200) fan->spin = -0x200;
    fan->angle -= fan->spin;
  }
}

void are_fan_draw(void)
{
  are_fan *fan;
  int i;
  
  fan = are_fans;
  mtx_push(0);
  i = 0;
  do {
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(&fan->pos.x);
    mtx_rot_x_s16((int)fan->angle);
    obj_draw(0x210268);
    i = i + 1;
    fan = fan + 1;
  } while (i < 2);
  mtx_pop(1);
  return;
}

void breath_init(void)
{
  ushort i;
  oneshot_anim_pool_alloc();
  for (i = 0; i < 2; i++) {
    char *p = breath_phase;
    p[i] = (int)lcg_next15() % 0x7f;
  }
}

void breath_draw(void)
{
  oneshot_anim_draw_all();
  return;
}

void breath_end(void)
{
  effect_free(oneshot_anim_pool);
  oneshot_anim_pool = 0;
  return;
}

void breath_emit(uint *anim_def)
{
  rob_rec *rob;
  uint divisor;
  uint dividend;
  ushort i;
  float x;
  float y;
  float z;
  int v[3];
  char *t;

  i = 0;
  do {
    rob = rob_get((uchar)i);
    if (((rob->flags & 1) != 0) &&
       ((rob->flags24 & 0x4000000) == 0)) {
      int *g = &game_frame_count;
      t = breath_phase;
      dividend = t[i] + *g;
      divisor = __fixunssfsi(((float)rob->life /
                             (float)rob->life_max) * 60.0 + 40.0);
      if (dividend % divisor == 0) {
        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
        mtx_push(0);
        mtx_set_current_wordwise(rob->mtx + 0x30);
        mtx_translate(lcg_next_unit() * 0.06 + 0.2,lcg_next_unit() * 0.1 + -0.05,
                      -0.04 - lcg_next_unit() * 0.04);
        mtx_store_translation((float *)v);
        mtx_pop(1);
        oneshot_anim_spawn(anim_def,(uint *)v,0);
      }
    }
    i = i + 1;
  } while (i < 2);
}

void cas_breath_update(void)
{
  breath_emit(cas_breath_anim);
  return;
}

void djo_breath_update(void)
{
  breath_emit(djo_breath_anim);
  return;
}

void dust_init(void)
{
  dust_on = 0;
  dust_count = 0;
  dust_slots_clear();
  dust_models_find();
  dust_uv_base_save();
  return;
}

void dust_update(void)
{
  if (dust_on != '\0') {
    dust_uv_scroll_step();
  }
  return;
}

void dust_draw(void)
{
  if (dust_on != '\0') {
    dust_uv_apply();
    dust_models_draw();
  }
  return;
}

void dust_end(void)
{
  if (dust_on != '\0') {
    dust_slots_clear();
  }
  return;
}

void dbg_efc_dust_model_remake(void)
{
  return;
}

void dust_slots_clear(void)
{
  dust_rec *p;
  int i;
  int j;

  p = dust_records;
  for (i = 0; i < 20; i++, p++) {
    p->flag = 0;
    p->n = 0;
    p->model = 0;
    p->u = 0.0;
    p->v = 0.0;
    p->du = 0.0;
    p->dv = 0.0;
    for (j = 0; j < 70; j++) {
      p->uv[j].s = 0.0;
      p->uv[j].t = 0.0;
    }
  }
}

void dust_models_find(void)
{
  uint i;
  int n;
  dust_rec *e;

  e = dust_records;
  i = 0;
  n = 0;
  for (; objbanks[3].names[i] != 0; i++) {
    if (strstr(objbanks[3].names[i],"_DUST_") != (char *)0x0) {
      if (0x13 < n) {
        return;
      }
      e[n].model = (int)obj_model_get(objbanks[3].set << 0x10 | i);
      if (strstr(objbanks[3].names[i],"DJO_DUST_A") != (char *)0x0) {
        e[n].dv = 0.006;
      }
      if (strstr(objbanks[3].names[i],"DJO_DUST_B") != (char *)0x0) {
        e[n].dv = 0.004;
      }
      if (strstr(objbanks[3].names[i],"DJO_DUST_C") != (char *)0x0) {
        e[n].dv = 0.002;
      }
      if (strstr(objbanks[3].names[i],"BAN_DUST") != (char *)0x0) {
        e[n].du = 0.002;
      }
      n = n + 1;
      dust_on = 1;
      dust_count = dust_count + 1;
    }
  }
}

void dust_uv_base_save(void)
{
  dust_rec *e;
  int i;
  int j;
  int k;
  int model;

  e = dust_records;
  for (i = 0; i < dust_count; i++, e++) {
    model = e->model;
    if (model != 0) {
      e->n = *(int *)(model + 0x24);
      if (*(int *)(model + 0x68) == 0x600) {
        e->flag = 1;
      }
      if (e->n > 0x45) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect_etc.c",0x960,
                 "assertion `%s\' failed.\n","0");
      }
      k = 0x30;
      for (j = 0; j < e->n; j++) {
        e->uv[j].s = *(float *)(model + k * 4 + 0x10);
        e->uv[j].t = *(float *)(model + k * 4 + 0x14);
        if (e->flag != '\0') {
          k = k + 8;
        }
        else {
          k = k + 6;
        }
      }
    }
    e->u = 0.0;
    e->v = 0.0;
  }
}

void dust_uv_scroll_step(void)
{
  dust_rec *p;
  int i;
  for (p = dust_records, i = 0; i < dust_count; i++, p++) {
    p->u += p->du;
    if (p->u >= 2.0) p->u = 0.0;
    p->v += p->dv;
    if (p->v >= 2.0) p->v = 0.0;
  }
}

void dust_uv_apply(void)
{
  char *model;
  int k;
  dust_rec *e;
  int j;
  int i;

  e = dust_records;
  for (i = 0; i < dust_count; i++, e = e + 1) {
    model = (char *)e->model;
    if (model != 0) {
      k = 0x30;
      *(uint *)(k * 4 + model - 0x18) &= 0xfffe7fff;
      *(uint *)(k * 4 + model - 0x10) &= 0xfffe7fff;
      for (j = 0; j < e->n; j++) {
        *(float *)(k * 4 + model + 0x10) = e->uv[j].s + e->u;
        *(float *)(k * 4 + model + 0x14) = e->uv[j].t + e->v;
        if (e->flag != '\0') {
          k = k + 8;
        }
        else {
          k = k + 6;
        }
      }
    }
  }
  return;
}

void dust_models_draw(void)
{
  int i;
  dust_rec *e;

  e = dust_records;
  for (i = 0; i < dust_count; i = i + 1, e = e + 1) {
      if (e->model != 0) {
        model_draw((compiled_model_rec *)e->model);
      }
  }
  return;
}

void break_time_start(float *start)
{
  int now;
  
  now = stopwatch_usec();
  *start = (float)now;
  return;
}

void break_time_lap(float *start, float *last, float *max)
{
  float t;
  float d;
  if (break_time_skip != 0) {
    if (break_time_skip_frame != game_frame_count) {
      break_time_skip_frame = game_frame_count;
      break_time_skip = break_time_skip + -1;
    }
  }
  else {
    t = (float)(int)stopwatch_usec();
    d = t - *start;
    *last = d;
    if (*max < d) {
      *max = d;
    }
    *start = t;
  }
}

void break_floor_init(void)
{
  break_piece *p;
  short i;
  for (p = break_floor_tiles; p->model != -1; p++) {
    p->flags = 0;
  }
  oneshot_anim_pool_alloc();
  stage_break_flags = '\0';
  break_floor_hit_frames = 0;
  break_fall_list_clear();
  light_set_load(2,3);
  break_floor_variants = effect_malloc(48000);
  break_floor_model_build();
  break_floor_variants_build();
  for (i = 0; i < 0x14; i++) {
    break_floor_queue[i] = 0;
  }
  break_floor_queue_len = 0;
}

void break_floor_exec(void)
{
  float total_start;
  float step_start;
  float *q;
  
  break_time_start(&total_start);
  break_time_start(&step_start);
  break_floor_hits = 0;
  if ((stage_break_flags & 1U) != 0) {
    break_floor_request_apply();
    if (break_floor_hits != 0) {
      break_floor_hit_frames = break_floor_hit_frames + 1;
    }
    stage_break_flags = stage_break_flags & 0xfe;
  }
  q = &break_time_slots[2];
  break_time_lap(&step_start,q,q + 1);
  break_floor_fall_anim_step();
  break_time_lap(&step_start,q + 2,q + 3);
  break_floor_crack_merge_one();
  break_time_lap(&step_start,q + 4,q + 5);
  break_time_lap(&total_start,q - 2,q - 1);
  return;
}

void break_floor_crack_merge_one(void)
{
  if (0 < break_floor_queue_len) {
    break_floor_queue_len = break_floor_queue_len + -1;
    break_floor_tile_crack_write(break_floor_queue[break_floor_queue_len]);
  }
  return;
}

void break_floor_draw(void)
{
  short i;
  break_piece *e;
  char *ip;
  float *q;
  float *p;
  float total_start[1];
  float step_start[1];
  
  break_time_start(total_start);
  break_time_start(step_start);
  mtx_push(0);
  camera_view_load();
  mtx_translate(0.0,(stage_cur->center.y - 0.05) - 0.5,0.0);
  light_set_send(0,2);
  mtx_push(0);
  q = &break_time_slots[12];
  break_time_lap(step_start,q,q + 1);
  for (i = 0; i < break_floor_queue_len; i++) {
    ip = (char *)(i * 4);
    e = break_floor_tiles + *(int *)(ip + (int)break_floor_queue);
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(&e->pos.x);
    mtx_rot_y_s16(e->turn);
    obj_draw(e->model2);
  }
  mtx_peek_pushed(0);
  if (break_floor_model != (uchar *)0x0) {
    model_draw((compiled_model_rec *)break_floor_model);
  }
  p = &break_time_slots[14];
  break_time_lap(step_start,p,p + 1);
  mtx_pop(1);
  break_floor_fall_draw();
  break_time_lap(step_start,p + 2,p + 3);
  mtx_pop(1);
  break_time_lap(total_start,p - 4,p - 3);
  break_floor_draw_tail();
}

void break_floor_free(void)
{
  if (break_floor_model != 0) {
    elan_heap_free(break_floor_model);
  }
  effect_free(break_floor_variants);
  break_floor_variants = 0;
  return;
}

void oneshot_anim_pool_alloc(void)
{
  oneshot_anim *slot;
  uint i;
  
  slot = effect_malloc(0x3c0);
  oneshot_anim_pool = (void *)slot;
  i = 0;
  do {
    slot->flags = 0;
    i = 1 + i;
    slot = slot + 1;
  } while (i < 0x14);
  return;
}

int oneshot_anim_spawn(uint *def, uint *pos, short turn)
{
  typedef struct { int w[3]; } v3;
  uint i;
  uint *p;
  
  p = (uint *)oneshot_anim_pool;
  effect_malloc_freed_check(oneshot_anim_pool);
  for (i = 0; i < 0x14; i++, p += 0xc) {
    if (p[6] == 0) {
      p[0] = def[0];
      p[1] = def[1];
      p[2] = def[2];
      p[3] = def[3];
      p[4] = def[4];
      p[5] = def[5];
      p[6] = def[6];
      p[7] = def[7];
      *(v3 *)(p + 8) = *(v3 *)pos;
      *(short *)(p + 0xb) = turn;
      return 0;
    }
  }
  return -1;
}

void oneshot_anim_draw_all(void)
{
  oneshot_anim *p;
  uint i;
  int *q;
  float s;
  
  p = (oneshot_anim *)oneshot_anim_pool;
  effect_malloc_freed_check(oneshot_anim_pool);
  mtx_push(0);
  for (i = 0; i < 0x14; i++, p++) {
    oneshot_anim *e = p;
    if (e->flags != 0) {
      q = e->ids;
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(&e->pos.x);
      if ((e->flags & 0x10) != 0) {
        mtx_rot_y_s16(e->turn);
        mtx_translate_by_vec3(&e->offset.x);
        mtx_rot_zyx_s16(e->rot[0],e->rot[1],e->rot[2]);
      }
      else {
        mtx_translate_by_vec3(&e->offset.x);
        if ((e->flags & 4) != 0) {
          mtx_rot_y_s16(camera.yaw);
        }
        if ((e->flags & 8) != 0) {
          mtx_rot_x_s16(camera.pitch);
        }
        if ((e->flags & 0x20) != 0) {
          mtx_rot_y_s16(0x8000);
        }
      }
      s = e->scale;
      mtx_scale_xyz(s,s,s);
      obj_draw(*q);
      if (pause_frame_held() == 0) {
        q++;
        if (*q < 0) {
          e->flags = 0;
        }
        else {
          e->ids = q;
        }
      }
    }
  }
  mtx_pop(1);
}

void break_floor_fall_anim_step(void)
{
  stage_anim_objs_step(2,4);
  return;
}

void break_floor_fall_draw(void)
{
  break_piece *piece;
  stage_anim_rec *e;
  int *list;
  float *m;
  int hide;
  float d;
  float fsz;
  float t;
  float h;
  float v[3];
  short k;
  short f;
  
  for (break_fall_list_end = break_fall_list; *break_fall_list_end != 0; break_fall_list_end++) {
    piece = (break_piece *)*break_fall_list_end;
    k = piece->idx;
    e = &stage_anim_obj2[k];
    if ((e->flags & 2) != 0) {
      list = break_fall_piece_lists[k];
      m = e->mtx;
      mtx_push(0);
      mtx_translate_by_vec3(&piece->pos.x);
      mtx_rot_y_s16(piece->turn2);
      mtx_push(0);
      m += 16;
      while (*list != -1) {
        d = e->frame_time;
        fsz = e->anim_frames;
        hide = 0;
        mtx_peek_pushed(0);
        mtx_mul_by(m);
        mtx_push_identity();
        mtx_translate_by_vec3(&piece->pos.x);
        mtx_rot_y_s16(piece->turn2);
        mtx_mul_by(m);
        mtx_store_translation(v);
        if (v[0] > 6.0 || v[0] < -6.0 || v[2] > 6.0 || v[2] < -6.0) {
          hide = 1;
        }
        mtx_pop(1);
        m += 16;
        if (hide) list++;
        else {
          t = 0.5 * fsz;
          if (d > t && (game_flags & 0x2000) == 0) {
            h = (d - t) / t;
            if (h < 1.0) {
              extra_light_parallel_set(h);
              obj_draw_translucent(*list++);
              extra_light_off();
            }
            else list++;
          }
          else obj_draw(*list++);
        }
      }
      mtx_pop(2);
      f = e->flags;
      if ((f & 4) != 0) {
        if ((f & 8) != 0) e->flags = f & 0xfffe;
        else e->flags = e->flags & 0xfffc;
      }
    }
  }
}

void oneshot_anim_pool_free(void)
{
  effect_free(oneshot_anim_pool);
  oneshot_anim_pool = 0;
  return;
}

void fence_b_free(void)
{
  oneshot_anim_pool_free();
  stage_anim_buffer_free();
  return;
}

void break_floor_rob_landed(rob_rec *rob)
{
  typedef struct { float x; float y; float z; } vec;
  float lim = 5.0;
  float step = 0.5;
  vec v;
  int ix;
  int iy;
  int s;
  
  if (stage_cur_no == 4) {
    v = *(vec *)&rob->pos;
    if (v.x <= lim && -lim <= v.x && v.z <= lim && -lim <= v.z) {
      stage_break_flags = stage_break_flags | 1;
      ix = (v.x + lim) / step;
      s = (short)ix * 0x14;
      iy = (v.z + lim) / step;
      break_floor_request_tile = (short)iy + s;
      break_floor_request_x = (char)ix;
      break_floor_request_z = (char)iy;
      *(vec *)&break_floor_impact = *(vec *)&rob->pos;
    }
  }
}

void break_floor_request_apply(void)
{
  uint bits;
  
  bits = lcg_next15();
  break_fall_list_clear();
  break_floor_tile_hit((int)((short)break_floor_request_tile),1);
  if (break_floor_request_x != 0) {
    if ((bits & 1) != 0) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + -0x14),0);
    }
    if ((break_floor_request_z != 0) && ((bits & 2) != 0)) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + -0x15),0);
    }
    if ((break_floor_request_z < 0x13) && ((bits & 4) != 0)) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + -0x13),0);
    }
  }
  if (break_floor_request_x < 0x13) {
    if ((bits & 8) != 0) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + 0x14),0);
    }
    if ((break_floor_request_z != 0) && ((bits & 0x10) != 0)) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + 0x13),0);
    }
    if ((break_floor_request_z < 0x13) && ((bits & 0x20) != 0)) {
      break_floor_tile_hit((int)(short)(break_floor_request_tile + 0x15),0);
    }
  }
  if ((break_floor_request_z != 0) && ((bits & 0x40) != 0)) {
    break_floor_tile_hit((int)(short)(break_floor_request_tile + -1),0);
  }
  if ((break_floor_request_z < 0x13) && ((bits & 0x80) != 0)) {
    break_floor_tile_hit((int)(short)(break_floor_request_tile + 1),0);
  }
  return;
}

void break_floor_tile_hit(int tile_no, int center)
{
  short id = tile_no;
  uchar mode = center;
  break_piece *e = break_floor_tiles;
  int *obj;
  short zero = 0;
  float dx;
  float dz;
  
  if ((uint)id > 399) return;
  if (break_floor_hits > 9) return;
  if (break_floor_queue_len > 19) return;
  break_floor_hits = break_floor_hits + 1;
  e = e + id;
  obj = e->anims;
  if ((e->flags & 2) != 0) {
    if (obj != 0) {
      if (mode != 0) goto done;
      obj++;
      dx = e->pos.x - break_floor_impact.x;
      dz = e->pos.z - break_floor_impact.z;
      oneshot_anim_spawn((uint *)*obj,(uint *)e,atan2_s16(dx,dz));
    }
    if (mode == 0) break_floor_tile_fall_start(e);
  done:
    e->flags = 4;
    break_floor_tile_hide(id);
    return;
  }
  if ((e->flags & 4) != 0) return;
  if (obj != 0) {
    e->turn2 = zero;
    if (mode == 0) {
      obj++;
      dx = e->pos.x - break_floor_impact.x;
      dz = e->pos.z - break_floor_impact.z;
      oneshot_anim_spawn((uint *)*obj,(uint *)e,atan2_s16(dx,dz));
    }
  }
  e->flags = 2;
  break_floor_tile_hide(id);
  break_floor_queue[break_floor_queue_len] = id;
  break_floor_queue_len = break_floor_queue_len + 1;
}

void break_fall_list_clear(void)
{
  int i;
  
  i = 0;
  do {
    break_fall_list[i] = 0;
    i = i + 1;
  } while (i < 9);
  break_fall_list_end = break_fall_list;
  return;
}

void break_floor_tile_fall_start(break_piece *piece)
{
  typedef struct { int w[3]; } v3;
  uchar **q;
  stage_anim_rec *e;
  short k;
  float x, z;

  if (piece->idx >= 0) {
    q = break_fall_list_end;
    *q = (void *)piece;
    break_fall_list_end = q + 1;
    x = piece->pos.x - break_floor_impact.x;
    z = piece->pos.z - break_floor_impact.z;
    piece->turn2 = atan2_s16(x, z);
    k = 0x30;
    e = (stage_anim_rec *)((char *)stage_anim_obj2 + piece->idx * k);
    *(v3 *)e->pos = *(v3 *)&piece->pos;
    e->frame_time = 0.0;
    e->flags = 3;
    e->turn = piece->turn;
  }
}

void break_floor_model_build(void)
{
  int nmax = 400;
  int a;
  int n;
  int i;
  int total;
  int cnt;
  char *e;
  int *ent;
  int j;
  int k;
  int m;
  int lim;
  int nj;
  int q;
  int t;
  int u;
  int *s;
  int *d;
  short *rot;
  float base[3];
  float in[3];
  float out[3];

  d = (int *)obj_model_get(0x180084);
  if (d == 0) {
    break_floor_model = 0;
    return;
  }
  n = 42000;
  break_floor_model = elan_heap_alloc(n * 24 + 0xc0);
  if (break_floor_model == 0) return;
  for (i = 0; i < 0x30; i++) {
    ((int *)break_floor_model)[i] = d[i];
  }
  *(float *)(break_floor_model + 0x14) = 20.0f;
  *(int *)(break_floor_model + 0x18) = n * 24 + 0xc0;
  *(int *)(break_floor_model + 0x24) = n;
  *(int *)(break_floor_model + 0x28) = 1;
  *(int *)(break_floor_model + 0x2c) = 1;
  *(int *)(break_floor_model + 0x44) = n * 24 + 0x60;
  *(int *)(break_floor_model + 0xbc) = n;
  *(uint *)(break_floor_model + 0xa4) &= 0xe7ffffff;
  *(uint *)(break_floor_model + 0xa4) |= 0x08000000;
  total = 0;
  e = (char *)break_floor_tiles;
  ent = &break_floor_tile_info[0][0];
  d = (int *)(break_floor_model + 0xc0);
  mtx_push(0);
  for (i = 0; i < nmax; ) {
    a = obj_model_get(*(int *)(e + 0x10));
    s = (int *)obj_model_get(*(int *)(e + 0x14));
    if (a == 0 || s == 0) {
      for (;;) ;
    }
    t = *(int *)(a + 0x20);
    cnt = *(int *)(a + 0x24);
    lim = s[8];
    k = s[9];
    {
      int nv = t - cnt;
      lim -= k;
      u = nv + 103;
      ent[2] = lim - nv;
    }
    ent[0] = cnt;
    ent[1] = k;
    ent[3] = a;
    ent[4] = (int)s;
    total += u;
    t = 0;
    if (*(int *)(e + 0x14) == 0x180081) t = 1;
    if (*(int *)(e + 0x14) == 0x180082) t = 2;
    if (*(int *)(e + 0x14) == 0x180083) t = 3;
    rot = (short *)(e + 0xc);
    ent[5] = t + ((*rot >> 14) & 3) * 4;
    mtx_load_identity();
    mtx_rot_y_s16(*rot);
    q = i / 20;
    base[0] = (float)q * 0.5f + -4.75f;
    base[2] = (float)(i - q * 20) * 0.5f + -4.75f;
    base[1] = 0.55f;
    k = 0;
    s = (int *)(a + 0xbc);
    for (; k < cnt; ) {
      m = *s++;
      k += m;
      for (j = 0; j < m; j++) {
        *d++ = *s++;
        in[0] = *(float *)s++;
        in[1] = *(float *)s++;
        in[2] = *(float *)s++;
        mtx_xform_point(in, out);
        *(float *)d++ = out[0] + base[0];
        *(float *)d++ = out[1] + base[1];
        *(float *)d++ = out[2] + base[2];
        *d++ = *s++;
        *d++ = *s++;
      }
      if (*s == 0x08000500) s += 23;
      else s += 7;
    }
    nj = 105 - cnt;
    for (j = 0; j < nj; j++) {
      switch (j) {
      case 0:
      case 1:
        *d = 0;
        d++;
        break;
      case 2:
        *d = 0x60000000;
        d++;
        break;
      default:
        *d = (j == nj - 1) ? 0xa0000000 : 0x20000000;
        d++;
        break;
      }
      *(float *)d++ = (float)(j / 2) * 0.1f;
      *(float *)d++ = -50.0f;
      *(float *)d++ = (float)(j % 2) * 0.1f;
      *d++ = 0;
      *d++ = 0;
    }
    i++;
    e += 0x24;
    ent += 6;
  }
  *(int *)(break_floor_model + 0x20) = total;
  mtx_pop(1);
}

void break_floor_tile_crack_write(uint tile)
{
  typedef struct { int a; int n; int add; int c; int on; int src; } grp_ent;
  float off[3];
  grp_ent *e;
  int *d;
  int *s;
  int q;
  int k;
  int n;
  int m;
  int x;

  e = &((grp_ent *)&break_floor_tile_info[0][0])[tile];
  m = e->on;
  if (m != 0) {
    d = (int *)(break_floor_model + tile * 0x9d8 + 0xc0);
    q = (int)tile / 20;
    off[0] = (float)q * 0.5 + -4.75;
    off[2] = (float)(int)(tile - q * 20) * 0.5 + -4.75;
    off[1] = 0.55;
    n = e->n;
    s = (int *)(e->src * 0x9d8 + break_floor_variants);
    for (k = 0; k < n; k++) {
      *d++ = *s++;
      *(float *)d++ = *(float *)s++ + off[0];
      *(float *)d++ = *(float *)s++ + off[1];
      *(float *)d++ = *(float *)s++ + off[2];
      *d++ = *s++;
      *d++ = *s++;
    }
    m = 105 - n;
    for (k = 0; k < m; k++) {
      switch (k) {
      case 0:
      case 1:
        *d = 0;
        break;
      case 2:
        *d = 0x60000000;
        break;
      default:
        *d = k == m - 1 ? 0xa0000000 : 0x20000000;
        break;
      }
      d++;
      x = k / 2;
      *(float *)d++ = (float)x * 0.1;
      *(float *)d++ = -50.0;
      *(float *)d++ = (float)(k - x * 2) * 0.1;
      *d++ = 0;
      *d++ = 0;
    }
    *(int *)(break_floor_model + 0x20) = *(int *)(break_floor_model + 0x20) + e->add;
  }
}

void break_floor_tile_hide(int tile)
{
  char *vtx;
  int i;
  
  vtx = break_floor_model + tile * 0x9d8 + 0xc0;
  for (i = 0; i < 0x69; i++) {
    *(float *)(vtx + 8) = -50.0f;
    vtx = vtx + 0x18;
  }
}

void break_floor_variants_build(void)
{
  float in[3];
  int out[3];
  int ids[4];
  int i;
  int j;
  int n;
  int total;
  int cnt;
  int k;
  int *src;
  int *dst;
  ids[0] = 0x180080;
  ids[1] = 0x180081;
  ids[2] = 0x180082;
  ids[3] = 0x180083;
  mtx_push(0);
  mtx_load_identity();
  for (i = 0; i < 4; i++) {
    if (i > 0) mtx_rot_y_s16(0x4000);
    for (j = 0; j < 4; j++) {
      src = (int *)obj_model_get(ids[j]);
      if (src != 0) {
        n = src[9];
        total = 0;
        src = (int *)((char *)src + 0xbc);
        dst = (int *)(break_floor_variants + (i * 4 + j) * 0x9d8);
        while (total < n) {
          cnt = *src++;
          total += cnt;
          for (k = 0; k < cnt; k++) {
            in[0] = ((float *)src)[1];
            in[1] = ((float *)src)[2];
            in[2] = ((float *)src)[3];
            mtx_xform_point(in, (float *)out);
            *dst++ = src[0];
            *dst++ = out[0];
            *dst++ = out[1];
            *dst++ = out[2];
            *dst++ = src[4];
            *dst++ = src[5];
            src += 6;
          }
          if (*src == 0x8000500) src += 0x17;
          else src += 7;
        }
      }
    }
  }
  mtx_pop(1);
}

int break_floor_hit_frames_get(void)
{
  return break_floor_hit_frames;
}

void break_floor_draw_tail(void)
{
  return;
}

void audience_init(void)
{
  audience_anim_set *a;
  int i;
  int j;
  int *buf;
  audience_member *s;
  audience_place *src;
  
  a = audience_anim_sets;
  for (i = 0; i < 21; i++, a++) {
    buf = effect_malloc(a->frames << 2);
    if (buf == 0) {
      __asm__ volatile ("trapa #0x50");
    }
    a->objs = buf;
    for (j = 0; j < a->frames; j++) {
      buf[j] = (int)obj_model_get(j + a->first_obj);
    }
  }
  audience_members = effect_malloc(0x1440);
  if (audience_members == 0) {
    __asm__ volatile ("trapa #0x50");
  }
  s = audience_members;
  src = audience_places_0a;
  for (i = 0; i < 76; i++, s++, src++) {
    s->type = 0;
    s->set = 0;
    s->place = src;
    s->scale = 1.5f;
  }
  src = audience_places_0b;
  for (i = 0; i < 10; i++, s++, src++) {
    s->type = 0;
    s->set = 0;
    s->place = src;
    s->scale = 1.1f;
  }
  src = audience_places_1a;
  for (i = 0; i < 20; i++, s++, src++) {
    s->type = 1;
    s->place = src;
    audience_member_reset(s);
  }
  src = audience_places_1b;
  for (i = 0; i < 16; i++, s++, src++) {
    s->type = 1;
    s->place = src;
    audience_member_reset(s);
  }
  src = audience_places_2a;
  for (i = 0; i < 24; i++, s++, src++) {
    s->type = 2;
    s->place = src;
    audience_member_reset(s);
  }
  src = audience_places_2b;
  for (i = 0; i < 16; i++, s++, src++) {
    s->type = 2;
    s->place = src;
    audience_member_reset(s);
  }
  s = audience_members;
  for (i = 0; i < 162; i++, s++) {
    s->anim = audience_anim_sets + s->set;
    s->frame = (int)lcg_next15() % s->anim->frames;
    s->frame_pos = s->frame;
    s->rate_ofs = lcg_next_unit() * 0.1f;
  }
  audience_mood_set(0);
}

void audience_member_reset(audience_member *member)
{
  member->reaction = '\0';
  member->scale = (float)lcg_next_unit() * 0.2 + 1.4;
  switch (member->type) {
  case 1:
    member->set = '\b';
    member->up_list = &audience_up_list_1;
    member->down_list = &audience_down_list_1;
    break;
  case 2:
    member->set = '\x12';
    member->up_list = &audience_up_list_2;
    member->down_list = &audience_down_list_2;
    break;
  }
}

void audience_free(void)
{
  audience_anim_set *set;
  int i;

  set = audience_anim_sets;
  i = 0;
  do {
    effect_free(set->objs);
    set->objs = 0;
    i = i + 1;
    set = set + 1;
  } while (i < 0x15);
  effect_free(audience_members);
  audience_members = 0;
  return;
}

void audience_exec(void)
{
  int mx;
  rob_rec *rob;
  audience_member *m;
  uint *q;
  int i;
  short v;
  char *d;

  mx = 0;
  rob = rob_get(0);
  m = audience_members;
  q = (uint *)sound_state_get();
  if (mode_get_main() == 1 && mode_get_sub() == 8) return;
  if (mode_get_main() != 3 || mode_get_sub() == 10) {
    for (i = 0; i < 2; i++, rob++) {
      if (rob->combo[0].hits > mx) mx = rob->combo[0].hits;
    }
    if (mx > 3 && (audience_flags & 4) != 0) {
      if ((*q & 0x2000) != 0) sound_play_id(0x3a4, 0);
      audience_flags &= 0xfb;
    }
    if (mx > 4) {
      audience_flags |= 8;
      audience_mood_target = 50;
      audience_rate_target = 1.4;
      audience_hold = 120;
    }
    else if (mx == 4) {
      audience_mood_target = 50;
      audience_rate_target = 1.4;
      audience_hold = 120;
    }
    else if (mx == 3) {
      audience_mood_target = 50;
      audience_rate_target = 1.2;
      audience_hold = 60;
    }
    else if (audience_hold == 0) {
      audience_flags &= 0xf7;
      audience_mood_target = 25;
      audience_rate_target = 1.0;
    }
    v = audience_volume_base + audience_volume_step * (int)mx;
    if (v > 0x7f) v = 0x7f;
    audience_volume_target = v;
  }
  if (--audience_hold < 0) {
    audience_hold = 0;
    audience_flags |= 4;
  }
  if ((game_frame_count & 1) == 0) {
    if (audience_volume > audience_volume_target) audience_volume--;
    else if (audience_volume < audience_volume_target) audience_volume++;
    if ((game_frame_count & 3) == 0) {
      if (audience_mood < audience_mood_target) audience_mood++;
      else if (audience_hold == 0 && audience_mood > audience_mood_target) audience_mood--;
      if (audience_rate < audience_rate_target) audience_rate = audience_rate + 0.01;
      else if (audience_hold == 0 && audience_rate > audience_rate_target) audience_rate = audience_rate - 0.01;
    }
  }
  for (i = 0; i < 0xa2; i++, m++) {
    m->frame_pos += (float)*(d = (char *)m->anim) * (audience_rate + m->rate_ofs);
    m->frame = (char)(int)m->frame_pos;
    if (m->frame >= *(short *)(d + 2) || m->frame < 0) {
      audience_anim_next(m);
    }
  }
  audience_exec_tail();
}

void audience_mood_set(char mood)
{
  uint *p;
  
  p = (uint *)sound_state_get();
  if (stage_cur_no != 0x16) {
    return;
  }
  audience_hold = '\0';
  switch(mood) {
  case 0:
    audience_mood_target = '\x19';
    audience_mood = '\x19';
    audience_rate_target = 1.0;
    audience_rate = 1.0;
    audience_volume = audience_volume_base + audience_volume_step;
    audience_volume_target = audience_volume;
    audience_flags = audience_flags & 0xf3;
    break;
  case 1:
    audience_mood_target = '2';
    audience_mood = '2';
    audience_rate_target = 1.0;
    audience_rate = 1.0;
    audience_volume = audience_volume_base + audience_volume_step * '\x02';
    audience_volume_target = audience_volume;
    audience_flags = audience_flags & 0xf3;
    break;
  case 2:
    audience_mood_target = '2';
    audience_mood = '2';
    audience_rate_target = 1.2;
    audience_rate = 1.2;
    audience_volume = audience_volume_base + audience_volume_step * '\x02';
    audience_volume_target = audience_volume;
    if ((*p & 0x2000) != 0) {
      sound_play_id(0x3a4,0);
    }
    audience_flags = (audience_flags | 8) & 0xfb;
    break;
  case 3:
    audience_mood_target = -0x32;
    audience_mood = -0x32;
    audience_rate_target = 1.0;
    audience_rate = 1.0;
    if ((*p & 0x2000) != 0) {
      sound_play_id(0x3a5,0);
    }
    audience_volume = audience_volume_base + audience_volume_step;
    audience_volume_target = audience_volume;
    audience_flags = audience_flags & 0xf3;
    break;
  }
}

void audience_exec_tail(void)
{
  return;
}

void audience_draw(void)
{
  int i;
  audience_member *m;
  uint v;
  float f;
  list_block_ref buf;

  if (mode_get_main() != MODE_MAIN_ADVERTISE || mode_get_sub() != 8) {
    mtx_push(0);
    v = elan_write_ptr_get();
    buf.addr = v;
    *(volatile uint *)0xff000038 = v >> 0x18;
    *(volatile uint *)0xff00003c = v >> 0x18;
    v = v & 0x3ffffff | 0xe0000000;
    m = audience_members;
    for (i = 0; i < 0xa2; i++, m++) {
      if ((ushort)((camera.yaw - m->place->angle) + 0x3fffU) < 0x7fff) {
        mtx_peek_pushed(0);
        mtx_translate_by_vec3(&m->place->pos.x);
        mtx_rot_y_s16(m->place->angle);
        f = m->scale;
        mtx_scale_xyz(f, f, f);
        v = model_matrix_cmds_write_at(v);
        v = model_part0_call_write_at(v, (compiled_model_rec *)m->anim->objs[m->frame]);
      }
    }
    v += 0x20;
    v = buf.addr & 0xfc000000 | v & 0x3ffffff;
    buf.length = v - buf.addr;
    elan_block_queue_relay(0, &buf);
    elan_write_ptr_set(v);
    mtx_pop(1);
  }
}

void audience_anim_next(audience_member *member)
{
  audience_anim_set *q;
  char c;

  switch (member->type) {
  case 0:
    member->set = 0;
    break;
  case 1:
    switch (member->set) {
    case 1: case 2: case 3: case 4: case 6: case 10:
      audience_reaction_pick(member);
      c = member->reaction;
      if (c != 0) {
        if (c == 3) {
          member->set = 1;
        } else {
          audience_anim_pick(member);
        }
      } else {
        member->set = 7;
      }
      break;
    case 5:
      member->set = 6;
      break;
    case 8:
      member->set = 9;
      break;
    case 7: case 9:
      audience_reaction_pick(member);
      if (member->reaction == 0) {
        member->set = 8;
      } else {
        member->set = 10;
      }
      break;
    }
    break;
  case 2:
    switch (member->set) {
    case 11: case 12: case 13: case 14: case 15: case 16: case 20:
      audience_reaction_pick(member);
      c = member->reaction;
      if (c != 0) {
        if (c == 3) {
          member->set = 14;
        } else {
          audience_anim_pick(member);
        }
      } else {
        member->set = 17;
      }
      break;
    case 18:
      member->set = 19;
      break;
    case 17: case 19:
      audience_reaction_pick(member);
      if (member->reaction == 0) {
        member->set = 18;
      } else {
        member->set = 20;
      }
      break;
    }
    break;
  }
  q = audience_anim_sets + member->set;
  member->anim = q;
  if (q->step > 0) {
    member->frame = 0;
  } else {
    member->frame = *(char *)&member->anim->frames - 1;
  }
  member->frame_pos = (float)member->frame;
}

void audience_reaction_pick(audience_member *member)
{
  char a;
  char *c;
  if ((audience_flags & 8) != 0) {
    member->reaction = 3;
  }
  else if (audience_mood == 0) {
    member->reaction = 0;
  }
  else {
    c = &audience_mood;
    a = abs(*c);
    if ((char)((int)lcg_next15() % 50) < a) {
      if (*c > 0) member->reaction = 1;
      else member->reaction = 2;
    }
    else member->reaction = 0;
  }
}

void audience_anim_pick(audience_member *member)
{
  audience_react_list *list;
  char c;
  char v;
  char *q;
  char i;

  switch (member->reaction) {
  case 0:
    list = 0;
    break;
  case 1:
    list = member->up_list;
    break;
  case 2:
    list = member->down_list;
    break;
  default:
    list = 0;
    break;
  }
  if (list != 0) {
    c = member->anim->no_repeat_set;
    i = (char)((int)lcg_next15() % list->count);
    q = &member->set;
    *q = list->sets[i];
    v = *q;
    if (v == c) {
      *q = (*list->sets == v) ? list->sets[1] : *list->sets;
    }
  }
}

void bird_init(void)
{
  short v;
  
  perf_color_mark(0xc0c020);
  bird_flock = effect_malloc(0x1db0);
  if (bird_flock == 0) {
    __asm__ volatile ("trapa #0x50");
  }
  bird_paths = (bird_path_member *)effect_malloc(0x1c20);
  if (bird_paths == (bird_path_member *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  bird_init_word = 0;
  if (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 7) {
    v = 1000;
  }
  else {
    v = 2;
  }
  bird_density_div = v;
  bird_model_sets_load(bird_model_sets,4);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_path_spawn();
  bird_flock_spawn(bird_flock_defs,bird_model_sets);
  mtx_pop(1);
  bird_exec();
  perf_color_mark(0);
  return;
}

void bird_exec(void)
{
  perf_color_mark(0xc0c020);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_path_update();
  bird_flock_update();
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void bird_draw(void)
{
  perf_color_mark(0xc0c020);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_path_draw();
  bird_flock_draw();
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void bird_free(void)
{
  perf_color_mark(0xc0c020);
  effect_free(bird_flock);
  bird_flock = 0;
  effect_free(bird_paths);
  bird_paths = (bird_path_member *)0x0;
  bird_model_sets_free(bird_model_sets,4);
  perf_color_mark(0);
  return;
}

void bird_flock_only_init(void)
{
  perf_color_mark(0xc0c020);
  bird_flock = effect_malloc(0x1db0);
  if (bird_flock == 0) {
    __asm__ volatile ("trapa #0x50");
  }
  bird_density_div = 1;
  bird_init_word = 0;
  bird_spare_box.max_z = -50.0;
  bird_spare_box.min_z = -70.0;
  bird_model_sets_load(bird_flock_only_sets,1);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_flock_spawn(bird_flock_only_defs,bird_flock_only_sets);
  mtx_pop(1);
  bird_flock_only_exec();
  perf_color_mark(0);
  return;
}

void bird_flock_only_exec(void)
{
  perf_color_mark(0xc0c020);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_flock_update();
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void bird_flock_only_draw(void)
{
  perf_color_mark(0xc0c020);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  bird_flock_draw();
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void bird_flock_only_free(void)
{
  perf_color_mark(0xc0c020);
  effect_free(bird_flock);
  bird_flock = 0;
  bird_model_sets_free(bird_flock_only_sets,1);
  perf_color_mark(0);
  return;
}

void fish_init(void)
{
  perf_color_mark(0xc0c020);
  fish_school = effect_malloc(0x41a0);
  if (fish_school == 0) {
    __asm__ volatile ("trapa #0x50");
  }
  fish_paths = effect_malloc(12000);
  if (fish_paths == 0) {
    __asm__ volatile ("trapa #0x50");
  }
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  fish_path_spawn();
  fish_solo1_init();
  fish_solo2_init();
  fish_solo3_init();
  fish_school_spawn();
  fish_seeker_init();
  mtx_pop(1);
  light_set_load(2,6);
  fish_exec();
  perf_color_mark(0);
  return;
}

void fish_exec(void)
{
  e_mode_main main_mode;
  int sub_mode;
  
  perf_color_mark(0xc0c020);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  main_mode = mode_get_main();
  if ((main_mode != MODE_MAIN_GAME) || (sub_mode = mode_get_sub(), sub_mode != 9)) {
    fish_path_update();
    fish_solo1_update();
    fish_solo2_update();
    fish_solo3_update();
  }
  fish_school_update();
  main_mode = mode_get_main();
  if ((main_mode != MODE_MAIN_GAME) || (sub_mode = mode_get_sub(), sub_mode != 9)) {
    fish_seeker_update();
  }
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void fish_draw(void)
{
  e_mode_main main_mode;
  int sub_mode;
  
  perf_color_mark(0xc0c020);
  light_set_send(0,2);
  mtx_push(0);
  mtx_store(stage_fx_base_mtx);
  main_mode = mode_get_main();
  if ((main_mode != MODE_MAIN_GAME) || (sub_mode = mode_get_sub(), sub_mode != 9)) {
    fish_solo1_draw();
    fish_path_draw();
    fish_solo2_draw();
    fish_solo3_draw();
  }
  fish_school_draw();
  main_mode = mode_get_main();
  if ((main_mode != MODE_MAIN_GAME) || (sub_mode = mode_get_sub(), sub_mode != 9)) {
    fish_seeker_draw();
  }
  mtx_pop(1);
  perf_color_mark(0xffff00);
  return;
}

void fish_free(void)
{
  perf_color_mark(0xc0c020);
  effect_free(fish_school);
  fish_school = 0;
  effect_free(fish_paths);
  fish_paths = 0;
  fish_model_sets_free();
  mot_paths_free();
  perf_color_mark(0);
  return;
}

void bird_flock_spawn(bird_flock_def *def, bird_model_set *sets)
{
  bird_flock_member *bird;
  int left;
  bird_model_set *set;
  int n;
  bird = bird_flock;
  left = 100;
  do {
    if (def->count[0] < 0) break;
    set = &sets[def->prm->set];
    n = def->count[0] / bird_density_div;
    while (n--) {
      bird->flags = 3;
      bird_flock_member_init(bird,def);
      bird->models = set->models;
      bird->frames = set->frames;
      bird = bird + 1;
      left--;
    }
    n = def->count[1];
    n = n / bird_density_div;
    while (n--) {
      bird->flags = 3;
      bird_flock_member_init(bird,def);
      bird->models = set->clump4_models;
      bird->frames = set->frames;
      bird = bird + 1;
      left--;
    }
    n = def->count[2] / bird_density_div;
    while (n--) {
      bird->flags = 3;
      bird_flock_member_init(bird,def);
      bird->models = set->clump8_models;
      bird->frames = set->frames;
      bird = bird + 1;
      left--;
    }
    def = def + 1;
  } while (0 < left);
  bird_flock_count = 100 - (short)left;
  while (left--) {
    bird->flags = 0;
    bird = bird + 1;
  }
}

void bird_flock_member_init(bird_flock_member *bird, bird_flock_def *def)
{
  typedef struct { int w[3]; } vec3_blk;
  bird_flock_prm *prm;
  float *box;
  prm = def->prm;
  bird->prm = prm;
  box = def->box;
  bird->box = box;
  bird->frame_pos = 0.0;
  bird->frame = 0;
  bird->rate = 1.0;
  bird->wait = 0x3c;
  bird->speed = prm->speed;
  bird->turn = prm->turn_min;
  bird->pos.x = lcg_next_unit() * (box[0] - box[3]) + box[3];
  bird->pos.y = lcg_next_unit() * (box[1] - box[4]) + box[4];
  bird->pos.z = lcg_next_unit() * (box[2] - box[5]) + box[5];
  bird->yaw = (int)lcg_next15() % (def->yaw_range * 2) - *(short *)&def->yaw_range;
  bird->dir.x = sin_s16(bird->yaw);
  bird->dir.z = cos_s16(bird->yaw);
  bird->pitch = (int)lcg_next15() % (def->pitch_range * 2) - *(short *)&def->pitch_range;
  bird->dir.y = sin_s16(bird->pitch);
  vec3_normalize_nolen(&bird->dir.x);
  *(vec3_blk *)&bird->want = *(vec3_blk *)&bird->dir;
}

void bird_flock_update(void)
{
  typedef struct { float x, y, z; } v3;
  int i;
  bird_flock_member *bird;
  bird_flock_prm *prm;
  float *box;
  short *wait;
  short *yaw;
  short ang;
  short a;
  float d;
  v3 v;
  bird = bird_flock;
  for (i = 0; i < bird_flock_count; i++, bird++) {
    prm = bird->prm;
    box = bird->box;
    bird->frame_pos = bird->frame_pos + bird->rate;
    bird->frame = bird->frame_pos;
    if (bird->frame >= bird->frames) {
      bird->frame_pos = 0.0;
      bird->frame = 0;
    }
    if ((bird->want.x > 0.0 && bird->pos.x >= box[0]) ||
        (bird->want.x < 0.0 && bird->pos.x <= box[3])) {
      bird->want.x = -bird->want.x;
      bird->wait = bird->wait + 60;
    }
    if ((bird->want.y > 0.0 && bird->pos.y >= box[1]) ||
        (bird->want.y < 0.0 && bird->pos.y <= box[4])) {
      bird->want.y = -bird->want.y;
      bird->wait = bird->wait + 60;
    }
    if ((bird->want.z > 0.0 && bird->pos.z >= box[2]) ||
        (bird->want.z < 0.0 && bird->pos.z <= box[5])) {
      bird->want.z = -bird->want.z;
      bird->wait = bird->wait + 60;
    }
    d = vec3_dot(&bird->want.x,&bird->dir.x);
    if (d < 0.8660254) {
      if (bird->turn < prm->turn_max) bird->turn = prm->turn_step + bird->turn;
    }
    else if (bird->turn > prm->turn_min) bird->turn = bird->turn - prm->turn_step;
    vec3_cross(&bird->want.x,&bird->dir.x,&v.x);
    if (v.y >= 0.0) ang = -0x4000;
    else ang = 0x4000;
    yaw = &bird->yaw;
    ang = *yaw + ang;
    v.x = sin_s16(ang);
    v.z = cos_s16(ang);
    v.y = bird->want.y - bird->dir.y;
    v.y = v.y + v.y;
    vec3_normalize_nolen(&v.x);
    vec3_scale(&v.x,&v.x,bird->turn);
    vec3_add_to(&v.x,&bird->dir.x);
    vec3_normalize_nolen(&v.x);
    *(v3 *)&bird->dir = v;
    vec3_scale(&v.x,&v.x,bird->speed);
    vec3_add_to(&bird->pos.x,&v.x);
    bird->rate = (float)(-bird->pitch / 0x300) * 0.1 + 1.0;
    wait = &bird->wait;
    if (--*wait < 1) {
      ang = (int)lcg_next15() % (prm->yaw_range * 2) - prm->yaw_range + *yaw;
      v.x = sin_s16(ang);
      v.z = cos_s16(ang);
      ang = (int)lcg_next15() % (prm->pitch_range * 2) - prm->pitch_range;
      v.y = sin_s16(ang);
      vec3_normalize_nolen(&v.x);
      *(v3 *)&bird->want = v;
      *wait = (int)lcg_next15() % 200 + 60;
    }
    dir_to_pitch_yaw(&bird->dir.x,&bird->pitch,&bird->yaw);
  }
}

void bird_flock_draw(void)
{
  int i;
  bird_flock_member *bird;
  bird = bird_flock;
  for (i = 0; i < bird_flock_count; i++, bird++) {
    mtx_set_current(stage_fx_base_mtx);
    mtx_translate_by_vec3(&bird->pos.x);
    mtx_rot_y_s16(bird->yaw);
    mtx_rot_x_s16(bird->pitch);
    model_draw((compiled_model_rec *)bird->models[bird->frame]);
  }
}

void bird_path_spawn(void)
{
  bird_path_member *bird;
  int left;
  bird_path_def *def;
  bird_model_set *set;
  int n;

  bird = bird_paths;
  def = bird_path_defs;
  left = 100;
  do {
    bird->flags = 0;
    if (def->set < 0) break;
    set = &bird_model_sets[def->set];
    n = def->count[0] / bird_density_div;
    while (n--) {
      bird_path_member_init(bird,def);
      bird->frames = set->frames;
      bird->models = set->models;
      bird++;
      left--;
    }
    n = def->count[1] / bird_density_div;
    while (n--) {
      bird_path_member_init(bird,def);
      bird->frames = set->frames;
      bird->models = set->clump4_models;
      bird++;
      left--;
    }
    n = def->count[2] / bird_density_div;
    while (n--) {
      bird_path_member_init(bird,def);
      bird->frames = set->frames;
      bird->models = set->clump8_models;
      bird++;
      left--;
    }
    def++;
  } while (0 < left);
  bird_path_count = 100 - (short)left;
  while (left--) {
    bird->flags = 0;
    bird++;
  }
}

void bird_path_member_init(bird_path_member *bird, bird_path_def *def)
{
  typedef struct { float x; float y; float z; } vec;
  vec v;
  vec z = { 0.0, 0.0, 0.0 };
  bird_path_step *step;
  float b;
  float c;
  float t;

  bird->flags = 3;
  bird->frame_pos = -1.0;
  bird->rate = (float)((int)effect_rand15() % 20) * 0.01;
  bird->spread.x = (float)((int)effect_rand15() % def->spread[0]) * 0.1;
  bird->spread.y = (float)((int)effect_rand15() % def->spread[1]) * 0.1;
  bird->spread.z = c = (float)((int)effect_rand15() % def->spread[2]) * 0.1;
  bird->pos.x = def->pos.x + bird->spread.x;
  bird->pos.y = (t = def->pos.y) + (b = bird->spread.y);
  bird->pos.z = (t = def->pos.z) + c;
  bird->yaw = def->yaw;
  bird->prev_yaw = def->yaw;
  bird->pitch = def->pitch;
  bird->y_min = def->y_min + b;
  bird->y_max = (t = def->y_max) + b;
  bird->field_0xe = 0;
  bird->step_frame = 0;
  if (def->push != 0) {
    v = z;
    v.z = (float)((int)effect_rand15() % def->push) * 0.1;
    mtx_load_identity();
    mtx_rot_y_s16(bird->yaw);
    mtx_rot_x_s16(bird->pitch);
    mtx_xform_point((float *)&v,(float *)&v);
    bird->pos.x = bird->pos.x + v.x;
    bird->pos.y = bird->pos.y + v.y;
    bird->pos.z = bird->pos.z + v.z;
  }
  bird->step = def->script;
  bird->script_loop = def->script_loop;
  step = bird->step;
  if (step->op == -2) {
    bird->goal = step->goal + bird->spread.x;
  }
  else if (step->op == -3) {
    bird->goal = step->goal + bird->spread.z;
  }
}

void bird_path_update(void)
{
  typedef struct { float x, y, z; } vec;
  typedef struct { int w[3]; } blk;
  vec v;
  int zero[3];
  bird_path_member *bird;
  bird_path_step *step;
  int i;
  float *frame_pos;
  short *frame;
  float f;
  int done;

  bird = bird_paths;
  zero[0] = 0;
  zero[1] = 0;
  zero[2] = 0;
  for (i = 0; i < bird_path_count; i++, bird++) {
    step = bird->step;
    frame_pos = &bird->frame_pos;
    f = step->frame_rate + bird->rate + *frame_pos;
    *frame_pos = f;
    frame = &bird->frame;
    *frame = (short)(int)f;
    if (*frame >= bird->frames) {
      *frame_pos = 0.0f;
      *frame = 0;
    }
    bird->yaw = bird->yaw + step->yaw_rate;
    bird->pitch = bird->pitch + step->pitch_rate;
    if (bird->pitch < 0 && bird->pos.y > bird->y_max) {
      bird->pitch = 0;
    } else if (bird->pitch > 0 && bird->pos.y < bird->y_min) {
      bird->pitch = 0;
    }
    *(blk *)&v = *(blk *)zero;
    v.z = step->speed;
    mtx_load_identity();
    mtx_rot_y_s16(bird->yaw);
    mtx_rot_x_s16(bird->pitch);
    mtx_xform_point((float *)&v, (float *)&v);
    bird->pos.x = bird->pos.x + v.x;
    bird->pos.y = bird->pos.y + v.y;
    bird->pos.z = bird->pos.z + v.z;
    done = 0;
    switch (step->op) {
    case -2:
      if (v.x >= 0.0f) {
        float *q = &bird->pos.x;
        if (*q > bird->goal) done = 1;
      } else {
        float *q = &bird->pos.x;
        if (*q < bird->goal) done = 1;
      }
      break;
    case -3:
      if (v.z >= 0.0f) {
        float *q = &bird->pos.z;
        if (*q > bird->goal) done = 1;
      } else {
        float *q = &bird->pos.z;
        if (*q < bird->goal) done = 1;
      }
      break;
    case -4:
      if (step->yaw_rate > 0) {
        if (bird->yaw >= step->goal_yaw && bird->prev_yaw < step->goal_yaw) done = 1;
      } else {
        if (bird->yaw <= step->goal_yaw && bird->prev_yaw > step->goal_yaw) done = 1;
      }
      break;
    case -1:
      break;
    default:
      bird->step_frame = bird->step_frame + 1;
      if (bird->step_frame >= step->op) done = 1;
      break;
    }
    bird->prev_yaw = bird->yaw;
    if (done) {
      step++;
      bird->step_frame = 0;
      if (step->op == -1) step = bird->script_loop;
      if (step->op == -2) {
        bird->goal = step->goal + bird->spread.x;
      } else if (step->op == -3) {
        bird->goal = step->goal + bird->spread.z;
      }
      bird->step = step;
    }
  }
}

void bird_path_draw(void)
{
  bird_path_member *bird;
  int i;

  bird = bird_paths;
  for (i = 0; i < bird_path_count; i++, bird++) {
    mtx_set_current(stage_fx_base_mtx);
    mtx_translate_by_vec3(&bird->pos.x);
    mtx_rot_y_s16(bird->yaw);
    mtx_rot_x_s16(bird->pitch);
    model_draw((compiled_model_rec *)bird->models[bird->frame]);
  }
}

void bird_model_sets_load(bird_model_set *set, int count)
{
  int i;
  int j;
  int *models;

  for (i = 0; i < count; i++, set++) {
    if (set->flags & 2) {
      models = effect_malloc((int)set->frames << 2);
      if (models == 0) __asm__ volatile ("trapa #0x50");
      set->clump4_models = models;
      model_clump_frames_build(set->clump4_x_range,set->clump4_y_range,4,models,
                   set->first_obj,set->frames);
    }
    if (set->flags & 4) {
      models = effect_malloc((int)set->frames << 2);
      if (models == 0) __asm__ volatile ("trapa #0x50");
      set->clump8_models = models;
      model_clump_frames_build(set->clump8_x_range,set->clump8_y_range,8,models,
                   set->first_obj,set->frames);
    }
    if (set->flags & 1) {
      models = effect_malloc((int)set->frames << 2);
      if (models == 0) __asm__ volatile ("trapa #0x50");
      set->models = models;
      for (j = 0; j < set->frames; j++)
        { int b = (int)set->models; int *t = (int *)(j * 4 + b); *t = (int)obj_model_get(j + set->first_obj); }
    }
  }
}

void bird_model_sets_free(bird_model_set *set, int count)
{
  int i;

  for (i = 0; i < count; i = i + 1, set = set + 1) {
      if (set->clump4_models != 0) {
        model_clump_frames_free(set->clump4_models,(int)set->frames);
      }
      if (set->clump8_models != 0) {
        model_clump_frames_free(set->clump8_models,(int)set->frames);
      }
      effect_free(set->models);
      effect_free(set->clump4_models);
      effect_free(set->clump8_models);
      set->models = 0;
      set->clump4_models = 0;
      set->clump8_models = 0;
  }
  return;
}

void fish_seeker_init(void)
{

  fish_seeker.flags = 3;
  fish_seeker.prm = &fish_seeker_params;
  fish_seeker.state = 1;
  fish_seeker.field_0x2 = 0x3c;
  fish_seeker.field_0xc = 0.0;
  fish_seeker.field_0x4 = 0;
  fish_seeker.rate = 1.0;
  fish_seeker.spd = fish_seeker_params.cruise;
  fish_seeker.rot_z = 0;
  fish_seeker.pos.x = 14.0;
  fish_seeker.pos.y = 6.0;
  fish_seeker.pos.z = -10.0;
  fish_seeker.heading.x = 0.0;
  fish_seeker.heading.y = 0.0;
  fish_seeker.heading.z = -1.0;
  fish_seeker.goal.x = 8.0;
  fish_seeker.goal.y = 6.0;
  fish_seeker.goal.z = 4.0;
  fish_seeker.edge.x = 8.0;
  fish_seeker.edge.y = 6.0;
  fish_seeker.edge.z = 4.0;
  fish_seeker.want.x = 0.0;
  fish_seeker.want.y = 0.0;
  fish_seeker.want.z = -1.0;
  fish_mot_path_bind();
  morph_anim_init(0);
}

void fish_seeker_update(void)
{
  fish_seeker_rec *p = &fish_seeker;
  fish_seeker_prm *prm = p->prm;
  vec3 v;
  float d;
  rob_rec *rob;
  short *q = fish_mot_list;
  
  fish_seeker_view_check(p);
  switch (p->state) {
  case 1:
    fish_seeker_target_update(p);
    vec3_sub(&v.x,&p->edge.x,&p->pos.x);
    d = v.x * v.x + v.z * v.z;
    if (d > fish_seeker_near_dist_sq) {
      vec3_sub(&v.x,&p->goal.x,&p->pos.x);
      vec3_normalize_nolen(&v.x);
      p->want = v;
    }
    else {
      p->state = 2;
      if (lcg_next15() & 1) {
        p->flags |= 8;
      }
      else {
        p->flags &= ~8;
      }
    }
    vec3_sub(&v.x,&p->edge.x,&p->pos.x);
    d = v.x * v.x + v.z * v.z;
    if (d < fish_seeker_slow_dist_sq || (p->flags & 4)) {
      if (p->spd > prm->cruise) {
        p->spd = p->spd - prm->step;
      }
      else if (p->spd < prm->cruise) {
        p->spd = prm->step + p->spd;
      }
    }
    else if (p->spd < prm->fast) {
      p->spd = prm->step + p->spd;
    }
    fish_seeker_steer(p);
    fish_seeker_move(p);
    p->rate = p->spd / 0.1;
    if (p->flags & 4) {
      fish_seeker_scare_check(p);
    }
    break;
  case 2:
    fish_seeker_target_update(p);
    vec3_sub(&v.x,&p->edge.x,&p->pos.x);
    d = v.x * v.x + v.z * v.z;
    if (d > fish_seeker_far_dist_sq) {
      p->state = 1;
    }
    if (p->flags & 8) {
      rob = rob_get(0);
    }
    else {
      rob = rob_get(1);
    }
    vec3_sub(&v.x,&rob->pos.x,&p->pos.x);
    vec3_normalize_nolen(&v.x);
    p->want = v;
    if (p->spd > prm->close) {
      p->spd = p->spd - prm->step;
    }
    fish_seeker_steer(p);
    fish_seeker_move(p);
    p->rate = p->spd / 0.1;
    if (p->rate <= 0.0) {
      p->rate = 0.2;
    }
    if (p->flags & 4) {
      fish_seeker_scare_check(p);
    }
    break;
  case 3:
    p->t = p->t + 4.0;
    if (p->spd > 0.0) {
      p->spd = p->spd - prm->step;
    }
    fish_seeker_move(p);
    fish_mot_path_eval(0,p->t);
    if (p->t >= (float)q[1]) {
      p->state = 1;
      p->spd = prm->cruise;
    }
    break;
  case 4:
    if (p->t < 64.0) {
      p->rot_z = fish_seeker_turn_table[(int)p->t];
      if (p->spd > 0.0) {
        p->spd = p->spd - 0.002;
      }
    }
    else if (p->t >= 160.0) {
      short *tb = fish_seeker_turn_table;
      p->rot_z = tb[(int)p->t - 160] + -0x8000;
      p->rate = 1.0;
      if (p->spd < prm->cruise) {
        p->spd = 0.002 + p->spd;
      }
      fish_seeker_steer(p);
    }
    else {
      if (p->spd > 0.0) {
        p->spd = p->spd - 0.002;
      }
    }
    fish_seeker_move(p);
    p->t = p->t + 1.0;
    p->pos.y = p->pos.y - 0.01;
    if (p->t > 224.0) {
      p->state = 1;
      p->rot_z = 0;
    }
    break;
  case 5:
    if (p->t < 64.0) {
      p->rot_z = p->rot_z + 0x800;
      p->t = 1.0 + p->t;
    }
    else {
      p->rot_z = 0;
      p->t = 0.0;
      p->state = 1;
    }
    fish_seeker_move(p);
    break;
  }
  morph_anim_update(0,p->rate);
  dir_to_pitch_yaw(&p->heading.x,&p->rot_x,&p->rot_y);
}

void fish_seeker_scare_check(fish_seeker_rec *fish)
{
  rob_rec *rob;
  int i;

  rob = rob_get(0);
  for (i = 0; i < 2; i++, rob = rob + 1) {
    switch ((int)rob->move_no) {
    case 0x74e:
    case 0x991:
    case 0xa06:
    case 0xa3e:
    case 0xc12:
    case 0xc5e:
    case 0xcb4:
    case 0xd8e:
      if (rob->move_no != rob->old_cur->move_no) {
        fish->state = 3;
        fish->t = 0.0;
        fish_mot_path_eval(0,0.0);
        fish->rate = 1.0;
      }
      break;
    }
  }
}

void fish_seeker_target_update(fish_seeker_rec *fish)
{
  typedef struct { float x; float y; float z; } v3;
  v3 a;
  v3 c;
  float f;
  float r;
  
  fish_tank_view_edge_point(&a.x,&a.z);
  c = a;
  fish_tank_target_clamp(fish,&c.x,&c.z);
  fish->goal.x = c.x;
  fish->goal.z = c.z;
  fish->edge.x = a.x;
  fish->edge.z = a.z;
  if ((int)lcg_next15() % 180 == 0) {
    f = (lcg_next_unit() * 4.0 - 2.0) + fish->pos.y;
    r = f < 5.0 ? 5.0 : (f > 8.0 ? 8.0 : f);
    fish->goal.y = fish->edge.y = r;
  }
}

void fish_seeker_steer(fish_seeker_rec *fish)
{
  typedef struct { int w[3]; } vec_blk;
  float v[3];
  float t;
  float inv;
  t = fish->prm->keep;
  if (vec3_dot(&fish->heading.x, &fish->want.x) < -0.9238795) {
    t -= 0.1;
  }
  v[0] = t * fish->heading.x + fish->want.x * (1.0 - t);
  v[2] = t * fish->heading.z + (1.0 - t) * fish->want.z;
  inv = 1.0 / fsqrt_inl(v[0] * v[0] + v[2] * v[2]);
  v[0] = inv * v[0];
  v[2] = inv * v[2];
  v[1] = t * fish->heading.y + (1.0 - t) * fish->want.y;
  vec3_normalize_nolen(v);
  *(vec_blk *)&fish->heading = *(vec_blk *)v;
}

void fish_seeker_move(fish_seeker_rec *fish)
{
  float step[3];
  
  vec3_scale(step,&fish->heading.x,fish->spd);
  vec3_add_to(&fish->pos.x,step);
  fish_tank_pos_clamp(&fish->pos.x);
  return;
}

void fish_seeker_view_check(fish_seeker_rec *fish)
{
  float a [3];
  float b [3];
  
  b[1] = 0.0;
  a[1] = 0.0;
  a[0] = camera.target.x - camera.eye.x;
  a[2] = camera.target.z - camera.eye.z;
  b[0] = fish->pos.x - camera.eye.x;
  b[2] = fish->pos.z - camera.eye.z;
  vec3_normalize_nolen(a);
  vec3_normalize_nolen(b);
  if (vec3_dot(a,b) > 0.809017) {
    fish->flags |= 4;
  }
  else {
    fish->flags &= 0xfb;
  }
}

void fish_tank_target_clamp(fish_seeker_rec *fish, float *x, float *z)
{
  float fish_z = fish->pos.z;
  float c = fish_tank_limit_z;
  if (!(fish_z > c)) {
    if (*z > c) {
      *z = c;
    }
  }
  else if (*x < fish_tank_limit_x) {
    *x = fish_tank_limit_x;
  }
  return;
}

void fish_tank_pos_clamp(float *pos)
{
  float r;
  if (*pos < fish_tank_pos_min_x) {
    *pos = fish_tank_pos_min_x;
  }
  r = fish_tank_limit_z / fish_tank_limit_x;
  if (pos[2] < r * *pos) {
    if (fish_tank_limit_z <= pos[2]) {
      pos[2] = fish_tank_limit_z - 0.01;
    }
  }
  else if (*pos <= fish_tank_limit_x) {
    *pos = fish_tank_limit_x + 0.01;
  }
}

void fish_tank_view_edge_point(float *x, float *z)
{
  camera_state *a = &camera;
  float x0 = a->eye.x;
  float y0 = a->eye.z;
  float x1 = a->target.x;
  float y1 = a->target.z;
  float X, Y, A, B, s1, s2;

  if (x0 > x1) X = fish_tank_min_x; else X = fish_tank_max_x;
  if (y0 > y1) Y = fish_tank_min_z; else Y = fish_tank_max_z;
  s1 = (y0 - y1) / (x0 - x1);
  s2 = (x0 - x1) / (y0 - y1);
  A = s1 * (X - x0) + y0;
  B = s2 * (Y - y0) + x0;
  if (A * A > B * B) A = Y;
  else B = X;
  if (fish_tank_view_min_x > B) {
    if (A > 0.0) { A = fish_tank_max_z; B = fish_tank_max_x; }
    else { A = fish_tank_min_z; B = fish_tank_view_min_x; }
  }
  else if (!(fish_tank_max_z - 0.01 > A)) {
    B = fish_tank_max_x;
  }
  if (A > fish_tank_view_max_z) A = fish_tank_view_max_z;
  *x = B;
  *z = A;
}

void fish_seeker_rob_landed(char kind, rob_rec *rob)
{
  fish_seeker_rec *p = &fish_seeker;
  if (stage_cur_no == 3) {
    if (kind == 0 && (p->flags & 4) != 0 &&
        4.0 <= p->pos.y && (byte)(p->state - 1U) < 2) {
      p->state = 4;
      p->t = 0.0;
      p->rate = 0.0;
    }
  }
}

void fish_seeker_draw(void)
{
  float *m;
  fish_seeker_rec *p;
  bone_work_rec *v;
  float pos[3];
  
  m = stage_fx_base_mtx;
  p = &fish_seeker;
  v = mot_path_slots[0].bone;
  if ((p->flags & 2) != 0) {
    mtx_load_identity();
    mtx_translate_by_vec3(&p->pos.x);
    mtx_rot_y_s16(p->rot_y);
    mtx_rot_x_s16(p->rot_x);
    mtx_rot_z_s16(p->rot_z);
    if (p->state == 3) {
      mtx_translate_by_vec3(v->pos);
      mtx_store_translation(pos);
      fish_tank_pos_clamp(pos);
      mtx_overwrite_translation(pos);
      mtx_rot_zyx_s16(v->rot[0],v->rot[1],v->rot[2]);
    }
    ((int (*)())mtx_store)(m);
    mtx_peek_pushed(0);
    mtx_mul_by(m);
    morph_anim_draw(0);
  }
}

void fish_school_spawn(void)
{
  fish_school_member *fish;
  int left;
  fish_school_def *def;
  int n;

  fish = fish_school;
  left = 200;
  fish_school_near_count = 0;
  def = fish_school_near_defs;
  do {
    if (def->count < 0) break;
    n = def->count / fish_density_div;
    while (n--) {
      fish_school_member_init(fish,def);
      fish_school_near_count = fish_school_near_count + 1;
      fish = fish + 1;
      left--;
    }
    def = def + 1;
  } while (0 < left);
  def = fish_school_other_defs;
  while (0 < left) {
    if (def->count < 0) break;
    n = def->count / fish_density_div;
    while (n--) {
      fish_school_member_init(fish,def);
      fish = fish + 1;
      left--;
    }
    def = def + 1;
  }
  fish_school_count = 200 - (short)left;
  while (left--) {
    fish->flags = 0;
    fish = fish + 1;
  }
}

void fish_school_member_init(fish_school_member *fish, fish_school_def *def)
{
  fish_model_set *tb;
  short a;

  fish->flags = 3;
  fish->state = 0;
  fish->box = def->box;
  fish->prm = def->prm;
  fish->frame = -1;
  fish->frames = (tb = fish_model_sets)[fish->prm->set].frames;
  fish->models = tb[fish->prm->set].objs;
  fish->wait = 60;
  fish->pos.x = (float)((short)((int)lcg_next15() % def->spread[0]) - (short)(def->spread[0] / 2)) * 0.1f + def->pos.x;
  fish->pos.y = (float)((short)((int)lcg_next15() % def->spread[1]) - (short)(def->spread[1] / 2)) * 0.1f + def->pos.y;
  fish->pos.z = (float)((short)((int)lcg_next15() % def->spread[2]) - (short)(def->spread[2] / 2)) * 0.1f + def->pos.z;
  a = (int)lcg_next15() % (def->yaw_range * 2) - def->yaw_range;
  fish->dir.x = sin_s16(a);
  fish->dir.z = cos_s16(a);
  a = (int)lcg_next15() % (def->pitch_range * 2) - def->pitch_range;
  fish->dir.y = sin_s16(a);
  vec3_normalize_nolen(&fish->dir.x);
  fish->want = fish->dir;
  fish->speed = fish->prm->speed;
  fish->turn = fish->prm->turn;
  fish->turn_frames = 0;
  dir_to_pitch_yaw(&fish->dir.x,&fish->pitch,&fish->yaw);
  vec3_scale(&fish->step.x,&fish->dir.x,fish->speed);
}

void fish_school_update(void)
{
  fish_school_member *p;
  int i;
  fish_school_prm *c;
  float *b;
  short a;
  vec3 t;
  int n;

  p = fish_school;
  n = fish_school_count;
  if (mode_get_main() == MODE_MAIN_GAME && mode_get_sub() == 9) {
    n = fish_school_near_count;
  }
  for (i = 0; i < n; i++, p++) {
    c = p->prm;
    b = p->box;
    p->frame++;
    if (p->frame >= p->frames) {
      p->frame = 0;
    }
    if (p->want.x > 0.0 && p->pos.x >= b[0]) goto flip_x;
    if (p->want.x < 0.0 && p->pos.x <= b[3]) {
    flip_x:
      p->state = 0;
      p->speed = c->speed;
      p->want.x = -p->want.x;
      p->wait += 60;
      p->turn_frames = 60;
    }
    if ((p->want.y > 0.0 && p->pos.y >= b[1]) || (p->want.y < 0.0 && p->pos.y <= b[4])) {
      p->want.y = -p->want.y;
      p->wait += 60;
      p->turn_frames = 60;
    }
    if ((p->want.z > 0.0 && p->pos.z >= b[2]) || (p->want.z < 0.0 && p->pos.z <= b[5])) {
      p->state = 0;
      p->speed = c->speed;
      p->want.z = -p->want.z;
      p->wait += 60;
      p->turn_frames = 60;
    }
    if (p->turn_frames > 0) {
      vec3_weighted_sum(&p->dir.x,&p->want.x,p->turn,&t.x,1.0 - p->turn);
      vec3_normalize_nolen(&t.x);
      p->dir = t;
      dir_to_pitch_yaw(&p->dir.x,&p->pitch,&p->yaw);
      p->turn_frames--;
    }
    switch (p->state) {
    case 1:
      if (p->speed < c->dash_max) {
        p->speed = c->dash_step + p->speed;
      }
      else {
        p->state = 2;
      }
      break;
    case 2:
      if (p->speed > c->speed) {
        p->speed = p->speed - c->slow_step;
      }
      else {
        p->state = 0;
        p->speed = c->speed;
        p->turn = c->turn;
      }
      break;
    }
    vec3_scale(&p->step.x,&p->dir.x,p->speed);
    vec3_add_to(&p->pos.x,&p->step.x);
    if (p->pos.x > b[0]) p->pos.x = b[0];
    else if (p->pos.x < b[3]) p->pos.x = b[3];
    if (p->pos.y > b[1]) p->pos.y = b[1];
    else if (p->pos.y < b[4]) p->pos.y = b[4];
    if (p->pos.z > b[2]) p->pos.z = b[2];
    else if (p->pos.z < b[5]) p->pos.z = b[5];
    if (--p->wait <= 0) {
      a = (int)lcg_next15() % (c->yaw_range * 2) - c->yaw_range + p->yaw;
      t.x = sin_s16(a);
      t.z = cos_s16(a);
      a = (int)lcg_next15() % (c->pitch_range * 2) - c->pitch_range;
      t.y = sin_s16(a);
      vec3_normalize_nolen(&t.x);
      p->want = t;
      p->wait = (int)lcg_next15() % 200 + 60;
      p->turn_frames = 20;
    }
  }
}

void fish_school_draw(void)
{
  fish_school_member *p;
  int i;
  int n;
  uint q;
  list_block_ref b;
  p = fish_school;
  stopwatch_usec();
  q = elan_write_ptr_get();
  b.addr = q;
  *((uint *) 0xff000038) = q >> 0x18;
  *((uint *) 0xff00003c) = q >> 0x18;
  q = (q & 0x3ffffff) | 0xe0000000;
  n = fish_school_count;
  if ((mode_get_main() == MODE_MAIN_GAME) && (mode_get_sub() == 9))
  {
    if (p)
    {
      n = fish_school_near_count;
    }
    else
    {
      n = fish_school_near_count;
    }
  }
  for (i = 0; i < n; i++, p++)
  {
    mtx_set_current(stage_fx_base_mtx);
    mtx_translate_by_vec3(&p->pos.x);
    mtx_rot_yx_s16(p->pitch, p->yaw);
    q = model_matrix_cmds_write_at(q);
    q = model_part0_call_write_at(q, (compiled_model_rec *)p->models[p->frame]);
  }

  q += 0x20;
  q = (b.addr & 0xfc000000) | (q & 0x3ffffff);
  b.length = q - b.addr;
  elan_block_queue_relay(0, &b);
  elan_write_ptr_set(q);
  stopwatch_usec();
}

void fish_school_rob_landed(int kind, rob_rec *rob)
{
  typedef struct { float x; float y; float z; } vec;
  char c = kind;
  fish_school_member *p = fish_school;
  int i;
  fish_school_prm *q;
  vec v;

  if (stage_cur_no == 3) {
  if (c == 1) {
    v = *(vec *)&rob->ground_pos;
    for (i = 0; i < fish_school_near_count; i++, p++) {
      q = p->prm;
      v.y = p->pos.y + 0.01;
      vec3_sub(&p->want.x,&p->pos.x,(float *)&v);
      vec3_normalize_nolen(&p->want.x);
      p->state = 1;
      p->turn = q->scatter_turn;
      p->wait = p->wait + 0x3c;
      p->turn_frames = 0x14;
    }
  }
  }
}

void fish_path_spawn(void)
{
  fish_model_set *s;
  int i;
  int j;
  int k;
  void *m;
  fish_path_rec *p;
  fish_path_def *e;

  p = fish_paths;
  e = fish_path_defs;
  for (s = fish_model_sets, i = 0; i < 5; i++, s++) {
    m = effect_malloc(s->frames << 2);
    if (m == 0) {
      __asm__ volatile ("trapa #0x50");
    }
    s->clumps3 = (int)m;
    model_clump_frames_build(s->x_range,s->y_range,3,m,s->first_obj,s->frames);
    m = effect_malloc(s->frames << 2);
    if (m == 0) {
      __asm__ volatile ("trapa #0x50");
    }
    s->clumps6 = (int)m;
    model_clump_frames_build(s->x_range,(int)((float)s->y_range + 1.0),6,m,s->first_obj,s->frames);
    m = effect_malloc(s->frames << 2);
    if (m == 0) {
      __asm__ volatile ("trapa #0x50");
    }
    s->objs = (int *)m;
    for (j = 0; j < s->frames; j++) {
      s->objs[j] = (int)obj_model_get(j + s->first_obj);
    }
  }
  i = 0x96;
  do {
    p->flags = 0;
    if (e->set < 0) break;
    s = fish_model_sets + e->set;
    k = e->count[0] / fish_density_div;
    while (k--) {
      fish_path_member_init(p,s,e);
      p->models = s->objs;
      p++;
      i--;
    }
    k = e->count[1];
    k = k / fish_density_div;
    while (k--) {
      fish_path_member_init(p,s,e);
      p->models = (int *)s->clumps3;
      p++;
      i--;
    }
    k = e->count[2] / fish_density_div;
    while (k--) {
      fish_path_member_init(p,s,e);
      p->models = (int *)s->clumps6;
      p++;
      i--;
    }
    e++;
  } while (i > 0);
  fish_path_count = 0x96 - i;
  while (i--) {
    p->flags = 0;
    p++;
  }
}

void fish_solo1_init(void)
{
  fish_path_rec *p = &fish_solo1;
  fish_solo_setup(p,&fish_solo1_def);
  p->flags = 0x23;
  p->frames = 0xb4;
  p->rz = (int)effect_rand15() % 0x2000 - 0x1000;
  morph_anim_init(1);
  return;
}

void fish_solo2_init(void)
{
  fish_path_rec *p = &fish_solo2;
  fish_solo_setup(p, &fish_solo2_def);
  p->flags = 3;
  p->frames = 0x78;
  p->rz = (int)effect_rand15() % 0x2000 - 0x1000;
}

void fish_solo3_init(void)
{
  fish_solo_setup(&fish_solo3,&fish_solo3_def);
  fish_solo3.flags = 3;
  morph_anim_init(2);
  return;
}

void fish_path_member_init(fish_path_rec *fish, fish_model_set *set, fish_path_def *def)
{
  vec3 v;
  vec3 zero = {0.0f, 0.0f, 0.0f};
  short n;
  fish_path_seg *seg;
  float f;

  fish->flags = 11;
  fish->frames = set->frames;
  fish->frame_f = -1.0f;
  fish->frame = 0;
  fish->rate = (float)((int)effect_rand15() % 20) * 0.01f;
  fish->speed = def->speed;
  fish->off.x = (float)((short)((int)effect_rand15() % def->spread[0]) - (short)(def->spread[0] / 2)) * 0.1f;
  fish->off.y = (float)((short)((int)effect_rand15() % def->spread[1]) - (short)(def->spread[1] / 2)) * 0.1f;
  fish->off.z = f = (float)((short)((int)effect_rand15() % def->spread[2]) - (short)(def->spread[2] / 2)) * 0.1f;
  fish->pos.x = def->pos.x + fish->off.x;
  fish->pos.y = def->pos.y + fish->off.y;
  {
    float g;
    fish->pos.z = (g = def->pos.z) + f;
  }
  fish->ry = def->yaw;
  fish->ry_prev = def->yaw;
  fish->rx = def->pitch;
  fish->turn = 0;
  if (def->pitch_spread != 0) {
    fish->rx += (int)effect_rand15() % def->pitch_spread - def->pitch_spread / 2;
  }
  fish->rz = 0;
  fish->lo = def->lo;
  fish->hi = def->hi;
  fish->max_turn = def->max_turn;
  fish->wait = (int)effect_rand15() % 60;
  fish->count = 0;
  if (def->push != 0) {
    v = zero;
    v.z = (float)((int)effect_rand15() % def->push) * 0.1f;
    mtx_load_identity();
    mtx_rot_y_s16(fish->ry);
    mtx_xform_point(&v.x, &v.x);
    fish->pos.x += v.x;
    fish->pos.y += v.y;
    fish->pos.z += v.z;
  }
  fish->seg = def->script;
  seg = fish->seg;
  if (seg->kind == -2) {
    fish->lim = seg->off + fish->off.x;
  }
  else if (seg->kind == -3) {
    fish->lim = seg->off + fish->off.z;
  }
}

void fish_solo_setup(fish_path_rec *fish, fish_path_def *def)
{
  fish_path_rec *p = fish;
  fish_path_def *s = def;
  float v[3];
  int z[3];
  fish_path_seg *seg;
  z[0] = 0;
  z[1] = 0;
  z[2] = 0;
  p->frame_f = -1.0;
  p->frame = 0;
  p->rate = (float)((int)effect_rand15() % 20) * 0.01;
  p->speed = s->speed;
  p->off.x = (float)((short)((int)effect_rand15() % s->spread[0]) - (short)(s->spread[0] / 2)) * 0.1;
  p->off.y = (float)((short)((int)effect_rand15() % s->spread[1]) - (short)(s->spread[1] / 2)) * 0.1;
  p->off.z = (float)((short)((int)effect_rand15() % s->spread[2]) - (short)(s->spread[2] / 2)) * 0.1;
  p->pos.x = s->pos.x + p->off.x;
  p->pos.y = s->pos.y + p->off.y;
  p->pos.z = s->pos.z + p->off.z;
  p->ry = s->yaw;
  p->ry_prev = s->yaw;
  p->rx = s->pitch;
  p->turn = 0;
  if (s->pitch_spread != 0) {
    p->rx += (short)((int)effect_rand15() % s->pitch_spread) - (short)(s->pitch_spread / 2);
  }
  p->lo = s->lo;
  p->hi = s->hi;
  p->max_turn = s->max_turn;
  p->wait = (int)effect_rand15() % 60;
  p->count = 0;
  if (s->push != 0) {
    ((int *)v)[0] = z[0];
    ((int *)v)[1] = z[1];
    ((int *)v)[2] = z[2];
    v[2] = (float)((int)effect_rand15() % s->push) * 0.1;
    mtx_load_identity();
    mtx_rot_y_s16(p->ry);
    mtx_xform_point(v,v);
    p->pos.x += v[0];
    p->pos.y += v[1];
    p->pos.z += v[2];
  }
  p->seg = s->script;
  seg = p->seg;
  if (seg->kind == -2) {
    p->lim = seg->off + p->off.x;
  }
  else if (seg->kind == -3) {
    p->lim = seg->off + p->off.z;
  }
}

void fish_path_update(void)
{
  int i;
  fish_path_rec *p;
  fish_path_seg *q;

  p = fish_paths;
  for (i = 0; i < fish_path_count; i++, p++) {
    q = p->seg; p->frame_f += q->rate + p->rate;
    p->frame = (short)(int)p->frame_f;
    if (p->frame >= p->frames) {
      p->frame_f = 0.0;
      p->frame = 0;
    }
  }
  fish_path_follow(fish_paths,(int)fish_path_count);
}

void fish_path_draw(void)
{
  uint cmd;
  int model;
  fish_path_rec *p;
  uint q;
  int i;
  list_block_ref buf;

  p = fish_paths;
  q = elan_write_ptr_get();
  buf.addr = q;
  *(volatile uint *)0xff000038 = q >> 0x18;
  *(volatile uint *)0xff00003c = q >> 0x18;
  q = q & 0x3ffffff | 0xe0000000;
  for (i = 0; i < fish_path_count; i++, p = p + 1) {
    mtx_set_current(stage_fx_base_mtx);
    mtx_translate_by_vec3(&p->pos.x);
    mtx_rot_yx_s16((int)p->rx,(int)p->ry);
    model = p->models[p->frame];
    cmd = model_matrix_cmds_write_at(q);
    q = model_part0_call_write_at(cmd,(compiled_model_rec *)model);
  }
  q += 0x20;
  q = buf.addr & 0xfc000000 | q & 0x3ffffff;
  buf.length = q - buf.addr;
  elan_block_queue_relay(0,&buf);
  elan_write_ptr_set(q);
  return;
}

void fish_solo1_update(void)
{
  morph_anim_update(1,fish_solo1.seg->rate);
  fish_path_follow(&fish_solo1,1);
  return;
}

void fish_solo1_draw(void)
{
  fish_path_rec *p = &fish_solo1;
  mtx_set_current(stage_fx_base_mtx);
  mtx_translate_by_vec3(&p->pos.x);
  mtx_rot_y_s16(p->ry);
  mtx_rot_x_s16(p->rx);
  mtx_rot_z_s16(p->rz);
  morph_anim_draw(1);
}

void fish_solo2_update(void)
{
  fish_path_rec *p;
  fish_path_seg *q;

  p = &fish_solo2;
  q = p->seg;
  p->frame_f += q->rate + p->rate;
  p->frame = (short)(int)p->frame_f;
  if (p->frame >= p->frames) {
    p->frame_f = 0.0;
    p->frame = 0;
  }
  fish_path_follow(p,1);
}

void fish_solo2_draw(void)
{
  fish_path_rec *p = &fish_solo2;
  mtx_set_current(stage_fx_base_mtx);
  mtx_translate_by_vec3(&p->pos.x);
  mtx_rot_y_s16(p->ry);
  mtx_rot_x_s16(p->rx);
  mtx_rot_z_s16(p->rz);
  obj_draw(p->frame + 0x1900ca);
}

void fish_solo3_update(void)
{
  morph_anim_update(2,fish_solo3.seg->rate);
  fish_path_follow(&fish_solo3,1);
  return;
}

void fish_solo3_draw(void)
{
  fish_path_rec *p = &fish_solo3;
  mtx_set_current(stage_fx_base_mtx);
  mtx_translate_by_vec3(&p->pos.x);
  mtx_rot_y_s16(p->ry);
  mtx_rot_x_s16(p->rx);
  mtx_rot_z_s16(p->rz);
  morph_anim_draw(2);
}

void fish_path_follow(fish_path_rec *fish, int count)
{
  fish_path_rec *p = fish;
  int n = count;
  int i;
  fish_path_seg *s;
  short t;
  int hit;
  vec3 v;
  vec3 o = { 0.0, 0.0, 0.0 };

  for (i = 0; i < n; i++, p++) {
    if (p->flags & 8) {
      if (p->wait == 0) p->flags &= ~8;
      else p->wait--;
      continue;
    }
    s = p->seg;
    p->ry += s->dy;
    p->rx += s->dx;
    p->speed += s->spd;
    if (p->turn != 0) {
      t = p->max_turn;
      if (p->turn > 0) {
        if (p->turn < t) t = p->turn;
      }
      else {
        t = -t;
        if (p->turn > t) t = p->turn;
      }
      p->rx += t;
      p->turn -= t;
    }
    else if ((p->rx <= 0 && p->pos.y > p->hi) || (p->rx >= 0 && p->pos.y < p->lo)) {
      p->turn = -(p->rx * 2);
    }
    v = o;
    v.z = p->speed;
    mtx_load_identity();
    mtx_rot_y_s16(p->ry);
    mtx_rot_x_s16(p->rx);
    mtx_xform_point(&v.x,&v.x);
    p->pos.x += v.x;
    p->pos.y += v.y;
    p->pos.z += v.z;
    hit = 0;
    switch (s->kind) {
    case -7:
    case -6:
    case -5:
    case -1:
      break;
    case -2:
      if (!(v.x < 0.0)) {
        if (p->pos.x > p->lim) hit = 1;
      }
      else {
        if (p->pos.x < p->lim) hit = 1;
      }
      break;
    case -3:
      if (!(v.z < 0.0)) {
        if (p->pos.z > p->lim) hit = 1;
      }
      else {
        if (p->pos.z < p->lim) hit = 1;
      }
      break;
    case -4:
      if (s->dy > 0) {
        if (p->ry >= s->lim && p->ry_prev < s->lim) hit = 1;
      }
      else {
        if (p->ry <= s->lim && p->ry_prev > s->lim) hit = 1;
      }
      break;
    default:
      p->count++;
      if (p->count >= s->kind) hit = 1;
      break;
    }
    p->ry_prev = p->ry;
    if (!hit) continue;
    s++;
    p->count = 0;
    for (;;) {
      switch (s->kind) {
      case -5:
        if (!(v.x < 0.0)) {
          if (p->pos.x > s->off + p->off.x) s = s->next;
          else s++;
        }
        else {
          if (p->pos.x < s->off + p->off.x) s = s->next;
          else s++;
        }
        continue;
      case -6:
        if (!(v.z < 0.0)) {
          if (p->pos.z > s->off + p->off.z) s = s->next;
          else s++;
        }
        else {
          if (p->pos.z < s->off + p->off.z) s = s->next;
          else s++;
        }
        continue;
      case -7:
        s = s->next;
        continue;
      }
      break;
    }
    if (s->kind == -2) p->lim = s->off + p->off.x;
    else if (s->kind == -3) p->lim = s->off + p->off.z;
    p->seg = s;
  }
}

void fish_model_sets_free(void)
{
  fish_model_set *set;
  int i;

  set = fish_model_sets;
  i = 0;
  do {
    model_clump_frames_free((int *)set->clumps3,(int)set->frames);
    model_clump_frames_free((int *)set->clumps6,(int)set->frames);
    effect_free(set->objs);
    effect_free((void *)set->clumps3);
    effect_free((void *)set->clumps6);
    set->objs = 0;
    set->clumps3 = 0;
    set->clumps6 = 0;
    i = i + 1;
    set = set + 1;
  } while (i < 5);
  return;
}

void dir_to_pitch_yaw(float *dir,short *pitch,short *yaw)
{
  *yaw = atan2_s16(dir[0],dir[2]);
  *pitch = atan2_s16(-dir[1],dir_fsqrt(dir[0] * dir[0] + dir[2] * dir[2]));
}
