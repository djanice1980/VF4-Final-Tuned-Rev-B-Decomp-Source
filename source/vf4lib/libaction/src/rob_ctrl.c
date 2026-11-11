/*
 * vf4lib/libaction/src/rob_ctrl.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sound_request
#define KRPROTO_mothead_load
#define KRPROTO_mot_bank_load
#define KRPROTO_vec3_set_length
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char demo_type; /* 0x0c28c784 */
extern int round_time; /* 0x0c292558 */
extern uint game_flags; /* 0x0c2925b8 */
extern char round_winner; /* 0x0c2925d2 */
extern char demo_kind; /* 0x0c2925db */
extern short match_energy; /* 0x0c2925e6 */
extern float mode_timer_rate; /* 0x0c292678 */
extern char rob_start_side; /* 0x0c29f9d0 */
extern int rob_load_slots[2]; /* 0x0c29f9d4 */
extern char rob_side_chara[2]; /* 0x0c29f9dc */
extern char rob_side_costume[2]; /* 0x0c29f9e0 */
extern char rob_cmd_load_side; /* 0x0c29f9e2 */
extern char rob_cmd_load_chara; /* 0x0c29f9e3 */
extern int rob_cmd_table_peak[2]; /* 0x0c29f9e4 */
extern short hit_wait_timer; /* 0x0c29f9ec */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern static_heap_rec rob_load_heaps[2][4]; /* 0x0c360028 */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern short cam_anim_index; /* 0x0c370d5c */
extern uint cam_anim_flags; /* 0x0c370dc4 */
extern char vs_stats[2][220]; /* 0x0c37cb14 */
extern int rob_start_mirror_next; /* 0x0c42202c */
extern char move_timed_done[2][64]; /* 0x0c422030 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern rob_rec robs[2]; /* 0x0c37ba30 */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void robs_boot_init(void)
{
  rob_rec *rob0;
  rob_rec *rob1;

  robs_clear();
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  rob0->stats = &vs_stats[0][0];
  rob1->stats = &vs_stats[1][0];
  return;
}

void robs_clear(void)
{
  rob_rec *rob;
  int side;

  rob = rob_get(0);
  side = 0;
  do {
    rob->flags = 0;
    rob->side = (char)side;
    rob->rig = body_rigs;
    rob_chara_bind(rob,0);
    side = side + 1;
    rob = rob + 1;
  } while (side < 2);
  rob_coli_clear();
  return;
}

void robs_state_clear(void)
{
  rob_rec *rob;
  int i;
  int z;

  z = 0;
  rob = rob_get(0);
  for (i = 0; i < 2; i++, rob++) {
    rob->flags = z;
    rob->attack_rate = z;
    rob->card_flags = z;
  }
}

void robs_round_init(void)
{
  rob_rec *rob;
  int i;

  rob = rob_get(0);
  for (i = 0; i < 2; i++, rob++) {
    rob->reach.flags = 8;
    rob->reach.look_weight = 1.0;
    rob->flags = rob->flags | 0x100000;
  }
  hit_light_reset();
}

void robs_card_flags_refresh(void)
{
  int side;

  if ((game_flags & 0x100000) != 0) {
    side = 0;
    do {
      rob_card_flags_high_set((int)(char)side);
      side = side + 1;
    } while (side < 2);
  }
  return;
}

void robs_match_clear(void)
{
  robs_state_clear();
  rob_work_buffers_assign();
  return;
}

void rob_round_reset(rob_rec *rob)
{
  vec3 zero = { 0, 0, 0 };
  int i;
  int z;
  short *s;
  float one;
  float f0;

  z = 0;
  one = 1.0;
  f0 = 0.0;
  rob->flags = rob->flags & 0x40004004 | 0x100001;
  if (rope_stage_on != 0)
    rob->flags |= 0x800;
  rob->flags_copy = rob->flags;
  rob->skel_heading = rob->heading;
  rob->pos.y = one;
  rob->frame_speed = one;
  rob->floor_y = one;
  rob->floor_y_kept = one;
  rob->velocity = zero;
  rob->knockback = zero;
  rob->slide_velocity = zero;
  rob->throw_offset = zero;
  rob->push_velocity = zero;
  rob->approach_target = zero;
  hit_wait_timer_set((short)z);
  rob->state = 24;
  rob->flags24 = z;
  rob->move_no = z;
  rob->face_motion = z;
  rob->frame_parity = z;
  rob->side_flags = z;
  rob->action.word = rob->request.word = rob->request_pending.word = z;
  rob->action_step = z;
  rob->field_0xfa = z;
  rob->field_0x30 = z;
  rob->guard_state = z;
  rob->stun = z;
  rob->field_0x166 = z;
  rob->field_0x226 = z;
  rob->pending_damage = z;
  rob->slot = z;
  rob->field_0x220 = z;
  rob->combo[0].hits = z;
  rob->combo[0].damage = z;
  rob->combo[1].hits = z;
  rob->combo[1].damage = z;
  rob->combo[2].hits = z;
  rob->combo[2].damage = z;
  rob->combo[3].hits = z;
  rob->combo[3].damage = z;
  rob->combo[4].hits = z;
  rob->combo[4].damage = z;
  rob->combo_hits_best = z;
  rob->combo_damage_best = z;
  rob->max_wall_combo = z;
  rob->max_wall_damage = z;
  rob->max_combo = z;
  rob->max_damage = z;
  rob->press_count = z;
  rob->field_0x100 = z;
  rob->event_flags = z;
  rob->field_0x174 = z;
  rob->relation_flags = z;
  rob->reach_flags = z;
  rob->field_0x124 = z;
  rob->field_0x106 = z;
  rob->followup_count = rob->followup_flags = z;
  rob->throw_damage = z;
  rob->field_0x11a = z;
  rob->field_0x11b = z;
  rob->frame_disadvantage = z;
  rob->guard_input_hold = z;
  rob->guard_input_time = z;
  rob->field_0x178 = z;
  rob->field_0x544 = z;
  rob->grip_mode = 0;
  rob->grip_bone = 0;
  rob->grip_link = 0;
  rob->field_0x115 = z;
  rob->field_0x10c = z;
  rob->field_0x114 = z;
  rob->field_0x110 = z;
  rob->field_0x223 = z;
  rob->field_0x551 = z;
  rob->attack_rate_delta = z;
  rob->approach_mode = 0;
  rob->field_0x554 = f0;
  rob->field_0x558 = f0;
  rob->reach.rate = f0;
  rob->blend_length = 5.0;
  rob->opp_dir = rob->heading;
  rob->opp_dist = 2.0;
  s = rob->attack_history;
  i = 16;
  do {
    *s = z;
    s++;
  } while (--i != 0);
  rob->attack_history_idx = z;
  rob->knockback_scale = 0.0;
  rob->motion_flags = 2;
  rob->field_0x3b4 = 5;
  rob->root_offset_y = f0;
  rob->yaw_delta = z;
  rob_card_flags_low_set((char)rob->side);
  if (rob->side != 0)
    rob->opp = rob_get(0);
  else
    rob->opp = rob_get(1);
  rob->pos = rob->ground_pos = rob->skel_place_pos = rob->place_pos;
  rob->stance = z;
  rob->reaction = **(uchar ***)(rob->chara_data + 0x40);
  rob->life_max = rob->life = match_energy;
  if (msnm_active() != 0) {
    rob->life = msnm_cpu_energy_get((char)rob->side);
    rob->life_max = msnm_match_energy_get((char)rob->side);
  }
  if ((game_flags & 0x200) != 0)
    rob->life = 1;
  rob->old_idx = 1;
  make_old_task_frame();
  rob->eye_look = z;
  rob->eye_angles[0] = rob->eye_angles[1] = z;
  rob->eye_angles[2] = rob->eye_angles[3] = z;
  rob->eye_angles[4] = rob->eye_angles[5] = z;
  rob->cam_bits = z;
  rob_hands_reset(rob);
  rob_face_reset(rob);
  rob_limbs_reset(rob);
  rob_facing_override_clear(rob);
  if (rob->chara == 3)
    rob->flags |= 0x80000;
}

void rob_hands_reset(rob_rec *rob)
{
  rob_hand_rec *hand = rob->hands;
  short z = 0;
  hand->flags = z;
  hand->shape = 0xff;
  hand->field_0x0 = 0.0;
  hand->target = 0.0;
  hand->current = 0.0;
  hand->frames = z;
  hand++;
  hand->flags = z;
  hand->shape = 0xff;
  hand->field_0x0 = 0.0;
  hand->target = 0.0;
  hand->current = 0.0;
  hand->frames = z;
  return;
}

void rob_face_reset(rob_rec *rob)
{
  int z = 0;
  rob->face_flags = z;
  rob->face_timer = z;
  rob->rigid_face = 0xffffffff;
  return;
}

void rob_limbs_reset(rob_rec *rob)
{
  rob_reach_rec *reach;
  rob_limb_rec *limb;
  char i;
  reach = &rob->reach;
  limb = reach->limbs;
  reach->flags = 0;
  reach->look_weight = 0.0f;
  for (i = 0; i < 4; i++, limb++) {
    limb->bone = 0;
  }
}

void rob_chara_load(int chara_arg, int costume_arg, int side_arg)
{
  char chara = (char)chara_arg;
  char costume = (char)costume_arg;
  char side = (char)side_arg;
  char *tab;
  uint slot;

  tab = rob_side_chara; tab[side] = chara;
  tab = rob_side_costume; tab[side] = costume;
  tab = rob_reload_chara; tab[side] = chara;
  tab = rob_reload_costume; tab[side] = costume;
  slot = (uchar)(side + 1);
  mothead_load(slot,*(byte *)((char *)chara_data_get(chara) + 0x32));
  if (objbank_in_use(rob_load_slots[side]) == 0) {
    rob_obj_load(side,chara,costume);
    rob_command_table_load(side,chara);
    mot_bank_load(slot,*(uchar *)((char *)chara_data_get(chara) + 0x32));
  }
}

void rob_costume_colors_send(int side_arg)
{
  e_mode_main mode;
  int side;
  int costume;
  int chara;
  char *tab;

  side = (int)(char)side_arg;
  tab = rob_side_chara;
  chara = (int)tab[side];
  tab = rob_side_costume;
  costume = (int)tab[side];
  mode = mode_get_main();
  if (mode != MODE_MAIN_ENDING) {
    if (rob_obj_alt[side] != '\0') {
      rob_hand_models_build(side,*(uint *)(costume * 0x20 + (int)chara_data_get(chara)[0x12] + 0x4c));
    }
    else {
      rob_hand_models_build(side,*(uint *)(costume * 0x20 + (int)chara_data_get(chara)[0x12] + 0xc));
    }
    rob_costume_set_nop(side,chara,costume);
  }
  return;
}

void rob_chara_setup(char chara, char costume, char side)
{
  if (-1 < (char)chara) {
    rob_chara_load((int)(char)chara,(int)(char)costume,(int)(char)side);
    rob_costume_colors_send((int)(char)side);
  }
  return;
}

void rob_chara_load_queued(int chara_arg, int costume_arg, int side_arg)
{
  char chara = (char)chara_arg;
  char costume = (char)costume_arg;
  char side = (char)side_arg;
  char *tab;
  uint slot;

  tab = rob_side_chara; tab[side] = chara;
  tab = rob_side_costume; tab[side] = costume;
  tab = rob_reload_chara; tab[side] = chara;
  tab = rob_reload_costume; tab[side] = costume;
  if (objbank_in_use(rob_load_slots[side]) == 0) {
    rob_obj_load_queued(side,chara,costume);
    rob_command_table_load(side,chara);
    if (demo_kind == 0 && mode_get_main() == MODE_MAIN_ADVERTISE) {
      if (side != 0) {
        return;
      }
      switch (demo_type) {
      case 0:
        mot_bank_load_req(1,0x11);
        mothead_load_req(1,0x11);
        break;
      case 1:
        mot_bank_load_req(1,0x12);
        mothead_load_req(1,0x12);
        break;
      case 2:
        mot_bank_load_req(1,0x13);
        mothead_load_req(1,0x13);
        break;
      default:
        mot_bank_load_req(1,0x11);
        mothead_load_req(1,0x11);
        break;
      }
    }
    else {
      slot = (uchar)(side + 1);
      mot_bank_load_req(slot,*(uchar *)((char *)chara_data_get(chara) + 0x32));
      mothead_load_req(slot,*(uchar *)((char *)chara_data_get(chara) + 0x32));
    }
  }
}

void rob_command_table_load(int side_arg, int chara_arg)
{
  int *tab[16];
  char side = side_arg;
  char chara = chara_arg;
  uint size;
  int base;
  int zero;
  int *p;
  int **q;
  int n;
  int k;
  int j;
  int v;
  int m;
  int t;
  int *e;
  int *last;
  int **r;

  p = (int *)rob_load_heaps[side][0].addr;
  size = rob_load_heaps[side][0].size;
  base = (int)p;
  rob_cmd_load_side = side;
  rob_cmd_load_chara = chara;
  zero = 0;
  load_file(*(char **)((char *)chara_data_get(chara) + 0x38),p,size);
  q = tab;
  n = 0;
  while (*p != 0) {
    v = *p;
    v += base;
    *q++ = (int *)v;
    *p++ = v;
    n++;
  }
  *q = 0;
  p += 2;
  for (k = n - 1, r = tab + 1; k > 0; k--, p = *r++) {
    last = 0;
    for (j = 7; j != 0; j--) {
      v = *p;
      v += base;
      *p = v;
      p++;
    }
    t = *p;
    if (t == 0) {
      m = 0;
    }
    else {
      m = (*p = t / 4);
      last = (int *)v;
    }
    p++;
    while (p < *r) {
      if ((m != 0) & (p >= last)) {
        for (; m != 0; m--) {
          *p += base;
          p++;
        }
        break;
      }
      switch (*(char *)p) {
      case 24:
        e = p;
        if (p[1] != 0) p[1] += base;
        if (e[2] != 0) e[2] += base;
        if (e[3] != 0) e[3] += base;
        p += 4;
        break;
      case 59:
        if (p[3] != 0) p[3] += base;
        p += 4;
        break;
      case 1: case 10: case 11: case 12: case 19: case 44: case 47: case 65: case 68:
        if (p[1] != 0) p[1] += base;
      case 2: case 3: case 16: case 17: case 20: case 22: case 23: case 27: case 28: case 29:
      case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 38: case 45: case 46:
      case 49: case 52: case 53: case 54: case 55: case 56: case 57: case 60: case 61: case 62:
        p += 2;
        break;
      case 21: case 26: case 41: case 42: case 77:
        if (p[2] != 0) p[2] += base;
        p += 3;
        break;
      default:
        p += 1;
        break;
      }
    }
  }
  if (zero > rob_cmd_table_peak[side]) {
    rob_cmd_table_peak[side] = zero;
  }
}

