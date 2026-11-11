/*
 * vf4lib/libaction/src/rob_coli.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_vec3_set_length
#define KRPROTO_field_floor_height
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern float robs_push_share; /* 0x0c4220b0 */
extern int robs_push_state_mask; /* 0x0c4220c0 */
extern est_row est_data[14]; /* 0x0c3713b4 */
extern coli_test_nearest_rec coli_test_nearest_hits; /* 0x0c371994 */
extern agg_0c3719a0 agg_0c3719a0_0c3719a0; /* 0x0c3719a0 */
extern agg_0c371a3c agg_0c371a3c_0c371a3c; /* 0x0c371a3c */
extern agg_0c371a44 agg_0c371a44_0c371a44; /* 0x0c371a44 */
extern agg_0c371a5c agg_0c371a5c_0c371a5c; /* 0x0c371a5c */
extern coli_test_hits_rec coli_test_hits; /* 0x0c371ad8 */
extern agg_0c371b98 agg_0c371b98_0c371b98; /* 0x0c371b98 */
extern agg_0c372430 agg_0c372430_0c372430; /* 0x0c372430 */
extern agg_0c372448 agg_0c372448_0c372448; /* 0x0c372448 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern rob_coli_rec rob_coli_records[2]; /* 0x0c37cccc */
extern float robs_push_dir[3]; /* 0x0c4220b4 */

void rob_coli_clear(void)
{
  vec3 zero = {0, 0, 0};
  rob_coli_rec *coli;
  uint n;
  int i;
  vec3 *sph_a;
  vec3 *sph_b;
  float *rad_a;
  float *rad_b;
  short *sph_flag;
  rob_rec *rob;

  for (n = 0, coli = rob_coli_records; (int)n < 2; n++, coli++) {
    sph_a = coli->bank_a;
    sph_b = coli->bank_b;
    rad_a = coli->rad_a;
    rad_b = coli->rad_b;
    coli->sph_a = sph_a;
    coli->sph_b = sph_b;
    sph_flag = coli->sph_flag;
    for (i = 0; i < 26; i++) {
      *sph_a++ = zero;
      *sph_b++ = zero;
      *rad_a++ = 0;
      *rad_b++ = 0;
      *sph_flag++ = 0;
    }
    rob = rob_get(n & 0xff);
    rob->coli = coli;
    coli->rob = rob;
    coli->flags = 0;
    coli->field_0x10 = 0;
    coli->touch_mask = 0;
    coli->field_0x1c[0] = 0;
    coli->field_0x1c[1] = 0;
    coli->touched_by_masked = 0;
    coli->field_0x28[0] = 0;
    coli->field_0x28[1] = 0;
    coli->field_0x28[2] = 0;
    coli->touch_bits = 0;
    coli->touch_pairs = 0;
    coli->field_0x3c[0] = 0;
    coli->field_0x3c[1] = 0;
    coli->field_0x3c[2] = 0;
    coli->field_0x3c[4] = 0;
    coli->field_0x3c[5] = 0;
    coli->field_0x3c[3] = 0;
    coli->field_0x3c[6] = 0;
    coli->field_0x98 = 0;
    coli->field_0x94 = 0;
    coli->ofs[0] = zero;
    coli->ofs[1] = zero;
    coli->ofs[2] = zero;
    coli->ofs[3] = zero;
    coli->ofs[4] = zero;
    coli->ofs[5] = zero;
    coli->ofs[6] = zero;
    coli->ofs[7] = zero;
    coli->ofs[8] = zero;
    coli->field_0x380[0] = 0;
    coli->field_0x380[1] = 0;
    coli->touch_dist = 0;
    coli->field_0x38c = 0;
  }
}

void rob_coli_task_start(void)
{
  vec3 pos;
  int i;
  int j;
  rob_coli_rec *coli;
  rob_rec *rob;
  vec3 *sph_a;
  vec3 *sph_b;
  short *sph_flag;

  for (i = 0, coli = rob_coli_records; i < 2; i++, coli++) {
    sph_a = coli->bank_a;
    sph_b = coli->bank_b;
    rob = rob_get((uchar)i);
    pos = rob->pos;
    rob->coli = coli;
    coli->rob = rob;
    coli->flags = 0;
    coli->field_0x10 = 0;
    coli->touch_mask = 0;
    coli->field_0x1c[0] = 0;
    coli->field_0x1c[1] = 0;
    coli->touched_by_masked = 0;
    coli->field_0x28[0] = 0;
    coli->field_0x28[1] = 0;
    coli->field_0x28[2] = 0;
    coli->touch_bits = 0;
    coli->touch_pairs = 0;
    coli->field_0x3c[1] = 0;
    coli->field_0x3c[2] = 0;
    coli->field_0x3c[4] = 0;
    coli->field_0x3c[5] = 0;
    coli->field_0x3c[3] = 0;
    coli->field_0x3c[6] = 0;
    coli->field_0x98 = coli->field_0x3c[0] = coli->field_0x94 = 0;
    coli->ofs[0] = pos;
    coli->ofs[1] = pos;
    coli->ofs[2] = pos;
    coli->ofs[3] = pos;
    coli->ofs[4] = pos;
    coli->ofs[5] = pos;
    coli->ofs[6] = pos;
    coli->ofs[7] = pos;
    coli->ofs[8] = pos;
    coli->field_0x380[0] = 0.0;
    coli->field_0x380[1] = 0.0;
    coli->touch_dist = 0.0;
    coli->field_0x38c = 0.0;
    coli->ofs[1].x = 0.0;
    coli->ofs[1].y = 0.0;
    coli->ofs[1].z = 0.0;
    coli->sph_a = sph_a;
    coli->sph_b = sph_b;
    sph_flag = coli->sph_flag;
    for (j = 0; j < 26; j++, sph_a++, sph_b++, sph_flag++) {
      *sph_a = pos;
      *sph_b = pos;
      *sph_flag = 0;
    }
  }
  vec3_sub(robs_push_dir,&(rob_get(0))->place_pos.x,&(rob_get(1))->place_pos.x);
  est_data_init();
  agg_0c3719a0_0c3719a0.field_0x4 = 0.0;
  agg_0c3719a0_0c3719a0.field_0x0 = 0.0;
  agg_0c371a44_0c371a44.field_0x4 = 0.0;
  agg_0c371a44_0c371a44.field_0x0 = 0.0;
  agg_0c372430_0c372430.field_0x4 = 0.0;
  agg_0c372430_0c372430.field_0x0 = 0.0;
  agg_0c371a5c_0c371a5c.field_0x4 = 0.0;
  agg_0c371a5c_0c371a5c.field_0x0 = 0.0;
  agg_0c371b98_0c371b98.field_0x4 = 0.0;
  agg_0c371b98_0c371b98.field_0x0 = 0.0;
  agg_0c371a3c_0c371a3c.field_0x4 = 0.0;
  agg_0c371a3c_0c371a3c.field_0x0 = 0.0;
  agg_0c372448_0c372448.field_0x4 = 0.0;
  agg_0c372448_0c372448.field_0x0 = 0.0;
}

