/*
 * vf4lib/libaction/src/rob_action.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_vec3_set_length
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern rob_action_handler_rec rob_action_handlers[28]; /* 0x0c17e1ac */
extern char lever_dir_mirror[8]; /* 0x0c17e28c */
extern char lever_dir_guard_sides[8]; /* 0x0c17e294 */
extern int game_frame_count; /* 0x0c292540 */
extern uint game_flags; /* 0x0c2925b8 */
extern undefined *guard_motion_tables[86]; /* 0x0c29fcdc */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern char lever_dir_motion_index[8]; /* 0x0c17e29c */

void rob_action_requests_take(void)
{
  rob_rec *rob;
  uint side;
  
  if ((game_flags & 0x2000) == 0) {
    side = 0;
    do {
      rob = rob_get(side & 0xff);
      rob_action_request_take(rob);
      side = side + 1;
    } while ((int)side < 2);
    throw_pair_connect();
    side = 0;
    do {
      rob = rob_get(side & 0xff);
      if (rob->request.word != rob->request_last.word) {
        control_marked_slots_clear((int)(char)rob->side);
      }
      side = side + 1;
    } while ((int)side < 2);
  }
  return;
}

void rob_action_request_take(rob_rec *rob)
{
  int ok;
  int v;
  char c;
  char b;
  short *ps;
  
  rob->request_last.word = rob->request.word;
  if (0 < rob->stun) {
    switch (rob->action.part.type) {
    case 6:
    case 8:
    case 0x16:
    case 0x18:
    case 0x1a:
      break;
    default:
      rob->stun = rob->stun - 1;
    }
  }
  if (rob->request_pending.word != 0) {
    rob->request.word = rob->request_pending.word;
    rob->request_pending.word = 0;
    rob->ctrl->dir_branch_list = 0;
    if (action_throw_kind(rob) == 1 && (rob->request.part.lever & 8) != 0 &&
        rob->throw_request != 0 &&
        (rob->reach_flags2 & rob->throw_request) == 0) {
      rob->request.word = 0;
    }
    rob->throw_request = 0;
    if (rob->request.word != 0) {
      return;
    }
  }
  if ((game_flags & 0x200000) != 0 && rob->request.word != 0x1000000) {
    v = rob->request.part.move & 0x7fff;
    ok = 0;
    switch (v) {
    case 0x724:
    case 0x11c3:
    case 0x11c4:
      ok = 1;
    }
    if (!ok) {
      rob->request.word = 0x1000000;
      rob->ctrl->continuation = 0;
      rob->ctrl->wait = 0;
    }
  }
  c = rob->request.part.type;
  switch (c) {
  case 0xb:
    b = rob->followup_flags;
    if ((b & 2) != 0) {
      if ((b & 8) != 0) {
        ps = &rob->stun;
        *ps = *ps - 3;
        if (*ps < 0) {
          *ps = 0;
        }
      }
      else {
        rob->frame = rob->frame + 3;
        rob->frame_pos = rob->frame_pos + 3.0;
      }
      hit_wait_timer_set((short)(hit_wait_timer_get() - 3));
    }
    rob->request.word = 0;
    rob->ctrl->dir_branch_list = 0;
    rob->followup_count = rob->followup_count + 1;
    return;
  case 4:
    if ((rob->move_flags & 1) == 0) break;
  default:
    rob->ctrl->dir_branch_list = 0;
  }
  if ((rob->flags & 0x80) != 0) {
    switch (c) {
    case 9:
    case 0xc:
    case 0xd:
    case 0xf:
    case 0x10:
    case 0x11:
    case 0x14:
      break;
    default:
      rob->request.word = 0;
    }
  }
  if (c == 0x10) {
    rob->request.part.type = 5;
    rob->request.part.lever = rob->request.part.lever | 4;
  }
}

void throw_pair_connect(void)
{
  typedef struct {
    unsigned id : 15;
    unsigned held : 1;
    unsigned rest : 16;
  } move_bits;
  rob_rec *thrower;
  rob_rec *thrown;
  rob_rec *t;
  uint fb;
  int ra;
  int rb;
  ushort *pv;
  int mode;
  int *lst;
  short s;
  short u;
  int k;
  ushort id;

  thrower = rob_get(0);
  thrown = rob_get(1);
  thrower->flags &= 0xffbfffff;
  fb = thrown->flags;
  thrown->flags = fb & 0xffbfffff;
  if ((thrower->flags & 1) == 0 || (fb & 1) == 0) return;
  ra = action_throw_kind(thrower);
  rb = action_throw_kind(thrown);
  if (ra == 1 | rb == 1) {
  if (ra == rb) {
    if ((game_frame_count & 1) == 0) goto keep;
  }
  else if (rb != 1) goto keep;
  t = thrower;
  thrower = thrown;
  thrown = t;
keep:
  thrown->ctrl->field_0x70 = 0;
  pv = (ushort *)move_cmd_arg_find(thrower->request.part.move & 0x7fff, 0, 0xf);
  if (pv != 0) {
  mode = 0;
  switch ((char)thrower->request.part.type) {
  case 0x15:
    thrown->request.part.type = 0x16;
    break;
  case 0x17:
    thrown->request.part.type = 0x18;
    if (thrown->flags & 8) rob_airborne_end(thrown);
    if (thrower->flags & 8) {
      thrower->place_pos.y = thrown->place_pos.y;
      rob_airborne_end(thrower);
    }
    break;
  case 0x19:
    thrown->request.part.type = 0x1a;
    if (thrown->flags & 8) rob_airborne_end(thrown);
    break;
  default:
    thrown->request.part.type = 6;
    if (thrown->flags & 8) {
      thrown->place_pos.y = thrower->place_pos.y;
      rob_airborne_end(thrown);
    }
    break;
  }
  thrown->request.part.lever = thrower->request.part.lever;
  ((move_bits *)&thrown->request)->id = *pv;
  thrower->throw_damage = 0;
  thrown->throw_damage = 0;
  rob_facing_override_clear(thrower);
  rob_facing_override_clear(thrown);
  u = thrower->request.part.move & 0x7fff;
  lst = *(int **)(move_record((short)(u - 1)) + 8);
  if (lst != 0) {
    for (; *lst != 0; lst += 2) {
      if (lst[1] == 0x2e && *(short *)*lst == thrown->chara) {
        {
          short *q = (short *)(*lst + 2);
          ((move_bits *)&thrown->request)->id = *q;
        }
        break;
      }
    }
  }
  if ((char)thrown->action.part.type != 0xe) thrown->field_0x1ec = 0;
  if (thrower->request.part.lever & 0x10) {
    if ((thrower->request.word & 0x7fff) != 0x108) mode = 2;
    if (pv[1] != 0) {
      s = thrown->attack_damage >> 1;
      thrower->throw_damage = s;
      thrown->throw_damage = s;
    }
    thrower->heading = thrower->opp_dir;
    thrown->heading = thrown->opp_dir;
    thrower->state &= 0xfffffeff;
    thrown->state &= 0xfffffeff;
    if (thrower->request.part.lever & 2) thrower->heading += -0x8000;
  }
  else if (thrower->request.part.lever & 4) {
    thrower->request.part.lever |= 0x10;
    thrown->heading = thrower->heading;
    thrower->facing_offset = 0;
    thrown->facing_offset = 0;
    thrower->state &= 0xfffffeff;
    thrown->state &= 0xfffffeff;
    camera.mode = camera.mode_return;
    camera.pers_req = 45.0;
  }
  else if (thrower->request.part.lever & 1) {
    mode = 2;
    if ((char)thrown->action.part.type == 8 && (char)thrown->action_step == 4) {
      thrown->heading = rob_move_facing(thrown, 0);
      thrown->state &= 0xfffffeff;
    }
    else {
      thrower->heading = rob_move_facing(thrower, 0);
      thrown->heading = k = rob_move_facing(thrown, 0);
      if ((user_back_data.option_bits & 0x40000) && (thrown->flags24 & 0x40)) {
        if (thrown->side_flags & 0x10) thrown->heading = k + 0x4000;
        else thrown->heading += -0x4000;
      }
      thrower->state &= 0xfffffeff;
      thrown->state &= 0xfffffeff;
    }
  }
  else if (thrower->request.part.lever & 0x20) {
    mode = 1;
  }
  switch (thrower->request.part.move & 0x7fff) {
  case 0xe13:
    thrower->heading = thrower->opp_dir;
    thrown->heading = thrown->opp_dir;
    thrower->state &= 0xfffffeff;
    thrown->state &= 0xfffffeff;
    break;
  case 0x103a:
    thrower->heading = thrower->opp_dir;
    thrown->heading = thrower->heading;
    thrower->state &= 0xfffffeff;
    thrown->state &= 0xfffffeff;
    break;
  }
  if (thrower->request.part.lever & 0x40) {
    s = rob_move_facing(thrower, 0);
    thrown->state &= 0xfffffeff;
    thrown->heading = s;
  }
  if (pv[4] != 0 && ((thrower->side_flags ^ thrown->side_flags) & 8)) {
    ((move_bits *)&thrown->request)->id = ((short *)pv)[4];
    thrown->side_flags ^= 8;
  }
  else {
    switch (mode) {
    case 0:
      if (thrown->side_flags & 8) thrower->side_flags |= 8;
      else thrower->side_flags &= 0xfff7;
      break;
    case 1:
      if (thrower->side_flags & 8) thrown->side_flags |= 8;
      else thrown->side_flags &= 0xfff7;
      break;
    case 2:
      thrower->side_flags &= 0xffe7;
      thrown->side_flags &= 0xffe7;
      break;
    }
  }
  if (thrower->request.word & 0x8000) {
    thrown->side_flags ^= 8;
    if (thrown->side_flags & 8) thrower->side_flags |= 8;
    else thrower->side_flags &= 0xfff7;
  }
  if (thrower->request.part.lever & 0x80) {
    thrower->state &= 0xfffffeff;
    thrower->heading = thrown->heading;
  }
  }
  else {
    thrower->request.word = 0;
  }
  }
  else {
    if (thrower->request.word & 0x8000) thrower->side_flags ^= 8;
    if (thrown->request.word & 0x8000) thrown->side_flags ^= 8;
  }
  thrower->request.word &= 0xffff7fff;
  thrown->request.word &= 0xffff7fff;
}

