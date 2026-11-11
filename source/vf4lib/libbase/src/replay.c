/*
 * vf4lib/libbase/src/replay.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char rep_repeat_mode; /* 0x0c243078 */
extern char ending_rep_tasks_started; /* 0x0c243080 */
extern char ending_rep_state; /* 0x0c243088 */
extern char ending_rep_next; /* 0x0c243089 */
extern char ending_rep_slot; /* 0x0c24308a */
extern char ending_rep_robs_loaded; /* 0x0c24308b */
extern int ending_rep_times[10][3]; /* 0x0c243090 */
extern uchar *ending_rep_states; /* 0x0c243108 */
extern int mode_timer; /* 0x0c292550 */
extern uint game_flags; /* 0x0c2925b8 */
extern float mode_timer_rate; /* 0x0c292678 */
extern uchar rep_play_while_held; /* 0x0c2927cc */
extern int stage_cur_no; /* 0x0c306cf0 */
extern short rep_ring_write; /* 0x0c360248 */
extern short rep_mark; /* 0x0c360250 */
extern short rep_play_pos; /* 0x0c36025c */
extern short rep_play_end; /* 0x0c36025e */
extern ushort rep_ring_wrapped; /* 0x0c370ac2 */
extern rep_highlight_rec rep_highlights[11]; /* 0x0c370ac8 */
extern float cam_anim_time; /* 0x0c370c8c */
extern short cam_anim_kind; /* 0x0c370c94 */
extern uint hit_light_flags; /* 0x0c3728a8 */
extern short sound_rep_log[4]; /* 0x0c37d72a */
extern stage_rec *stage_table[24]; /* 0x0c306c88 */
extern camera_state camera; /* 0x0c370ce0 */
extern hit_light_pos_rec hit_light_pos; /* 0x0c37422c */

void rep_save_init(void)
{
  rep_ring_wrapped = 0;
  rep_ring_write = 0;
  rep_mark = -1;
  return;
}

void rep_save_exec(void)
{
  rep_frame_rec *rec;
  rep_rob_frame_rec *frame;
  rob_rec *rob;
  short *src;
  short *dst;
  int i;
  int j;
  uint *q;
  char *b;

  rec = &rep_ring[rep_ring_write];
  if (mode_get_main() == MODE_MAIN_GAME && mode_get_sub() > 7 && mode_get_sub() < 0xe &&
      pause_frame_held() == 0) {
    frame = rec->robs;
    for (i = 0; i < 2; i++, frame++) {
      rob = rob_get(i & 0xff);
      frame->place_pos = rob->skel_place_pos;
      frame->flags = rob->flags_copy;
      frame->heading = rob->skel_heading;
      frame->side_flags = rob->side_flags;
      frame->move_no = rob->move_no;
      frame->state = rob->state;
      frame->flags24 = rob->flags24;
      frame->flags28 = rob->flags28;
      frame->frame_pos = rob->frame_pos;
      frame->frame = (short)rob->frame_pos;
      frame->frame_speed = rob->frame_speed;
      frame->field_0x38 = rob->field_0x390;
      frame->face_flags = rob->face_flags;
      frame->hand_shape[0] = rob->hands[0].shape;
      frame->hand_shape[1] = rob->hands[1].shape;
      frame->facing_offset = rob->facing_offset;
      frame->root_offset_y = rob->root_offset_y;
      frame->motion_flags = rob->motion_flags;
      frame->root_offset_x = rob->root_offset.x;
      frame->root_offset_z = rob->root_offset.z;
      frame->flags2c = rob->flags2c;
      frame->action = *(uint *)&rob->action;
      frame->field_0xe = rob->field_0x2f2;
    }
    q = &rec->hit_light_flags;
    *q = hit_light_flags;
    *(hit_light_pos_rec *)(q + 1) = hit_light_pos;
    j = 4;
    src = sound_rep_log;
    dst = rec->sounds;
    for (; j != 0; j--) {
      *dst++ = *src++;
    }
    b = &rec->fence_segment;
    b[0] = fence_break_segment;
    b[1] = stage_break_flags;
    if (++rep_ring_write == 0x168) {
      rep_ring_wrapped = 1;
      rep_ring_write = 0;
    }
  }
  return;
}