int robs_start(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  int start;
  char *swap;
  uchar *dst;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  start = stage_cur->start_pos;
  if ((side_entry_flags[0] & 4U) != 0) {
    rob0->flags = 4;
  }
  else {
    rob0->flags = 0;
  }
  if ((side_entry_flags[1] & 4U) != 0) {
    rob1->flags = 4;
  }
  else {
    rob1->flags = 0;
  }
  rob0->costume = rob_side_costume[0];
  rob1->costume = rob_side_costume[1];
  dst = &rob0->field_0x3;
  swap = chara_costume_swap;
  *dst = rob_side_costume[0] ^ swap[rob_side_chara[0]];
  rob1->field_0x3 = rob_side_costume[1] ^ swap[rob_side_chara[1]];
  rob_start(0,rob_side_chara[0],start);
  rob_start(1,rob_side_chara[1],start + 0x10);
  return 0;
}

void rob_start_mirror_request(void)
{
  rob_start_mirror_next = 1;
  return;
}

void rob_start(uchar side, char chara, int start)
{
  rob_rec *rob;
  float v;
  rob = rob_get(side);
  rob_start_side = side;
  v = *(float *)(start + 4);
  rob->pos.x = v;
  rob->ground_pos.x = v;
  rob->place_pos.x = v;
  v = *(float *)(start + 8);
  rob->pos.y = v;
  rob->ground_pos.y = v;
  rob->place_pos.y = v;
  v = *(float *)(start + 0xc);
  rob->pos.z = v;
  rob->ground_pos.z = v;
  rob->place_pos.z = v;
  rob->heading = *(short *)(start + 2);
  rob->old_idx = 1;
  rob_chara_bind(rob,(uchar)chara);
  rob_round_reset(rob);
  control_init(rob);
  rob_coli_init(rob);
  if (chara == 0xf || rob_start_mirror_next != 0) {
    rob->side_flags |= 8;
    rob_start_mirror_next = 0;
  }
  rob_skeleton_init(rob);
  rob_disp_setup(rob);
}

void rob_flags_clear(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  rob->flags = 0;
  return;
}

void robs_flags_clear(void)
{
  int side;

  side = 0;
  do {
    rob_flags_clear((int)(char)side);
    side = side + 1;
  } while (side < 2);
  return;
}

void rob_chara_bind(rob_rec *rob, uchar chara)
{
  char *data;
  short h;

  data = (char *)chara_data_get(chara);
  rob->chara_data = (uchar *)data;
  rob->chara = *(byte *)(data + 0x30);
  rob->field_0x128 = *(float *)(data + 8);
  rob->rig = body_rigs + *(byte *)(data + 0x31);
  if (rob_obj_alt[rob->side] != 0) {
    rob->flags |= 0x40000000;
    int costumes = *(int *)(data + 0x48);
    uint costume = rob->costume;
    rob->body_model = *(int *)(costumes + costume * 32 + 0x40);
    h = *(short *)(*(int *)(data + 0x48) + costume * 32 + 0x58);
  }
  else {
    rob->flags &= 0xbfffffff;
    int costumes = *(int *)(data + 0x48);
    uint costume = rob->costume;
    rob->body_model = *(int *)(costumes + costume * 32);
    h = *(short *)(*(int *)(data + 0x48) + costume * 32 + 0x18);
  }
  rob->field_0x53a = h;
  rob->move_table = **(ushort ***)(data + 0x34);
  rob->move_tables = *(ushort ***)(data + 0x34);
  rob->fl_rate = *(float *)(*(int *)(data + 0x48) + 0x98);
  { int d = (int)&control_objects[0][0x5c]; *(int *)(d + (uint)rob->side * 0x9c) = (int)rob_load_heaps[rob->side][0].addr; }
  switch (chara) {
  case 1:
  case 5:
  case 10:
  case 0xc:
    rob->flags |= 0x4000;
    break;
  default:
    rob->flags &= 0xffffbfff;
  }
}

void rob_pos_reset(void)
{
#define SET(rob, s) \
  rob->place_pos.x = rob->ground_pos.x = rob->pos.x = *(float *)(s + 4); \
  rob->place_pos.y = rob->ground_pos.y = rob->pos.y = *(float *)(s + 8); \
  rob->place_pos.z = rob->ground_pos.z = rob->pos.z = *(float *)(s + 0xc); \
  rob->heading = *(short *)(s + 2); \
  rob->flags = rob->flags & 0xfffffff7; \
  rob->state = rob->state | 0x10; \
  rob->side_flags = rob->side_flags & 0xffe7;
  int start;
  rob_rec *rob;

  start = stage_cur->start_pos;
  rob = rob_get(0);
  SET(rob, start);
  start += 0x10;
  rob++;
  SET(rob, start);
}

void rob_costume_set_nop(int side, int chara, int costume)
{
  return;
}

void rob_card_flags_low_set(int side_arg)
{
  card_record_rec *record;
  rob_rec *rob;
  char side;
  uint flags;

  side = (char)side_arg;
  record = card_record_get(side);
  rob = rob_get((uchar)side);
  if (((rob != 0 && record != 0) && ((rob->flags & 4) != 0)) &&
     (record->in_use != 0)) {
    flags = rob->card_flags & 0xffffff80;
    if (item_has_79(side) != 0) flags |= 1;
    if (item_has_7a(side) != 0) flags |= 2;
    if (item_has_7b(side) != 0) flags |= 4;
    if (item_has_7c(side) != 0) flags |= 8;
    if (item_has_7d(side) != 0) flags |= 0x10;
    if (item_has_7e(side) != 0) flags |= 0x20;
    if (item_has_7f(side) != 0) flags |= 0x40;
    rob->card_flags = flags;
  }
}

void rob_card_flags_high_set(int side_arg)
{
  char side;
  card_record_rec *record;
  card_image_rec *image;
  rob_rec *rob;
  uint flags;

  side = (char)side_arg;
  record = card_record_get(side);
  image = card_image_get(side);
  rob = rob_get((byte)side);
  if (((rob != 0 && record != 0) && ((rob->flags & 4) != 0)) &&
     (record->in_use != 0)) {
    flags = rob->card_flags & 0x7f;
    if (card_hdr_ftf_skill_test(&image->header,0)) {
      flags = flags | 0x80;
    }
    if (card_hdr_ftf_skill_test(&image->header,1)) {
      flags = flags | 0x100;
    }
    rob->card_flags = flags;
  }
  return;
}

void rob0_reload(void)
{
  rob_chara_setup((int)rob_reload_chara[0],(int)rob_reload_costume[0],0);
  robs_start();
  return;
}

void rob1_reload(void)
{
  rob_chara_setup((int)rob_reload_chara[1],(int)rob_reload_costume[1],1);
  robs_start();
  return;
}

void rob_ctrl_task_start(void)
{
  int i;
  rob_rec *rob;

  task_start(&task_make_old);
  task_start(&task_enemy);
  task_start(&task_control);
  task_start(&task_rob_coli);
  task_start(&task_rob_info);
  task_start(&task_rob_coli_atk);
  task_start(&task_rob_effect);
  rob = robs;
  i = 0;
  do {
    rob->flags = 0;
    i = i + 1;
    rob = rob + 1;
  } while (i < 2);
  return;
}

void rob_ctrl_task_frame(void)
{
  int paused;

  paused = pause_frame_held();
  if (paused == 0) {
    robs_frame_clear();
    rob_action_requests_take();
    robs_frame_step();
  }
  return;
}

void rob_ctrl_task_free(void)
{
  robs_clear();
  task_stop(&task_make_old);
  task_stop(&task_control);
  task_stop(&task_rob_coli);
  task_stop(&task_rob_info);
  task_stop(&task_rob_coli_atk);
  task_stop(&task_rob_effect);
  return;
}

void make_old_task_start(void)
{
  rob_rec *rob;
  int n;

  rob = robs;
  n = 2;
  do {
    rob->old_idx = 1;
    rob_old_push(rob);
    rob = rob + 1;
    n = n + -1;
  } while (n != 0);
  return;
}

void make_old_task_frame(void)
{
  uint paused;
  rob_rec *rob;
  int n;

  paused = pause_frame_held();
  if (paused == 0) {
    n = 2;
    rob = robs;
    do {
      if ((rob->flags & 1) != 0) {
        rob_old_push(rob);
      }
      rob = rob + 1;
      n = n + -1;
    } while (n != 0);
  }
  return;
}

void rob_old_push(rob_rec *rob)
{
  rob_old_rec *prev;
  rob_old_rec *cur;
  int n;

  prev = rob->old + rob->old_idx;
  n = rob->old_idx + 1U & 3;
  cur = rob->old + n;
  rob->old_idx = n;
  rob->old_cur = cur;
  cur->move_no = rob->move_no;
  cur->frame = rob->frame;
  cur->heading = rob->heading;
  cur->flags = rob->flags;
  cur->state = rob->state;
  cur->flags24 = rob->flags24;
  cur->flags28 = rob->flags28;
  cur->flags2c = rob->flags2c;
  cur->move_flags = rob->move_flags;
  cur->relation_flags = rob->relation_flags;
  cur->life = rob->life;
  cur->action.word = rob->action.word;
  cur->field_0x16 = rob->field_0x22a;
  cur->field_0x18 = rob->field_0x22c;
  cur->stun = rob->stun;
  cur->attack_rate = rob->attack_rate;
  cur->active_start = rob->active_start;
  cur->field_0x0 = rob->field_0x30;
  cur->field_0x50 = rob->field_0x328;
  cur->attack_flags2 = rob->attack_flags2;
  cur->attack_level = rob->attack_level;
  cur->place_pos = rob->place_pos;
  cur->pos = rob->pos;
  cur->ground_pos = rob->ground_pos;
  cur->velocity = rob->velocity;
  cur->pos_prev = prev->pos;
  cur->flags_prev = prev->flags;
  cur->state_prev = prev->state;
  cur->flags24_prev = prev->flags24;
  cur->flags28_prev = prev->flags28;
  cur->flags2c_prev = prev->flags2c;
  cur->action_prev.word = prev->action.word;
  cur->relation_flags_prev = prev->relation_flags;
  cur->side_flags = rob->side_flags;
  cur->field_0x12 = rob->field_0x2f8;
  cur->facing_offset = rob->facing_offset;
}

void robs_frame_clear(void)
{
  rob_rec *rob;
  int side;

  side = 0;
  rob = robs;
  do {
    if ((rob->flags & 1) != 0) {
      rob->field_0x226 = 0;
      rob->field_0x108 = 0xffff;
      rob_frame_word[side] = 0;
    }
    side = side + 1;
    rob = rob + 1;
  } while (side < 2);
  return;
}

void robs_frame_step(void)
{
  rob_rec *rob;
  int i;
  float *t;

  for (i = 0, rob = robs; i < 2; i++, rob++) {
    if ((rob->flags & 1) != 0) {
      t = rob->bone_mtx;
      rob->bone_mtx = rob->bone_mtx_prev;
      rob->bone_mtx_prev = t;
      rob->frame_parity ^= 1;
      if ((game_flags & 0x2000) == 0) {
        if (rob->field_0x3b4 > 0) (rob->field_0x3b4)--;
        if (rob->field_0x119 > 0) (rob->field_0x119)--;
        if (rob->field_0x124 > 0) (rob->field_0x124)--;
        if (rob->field_0x104 > 0) (rob->field_0x104)--;
        if (rob->field_0x106 > 0) (rob->field_0x106)--;
        if (rob->field_0x316 > 0) (rob->field_0x316)--;
        else {
          rob->field_0x340 = 0;
          rob->field_0x370 = 1.0;
        }
        (rob->field_0x100)++;
        (rob->field_0xfa)++;
        rob->field_0x274 = 0;
        if ((rob->state & 0x400) == 0) {
          rob->flags &= 0xffdfffff;
        }
        rob_frame_advance(rob);
      }
    }
  }
  for (i = 0, rob = robs; i < 2; i++, rob++) {
    if ((rob->flags & 1) != 0) {
      rob_move_frame(rob);
    }
  }
  for (i = 0, rob = robs; i < 2; i++, rob++) {

    if ((rob->flags & 1) != 0) {
      if ((game_flags & 0x2000) == 0) {
        rob_velocity_apply(rob);
        rob_turn_update(rob);
        rob_state_flags_update(rob);
      }
      rob_skeleton_update(rob);
      face_rigid_pick(rob);
      rob_hands_step(rob);
    }
    rob->heading_deg = (rob->heading * 0x168) >> 16;
    rob->facing_deg = (rob->facing * 0x168) >> 16;
    rob->facing_offset_deg = (rob->facing_offset * 0x168) >> 16;
    rob->body_yaw_deg = (rob->body_yaw * 0x168) >> 16;
    rob->attack_angle_deg = (rob->attack_angle * 0x168) >> 16;
  }
  hit_lights_frame();
}

void rob_move_frame(rob_rec *rob)
{
  short frame;
  int limit;
  short *timer;

  if ((game_flags & 0x2000) != 0) {
    rob_motion_set(rob);
    if ((rob->side_flags & 0x20) != 0) {
      move_timed_done_clear(rob);
    }
    if (rob->timed_list != 0) {
      move_timed_run(rob);
    }
    timer = &rob->field_0x3b4;
    if (0 < *timer) {
      *timer = *timer + -1;
    }
  }
  else {
    rob_action_step(rob);
    move_event_branch_run(rob);
    rob_move_step(rob);
    if (msnm_rule_get() == 9) {
      frame = rob->frame;
      limit = msnm_combo_d0_get((uint)rob->side);
      if (frame >= limit) {
        msnm_combo_reset((uint)rob->side);
      }
    }
  }
  return;
}

float move_sync_rate(rob_rec *rob, int entry)
{
  short left;
  float rate;

  left = (short)*(char *)(entry + 7) - rob->opp->frame;
  rate = 1.0;
  switch(*(char *)(entry + 6)) {
  case 0:
    break;
  case 1:
    left = rob->opp->move_length + left;
    break;
  case 2:
    left = rob->opp->active_start + left;
    break;
  case 3:
    left = rob->opp->active_end + left;
    break;
  case 4:
    left = rob->opp->recovery_end + left;
    break;
  }
  if (0 < left) {
    float n = (float)(int)*(short *)(entry + 4) - rob->frame_pos;
    rate = n / (float)(int)left;
  }
  return rate;
}

byte rob_slow_check(rob_rec *rob)
{
  rob_rec *opp;
  byte r;

  r = 0;
  opp = rob->opp;
  if (half_gravity_active() == 0) {
    return 0;
  }
  if (((rob->move_flags & 0x4080) == 0x4000) &&
     ((rob->state & 0x40) != 0)) {
    r = 1;
  }
  if ((((rob->move_flags & 0x480) == 0x480) &&
      ((rob->state & 0x40) != 0)) && ((rob->action.part.lever & 1) != 0)) {
    r = 1;
  }
  if ((((rob->state & opp->state & 0x40) != 0) &&
      ((rob->state & 0x300000) != 0)) && ((opp->state & 0x300000) != 0)) {
    r = 1;
  }
  if ((((rob->state & 0x200000) != 0) &&
      (rob->opp->ctrl->continuation == 0)) &&
     ((rob->frame < rob->field_0x2f2 &&
      ((rob->coli->field_0x3c[1] == 0 &&
       (rob->coli->touch_bits == 0)))))) {
    r = 1;
  }
  if ((rob->action.part.type == '\0') && ((rob->state & 0x40) != 0)) {
    r = 1;
  }
  if ((rob->state & 0x200000) != 0) {
    if ((rob->opp->state & 0x100000) != 0) {
      if ((rob->opp->action.part.lever & 4) != 0) {
        r = 0;
      }
    }
    if ((short)rob->move_no == 0x14cc) {
      r = 0;
    }
  }
  return r;
}

