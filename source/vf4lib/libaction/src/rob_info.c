/*
 * vf4lib/libaction/src/rob_info.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint game_flags; /* 0x0c2925b8 */
extern char round_end_kind; /* 0x0c2925cd */
extern char round_winner; /* 0x0c2925d2 */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern rob_rec robs[2]; /* 0x0c37ba30 */

void rob_info_window_refresh(void)
{
  rob_rec *t = robs;
  rob_info0_rob = rob_get((uchar)no0);
  rob_info1_rob = rob_get((uchar)no1);
  coli_info0_coli = t[no0].coli;
  coli_info1_coli = t[no1].coli;
  ctrl_info0_ctrl = t[no0].ctrl;
  ctrl_info1_ctrl = t[no1].ctrl;
}

void rob_info_task_start(void)
{
  return;
}

void rob_info_clear(rob_rec *rob)
{
  rob->opp_dist = 1.5f;
  rob->opp_dir = rob->heading;
  rob->relation_flags = 0;
  rob->reach_flags = 0;
  rob->reach_flags2 = 0;
}

void rob_info_task_frame(void)
{
  int i;
  int n;
  rob_rec *rob;

  if (pause_frame_held() == 0) {
    n = hit_wait_timer_get();
    if (n > 0) {
      n = (short)(n - 1);
    }
    else {
      n = 0;
    }
    hit_wait_timer_set(n);
  }
  for (i = 0; i < 2; i++) {
    rob = rob_get((uchar)i);
    if ((rob->flags & 1) != 0) {
      rob_info_update(rob);
    }
    else {
      rob_info_clear(rob);
    }
  }
  if ((((play_style == '\x02') && (((rob_get(0))->flags & 4) != 0))
      && (((rob_get(1))->flags & 4) != 0)) &&
     ((mode_get_main() == MODE_MAIN_GAME &&
      ((mode_get_sub() == 10 || (mode_get_sub() == 0xd)))))) {
    for (i = 0; i < 2; i++) {
      rob = rob_get((uchar)i);
      if ((rob->flags & 1) != 0) {
        vs_stats_events_count(rob);
      }
    }
    for (i = 0; i < 2; i++) {
      rob = rob_get((uchar)i);
      if ((rob->flags & 1) != 0) {
        vs_stats_streaks_count(rob);
      }
    }
  }
}