void rob_coli_init(rob_rec *rob)
{
  float *rad;
  chara_sphere_rec *sph;
  int i;
  rad = rob->coli->rad_a;
  sph = *(chara_sphere_rec **)(rob->chara_data + 0x3c);
  for (i = 26; i != 0; i--) {
    *rad = sph->radius;
    sph = sph + 1;
    rad = rad + 1;
  }
  rob_coli_task_start();
}

void rob_coli_task_frame(void)
{
  rob_rec *rob;
  int i;
  int near;
  rob_rec *rob0;
  rob_rec *rob1;
  float buf[15];

  if (cb_test_window_on == 0 && pause_frame_held() != 0) return;
  if ((mode_get_main() == MODE_MAIN_ADVERTISE && (mode_get_sub() == 8 || mode_get_sub() == 7)) ||
      (game_flags & 0x2000) != 0 ||
      (mode_get_main() == MODE_MAIN_GAME &&
       (mode_get_sub() > 13 || mode_get_sub() == 7 || mode_get_sub() == 15))) {
    for (i = 0; i < 2; i++) {
      rob = rob_get(i & 0xff);
      if ((rob->flags & 1) != 0) {
        { rob_coli_rec *coli = rob->coli; near = 0; if (coli->field_0x3c[3] != 0 || (rob->flags & 0x100) != 0) near = 1; }
        *(float *)&rob->floor_y = field_floor_height((coli_poly_rec *)buf,&rob->pos.x,near,0.5);
      }
    }
  }
  else {
    robs_coli_prepare();
    if ((user_back_data.option_bits & 0x10) != 0) {
      for (i = 0; i < 2; i++) {
        rob = rob_get(i & 0xff);
        if ((rob->relation_flags & 1) != 0 &&
            (rob->action.word & 0xff300000) == 0x2000000 &&
            (rob->action.part.lever & 7) < 5) {
          vec3_sub(buf,&rob->old_cur->pos.x,&rob->pos.x);
          vec3_scale(buf,buf,rob_old_pull_scale);
          buf[1] = 0.0;
          rob_coli_offset(rob,buf);
        }
      }
    }
    robs_body_push();
    robs_max_distance_pull();
    rob0 = rob_get(0);
    rob1 = rob_get(1);
    rob_coli_offset(rob0,&rob0->coli->ofs[7].x);
    rob_coli_offset(rob1,&rob1->coli->ofs[7].x);
    robs_wall_coli();
    robs_floor_check();
    robs_coli_shift_apply();
  }
}

void robs_coli_prepare(void)
{
  vec3 z;
  rob_rec *rob;
  rob_coli_rec *coli;
  int i;
  int zero;

  i = 0;
  zero = 0;
  for (; i < 2; i++) {
    z.x = 0.0f;
    z.y = 0.0f;
    z.z = 0.0f;
    rob = rob_get((unsigned char)i);
    coli = rob->coli;
    rob->field_0x224 = zero;
    coli->field_0x10 = coli->flags;
    coli->field_0x14 = rob->flags;
    coli->field_0x90[2] = coli->field_0x90[0];
    coli->field_0x90[3] = coli->field_0x90[1];
    coli->ofs[0] = z;
    coli->ofs[2] = z;
    coli->ofs[3] = z;
    coli->ofs[4] = z;
    coli->ofs[5] = z;
    coli->ofs[6] = z;
    coli->ofs[7] = z;
    coli->ofs[8] = z;
    coli->touch_dist = 0.0f;
    coli->field_0x38c = 0.0f;
    coli->flags = zero;
    coli->touch_mask = zero;
    coli->field_0x1c[0] = zero;
    coli->field_0x1c[1] = zero;
    coli->touched_by_masked = zero;
    coli->field_0x28[0] = zero;
    coli->field_0x28[1] = zero;
    coli->field_0x28[2] = zero;
    coli->field_0x58 = zero;
    coli->field_0x94 = zero;
    coli->field_0x3c[0] = zero;
    coli->field_0x98 = zero;
    coli->field_0x380[0] = 0.0f;
    coli->field_0x380[1] = 0.0f;
    if (!(rob->flags & 0x100) && (rob->move_flags & 0x4000) &&
        rob->frame < rob->active_end) {
      coli->touch_mask = limb_attack_sphere_masks[rob->attack_limb];
      coli->field_0x1c[0] = 0x3fffde0;
    }
    if (rob->state & 0x300000) {
      if (!(rob->opp->state & 0x300000)) {
        coli->flags |= 1;
      }
    }
    else if (!(rob->flags24 & 0x80) &&
             !(rob->opp->flags24 & 0x80) &&
             !(rob->flags & 0x40000) &&
             !(rob->opp->flags & 0x40000)) {
      coli->flags |= 1;
    }
    if (rob->flags & 0x100) {
      if ((rob->flags24 & 0x08000000) && !(rob->flags & 8)) {
        coli->field_0x3c[6] = 5;
      }
      else if (!(rob->state & 0x40000000) ||
               (rob->frame > rob->field_0x2ee && rob->frames_to_land < 4)) {
        coli->field_0x3c[6] = 0x41;
      }
    }
    else if (rob->flags & 0x40000) {
      coli->field_0x3c[6] = 0x41;
    }
    else {
      if ((rob->flags24 & 0x20000000) || (rob->state & 0x40000)) {
        coli->field_0x3c[6] = 7;
      }
      else {
        coli->field_0x3c[6] = 5;
      }
      if ((rob->state & 0x400) && (rob->flags & 0x10000)) {
        coli->field_0x3c[6] |= 0x10;
      }
      if (!coli->field_0x90[1]) {
        coli->field_0x3c[6] = 0x40;
      }
    }
    rob_coli_spheres_update(rob);
  }
  robs_push_dir_update();
}

