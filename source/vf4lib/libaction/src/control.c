/*
 * vf4lib/libaction/src/control.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_script_peek_move
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint game_flags; /* 0x0c2925b8 */
extern uint control_dir_moves[16]; /* 0x0c29f638 */
extern uint control_buffer_kind_masks[8]; /* 0x0c29f9b0 */
extern char control_objects[2][156]; /* 0x0c37b8f4 */
extern char control_history[2][64][40]; /* 0x0c42071c */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern script_choice script_choice_stack; /* 0x0c421b54 */

void control_task_start(void)
{
  rob_rec *rob;
  uint side;

  side = 0;
  do {
    rob = rob_get(side & 0xff);
    control_init(rob);
    side = side + 1;
  } while ((int)side < 2);
  input_log_clear();
  return;
}

void control_init(rob_rec *rob)
{
  control_rec *ctrl = (control_rec *)(control_objects[rob->side]);
  int z = 0;
  char *side;
  uchar *kind;
  control_history_rec *entry;
  int n;
  int j;

  rob->ctrl = ctrl;
  ctrl->rob = rob;
  ctrl->flags = z;
  side = &ctrl->side;
  *side = rob->side;
  ctrl->scan_pos = z;
  ctrl->history_idx = z;
  ctrl->releases = z;
  ctrl->presses = z;
  ctrl->dirs = z;
  ctrl->buttons = z;
  ctrl->prev_buttons = z;
  ctrl->prev_presses = z;
  ctrl->rob_flags = z;
  ctrl->dir_bits = z;
  ctrl->dir_frames = z;
  ctrl->wait = z;
  ctrl->branch_wait = z;
  kind = &ctrl->branch_kind;
  *kind = z;
  ctrl->continuation = 0;
  ctrl->field_0x70 = 0;
  ctrl->branch_pending = 0;
  ctrl->dir_branch_list = 0;
  ctrl->dir_branch_list_done = 0;
  ctrl->mark = 1;
  ctrl->mark_keep = 1;
  ctrl->mark_saved = -1;
  control_slot_marks_reset(ctrl);
  control_buffer_clear(*side);
  control_resume_clear(*side);
  control_pending_clear(*side);
  entry = &((control_history_rec *)&control_history[0][0][0])[*side * 0x40];
  ctrl->history = entry;
  n = 0x40;
  do {
    entry->rob_flags = z;
    entry->prev_buttons = z;
    entry->releases = z;
    entry->dirs = z;
    entry->presses = z;
    entry->buttons = z;
    entry->flag_0x80 = z;
    for (j = 0; j < 11; j++) entry->counters[j] = z;
    entry++;
    n--;
  } while (n != 0);
  return;
}

void control_task_frame(void)
{
  int held;
  rob_rec *rob;
  uint side;

  held = pause_frame_held();
  if (held == 0) {
    side = 0;
    do {
      rob = rob_get(side);
      if ((rob->flags & 1) == 0) {
        rob->request.word = 0;
      }
      else if (!((rob->flags & 0x400) == 0)) {
        rob->request.word = 0;
      }
      else {
        control_update(rob->ctrl);
      }
      side = side + 1 & 0xff;
    } while (side < 2);
  }
  input_mute_off();
  return;
}

control_rec * control_object_get(int side)
{
  return (control_rec *)control_objects[side];
}

void control_update(control_rec *ctrl)
{
  input_sample(ctrl);
  control_context_update(ctrl);
  control_command_step(ctrl);
  return;
}

void input_sample(control_rec *ctrl)
{
  short idx;
  control_history_rec *entry;
  short t;
  ushort *pad;
  ushort btn;
  uint press;
  uint last;
  uint rel;
  int dir;
  ushort old;
  int x;
  rob_rec *rob;
  int mark;
  ushort flags;
  ushort prev_flags;
  ushort held;
  ushort prev_held;
  int i;
  ushort *counter;
  uint *combo;
  ushort counters[11];

  idx = ctrl->history_idx;
  entry = ctrl->history;
  if (ctrl->rob->flags & 0x20) {
    input_forward_mask = 0x20;
    input_back_mask = 0x10;
    input_bit10_mask = 0x40;
    input_bit11_mask = 0x80;
  }
  else {
    input_forward_mask = 0x10;
    input_back_mask = 0x20;
    input_bit10_mask = 0x80;
    input_bit11_mask = 0x40;
  }
  if (ctrl->rob->opp->side_flags & 8) {
    input_bit12_mask = input_bit11_mask;
    input_bit13_mask = input_bit10_mask;
  }
  else {
    input_bit12_mask = input_bit10_mask;
    input_bit13_mask = input_bit11_mask;
  }
  if (ctrl->rob->side_flags & 8) {
    t = input_bit10_mask;
    input_bit10_mask = input_bit11_mask;
    input_bit11_mask = t;
  }
  if (ctrl->rob->opp->state & 0x100000) {
    if (ctrl->rob->opp->flags28 & 0x80000)
      input_toward_mask = 0x10;
    else if (ctrl->rob->opp->flags28 & 0x40000)
      input_toward_mask = 0x20;
    else
      input_toward_mask = 0;
  }
  pad = &player_buttons[ctrl->side];
  if (ctrl->rob->flags & 4)
    btn = *pad;
  else
    btn = ctrl->cpu_buttons;
  if (ctrl->flags & 0x20)
    btn = 0;
  btn &= 0x7fff;
  btn &= ~8;
  if (btn & input_forward_mask) btn |= 0x100;
  if (btn & input_back_mask) btn |= 0x200;
  if (btn & (ushort)input_bit10_mask) btn |= 0x400;
  if (btn & (ushort)input_bit11_mask) btn |= 0x800;
  if (btn & (ushort)input_bit12_mask) btn |= 0x1000;
  if (btn & input_bit13_mask) btn |= 0x2000;
  last = ctrl->buttons;
  press = (ushort)(~last & btn);
  rel = last & ~btn;
  if (input_mute_get()) {
    rel = 0;
    press = 0;
  }
  old = ctrl->dirs;
  x = btn;
  x = !(x & 0x30) ? (ushort)(x | (old & 0x30)) : (ushort)x;
  dir = x;
  dir = !(dir & 0xc0) ? dir | (old & 0xc0) : (ushort)dir;

  entry += idx;
  control_scan_pos = idx;
  sample_mark = ctrl->mark;
  sample_mark_keep = ctrl->mark_keep;
  mark = ctrl->mark_saved;
  idx = (idx + 1) & 63;
  sample_mark = sample_mark == idx ? (idx + 1) & 63 : sample_mark;
  sample_mark_keep = sample_mark_keep == idx ? (idx + 1) & 63 : sample_mark_keep;
  mark = mark == -1 ? -1 : (idx == mark ? -1 : mark);
  flags = 0;
  prev_flags = ctrl->counters[0];
  for (i = 0; i < 11; i++)
    counters[i] = ctrl->counters[i];
  counters[3] = 0;
  held = btn & 0x3c0;
  prev_held = ctrl->counters[3];
  if (held) {
    flags |= 1;
    counters[3] = held;
    if ((((short)prev_flags >> 1 & 1) == 0) & (held == prev_held))
      counters[2]++;
    else
      counters[2] = 1;
    counters[1]++;
  }
  else if (prev_held && (prev_flags & 1)) {
    flags |= 2;
    counters[3] = prev_held;
  }
  else {
    counters[1] = 0;
    counters[2] = 0;
  }
  for (i = 4, counter = &counters[4], combo = input_combo_counters; i < 11; i++, counter++, combo += 3) {
    if ((btn & combo[0]) == combo[0]) {
      flags |= (ushort)combo[1];
      *counter = (prev_flags & combo[2]) ? 1 : *counter + 1;
    }
    else if (prev_flags & combo[1]) {
      flags |= (ushort)combo[2];
    }
    else {
      *counter = 0;
    }
  }
  input_counter_flags = flags;
  counters[0] = flags;
  for (i = 0; i < 11; i++)
    ctrl->counters[i] = entry->counters[i] = counters[i];
  input_presses = input_new_presses(ctrl, press, btn);
  ctrl->prev_buttons = entry->prev_buttons = last;
  ctrl->prev_presses = ctrl->presses;
  ctrl->releases = entry->releases = input_releases = rel;
  ctrl->buttons = entry->buttons = input_buttons = btn;
  ctrl->presses = entry->presses = input_presses;
  ctrl->dirs = entry->dirs = input_dirs = dir;
  entry->flag_0x80 = (ctrl->flags & 0x80) >> 7;
  ctrl->rob_flags = entry->rob_flags = input_rob_flags = ctrl->rob->flags;
  ctrl->history_idx = idx;
  ctrl->mark = sample_mark;
  ctrl->mark_keep = sample_mark_keep;
  ctrl->mark_saved = mark;
  input_log_push(ctrl->side, (input_presses & 7) | (btn & 0xf0), (input_presses & 7) | (btn & 0x3c0), press, btn);
  input_log_extra_set(ctrl->side, ctrl->scan_pos);
  rob = ctrl->rob;
  if (rob->guard_input_hold) {
    rob->guard_input_hold--;
    rob->guard_input_time++;
    if (rel & 1)
      rob->guard_input_time += guard_input_time_idle;
  }
  else if (press & 1) {
    rob->guard_input_hold = guard_input_hold_start;
    rob->guard_input_time = 0;
  }
  else {
    rob->guard_input_hold = 0;
    rob->guard_input_time = guard_input_time_idle;
  }
}

ushort input_new_presses(control_rec *ctrl, int presses, uint buttons)
{
  ushort cur;
  ushort prev;
  ushort dir;
  ushort old;
  char cnt;
  ushort m;

  m = 0xf;
  cur = presses;
  prev = buttons;
  dir = cur & m;
  cnt = (char)ctrl->dir_frames;
  ctrl->flags &= ~0x80;
  if (cnt == 0) {
    if ((dir == 0xf | dir == 0) || dir == 7) {
      ctrl->dir_bits = 0;
      ctrl->dir_branch_list = 0;
      ctrl->dir_branch_list_done = 0;
    }
    else {
      ctrl->dir_frames = 1;
      ctrl->dir_bits = dir;
      cur &= ~m;
      ctrl->dir_branch_list_done = 0;
      ctrl->dir_branch_list = ctrl->rob->branch_list;
    }
    ctrl->dir_scan_pos = ctrl->scan_pos;
  }
  else {
    old = ctrl->dir_bits;
    cur |= old;
    cnt = cnt - 1;
    if (cnt != 0) {
      ctrl->dir_frames = cnt;
      ctrl->dir_bits = cur;
    }
    else {
      ctrl->dir_frames = ctrl->dir_bits = 0;
      ctrl->dir_branch_list_done = ctrl->dir_branch_list;
      ctrl->dir_branch_list = 0;
      if ((prev & 0xf) == 4 & old == 5) {
        ctrl->flags |= 0x80;
      }
      else if ((prev & 0xf) == 0 & old == 1) {
        ctrl->flags |= 0x80;
      }
    }
  }
  return cur;
}