int action_throw_kind(rob_rec *rob)
{
  int kind;
  
  switch(rob->request.part.type) {
  case 5:
  case 0x15:
  case 0x17:
  case 0x19:
    kind = 1;
    break;
  case 0x10:
    kind = 2;
    break;
  default:
    kind = 0;
    break;
  }
  return kind;
}

void rob_action_step(rob_rec *rob)
{
  rob_action_start(rob);
  rob_action_update(rob);
  return;
}

void action_reaction_record_reset(rob_rec *rob)
{
  int kind;
  kind = *(int *)(rob->reaction + 0x14);
  if (kind != 1) {
    if (kind != 3) {
      switch(rob->request.part.type) {
      case 6:
      case 7:
      case 8:
      case 10:
      case 0xc:
      case 0xd:
      case 0x11:
      case 0x16:
      case 0x18:
      case 0x1a:
        rob->stance = 0;
        rob->reaction = **(uchar ***)(rob->chara_data + 0x40);
        rob->move_table = *rob->move_tables;
      }
    }
  }
}

void rob_action_start(rob_rec *rob)
{
  char type;
  
  if (rob->request.word != 0) {
    type = rob->request.part.type;
    action_reaction_record_reset(rob);
    reaction_entry_flag_update(rob);
    if (type < 0x1c) {
      (*rob_action_handlers[type].start)(rob);
      rob->request.word = 0;
    }
  }
  return;
}

void rob_action_update(rob_rec *rob)
{
  if ((rob->action.word != 0) && (rob->action.part.type < 0x1c)) {
    (*rob_action_handlers[*(char *)&rob->action.part.type].update)(rob);
  }
}

void rob_airborne_begin(rob_rec *rob)
{
  if ((~rob->flags & 8) != 0) {
    rob->flags = rob->flags | 8;
    rob->place_pos.y = rob->pos.y;
    if ((~rob->air_flags & 1) != 0) {
      rob->place_pos.x = rob->pos.x;
      rob->place_pos.z = rob->pos.z;
    }
  }
  rob->state = rob->state | 0x40;
  return;
}

void rob_airborne_end(rob_rec *rob)
{
  float *mtx;
  float v [3];
  
  if ((rob->flags & 8) != 0) {
    rob->flags = rob->flags & 0xfffbfff7;
    rob->state = rob->state & 0xffffffbf;
    if ((~rob->air_flags & 1) != 0) {
      rob_base_mtx_update(rob,(float)(int)rob->field_0x2f2 * 0.033333335);
      mtx = (float *)rob_base_mtx_get((uint)rob->side);
      vec3_rot_y_s16((int)rob->heading,mtx + 12,v);
      rob->place_pos.x = rob->place_pos.x - v[0];
      rob->place_pos.z = rob->place_pos.z - v[2];
    }
    rob->place_pos.y = rob->floor_y_kept;
  }
  return;
}

void action_move_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_move_update(rob_rec *rob)
{
  char *step;
  
  step = &rob->action_step;
  if (*step == '\0') {
    rob_move_set(rob,(int)rob->action.part.move & 0x7fff);
    *step = *step + '\x01';
  }
  return;
}

void neutral_catch_set(rob_rec *rob)
{
  if (rob->action.word == 0x1000000) {
    switch (rob->chara) {
    case 6:
    case 12:
      if (rob->stance != '\0') {
        return;
      }
      rob->flags24 = rob->flags24 | 0x400;
      rob->catch_mask = 0x3000001;
      break;
    case 11:
      if (rob->stance != '\x02') {
        return;
      }
      if ((rob->state & 1) != 0) {
        return;
      }
      rob->flags24 = rob->flags24 | 0x400;
      rob->catch_mask = 0x7000023;
      break;
    }
  }
}

void action_neutral_start(rob_rec *rob)
{
  e_mode_main main;
  int sub;
  
  main = mode_get_main();
  if (((main != MODE_MAIN_OVER) &&
      ((main = mode_get_main(), main != MODE_MAIN_GAME ||
       ((sub = mode_get_sub(), sub != 7 && (sub = mode_get_sub(), sub != 0xf)))))) &&
     ((main = mode_get_main(), main != MODE_MAIN_ADVERTISE || (sub = mode_get_sub(), sub != 8)
      ))) {
    char c;
    byte b;
    if ((rob->side_flags & 6) == 0) {
      c = rob->action.part.type;
      b = rob->action.part.lever;
      if (c == '\x02' && (b & 0x30) != 0) {
        neutral_catch_set(rob);
        return;
      }
    }
    rob_action_accept(rob);
  }
  return;
}

void action_neutral_update(rob_rec *rob)
{
  uint f;
  uchar b;
  ushort v;
  f = rob->state;
  b = rob->action.part.lever;
  if (b & 2) {
    rob_move_set(rob,rob->move_table[47]);
    rob->action.word = 0;
    return;
  }
  if (f & 0x20) {
    if (rob->followup_word == 0) {
      if ((rob->side_flags & 2) == 0) return;
      rob_move_set(rob,*rob->move_table);
    }
    else rob_move_set(rob,*(ushort *)&rob->followup_word);
    return;
  }
  if (rob->flags24 & 0x4000000) return;
  if ((rob->move_flags & 1) == 0 || (rob->side_flags & 2) != 0 ||
      ((rob->old_cur->action.word & 0xff020000) == 0x04020000 && (rob->move_flags & 0x40000) == 0)) {
    if (b & 1) {
      v = rob->move_table[2];
      if ((f & 0x2000) == 0) {
        if (rob->field_0x284 & 0x80) motion_switch_keep_phase(rob,v,1);
        else if ((rob->move_stance & 0x80) == 0) rob_move_set(rob,v);
      }
      else if (rob->side_flags & 4) rob_move_set(rob,rob->move_table[1]);
      else if ((f & 0x1000) == 0) {
        if ((rob->field_0x284 & 0x80) == 0 && (rob->move_flags & 0x4000) == 0) motion_switch_keep_phase(rob,v,0);
        else if ((rob->move_stance & 0x80) == 0) motion_switch_keep_phase(rob,v,1);
        else if (f & 0x8000) rob_move_set(rob,rob->move_table[9]);
        else if (rob->flags28 & 0x100) rob_move_set(rob,rob->move_table[1]);
      }
      guard_frame_catch_up(rob);
      return;
    }
    if (f & 0x2000) {
      v = rob->move_table[3];
      if (rob->state & 0x4000) rob_move_set(rob,v);
      else if ((rob->field_0x284 & 0x80) == 0) motion_switch_keep_phase(rob,v,1);
      else if (rob->move_stance & 0x80) rob_move_set(rob,v);
      else rob_move_set(rob,rob->move_table[1]);
    }
    else if (rob->side_flags & 4) rob_move_set(rob,*rob->move_table);
    else if ((f & 0x1000) == 0) {
      if (rob->field_0x284 & 0x80) motion_switch_keep_phase(rob,rob->move_table[3],0);
      else if (rob->move_stance & 0x80) motion_switch_keep_phase(rob,rob->move_table[3],1);
      else if (f & 0x8000) rob_move_set(rob,rob->move_table[6]);
      else rob_move_set(rob,*rob->move_table);
    }
  }
  neutral_catch_set(rob);
}