void robs_push_dir_update(void)
{
  typedef struct { int w[3]; } vec;
  rob_rec *rob0;
  rob_rec *rob1;
  uint state;
  uint flags28;
  ushort side_flags;
  vec push_pos[2];
  vec pos[2];
  float *dir;
  
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  state = rob0->state | rob1->state;
  flags28 = rob0->flags28 | rob1->flags28;
  side_flags = rob0->side_flags | rob1->side_flags;
  robs_push_state_mask = 0x40820;
  if (rob0->combo[0].hits > 3 || rob1->combo[0].hits > 3) {
    robs_push_state_mask |= 0x1000640;
  }
  if ((state & robs_push_state_mask) != 0 || ((side_flags & 0x20) != 0 && (state & 0x4000000) != 0) ||
      rob0->frame == 1 || rob1->frame == 1 ||
      rob0->opp_dist > robs_push_dir_far || (flags28 & 1) != 0) {
    push_pos[0] = *(vec *)&rob0->pos;
    push_pos[1] = *(vec *)&rob1->pos;
    vec3_add_to((float *)&push_pos[0], &rob0->coli->ofs[1].x);
    vec3_add_to((float *)&push_pos[1], &rob1->coli->ofs[1].x);
    dir = robs_push_dir;
    vec3_sub(dir, (float *)&push_pos[0], (float *)&push_pos[1]);
  }
  else {
    if ((state & 0x300000) == 0) {
      return;
    }
    pos[0] = *(vec *)&rob0->pos;
    pos[1] = *(vec *)&rob1->pos;
    dir = robs_push_dir;
    vec3_sub(dir, (float *)&pos[0], (float *)&pos[1]);
  }
  vec3_normalize(dir);
  return;
}

void rob_coli_spheres_update(rob_rec *rob)
{
  float *rad_out;
  uint attack_mask;
  float *rad;
  uint mask_340;
  vec3 *pos;
  chara_sphere_rec *sph;
  int i;
  rob_coli_rec *coli;
  float *mtx;
  float top;
  float floor_y;
  float bottom;
  float s, f, v;
  
  coli = rob->coli;
  pos = coli->sph_b;
  coli->sph_b = coli->sph_a;
  coli->sph_a = pos;
  mtx = rob->mtx;
  sph = *(chara_sphere_rec **)(rob->chara_data + 0x3c);
  top = 0.0;
  bottom = 999.0;
  floor_y = rob->floor_y;
  mtx_push(0);
  for (i = 0; i < 0x1a; i++, pos = pos + 1, sph = sph + 1) {
    mtx_set_current_wordwise(mtx + sph->slot * 0x10);
    mtx_xform_point(&sph->pos.x,&pos->x);
    if (pos->y < floor_y) {
      pos->y = floor_y + 0.0001;
    }
    if (pos->y > top) top = pos->y;
    else if (pos->y < bottom) bottom = pos->y;
  }
  coli->field_0x380[0] = top;
  coli->field_0x380[1] = bottom;
  mtx_pop(1);
  attack_mask = coli->touch_mask;
  mask_340 = rob->field_0x340;
  rad = coli->rad_a;
  rad_out = coli->rad_b;
  s = 1.0;
  f = rob->field_0x370;
  if ((rob->state & 0x800) != 0) {
    s = 1.3;
  }
  if (rob->frame < rob->active_start) attack_mask = 0;
  if (rob->field_0x316 == 0) mask_340 = 0;
  for (i = 0; i < 0x1a; i++, rad = rad + 1, rad_out = rad_out + 1, attack_mask = attack_mask >> 1, mask_340 = mask_340 >> 1) {
    v = s * *rad;
    if ((attack_mask & 1) != 0) {
      v = v * rob->field_0x364;
    }
    if ((mask_340 & 1) != 0) {
      v = v * f;
    }
    if ((i == 3) && ((rob->flags & 0x10) != 0)) {
      v = v * 1.2;
    }
    *rad_out = v;
  }
  coli->touch_bits = 0;
  coli->touch_pairs = 0;
}

void robs_body_push(void)
{
  vec3 v;
  vec3 t1;
  vec3 t2;
  rob_rec *rob0;
  rob_rec *rob1;
  rob_rec *rob;
  rob_coli_rec *coli;
  rob_coli_rec *coli0;
  rob_coli_rec *coli1;
  unsigned int state;
  int mode;
  unsigned int mask;
  unsigned char lever;
  float close;
  unsigned int flags24_0, flags24_1;
  float tt;
  int dir;
  unsigned int word;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  coli0 = rob_coli_records;
  coli1 = &rob_coli_records[1];
  if (!(rob0->flags & rob1->flags & 1)) return;
  if (!coli_sphere_sets_test((int)rob0, (int)rob1, (int)coli0, (int)coli1)) return;
  coli0->flags |= 8;
  coli1->flags |= 8;
  v = *(vec3 *)&robs_push_dir;
  state = rob0->state | rob1->state;
  if ((state & 0x2000400) == 0x2000400) {
    rob = (rob0->state & 0x2000000) ? rob0 : rob1;
    if (rob->field_0x36c > 0.0f) {
      coli = rob->coli;
      if ((coli->flags & 1) && !(rob->flags & 8)) {
        coli->ofs[7].y += rob->field_0x36c;
      }
    }
    return;
  }
  if (!(rob0->flags & 8) || !(rob1->flags & 8)) {
    v.y = 0.0f;
  }
  word = rob0->flags;
  mask = 0x100;
  if (word & mask) {
    word = rob1->flags;
    mode = (word & mask) ? 3 : 2;
  }
  else if (rob1->flags & 0x100) mode = 1;
  else {
    mask = 0x800;
    if (state & mask) {
      word = rob0->state;
      if (!(word & mask)) mode = 1;
      else {
        word = rob1->state;
        mode = (word & mask) ? 3 : 2;
      }
    }
    else {
      mask = 0x4000;
      if (state & mask) {
      word = rob0->state;
      if (!(word & mask)) mode = 1;
      else {
        word = rob1->state;
        mode = (word & mask) ? 3 : 2;
      }
    }
      else if ((rob0->flags & 8) || (rob1->flags & 8)) mode = 3;
      else {
        mask = 0x40400;
        if (!(state & mask)) mode = 3;
        else {
      word = rob0->state;
      if (!(word & mask)) mode = 1;
      else {
        word = rob1->state;
        mode = (word & mask) ? 3 : 2;
      }
    }
      }
    }
  }
  robs_push_share = robs_push_share_base;
  if (rob0->action.part.type == 2) {
    lever = rob0->action.part.lever;
    dir = lever & 7;
    if (lever & 0x40) {
      robs_push_share += robs_push_share_lever40;
    }
    else {
      if ((lever & 0x20) && dir == 0) {
        robs_push_share += robs_push_share_lever20;
      }
    }
  }
  if (rob1->action.part.type == 2) {
    lever = rob1->action.part.lever;
    if (lever & 0x40) {
      robs_push_share += robs_push_share_lever40;
    }
    else if (lever & 0x20) {
      robs_push_share += robs_push_share_lever20;
    }
  }
  if (rob0->opp_dist < 1.0f) {
    close = 1.0f - rob0->opp_dist;
    robs_push_share = close * close * robs_push_share_sq + close * robs_push_share_lin + robs_push_share + (float)coli0->touch_pairs * robs_push_share_c38;
  }
  flags24_0 = rob0->flags24;
  flags24_1 = rob1->flags24;
  if (flags24_0 & 0x2000) robs_push_share += 0.4f;
  if (flags24_1 & 0x2000) robs_push_share += 0.4f;
  if (robs_push_share > 1.0f) {
    robs_push_share = 1.0f;
  }
  else if (robs_push_share < 0.0f) {
    robs_push_share = robs_push_share_base;
  }
  if (mode == 3) {
    vec3_scale((float *)&v, (float *)&v, coli0->touch_dist * 0.5f);
  }
  else {
    vec3_scale((float *)&v, (float *)&v, coli0->touch_dist);
  }
  if (mode & 1) {
    t1 = v;
    vec3_scale((float *)&t1, (float *)&t1, robs_push_share);
    vec3_add_to(&coli0->ofs[0].x, (float *)&t1);
    vec3_sub((float *)&t1, (float *)&v, (float *)&t1);
    coli0->ofs[1] = t1;
    if (coli0->flags & 1) {
      vec3_add_to(&coli0->ofs[7].x, &coli0->ofs[0].x);
    }
  }
  if (mode & 2) {
    t2 = v;
    vec3_scale((float *)&t2, (float *)&t2, robs_push_share);
    vec3_sub_from(&coli1->ofs[0].x, (float *)&t2);
    vec3_sub((float *)&t2, (float *)&v, (float *)&t2);
    vec3_scale(&coli1->ofs[1].x, (float *)&t2, -1.0f);
    if (coli1->flags & 1) {
      vec3_add_to(&coli1->ofs[7].x, &coli1->ofs[0].x);
    }
  }
}

