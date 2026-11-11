/*
 * vf4lib/libenemy/src/vp_main.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern ushort vp_no_follow_moves[10]; /* 0x0c16c3b8 */
extern char vp_ukemi_types[3]; /* 0x0c16c3cc */
extern ushort vp_wait_cut_moves[4]; /* 0x0c16c3d0 */
extern short vp_drink_inputs[8]; /* 0x0c16c3d8 */
extern short vp_exc_p_or_k[2]; /* 0x0c16c3e8 */
extern short vp_exc_punches[6]; /* 0x0c16c3ec */
extern short vp_exc_kicks[5]; /* 0x0c16c3f8 */
extern char vp_level_by_difficulty_table[5][11]; /* 0x0c16c404 */
extern char *vp_level_param_sets[3]; /* 0x0c16cac8 */
extern char vp_trait_table[9][5]; /* 0x0c16cad4 */
extern short vp_exc_punch_input; /* 0x0c4202b0 */
extern short vp_exc_kick_input; /* 0x0c4202b2 */
extern float vp_decay_rate; /* 0x0c4202b4 */
extern int vp_mode_flag; /* 0x0c4202c0 */
extern int vp_frame_usec; /* 0x0c4202c4 */
extern int vp_frame_usec_max; /* 0x0c4202c8 */
extern uint vp_frame_count; /* 0x0c4202cc */
extern vp_work vp_work_2p; /* 0x0c420690 */
extern int vp_level_param_set; /* 0x0c420718 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern short vp_rank_weights[8]; /* 0x0c4202d0 */
extern vp_work vp_work_1p; /* 0x0c420608 */

static inline int result_ok(int r)
{
  return !(r != 2 && r != 4);
}

void vp_boot_init(void)
{
  int i;

  
  vp_level_param_set_select(2);
  debug_disp = OFF_ON_OFF;
  vp_frame_usec_max = 0;
  vp_frame_usec = 0;
  vp_frame_count = 0;
  debug_level = 0;
  debug_user = -1;
  debug_start_act = 0xffff;
  vp_decay_rate = 0.666;
  vp_rank_weights[0] = 0;
  vp_rank_weights[1] = 1000;

  for (i = 2; i < 8; i++) {
    short *a = vp_rank_weights;
    a[i] = (short)(int)((float)a[i - 1] * 0.666);
  }
  debug_y_pos = 0x19;
}

void vp_work_init(void)
{
  int k;
  int i;
  uint side;
  vp_work *work;

  work = &vp_work_1p;
  side = 0;
  do {
    work->rob = rob_get(side & 0xff);
    work->snap = &vp_rob_snaps[side];
    work->history = vp_mode_histories[side];
    work->other = (vp_work *)((int)&vp_work_1p + (side ^ 1) * 0x88);
    work->traits = vp_traits[side];
    work->renkei_work = vp_renkei_works[side];
    work->log = &vp_hame_logs[side * 0x40];
    i = 0;
    work->flags = 0;
    work->stroke = 0;
    do {
      k = side * 0x40 + i;
      vp_hame_logs[k].move = 0xffff;
      vp_hame_logs[k].count = 0;
      i = i + 1;
    } while (i < 0x40);
    side = side + 1;
    work = work + 1;
  } while ((int)side < 2);
  return;
}

void vp_round_init(void)
{
  vp_work *work;
  int side;
  rob_rec *rob;
  vp_chara_actions_rec *chara_tab;
  vp_chara_actions_rec *opp_chara_tab;
  int i;
  vp_renkei_work_rec *rw;
  vp_mode_history_rec *hist;
  int *list;
  int n;
  uchar level;
  char *traits;
  vp_renkei_rec *renkei;
  short mode_bits;

  work = &vp_work_1p;
  for (side = 0; side < 2; side++, work++) {
    rob = work->rob;
    chara_tab = vp_chara_table_get(rob->chara);
    opp_chara_tab = vp_chara_table_get(rob->opp->chara);
    if ((chara_tab == 0) | (opp_chara_tab == 0)) {
      work->flags = 0;
      work->stroke = 0;
      continue;
    }
    work->chara = (int)chara_tab;
    work->opp_chara = (int)opp_chara_tab;
    work->attacks = 0;
    work->stroke_step = 0xff;
    work->run_step = 0xff;
    work->esc_step = 0xff;
    work->field_0x10 = 0;
    work->seq_frames = 0;
    work->disadv = 0;
    work->flags = 5;
    work->mode = 0;
    work->mode_frames = 0;
    work->idle_frames = 0;
    work->mash = 0;
    work->yoke_side = 0;
    work->move = 0;
    work->seq = 0;
    work->action = 0;
    work->combo = 0;
    hist = (vp_mode_history_rec *)work->history;
    for (i = 0; i < 8; i++, hist++) {
      hist->mode = 0;
      hist->frames = -1;
    }
    list = opp_chara_tab->escape_seqs;
    n = 0;
    for (i = 0; i < 3; i++)
      work->esc_seqs[i] = 0;
    while (*list++ != 0)
      n++;
    work->n = n;
    traits = work->traits;
    level = vp_level_get(side);
    *traits = level;
    work->level_params = vp_level_params_get(level);
    *(short *)(traits + 10) = vp_card_count_sum(side);
    traits[1] = vp_trait_get(0,0) / 2 + vp_trait_get(8,0) / 2;
    traits[2] = vp_trait_get(1,0);
    traits[3] = vp_trait_get(2,0);
    traits[4] = vp_trait_get(3,0);
    traits[5] = vp_trait_get(4,0);
    traits[6] = vp_trait_get(5,0);
    traits[7] = vp_trait_get(6,0);
    traits[8] = vp_trait_get(7,0);
    if (debug_user >= 0)
      work->renkei = vp_user_renkei_get(debug_user);
    else
      work->renkei = 0;
    if (work->renkei != 0) {
      rw = (vp_renkei_work_rec *)work->renkei_work;
      renkei = (vp_renkei_rec *)work->renkei;
      rw->combo_action.flags = 0;
      rw->combo_action.drank = 0;
      rw->combo_action.frames = 100;
      rw->combo_action.stroke = renkei->combo.stroke;
      rw->combo_action.combos = rw->combos;
      switch (renkei->combo.kind) {
      case 1:
      case 3:
        mode_bits = 0xff;
        break;
      case 2:
        mode_bits = 4;
        break;
      case 6:
        mode_bits = 0x40;
        break;
      case 7:
        mode_bits = 0x100;
        break;
      default:
        mode_bits = 0;
        break;
      }
      rw->combos[0].flags = mode_bits;
      rw->combos[0].chara_mask = renkei->combo.chara_mask;
      rw->combos[0].strokes = renkei->combo.strokes;
      rw->combos[1].flags = 0;
      rw->combos[1].chara_mask = 0;
      rw->combos[1].strokes = 0;
    }
    if (rob->chara == 0xc && (vp_rand() & 1) != 0)
      work->action = (int)vp_common_action_get(3);
    if (work->flags & 1)
      work->flags |= 2;
  }
}

void vp_judge_init(int match_end)
{
  vp_work *work;
  int side;

  work = &vp_work_1p;
  side = 0;
  do {
    if (work->rob->chara == '\x03' && match_end == 0) {
      vp_state_restart(work,0x18);
    }
    else {
      work->flags = work->flags & 0xfffffffd;
    }
    side = side + 1;
    work = work + 1;
  } while (side < 2);
  return;
}

void vp_work_disable(void)
{
  vp_work *work;
  int side;

  work = &vp_work_1p;
  side = 0;
  do {
    work->flags = 0;
    work->stroke = 0;
    side = side + 1;
    work = work + 1;
  } while (side < 2);
  return;
}

int vp_both_enabled(void)
{
  int side;
  vp_work *work;

  work = &vp_work_1p;
  side = 0;
  do {
    if ((work->flags & 1) == 0) {
      return 0;
    }
    side = side + 1;
    work = work + 1;
  } while (side < 2);
  return 1;
}

void vp_frame(void)
{
  rob_rec *rob1;
  rob_rec *rob2;
  int *usec;
  int now;

  rob1 = rob_get(0);
  rob2 = rob_get(1);
  switch(cpu_on_off) {
  case BOTH_OFF:
    rob1->flags |= 4;
    rob2->flags |= 4;
    break;
  case _1P_ON:
    rob1->flags &= ~4;
    rob2->flags |= 4;
    break;
  case _2P_ON:
    rob1->flags |= 4;
    rob2->flags &= ~4;
    break;
  case BOTH_ON:
    rob1->flags &= ~4;
    rob2->flags &= ~4;
    break;
  }
  if (pause_frame_held() == 0) {
    vp_frame_usec = stopwatch_usec();
    vp_frame_count = vp_frame_count + 1;
    vp_move_change_note();
    if ((rob1->flags & 4) == 0) {
      vp_think(&vp_work_1p);
    }
    if ((rob2->flags & 4) == 0) {
      vp_think(&vp_work_2p);
    }
    vp_rob_state_snapshot();
    usec = (int *)&vp_frame_usec;
    now = stopwatch_usec();
    *usec = now - *usec;
    if (vp_frame_usec_max < *usec) {
      vp_frame_usec_max = *usec;
    }
  }
}

void vp_level_set(unsigned char level)
{
  if (level > 0x1e) {
    level = 0x1e;
  }
  debug_level = level;
}

void vp_level_set_for_round(uint round)
{
  debug_level = vp_level_by_difficulty(round & 0xff);
  return;
}

void vp_mode_flag_set(uint flag)
{
  int t = flag > 1;
  int m = t == 0;
  vp_mode_flag = flag & -m;
}

void vp_mode_flag_clear(void)
{
  vp_mode_flag_set(0);
  return;
}

char * vp_version_get(void)
{
  return "Ver.1.10 2004-03-04 Wangan-Kage";
}

int * vp_rob_list_get(rob_rec *rob)
{
  return (int *)rob->timed_list;
}

void vp_move_change_note(void)
{
  int side;
  vp_work *work;

  work = &vp_work_1p;
  side = 0;
  do {
    if (work->rob->move_no != work->snap->move_no) {
      work->move = (int)work->rob->ctrl->continuation;
    }
    side = side + 1;
    work = work + 1;
  } while (side < 2);
  return;
}

void vp_rob_state_snapshot(void)
{
  vp_rob_snap_rec *snap;
  rob_rec *rob;
  int side;
  vp_work *work;

  work = &vp_work_1p;
  side = 0;
  do {
    rob = work->rob;
    snap = work->snap;
    snap->action_type = rob->action.part.type;
    snap->move_no = rob->move_no;
    snap->state = rob->state;
    snap->flags24 = rob->flags24;
    snap->flags28 = rob->flags28;
    side = side + 1;
    work = work + 1;
  } while (side < 2);
  return;
}

uchar vp_level_get(int side)
{
  return debug_level;
}

int vp_card_count_sum(int side)
{
  card_image_rec *img;
  
  img = card_image_get(side);
  return (int)(short)(img->header.all_win + img->header.all_lose);
}

int vp_rand(void)
{
  int r;
  
  r = lcg_next15();
  return r;
}

uint vp_crouch_bit_ahead(rob_rec *rob, int frames)
{
  int *ent;
  int p;
  uint bit;
  int clear_frame;
  int set_frame;

  set_frame = -1;
  clear_frame = -1;
  bit = rob->flags & 0x10;
  ent = vp_rob_list_get(rob);
  if (ent != (int *)0x0) {
    p = *ent;
    while (p != 0) {
      if (ent[1] == 4) {
        if (((short *)*ent)[1] == 0xd) {
          set_frame = (int)*(short *)*ent;
        }
      }
      else if ((ent[1] == 5) && (((short *)*ent)[1] == 0xd)) {
        clear_frame = (int)*(short *)*ent;
      }
      ent = ent + 2;
      p = *ent;
    }
  }
  if (set_frame > clear_frame) {
    if (rob->frame < clear_frame) {
      if (clear_frame <= rob->frame + frames) bit = 0;
    }
    if ((rob->frame < set_frame) && (set_frame <= rob->frame + frames)) {
      bit = 0x10;
    }
  }
  else if (set_frame < clear_frame) {
    if ((rob->frame < set_frame) && (set_frame <= rob->frame + frames)) {
      bit = 0x10;
    }
    if (rob->frame < clear_frame) {
      if (clear_frame <= rob->frame + frames) bit = 0;
    }
  }
  return bit;
}