void rob_info_update(rob_rec *rob)
{
  rob_rec *opp;
  vec3 a;
  vec3 b;
  float dx;
  float dz;
  float d;
  float d2;
  int c1;
  int ang;
  int rel;
  uint t20;
  uchar fl;
  int s1;
  int cur;
  int s2;
  int lim;
  int v;
  uint u;
  short *q;

  opp = rob->opp;
  if ((opp->flags & 1) == 0) {
    rob_info_clear(rob);
  }
  else {
    a = rob->pos;
    b = opp->pos;
    dx = b.x - a.x;
    dz = b.z - a.z;
    d2 = dx * dx + dz * dz;
    __asm__ ("fsqrt %0" : "=f" (d) : "0" (d2));
    ang = rob->opp_dir;
    if (d > 0.05) {
      rob->opp_dist = d;
      if (d > 0.2) {
        ang = atan2_s16(dx,-dz);
        rob->opp_dir = ang;
      }
    }
    rel = ang - rob->facing;
    rel = (short)rel;
    rob->relation_flags = 0;
    c1 = rel > 0x4800;
    c1 |= rel < -0x4800;
    if (c1) {
      rob->relation_flags = 1;
    }
    if ((ushort)(ang - opp->facing + 0x4000) <= 0x8000) {
      rob->relation_flags |= 2;
    }
    rel = (short)(opp->facing - ang + -0x8000);
    if ((ushort)(rel + 0x2400) < 0x4800) {
      rob->relation_flags |= 0x10;
    }
    c1 = rel <= -0x5c00;
    c1 |= rel > 0x5c00;
    if (c1) {
      rob->relation_flags |= 0x20;
    }
    if ((ushort)(rel - 0x800) <= 0x7000) {
      rob->relation_flags |= 0x40;
    }
    if ((ushort)(rel + 0x7800) <= 0x7000) {
      rob->relation_flags |= 0x80;
    }
    t20 = opp->state;
    if ((~t20 & 0x44000) && ((opp->flags28 & 0x40) || ((t20 & 0xc00) && !(opp->flags24 & 0x200000)))) {
      rob->relation_flags |= 0x108;
    }
    if ((user_back_data.option_bits & 0x40000) && (opp->flags24 & 0x40)) {
      rob->relation_flags |= 0x100;
    }
    rob_reach_flags_update(rob);
    if (rob->flags24 & 0x80000) {
      if ((short)(rob->body_yaw + camera.yaw) < 0) {
        rob->flags |= 0x20;
      }
      else {
        rob->flags &= ~0x20;
      }
    }
    else if (!(rob->state & 0x300000)) {
      if ((short)(rob->facing + camera.yaw) < 0) {
        rob->flags |= 0x20;
      }
      else {
        rob->flags &= ~0x20;
      }
    }
    else if (!(rob->flags24 & 0x40000)) {
      if ((short)(rob->body_yaw + camera.yaw) < 0) {
        rob->flags |= 0x20;
      }
      else {
        rob->flags &= ~0x20;
      }
    }
    else {
      if ((short)(rob->facing + camera.yaw) < 0) {
        rob->flags |= 0x20;
      }
      else {
        rob->flags &= ~0x20;
      }
    }
    u = rob->flags & ~0x200;
    rob->flags = u;
    if (rob->state & 0x800000) {
      rob->flags = u | 0x200;
    }
    q = &rob->frames_to_land;
    *q = rob_frames_to_ground(rob,rob->jump_height);
    fl = rob->field_0x118;
    if (fl & 1) {
      s1 = *q;
      cur = rob->frame;
      if (rob->field_0x119 != 0) {
        fl &= 0xdf;
      }
      else if (!(fl & 8)) {
        if (!(fl & 2)) {
        s2 = rob->frames_to_land;
        fl &= 0xcf;
        if (rob->field_0x2ee <= cur) {
          if (!(rob->flags & 8) && 0.0 > rob->velocity.y) {
            fl |= 0x24;
          }
          else if (rob->field_0x300[2] >= s2 && rob->field_0x300[3] <= s2) {
            fl |= 0x30;
          }
          else {
            lim = rob->field_0x300[1];
            v = rob->field_0x300[0];
            if (v < 0) {
              v = (short)(rob->frame - rob->field_0x2f2 + v);
            }
            else {
              v = (short)(v - s2);
            }
            if ((v >= 0) & (cur <= lim)) {
              fl |= 0x20;
            }
            else if (cur > lim) {
              fl = 0;
            }
            msnm_rule1_damage_set(rob,cur,v,lim);
          }
        }
        }
      }
      if ((fl != 0) & (s1 <= 0) && rob->field_0x2ee <= cur) {
        fl |= 4;
      }
      rob->field_0x118 = fl;
    }
    mtx_push(0);
    camera_view_load();
    mtx_xform_point(&rob->pos.x,&rob->view_pos.x);
    mtx_save_proj_x_current(640.0,480.0);
    proj_point_to_pixel_xy(&rob->pos.x,&rob->screen_xy[0]);
    mtx_pop(1);
  }
}