void action_lever_start(rob_rec *rob)
{
  typedef struct { unsigned char dir : 3; unsigned char b3 : 1; unsigned char b4 : 1; unsigned char b5 : 1; unsigned char b6 : 1; unsigned char b7 : 1; } lever_t;
  uchar f;
  lever_t l0;
  uint a;
  uint b;
  uint c;
  uchar *q;

  f = rob->request.part.lever;
  l0 = *(lever_t *)&rob->request.part.lever;
  a = f & 7;
  if (rob->action.part.type != 2) {
    if (!l0.b4 && !l0.b5) {
      if ((f & 8) != 0 && *(int *)(rob->reaction + 0x10) == 3) {
        rob->request.word = 0x1010000;
        rob_action_accept(rob);
        return;
      }
      if ((rob->flags & 0x10) != 0 && (a == 0 || a == 5)) {
        rob->request.word = 0x1000000;
        rob_action_accept(rob);
        return;
      }
    }
    b = a;
    c = 0;
    rob->action_step = 0;
  }
  else {
    c = rob->action.part.lever;
    b = c & 7;
  }
  if ((user_back_data.option_bits & 0x20) == 0 && (rob->relation_flags & 1) != 0) {
    switch (a) {
    case 1:
      a = 3;
      break;
    case 2:
      a = 4;
      break;
    }
  }
  if ((rob->request.part.lever & 0x38) != 0x28) {
    if ((rob->flags & 0x20) != 0) {
      q = (uchar *)&lever_dir_mirror;
      a = q[a];
    }
    if (rob->side_flags & 8) {
      q = (uchar *)&lever_dir_mirror;
      a = q[a];
    }
  }
  if ((rob->action.word & 0x400000) != 0) {
    if (a == 5) {
      rob->action.part.lever = 0x85;
    }
    else if (a != 0) {
      rob->action.part.lever = a + 0xa0;
    }
    else if (rob->opp_dist < 1.2 && (rob->relation_flags & 1) == 0) {
      rob->action.word = 0x1000000;
      return;
    }
  }
  else if ((rob->request.word & 0x100000) != 0) {
    if ((c & 8) != 0 && rob->frame < rob->active_end) {
      return;
    }
    rob->action.part.lever = a + 0x90;
  }
  else if ((rob->request.word & 0x200000) != 0) {
    if ((c & 8) != 0 && rob->frame < rob->active_end) {
      return;
    }
    if ((c & 0x20) != 0 && rob->frame < rob->active_end) {
      if ((rob->request.word & 0x80000) == 0) {
        return;
      }
      if ((rob->action.word & 0x7fff) - 0x1c > 1) {
        return;
      }
    }
    rob->action.part.lever = rob->request.part.lever | 0x80;
  }
  else if ((rob->action.word & 0x100000) != 0) {
    if (rob->frame < rob->recovery_end) {
      return;
    }
    rob->action.part.lever = a | 0x80;
  }
  else if ((rob->action.word & 0x200000) != 0) {
    if (*(int *)(rob->reaction + 0x10) == 3 && a == 0 && b == 0 && (c & 8) == 0 &&
        rob->frame >= rob->active_end &&
        (rob->relation_flags & 1) == 0 && rob->opp_dist > 2.0) {
      rob->action.part.lever = 0xc0;
    }
    else if (rob->frame >= rob->recovery_end) {
      rob->action.part.lever = a + 0x80;
      if ((f & 8) != 0) {
        rob->action.part.lever = a + 0x88;
      }
      else {
        rob->action.part.lever = a + 0x80;
      }
    }
  }
  else if (rob->action.part.type == 2 && a != b) {
    rob->action.part.lever = a + 0x80;
  }
  else {
    rob->action.part.lever = a;
  }
  rob->action.part.type = 2;
  rob->action.word = rob->action.word & 0xffff8000;
}

int lever_guard_check(rob_rec *rob, int dir_arg)
{
  char dir = (char)dir_arg;
  uchar bits;
  uchar *tp;
  rob_old_rec *old;
  uchar m;
  uint t;
  
  bits = rob->opp->old_cur->field_0x0;
  if ((bits & 0x22) == 0x22) return 1;
  if ((rob->relation_flags & 1) != 0) return 0;
  if (rob->field_0x104 > 0) return 0;
  if ((bits & 2) == 0) return 0;
  if ((bits & 8) != 0) return 0;
  if ((rob->opp->old_cur->side_flags & 0x1000) != 0 &&
      (int)rob->flags28 >= 0) return 0;
  if ((rob->opp->old_cur->flags28 & 0x1000020) != 0) return 0;
  old = rob->opp->old_cur;
  m = 3 & old->field_0x50 >> 8;
  if (m != 0) {
    tp = lever_dir_guard_sides;
    t = tp[dir];
    if ((old->side_flags & 0x10) != 0) m ^= 3;
    if ((rob->side_flags & 8) != 0) m ^= 3;
    if (t != 0 && (m & t) == 0) return 0;
  }
  return 1;
}

void lever_motion_set(rob_rec *rob, char idx, char mode)
{
  rob->action.part.lever = rob->action.part.lever & 0x7f;
  ((struct { unsigned int f : 15; } *)&rob->action)->f = idx;
  rob->action_step = 1;
  if (mode == 2) {
    rob_move_set(rob,(uint)rob->move_table[idx]);
  }
  else {
    motion_switch_keep_phase(rob,(uint)rob->move_table[idx],mode);
  }
}

void action_lever_update(rob_rec *rob)
{
  uchar b;
  char sb;
  int lo;
  int f;
  char x;
  int y;
  char *q;

  sb = rob->action.part.lever;
  b = sb;
  lo = b & 7;
  f = 0;
  if ((rob->action_step == '\0') || ((rob->side_flags & 2) != 0) ||
      ((sb & -0x80) != 0)) {
    if ((rob->relation_flags & 1) != 0) {
      f = 1;
    }
    if ((b & 0x28) == 0x28) {
      if (lo == 0) {
        if ((rob->flags & 0x10) != 0) x = 0x27;
        else x = 0x25;
        x = x + f;
      }
      else {
        x = 0x29 + ((rob->flags & 0x10) != 0);
      }
      rob->opp->flags =
           rob->opp->flags & 0xfffdffff;
    }
    else if ((b & 0x40) != 0) {
      x = 0x2b;
    }
    else if ((b & 0x10) != 0) {
      x = 0x20 - (lo == 3);
      if (lever_guard_check(rob,lo) == 0) {
        if (x == 0x1f) y = 0x21;
        else y = 0x23;
        x = y + f;
        if (rob->field_0x104 == 0) {
          rob->field_0x104 = 0x3c;
        }
      }
      else {
        attack_result_sounds_play(rob,rob,0x800);
      }
    }
    else if ((b & 0x20) != 0) {
      q = lever_dir_motion_index;
      x = q[lo];
    }
    else if ((rob->relation_flags & 1 & (lo < 5)) != 0) {
      x = lo + 0x16;
    }
    else {
      x = lo + 0xe;
    }
    lever_motion_set(rob,x,2);
  }
}

int jump_move_kind(rob_rec *rob, int move)
{
  short *args;
  short *frames;
  char t;
  
  args = (short *)move_cmd_arg_find((int)(short)move,0,2);
  if (args == 0) return 0;
  frames = &rob->field_0x2f4;
  *frames = args[3];
  if (half_gravity_active() == 1) {
    *frames = *frames * 2;
  }
  t = args[1] == 0;
  t = !t;
  return t + 1;
}

void action_jump_start(rob_rec *rob)
{
  uchar f1;
  int m1;
  int m;
  uchar f;
  uchar f2;
  uchar b;
  int k;
  int r;
  short sv;
  uint x;
  int v;
  uchar *base;

  if (rob->action.part.type != 3) {
    f1 = rob->request.part.lever;
    m1 = rob->request.part.move & 0x7fff;
    k = 0x2c;
    if ((f1 & 4) != 0) {
      k = 0x2d;
    }
    if ((f1 & 1) != 0) {
      rob->jump_dir = 1;
    }
    else if ((f1 & 2) != 0) {
      rob->jump_dir = -1;
    }
    else {
      rob->jump_dir = 0;
    }
    if (m1 != 0) {
      switch (jump_move_kind(rob,m1)) {
      case 0:
        rob->action.word = m1;
        rob->action_step = 0;
        return;
      default:
        v = (short)rob->move_table[k];
        v = (ushort)v;
        break;
      case 2:
        v = (ushort)m1;
        break;
      }
      rob_move_set(rob,v);
      rob->action.part.lever = f1;
      ((struct { unsigned int m : 15; } *)&rob->action)->m = (ushort)m1;
    }
    else {
      rob_move_set(rob,rob->move_table[k]);
      rob->action.part.lever = f1;
      rob->action.word = rob->action.word & 0xffff8000;
    }
    rob->action.part.type = 3;
    rob->action_step = 0;
    return;
  }
  switch ((uchar)rob->action_step) {
  case 0:
    m = rob->request.part.move & 0x7fff;
    f = rob->request.part.lever;
    b = rob->action.part.lever;
    if ((b & 0x40) != 0) {
      return;
    }
    sv = rob->frame;
    if (m == 0 && (f & 4) != 0 && (~b & 4) != 0) {
      if ((b & 2) != 0) {
        m = (short)rob->move_table[46];
      }
      else {
        m = (short)rob->move_table[45];
      }
    }
    if (m == rob->move_no) {
      return;
    }
    switch (jump_move_kind(rob,m)) {
    case 0:
      rob->action.word = m;
      rob->action_step = 0;
      return;
    case 2:
      if ((~f & 8) != 0) {
        rob_move_set(rob,(ushort)m);
        if (rob->field_0x2ee < sv) {
          sv = rob->field_0x2ee;
        }
        rob->frame = sv;
        rob->frame_pos = sv;
        return;
      }
    case 1:
      ((struct { unsigned int m : 15; } *)&rob->action)->m = m;
      rob->action.part.lever = b | f;
    }
    break;
  case 1:
    base = (uchar *)rob + 0x40;
    m = rob->request.part.move & 0x7fff;
    f2 = rob->request.part.lever;
    b = rob->action.part.lever;
    if (m == 0) {
      return;
    }
    jump_move_kind(rob,m);
    b = b | f2;
    rob->action.part.lever = b;
    ((struct { char pad[0x20]; unsigned int m : 15; } *)base)->m = (ushort)m;
    if ((b & 8) != 0) {
      return;
    }
    rob_move_set(rob,(ushort)m);
  }
}