void rob_frame_advance(rob_rec *rob)
{
  int flags;
  int v;
  uint *entry;
  rob_rec *opp;
  uint f;
  float rate;
  float rate_min;
  float rate_max;
  float *pos;
  float *spd;
  float speed;
  float prev_pos;
  short left;
  short land;
  char *slow;
  rob_old_rec *old;
  short hold;
  int k;

  v = (short)rob->side_flags;
  flags = v & 0xff9f;
  rob->frame_speed = mode_timer_rate;
  if (v & 0x80) {
    entry = (uint *)rob->field_0x37c;
    opp = rob->opp;
    rate = 1.0;
    if (rob->frame_pos < (float)*(short *)(entry + 1)) {
      f = *entry;
      rate_min = *(float *)(entry + 2);
      rate_max = *(float *)(entry + 3);
      if (f & 0x100) {
        if ((opp->field_0x30 & 2) && (k = f & 0xff, move_sync_by_level[opp->attack_level][k] != '\0')) {
          rate = move_sync_rate(rob, (int)entry);
        }
      }
      else if (f & 0x200) {
        rate = move_sync_rate(rob, (int)entry);
      }
      if (rate_min > rate) rate = rate_min;
      if (rate > rate_max) rate = rate_max;
    }
    else {
      flags &= ~0x80;
    }
    rob->frame_speed = rob->frame_speed * rate;
  }
  else {
    slow = &rob->slow_frames;
    if (*slow != '\0') {
      rob->frame_speed = rob->frame_speed * rob->slow_rate;
      *slow = *slow - 1;
    }
  }
  pos = &rob->frame_pos;
  spd = &rob->frame_speed;
  prev_pos = *pos;
  speed = *spd;
  *pos = prev_pos + speed;
  rob->frame = (int)*pos;
  if (rob->flags & 8) {
    if (rob->frame > rob->field_0x2f0) {
      left = (short)(int)((float)(rob->field_0x2f2 - rob->frame) * speed);
      land = rob->frames_to_land;
      if (left < land) {
        if (half_gravity_active() == 1) {
          if (left < land >> 1) {
            *pos = *pos - *spd;
          }
          else {
            rob->frame_pos = rob->frame_pos - rob->frame_speed * 0.5;
          }
        }
        else {
          rob->frame_pos = rob->frame_pos - rob->frame_speed;
        }
        rob->frame = (short)(int)rob->frame_pos;
      }
      else if (left > land) {
        rob->frame_pos = rob->frame_speed * 0.5 + rob->frame_pos;
        rob->frame = (short)(int)rob->frame_pos;
      }
    }
    else {
      if (half_gravity_active() == 1) {
        rob->frame_pos = rob->frame_pos - rob->frame_speed * 0.5;
      }
    }
  }
  else {
    old = rob->old_cur;
    if ((old->flags_prev & 8) && (old->action_prev.part.type == rob->action.part.type || rob->action.part.type == 15)) {
      hold = rob->field_0x2f2;
      if (rob->frame < hold) {
        rob->frame = hold;
        rob->frame_pos = (float)hold;
      }
    }
    else if (rob_slow_check(rob)) {
      rob->frame_pos = rob->frame_pos - rob->frame_speed * 0.5;
      rob->frame = (short)(int)rob->frame_pos;
    }
  }
  if (rob->frame != rob->old_cur->frame) {
    flags |= 0x40;
  }
  if (rob->move_length < rob->frame) {
    flags |= 2;
    if (rob->followup_word == 0) {
      flags |= 4;
    }
  }
  else {
    flags &= ~6;
  }
  rob->side_flags = flags;
}

void rob_state_flags_update(rob_rec *rob)
{
  uint st;
  uint f;
  char cnt;
  short lim;
  short h;
  int b;
  rob_rec *opp;
  char old;
  char v;

  if (rob->field_0x108 >= 0) {
    rob->stun = rob->field_0x108;
  }
  rob_guard_state_update(rob);
  st = rob->state;
  f = rob->flags;
  f &= 0xfbff7fef;
  rob->facing = (st & 0x100) ? ((rob->side_flags & 0x10) ? rob->heading - rob->facing_offset : rob->heading + rob->facing_offset) : rob->heading;
  if ((rob->flags28 & 0x400) != 0) {
    f |= 0x4000000;
  }
  if ((rob->state & 1) != 0) {
    if ((st & 0x8002000) != 0) f |= 0x10;
    else f &= ~0x10;
  }
  else {
    lim = rob->frame;
    h = rob->move_length >> 1;
    b = rob->field_0x284;
    if ((st & 0x2000) != 0) {
      int o = 0x324;
      if ((*(uint *)((char *)rob + o) & 0x4000) != 0) f |= 0x10;
      else if ((b & 0x80) != 0) f |= 0x10;
      else if (b == 3) f |= 0x10;
      else if ((rob->flags & 0x4000000) != 0) f |= 0x10;
      else if (lim > h) f |= 0x10;
    }
    else {
      if ((b >> 7 & 1 & (lim <= h)) != 0) f |= 0x10;
    }
  }
  cnt = 0;
  if ((st & 0x14000) != 0) {
    rob->combo[0].hits = 0;
    rob->combo[0].damage = 0;
    cnt = 1;
  }
  if ((st & 1) == 0 && (f & 0x80) == 0 && rob->stun == 0) {
    rob->combo[0].hits = 0;
    rob->combo[0].damage = 0;
    cnt++;
  }
  if ((st & 0xc00) == 0) {
    rob->combo[2].hits = 0;
    rob->combo[2].damage = 0;
    rob->combo[3].hits = 0;
    rob->combo[3].damage = 0;
    rob->field_0x224 = 0;
    attack_history_clear(rob->opp);
    cnt++;
  }
  if ((st & 0x40) == 0) {
    rob->combo[1].hits = 0;
    rob->combo[1].damage = 0;
    cnt++;
  }
  if (cnt > 2) {
    f &= 0xf7ffffff;
  }
  f = rob_flag_11b_take(rob,f);
  if ((f & 0x20000) != 0) {
    opp = rob->opp;
    if ((short)opp->move_no == 0xd27 && (opp->state & 0x4000000) == 0) {
      f &= 0xfffdffff;
      rob->side_flags &= 0xafff;
    }
  }
  else {
    opp = rob->opp;
    if ((opp->state & 0x4000000) != 0 && opp->frame < opp->active_end &&
        (opp->flags28 & 0x18) == 0 && (rob->field_0x30 & 8) == 0 &&
        (rob->flags28 & 0x20) == 0 && (rob->turn_flags & 2) == 0) {
      f |= 0x20000;
      rob->side_flags |= 0x1000;
    }
  }
  if ((f & 8) != 0 && rob->velocity.y > -1.1920929e-07) {
    f |= 0x40;
  }
  else {
    f &= ~0x40;
  }
  if (rob->attack_rate_delta != 0) {
    old = rob->attack_rate;
    v = rob->attack_rate_delta + old;
    v = v > 40 ? 40 : (v < 0 ? 0 : v);
    rob->attack_rate_delta = 0;
    rob->attack_rate = v;
    if (old != v) f |= 0x80000;
  }
  if (move_branch_open(rob) != 0) {
    f |= 0x8000;
  }
  rob->flags = f;
}

void rob_guard_state_update(rob_rec *rob)
{
  uint f = rob->field_0x33c;
  uint s20 = rob->state;
  uint s24 = rob->flags24;
  ushort h = rob->ctrl->buttons;
  uint b;
  short a0;
  short a1;
  rob->state = s20 & 0xffff7fff;
  rob->guard_state = 0;
  if (s24 & 0x30) {
    if (s24 & 0x10) {
      rob->guard_state = (s24 & 0x20) ? 5 : 1;
    }
    else {
      rob->guard_state = 3;
    }
    return;
  }
  if (f != 0) {
    if ((int)f < 0 && !(h & 1)) return;
    a0 = rob->frame;
    a1 = rob->field_0x30a;
    if (((int)f >> 16 & 1) & (a0 < a1)) return;
    if (((int)f >> 15 & 1) & (a1 < a0)) return;
    if (rob->stun != 0) {
      if (f & 0x40000000) return;
      if (f & 0x01000000) {
        rob->guard_state = (h & 0x40) ? 3 : 1;
      }
      else if (f & 0x00800000) {
        rob->guard_state = 5;
      }
      else if (f & 0x00400000) {
        rob->guard_state = 1;
      }
      else if (f & 0x00200000) {
        rob->guard_state = 3;
      }
      else {
        b = f & 0xff;
        rob->guard_state = (b == 3) ? ((h & 0x40) ? 3 : 1) : (b == 0) ? 5 : (b == 2) ? 3 : 1;
      }
    }
    else {
      if (f & 0x02000000) return;
      if (f & 0x20000000) {
        rob->guard_state = (h & 0x40) ? 3 : 1;
      }
      else if (f & 0x10000000) {
        rob->guard_state = 5;
      }
      else if (f & 0x08000000) {
        rob->guard_state = 1;
      }
      else if (f & 0x04000000) {
        rob->guard_state = 3;
      }
      else {
        b = f & 0xff;
        rob->guard_state = (b == 3) ? ((h & 0x40) ? 3 : 1) : (b == 0) ? 5 : (b == 2) ? 3 : 1;
      }
    }
  }
  else {
    if (rob->field_0x30 & 4) {
      if (rob->frame < rob->active_start) return;
      if (rob->frame >= rob->active_end) return;
      rob->guard_state = (s20 & 0x2000) ? 3 : 1;
    }
    else {
      if (!(s20 & 0x8000)) return;
      if (s20 & 1) {
        rob->guard_state = (s20 & 0x2000) ? 3 : 1;
      }
      else {
        rob->guard_state = (h & 0x40) ? 3 : 1;
      }
    }
  }
  rob->state = s20 | 0x8000;
}

void rob_velocity_apply(rob_rec *rob)
{
  vec3 v = rob->velocity;
  vec3 vel = {0.0, 0.0, 0.0};
  vec3 out;
  float s;
  float k;
  short n;
  uchar c;

  s = rob->frame_speed;
  if (s < 0.01) {
    s = 0.01;
  }
  else if (s > 2.0) {
    s = 2.0;
  }
  if ((rob->flags & 8) != 0) {
    if (half_gravity_active() == 1) {
      v.y = v.y + rob->slide_velocity.y * 0.5;
    }
    else {
      v.y = v.y + rob->slide_velocity.y;
    }
    vel.x = v.x + rob->slide_velocity.x + rob->throw_offset.x;
    vel.y = v.y + rob->throw_offset.y;
    vel.z = v.z + rob->slide_velocity.y + rob->throw_offset.z;
    v.y = v.y - world_gravity * s * rob->gravity_scale;
  }
  else if ((~rob->flags & 0x40) != 0) {
    v.y = 0.0;
    vel.x = v.x + rob->slide_velocity.x + rob->throw_offset.x;
    vel.y = rob->throw_offset.y;
    vel.z = v.z + rob->slide_velocity.y + rob->throw_offset.z;
    if ((rob->state & 0xc00) != 0) {
      k = 0.6;
    }
    else {
      k = 0.4;
    }
    k = pow_float(k,s);
    if (rob->approach_mode != 0) {
      k = 1.0;
    }
    v.x = v.x * k;
    v.z = v.z * k;
    rob->slide_velocity.x = rob->slide_velocity.x * k;
    rob->slide_velocity.z = rob->slide_velocity.z * k;
    if (((rob->flags & 0x100) != 0) &&
        (rob->frame < rob->field_0x2ee)) {
      vel.x = vel.x + rob->push_velocity.x;
      vel.z = vel.z + rob->push_velocity.z;
    }
    if (rob->approach_mode == 3) {
      vel.x = 0.0;
      vel.y = 0.0;
      vel.z = 0.0;
      if (rob->frame > rob->approach_frame) {
        rob->approach_mode = 0;
      }
      else {
        vel.y = rob->approach_target.y;
      }
    }
    else {
      c = rob->approach_mode;
      if (c != 0) {
      vel.x = 0.0;
      vel.z = 0.0;
      if (rob->frame > rob->approach_frame ||
          (rob->coli->field_0x58 != 0 && rob->approach_mode == 1)) {
        rob->approach_mode = 0;
      }
      else {
        n = ((float)rob->approach_frame - rob->frame_pos) / s;
        if (n < 0) {
          n = 0;
        }
        n = n + 1;
        vec3_sub((float *)&out,(float *)&rob->approach_target,(float *)&rob->pos);
        vel.x = out.x / (float)n + vel.x;
        vel.z = out.z / (float)n;
      }
      }
    }
  }
  if ((rob->flags28 & 2) != 0 &&
      (((rob->old_cur->relation_flags_prev & 0x20) != 0 && (rob->relation_flags & 0x10) != 0) ||
       ((rob->old_cur->relation_flags_prev & 0x10) != 0 && (rob->relation_flags & 0x20) != 0))) {
    v.x = 0.0;
    v.z = 0.0;
  }
  rob->velocity = v;
  rob->throw_offset.x = 0.0;
  rob->throw_offset.y = 0.0;
  rob->throw_offset.z = 0.0;
  vec3_scale((float *)&vel,(float *)&vel,s);
  vec3_add_to((float *)&rob->place_pos,(float *)&vel);
}

void rob_turn_update(rob_rec *rob)
{
  uint f;
  uint v;
  rob_rec *opp;

  f = rob->state;
  if (rob->flags & 0x100) return;
  if (f & 0x800) return;
  if ((f & 0x40400) == 0x400) return;
  if (rob->turn_flags & 1) return;
  if (mode_get_main() == MODE_MAIN_GAME) {
    if (mode_get_sub() == 7) return;
    if (mode_get_sub() == 0xf) return;
  }
  if (mode_get_main() == MODE_MAIN_OVER) {
    if (mode_get_sub() == 1) return;
    if (mode_get_sub() == 3) return;
  }
  if (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) return;
  if (rob->flags24 & 0x4000000) return;
  if (rob->flags & 0x100) return;
  if ((*(uint *)&rob->field_0x30 & 0x2000008) != 0)
    rob->turn_target = rob->opp_dir;
  else if ((opp = rob->opp, opp->field_0x104) > 0)
    rob->turn_target = rob->opp_dir;
  else if ((rob->flags & 0x20000) == 0) {
    if ((rob->move_flags & 0x4000) == 0 ||
        (v = rob->opp->state,
         (rob->state & 0x4000000) != 0))
      rob->turn_target = rob->opp_dir;
    else if ((rob->flags & 0x20000) == 0 &&
             ((rob->side_flags & 0x20) != 0 ||
              rob->frame >= rob->active_end ||
              (v & 0x600) != 0 || (v & 0x8001) == 0x8001))
      rob->turn_target = rob->opp_dir;
  }
  if (rob->turn_flags & 2) { rob_turn_step(rob,4); return; }
  if (rob->turn_flags & 4) { rob_turn_step(rob,6); return; }
  if (rob->turn_flags & 8) { rob_turn_step(rob,7); return; }
  if (rob->flags & 0x100) { rob_turn_step(rob,4); return; }
  v = rob->opp->state;
  if (f & 0x300480) return;
  if ((f & 0x21000200) && rob->frame < rob->active_end) return;
  if ((v & 0x4000000) && !(rob->field_0x30 & 8)) return;
  if (f & 0x21000200) { rob_turn_step(rob,5); return; }
  if (rob->move_flags & 0x50000) { rob_turn_step(rob,4); return; }
  if (rob->guard_state != 0) { rob_turn_step(rob,3); return; }
  if ((rob->move_flags & 0x4000) && rob->frame >= rob->active_end) {
    rob_turn_step(rob,8); return;
  }
  if (f & 1) {
    if (f & 2) { rob_turn_step(rob,2); return; }
    if (!(f & 0x4000000)) {
      if (!(f & 4)) return;
      rob_turn_step(rob,7); return;
    }
  }
  else if (!(rob->turn_flags & 2) && (f & 0x1000)) return;
  rob_turn_step(rob,0);
}

