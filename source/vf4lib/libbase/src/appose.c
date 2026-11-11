/*
 * vf4lib/libbase/src/appose.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern appose_pose_motion_rec appose_intro_motions[19][3]; /* 0x0c165e50 */
extern short win_pose_motions[19][10][2]; /* 0x0c16608a */
extern short win_cam_stage_overrides; /* 0x0c166384 */
extern short continue_pose_motions[19][2]; /* 0x0c166408 */
extern short fukkatu_pose_motions[19][2]; /* 0x0c166454 */
extern void (**appose_scripts[2])(void); /* 0x0c28fbac */
extern pose_rule_rec *appose_pose_rules[19][3]; /* 0x0c28fc0c */
extern pose_rule_rec *win_pose_rules[19][10]; /* 0x0c28fd30 */
extern char pose_next_choice[8][2]; /* 0x0c290028 */
extern int game_frame_count; /* 0x0c292540 */
extern int mode_timer; /* 0x0c292550 */
extern float mode_timer_float; /* 0x0c292578 */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern char round_winner; /* 0x0c2925d2 */
extern char champion_side; /* 0x0c2925d4 */
extern float mode_timer_rate; /* 0x0c292678 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern short cam_anim_index; /* 0x0c370d5c */
extern uint cam_anim_flags; /* 0x0c370dc4 */
extern uchar appose_side; /* 0x0c41f44c */
extern char appose_step; /* 0x0c41f44d */
extern char appose_script; /* 0x0c41f44e */
extern appose_pair_data_rec *appose_pair_entry; /* 0x0c41f450 */
extern uchar appose_pair_side; /* 0x0c41f454 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern camera_state camera; /* 0x0c370ce0 */
extern appose_pose_pair appose_pose_choice; /* 0x0c41f458 */
extern appose_tried_pair appose_pose_tried; /* 0x0c41f45c */

void appose_intro_start(void)
{
  typedef struct { char chara_a; char chara_b; char pad[2]; appose_pair_data_rec *data; } pair_ent;
  rob_rec *rob0;
  rob_rec *rob1;
  pair_ent *pair;
  int found;
  char *choice;
  uint buttons;
  rob_rec *rob;
  char chara0;
  char chara1;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  found = 0;
  if ((game_flags & 0x80000) != 0) {
    appose_intro_end();
    return;
  }
  goto search;
found0:
  appose_pair_side = 0;
  goto hit;
found1:
  appose_pair_side = 1;
hit:
  found = 1;
  appose_pair_entry = pair->data;
  appose_script = pair->data->script;
  goto done;
search:
  appose_step = 0;
  appose_script = 0;
  appose_pair_entry = (appose_pair_data_rec *)0x0;
  pair = (pair_ent *)appose_pair_table;
  chara0 = rob0->chara;
  chara1 = rob1->chara;
  for (; pair->chara_a != -1; pair++) {
    if (pair->chara_a == chara0 && pair->chara_b == chara1) goto found0;
    if (pair->chara_a == chara1 && pair->chara_b == chara0) goto found1;
  }
done:
  if (found == 0) {
    appose_side = (uchar)champion_side;
    appose_pose_choice.side0 = -1;
    appose_pose_choice.side1 = -1;
    appose_pose_tried.side0 = 0;
    appose_pose_tried.side1 = 0;
    switch (play_style) {
    case 1:
      buttons = player_input_get((char)appose_side)->now;
      if (buttons & 1) { char *p = (char *)&appose_pose_choice; p[champion_side] = 0; }
      else if (buttons & 2) { char *p = (char *)&appose_pose_choice; p[champion_side] = 1; }
      else if (buttons & 4) { char *p = (char *)&appose_pose_choice; p[champion_side] = 2; }
      else { int b = (int)&appose_pose_choice; *(char *)(b + (champion_side)) = (int)lcg_next15() % 3; }
      { int b = (int)&appose_pose_choice; *(char *)(b + (champion_side ^ 1)) = (int)lcg_next15() % 3; }
      if (((uint)((char *)side_entry_flags)[(uchar)champion_side != 0] >> 4 & 1) == 0) {
        appose_side ^= 1;
        appose_step += 2;
        pose_choice_reroll(appose_pose_rules[rob0->chara],(char)appose_side);
      }
      else {
        appose_pose_pair_resolve();
      }
      break;
    case 2:
      buttons = player_input_get(0)->now;
      choice = &appose_pose_choice.side0;
      if (buttons & 1) *choice = 0;
      else if (buttons & 2) *choice = 1;
      else if (buttons & 4) *choice = 2;
      else *choice = (int)lcg_next15() % 3;
      buttons = player_input_get(1)->now;
      choice++;
      if (buttons & 1) *choice = 0;
      else if (buttons & 2) *choice = 1;
      else if (buttons & 4) *choice = 2;
      else *choice = (int)lcg_next15() % 3;
      if (((uint)((char *)side_entry_flags)[(uchar)champion_side != 0] >> 4 & 1) == 0) {
        appose_side ^= 1;
        appose_step += 2;
        rob = rob_get(appose_side);
        pose_choice_reroll(appose_pose_rules[rob->chara],(char)appose_side);
      }
      else {
        appose_pose_pair_resolve();
      }
      break;
    default:
      mode_req_sub(8);
      return;
    }
  }
  appose_intro_step();
}