void jump_target_offset(rob_rec *rob, float *out)
{
  float x;
  rob_rec *opp;
  if ((rob->side_flags & 0x10) == 0) {
    x = rob->jump_target_x;
  }
  else {
    x = -rob->jump_target_x;
  }
  *out = x;
  out[2] = -rob->jump_target_z;
  out[1] = 0.0;
  vec3_rot_y_s16((int)rob->opp_dir,out,out);
  opp = rob->opp; vec3_add_to(out,&opp->old_cur->pos.x);
  vec3_sub_from(out,&rob->pos.x);
  out[1] = 0.0;
}

void action_jump_update(rob_rec *rob)
{
  vec3 v;
  float w[3];
  int m;
  int n;
  int k;
  float fn;
  float len;
  float f;
  float d;
  float l1;

  switch ((byte)rob->action_step) {
  case 0:
    if (rob->frame < rob->field_0x2ee) return;
    rob_airborne_begin(rob);
    rob->state &= ~2;
    if ((~rob->action.part.lever & 8) != 0 && (m = rob->action.part.move & 0x7fff) != 0 &&
        rob->move_no != m) {
      rob_move_set(rob,(ushort)m);
    }
    n = rob->air_frames;
    if (n > 0) k = n;
    else if (rob->action.part.lever & 4) k = ((char *)rob->chara_data)[13];
    else k = ((char *)rob->chara_data)[12];
    fn = (float)k;
    v.y = (world_gravity - rob->slide_velocity.y) * fn * 0.5 +
          ((rob->jump_height + rob->floor_y) - rob->pos.y) / fn;
    v.z = 0.0;
    v.x = 0.0;
    if (rob->action.part.lever & 0x10) {
      if (rob->opp_dist > 0.1) {
        jump_target_offset(rob,w);
        len = vec3_length(w);
        if (len > *(float *)(rob->chara_data + 0x14)) len = *(float *)(rob->chara_data + 0x14);
        if (half_gravity_active() == 1) len *= 0.5;
        vec3_set_length(w,w,len / fn);
        v.x = w[0];
        v.z = w[2];
      }
    }
    else if (rob->jump_dir != 0) {
      if (rob->action.part.lever & 0x20) f = 0.1;
      else f = 0.05;
      vec_xz_from_heading((float)rob->jump_dir * f,rob->heading,&v.x);
    }
    rob->velocity = v;
    rob->action_step = 1;
    return;
  case 1:
    if ((rob->action.part.lever & 8) && rob->frames_to_land <= rob->field_0x2f4) {
      rob_move_set(rob,rob->action.part.move & 0x7fff);
      rob->action.part.lever &= ~8;
    }
    if (rob->flags & 8) {
      if (!(rob->action.part.lever & 0x10)) return;
      if (rob->frame > jump_steer_frame_max) return;
      d = (float)(rob->air_frames - rob->field_0x2ee);
      if (d > 0.0) {
        jump_target_offset(rob,w);
        l1 = vec3_length(w);
        if (l1 > *(float *)(rob->chara_data + 0x14)) l1 = *(float *)(rob->chara_data + 0x14);
        if (half_gravity_active() == 1) l1 *= 0.5;
        vec3_set_length(w,w,l1 / d);
        rob->velocity.x = w[0];
        rob->velocity.z = w[2];
      }
      return;
    }
    rob->action_step = 2;
    rob->velocity.x = 0.0;
    rob->velocity.y = 0.0;
    rob->velocity.z = 0.0;
    rob->slide_velocity.x = 0.0;
    rob->slide_velocity.y = 0.0;
    rob->slide_velocity.z = 0.0;
    control_resume_clear((char)rob->side);
  case 2:
    if ((rob->side_flags & 2) || !(rob->state & 1)) {
      rob->action.word = 0;
    }
    break;
  }
}

void action_guard_start(rob_rec *rob)
{
  if (rob->stun == 0) {
    rob_action_accept(rob);
  }
  return;
}

int guard_motion_pick(rob_rec *rob, char crouching)
{
  typedef struct { unsigned int f : 15; unsigned int : 2; unsigned int h : 1; } bits;
  rob_rec *opp = rob->opp;
  uchar *base;
  uchar *tbl;
  int side;
  int z, m;
  short id, dd;
  short *res;
  int d, n;
  float f;

  if ((opp->old_cur->move_flags & 0x4000) == 0 || (opp->old_cur->state & 2) == 0) return 0;
  if (rob->opp_dist > guard_pick_dist_max) return 0;
  if ((rob->relation_flags & 1) != 0) return 0;
  tbl = guard_motion_tables[opp->old_cur->field_0x50 & 0xff];
  base = tbl;
  switch (opp->old_cur->attack_level) {
  case 1:
  case 2:
  case 4:
  case 7:
    if (crouching == 1) return 0;
    break;
  case 6:
    if ((rob->flags & 0x10) != 0) tbl = base + 0x10;
    break;
  case 3:
  case 5:
  case 13:
    if (crouching == 0) return 0;
    tbl = base + 0x10;
    break;
  default:
    return 0;
  }
  m = 0x100;
  z = (opp->old_cur->field_0x50 & m) == 0;
  side = z == 0;
  if ((rob->relation_flags & 1) != 0) side = (short)(side + 2);
  if ((rob->side_flags & 0x10) != 0) side ^= 1;
  if ((opp->old_cur->side_flags & 0x10) != 0) side ^= 1;
  id = *(short *)(tbl + side * 4);
  if (id == -1) return 0;
  if (id != rob->move_no) {
    res = (short *)move_cmd_arg_find(id, 0, 1);
    if (res != 0) {
      dd = opp->old_cur->active_start - opp->old_cur->frame;
      if (dd > (short)(*res - 1)) return 0;
    }
    rob_move_set(rob, (ushort)id);
    rob->state &= ~1;
  } else {
    d = (short)(opp->old_cur->active_start - opp->old_cur->frame);
    n = (short)(rob->active_start - rob->frame);
    if (rob->frame > rob->active_end) return 0;
    if (n < 1) {
      rob->frame_pos = rob->frame_pos - rob->frame_speed;
      rob->frame = (short)rob->frame_pos;
    } else {
      rob->frame_pos = (float)(n - d) / (float)d + rob->frame_pos;
      rob->frame = (short)rob->frame_pos;
    }
  }
  rob->action.part.lever |= 2;
  ((bits *)&rob->action)->f = id;
  return 1;
}

void action_guard_update(rob_rec *rob)
{
  uint f;
  uchar b;
  ushort id;

  f = rob->state;
  b = rob->action.part.lever;
  if (b & 1) {
    id = rob->move_table[13];
    if (b & 4) {
      if (rob->move_no != id) rob_move_set(rob,id);
      return;
    }
    if (rob->move_no == id && (rob->side_flags & 2) == 0) return;
    if (guard_motion_pick(rob,1) != 0) return;
    id = rob->move_table[10];
    if ((f & 0x2000) == 0) {
      if (rob->field_0x284 & 0x80) {
        motion_switch_keep_phase(rob,id,1);
      }
      else if ((rob->move_stance & 0x80) == 0) {
        rob_move_set(rob,id);
      }
    }
    else if (rob->side_flags & 4) {
      rob_move_set(rob,rob->move_table[7]);
    }
    else if (rob->move_flags & 0x4000) {
      rob_move_set(rob,rob->move_table[8]);
    }
    else if ((rob->move_stance & 0x80) == 0) {
      motion_switch_keep_phase(rob,id,1);
    }
    else if ((rob->field_0x284 & 0x80) == 0) {
      motion_switch_keep_phase(rob,id,0);
    }
    else if ((f & 0x8000) == 0) {
      rob_move_set(rob,rob->move_table[8]);
    }
    else if ((rob->move_flags & 1) && (rob->flags24 & 0x10000)) {
      rob_move_set(rob,rob->move_table[7]);
    }
    guard_frame_catch_up(rob);
  }
  else {
    id = rob->move_table[12];
    if (b & 4) {
      if (rob->move_no != id) rob_move_set(rob,id);
      return;
    }
    if (rob->move_no == id && (rob->side_flags & 2) == 0) return;
    if (guard_motion_pick(rob,0) != 0) return;
    if (f & 0x2000) {
      id = rob->move_table[11];
      if (rob->move_flags & 0x4000) {
        rob_move_set(rob,id);
      }
      else if ((rob->field_0x284 & 0x80) == 0) {
        motion_switch_keep_phase(rob,id,1);
      }
      else if (rob->move_stance & 0x80) {
        rob_move_set(rob,id);
      }
    }
    else if (rob->side_flags & 4) {
      rob_move_set(rob,rob->move_table[4]);
    }
    else if (rob->move_flags & 0x4000) {
      rob_move_set(rob,rob->move_table[5]);
    }
    else if (rob->field_0x284 & 0x80) {
      motion_switch_keep_phase(rob,rob->move_table[11],0);
    }
    else if ((f & 0x8000) == 0) {
      rob_move_set(rob,rob->move_table[5]);
    }
    else if ((rob->move_flags & 1) && (rob->flags24 & 0x10000)) {
      rob_move_set(rob,rob->move_table[4]);
    }
  }
  if (rob->side_flags & 0x20) {
    rob->action.part.lever &= ~2;
  }
}