void robs_max_distance_pull(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  float share0;
  float share1;
  float push[3];

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if (rob0->opp_dist < robs_max_distance)
    return;
  vec3_sub(push,&rob1->pos.x,&rob0->pos.x);
  push[1] = 0.0;
  vec3_set_length(robs_max_distance - rob0->opp_dist,push,push);
  if ((rob0->flags & 0x100) != 0 && (rob1->flags & 0x100) != 0)
    return;
  if ((rob0->flags & 0x100) != 0) {
    share0 = 0.0; share1 = 1.0;
  }
  else if ((rob1->flags & 0x100) != 0) {
    share0 = 1.0; share1 = 0.0;
  }
  else if ((rob0->state & 0x10) != 0 && (rob1->state & 0x10) != 0) {
    share1 = share0 = 0.5;
  }
  else if ((rob0->state & 0x10) != 0) {
    share0 = 0.2; share1 = 0.8;
  }
  else if ((rob1->state & 0x10) != 0) {
    share0 = 0.8; share1 = 0.2;
  }
  else {
    share1 = share0 = 0.5;
  }
  vec3_scale(&rob0->coli->ofs[5].x,push,-share0);
  vec3_scale(&rob1->coli->ofs[5].x,push,share1);
  vec3_add_to(&rob0->coli->ofs[7].x,&rob0->coli->ofs[5].x);
  vec3_add_to(&rob1->coli->ofs[7].x,&rob1->coli->ofs[5].x);
}

void robs_wall_coli(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  float d0;
  float d1;
  float v0[3];
  float v1[3];
  float out[3];

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if ((rob0->flags & 1) != 0) {
    rob_wall_coli_side(rob0,0);
    rob_coli_offset(rob0,&rob0->coli->ofs[2].x);
    rob_wall_coli_side(rob0,1);
    rob_coli_offset(rob0,&rob0->coli->ofs[3].x);
  }
  if ((rob1->flags & 1) != 0) {
    rob_wall_coli_side(rob1,0);
    rob_coli_offset(rob1,&rob1->coli->ofs[2].x);
    rob_wall_coli_side(rob1,1);
    rob_coli_offset(rob1,&rob1->coli->ofs[3].x);
  }
  if ((rob0->flags & rob1->flags & 1) != 0) {
    if (vec3_dot(&rob0->coli->ofs[4].x,&rob1->coli->ofs[4].x) >= 0.0) {
      rob_coli_offset(rob0,&rob0->coli->ofs[4].x);
      rob_coli_offset(rob1,&rob1->coli->ofs[4].x);
    }
    else if (rob0->opp_dist < 1.4) {
      vec3_rot_y_s16(0x2000,robs_push_dir,v0);
      vec3_rot_y_s16(-0x6000,robs_push_dir,v1);
      d0 = vec3_dot(v0,&rob0->coli->ofs[4].x);
      d1 = vec3_dot(v1,&rob0->coli->ofs[4].x);
      if (d0 > d1) {
        vec3_set_length(out,v1,vec3_length(&rob0->coli->ofs[4].x));
      }
      else {
        vec3_set_length(out,v0,vec3_length(&rob0->coli->ofs[4].x));
      }
      rob_coli_offset(rob0,out);
      d0 = vec3_dot(v0,&rob1->coli->ofs[4].x);
      d1 = vec3_dot(v1,&rob1->coli->ofs[4].x);
      if (d1 > d0) {
        vec3_set_length(out,v1,vec3_length(&rob1->coli->ofs[4].x));
      }
      else {
        vec3_set_length(out,v0,vec3_length(&rob1->coli->ofs[4].x));
      }
      rob_coli_offset(rob1,out);
    }
  }
}