void rob_reach_flags_update(rob_rec *rob)
{
  rob_rec *opp;
  uint res;
  uint fl;
  uint u;
  uint m;
  uint t;
  uint w;
  float z;
  float h;
  short d;
  short a;
  short q;
  short k;
  char s;
  opp = rob->opp;
  res = 0;
  fl = 0;
  if (rob->flags & 0x180) goto done;
  if (opp->flags & 0x100) goto done;
  u = rob->state;
  if (u & 0x40000) {
    if (!(u & 0x10000) && !(rob->flags28 & 0x1000000) && (rob->move_flags & 0x4000) && (u & 0x4002)) goto done;
  }
  if (opp->state & 0x10000000) goto done;
  if ((rob->state & 0x300000) && !(rob->flags2c & 4)) goto done;
  if ((opp->state & 0x300000) && !(opp->flags2c & 4)) goto done;
  if ((opp->flags24 & 0x40) && !(user_back_data.option_bits & 0x40000)) goto done;
  d = rob->opp_dir - rob->facing;
  if ((d >= 0x5c00) | (d <= -0x5c00)) fl |= 2;
  else if ((ushort)(d + 0x2400) <= 0x4800) fl |= 1;
  else if ((opp->state & 0x4000000) && !(rob->state & 0x4000)) fl |= 1;
  a = opp->opp_dir - opp->facing;
  q = 0x2000;
  if (opp->flags24 & 0x40) {
    if (opp->side_flags & 0x10) k = -0x4000;
    else k = 0x4000;
    a = k + a;
  }
  if (a >= q && a <= 0x8000 - q) fl |= 4;
  else if (a >= -0x8000 + q && a <= -q) fl |= 8;
  else if (a >= 0x8000 - q || a <= q - 0x8000) fl |= 0x10;
  if ((opp->flags & 0x10) || (opp->state & 0x8000000)) fl |= 0x20;
  if ((opp->state & 0x440) == 0x400 && !(opp->flags24 & 0x200000)) fl |= 0x40;
  else if (opp->flags24 & 0x40) fl |= 0x40;
  else if ((opp->move_flags & 0x44000) == 0x44000) {
    if (opp->state & 0x4002) fl = 0;
  }
  else if ((opp->state & 0x40000) && opp->frame < opp->recovery_end && !(opp->flags24 & 0x8000)) fl = 0;
  else if ((rob->state & 0x40) || (opp->state & 0x40)) fl |= 0x80;
  else if (opp->state & 0x400) {
    if (!(opp->flags24 & 0x200000)) fl = 0;
  }
  m = opp->field_0x344;
  s = rob->chara;
  if (m != 0) {
    if (!(m & (1 << s))) fl = 0;
  }
  if (opp->flags24 & 0x400000) fl &= 0x11;
  if (fl == 0) goto done;
  if (fl & 0x80) {
    if (rob->opp_dist > *(float *)(rob->chara_data + 0x1c)) goto done;
    if ((rob->state & 0x40) && (opp->state & 0x40)) goto done;
    if (rob->state & 0x40) {
      float y;
      if (opp->state & 0x400) goto done;
      if (fl & 0x2c) goto done;
      y = rob->pos.y;
      if (rob->move_flags & 0x80) {
        if (rob->flags & 0x40) goto done;
        h = 1.5;
        h += rob->floor_y;
        if (h > y) goto done;
      }
      y = __builtin_fabsf(y - rob->opp->pos.y);
      if (!(y < 1.2)) goto done;
      if (fl & 0x10) res = 0x800;
      else if (fl & 1) res = 0x400;
      else res = 0x40000000;
    }
    else {
      w = opp->state;
      if (!(w & 0x40)) goto done;
      if (!(w & 0x400)) goto done;
      if (!(fl & 0x11)) goto done;
      z = rob->pos.y;
      {
        rob_rec *o = rob->opp;
        if (!(__builtin_fabsf(z - o->pos.y) < 1.2)) goto done;
      }
      if (!(w & 0x800000)) {
        if (fl & 0x10) res = 0x2000;
        else res = 0x1000;
      }
      else {
        if (fl & 0x10) res = 0x4000;
        else res = 0x8000;
      }
    }
    goto done;
  }
  if (fl & 0x40) {
    float hi = 1.4;
    float lo = 1.2;
    if (opp->state & 0x800000) {
      if (rob->opp_dist < lo) {
        if (fl & 4) res |= 0x800000;
        if (fl & 8) res |= 0x400000;
      }
      if (!(rob->opp_dist < hi)) goto done;
      if (fl & 0x10) res |= 0x100000;
      else if (!(fl & 0xc)) res |= 0x200000;
    }
    else {
      if (rob->opp_dist < lo) {
        if (fl & 4) res |= 0x40000;
        if (fl & 8) res |= 0x80000;
      }
      if (!(rob->opp_dist < hi)) goto done;
      if (fl & 0x10) res |= 0x20000;
      else if (!(fl & 0xc)) res |= 0x10000;
    }
    goto done;
  }
  if (rob->state & 0x40) goto done;
  if (*(float *)(rob->chara_data + 0x18) < rob->opp_dist) goto done;
  if (fl & 2) {
    if (fl == 0x22) res |= 0x200;
    if (fl == 2) res |= 0x100;
    goto done;
  }
  if (fl & 0xc0) goto done;
  switch (fl) {
  case 1:
    if (rob->wall_dist[0] < 2.4) res |= 0x1000000;
    if (rob->wall_dist[2] < 1.5) res |= 0x2000000;
    res |= 1;
    if (opp->flags24 & 0x400000) res = 0x4000000;
    break;
  case 0x21:
    res |= 2;
    break;
  case 0x11:
    res |= 4;
    if (opp->flags24 & 0x400000) res = 0x8000000;
    break;
  case 0x31:
    res |= 8;
    break;
  case 5:
    res |= 0x10;
    break;
  case 0x25:
    res |= 0x20;
    break;
  case 9:
    res |= 0x40;
    break;
  case 0x29:
    res |= 0x80;
    break;
  }
done:
  if (!(user_back_data.option_bits & 0x100) && hit_wait_timer_get() > 0) {
    if (opp->state & 0x1000000) {
      if (res == 1) t = 0x10000000;
      else if (res == 2) t = 0x20000000;
      else t = 0;
      if (opp->flags24 & 0x8000) res |= t;
      else res = t;
    }
    else if (!(opp->flags24 & 0x8000)) res = 0;
  }
  t = res;
  if (res != 0 && (opp->attack_mask & 0x10000000) && (fl & 1)) res = 1;
  rob->reach_flags = res;
  rob->reach_flags2 = t;
}