void action_guard_stun_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_guard_stun_update(rob_rec *rob)
{
  short v;
  short t;
  short s;
  float f;
  float g;
  float h;
  switch ((uchar)rob->action_step) {
  case 0:
    rob->ctrl->continuation = 0;
    rob->ctrl->wait = 0;
    v = rob->action.part.move & 0x7fff;
    if (v == 0) {
      if ((rob->guard_state & 2) != 0) v = rob->move_table[7];
      else v = rob->move_table[4];
    }
    rob_move_set(rob,(ushort)v);
    f = rob->knockback.x;
    g = rob->knockback.z;
    h = f + f;
    rob->velocity.x = f;
    rob->velocity.z = g;
    rob->slide_velocity.x = h;
    rob->slide_velocity.z = h;
    rob->flags = rob->flags | 0x80;
    rob->state = rob->state | 1;
    rob->frame = rob->active_start;
    rob->frame_pos = (float)rob->active_start;
    rob->action_step = rob->action_step + 1;
  case 1:
    s = rob->active_end - 1;
    if (rob->frame < s) break;
    rob->action_step = rob->action_step + 1;
  case 2:
    t = rob->active_end - 1;
    if (t < (short)(rob->recovery_end - rob->stun) || rob->stun == 0) {
      rob->action_step = rob->action_step + 1;
    }
    else {
      rob->frame = t;
      rob->frame_pos = (float)t;
      break;
    }
  case 3:
    if (rob->stun == 0) {
      rob->action.word = 0;
      rob->state = rob->state & 0xfffffffe;
      rob->flags = rob->flags & 0xffffff7f;
    }
  }
}

void action_throw_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_throw1_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_throw2_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_throw3_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_throw_update(rob_rec *rob)
{
  throw_update_common(rob,0);
  return;
}

void action_throw1_update(rob_rec *rob)
{
  throw_update_common(rob,1);
  return;
}

void action_throw2_update(rob_rec *rob)
{
  throw_update_common(rob,2);
  return;
}

void action_throw3_update(rob_rec *rob)
{
  throw_update_common(rob,3);
  return;
}

void throw_update_common(rob_rec *rob, char kind)
{
  struct z12 { int a; int b; int c; } z;
  uint v;
  uint h;
  uchar *script;
  rob_rec *opp;

  opp = rob->opp;
  if ((opp->old_cur->flags & 0x100) != 0 &&
      rob->frame < rob->field_0x296) {
    camera.mode = MODE_ROB;
    camera.pers_req = 45.0;
    camera.flags = camera.flags | 0x10;
    rob->action.word = 0;
    rob->flags = rob->flags & 0xffdfff7f;
    v = rob->state & 0xfffffeff;
    rob->state = v;
    rob->heading = rob->opp_dir;
    if ((v & 0x2000) != 0) {
      rob_move_set(rob, (rob->move_table)[1]);
    }
    else {
      rob_move_set(rob, *rob->move_table);
    }
    return;
  }
  switch ((uchar)rob->action_step) {
  case 0:
    control_buffer_use((int)rob->side, 6);
    script = rob->ctrl->field_0x70;
    if (script != 0) {
      rob->ctrl->continuation = script;
    }
    rob->flags = rob->flags & 0xffffff7f;
    if (rob->old_cur->action.part.type != 0xe) {
      rob->side_flags = rob->side_flags | 0x2000;
    }
    rob_move_set(rob, rob->action.part.move & 0x7fff);
    z.a = 0; z.b = 0; z.c = 0;
    *(struct z12 *)&rob->velocity = z;
    *(struct z12 *)&rob->slide_velocity = z;
    rob->action_step = 1;
    break;
  case 1:
    if ((uchar)(kind - 2) > 1 && throw_position_follow(rob, rob->opp) == 0) {
      return;
    }
    rob->action_step = 2;
    break;
  case 2:
    if ((rob->flags & 8) != 0) return;
    if (rob->frame < rob->recovery_end) return;
    if (rob->move_no == 0x8b5) {
      if ((rob->state & 0x40) != 0) return;
      opp = rob->opp;
      if ((opp->old_cur->flags & 8) != 0) return;
    }
    if (rob->move_stance == 3 && (rob->state & 0x800) != 0) {
      rob->throw_offset.y = rob->floor_y_kept - rob->place_pos.y;
    }
    if (rob->move_stance == 3) {
      rob->flags = rob->flags | 0x80;
      rob->stun = 2;
      rob->request_pending.word = 0x9000000;
      return;
    }
    if ((rob->followup_word & 0x40000000) != 0) return;
    if (rob->followup_word == -1) return;
    rob->action.word = 0;
    break;
  }
}

void action_thrown_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_thrown1_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_thrown2_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_thrown3_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_thrown_update(rob_rec *rob)
{
  thrown_update_common(rob,0);
  return;
}

void action_thrown1_update(rob_rec *rob)
{
  thrown_update_common(rob,1);
  return;
}

void action_thrown2_update(rob_rec *rob)
{
  thrown_update_common(rob,2);
  return;
}

void action_thrown3_update(rob_rec *rob)
{
  thrown_update_common(rob,3);
  return;
}

void thrown_update_common(rob_rec *rob, int kind_arg)
{
  char k = kind_arg;
  struct z12 { int a; int b; int c; } z;
  uint v;
  rob_rec *opp;
  char *base;
  char c;
  short *s;
  short *t;
  short h;
  short a;
  float f;

  opp = rob->opp;
  if ((opp->old_cur->flags & 0x100) != 0 &&
      rob->frame < rob->field_0x296) {
    camera.mode = MODE_ROB;
    camera.pers_req = 45.0;
    camera.flags = camera.flags | 0x10;
    rob->action.word = 0;
    rob->flags = rob->flags & 0xffdfff7f;
    v = rob->state & 0xfffffeff;
    rob->state = v;
    rob->heading = rob->opp_dir;
    if ((v & 0x2000) != 0) {
      rob_move_set(rob, (rob->move_table)[1]);
    }
    else {
      rob_move_set(rob, *rob->move_table);
    }
    return;
  }
  switch ((uchar)rob->action_step) {
  case 0:
    rob->stun = 0;
    base = (char *)rob + 0x40;
    rob_move_set(rob, rob->action.part.move & 0x7fff);
    rob->flags24 = rob->flags24 | 0x2000000;
    z.a = 0; z.b = 0; z.c = 0;
    *(struct z12 *)&rob->velocity = z;
    *(struct z12 *)&rob->slide_velocity = z;
    (*(control_rec **)(base + 8))->continuation = 0;
    (*(control_rec **)(base + 8))->wait = 0;
    rob->face_flags = rob->face_flags | 8;
    rob->flags = rob->flags | 0x10080;
    rob->action_step = 1;
    control_buffer_clear((int)(char)rob->side);
    control_pending_clear((int)(char)rob->side);
    break;
  case 1:
    if ((uchar)(k - 2) < 2 && throw_position_follow(rob, rob->opp) == 0) {
      return;
    }
    rob->action_step = 2;
    break;
  case 2:
    if ((rob->flags & 8) != 0) return;
    if (rob->frame < rob->field_0x2f2) return;
    if (rob->move_no == 0x8b6 && (rob->state & 0x40) != 0) return;
    if ((rob->move_flags & 0x400) != 0) {
      attack_result_sounds_play(rob, rob, 0x200);
      fish_seeker_rob_landed(0, rob);
      fish_school_rob_landed(1, rob);
      break_floor_rob_landed(rob);
      camera_shake_request(1, 1);
      rob->throw_offset.y = rob->floor_y_kept - rob->place_pos.y;
      rob->state = rob->state | 0x400;
    }
    c = rob->opp->throw_damage;
    if ((rob->move_flags & 0x400) != 0) {
      t = &rob->field_0x166;
      h = *t + c;
      rob->stun = h >> 1;
      *t = 0;
    }
    else {
      rob->stun = 0;
      rob->field_0x166 = 0;
    }
    if (c != 0 && (rob->flags & 0x10000) != 0) {
      s = &rob->pending_damage;
      *s = c;
      if (msnm_rule_is(4)) {
        f = (float)*s;
        f = f * msnm_rule_params_get()->field_0x18;
        *s = (short)f;
      }
      if (msnm_rule10_cpu_check(rob->side) != 0) {
        rob->pending_damage = 0;
      }
      rob->combo[0].hits = rob->combo[0].hits + 1;
      rob_damage_apply(rob, 2);
    }
    if ((~rob->state & 0x400) != 0) {
      if (rob->life > 0) {
        if (rob->followup_word == 0) {
          rob->flags = rob->flags & 0xffffff7f;
          rob->action.word = 0;
          break;
        }
        rob->action_step = 3;
        break;
      }
      if (move_event_entry_find(rob, 0x10) != 0) {
        rob->event_flags = rob->event_flags | 0x10;
        rob->action_step = 3;
        break;
      }
      rob->request_pending.word = 0x8000209;
      break;
    }
    rob->combo[2].hits = rob->combo[2].hits + 1;
    rob->combo[2].damage = rob->combo[2].damage + c;
    rob->action_step = 3;
  case 3:
    a = rob->recovery_end;
    if (rob->frame < a) return;
    if ((rob->state & 0x400) != 0) {
      rob->request_pending.word = 0x9000000;
      return;
    }
    if (((uint)rob->followup_word & 0x40000000) != 0) return;
    if (rob->followup_word == -1) return;
    rob->flags = rob->flags & 0xffffff7f;
    rob->action.word = 0;
    break;
  }
}

