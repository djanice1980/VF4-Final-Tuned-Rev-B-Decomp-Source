/*
 * vf4lib/libsound/src/sound.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern sound_id_rec sound_id_table[3218]; /* 0x0c17e2ac */
extern int game_frame_count; /* 0x0c292540 */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern undefined4 course_time_total; /* 0x0c2925f8 */
extern int sound_load_vblanks; /* 0x0c29ff30 */
extern short sound_default_banks[8]; /* 0x0c29ff64 */
extern short sound_alt_banks[8]; /* 0x0c29ff74 */
extern int sound_queue_vblank; /* 0x0c29ff90 */
extern char bgm_fade_on; /* 0x0c29ffb0 */
extern char ambient_programs_set; /* 0x0c29ffb1 */
extern float ambient_near_dist; /* 0x0c29ffb4 */
extern float ambient_far_dist; /* 0x0c29ffb8 */
extern float ambient_facing_floor; /* 0x0c29ffbc */
extern char ban_rain_on; /* 0x0c29ffd0 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern short sound_rep_log_count; /* 0x0c37d728 */
extern short sound_rep_log[4]; /* 0x0c37d72a */
extern sound_bank_entry_rec sound_se_list[20]; /* 0x0c4220f4 */
extern sound_bank_entry_rec sound_voice_list[15]; /* 0x0c4221e4 */
extern sound_music_entry_rec sound_music_list[36]; /* 0x0c422298 */
extern char bgm_fade_last; /* 0x0c4223b8 */
extern float bgm_fade_volume; /* 0x0c4223bc */
extern char *sound_bank_files[80]; /* 0x0c29ffd4 */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern sound_rec sound_state; /* 0x0c37d65c */
extern sound_test_rec sound_test_state; /* 0x0c37d734 */
extern short *chara_se_tables[2]; /* 0x0c4220e0 */
extern char *chara_voice_tables[2]; /* 0x0c4220e8 */
extern short extra_sound_no; /* 0x0c29ff94 */
extern short other_bgm_id; /* 0x0c29ff96 */
extern short extra_sound_bgm_ids[4]; /* 0x0c29ffa8 */

void sound_flag_noop(int flag)
{
  return;
}

void sound_boot(void)
{
  sound_test_state.cv_damage_off = 0;
  sound_test_state.title_se = 0;
  sound_system_init();
  return;
}

void sound_frame_update(void)
{
  short n;
  ushort *q;
  sound_rec *p = &sound_state;
  
  n = 4;
  q = sound_rep_log;
  do {
    *q = 0xffff;
    n = n + -1;
    q = q + 1;
  } while (n != 0);
  sound_rep_log_count = 0;
  p->drv_e8 = drv_word_e8_read();
  p->drv_ec = drv_word_ec_read();
  if (p->drv_ec > p->drv_ec_peak) {
    p->drv_ec_peak = p->drv_ec;
  }
  sound_requests_step();
  robs_blow_sounds_step();
  bgm_fade_out_step();
  if (p->req_count > p->req_peak) {
    p->req_peak = p->req_count;
  }
  p->req_last = p->req_count;
  p->req_count = 0;
}

uint sound_play_id(short s, int arg)
{
  uint r;
  short t = 12;

  if (s == -1) return -1;
  r = sound_request_post(((sound_id_rec *)((char *)sound_id_table + s * t))->request);
  sound_state.play_result = r;
  return r;
}

void sound_test_open(void)
{
  sound_test_state.code = 0x100a0;
  sound_test_state.post_off = 0;
  sound_test_state.num = 0;
  sound_test_state.rob0_se_num = 0;
  sound_test_state.rob0_vo_num = 0;
  sound_test_state.bgm_num = 0;
  sound_test_state.cv_type = 0;
  sound_test_state.cv_num = 1;
  sound_state.drv_ec_peak = 0;
  return;
}

void sound_test_draw(void)
{
  ushort t = 12;
  sound_id_rec *e = (sound_id_rec *)((char *)sound_id_table + sound_test_state.num * t);
  sound_test_state.code = e->request;
  sound_test_state.name = e->name;
}