void vp_edge_flags_update(vp_work *work)
{
  uint flags;
  rob_rec *rob;

  rob = work->rob;
  flags = work->flags & 0xfffff3ff;
  work->flags = flags;
  if ((rob->bound_dist[0] < 2.0) && ((rob->wall_bits & 1) == 0)) {
    work->flags = flags | 0x400;
  }
  if ((rob->bound_dist[2] < 2.0) && ((rob->wall_bits & 4) == 0)) {
    work->flags = work->flags | 0x800;
  }
  return;
}

void vp_state_change(vp_work *work, int mode)
{
  vp_mode_history_rec *hist;
  int i;

  hist = (vp_mode_history_rec *)work->history;
  for (i = 7; i > 0; i--) {
    hist[i] = hist[i - 1];
  }
  hist->frames = work->mode_frames;
  hist->mode = work->mode;
  work->mode = mode;
  work->field_0x10 = 0;
  work->mode_frames = 0;
  if (mode != 6) {
    *(ushort *)&work->field_0x20[2] = 0;
    work->combo = 0;
  }
  if (mode == 5) {
    if (work->action == (int)vp_common_action_get(1)) {
      *(ushort *)&work->field_0x20[2] = 0x40;
    }
  }
  if (mode != 5) {
    work->action = 0;
  }
  if (1 < mode - 5U) {
    work->flags = work->flags & 0xfffcffff;
  }
  vp_stance_from_move(work,0);
  return;
}

void vp_state_restart(vp_work *work, int mode)
{
  uint flags;

  vp_input_clear(work);
  vp_stroke_result_clear(work);
  flags = work->flags & 0xffffdfff;
  work->flags = flags;
  if (mode != 0xd) {
    work->flags = flags & 0xffffffcf;
  }
  work->gnt = 0;
  work->ynt = 0;
  vp_state_change(work,mode);
  return;
}

void vp_guard_start(vp_work *work, uint gnt, byte set_height, uint height)
{
  if (2 < gnt) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/vp_main.c",0x389,"assertion `%s\' failed.\n",
             "gnt < VP_GN_MAX");
  }
  vp_state_change(work,10);
  work->gnt = (char)gnt;
  if ((char)set_height == '\x01') {
    work->flags = work->flags & 0xffffffcf | height;
  }
  return;
}

void vp_yoke_start(vp_work *work, uint ynt)
{
  if (3 < ynt) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/vp_main.c",0x39d,"assertion `%s\' failed.\n",
             "ynt < VP_YN_MAX");
  }
  vp_state_change(work,0xe);
  work->ynt = (char)ynt;
  return;
}

int vp_mode_history_last(vp_work *work)
{
  vp_mode_history_rec *hist;
  int i;
  int mode;
  hist = (vp_mode_history_rec *)work->history;
  for (i = 0; i < 8; i++, hist++) {
    mode = hist->mode;
    switch (mode) {
    case 0:
    case 1:
    case 4:
    case 9:
      break;
    default:
      return mode;
    }
  }
  return 0;
}

void vp_stroke_result_clear(vp_work *work)
{
  work->stroke = work->stroke & 0xfffffc00;
  return;
}

ushort vp_input_side_mirror(rob_rec *rob, ushort input)
{
  if ((rob->screen_xy[0] > rob->opp->screen_xy[0]) &&
     ((input & 0x30) != 0)) {
    input ^= 0x30;
  }
  return input;
}

int vp_input_seq_start(vp_work *work, short *seq)
{
  rob_rec *rob;

  rob = work->rob;
  if ((work->flags & 4) == 0) return -1;
  if ((ushort)(seq[1] + 1U) <= 1) return -1;
  work->seq = (int)seq;
  work->seq_frames = seq[1];
  rob->ctrl->cpu_buttons = 0;
  work->flags = work->flags | 8;
  return 0;
}

int vp_input_g_tap(vp_work *work, int clear)
{
  if (clear != 0) {
    vp_input_clear(work);
  }
  work->flags = work->flags | 4;
  vp_input_seq_start(work,vp_seq_g_tap);
  return 0;
}

int vp_input_sidestep(vp_work *work, char dir)
{
  rob_rec *rob;
  rob_rec *opp;
  uint idx;

  rob = work->rob;
  opp = rob->opp;
  switch (dir) {
  case 1:
    idx = 0;
    break;
  case 2:
    idx = 1;
    break;
  default:
    return -1;
  }
  if ((opp->side_flags & 0x10) != 0) {
    idx = idx ^ 1;
  }
  if (rob->screen_xy[0] < opp->screen_xy[0]) {
    idx = idx ^ 1;
  }
  vp_input_clear(work);
  work->flags = work->flags | 4;
  vp_input_seq_start(work,vp_seq_sidestep[idx]);
  return 0;
}

int vp_input_dash(vp_work *work, int kind)
{
  unsigned char c = kind;

  if (c == 0) return -1;
  c = c - 1;
  if (c > 3) return -1;
  vp_input_clear(work);
  work->flags = work->flags | 4;
  vp_input_seq_start(work,vp_seq_dash[c]);
  return 0;
}

int vp_input_seq_step(vp_work *work)
{
  rob_rec *rob;
  short *ent;
  short *input;

  rob = work->rob;
  if ((work->flags & 8) == 0) return -1;
  while (work->seq_frames == 0) {
    ent = (short *)work->seq;
    work->seq = (int)(ent + 2);
    work->seq_frames = ent[3];
  }
  if (work->seq_frames == -1) {
    rob->ctrl->cpu_buttons = 0;
    work->flags = work->flags & 0xfffffff3;
    work->seq = 0;
    return 1;
  }
  input = (short *)&rob->ctrl->cpu_buttons;
  *input = vp_input_side_mirror(rob,(uint)*(ushort *)work->seq);
  work->seq_frames = work->seq_frames + -1;
  return 0;
}

int vp_input_guard(vp_work *work)
{
  short input = 1;
  rob_rec *rob = work->rob;
  if ((work->flags & 0x30) == 0) return -1;
  if ((work->flags & 8) != 0) {
    vp_input_clear(work);
  }
  if ((work->flags & 0x20) != 0) {
    input |= 0x40;
  }
  rob->ctrl->cpu_buttons = input;
  return 0;
}

int vp_input_yoroke_mash(vp_work *work)
{
  rob_rec *rob;
  uint d = 2;
  uchar k;
  short *input;
  rob = work->rob;
  if ((work->flags & 8) != 0) {
    vp_input_clear(work);
  }
  k = (uchar)work->mash % d;
  input = vp_mash_inputs[k];
  rob->ctrl->cpu_buttons = *input;
  return 0;
}

int vp_input_ukemi(vp_work *work)
{
  ushort input = 0;
  rob_rec *rob = work->rob;
  if (work->ukemi == '\0') return -1;
  if ((work->flags & 8) != 0) {
    vp_input_clear(work);
  }
  if (work->ukemi == 2) input |= 0x80;
  else if (work->ukemi == 3) input |= 0x40;
  if ((vp_frame_count & 3) == 0) {
    input |= 7;
  }
  rob->ctrl->cpu_buttons = input;
  return 0;
}

int vp_input_rise(vp_work *work)
{
  ushort input;
  rob_rec *rob;

  input = 0;
  rob = work->rob;
  if (work->rise == '\0') {
    return -1;
  }
  if ((work->flags & 8) != 0) {
    vp_input_clear(work);
  }
  if ((work->flags & 0x300) != 0) {
    if ((work->flags & 0x200) != 0) {
      input |= 0x40;
    }
    if ((vp_frame_count & 3) == 0) {
      input |= 4;
    }
  }
  else {
    switch (work->rise) {
    case 2:
      input |= 0x80;
      break;
    case 3:
      input |= 0x40;
      break;
    case 4:
      input |= 0x20;
      break;
    }
    if ((vp_frame_count & 3) == 0) {
      input |= 1;
    }
    input = vp_input_side_mirror(rob, input);
  }
  rob->ctrl->cpu_buttons = input;
  return 0;
}

int vp_input_hit_followup(vp_work *work)
{
  rob_rec *rob;
  vp_level_params_rec *params;
  vp_action_rec *action;
  int kind;
  ushort input;

  rob = work->rob;
  params = work->level_params;
  action = (vp_action_rec *)work->action;
  if (action == 0 || work->attacks != 1) {
    return 0;
  }
  kind = (ushort)action->flags & 0x7c0;
  if (kind == 0) {
    return 0;
  }
  if (vp_rand() % 100 >= params->followup_rate) {
    return 0;
  }
  switch (kind) {
  case 0x100:
    input = 0x13;
    break;
  case 0x80:
    input = 0x23;
    break;
  case 0x200:
    input = 0x16;
    break;
  case 0x400:
    input = 2;
    break;
  case 0x40:
  default:
    input = 3;
    break;
  }
  input = vp_input_side_mirror(rob, input);
  if ((work->flags & 8) != 0) {
    vp_input_clear(work);
  }
  rob->ctrl->cpu_buttons = input;
  return 1;
}

int vp_input_clear(vp_work *work)
{
  work->rob->ctrl->cpu_buttons = 0;
  work->flags = work->flags & 0xfffffff3;
  work->seq = 0;
  work->seq_frames = 0;
  *(ushort *)&work->field_0x20[2] = 0;
  return 0;
}

int vp_stroke_watch_reset(vp_work *work)
{
  work->stroke_step = 0;
  vp_stroke_result_clear(work);
  return 0;
}

int vp_stroke_result_watch(vp_work *work)
{
  rob_rec *rob = work->rob;
  rob_coli_rec *coli;
  uint result;

  if (work->stroke_step < 0) {
    return -1;
  }
  if ((rob->action.part.type == 5) && (work->snap->action_type != 5)) {
    vp_stroke_result_clear(work);
    work->stroke |= 1;
    goto done;
  }
  switch (work->stroke_step) {
  case 0:
    if ((rob->state & 0x4000) == 0) {
      if ((rob->flags28 & 0x40000000) == 0 || rob->frame != rob->active_start) {
        return 0;
      }
    }
    work->stroke_step += 1;
  case 1:
    if (rob->flags28 & 0x40000000) {
      if (rob->frame != rob->active_end) {
        return 0;
      }
    } else {
      coli = rob->coli;
      if (rob->state & 0x4000) {
        if (coli->field_0x3c[0] & 1) {
          result = work->stroke;
          work->stroke = result | 1;
          if (coli->field_0x3c[0] & 0x800) {
            work->stroke = result | 5;
          }
          if (coli->field_0x3c[0] & 2) {
            if (coli->field_0x3c[0] & 0x4000) {
              work->stroke |= 8;
            } else if (coli->field_0x3c[0] & 0x8000) {
              work->stroke |= 0x10;
            } else if (coli->field_0x3c[0] & 0x10000) {
              work->stroke |= 0x20;
            }
          }
          if (coli->field_0x3c[0] & 4) {
            work->stroke |= 0x40;
          }
          if (coli->field_0x3c[0] & 8) {
            work->stroke |= 0x80;
          }
          if (vp_input_hit_followup(work) != 0) {
            goto adv;
          }
        }
        if (coli->field_0x3c[0] & 0x80) {
          work->stroke |= 2;
        }
        if (coli->field_0x3c[0] & 0x20) {
          work->stroke |= 0x100;
          if (vp_input_hit_followup(work) != 0) {
          adv:
            work->stroke_step += 1;
          }
        }
        return 0;
      }
    }
    work->stroke_step += 1;
  case 2:
    if ((work->stroke & 0x103) == 0) {
      work->stroke |= 0x200;
    }
  done:
    work->stroke_step = -1;
    return 1;
  default:
    return -1;
  }
}

int vp_stance_get(vp_work *work)
{
  int stance;
  int *ent;
  rob_rec *rob;

  rob = work->rob;
  if (rob->chara == 0xf) {
    vp_stance_by_player[(uint)rob->side] = (int)work->stance;
    return (int)work->stance;
  }
  stance = (int)rob->stance;
  if (((rob->state & 1) != 0) &&
     (ent = vp_rob_list_get(rob), ent != (int *)0x0)) {
    for (; *ent != 0; ent += 2) {
      if (ent[1] == 0x22) {
        stance = (int)*(short *)(*ent + 2);
      }
    }
  }
  vp_stance_by_player[(uint)rob->side] = stance;
  return stance;
}

bool vp_stance_allows_approach(vp_work *work)
{
  int r;
  bool b;
  r = vp_stance_get(work);
  if (r == 0) {
    b = 1;
  }
  else {
    switch (work->rob->chara) {
    case 2:
    case 0xb:
    case 0xf:
      b = 0;
      break;
    case 3:
      { int t = result_ok(r) == 0; b = t == 0; }
      break;
    case 1:
    default:
      b = 1;
      break;
    }
  }
  r = b;
  return r;
}