void rob_turn_step(rob_rec *rob, int kind_arg)
{
  int kind = (char)kind_arg;
  int d = (short)(rob->turn_target - rob->facing);
  int f;
  int lo;
  int g;
  int a;
  int s;
  int r;
  float x;
  float z;
  vec3 v;
  if ((ushort)(d + 0x5b) > 0xb6) {
    if (kind != 4) {
      if (kind == 5) {
        f = d > 0x6000;
        lo = -0x6000;
      }
      else {
        f = d > 0x4800;
        lo = -0x4800;
      }
      g = lo > d;
      f |= g;
      if (f) {
        d = (short)(d + -0x8000);
        d = (short)d;
      }
    }
    a = (short)((float)rob_turn_rates[kind] * rob->frame_speed);
    if (d <= a) {
      a = -a;
      f = d;
      if (d < a) {
        f = a;
      }
      r = (short)f;
    }
    else {
      r = a;
    }
    d = r;
    if (((rob->flags & 8) == 0) || ((rob->air_flags & 1) == 0)) {
      v = rob->place_pos;
      v.x = v.x - rob->pos.x;
      v.z = v.z - rob->pos.z;
      vec3_rot_y_s16(d,&v.x,&v.x);
      x = v.x + rob->pos.x;
      v.x = x;
      z = v.z + rob->pos.z;
      v.z = z;
      rob->place_pos.x = x;
      rob->place_pos.z = z;
    }
    rob->heading = d + rob->heading;
  }
}

void move_timed_done_clear(rob_rec *rob)
{
  int i;
  uint side = rob->side; char *p; char *q; i = 0; p = move_timed_done[side]; for (; i < 0x40; i++) { q = p + i; *q = 0; }
}

int move_cmd_arg_find(int move, int list_arg, int command)
{
  char list = (char)list_arg;
  int rec;
  ushort id = (ushort)move & 0x7fff;
  int *entry;

  if (id == 0) return 0;
  rec = (int)move_record((short)(id - 1));
  if (rec == 0) return 0;
  if (list == 1) entry = *(int **)(rec + 0xc);
  else entry = *(int **)(rec + 8);
  if (entry == 0) return 0;
  for (; *entry != 0; entry += 2) {
    if (entry[1] == command) return *entry;
  }
  return 0;
}

void rob_move_set(rob_rec *rob, uint move)
{
  rob_move_change(rob,move & 0xffff,0);
  return;
}

void rob_move_set_alt_facing(rob_rec *rob, uint move)
{
  rob_move_change(rob,move & 0xffff,1);
  return;
}

void rob_move_change(rob_rec *rob, ushort move, char alt)
{
  ushort side_flags;
  control_rec *ctrl;

  rob_move_alt = alt;
  rob->slide_velocity.y = 0.0;
  rob->event_flags = 0;
  rob->side_flags = rob->side_flags & 0xa97f;
  rob->frame = 1;
  rob->frame_pos = 1.0;
  if ((rob->state & 0x10) == 0) {
    rob->place_pos.x = rob->pos.x;
    rob->place_pos.z = rob->pos.z;
  }
  rob->heading = rob_move_facing(rob,alt);
  rob_facing_override_clear(rob);
  if (move != 0) {
    side_flags = rob->side_flags;
    move = move & 0x7fff;
    rob->move_no = move;
    if ((rob->motion_flags & 0x200) != 0) {
      rob->motion_flags = rob->motion_flags | 0x80;
    }
    else {
      rob->motion_flags = rob->motion_flags & 0xff7f;
    }
    rob->motion_flags = rob->motion_flags & 0xddff;
    move_start(rob,(short)move);
    ctrl = rob->ctrl;
    if ((rob->flags & 0x80) != 0) {
      ctrl->dir_branch_list = 0;
    }
    if (((rob->flags28 & 0x8000) == 0) &&
       (((rob->flags & 0x80) != 0 || ((rob->move_flags & 0x4000) == 0))))
    {
      ctrl->buffer_kind = 0;
    }
    else if (ctrl->buffer_kind_2 == 0) {
      ctrl->buffer_kind = 0;
    }
    else {
      ctrl->buffer_kind = ctrl->buffer_kind_2;
      ctrl->buffer_script = ctrl->buffer_script_2;
      ctrl->buffer_kind_2 = 0;
      ctrl->buffer_script_2 = 0;
    }
    if ((side_flags & 8) != 0) {
      rob->side_flags = rob->side_flags | 0x18;
    }
    else {
      rob->side_flags = rob->side_flags & 0xffe7;
    }
    rob_motion_set(rob);
  }
  else {
    rob->state = rob->state & 0xd10;
    rob->move_no = move;
  }
  rob->side_flags = rob->side_flags & 0xdefd;
  rob->flags = rob->flags & 0xefffffff;
  rob_zanzou_motion_nop(rob);
}

int rob_move_facing(rob_rec *rob, char alt)
{
  short r;
  r = rob->heading;
  if ((rob->flags24 & 0x80000) != 0 ||
      ((((int)rob->flags24 >> 0x12 ^ 1U) & 1 & (alt != '\0')) != 0 &&
       (rob->flags & 0x80) != 0)) {
    r = rob->body_yaw;
  }
  else {
    int *q = (int *)&rob->throw_request;
    if (q[1] != -1) {
      r = *(short *)(q + 1);
    }
    else if ((rob->state & 0x100) != 0) {
    if ((rob->side_flags & 0x10) != 0) {
      r -= rob->facing_offset;
    }
    else {
      r += rob->facing_offset;
    }
    }
  }
  return r;
}

int mot_frames(short motion_no)
{
  int *dir;
  int base;

  dir = (int *)mot_dir_get((int)(short)((short)motion_no + -1));
  base = mot_bank_base((int)(short)motion_no);
  return (int)*(short *)(base + *dir);
}

void move_start(rob_rec *rob, int move_arg)
{
  short move = move_arg;
  float *gravity;
  uint *rec;
  short old;
  int frames;
  uint flags;
  uint prev;
  uint move_flags;
  rob_limb_rec *limb;
  char i;
  int *cmds;
  uint v;
  int *list;
  int *found;
  int *it;

  rob->flags24 = 0;
  rob->flags28 = 0;
  rob->flags2c = 0;
  gravity = &rob->gravity_scale;
  *gravity = 1.0;
  rob->move_phase = 0;
  rob->loop_count = 1;
  rob->field_0x30 = 0;
  rob->air_flags = 0;
  rob->followup_word = 0;
  rob->jump_height = 1.0;
  rob->facing_offset = 0;
  rob->attack_limb = 0;
  rob->attack_level = 0;
  rob->attack_damage = 0;
  rob->attack_damage_base = 0;
  rob->field_0x118 = 0;
  rob->field_0x119 = 0;
  rob->face_motion = 0;
  rob->field_0x3a8 = 0;
  rob->field_0x22a = 0;
  rob->field_0x22c = 0;
  rob->throw_request = 0;
  rob->event_flags = 0;
  rob->field_0x174 = 0;
  rob->air_frames = 0xffff;
  rob->jump_target_x = 0.0;
  rob->jump_target_z = 0.0;
  rob->knockback_scale = 0.0;
  rob->field_0x36c = 0.0;
  rob->turn_flags = 0;
  rob->event_count = 0;
  rob->event_list = 0;
  rob->shake_count = 0;
  rob->shake_list = 0;
  rob->field_0x37c = 0;
  rob->catch_mask = 0;
  rob->attack_mask = 0;
  rob->field_0x338 = 0;
  rob->field_0x33c = 0;
  rob->field_0x2ec = 0;
  rob->field_0x290 = 0;
  rob->followup_move = 0;
  rob->followup_frame = 0;
  rob->attack_events = 0;
  rob->field_0x166 = 0;
  rob->followup_count = 0;
  rob->followup_flags = 0;
  rob->hands[0].flags = 0;
  rob->hands[1].flags = 0;
  rob->eye_look = 0;
  { short *s = &rob->eye_angles[0]; short *t = &rob->eye_angles[1];
  *s = 0; rob->eye_angles[2] = *t = 0; }
  rob->eye_angles[4] = rob->eye_angles[5] = 0;
  rob->cam_bits = 0;
  rob->field_0x11a = 0;
  rob->field_0x291 = 0;
  rob->field_0x29a = 0;
  rob->field_0x348 = 0.0;
  rob->grip_mode = 0;
  rob->grip_bone = 0;
  rob->grip_link = 0;
  rob->preset_throw = -1;
  rob->preset_throw_move = 0;
  rob->approach_mode = 0;
  rob->field_0x2f8 = 0;
  rob->field_0x312 = 0xffff;
  rob->field_0x314 = 0;
  rob->react_stun[0] = 0;
  rob->react_stun[1] = 0;
  rob->react_stun[2] = 0;
  rob->react_stun[3] = 0;
  rob->react_stun[4] = 0;
  rob->react_recovery[0] = 0xffff;
  rob->react_recovery[1] = 0xffff;
  rob->react_recovery[2] = 0xffff;
  rob->react_recovery[3] = 0xffff;
  rob->hit_params = 0;
  rob->field_0x364 = 1.2;
  rob->field_0x368 = 1.0;
  rob->field_0x288 = 1;
  rob->react_wait[0] = 0xffff;
  rob->react_wait[1] = 0xffff;
  rob->react_wait[2] = 0xffff;
  rob->react_wait[3] = 0xffff;
  rob->field_0x298 = 0xffff;
  rob->result_rows[0].stun = 0;
  rob->result_rows[0].recovery = 0xffff;
  rob->result_rows[0].wait = 0xffff;
  rob->result_rows[1].stun = 0;
  rob->result_rows[1].recovery = 0xffff;
  rob->result_rows[1].wait = 0xffff;
  rob->result_rows[2].stun = 0;
  rob->result_rows[2].recovery = 0xffff;
  rob->result_rows[2].wait = 0xffff;
  rob->result_rows[3].stun = 0;
  rob->result_rows[3].recovery = 0xffff;
  rob->result_rows[3].wait = 0xffff;
  rob->react_recovery[4] = 0xffff;
  rob->react_wait[4] = 0xffff;
  rob->field_0x316 = 0;
  rob->field_0x340 = 0;
  rob->field_0x370 = 0.0;
  rob->field_0x344 = 0;
  rob->flags = rob->flags & 0xfe7fffff;
  rob->coli->flags = rob->coli->flags | 0x40;
  if (move == 0) {
    return;
  }
  if (half_gravity_active() == 1) {
    *gravity = *gravity * 0.5;
  }
  rec = (uint *)move_record((short)(move - 1));
  old = rob->recovery_end;
  rob->old_cur->state = rob->state;
  rob->old_cur->move_flags = rob->move_flags;
  frames = mot_frames(move);
  rob->move_length = frames;
  rob->active_end = frames;
  rob->recovery_end = frames;
  rob->field_0x2f0 = frames;
  rob->field_0x2f2 = frames;
  rob->field_0x2ee = 1;
  rob->active_start = 1;
  rob->jump_height = 1.0;
  flags = *rec;
  prev = rob->state;
  rob->move_flags = flags;
  rob->state = flags & 0xffffbfff;
  if ((flags & 0x100) != 0) {
    rob->facing_offset = -0x8000;
  }
  if ((flags & 0x400000) != 0) {
    rob->field_0x2f8 = -0x8000;
  }
  if (!((flags & 0x4000) && (prev & 1) && (rob->ctrl->flags & 8) &&
        rob->old_cur->frame < old)) {
    rob->flags = rob->flags & 0xfffdffff;
    if ((flags & 0x4000000) == 0) {
      rob->opp->flags = rob->opp->flags & 0xfffdffff;
    }
  }
  if ((flags & 0x800) != 0) {
    rob->face_flags = rob->face_flags & 8;
  }
  else {
    rob->face_flags = 0;
  }
  rob->field_0x284 = *(char *)((int)rec + 4);
  rob->move_stance = *(char *)((int)rec + 5);
  if ((rob->move_stance & 0x40) != 0) {
    rob->move_stance = rob->move_stance & 0xbf;
    if ((rob->side_flags & 8) != 0) {
      rob->motion_flags = rob->motion_flags & 0xffef;
    }
    else {
      rob->motion_flags = rob->motion_flags | 0x10;
    }
  }
  else {
    if ((rob->side_flags & 8) != 0) {
      rob->motion_flags = rob->motion_flags | 0x10;
    }
    else {
      rob->motion_flags = rob->motion_flags & 0xffef;
    }
  }
  move_flags = rob->move_flags;
  if ((move_flags & 0x8000) != 0) {
    rob_hands_shape_set(rob,2.0);
  }
  if ((move_flags & 0x4000) != 0) {
    rob->reach.flags = rob->reach.flags | 2;
  }
  else {
    rob->reach.flags = rob->reach.flags & 0xfd;
  }
  limb = rob->reach.limbs;
  for (i = 0; i < 4; i++, limb = limb + 1) {
    limb->bone = 0;
  }
  if ((rob->motion_flags & 0x40) != 0) {
    rob->motion_flags = rob->motion_flags | 0x20;
  }
  else {
    rob->motion_flags = rob->motion_flags & 0xffdf;
  }
  if ((rob->motion_flags & 0x1000) != 0) {
    rob->motion_flags = rob->motion_flags | 0x800;
  }
  else {
    rob->motion_flags = rob->motion_flags & 0xf7ff;
  }
  rob->motion_flags = rob->motion_flags & 0x8b2;
  cmds = (int *)rec[2];
  v = rec[3];
  list = (int *)rec[4];
  rob->timed_list = (uchar *)v;
  rob->field_0x38c = (uchar *)v;
  rob->field_0x390 = (uchar *)v;
  rob->branch_list = (uchar *)list;
  rob->ctrl->branch_pending = 0;
  rob->ctrl->branch_wait = 0;
  move_timed_done_clear(rob);
  found = (int *)0x0;
  it = list;
  if (list != (int *)0x0) {
    for (; *it != 0; it = it + 2) {
      if ((*(byte *)*it & 2) != 0) {
        found = list;
        break;
      }
    }
  }
  rob->field_0x398 = (uchar *)found;
  if (cmds != (int *)0x0) {
    while (*cmds != 0) {
      ((void (*)(int,int,int *))move_commands[cmds[1]])((int)rob,*cmds,cmds);
      cmds = cmds + 2;
    }
  }
  if ((rob->state & 0x400000) != 0) {
    if ((rob->side_flags & 0x10) != 0) {
      rob->heading = rob->heading - rob->field_0x2f8;
    }
    else {
      rob->heading = rob->heading + rob->field_0x2f8;
    }
  }
  if ((rob->move_flags & 0x4000) == 0) {
    rob->field_0x110 = 0;
    rob->field_0x114 = 0;
  }
  if (msnm_rule_get() == 9) {
    msnm_combo_move_start(rob);
  }
  return;
}

