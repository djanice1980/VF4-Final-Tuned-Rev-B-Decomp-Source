/*
 * vf4lib/libaction/src/coli_attack.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_vec3_set_length
#define KRPROTO_mot_frames
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar *guard_motion_tables[86]; /* 0x0c29fcdc */
extern uint hit_light_flags; /* 0x0c3728a8 */
extern char hit_attack_level; /* 0x0c4220c4 */
extern short hit_angle; /* 0x0c4220c6 */
extern int hit_attack_flags; /* 0x0c4220c8 */
extern int hit_attack_flags2; /* 0x0c4220cc */
extern int hit_result_flags; /* 0x0c4220d0 */
extern int hit_damage_kinds; /* 0x0c4220d4 */
extern float hit_power; /* 0x0c4220d8 */
extern est_row est_data[14]; /* 0x0c3713b4 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

static inline float fast_sqrtf(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline int bits_clear(uint v, uint m)
{
  return (v & m) == 0;
}

void rob_coli_atk_task_start(void)
{
  hit_row_low_damage = 0xf;
  hit_row_high_damage = 0x19;
  return;
}

void rob_coli_atk_task_frame(void)
{
  int held;
  
  held = pause_frame_held();
  if (held == 0) {
    robs_attack_step();
  }
  return;
}

void est_data_init(void)
{
  uint *dst;
  uint *src;
  int n;
  
  src = (uint *)est_data_default;
  n = 0xe;
  dst = (uint *)est_data;
  do {
    *dst = *src;
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[0xb] = src[0xb];
    src = src + 0xc;
    dst = dst + 0xc;
    n = n + -1;
  } while (n != 0);
  return;
}

void robs_attack_step(void)
{
  rob_rec *rob;
  rob_rec *rob2;
  uint n;
  
  n = 0;
  do {
    rob = rob_get(n & 0xff);
    if ((rob->flags & 1) != 0) {
      rob_attack_hit_check(rob);
    }
    n = n + 1;
  } while ((int)n < 2);
  robs_hits_resolve();
  robs_throw_try();
  hit_cmd51_count_apply();
  robs_frame_advantage_update();
  rob2 = rob_get(0);
  rob_combo_max_update(rob2);
  rob2 = rob_get(1);
  rob_combo_max_update(rob2);
  return;
}

void rob_combo_max_update(rob_rec *rob)
{
  short hits;
  short damage;
  short *p;

  hits = rob->combo[0].hits;
  damage = rob->combo[0].damage;
  if ((rob->flags & 0x400) == 0) {
    if ((rob->flags & 0x08000000) != 0) {
      if (rob->max_wall_combo < hits) {
        rob->max_wall_combo = hits;
      }
      p = &rob->max_wall_damage;
    }
    else {
      if (rob->max_combo < hits) {
        rob->max_combo = hits;
      }
      p = &rob->max_damage;
    }
    if (*p < damage) {
      *p = damage;
    }
    if (hits < rob->combo[1].hits) {
      hits = rob->combo[1].hits;
    }
    if (damage < rob->combo[1].damage) {
      damage = rob->combo[1].damage;
    }
    if (hits < rob->combo[2].hits) {
      hits = rob->combo[2].hits;
    }
    if (damage < rob->combo[2].damage) {
      damage = rob->combo[2].damage;
    }
    if (hits < rob->combo[4].hits) {
      hits = rob->combo[4].hits;
    }
    if (damage < rob->combo[4].damage) {
      damage = rob->combo[4].damage;
    }
    rob->combo_hits_best = hits;
    rob->combo_damage_best = damage;
  }
}

void rob_attack_hit_check(rob_rec *rob)
{
  rob_coli_rec *coli;
  rob_rec *opp;
  uint m;
  coli = rob->coli;
  opp = rob->opp;
  m = coli->touch_mask & coli->touch_bits;
  if (m == 0) return;
  if ((rob->state & 0x4000) == 0) return;
  if ((rob->field_0x30 & 2) == 0) return;
  if ((opp->flags24 & 8) != 0) return;
  if ((opp->flags & 0x100) != 0) return;
  if ((opp->flags & 0x40000) != 0) return;
  if ((rob->field_0x30 & 0x28) == 0 &&
      ((opp->state & 0x4000000) != 0 || (rob->flags & 0x20000) != 0)) {
    if ((opp->flags28 & 0x18) != 0) {
      if (lever_guard_check(opp, (opp->flags28 & 0x10) != 0 ? 2 : 1) == 1) {
        rob->side_flags |= 0x4000;
        return;
      }
    }
    else {
      if ((opp->move_flags & 0x4000) == 0) {
        rob->side_flags |= 0x4000;
        return;
      }
    }
  }
  switch (rob->attack_level) {
  case 8:
  case 9:
  case 11:
    if ((opp->flags24 & 0x8000) != 0) break;
  default:
    if ((opp->state & 0x40000) != 0) {
      if ((opp->move_flags & 0x4000) == 0) return;
      if (rob->reach_flags2 == 0) return;
      if ((opp->side_flags & 0x20) != 0) return;
    }
    if (msnm_rule_is(2) == 0 && (opp->state & 0x1800) == 0x1800 && (opp->flags & 0x200000) == 0) return;
    if ((opp->state & 0x200400) == 0x200400 && (opp->flags24 & 0x800) == 0) return;
    break;
  case 10:
  case 12:
  case 14:
    break;
  }
  if ((opp->state & 0x1000200) == 0 || (opp->state & 1) == 0) {
    switch (rob->attack_level) {
    case 1:
      if ((opp->flags & 0x10) != 0 || (opp->flags2c & 1) != 0) return;
      break;
    case 3:
    case 5:
    case 6:
      if (opp->coli->field_0x3c[1] == 0 && (opp->state & 0x400) == 0 &&
          (opp->state & 0xc0) != 0) return;
      break;
    case 7:
      if ((opp->flags2c & 1) != 0) return;
      if ((opp->move_flags & 0x4000) == 0 && (opp->flags & 0x10) != 0) return;
      if (opp->attack_level == 5) return;
      break;
    case 8:
      if ((opp->flags & 0x10) != 0) return;
      if ((opp->state & 0x40) != 0) return;
      if (opp->coli->field_0x3c[1] == 0 && (opp->move_flags & 0x4c0) != 0) return;
      break;
    case 11:
      if ((opp->flags & 0x10) != 0) return;
      break;
    case 9:
      if ((opp->flags & 0x10) == 0 && (opp->state & 0x8000000) == 0) return;
      break;
    case 13:
    default:
      break;
    }
  }
  coli->field_0x1c[1] |= m;
}

void robs_hits_resolve(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  rob_coli_rec *coli0;
  rob_coli_rec *coli1;
  int hit0;
  int hit1;
  short damage0;
  short damage1;
  short frame0;
  short frame1;
  short d;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  coli0 = rob0->coli;
  coli1 = rob1->coli;
  hit0 = coli0->field_0x1c[1];
  hit1 = coli1->field_0x1c[1];
  rob0->last_damage = rob0->last_damage_2 = 0;
  rob1->last_damage = rob1->last_damage_2 = 0;
  if ((hit0 != 0) & (hit1 != 0)) {
    damage0 = rob0->attack_damage;
    damage1 = rob1->attack_damage;
    frame0 = rob0->frame;
    frame1 = rob1->frame;
    if (damage0 > damage1) {
      attack_connect(rob0,rob1);
    }
    else if (damage1 > damage0) {
      attack_connect(rob1,rob0);
    }
    else if (frame0 > frame1) {
      attack_connect(rob0,rob1);
    }
    else if (frame1 > frame0) {
      attack_connect(rob1,rob0);
    }
    else {
      d = rob0->life - rob1->life;
      if (d > 49) {
        attack_connect(rob1,rob0);
      }
      else if (d > -50) {
        attack_connect(rob0,rob1);
        attack_connect(rob1,rob0);
      }
      else {
        attack_connect(rob0,rob1);
      }
    }
  }
  else if (hit0 != 0) {
    attack_connect(rob0,rob1);
  }
  else if (hit1 != 0) {
    attack_connect(rob1,rob0);
  }
  if (coli0->field_0x94 != 0 && (coli0->flags & 0x800) == 0) {
    rob0->guard_input_hold = 0;
  }
  if (coli1->field_0x94 != 0 && (coli1->flags & 0x800) == 0) {
    rob1->guard_input_hold = 0;
  }
}

void attack_recovery_apply(rob_coli_rec *coli)
{
  short hit_end;
  short counter_end;
  int r;
  uint result;
  
  result = coli->field_0x3c[0];
  if ((((result != 0) &&
       ((r = msnm_rule_get(), r != 9 ||
        (r = msnm_combo_active(coli->rob->side), r == 0)))) &&
      (((result & 0x20) == 0 ||
       (r = move_recovery_set(coli,(int)coli->rob->react_recovery[0]), r == 0)))) &&
     ((((result & 0x40) == 0 ||
       (r = move_recovery_set(coli,(int)coli->rob->react_recovery[4]), r == 0)) &&
      (((coli->rob->opp->state & 0x1000000) == 0 ||
       (((result & 1) == 0 ||
        (r = move_recovery_set(coli,(int)coli->rob->react_recovery[3]), r == 0)))))))) {
    if ((result & 2) != 0) {
      counter_end = coli->rob->react_recovery[2];
      if (((result & 0x4000) != 0) && (coli->rob->result_rows[0].recovery != -1)) {
        counter_end = coli->rob->result_rows[0].recovery;
      }
      if (((result & 0x8000) != 0) && (coli->rob->result_rows[1].recovery != -1)) {
        counter_end = coli->rob->result_rows[1].recovery;
      }
      if (((result & 0x10000) != 0) && (coli->rob->result_rows[2].recovery != -1)) {
        counter_end = coli->rob->result_rows[2].recovery;
      }
      if (move_recovery_set(coli,counter_end) != 0) {
        return;
      }
    }
    if ((result & 0x81) != 0) {
      hit_end = coli->rob->react_recovery[1];
      if ((result & 0x800) != 0) {
        hit_end = coli->rob->result_rows[3].recovery;
      }
      move_recovery_set(coli,(int)hit_end);
    }
  }
  return;
}

int move_recovery_set(rob_coli_rec *coli, int frame)
{
  int rule;
  int set;
  
  set = 0;
  if (frame != -1) {
    if (coli->rob->frame >= frame) {
      frame = coli->rob->frame + 1;
    }
    coli->rob->recovery_end = (short)frame;
    rule = msnm_rule_get();
    if (rule == 9) {
      msnm_combo_d0_set(coli->rob->side,frame);
    }
    set = 1;
  }
  return set;
}

void attack_connect(rob_rec *attacker, rob_rec *defender)
{
  char c;
  est_row *row;
  byte idx;
  short damage;
  short angle;
  int flags;

  hit_params_custom_load(attacker);
  if (msnm_rule_get() == 9) {
    msnm_combo_reset(defender->side);
  }
  idx = attacker->hit_param_row;
  damage = attacker->attack_damage;
  angle = attacker->attack_angle + attacker->heading;
  flags = attacker->field_0x328;
  row = est_data + idx;
  hit_est_record = row;
  hit_power = fast_sqrtf(damage * 0.033333335) * 30.0 * 0.0025 * row->f[0];
  hit_attack_flags = flags;
  hit_angle = angle;
  hit_attack_flags2 = attacker->attack_flags2;
  hit_attack_level = attacker->attack_level;
  attacker->slot = attacker->attack_limb;
  defender->pending_damage = damage;
  hit_damage_kinds = 0;
  attacker->field_0x30 &= 0xfd;
  control_resume_clear((char)defender->side);
  defender->flags &= 0xfffbffff;
  rob_slot_pos_get(attacker,&hit_light_pos.x);
  hit_light_damage = defender->pending_damage;
  if (defender->side != 0) {
    hit_light_flags |= 0x40;
  }
  else {
    hit_light_flags &= 0xffffffbf;
  }
  if ((uint)(hit_attack_level - 8) <= 3) {
    attacker->field_0x30 |= 2;
    if (defender->flags28 & 0x40000000) {
      if ((user_back_data.option_bits & 0x1000) == 0) return;
    }
    else if ((defender->state & 2) && (defender->move_flags & 0x4000)) {
      if ((user_back_data.option_bits & 0x2000) == 0) return;
    }
    attacker->coli->field_0x94 = attacker->field_0x115 = 1;
    attacker->coli->field_0x3c[0] = attacker->field_0x10c = 1;
  }
  else {
    defender->coli->flags |= 0x1000;
    c = attacker->attack_history_idx;
    attacker->attack_history[c] = attacker->move_no;
    c++;
    attacker->attack_history_idx = c > 15 ? 0 : c;
    if (attack_vs_guard(attacker,defender) != 0) return;
    if ((defender->flags24 & 0x400) && hit_catch_mask_check(attacker,defender) != 0) return;
    if ((defender->flags24 & 0x800000) && hit_reversal_move_check(attacker,defender) != 0) return;
    attacker->coli->field_0x94 = attacker->field_0x115 = 1;
    attack_hit_resolve(attacker,defender);
  }
}

void attack_hit_resolve(rob_rec *attacker, rob_rec *defender)
{
  int motion;
  uint flags;
  uint sound_mask;
  uint shake_mode;
  uint def_state;
  int damage;
  int add;
  int row_idx;
  int t;
  hit_damage_row_rec *row;
  int atk_damage;
  uint hit_spheres;
  short *cnt;
  int i;
  int launch;
  int life;
  short n;
  rob_coli_rec *coli;
  rob_coli_rec *coli2;
  rob_coli_rec *coli3;
  int scaled;
  float f;
  float m;

  coli = attacker->coli;
  motion = coli->field_0x94;
  flags = coli->field_0x3c[0];
  hit_spheres = defender->coli->touched_by_masked;
  cnt = defender->coli->sph_flag;
  i = 0x1a;
  do {
    if ((hit_spheres & 1) != 0) {
      *cnt = *cnt + 1;
    }
    cnt = cnt + 1;
    hit_spheres = hit_spheres >> 1;
    i = i + -1;
  } while (i != 0);
  flags |= 0x1001;
  sound_mask = 0x80;
  shake_mode = 1;
  hit_light_flags |= 2;
  attacker->event_flags |= 1;
  if ((attacker->relation_flags & 2) != 0) {
    flags |= 0x2000;
  }
  attacker->field_0xfa = 0;
  defender->field_0x104 = 0;
  defender->ctrl->dir_branch_list = 0;
  defender->ctrl->field_0x70 = 0;
  add = 0;
  damage = defender->pending_damage;
  if ((((defender->move_flags & 0x4000) != 0) && ((defender->state & 1) != 0)) ||
      ((defender->flags2c & 0x20) != 0)) {
    def_state = defender->state;
    if (((def_state & 4) == 0) && ((defender->flags2c & 0x20) == 0)) {
      flags &= ~0x1000;
      flags |= 2;
      sound_mask |= 0x20;
      shake_mode |= 2;
      attacker->event_flags |= 2;
      hit_light_flags |= 4;
    }
    if (((def_state & 4) != 0) || ((defender->flags2c & 0x20) != 0)) {
      if (msnm_rule_is(7) != 0) {
        t = (int)((float)damage * (msnm_rule_params_get()->field_0x14 - 1.0));
      }
      else {
        t = damage >> 2;
      }
      add = (short)t;
      motion = 0x12;
      flags |= 0x800;
      flags &= ~0x1000;
    }
    else {
      atk_damage = defender->attack_damage;
      row_idx = 1;
      if ((defender->flags28 & 0x800000) != 0) {
        atk_damage = rob_pending_attack_damage((char)defender->side);
      }
      if (((user_back_data.option_bits & 0x10000) != 0) && (hit_row_high_damage <= atk_damage)) {
        row_idx = 2;
      }
      if (hit_row_low_damage >= atk_damage) {
        row_idx = 0;
      }
      row = hit_damage_rows + row_idx;
      f = row->damage_scale;
      if (msnm_rule_is(7) != 0) {
        f = msnm_rule_params_get()->field_0x10 - 1.0;
      }
      add = (short)(int)((float)damage * f);
      motion = row->motion;
      flags |= row->result_flags;
      hit_light_flags |= row->light_flags;
      sound_mask |= row->sound_mask;
      if (row->shake != 0) {
        camera_shake_request(1,1);
      }
    }
    if ((def_state & 0x40) != 0) {
      motion = 7;
      flags |= 4;
    }
  }
  else {
    if ((defender->state & 0x40) != 0) {
      motion = 6;
      flags |= 4;
    }
  }
  damage = (short)(damage + add);
  control_buffer_use((char)attacker->side,0);
  if ((flags & 2) != 0) {
    control_buffer_use((char)attacker->side,1);
  }
  if ((defender->flags & 0x200000) == 0) {
    if ((((hit_attack_level == 0xc) | (hit_attack_level == 0xe)) == 0) && ((defender->state & 0x800) != 0)) {
      if (msnm_rule_is(3) != 0) {
        damage = (short)(int)((float)damage * msnm_rule_params_get()->field_0xc);
      }
      else {
        damage = damage >> 1;
      }
      defender->field_0x224 = 1;
    }
    if ((defender->state & 0xc00) != 0) {
      damage = (short)(int)((float)damage * attacker->field_0x368);
    }
  }
  else {
    defender->flags &= 0xffdfffff;
  }
  if ((flags & 4) == 0) {
    if ((defender->state & 0x20000000) != 0) {
      if (((hit_attack_flags & 0x8000000) != 0) && ((defender->move_flags & 0x2000) == 0)) {
        motion = 0x10;
        flags |= 0x200;
      }
      else if (((hit_attack_flags & 0x10000000) != 0) && ((defender->move_flags & 0x2000) != 0)) {
        motion = 0x10;
        flags |= 0x200;
      }
    }
    else {
      if (((hit_attack_flags & 0x2000000) != 0) && ((defender->move_flags & 0x2000) == 0)) {
        motion = 0x10;
        flags |= 0x200;
      }
      else if (((hit_attack_flags & 0x4000000) != 0) && ((defender->move_flags & 0x2000) != 0)) {
        motion = 0x10;
        flags |= 0x200;
      }
    }
  }
  if ((defender->field_0x224 == 0) &&
      (((defender->state & 0x40) != 0) ||
       ((defender->coli->field_0x14 & 8) != 0) ||
       ((defender->action.part.type == 8) &&
        (((defender->action.part.lever & 8) != 0) ||
         (((defender->move_flags & 0x400) != 0) &&
          (defender->frame >= defender->field_0x2f2) &&
          (move_cmd_arg_find((short)defender->move_no,0,2) != 0)))))) {
    if (msnm_rule_is(3) != 0) {
      damage = (short)(int)((float)damage * msnm_rule_params_get()->field_0x4);
    }
    else {
      damage = (short)(int)((float)damage * 0.8);
    }

  }
  if (msnm_rule11_cpu_check((char)defender->side) == 0) {
    life = defender->life;
    scaled = damage;
    n = defender->combo[2].hits;
    if (life <= damage) {

      n = n + 1;
    }
    if (n > 3) {
      f = (float)scaled;
      m = 0.8;
      n = n - 3;
      if (msnm_rule_is(3) != 0) {
        m = msnm_rule_params_get()->field_0x8;
      }
      if (msnm_rule9_combo_check(defender) != 0) {
        m = m * 0.5;
      }
      while (n-- != 0) {
        f = f * m;
      }
      scaled = (short)(int)f;
    }
    if ((((((int)defender->flags >> 10) & 1) == 0) & (scaled >= life)) != 0) {
      flags |= 0x10;
      sound_mask |= 0x10;
      shake_mode |= 4;
      if (((flags & 4) != 0) || ((defender->state & 0x400) != 0)) {
        motion = 9;
      }
      else {
        motion = 8;
      }
    }
  }
  defender->pending_damage = damage;
  coli2 = attacker->coli;
  attacker->field_0x115 = motion;
  coli2->field_0x94 = motion;
  coli3 = attacker->coli;
  attacker->field_0x10c = flags;
  hit_result_flags = flags;
  coli3->field_0x3c[0] = flags;

  attack_recovery_apply(attacker->coli);
  launch = 0;
  if ((defender->state & 0x10040c40) != 0) {
    launch = 1;
  }
  if ((defender->flags24 & 0x40) != 0) {
    launch = 1;
  }
  if (*(int *)(defender->reaction + 0x14) - 2U < 2) {
    launch = 1;
  }
  if (((defender->state & 0x20000000) != 0) && ((hit_attack_flags & 0x2000) != 0)) {
    launch = 1;
  }
  if ((flags & 0x10) != 0) {
    launch = 1;
  }
  if ((defender->flags & 0x400) != 0) {
    launch = 1;
  }
  else if ((defender->state & 0x1000000) != 0) {
    if (hit_attack_flags < 0) {
    }
    else if (((hit_attack_level == 2) | (hit_attack_level == 4)) != 0) {
      launch = 1;
    }
    else if ((hit_attack_flags & 0x1804400) != 0) {
      launch = 1;
    }
    else if (launch_damage_min > damage) {
    }
    else if ((hit_attack_flags & 0x8000) == 0) {
      launch = 1;
    }
  }
  else if ((((hit_attack_flags2 & 0x400) != 0) && ((flags & 0x10000) != 0)) ||
           (((hit_attack_flags2 & 0x100) != 0) && ((flags & 0x18000) != 0)) ||
           (((hit_attack_flags2 & 0x40) != 0) && ((flags & 2) != 0))) {
    launch = 1;
  }
  else if ((((hit_attack_flags & 0x40000000) != 0) && ((flags & 2) != 0)) ||
           (((hit_attack_flags2 & 0x800) != 0) && ((flags & 0x10000) != 0)) ||
           (((hit_attack_flags2 & 0x200) != 0) && ((flags & 0x18000) != 0)) ||
           (((hit_attack_flags2 & 0x80) != 0) && ((flags & 0x4000) != 0)) ||
           (((hit_attack_flags & 0x100000) != 0) && ((flags & 0x800) != 0))) {
  }
  else if ((hit_attack_flags & 0x400) != 0) {
    launch = 1;
  }
  else if ((((hit_attack_flags & 0x1000) != 0) && ((flags & 2) != 0)) ||
           (((hit_attack_flags2 & 0x20) != 0) && ((flags & 0x800) != 0)) ||
           (((hit_attack_flags & 0x800) != 0) && ((defender->state & 0x2000) == 0)) ||
           (((hit_attack_flags2 & 1) != 0) && ((defender->relation_flags & 1) != 0)) ||
           (((hit_attack_flags2 & 2) != 0) && ((defender->relation_flags & 1) != 0) && ((flags & 2) != 0)) ||
           (((hit_attack_flags2 & 4) != 0) && ((defender->relation_flags & 1) != 0) && ((flags & 0x800) != 0))) {
    launch = 1;
  }
  else if (launch_damage_min > damage) {
  }
  else if ((hit_attack_flags & 0x8000) != 0) {
  }
  else if (((hit_attack_flags2 & 8) == 0) || ((defender->relation_flags & 1) == 0)) {
    launch = 1;
  }
  cnt = &defender->combo[0].hits;
  *cnt = *cnt + 1;
  hit_damage_kinds |= 2;
  if (*cnt == 1) {
    if ((((attacker->attack_cmd_flags & 0x10) != 0) && ((attacker->field_0x10c & 2) != 0)) ||
        (((attacker->attack_cmd_flags & 0x20) != 0) && ((attacker->field_0x10c & 0x802) != 0))) {
      attacker->field_0x116 = attacker->field_0x115;
    }
    else {
      attacker->field_0x116 = 0;
    }
  }
  else if ((defender->combo[0].hits == 2) && (attacker->field_0x116 != 0) &&
           ((attacker->side_flags & 0x100) != 0)) {
    attacker->attack_cmd_flags |= 2;
    attacker->field_0x116 = 0;
  }
  if (hit_cmd49_absorb_check(attacker,defender,launch) == 0) {
    control_buffer_clear((char)defender->side);
    control_pending_clear((char)defender->side);
    defender->slow_frames = 0;
    defender->slow_rate = 0.0;
    rob_voice_cut(defender);
    if (launch != 0) {
      shake_mode |= 8;
      defender->combo[2].hits = defender->combo[2].hits + 1;
      hit_damage_kinds |= 8;
      if ((defender->state & 0x40) != 0) {
        defender->combo[1].hits = defender->combo[1].hits + 1;
        hit_damage_kinds |= 4;
      }
      if ((defender->state & 0x440) != 0) {
        defender->combo[3].hits = defender->combo[3].hits + 1;
        hit_damage_kinds |= 0x10;
      }
      if ((hit_attack_flags & 0x20000000) != 0) {
        defender->field_0x220 = 1;
        defender->flags |= 0x10000;
      }
      rob_launch_react(attacker,defender);
    }
    else if (motion == 0x10) {
      rob_hit_react_kind16(attacker,defender);
    }
    else {
      if ((((hit_attack_flags & 0x800000) != 0) && ((defender->flags & 0x10) == 0)) ||
          (((hit_attack_flags & 0x1000000) != 0) && ((defender->flags & 0x10) != 0)) ||
          (((hit_attack_flags & 0x40000000) != 0) && ((flags & 2) != 0)) ||
          (((hit_attack_flags2 & 0x800) != 0) && ((flags & 0x10000) != 0)) ||
          (((hit_attack_flags2 & 0x200) != 0) && ((flags & 0x18000) != 0)) ||
          (((hit_attack_flags2 & 0x80) != 0) && ((flags & 0x4000) != 0)) ||
          (((hit_attack_flags & 0x100000) != 0) && ((flags & 0x800) != 0))) {
        hit_result_guard_knockback(attacker,defender,2,0);
        return;
      }
      rob_hit_react(attacker,defender);
    }
    attack_result_sounds_play(attacker,defender,sound_mask);
    move_hit_camera_shake(attacker,shake_mode);
    rob_damage_apply(defender,hit_damage_kinds);
  }
  return;
}

int rob_throw_pick(rob_rec *rob)
{
  throw_pick_rec *e;
  uint reach;
  uint level_mask;
  rob_rec *opp;
  int preset;
  uint move_word;
  uint *entry;
  int word;
  uint arg;
  ushort move;

  reach = rob->reach_flags2;
  switch (rob->attack_level) {
  case 8:
    level_mask = 0xf000555;
    break;
  case 0xb:
    level_mask = 0xf00fd55;
    break;
  case 9:
    level_mask = 0x2aa;
    break;
  case 10:
    level_mask = 0xff0000;
    break;
  default:
    level_mask = 0;
    break;
  }
  if ((reach & level_mask) == 0) return 0;
  if ((reach & rob->throw_request) != 0) return 1;
  opp = rob->opp;
  if ((opp->flags28 & 0x40000000) == 0 && (opp->state & 0x4000) != 0 &&
      (opp->flags28 & 0x1000000) == 0 && (opp->flags24 & 0x8000) == 0) {
    return 0;
  }
  preset = rob->preset_throw;
  if (preset >= 0 && (move_word = rob->preset_throw_move) != 0 && (reach & (1 << preset)) != 0) {
    rob->request_pending.word = move_word;
    if ((move_word & 0x5000000) != 0) {
      rob->request_pending.word = move_word | 0x80000;
    }
    rob_voice_cut(rob->opp);
    return 1;
  }
  for (e = throw_pick_table; e->mask != 0; e++) {
    if ((reach & e->mask) == 0) continue;
    entry = move_event_entry_find(rob, e->event);
    word = e->code + 0x5080000;
    if (entry == 0) {
      if (e->move_idx < 0) break;
      move = rob->move_table[e->move_idx];
      if (move == 0) break;
      word = word + move;
    }
    else {
      arg = entry[2];
      if ((arg & 0xff0000) != 0) {
        word = arg + 0x5080000;
      }
      else {
        word = word + arg;
      }
    }
    rob->request_pending.word = word;
    rob->ctrl->field_0x70 = 0;
    rob_voice_cut(rob->opp);
    return 1;
  }
  return 0;
}

void robs_throw_try(void)
{
  int side;
  
  side = 0;
  do {
    rob_throw_try(side);
    side = side + 1;
  } while (side < 2);
  return;
}

void rob_throw_try(uint side)
{
  rob_rec *rob;
  rob_rec *opp;
  uint *entry;
  uint word;
  uint result;
  
  rob = rob_get(side & 0xff);
  opp = rob_get((side ^ 1) & 0xff);
  result = rob->coli->field_0x3c[0];
  if ((hit_wait_timer_get() != 0) && ((opp->flags24 & 0x8000) == 0)) return;
  if ((result & 0x21) == 0) return;
  if ((rob->flags28 & 0x40000000) != 0) {
    if (rob_throw_pick(rob) != 1) return;
    attack_result_sounds_play(rob,opp,0x40);
    return;
  }
  entry = move_event_entry_find(rob,0x40);
  if (entry == (uint *)0x0) return;
  word = *entry;
  if ((word & 2) != 0) {
    if ((result & 2) == 0) return;
  }
  else if ((word & 1) != 0) {
    if ((result & 1) == 0) return;
  }
  if (((word & 0x80) != 0) && ((rob->relation_flags & 1) != 0)) return;
  if (((word & 0x100) != 0) && ((rob->relation_flags & 1) == 0)) return;
  rob->request_pending.word = entry[2] + 0x5080000;
  attack_result_sounds_play(rob,opp,0x40);
}

void rob_hit_react(rob_rec *attacker, rob_rec *defender)
{
  int word;
  int stun;
  int combo;
  word = hit_reaction_move_pick(attacker,defender,(int)attacker->coli->field_0x94);
  word += 0x7000000;
  defender->request_pending.word = word;
  defender->ctrl->continuation = 0;
  defender->ctrl->wait = 0;
  stun = hit_stun(attacker,defender);
  defender->stun = (short)stun;
  hit_reaction(attacker,defender,stun);
  combo = msnm_rule9_combo_check(attacker);
  if (combo != 0) {
    defender->stun = (short)(int)((float)stun * 0.5);
    hit_wait_timer_set(0);
  }
  hit_knockback_set(defender,0);
  return;
}

void rob_wall_hit_react(rob_rec *rob, char wall_side, char wall_flags)
{
  typedef struct { short a; short b; } pair_s16;
  short move;
  int bit23_clear;
  int col;
  float v[3];
  int *src;
  char *pi;

  if ((rob->state & 0x440) != 0 || (rob->flags & 0x400) != 0) {
    if ((rob->state & 0x800000) == 0) bit23_clear = 1; else bit23_clear = 0;
    col = !bit23_clear;
    if ((wall_flags & 1) != 0) {
      src = (int *)&rob->pos;
      *(int *)&v[0] = src[0];
      *(int *)&v[2] = src[2];
      v[1] = 0.0;
      if (__builtin_fabsf(v[0]) < 1.1920929e-07 && __builtin_fabsf(v[2]) < 1.1920929e-07) {
        v[0] = 1.0;
      }
      vec3_set_length(v,v,wall_bounce_push);
      rob->push_velocity.x = v[0];
      rob->push_velocity.z = v[2];
      rob->push_velocity.y = (float)wall_bounce_rise * world_gravity;
      rob->request_pending.word = wall_react_bounce_moves[wall_side][col] + 0x8080000;
    }
    else {
      rob->request_pending.word = wall_react_down_moves[wall_side][col] + 0x8080000;
    }
    rob->knockback.x = 0.0;
    rob->knockback.z = 0.0;
    rob->knockback.y = 0.0;
  }
  else {
    switch (rob->field_0x314) {
    case 1:
      move = ((pair_s16 *)&wall_react_stand_moves)[wall_side].a;
      break;
    case 2:
      pi = (char *)(wall_side * 4);
      move = *(short *)(pi + (int)&wall_react_stand_moves + 2);
      break;
    default:
      if (rob->pending_damage > 0x14) {
        pi = (char *)(wall_side * 4);
        move = *(short *)(pi + (int)&wall_react_stand_moves + 2);
      }
      else if (rob->field_0x22a > 0x14) {
        pi = (char *)(wall_side * 4);
        move = *(short *)(pi + (int)&wall_react_stand_moves + 2);
      }
      else {
        move = ((pair_s16 *)&wall_react_stand_moves)[wall_side].a;
      }
    }
    rob->request_pending.word = 0x0d010000 + move;
    rob->stun = 0;
    hit_wait_timer_set((short)(reaction_length_get(move,0) + 3));
    rob->knockback.x = 0.0;
    rob->knockback.y = 0.0;
    rob->knockback.z = 0.0;
  }
  rob->flags = rob->flags & 0xfffeffff;
  rob->field_0x106 = 0x78;
  rob->ctrl->continuation = 0;
  rob->ctrl->wait = 0;
  if ((wall_flags & 2) != 0) {
    attack_result_sounds_play(rob,rob,0x2000);
  }
  else {
    attack_result_sounds_play(rob,rob,0x400);
  }
}

void rob_hit_react_kind16(rob_rec *attacker, rob_rec *defender)
{
  ushort move;
  int stun;
  uint word;
  move = hit_reaction_move_pick(attacker,defender,(int)attacker->coli->field_0x94);
  word = move + 0x11000000;
  defender->request_pending.word = word;
  defender->ctrl->continuation = 0;
  defender->ctrl->wait = 0;
  stun = hit_stun(attacker,defender);
  defender->stun = (short)stun;
  attack_recovery_apply(attacker->coli);
  hit_reaction(attacker,defender,reaction_length_get((int)(short)move,stun));
  hit_knockback_set(defender,0);
  hit_light_flags = hit_light_flags | 8;
  control_buffer_use((char)attacker->side,3);
}

void rob_launch_react(rob_rec *attacker, rob_rec *defender)
{
  rob_coli_rec *coli;
  uint result;
  int kind;
  int move;
  int m;
  short *launch_arg;
  float *power_scale;
  short *angle_src;
  short repeats;
  short i;
  short angle_min;
  short mul;
  short combo_n;
  short angle[1];
  uint def_state;
  uint word;
  short lo;
  short move_id;

  defender->flags = defender->flags & 0xfffbffff | 0x10000;
  defender->flags24 = defender->flags24 & 0xdfffffff;
  defender->ctrl->continuation = 0;
  defender->ctrl->wait = 0;
  coli = attacker->coli;
  result = hit_result_flags + 8;
  kind = coli->field_0x94;
  if (__builtin_fabsf(defender->pos.y - defender->floor_y) <= launch_ground_tolerance) {
    defender->field_0x223 = defender->field_0x223 + 1;
  }
  if ((result & 4) != 0) goto store;
  if ((result & 0x10) != 0) {
    if ((defender->move_flags & 0x400) == 0) goto store;
    def_state = defender->state;
    if ((def_state & 0x40400) == 0) goto store;
    if (attacker->attack_level == 12 ||
        (attacker->attack_level == 14 && (attacker->opp->state & 0x800) != 0)) goto k14;
    if (defender->frame >= defender->field_0x2f2 || (def_state & 0x800) != 0 ||
        (defender->old_cur->flags & 8) != 0) goto k15;
    if (kind != 9) kind = 2;
  }
  else {
    def_state = defender->state;
    kind = 2;
    if ((def_state & 0x40400) == 0) goto store;
    if ((attacker->attack_level == 12) | (attacker->attack_level == 14)) {
k14:
      kind = 14;
      result |= 0x100;
    }
    else if (defender->frame >= defender->field_0x2f2 || (def_state & 0x800) != 0 ||
        (defender->old_cur->flags & 8) != 0) {
k15:
      kind = 15;
    }
    else if ((defender->flags24 & 0x1000) != 0) {
      kind = 6;
    }
  }
store:
  attacker->field_0x10c = result;
  coli->field_0x3c[0] = result;
  attacker->field_0x115 = kind;
  coli->field_0x94 = kind;
  move = hit_reaction_move_pick(attacker,defender,kind);
  m = move & 0x7fff;
  word = 0x08000000 + move;
  defender->request_pending.word = word;
  angle[0] = 0;
  launch_arg = (short *)move_cmd_arg_find((short)m,0,14);
  if (m == 0x136 && (uchar)(attacker->hit_param_row - 8) <= 1) launch_arg = 0;
  if (launch_arg != 0) {
    hit_power = *(float *)(launch_arg + 2);
    angle[0] = *launch_arg;
  }
  else if (kind == 15) {
    hit_power = hit_est_record->f[2];
    angle[0] = hit_est_record->s[0];
    msnm_rule2_scale(attacker,hit_est_record->s,&hit_power,angle);
  }
  else {
    power_scale = &hit_est_record->f[3];
    angle_src = &hit_est_record->s[1];
    if ((hit_result_flags & 4) != 0 || (defender->flags24 & 0x1000) != 0) {
      power_scale++;
      angle_src++;
    }
    else if ((hit_result_flags & 2) != 0) {
      power_scale += 2;
      angle_src += 2;
    }
    if ((defender->state & 0x1000000) != 0) {
      power_scale = &hit_est_record->f[6];
      angle_src = &hit_est_record->s[4];
    }
    hit_power = hit_power * *power_scale;
    angle[0] = *angle_src;
    if (defender->combo[2].hits > 1) {
      repeats = 0;
      move_id = attacker->move_no;
      lo = -0x3e00;
      for (i = repeats; i < 16; i++) {
        if (attacker->attack_history[i] == move_id) repeats++;
      }
      if (repeats != 0) {
        angle[0] = angle[0] - launch_repeat_drop * repeats;
        if (angle[0] < lo) angle[0] = lo;
      }
    }
  }
  if (defender->combo[2].hits > 3) {
    angle_min = -0x3e00;
    combo_n = defender->combo[2].hits - 1;
    if (angle[0] < angle_min) angle_min = 0;
    angle[0] = angle[0] - combo_n * (short)0x600;
    if (angle[0] < angle_min) angle[0] = angle_min;
  }
  if (angle[0] > 0x3e00) angle[0] = 0x3e00;
  hit_knockback_set(defender,angle[0]);
  m = hit_stun(attacker,defender);
  defender->stun = m;
  hit_reaction(attacker,defender,m);
}

void rob_guard_react(rob_rec *attacker, rob_rec *defender, char extra)
{
  char react_kind;
  uint word;
  int stun;
  
  react_kind = attacker->coli->field_0x94;
  attacker->field_0xfa = 0;
  word = 0x0a000000 + hit_reaction_move_pick(attacker,defender,(int)react_kind);
  defender->ctrl->continuation = 0;
  defender->ctrl->wait = 0;
  defender->request_pending.word = word;
  stun = guard_stun(attacker,defender,extra);
  defender->stun = (short)stun;
  if (msnm_rule9_combo_check(attacker) != 0) {
    defender->stun = 1;
  }
  hit_reaction(attacker,defender,stun);
  hit_knockback_set(defender,0);
}

int hit_reaction_move_pick(rob_rec *attacker,rob_rec *defender,int kind)
{
  uint v;
  int (*rows)[4];
  int *p;
  int flag;
  int z;
  uint f20;
  int *q;
  int m;
  int r;
  uint k;
  short h;
  uint j;
  char c;
  
  v = hit_attack_flags & 0xff;
  if (v >= 0x56) v = 0;
  rows = (int (*)[4])guard_motion_tables[v];
  p = rows[6];
  z = bits_clear(defender->state, 0x2000);
  flag = !z;
  if (kind == 0xf) {
    p = rows[20];
  }
  else if (kind == 0xe && hit_attack_level == 0xe) {
    rows = (int (*)[4])guard_motion_tables[27];
    p = rows[14];
  }
  else if (((f20 = defender->state) & 0x8000000) != 0) {
    p = reaction_row_state_bit27;
  }
  else if ((f20 & 0x10000000) != 0) {
    p = reaction_row_state_bit28;
  }
  else if ((defender->flags24 & 0x40) != 0) {
    p = reaction_row_flags24_bit6;
  }
  else {
    switch (kind) {
    case 0:
      return 0;
    case 1:
      if (flag) p = rows[7];
      else p = rows[6];
      break;
    case 3:
    case 4:
      if (flag) p = rows[11];
      else p = rows[10];
      break;
    case 5:
      if (flag) p = rows[9];
      else p = rows[8];
      break;
    case 2:
      if ((attacker->field_0x10c & 0x4000) != 0) {
        if (flag) p = rows[17];
        else p = rows[16];
      }
      else if ((attacker->field_0x10c & 2) != 0) {
        if (flag) p = rows[19];
        else p = rows[18];
      }
      else {
        if (flag) p = rows[15];
        else p = rows[14];
      }
      break;
    case 6:
    case 7:
      p = rows[21];
      break;
    case 8:
      p = rows[22];
      break;
    case 9:
      p = rows[23];
      break;
    case 10:
      if ((defender->guard_state & 2) != 0) p = rows[1];
      else p = rows[0];
      break;
    case 11:
    case 12:
      if (flag) p = rows[5];
      else p = rows[4];
      break;
    case 13:
      if (flag) p = rows[3];
      else p = rows[2];
      break;
    case 16:
      if (flag) p = rows[13];
      else p = rows[12];
      break;
    case 14:
      p = rows[14];
      break;
    case 15:
      p = rows[20];
      break;
    }
  }
  if ((uint)(kind - 0xe) < 2) {
    int *q1 = p;
    if ((defender->state & 0x800000) != 0 || (defender->flags & 0x200) != 0) r = q1[2];
    else r = p[0];
  }
  else if ((attacker->field_0x30 & 0x40) != 0) {
    int *q2 = p;
    k = (int)(hit_angle - defender->facing + 0xa000) >> 0xe;
    h = k & 3;
    if ((defender->side_flags & 0x10) != 0 && (h & 1) != 0) h ^= 2;
    r = q2[h];
  }
  else {
    if ((hit_attack_flags & 0x100) == 0) z = 1;
    else z = 0;
    j = !z;
    if ((defender->relation_flags & 1) != 0) j = (short)(j + 2);
    if ((attacker->side_flags & 0x10) != 0) j ^= 1;
    if ((defender->side_flags & 8) != 0) j ^= 1;
    if ((hit_attack_flags2 & 0x1000) != 0) j ^= 1;
    r = p[j];
  }
  if (r == 0x209) {
    c = 0;
    if ((defender->state & 0x40) != 0) {
      if (defender->pos.y - defender->floor_y <= *(float *)(defender->chara_data + 0x28)) c = 1;
    }
    else if (flag) c++;
    if ((defender->state & 0x800000) != 0) c += 2;
    return height_reaction_moves[c];
  }
  if (r == 0xcb) {
    m = 2;
    if (defender->pos.y - defender->floor_y <= reaction_low_height_max) m = 3;
    return height_reaction_moves[m];
  }
  if (r == 0x13f) {
    if ((defender->state & 0x800000) != 0 || (defender->flags & 0x200) != 0) r = 0x140;
  }
  return r;
}

int attack_vs_guard(rob_rec *attacker, rob_rec *defender)
{
  char b;
  
  if ((defender->guard_state & 1) == 0) return 0;
  b = defender->guard_state;
  if ((b & 4) != 0) {
    defender->guard_state = b & 0xfb;
    switch(hit_attack_level) {
    case 3:
    case 5:
    case 6:
    case 13:
      defender->guard_state = defender->guard_state | 2;
    }
  }
  if ((defender->guard_state & 2) != 0) return attack_vs_crouch_guard(attacker,defender);
  return attack_vs_stand_guard(attacker,defender);
}

int attack_vs_crouch_guard(rob_rec *attacker, rob_rec *defender)
{
  int k;
  short s;

  if ((attacker->attack_mask & 0x20000000) && !(defender->move_flags & 0x4000)) {
    defender->catch_mask = 0x22000000;
    hit_catch_mask_check(attacker, defender);
    return 1;
  }
  if (defender->field_0x30 & 4)
    return hit_catch_mask_check(attacker, defender);
  if (defender->move_no != 0x1087 && (defender->relation_flags & 1))
    return 0;
  switch (hit_attack_level) {
  case 0: case 1: case 8: case 10: case 11:
    return 1;
  case 2: case 4:
    if (!(defender->field_0x30 & 4) && !(defender->state & 0x20000000)) {
    if (!(attacker->field_0x30 & 0x10) && (hit_attack_flags & 0x400000)) {
      k = 2;
      s = defender->pending_damage;
      if (hit_attack_flags & 0x20000) {
        k = 1;
        s = s >> 1;
      }
      if (defender->life > s)
        return hit_result_guard_knockback(attacker, defender, k, 1);
    }
    else {
    if (hit_attack_flags & 0x40000)
      return hit_result_guard_row5(attacker, defender);
    if (hit_attack_flags & 0x20000)
      return hit_result_half_damage(attacker, defender);
    }
    }
    break;
  case 3: case 5: case 6: case 13:
    if (!(hit_attack_flags & 0x10000)) {
    if (defender->flags24 & 0x20)
      return hit_result_guard_reversed(defender, attacker);
    if (hit_attack_flags & 0x40000)
      return hit_result_guard_row5(attacker, defender);
    return hit_result_guard(attacker, defender, 0);
    }
  }
  return 0;
}

int attack_vs_stand_guard(rob_rec *attacker, rob_rec *defender)
{
  if ((defender->field_0x30 & 4) != 0) return hit_catch_mask_check(attacker, defender);
  if (defender->move_no != 0x1087 && (defender->relation_flags & 1) != 0) return 0;
  switch (hit_attack_level) {
  case 3:
  case 5:
  case 13:
    if ((defender->state & 0x20000000) != 0 || (attacker->field_0x30 & 0x10) != 0) break;
    if ((hit_attack_flags & 0x40000) != 0) return hit_result_guard_row5(attacker, defender);
    if ((hit_attack_flags & 0x200000) != 0) return hit_result_guard_knockback(attacker, defender, 0, 1);
    break;
  case 6:
    if ((defender->flags24 & 0x10) != 0) return 0;
  default:
    if ((hit_attack_flags & 0x10000) != 0) return 0;
    if ((defender->flags24 & 0x10) != 0) return hit_result_guard_reversed(defender, attacker);
    if ((hit_attack_flags & 0x200000) != 0) return hit_result_guard_knockback(attacker, defender, 0, 1);
    if ((hit_attack_flags & 0x80000) != 0) return hit_result_guard_row5(attacker, defender);
    return hit_result_guard(attacker, defender, 0);
  }
  return 0;
}

int hit_catch_mask_check(rob_rec *attacker, rob_rec *defender)
{
  uint r;
  uint m;
  uint a;
  uint b;
  short s;
  signed char x;
  signed char y;

  a = attacker->attack_mask;
  b = defender->catch_mask;
  r = attack_point_mask((int)attacker->attack_level);
  if ((r & b) != 0) {
    m = b & a;
    if (m != 0) {
      r = r | m;
      if (defender->chara == 9 && (a & 4) != 0 && defender->life <= 0x14) return 0;
      if ((b & 0x8000000) != 0) {
        if ((defender->relation_flags & 1) == 0) return 0;
        if ((b & 0xc00) != 0) {
          s = defender->facing - defender->opp_dir;
          if ((s > -0x5800) & (s < 0x5800)) return 0;
        }
        r = r | 0x8000000;
      }
      else {
        if ((defender->relation_flags & 1) != 0) return 0;
        if ((b & 0xc00) != 0 &&
            (ushort)((defender->facing - defender->opp_dir) + 0x2800U) > 0x5000)
          return 0;
      }
      x = (a >> 0x1f) ^ ((attacker->side_flags >> 4) & 1);
      if (x) {
        r = r | 0x80000000;
      }
      defender->field_0x274 = r;
      defender->field_0x278 = attacker->field_0x338;
      rob_voice_cut(attacker);
      return 1;
    }
  }
  return 0;
}

int hit_reversal_move_check(rob_rec *attacker, rob_rec *defender)
{
  uint *rec;
  uint point_mask;
  short *frame_arg;
  short *recovery_arg;
  ushort *motion_p;
  int *cmd;
  uint atk_mask;
  move_event39_rec *entry;
  int m;
  int hi;
  int flag;
  uint entry_mask;
  int c;
  uint v;
  uint w;
  int frames;

  atk_mask = attacker->attack_mask;
  if (atk_mask == 0) return 0;
  rec = (uint *)move_record((int)(short)(defender->move_no + -1));
  cmd = (int *)rec[2];
  if (cmd == (int *)0x0) return 0;
  point_mask = attack_point_mask(attacker->attack_level);
  for (; *cmd != 0; cmd += 2) {
    if (cmd[1] == 0x27) {
      entry = (move_event39_rec *)*cmd;
      entry_mask = entry->mask;
      if (((entry_mask & atk_mask) != 0) && ((entry_mask & point_mask) != 0)) {
        hi = atk_mask >> 0x1f;
        flag = 0;
        attack_result_sounds_play(defender,defender,0x1000);
        control_buffer_use((char)defender->side,5);
        frame_arg = (short *)move_cmd_arg_find((int)(short)attacker->move_no,1,6);
        if ((frame_arg != (short *)0x0) && (*frame_arg <= attacker->frame)) {
          flag = 1;
        }
        if (entry->mode == 1) {
          if ((attacker->side_flags & 8) != 0) {
            flag = flag ^ 1;
          }
          if ((defender->side_flags & 8) != 0) {
            flag = flag ^ 1;
          }
        }
        hi = (char)(flag ^ hi);
        if (hi) motion_p = &entry->motion[1];
        else motion_p = &entry->motion[0];
        c = *motion_p;
        if (c != 0) {
          if (move_cmd_arg_find(c & 0x7fff,0,0xf) != 0) {
            defender->request_pending.word = c + 0x5000000;
          }
          else {
            m = c & 0x7fff;
            frames = mot_frames(m);
            recovery_arg = (short *)move_cmd_arg_find(m,0,1);
            if ((recovery_arg != 0) && (recovery_arg[2] < frames)) {
              frames = (int)recovery_arg[2];
            }
            attacker->stun = (short)frames;
            attacker->request_pending.word = c + 0x7000000;
            attacker->knockback.x = 0;
            attacker->knockback.y = 0;
            attacker->knockback.z = 0;
            hit_wait_timer_set((int)attacker->stun);
            v = (uint)entry->move;
            if (v != 0) defender->request_pending.word = v;
          }
          *(short *)(defender->stats + 0x82) =
               *(short *)(defender->stats + 0x82) + 1;
          return 1;
        }
        if (entry->move != 0) {
          defender->request_pending.word = (uint)entry->move;
          return 1;
        }
      }
    }
  }
  return 0;
}

int hit_cmd49_absorb_check(rob_rec *attacker, rob_rec *defender, int launch)
{
  char c = (char)launch;
  move_cmd49_arg_rec *e;
  uint m;
  uint k;
  uint mask;
  short v;
  int ok;

  e = (move_cmd49_arg_rec *)move_cmd_arg_find((short)defender->move_no,0,0x31);
  if (e == 0) return 0;
  if ((defender->flags2c & 8) == 0) return 0;
  if ((e->no_launch != 0) & (c != 0)) return 0;
  if (e->damage_limit != -1 && defender->pending_damage >= e->damage_limit) return 0;
  m = attacker->attack_mask;
  k = attack_point_mask(attacker->attack_level);
  mask = e->mask;
  v = defender->pending_damage;
  if (e->damage_min < v) {
    v = (short)(v * e->damage_percent / 100);
    if (e->damage_min > v) v = e->damage_min;
  }
  if (defender->life <= v) return 0;
  if (mask != 0) {
    ok = 0;
    if ((mask & k) != 0) {
      if ((mask & 0xffffff) == 0xffffff || (mask & m) != 0) ok = 1;
    }
    if (!ok) return 0;
  }
  else {
    if ((k & 0x4000000) != 0) return 0;
  }
  defender->flags2c = defender->flags2c & 0xfffffff7;
  attacker->slow_frames = e->slow_frames;
  attacker->slow_rate = e->slow_rate;
  hit_light_flags = hit_light_flags & 0xfffffffd | 0x80;
  hit_light_color = e->light_color;
  switch (e->sound_kind) {
  case 0:
    attack_result_sounds_play(attacker,defender,0x80);
    break;
  case 1:
    attack_result_sounds_play(attacker,defender,2);
    break;
  case 2:
    if (e->sound_id != 0) rob_voice_request(defender,(short)e->sound_id);
    break;
  case 3:
    if (e->sound_id != 0) sound_request(defender,(short)e->sound_id);
    break;
  }
  if (v > 0) {
    defender->pending_damage = v;
    rob_damage_apply(defender,hit_damage_kinds);
  }
  control_buffer_use((char)defender->side,4);
  return 1;
}

int hit_result_guard(rob_rec *attacker, rob_rec *defender, char extra)
{
  attacker->coli->field_0x94 = attacker->field_0x115 = 10;
  attacker->coli->field_0x3c[0] = attacker->field_0x10c = hit_result_flags = 0x20;
  attacker->event_flags = attacker->event_flags | 4;
  hit_power = hit_power * hit_est_record->f[1];
  hit_light_flags = hit_light_flags | 1;
  attack_recovery_apply(attacker->coli);
  control_buffer_use((char)attacker->side,2);
  attack_result_sounds_play(attacker,defender,2);
  rob_guard_react(attacker,defender,(int)extra);
  msnm_mission27_mark((char)defender->side);
  return 1;
}

int reaction_length_get(uint motion, int stun)
{
  int s = (short)stun;
  int m = motion & 0x7fff;
  short *a = (short *)move_cmd_arg_find(m,0,1);
  int f = mot_frames(m);
  int n;
  int x;
  short *b;
  if (a == 0 || (n = a[2], ((user_back_data.option_bits & 0x8000000) && (n = (short)(mode_recovery_extra + n))), n > f)) n = f;
  b = (short *)move_cmd_arg_find(m,0,0x19);
  if (b != 0 && b[1] != 0) {
    if (a != 0) {
      if (a[1] < s) n = s;
      else n = a[1];
    }
    a = (short *)move_cmd_arg_find(b[1],0,1);
    if (a != 0) n = (short)(a[2] + n);
    else n = (short)(n + mot_frames(b[1] & 0x7fff));
    s = n;
  }
  else if (s < n) s = n;
  return s;
}

int hit_result_guard_knockback(rob_rec *attacker, rob_rec *defender, int mode_arg, int flag_arg)
{
  char mode = (char)mode_arg;
  unsigned char flag = (unsigned char)flag_arg;
  int k = 0xd;
  ushort move;
  int stun;
  int v;
  int w;
  
  v = 0x80;
  if (flag == 1) {
    v |= 0x40;
  }
  attacker->coli->field_0x94 = attacker->field_0x115 = k;
  attacker->coli->field_0x3c[0] = attacker->field_0x10c = hit_result_flags = v;
  attacker->field_0xfa = 0;
  attack_recovery_apply(attacker->coli);
  rob_voice_cut(defender);
  w = 0x100;
  switch (mode) {
  case 1:
    defender->pending_damage = defender->pending_damage >> 1;
    hit_power = hit_power * 0.5;
  case 2:
    rob_damage_apply(defender,hit_damage_kinds);
    w = 0x80;
  }
  attack_result_sounds_play(attacker,defender,w);
  move = hit_reaction_move_pick(attacker,defender,k);
  { uint t = 0x0d000000 + move; defender->request_pending.word = t; }
  defender->ctrl->continuation = 0;
  defender->ctrl->wait = 0;
  stun = guard_stun(attacker,defender,0);
  defender->stun = (short)stun;
  stun = reaction_length_get((int)(short)move,stun);
  hit_reaction(attacker,defender,stun);
  hit_light_flags = hit_light_flags | 8;
  control_buffer_use((char)attacker->side,3);
  hit_knockback_set(defender,0);
  return 1;
}

int hit_result_guard_reversed(rob_rec *defender, rob_rec *attacker)
{
  defender->coli->field_0x94 = defender->field_0x115 = 0xb;
  defender->coli->field_0x3c[0] = defender->field_0x10c = hit_result_flags = 0x60;
  hit_power = hit_power * hit_est_record->f[1];
  attack_result_sounds_play(defender,defender,0x1000);
  rob_guard_react(defender,attacker,0);
  return 1;
}

int hit_result_guard_row5(rob_rec *attacker, rob_rec *defender)
{
  short stun;
  rob_coli_rec *coli;
  
  coli = attacker->coli;
  attacker->field_0x115 = 0xb;
  coli->field_0x94 = 0xb;
  attacker->coli->field_0x3c[0] = attacker->field_0x10c = hit_result_flags = 0x60;
  hit_power = hit_power * hit_est_record->f[1];
  attack_recovery_apply(attacker->coli);
  rob_voice_cut(defender);
  attack_result_sounds_play(attacker,defender,8);
  rob_guard_react(attacker,defender,0);
  stun = attacker->react_stun[4];
  if (0 < stun) {
    defender->stun = stun;
    hit_reaction(attacker,defender,(int)stun);
  }
  return 1;
}

int hit_result_half_damage(rob_rec *attacker, rob_rec *defender)
{
  rob_coli_rec *coli;
  rob_coli_rec *coli2;
  coli = attacker->coli;
  attacker->field_0x115 = 0xc;
  coli->field_0x94 = 0xc;
  coli2 = attacker->coli;
  hit_result_flags = 0x60;
  attacker->field_0x10c = 0x60;
  coli2->field_0x3c[0] = 0x60;
  defender->pending_damage = attacker->attack_damage >> 1;
  hit_power = hit_power * 0.5;
  attack_result_sounds_play(attacker,defender,4);
  attack_hit_resolve(attacker,defender);
  return 1;
}

int hit_stun(rob_rec *attacker, rob_rec *defender)
{
  short r;
  short x;
  int off;
  short d1;
  short d2;
  uint f;

  if ((defender->state & 0x1000000) != 0) {
    x = attacker->react_stun[3];
    if (x != 0) {
      r = x;
      goto have;
    }
  }
  if ((attacker->coli->field_0x3c[0] & 0x10000) != 0) {
    x = attacker->result_rows[2].stun;
    if (x != 0) {
      r = x;
      goto have;
    }
  }
  if ((attacker->coli->field_0x3c[0] & 0x8000) != 0) {
    x = attacker->result_rows[1].stun;
    if (x != 0) {
      r = x;
      goto have;
    }
  }
  if ((attacker->coli->field_0x3c[0] & 0x4000) != 0) {
    x = attacker->result_rows[0].stun;
    if (x != 0) {
      r = x;
      goto have;
    }
  }
  if ((attacker->coli->field_0x3c[0] & 2) != 0) off = offsetof(rob_rec, react_stun[2]);
  else if ((attacker->coli->field_0x3c[0] & 0x800) != 0) off = offsetof(rob_rec, result_rows[3].stun);
  else off = offsetof(rob_rec, react_stun[1]);
  r = *(short *)((char *)attacker + off);
have:
  if (r != 0 && (attacker->attack_cmd_flags & 2) == 0) return r;
  {
    r = defender->pending_damage * 4 / 5 + 8;
    if ((user_back_data.option_bits & 0x08000000) != 0) {
      r = mode_stun_extra + r;
    }
    d1 = attacker->recovery_end - attacker->frame;
    d2 = attacker->recovery_end - attacker->active_start;
    f = defender->state;
    if ((f & 0x400) == 0 && ((f & 0x40000) == 0 || attacker->attack_level != 12)) {
      if ((attacker->attack_cmd_flags & 2) != 0) {
        r = d2 >= r ? d2 : r;
      }
      else if ((user_back_data.option_bits & 0x20000000) == 0) {
        r = d1 + 6 > r ? r : d1 + 6;
        if ((attacker->attack_cmd_flags & 1) == 0 && (attacker->state & 0x80) == 0) {
          r = r >= d1 - 2 ? r : d1 - 2;
        }
      }
    }
  }
  return r;
}

int guard_stun(rob_rec *attacker, rob_rec *defender, char extra)
{
  short v;
  short b;
  if (attacker->react_stun[0] != 0) return attacker->react_stun[0];
  if (defender->pending_damage > 14) b = 10; else b = 6;
  v = (defender->pending_damage * 2) / 3 + b;
  if (extra != '\0') v = v + 6;
  if ((user_back_data.option_bits & 0x8000000) != 0) v = mode_stun_extra + v;
  if ((attacker->state & 0x80) == 0) {
    v = v > 25 ? 26 : (v < 10 ? 10 : v);
  }
  else {
    v = v < 10 ? 10 : v;
  }
  return v;
}

void hit_reaction(rob_rec *attacker, rob_rec *defender, int length)
{
  short lv = length;
  uint f;
  short r;
  
  f = attacker->coli->field_0x3c[0];
  if ((f & 0x20) && (r = attacker->react_wait[0]) != -1) lv = r;
  else if ((f & 0x40) && (r = attacker->react_wait[4]) != -1) lv = r;
  else if ((defender->state & 0x1000000) && (r = attacker->react_wait[3]) != -1) lv = r;
  else if ((f & 0x10000) && (r = attacker->result_rows[2].wait) != -1) lv = r;
  else if ((f & 0x8000) && (r = attacker->result_rows[1].wait) != -1) lv = r;
  else if ((f & 0x4000) && (r = attacker->result_rows[0].wait) != -1) lv = r;
  else if ((f & 2) && (r = attacker->react_wait[2]) != -1) lv = r;
  else if ((f & 0x800) && (r = attacker->result_rows[3].wait) != -1) lv = r;
  else if ((r = attacker->react_wait[1]) != -1) lv = r;
  else if (f & 0x200) lv += 3;
  else if (f & 0x80) {
    if (!(attacker->flags24 & 0x100000)) lv += 3;
  }
  else {
    int t = lv + 3;
    if ((short)t > 0x22) t = 0x22;
    lv = t;
  }
  hit_wait_timer_set(lv);
}

void hit_knockback_set(rob_rec *rob,int angle)
{
  char c;
  rob_rec *opp;
  int v;
  float s;
  float t;
  float q;
  float x, y, z;
  float buf[4];

  fsca_sincos16((short)angle,&buf[0],&buf[1]);
  fsca_sincos16(hit_angle,&buf[2],&buf[3]);
  s = fast_sqrtf(hit_power / (*(float *)(rob->chara_data + 0x10) * knockback_weight_scale + knockback_weight_bias));
  if (half_gravity_active() == 1) {
    s = s * knockback_half_gravity_scale;
  }
  t = s * buf[1];
  x = s * buf[0];
  y = t * buf[2];
  z = -t * buf[3];
  if (1.1920929e-07 < __builtin_fabsf(rob->knockback_scale)) {
    x = x * rob->knockback_scale;
  }
  if (((hit_result_flags & 0x10) != 0) && ((opp = rob->opp)->field_0x115 != 0xf)) {
    x = x * hit_est_record->f[7];
    y = y * hit_est_record->f[8];
    z = z * hit_est_record->f[8];
  }
  else if (((rob->action.word & 0xff080000) == 0x8080000) && (0.0 < x)) {
    x = 0.0;
  }
  rob->knockback.x = y;
  rob->knockback.y = x;
  rob->knockback.z = z;
  opp = rob->opp;
  c = opp->field_0x115;
  if (opp->field_0x30 & 0x40) {
    rob->facing_override = rob->facing;
  }
  else if (((((int)rob->state >> 6) ^ 1) & 1 & (c != 0xf)) != 0) {
    if (c != 0xe) {
      v = rob->opp_dir;
      if ((rob->relation_flags & 1) != 0) {
        v = v + 0x8000;
      }
      rob->facing_override = v;
    }
  }
}

void hit_params_custom_load(rob_rec *rob)
{
  est_row *a;
  est_row *d;

  if (rob->hit_param_row == 1) {
    a = est_data;
    d = a + 1;
    *d = *a;
    if (rob->hit_params != (est_row *)0x0) {
      a = rob->hit_params;
      if (a->s[0] != -0x8000) (*d).s[0] = a->s[0];
      if (a->s[1] != -0x8000) (*d).s[1] = a->s[1];
      if (a->s[2] != -0x8000) (*d).s[2] = a->s[2];
      if (a->s[3] != -0x8000) (*d).s[3] = a->s[3];
      if (a->s[4] != -0x8000) (*d).s[4] = a->s[4];
      if (__builtin_fabsf(a->f[0]) > 1.1920929e-07f) (*d).f[0] = a->f[0];
      if (__builtin_fabsf(a->f[1]) > 1.1920929e-07f) (*d).f[1] = a->f[1];
      if (__builtin_fabsf(a->f[2]) > 1.1920929e-07f) (*d).f[2] = a->f[2];
      if (__builtin_fabsf(a->f[3]) > 1.1920929e-07f) (*d).f[3] = a->f[3];
      if (__builtin_fabsf(a->f[4]) > 1.1920929e-07f) (*d).f[4] = a->f[4];
      if (__builtin_fabsf(a->f[5]) > 1.1920929e-07f) (*d).f[5] = a->f[5];
      if (__builtin_fabsf(a->f[6]) > 1.1920929e-07f) (*d).f[6] = a->f[6];
      if (__builtin_fabsf(a->f[7]) > 1.1920929e-07f) (*d).f[7] = a->f[7];
      if (__builtin_fabsf(a->f[8]) > 1.1920929e-07f) (*d).f[8] = a->f[8];
    }
  }
}

void attack_history_clear(rob_rec *rob)
{
  short *p;
  int n;
  
  p = rob->attack_history;
  n = 0x10;
  do {
    *p = 0;
    p = p + 1;
    n = n + -1;
  } while (n != 0);
  rob->attack_history_idx = 0;
  return;
}

uint attack_point_mask(char point)
{
  if (0xe < (uint)(int)(char)point) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libaction/src/coli_attack.c",0xc72,
             "assertion `%s\' failed.\n","(attack_point>=0)&&(attack_point<AP_MAX)");
  }
  return attack_point_masks[(char)point];
}

void hit_cmd51_count_apply(void)
{
  rob_rec *rob;
  char *p;
  uint flags;
  int i;

  for (i = 0; i < 2; i++) {
    rob = rob_get(i & 0xff);
    flags = rob->coli->field_0x3c[0];
    if (flags == 0) continue;
    p = (char *)move_cmd_arg_find((int)(short)rob->move_no,0,0x33);
    if (p == 0) continue;
    switch (*p) {
    case 0:
      continue;
    case 1:
      if (!(flags & 0x80) || rob->opp->last_damage == 0) {
        if (!(flags & 1)) continue;
      }
      break;
    case 2:
      if (!(flags & 0x80) || rob->opp->last_damage != 0) {
        if (!(flags & 0x20)) continue;
      }
      break;
    }
    rob_rate_adjust((int)(char)rob->opp->side,(int)-p[1]);
  }
}

void empty_fn_0c072160(void)
{
  return;
}