void control_context_update(control_rec *ctrl)
{
  char c;
  uchar v;
  uint w;
  uint s;

  s = ctrl->flags;
  w = ctrl->rob->state;
  c = ctrl->rob->action.part.type;
  if ((s & 8) != 0) {
    s = s | 0x10;
    s = s & 0xfffffff7;
  }
  else {
    s = s & 0xffffffef;
  }
  s = s & 0xffffffbd;
  if (((w & 1) != 0) || ((ctrl->rob->flags & 0x80) != 0)) {
    s = s | 8;
  }
  control_script = ctrl->continuation;
  if (((ctrl->rob->state & 0x10000) == 0) &&
     ((c == '\x0f' || (ctrl->rob->field_0x118 != '\0')))) {
    v = 6;
  }
  else if (((c == '\t') && ((uchar)ctrl->rob->action_step < 4)) || ((w & 0x800) != 0)) {
    v = 1;
  }
  else if ((c == '\x03') && ((uchar)ctrl->rob->action_step <= 1)) {
    v = 2;
  }
  else if (c == '\r') {
    v = 5;
  }
  else if (c == '\x1b') {
    v = 7;
  }
  else if ((ctrl->rob->flags24 & 1) != 0) {
    v = 3;
  }
  else if ((ctrl->rob->flags24 & 2) != 0) {
    v = 4;
  }
  else {
    v = 0;
    if (((~s & 8) != 0) && ((control_script == 0 || (ctrl->wait == 0)))) {
      control_script = ctrl->control_sets[ctrl->rob->stance]->scripts[0];
      ctrl->continuation = 0;
    }
  }
  ctrl->context_prev = ctrl->context;
  ctrl->context = v;
  ctrl->flags = s;
  control_slot_marks_reset(ctrl);
  return;
}

void control_command_step(control_rec *ctrl)
{
  rob_rec *rob;
  rob_rec *fighter;
  uchar *saved;
  short s;
  short t;
  short v;
  uint r;
  uchar *x;

  rob = ctrl->rob;
  if ((rob->state & 0x80000) != 0 && rob->followup_flags != 0 && input_presses != 0) {
    if ((rob->followup_flags & 2) != 0) {
      short lim = rob->recovery_end - rob->frame - 3;
      if (lim > (v = rob->stun)) {
        v = ctrl->history_idx;
        ctrl->scan_pos = v;
        ctrl->mark = v;
        ctrl->mark_keep = v;
        rob->request.word = 0xb000000;
        return;
      }
    }
    else {
      v = ctrl->history_idx;
      ctrl->scan_pos = v;
      ctrl->mark = v;
      ctrl->mark_keep = v;
      rob->request.word = 0xb000000;
      return;
    }
  }
  fighter = ctrl->rob;
  if (fighter->field_0x274 != 0) {
    ctrl->state = fighter->field_0x274;
    ctrl->wait = 0;
    if (fighter->field_0x278 != 0) {
      fighter->request.word = fighter->field_0x278;
    }
    else {
      move_script_run(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[2],0);
    }
    ctrl->rob->field_0x274 = 0;
    ctrl->rob->field_0x278 = 0;
    ctrl->mark = ctrl->mark_keep = ctrl->scan_pos;
    ctrl->mark_saved = -1;
    return;
  }
  saved = control_script;
  if (ctrl->branch_pending != 0) {
    s = ctrl->branch_wait;
    if (s != 0) {
      if (s != 0x7fff) {
        t = ctrl->rob->frame + 1;
        if (t < s && t < ctrl->rob->move_length) {
          return;
        }
        ctrl->branch_wait = 0;
      }
      script_branch_mode = 1;
      move_branch_take(ctrl,ctrl->branch_pending);
      return;
    }
  }
  else {
    if (move_branch_scan(ctrl,(int *)ctrl->dir_branch_list_done,1) != 0) {
      return;
    }
    if (move_branch_scan(ctrl,(int *)ctrl->rob->branch_list,0) != 0) {
      return;
    }
  }
  control_script = saved;
  script_branch_mode = 0;
  x = ctrl->pending_script;
  if (x != 0) {
    if (ctrl->pending_wait > 0) {
      ctrl->pending_wait = ctrl->pending_wait - 1;
      if ((ctrl->pending_kind & 0x10) != 0) {
        r = move_script_exec(ctrl,x);
        switch (r) {
        case 1:
          ctrl->mark = ctrl->scan_pos;
        case 2:
          ctrl->mark_keep = ctrl->scan_pos;
          ctrl->mark_saved = -1;
        }
        ctrl->pending_script = 0;
        ctrl->pending_kind = 0;
        ctrl->pending_wait = 0;
        return;
      }
    }
    else {
      ctrl->pending_script = 0;
      ctrl->pending_kind = 0;
    }
  }
  if (control_guard_press(ctrl) == 0) {
    if (ctrl->continuation != 0 && (s = ctrl->wait) != 0) {
      if (s != 0x7fff) {
        t = ctrl->rob->frame + 1;
        if (t < s && t < ctrl->rob->move_length) {
          return;
        }
        ctrl->wait = 0;
      }
      move_script_run(ctrl,control_script,0);
    }
    else {
      (*control_context_funcs[ctrl->context].handler)(ctrl);
    }
  }
}

void control_scan_from(control_rec *ctrl, int pos)
{
  short v = pos;
  short cur;
  int d;
  cur = ctrl->history_idx;
  control_scan_pos = v;
  d = (cur - v) & 0x3f;
  if ((short)d > 0xc) d = 0xc;
  d = cur - (short)d;
  control_scan_pos = d & 0x3f;
  ctrl->scan_pos = control_scan_pos;
}

int control_scan_pending(control_rec *ctrl)
{
  int boolflag_r = 0;
  if (ctrl->scan_pos != ctrl->history_idx) boolflag_r = 1;
  return boolflag_r;
}

void control_ctx_normal(control_rec *ctrl)
{
  int r;
  uint u;
  int t;
  u = ctrl->flags;
  if ((u & 8) != 0) {
    if (ctrl->pending_script == 0 && control_script == 0 && ctrl->rob->field_0x398 == 0) {
      if ((u & 4) != 0) return;
      control_scan_from(ctrl, ctrl->scan_pos);
      return;
    }
    if (ctrl->rob->field_0x398 != 0 && (u & 0x10) != 0 &&
        (ctrl->rob->side_flags & 0x20) != 0) {
      control_scan_from(ctrl, ctrl->mark);
    }
  }
  else {
    if ((u & 0x14) == 0x10) {
      control_scan_from(ctrl, ctrl->mark);
    }
  }
  control_scan_pos = ctrl->history_idx;
  t = (ctrl->scan_pos + 1) & 0x3f;
  if (control_scan_pos == t) {
    ctrl->scan_pos = ctrl->history_idx;
    r = control_frame_try(ctrl);
  }
  else {
    r = control_history_scan(ctrl);
  }
  if (r == 0) control_dir_request(ctrl);
}

int control_history_scan(control_rec *ctrl)
{
  control_history_rec *e;
  int *b;
  if ((ctrl->flags & 0x20) != 0) return 0;
  do {
    control_scan_pos = ctrl->scan_pos;
    e = &ctrl->history[control_scan_pos];
    input_buttons = e->buttons;
    input_presses = e->presses;
    input_releases = e->releases;
    input_counter_flags = e->counters[0];
    input_rob_flags = e->rob_flags;
    control_scan_pos = (control_scan_pos + 1) & 0x3f;
    if (e->flag_0x80 != 0) {
      ctrl->flags = ctrl->flags | 0x80;
    }
    else {
      ctrl->flags = ctrl->flags & 0xffffff7f;
    }
    b = (int *)ctrl->rob->field_0x398;
    if (b != 0) {
      if (move_branch_scan(ctrl,b,2) != 0) {
        return 1;
      }
      script_branch_mode = 0;
    }
    ctrl->scan_pos = control_scan_pos;
    if (((ctrl->flags & 8) == 0) || (ctrl->pending_script != 0) || (control_script != 0)) {
      if (control_frame_try(ctrl) != 0) return 1;
    }
  } while (control_scan_pending(ctrl) != 0);
  return 0;
}

int control_frame_try(control_rec *ctrl)
{
  short s;

  if ((ctrl->flags & 0x20) != 0) {
    return 0;
  }
  if (((ctrl->flags & 4) != 0) &&
     (((int)ctrl->history_idx - (int)control_scan_pos & 0x3f) < 0xd)) {
    ctrl->flags = ctrl->flags & 0xfffffffb;
  }
  if ((ctrl->rob->action.part.type == '\x03') && ((uchar)ctrl->rob->action_step < 2)) {
    return control_hold_release_check(ctrl);
  }
  s = input_counter_flags;
  if ((s & 0xa2aa) != 0) {
    ctrl->state = (uint)(ushort)s | 0x08000000;
    ctrl->flags = ctrl->flags | 2;
    if (control_script_try(ctrl,control_script) != 0) {
      return 1;
    }
    ctrl->flags = ctrl->flags & 0xfffffffd;
  }
  ctrl->state = input_presses & 0xf;
  if ((ctrl->flags & 0x80) != 0) {
    ctrl->state |= 0x04000000;
  }
  if ((ctrl->state != 0) && (control_script_try(ctrl,control_script) != 0)) {
    return 1;
  }
  if ((((input_buttons & 0x3c0) != 0) && ((input_presses + input_releases & 0x3c0U) != 0)) &&
     ((ctrl->flags & 4) == 0)) {
    ctrl->state = 0;
    if (control_dir_script_run(ctrl,control_script) != 0) {
      return 1;
    }
  }
  return 0;
}

void control_dir_request(control_rec *ctrl)
{
  rob_rec *rob;
  uint r;
  uint v;

  v = 0;
  rob = ctrl->rob;
  if ((ctrl->flags & 8) != 0 || (rob->state & 0x80) != 0) return;
  if (*(int *)(rob->reaction + 0x10) == 0) {
    rob->request.word = 0x1000000;
    return;
  }
  if ((rob->state & 0x20) != 0 && rob->action.part.type == 1) {
    v = 0x1000000;
  }
  else {
    r = 0;
    if (input_buttons & 0x100) r |= 1;
    else if (input_buttons & 0x200) r |= 2;
    if (input_buttons & 0x80) r |= 4;
    else if (input_buttons & 0x40) { r |= 8; v = 0x80000; }
    if ((r & 2) != 0 && (rob->relation_flags & 1) != 0) v = 0x1020000;
    else if ((r & 8) != 0 && ((rob->move_flags | rob->state) & 0x2000) != 0) v = 0x1010000;
    else {
      if (*(int *)(rob->reaction + 0x10) == 2) r &= 3;
      v |= control_dir_moves[r];
    }
  }
  if (rob->frame >= rob->active_end || rob->action.part.type != 2 ||
      (rob->action.part.lever & 0x30) == 0 || (v & 0x1020000) != 0x1000000) {
    if ((game_flags & 4) == 0) v = 0x1000000;
    if (msnm_rule_get() != 9 || msnm_combo_active(ctrl->rob->side) == 0)
      rob->request.word = v;
  }
}