void sound_test_update(void)
{
  sound_rec *p = &sound_state;
  sound_test_frames = sound_test_frames + 1;
  if (p->field_0x3c == 1) {
    sound_test_frames_3c = sound_test_frames_3c + 1;
  }
  p->dtpk_free = dtpk_space_left();
  return;
}

void sound_test_focus_update(void)
{
  sound_test_rec *st = &sound_test_state;
  player_input_rec *input;

  input = player_input_get(0);
  if (((input->pressed & 1) != 0) && (st->post_off == '\0')) {
    sound_state.play_result = sound_request_post(st->code);
  }
  input = player_input_get(0);
  if ((input->pressed & 4) != 0) {
    sound_all_off();
  }
  return;
}

void robsound_set(void)
{
  sound_bank_entry_rec *e1;
  sound_bank_entry_rec *e2;
  char **t;
  
  e1 = sound_se_list + sound_test_state.rob0_se_num;
  e2 = sound_voice_list + sound_test_state.rob0_vo_num;
  t = sound_bank_files;
  sound_pack_reload(3,t[e1->bank]);
  sound_pack_reload(4,t[e2->bank]);
  chara_se_tables[0] = (short *)e1->data;
  chara_voice_tables[0] = (char *)e2->data;
}

void bgm_set(void)
{
  sound_music_entry_rec *p;
  p = &sound_music_list[sound_test_state.bgm_num];
  if (p->bank != -1) {
    sound_pack_reload(7,sound_bank_files[p->bank]);
  }
  sound_play_id(p->sound_id,0);
}

void sound_banks_default_load(void)
{
  int i;
  for (i = 7; i >= 0; i--) {
    dtpk_drop_compact(i);
  }
  for (i = 0; i < 8; i++) {
    if ((short)sound_default_banks[i] != -1) {
      sound_pack_load(i,sound_bank_files[(short)sound_default_banks[i]]);
    }
  }
  return;
}

void sound_banks_alt_load(void)
{
  int i;
  for (i = 7; i >= 0; i--) dtpk_drop_compact(i);
  for (i = 0; i < 8; i++) {
    if (sound_alt_banks[i] != -1) {
      sound_pack_load(i, (sound_bank_files)[sound_alt_banks[i]]);
    }
  }
}

void cv_test_update(void)
{
  sound_test_rec *st = &sound_test_state;
  if (player_input_get(0)->pressed & 1) {
    char *t = chara_voice_tables[0];
    char i = st->cv_type;
    short *e = *(short **)(t + i * 4 + 12);
    uchar n = st->cv_num;
    if (n <= t[i]) {
      e += n - 1; sound_play_id(*e, 0);
    }
  }
}

void robsound_chg_draw(void)
{
  short i = sound_test_state.rob0_se_num;
  short t = 12;
  sound_bank_entry_rec *e1 = (sound_bank_entry_rec *)((char *)sound_se_list + i * t);
  sound_bank_entry_rec *e2 = (sound_bank_entry_rec *)((char *)sound_voice_list + sound_test_state.rob0_vo_num * t);
  sound_test_state.rob0_se_name = e1->name;
  sound_test_state.rob0_vo_name = e2->name;
}

void bgm_chg_draw(void)
{
  sound_music_entry_rec *p;
  p = &sound_music_list[sound_test_state.bgm_num];
  sound_test_state.bgm_name = p->name;
}

int extra_sound_no_pick(void)
{
  uint x;
  uint y;
  uint v;
  
  x = player_input_get(0)->now;
  y = player_input_get(1)->now;
  v = x & 0xf;
  if (v != (y & 0xf)) return -1;
  if ((v == 0) | (v == 2)) {
  }
  else if ((v == 4) | (v == 1)) {
  }
  else {
    v = x & 0xf0;
    if (v != (y & 0xf0)) return -1;
    switch (v) {
    case 0x10: return 2;
    case 0x20: return 3;
    case 0x40: return 0;
    case 0x80: return 1;
    }
  }
  return -1;
}