void rob_wall_coli_side(rob_rec *rob, int bank)
{
  vec3 best_pt;
  vec3 hit;
  vec3 ctr;
  int idx;
  rob_coli_rec *coli;
  coli_poly_rec *best;
  float *pos;
  float *tri;
  float *rad;
  float bestd;
  uint mask;
  uint acc;
  int i;
  uint bit;
  coli_poly_rec *h;
  float d;
  short a;
  char dir;
  char flags;

  idx = (char)bank;
  coli = rob->coli;
  best = 0;
  pos = &coli->sph_a->x;
  tri = &coli->sph_b->x;
  rad = coli->rad_b;
  bestd = 0.0;
  mask = 0x36db18f;
  acc = 0;
  for (i = 26; i != 0; i--) {
    if ((mask & 1) != 0) {
      h = coli_sweep_hit(&hit.x,pos,tri,coli->field_0x3c[6],*rad);
      if (h != 0) {
        d = vec3_distance(&hit.x,pos);
        if (d > bestd) {
          best = h;
          bestd = d;
          best_pt = hit;
        }
        if (h->field_0xc > 0x1000 || (h->flags & 0x200U) != 0) {
          bit = 1 << (26 - i);
          vec3_sub_from(&hit.x,pos);
          if (__builtin_fabsf(h->normal[0]) > 1.1920929e-07 &&
              __builtin_fabsf(hit.x) > __builtin_fabsf(coli->ofs[idx + 2].x)) {
            coli->ofs[idx + 2].x = hit.x;
            acc = acc | bit;
          }
          if (__builtin_fabsf(h->normal[1]) > 1.1920929e-07 &&
              __builtin_fabsf(hit.y) > __builtin_fabsf(coli->ofs[idx + 2].y)) {
            coli->ofs[idx + 2].y = hit.y;
            acc = acc | bit;
          }
          if (__builtin_fabsf(h->normal[2]) > 1.1920929e-07 &&
              __builtin_fabsf(hit.z) > __builtin_fabsf(coli->ofs[idx + 2].z)) {
            coli->ofs[idx + 2].z = hit.z;
            acc = acc | bit;
          }
        }
      }
    }
    mask = mask >> 1;
    pos = pos + 3;
    rad = rad + 1;
  }
  coli->field_0x58 = coli->field_0x58 | acc;
  if ((rob->state & 0x40) == 0) {
    coli->ofs[idx + 2].y = 0.0;
  }
  if (!(vec3_length(&coli->ofs[idx + 2].x) > 0.0)) {
    return;
  }
  if ((rob->flags28 & 4) == 0) {
    rob->velocity.z = 0.0;
    rob->velocity.x = 0.0;
  }
  if (idx == 0 && best->field_0x0 == 0 && (rob->flags & 0x10000) != 0 &&
      (coli->flags & 0x1000U) == 0 && (best->flags & 0x400U) == 0 &&
      rob->frame < rob->active_end &&
      (rob->flags24 & 0x2000000) == 0 &&
      ((rob->state & 0x880) == 0 ||
       (rob->frame >= rob->field_0x2ee &&
        !(0.8 > rob->pos.y - rob->floor_y)))) {
    flags = 0;
    if ((rob->state & 0x200000) != 0) {
      short s;
      s = rob->field_0x312;
      if (s == -1) {
        s = rob->opp->throw_damage;
      }
      rob->pending_damage = s;
      if (s != 0 && rob->frame < rob->field_0x2f2) {
        rob_damage_apply(rob,2);
        rob->combo[0].hits = rob->combo[0].hits + 1;
      }
      if ((int)rob->flags24 < 0) {
        rob->field_0x220 = 1;
      }
      else {
        rob->field_0x220 = 0;
      }
      if (rob->opp->field_0x298 != -1) {
        rob->opp->recovery_end =
             rob->opp->field_0x298 > rob->opp->frame ?
             rob->opp->field_0x298 : rob->opp->frame + 1;
        if (rob->opp->active_end > rob->opp->recovery_end) {
          rob->opp->active_end = rob->opp->recovery_end;
        }
      }
      else if (rob->opp->react_recovery[1] != -1) {
        rob->opp->recovery_end =
             rob->opp->react_recovery[1] > rob->opp->frame ?
             rob->opp->react_recovery[1] : rob->opp->frame + 1;
        if (rob->opp->active_end > rob->opp->recovery_end) {
          rob->opp->active_end = rob->opp->recovery_end;
        }
      }
    }
    msnm_rule5_damage_set(rob);
    msnm_mission19_cpu_mark((char)rob->side);
    if (best->field_0xc == 0 && (rob->flags & 0x40) == 0) {
      ctr = *(vec3 *)best->verts[0];
      vec3_add_to(&ctr.x,best->verts[1]);
      vec3_add_to(&ctr.x,best->verts[2]);
      vec3_add_to(&ctr.x,best->verts[3]);
      vec3_scale(&ctr.x,&ctr.x,0.25);
      hit.x = stage_cur->center.x - rob->pos.x;
      hit.z = stage_cur->center.z - rob->pos.z;
      hit.y = 0.0;
      if (__builtin_fabsf(ctr.x) > __builtin_fabsf(ctr.z)) {
        ctr.z = rob->pos.z;
      }
      else {
        ctr.x = rob->pos.x;
      }
      if (__builtin_fabsf(hit.x) > __builtin_fabsf(hit.z)) {
        hit.z = 0.0;
      }
      else {
        hit.x = 0.0;
      }
      a = atan2_s16(hit.x,-hit.z) - rob->body_yaw;
      if ((a > 0x4000) | (a < -0x4000)) {
        a = a + -0x8000;
        dir = 0;
      }
      else {
        dir = 2;
      }
      rob->state = rob->state & 0xfffffeff;
      rob->heading = a;
      rob->flags = rob->flags | 0x40000;
      coli->ofs[idx + 2].x = ctr.x - rob->pos.x;
      coli->ofs[idx + 2].z = ctr.z - rob->pos.z;
      coli->ofs[idx + 2].y = 0.0;
      flags = flags | 1;
    }
    else {
      coli->ofs[idx + 2].y = 0.0;
      a = rob->body_yaw - best->field_0xe;
      if (a >= -0x2000 && a <= 0x2000) {
        dir = 0;
      }
      else if (a > 0x2000 && a < 0x6000) {
        dir = 1;
      }
      else if (a < -0x2000 && a > -0x6000) {
        dir = 3;
      }
      else {
        dir = 2;
      }
      rob->state = rob->state & 0xfffffeff;
      rob->heading = best->field_0xe + (dir << 14);
      flags = flags & ~1;
      if (rob->field_0x22a > 30) {
        camera_shake_request(1,5);
      }
      else {
        camera_shake_request(1,4);
      }
      if ((best->flags & 1U) != 0 &&
          (rob->field_0x220 != 0 ||
           ((user_back_data.option_bits & 0x10000000) != 0 &&
            (rob->flags & 8) != 0))) {
        rob->field_0x220 = 0;
        if (coli_flag_list_set((coli_wall_rec *)best->field_0x10) != 0) {
          fence_hit_segment = ((coli_wall_rec *)best->field_0x10)->wall_no;
          stage_break_flags = stage_break_flags | 8;
          coli->flags = coli->flags | 0x2000;
          flags = flags | 2;
          rob->velocity.x = 0.0;
          rob->velocity.z = 0.0;
          rob->knockback.x = 0.0;
          rob->knockback.z = 0.0;
          rob->slide_velocity.x = 0.0;
          rob->slide_velocity.z = 0.0;
          rob->flags24 = rob->flags24 | 0x20000008;
        }
      }
    }
    rob_wall_hit_react(rob,dir,flags);
    if ((coli->rob->state & 0x200600) != 0) {
      coli->rob->flags = coli->rob->flags | 0x8000000;
    }
  }
  coli->field_0x38c = vec3_length(&coli->ofs[idx + 2].x);
  coli->flags = coli->flags | 0x30;
  coli->field_0x98 = (int)best;
  if ((rob->opp->flags & 1) != 0 &&
      (rob->flags & 0x40000) == 0) {
    if ((ushort)(rob->move_no - 0x8b5U) < 2) {
      vec3_add_to(&rob->opp->coli->ofs[4].x,&coli->ofs[idx + 2].x);
    }
    else {
      float l;
      if (coli->touch_bits == 0) {
        return;
      }
      l = vec3_length(&coli->ofs[idx + 2].x);
      if ((rob->state & 0x21300600) != 0) {
        vec3_add_to(&rob->opp->coli->ofs[4].x,&coli->ofs[idx + 2].x);
      }
      else {
        if (!(l > rob->opp_dist)) {
          return;
        }
        vec3_set_length(&hit.x,&coli->ofs[idx + 2].x,l - rob->opp_dist);
        vec3_add_to(&rob->opp->coli->ofs[4].x,&hit.x);
      }
    }
  }
}