void move_cmd_00(rob_rec *rob, uint *arg)
{
  rob->followup_word = *arg;
  rob->field_0x2de = *(short *)(arg + 1);
  rob->field_0x2e0 = *(short *)((int)arg + 6);
  rob->field_0x2e2 = *(short *)(arg + 2);
  return;
}

void move_cmd_01_frames(rob_rec *rob, short *arg)
{
  short recovery;

  recovery = arg[2];
  rob->active_start = *arg++;
  if (*arg != -1) {
    rob->active_end = *arg;
  }
  if (((user_back_data.option_bits & 0x08000000) != 0) &&
     (((int)rob->move_flags >> 0xe & 1U & (uint)(recovery != -1)) != 0)) {
    short m = rob->move_length;
    recovery = mode_recovery_extra + recovery;
    if (recovery > m) {
      recovery = m;
    }
  }
  if (recovery != -1) {
    rob->recovery_end = recovery;
  }
  rob->state = rob->state | 2;
  rob->move_phase = 0;
}

void move_cmd_02(rob_rec *rob, uchar *arg)
{
  rob->air_flags = *arg;
  rob->field_0x2ee = *(short *)(arg + 2);
  rob->field_0x2f0 = *(short *)(arg + 4);
  rob->field_0x2f2 = *(short *)(arg + 6);
  rob->jump_height = *(float *)(arg + 0xc);
  rob->knockback_scale = *(float *)(arg + 0x14);
  if (half_gravity_active() == 1) {
    rob->gravity_scale += *(float *)(arg + 0x10) * 0.5;
  }
  else {
    rob->gravity_scale += *(float *)(arg + 0x10);
  }
  rob->slide_velocity.y = -(*(float *)(arg + 0x10) * world_gravity);
  if (rob->field_0x2f2 < rob->field_0x2f0) {
    rob->field_0x2f2 = rob->field_0x2f0;
  }
}

void move_cmd_03_attack(rob_rec *rob, uchar *arg)
{
  short *angle;
  char *limb;

  rob->field_0x30 = *arg;
  limb = &rob->attack_limb;
  *limb = arg[1];
  rob->attack_level = arg[2];
  rob->attack_cmd_flags = *(short *)(arg + 10);
  angle = &rob->attack_angle;
  *angle = *(short *)(arg + 0xc);
  rob->field_0x328 = *(uint *)(arg + 0x10);
  rob->attack_flags2 = *(uint *)(arg + 0x14);
  rob->hit_param_row = arg[3];
  rob->hit_sound = arg[4];
  rob->hit_sound_2 = arg[5];
  rob->hit_sound_3 = arg[6];
  rob->attack_damage_base = *(short *)(arg + 8);
  rob->attack_damage = damage_scale((int)rob->attack_rate,(int)*(short *)(arg + 8));
  rob->field_0x110 = rob->field_0x10c;
  rob->field_0x114 = rob->field_0x115;
  rob->field_0x115 = 0;
  rob->field_0x10c = 0;
  if ((rob->side_flags & 8) != 0) {
    *limb = attack_limb_mirror[*limb];
    *angle = -*angle;
  }
  return;
}

void move_cmd_04_frame_table(rob_rec *rob, short *arg)
{
  short *guard_recovery;
  short *recovery;
  short *length;

  rob->react_stun[0] = *arg;
  rob->react_stun[1] = arg[1];
  rob->react_stun[2] = arg[2];
  rob->react_stun[3] = arg[0xc];
  guard_recovery = &rob->react_recovery[0];
  *guard_recovery = arg[8];
  rob->react_recovery[1] = arg[9];
  rob->react_recovery[2] = arg[10];
  rob->react_recovery[3] = arg[0xb];
  rob->field_0x364 = *(float *)(arg + 4);
  rob->field_0x368 = *(float *)(arg + 6);
  rob->react_wait[0] = arg[0xd];
  rob->react_wait[1] = arg[0xe];
  rob->react_wait[2] = arg[0xf];
  rob->react_wait[3] = arg[0x10];
  rob->result_rows[3].stun = arg[1];
  rob->result_rows[3].recovery = arg[9];
  rob->result_rows[3].wait = arg[0xe];
  rob->react_stun[4] = arg[0x11];
  rob->react_recovery[4] = 0xffff;
  rob->react_wait[4] = 0xffff;
  if (*guard_recovery > rob->move_length) {
    *guard_recovery = rob->move_length;
  }
  recovery = &rob->react_recovery[1];
  length = &rob->move_length;
  if (*recovery > *length) {
    rob->result_rows[3].recovery = *recovery = *length;
  }
  recovery = &rob->react_recovery[2];
  length = &rob->move_length;
  if (*recovery > *length) {
    rob->result_rows[2].recovery = rob->result_rows[1].recovery = rob->result_rows[0].recovery = *recovery = *length;
  }
  return;
}

void move_cmd_05(rob_rec *rob, short *arg)
{
  if (0 < arg[0]) {
    rob->result_rows[0].stun = arg[0];
  }
  if (0 < arg[3]) {
    rob->result_rows[1].stun = arg[3];
  }
  if (0 < arg[6]) {
    rob->result_rows[2].stun = arg[6];
  }
  if (0 < arg[9]) {
    rob->result_rows[3].stun = arg[9];
  }
  if (arg[1] != -1) {
    rob->result_rows[0].recovery = arg[1];
  }
  if (rob->result_rows[0].recovery > rob->move_length) {
    rob->result_rows[0].recovery = rob->move_length;
  }
  if (arg[4] != -1) {
    rob->result_rows[1].recovery = arg[4];
  }
  if (rob->result_rows[1].recovery > rob->move_length) {
    rob->result_rows[1].recovery = rob->move_length;
  }
  if (arg[7] != -1) {
    rob->result_rows[2].recovery = arg[7];
  }
  if (rob->result_rows[2].recovery > rob->move_length) {
    rob->result_rows[2].recovery = rob->move_length;
  }
  if (arg[10] != -1) {
    rob->result_rows[3].recovery = arg[10];
  }
  if (rob->result_rows[3].recovery > rob->move_length) {
    rob->result_rows[3].recovery = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->result_rows[0].wait = arg[2];
  }
  if (arg[5] != -1) {
    rob->result_rows[1].wait = arg[5];
  }
  if (arg[8] != -1) {
    rob->result_rows[2].wait = arg[8];
  }
  if (arg[11] != -1) {
    rob->result_rows[3].wait = arg[11];
  }
}

void move_cmd_06_hit_row(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->react_stun[1] = *arg;
  }
  if (arg[1] != -1) {
    rob->react_recovery[1] = arg[1];
  }
  if (rob->react_recovery[1] > rob->move_length) {
    rob->react_recovery[1] = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->react_wait[1] = arg[2];
  }
  return;
}

void move_cmd_07_guard_row(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->react_stun[0] = *arg;
  }
  if (arg[1] != -1) {
    rob->react_recovery[0] = arg[1];
  }
  if (rob->react_recovery[0] > rob->move_length) {
    rob->react_recovery[0] = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->react_wait[0] = arg[2];
  }
  return;
}

void move_cmd_08_counter_row(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->react_stun[2] = *arg;
  }
  if (arg[1] != -1) {
    rob->react_recovery[2] = arg[1];
  }
  if (rob->react_recovery[2] > rob->move_length) {
    rob->react_recovery[2] = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->react_wait[2] = arg[2];
  }
  return;
}

void move_cmd_09_row4000(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->result_rows[0].stun = *arg;
  }
  if (arg[1] != -1) {
    rob->result_rows[0].recovery = arg[1];
  }
  if (rob->result_rows[0].recovery > rob->move_length) {
    rob->result_rows[0].recovery = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->result_rows[0].wait = arg[2];
  }
  return;
}

void move_cmd_10_row8000(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->result_rows[1].stun = *arg;
  }
  if (arg[1] != -1) {
    rob->result_rows[1].recovery = arg[1];
  }
  if (rob->result_rows[1].recovery > rob->move_length) {
    rob->result_rows[1].recovery = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->result_rows[1].wait = arg[2];
  }
  return;
}

void move_cmd_11_row10000(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->result_rows[2].stun = *arg;
  }
  if (arg[1] != -1) {
    rob->result_rows[2].recovery = arg[1];
  }
  if (rob->result_rows[2].recovery > rob->move_length) {
    rob->result_rows[2].recovery = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->result_rows[2].wait = arg[2];
  }
  return;
}

void move_cmd_12_row800(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->result_rows[3].stun = *arg;
  }
  if (arg[1] != -1) {
    rob->result_rows[3].recovery = arg[1];
  }
  if (rob->result_rows[3].recovery > rob->move_length) {
    rob->result_rows[3].recovery = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->result_rows[3].wait = arg[2];
  }
  return;
}

void move_cmd_13_row5(rob_rec *rob, short *arg)
{
  if (0 < *arg) {
    rob->react_stun[4] = *arg;
  }
  if (arg[1] != -1) {
    rob->react_recovery[4] = arg[1];
  }
  if (rob->react_recovery[4] > rob->move_length) {
    rob->react_recovery[4] = rob->move_length;
  }
  if (arg[2] != -1) {
    rob->react_wait[4] = arg[2];
  }
  return;
}

void move_cmd_14_nop(void)
{
  return;
}

void move_cmd_15_throw_damage(rob_rec *rob, int arg)
{

  rob->field_0x328 = 0;
  rob->attack_flags2 = 0;
  rob->attack_damage = damage_scale((int)rob->attack_rate,(int)*(short *)(arg + 2));
  rob->throw_damage += rob->attack_damage;
  rob->facing_offset = *(short *)(arg + 4);
  rob->field_0x294 = (*(short *)(arg + 6) == -1) ? rob->active_start : *(short *)(arg + 6);
  rob->field_0x296 = *(short *)(arg + 10);
  rob->field_0x298 = *(short *)(arg + 0xc);
  return;
}

void move_cmd_16(rob_rec *rob, short *arg)
{
  rob->field_0x2f2 = *arg;
  rob->facing_offset = arg[1];
  rob->field_0x294 = arg[2];
  rob->field_0x296 = arg[3];
  rob->field_0x292 = 1;
  return;
}

void move_cmd_17(rob_rec *rob, short *arg)
{
  rob->air_frames = *arg;
  rob->jump_target_x = *(float *)(arg + 2);
  rob->jump_target_z = *(float *)(arg + 4);
  rob->slide_velocity.y = -(*(float *)(arg + 6) * world_gravity);
}

void move_cmd_18_nop(void)
{
  return;
}

void move_cmd_19_list_374(rob_rec *rob, int arg, int *cmd)
{
  if (rob->event_count == '\0') {
    rob->event_list = (uchar *)cmd;
  }
  rob->event_count = rob->event_count + '\x01';
  return;
}

void move_cmd_20_damage_by_distance(rob_rec *rob, byte *arg)
{
  float d1;
  float dm;
  float d0;
  int am;
  int a0;
  int a1;
  float dist;
  float t;
  int ti;

  d1 = *(float *)(arg + 0xc);
  dm = *(float *)(arg + 8);
  d0 = *(float *)(arg + 4);
  am = arg[2];
  a0 = arg[0];
  a1 = *(byte *)&rob->attack_damage_base;
  dist = rob->opp_dist;
  if (d0 > d1) {
    t = d1; d1 = d0; d0 = t;
    ti = a1; a1 = a0; a0 = ti;
  }
  if (dist < d0) {
    rob->attack_damage = a0;
  }
  else if (dist > d1) {
    rob->attack_damage = a1;
  }
  else {
    if (am != 0) {
      if (dist < dm) {
        d1 = dm;
        a1 = am;
      }
      else {
        d0 = dm;
        a0 = am;
      }
    }
    rob->attack_damage = (float)a0 + ((float)(a1 - a0) * (dist - d0)) / (d1 - d0);
  }
  rob->attack_damage = damage_scale((int)rob->attack_rate,(int)rob->attack_damage);
}

void move_cmd_21(rob_rec *rob, short *arg)
{
  rob->field_0x118 = 1;
  rob->field_0x300[0] = *arg;
  rob->field_0x300[1] = arg[1];
  rob->field_0x300[2] = arg[2];
  rob->field_0x300[3] = arg[3];
  rob->field_0x308 = arg[5];
  rob->field_0x28f = *(uchar *)(arg + 4);
  return;
}

void move_cmd_22(rob_rec *rob, uint *arg)
{
  rob->attack_mask = *arg;
  rob->field_0x338 = arg[1];
  return;
}

void move_cmd_23(rob_rec *rob, uint *arg)
{
  rob->catch_mask = *arg;
  return;
}

void move_cmd_24(rob_rec *rob, short *arg)
{
  if ((rob->state & 0x100) != 0) {
    rob->facing_offset = *arg;
  }
  return;
}

void move_cmd_25(rob_rec *rob, uchar *arg)
{
  rob->followup_flags = *arg;
  rob->followup_move = *(short *)(arg + 2);
  rob->field_0x290 = arg[4];
  rob->followup_frame = *(short *)(arg + 6);
  return;
}

void move_cmd_26(rob_rec *rob, uint *arg)
{
  rob->field_0x33c = *arg;
  rob->field_0x30a = *(short *)(arg + 1);
  return;
}

void move_cmd_27_list_378(rob_rec *rob, int arg)
{
  if (rob->shake_count == '\0') {
    rob->shake_list = (uchar *)arg;
  }
  rob->shake_count = rob->shake_count + '\x01';
  return;
}

void move_cmd_28_list_37c(rob_rec *rob, int arg)
{
  rob->field_0x37c = (uchar *)arg;
  rob->side_flags = rob->side_flags | 0x80;
  return;
}

void move_cmd_29_list_380(rob_rec *rob, est_row *arg)
{
  rob->hit_params = arg;
  return;
}

void move_cmd_30(rob_rec *rob, uint *arg)
{
  rob->field_0x291 = 1;
  rob->field_0x29a = *(short *)(arg + 1);
  rob->field_0x348 = *(float *)arg;
  return;
}

void move_cmd_31(rob_rec *rob, short *arg)
{
  rob->field_0x2ec = *arg;
  return;
}

void move_cmd_32(rob_rec *rob, short *arg)
{
  rob->field_0x2f8 = *arg;
  return;
}

void move_cmd_33_flags24(rob_rec *rob, uint *arg)
{
  rob->flags24 = *arg;
  return;
}

void move_cmd_34_flags28(rob_rec *rob, uint *arg)
{
  rob->flags28 = *arg;
  return;
}