void robs_frame_advantage_update(void)
{
  short s;
  rob_rec *rob0;
  rob_rec *rob1;
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  s = 0;
  if (pause_frame_held() == 0) {
    if ((((rob0->state | rob1->state) & 0x21008200) != 0) &&
       ((rob1->state & rob0->state & 1) != 0)) {
      s = rob_frames_left(rob0) - rob_frames_left(rob1);
    }
    rob0->frame_disadvantage = s;
    rob1->frame_disadvantage = -s;
  }
}

int rob_frames_left(rob_rec *rob)
{
  int r = 0;
  int m = rob->stun;
  uint f = rob->state;

  if ((f & 1) != 0) {
    if ((f & 0x1000000) != 0) {
      r = (short)(rob->recovery_end - rob->frame);
      if (m > r) r = m;
    }
    else if ((f & 0x8200) != 0) {
      r = (short)(rob->active_end - rob->frame);
      if (m > r) r = m;
    }
    else if ((rob->move_flags & 0x4000) != 0 && (f & 2) != 0) {
      r = (short)(rob->active_start - rob->frame);
    }
    else {
      r = (short)(rob->recovery_end - rob->frame);
    }
  }
  if (r < 0) r = 0;
  return r;
}

void vs_stats_round_clear(void)
{
  rob_rec *rob;
  int i;
  
  rob = rob_get(0);
  if (play_style == '\x02') {
    i = 0;
    do {
      vs_stats_clear(rob);
      i = i + 1;
      rob = rob + 1;
    } while (i < 2);
  }
  return;
}