void robs_floor_check(void)
{
  rob_rec *rob;
  uint n;
  
  n = 0;
  do {
    rob = rob_get(n & 0xff);
    if ((rob->flags & 1) != 0) {
      rob_floor_check(rob);
    }
    n = n + 1;
  } while ((int)n < 2);
  return;
}

void rob_ring_out_set(rob_rec *rob)
{
  rob->flags = rob->flags & 0xfffeffff | 0x100;
  rob->ctrl->flags = rob->ctrl->flags | 0x20;
  game_flags = game_flags | 0x48;
  return;
}

void rob_ring_out_start(rob_rec *rob, short *table, char side_test)
{
  float away[3];
  float side_pt[3];
  float ahead_pt[3];
  float shift[3];
  short *tab;
  char probe;
  char hits;
  int ang;
  short turn;
  int ang2;
  short probe_ang;
  float x;
  float *center;
  uint base = 0x0c010000;

  tab = table;
  probe = side_test;
  center = &stage_cur->center.x;
  x = rob->pos.x;
  away[0] = x - *center;
  away[2] = rob->pos.z - stage_cur->center.z;
  away[1] = 0.0;
  if (__builtin_fabsf(away[0]) > __builtin_fabsf(away[2])) {
    away[2] = 0.0;
    rob->ring_out_point.x = x;
  }
  else {
    away[0] = 0.0;
    rob->ring_out_point.z = rob->pos.z;
  }
  ang = atan2_s16(away[0],-away[2]);
  vec3_set_length(&rob->push_velocity.x,away,ring_out_push_length);
  turn = ang - rob_move_facing(rob,0);
  if ((turn > 0x4000) | (turn < -0x4000)) {
    tab = tab + 1;
    ang = (short)(ang + -0x8000);
    if (probe != 0) {
      probe = probe + 1;
    }
  }
  ring_out_heading = ang;
  if ((rob->flags & 8) != 0) {
    rob_airborne_end(rob);
  }
  if (probe != 0) {
    hits = 0;
    probe = probe - 1;
    if (probe != 0) {
      ang2 = (short)(ang - 0x8000);
    }
    else {
      ang2 = ang;
    }
    probe_ang = ang2 + 0x6000;
    vec3_move_on_heading(probe_ang,&rob->pos.x,side_pt,0.7);
    vec3_move_on_heading(ang2,side_pt,ahead_pt,2.0);
    if (coli_segment_hit(ahead_pt,ahead_pt,side_pt,5) != 0) {
      hits = hits + 1;
    }
    probe_ang = ang2 - 0x6000;
    vec3_move_on_heading(probe_ang,&rob->pos.x,side_pt,0.7);
    vec3_move_on_heading(ang2,side_pt,ahead_pt,2.0);
    if (coli_segment_hit(ahead_pt,ahead_pt,side_pt,5) != 0) {
      hits = hits + 2;
    }
    if (hits > 0) if (hits < 3) {
      if ((rob->side_flags & 8) != 0) {
        hits = hits ^ 3;
      }
      tab = (short *)stage_cur->ring_out[5];
      tab = tab + (char)(hits - 1);
      tab = tab + probe * 2;
      if (probe == 0 && (rob->opp->flags & 0x100) != 0) {
        tab = tab + 4;
      }
      if (vec3_length(&rob->ring_out_point.x) > 0.0) {
        rob_coli_rec *coli;
        shift[0] = (rob->ring_out_point.x - rob->pos.x) - (coli = rob->coli)->ofs[8].x;
        shift[2] = (rob->ring_out_point.z - rob->pos.z) - coli->ofs[8].z;
        shift[1] = 0.0;
        rob_coli_offset(rob,shift);
      }
    }
  }
  rob->request_pending.word = base + *tab;
  rob->heading = ang;
  rob->state = rob->state & 0xfffffeff;
  rob_ring_out_set(rob);
}

void rob_ring_out_kind1(rob_rec *rob)
{
  short *table;
  
  table = (short *)*stage_cur->ring_out;
  if ((rob->opp->flags & 0x100) != 0) {
    table = table + 2;
  }
  rob_ring_out_start(rob,table,1);
  return;
}

void rob_ring_out_kind2(rob_rec *rob)
{
  short *table;
  table = (short *)*stage_cur->ring_out;
  table += 4;
  if ((rob->state & 0x800000) != 0) {
    table += 2;
  }
  rob_ring_out_start(rob,table,0);
}