void rep_mark_set(void)
{
  rep_mark = rep_ring_write;
  return;
}

void rep_load_init(void)
{
  rob_rec *rob;
  uint side;
  short pos;
  short end;
  short size;
  int i;
  char segment;
  
  task_pause(&task_rep_save);
  task_pause(&task_control);
  task_pause(&task_make_old);
  task_pause(&task_rob_info);
  task_pause(&task_rob_coli_atk);
  game_flags = game_flags & 0xffffefff | 0x2000;
  rep_play_time = 0.0;
  if (rep_repeat_mode == '\0') {
    if (rep_mark == -1) {
      rep_mark = 0x78;
    }
    rep_play_end = rep_mark + rep_judge_post_frames;
    rep_play_pos = (rep_mark - rep_judge_pre_frames) + 1;
    if ((rep_play_pos < 0) && (rep_ring_wrapped == 0)) {
      rep_play_pos = rep_ring_wrapped;
      rep_play_left = (float)(rep_ring_write + -1);
      rep_play_end = (short)(int)rep_play_left;
    }
    else {
      rep_play_left = 240.0;
    }
  }
  else {
    rep_play_end = rep_ring_write + -1;
    rep_play_end = (rep_play_end < 0) ? rep_play_end + 0x168 : rep_play_end;
    rep_play_pos = (short)(int)((float)(int)rep_play_end - 18.0);
    rep_play_left = 18.0;
    rep_repeat_count = rep_repeat_count + '\x01';
  }
  if (rep_play_pos < 0) {
    rep_play_pos = rep_play_pos + 0x168;
  }
  side = 0;
  do {
    rob = rob_get(side & 0xff);
    rob_hands_reset(rob);
    rob_face_reset(rob);
    rob_limbs_reset(rob);
    rob->field_0x578 = 0;
    side = side + 1;
  } while ((int)side < 2);
  if ((rep_repeat_mode == '\0') && ((stage_cur_no == 0 || stage_cur_no == 4 || (stage_cur_no == 0x10)))
     ) {
    pos = rep_play_pos;
    end = rep_ring_write + -1;
    if (end < 0) {
      end = end + 0x168;
    }
    rep_fence_from = pos;
    rep_fence_to = end;
    for (i = 0, size = 0xbc; i < 0x168; i++) {
      segment = (&rep_ring[0].fence_segment)[pos * size];
      if (segment != 0) {
        fence_segment_unbreak(segment);
      }
      if (pos == end) break;
      pos = pos + 1;
      if (pos >= 0x168) pos = 0;
    }
    fence_row2_unbreak_sync();
  }
  rep_frame_apply(rep_ring);
  return;
}

void rep_load_exec(void)
{
  char sat_row;
  uint held;

  held = pause_frame_held();
  if ((held == 0) || (rep_play_while_held != '\0')) {
    if ((rep_play_pos == rep_play_end) || (rep_play_left <= 0.0)) {
      if ((rep_repeat_mode == '\x01') && (rep_repeat_count < '\x03')) {
        sat_row = camera.sat_row;
        rep_load_init();
        sat_row = sat_row + '\x01';
        if ('\x05' < sat_row) {
          sat_row = '\x03';
        }
        camera.sat_row = sat_row;
      }
      else {
        mode_timer = 0;
      }
    }
    else {
      rep_play_left = rep_play_left - mode_timer_rate;
      rep_play_time = rep_play_time + mode_timer_rate;
      rep_frame_apply(rep_ring);
    }
  }
  return;
}