void appose_intro_step(void)
{
  void (*step)(void);
  void (**steps)(void);
  if ((game_flags & 0x80000) != 0) {
    step = appose_intro_end;
  } else {
    steps = appose_scripts[appose_script];
    step = steps[appose_step];
  }
  (*step)();
}

void appose_intro_end(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  e_mode_main main_mode;
  struct { int w[4]; } *start;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  main_mode = mode_get_main();
  if (main_mode != MODE_MAIN_INTRUDE) {
    rob0->flags = rob0->flags | 0x100001;
    rob1->flags = rob1->flags | 0x100001;
    rob0->side_flags = rob0->side_flags & 0xffe7;
    rob1->side_flags = rob1->side_flags & 0xffe7;
    start = (void *)stage_cur->start_pos;
    rob_start(0,rob0->chara,(int)start++);
    rob_start(1,rob1->chara,(int)start);
    rob0->frame_pos = 0.0;
    rob0->frame = 0;
    rob1->frame_pos = 0.0;
    rob1->frame = 0;
    fog_flags = fog_flags & 3;
    mode_req_sub(8);
    side_entry_flags[0] &= ~0x10;
    side_entry_flags[1] &= ~0x10;
  }
}

void appose_intro_solo_play(void)
{
  typedef struct { int w[3]; } v3;
  rob_rec *rob;
  appose_pose_motion_rec *pose;
  char side;
  short bits;
  camera_state *cam;

  rob = rob_get((uint)appose_side);
  pose = appose_intro_motions[rob->chara];
  cam = &camera;
  rob->opp->flags &= 0xfffffffe;
  side = rob->side;
  pose = pose + (char)(&appose_pose_choice.side0)[(byte)side];
  bits = rob->side_flags;
  rob->side_flags = bits & ~0x18;
  if ((side & 1) != 0) {
    rob->side_flags = bits & ~0x18 | 0x18;
  }
  if (pose->camera >= 0 ? cam_clips_chara[pose->camera]->flags & 0x100 : 0) {
    rob->side_flags ^= 0x18;
  }
  rob_move_set(rob,pose->motion);
  rob->frame_pos = 0.0;
  rob->frame = 0;
  rob->flags |= 0x100000;
  item_zanzou_restart();
  if (pose->camera == -1) {
    cam->mode = STAY;
    cam->cam_rob_no = appose_side;
    *(v3 *)&cam->eye.x = *(v3 *)&stage_cur->center.x;
    cam->eye.y = cam->eye.y + 2.0;
    cam->eye.z = cam->eye.z + 3.0;
    *(v3 *)&cam->field_0x0 = *(v3 *)&cam->eye.x;
    *(v3 *)&cam->target.x = *(v3 *)&rob->pos;
    cam->target.y = cam->target.y + 0.5;
  }
  else {
    cam->mode = SOFTIMAGE;
    sic_rob_id = rob->side;
    cam_anim_kind = 2;
    cam_anim_flags = 1;
    cam_anim_time = 0.0;
    if (pose->motion == 0x830 && rob->side == 1) cam_anim_index = 0x59;
    else cam_anim_index = pose->camera;
  }
  appose_step = appose_step + '\x01';
}