void move_cmd_35_flags2c(rob_rec *rob, uint *arg)
{
  rob->flags2c = *arg;
  return;
}

void move_cmd_face_motion(rob_rec *rob, short *arg, int *cmd)
{
  rob->face_motion = *arg;
  return;
}

void move_cmd_37(rob_rec *rob, short *arg)
{
  rob->field_0x30e = *arg;
  rob->field_0x310 = arg[1];
  return;
}

void move_cmd_38(rob_rec *rob)
{
  rob->hands[0].flags = rob->hands[0].flags | 1;
  rob->hands[1].flags = rob->hands[1].flags | 1;
  return;
}

void move_cmd_39(rob_rec *rob)
{
  rob->flags28 = rob->flags28 | 0x8000;
  return;
}

void move_cmd_40_velocity_length(rob_rec *rob, float *arg)
{
  float *a = arg;
  float *z;
  float *v;

  z = &rob->push_velocity.y;
  *z = 0.0;
  v = (float *)&rob->push_velocity;
  if (-1.1920929e-07 < (float)vec3_length(v)) {
    vec3_set_length(v,v,a[0]);
    *z = world_gravity * a[1];
  }
}

void move_cmd_41(rob_rec *rob, char *arg)
{
  ushort v; if (*arg == 0) v = 0x200; else v = 0x400; rob->side_flags |= v;
}

void move_cmd_42(rob_rec *rob, ushort *arg)
{
  ushort bits;

  bits = *arg;
  if ((bits & 0x2000) != 0) {
    bits = bits & 0xdfff;
    if (rob->move_no == rob->old_cur->move_no) {
      bits = bits | 0x80;
    }
  }
  rob->motion_flags = rob->motion_flags | bits;
  return;
}

void move_cmd_43(rob_rec *rob, float *arg)
{
  if (-1.1920929e-07 < *arg) {
    rob->field_0x36c = *arg;
  }
  return;
}

void move_cmd_44_list_384(rob_rec *rob, int arg, int *cmd)
{
  if (rob->attack_events == 0) {
    rob->attack_events = (uchar *)cmd;
  }
  return;
}

void move_cmd_45(rob_rec *rob, short *arg)
{
  rob->field_0x312 = *arg;
  rob->field_0x314 = arg[1];
  return;
}

void move_cmd_46_nop(void)
{
  return;
}

void move_cmd_47(rob_rec *rob, short *arg)
{
  rob->field_0x3aa = *arg;
  return;
}

void move_cmd_48(rob_rec *rob, uchar *arg)
{
  rob->field_0x178 = *arg;
  return;
}

void move_cmd_49_nop(void)
{
  return;
}

void move_cmd_50(rob_rec *rob, byte *arg)
{
  if (*arg < 0x13) {
    rob->field_0x344 |= 1 << (char)*arg;
  }
}

void move_cmd_51_nop(void)
{
  return;
}

void rob_move_step(rob_rec *rob)
{
  uint m;
  uint n;
  short s;
  uint *rec;
  ushort *pv;
  short v;
  int x;
  short frame;
  uint f;
  byte st;
  short t;

  if (rob->frame > rob->move_length) {
    m = rob->followup_word;
    switch (m) {
    case 0:
      rob->state &= 0xffffff7e;
      return;
    case 0xffffffff:
      if (rob->field_0x2de != 0 && rob->field_0x2e0 != 0) {
        if ((uint)rob->loop_count >= rob->field_0x2de) {
          s = rob->field_0x2e0;
          rec = (uint *)move_record((short)(s - 1));
          if (*rec & 0x100000) {
            rob->request_pending.word = s + 0x5000000;
          }
          else if (*rec & 0x200000) {
            rob->request_pending.word = s + 0x6000000;
            rob->flags |= 0x80;
          }
          else {
            rob_move_set(rob,rob->field_0x2e0);
          }
          break;
        }
        if (rob->state & 0x300000) {
          rob->throw_damage = rob->attack_damage;
          rob->action_step = 1;
        }
      }
      rob->frame = 1;
      rob->frame_pos = 1.0;
      rob->timed_list = rob->field_0x38c;
      rob->state = rob->move_flags & 0xffffbfff;
      rob->motion_flags |= 0x180;
      rob->loop_count += 1;
      move_timed_done_clear(rob);
      break;
    case 0xfffffffd:
      rob->place_pos.y -= 0.6;
      if (rob->side != round_winner) v = round_end_pose_moves[rob->chara];
      else v = *rob->move_table;
      rob_move_set(rob,(ushort)v);
      if ((game_flags & 0x2000) == 0) rob->root_offset_y = 0.6;
      break;
    default:
      if (m & 0x20000000) {
        rob->field_0x178 += *(byte *)&rob->field_0x2e2;
        if ((char)rob->field_0x178 >= rob->field_0x2de) m = (m & 0xffff0000) + rob->field_0x2e0;
      }
      n = m & 0x7fff;
      rec = (uint *)move_record((short)(n - 1));
      if (*rec & 0x100000) {
        if (rob->opp->flags & 0x100) goto prev_move;
        rob->request_pending.word = n + 0x5000000;
        if (m & 0x8000) rob->request_pending.word = (n + 0x5000000) | 0x8000;
      }
      else if (*rec & 0x200000) {
        if (rob->opp->flags & 0x100) {
    case 0xfffffffe:
    prev_move:
          if (rob->flags & 0x10) pv = rob->move_table + 1;
          else pv = rob->move_table;
          rob_move_set(rob,*pv);
          break;
        }
        rob->request_pending.word = n + 0x6000000;
        rob->flags |= 0x80;
        if (m & 0x8000) rob->request_pending.word = (n + 0x6000000) | 0x8000;
      }
      else {
        rob_move_set(rob,m & 0x7fff);
      }
      rob->loop_count += 1;
      break;
    }
  }
  frame = rob->frame;
  f = rob->state;
  st = rob->move_phase;
  switch (st) {
  case 0:
    if (frame < rob->active_start) break;
    if (rob->move_flags & 0x4000) {
      f |= 0x4000;
      if (rob->move_flags & 0x40000) f &= 0xfffbffff;
    }
    f &= ~2;
    rob->move_phase = ++st;
  case 1:
    if (frame < rob->active_end) break;
    rob->flags2c &= ~8;
    if (f & 0x4000) {
      f &= 0xffffbfff;
      x = rob->field_0x30 & 0xfd;
      if (x & 4) f &= 0xffff7fff;
      rob->field_0x30 = x;
    }
    else if ((f & 0x4000001) == 0x4000001 && (rob->flags28 & 0x18) == 0) {
      f &= ~1;
    }
    if ((rob->move_flags & 0x50000) == 0x50000) f &= 0xfffbffff;
    f |= 4;
    rob->move_phase = ++st;
    rob->field_0x1ec = 0;
  case 2:
    t = rob->recovery_end;
    if (t == -1) break;
    if (t > frame) break;
    if (rob->recovery_end != rob->move_length || rob->followup_word == 0 || rob->followup_word == -2) {
      f &= 0xfbfbff7e;
    }
    if ((rob->state & 0x40000) && (uchar)rob->action.part.type == 9 && (uchar)rob->action_step < 2) f |= 0x40000;
    rob->field_0x220 = 0;
    rob->move_phase = st + 1;
    break;
  case 3:
  default:
    break;
  }
  rob->state = f;
  if (rob->timed_list != 0) move_timed_run(rob);
  rob_root_offset_apply(rob);
}

void rob_root_offset_apply(rob_rec *rob)
{
  uint f;
  float *d;
  float *mtx;
  float x;
  float z;
  float v[3];

  f = rob->state;
  d = (float *)&rob->root_offset;
  if ((f & 8) == 0) {
    f |= 8;
    mtx = (float *)rob_base_mtx_get(rob->side);
    d[0] = v[0] = mtx[12];
    d[2] = v[2] = mtx[14];
    d[1] = v[1] = 0.0;
    mtx_push_identity();
    mtx_translate_by_vec3((float *)&rob->pos);
    mtx_rot_y_s16(0x8000 - rob->heading);
    mtx_xform_point(v,v);
    mtx_pop(1);
    x = v[0];
    rob->pos.x = x;
    rob->place_pos.x = x;
    z = v[2];
    rob->pos.z = z;
    rob->place_pos.z = z;
    rob->old_cur->place_pos.x = x;
    rob->old_cur->place_pos.z = z;
  }
  else {
    d[0] = 0.0;
    d[1] = 0.0;
    d[2] = 0.0;
  }
  rob->state = f;
}

void move_timed_run(rob_rec *rob)
{
  short *arg;
  char *done;
  uint *entry;

  entry = (uint *)rob->timed_list;
  arg = (short *)*entry;
  rob->field_0x390 = (uchar *)entry;
  done = move_timed_done[(uint)rob->side];
  while (arg != (short *)0x0) {
    short f = rob->frame;
    if ((*done == '\0') && (*arg <= f)) {
      ((void (*)(rob_rec *, short *))move_timed_commands[entry[1]])(rob, arg);
      *done = '\x01';
    }
    entry = entry + 2;
    arg = (short *)*entry;
    done = done + 1;
  }
}

void move_tcmd_00_random_follow(rob_rec *rob, int arg)
{
  char every;
  char rest;
  char chance;
  uint move;
  char roll;
  char *q;

  q = (char *)rob->field_0x2fc;
  every = *(char *)(arg + 2);
  rest = *(uint *)(q + 0x20) % every;
  chance = *(char *)(arg + 3);
  move = *(uint *)(arg + 4);
  roll = (int)lcg_next15() % 100;
  if ((roll <= chance) & (rest == 0)) {
    *(uint *)(q + 0x24) = move | 0x80000000;
  }
}

void move_tcmd_01_sound(rob_rec *rob, int arg)
{
  short s;
  s = *(short *)(arg + 2);
  if ((game_flags & 0x2000) == 0) {
    if (s == 900) {
      s = *(short *)(stage_sound_record_get() + 6);
    }
    sound_request(rob,s);
  }
}

void move_tcmd_02_voice(rob_rec *rob, int arg)
{
  if ((game_flags & 0x2000) == 0) {
    rob_voice_request(rob,(int)*(short *)(arg + 2));
    rob->face_flags = rob->face_flags | 2;
  }
  return;
}

void move_tcmd_03(rob_rec *rob, int arg)
{
  rob->state = rob->state | 0x100;
  rob->facing_offset = rob->facing_offset + *(short *)(arg + 2);
  return;
}

void move_tcmd_04_set_flag20(rob_rec *rob, int arg)
{
  rob->state |= 1 << *(ushort *)(arg + 2);
}

void move_tcmd_05_clear_flag20(rob_rec *rob, int arg)
{
  rob->state &= ~(1 << *(ushort *)(arg + 2));
}

void move_tcmd_06_toggle_side(rob_rec *rob)
{
  rob->side_flags = rob->side_flags ^ 8;
  return;
}

void move_tcmd_07(rob_rec *rob)
{
  rob->field_0x166 = 0;
  ((struct { unsigned int : 10; unsigned int a : 1; unsigned int b : 1; } *)&rob->state)->a = 1;
  ((struct { unsigned int : 10; unsigned int a : 1; unsigned int b : 1; } *)&rob->state)->b = 1;
}

void move_tcmd_08_launch(rob_rec *rob, int arg)
{
  short a;
  float *f;
  vec3 v;
  vec3 *d;

  a = -*(short *)(arg + 2);
  if ((rob->side_flags & 0x10) != 0) {
    a = -a;
  }
  a = rob->heading + a;
  f = (float *)(arg + 4);
  v.x = sin_s16(a) * *f;
  v.y = *(float *)(arg + 8) * world_gravity;
  v.z = -*f * cos_s16(a);
  d = &rob->velocity;
  *d = v;
  if (half_gravity_active() == 1) {
    vec3_scale((float *)d,(float *)d,half_gravity_launch_scale);
  }
  rob_airborne_begin(rob);
}

void move_tcmd_09_sound_flag(rob_rec *rob, int arg)
{
  int sound;

  if ((game_flags & 0x2000) == 0) {
    sound = (int)*(short *)(arg + 2);
    sound_flag_noop(1);
    if (sound == 0x167) {
      sound = 0x332;
    }
    sound_request(rob,sound);
  }
  camera_shake_request(1,1);
  return;
}

void move_tcmd_10_state33(rob_rec *rob, int arg)
{
  switch(*(short *)(arg + 2)) {
  case 0:
    rob->turn_flags = 1;
    break;
  case 1:
    rob->turn_flags = rob->turn_flags & 0xfe;
    break;
  case 2:
    rob->turn_flags = rob->turn_flags | 2;
    break;
  case 3:
    rob->turn_flags = rob->turn_flags | 4;
    break;
  case 5:
    rob->turn_flags = rob->turn_flags | 8;
    break;
  case 4:
    rob->turn_flags = 0;
    break;
  }
}

void move_tcmd_11_nop(void)
{
  return;
}

void move_tcmd_12(rob_rec *rob, short *arg)
{
  if (((((rob->move_flags & 0x4000) == 0) ||
       ((rob->coli->field_0x3c[0] & 1) == 0)) ||
      (rob->attack_level == '\f')) || (rob->attack_level == '\x0e')) {
    rob->stun = arg[1];
    rob->field_0x166 = 0;
    if ((rob->move_flags & 0x200000) == 0) {
      rob->flags = rob->flags | 0x200000;
    }
  }
  return;
}

void move_tcmd_13_hand_shape(rob_rec *rob, short *arg)
{
  float v;
  short d;
  short fl;
  rob_hand_rec *a;
  rob_hand_rec *b;
  char *q;
  char *r;
  char t;

  v = *(float *)(arg + 2);
  d = arg[4] - arg[0];
  fl = arg[1];
  if (v > 2.0) v = 2.0;
  else if (v < 0.0) v = 0.0;
  if ((rob->side_flags & 0x10) != 0) {
    a = &rob->hands[1];
    b = &rob->hands[0];
  }
  else {
    a = &rob->hands[0];
    b = &rob->hands[1];
  }
  if ((fl & 2) != 0) {
    b->target = v;
    b->frames = d;
    q = (char *)&b->flags;
    t = *q & ~1;
    *q = t;
    if ((fl & 8) != 0) *q = t | 2;
  }
  if ((fl & 1) != 0) {
    a->target = v;
    a->frames = d;
    r = (char *)&a->flags;
    t = *r & ~1;
    *r = t;
    if ((fl & 8) != 0) *r = t | 2;
  }
}

void move_tcmd_14_rigid_face(rob_rec *rob, int arg)
{
  switch (*(short *)(arg + 2)) {
  case 1:
    rob->face_flags |= 2;
    break;
  case 2:
    rob->face_flags |= 8;
    break;
  default:
    rob->face_flags = 0;
    break;
  }
}