int control_guard_press(control_rec *ctrl)
{
  uint flags;
  short v;
  int taken;
  uchar **list;

  if ((ctrl->rob->state & 0xc00) != 0) return 0;
  if ((ctrl->rob->flags24 & 1) != 0) return 0;
  if (ctrl->rob->action.part.type == '\x1b') return 0;
  v = input_presses;
  if ((v & 1) != 0) {
    if ((v & 6) == 0) {
      flags = ctrl->flags;
      ctrl->flags = flags & 0xfffffeff;
      ctrl->scan_pos = ctrl->history_idx;
      if ((((ctrl->continuation == 0) && ((flags & 8) == 0)) &&
          ((ctrl->rob->state & 0x40) == 0)) ||
         ((ctrl->continuation != 0 && (ctrl->wait == 0)))) {
        ctrl->state = 1;
        taken = control_script_try(ctrl,control_script);
        if (taken != 0) {
          return 1;
        }
      }
      ctrl->mark_keep = ctrl->mark = ctrl->dir_scan_pos;
      ctrl->mark_saved = 0xffff;
      ctrl->continuation = 0;
      ctrl->wait = 0;
      if ((ctrl->branch_kind & 1) != 0) {
        ctrl->branch_pending = 0;
        ctrl->branch_kind = 0;
        ctrl->branch_wait = 0;
      }
      list = (uchar **)ctrl->rob->branch_list;
      if ((list != (uchar **)0x0) && ((*(byte *)*list & 1) != 0)) {
        ctrl->rob->branch_list = 0;
        ctrl->branch_pending = 0;
        ctrl->branch_kind = 0;
        ctrl->branch_wait = 0;
      }
      control_guard_request(ctrl);
      return 1;
    }
    else {
      ctrl->flags = ctrl->flags | 0x100;
      return 0;
    }
  }
  return control_guard_request(ctrl);
}

int control_guard_request(control_rec *ctrl)
{
  byte b;
  uint low;
  uint t;
  uint e;
  uint m;
  int w;
  int one;
  rob_rec *rob;
#define F34 (*(volatile short *)&input_buttons)
#define FINISH(M) \
    if ((rob->state & 0x20) != 0) { \
      return 0; \
    } \
    ctrl->scan_pos = ctrl->history_idx; \
    m = M; \
    ctrl->wait = 0; \
    ctrl->continuation = 0; \
    if ((rob->relation_flags & 1) != 0) { \
      m |= 0x40000; \
    } \
    rob->request.word = m; \
    w = (short)input_presses; \
    if ((w & 1) == 0 && (m & 0x40000) == 0 && (w & 6) != 0 && (game_flags & 4) != 0) { \
      ctrl->state = (w & 6) | 0x1000000; \
      if (control_script_try(ctrl,control_script) != 0) { \
        return 1; \
      } \
    } \
    if ((game_flags & 4) == 0) { \
      rob->request.word = 0x1000000; \
    } \
    return 1;

  one = 1;
  rob = ctrl->rob;
  if ((rob->state & 0x80) != 0) {
    return 0;
  }
  if ((F34 & 1) == 0) {
    return 0;
  }
  if ((F34 & 6) == 0) {
    ctrl->flags = ctrl->flags & 0xfffffeff;
  }
  if (*(int *)(rob->reaction + 8) == 0) {
    return 0;
  }
  if (rob->action.part.type == 2) {
    b = rob->action.part.lever;
    low = b & 7;
    if ((b & 0x10) != 0) {
      if (!(rob->frame >= rob->recovery_end)) {
        if ((rob->state & 0x4000000) != 0 && (F34 & 7) == 1 &&
            (ctrl->prev_buttons & 1) != 0 && (input_presses & 1) == 0) {
          ctrl->scan_pos = ctrl->mark =
              ctrl->mark_keep = ctrl->history_idx;
          ctrl->mark_saved = -1;
          return 1;
        }
        return 0;
      }
    } else if ((b & 0x20) != 0) {
      t = (((char)b >> 3) ^ 1) & 1;
      e = (low == 0);
      if ((e & t) == 0) {
        e = (low == 5);
        if (e & t) {
          if (!(rob->frame >= rob->recovery_end)) {
            return 0;
          }
        } else {
          if (!(rob->frame > 4)) {
            return 0;
          }
        }
      }
    }
  }
  if ((ctrl->flags & 8) != 0) {
    return 0;
  }
  if ((ctrl->flags & 0x100) != 0) {
    return 0;
  }
  if ((rob->state & 0x40480) != 0) {
    return 0;
  }
  if ((F34 & 0x40) != 0) {
    FINISH(0x4010000)
  } else {
    if (one == 0) {
      return 0;
    }
    FINISH(0x4000000)
  }
}

int control_hold_release_check(control_rec *ctrl)
{
  rob_rec *rob;
  uint m;
  ushort b;
  short sb;
  uint t;

  rob = ctrl->rob;
  ctrl->state = input_presses & 0xf;
  if (rob->flags & 0x100) return 0;
  if (ctrl->continuation != 0) return move_script_run(ctrl,ctrl->continuation,1);
  if (ctrl->flags & 8) return 1;
  if (rob->action.part.lever & 8) return 1;
  m = input_presses & 0xf;
  switch ((uchar)rob->action_step) {
  case 2:
    break;
  case 1:
    if (m == 0) {
      sb = input_counter_flags;
      if (sb & 0xa2aa) {
        b = sb;
        if (sb < 0 && ctrl->resume_mask == 7) m = 7;
        else if ((b & 0x800) && ctrl->resume_mask == 5) m = 5;
        else if ((b >> 9 & 1) && ctrl->resume_mask == 3) m = 3;
        else if ((b & 0x2000) && ctrl->resume_mask == 6) m = 6;
        else if ((b & 0x20) && ctrl->resume_mask == 4) m = 4;
        else if ((b & 8) && ctrl->resume_mask == 2) m = 2;
        control_resume_clear((int)ctrl->side);
      }
      if (m == 0) break;
    }
    ctrl->state = m + 0x40000000;
    control_script_try(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[1]);
    ctrl->mark = ctrl->mark_keep = ctrl->scan_pos;
    ctrl->mark_saved = -1;
    return 1;
  default:
    t = ctrl->resume_move;
    if (t != 0 && (input_buttons & ctrl->resume_mask) != ctrl->resume_mask) {
      ctrl->mark = ctrl->mark_keep = ctrl->scan_pos;
      ctrl->mark_saved = -1;
      ctrl->continuation = ctrl->resume_script;
      rob->request.word = t;
      control_resume_clear((int)ctrl->side);
    }
    return 1;
  }
  return 0;
}

void control_ctx_down(control_rec *ctrl)
{
  rob_rec *rob;
  short v;
  ushort m;
  int id;

  rob = ctrl->rob;
  if ((rob->move_flags & 0x40000) != 0) {
    return;
  }
  if (rob->action.part.type != 9) {
    if (rob->action.part.type == 0x14) {
      return;
    }
    if ((rob->side_flags & 2) == 0) {
      return;
    }
    rob->request.word = 0x9000000;
    return;
  }
  if (((rob->state & 0x800) != 0) && ((input_presses & 0xf) != 0)) {
    ctrl->state = (input_presses & 0xf) + 0x2000000;
    if (move_script_run(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[0],0) != 0) {
      return;
    }
    if (ctrl->wait != 0) {
      return;
    }
  }
  id = 0x32;
  if (input_presses != 0) {
    rob->press_count = rob->press_count + 1;
  }
  if ((rob->action.part.lever & 1) != 0) {
    id = rob->action.part.move & 0x7fff;
  }
  if ((input_presses & 2) != 0) {
    if (id == 0x32) {
      id = 0x3c;
    }
  }
  else {
    if ((input_presses & 4) != 0) {
      rob->request.part.lever = 1;
      if ((input_buttons & 0x40) != 0) {
        id = 0x5d;
      }
      else {
        id = 0x59;
      }
      goto done;
    }
    if (((input_presses & 1) != 0) && ((input_buttons & 0xc0) != 0)) {
      rob->request.part.lever = 1;
      if ((input_buttons & 0x40) != 0) {
        id = 0x3a;
      }
      else {
        id = 0x38;
      }
      goto done;
    }
    if (((input_presses & 1) != 0) && ((input_buttons & 0x30) != 0)) {
      m = 0x10;
      rob->request.part.lever = 1;
      if (rob->screen_xy[0] > rob->opp->screen_xy[0]) {
        m = 0x20;
      }
      if ((m & input_buttons) != 0) {
        id = 0x32;
      }
      else if ((rob->relation_flags & 1) != 0) {
        id = 0x34;
      }
      else {
        id = 0x36;
      }
      goto done;
    }
    if ((input_presses & 1) == 0) goto done;
  }
  rob->request.part.lever = 1;
done:
  rob->request.part.type = 9;
  ((struct { unsigned int f : 15; } *)&rob->request)->f = (ushort)id;
  v = ctrl->history_idx;
  ctrl->scan_pos = v;
  ctrl->mark = v;
  ctrl->mark_keep = v;
  ctrl->mark_saved = -1;
}

void control_ctx_rob24_bit0(control_rec *ctrl)
{
  int taken;

  ctrl->scan_pos = control_scan_pos;
  if (((int)(short)input_presses & 0xfU) != 0) {
    ctrl->state = (int)(short)input_presses & 0xfU;
    taken = move_script_run(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[4],0);
    if (taken != 0) {
      return;
    }
  }
  if ((input_presses & 0x3c0) != 0) {
    ctrl->state = 0;
    move_script_run(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[4],0);
  }
  return;
}

void control_ctx_rob24_bit1(control_rec *ctrl)
{
  ctrl->scan_pos = control_scan_pos;
  if (((int)input_presses & 0xfU) != 0) {
    ctrl->state = (int)input_presses & 0xfU;
    move_script_run(ctrl,ctrl->control_sets[ctrl->rob->stance]->scripts[5],0);
  }
  return;
}

void control_ctx_none(control_rec *ctrl)
{
  return;
}

void control_ctx_ukemi(control_rec *ctrl)
{
  rob_rec *rob;
  uchar f;
  uint t;
  int big;
  uint v;
  int m7;

  if ((ctrl->rob->field_0x398 != 0) && (ctrl->context != ctrl->context_prev)) {
    control_scan_from(ctrl, ctrl->mark);
    ctrl->pending_script = 0;
    control_script = 0;
    if (control_history_scan(ctrl) != 0) return;
  }
  rob = ctrl->rob;
  f = rob->field_0x118;
  m7 = 7;
  big = 0;
  ctrl->scan_pos = control_scan_pos;
  ctrl->mark = control_scan_pos;
  ctrl->mark_keep = control_scan_pos;
  ctrl->mark_saved = -1;
  if (rob->field_0x119 != 0) return;
  if (f & 2) {
    t = f & 4;
  } else {
    if ((*(short *)&input_presses & m7) != m7) return;
    t = f & 0x20;
  }
  if (t == 0) return;
  if (rob->combo[0].damage > 60) big = 1;
  if (f & 4) {
    v = 0xf010000;
    if (input_buttons & 0x80) v += 1;
    else if (input_buttons & 0x40) v += 2;
    else if (input_buttons & 0x10) v += 3;
    else if (input_buttons & 0x20) v += 4;
    rob->request.word = v;
  } else if (f & 0x20) {
    if (f & 0x10) {
      int *d = (int *)&rob->bones;
      d[9] = action15_motion_table[rob->field_0x28f][big][0][5] + 0xf030000;
    } else {
      f |= 2;
      rob->request.word = 0xf000000;
      rob->field_0x118 = f;
    }
  } else {
    rob->field_0x119 = 0x28;
  }
}