void stage_sound_banks_load(int stage, int attract, int how)
{
  short *s;
  short no;
  uint *flags = &sound_state.flags;

  s = stage_table[stage]->sound_banks;
  if (s != 0) {
    if (stage == 0xb && course_time_total > 0x7e90) {
      s = &stage_sound_record_alt.bank_no;
    }
    stage_sound_record = (stage_sound_rec *)s;
    extra_sound_no = extra_sound_no_pick();
    if (extra_sound_no >= 0) {
      if (extra_sound_no > 3) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound.c",0x1c5,"assertion `%s\' failed.\n",
                 "extra_sound_no < SOUND_EXTRA_MAX");
      }
      *flags = *flags & 0xffffffbf;
      no = extra_sound_banks[extra_sound_no];
    }
    else {
      no = *s;
    }
    if (how == 0) {
      perf_color_mark(0x66ff99);
      sound_load_vblanks = vblank_count;
      if (attract != 0) {
        char **t = sound_bank_files;
        sound_pack_reload(7,t[0x1c]);
        sound_pack_reload(2,t[0x30]);
      }
      else {
        char **t = sound_bank_files;
        sound_pack_reload(7,t[no]);
        if (s[2] != -1) {
          sound_pack_reload(2,t[s[2]]);
        }
      }
      sound_load_vblanks = vblank_count - sound_load_vblanks;
    }
    else if (how == 1) {
      if ((sound_state.flags & 0x10000) != 0) {
        if ((sound_state.flags & 0x20000) == 0) {
          sound_state.flags = sound_state.flags | 0x20000;
          sound_reload_queue_clear();
        }
      }
      if (attract != 0) {
        char **t = sound_bank_files;
        sound_reload_queue_add(7,t[0x1c]);
        sound_reload_queue_add(2,t[0x30]);
      }
      else {
        char **t = sound_bank_files;
        sound_reload_queue_add(7,t[no]);
        if (s[2] != -1) {
          sound_reload_queue_add(2,t[s[2]]);
        }
      }
      sound_load_vblanks = vblank_count;
    }
  }
}

void stage_sound_banks_load_own(int stage, int how)
{
  stage_sound_banks_load(stage,0,how);
  return;
}

void stage_sound_banks_load_attract(int stage, int how)
{
  stage_sound_banks_load(stage,1,how);
  return;
}

void attract_stage_se_load(int how)
{
  if (how == 0) {
    perf_color_mark(0x66ff99);
    sound_load_vblanks = vblank_count;
    sound_pack_reload(2,sound_bank_files[48]);
    sound_load_vblanks = vblank_count - sound_load_vblanks;
  }
  else if (how == 1) {
    if ((sound_state.flags & 0x10000) != 0) {
      if ((sound_state.flags & 0x20000) == 0) {
        sound_state.flags = sound_state.flags | 0x20000;
        sound_reload_queue_clear();
      }
    }
    sound_reload_queue_add(2,sound_bank_files[48]);
    sound_load_vblanks = vblank_count;
  }
}

void other_bgm_load(int kind, int how)
{
  card_record_rec *r;
  card_image_rec *img;
  int side;
  uint flags;
  int bank;
  
  flags = sound_state.flags;
  bank = other_bgm_banks[kind];
  if (bank == -1) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound.c",0x231,"other bgm load ERROR.");
  }
  if (kind == 0) {
    other_bgm_id = 0xa9;
    flags = flags & 0xffffffdf;
    side = 0;
    do {
      r = card_record_get(side);
      img = card_image_get(side);
      if ((r->in_use != 0) && (img->header.level == 0x1d)) {
        bank = 4;
        other_bgm_id = 0x19;
        break;
      }
      side = side + 1;
    } while (side < 2);
  }
  if (how == 0) {
    if ((flags & 0x10000) != 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound.c",0x250,"other bgm load ERROR.");
    }
    perf_color_mark(0x66ff99);
    sound_load_vblanks = vblank_count;
    sound_pack_reload(7,sound_bank_files[bank]);
    sound_load_vblanks = vblank_count - sound_load_vblanks;
  }
  else if (how == 1) {
    if ((flags & 0x10000) != 0) {
      if ((flags & 0x20000) == 0) {
        flags = flags | 0x20000;
        sound_reload_queue_clear();
      }
    }
    sound_reload_queue_add(7,sound_bank_files[bank]);
    sound_load_vblanks = vblank_count;
  }
  sound_state.flags = flags;
  return;
}

void chara_sound_banks_load(char chara_a, char chara_b, int how)
{
  struct { short *a; short *b; } r;

  r.a = (short *)chara_data_get(chara_a)[0x11];
  r.b = (short *)chara_data_get(chara_b)[0x11];
  if (how == 0) {
    perf_color_mark(0x66ff99);
    sound_load_vblanks = vblank_count;
    {
      char **t = sound_bank_files;
      sound_pack_reload(3, t[r.a[1]]);
      if (*r.a != -1) {
        sound_pack_reload(4, t[*r.a]);
      }
    }
    {
      char **t = sound_bank_files;
      sound_pack_reload(5, t[r.b[1]]);
      if (*r.b != -1) {
        sound_pack_reload(6, t[*r.b]);
      }
    }
    sound_load_vblanks = vblank_count - sound_load_vblanks;
  }
  else if (how == 1) {
    if ((sound_state.flags & 0x10000) != 0) {
      if ((sound_state.flags & 0x20000) == 0) {
        sound_state.flags = sound_state.flags | 0x20000;
        sound_reload_queue_clear();
      }
    }
    {
      char **t = sound_bank_files;
      sound_reload_queue_add(3, t[r.a[1]]);
      if (*r.a != -1) {
        sound_reload_queue_add(4, t[*r.a]);
      }
    }
    {
      char **t = sound_bank_files;
      sound_reload_queue_add(5, t[r.b[1]]);
      if (*r.b != -1) {
        sound_reload_queue_add(6, t[*r.b]);
      }
    }
    sound_load_vblanks = vblank_count;
  }
  chara_se_tables[0] = *(short **)(r.a + 6);
  chara_se_tables[1] = *(short **)(r.b + 8);
  chara_voice_tables[0] = *(char **)(r.a + 2);
  chara_voice_tables[1] = *(char **)(r.b + 4);
}

void sound_state_bit6_set(void)
{
  sound_state.flags = sound_state.flags | 0x40;
  return;
}

void sound_requests_step(void)
{
  uint *f;
  uint v;
  short s;

  f = &sound_state.flags;
  v = *f;
  if ((v & 0x1f) != 0) {
    if ((user_back_data.option_bits & 0x4000000) == 0) {
      if ((v & 1) != 0) {
        if (extra_sound_no == -1) {
          s = stage_sound_record->bgm_id;
        }
        else {
          s = extra_sound_bgm_ids[extra_sound_no];
        }
        sound_play_id(s,0);
        extra_sound_no = -1;
      }
      else if ((v & 2) != 0) {
        sound_play_id((short)other_bgm_id,0);
      }
      else if ((v & 4) != 0) {
        if (attract_sound_setting == 1) {
          sound_play_id(0xa3,0);
        }
      }
      else if ((v & 8) != 0) {
        sound_play_id(0xad,0);
      }
      else if ((v & 0x10) != 0) {
        sound_play_id(0x2b,0);
      }
    }
    v = v & 0xffffffe0;
  }
  if (mode_get_main() == MODE_MAIN_ADVERTISE && attract_sound_setting != 1) {
    v = v & 0xffff8fff;
  }
  else if ((v & 0x1000) != 0) {
    if (stage_sound_record->start != 0) {
      stage_sound_record->start();
    }
    v = v & 0xffffefff;
    v = v | 0x2000;
  }
  else if ((v & 0x4000) != 0) {
    if (stage_sound_record->stop != 0) {
      stage_sound_record->stop();
    }
    v = v & 0xffff9fff;
  }
  else if ((v & 0x2000) != 0) {
    if (stage_sound_record->frame != 0) {
      stage_sound_record->frame();
    }
  }
  *f = v;
}

void stage_sound_stop_hook_run(void)
{
  if (stage_sound_record->stop != 0) {
    stage_sound_record->stop();
  }
  sound_state.flags = sound_state.flags & 0xffff9fff;
  return;
}

void robs_blow_sounds_step(void)
{
  rob_rec *rob;
  short *se;
  byte i;
  for (i = 0; i < 2; i++) {
    rob = rob_get(i);
    se = chara_se_tables[i];
    if ((rob->flags & 1) != 0) {
      rob_blow_sounds_step(rob, se);
    }
  }
}

void rob_blow_sounds_step(rob_rec *rob, short *se)
{
  short v;
  uint u;
  int n;
  ushort s;

  if (pause_frame_held() != 0) {
    return;
  }
  if ((game_flags & 0x2000) != 0) {
    return;
  }
  if (((rob->move_flags & 0x4000) != 0) && ((rob->state & 4) == 0)) {
    u = rob->coli->touch_mask;
    if ((u & 0x40000) != 0) {
      n = 3;
      v = se[2];
    }
    else if ((u & 0x1000000) != 0) {
      n = 3;
      v = se[3];
    }
    else if ((u & 0x100) != 0) {
      n = 2;
      v = se[0];
    }
    else {
      n = 2;
      v = se[1];
    }
    if (rob->active_start == rob->frame + n) {
      sound_request(rob,(int)v);
    }
  }
  if (((ushort)(*(short *)&rob->move_no - 0x19fU) < 2) && (rob->frame == 4)) {
    sound_request(rob,(int)se[0x14]);
  }
  if (((ushort)(*(short *)&rob->move_no - 0x227U) < 2) && (rob->frame == 4)) {
    sound_request(rob,(int)se[0x14]);
  }
  s = rob->move_no;
  if (((s == 0xd2b || s == 0xd27) || (s == 0x96c)) && (rob->frame == 4)) {
    sound_request(rob,(int)se[0x14]);
  }
  if (*(int *)&rob->move_no == 0x2501e2) {
    sound_request(rob,(int)se[0x15]);
  }
  if (*(int *)&rob->move_no == 0x3d01b5) {
    sound_request(rob,(int)se[0x15]);
  }
}

void bgm_code_aa0_level_set(int level)
{
  uint v;
  
  switch ((unsigned char)level) {
  case 0:
    v = 0;
    break;
  case 1:
    v = 0x40;
    break;
  default:
    v = 0x7f;
  }
  sound_state.cmd_result = snd_cmd_a0_post(7,0xaa0,v);
}

void bgm_fade_out_start(void)
{
  bgm_fade_volume = 127.0;
  bgm_fade_last = '\x7f';
  bgm_fade_on = 1;
  return;
}

void bgm_fade_out_cancel(void)
{
  bgm_fade_on = 0;
  return;
}

void bgm_fade_out_step(void)
{
  char volume;
  
  if ((bgm_fade_on != '\0') && ((game_frame_count & 1U) == 0)) {
    bgm_fade_volume = bgm_fade_volume - 0.55;
    volume = (char)(int)bgm_fade_volume;
    if (volume < 0) {
      bgm_fade_on = '\0';
    }
    else {
      if (bgm_fade_last != volume) {
        snd_cmd_a0_post(7,0x10a0,volume);
      }
      bgm_fade_last = volume;
    }
  }
}

void sound_bank_queue_poll(void)
{
  int pending;
  
  if ((sound_queue_vblank != vblank_count) && (pending = load_pending_get(), pending == 0)) {
    perf_color_mark(0x66ff99);
    sound_reload_bg_poll();
    perf_color_mark(0);
    sound_queue_vblank = vblank_count;
  }
  return;
}

void sound_request(rob_rec *rob, short id)
{
  e_mode_main mode;
  
  mode = mode_get_main();
  if ((mode != MODE_MAIN_ADVERTISE) || (attract_sound_setting == 1)) {
    sound_play_id((int)(short)id,0);
    sound_rep_log_push((int)(short)id);
  }
  return;
}

void sound_rep_log_push(short id)
{
  if (sound_rep_log_count < 4) sound_rep_log[sound_rep_log_count++] = id;
}

void sound_rep_log_play(int id)
{
  short s = id;
  switch (s) {
  case -1:
    break;
  case -0x8000:
    voice_channel_sound_off(0);
    break;
  case -0x7fff:
    voice_channel_sound_off(1);
    break;
  default:
    sound_play_id(s,0);
    break;
  }
}

void rob_voice_request(rob_rec *rob, int voice)
{
  struct snd_tbl { char lim[12]; short *ids[1]; } *t;
  char hi;
  char lo;
  sound_test_rec *st = &sound_test_state;
  short *v;
  v = (short *)(int)(short)voice;
  hi = (char)((uint)v >> 8);
  lo = (char)(int)v;
  t = ((struct snd_tbl **)chara_voice_tables)[rob->side];
  v = t->ids[hi];
  if (lo < t->lim[hi]) {
    if (st->cv_damage_off & 1) {
      if (hi == 4) return;
    }
    sound_request(rob,v[lo]);
  }
}

void rob_voice_cut(rob_rec *rob)
{
  voice_channel_sound_off((uint)rob->side);
  if (rob->side != 0) {
    sound_rep_log_push(-0x7fff);
  }
  else {
    sound_rep_log_push(-0x8000);
  }
  return;
}

void voice_channel_sound_off(int side)
{
  if (!(side == 0)) {
    snd_cmd_b0_post(4,0xf,0x78b0,0);
    snd_cmd_b0_post(6,0xf,0x78b0,0);
  }
  else {
    snd_cmd_b0_post(4,0x10,0x78b0,0);
  }
  return;
}

void attack_result_sounds_play(rob_rec *attacker, rob_rec *defender, uint result)
{
  rob_rec *def = defender;
  rob_rec *atk = attacker;
  short *t2;
  short *t1;
  int *ent;
  short **tb;
  int *tmp;
  char *ev;
  short *ps;
  short *q;
  short **se;
  short **tbl;
  byte k;
  char sel;
  uint st;
  uint snd;
  int flag;

  tb = chara_se_tables;
  t2 = tb[defender->side];
  t1 = tb[atk->side];
  tmp = (int *)attacker->attack_events;
  if (tmp != 0) {
    ent = tmp;
    while (ent[1] == 0x2c) {
      ev = (char *)*ent;
      ent += 2;
      if (*(uint *)(ev + 8) == 0 || (*(uint *)(ev + 8) & atk->coli->field_0x3c[0]) != 0) {
        switch (*ev) {
        case 1:
          tbl = hit_se_lists;
          ps = tbl[*(int *)(ev + 4) - 1];
          for (k = 0; k < 3; k++, ps++) {
            if (*ps < 0) return;
            sound_request(attacker,*ps);
          }
          return;
        case 2:
          sound_request(attacker,*(short *)(ev + 4));
          return;
        case 3:
          rob_voice_request(attacker,*(short *)(ev + 4));
          attacker->face_flags = attacker->face_flags | 2;
          return;
        }
      }
    }
  }
  if ((result & 0x80) != 0 || ((result & 4) != 0 && def->pending_damage != 0)) {
    snd = atk->hit_sound;
    flag = 0;
    sel = atk->attack_level;
    st = atk->coli->touch_mask;
    if ((result & 0x20) != 0 && atk->hit_sound_2 != 0) snd = atk->hit_sound_2;
    if ((result & 0x4000) != 0 && atk->hit_sound_3 != 0) snd = atk->hit_sound_3;
    if (snd != 0) {
      se = hit_se_lists;
      ps = se[snd - 1];
      for (k = 0; k < 3; k++, ps++) {
        if (*ps < 0) return;
        sound_request(attacker,*ps);
      }
      return;
    }
    if ((atk->coli->field_0x3c[0] & 2) != 0) flag = 1;
    if ((st & 0x100) != 0) {
      if (flag != 0) q = &t1[10];
      else q = &t1[8];
    }
    else if ((st & 0x2000) != 0) {
      if (flag != 0) q = &t1[11];
      else q = &t1[9];
    }
    else if ((st & 0x40000) != 0) {
      if (flag != 0) q = &t1[14];
      else q = &t1[12];
    }
    else {
      if (flag != 0) q = &t1[15];
      else q = &t1[13];
    }
    sound_request(attacker,*q);
    switch (sel) {
    case 2:
      q = &t2[17];
      break;
    case 3:
    case 5:
    case 6:
    case 9:
    case 0xd:
      q = &t2[18];
      break;
    default:
      q = &t2[16];
    }
    sound_request(defender,*q);
  }
  else if ((result & 2) != 0) {
    if ((def->flags & 0x10) != 0) q = &t2[7];
    else q = &t2[5];
    sound_request(defender,*q);
  }
  else if ((result & 0x10c) != 0) {
    sound_request(attacker,0x301);
  }
  else if ((result & 0x200) != 0) {
    sound_request(defender,t2[22]);
  }
  else if ((result & 0x400) != 0) {
    sound_request(attacker,stage_sound_record->se_a);
  }
  else if ((result & 0x2000) != 0) {
    sound_request(attacker,stage_sound_record->se_b);
  }
  else if ((result & 0x800) != 0) {
    sound_request(attacker,0x2fb);
    sound_request(attacker,0x33b);
  }
  else if ((result & 0x1000) != 0) {
    sound_request(attacker,0x330);
  }
}

void attract_adv_se_play(void)
{
  if (sound_test_state.title_se == '\0') {
    attract_sound_play(attract_adv_se_id);
  }
}

void attract_sound_play(short id)
{
  if (attract_sound_setting == 1) {
    sound_play_id((int)(short)id,0);
  }
  return;
}

void stage_se_slot_off(void)
{
  snd_cmd_a0_post(2,0x1100a0,0);
  return;
}

void stage_building2_sound_start(void)
{
  ambient_programs_set = 0;
  return;
}

void stage_building2_channels_set(void)
{
  snd_cmd_a5_post(0,0xa6,0);
  snd_cmd_a5_post(1,0xa6,0);
  snd_cmd_a5_post(2,0xa6,0);
  snd_cmd_a5_post(3,0xa6,0);
  snd_cmd_a5_post(0,0xa5,ambient_program_ch0);
  snd_cmd_a5_post(1,0xa5,ambient_program_ch1);
  snd_cmd_a5_post(2,0xa5,ambient_program_ch2);
  snd_cmd_a5_post(3,0xa5,ambient_program_ch3);
  return;
}

void stage_building2_sound_frame(void)
{
  int idx;
  int ok;
  int v;
  float len;
  float d;
  float pos[3];
  float a[3];
  float b[3];

  idx = game_frame_count % 4;
  nyc_heli_pos_get(pos,idx);
  if (mode_get_main() == MODE_MAIN_GAME) {
    if (mode_get_sub() <= 8) return;
  }
  else {
    if (mode_get_main() != MODE_MAIN_ADVERTISE) {
      return;
    }
    if (mode_get_sub() != 8 && mode_get_sub() != 10) return;
  }
  if (ambient_programs_set == 0) {
    snd_cmd_a5_post(0,0xa5,ambient_program_ch0);
    snd_cmd_a5_post(1,0xa5,ambient_program_ch1);
    snd_cmd_a5_post(2,0xa5,ambient_program_ch2);
    snd_cmd_a5_post(3,0xa5,ambient_program_ch3);
    ambient_programs_set = 1;
  }
  else {
    a[0] = pos[0] - camera.eye.x;
    a[1] = pos[1] - camera.eye.y;
    a[2] = pos[2] - camera.eye.z;
    b[0] = camera.target.x - camera.eye.x;
    b[1] = camera.target.y - camera.eye.y;
    b[2] = camera.target.z - camera.eye.z;
    len = vec3_length(a);
    vec3_normalize_nolen(a);
    vec3_normalize_nolen(b);
    d = vec3_dot(a,b);
    d = d * (1.0 - ambient_facing_floor) + ambient_facing_floor;
    if (len >= ambient_far_dist) {
      v = 0;
    }
    else {
      if (len <= ambient_near_dist) {
        v = (int)(d * 127.0);
      }
      else {
        v = (int)((d * 128.0 * (ambient_far_dist - len)) / (ambient_far_dist - ambient_near_dist));
      }
    }
    snd_cmd_a5_post(idx,0xa6,v);
  }
}

void stage_building2_sound_stop(void)
{
  snd_cmd_a5_post(0,0xa6,0);
  snd_cmd_a5_post(1,0xa6,0);
  snd_cmd_a5_post(2,0xa6,0);
  snd_cmd_a5_post(3,0xa6,0);
  return;
}

void stage_sea_sound_start(void)
{
  sound_play_id(0x39e,0);
  return;
}

void stage_sea_sound_stop(void)
{
  snd_cmd_a0_post(2,0x1100a0,0);
  return;
}

void stage_temple_sound_start(void)
{
  sound_play_id(0x3a7,0);
  return;
}

void stage_temple_sound_stop(void)
{
  snd_cmd_a0_post(2,0x1100a0,0);
  return;
}

void stage_stands_sound_start(void)
{
  snd_cmd_a5_post(0,0xa6,0);
  snd_cmd_a5_post(0,0xa5,1);
  return;
}

void stage_stands_sound_frame(void)
{
  if ((game_frame_count & 1) != 0) {
    snd_cmd_a5_post(0,0xa6,(int)audience_volume);
  }
  return;
}

void stage_stands_sound_stop(void)
{
  snd_cmd_a5_post(0,0xa6,0);
  return;
}

void stage_ban_sound_start(void)
{
  sound_play_id(0x37e,0);
  ban_rain_on = 1;
  return;
}

void stage_ban_rain_round_off(void)
{
  if ((stage_cur_no == 0x17) && ((char)ban_rain_on != '\0')) {
    sound_play_id(0x37f,0);
    ban_rain_on = '\0';
  }
  return;
}

void stage_ban_rain_judge_on(void)
{
  if (stage_cur_no == 0x17) {
    sound_play_id(0x382,0);
  }
  return;
}

void stage_ban_sound_stop(void)
{
  snd_cmd_a0_post(2,0x1100a0,0);
  return;
}

uint * sound_state_get(void)
{
  return &sound_state.flags;
}

uint stage_sound_record_get(void)
{
  return (uint)stage_sound_record;
}

void chara_se_table_set(int side, short *table)
{
  chara_se_tables[side] = table;
  return;
}

void chara_voice_table_set(int side, char *table)
{
  chara_voice_tables[side] = table;
  return;
}

void stage_sound_record_set(stage_sound_rec *rec)
{
  stage_sound_record = rec;
  return;
}

void sound_se_table_copy(void *src)
{
  typedef struct { uint a, b, c; } t;
  int i;
  t *d;
  for (i = 0; i < 0x14; i++) { d = (t *)sound_se_list; d[i].a = ((t *)src)[i].a; d[i].b = ((t *)src)[i].b; d[i].c = ((t *)src)[i].c; }
}

void sound_voice_table_copy(void *src)
{
  typedef struct { uint w[3]; } e;
  int i;
  for (i = 0; i < 0xf; i++) {
    e *d = (e *)sound_voice_list;
    d[i] = ((e *)src)[i];
  }
}

void sound_music_table_copy(void *src)
{
  struct s8 { uint a; uint b; };
  int i;
  
  for (i = 0; i < 0x24; i++) {
    ((struct s8 *)sound_music_list)[i] = ((struct s8 *)src)[i];
  }
}