void action_hit_stun_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_hit_stun_update(rob_rec *rob)
{
  uint m;
  short t;
  float a;
  float b;
  float c;
  
  switch((uchar)rob->action_step) {
  case 0:
    rob->ctrl->continuation = 0;
    rob->ctrl->wait = 0;
    rob->ctrl->mark_saved = -1;
    m = rob->action.part.move & 0x7fff;
    if (m != 0) {
      rob_move_set_alt_facing(rob,(ushort)m);
    }
    a = rob->knockback.x;
    b = rob->knockback.z;
    c = a + a;
    rob->velocity.x = a;
    rob->velocity.z = b;
    rob->slide_velocity.x = c;
    rob->slide_velocity.z = c;
    rob->face_flags = rob->face_flags | 8;
    rob->flags = rob->flags | 0x80;
    rob->action_step = rob->action_step + '\x01';
  case 1:
    {
      short t = rob->active_end - 1;
      if (t > rob->frame) break;
    }
    rob->action_step = rob->action_step + '\x01';
  case 2:
    t = rob->active_end + -1;
    if ((int)t < (int)(short)(rob->recovery_end - rob->stun)) {
      rob->action_step = rob->action_step + '\x01';
      rob->flags = rob->flags & 0xffffff7f;
    }
    else {
      rob->frame = t;
      rob->frame_pos = (float)(int)t;
      break;
    }
  case 3:
    if (rob->stun == 0) {
      rob->action.word = 0;
      rob->state = rob->state & 0xfffffffe;
    }
    break;
  }
}

void action_launch_start(rob_rec *rob)
{
  if (((rob->flags & 8) != 0) && ((rob->request.part.lever & 1) != 0)) {
    rob_airborne_end(rob);
  }
  rob_action_accept(rob);
  return;
}

void action_launch_update(rob_rec *rob)
{
  typedef struct { int w[3]; } vec;
  byte b;
  uint f;
  float v;

  switch ((uchar)rob->action_step) {
  case 0:
    rob->velocity.y = 0.0;
    rob->ctrl->continuation = 0;
    rob->ctrl->wait = 0;
    rob->ctrl->mark_saved = -1;
    b = rob->action.part.lever;
    f = 0;
    if ((rob->state & 0x400) == 0) {
      rob->flags = rob->flags & 0xffffff7f;
    }
    if ((rob->flags24 & 0x20000000) != 0) f |= 1;
    if ((rob->flags24 & 8) != 0) f |= 2;
    if ((rob->flags24 & 0x40) == 0) {
      if ((rob->state & 0x2440) == 0) {
        v = rob->floor_y + launch_floor_offsets[rob->chara];
        if (v > rob->pos.y) rob->pos.y = v;
      }
      else if ((rob->state & 0x2000) != 0 && (rob->move_flags & 0x4000) != 0) {
        v = rob->floor_y + launch_floor_offsets[rob->chara];
        if (v > rob->pos.y) rob->pos.y = v;
      }
    }
    rob_move_set_alt_facing(rob,rob->action.part.move & 0x7fff);
    rob->flags = rob->flags | 0x80;
    rob->face_flags = 8;
    if ((b & 2) != 0) {
      rob->action_step = 4;
      break;
    }
    if ((f & 1) != 0) {
      rob->flags24 = rob->flags24 | 0x20000000;
    }
    if ((f & 2) != 0) {
      rob->flags24 = rob->flags24 | 8;
    }
    if ((b & 1) != 0) {
      vec z = {{0, 0, 0}};
      *(vec *)&rob->velocity = z;
      rob->action_step = 3;
      rob_airborne_end(rob);
      break;
    }
    if (rob->field_0x2ee == -1) {
      rob->action_step = 4;
      break;
    }
    if ((rob->action.part.lever & 8) != 0) {
      rob_airborne_begin(rob);
      rob->velocity.x = 0.0;
      rob->velocity.y = world_gravity;
      rob->velocity.z = 0.0;
      rob->action_step = 5;
      break;
    }
    rob->action_step = rob->action_step + 1;
  case 1:
    if (rob->frame >= rob->field_0x2ee) {
      rob_airborne_begin(rob);
      if ((rob->flags & 0x40000) != 0) {
        rob->velocity = rob->push_velocity;
      }
      else {
        rob->velocity.x = rob->knockback.x;
        rob->velocity.z = rob->knockback.z;
        if ((rob->flags & 0x10000) != 0 ||
            !(rob->velocity.y < rob->knockback.y)) {
          rob->velocity.y = rob->knockback.y;
        }
      }
      rob->action_step = rob->action_step + 1;
    }
    break;
  case 2:
    if ((rob->flags & 8) == 0) {
      rob->frame = rob->field_0x2f2;
      rob->frame_pos = rob->field_0x2f2;
      attack_result_sounds_play(rob,rob,0x200);
      fish_seeker_rob_landed(0,rob);
      fish_school_rob_landed(1,rob);
      sky_rob_landed();
      break_floor_rob_landed(rob);
      camera_shake_request(1,1);
      rob->action_step = 4;
    }
    break;
  case 3:
    if (rob->frame >= rob->field_0x2f2) {
      break_floor_rob_landed(rob);
      rob->action_step = rob->action_step + 1;
    }
    break;
  case 4:
    {
      uint m = rob->flags24;
      if ((m & 0x20000008) == 0x20000008) {
        rob->flags24 = m & 0xfffffff7;
      }
    }
    if (rob->frame >= rob->recovery_end) {
      rob->field_0x220 = 0;
      if ((rob->state & 0x40000) != 0) {
        rob->flags = rob->flags & 0xffffff7f;
        rob->action.word = 0;
      }
      else {
        rob->request_pending.word = 0x9000000;
      }
    }
    break;
  case 5:
    if (rob->frame < rob->field_0x2ee) {
      rob->velocity.y = world_gravity;
      break;
    }
    rob->field_0x100 = 0;
    rob->action_step = rob->action_step + 1;
  case 6:
    if (rob->field_0x100 < launch_hold_frames) {
      rob->velocity.y = world_gravity;
    }
    else {
      if ((rob->flags & 0x40000) != 0) {
        rob->velocity = rob->push_velocity;
      }
      else {
        rob->velocity = rob->knockback;
      }
      rob->action_step = 2;
    }
    break;
  }
}

void action_down_start(rob_rec *rob)
{
  short idx;
  
  if ((rob->request.part.lever & 2) != 0) {
    idx = rob->request.part.move & 0x7fff;
    if ((rob->relation_flags & 1) != 0) {
      idx = idx + 1;
    }
    rob->action_step = 0;
    rob->action.word = rob->move_table[idx];
    if ((rob->opp->old_cur->state & 0x21040e00) == 0) {
      hit_wait_timer_set(0);
    }
  }
  else if (rob->action.part.type != '\t') {
    rob->action_step = 0;
    rob->action.word = rob->request.word;
  }
  else if ((rob->request.part.lever & 1) != 0) {
    rob->action.word = rob->request.word;
    if ((rob->action_step == '\x04') && ((rob->action.part.lever & 4) != 0)) {
      rob->action_step = '\x03';
    }
  }
}

void action_down_update(rob_rec *rob)
{
  short *q;
  short v;
  uint f;
  int b;
  short s;
  short n;
  short k;
  short m;
  uint u;
  short t;
  int r;
  uint fl;

  if (rob->action_step != 4 && (rob->side_flags & 4) != 0) {
    if ((rob->flags & 0x200) != 0) q = (short *)&rob->move_table[0x31];
    else q = (short *)&rob->move_table[0x30];
    v = *q;
    if (rob->move_no != v) rob_move_set_alt_facing(rob,(ushort)v);
  }
  f = rob->state;
  switch ((uchar)rob->action_step) {
  case 0:
    f |= 0x1c00;
    rob->field_0x220 = 0;
    rob->state = f;
    rob->ctrl->continuation = 0;
    rob->ctrl->wait = 0;
    rob->ctrl->mark_saved = -1;
    rob->action_step = 1;
  case 1:
    rob->press_count = 0;
    if ((rob->flags & 0x500) != 0) {
      rob->action_step = 5;
      return;
    }
    rob->action_step = 2;
  case 2:
    n = rob->stun;
    k = rob->press_count;
    if (n > 0 && n > k * 2) return;
    rob->field_0x100 = 0;
    rob->stun = 0;
    rob->flags &= ~0x80;
    rob->action_step = 3;
  case 3:
    if (rob->action.part.lever == 0) {
      if (rob->field_0x100 < 40) return;
      fl = rob->flags;
      m = (fl & 0x200) ? 51 : 50;
    }
    else {
      b = 0;
      m = rob->action.part.move & 0x7fff;
      switch (m) {
      case 58: case 59:
        b = 1;
      case 56: case 57:
        t = rob->opp_dir;
        if ((rob->relation_flags & 1) != 0) t = t + -0x8000;
        t = camera.yaw + t;
        if (t < 0) b ^= 1;
        if ((rob->side_flags & 8) != 0) b ^= 1;
        if (b) m = 58;
        else m = 56;
        break;
      case 0x17f:
        rob_move_set(rob,m);
        goto done;
      case 89: case 93:
        if ((rob->relation_flags & 1) != 0) m = m + 2;
        break;
      }
      fl = rob->flags;
      if ((fl & 0x200) != 0) m = m + 1;
    }
    rob_move_set(rob,(rob->move_table)[m]);
done:
    rob->flags &= 0xffdfffff;
    rob->field_0x223 = 0;
    rob->combo[2].hits = 0;
    rob->combo[0].hits = 0;
    rob->combo[0].damage = 0;
    rob->action_step = 4;
    s = rob->recovery_end;
    r = hit_wait_timer_get();
    if (r < s) r = s;
    if ((rob->move_flags & 0x4000) != 0) r = rob->active_end;
    hit_wait_timer_set(r);
    return;
  case 4:
    if ((rob->state & 1) != 0 && rob->frame < rob->recovery_end) return;
    if (rob->followup_word != 0) return;
    f &= ~1;
    rob->state = f;
    rob->action.word = 0;
    if ((rob->ctrl->buttons & 1) == 0) return;
    if ((int)rob->field_0x33c != 0) return;
    rob->field_0x33c = 0x80000003;
    return;
  case 5:
  default:
    return;
  }
}