void control_ctx_mash(control_rec *ctrl)
{
  rob_rec *rob;

  rob = ctrl->rob;
  if (1 < rob->field_0x544) {
    if ((input_presses & 0xf) != 0) {
      rob->press_count = rob->press_count + 1;
    }
    ctrl->rob->request.word = 0x1b020000;
  }
  return;
}

int control_dir_script_run(control_rec *ctrl, uchar *script)
{
  int r;
  ctrl->field_0x44 = 0xf1;
  ctrl->field_0x46 = 0xff;
  ctrl->scan_limit = ctrl->mark_keep;
  r = move_script_exec(ctrl,script);
  switch (r) {
  case 1:
    ctrl->mark = ctrl->scan_pos;
  case 2:
    ctrl->mark_keep = ctrl->scan_pos;
    ctrl->mark_saved = 0xffff;
    break;
  default:
    return r;
  }
  return r;
}

int control_script_try(control_rec *ctrl, uchar *script)
{
  int r;

  ctrl->field_0x44 = 0xf1;
  ctrl->field_0x46 = 0xff;
  if (ctrl->pending_script != 0) {
    uchar *id = script;
    script = ctrl->pending_script;
    ctrl->scan_limit = ctrl->mark_saved == -1 ? ctrl->mark : ctrl->mark_saved;
    r = move_script_exec(ctrl,script);
    switch (r) {
    case 5:
      if ((ctrl->flags & 0x40) != 0) {
        ctrl->mark_saved = ctrl->mark;
        ctrl->mark = ctrl->scan_pos;
      }
      break;
    case 1:
      ctrl->mark = ctrl->scan_pos;
    case 2:
      ctrl->mark_keep = ctrl->scan_pos;
      ctrl->mark_saved = 0xffff;
      return r;
    }
    if (id == 0) {
      return r;
    }
    if ((ctrl->continuation != 0) && (id != ctrl->continuation)) {
      return 1;
    }
    script = id;
  }
  return move_script_run(ctrl,script,1);
}

int move_script_run(control_rec *ctrl, uchar *script, int keep)
{
  int r;

  if ((char)keep == 0)
    ctrl->continuation = 0;
  ctrl->scan_limit = ctrl->mark_saved == -1 ? ctrl->mark : ctrl->mark_saved;
  r = move_script_exec(ctrl,script);
  switch (r) {
  case 5:
    if (ctrl->flags & 0x40) {
      ctrl->mark_saved = ctrl->mark;
      ctrl->mark = ctrl->scan_pos;
    }
    break;
  case 1:
    ctrl->mark = ctrl->scan_pos;
  case 2:
    ctrl->mark_keep = ctrl->scan_pos;
    ctrl->mark_saved = -1;
    break;
  }
  return r;
}

uint move_script_exec(control_rec *ctrl, uchar *script)
{
  uint r;
  script_choice *s;

  r = 0;
  if (script == (uchar *)0x0) return 0;
  if ((game_flags & 0x200004) == 0) return r;
  {
    script_pc = script;
    s = &script_choice_stack;
    script_choice_depth = '\0';
    s->scan_pos = control_scan_pos;
    s->input = input_buttons;
    s->new_press = input_presses;
    s->field_0x14 = input_rob_flags;
    s->field_0x8 = 0;
    s->field_0xa = input_buttons;
    s->resume_pc = (int)script;
    s->field_0x10 = ctrl->state;
    s->saved_allow_frames = 0;
    s->saved_window = 0;
    s->pc_only = 0;
    control_scan_pos = ctrl->scan_pos - 1U & 0x3f;
    input_buttons = input_buttons & 0xf0;
    input_presses = input_presses & 0xf0;
    script_input_last = input_buttons;
    script_allow_mask = 0;
    script_allow_frames = 0;
    script_window = 0;
    while (script_pc != (uchar *)0x0) {
      if ((char)*script_pc >= 0x50) break;
      {
        uchar **t = &move_script_ops;
        r = (*(uint (**)(control_rec *))(t + (char)*script_pc * 2))(ctrl);
      }
      if (r != 0) {
        if ((int)r >= 0) {
          if (r - 3 <= 1) r = 0;
        }
        input_buttons = script_choice_stack.input;
        input_presses = script_choice_stack.new_press;
        input_rob_flags = script_choice_stack.field_0x14;
        control_scan_pos = script_choice_stack.scan_pos;
        ctrl->state = script_choice_stack.field_0x10;
        return r;
      }
    }
    input_buttons = script_choice_stack.input;
    input_presses = script_choice_stack.new_press;
    input_rob_flags = script_choice_stack.field_0x14;
    control_scan_pos = script_choice_stack.scan_pos;
    ctrl->state = script_choice_stack.field_0x10;
  }
  return r;
}

uint script_read(uint *field, int type)
{
  uint v;
  switch((char)type) {
  case 4:
  case 5:
    v = *field;
    break;
  case 3:
    v = (uint)*(ushort *)field;
    break;
  case 2:
    v = (uint)*(short *)field;
    break;
  case 1:
    v = (uint)*(byte *)field;
    break;
  case 0:
  default:
    v = (uint)*(char *)field;
    break;
  }
  return v;
}

void script_write(int *field, int type, int value)
{
  switch((char)type) {
  case 4:
  case 5:
    *field = value;
    break;
  case 2:
  case 3:
    *(short *)field = (short)value;
    break;
  case 0:
  case 1:
  default:
    *(char *)field = (char)value;
    break;
  }
}

int script_press_held(control_rec *ctrl)
{
  if (script_allow_mask == 0) {
    if (input_buttons != script_input_last) {
      return script_backtrack(ctrl);
    }
    return script_step_back(ctrl);
  }
  if (script_allow_mask == script_input_last) {
    if (0 < script_allow_frames) {
      script_allow_frames = script_allow_frames + -1;
      if (input_buttons == (script_allow_mask & input_buttons)) {
        script_input_last = input_buttons;
        script_allow_mask = input_buttons;
        return script_step_back(ctrl);
      }
    }
    return script_backtrack(ctrl);
  }
  if (input_buttons == script_input_last) {
    return script_step_back(ctrl);
  }
  if (input_buttons == 0) {
    script_input_last = 0;
    return script_step_back(ctrl);
  }
  if (input_buttons == (script_allow_mask & input_buttons)) {
    script_input_last = input_buttons;
    script_allow_mask = input_buttons;
    script_allow_frames = script_allow_frames + -1;
    return script_step_back(ctrl);
  }
  return script_backtrack(ctrl);
}

int script_step_back(control_rec *ctrl)
{
  short s;
  uint u;
  if (script_window != 0) {
    s = ctrl->scan_limit;
    script_window = script_window + -1;
    if (control_scan_pos != s) {
      control_scan_pos = control_scan_pos - 1U & 0x3f;
      u = (ushort)control_scan_pos;
      input_rob_flags = ctrl->history[u].rob_flags;
      input_buttons = ctrl->history[u].buttons & 0xf0;
      input_presses = ctrl->history[u].presses & 0xf0;
      return 0;
    }
  }
  return script_backtrack(ctrl);
}

int script_backtrack(control_rec *ctrl)
{
  char n;
  script_choice *e;
  int r;

  n = script_choice_depth;
  e = &script_choice_stack;
  if (n != 0 && ctrl->wait != 0x7fff) {
    e = &e[n];
    if (e->pc_only == 0) {
      input_buttons = e->input;
      input_presses = e->new_press;
      input_rob_flags = e->field_0x14;
      script_input_last = e->field_0x8;
      script_allow_mask = e->field_0xa;
      control_scan_pos = e->scan_pos;
      script_window = e->saved_window;
    }
    script_pc = (uchar *)e->resume_pc;
    script_choice_depth = n - 1;
    r = 0;
  }
  else {
    input_buttons = e->input;
    input_presses = e->new_press;
    input_rob_flags = e->field_0x14;
    control_scan_pos = e->scan_pos;
    script_pc = (uchar *)e->resume_pc;
    ctrl->state = e->field_0x10;
    if (ctrl->wait == 0x7fff) {
      r = 1;
    }
    else {
      r = 3;
    }
  }
  return r;
}

void script_advance(int size)
{
  uchar **p;
  if (script_branch_mode != '\0') {
    p = (uchar **)script_branch_pos;
    script_branch_pos = (uchar *)(p + 1);
    script_pc = p[1];
  }
  else {
    uchar *q = script_pc; q += size; script_pc = q;
  }
}

int script_op_00_fail(control_rec *ctrl)
{
  char depth;

  depth = (int)script_choice_depth;
  input_buttons = (&script_choice_stack)[depth].input;
  input_presses = (&script_choice_stack)[depth].new_press;
  input_rob_flags = (&script_choice_stack)[depth].field_0x14;
  control_scan_pos = (&script_choice_stack)[depth].scan_pos;
  script_pc = (uchar *)(&script_choice_stack)[depth].resume_pc;
  ctrl->state = (&script_choice_stack)[depth].field_0x10;
  return 3;
}

int script_op_01_resume_at(control_rec *ctrl)
{
  int r;
  script_choice *e;

  e = &(&script_choice_stack)[(int)script_choice_depth];
  if ((script_branch_mode != '\0') || ((ctrl->flags & 2) != 0)) {
    r = script_op_00_fail(ctrl);
  }
  else {
    ctrl->continuation = *(uchar **)(script_pc + 4);
    input_buttons = e->input;
    input_presses = e->new_press;
    input_rob_flags = e->field_0x14;
    control_scan_pos = e->scan_pos;
    script_pc = (uchar *)e->resume_pc;
    ctrl->state = e->field_0x10;
    r = 4;
  }
  return r;
}