void appose_intro_solo_wait(void)
{
  rob_rec *rob;

  rob = rob_get((uint)appose_side);
  if ((rob->side_flags & 2) != 0) {
    rob->flags = rob->flags & 0xfffffffe;
    rob->opp->flags = rob->opp->flags | 1;
    appose_side = appose_side ^ 1;
    appose_step = appose_step + '\x01';
  }
  return;
}

void appose_intro_pair_play(void)
{
  camera_state *cam = &camera;
  rob_rec *rob;
  rob_rec *opp;

  rob = rob_get((uint)appose_pair_side);
  opp = rob->opp;
  rob_move_set(rob,(uint)appose_pair_entry->rob_motion);
  rob_move_set(opp,(uint)appose_pair_entry->opp_motion);
  rob->frame_pos = 0.0;
  rob->frame = 0;
  opp->frame_pos = 0.0;
  opp->frame = 0;
  rob->flags = rob->flags | 0x100000;
  opp->flags = opp->flags | 0x100000;
  item_zanzou_restart();
  if (appose_pair_entry->camera == -1) {
    camera_reset();
    cam->mode = MODE_ROB;
  }
  else {
    cam->mode = SOFTIMAGE;
    cam_anim_index = appose_pair_entry->camera;
    cam_anim_flags = 1;
    cam_anim_time = 0.0;
    sic_rob_id = rob->side;
    cam_anim_kind = 2;
  }
  rob->place_pos.x = 0.0;
  rob->place_pos.z = 0.0;
  opp->place_pos.x = 0.0;
  opp->place_pos.z = 0.0;
  appose_step = appose_step + '\x01';
  return;
}

void appose_intro_pair_wait(void)
{
  rob_rec *rob0;
  rob_rec *rob1;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  if (((rob0->side_flags & 2) != 0) && ((rob1->side_flags & 2) != 0)) {
    appose_step = appose_step + '\x01';
  }
  return;
}

int win_pose_pick(rob_rec *rob)
{
  player_input_rec *input;
  uint buttons;
  int x;

  input = player_input_get((uint)rob->side);
  buttons = input->now;
  if ((rob->card_flags & 1) && (buttons & 6) == 6) return 3;
  if ((rob->card_flags & 2) && (buttons & 5) == 5) return 4;
  if ((rob->card_flags & 4) && (buttons & 0x83) == 0x83) return 5;
  if ((rob->card_flags & 8) && (buttons & 0x13) == 0x13) return 6;
  if ((rob->card_flags & 0x10) && (buttons & 0x43) == 0x43) return 7;
  if ((rob->card_flags & 0x20) && (buttons & 0x23) == 0x23) return 8;
  if ((rob->card_flags & 0x40) && (buttons & 3) == 3) return 9;
  if (buttons & 1) return 0;
  if (buttons & 2) return 1;
  if (buttons & 4) return 2;
  if (rob->win_pose > 2) {
    return (uchar)(game_frame_count % 3);
  }
  x = rob->win_pose + game_frame_count % 2 + 1;
  return (uchar)(x % 3);
}