void vs_stats_clear(rob_rec *rob)
{
  short *stats;
  
  stats = (short *)rob->stats;
  card_image_get((uint)rob->side);
  card_record_get((uint)rob->side);
  stats[0x5f] = 0;
  stats[0x60] = 0;
  stats[0x61] = 0;
  stats[0x62] = 0;
  stats[99] = 0;
  *(int *)(stats + 0x42) = 0;
  *(int *)(stats + 0x44) = 0;
  *(int *)(stats + 0x46) = 0;
  *(int *)(stats + 0x48) = 0;
  *(int *)(stats + 0x4c) = 0;
  *(int *)(stats + 0x4e) = 0;
  *(int *)(stats + 0x50) = 0;
  *(char *)(stats + 0x55) = 0;
  *stats = 0;
  stats[1] = 0;
  stats[2] = 0;
  stats[3] = 0;
  stats[4] = 0;
  stats[5] = 0;
  stats[6] = 0;
  stats[7] = 0;
  stats[8] = 0;
  stats[9] = 0;
  stats[10] = 0;
  stats[0xb] = 0;
  stats[0xc] = 0;
  stats[0xd] = 0;
  stats[0xe] = 0;
  stats[0xf] = 0;
  stats[0x10] = 0;
  stats[0x11] = 0;
  stats[0x12] = 0;
  stats[0x13] = 0;
  stats[0x14] = 0;
  stats[0x15] = 0;
  stats[0x16] = 0;
  stats[0x17] = 0;
  stats[0x18] = 0;
  stats[0x19] = 0;
  stats[0x1a] = 0;
  stats[0x1b] = 0;
  stats[0x1c] = 0;
  stats[0x1d] = 0;
  stats[0x20] = 0;
  stats[0x21] = 0;
  stats[0x22] = 0;
  stats[0x23] = 0;
  stats[0x24] = 0;
  stats[0x25] = 0;
  stats[0x26] = 0;
  stats[0x27] = 0;
  stats[0x28] = 0;
  stats[0x29] = 0;
  stats[0x2a] = 0;
  stats[0x2b] = 0;
  stats[0x2c] = 0;
  stats[0x2d] = 0;
  stats[0x2e] = 0;
  stats[0x2f] = 0;
  stats[0x31] = 0;
  stats[0x32] = 0;
  stats[0x33] = 0;
  stats[0x34] = 0;
  stats[0x35] = 0;
  stats[0x36] = 0;
  stats[0x37] = 0;
  stats[0x38] = 0;
  stats[0x39] = 0;
  stats[0x3a] = 0;
  stats[0x3b] = 0;
  stats[0x3c] = 0;
  stats[0x3d] = 0;
  stats[0x3e] = 0;
  stats[0x3f] = 0;
  stats[0x40] = 0;
  stats[0x41] = 0;
  return;
}

void vs_stats_events_count(rob_rec *rob)
{
  uint st;
  uint fl;
  char *a;
  rob_rec *opp;
  char *d;
  char *e;
  uint m;
  uint sw;
  char *w;
  char *c;
  uint hit;
  
  w = rob->stats;
  c = w;
  if (pause_frame_held() != 0) return;
  if ((rob->side_flags & 0x20) == 0) return;
  fl = 0;
  st = *(uint *)(w + 0x88);
  hit = rob->move_flags;
  sw = rob->flags28;
  if ((sw & 0x20000000) != 0) {
    fl = 2;
    *(short *)(w + 0x50) = *(short *)(w + 0x50) + 1;
    if ((st & 0x1000) != 0) {
      *(short *)(w + 0x68) = *(short *)(w + 0x68) + 1;
    }
  }
  if ((sw & 0x1000000) != 0) {
    fl |= 0x200;
    *(short *)(c + 0x62) = *(short *)(c + 0x62) + 1;
  }
  if ((sw & 0x2000000) != 0) {
    fl |= 0x400;
    *(short *)(c + 100) = *(short *)(c + 100) + 1;
  }
  if ((int)sw < 0) {
    fl |= 0x1000;
  }
  if ((sw & 0x800000) != 0) {
    fl |= 0x2000;
    *(short *)(c + 0x6a) = *(short *)(c + 0x6a) + 1;
  }
  if ((sw & 0x400000) != 0) {
    fl |= 0x4000;
    *(short *)(c + 0x6c) = *(short *)(c + 0x6c) + 1;
  }
  if (rob->field_0x118 != '\0') {
    fl |= 0x80;
    *(short *)(c + 0x5a) = *(short *)(c + 0x5a) + 1;
  }
  if (((rob->state & 0x21018201) == 0x8001) && ((sw & 0x1000000) == 0)) {
    a = rob->opp->stats;
    fl |= 0x8000;
    *(short *)(c + 0x72) = *(short *)(c + 0x72) + 1;
    m = *(uint *)(a + 0x88);
    if ((m & 1) != 0) {
      *(uint *)(a + 0x88) = m | 0x20000000;
    }
  }
  opp = rob->opp;
  if ((((opp->field_0x10c & 0x281) != 0) && (opp->field_0xfa < 2)) &&
     ((hit & 0x21000600) != 0)) {
    rob_rec *o;
    d = opp->stats;
    fl |= 0x20;
    *(short *)(c + 0x6e) = *(short *)(c + 0x6e) + 1;
    *(short *)(c + 0x20) = 0;
    if ((int)rob->opp->flags28 < 0) {
      fl |= 0x1000;
      *(short *)(c + 0x70) = *(short *)(c + 0x70) + 1;
    }
    o = rob->opp;
    if ((o->field_0x10c & 2) != 0) {
      *(uint *)(d + 0x88) = *(uint *)(d + 0x88) | 0x10000000;
      *(short *)(d + 0x74) = *(short *)(d + 0x74) + 1;
    }
    *(uint *)(d + 0x88) = *(uint *)(d + 0x88) | 0x08000000;
  }
  if ((((hit & 0x4000) != 0) && (3 < (int)rob->attack_level - 8U)) &&
     ((sw & 0x1000000) == 0)) {
    fl |= 1;
    *(short *)(c + 0x48) = *(short *)(c + 0x48) + 1;
  }
  if ((st & 1) != 0) {
    e = rob->opp->stats;
    *(uint *)(w + 0x90) = st;
    *(uint *)(e + 0x94) = st;
  }
  *(uint *)(w + 0x8c) = st;
  *(uint *)(w + 0x88) = fl;
}