int script_op_02_move(control_rec *ctrl)
{
  int rule;

  ctrl->rob->request.word = *(uint *)(script_pc + 4);
  rule = msnm_rule_get();
  if (rule == 9) {
    msnm_combo_move_add((uint)ctrl->rob->side,ctrl->rob->request.word);
  }
  ctrl->wait = 0;
  ctrl->field_0x70 = 0;
  ctrl->dir_branch_list = 0;
  ctrl->rob->branch_list = 0;
  ctrl->rob->preset_throw = 0;
  ctrl->rob->preset_throw_move = 0;
  if ((ctrl->rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  return 1;
}

int script_op_03_move_keep(control_rec *ctrl)
{
  script_op_02_move(ctrl);
  return 2;
}

int script_op_04_input(control_rec *ctrl)
{
  ushort v;
  ushort m;
  v = *(ushort *)(script_pc + 2);
  m = v & 0xff;
  script_input_tested = m;
  if ((v >> 8 & 1) != 0) m = input_forward_mask | m;
  if ((v >> 10 & 1) != 0) m = input_bit10_mask | m;
  if ((v >> 9 & 1) != 0) m = input_back_mask | m;
  if ((v & 0x800) != 0) m = input_bit11_mask | m;
  if ((v & 0x1000) != 0) m = input_bit12_mask | m;
  if ((v & 0x2000) != 0) m = input_bit13_mask | m;
  script_input_tested = m;
  if (input_buttons == m) {
    script_input_last = input_buttons;
    script_allow_mask = 0;
    script_allow_frames = 0;
    script_advance(4);
    return 0;
  }
  if (input_buttons == script_input_last) return script_step_back(ctrl);
  return script_press_held(ctrl);
}

int script_op_05_allow(control_rec *ctrl)
{
  ushort v;

  v = *(ushort *)(script_pc + 2);
  script_allow_mask = v & 0xff;
  if ((v >> 8 & 1) != 0) {
    script_allow_mask = input_forward_mask | script_allow_mask;
  }
  if ((v >> 10 & 1) != 0) {
    script_allow_mask = script_allow_mask | input_bit10_mask;
  }
  if ((v >> 9 & 1) != 0) {
    script_allow_mask = script_allow_mask | input_back_mask;
  }
  if ((v & 0x800) != 0) {
    script_allow_mask = script_allow_mask | input_bit11_mask;
  }
  if ((v & 0x1000) != 0) {
    script_allow_mask = script_allow_mask | input_bit12_mask;
  }
  if ((v & 0x2000) != 0) {
    script_allow_mask = script_allow_mask | input_bit13_mask;
  }
  script_allow_frames = (short)(char)script_pc[1];
  script_advance(4);
  if (input_buttons == script_input_last) return 0;
  if ((input_buttons & ~script_allow_mask) != 0) return script_backtrack(ctrl);
  else return 0;
}

int script_op_06_input_bit(control_rec *ctrl)
{
  ushort n;
  ushort m;
  ushort *p;
  short s;
  n = (char)script_pc[1];
  m = 1 << n;
  if (n == 8) p = &input_forward_mask;
  else if (n == 10) p = (ushort *)&input_bit10_mask;
  else if (n == 9) p = &input_back_mask;
  else if (n == 11) p = (ushort *)&input_bit11_mask;
  else if (n == 12) p = (ushort *)&input_bit12_mask;
  else if (n == 13) p = &input_bit13_mask;
  else goto skip;
  m = *p;
skip:
  s = input_buttons;
  if ((m & s) != 0) {
    script_input_last = s;
    script_allow_mask = 0;
    script_allow_frames = 0;
    script_advance(4);
    return 0;
  }
  if (input_buttons == script_input_last) return script_step_back(ctrl);
  return script_press_held(ctrl);
}

int script_op_07_not_input_bit(control_rec *ctrl)
{
  ushort n;
  ushort m;
  ushort *p;
  short s;
  n = (char)script_pc[1];
  m = 1 << n;
  if (n == 8) p = &input_forward_mask;
  else if (n == 10) p = (ushort *)&input_bit10_mask;
  else if (n == 9) p = &input_back_mask;
  else if (n == 11) p = (ushort *)&input_bit11_mask;
  else if (n == 12) p = (ushort *)&input_bit12_mask;
  else if (n == 13) p = &input_bit13_mask;
  else goto skip;
  m = *p;
skip:
  s = input_buttons;
  if ((m & s) == 0) {
    script_input_last = s;
    script_allow_mask = 0;
    script_allow_frames = 0;
    script_advance(4);
    return 0;
  }
  if (input_buttons == script_input_last) return script_step_back(ctrl);
  return script_press_held(ctrl);
}

int script_op_08_new_press(control_rec *ctrl)
{
  ushort v;
  ushort m;

  if (input_presses == 0) return script_step_back(ctrl);
  v = *(ushort *)(script_pc + 2);
  m = v & 0xff;
  if ((v >> 8 & 1) != 0) m = input_forward_mask | m;
  if ((v >> 10 & 1) != 0) m = input_bit10_mask | m;
  if ((v >> 9 & 1) != 0) m = input_back_mask | m;
  if ((v & 0x800) != 0) m = input_bit11_mask | m;
  if ((v & 0x1000) != 0) m = input_bit12_mask | m;
  if ((v & 0x2000) != 0) m = input_bit13_mask | m;
  if ((input_presses != m) || (input_buttons != m))
    return script_backtrack(ctrl);
  script_input_last = input_buttons;
  script_allow_mask = 0;
  script_allow_frames = 0;
  script_advance(4);
  return 0;
}

int script_op_09_new_press_bit(control_rec *ctrl)
{
  ushort b;
  ushort m;

  if (input_presses != 0) {
    b = (char)script_pc[1];
    m = 1 << b;
    if (b == 8) m = input_forward_mask;
    else if (b == 10) m = input_bit10_mask;
    else if (b == 9) m = input_back_mask;
    else if (b == 11) m = input_bit11_mask;
    else if (b == 12) m = input_bit12_mask;
    else if (b == 13) m = input_bit13_mask;
    if ((m & input_presses) != 0) {
      script_input_last = input_buttons;
      script_allow_mask = 0;
      script_allow_frames = 0;
      script_advance(4);
      return 0;
    }
  }
  return script_step_back(ctrl);
}

int script_op_13_window(void)
{
  script_window = (short)*(char *)(script_pc + 1);
  script_advance(4);
  return 0;
}

int script_op_10_choice(control_rec *ctrl)
{
  script_choice *e;
  if (script_branch_mode != '\0') {
    return script_op_00_fail(ctrl);
  }
  script_choice_depth = script_choice_depth + '\x01';
  if ('\x1f' < script_choice_depth) {
    script_choice_depth = ' ';
  }
  e = &(&script_choice_stack)[(int)script_choice_depth];
  e->input = input_buttons;
  e->new_press = input_presses;
  e->field_0x14 = input_rob_flags;
  e->field_0x8 = script_input_last;
  e->field_0xa = script_allow_mask;
  e->saved_allow_frames = script_allow_frames;
  e->saved_window = script_window;
  e->scan_pos = control_scan_pos;
  e->pc_only = 0;
  e->field_0x10 = ctrl->state;
  e->resume_pc = *(int *)(script_pc + 4);
  script_pc = script_pc + 8;
  return 0;
}

int script_op_11_choice_pc(control_rec *ctrl)
{
  int r;
  int depth;

  if (!(script_branch_mode == '\0')) {
    r = script_op_00_fail(ctrl);
  }
  else {
    script_choice_depth = script_choice_depth + '\x01';
    if ('\x1f' < script_choice_depth) {
      script_choice_depth = ' ';
    }
    depth = (int)script_choice_depth;
    (&script_choice_stack)[depth].pc_only = 1;
    (&script_choice_stack)[depth].resume_pc = *(int *)(script_pc + 4);
    script_pc = script_pc + 8;
    r = 0;
  }
  return r;
}

int script_op_12_continue_with(control_rec *ctrl)
{
  control_set_rec *set;
  uchar **table;
  uint n;
  uchar *p;

  if (script_branch_mode != '\0') {
    set = ctrl->control_sets[ctrl->rob->stance];
    table = set->table;
    n = *(uint *)(script_pc + 4);
    n = (n < set->count) ? n : 0;
    if ((table == 0) || (table[n] == 0)) {
      return script_op_00_fail(ctrl);
    }
    ctrl->continuation = table[n];
    p = script_branch_pos;
    script_branch_pos = p + 4;
    script_pc = *(uchar **)(p + 4);
  }
  else {
    ctrl->continuation = *(uchar **)(script_pc + 4);
    script_pc = script_pc + 8;
  }
  return 0;
}

int script_op_14_wait_here(control_rec *ctrl)
{
  uchar *p;
  if (script_branch_mode != '\0') {
    ctrl->branch_pending = (uchar **)script_branch_pos;
    ctrl->branch_wait = 0x7fff;
    p = script_branch_pos;
    script_branch_pos = p + 4;
    script_pc = *(uchar **)(p + 4);
  }
  else {
    ctrl->continuation = script_pc;
    ctrl->wait = 0x7fff;
    script_pc = script_pc + 4;
  }
  ctrl->rob->side_flags = ctrl->rob->side_flags | 0x100;
  ctrl->flags = ctrl->flags | 0x40;
  return 0;
}

int script_op_15_clear_wait(control_rec *ctrl)
{
  uchar *t;
  if (script_branch_mode != '\0') {
    ctrl->branch_pending = 0;
    ctrl->branch_kind = 0;
    ctrl->branch_wait = 0;
    t = script_branch_pos;
    script_branch_pos = t + 4;
    script_pc = *(uchar **)(t + 4);
  }
  else {
    ctrl->continuation = 0;
    ctrl->wait = 0;
    script_pc = script_pc + 4;
  }
  return 0;
}

int script_wait_frame(control_rec *ctrl, uchar resume)
{
  short v;
  rob_rec *rob = ctrl->rob;
  v = script_read((uint *)((char *)rob + *(int *)(script_pc + 4)),(int)(char)script_pc[1]);
  v = v + *(short *)(script_pc + 2);
  script_advance(8);
  if (v <= ctrl->rob->frame + 1 || script_branch_mode == '\x03') {
    if (script_branch_mode != '\0') {
      ctrl->branch_pending = 0;
      ctrl->branch_kind = 0;
    }
    else {
      ctrl->continuation = 0;
    }
    return 0;
  }
  if (script_branch_mode != '\0') {
    ctrl->branch_wait = v;
    ctrl->branch_pending = (uchar **)script_branch_pos;
    ctrl->branch_kind = script_branch_kind;
  }
  else {
    ctrl->wait = v;
    ctrl->continuation = script_pc;
  }
  ctrl->rob->side_flags = ctrl->rob->side_flags | 0x100;
  ctrl->flags = ctrl->flags | 0x40;
  return resume ? 5 : 1;
}

int script_op_16_wait_frame(control_rec *ctrl)
{
  return script_wait_frame(ctrl,0);
}

int script_op_78_wait_frame_resume(control_rec *ctrl)
{
  return script_wait_frame(ctrl,1);
}

int script_op_17_before_frame(control_rec *ctrl)
{
  short v;
  char *base;
  base = (char *)ctrl->rob;
  v = script_read((uint *)(base + *(int *)(script_pc + 4)),
                      (int)(char)script_pc[1]) + *(short *)(script_pc + 2);
  script_advance(8);
  if (ctrl->rob->frame + 1 > v) {
    return script_backtrack(ctrl);
  }
  return 0;
}

int script_op_18_flush_input(control_rec *ctrl)
{
  control_buffer_clear((int)ctrl->side);
  control_pending_clear((int)ctrl->side);
  script_advance(4);
  return 0;
}

int script_op_19_jump(control_rec *ctrl)
{
  int r;

  if (!(script_branch_mode == '\0')) {
    r = script_op_00_fail(ctrl);
  }
  else {
    script_pc = *(uchar **)(script_pc + 4);
    r = 0;
  }
  return r;
}

int script_op_20_if_state_eq(control_rec *ctrl)
{
  int v = *(int *)(script_pc + 4);
  if (v == ctrl->state) {
    script_advance(8);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_21_jump_state_eq(control_rec *ctrl)
{
  int v = *(int *)(script_pc + 4);
  if (script_branch_mode != '\0') return script_op_00_fail(ctrl);
  if (v == ctrl->state) {
    script_pc = *(uchar **)(script_pc + 8);
  }
  else {
    script_pc = script_pc + 0xc;
  }
  return 0;
}

int script_op_22_if_state_ge(control_rec *ctrl)
{
  uint v = *(uint *)(script_pc + 4);
  if (script_branch_mode != '\0') return script_op_00_fail(ctrl);
  if (ctrl->state >= v) {
    script_advance(8);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_23_if_state_le(control_rec *ctrl)
{
  uint v = *(uint *)(script_pc + 4);
  if (script_branch_mode != '\0') return script_op_00_fail(ctrl);
  if (ctrl->state <= v) {
    script_advance(8);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_24_switch_held_dir(control_rec *ctrl)
{
  int v = (byte)ctrl->history[control_scan_pos].dirs;
  if (script_branch_mode != '\0') {
    return script_op_00_fail(ctrl);
  }
  if ((v & (ushort)input_forward_mask) != 0) {
    script_pc = *(uchar **)(script_pc + 4);
  }
  else if ((v & (ushort)input_back_mask) != 0) {
    script_pc = *(uchar **)(script_pc + 8);
  }
  else {
    script_pc = *(uchar **)(script_pc + 0xc);
  }
  return 0;
}

int script_op_25_history_mark(control_rec *ctrl)
{
  if (script_pc[1] == '\0') {
    ctrl->scan_limit = ctrl->mark;
  }
  else {
    ctrl->scan_limit = ctrl->mark_keep;
  }
  script_advance(4);
  return 0;
}

int script_op_26_buffer(control_rec *ctrl)
{
  uchar *pc;
  ushort n;
  ushort v;
  uchar *w;
  int k;

  pc = script_pc;
  n = *(ushort *)(pc + 4);
  v = *(ushort *)(pc + 6);
  w = *(uchar **)(pc + 8);
  if (n == 0) {
    ctrl->pending_wait = v;
    ctrl->pending_script = w;
    ctrl->pending_kind = n;
    k = 2;
  }
  else {
    ctrl->buffer_kind_2 = n;
    ctrl->buffer_wait = v;
    ctrl->buffer_script_2 = w;
    k = 1;
  }
  control_slot_mark(ctrl,k);
  script_advance(0xc);
  return 0;
}

int script_op_27_if_bit(control_rec *ctrl)
{
  uint mask;
  uint v;
  char *base;
  base = (char *)ctrl->rob;
  mask = 1 << (char)script_pc[1];
  v = script_read((uint *)(base + *(int *)(script_pc + 4)),(int)(char)script_pc[2]);
  if ((mask & v) == 0) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_28_if_not_bit(control_rec *ctrl)
{
  char *base;
  char *p;
  uint bit;
  base = (char *)ctrl->rob;
  p = (char *)script_pc;
  bit = 1 << p[1];
  if ((bit & script_read((uint *)(base + *(int *)(p + 4)),(int)p[2])) != 0) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_29_if_opp_bit(control_rec *ctrl)
{
  char *base;
  uint m;

  base = (char *)ctrl->rob->opp;
  m = 1 << (char)script_pc[1];
  if ((m & script_read((uint *)(base + *(int *)(script_pc + 4)), (int)(char)script_pc[2])) == 0) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_30_if_opp_not_bit(control_rec *ctrl)
{
  char *base = (char *)ctrl->rob->opp;
  uint m = 1 << (char)script_pc[1];
  if (m & script_read((uint *)(base + *(int *)(script_pc + 4)), (char)script_pc[2])) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_31_load(control_rec *ctrl)
{
  char *base = (char *)ctrl->rob;
  base += *(int *)(script_pc + 4);
  ctrl->state = script_read((uint *)base, (int)(char)script_pc[1]);
  script_advance(8);
  return 0;
}

int script_op_32_store(control_rec *ctrl)
{
  char *base;
  base = (char *)ctrl->rob;
  base += *(int *)(script_pc + 4);
  script_write((int *)base,(int)(char)script_pc[1], ctrl->state);
  script_advance(8);
  return 0;
}

int script_op_33_load_opp(control_rec *ctrl)
{
  char *base;
  base = (char *)ctrl->rob->opp;
  base += *(int *)(script_pc + 4);
  ctrl->state = script_read((uint *)base, (int)(char)script_pc[1]);
  script_advance(8);
  return 0;
}

int script_op_34_store_opp(control_rec *ctrl)
{
  char *base = (char *)ctrl->rob->opp;
  script_write((int *)(base + *(int *)(script_pc + 4)),
               (int)(char)script_pc[1],ctrl->state);
  script_advance(8);
  return 0;
}

int script_op_35_if_distance_le(control_rec *ctrl)
{
  float l = *(float *)(script_pc + 4);
  if (l < ctrl->rob->opp_dist) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_36_if_distance_ge(control_rec *ctrl)
{
  float d = *(float *)(script_pc + 4);
  if (d > ctrl->rob->opp_dist) {
    return script_backtrack(ctrl);
  }
  script_advance(8);
  return 0;
}

int script_op_37_if_rob62(control_rec *ctrl)
{
  int r;

  if (!((ctrl->rob->action.part.lever & *(char *)(script_pc + 1)) == 0)) {
    script_advance(4);
    r = 0;
  }
  else {
    r = script_backtrack(ctrl);
  }
  return r;
}

int script_op_38_cancel_move(control_rec *ctrl)
{
  int arg;
  int r;
  uint move;

  if ((ctrl->rob->state & 0x40) != 0) {
    move = *(uint *)(script_pc + 4);
    arg = move_cmd_arg_find(move & 0x7fff,0,2);
    if ((arg != 0) && (ctrl->rob->frames_to_land >= *(short *)(arg + 6))) {
      ctrl->rob->request.word = move;
      ctrl->wait = 0;
      if (ctrl->resume_script != 0) {
        ctrl->continuation = ctrl->resume_script;
        ctrl->resume_script = 0;
      }
      return 1;
    }
  }
  ctrl->continuation = 0;
  ctrl->resume_script = 0;
  r = script_backtrack(ctrl);
  return r;
}

int script_op_39_if_state_bit(control_rec *ctrl)
{
  if (((1 << (char)script_pc[1]) & ctrl->state) != 0) {
    script_advance(4);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_40_if_not_state_bit(control_rec *ctrl)
{
  if (((1 << ((char *)script_pc)[1]) & ctrl->state) == 0) {
    script_advance(4);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_41_jump_state_any(control_rec *ctrl)
{
  uint m = *(uint *)(script_pc + 4);
  if (script_branch_mode != '\0') {
    return script_op_00_fail(ctrl);
  }
  if ((m & ctrl->state) != 0) {
    script_pc = *(uchar **)(script_pc + 8);
  }
  else {
    script_pc = script_pc + 0xc;
  }
  return 0;
}

int script_op_42_jump_state_none(control_rec *ctrl)
{
  uint m = *(uint *)(script_pc + 4);
  if (script_branch_mode != '\0') {
    return script_op_00_fail(ctrl);
  }
  if ((m & ctrl->state) != 0) {
    script_pc = script_pc + 0xc;
  }
  else {
    script_pc = *(uchar **)(script_pc + 8);
  }
  return 0;
}

int script_cond(control_rec *ctrl, int cmp_le, int jump)
{
  uchar m = cmp_le;
  uchar f = jump;
  int idx;
  ushort thr;
  ushort *ent;
  ushort v;
  ushort d;
  int ok;
  uint mask;

  if (script_branch_mode != 0) return script_op_00_fail(ctrl);
  idx = (char)script_pc[1];
  thr = *(ushort *)(script_pc + 2);
  ent = &input_conditions[idx * 3];
  mask = ctrl->state;
  if ((mask & ent[1]) == 0) {
    if (f == 0) return script_backtrack(ctrl);
    script_pc = script_pc + 8;
    return 0;
  }
  { ushort *row = ctrl->history[control_scan_pos].counters; v = row[ent[0]]; }
  d = control_scan_pos - ctrl->scan_limit & 0x3f;
  if (d <= 12 && d <= v) v = 0;
  if (m == 1) { uchar a = v <= thr; uchar b = v != 0; ok = a & b; }
  else { uchar a = thr <= v; uchar b = v != 0; ok = a & b; }
  if (ok != 1) {
    if (f == 0) return script_backtrack(ctrl);
    script_pc = script_pc + 8;
    return 0;
  }
  if (idx <= 7 || ctrl->history[control_scan_pos].counters[3] == ent[2]) {
    if (f == 1) script_pc = *(uchar **)(script_pc + 4);
    else script_pc = script_pc + 4;
    return 0;
  }
  if (f == 0) return script_backtrack(ctrl);
  script_pc = script_pc + 8;
  return 0;
}

int script_op_43_if_cond_ge(control_rec *ctrl)
{
  return script_cond(ctrl,0,0);
}

int script_op_79_if_cond_le(control_rec *ctrl)
{
  return script_cond(ctrl,1,0);
}

int script_op_44_jump_cond_ge(control_rec *ctrl)
{
  return script_cond(ctrl,0,1);
}

int script_op_65_jump_cond_le(control_rec *ctrl)
{
  return script_cond(ctrl,1,1);
}

int script_op_45_move_restart(control_rec *ctrl)
{
  ctrl->rob->request.word = *(int *)(script_pc + 4) + 0x12000000;
  ctrl->wait = 0;
  ctrl->field_0x70 = 0;
  ctrl->dir_branch_list = 0;
  ctrl->rob->branch_list = 0;
  ctrl->continuation = 0;
  ctrl->rob->preset_throw = 0;
  ctrl->rob->preset_throw_move = 0;
  if ((ctrl->rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  control_buffer_clear((int)ctrl->side);
  control_pending_clear((int)ctrl->side);
  return 5;
}

int script_op_46_move_masked(control_rec *ctrl)
{
  int rule;
  uint move;

  move = *(uint *)(script_pc + 4);
  rule = msnm_rule_get();
  if (rule == 9) {
    msnm_combo_move_add((uint)ctrl->rob->side,move);
  }
  ctrl->rob->request.word = move & 0x3070000;
  ctrl->resume_move = move;
  ctrl->resume_mask = ctrl->state;
  ctrl->wait = 0;
  ctrl->field_0x70 = 0;
  ctrl->rob->preset_throw = 0;
  ctrl->rob->preset_throw_move = 0;
  if ((ctrl->rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  control_slot_mark(ctrl,3);
  return 1;
}

int script_op_47_set_resume(control_rec *ctrl)
{
  uchar *p;
  if (script_branch_mode != '\0') {
    p = script_branch_pos;
    script_branch_pos = p + 4;
    script_pc = *(uchar **)(p + 4);
  }
  else {
    ctrl->resume_script = *(uchar **)(script_pc + 4);
    script_pc = script_pc + 8;
    control_slot_mark(ctrl,3);
  }
  return 0;
}

int script_op_72_input_reset(control_rec *ctrl)
{
  control_resume_clear((int)ctrl->side);
  script_advance(4);
  return 0;
}

int script_op_48_if_free(control_rec *ctrl)
{
  rob_rec *rob = ctrl->rob;
  if (script_branch_mode != '\0') return script_op_00_fail(ctrl);
  if ((hit_wait_timer_get() != 0 && (rob->opp->flags24 & 0x8000) == 0) ||
      (rob->state & 1) != 0 || (rob->relation_flags & 2) != 0 ||
      (rob->opp->state & 0x400064c1) != 0) {
    return script_backtrack(ctrl);
  }
  script_pc = script_pc + 4;
  return 0;
}

int script_op_49_set_rob338(control_rec *ctrl)
{
  rob_rec *q = ctrl->rob; q->field_0x338 = *(uint *)(script_pc + 4);
  ctrl->wait = 0;
  if ((ctrl->rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  return 1;
}

int script_op_50_if_entry_current(control_rec *ctrl)
{
  int r;

  if (!((int)(uint)ctrl->history[(short)control_scan_pos].counters[2] <
      (int)ctrl->rob->frame)) {
    script_advance(4);
    r = 0;
  }
  else {
    r = script_backtrack(ctrl);
  }
  return r;
}

int script_op_51_if_rate_ge(control_rec *ctrl)
{
  int r;

  if (!(ctrl->rob->attack_rate < *(char *)(script_pc + 1))) {
    script_advance(4);
    r = 0;
  }
  else {
    r = script_backtrack(ctrl);
  }
  return r;
}

int script_op_52_state_set(control_rec *ctrl)
{
  ctrl->state = *(uint *)(script_pc + 4);
  script_advance(8);
  return 0;
}

int script_op_53_state_add(control_rec *ctrl)
{
  ctrl->state = ctrl->state + *(int *)(script_pc + 4);
  script_advance(8);
  return 0;
}

int script_wall_dist(control_rec *ctrl, char opp, char le)
{
  uchar *p;
  int idx;
  float thr;
  rob_rec *rob;
  float f;

  p = script_pc;
  idx = (char)p[1] & 3;
  thr = *(float *)(p + 4);
  if (opp != '\0') {
    rob = ctrl->rob->opp;
  }
  else {
    rob = ctrl->rob;
  }
  f = rob->wall_dist[idx];
  if (le != '\0' ? f > thr : thr > f) {
    return 0;
  }
  if (rob->wall_angle[idx] > 0x1000) {
    return 0;
  }
  script_advance(8);
  return 1;
}

int script_op_54_if_wall_dist_le(control_rec *ctrl)
{
  int hit;

  hit = script_wall_dist(ctrl,0,1);
  if (!(hit == 0)) {
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_55_if_wall_dist_ge(control_rec *ctrl)
{
  int hit;

  hit = script_wall_dist(ctrl,0,0);
  if (!(hit == 0)) {
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_56_if_opp_wall_dist_le(control_rec *ctrl)
{
  int hit;

  hit = script_wall_dist(ctrl,1,1);
  if (!(hit == 0)) {
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_57_if_opp_wall_dist_ge(control_rec *ctrl)
{
  int hit;

  hit = script_wall_dist(ctrl,1,0);
  if (!(hit == 0)) {
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_58_nop(void)
{
  script_advance(4);
  return 0;
}

int script_op_59_move_or_hold(control_rec *ctrl)
{
  uint mask;
  ushort f;
  rob_rec *rob;

  mask = 1 << (char)script_pc[1];
  rob = ctrl->rob;
  if ((user_back_data.option_bits & 0x100) != 0 || (mask & rob->reach_flags) == 0) {
    rob->request.word = *(uint *)(script_pc + 4);
    rob->preset_throw = script_pc[1];
    rob->preset_throw_move = *(uint *)(script_pc + 8);
    rob->field_0x1ec = *(ushort *)(script_pc + 2);
    if (rob->field_0x1ec == 3) {
      f = input_buttons & 0xc0;
      if ((input_buttons & input_forward_mask) != 0) {
        f = f | 0x100;
      }
      if ((input_buttons & input_back_mask) != 0) {
        f = f | 0x200;
      }
      rob->field_0x1ec = rob->field_0x1ec | f;
    }
    ctrl->field_0x70 = *(uchar **)(script_pc + 0xc);
    ctrl->wait = 0;
    ctrl->dir_branch_list = 0;
    ctrl->rob->branch_list = 0;
  }
  else {
    rob->request.word = *(uint *)(script_pc + 8);
    ctrl->continuation = *(uchar **)(script_pc + 0xc);
    rob->preset_throw = 0;
    rob->preset_throw_move = 0;
  }
  if ((rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  return 1;
}

int script_move_if(control_rec *ctrl, char other_side, char chara)
{
  uint r;

  if (chara >= 0) {
    r = ctrl->rob->chara != chara;
  }
  else {
    r = (*(int *)&ctrl->rob->flags >> 0xe ^ 1U) & 1 ^ other_side;
  }
  if (r != 0) {
    script_advance(8);
    return 0;
  }
  ctrl->rob->request.word = *(uint *)(script_pc + 4);
  ctrl->field_0x70 = 0;
  ctrl->wait = 0;
  ctrl->dir_branch_list = 0;
  ctrl->rob->branch_list = 0;
  ctrl->rob->preset_throw = 0;
  ctrl->rob->preset_throw_move = 0;
  if ((ctrl->rob->side_flags & 0x100) == 0) {
    ctrl->flags = ctrl->flags | 0x40;
  }
  return 1;
}

int script_op_60_move_if_side(control_rec *ctrl)
{
  return script_move_if(ctrl,0,-1);
}

int script_op_61_move_if_other_side(control_rec *ctrl)
{
  return script_move_if(ctrl,1,-1);
}

int script_op_62_move_if_chara(control_rec *ctrl)
{
  return script_move_if(ctrl,0,(int)*(char *)(script_pc + 1));
}

int script_op_63_escape(control_rec *ctrl)
{
  int r;
  char *count;

  if ((ctrl->rob->opp->flags28 & 0xc1000) != 0) {
    if (ctrl->rob->field_0x292 < '\x01') {
      r = script_backtrack(ctrl);
      return r;
    }
    count = &ctrl->rob->field_0x292;
    *count = *count + -1;
  }
  ctrl->rob->flags =
       ctrl->rob->flags & 0xdfffffff;
  ctrl->rob->flags =
       ctrl->rob->flags | 0x10000000;
  script_advance(4);
  return 0;
}

int script_op_64_hold(control_rec *ctrl)
{
  user_back_data_rec *g = &user_back_data;
  uchar *p;

  if (((g->option_bits & 0x800000) != 0) && (hit_wait_timer_get() > 0) &&
      ((ctrl->rob->flags24 & 0x8000) == 0)) {
    if ((g->option_bits & 0x1000000) != 0) {
    if (script_branch_mode != '\0') {
      ctrl->branch_pending = (uchar **)script_branch_pos;
      ctrl->branch_kind = script_branch_kind;
      ctrl->branch_wait = 0x7fff;
    }
    else {
      ctrl->continuation = script_pc;
      ctrl->wait = 0x7fff;
    }
    ctrl->rob->side_flags = ctrl->rob->side_flags | 0x100;
    ctrl->flags = ctrl->flags | 0x40;
    return 4;
    }
    return script_op_00_fail(ctrl);
  }
  if (script_branch_mode != '\0') {
    p = script_branch_pos;
    script_branch_pos = p + 4;
    script_pc = *(uchar **)(p + 4);
    ctrl->branch_pending = 0;
    ctrl->branch_kind = 0;
    ctrl->branch_wait = 0;
  }
  else {
    script_pc = script_pc + 4;
    ctrl->continuation = 0;
    ctrl->wait = 0;
  }
  return 0;
}

int script_op_66_if_frame_flag(control_rec *ctrl)
{
  int r;

  if (((1 << ((char *)script_pc)[1]) & input_rob_flags) == 0) {
    r = script_step_back(ctrl);
  }
  else {
    script_advance(4);
    r = 0;
  }
  return r;
}

int script_op_67_if_not_frame_flag(control_rec *ctrl)
{
  if ((1 << (char)script_pc[1]) & input_rob_flags) {
    return script_step_back(ctrl);
  }
  script_advance(4);
  return 0;
}

int script_op_68_jump_move_eq(control_rec *ctrl)
{
  int s;
  s = *(short *)(script_pc + 2);
  if ((uint)ctrl->rob->move_no == s) {
    script_pc = *(uchar **)(script_pc + 4);
  }
  else {
    script_advance(8);
  }
  return 0;
}

int script_op_69_raw_bit(control_rec *ctrl)
{
  ushort idx;
  ushort mask;
  short *p;
  idx = (char)script_pc[1];
  mask = 1 << idx;
  if (idx == 8) p = (short *)&input_forward_mask;
  else if (idx == 10) p = &input_bit10_mask;
  else if (idx == 9) p = (short *)&input_back_mask;
  else if (idx == 11) p = &input_bit11_mask;
  else if (idx == 12) p = &input_bit12_mask;
  else if (idx == 13) p = (short *)&input_bit13_mask;
  else goto test;
  mask = *p;
test:
  if (mask & input_dirs) {
    script_advance(4);
    return 0;
  }
  return script_backtrack(ctrl);
}

int script_op_70_input_toward(control_rec *ctrl)
{
  int r;

  if (!((input_buttons & input_toward_mask) == 0)) {
    script_advance(4);
    r = 0;
  }
  else {
    r = script_backtrack(ctrl);
  }
  return r;
}

int script_op_71_clear_all(control_rec *ctrl)
{
  ctrl->branch_pending = 0;
  ctrl->branch_kind = 0;
  ctrl->branch_wait = 0;
  ctrl->continuation = 0;
  ctrl->wait = 0;
  script_advance(4);
  return 0;
}

int script_op_74_clear_branches(control_rec *ctrl)
{
  rob_rec *q = ctrl->rob; q->branch_list = 0;
  ctrl->branch_pending = 0;
  ctrl->branch_kind = 0;
  ctrl->branch_wait = 0;
  ctrl->continuation = 0;
  ctrl->wait = 0;
  script_advance(4);
  return 0;
}

int script_op_73_set_rob1e8(control_rec *ctrl)
{
  rob_rec *rob = ctrl->rob; rob->preset_throw_move = *(uint *)(script_pc + 4);
  ctrl->field_0x70 = *(uchar **)(script_pc + 8);
  return 1;
}

int script_op_75_if_card_flag(control_rec *ctrl)
{
  char *m = script_pc;
  if (rob_card_flag_test(ctrl->side,(char)m[1]) == 0) {
    return script_step_back(ctrl);
  }
  script_advance(4);
  return 0;
}

int script_op_76_if_not_card_flag(control_rec *ctrl)
{
  uchar *s = script_pc;
  if (rob_card_flag_test((int)ctrl->side,(int)(char)s[1]) == 1) {
    return script_step_back(ctrl);
  }
  script_advance(4);
  return 0;
}

int script_op_77_move_if_card_flag(control_rec *ctrl)
{
  uchar *s = script_pc;
  rob_rec *rob;
  uint v;
  control_set_rec *set;
  uchar **list;

  if (rob_card_flag_test((int)ctrl->side,(int)(char)s[1]) == 1) {
    rob = ctrl->rob;
    v = *(uint *)(s + 8);
    if (v != 0) {
      if (script_branch_mode != '\0') {
        set = ctrl->control_sets[rob->stance];
        list = set->table;
        v = (v < set->count) ? v : 0;
        if (list == 0 || list[v] == 0) {
          script_advance(0xc);
          return 0;
        }
        ctrl->continuation = list[v];
      }
      else {
        ctrl->continuation = *(uchar **)(s + 8);
      }
    }
    rob->request.word = *(uint *)(s + 4);
    if (msnm_rule_get() == 9) {
      msnm_combo_move_add((uint)ctrl->rob->side,ctrl->rob->request.word);
    }
    rob->branch_list = 0;
    rob->preset_throw = 0;
    rob->preset_throw_move = 0;
    ctrl->wait = 0;
    ctrl->field_0x70 = 0;
    if ((ctrl->rob->side_flags & 0x100) == 0) {
      ctrl->flags = ctrl->flags | 0x40;
    }
    return 1;
  }
fail:
  script_advance(0xc);
  return 0;
}

int input_match(int pattern)
{
  int c;
  int i;
  ushort *p;
  ushort m;

  c = (char)pattern;
  if (c < 0) i = 0;
  else if (c <= 0x35) i = c;
  else i = 0;
  p = &input_patterns[i * 3];
  m = input_buttons;
  switch (p[2]) {
  case 0:
    if ((m & p[0]) == 0) return 1;
    break;
  case 1:
    m = input_presses;
  case 2:
    if (p[0] == (p[1] & m)) return 1;
    break;
  }
  return 0;
}

int move_branch_cond(control_rec *ctrl, char cond, ushort min)
{
  ushort *e;
  ushort *p;

  if (cond == 0) return 1;
  e = &input_conditions[cond * 3];
  p = ctrl->counters;
  if ((*p & e[1]) != 0 && p[e[0]] >= min) {
    if (cond > 7 && ctrl->counters[3] != e[2]) return 0;
    return 1;
  }
  return 0;
}

int move_branch_scan(control_rec *ctrl, int *list, int mode_arg)
{
  char mode = mode_arg;
  int *p = list;
  char v;
  int (*fn)(control_rec *, uchar *);
  byte *e;

  if (p == (int *)0x0) return 0;
  if (mode == 1 && (ctrl->rob->side_flags & 0x20) != 0) v = 3;
  else v = 1;
  script_branch_mode = v;
  if (*p != 0) {
    fn = move_branch_try;
  loop:
    e = (byte *)*p;
    if (mode != 2 || (*e & 2) != 0) {
      if (fn(ctrl,e)) return 1;
    }
    p += 2;
    if (*p != 0) goto loop;
  }
  return 0;
}

int move_branch_try(control_rec *ctrl, uchar *entry)
{
  short frame;
  short end;
  int replay;
  char *count;
  int ok;
  rob_rec *rob;
  rob_rec *q;

  replay = 0;
  rob = ctrl->rob;
  frame = rob->frame;
  end = *(short *)(entry + 8);
  if (((rob->state & 0x200000) != 0) &&
     ((rob->flags28 & 0x4000000) != 0)) {
    if (((user_back_data.option_bits & 0x1800000) == 0x1000000) &&
       (rob->field_0x1ec != 0)) {
      replay = 1;
      input_buttons = rob->field_0x1ec;
      q = ctrl->rob; q->field_0x1ec = 0;
      count = &ctrl->rob->field_0x292;
      *count = *count + '\x01';
      if ((input_buttons & 0x100) != 0) {
        input_buttons = input_forward_mask | input_buttons;
      }
      if ((input_buttons & 0x200) != 0) {
        input_buttons = input_back_mask | input_buttons;
      }
      input_presses = input_buttons;
    }
    if (frame <= end) {
      ctrl->rob->flags |= 0x20000000;
    }
    else {
      ctrl->rob->flags &= 0xdfffffff;
    }
  }
  if (frame < *(short *)(entry + 6) || end < frame) {
    return 0;
  }
  ok = input_match((int)(char)entry[1]);
  if (ok == 0) {
    if (replay) {
      count = &ctrl->rob->field_0x292;
      *count = *count + -1;
    }
  }
  else {
    if ((!replay) && (ok = input_match((int)(char)entry[2]), ok == 0)) {
      return 0;
    }
    ok = move_branch_cond(ctrl,(int)(char)entry[3],(uint)*(ushort *)(entry + 4));
    if (ok == 0) {
      return 0;
    }
    script_branch_kind = *entry;
    ok = move_branch_take(ctrl,*(uchar ***)(entry + 0xc));
    if (ok != 0) {
      return 1;
    }
    ctrl->rob->flags =
         ctrl->rob->flags & 0xcfffffff;
  }
  return 0;
}

int move_branch_take(control_rec *ctrl, uchar **pos)
{
  short saved;

  script_branch_pos = (uchar *)pos;
  if (pos == (uchar **)0x0) return 0;
  saved = ctrl->scan_pos;
  ctrl->scan_pos = ctrl->history_idx;
  if (move_script_run(ctrl,*pos,1) != 0) {
    ctrl->wait = 0;
    return 1;
  }
  ctrl->scan_pos = saved;
  return 0;
}

uchar ** control_branch_pending_get(rob_rec *rob)
{
  return rob->ctrl->branch_pending;
}

void control_slot_clear(int side, uint kind)
{
  control_rec *ctrl;

  ctrl = (control_rec *)control_object_get((int)(char)side);
  switch (kind) {
  case 1:
    ctrl->buffer_wait = 0;
    ctrl->buffer_kind = 0;
    ctrl->buffer_kind_2 = 0;
    ctrl->buffer_script = 0;
    ctrl->buffer_script_2 = 0;
    break;
  case 2:
    ctrl->pending_kind = 0;
    ctrl->pending_wait = 0;
    ctrl->pending_script = 0;
    break;
  case 3:
    ctrl->resume_move = 0;
    ctrl->resume_mask = 0;
    ctrl->resume_script = 0;
    break;
  }
  return;
}

void control_marked_slots_clear(int side)
{
  control_rec *ctrl;
  int player;

  player = (int)(char)side;
  ctrl = (control_rec *)control_object_get(player);
  control_slot_clear(player,ctrl->slot_marks[0]);
  control_slot_clear(player,ctrl->slot_marks[1]);
  control_slot_marks_reset(ctrl);
  return;
}

void control_buffer_clear(int side)
{
  control_slot_clear((int)(char)side,1);
  return;
}

void control_pending_clear(int side)
{
  control_slot_clear((int)(char)side,2);
  return;
}

void control_resume_clear(int side)
{
  control_slot_clear((int)(char)side,3);
  return;
}

void control_slot_marks_reset(control_rec *ctrl)
{
  ctrl->slot_marks[0] = 0;
  ctrl->slot_marks[1] = 0;
  return;
}

void control_slot_mark(control_rec *ctrl, int kind)
{
  ctrl->slot_marks[0] = kind;
  return;
}

int control_buffer_use(char side, int request)
{
  control_rec *ctrl;
  int used;
  uint n;

  n = (uint)(char)request;
  ctrl = (control_rec *)control_object_get((int)(char)side);
  if ((n < 8) && (((uint)ctrl->buffer_kind & control_buffer_kind_masks[n]) != 0)) {
    int boolflag_r = 0;
    if (n == 4) boolflag_r = 1;
    used = control_buffer_move(ctrl,boolflag_r);
  }
  else {
    used = 0;
  }
  return used;
}

int control_buffer_move(control_rec *ctrl, int as_continuation)
{
  if ((uchar)as_continuation != 0) {
    ctrl->continuation = ctrl->buffer_script;
  }
  else {
    ctrl->pending_kind = ctrl->buffer_kind;
    ctrl->pending_script = ctrl->buffer_script;
    ctrl->pending_wait = ctrl->buffer_wait;
  }
  control_slot_clear((int)ctrl->side,1);
  return 1;
}

int script_idle_check(int side)
{
  control_rec *ctrl;
  int idle;

  ctrl = (control_rec *)control_object_get((int)(char)side);
  idle = 0;
  if ((ctrl->wait < 1) && (ctrl->continuation == 0)) {
    idle = 1;
  }
  return idle;
}

int script_wait_frames(int side)
{
  control_rec *ctrl;
  int ret;

  ctrl = (control_rec *)control_object_get((int)(char)side);
  ret = -1;
  if (ctrl->branch_wait != 0) {
    ret = ctrl->branch_wait;
  }
  else if (ctrl->wait != 0) {
    ret = ctrl->wait;
  }
  return ret;
}

int script_next_move(int side)
{
  control_rec *ctrl;
  int move;
  int player;

  player = (int)(char)side;
  ctrl = (control_rec *)control_object_get(player);
  move = 0;
  if (ctrl->branch_pending != (uchar **)0x0) {
    move = script_peek_move(player,*ctrl->branch_pending,(int)ctrl->branch_wait);
  }
  if (move == 0) {
    move = script_peek_move(player,ctrl->continuation,(int)ctrl->wait);
  }
  return move;
}

ushort script_peek_move(char side, char *pc, short wait)
{
  typedef struct { int code; int len; } cond_ent;
  ushort r;
  char n;
  int done;
  uint c;
  r = 0;
  if ((pc != 0) & (wait != 0) && wait != 0x7fff) {
    n = 62;
    while (1) {
      c = *pc;
      if (c >= 0x50) break;
      done = 0;
      switch (c) {
      case 2: r = *(ushort *)(pc + 4) & 0x7fff; done = 1; break;
      case 3: r = *(ushort *)(pc + 4) & 0x7fff; done = 1; break;
      case 0x2d: r = *(ushort *)(pc + 4) & 0x7fff; done = 1; break;
      case 0x2e: r = *(ushort *)(pc + 4) & 0x7fff; done = 1; break;
      case 0x4d:
        if (rob_card_flag_test(side, pc[1]) == 1) goto hit;
        pc += ((cond_ent *)((char *)&move_script_op_sizes - 4))[c].len;
        break;
      case 0x3c:
        if (((rob_get((byte)side))->flags & 0x4000) == 0) goto hit;
        goto skip;
      case 0x3d:
        if (((rob_get((byte)side))->flags & 0x4000) != 0) goto hit;
      case 0x11:
        pc += ((cond_ent *)((char *)&move_script_op_sizes - 4))[c].len;
        break;
      case 0x3e:
        if ((rob_get((byte)side))->chara != pc[1]) goto hit;
      skip:
        pc += ((cond_ent *)((char *)&move_script_op_sizes - 4))[c].len;
        break;
      hit:
        r = 0x7fff & *(ushort *)(pc + 4);
        done = 1;
        break;
      default:
        done = 1;
        break;
      }
      if (done || --n == -1) return r;
    }
  }
  return r;
}