void action_12_start(rob_rec *rob)
{
  if (rob->action.part.type != '\f') {
    rob->ctrl->flags = rob->ctrl->flags | 0x20;
    rob_action_accept(rob);
    {
    control_rec *ctrl = rob->ctrl;
    ctrl->continuation = 0;
    ctrl->pending_script = 0;
    ctrl->branch_pending = 0;
    ctrl->branch_kind = 0;
    ctrl->wait = 0;
    ctrl->branch_wait = 0;
    ctrl->buffer_script = 0;
    ctrl->buffer_script_2 = 0;
    ctrl->buffer_kind = 0;
    ctrl->buffer_kind_2 = 0;
    ctrl->field_0x70 = 0;
    ctrl->mark_saved = -1;
    }
    if ((rob->state & 0x300000) != 0) {
    control_rec *ctrl = rob->opp->ctrl;
      ctrl->continuation = 0;
      ctrl->pending_script = 0;
      ctrl->branch_pending = 0;
      ctrl->branch_kind = 0;
      ctrl->wait = 0;
      ctrl->branch_wait = 0;
      ctrl->buffer_script = 0;
      ctrl->buffer_script_2 = 0;
      ctrl->buffer_kind = 0;
      ctrl->buffer_kind_2 = 0;
      ctrl->field_0x70 = 0;
      ctrl->mark_saved = -1;
    }
  }
  return;
}

void action_12_update(rob_rec *rob)
{
  rob_old_rec *old;
  rob_rec *opp;
  float *v;
  int *base;
  float *f;
  float tmp[3];

  if (rob->action.part.lever & 1) {
    switch ((uchar)rob->action_step) {
    case 0:
      rob_airborne_end(rob);
      base = (int *)&rob->bones;
      rob_move_set(rob, rob->action.part.move & 0x7fff);
      rob->flags = rob->flags | 0x80;
      rob->velocity.y = 0.0;
      ((control_rec *)base[2])->continuation = 0;
      ((control_rec *)base[2])->wait = 0;
      ((control_rec *)base[2])->mark_saved = -1;
      rob->face_flags = 8;
      if ((rob->state & 0x80) == 0) {
        rob->action_step = rob->action_step + 3;
        break;
      }
      rob->action_step = rob->action_step + 1;
      break;
    case 1:
      if (rob->frame >= rob->field_0x2ee) {
        rob->velocity = rob->push_velocity;
        rob_airborne_begin(rob);
        rob->action_step = rob->action_step + 1;
      }
      break;
    case 2:
      if (rob->flags & 8) {
        return;
      }
      if (rob->state & 0x400) {
        attack_result_sounds_play(rob, rob, 0x200);
        fish_seeker_rob_landed(0, rob);
        fish_school_rob_landed(1, rob);
        sky_rob_landed();
        break_floor_rob_landed(rob);
        camera_shake_request(1, 1);
      }
      rob->frame = rob->field_0x2f2;
      rob->frame_pos = (float)rob->field_0x2f2;
      rob->action_step = rob->action_step + 1;
      break;
    case 3:
      break;
    }
  }
  else if (rob->action.part.lever & 2) {
    switch ((uchar)rob->action_step) {
    case 0:
      if ((ushort)(rob->move_no - 0x8b5U) < 2 &&
          (opp = rob->opp, old = opp->old_cur, (old->flags & 8) != 0)) {
        rob->velocity = old->velocity;
      }
      else {
        if (!(vec3_length(&rob->velocity.x) > 0.0)) {
          if ((rob->old_cur->flags & 8) == 0 && vec3_length(&rob->velocity.x) < 1.1920929e-07) {
            vec3_sub(&rob->velocity.x, &rob->pos.x, &rob->old_cur->pos_prev.x);
          }
          else {
            vec3_set_length(&rob->velocity.x, &rob->pos.x, 0.03);
          }
        }
      }
      f = (float *)move_cmd_arg_find((short)rob->move_no, 0, 0x28);
      if (f != 0) {
        vec3_set_length(tmp, &rob->velocity.x, *f);

        vec3_add_to(&rob->velocity.x, tmp);
        rob->velocity.y = world_gravity * f[1];
      }
      if (rob->velocity.y > 0.0) {
        rob->velocity.y = 0.0;
      }
      rob_airborne_begin(rob);
      if (rob->field_0x2f2 == rob->move_length) {
        rob->field_0x2f2 = rob->frame + 1;
      }
      rob->action_step = rob->action_step + 1;
    case 1:
      if (rob->flags & 8) {
        if ((rob->flags24 & 0x20000) && rob->frame < rob->recovery_end) {
          return;
        }
        if (rob->state & 0x80) {
          return;
        }
        rob->frame_pos = rob->frame_pos - rob->frame_speed;
        rob->frame = (int)rob->frame_pos;
        return;
      }
      rob->action_step = rob->action_step + 1;
    case 2:
      if (rob->frame >= rob->move_length) {
        rob->action.word = 0;
      }
      break;
    }
  }
  else if ((rob->flags & 8) && (rob->state & 0x400)) {
    action_neutral_update(rob);
  }
}

void action_stun_followup_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_stun_followup_update(rob_rec *rob)
{
  short m;
  short v;
  short u;
  char b;
  float f;
  float g;
  float d;
  
  switch ((uchar)rob->action_step) {
  case 0:
    rob->ctrl->continuation = 0;
    rob->ctrl->wait = 0;
    rob->ctrl->mark_saved = -1;
    m = rob->action.part.move & 0x7fff;
    if (m != 0) {
      rob_move_set_alt_facing(rob,(ushort)m);
    }
    f = rob->knockback.x;
    g = rob->knockback.z;
    d = f + f;
    rob->velocity.x = f;
    rob->velocity.z = g;
    rob->slide_velocity.x = d;
    rob->slide_velocity.z = d;
    rob->face_flags = rob->face_flags | 8;
    rob->flags = rob->flags | 0x80;
    rob->action_step = rob->action_step + 1;
  case 1:
    {
      short w = rob->active_end - 1;
      if (rob->frame < w) break;
    }
    rob->action_step = rob->action_step + 1;
  case 2:
    v = rob->active_end - 1;
    if (v < (short)(rob->recovery_end - rob->stun)) {
      rob->action_step = rob->action_step + 1;
      rob->flags = rob->flags & 0xffffff7f;
    }
    else {
      rob->frame = v;
      rob->frame_pos = (float)v;
      break;
    }
  case 3:
    b = rob->followup_flags;
    if (b != 0) {
      u = rob->followup_move;
      if (u != 0) {
        if (rob->stun == 0 || (b & 1) == 0) {
          if (rob->followup_count >= rob->field_0x290) {
            if (rob->frame >= rob->followup_frame) {
              rob_move_set(rob,(ushort)u);
              hit_wait_timer_set((short)(rob->recovery_end + 3));
              rob->action.word = u;
              return;
            }
          }
        }
      }
    }
    if (rob->stun == 0 && rob->frame >= rob->recovery_end) {
      rob->action.word = 0;
      rob->state = rob->state & 0xfffffffe;
    }
    break;
  }
}

void action_14_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_14_update(rob_rec *rob)
{
  char preset;
  int preset_move;

  switch ((int)(uchar)rob->action_step) {
  case 0:
    preset = rob->preset_throw;
    preset_move = rob->preset_throw_move;
    rob_move_set(rob,(int)rob->action.part.move & 0x7fff);
    rob->preset_throw = preset;
    rob->preset_throw_move = preset_move;
    rob->side_flags = rob->side_flags | 0x2000;
    rob->action_step = rob->action_step + '\x01';
  case 1:
    rob->action_step = rob->action_step + '\x01';
    break;
  case 2:
    if (rob->frame >= rob->recovery_end) {
      rob->action.word = 0;
    }
    break;
  }
}

void action_15_start(rob_rec *rob)
{
  uchar v;
  short s;
  uint *b;
  
  v = rob->request.part.lever;
  if ((v & 1) != 0) {
    b = (uint *)((char *)rob + 0x40);
    s = rob->request.part.move;
    rob->action.part.type = 0xf;
    rob->action.part.lever = v;
    { uint m = 0x7fff; m &= s; b[8] = (b[8] & 0xffff8000) | m; }
    rob->action_step = 0;
    rob->field_0x220 = 0;
  }
  else if (rob->action.part.type != '\x0f') {
    rob_action_accept(rob);
  }
}