void vs_stats_streaks_count(rob_rec *rob)
{
#define BUMP(cur, max) if (++(cur) > (max)) (max) = (cur);
#define S(p, o) (*(short *)((p) + (o)))
  int s;
  int t;
  int o;
  char *q;
  uint f;
  uint g;
  uint h;

  s = (int)rob->stats;
  q = (char *)rob->opp + 0x40;
  t = s;
  o = *(int *)(q + 0x1c);
  f = *(uint *)(s + 0x88);
  g = *(uint *)(s + 0x8c);
  h = *(uint *)(o + 0x88);
  if (pause_frame_held() == 0) {
    if ((rob->side_flags & 0x20) != 0) {
      if ((f & 1) != 0) {
        S(s, 0x1c) = 0;
      }
      if (((rob->state & 0x200000) != 0) && ((h & 0x400) == 0)) {
        f |= 4;
      }
      if (((g & 2) != 0) && ((h & 8) == 0)) {
        BUMP(S(t, 8), S(t, 10));
        S(o, 0x58)++;
      }
      if ((f & 2) != 0) {
        BUMP(S(t, 0xc), S(t, 0xe));
      } else if ((g & 0x800) != 0) {
        S(t, 0xc) = 0;
        f |= 0x04000000;
      }
      if ((f & 0x400) != 0) {
        S(o, 0x66)++;
      }
      if ((f & 0x20) != 0) {
        S(o, 0x4a)++;
        S(t, 0x4c)++;
        S(t, 4) = 0;
        S(o, 4)++; if (S(o, 4) > S(o, 6)) S(o, 6) = S(o, 4);
        if ((h & 0x1000) != 0) {
          S(o, 0x68)++;
          BUMP(S(o, 0x24), S(o, 0x26));
        }
        if ((h & 0x10000000) != 0) {
          BUMP(S(o, 0x28), S(o, 0x2a));
        } else {
          S(o, 0x28) = 0;
        }
        if ((rob->relation_flags & 1) != 0) {
          S(t, 0x78)++;
          S(o, 0x76)++;
        }
        S(o, 0x1c) = 0;
        S(o, 0x20) = 0;
      }
      if ((f & 0x8000) != 0) {
        BUMP(S(t, 0x1c), S(t, 0x1e));
        BUMP(S(t, 0x20), S(t, 0x22));
      }
      if ((g & 1) != 0) {
        if ((h & 0x400) == 0) {
          S(o, 0x18) = 0;
        }
        if ((g & 0x20000000) == 0) {
          S(o, 0x1c) = 0;
        }
        if ((g & 0x08000000) == 0) {
          S(t, 0x24) = 0;
          S(t, 0x28) = 0;
        }
      }
      if (((h & 1) != 0) && ((rob->opp->state & 0x4002) != 0)) {
        f |= 0x80000000;
      }
    }
    *(uint *)(s + 0x88) = f;
  }
}