void winner_pose_start(void)
{
  rob_rec *rob;
  rob_rec *opp;
  short opp_motion;
  int motion;
  int start;
  short *pose;
  uint buttons;
  camera_state *cam;
  int choice;
  camera_state *view;

  view = &camera;
  if (play_style != 1 && (both_pose_flags & 3) != 0) {
    rob = rob_get((uchar)round_winner);
    opp = rob->opp;
    if (both_pose_flags & 1) {
      motion = both_pose_win_pairs[rob->chara][0];
      opp_motion = both_pose_lose_motions[opp->chara];
      cam_anim_index = (&both_pose_win_pairs[0][1])[rob->chara * 2];
    }
    else {
      motion = both_pose_win_motions[rob->chara];
      opp_motion = both_pose_lose_rows[opp->chara].motion;
      cam_anim_index = both_pose_lose_rows->cams[opp->chara * 5 + both_pose_cam_no];
    }
    start = stage_cur->start_pos;
    rob_start((uchar)round_winner, rob->chara, start);
    rob_start((uchar)round_winner ^ 1, opp->chara, start);
    if ((uchar)round_winner & 1) {
      rob->side_flags |= 0x18;
      opp->side_flags |= 0x18;
    }
    rob_move_set(rob, (ushort)motion);
    rob_move_set(opp, (ushort)opp_motion);
    rob->heading = 0;
    opp->heading = 0;
    rob->place_pos.x = 0.0;
    rob->place_pos.z = 0.0;
    opp->place_pos.x = 0.0;
    opp->place_pos.z = 0.0;
    rob->frame_pos = opp->frame_pos = 0.0;
    rob->frame = opp->frame = 0;
    rob->flags |= 0x100000;
    opp->flags |= 0x100000;
    view->mode = SOFTIMAGE;
    cam_anim_flags = 1;
    cam_anim_time = 0.0;
    sic_rob_id = rob->side;
    cam_anim_kind = 2;
    rob->action.word = motion;
    rob->action_step = 1;
    opp->action.word = opp_motion;
    opp->action_step = 1;
  }
  else {
    pose = 0;
    cam = &camera;
    appose_pose_choice.side0 = 0xff;
    appose_pose_choice.side1 = 0xff;
    appose_pose_tried.side0 = 0;
    appose_pose_tried.side1 = 0;
    if (play_style == 1 && msnm_active() != 0) {
      appose_side = winner_disp_side = (uchar)round_winner;
      rob = rob_get((uchar)round_winner);
      if (msnm_cleared(round_winner) == 1) {
        pose = win_pose_motions[(rob_get(appose_side))->chara][0];
        buttons = player_input_get((char)appose_side)->now;
        choice = win_pose_pick(rob);
        *(char *)(rob->side + (int)&appose_pose_choice) = choice;
        pose_choice_reroll(win_pose_rules[rob->chara], (char)rob->side);
        pose = (short *)((char *)pose + (char)((struct { char a[2]; } *)&appose_pose_choice)->a[rob->side] * 4);
        rob->win_pose = ((struct { char a[2]; } *)&appose_pose_choice)->a[rob->side];
      }
      else {
        pose = win_pose_fallback_motions[(rob_get(appose_side))->chara];
        buttons = 0;
      }
    }
    else if (play_style == 1 && (game_flags & 0x400) != 0 && ((rob_get((uchar)round_winner))->flags & 4) == 0) {
      appose_side = winner_disp_side = (uchar)round_winner ^ 1;
      pose = win_pose_fallback_motions[(rob_get(winner_disp_side))->chara];
      buttons = 0;
      rob = rob_get(appose_side);
    }
    else {
      appose_side = winner_disp_side = (uchar)round_winner;
      pose = win_pose_motions[(rob_get(appose_side))->chara][0];
      buttons = player_input_get((char)appose_side)->now;
      rob = rob_get(appose_side);
      choice = win_pose_pick(rob);
      { char *b = (char *)&appose_pose_choice; b[rob->side] = choice; }
      pose_choice_reroll(win_pose_rules[rob->chara], (char)rob->side);
      pose = (short *)((char *)pose + (char)((struct { char a[2]; } *)&appose_pose_choice)->a[rob->side] * 4);
      rob->win_pose = ((struct { char a[2]; } *)&appose_pose_choice)->a[rob->side];
    }
    rob->opp->flags = 0;
    {
      int start2 = stage_cur->start_pos;
      start2 += (char)appose_side * 0x10;
      rob_start(appose_side, rob->chara, start2);
    }
    rob->side_flags &= 0xffe7;
    if (pose != 0) {
      rob_move_set(rob, (ushort)pose[0]);
      rob->action.word = pose[0];
      rob->frame_pos = 0.0;
      rob->frame = 0;
      rob->action_step = 1;
      rob->flags |= 0x100000;
    }
    if (pose == 0 || pose[1] == -1) {
      cam->mode = SAT;
      cam->sat_row = 4;
      cam->pers_req = 20.0;
      cam->flags |= 0x10;
      cam->cam_rob_no = appose_side;
      if (buttons & 0xf)
        cam->sat_row = 5;
    }
    else {
      short *row;
      int i;
      uint stage_mask;
      cam->mode = SOFTIMAGE;
      sic_rob_id = rob->side;
      cam_anim_kind = 2;
      cam_anim_flags = 1;
      cam_anim_time = 0.0;
      row = &win_cam_stage_overrides;
      stage_mask = stage_cur->flags;
      cam_anim_index = pose[1];
      for (i = 0; i < 0x10 || *row != 0; i++, row += 4) {
        if (*row == pose[0] && (stage_mask & *(uint *)(row + 2)) != 0) {
          cam_anim_index = row[1];
          break;
        }
      }
    }
  }
  mode_timer = rob->move_length;
  mode_timer_float = (float)rob->move_length;
  mode_timer_rate = 1.0;
}