int vp_attack_list_get(vp_work *work)
{
  int stance;
  rob_rec *rob;
  uint idx;

  rob = work->rob;
  stance = vp_stance_get(work);
  idx = stance * 6;
  if (rob->opp_dist > 3.0) {
    idx = idx + 4;
  }
  else if (rob->opp_dist > 1.8) {
    idx = idx + 2;
  }
  if ((rob->chara != '\v') || (1 < stance - 3U)) {
    idx = idx + (rob->relation_flags & 1);
  }
  if ((int)idx >= (int)(char)vp_attack_list_size[rob->chara]) idx = 0;
  return (int)((vp_chara_actions_rec *)work->chara)->attack_lists[idx];
}

int vp_attack_pick(vp_work *work, vp_rank_action_rec *list)
{
  int cand[32];
  int n;
  int pick;
  int max_rank;
  short sum;
  rob_rec *rob;
  uint crouch;
  vp_rank_action_rec *p;
  vp_action_rec *act;
  short act_flags;
  int i;
  short r;
  int ret;

  n = 0;
  pick = 1;
  max_rank = 1;
  sum = 0;
  rob = work->rob;
  crouch = rob->flags >> 4 & 1;
  if (debug_user == 500 && rob->chara == 7 && rob->opp->life <= 0x1d &&
      rob->opp_dist < 1.8) {
    return (int)vp_common_action_get(8);
  }
  if (0 < work->disadv) {
    for (p = list; p->action != 0; p++) {
      if (p->rank == 0xff) {
        pick = 0xff;
        break;
      }
    }
  }
  if (pick != 0xff) {
    for (p = list; p->action != 0; p++) {
      if (p->rank > max_rank && p->rank != 0xff) max_rank = p->rank;
    }
    for (i = 1; i <= max_rank; i++) {
      sum += vp_rank_weights[i];
    }
    r = vp_rand() % sum;
    for (i = 1; i <= max_rank; i++) {
      if (r < vp_rank_weights[i]) {
        pick = (short)i;
        break;
      }
      r -= vp_rank_weights[i];
    }
  }
  for (p = list; p->action != 0; p++) {
    act = p->action;
    act_flags = act->flags;
    if ((act_flags & 1) && crouch == 0) continue;
    if ((act_flags & 2) && crouch != 0) continue;
    if (rob->chara == 3) {
      if (rob->attack_rate >= act->drank) ;
      else continue;
    }
    else if (act->drank != 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/vp_main.c",0x650,"assertion `%s\' failed.\n","st_act->drank == 0");
    }
    if (p->rank == pick) {
      cand[n] = (int)act;
      n++;
    }
    else if (p->rank > pick && (act_flags & 8) == 0 && act->frames + work->disadv < 0xd) {
      cand[n] = (int)act;
      n++;
    }
  }
  if (n != 0) {
    ret = cand[vp_rand() % n];
  }
  else {
    ret = (int)vp_common_action_get(1);
  }
  if (ret == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/vp_main.c",0x661,"assertion `%s\' failed.\n","retval != NULL");
  }
  return ret;
}

int vp_close_attack_pick(vp_work *work)
{
  rob_rec *rob;
  rob_rec *opp;
  vp_rate_action_rec *e;
  short roll;
  int ret;

  rob = work->rob;
  opp = rob->opp;
  if (rob->opp_dist > 1.6) {
    return 0;
  }
  if ((vp_crouch_bit_ahead(opp,rob->stun + 8) & 0x10) != 0) {
    if (rob->chara == 12 && work->disadv > -13) {
      return 0;
    }
    e = ((vp_chara_actions_rec *)work->chara)->close_attacks_vs_crouch;
  }
  else if (rob->chara == 3) {
    e = vp_chr3_close_list(rob->attack_rate);
  }
  else if (rob->chara == 12) {
    e = vp_chr12_close_list(vp_stance_get(work));
  }
  else {
    e = ((vp_chara_actions_rec *)work->chara)->close_attacks;
  }
  if (e == 0) {
    ret = 0;
  }
  else {
    roll = vp_rand() % 100;
    for (; e->action != 0; e++) {
      if (e->rate > roll) break;
      roll = roll - e->rate;
    }
    ret = e->action;
  }
  return ret;
}

vp_action_rec * vp_punish_pick(vp_work *work)
{
  uint crouch;
  char *traits;
  rob_rec *opp;
  int last_mode;
  rob_rec *rob;
  vp_action_rec **list;
  vp_action_rec *q;
  uint opp_crouch;
  int r;
  int f2;
  int f3;
  int v;
  int m;

  traits = work->traits;
  last_mode = vp_mode_history_last(work);
  rob = work->rob;
  opp = rob->opp;
  if (vp_rand() % 100 >= work->level_params->punish_rate) return 0;
  if (((work->disadv >= -8) | (last_mode != 0xd)) || (rob->relation_flags & 1) != 0) return 0;
  if (rob->chara == '\f') {
    list = vp_chr12_punish_list(vp_stance_get(work));
  }
  else {
    list = ((vp_chara_actions_rec *)work->chara)->punish_lists;
  }
  crouch = rob->flags & 0x10;
  opp_crouch = opp->flags & 0x10;
  if ((rob->relation_flags & 0x20) != 0) {
    if (opp_crouch != 0) {
      q = ((vp_chara_actions_rec *)work->chara)->punish_vs_crouch;
      if (q != 0) return q;
    }
    else {
      return vp_common_action_get(2);
    }
  }
  r = vp_rand() % 0x70;
  f2 = r < traits[2];
  f3 = r < traits[3];
  q = *list++;
  if (q == 0) goto done;
loop:
  {
    v = (short)q->flags;
    m = v & 0xc;
    if ((v >> 1 & 1) & (crouch != 0)) goto next;
    if ((v & 1) != 0) {
      if (crouch == 0) goto next;
    }
    if ((v >> 3 & 1) & (opp_crouch != 0)) goto next;
    if ((v >> 2 & 1) & (opp_crouch == 0)) goto next;
    if ((m >> 3 & 1) | (m == 0)) {
      if (!f2) goto next;
    }
    if ((m >> 2 & 1) | (m == 0)) {
      if (!f3) goto next;
    }
    if (q->frames + work->disadv > 0) goto next;
    goto found;
  }
next:
  q = *list++;
  if (q != 0) goto loop;
  goto done;
found:
  if (q == 0) {
done:
    q = (vp_action_rec *)vp_close_attack_pick(work);
  }
  return q;
}

bool vp_can_act(vp_work *work)
{
  rob_rec *rob;
  bool r;

  rob = work->rob;
  r = 0;
  if ((rob->state & 1) == 0) {
    r = 1;
  }
  if ((rob->state & 0x1000000) != 0) {
    r = 0;
  }
  else if (rob->stun != 0) {
    if (rob->stun <= 10) {
      r = 1;
    }
  }
  else if ((rob->state & 0x4000000) != 0) {
    if (rob->frame + 10 >= rob->active_end) {
      r = 1;
    }
  }
  else {
    if (rob->chara == '\x0f') {
      vp_stance_from_move(work,0);
      if (work->stance != '\0') {
        r = 1;
      }
    }
    if (rob->frame + 10 >= rob->recovery_end) {
      if (rob->ctrl->branch_pending == 0) {
        r = 1;
      }
    }
  }
  if (r == 1) {
    work->flags = work->flags | 4;
  }
  else {
    work->flags = work->flags & 0xfffffffb;
  }
  return r;
}

int vp_frame_disadv_calc(vp_work *work, int after_opp_attack)
{
  typedef struct { ushort move; short disadv; } ent;
  ent *t;
  rob_rec *rob;
  rob_rec *opp;

  t = (ent *)&vp_disadv_by_move;
  rob = work->rob;
  opp = rob->opp;
  if ((opp->state & 1) == 0) {
    work->disadv = 9;
    return 1;
  }
  if (after_opp_attack == 0) {
    if ((rob->state & 1) == 0) {
      work->disadv = -1;
      return 1;
    }
    if (rob->action.part.type == 5) {
      work->disadv = -2;
    }
    else {
      if (opp->stun == 0) {
        work->disadv = 4;
        return 1;
      }
      for (; t->move != 0xffff; t++) {
        if (rob->move_no == t->move) {
          work->disadv = t->disadv;
          return 0;
        }
      }
      work->disadv =
           (rob->recovery_end - rob->frame) - opp->stun;
    }
  }
  else {
    work->disadv =
         rob->stun - (opp->recovery_end - opp->frame);
  }
  return 0;
}

int vp_frame_disadv_set(vp_work *work, int frames)
{
  work->disadv = (short)frames;
  return 0;
}

uint vp_combo_entry_match(vp_work *work, vp_combo_entry_rec *ent)
{
  rob_rec *rob;
  rob_rec *opp;
  uint r;
  uint x;
  uint d;
  uint c;

  r = 0;
  rob = work->rob;
  opp = rob->opp;
  if ((1 << opp->chara & ent->chara_mask) == 0) return 0;
  x = (int)(short)ent->flags & 0x600;
  if ((x != 0) & (x != 0x600)) {
    d = rob->side_flags;
    d = (d ^ opp->side_flags) & 0x10;
    if ((x >> 9 & 1) & (d != 0) || (x >> 10 & 1) & (d == 0)) return 0;
  }
  if ((ent->flags & 0x100) != 0 && (work->stroke & 2) != 0) r = 1;
  x = ent->flags & 0xff;
  if ((x & 0x40) != 0 && (work->stroke & 0x40) != 0) r = 1;
  if ((x & 0x80) != 0 && (work->stroke & 0x80) != 0) r = 1;
  if ((x & 0x20) != 0 && (work->stroke & 0x20) != 0) r = 1;
  if ((x & 0x10) != 0 && (work->stroke & 0x10) != 0) r = 1;
  if ((x & 8) != 0 && (work->stroke & 8) != 0) r = 1;
  if ((x & 4) != 0 && (work->stroke & 0x38) != 0) r = 1;
  if ((x & 2) != 0 && (work->stroke & 4) != 0) r = 1;
  if ((x & 1) != 0 && (work->stroke & 1) != 0) r = 1;
  if (r != 0 && (ent->flags & 0x800) != 0) r = vp_frame_count & 1;
  if (ent == (vp_combo_entry_rec *)&vp_combo_entry_0b8c) {
    if (rob->move_no != 0xb8c) r = 0;
  }
  else if (ent == (vp_combo_entry_rec *)&vp_combo_entry_113d) {
    r &= -(uint)(rob->move_no == 0x113d);
  }
  return r;
}

int vp_after_input_set(vp_work *work)
{
  if (work->field_0xb == '\0') {
    return -1;
  }
  switch (work->field_0xb) {
  case 3:
    ((struct { char pad[0x22]; ushort a; } *)work)->a = vp_input_side_mirror(work->rob, 0x10);
    work->field_0xb = 0;
    break;
  case 4:
    ((struct { char pad[0x22]; ushort a; } *)work)->a = vp_input_side_mirror(work->rob, 0x50);
    work->field_0xb = 0;
    break;
  case 5:
  case 6:
    *(ushort *)&work->field_0x20[2] = 0;
    work->field_0xb = 0;
    break;
  case 0xb:
    work->flags = work->flags | 0x8000;
    work->field_0xb = 0;
    break;
  case 7:
    *(ushort *)&work->field_0x20[2] = 2;
    break;
  case 8:
    *(ushort *)&work->field_0x20[2] = 4;
    break;
  case 9:
    *(ushort *)&work->field_0x20[2] = 6;
    break;
  case 10:
    *(ushort *)&work->field_0x20[2] = 5;
    break;
  default:
    work->field_0xb = 0;
    return -1;
  }
  return 0;
}

int vp_after_input_release(vp_work *work)
{
  int r;

  if (work->field_0xb == 0) r = -1;
  else if ((uint)(work->field_0xb - 7) > 3) {
    work->field_0xb = 0;
    r = -1;
  }
  else {
    *(ushort *)&work->field_0x20[2] = 0;
    work->field_0xb = 0;
    r = 0;
  }
  return r;
}

bool vp_g_cancel_roll(vp_work *work)
{
  rob_rec *rob = work->rob;
  vp_action_rec *action = (vp_action_rec *)work->action;
  if (action == 0 || work->attacks != 1 || rob->frame != 6) return 0;
  if ((action->flags & 0x20) == 0) return 0;
  return (vp_rand() & 3) == 0;
}

int vp_stance_from_move(vp_work *work, int on_change)
{
  typedef struct { ushort move; ushort pad; char stance; char pad2[3]; } ent;
  ent *t;
  rob_rec *rob;
  vp_rob_snap_rec *snap;

  t = (ent *)&vp_stance_moves;
  rob = work->rob;
  snap = work->snap;
  work->stance = 0;
  if (rob->chara != 0xf) {
    return 0;
  }
  if (on_change != 0) {
    if ((((vp_action_rec *)work->action)->flags & 0x800) == 0) {
      return 0;
    }
    for (; t->move != 0xffff; t++) {
      if (rob->move_no == t->move && snap->move_no != rob->move_no) {
        work->stance = t->stance;
        return 1;
      }
    }
  }
  else {
    for (; t->move != 0xffff; t++) {
      if (rob->move_no == t->move) {
        work->stance = t->stance;
        return 1;
      }
    }
  }
  return 0;
}

int vp_stroke_run_reset(vp_work *work)
{
  int r;

  if (!(work->seq != 0)) {
    return -1;
  }
  vp_stroke_watch_reset(work);
  work->run_step = 0;
  work->flags = work->flags & 0xffff7fff;
  return 0;
}

int vp_stroke_stalled(vp_work *work)
{
  rob_rec *rob = work->rob;
  vp_rob_snap_rec *snap = work->snap;

  if ((rob->state & 0x1000) != 0 && (snap->state & 0x1000) != 0) return -1;
  return (work->idle_frames < 0xb) - 1;
}

int vp_stroke_run(vp_work *work)
{
  rob_rec *rob = work->rob;
  vp_action_rec *action = (vp_action_rec *)work->action;
  int r;
  int t;

  switch (work->run_step) {
  case 0:
    r = vp_input_seq_step(work);
    if (vp_stroke_result_watch(work) == 1) {
      work->attacks -= 1;
      if (work->attacks == 0) {
        vp_frame_disadv_calc(work, 0);
        (work->run_step)++;
        return 0;
      }
      vp_stroke_watch_reset(work);
    }
    if (r != 0) {
      vp_after_input_set(work);
      if (action != 0 && (action->flags & 0x1000) != 0) {
        vp_frame_disadv_set(work, 0);
        work->attacks = 0;
        work->run_step = 3;
        return 0;
      }
      r = vp_g_cancel_roll(work);
      if (r != 0) {
        vp_frame_disadv_set(work, -2);
        vp_input_g_tap(work, 1);
        (work->run_step)++;
        work->attacks = 0;
        return 0;
      }
      r = vp_stroke_stalled(work);
      if (r < 0) return -1;
    }
    return 0;
  case 1:
    r = vp_input_seq_step(work);
    if (r == 0) return 0;
    if (((rob->chara == 0xf) & (action != 0)) && (action->flags & 0x800) != 0) {
      t = vp_stance_from_move(work, 1);
      if (t != 0) {
        work->flags |= 4;
        work->run_step = -1;
        return 1;
      }
      t = vp_stroke_stalled(work);
      return t < 0 ? -1 : 0;
    }
    vp_after_input_set(work);
    if (rob->ctrl->continuation == 0 || (work->flags & 0x8000) != 0
        || (action != 0 && (action->flags & 0x7c0) != 0)) {
      work->run_step = 3;
      return 0;
    }
    vp_input_g_tap(work, 0);
    (work->run_step)++;
  case 2:
    if (vp_input_seq_step(work) == 0) return 0;
    (work->run_step)++;
  case 3:
    if (vp_can_act(work) == 0) return 0;
    vp_after_input_release(work);
    work->run_step = -1;
    return 1;
  default:
    return -1;
  }
}

int vp_escape_seq_pick(vp_work *work, int count)
{
  int n;
  int *list;
  int i;

  n = work->n;
  list = ((vp_chara_actions_rec *)work->opp_chara)->escape_seqs;
  work->esc_step = 0;
  for (i = 0; i < 3; i++) {
    work->esc_seqs[i] = 0;
  }
  if (count > 3) return -1;
  switch (count) {
  case 1:
    if (vp_rand() % 3 != 0) {
      work->esc_seqs[0] = list[0];
    }
    else {
      work->esc_seqs[0] = list[vp_rand() % (n - 1) + 1];
    }
    break;
  case 2:
    if (vp_rand() % 5 != 0) {
      work->esc_seqs[0] = list[0];
      work->esc_seqs[1] = list[vp_rand() % (n - 1) + 1];
    }
    else {
      work->esc_seqs[0] = list[1];
      work->esc_seqs[1] = list[vp_rand() % (n - 2) + 2];
    }
    break;
  case 3:
    work->esc_seqs[0] = list[0];
    if (vp_rand() % 3 != 0) {
      work->esc_seqs[1] = list[1];
      work->esc_seqs[2] = list[vp_rand() % (n - 2) + 2];
    }
    else {
      work->esc_seqs[1] = list[2];
      work->esc_seqs[2] = list[vp_rand() % (n - 3) + 3];
    }
    break;
  default:
    return -1;
  }
  return 0;
}

int vp_escape_seq_run(vp_work *work)
{
  int *slot;
  int i;
  int seq;

  switch (work->esc_step) {
  case 0:
    for (i = 0; i < 3; i++) {
      slot = &work->esc_seqs[i];
      seq = *slot;
      if (seq != 0) {
        *slot = 0;
        break;
      }
    }
    if (seq == 0) {
      work->esc_step = 0xff;
      return 1;
    }
    work->esc_step = work->esc_step + '\x01';
    work->flags = work->flags | 4;
    vp_input_seq_start(work,(short *)seq);
    break;
  case 1:
    if (vp_input_seq_step(work) == 1) {
      work->esc_step = 0;
    }
    break;
  default:
    return -1;
  }
  return 0;
}

int vp_escape_input_pick(vp_work *work)
{
  short *tbl;
  short *out;
  short *q;
  int n;
  int cnt;
  int i;

  tbl = ((vp_chara_actions_rec *)work->opp_chara)->escape_inputs;
  out = (short *)&work->field_0x20[4];
  n = work->level_params->yoke2_escapes - 1;
  cnt = 0;
  for (i = 0; i < 3; i++) {
    out[i] = 0;
  }
  q = tbl;
  while (*q++ != 0) {
    cnt++;
  }
  if ((n <= 0) | (cnt <= 1)) {
    return 0;
  }
  if (n == 1) {
    *out = tbl[vp_rand() % cnt];
  }
  else {
    *out++ = tbl[0];
    *out = tbl[vp_rand() % (cnt - 1) + 1];
  }
  return n;
}

int vp_exception_check(vp_work *work)
{
  typedef struct { ushort move; short pad; int exc_mode; } ent;
  rob_rec *rob = work->rob;
  vp_rob_snap_rec *snap = work->snap;
  ent *e;

  for (e = (ent *)&vp_exc_moves; e->move != 0xffff; e++) {
    if (rob->move_no == e->move && snap->move_no != rob->move_no) {
      work->exc_mode = e->exc_mode;
      return 1;
    }
  }
  if (rob->chara == '\x0f' && (rob->flags24 & 1) != 0 &&
      (snap->flags24 & 1) == 0) {
    work->exc_mode = 2;
    return 1;
  }
  return 0;
}

bool vp_guard_success_check(vp_work *work)
{
  if (work->other->rob->coli->field_0x3c[0] & 0x20) return 1;
  return 0;
}

int vp_hit_check(vp_work *work)
{
  int r;
  rob_rec *opp;

  opp = work->other->rob;
  if (!(((opp->coli->field_0x3c[0] & 0x81) == 0) ||
     (r = 1, opp->action.part.type == '\x0e'))) return r;
  r = 0;
  return r;
}

int vp_thrown_check(vp_work *work)
{
  int r;

  if (!(((work->rob->state & 0x200000) == 0) ||
     (r = 1, work->rob->frame != 0xe))) return r;
  r = 0;
  return r;
}

int vp_throw_escape_check(vp_work *work)
{
  int r;

  if (!(((work->rob->flags28 & 0x8000000) == 0) ||
     (r = 1, (work->snap->flags28 & 0x8000000) != 0))) return r;
  r = 0;
  return r;
}

int vp_opp_attack_over(vp_work *work)
{
  int r;
  rob_rec *opp;

  opp = work->other->rob;
  if (opp->frame > opp->active_end) {
    r = vp_opp_can_follow_up(work);
    if (r == 0) {
      return 1;
    }
    if (opp->frame + 10 >= (int)opp->recovery_end) {
      return 1;
    }
  }
  return 0;
}

bool vp_opp_not_attacking(vp_work *work)
{
  rob_rec *opp = work->other->rob;
  int t = opp->move_flags & 0x4000;
  return t == 0;
}

int vp_opp_attack_level(vp_work *work)
{
  rob_rec *opp;
  int level;

  level = -1;
  opp = work->other->rob;
  if ((((opp->move_flags & 0x4000) != 0) &&
      (opp->frame < opp->active_end)) && (opp->attack_level != 0)) {
    level = (int)opp->attack_level;
  }
  return level;
}

int vp_opp_attack_yokeable(vp_work *work)
{
  vp_work *other;
  rob_rec *opp;
  int v;

  other = work->other;
  opp = other->rob;
  if ((opp->move_flags & 0x4000) == 0) {
    return 0;
  }
  if ((((other->snap->flags24 & 0x100) != 0) &&
      ((opp->flags24 & 0x100) == 0)) && ((opp->field_0x30 & 8) == 0)) {
    return 1;
  }
  if (((opp->field_0x328 & 0x600000) != 0) && ((opp->field_0x30 & 8) == 0)) {
    v = (int)opp->attack_level;
    if ((opp->field_0x328 & 0x200000) != 0) {
      switch (v) {
      case 2:
      case 4:
      case 0xc:
      case 0xe:
        return 1;
      }
    }
    else if ((opp->field_0x328 & 0x400000) != 0) {
      switch (v) {
      case 3:
      case 5:
      case 0xd:
        return 1;
      }
    }
  }
  return 0;
}

int vp_opp_attack_cut_roll(vp_work *work)
{
  rob_rec *rob = work->rob;
  rob_rec *opp = rob->opp;
  if ((opp->move_flags & 0x4000) == 0) return 0;
  if (!(rob->opp_dist < 1.8)) return 0;
  if ((opp->flags24 & 0x100) != 0 && (opp->field_0x30 & 8) != 0 && vp_frame_count % 0x23 == 0) return 1;
  return 0;
}

uint vp_guard_height_set(vp_work *work)
{
  char c;
  uint r;
  rob_rec *opp;

  r = 0;
  c = work->opp_level;
  opp = work->other->rob;
  if ((work->flags & 0x30) == 0x30) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libenemy/src/vp_main.c",0xa37,"assertion `%s\' failed.\n","0"
            );
    work->flags = work->flags & 0xffffffdf;
  }
  if (c < 1) {
    return 0;
  }
  switch(c) {
  case 2:
  case 4:
  case 12:
  case 14:
    r = 0x10;
    break;
  case 1:
  case 7:
    if ((opp->field_0x328 & 0x200000) == 0) break;
  case 3:
  case 5:
  case 13:
    r = 0x20;
    break;
  }
  if (r != 0) {
    work->flags = work->flags & 0xffffffcf | r;
  }
  return r;
}

int vp_opp_can_follow_up(vp_work *work)
{
  rob_rec *opp;
  ushort *p;
  vp_work *other;

  p = (ushort *)vp_no_follow_moves;
  other = work->other;
  opp = other->rob;
  if ((opp->move_flags & 0x4000) == 0) return 0;
  if (other->move == 0) return 0;
  for (; *p != 0xffff; p++) {
    if (opp->move_no == *p) return 0;
  }
  return 1;
}

int vp_intentional_counter_check(vp_work *work)
{
  rob_rec *rob;
  rob_rec *opp;
  vp_level_params_rec *params;

  rob = work->rob;
  opp = rob->opp;
  params = work->level_params;
  work->flags = work->flags & 0xfff7ffff;
  if ((vp_mode_flag != 1) && ((params->flags & 8) == 0)) {
    return 0;
  }
  if (((opp->move_flags & 0x4000) != 0) &&
     (rob->opp_dist < 1.8)) {
    if (opp->action.part.type == '\x0e') {
      return 0;
    }
    if (opp->frame + 4 == (int)opp->active_start) {
      work->flags = work->flags | 0x80000;
      return 1;
    }
  }
  return 0;
}

int vp_common_attack_roll(vp_work *work)
{
  rob_rec *rob;
  rob = work->rob;
  if (vp_rand() % 100 >= ((vp_chara_actions_rec *)work->chara)->common_attack_rate) return 0;
  if (rob->chara == '\v') {
    if (vp_frame_count & 1) return (int)vp_common_action_get(6);
    return (int)vp_common_action_get(7);
  }
  else {
    if (vp_frame_count & 1) return (int)vp_common_action_get(4);
    return (int)vp_common_action_get(5);
  }
}

int vp_defend_disadv_large(vp_work *work)
{
  vp_level_params_rec *params;
  char a;
  char b;

  params = work->level_params;
  a = params->guard2_escapes;
  b = params->yoke3_escapes;
  if ((a != 0) & (b != 0)) {
    if (vp_rand() & 1) vp_guard_start(work,2,1,0x10);
    else vp_yoke_start(work,3);
  }
  else if (a != 0) {
    vp_guard_start(work,2,1,0x10);
  }
  else if (b != 0) {
    if (vp_rand() & 1) vp_yoke_start(work,3);
    else vp_guard_start(work,1,1,0);
  }
  else if (params->yoke2_escapes != 0) {
    switch (vp_rand() % 3) {
    case 0: vp_yoke_start(work,2); break;
    case 1: vp_yoke_start(work,1); break;
    case 2: vp_guard_start(work,1,1,0); break;
    }
  }
  else {
    if ((params->flags & 1) && (vp_rand() & 1)) vp_yoke_start(work,1);
    else vp_guard_start(work,1,1,0);
  }
  return 1;
}

int vp_defend_disadv_mid(vp_work *work)
{
  vp_level_params_rec *params;

  params = work->level_params;
  if (params->yoke3_escapes != 0) {
    if ((vp_rand() & 3) != 0) vp_yoke_start(work,3);
    else vp_guard_start(work,1,1,0);
  }
  else if (params->yoke2_escapes != 0) {
    switch (vp_rand() % 3) {
    case 0:
      vp_yoke_start(work,2);
      break;
    case 1:
      vp_yoke_start(work,1);
      break;
    case 2:
      vp_guard_start(work,1,1,0);
      break;
    }
  }
  else if ((params->flags & 1) != 0 && (vp_rand() & 1) != 0) {
    vp_yoke_start(work,1);
  }
  else {
    vp_guard_start(work,1,1,0);
  }
  return 1;
}

int vp_defend_disadv_small(vp_work *work)
{
  vp_level_params_rec *params;
  char b5;
  int t;

  params = work->level_params;
  b5 = params->yoke3_escapes;
  switch (work->rob->chara) {
  case 3:
  case 8:
  case 10:
  case 0xb:
  case 0xe:
    if (vp_rand() % 100 < (char)(work->traits[6] / 2)) {
      vp_state_change(work,4);
      break;
    }
  case 0xc:
    if (vp_stance_get(work) == 0 &&
        vp_rand() % 100 < (char)(work->traits[6] / 2)) {
      vp_state_change(work,4);
      break;
    }
  default:
    if (b5 != 0) {
      switch (vp_rand() % 7) {
      case 0:
        vp_guard_start(work,1,1,0);
        break;
      case 1:
        work->action = (int)vp_common_action_get(1);
        vp_state_change(work,5);
        break;
      case 2:
        vp_state_change(work,3);
        break;
      case 3:
        t = vp_common_attack_roll(work);
        if (t != 0) {
          work->action = t;
          vp_state_change(work,5);
          break;
        }
      default:
        vp_yoke_start(work,3);
        break;
      }
    }
    else if (params->flags & 1) {
      switch (vp_rand() % 5) {
      case 3:
        vp_state_change(work,3);
        break;
      case 4:
        t = vp_common_attack_roll(work);
        if (t != 0) {
          work->action = t;
          vp_state_change(work,5);
          break;
        }
      case 0:
        vp_yoke_start(work,1);
        break;
      case 1:
        vp_guard_start(work,1,1,0);
        break;
      case 2:
        work->action = (int)vp_common_action_get(1);
        vp_state_change(work,5);
        break;
      }
    }
    else if (vp_rand() & 1) {
      vp_guard_start(work,1,1,0);
    }
    else {
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work,5);
    }
    break;
  }
  return 1;
}

int vp_defend_disadv_none(vp_work *work)
{
  int r;

  if (!((work->level_params->flags & 2) == 0)) {
    vp_guard_start(work,1,1,0x20);
    r = 1;
  }
  else {
    r = vp_defend_disadv_small(work);
  }
  return r;
}

int vp_defend_far(vp_work *work)
{
  int r;

  r = vp_approach_wanted(work);
  if (!(r == 0)) {
    vp_state_change(work,2);
    work->flags = work->flags | 0x200000;
  }
  else {
    vp_guard_start(work,1,1,0);
  }
  return 1;
}

int vp_user_renkei_combo_pick(vp_work *work)
{
  byte level;
  rob_rec *rob;
  rob_rec *opp;
  vp_renkei_rec *renkei;
  vp_renkei_combo_rec *combo;
  int n;
  int k;
  vp_renkei_work_rec *rw;
  level = *(byte *)work->traits;
  rob = work->rob;
  opp = rob->opp;
  renkei = (vp_renkei_rec *)work->renkei;
  work->flags = work->flags & 0xfffcffff;
  if (level <= 9) n = 4;
  else if (level <= 19) n = 6;
  else n = 8;
  if (renkei == 0 || rob->opp_dist > 1.8 || (k = vp_rand()) % n != 0) return 0;
  if (vp_frame_count & 1) {
    if (renkei->steps[0].stroke == 0) return 0;
    if (rob->chara == 3 && renkei->attack_rate > rob->attack_rate) return 0;
    if (vp_stance_get(work) != renkei->stance) return 0;
    work->flags = work->flags | 0x10000;
    work->renkei_step = (int)renkei->steps;
  }
  else {
    rw = (vp_renkei_work_rec *)work->renkei_work;
    combo = &renkei->combo;
    if (rw->combo_action.stroke == 0 || rw->combos[0].strokes == 0) return 0;
    if ((((int)rw->combos[0].chara_mask >> opp->chara) & 1) == 0) return 0;
    if (rob->chara == 3 && combo->attack_rate > rob->attack_rate) return 0;
    if (vp_stance_get(work) != combo->stance) return 0;
    work->flags = work->flags | 0x20000;
    work->action = (int)&rw->combo_action;
  }
  return 1;
}

void vp_renkei_action_build(vp_work *work)
{
  vp_renkei_work_rec *rw;
  vp_renkei_step_rec *step;

  step = (vp_renkei_step_rec *)work->renkei_step;
  rw = (vp_renkei_work_rec *)work->renkei_work;
  rw->step_action.flags = 0;
  rw->step_action.drank = 0;
  rw->step_action.frames = 100;
  rw->step_action.stroke = step->stroke;
  rw->step_action.combos = 0;
  work->action = (int)&rw->step_action;
  return;
}

byte vp_renkei_next_check(vp_work *work)
{
  byte ok;
  vp_renkei_step_rec *q;

  ok = 0;
  switch (((vp_renkei_step_rec *)work->renkei_step)->next_cond) {
  case 0:
    break;
  case 1:
    ok = 1;
    break;
  case 2:
    if ((work->stroke & 0x38) != 0) ok = 1;
    break;
  case 3:
    if ((work->stroke & 1) != 0) ok = 1;
    break;
  case 4:
    if ((work->stroke & 0x100) != 0) ok = 1;
    break;
  case 5:
    if ((work->stroke & 0x200) != 0) ok = 1;
    break;
  case 6:
    if ((work->stroke & 0x40) != 0) ok = 1;
    break;
  case 7:
    if ((work->stroke & 2) != 0) ok = 1;
    break;
  }
  if (ok) {
    q = (vp_renkei_step_rec *)work->renkei_step;
    work->renkei_step = (int)(q + 1);
    if (q[1].stroke == 0) ok = 0;
  }
  return ok;
}

void vp_ukemi_pick(vp_work *work)
{
  rob_rec *rob;
  char buf [3];

  memcpy(buf,vp_ukemi_types,3);
  rob = work->rob;
  if ((vp_rand() % 100 < work->level_params->ukemi_pick_rate) &&
     ((rob->state & 0x800000) != 0)) {
    if ((rob->relation_flags & 1) != 0) {
      work->ukemi = (vp_rand() & 1 ^ 1) + 2;
    }
    else {
      work->ukemi = 1;
    }
  }
  else {
    work->ukemi = buf[(uint)vp_rand() % 3];
  }
  return;
}

int vp_approach_wanted(vp_work *work)
{
  rob_rec *rob;
  rob_rec *opp;
  int d;
  char *traits;
  int r;
  rob = work->rob;
  opp = rob->opp;
  d = rob->life;
  d -= opp->life;
  traits = work->traits;
  r = vp_rand() % 100;
  if (r < traits[1]) return 1;
  if (d <= 29) return 1;
  if (opp->chara == 3 && opp->attack_rate > 9) return 1;
  return 0;
}

int vp_rob_move_early_listed(rob_rec *rob)
{
  ushort *p = (ushort *)vp_wait_cut_moves;
  if (rob->frame > 30)
    return 0;
  for (; *p != 0xffff; p++) {
    if (rob->move_no == *p)
      return 1;
  }
  return 0;
}

int vp_chr3_drink_attack(vp_work *work)
{
  char c;
  rob_rec *rob;
  rob_rec *opp;
  int a;
  int b;

  rob = work->rob;
  opp = rob->opp;
  c = opp->stance;
  if (opp->chara != '\x03') {
    return -1;
  }
  a = c != '\x01';
  b = c != '\x03';
  if (a & b) {
    return -1;
  }
  if (vp_stance_get(work) > 0 && rob->chara != '\f') {
    work->action = (int)vp_common_action_get(9);
  }
  else {
    if (1.8 < rob->opp_dist) {
      vp_state_change(work,2);
      work->flags = work->flags | 0x200000;
      return 1;
    }
    opp = (rob_rec *)work->chara;
    work->action = ((vp_chara_actions_rec *)opp)->drink_attack;
  }
  vp_state_change(work,5);
  return 1;
}

void vp_hame_count_add(vp_work *work)
{
  vp_hame_rec *ent;
  int i;

  ent = work->log;
  if (0x13 < *(byte *)work->traits && *(ushort *)work->field_0x20 != 0xffff) {
    for (i = 0; i < 0x40; i++, ent++) {
      if (ent->move == *(ushort *)work->field_0x20) {
        ent->count = ent->count + 2;
        return;
      }
      if (ent->move == 0xffff) {
        ent->move = *(ushort *)work->field_0x20;
        ent->count = 2;
        return;
      }
    }
  }
}

void vp_hame_count_decay(vp_work *work)
{
  vp_hame_rec *ent;
  short *count;
  int i;

  ent = work->log;
  if (*(ushort *)work->field_0x20 != 0xffff) {
    for (i = 0; i < 0x40; i++, ent = ent + 1) {
      if (ent->move == *(ushort *)work->field_0x20) {
        count = &ent->count;
        if (*count > 0) {
          *count = *count - 1;
        }
        return;
      }
      if (ent->move == 0xffff) {
        return;
      }
    }
  }
}

uint vp_hame_known(vp_work *work)
{
  vp_hame_rec *ent;
  int i;

  ent = work->log;
  for (i = 0; i < 0x40; i++, ent++) {
    if (ent->move == *(ushort *)work->field_0x20) {
      if (ent->count < 1) {
        return 0;
      }
      if (ent->count < 8) {
        return vp_frame_count & 1;
      }
      return 1;
    }
    if (ent->move == 0xffff) {
      break;
    }
  }
  return 0;
}

void vp_think(vp_work *work)
{
  int r;
  ushort *input;
  rob_rec *rob;

  rob = work->rob;
  rob->ctrl->cpu_buttons = 0;
  if ((work->flags & 2) == 0) {
    return;
  }
  vp_edge_flags_update(work);
  work->mode_frames = work->mode_frames + 1;
  if ((rob->state & 0x81) != 0) {
    work->idle_frames = 0;
  }
  else {
    work->idle_frames = work->idle_frames + 1;
  }
  if (vp_hit_check(work) != 0) vp_state_restart(work,0x12);
  else if (vp_thrown_check(work) != 0) vp_state_restart(work,0x15);
  else if (vp_throw_escape_check(work) != 0) vp_state_restart(work,0x10);
  else if (vp_guard_success_check(work) != 0) vp_state_restart(work,0xd);
  else if (vp_exception_check(work) != 0) vp_state_restart(work,0x19);
  do {
    r = (*vp_mode_funcs[work->mode])(work);
  } while (r == 1);
  if (*(ushort *)&work->field_0x20[2] != 0) {
    input = &rob->ctrl->cpu_buttons;
    *input = *input | *(ushort *)&work->field_0x20[2];
  }
  return;
}

int vp_mode_no_act(vp_work *work)
{
  int mode;
  if (work->action != 0) {
    mode = 5;
  }
  else mode = 1;
  vp_state_change(work,mode);
  return 1;
}

int vp_mode_act_select(vp_work *work)
{
  rob_rec *rob;
  rob_rec *opp;

  rob = work->rob;
  opp = rob->opp;
  if (vp_can_act(work) == 0) return 0;
  if ((rob->state & 0x800) != 0) {
    vp_state_change(work,0x17);
    return 1;
  }
  if ((opp->state & 0x400) != 0 && (opp->field_0x118 & 1) != 0) {
    vp_state_change(work,7);
    return 1;
  }
  if ((opp->state & 0x400) != 0 && (opp->field_0x118 & 1) == 0) {
    vp_state_change(work,8);
    return 1;
  }
  if ((opp->state & 0x10000) != 0) {
    vp_state_change(work,7);
    return 1;
  }
  if ((opp->state & 0x40000) != 0) {
    vp_state_change(work,8);
    return 1;
  }
  if ((work->flags & 0x400) != 0 &&
      vp_rand() % 100 >= work->traits[5]) {
    vp_state_change(work,3);
    return 1;
  }
  if (vp_rand() % 100 < work->level_params->disadv_flip_rate) {
    work->flags = work->flags | 0x4000;
    work->disadv = -work->disadv;
  }
  else {
    work->flags = work->flags & 0xffffbfff;
  }
  if (work->disadv == -1) {
    work->disadv = 0;
  }
  if (work->disadv != 0 ? work->disadv < 0 : (vp_rand() & 1) != 0) {
    vp_state_change(work,4);
    return 1;
  }
  vp_state_change(work,9);
  return 1;
}

int vp_mode_move_f(vp_work *work)
{
  int kind;
  vp_level_params_rec *params;
  uchar c;

  switch (work->field_0x10) {
  case 0:
    kind = (vp_rand() & 1 ^ 1) + 1;
    vp_input_dash(work,kind);
    work->field_0x6 = kind;
    work->field_0x7 = vp_rand() % 10 + 10;
    work->field_0x10 = work->field_0x10 + 1;
  case 1:
    if (vp_input_seq_step(work) == 0) break;
    work->field_0x10 = work->field_0x10 + 1;
  case 2:
    c = (work->field_0x7)--;
    if (c != 0) {
      if ((work->flags & 0x200000) == 0) break;
      if (vp_opp_attack_level(work) < 0) break;
      params = work->level_params;
      work->flags = work->flags & 0xffdfffff;
      if (vp_rand() % 100 >= params->guard_rate) break;
      vp_guard_start(work,1,0,0);
      return 1;
    }
    work->field_0x10 = work->field_0x10 + 1;
  case 3:
    if (vp_can_act(work) == 0) break;
    work->flags = work->flags & 0xffdfffff;
    vp_frame_disadv_set(work,-2);
    vp_state_change(work,1);
    return 1;
  }
  return 0;
}

int vp_mode_move_b(vp_work *work)
{
  int kind;
  uchar c;

  switch(work->field_0x10) {
  case 0:
    kind = (vp_rand() & 1 ^ 1) + 3;
    vp_input_dash(work,kind);
    work->field_0x6 = kind;
    work->field_0x7 = vp_rand() % 10 + 10;
    work->field_0x10 += 1;
  case 1:
    if (vp_input_seq_step(work) == 0) break;
    work->field_0x10 += 1;
  case 2:
    c = (work->field_0x7)--;
    if (c != 0) break;
    work->field_0x10 += 1;
  case 3:
    if (vp_can_act(work)) {
      vp_frame_disadv_set(work,0xfffffffe);
      vp_state_change(work,1);
      return 1;
    }
  }
  return 0;
}

int vp_mode_attack(vp_work *work)
{
  rob_rec *rob = work->rob;
  int r;

  r = (int)vp_punish_pick(work);
  if (r != 0) {
    work->action = r;
    vp_state_change(work, 5);
    return 1;
  }
  if (vp_stance_allows_approach(work)) {
    if (3.0 < rob->opp_dist && vp_approach_wanted(work)) {
      vp_state_change(work, 2);
      work->flags |= 0x200000;
      return 1;
    }
    if (work->disadv < 0) {
      if (vp_rand() % 100 < work->traits[1]) {
        if (vp_rand() % 5 == 0) {
          vp_state_change(work, 2);
          return 1;
        }
      }
    }
  }
  if (vp_user_renkei_combo_pick(work)) {
    vp_state_change(work, 5);
    return 1;
  }
  r = vp_chr3_drink_attack(work);
  if (r != -1) return r;
  r = vp_attack_list_get(work);
  if (r == 0) {
    r = vp_stance_get(work);
    if ((r <= 0 || rob->chara == 12) && (vp_frame_count & 1)) {
      vp_yoke_start(work, 1);
    } else {
      vp_guard_start(work, 1, 1, 0);
    }
    return 1;
  }
  *(int *)&work->action = vp_attack_pick(work, (vp_rank_action_rec *)r);
  vp_state_change(work, 5);
  return 1;
}

int vp_mode_stroke(vp_work *work, int unused)
{
  vp_action_rec *action;
  vp_stroke_rec *ent;
  int r;
  vp_combo_entry_rec *combo;
  if (work->field_0x10 == 0 && (work->flags & 0x10000) != 0) {
    vp_renkei_action_build(work);
  }
  action = (vp_action_rec *)work->action;
  ent = action->stroke;
  switch (work->field_0x10) {
  case 0:
    if ((work->flags & 0x80000) != 0) {
      work->flags &= 0xfff7ffff;
      work->react = 0;
    }
    else {
      work->react = work->level_params->stroke_delay + vp_rand() % 10;
    }
    if (action == vp_common_action_get(1)) {
      work->flags |= 0x1000;
    }
    else {
      work->flags &= ~0x1000;
    }
    if (vp_mode_history_last(work) != 13) {
      *(short *)work->field_0x20 = -1;
    }
    (work->field_0x10)++;
  case 1:
    if (--work->react > 0) break;
    work->attacks = ent->attacks;
    work->field_0xb = ent->after_input;
    vp_stroke_result_clear(work);
    vp_input_seq_start(work, ent->seq);
    vp_stroke_run_reset(work);
    (work->field_0x10)++;
    break;
  case 2:
    r = vp_stroke_run(work);
    if (r == 0) break;
    if (r == -1) {
      vp_state_restart(work, 1);
      return 1;
    }
    vp_hame_count_decay(work);
    (work->field_0x10)++;
  case 3:
    if ((work->flags & 0x10000) != 0) {
      if (vp_renkei_next_check(work) != 0) {
        vp_state_change(work, 5);
        return 1;
      }
    }
    else {
      if ((work->stroke & 2) != 0) {
        vp_frame_disadv_set(work, -2);
      }
      if ((work->stroke & 0x200) != 0) {
        vp_state_change(work, 1);
        return 1;
      }
      if ((work->stroke & 0x100) != 0) {
        vp_state_change(work, 1);
        return 1;
      }
      if (action->combos == 0) {
        vp_state_change(work, 1);
        return 1;
      }
      if ((work->rob->flags28 & 0xd0000) != 0) {
        vp_state_change(work, 1);
        return 1;
      }
      for (combo = action->combos; combo->strokes != 0; combo++) {
        if (vp_combo_entry_match(work, combo) != 0) {
          work->combo = (int)combo->strokes;
          vp_state_change(work, 6);
          return 1;
        }
      }
    }
    vp_state_change(work, 1);
    return 1;
  }
  return 0;
}

int vp_mode_combo(vp_work *work)
{
  vp_level_params_rec *params;
  vp_stroke_rec *ent;
  int r;
  params = work->level_params;
  ent = *(vp_stroke_rec **)work->combo;
  if (ent == 0) {
    vp_frame_disadv_set(work, -1);
    vp_state_change(work, 1);
    return 1;
  }
  switch (work->field_0x10) {
  case 0:
    if (vp_rand() % 100 >= params->combo_rate) {
      vp_frame_disadv_set(work, 2);
      vp_state_change(work, 1);
      return 1;
    }
    work->attacks = ent->attacks;
    work->field_0xb = ent->after_input;
    vp_stroke_result_clear(work);
    vp_input_seq_start(work, ent->seq);
    vp_stroke_run_reset(work);
    work->field_0x10 += 1;
    break;
  case 1:
    r = vp_stroke_run(work);
    if (r == 0) break;
    if (r == -1) {
      vp_state_restart(work, 1);
      return 1;
    }
    work->field_0x10 += 1;
  case 2:
    if ((work->stroke & 1) != 0 ||
        ((work->stroke & 2) != 0 && (work->flags & 0x20000) != 0) ||
        vp_rand() % 100 < params->combo_rate2) {
      work->combo += 4;
      vp_state_change(work, 6);
      return 1;
    }
    vp_frame_disadv_set(work, 2);
    vp_state_change(work, 1);
    return 1;
  }
  return 0;
}

int vp_mode_ukemi_attack(vp_work *work)
{
  vp_frame_disadv_set(work,-1);
  vp_state_change(work,4);
  return 0;
}

int vp_mode_rise_attack(vp_work *work)
{
  int r;
  uint roll;
  
  if (!(work->rob->opp_dist <= 3.0)) {
    vp_state_change(work,2);
    r = 1;
  }
  else {
    vp_frame_disadv_set(work,2);
    roll = vp_rand();
    if (!((roll & 3) == 0)) {
      vp_guard_start(work,1,1,0);
    }
    else {
      vp_state_change(work,1);
    }
    r = 0;
  }
  return r;
}

int vp_mode_defense(vp_work *work)
{
  int last_mode;
  vp_level_params_rec *params;
  rob_rec *rob;
  int t;

  last_mode = vp_mode_history_last(work);
  params = work->level_params;
  rob = work->rob;
  if (rob->chara != 12) {
    if (vp_stance_get(work) > 0) {
      vp_state_change(work,4);
      return 1;
    }
  }
  if ((rob->relation_flags & 1) != 0) {
    if (rob->opp_dist < 1.8) {
      vp_guard_start(work,1,1,0);
    }
    else {
      vp_yoke_start(work,1);
    }
    return 1;
  }
  if ((work->disadv < 8 || last_mode == 22) &&
      (t = vp_opp_attack_level(work)) != -1 &&
      vp_rand() % 100 < (char)params->guard_rate) {
    work->opp_level = t;
    vp_guard_height_set(work);
    vp_guard_start(work,1,0,0);
    return 1;
  }
  if (last_mode == 22 && rob->opp_dist < 1.8) {
    switch (vp_rand() % 4) {
    case 0:
    case 1:
      work->flags = work->flags | 0x400000;
      vp_guard_start(work,1,1,0);
      break;
    case 2:
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work,5);
      break;
    case 3:
      if ((params->flags & 1) != 0) {
        vp_yoke_start(work,1);
      }
      else {
        vp_guard_start(work,1,1,0);
      }
      break;
    }
    return 1;
  }
  if (last_mode == 15) {
    if (rob->opp_dist < 1.8) {
      if ((vp_rand() & 1) != 0) {
        work->action = (int)vp_common_action_get(1);
        vp_state_change(work,5);
      }
      else {
        vp_guard_start(work,1,1,0);
      }
      return 1;
    }
    if (rob->opp_dist < 3.0) {
      vp_guard_start(work,1,1,0);
      return 1;
    }
    return vp_defend_far(work);
  }
  if (last_mode == 5 && (work->stroke & 0x200) != 0) {
    if (rob->opp_dist < 1.8) {
      switch (vp_rand() % 5) {
      case 0:
      case 1:
        if (params->yoke3_escapes != 0) {
          vp_yoke_start(work,3);
        }
        else if ((params->flags & 1) != 0) {
          vp_yoke_start(work,1);
        }
        else {
          vp_guard_start(work,1,1,0);
        }
        break;
      case 2:
      case 3:
        vp_guard_start(work,1,1,0);
        break;
      case 4:
        vp_state_change(work,3);
        break;
      }
      return 1;
    }
    if (rob->opp_dist < 3.0) {
      vp_guard_start(work,1,1,0);
      return 1;
    }
    return vp_defend_far(work);
  }
  if (last_mode == 13 || (last_mode == 5 && (work->stroke & 0x100) != 0)) {
    if (work->disadv > 13) {
      return vp_defend_disadv_large(work);
    }
    if (work->disadv > 7) {
      return vp_defend_disadv_mid(work);
    }
    if (work->disadv > 1) {
      return vp_defend_disadv_small(work);
    }
    return vp_defend_disadv_none(work);
  }
  if (last_mode == 19) {
    if ((work->flags & 0x2000) == 0) {
      if (work->disadv > 1) {
        return vp_defend_disadv_small(work);
      }
      return vp_defend_disadv_none(work);
    }
    if ((vp_rand() & 1) != 0) {
      vp_guard_start(work,1,1,0);
    }
    else {
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work,5);
    }
    return 1;
  }
  if (last_mode == 21 && (work->flags & 0x2000) != 0) {
    if ((vp_rand() & 1) != 0) {
      vp_guard_start(work,1,1,0);
    }
    else {
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work,5);
    }
    return 1;
  }
  if (rob->opp_dist > 3.0) {
    return vp_defend_far(work);
  }
  if (rob->opp_dist > 1.8) {
    switch (vp_rand() % 4) {
    case 0:
      vp_state_change(work,3);
      break;
    case 1:
      if ((params->flags & 1) != 0) {
        vp_yoke_start(work,1);
      }
      else {
        vp_guard_start(work,1,1,0);
      }
      break;
    default:
      vp_guard_start(work,1,1,0);
      break;
    }
    return 1;
  }
  switch (vp_rand() % 6) {
  case 0:
  case 1:
    vp_guard_start(work,1,1,0);
    break;
  case 2:
  case 3:
    work->action = (int)vp_common_action_get(1);
    vp_state_change(work,5);
    break;
  case 4:
    vp_state_change(work,3);
    break;
  case 5:
    if ((params->flags & 1) != 0) {
      vp_yoke_start(work,1);
    }
    else {
      vp_guard_start(work,1,1,0);
    }
    break;
  }
  return 1;
}

int vp_mode_guard_start(vp_work *work)
{
  int v;
  int new_var;
  switch (work->field_0x10)
  {
    case 0:
    {
      short *q = &work->field_0x18;
      new_var = 30;
      v = ((int) vp_rand()) % 40;
      *q = v + new_var;
      if (((work->level_params->flags) & 8) != 0)
      {
        *q = v + 90;
      }
    }
      if (((work->flags) & 0x30) == 0)
    {
      if ((((int) vp_rand()) % 10) < 3)
      {
        work->flags = (work->flags) | 0x20;
      }
      else
      {
        work->flags = (work->flags) | 0x10;
      }
    }
      if (((work->disadv) > 8) && (work->gnt == 2))
    {
      if (work || new_var)
      {
        vp_escape_seq_pick(work, work->level_params->guard2_escapes);
        work->field_0x10 = work->field_0x10 + 1;
      }
      else
      {
        vp_escape_seq_pick(work, work->level_params->guard2_escapes);
        work->field_0x10 = work->field_0x10 + 1;
      }
    }
    else
    {
      goto done;
    }

    case 1:
      if (vp_escape_seq_run(work) == 0)
    {
      break;
    }
      done:
    vp_state_change(work, 0xb);

      return 1;

  }

  return 0;
}

int vp_mode_guard_wait(vp_work *work)
{
  rob_rec *rob;
  rob_rec *opp;
  vp_level_params_rec *params;
  int r;
  int s;

  rob = work->rob;
  opp = rob->opp;
  params = work->level_params;
  switch (work->field_0x10) {
  case 0:
    r = vp_opp_attack_level(work);
    if (r >= 0) {
      work->opp_level = r;
      {
        short *t = &work->guard_react;
        char b = params->guard_react;
        s = vp_rand() % 10 + b;
        *t = s - 5;
        if (work->flags & 0x400000) {
          work->flags &= ~0x400000;
          *t = s - 10;
        }
      }
      (work->field_0x10)++;
    }
    else {
      if (rob->opp_dist < 3.0 && (params->flags & 4) && vp_rand() % 10 == 0) {
        work->flags ^= 0x30;
      }
      if (rob->opp_dist > 3.0 && vp_approach_wanted(work) && work->field_0x18 > 10) {
        work->field_0x18 = 10;
      }
      if (vp_rob_move_early_listed(opp) && *(uchar *)work->traits > 15 && work->field_0x18 > 1) {
        work->field_0x18 = 1;
      }
      if (--work->field_0x18 < 1) {
        work->flags &= ~0x400000;
        vp_state_change(work, 12);
      }
    }
    vp_input_guard(work);
    break;
  case 1:
    if (vp_intentional_counter_check(work)) {
      work->flags &= ~0x30;
      work->gnt = 0;
      switch (rob->chara) {
      case 3:
      case 5:
      case 10:
        work->action = (int)vp_common_action_get(13);
        break;
      case 12:
        work->action = (int)vp_common_action_get(14);
        break;
      case 0:
      case 18:
      default:
        work->action = (int)vp_common_action_get(12);
        break;
      }
      vp_state_change(work, 5);
      return 0;
    }
    if (work->opp_level == 1 && (work->flags & 0x20) && (rob->state & 0x8000) &&
        rob->opp_dist < 1.5 && opp->frame < 10) {
      work->flags &= ~0x30;
      work->gnt = 0;
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work, 5);
      return 0;
    }
    r = vp_opp_attack_level(work);
    if (r >= 0 && work->opp_level != r) {
      work->opp_level = r;
      work->guard_react = params->guard_react + vp_rand() % 10 - 5;
    }
    if (--work->guard_react < 1) {
      vp_guard_height_set(work);
    }
    if (vp_opp_attack_cut_roll(work) && vp_rand() % 100 < params->cut_rate) {
      work->flags &= ~0x30;
      work->gnt = 0;
      work->action = (int)vp_common_action_get(1);
      vp_state_change(work, 5);
      return 0;
    }
    if (vp_opp_attack_yokeable(work) && vp_rand() % 100 < params->cut_rate) {
      if (opp->field_0x328 & 0x100) {
        work->yoke_side = 1;
      }
      else if (opp->field_0x328 & 0x200) {
        work->yoke_side = 2;
      }
      uint f = work->flags & ~0x30;
      work->gnt = 0;
      work->flags = f | 0x40000;
    }
    if ((work->flags & 0x40000) && opp->frame > opp->active_start - 10) {
      work->flags &= ~0x40000;
      vp_yoke_start(work, 1);
      return 0;
    }
    if (vp_opp_attack_over(work)) {
      if (work->rob->opp_dist < 1.8)
        vp_frame_disadv_set(work, -7);
      else
        vp_frame_disadv_set(work, 0);
      (work->field_0x10)++;
    }
    if (vp_opp_not_attacking(work)) {
      vp_frame_disadv_set(work, 0);
      (work->field_0x10)++;
    }
    vp_input_guard(work);
    break;
  case 2:
    if (vp_can_act(work)) {
      work->flags &= ~0x30;
      work->gnt = 0;
      vp_state_change(work, 1);
      return 1;
    }
    break;
  }
  if (rob->state & 0x80000) {
    if (vp_rand() % 100 < work->level_params->mash_rate) {
      (work->mash)++;
    }
    vp_input_yoroke_mash(work);
  }
  return 0;
}

int vp_mode_guard_next(vp_work *work)
{
  
  switch (work->field_0x10) {
  case 0:
    work->field_0x18 = (short)work->level_params->guard_next_frames;
    work->field_0x10 = work->field_0x10 + '\x01';
    vp_input_guard(work);
    break;
  case 1:
    work->field_0x18 -= 1;
    if (0 < work->field_0x18) {
      vp_input_guard(work);
    }
    else {
      vp_frame_disadv_set(work,0xfffffffe);
      work->field_0x10 = work->field_0x10 + '\x01';
    }
    break;
  case 2:
    if (vp_can_act(work) != 0) {
      work->flags = work->flags & 0xffffffcf;
      work->gnt = 0;
      vp_state_change(work,1);
      return 1;
    }
    break;
  }
  return 0;
}

int vp_mode_guard_success(vp_work *work)
{
  rob_rec *opp;

  opp = work->other->rob;
  switch (work->field_0x10) {
  case 0:
    work->flags = work->flags & 0xffffff7f;
    if (vp_opp_can_follow_up(work) != 0) {
      work->flags = work->flags | 0x80;
      vp_frame_disadv_calc(work,1);
    }
    *(short *)work->field_0x20 = opp->move_no;
    if (vp_hame_known(work) != 0) {
      vp_hame_count_decay(work);
      work->flags = work->flags | 0x100000;
      *(short *)work->field_0x20 = -1;
    }
    else {
      work->flags = work->flags & 0xffefffff;
    }
    vp_input_g_tap(work,1);
    work->field_0x10 = work->field_0x10 + 1;
  case 1:
    if (vp_input_seq_step(work) == 0) break;
    work->field_0x10 = work->field_0x10 + 1;
  case 2:
    if (vp_can_act(work) == 0) break;
    if ((work->flags & 0x80) == 0) {
      vp_frame_disadv_calc(work,1);
      work->flags = work->flags & 0xffffffcf;
      if ((work->flags & 0x100000) != 0) vp_frame_disadv_set(work,2);
      vp_state_change(work,1);
      return 1;
    }
  case 3:
    work->field_0x10 = work->field_0x10 + 1;
    break;
  case 4:
    if (opp->frame >= opp->active_end && (vp_rand() & 1) != 0) {
      work->flags = work->flags & 0xffffffcf;
      if ((work->flags & 0x100000) != 0) vp_frame_disadv_set(work,2);
      vp_state_change(work,1);
      return 1;
    }
    vp_guard_start(work,1,0,0);
    return 1;
  }
  return 0;
}

int vp_mode_yoke(vp_work *work)
{
  int sub;
  rob_rec *rob;
  sub = 0;
  rob = work->rob;
  switch (work->field_0x10) {
  case 0:
    if (work->disadv < 8 && (rob->state & 1) != 0) {
      if (rob->stun != 0) {
        if (rob->stun > 5) goto fail;
      }
      else if (rob->frame + 5 < rob->recovery_end) goto fail;
    }
    if (*(uchar *)&work->ynt == 2) {
      work->field_0x10 = 2;
      goto fail;
    }
    if (work->yoke_side == 0) {
      work->yoke_side = (vp_rand() & 1 ^ 1) + 1;
    }
    vp_input_sidestep(work,(int)work->yoke_side);
    work->yoke_side = 0;
    work->field_0x10 = work->field_0x10 + 1;
  case 1:
    if (vp_input_seq_step(work) == 0) goto fail;
    if (work->ynt == 1) goto done;
    work->field_0x10 = work->field_0x10 + 1;
  case 2:
    if (work->ynt == 2) {
      sub = work->level_params->yoke2_escapes;
    }
    else if (work->ynt == 3) {
      sub = work->level_params->yoke3_escapes;
    }
    vp_escape_seq_pick(work,sub);
    work->field_0x10 = work->field_0x10 + 1;
  case 3:
    if (vp_escape_seq_run(work) == 0) goto fail;
  done:
    vp_state_change(work,0xf);
    return 1;
  }
fail:
  return 0;
}

int vp_mode_yoke_next(vp_work *work)
{
  int r;
  rob_rec *rob;
  rob_rec *opp;
  rob = work->rob;
  opp = rob->opp;
  switch(work->field_0x10) {
  case 0:
  case 1:
  case 2:
  case 3:
    work->field_0x10 = work->field_0x10 + '\x01';
    if (work->ynt == '\x03') {
      *(ushort *)&work->field_0x20[2] = *(ushort *)&work->field_0x20[2] | 1;
    }
    break;
  case 4:
    r = vp_can_act(work);
    if (r == 0) {
      break;
    }
    if (1.8 > rob->opp_dist) {
      if ((rob->state & 0x4000000) != 0) {
        vp_frame_disadv_set(work,0xfffffffc);
      }
      else {
        vp_frame_disadv_set(work,3);
      }
    }
    else {
      vp_frame_disadv_set(work,0);
    }
    work->ynt = 0;
    if (work->disadv >= 0 || (opp->move_flags & 0x4000) == 0 ||
        opp->frame + 10 >= opp->active_end) {
      vp_state_change(work,1);
      return 1;
    }
    *(ushort *)&work->field_0x20[2] = 0;
    work->field_0x10 = work->field_0x10 + '\x01';
  case 5:
    if (opp->frame >= opp->active_end) {
      vp_state_change(work,1);
      return 1;
    }
  }
  return 0;
}

int vp_mode_nagenuke(vp_work *work)
{
  rob_rec *rob;
  
  rob = work->rob;
  switch (work->field_0x10) {
  case 0:
  case 1:
    work->field_0x10 = work->field_0x10 + 1;
    break;
  case 2:
    if ((rob->state & 0x800) != 0) {
      vp_state_change(work,0x17);
    }
    else if (vp_can_act(work) != 0) {
      vp_frame_disadv_set(work,-1);
      vp_state_change(work,1);
      return 1;
    }
    break;
  }
  return 0;
}

int vp_mode_atemi(vp_work *work)
{
  vp_state_change(work,1);
  return 0;
}

int vp_mode_yarare(vp_work *work)
{
  rob_rec *rob = work->rob;
  switch (work->field_0x10) {
  case 0:
    vp_frame_disadv_calc(work,1);
    work->flags &= ~0x80;
    work->flags &= ~0x2000;
    if (vp_opp_can_follow_up(work) != 0) {
      work->flags |= 0x80;
    }
    if ((vp_mode_history_last(work) == 5) && ((work->flags & 0x1000) == 0)) {
      vp_hame_count_add(work);
    }
  case 1:
    work->field_0x10 += 1;
    break;
  case 2:
    if ((rob->state & 0x400) != 0) {
      work->flags &= ~0x80;
      vp_state_change(work,0x14);
    }
    else if ((rob->state & 0x200) != 0) {
      if ((rob->state & 0x1000000) != 0) {
        work->flags |= 0x80;
        vp_frame_disadv_set(work,2);
        work->flags |= 0x2000;
      }
      vp_state_change(work,0x13);
    }
    else {
      vp_state_change(work,1);
    }
    return 1;
  }
  return 0;
}

int vp_mode_yarare_land(vp_work *work)
{
  if ((work->rob->state & 0x80000) != 0) {
    if (vp_rand() % 100 < work->level_params->mash_rate) {
      work->mash = work->mash + 1;
    }
    vp_input_yoroke_mash(work);
  }
  else if (vp_can_act(work) != 0) {
    if ((work->flags & 0x80) != 0) {
      if (work->disadv < 1) {
        if ((vp_rand() & 3) != 0) {
          vp_guard_start(work,1,0,0);
        }
        else {
          vp_state_change(work,1);
        }
      }
      else {
        if ((vp_rand() & 3) != 0) {
          vp_state_change(work,1);
        }
        else {
          vp_guard_start(work,1,0,0);
        }
      }
    }
    else {
      if (work->disadv < 8) {
        vp_frame_disadv_calc(work,1);
      }
      vp_state_change(work,1);
    }
    return 1;
  }
  return 0;
}

int vp_mode_yarare_down(vp_work *work)
{
  rob_rec *rob;

  rob = work->rob;
  if (rob->state & 0x10000) {
    vp_state_change(work,0x16);
    return 0;
  }
  if ((rob->state & 0x800) != 0 && (rob->field_0x118 & 1) == 0) {
    vp_state_change(work,0x17);
    return 0;
  }
  {
    switch (work->field_0x10) {
    case 0:
      if ((rob->field_0x118 & 1) != 0 &&
          vp_rand() % 100 < work->traits[7] &&
          vp_rand() % 100 < work->level_params->ukemi_rate) {
        vp_ukemi_pick(work);
      }
      else {
        work->ukemi = 0;
      }
      work->field_0x10 = work->field_0x10 + 1;
      break;
    case 1:
      if ((rob->field_0x118 & 1) != 0 && rob->pos.y - rob->floor_y < 0.7) {
        vp_input_ukemi(work);
      }
      break;
    }
  }
  return 0;
}

int vp_mode_nagerare(vp_work *work)
{
  rob_rec *rob = work->rob;
  int r;
  if (rob->state & 0x400) {
    vp_state_change(work,0x14);
  }
  else {
    if (rob->state & 0x80000) {
      r = vp_rand() % 100;
      if (r < work->level_params->mash_rate) {
        work->mash = work->mash + 1;
      }
      vp_input_yoroke_mash(work);
      work->flags = work->flags | 0x2000;
    }
    if (vp_can_act(work) != 0) {
      vp_frame_disadv_set(work,2);
      vp_state_change(work,1);
      return 1;
    }
  }
  return 0;
}

int vp_mode_ukemi(vp_work *work)
{
  int r;
  r = vp_can_act(work);
  if (r != 0) {
    vp_frame_disadv_set(work,2);
    vp_state_change(work,1);
    return 1;
  }
  return 0;
}

int vp_mode_rise(vp_work *work)
{
  rob_rec *rob;

  rob = work->rob;
  switch (work->field_0x10) {
  case 2:
    work->rise = 0;
    work->flags = work->flags & 0xfffffcff;
    switch (vp_rand() % 4) {
    case 0:
      work->rise = 1;
      break;
    case 1:
      work->rise = 2;
      break;
    case 2:
      work->rise = 3;
      break;
    case 3:
      work->rise = 4;
      break;
    }
    work->field_0x10 = work->field_0x10 + 1;
  case 3:
    vp_input_rise(work);
    if ((rob->state & 0x40000) == 0) {
      if ((rob->state & 0x800) != 0) {
        break;
      }
      vp_state_change(work,1);
      return 1;
    }
    if (rob->opp_dist <= 1.8) {
      if ((vp_rand() & 3) != 0) {
        if ((vp_frame_count & 1) != 0) {
          work->flags = work->flags | 0x100;
        }
        else {
          work->flags = work->flags | 0x200;
        }
      }
    }
    else {
      if (vp_rand() % 5 == 0) {
        if ((vp_frame_count & 1) != 0) {
          work->flags = work->flags | 0x100;
        }
        else {
          work->flags = work->flags | 0x200;
        }
      }
    }
    work->field_0x10 = work->field_0x10 + 1;
    if ((work->flags & 0x300) == 0) {
      work->field_0x10 = work->field_0x10 + 1;
      break;
    }
    vp_stroke_watch_reset(work);
  case 4:
    vp_input_rise(work);
    if (vp_stroke_result_watch(work) == 1) {
      vp_frame_disadv_calc(work,0);
      work->field_0x10 = work->field_0x10 + 1;
    }
    break;
  case 0:
  case 1:
    work->field_0x10 = work->field_0x10 + 1;
    break;
  case 5:
    if (vp_can_act(work) == 0) {
      break;
    }
    vp_state_change(work,1);
    return 1;
  }
  return 0;
}

int vp_mode_drink(vp_work *work)
{
  rob_rec *rob;
  int n;
  rob = work->rob;
  n = 8;
  if ((rob->relation_flags & 1) != 0) {
    rob->ctrl->cpu_buttons = 1;
  }
  else {
    rob->ctrl->cpu_buttons = vp_drink_inputs[vp_frame_count % n];
  }
  return 0;
}

int vp_mode_exception(vp_work *work)
{
  return (*vp_exc_funcs[work->exc_mode])(work);
}

int vp_exc_action_pick(vp_work *work)
{
  vp_rate_action_rec *p;
  int r;

  p = vp_exc_action_list();
  r = vp_rand() % 100;
  while (p->action != 0) {
    if (r < p->rate) break;
    r = r - p->rate;
    p = p + 1;
  }
  work->flags = work->flags | 4;
  work->action = p->action;
  vp_state_change(work,5);
  return 1;
}

int vp_exc_idle(vp_work *work)
{
  vp_input_clear(work);
  return 0;
}

int vp_exc_press_p_or_k(vp_work *work)
{
  control_rec *ctrl;
  int d = 2;
  vp_input_clear(work);
  ctrl = work->rob->ctrl;
  ctrl->cpu_buttons = vp_exc_p_or_k[vp_rand() % d];
  vp_state_change(work,1);
  return 0;
}

int vp_exc_press_punch(vp_work *work)
{
  int n = 6;
  switch(work->field_0x10) {
  case 0:
    vp_input_clear(work);
    vp_exc_punch_input = vp_exc_punches[vp_rand() % n];
    work->field_0x10 = work->field_0x10 + 1;
  case 1:
  case 2:
    work->rob->ctrl->cpu_buttons = 0;
    work->field_0x10 = work->field_0x10 + 1;
    break;
  case 3:
  case 4:
    work->rob->ctrl->cpu_buttons = vp_exc_punch_input;
    work->field_0x10 = work->field_0x10 + 1;
    break;
  case 5:
    vp_state_change(work,1);
    break;
  }
  return 0;
}

int vp_exc_press_kick(vp_work *work)
{
  int n = 5;

  switch (work->field_0x10) {
  case 0:
    vp_input_clear(work);
    vp_exc_kick_input = vp_exc_kicks[vp_rand() % n];
    (work->field_0x10)++;
  case 1:
  case 2:
    work->rob->ctrl->cpu_buttons = 0;
    (work->field_0x10)++;
    break;
  case 3:
  case 4:
    work->rob->ctrl->cpu_buttons = vp_exc_kick_input;
    (work->field_0x10)++;
    break;
  case 5:
    vp_state_change(work,1);
    break;
  }
  return 0;
}

int vp_exc_escape_input(vp_work *work)
{
  rob_rec *rob;
  rob = work->rob;
  switch (work->field_0x10) {
  case 0:
    if (vp_escape_input_pick(work) == 0) {
      vp_state_change(work,1);
      return 1;
    }
    work->field_0x10 += 1;
  case 1:
    if ((rob->state & 0x200000) == 0) {
      if (rob->move_no != 0x172) {
        vp_state_change(work,1);
        return 1;
      }
    }
    else {
      work->field_0x10 += 1;
    }
    break;
  case 2:
    vp_input_clear(work);
    work->rob->ctrl->cpu_buttons =
      vp_input_side_mirror(rob,*(ushort *)&work->field_0x20[4]);
    if (*(short *)&work->field_0x20[6] == 0) {
      vp_state_change(work,1);
    }
    else {
      work->field_0x10 += 1;
    }
    break;
  case 3:
  case 4:
    work->field_0x10 += 1;
    break;
  case 5:
    vp_input_clear(work);
    work->rob->ctrl->cpu_buttons =
      vp_input_side_mirror(rob,*(ushort *)&work->field_0x20[6]);
    vp_state_change(work,1);
    break;
  }
  return 0;
}

uchar vp_level_by_difficulty(uint round)
{
  uchar x = round;
  uint d = (uchar)user_back_data.rank_difficult;
  uint n = 11;
  uint m = 5;
  if (d >= m) d = 4;
  if (x >= n) x = n - 1;
  return vp_level_by_difficulty_table[d][x];
}

void vp_level_param_set_select(uint set)
{
  if (set < 3) {
    vp_level_param_set = set;
  }
  return;
}

vp_level_params_rec * vp_level_params_get(uchar level)
{
  vp_level_params_rec *p = (vp_level_params_rec *)vp_level_param_sets[vp_level_param_set];
  if (level > 30) return p + 30;
  return p + level;
}

int vp_trait_get(int trait, uchar *choices)
{
  uint v;
  char *p;
  int b;
  
  if (trait < 9) {
    if (choices != 0) {
      v = choices[trait];
      if (v == 0xff) {
        v = 4;
      }
      else {
        v = (byte)(v - 1);
      }
      if (v < 5) {
        p = (char *)v;
        b = trait * 5 + (int)&vp_trait_table[0][0];
        return p[b];
      }
    }
    return vp_trait_table[trait][1];
  }
  return -1;
}

char * vp_mode_name(int mode)
{
  return vp_mode_names[mode];
}

char * vp_flag_name(int flag)
{
  return vp_flag_names[flag];
}

char * vp_stroke_name(int stroke)
{
  return vp_stroke_names[stroke];
}

int vp_user_renkei_get(int user)
{
  return 0;
}

vp_action_rec ** vp_chr12_punish_list(int stance)
{
  if (!(stance == 0)) {
    return vp_chr12_punish_list1;
  }
  return vp_chr12_punish_list0;
}

vp_rate_action_rec * vp_chr12_close_list(int stance)
{
  if (!(stance == 0)) {
    return vp_chr12_close_list1;
  }
  return vp_chr12_close_list0;
}

vp_rate_action_rec * vp_chr3_close_list(int attack_rate)
{
  vp_rate_action_rec *list;

  if (attack_rate > 9) {
    list = vp_chr3_close_list_high;
  }
  else if (attack_rate > 5) {
    list = vp_chr3_close_list_mid;
  }
  else {
    list = vp_chr3_close_list_low;
  }
  return list;
}

vp_chara_actions_rec * vp_chara_table_get(int chara)
{
  vp_chara_actions_rec *tab;

  if (chara > 0x12) return (vp_chara_actions_rec *)0x0;
  tab = &vp_chara_tables[chara];
  if (tab->attack_lists == (vp_rank_action_rec **)0x0) return (vp_chara_actions_rec *)0x0;
  return tab;
}

vp_action_rec * vp_common_action_get(uint no)
{
  if (!(0xf > no)) {
    return (vp_action_rec *)0x0;
  }
  return vp_common_actions[no];
}

vp_rate_action_rec * vp_exc_action_list(void)
{
  return vp_exc_action_weights;
}