void rep_frame_apply(rep_frame_rec *frames)
{
  float tmp[3];
  int not_end;
  int is_end;
  rep_rob_frame_rec *frame;
  rep_rob_frame_rec *cur;
  rep_rob_frame_rec *nxt;
  rob_rec *rob;
  short *sound;
  short c;
  short n;
  int flag;
  int i;
  int j;
  char k;
  uint *q;

  if (mode_get_main() == 4) is_end = 1; else is_end = 0;
  not_end = !is_end;
  if ((game_flags & 0x10000) != 0) {
    frames += rep_play_pos;
    frame = frames->robs;
    for (i = 0; i < 2; i++, frame++) {
      rob = rob_get(i & 0xff);
      rob->old_cur->side_flags = rob->side_flags;
      rob->old_cur->heading = rob->heading;
      rob->old_cur->state = rob->state;
      rob->old_cur->flags24 = rob->flags24;
      rob->old_cur->flags28 = rob->flags28;
      rob->old_cur->move_no = rob->move_no;
      rob->old_cur->facing_offset = rob->facing_offset;
      rob->old_cur->flags = rob->flags;
      rob->flags = frame->flags;
      rob->side_flags = frame->side_flags;
      rob->place_pos = frame->place_pos;
      rob->heading = frame->heading;
      rob->move_no = frame->move_no;
      rob->state = frame->state;
      rob->flags24 = frame->flags24;
      rob->flags28 = frame->flags28;
      rob->frame_pos = frame->frame_pos;
      rob->frame = (short)rob->frame_pos;
      rob->frame_speed = frame->frame_speed;
      rob->timed_list = frame->field_0x38;
      rob->face_flags = frame->face_flags;
      rob->hands[0].shape = frame->hand_shape[0];
      rob->hands[1].shape = frame->hand_shape[1];
      rob->facing_offset = frame->facing_offset;
      rob->root_offset_y = frame->root_offset_y;
      rob->motion_flags = frame->motion_flags;
      rob->root_offset.x = frame->root_offset_x;
      rob->root_offset.z = frame->root_offset_z;
      rob->flags2c = frame->flags2c;
      *(uint *)&rob->action = frame->action;
      rob->field_0x2f2 = frame->field_0xe;
      rob->move_length = mot_frames(rob->move_no);
      if ((rob->side_flags & 0x20) != 0) {
        rob_limb_rec *e = rob->reach.limbs;
        for (k = 0; k < 4; k++, e++) {
          e->bone = 0;
        }
      }
      if (mode_get_main() == 4) {
        rob->place_pos.y = rob->place_pos.y - ending_rep_y_offset;
      }
    }
    q = &frames->hit_light_flags;
    hit_light_flags = *q;
    hit_light_pos = *(hit_light_pos_rec *)(q + 1);
    if (not_end) {
      j = 4;
      sound = frames->sounds;
      for (; j != 0; j--, sound++) {
        if (*sound != -1) sound_rep_log_play(*sound);
      }
    }
    stage_break_flags = frames->field_0xb9;
  }
  else {
    rob_rec *rob2;
    c = rep_play_pos;
    n = c;
    n++;
    n = (n == 0x168) ? 0 : n;
    { int oc = c * 0xbc; int on = n * 0xbc;
    cur = (rep_rob_frame_rec *)((char *)frames + oc);
    nxt = (rep_rob_frame_rec *)((char *)frames + on); }
    for (i = 0; i < 2; i++, cur++, nxt++) {
      flag = 0;
      rob2 = rob_get(i & 0xff);
      rob2->coli->field_0x1c[1] = 0;
      if (__builtin_fabsf(cur->frame_pos - nxt->frame_pos) < 1.1920929e-07) {
        rob2->frame_pos = cur->frame_speed * mode_timer_rate + rob2->frame_pos;
      }
      if ((cur->flags & nxt->flags & 8) != 0 || cur->frame == nxt->frame) {
        flag = 1;
      }
      if (flag) {
        vec3_sub(tmp,&nxt->place_pos.x,&cur->place_pos.x);
        vec3_scale(tmp,tmp,mode_timer_rate);
        vec3_add_to(&rob2->place_pos.x,tmp);
        if (cur->move_no == nxt->move_no) {
          rob2->heading = (short)((float)(nxt->heading - cur->heading) * mode_timer_rate) + rob2->heading;
        }
      }
      if (mode_get_main() == 4) {
        rob2->place_pos.y = rob2->place_pos.y - ending_rep_y_offset;
      }
    }
  }
  if ((game_flags & 0x10000) != 0) {
    rep_play_pos = (++rep_play_pos == 0x168) ? 0 : rep_play_pos; /* WORKAROUND (kept as possibly original; no game instruction of its own): the else arm stores the counter back into itself. The plain 'if (++x == 0x168) x = 0;' gives the same instructions with another address register (r3 for r7). Kept as a plausible original way to write the wrap. */
  }
}