void action_15_update(rob_rec *rob)
{
  uint v;
  uint s;
  char c;
  short t;
  int a;
  int b;
  int u;
  int flag;
  char *p;
  v = rob->action.word;
  switch ((uchar)rob->action_step) {
  case 0:
    if ((rob->flags & 0x100) != 0 || (p = (char *)rob->coli)[0x91] == 0) {
      rob->action_step = 2;
      return;
    }
    if ((v & 0x10000) == 0) {
      return;
    }
    flag = 0;
    rob->stun = flag;
    s = (ushort)rob->opp_dir;
    if ((rob->relation_flags & 1) != 0) {
      s = (ushort)(s + 0x8000);
    }
    rob->heading = s;
    rob->state = rob->state & 0xfffffeff;
    s = (ushort)(camera.yaw + s);
    if (s > 0x7fff) {
      flag = 1;
    }
    if ((v & 0x20000) != 0) {
      if ((rob->flags & 8) == 0) {
        rob_airborne_begin(rob);
      }
      rob_move_set(rob,v & 0x7fff);
      rob->flags = rob->flags & 0xffffff7f;
      rob->velocity.y = world_gravity * 10.0 + rob->velocity.y;
      rob->action_step = 1;
    }
    else {
      if ((rob->flags & 8) != 0) {
        return;
      }
      u = rob->action.part.move & 0x7fff;
      b = 0;
      a = 0;
      if (u > 2) {
        c = u - 3;
        if (flag) {
          c ^= 1;
        }
        if (c != 0) {
          u = 1;
        }
        else {
          u = 3;
        }
      }
      else if (u != 0) {
        c = u - 1;
        if (flag) {
          c ^= 1;
        }
        if ((rob->side_flags & 8) != 0) {
          c ^= 1;
        }
        if ((c & 1) != 0) {
          u = 4;
        }
        else {
          u = 2;
        }
      }
      else {
        u = 0;
      }
      if (rob->combo[0].damage > 0x3c) {
        a = 1;
      }
      if ((rob->state & 0x800000) != 0) {
        b = 1;
      }
      u = action15_motion_table[rob->field_0x28f][a][b][u];
      rob_move_set(rob,(ushort)u);
      rob->flags = rob->flags & 0xffdfff7f;
      rob->action_step = 2;
    }
    t = rob->recovery_end + 2;
    if (t > hit_wait_timer_get()) {
      hit_wait_timer_set(t);
    }
    return;
  case 1:
    if ((rob->flags & 8) != 0) {
      return;
    }
    rob->action_step = 2;
  case 2:
    if ((rob->side_flags & 2) != 0 || (rob->state & 1) == 0) {
      rob->action.word = 0;
    }
  }
}

void action_18_start(rob_rec *rob)
{
  rob_action_accept(rob);
  return;
}

void action_18_update(rob_rec *rob)
{
  char *step;
  
  step = &rob->action_step;
  if (*step == '\0') {
    rob_move_set(rob,(int)rob->action.part.move & 0x7fff);
    rob->field_0x11a = 1;
    *step = *step + '\x01';
  }
  return;
}

void action_19_start(rob_rec *rob)
{
  rob->action.word = 0;
  rob->action_step = 0;
  rob->field_0x100 = 0;
  rob_move_set(rob,(int)rob->request.part.move & 0x7fff);
  return;
}

void action_19_update(rob_rec *rob)
{
  rob->action.word = 0;
  return;
}

void action_20_start(rob_rec *rob)
{
  byte b;
  int v;
  
  b = rob->request.part.lever;
  if ((rob->request.word & 0x7fff) != 0) {
    v = (int)rob->request.part.move & 0x7fff;
  }
  else {
    v = (int)(short)rob->move_table[0x55];
  }
  if ((b & 1) != 0) {
    v = (int)(short)rob->move_table[0x52];
  }
  else if ((b & 2) != 0) {
    v = (int)(short)rob->move_table[0x53];
  }
  else if ((b & 4) != 0) {
    v = (int)(short)rob->move_table[0x54];
  }
  rob_move_set(rob,v & 0xffff);
  rob->action.word = v;
  rob->action_step = 0;
  rob->field_0x100 = 0;
  rob->flags = rob->flags & 0xffffff7f;
}

void action_20_update(rob_rec *rob)
{
  rob->action.word = 0;
  return;
}

void action_27_start(rob_rec *rob)
{
  typedef struct { unsigned int move : 15; unsigned int rest : 17; } act_word;
  uchar f = rob->request.part.lever;
  act_word w;
  int m = rob->request.part.move & 0x7fff;

  if (f & 2) {
    if (rob->state & 0x1000) return;
    m = (short)rob->move_table[0x57];
  }
  else if (f & 1) {
    m = (short)rob->move_table[0x56];
  }
  w = *(act_word *)&rob->request;
  w.move = m;
  *(act_word *)&rob->action = w;
  rob->action_step = 0;
}

void action_27_update(rob_rec *rob)
{
  char *step;
  
  step = &rob->action_step;
  if (*step == '\0') {
    rob_move_set(rob,(int)rob->action.part.move & 0x7fff);
    *step = *step + '\x01';
  }
  return;
}

void rob_action_accept(rob_rec *rob)
{
  rob->action.word = rob->request.word;
  rob->action_step = 0;
  return;
}

void reaction_entry_flag_update(rob_rec *rob)
{
  char a = rob->request.part.type;
  char b = rob->action.part.type;

  switch(a) {
  case 6:
  case 7:
  case 8:
  case 0xd:
  case 0x11:
  case 0x16:
  case 0x18:
  case 0x1a:
    switch(b) {
    case 6:
    case 7:
    case 8:
    case 0xd:
    case 0x11:
    case 0x16:
    case 0x18:
    case 0x1a:
      return;
    default:
      if (rob->field_0x106 != 0) {
        return;
      }
      rob->flags = rob->flags | 0x10000;
    }
    break;
  default:
    rob->flags = rob->flags & 0xfffeffff;
  }
}

void motion_switch_keep_phase(rob_rec *rob, ushort move, char reverse)
{
  float s;
  float pos;
  
  if (rob->move_no != move) {
    if ((rob->move_flags & 0x4000) != 0) {
      rob_move_set(rob,move);
    }
    else {
      s = (float)rob->move_length;
      pos = rob->frame_pos;
      if (reverse != '\0') {
        pos = s - pos;
      }
      if (pos < 1.0) {
        pos = 1.0;
      }
      rob_move_set(rob,move);
      rob->frame_pos = ((float)rob->move_length * pos) / s;
      rob->frame = (short)(int)rob->frame_pos;
    }
  }
}

void guard_frame_catch_up(rob_rec *rob)
{
  rob_old_rec *old = rob->opp->old_cur;
  int d, t, c;
  if (((((old->flags28 & 0x40000000) == 0) && ((rob->flags & 0x10) == 0))
      && ((old->attack_level != '\0' ||
          ((user_back_data.option_bits & 0x80000) != 0)))) &&
     ((old = rob->opp->old_cur, (old->move_flags & 0x4000) != 0
      && ((old->state & 0x4002) != 0)))) {
    d = (short)(old->active_start - old->frame);
    t = rob->move_length;
    c = rob->frame;
    if (c < t) {
      if (0 < d) {
        t = (short)((short)(t - c) / d + c);
      }
      rob->frame = t;
      rob->frame_pos = (float)t;
    }
  }
}

int throw_position_follow(rob_rec *rob, rob_rec *opp)
{
  float v[3];
  int ret = 0;
  float f;

  if (rob->state & 2) {
    if ((rob->action.word & 0x10000) && (rob->flags2c & 0x100) == 0) {
      vec3_sub(&rob->throw_offset.x, &rob->opp->old_cur->place_pos.x, &rob->place_pos.x);
      rob->heading = rob->opp->old_cur->heading;
    } else {
      v[2] = 0;
      v[1] = 0;
      v[0] = 0;
      if (rob->flags2c & 0x100) {
        throw_offset_compute(rob, opp, v);
      } else {
        vec3_sub(v, &rob->opp->old_cur->place_pos.x, &rob->place_pos.x);
      }
      f = (float)(rob->active_start - rob->frame + 1);
      if (f < 1.0) {
        f = 1.0;
      }
      f = rob->frame_speed / f;
      vec3_scale(&rob->throw_offset.x, v, f);
      rob->heading = rob->opp->old_cur->heading;
    }
  } else {
    if (rob->flags2c & 0x100) {
      throw_offset_compute(rob, rob->opp, &rob->throw_offset.x);
    } else {
      vec3_sub(&rob->throw_offset.x, &rob->opp->old_cur->place_pos.x, &rob->place_pos.x);
    }
    rob->heading = rob->opp->old_cur->heading;
    ret = 1;
  }
  return ret;
}

void throw_offset_compute(rob_rec *rob, rob_rec *opp, float *out)
{
  float a[3];
  float b[3];
  float *bp = b;
  rob_base_pos_get(rob,1.0,a);
  rob_base_pos_get(opp,1.0,bp);
  vec3_sub_from(a,bp);
  vec3_rot_y_s16((int)opp->old_cur->heading,a,a);
  vec3_add_to(a,&opp->old_cur->place_pos.x);
  vec3_sub_from(a,&rob->old_cur->place_pos.x);
  out[0] = a[0];
  out[2] = a[2];
  out[1] = opp->old_cur->place_pos.y - rob->place_pos.y;
}