void vs_stats_round_end(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  rob_rec *winner;
  rob_rec *loser;
  char *a;
  char *b;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if (play_style == '\x02' && (rob0->flags & 4) != 0 && (rob1->flags & 4) != 0) {
    if ((uchar)round_winner == '\0') {
      winner = rob0;
      loser = rob1;
    }
    else {
      winner = rob1;
      loser = rob0;
    }
    a = winner->stats;
    b = loser->stats;
    switch (round_end_kind) {
    case 2:
      if (++*(short *)(a + 0xae) > *(short *)(a + 0xb0)) *(short *)(a + 0xb0) = *(short *)(a + 0xae);
      if (++*(short *)(b + 0xc0) > *(short *)(b + 0xc2)) {
        *(short *)(b + 0xc2) = *(short *)(b + 0xc0);
        *(uint *)(b + 0x84) = *(uint *)(b + 0x84) | 4;
      }
      *(short *)(a + 0xb2) = 0;
      *(short *)(a + 0xc0) = 0;
      *(short *)(a + 0xc4) = 0;
      *(short *)(b + 0xc4) = 0;
      *(short *)(b + 0xae) = 0;
      *(short *)(b + 0xb2) = 0;
      break;
    case 4:
      if (++*(short *)(a + 0xb2) > *(short *)(a + 0xb4)) *(short *)(a + 0xb4) = *(short *)(a + 0xb2);
      if (++*(short *)(b + 0xc4) > *(short *)(b + 0xc6)) {
        *(short *)(b + 0xc6) = *(short *)(b + 0xc4);
        *(uint *)(b + 0x84) = *(uint *)(b + 0x84) | 8;
      }
      *(short *)(a + 0xae) = 0;
      *(short *)(a + 0xc0) = 0;
      *(short *)(a + 0xc4) = 0;
      *(short *)(b + 0xc0) = 0;
      *(short *)(b + 0xae) = 0;
      *(short *)(b + 0xb2) = 0;
      break;
    default:
      *(short *)(a + 0xae) = 0;
      *(short *)(a + 0xb2) = 0;
      *(short *)(a + 0xc0) = 0;
      *(short *)(a + 0xc4) = 0;
      *(short *)(b + 0xae) = 0;
      *(short *)(b + 0xb2) = 0;
      *(short *)(b + 0xc0) = 0;
      *(short *)(b + 0xc4) = 0;
      break;
    }
  }
}

int vs_stat_get(uint side, int kind, int own)
{
  char *a;
  char *b;
  rob_rec *rob;
  rob_rec *opp;
  int r;

  rob = rob_get(side & 0xff);
  opp = rob_get((side ^ 1) & 0xff);
  a = rob->stats;
  b = opp->stats;
  r = 0;
  switch(kind) {
  case 0:
    r = !own ? *(short *)(a + 0xc2) : *(short *)(a + 0xb0);
    break;
  case 1:
    r = !own ? *(short *)(a + 0x58) : *(short *)(a + 0x50);
    break;
  case 2:
    r = !own ? *(short *)(a + 0xc6) : *(short *)(a + 0xb4);
    break;
  case 3:
    r = !own ? *(short *)(b + 0x26) : *(short *)(a + 0x68);
    break;
  case 4:
    r = !own ? *(short *)(b + 100) : *(short *)(a + 100);
    break;
  case 5:
    r = !own ? *(short *)(b + 0x22) : *(short *)(a + 0x72);
    break;
  case 6:
    r = !own ? *(short *)(b + 0x4a) : *(short *)(a + 6);
    break;
  case 7:
    r = !own ? *(short *)(b + 0x82) : *(short *)(a + 0x82);
    break;
  }
  return r;
}

int half_gravity_active(void)
{
  char round_no;
  int active;
  
  active = 0;
  if ((game_flags & 0x800) != 0) {
    round_no = rank_round;
  }
  else {
    round_no = rank_round + '\x01';
  }
  if (msnm_rule_get() == 0xd) {
    active = 1;
  }
  else if (play_style == '\x01' && msnm_active() == 0 && round_no == '\v' && dural_alt_active != '\0') {
    active = 1;
  }
  return active;
}