void rep_load_end(void)
{
  short i;
  short end;
  int n;
  short size;
  char segment;
  int last;
  short *write;
  if ((game_flags & 0x2000) != 0) {
    if (rep_repeat_mode == 0 && (((stage_cur_no == 0) | (stage_cur_no == 4)) || stage_cur_no == 0x10)) {
      i = rep_fence_from;
      end = rep_fence_to;
      n = 0;
      size = 0xbc;
      for (; n < 0x168; n++) {
        segment = (&rep_ring[0].fence_segment)[i * size];
        if (segment != 0) {
          fence_segment_break_set(segment);
        }
        if (i == end) break;
        i++;
        if (i > 0x167) i = 0;
      }
      fence_row2_break_sync();
    }
    write = &rep_ring_write;
    if (*write - 1 < 0) last = 0x167;
    else { int t = *write; last = t - 1; }
    rep_play_pos = last;
    rep_frame_apply(rep_ring);
    game_flags = game_flags & 0xffffcfff;
    task_stop(&task_rep_load);
    task_resume(&task_control);
    task_resume(&task_make_old);
    task_resume(&task_rob_info);
    task_resume(&task_rob_coli_atk);
  }
  return;
}

void rep_highlights_clear(void)
{
  int n;
  rep_highlight_rec *e;

  e = rep_highlights;
  n = 10;
  do {
    e->frames = 0;
    e->stage = -1;
    e->rob0_chara = -1;
    e->rob1_chara = -1;
    e->rob0_costume = -1;
    e->rob1_costume = -1;
    e = e + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void rep_highlights_free(void)
{
  rep_highlight_rec *e;
  int n;

  e = rep_highlights;
  n = 10;
  do {
    if (e->frames != (void *)0x0) {
      free(e->frames);
      e->frames = 0;
      e->stage = -1;
      e->rob0_chara = -1;
      e->rob1_chara = -1;
      e->rob0_costume = -1;
      e->rob1_costume = -1;
    }
    e = e + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

int rep_highlights_alloc(void)
{
  void *frames;
  rep_highlight_rec *e;
  int n;
  int ok;
  
  e = rep_highlights;
  ok = 1;
  n = 10;
  do {
    if (e->frames == 0) {
      frames = malloc(0xb040);
      e->frames = frames;
      if (frames == (void *)0x0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/replay.c",0x34a,"assertion `%s\' failed.\n",
                 "0");
        ok = 0;
      }
    }
    e = e + 1;
    n = n + -1;
  } while (n != -1);
  return ok;
}

void rep_highlight_store(void)
{
  rep_highlight_rec *e;
  rob_rec *rob0;
  rob_rec *rob1;
  short from;
  short to;
  short n;
  short lim;
  rep_frame_rec *src;
  rep_frame_rec *dst;
  
  e = &rep_highlights[(char)rank_round - 1];
  n = 360;
  lim = 240;
  from = rep_mark - 0x77;
  to = rep_mark + 0x78;
  if (to >= n) {
    to = to - n;
  }
  if (from < 0) {
    if (*(short *)&rep_ring_wrapped == 0) {
      from = 0;
      to = lim > rep_ring_write ? rep_ring_write : lim;
    }
    else {
      from = from + n;
    }
  }
  e->stage = *(char *)&stage_cur_no;
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  e->rob0_chara = rob0->chara;
  e->rob0_costume = rob0->costume;
  e->rob1_chara = rob1->chara;
  e->rob1_costume = rob1->costume;
  e->from = from;
  e->to = to;
  e->mark = rep_mark;
  src = &rep_ring[from];
  dst = e->frames;
  while (from != to) {
    *dst++ = *src++;
    from = from + 1;
    if (from >= n) {
      from = 0;
      src = rep_ring;
    }
  }
  *dst = *src;
}

void ending_rep_reset(void)
{
  ending_rep_state = 0;
  ending_rep_next = -1;
  ending_rep_slot = 0;
  mode_timer = 6000;
  ending_rep_tasks_started = 0;
  return;
}

void ending_rep_run(void)
{
  int timer;
  ((void (*)(void))(&ending_rep_states)[ending_rep_state])();
  timer = mode_timer;
  if (timer == (int)((float)(rep_fade_frames * 3) + 5820.0)) {
    fog_timer = rep_fade_frames * 3;
    fade_color[0] = 0;
    fade_color[1] = 0;
    fade_color[2] = 0;
    fog_flags = 0x20;
  }
  while (ending_rep_next != -1) {
    ending_rep_state = ending_rep_next;
    ending_rep_next = -1;
    ((void (*)(void))(&ending_rep_states)[ending_rep_state])();
  }
}

void ending_rep_setup(void)
{
  rep_highlight_rec *entry;
  stage_rec **stages;
  int slot;
  
  slot = ending_rep_slot;
  entry = &rep_highlights[slot];
  if (mode_timer <= ending_rep_times[slot][0]) {
    objbank_free(4);
    objbank_free(5);
    objbank_free(6);
    objbank_free(7);
    objbank_free(8);
    objbank_free(9);
    objbank_free(10);
    objbank_free(0xb);
    objbank_free(0xc);
    objbank_free(0xd);
    objbank_free(0xe);
    objbank_free(0xf);
    objbank_free(0x10);
    objbank_free(0x11);
    objbank_free(0x12);
    objbank_free(0x13);
    objbank_free(0x14);
    rob_hand_models_clear(0);
    rob_hand_models_clear(1);
    effect_slots_free_all();
    ending_rep_robs_loaded = '\0';
    vip_stage_no = '\x12';
    stages = stage_table;
    ending_rep_stage_y = stages[entry->stage]->center.y;
    ending_rep_y_offset = stages[entry->stage]->center.y - stages[18]->center.y;
    if ((uchar)entry->rob0_chara < 0x13) {
      rob_obj_alt_request(0);
      rob_draw_mode_set(0,2);
      rob_chara_load_queued(entry->rob0_chara,entry->rob0_costume,0);
      ending_rep_robs_loaded = ending_rep_robs_loaded + '\x01';
    }
    if ((uchar)entry->rob1_chara < 0x13) {
      rob_obj_alt_request(1);
      rob_draw_mode_set(1,2);
      rob_chara_load_queued(entry->rob1_chara,entry->rob1_costume,1);
      ending_rep_robs_loaded = ending_rep_robs_loaded + '\x01';
    }
    ending_rep_next = 1;
  }
}

void ending_rep_wait_load(void)
{
  uint *flags;
  rep_highlight_rec *entry;
  entry = &rep_highlights[ending_rep_slot];
  flags = (uint *)sound_state_get();
  if (poly_queue_busy_get() == 0 && (*flags & 0x10000) == 0 && load_pending_get() == 0) {
    if (ending_rep_robs_loaded == 2) {
      if (ending_rep_tasks_started == 0) {
        ending_rep_tasks_started = 1;
        task_start(&task_camera);
        task_start(&task_fog);
        task_start(&task_effect);
        task_start(&task_stage);
        camera.mode = SOFTIMAGE;
        cam_anim_time = 0.0;
        cam_anim_kind = 0;
        cam_anim_index_camera = 0x36;
        fog_timer = rep_fade_frames * 3;
        fade_color[0] = 0;
        fade_color[1] = 0;
        fade_color[2] = 0;
        fog_flags = 0x10;
      }
      mothead_relocate(1,*(byte *)((int)chara_data_get(entry->rob0_chara) + 0x32));
      mothead_relocate(2,*(byte *)((int)chara_data_get(entry->rob1_chara) + 0x32));
      rob_costume_colors_send(0);
      rob_costume_colors_send(1);
      stage_apply();
      stage_field_init();
      vram_used_total_update();
    }
    ending_rep_next = 2;
  }
}

void ending_rep_start(void)
{
  int *times;
  ushort *fade;
  char light_no;

  times = ending_rep_times[ending_rep_slot];
  if (mode_timer > times[1]) {
    if (mode_timer == times[1] + rep_fade_frames) {
      fog_timer = rep_fade_frames * 3;
      fade = fade_color;
      fade[0] = 0;
      fade[1] = 0;
      fade[2] = 0;
      fog_flags = 0x10;
    }
  }
  else {
    if (ending_rep_robs_loaded == '\x02') {
      game_flags = game_flags | 0x2000;
      task_start(&task_rob_ctrl);
      task_start(&task_rob_disp);
      task_start(&task_rendtx);
      robs_start();
      if (rob_light_num >= 0) light_no = rob_light_num;
      else light_no = stg_light_num;
      light_set_load(1,light_no);
      camera_view_reset();
      camera.mode = camera.mode_return = 7;
      camera_shake_request(0xffffffff,0);
    }
    rep_play_pos = 0;
    rep_play_end = 0xf0;
    ending_rep_next = 3;
  }
  return;
}

void ending_rep_play(void)
{
  int *times;
  rep_highlight_rec *entry;
  int slot;
  rob_rec *rob0;
  rob_rec *rob1;
  int fade;

  slot = ending_rep_slot;
  entry = &rep_highlights[slot];
  times = ending_rep_times[slot];
  fade = rep_fade_frames * 2;
  if (mode_timer == fade + times[2]) {
    fog_timer = fade;
    fade_color[0] = 0;
    fade_color[1] = 0;
    fade_color[2] = 0;
    fog_flags = 0x20;
  }
  if (mode_timer > times[2]) {
    if ((ending_rep_robs_loaded == 2) && (rep_play_pos < rep_play_end)) {
      rep_frame_apply(entry->frames);
      rob0 = rob_get(0);
      rob1 = rob_get(1);
      rob0->flags |= 0x40000000;
      rob1->flags |= 0x40000000;
    }
  }
  else {
    task_stop(&task_rob_ctrl);
    task_stop(&task_rob_disp);
    task_stop(&task_rendtx);
    ending_rep_slot++;
    game_flags &= 0xffffdfff;
    if (ending_rep_slot > 9) ending_rep_next = 4; else ending_rep_next = 0;
  }
}

void ending_rep_done(void)
{
  return;
}

int rep_load_finished(void)
{
  int held;
  int result;

  held = pause_frame_held();
  if ((((held == 0) && ((game_flags & 0x2000) != 0)) && (((short)rep_play_pos) == ((short)rep_play_end))) &&
     ((rep_repeat_mode != '\x01' || ('\x02' < rep_repeat_count)))) {
    rep_load_end();
    rob_motion_set(rob_get(0));
    rob_motion_set(rob_get(1));
    result = 1;
  }
  else {
    result = 0;
  }
  return result;
}

void rep_full_start(void)
{
  task_start(&task_rep_load);
  task_stop(&task_rob_ctrl);
  rep_play_pos = (rep_ring_write == 0x167) ? 0 : rep_ring_write + 1;
  if (rep_ring_wrapped == 0) {
    rep_play_pos = rep_ring_wrapped;
  }
  rep_full_flag = 0;
  rep_play_while_held = 1;
}

void rep_play_pos_clamp(void)
{
  short wrapped;

  wrapped = rep_ring_wrapped;
  if (wrapped == 0 && rep_play_pos >= rep_ring_write) {
    rep_play_pos = wrapped;
  }
  return;
}

void rep_full_stop(void)
{
  rep_load_end();
  task_resume(&task_rob_ctrl);
  rep_play_while_held = 0;
  return;
}