void move_tcmd_15_damage_tick(rob_rec *rob, int arg)
{
  int check;
  float *params;
  short *damage;
  float f;
  int d;
  short s;
  int o = 0x584;

  damage = &rob->pending_damage;
  *damage = damage_scale((int)(*(rob_old_rec **)((char *)rob->opp + o))->attack_rate,
                       (int)*(short *)(arg + 2));
  check = msnm_rule_is(4);
  if ((check != 0) && ((rob->state & 0x200000) != 0)) {
    f = (float)*damage;
    params = (float *)msnm_rule_params_get();
    *damage = (short)(int)(f * params[6]);
  }
  check = msnm_rule10_cpu_check((int)(char)rob->side);
  if ((check != 0) && ((rob->state & 0x200000) != 0)) {
    rob->pending_damage = 0;
  }
  s = rob->field_0x166;
  d = rob->pending_damage;
  rob->field_0x166 = s + d;
  rob->combo[0].damage = rob->combo[0].damage + d;
  rob->combo[0].hits = rob->combo[0].hits + 1;
  rob_damage_apply(rob,0);
  return;
}

void move_tcmd_16(rob_rec *rob, int arg)
{
  rob->cam_bits = *(uchar *)(arg + 2);
  return;
}

void move_tcmd_17_camera(rob_rec *rob, int arg)
{
  rob_rec *opp = rob->opp;
  int q = arg;
  uchar n = 0;

  if (mode_get_main() != MODE_MAIN_GAME) return;
  if (camera.mode != MODE_ROB) return;
  if (((rob->flags | rob->opp->old_cur->flags) &
      0x10000000) != 0) return;
  sic_rob_id = rob->side;
  cam_anim_kind = *(uchar *)(q + 4);
  if (cam_anim_kind == 1 && stage_cur_no == 0x10) {
    extra_light_spot_dist = 3.0;
  }
  cam_anim_index = *(short *)(q + 2) * 2;
  cam_anim_param = (float)*(short *)(q + 6);
  if ((rob->side_flags & 0x10) != 0) n++;
  if ((rob->state & 0x400000) != 0 &&
      (opp->old_cur->state & 0x400000) != 0) n++;
  if (rob->screen_xy[0] > opp->screen_xy[0]) {
    if ((n & 1) != 0) cam_anim_index++;
  }
  else {
    if ((n & 1) == 0) cam_anim_index++;
  }
  cam_anim_time = (rob->frame_pos - 1.0) * 0.016666668;
  cam_anim_flags = cam_anim_flags | 0x81;
  camera.mode = SOFTIMAGE;
}

void move_tcmd_18(rob_rec *rob, int arg)
{
  switch (*(char *)(arg + 2)) {
  case 0:
    rob->grip_mode = 1;
    rob->grip_bone = *(char *)(arg + 3);
    rob->grip_link = rope_link_at_distance(rob,*(float *)(arg + 4));
    break;
  case 1:
    rob->grip_mode = 0;
    rob->grip_bone = 0;
    rob->grip_link = 0;
    break;
  case 2:
    rob->grip_mode = 2;
    rob->grip_bone = *(char *)(arg + 3);
    rob->grip_link = rope_grip_link_index(rob,*(char *)(arg + 3));
    break;
  }
}

void move_tcmd_19_adjust_177(rob_rec *rob, int arg)
{
  if ((0 < round_time) && ((game_flags & 0x2000) == 0)) {
    rob_rate_adjust((int)(char)rob->side,(int)*(char *)(arg + 2));
  }
  return;
}

void move_tcmd_20_adjust_177_if(rob_rec *rob, int arg)
{
  uint bit;
  if (round_time < 1) {
    return;
  }
  if ((game_flags & 0x2000) != 0) {
    return;
  }
  switch (*(char *)(arg + 2)) {
  case 0:
    goto call;
  case 1:
    bit = rob->field_0x10c & 1;
    break;
  case 2:
    bit = rob->field_0x10c & 0x20;
    break;
  default:
    goto call;
  }
  if (bit == 0) {
    return;
  }
call:
  rob_rate_adjust((int)(char)rob->side,(char)-*(char *)(arg + 3));
}

void move_tcmd_21_adjust_177_opponent(rob_rec *rob, int arg)
{
  uint bit;

  if (round_time < 1) {
    return;
  }
  if ((game_flags & 0x2000) != 0) {
    return;
  }
  switch (*(char *)(arg + 2)) {
  case 1:
    bit = rob->field_0x10c & 1;
    break;
  case 2:
    bit = rob->field_0x10c & 0x20;
    break;
  case 0:
  default:
    goto LAB_0c0681ca;
  }
  if (bit == 0) {
    return;
  }
LAB_0c0681ca:
  rob_rate_adjust((int)(char)rob->opp->side,(char)-*(char *)(arg + 3));
}

void move_tcmd_22_approach(rob_rec *rob, int arg)
{
  ushort flags;
  short ang;
  short s;
  int n;
  float lim;
  float d;
  float fmax;
  float x;
  float z;
  float *pos;
  float *pos2;
  float *vel;
  float b[3];
  float a[3];

  flags = *(short *)(arg + 4);
  if ((flags & 2) != 0) {
    ang = rob->facing;
    if ((rob->wall_bits & 1) == 0) {
      return;
    }
    lim = rob->wall_dist[0] - *(float *)(arg + 0x18);
    if (*(float *)(arg + 8) > lim) {
      lim = *(float *)(arg + 8);
    }
    else if (lim > *(float *)(arg + 0xc)) {
      lim = *(float *)(arg + 0xc);
    }
    vec3_move_on_heading(ang,(float *)&rob->pos,(float *)&rob->approach_target,lim);
    rob->approach_mode = 1;
  }
  else if ((flags & 4) != 0) {
    ang = rob->facing + 0x8000;
    if ((rob->wall_bits & 4) == 0) {
      return;
    }
    lim = rob->wall_dist[2] - *(float *)(arg + 0x18);
    ang = ang + 0x8000;
    if (*(float *)(arg + 8) > lim) {
      lim = *(float *)(arg + 8);
    }
    else if (lim > *(float *)(arg + 0xc)) {
      lim = *(float *)(arg + 0xc);
    }
    vec3_move_on_heading(ang,(float *)&rob->pos,(float *)&rob->approach_target,lim);
    rob->approach_mode = 1;
  }
  else {
    if ((flags & 1) == 0) {
      return;
    }
    fmax = *(float *)(arg + 0xc);
    d = *(float *)(arg + 0x18);
    x = *(float *)(arg + 0x10);
    z = *(float *)(arg + 0x14);
    rob->approach_mode = 2;
    lim = rob->opp_dist - d;
    if (*(float *)(arg + 8) > lim) {
      lim = *(float *)(arg + 8);
    }
    else if (rob->opp_dist > fmax) {
      if ((flags & 0x20) != 0) {
        return;
      }
      lim = lim - (rob->opp_dist - fmax);
    }
    pos = (float *)&rob->approach_target;
    *pos = x;
    rob->approach_target.z = -z;
    rob->approach_target.y = 0.0;
    rob_base_pos_get(rob,(float)*(short *)(arg + 2),a);
    rob_base_pos_get(rob,(float)rob->frame,b);
    a[0] = a[0] - b[0];
    a[2] = a[2] - b[2];
    vec3_rot_y_s16(rob->facing,a,a);
    if ((flags & 0x10) != 0) {
      vec3_rot_y_s16(rob->opp->opp_dir,pos,pos);
    }
    else {
      vec3_rot_y_s16(rob->opp->facing,(float *)&rob->approach_target,(float *)&rob->approach_target);
    }
    pos2 = (float *)&rob->approach_target;
    *pos2 = *pos2 - a[0];
    rob->approach_target.z = rob->approach_target.z - a[2];
    vec3_add_to(pos2,(float *)&rob->opp->old_cur->pos);
    vec3_sub_from(pos2,(float *)&rob->pos);
    rob->approach_target.y = 0.0;
    if (vec3_length(pos2) > lim) {
      vec3_set_length(pos2,pos2,lim);
    }
    vec3_add_to((float *)&rob->approach_target,(float *)&rob->pos);
  }
  rob->approach_frame = *(short *)(arg + 2);
  if ((flags & 8) != 0) {
    s = rob->approach_frame;
    if (rob->field_0x2f2 > s) {
      s = rob->field_0x2f2;
    }
    n = (short)(s - rob->frame);
    if (n < 1) {
      n = 1;
    }
    vel = (float *)&rob->velocity;
    *vel = (rob->approach_target.x - rob->pos.x) / (float)n;
    rob->velocity.z = (rob->approach_target.z - rob->pos.z) / (float)n;
    rob->velocity.y = world_gravity * *(float *)(arg + 0x1c);
    rob->slide_velocity.y = 0.0;
    if (half_gravity_active() == 1) {
      vec3_scale(vel,vel,0.6);
    }
    rob_airborne_begin(rob);
  }
}

void move_tcmd_23(rob_rec *rob, int arg)
{
  char *c = (char *)arg;
  int wait;
  rob_rec *opp;
  rob_old_rec *old;
  int o = 0x584;

  rob->side_flags = rob->side_flags | 0x2000;
  if (rob->opp_dist > *(float *)(c + 0xc)) {
    return;
  }
  wait = hit_wait_timer_get();
  if ((0 < wait) && ((rob->opp->flags24 & 0x8000) == 0)) {
    return;
  }
  opp = rob->opp;
  if ((opp->flags & 0x400) != 0) {
    return;
  }
  old = opp->old_cur;
  if ((((old->move_flags & 0x4000) != 0) && ((opp->flags24 & 0x8000) == 0)) &&
     ((opp->flags28 & 0x1000000) == 0)) {
    if ((old->state & 2) != 0) {
      if (old->frame >= old->active_start >> 1) {
        return;
      }
    }
    else {
      if (((*(rob_old_rec **)((char *)rob->opp + o))->state & 0x4000) != 0) {
        return;
      }
    }
  }
  if ((rob->reach_flags2 & *(uint *)(c + 8)) != 0) {
    rob->request_pending.word = *(ushort *)(c + 4) + 0x5080000;
    rob->throw_request = *(uint *)(c + 8);
    switch(*(ushort *)(c + 2)) {
    case 1:
      rob->request_pending.word |= 0x10000;
      break;
    case 2:
      rob->request_pending.word |= 0x810000;
      break;
    case 3:
      rob->request_pending.word |= 0x200000;
      break;
    case 4:
      rob->request_pending.word |= 0x400000;
      break;
    }
  }
}

void move_tcmd_24(rob_rec *rob, int arg)
{
  int c = rob->followup_count;
  int v = *(short *)(arg + 2);

  if (*(short *)(arg + 4) == 1) {
    if (c >= v) return;
  }
  else if (c <= v) {
    return;
  }
  rob->request.word = *(uint *)(arg + 8);
}

void move_tcmd_25_set_flag24(rob_rec *rob, int arg)
{
  rob->flags24 |= 1 << *(ushort *)(arg + 2);
}

void move_tcmd_26_clear_flag24(rob_rec *rob, int arg)
{
  rob->flags24 &= ~(1 << *(ushort *)(arg + 2));
}

void move_tcmd_27_set_flag28(rob_rec *rob, int arg)
{
  rob->flags28 |= 1 << *(ushort *)(arg + 2);
}

void move_tcmd_28_clear_flag28(rob_rec *rob, int arg)
{
  rob->flags28 = ~(1 << *(ushort *)(arg + 2)) & rob->flags28;
}

void move_tcmd_29_set_flag2c(rob_rec *rob, int arg)
{
  rob->flags2c = 1 << *(ushort *)(arg + 2) | rob->flags2c;
}

void move_tcmd_30_clear_flag2c(rob_rec *rob, int arg)
{
  rob->flags2c &= ~(1 << *(ushort *)(arg + 2));
}

void move_tcmd_31_opponent_108(rob_rec *rob, short *arg)
{
  int o;
  short *value;

  if (*(int *)(arg + 2) != 0) {
    o = (*arg < rob->active_start) ? 0x110 : 0x10c;
    if ((*(uint *)((char *)rob + o) & *(uint *)(arg + 2)) == 0) {
      return;
    }
  }
  value = &rob->opp->field_0x108;
  *value = arg[1];
  if (*value < 0) *value = 0;
}

void move_tcmd_32_opponent_108_add(rob_rec *rob, char *arg)
{
  short *value;
  rob_rec *opp;

  if ((*(uint *)(arg + 4) == 0) || ((rob->field_0x110 & *(uint *)(arg + 4)) != 0))
  {
    opp = rob->opp;
    value = &opp->field_0x108;
    *value = opp->old_cur->stun + *(short *)(arg + 2);
    if (*value < 0) {
      *value = 0;
    }
  }
  return;
}

void move_tcmd_33_slot_effect(rob_rec *rob, short *arg)
{
/* ORIGINAL SHAPE (corroborated 2026-11-10: script_peek_move in control.c repeats its case bodies the same way and flow2's cross-jumping merges such copies; rob_pos_reset in this file has an in-function macro, SET; prize_fly_draw in libbase/console.c needs the same repeat): the hit-light block is repeated in cases 1 to 3 (the bytes need the repeat, GCC cross-jumps the copies into one); whether the original used a macro or pasted copies is not in the bytes */
#define HIT_LIGHT(rob, arg) \
  if (arg[3] != 0) { \
    hit_light_damage = arg[3]; \
    hit_light_pos = *(hit_light_pos_rec *)(rob->mtx + arg[2] * 16 + 12); \
    hit_light_flags |= 2; \
    if (rob->side != 0) { \
      hit_light_flags |= 0x40; \
    } \
    else { \
      hit_light_flags &= ~0x40; \
    } \
  }

  switch (arg[1]) {
  case 1:
    HIT_LIGHT(rob, arg);
    break;
  case 2:
    if ((ushort)(arg[2] - 1) < 30) {
      rob_fx_slot_spawn(1, rob->bone_mtx + arg[2] * 16);
    }
    HIT_LIGHT(rob, arg);
    break;
  case 3:
    if ((ushort)(arg[2] - 1) < 30) {
      rob_fx_slot_spawn(0, rob->bone_mtx + arg[2] * 16);
    }
    HIT_LIGHT(rob, arg);
    break;
  case 4:
  default:
    break;
  }
}

void move_tcmd_34(rob_rec *rob, char *arg)
{
  short stance;

  stance = *(short *)(arg + 2);
  rob->stance = (char)stance;
  rob->reaction = *(uchar **)(*(int *)(rob->chara_data + 0x40) + stance * 4);
  rob->move_table = rob->move_tables[stance];
  return;
}

void move_tcmd_35(rob_rec *rob, int arg)
{
  if ((game_flags & 0x2000) != 0) {
    return;
  }
  switch (*(short *)(arg + 2)) {
  case 1:
    rob->flags = (rob->flags & 0xfeffffff) | 0x800000;
    break;
  case 2:
    rob->flags = (rob->flags & 0xff7fffff) | 0x1000000;
    break;
  case 0:
  default:
    rob->flags = rob->flags & 0xfe7fffff;
    break;
  }
}

void move_tcmd_36_set_flag318(rob_rec *rob, int arg)
{
  rob->side_flags =
       rob->side_flags | (ushort)(1 << *(ushort *)(arg + 2));
}

void move_tcmd_37_clear_flag318(rob_rec *rob, int arg)
{
  rob->side_flags &= ~(1 << *(ushort *)(arg + 2));
}