void rob_floor_check(rob_rec *rob)
{
  uint all;
  uint stand;
  uint high;
  uint wall;
  uint air;
  char nstand;
  char nair;
  int near;
  float *box;
  int cnt;
  uchar bits;
  rob_coli_rec *coli;
  float *pt;
  float *rad;
  uint mask;
  int i;
  int action;
  uint f18;
  uint f20;
  int pend;
  float h;
  float gnd;
  float tol;
  float y;
  float len;
  float x;
  float z;
  float *wall_d;
  float *bound_d;
  short *wall_a;
  short *bound_a;
  int ang;
  float far;
  coli_poly_rec *hit;
  float buf[15];
  vec3 v;
  float probe_end[3];
  float hit_pt[3];
  float hit_dir[3];
  coli = rob->coli;
  all = 0;
  stand = 0;
  high = 0;
  wall = 0;
  air = 0;
  nstand = 0;
  nair = 0;
  near = 0;
  if ((coli->field_0x3c[3]) != 0)
  {
    near = 1;
  }
  gnd = field_floor_height((coli_poly_rec *) buf, &rob->pos.x, near, 0.5);
  if ((((rob->flags) & 0x100) != 0) || ((((volatile uint *) buf)[1] & 0x400) == 0))
  {
    rob->floor_y_kept = gnd;
  }
  rob->floor_y = gnd;
  rob->ring_out_point.x = 0.0;
  rob->ring_out_point.y = 0.0;
  rob->ring_out_point.z = 0.0;
  pt = &coli->sph_a->x;
  rad = coli->rad_b;
  mask = 0x32ca94f;
  if (((rob->flags) & 0x100) != 0)
  {
    mask = 0x1040009;
  }
  for (i = 0; i < 26; i++, mask >>= 1, pt += 3, rad++)
  {
    if ((mask & 1) != 0)
    {
      v = *((vec3 *) pt);
      v.y = (coli->field_0x380[0]);
      h = field_floor_height((coli_poly_rec *) buf, &v.x, *rad, near);
      tol = (*rad) + 0.01;
      y = pt[1];
      if ((((volatile uint *) buf)[1] & 0x80000400) == 0)
      {
        nstand++;
        if (((((rob->flags) & 0x100) == 0) && (y <= (h + tol))) && ((h - tol) <= y))
        {
          all |= 1 << i;
          stand |= 1 << i;
        }
      }
      else
      {
        air |= 1 << i;
        nair++;
        if ((y <= (h + tol)) && ((h - tol) <= y))
        {
          all |= 1 << i;
          high |= 1 << i;
        }
        else
          if (y <= ((rob->floor_y_kept) + tol))
        {
          wall |= 1 << i;
          box = &rob->ring_out_point.x;
          len = vec3_length(box);
          x = pt[0];
          z = pt[2];
          if (len < ((x * x) + (z * z)))
          {
            *box = x;
            rob->ring_out_point.z = z;
          }
        }
      }
    }
  }

  if ((mode_get_main() == MODE_MAIN_GAME) && ((mode_get_sub() < 10) || (mode_get_sub() > 14)))
  {
    goto done;
  }
  if (((rob->flags) & 0x140) != 0)
  {
    goto done;
  }
  if (((rob->flags24) & 0x20000000) != 0)
  {
    goto done;
  }
  if (((((rob->state) & 0x300000) != 0) && ((rob->frame) <= (rob->field_0x294))) && (((rob->flags) & 8) == 0))
  {
    goto done;
  }
  pend = 0;
  if (((rob->opp->flags) & 0x100) != 0)
  {
    if ((((rob->opp->flags24) & 0x40000000) != 0) && (nstand == 0))
    {
      pend = 1;
    }
  }
  if ((wall | pend) == 0)
  {
    goto done;
  }
  f18 = (rob->flags);
  if ((f18 & 0x48) == 0x48)
  {
    goto done;
  }
  f20 = (rob->state);
  action = 0;
  if ((f20 & 0x40000) == 0)
  {
    if (((f20 & 0x300000) != 0) && ((f18 & 8) == 0))
    {
      if ((nstand == 0) && (((f20 & 0x200000) != 0) || (((rob->flags24) & 0x10000000) != 0)))
      {
        action = 3;
      }
      else
        if ((((rob->move_flags) & 0x400) != 0) && ((rob->frame) >= ((rob->field_0x2f2) - 5)))
      {
        action = 2;
      }
      else
      {
        action = 1;
      }
    }
    else
      if (((f20 & 0x200) != 0) || (((((rob->move_flags) & 0x400) != 0) && ((rob->frame) < (rob->field_0x2ee))) && ((f20 & 0x840) == 0)))
    {
      action = 1;
    }
    else
      if (((f20 & 0x400) != 0) || (((rob->move_flags) & 0x400) != 0))
    {
      if (nair > 2)
      {
        if ((((rob->state) & 0x300000) != 0) && (((rob->flags) & 8) == 0))
        {
          action = (nstand == 0) ? (3) : (2);
        }
        else
          if (((rob->frame) < (rob->field_0x2ee)) && (((rob->state) & 0x40) == 0))
        {
          action = 1;
        }
        else
          if (nstand > 2)
        {
          action = 2;
        }
        else
          if (((((rob->action.part.lever) & 1) != 0) && (((rob->move_flags) & 0x40) != 0)) || (((rob->move_flags) & 0x4000) != 0))
        {
          action = 3;
        }
        else
          if (((rob->flags) & 8) != 0)
        {
          action = 4;
        }
        else
        {
          action = (nstand == 0) ? (3) : (2);
        }
      }
    }
    else
      if (((f20 & 0x40) != 0) && (nstand == 0))
    {
      action = 3;
    }
    else
    {
      action = 1;
    }
  }
  switch (action)
  {
    case 1:
      camera_shake_request_clear();
      all &= ~stand;
      stand = 0;
      rob_ring_out_kind1(rob);
      break;

    case 2:
      camera_shake_request_clear();
      all &= ~stand;
      stand = 0;
      rob_ring_out_kind2(rob);
      break;

    case 3:
      camera_shake_request_clear();
      all &= ~stand;
      stand = 0;
      rob->request_pending.word = 0x0c020000;
      rob_ring_out_set(rob);
      break;

    case 4:
      camera_shake_request_clear();
      all &= ~stand;
      stand = 0;
      rob_ring_out_set(rob);
      break;

  }

  done:
  coli->field_0x3c[1] = all;

  coli->field_0x3c[2] = stand;
  coli->field_0x3c[4] = high;
  coli->field_0x3c[3] = air;
  coli->field_0x3c[5] = wall;
  coli->field_0x90[0] = nair;
 coli->field_0x90[1] = nstand; f18 = (rob->flags); if (((((f18 & 8) != 0) && ((((~f18) & 0x40) != 0) && (all != 0))) && ((rob->pos.y) <= (((rob->jump_height) + (rob->floor_y)) + 0.1))) && (((f18 & 0x100) == 0) || (high != 0))) { rob_airborne_end(rob); } else if (((rob->move_flags) & 0x40) != 0) { if ((((all == 0) || ((rob->velocity.y) > 0.0)) || (((rob->flags) & 8) != 0)) && (((rob->state) & 0x800) == 0)) {
      rob->state |= 0x40;
    }
    else
    {
      rob->state &= ~0x40;
    }
  }
  v = rob->pos;
  far = 20.0;
  wall_d = rob->wall_dist;
  bound_d = rob->bound_dist;
  wall_a = rob->wall_angle;
  bound_a = rob->bound_angle;
  ang = (rob->facing);
  bits = 0;
  cnt = 4;
  do
  {
    vec3_move_on_heading((short) ang, (float *) (&v), probe_end, far);
    hit = coli_segment_hit(hit_pt, probe_end, (float *) (&v), 7);
    if (hit != 0)
    {
      vec3_sub(hit_dir, (float *) (&v), hit_pt);
      vec3_normalize_to(hit_dir, hit_dir);
      *bound_a = acos_ang16(vec3_dot(hit_dir, hit->normal));
      *bound_d = vec3_distance((float *) (&v), hit_pt);
      if (hit->field_0x0 == 0)
      {
        *wall_a = *bound_a;
        *wall_d = *bound_d;
        bits |= 1 << (4 - cnt);
      }
      else
        if (rob)
      {
        *wall_a = 0;
        *wall_d = far;
      }
      else
      {
        *wall_a = 0;
        *wall_d = far;
      }
    }
    else
    {
      *wall_d = far;
      *bound_d = far;
      *wall_a = 0;
      *bound_a = 0;
    }
    ang += 0x4000;
    wall_a++;
    wall_d++;
    bound_a++;
    bound_d++;
  }
  while ((--cnt) != 0);
  rob->wall_bits = bits;
}