void empty_fn_0c0431a0(void)
{
  return;
}

void loser_pose_start(short *motions)
{
  camera_state *cam = &camera;
  uchar *side = &appose_side;
  rob_rec *rob;
  short *pose;
  int start;
  winner_disp_side = *side = (uchar)round_winner ^ 1;
  rob = rob_get((uint)winner_disp_side);
  pose = motions + rob->chara * 2;
  rob->opp->flags = 0;
  start = stage_cur->start_pos;
  start += (char)*side * 0x10;
  rob_start(*side,rob->chara, start);
  rob->side_flags = rob->side_flags & 0xffe7;
  rob_move_set(rob,(ushort)*pose);
  rob->frame_pos = 0.0f;
  rob->frame = 0;
  rob->action.word = *pose;
  rob->action_step = 1;
  rob->flags = rob->flags | 0x100000;
  mode_timer = (int)rob->move_length;
  mode_timer_float = (float)(int)rob->move_length;
  mode_timer_rate = 1.0;
  if (pose[1] == -1) {
    cam->mode = SAT;
    cam->sat_row = '\x04';
    cam->pers_req = 20.0;
    cam->flags = cam->flags | 0x10;
    cam->cam_rob_no = *side;
  }
  else {
    cam->mode = SOFTIMAGE;
    sic_rob_id = rob->side;
    cam_anim_kind = 2;
    cam_anim_flags = 1;
    cam_anim_time = 0.0;
    cam_anim_index = pose[1];
  }
}

void continue_pose_start(void)
{
  loser_pose_start(continue_pose_motions[0]);
  return;
}

void fukkatu_pose_start(void)
{
  loser_pose_start(fukkatu_pose_motions[0]);
  return;
}