void move_tcmd_38(rob_rec *rob, short *arg)
{
  short s1;
  short d;
  float f;

  if ((game_flags & 0x2000) == 0) {
    switch (stage_cur_no) {
    case 0x11:
      f = -0.3;
      break;
    case 2:
      f = -0.4;
      break;
    default:
      return;
    }
    s1 = arg[1];
    d = s1 - *arg + 1;
    rob->approach_frame = s1;
    rob->approach_target.y = f / d;
    rob->approach_mode = 3;
  }
}

void move_tcmd_39_limb_target(rob_rec *rob, short *arg)
{
  float v, s, x, y, z;
  int u;
  char c;
  rob_limb_rec *l;
  v = (float)(arg[1] - arg[0]) / rob->frame_speed;
  u = *(char *)((char *)arg + 5);
  s = 1.0;
  if (v > 1.0) s = s / v;
  if ((rob->side_flags & 0x10) != 0) {
    if (u == 2) u = 3;
    else if (u == 3) u = 2;
    else if (u == 0) u = 1;
    else if (u == 1) u = 0;
  }
  l = &rob->reach.limbs[u];
  if (*(char *)(arg + 2) == 0) {
  c = *(char *)(arg + 3);
  l->weight = 0;
  l->speed = s;
  x = *(float *)(arg + 4);
  y = *(float *)(arg + 6);
  z = *(float *)(arg + 8);
  if ((rob->side_flags & 0x10) != 0) {
    switch (c) {
    case 7:
      c = 12;
      goto negz;
    case 12:
      c = 7;
    negz:
      z = z * -1.0;
      break;
    case 17:
      c = 21;
      goto negx;
    case 21:
      c = 17;
      goto negx;
    case 16:
      c = 20;
      goto negx;
    case 20:
      c = 16;
    negx:
      x = x * -1.0;
      break;
    case 6:
      c = 11;
      break;
    case 11:
      c = 6;
      break;
    case 3:
      y = y * -1.0;
      break;
    }
  }
  l->target.x = x;
  l->target.y = y;
  l->target.z = z;
  l->bone = c + 1;
  } else {
    l->speed = -s;
  }
}

void move_tcmd_40_stop(rob_rec *rob)
{
  rob->velocity.x = 0.0f;
  rob->velocity.z = 0.0f;
}

void move_tcmd_41_eye_look(rob_rec *rob, int arg)
{
  rob->eye_look = *(uchar *)(arg + 2);
  return;
}

void move_tcmd_42(rob_rec *rob)
{
  attack_result_sounds_play(rob,rob,0x400);
  return;
}

void move_tcmd_43_nop(void)
{
  return;
}

void move_tcmd_44(rob_rec *rob, char *arg)
{
  short d;

  d = rob->move_length - rob->frame;
  rob->field_0x340 = *(uint *)(arg + 4);
  rob->field_0x370 = *(float *)(arg + 8);
  rob->field_0x316 = (*(short *)(arg + 2) > d) ? d : *(short *)(arg + 2);
}

void move_tcmd_45(rob_rec *rob)
{
  rob_rec *opp;

  opp = rob->opp;
  if (((((opp->field_0x30 & 8) == 0) && ((opp->flags28 & 0x20) == 0)) &&
      ((opp->turn_flags & 2) == 0)) && (rob->state != 0)) {
    rob_flag_11b_set(opp);
  }
  return;
}

void move_event_branch_run(rob_rec *rob)
{
  char *entry;
  int event;

  event = rob->event_flags;
  entry = (char *)move_event_entry_find(rob,event);
  if (entry == 0) {
    rob->event_flags = 0;
  }
  else if (rob->frame >= *(short *)(entry + 4)) {
    if (event != 4) {
      rob->ctrl->continuation = 0;
      rob->ctrl->wait = 0;
    }
    rob_move_set(rob,(uint)*(ushort *)(entry + 8));
  }
  return;
}

uint * move_event_entry_find(rob_rec *rob, uint event)
{
  uint *entry;
  uint mask;
  uint *p;
  ushort n;

  p = (uint *)rob->event_list;
  n = rob->event_count;
  for (; n != 0; p += 2, n--) {
    if (p[1] != 0x13) break;
    entry = (uint *)*p;
    mask = *entry;
    if ((mask & 0x40) != 0) {
      if (event == 0x40) return entry;
    }
    else if ((event & mask) != 0 || event == mask) {
      return entry;
    }
  }
  return (uint *)0x0;
}

int rob_frames_to_ground(rob_rec *rob,float height)
{
  short n;
  short *s;
  float a;
  float b;
  if ((rob->flags & 8) == 0) {
    short id = rob->move_no;
    n = 0;
    s = (short *)move_cmd_arg_find(id,1,7);
    if (s != 0) {
      n = *s - rob->frame + 1;
    }
    else if (rob->state & 0x200040) {
      n = rob->field_0x2f2 - rob->frame + 1;
    }
  }
  else {
    a = world_gravity * rob->gravity_scale * rob->frame_speed;
    b = a * 0.5 - rob->velocity.y;
    n = (fsqrt_asm(b * b + (rob->pos.y - (rob->floor_y + height)) * (a + a)) - b) / a + 0.5;
    if (__builtin_fabsf(n * (rob->velocity.y - (n + 1) / (a + a)) + rob->pos.y - height) >= 0.02) n = n + 1;
  }
  if (n < 0) n = 0;
  return n;
}

void rob_damage_apply(rob_rec *rob, uint kinds)
{
  float *params;
  int check;
  e_mode_main mode;
  short hits;
  short life;
  short dmg;
  float rate;
  float scaled;
  rob_rec *opp;

  dmg = rob->pending_damage;
  if (dmg != 0) {
    hits = rob->combo[2].hits;
    if (3 < hits) {
      scaled = (float)dmg;
      rate = 0.8;
      hits = hits - 3;
      if (msnm_rule_is(3) != 0) {
        params = (float *)msnm_rule_params_get();
        rate = params[2];
      }
      if (msnm_rule9_combo_check(rob) != 0) {
        rate = rate * 0.5;
      }
      while (--hits != -1) {
        scaled = scaled * rate;
      }
      dmg = (short)scaled;
    }
    if ((kinds & 2) != 0) {
      rob->combo[0].damage = rob->combo[0].damage + dmg;
    }
    if ((kinds & 4) != 0) {
      rob->combo[1].damage = rob->combo[1].damage + dmg;
    }
    if ((kinds & 8) != 0) {
      rob->combo[2].damage = rob->combo[2].damage + dmg;
    }
    if ((kinds & 0x10) != 0) {
      rob->combo[3].damage = rob->combo[3].damage + dmg;
    }
    check = msnm_rule11_cpu_check((int)(char)rob->side);
    if ((((check == 0) || ((rob->state & 0x200000) != 0)) &&
        ((rob->flags & 0x400) == 0)) &&
       ((mode = mode_get_main(), mode != MODE_MAIN_GAME || (check = mode_get_sub(), check == 10)))
       ) {
      rob->field_0x22a = rob->field_0x22a + dmg;
      opp = rob->opp;
      opp->field_0x226 = opp->field_0x226 + dmg;
      opp->field_0x22c = opp->field_0x22c + dmg;
      rob->last_damage = dmg;
      opp->last_damage_2 = dmg;
      check = msnm_rule9_combo_check(rob);
      if (check != 0) {
        msnm_combo_d4_add((uint)rob->side,dmg);
      }
      life = rob->life - dmg;
      msnm_rule6_life_add(rob,dmg);
      rob->damage_last = dmg;
      rob->damage_sum = rob->damage_sum + dmg;
      if ((*(uint *)&rob->flags24 >> 9 & 1 & (uint)(life < 1)) != 0) {
        life = 1;
      }
      if (life < 1) {
        life = 0;
        rob->flags = rob->flags | 0x400;
        game_flags = game_flags | 0x28;
        rob->opp->event_flags =
             rob->opp->event_flags | 0x400000;
        rob->event_flags = rob->event_flags | 0x10;
      }
      else {
        rob->opp->event_flags =
             rob->opp->event_flags | 0x800000;
        rob->event_flags = rob->event_flags | 0x1000000;
      }
      rob->life = life;
    }
  }
  return;
}

void rob_hands_step(rob_rec *rob)
{
  rob_hand_step(rob,0);
  rob_hand_step(rob,1);
  return;
}

void rob_hands_shape_set(rob_rec *rob, float value)
{
  rob_hand_rec *hand0 = &rob->hands[0];
  rob_hand_rec *hand1 = &rob->hands[1];

  hand0->target = hand1->target = value;
  hand0->frames = hand1->frames = 0xc;
  hand0->flags &= 0xfe;
  hand1->flags &= 0xfe;
  return;
}

void rob_hand_step(rob_rec *rob, uint hand)
{
  rob_hand_rec *rec;
  int color;
  int tbl;
  char level;

  rec = rob->hands + (hand & 0xff);
  if ((rob->flags & 0x40000000) != 0) {
    tbl = *(int *)(rob->chara_data + 0x48);
    color = *(int *)(tbl + rob->costume * 0x20 + 0x4c);
  }
  else {
    tbl = *(int *)(rob->chara_data + 0x48);
    color = *(int *)(tbl + rob->costume * 0x20 + 0xc);
  }
  if (color >= 0) {
    if ((rec->flags & 1) != 0) {
      float f;
      f = rec->field_0x0 + 0.003125;
      if (0.2 < f) {
        f = 0.2;
      }
      else if (f < 0.0) {
        f = 0.0;
      }
      level = (int)(f * 155.0);
      rec->target = rec->current = f * 10.0;
    }
    else {
      float f, c, k, ik;
      short n;
      f = rec->target;
      c = rec->current;
      n = rec->frames;
      k = rob->rsmooth_weight;
      ik = 1.0 - k;
      if (n < 1) {
        n = 0;
      }
      else {
        f = c + (f - c) / (float)n;
        n = n - 1;
      }
      rec->frames = n;
      if ((rob->motion_flags & 8) != 0) {
        f = f * ik + rec->smooth_target * k;
      }
      rec->current = f;
      level = (int)(f * 15.5);
    }
    if (level > 31) {
      level = 31;
    }
    else if (level < 0) {
      level = 0;
    }
    if ((game_flags & 0x2000) != 0) {
      return;
    }
    rec->shape = 31 - level;
  }
  else {
    rec->shape = -1;
  }
}

void face_rigid_pick(rob_rec *rob)
{
  int tbl;
  int f;
  int idx;
  short *cnt;
  char *s;

  s = (char *)rob->chara_data + 0x40;
  tbl = *(int *)((rob->costume << 5) + *(int *)(s + 8) + 8);
  if (tbl != 0 && (rob->flags & 0x40000000) == 0) {
  f = rob->face_flags;
  if (f & 8) {
    f = 8;
    idx = 2;
  } else if (f & 2) {
    idx = 1;
    f &= ~2;
    rob->face_timer = 10;
    f |= 4;
  } else if (f & 4) {
    cnt = &rob->face_timer;
    (*cnt)--;
    idx = 1;
    if (*cnt < 0) {
      *cnt = 0;
      f &= ~4;
    }
  } else if (f & 0x10) {
    cnt = &rob->face_timer;
    (*cnt)--;
    f = 0x10;
    idx = 3;
    if (*cnt < 0) {
      *cnt = 0;
      f = 0;
    }
  } else {
    f = 0;
    if ((int)lcg_next15() % 100 <= 0 && (mode_get_main() != MODE_MAIN_ADVERTISE || mode_get_sub() != 8)) {
      rob->face_timer = (int)lcg_next15() % 9;
      f |= 0x10;
      idx = 3;
    } else {
      idx = 0;
    }
  }
  rob->face_flags = f;
  rob->rigid_face = *(int *)(tbl + idx * 4);
  } else {
    rob->rigid_face = -1;
  }
}

void move_hit_camera_shake(rob_rec *rob, byte mask)
{
  int n;
  byte *entry;

  entry = (byte *)rob->shake_list;
  n = (int)rob->shake_count;
  if (entry == (byte *)0x0 || n == 0) return;
  while (n = n + -1, n != -1) {
    if ((mask & *entry) != 0) {
      camera_shake_request(1,*(uint *)(entry + 4));
      return;
    }
    entry = entry + 8;
  }
}

int damage_scale(int rate, int damage)
{
  if (rate != 0) {
    rate = rate * 8 + 1000;
    damage = damage * rate;
    damage = damage / 1000;
  }
  return (short)damage;
}

int rob_rate_adjust(uint side, char delta)
{
  rob_rec *rob;
  rob = rob_get(side & 0xff);
  if ((rob->flags & 1) != 0) {
    rob->attack_rate_delta = rob->attack_rate_delta + delta;
    return 1;
  }
  return 0;
}

void rob_facing_override_clear(rob_rec *rob)
{
  rob->facing_override = 0xffffffff;
  return;
}

int rob_card_flag_test(uint side, int n_arg)
{
  char n = (char)n_arg;
  rob_rec *rob;
  int r;
  int o;
  rob = rob_get(side & 0xff);
  r = 0;
  if ((uint)(int)n < 0x19) {
    o = 0x1c0;
    if ((1 << (n + 7)) & *(uint *)((char *)rob + o)) r = 1;
  }
  return r;
}

int move_branch_open(rob_rec *rob)
{
  int *p;
  short v;
  int r;

  p = (int *)rob->branch_list;
  v = rob->frame;
  r = 0;
  if (p == 0) return 0;
  for (; *p != 0; p += 2) {
    if (*(short *)(*p + 8) >= v) {
      r = 1;
      break;
    }
  }
  return r;
}

void rob_flag_11b_set(rob_rec *rob)
{
  rob->field_0x11b = 1;
  return;
}

uint rob_flag_11b_take(rob_rec *rob, uint flags)
{
  if (rob->field_0x11b != '\0') {
    rob->field_0x11b = '\0';
    flags = flags | 0x20000;
    rob->side_flags = rob->side_flags | 0x1000;
  }
  return flags;
}

int rob_pending_attack_damage(char side)
{
  rob_rec *rob;
  int value;
  char *arg;
  int damage;

  rob = rob_get((byte)side);
  damage = 0;
  if ((rob->move_flags & 0x4000) != 0) {
    damage = (int)rob->attack_damage;
  }
  if (((((rob->flags28 & 0x800000) != 0) &&
       (value = script_wait_frames(side), value != -1 && value != 0x7fff)) &&
      (rob->frame + value < (int)rob->active_start)) &&
     ((value = script_next_move(side), (value - 1U & 0xffff) < 0x167b &&
      (arg = (char *)move_cmd_arg_find(value,0,3), arg != 0)))) {
    damage = damage_scale((int)rob->attack_rate,(int)*(short *)(arg + 8));
  }
  return damage;
}

rob_rec *rob_get(uint side)
{
  rob_rec *rob;

  if ((side & 0xff) < 2) {
    rob = &robs[side & 0xff];
  }
  else {
    rob = (rob_rec *)0x0;
  }
  return rob;
}

float * rob_mtx_get(uchar side)
{
  if (side < 2) {
    return robs[side].mtx;
  }
  return 0;
}

int hit_wait_timer_get(void)
{
  return (int)((short)hit_wait_timer);
}

int hit_wait_timer_set(int frames)
{
  short old;

  old = (int)hit_wait_timer;
  hit_wait_timer = (short)frames;
  return old;
}