void rob_coli_offset(rob_rec *rob, float *ofs)
{
  int i;
  float *p;

  if (1.1920929e-07 <= vec3_dot(ofs,ofs)) {
    p = &rob->coli->sph_a->x;
    for (i = 0x1a; i != 0; i--) {
      vec3_add_to(p,ofs);
      p = p + 3;
    }
    vec3_add_to(&rob->coli->ofs[8].x,ofs);
  }
}

void robs_coli_shift_apply(void)
{
  rob_rec *rob;
  float *shift;
  float *mtx;
  int i;
  uint n;
  
  for (n = 0; (int)n < 2; n++) {
    rob = rob_get(n & 0xff);
    if ((rob->flags & 1) != 0) {
      shift = &rob->coli->ofs[8].x;
      if (1.1920929e-07 <= (float)vec3_dot(shift,shift)) {
        mtx = rob->mtx + 0x10;
        for (i = 1; i < 0x17; i++, mtx += 0x10) {
          mtx[12] = mtx[12] + shift[0];
          mtx[13] = mtx[13] + shift[1];
          mtx[14] = mtx[14] + shift[2];
        }
        rob->place_pos.x = rob->place_pos.x + shift[0];
        rob->pos.x = rob->pos.x + shift[0];
        rob->ground_pos.x = rob->ground_pos.x + shift[0];
        rob->place_pos.z = rob->place_pos.z + shift[2];
        rob->pos.z = rob->pos.z + shift[2];
        rob->ground_pos.z = rob->ground_pos.z + shift[2];
      }
    }
  }
}

void cb_test_window_open(void)
{
  rob_rec *rob;
  chara_sphere_rec *sph;
  rob = rob_get(0);
  sph = *(chara_sphere_rec **)(rob->chara_data + 0x3c);
  cb_test_window_data = sph + ball_unit;
  cb_test_window_on = 1;
}

void cb_test_window_close(void)
{
  cb_test_window_on = 0;
  return;
}

void est_data_window_open(void)
{
  type = TYPE_NORMAL;
  est_data_window_gravity = gravity_mode;
  est_data_window_data = est_data;
  return;
}

void est_data_window_draw(void)
{
  rob_rec *rob;
  
  rob = rob_get(0);
  est_data_window_data = &est_data[(char)type];
  if (((char)type == 1) && (rob->hit_params != (est_row *)0x0)) {
    est_data_window_data = rob->hit_params;
  }
}

void coli_timing_test(void)
{
  int t;

  coli_test_nearest_hits.sphere = 0;
  coli_test_nearest_hits.segment = 0;
  coli_test_hits.sweep = 0;
  coli_test_hits.segment = 0;
  coli_test_hit_usec[0] = stopwatch_usec();
  coli_test_hits.segment = (uint)coli_segment_hit(coli_test_hit_points,coli_test_end,coli_test_start,0xd);
  t = stopwatch_usec();
  coli_test_hit_usec[0] = t - coli_test_hit_usec[0];
  coli_test_hit_usec[1] = stopwatch_usec();
  coli_test_hits.sweep = (uint)coli_sweep_hit(coli_test_hit_points + 3,coli_test_end,coli_test_start,0xd,coli_test_radius);
  t = stopwatch_usec();
  coli_test_hit_usec[1] = t - coli_test_hit_usec[1];
  coli_test_nearest_usec[0] = stopwatch_usec();
  coli_test_nearest_hits.segment = (uint)coli_segment_hit_nearest(coli_test_nearest_points,coli_test_end,coli_test_start,0xd);
  t = stopwatch_usec();
  coli_test_nearest_usec[0] = t - coli_test_nearest_usec[0];
  coli_test_nearest_usec[1] = stopwatch_usec();
  coli_test_nearest_hits.sphere = (uint)coli_sphere_hit_nearest(coli_test_nearest_points + 3,coli_test_end,coli_test_start,0xd,coli_test_radius);
  t = stopwatch_usec();
  coli_test_nearest_usec[1] = t - coli_test_nearest_usec[1];
  coli_test_cell_count = coli_grid_cells_on_segment(field_coli,coli_cells,coli_test_end[0],coli_test_end[2],coli_test_start[0],coli_test_start[2]);
  coli_test_cells_along = coli_grid_cells_along(coli_test_cells,coli_test_end,coli_test_start);
}