void pose_choice_reroll(pose_rule_rec **rules, int side)
{
  int i;
  rob_rec *rob;
  int choice;
  pose_rule_rec *rule;
  char tried;
  uchar flags;
  uint r;
  int tried_base;
  int base;
  int bit;
  int idx;

  i = (char)side;
  rob = rob_get((uchar)i);
  choice = (int)&appose_pose_choice;
  choice = *(char *)(choice + i);
  rule = rules[choice];
  if (rule != 0) {
    tried_base = (int)&appose_pose_tried;
    tried = *(char *)(tried_base + i);
    if ((tried & 7) != 7) {
      *(char *)(tried_base + i) = (1 << choice) | tried;
      flags = rule->flags;
      if (((1 << rob->opp->chara) & rule->chara_mask) != 0 || (flags & 2) != 0) {
        r = lcg_next15();
        bit = r & 1;
        base = (int)&appose_pose_tried;
        idx = (uchar)*(char *)(base + i);
        idx += idx;
        base = (int)pose_next_choice + idx;
        idx = *(char *)(base + bit);
        base = (int)&appose_pose_choice;
        *(char *)(base + i) = idx;
        pose_choice_reroll(rules,i);
      }
    }
  }
}

void appose_pose_pair_resolve(void)
{
  rob_rec *rob;
  uchar opp_flags;
  uchar flags;
  rob_rec *opp;
  pose_rule_rec **opp_rules;
  pose_rule_rec **rules;
  pose_rule_rec *rule;

  rob = rob_get((uint)(uchar)champion_side);
  opp = rob->opp;
  rules = appose_pose_rules[(uint)rob->chara];
  opp_rules = appose_pose_rules[(uint)opp->chara];
  pose_choice_reroll_masked(rules,(int)(char)rob->side,0);
  pose_choice_reroll_masked(opp_rules,(int)(char)opp->side,0);
  rule = rules[(char)(&appose_pose_choice.side0)[rob->side]];
  if (rule != 0) {
    flags = rule->flags;
  }
  else {
    flags = 0;
  }
  rule = opp_rules[(char)(&appose_pose_choice.side0)[opp->side]];
  if (rule != 0) {
    opp_flags = rule->flags;
  }
  else {
    opp_flags = 0;
  }
  if (flags & 1) {
    if (opp_flags & 1) {
      pose_choice_reroll_masked(opp_rules,(int)(char)opp->side,1);
    }
  }
  else if (opp_flags & 1) {
    appose_side = appose_side ^ 1;
  }
  else if (flags & 2) {
    if (opp_flags & 2) {
      pose_choice_reroll_masked(rules,(int)(char)rob->side,2);
    }
    else {
      appose_side = appose_side ^ 1;
    }
  }
  return;
}

void pose_choice_reroll_masked(pose_rule_rec **rules, int side, uint mask)
{
  int s = (char)side;
  uchar flag_mask = mask;
  rob_rec *rob;
  char choice;
  pose_rule_rec *rule;
  char tried;
  uchar flags;
  int tried_base;
  int bit;
  int next;
  char *choices;
  rob = rob_get((uchar)s);
  choices = (char *)&appose_pose_choice;
  choice = choices[s];
  rule = rules[choice];
  if (rule == 0) return;
  tried_base = (int)&appose_pose_tried;
  tried = *(char *)(tried_base + s);
  if (tried == 7) return;
  flags = rule->flags;
  *(char *)(tried_base + s) = tried | 1 << choice;
  if (((1 << rob->opp->chara) & rule->chara_mask) != 0 || (flags & flag_mask) != 0) {
    bit = 1;
    bit &= lcg_next15();
    {
      int base = (int)&appose_pose_tried;
      next = (int)pose_next_choice + *(uchar *)(base + s) * 2;
    }
    {
      char *choices2;
      char next_choice = *(char *)(next + bit);
      choices2 = (char *)&appose_pose_choice;
      choices2[s] = next_choice;
    }
    pose_choice_reroll_masked(rules,s,flag_mask);
  }
  return;
}
