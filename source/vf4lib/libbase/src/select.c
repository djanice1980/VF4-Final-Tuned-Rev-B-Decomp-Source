/*
 * vf4lib/libbase/src/select.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_card_item_log_restore
#define KRPROTO_card_data_log_restore
#define KRPROTO_oldgauge_flag_set
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int vblank_count; /* 0x0c292544 */
extern int mode_timer; /* 0x0c292550 */
extern float mode_timer_float; /* 0x0c292578 */
extern char play_style; /* 0x0c2925cc */
extern char champion_side; /* 0x0c2925d4 */
extern undefined4 match_stage; /* 0x0c2926dc */
extern char sel_opening_frames; /* 0x0c299296 */
extern int sel_bg_frame; /* 0x0c299ae4 */
extern float sel_bg_alpha; /* 0x0c299ae8 */
extern short stage_cursor_still_frames; /* 0x0c299aec */
extern int stage_random_count; /* 0x0c299af4 */
extern char stage_sel_player; /* 0x0c41fc1c */
extern char stage_cursor; /* 0x0c41fc1d */
extern char stage_cursor_prev; /* 0x0c41fc1e */
extern char sel_decided_count; /* 0x0c41fc2a */
extern int sel_frame_count; /* 0x0c41fc2c */
extern sel_screen_rec sel_screen; /* 0x0c41fc50 */
extern int sel_card_event_done[2]; /* 0x0c41fd98 */
extern int sel_1pmode_move_wait; /* 0x0c41fda4 */
extern char sel_1pmode_player; /* 0x0c41fda8 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern side_costume_sides side_costume; /* 0x0c2926e8 */
extern sel_just_decided_sides sel_just_decided; /* 0x0c299294 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern char sel_cursor[2]; /* 0x0c3778dc */
extern char sprite_set_loaded[81]; /* 0x0c3b63dc */
extern sel_cursor_prev_sides sel_cursor_prev; /* 0x0c41fc20 */
extern sel_slot_marks_rec sel_slot_marks; /* 0x0c41fc30 */

void chara_grid_slot_build(void)
{
  char c;
  char i;
  
  memset(chara_grid_slot,-1,0x10);
  for (i = 0; i < 0x10; i++) {
    char *t = chara_grid_slot;
    sel_chara_grid_rec *e = sel_chara_grid;
    c = e[i].chara;
    if (t[c] == -1) {
      t[c] = i;
    }
  }
}

void sel_chara_init(void)
{
  typedef struct { char pad[0x70]; int f70[2]; char pad2[0x8c]; int f104[2]; int f10c[2]; } fc50_t;
  char b;
  char c;
  char i;
  uint u;
  char *flags;
  char *cp;
  card_record_rec *rec[2];
  
  rec[0] = card_record_get(0);
  rec[1] = card_record_get(1);
  mode_timer = 0x4b0;
  mode_timer_float = 1200.0;
  mode_req_sub(3);
  card_user_count_clear();
  sel_decided_count = '\0';
  input_repeat_set(0x14,0xc);
  sel_card_event_reset();
  sel_just_decided.side1 = 0;
  sel_just_decided.side0 = 0;
  sel_1pmode_layers_release();
  sel_screen.cursor_flags_2p = 0;
  sel_screen.cursor_flags_1p = 0;
  for (i = 0; i < 2; i++) {
    flags = (char *)side_entry_flags;
    b = flags[i];
    if (((byte)b >> 2 & 1) != 0) {
      if (((byte)b >> 1 & 1) == 0 && (b & 1) == 0) {
        three_button_hold_reset(i);
        sel_card_state[i] = 1;
        card_renew_times_clear(i);
        card_renew_state_clear(i);
        item_select_state_clear(i);
        cp = sel_default_chara;
        (&player_chara.side0)[i] = cp[i];
        {
        int base = (int)sel_cursor;
        c = *(char *)(base + i) = (&sel_cursor_prev.side0)[i] = sel_chara_slot(i);
        }
        {
        char *p = (char *)&sel_slot_marks + c * 2;
        p[0] = 0xe;
        p[1] = 0xe;
        }
        sel_chara_scene_load(i,c);
        ((fc50_t *)&sel_screen)->f10c[i] = 7;
        ((fc50_t *)&sel_screen)->f104[i] |= 1;
        flags[i] |= 2;
        sel_cursor_still_frames[i] = 0;
        sel_299264[i] = 0;
        sel_cursor_dir[i] = 0;
        sel_move_wait[i] = 0;
        lau_vs_lau_wins[i] = 0;
        card_side_reset(i);
        team_name_set(i,empty_text);
      }
      else if ((cp = (char *)side_entry_flags, cp[i] & 1) != 0) {
        {
        int base = (int)sel_cursor;
        c = *(char *)(base + i) = (&sel_cursor_prev.side0)[i] = sel_chara_slot(i);
        }
        {
        char *p = (char *)&sel_slot_marks + c * 2;
        p[0] = 0x1c;
        p[1] = 0x1c;
        }
        sel_cursor_still_frames[i] = 0;
        sel_299264[i] = 0;
        sel_cursor_dir[i] = 0;
        sel_move_wait[i] = 0;
        sel_chara_scene_load(i,c);
        ((fc50_t *)&sel_screen)->f10c[i] = 7;
        ((fc50_t *)&sel_screen)->f104[i] |= 9;
        if (card_unit_used_get(i) != 0) {
          u = card_flags_get(i);
          if (rec[i]->in_use == 1 && (u & 0x81) == 0x81 &&
              aet_obj_is_playing(((fc50_t *)&sel_screen)->f70[i]) == 0) {
            card_message_post(i,0,0xffffffff,0);
          }
        }
      }
    }
    else {
      card_side_reset(i);
      (&sel_cursor_prev.side0)[i] = -1;
      cp = (char *)sel_cursor;
      cp[i] = -1;
      quest_mes_clear(i);
    }
  }
  if (play_style == '\x02') {
    card_log_session_start();
  }
  return;
}

void sel_chara_choose_step(int side)
{
  uint flags;
  int *renew;
  char *slot;
  int side_no;
  
  side_no = (int)(char)side;
  flags = card_flags_get(side_no);
  renew = card_renew_times_get(side_no);
  slot = card_mode_slot_get(side_no);
  if (card_unit_used_get(side_no) != 0 && card_service_mode_get() != -1) {
    if ((renew[2] < 2) && (((flags & 2) != 0 || ((flags & 0x201) == 1)))) {
      card_mode_start_entry(side_no);
    }
    else if ((flags & 4) != 0) {
      card_side_reset(side_no);
    }
    if (*slot == '\0') {
      sel_chara_cursor_update(side_no);
    }
    sel_card_event_once(side_no);
  }
  else {
    sel_chara_cursor_update(side_no);
  }
  return;
}

void sel_chara_decided_step(int side)
{
  card_record_rec *r;
  int used;
  uint flags;
  int side_no;
  
  side_no = (int)(char)side;
  r = card_record_get(side_no);
  used = card_unit_used_get(side_no);
  if (((used != 0) && (used = card_service_mode_get(), used != -1)) && (r->in_use == 1)) {
    sel_data_layer_show(side_no);
    flags = card_flags_get(side_no);
    if ((flags & 2) != 0) {
      card_mode_start_entry(side_no);
    }
  }
  return;
}

void three_button_hold_reset(int side)
{
  three_button_hold_rec *hold = &three_button_hold[(char)side];
  hold->count = -1;
  hold->timer = 0;
  hold->fired = 0;
  hold->rising = 0;
  hold->state = 0;
  hold->field_0x14 = 0;
  hold->field_0x18 = 0;
}

void three_button_hold_watch(int side)
{
  player_input_rec *in;
  three_button_hold_rec *hold;
  int i;
  int held;
  int off;
  uint mask[3];
  in = player_input_get((int)(char)side);
  hold = &three_button_hold[(char)side];
  mask[0] = 2;
  mask[1] = 4;
  mask[2] = 1;
  for (i = 0, held = 0; i < 3; i++) {
    off = (in->now & mask[i]) == 0;
    held += off == 0;
  }
  if ((hold->count < 0) & (held < 3)) {
    hold->count = 0;
  }
  if (hold->count >= 0) {
    if (hold->count > held) {
      hold->rising = 0;
    }
    else if (hold->count < held) {
      hold->rising = 1;
      hold->timer = 10;
      if (held == 3) {
        hold->timer = 0x3c;
      }
    }
    hold->count = held;
    if (hold->rising != 0) {
      if (--hold->timer < 0) {
        if (held == 3) {
          hold->fired = 1;
        }
        else {
          hold->rising = 0;
        }
      }
    }
    else {
      hold->timer = 0;
    }
  }
}

int sel_card_hold_step(int side)
{
  char c = side;
  int ret = 0;
  int pressed = 0;
  player_input_rec *in;
  three_button_hold_rec *hold;
  uint flags;
  card_image_rec *image;
  int obj;
  int kill;

  in = player_input_get(c);
  hold = &three_button_hold[c];
  if ((in->pressed & 7) != 0 &&
      (((struct { char pad[0x104]; int a[4]; } *)&sel_screen)->a[c] & 2) != 0) {
    pressed = 1;
  }
  switch (sel_card_state[c]) {
  case 1:
    flags = card_flags_get(c);
    image = card_image_get(c);
    if (card_unit_used_get(c) != 0 && card_service_mode_get() != -1 && (flags & 0x291) == 0x291) {
      if ((flags & 0x20) != 0) {
        sel_ksn_show(c);
        obj = ((struct { char pad[0x88]; int a[4]; } *)&sel_screen)->a[c];
        if (obj != -1 && aet_obj_end_reached(obj) != 0) card_renew_text_post(c, 0, 0, 1);
      } else if (image->header.use_count <= 9) {
        sel_ksn_show(c);
        obj = ((struct { char pad[0x88]; int a[4]; } *)&sel_screen)->a[c];
        if (obj != -1 && aet_obj_end_reached(obj) != 0) card_renew_text_post(c, 0, 0, 20);
      } else {
        card_renew_text_post(c, 0, 0, 0);
      }
      if (pressed) hold->state = 1;
      three_button_hold_watch(c);
      if (hold->fired != 0 && card_mode_start_renew(c) == 0) {
        sel_ksn_show(c);
        sel_card_state[c] = 2;
        three_button_hold_reset(c);
        ret = 1;
        break;
      }
      if (hold->state == 1 && hold->rising == 0) {
        sel_ksn_release(c);
        hold->state = 2;
      }
      break;
    }
    kill = 1;
    if (pressed) hold->state = 2;
    if (kill) sel_ksn_release(c);
    break;
  case 2:
    three_button_hold_watch(c);
    ret = 1;
    if (card_renew_done_get(c) != 0) {
      sel_ksn_release(c);
      sel_card_state[c] = 0;
      card_renew_state_clear(c);
      hold->state = 2;
      break;
    }
    if (hold->fired != 0) {
      three_button_hold_reset(c);
      card_renew_cancel_request(c);
    }
    break;
  default:
    if (pressed) hold->state = 2;
    break;
  }
  return ret;
}

void sel_chara_select_finish(void)
{
  card_record_rec *r;
  uint flags;
  int used;
  int side;
  
  side = 0;
  do {
    r = card_record_get(side);
    flags = card_flags_get(side);
    used = card_unit_used_get(side);
    if ((((used != 0) && (used = card_service_mode_get(), used != -1)) &&
        (((uint)(int)({ char *b = (char *)side_entry_flags; b[side]; }) >> 2 & 1) != 0)) &&
       (r->in_use == 1)) {
      if ((flags & 0x81) == 0x81) {
        if ((flags & 0x2000) == 0) {
          card_item_log_restore(side);
          card_data_log_restore(side);
          card_mode_start_select_write(side);
        }
      }
      else {
        r->in_use = 0;
        card_message_post(side,2,0xffffffff,0);
      }
    }
    side = (int)(char)((char)side + '\x01');
  } while (side < 2);
  mode_timer = 0x5a;
  mode_timer_float = 90.0;
  mode_req_sub(4);
  return;
}

void sel_chara_select(void)
{
  char i;
  card_record_rec *r;

  if (sel_card_state[0] == 2 || sel_card_state[1] == 2) {
    mode_timer = mode_timer + 1;
    mode_timer_float = mode_timer_float + 1.0;
  }
  sel_chara_screen_draw();
  if (sel_opening_frames < 'Z') {
    if (pause_frame_held() == 0) {
      sel_frame_count = sel_frame_count + 1;
      sel_opening_frames = sel_opening_frames + 1;
    }
    return;
  }
  for (i = 0; i < 2; i++) {
    r = card_record_get(i);
    card_flags_get(i);
    if (card_service_mode_get() == -1) {
      r->in_use = 0;
    }
    if (sel_card_hold_step(i) == 0) {
      char *b = (char *)(int)side_entry_flags;
      if ((uint)b[i] >> 1 & 1) sel_chara_choose_step(i);
      else {
        char *c = (char *)(int)side_entry_flags;
        if (c[i] & 1) sel_chara_decided_step(i);
      }
    }
    {
      char *b1 = (char *)(int)sel_cursor;
      char *b2 = (char *)(int)&sel_cursor_prev;
    if (b1[i] != b2[i]) {
      sel_cursor_still_frames[i] = 0;
      sel_move_wait[i] = 0x24;
      sel_move_scale[i] = 0.7;
      { char *p = (char *)&sel_screen + i * 4; uint *f = (uint *)(p + 0x104); *f &= ~1; }
    }
    else if (pause_frame_held() == 0) {
      sel_cursor_still_frames[i]++;
      { char *p = (char *)&sel_screen + i * 4; int *f = (int *)(p + 0x10c); if (*f > 0) (*f)--; }
      { char *p = (char *)&sel_screen + i * 4; int *f = (int *)(p + 0x114); if (*f > 0) (*f)--; }
      if (sel_move_wait[i] > 0) sel_move_wait[i]--;
      if (sel_decide_timer[i] > 0) sel_decide_timer[i]--;
      if (sel_299264[i] > 0) sel_299264[i]--;
    }
    }
    if (sel_data_layer_done(i) != 0) {
      sel_data_layer_release(i);
    }
  }
  if ((side_entry_flags[0] & 2) == 0 && (side_entry_flags[1] & 2) == 0) {
    sel_chara_select_finish();
  }
  if (pause_frame_held() == 0) {
    sel_frame_count = sel_frame_count + 1;
  }
}

void sel_chara_decide(void)
{
  int users;
  char i;
  int sub;
  int *q;
  card_record_rec *rec;

  users = card_user_count_get();
  for (i = 0; i < 2; i++) {
    if (pause_frame_held() == 0) {
      char *b = (char *)&sel_screen + i * 4;
      int o = 0x10c;
      q = (int *)(b + o);
      if (*q > 0) (*q)--;
    }
  }
  sel_chara_screen_draw();
  if ((mode_timer < 1) && (card_write_mode_busy() == 0)) {
    if (play_style == 1) {
      sub = 8;
    } else {
      for (i = 0; i < 2; i++) {
        rec = card_record_get(i);
        if (rec->in_use != 0) {
          card_user_count_inc();
        }
      }
      if (users == 0) {
        user_back_data.decided_no_card = user_back_data.decided_no_card + 1;
      }
      else if (users == 1) {
        user_back_data.decided_one_card = user_back_data.decided_one_card + 1;
      }
      else {
        user_back_data.decided_more_cards = user_back_data.decided_more_cards + 1;
      }
      if (user_back_data.stage_select == OFF_ON_OFF) {
        match_stage = stage_random_pick();
        sub = 0xb;
      }
      else {
        sub = 5;
      }
    }
    mode_req_sub(sub);
  }
  if (pause_frame_held() == 0) {
    sel_frame_count = sel_frame_count + 1;
    for (i = 0; i < 2; i++) {
      sel_cursor_still_frames[i]++;
      if (sel_move_wait[i] > 0) sel_move_wait[i]--;
      if (sel_decide_timer[i] > 0) sel_decide_timer[i]--;
    }
  }
}

uint sel_costume_choose(int side, int chara, int final)
{
  char me;
  char ch;
  uchar fin;
  player_input_rec *in;
  int costume;
  int other;
  char other_costume;

  me = (char)side;
  ch = (char)chara;
  fin = (uchar)final;
  in = player_input_get(me);
  if (({ char *t = side_entry_flags; t[me]; }) & 1) {
    if (fin == 1) return ((char *)&side_costume)[me * 4];
    return ({ char *t = sel_costume_preview; t[me]; });
  }
  if (fin == 1) costume = me ^ chara_costume_swap[ch];
  else costume = me;
  other = me ^ 1;
  if (fin == 1) other_costume = ((char *)&side_costume)[other * 4];
  else other_costume = ({ char *t = sel_costume_preview; t[other]; });
  if (in->now & 0x8000) {
    if (play_style == 1 || (({ char *t = side_entry_flags; t[other]; }) & 1) == 0 ||
        ({ char *t = sel_cursor; t[me]; }) != ({ char *t = sel_cursor; t[other]; }))
      costume ^= 1;
    else
      costume = other_costume ^ 1;
  }
  else {
    if ((({ char *t = side_entry_flags; t[other]; }) & 1) != 0 &&
        ({ char *t = sel_cursor; t[me]; }) == ({ char *t = sel_cursor; t[other]; }))
      costume = other_costume ^ 1;
  }
  return costume;
}

void sel_chara_cursor_update(int side)
{
  typedef struct { char a[2]; } carr;
  typedef struct { int a[2]; } iarr;
  char i;
  player_input_rec *in;
  uint flags;
  int *renew;
  char *message;
  uint move_bits;
  char c;
  char *t;
  int renewed;

  i = (char)side;
  in = player_input_get(i);
  card_record_get(i);
  card_image_get(i);
  flags = card_flags_get(i);
  renew = card_renew_times_get(i);
  message = card_message_get(i);
  ((carr *)&sel_cursor_prev)->a[i] = ((carr *)sel_cursor)->a[i];
  move_bits = 0;
  if (card_unit_used_get(i) != 0 && (flags & 0x11) == 0x11 && *(int *)(card_service_state_get() + 4) != -1) {
    ((iarr *)&player_chara)->a[i] = card_image_char_get((uchar)i);
    ((carr *)sel_cursor)->a[i] = sel_chara_slot(i);
    if (card_message_active(i)) {
      if (((carr *)sel_cursor)->a[i] != ((carr *)&sel_cursor_prev)->a[i]) {
        sel_cursor_dir[i] = 0;
        sel_cursor_moved_reset(i);
        sel_chara_display_release(i);
      }
      sel_data_layer_show(i);
    }
    t = (char *)&sel_slot_marks + ((carr *)sel_cursor)->a[i] * 2;
    if (t[0] + t[1] == 0) {
      t[0] = 14;
      t[1] = 14;
    }
    if (((carr *)sel_cursor)->a[i] != ((carr *)&sel_cursor_prev)->a[i])
      return;
  }
  else if (mode_timer > 0 && sel_move_wait[i] < 26 && (user_back_data.card_start == 0 || (flags & 8) != 0)) {
    if (((in->pressed | in->now) & 0x10) != 0) {
      ((carr *)sel_cursor)->a[i] = sel_cursor_move(((carr *)sel_cursor)->a[i], 3, 0x10, (uchar *)sel_chara_grid, 5);
      {
        char *u = (char *)&sel_slot_marks + ((carr *)sel_cursor)->a[i] * 2;
        if (u[0] + u[1] == 0)
          u[0] = 1;
      }
      move_bits |= 8;
    }
    else if (((in->pressed | in->now) & 0x20) != 0) {
      ((carr *)sel_cursor)->a[i] = sel_cursor_move(((carr *)sel_cursor)->a[i], 2, 0x10, (uchar *)sel_chara_grid, 5);
      {
        char *u = (char *)&sel_slot_marks + ((carr *)sel_cursor)->a[i] * 2;
        if (u[0] + u[1] == 0)
          u[0] = 1;
      }
      move_bits |= 4;
    }
    else if (((in->pressed | in->now) & 0x80) != 0) {
      ((carr *)sel_cursor)->a[i] = sel_cursor_move(((carr *)sel_cursor)->a[i], 0, 0x10, (uchar *)sel_chara_grid, 5);
      {
        char *u = (char *)&sel_slot_marks + ((carr *)sel_cursor)->a[i] * 2;
        if (u[0] + u[1] == 0)
          u[1] = 1;
      }
      move_bits |= 1;
    }
    else if (((in->pressed | in->now) & 0x40) != 0) {
      ((carr *)sel_cursor)->a[i] = sel_cursor_move(((carr *)sel_cursor)->a[i], 1, 0x10, (uchar *)sel_chara_grid, 5);
      {
        char *u = (char *)&sel_slot_marks + ((carr *)sel_cursor)->a[i] * 2;
        if (u[0] + u[1] == 0)
          u[1] = 1;
      }
      move_bits |= 2;
    }
    else
      goto check;
    sel_cursor_moved_reset(i);
    sel_chara_display_release(i);
  }
check:
  if (((carr *)sel_cursor)->a[i] != ((carr *)&sel_cursor_prev)->a[i]) {
    sel_cursor_dir[i] = sel_cursor_dir[i] & 0xfffffff0 | move_bits;
    sound_play_id(sel_cursor_sound[i][0], 0);
  }
  if (three_button_hold[i].state != 2 && mode_timer > 0)
    return;
  if ((sel_chara_grid[((carr *)sel_cursor)->a[i]].flags & 1) == 0)
    return;
  if (user_back_data.card_start != 0 && ((struct { char f; char r[7]; } *)&card_status)[i].f == 0)
    return;
  if (((struct { char pad[0xf4]; int a[2]; } *)&sel_screen)->a[i] == -1)
    return;
  ((iarr *)&player_chara)->a[i] = sel_cursor_chara(i);
  sel_decide_timer[i] = 0x5a;
  sel_299264[i] = 0;
  sel_chara_decided_show(i);
  ((iarr *)&side_costume)->a[i] = sel_costume_choose(i, (char)((iarr *)&player_chara)->a[i], 1);
  ((carr *)side_entry_flags)->a[i] = ((carr *)side_entry_flags)->a[i] & 0xfd | 1;
  card_image_char_set((uchar)i, (uchar)((iarr *)&player_chara)->a[i]);
  card_record_char_set((uchar)i, (uchar)((iarr *)&player_chara)->a[i]);
  sel_card_state[i] = 0;
  sel_ksn_release(i);
  if (card_unit_used_get(i) != 0 && card_service_mode_get() != -1) {
    if (flags & 1) {
      renewed = 0;
      if (renew[2] > 1)
        renewed = 1;
      if (((flags & 0xa0) == 0x80) & (renewed == 0)) {
        card_item_log_restore(i);
        card_data_log_restore(i);
        net_player_start((uchar)i);
        card_mode_start_select_write(i);
        goto done;
      }
      card_side_reset(i);
    }
    else if (*message != 1)
      goto done;
    card_message_post(i, 2, -1, 0);
  }
done:
  quest_mes_clear(i);
  sound_play_id(0x3ae, 0);
  sel_decided_count++;
  ((carr *)&sel_just_decided)->a[i] = 1;
}

char sel_cursor_move(char cur, char dir, char count, uchar *table, char columns)
{
  char x;
  char m;
  char d;
  char s;
  int c;
  int idx;
  
  x = cur;
  m = dir;
  d = count;
  s = columns;
  switch (m) {
  case 3:
    if (x + 1 == ((x + 1) / s) * s) {
      x = x - s + 1;
    }
    else {
      x = x + 1;
      if (x < d) break;
      x = s * (d / s);
    }
    break;
  case 2:
    if (x == (x / s) * s) {
      x = x + s - 1;
      if (x < d) break;
      c = d;
    }
    else {
      x = x - 1;
      if (x >= 0) goto noclamp;
      c = s;
    }
    c = c - 1;
    x = c;
    break;
  case 0:
    x = x - s;
    if (x >= 0) goto noclamp;
    x = s * (d / s) + s + x;
    if (x < d) break;
    c = x - s;
    x = c;
    break;
  case 1:
    x = x + s;
    if (x < d) break;
    c = x - (x / s) * s;
    x = c;
    break;
  }
  if (x < 0) x = 0;
noclamp:
  if (x >= d) x = d - 1;
  if (mode_get_sub() < 5) {
    idx = x * 12;
  }
  else {
    idx = x << 3;
  }
  if ((table[idx] & 1) == 0) {
    x = sel_cursor_move(x,m,d,table,s);
  }
  return x;
}

void sel_state_reset(void)
{
  char i;
  sel_opening_frames = '\0';
  sel_frame_count = 0;
  memset(&sel_slot_marks,0,0x20);
  for (i = 0; i < 4; i++) {
    player_message_text_set(0,i,empty_text,0,0);
    player_message_text_set(1,i,empty_text,0,0);
  }
  chara_grid_slot_build();
  sel_bg_reset();
}

void sel_scenes_load(void)
{
  sel_screen_reset();
  if (sel_screen.scenes_loaded == '\0') {
    sel_sprite_sets_load();
    sel_screen.scenes_loaded = '\x01';
  }
  return;
}

void sel_scenes_release(void)
{
  if (sel_screen.scenes_loaded != '\0') {
    sel_screen.scenes_loaded = '\0';
    sel_sprite_sets_free();
  }
  return;
}

int sel_both_players_in(void)
{
  int both;
  
  both = 0;
  if (((side_entry_flags[0] & 4) != 0) && ((side_entry_flags[1] & 4) != 0))
  {
    both = 1;
  }
  return both;
}

void sel_data_titles_show(float depth)
{
  int result;
  aet_obj_param param;
  
  if ((sel_screen.data_time_obj == -1) &&
     (result = aet_obj_is_playing(sel_screen.opening_obj), result == 0)) {
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0000);
    aet_param_flags_set(&param,4);
    aet_param_layer_set(&param,"SEL_BASE_DATATIME");
    aet_param_depth_range_set(&param,depth,depth);
    sel_screen.data_time_obj = aet_obj_start(&param);
  }
  result = card_message_active(0);
  if (((result == 0) && (sel_screen.data_title_obj_1p == -1)) &&
     (result = aet_obj_is_playing(sel_screen.opening_obj), result == 0)) {
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0000);
    aet_param_flags_set(&param,4);
    aet_param_layer_set(&param,"SEL_DATA_TITLE_1P");
    aet_param_depth_range_set(&param,depth - 0.01,depth - 0.01);
    sel_screen.data_title_obj_1p = aet_obj_start(&param);
  }
  result = card_message_active(1);
  if (((result == 0) && (sel_screen.data_title_obj_2p == -1)) &&
     (result = aet_obj_is_playing(sel_screen.opening_obj), 0 == result)) {
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0000);
    aet_param_flags_set(&param,4);
    aet_param_layer_set(&param,"SEL_DATA_TITLE_2P");
    aet_param_depth_range_set(&param,depth - 0.01,depth - 0.01);
    sel_screen.data_title_obj_2p = aet_obj_start(&param);
  }
  sel_offline_notice_draw(196.0,200.0,0.006);
  return;
}

void sel_cha_def_show(int slot, float depth)
{
  int obj;
  int slot_no;
  aet_still_param param;
  char *names[15];
  int *e;

  slot_no = (int)(char)slot;
  names[0] = "CHA_DEF_AKI";
  names[1] = "CHA_DEF_KAG";
  names[2] = "CHA_DEF_LEI";
  names[3] = "CHA_DEF_LIO";
  names[4] = "CHA_DEF_JAC";
  names[5] = "CHA_DEF_PAI";
  names[6] = "CHA_DEF_JEF";
  names[7] = "CHA_DEF_LAU";
  names[8] = "CHA_DEF_WOL";
  names[9] = "CHA_DEF_SAR";
  names[10] = "CHA_DEF_GOH";
  names[11] = "CHA_DEF_AOI";
  names[12] = "CHA_DEF_SHU";
  names[13] = "CHA_DEF_VAN";
  names[14] = "CHA_DEF_BRA";
  { char *p = (char *)&sel_screen + slot_no * 4; e = (int *)(p + 0xb0); }
  if (((*e == -1) && (sel_cursor[0] != slot_no)) &&
     (sel_cursor[1] != slot_no)) {
    aet_still_param_init(&param);
    aet_still_param_scene_set(&param,0xa0000);
    aet_still_param_layer_set(&param,names[slot_no]);
    aet_still_param_depth_range_set(&param,depth,depth);
    obj = aet_still_start(&param);
    *e = obj;
  }
}

void sel_timer_draw(float x, float y, float z)
{
  int digits[10];
  int t;
  int tens;
  int ones;
  digits[0] = 0x5c30;
  digits[1] = 0x5c31;
  digits[2] = 0x5c32;
  digits[3] = 0x5c33;
  digits[4] = 0x5c34;
  digits[5] = 0x5c35;
  digits[6] = 0x5c36;
  digits[7] = 0x5c37;
  digits[8] = 0x5c38;
  digits[9] = 0x5c39;
  t = mode_timer;
  if (t >= 0) {
    tens = t / 600;
    ones = t / 60 % 10;
    if (tens > 9) tens = tens % 10;
  }
  else {
    ones = 0;
    tens = 0;
  }
  sprite_put_xyz(digits[tens],x,y,z);
  sprite_put_xyz(digits[ones],x + 30.0,y,z);
}

void sel_offline_notice_draw(float x, float y, float z)
{
  struct big { char pad[0x8c]; int f; };
  int net;

  net = net_is_on();
  if ((net != 0) && (net = net_ready_and_idle(), net == 0)) {
    if ((sel_screen.ksn_obj_1p != -1) || (((struct big *)&sel_screen)->f != -1)) {
      y = 448.0;
    }
    sprite_put_xyz_scaled(0x12,x,y,z,36.0,0.9);
    sprite_text_put(&euc_text_font,x,y + 2.0,z - 1e-05,0,sel_offline_notice_text);
  }
}

void sel_cha_def_show_all(void)
{
  int slot;
  
  slot = 0;
  do {
    sel_cha_def_show(slot,0.03);
    slot = (int)(char)((char)slot + '\x01');
  } while (slot < 0xf);
  return;
}

void sel_bg_texture_update(void)
{
  int done;
  if ((sel_bg_frame & 1U) == 0) {
    ltop_frame_read(sel_bg_frame);
    perf_color_mark(0xff000080);
    texidx_base_put(0x17e8);
    done = 0;
    if (texidx_slot_overwrite(0,(int)ltop_frame_buf)) done = 1;
    perf_color_mark(0);
  }
}

void sel_bg_reset(void)
{
  sel_bg_frame = 0;
  sel_bg_alpha = 0.0;
  return;
}

void sel_bg_alpha_set(float alpha)
{
  sel_bg_alpha = alpha;
  return;
}

void sel_bg_draw(void)
{
  float x = 0.0;
  float y = 0.0;

  sprite_put_xyz_scaled_alpha(0x4fb0,x,y,0.09,1.25,0.9375,sel_bg_alpha);
  sprite_put_xyz_scaled_alpha(0x4fb0,x + 640.0,y,0.09,-1.25,0.9375,sel_bg_alpha);
  sprite_put_xyz_scaled_alpha(0x4fb0,x,y + 480.0,0.09,1.25,-0.9375,sel_bg_alpha);
  sprite_put_xyz_scaled_alpha(0x4fb0,x + 640.0,y + 480.0,0.09,-1.25,-0.9375,sel_bg_alpha);
  if (pause_frame_held() == 0) {
    if (sel_bg_frame < 0x77) {
      sel_bg_frame = sel_bg_frame + 1;
    }
    else {
      sel_bg_frame = 0;
    }
  }
  sel_bg_texture_update();
}

void sel_chara_cursor_draw(char side, char slot)
{
  typedef struct { char pad[0x3c]; int h0[2]; int h1[2]; int h2[2]; char pad2[0x104 - 0x54]; uint flags[2]; char pad3[8]; int f114[2]; } sel_view;
  int side_no;
  int slot_no;
  int other;
  int state;
  char row;
  char col;
  float x;
  float y;
  float depth;
  uint *fl;
  uint *f1;
  uint *f3;
  int *h;
  int *hp;
  int *h3;
  sel_view *v;
  char *base;
  char *b2;
  aet_obj_param buf;
  char *names[7][2];
  char *flash[2][2];
  char *sel[2];
  char **n;

  side_no = (char)side;
  slot_no = (char)slot;
  state = -1;
  other = 1;
  n = names[0];
  n[0] = "CSR_CHA_1P_D";
  n[1] = "CSR_CHA_2P_D";
  n += 2;
  n[0] = "CSR_CHA_1P_U";
  n[1] = "CSR_CHA_2P_U";
  n += 2;
  n[0] = "CSR_CHA_1P_R";
  n[1] = "CSR_CHA_2P_R";
  n += 2;
  n[0] = "CSR_CHA_1P_L";
  n[1] = "CSR_CHA_2P_L";
  n += 2;
  n[0] = "CSR_CHA_1P_TAIKI";
  n[1] = "CSR_CHA_2P_TAIKI";
  n += 2;
  n[0] = "CSR_CHA_1P_KETTEI";
  n[1] = "CSR_CHA_2P_KETTEI";
  n += 2;
  n[0] = "CHA_BASE_1P";
  n[1] = "CHA_BASE_2P";
  flash[0][0] = "CHA_FLASH_1P_IN";
  flash[0][1] = "CHA_FLASH_2P_IN";
  flash[1][0] = "CHA_FLASH_1P_KETTEI";
  flash[1][1] = "CHA_FLASH_2P_KETTEI";
  if ((sel_decide_timer[side_no] > 0 || mode_get_sub() == 7) && sel_move_wait[side_no] <= 0) {
    sel_move_wait[side_no] = 10;
  }
  if (side_no != 0) other = 0;
  if (((sel_view *)&sel_screen)->flags[side_no] & 8) return;
  sel[side_no] = names[4][side_no];
  if (sel_cursor_dir[side_no] & 4) sel[side_no] = names[3][side_no];
  else if (sel_cursor_dir[side_no] & 8) sel[side_no] = names[2][side_no];
  else if (sel_cursor_dir[side_no] & 1) sel[side_no] = names[1][side_no];
  else if (sel_cursor_dir[side_no] & 2) sel[side_no] = names[0][side_no];
  v = (sel_view *)((char *)&sel_screen + side_no * 4);
  if (v->f114[0] == 0) {
    f1 = &v->flags[0];
    if ((*f1 & 2) == 0) {
      sel[side_no] = names[4][side_no];
      aet_obj_stop(v->h0[0]);
      v->h0[0] = -1;
      *f1 = *f1 | 2;
      state = 0;
      sel_cursor_dir[side_no] &= 0xfffffff0;
    }
  }
  b2 = (char *)&sel_screen;
  v = (sel_view *)(b2 + side_no * 4);
  fl = &v->flags[0];
  if ((*fl & 4) != 0) {
    if ((*fl & 8) == 0) {
      sel[side_no] = names[5][side_no];
      aet_obj_stop(v->h0[0]);
      v->h0[0] = -1;
      hp = &v->h1[0];
      aet_obj_stop(*hp);
      *hp = -1;
      *fl = *fl | 8;
      if (*fl & 0x10) {
        sel_view *w = (sel_view *)(b2 + other * 4);
        aet_obj_stop(w->h0[0]);
        w->h0[0] = -1;
      }
      ((sel_view *)&sel_screen)->flags[side_no] &= 0xffffffed;
      state = 1;
    }
  }
  base = (char *)&sel_screen;
  if ((sel_screen.cursor_flags_1p & 8) == 0 && (sel_screen.cursor_flags_2p & 8) == 0 &&
      sel_cursor[0] == sel_cursor[1] &&
      (sel_screen.cursor_flags_1p & 2) != 0 && (sel_screen.cursor_flags_2p & 2) != 0) {
    state = 0;
    if (side_no == 1) {
      f3 = &((sel_view *)base)->flags[1];
      *f3 = *f3 | 0x10;
      h3 = &((sel_view *)base)->h0[1];
      aet_obj_stop(*h3);
      *h3 = -1;
      if ((*f3 & 8) == 0) return;
    }
    v = (sel_view *)((char *)&sel_screen + side_no * 4);
    fl = &v->flags[0];
    if ((*fl & 0x10) == 0) {
      sel[side_no] = "CSR_CHA_1P2P_TAIKI";
      aet_obj_stop(v->h0[0]);
      v->h0[0] = -1;
      *fl = *fl | 0x10;
    }
  }
  col = slot_no % 5;
  x = (float)col * 124.0f;
  row = slot_no / 5;
  y = (float)row * 74.0f;
  if (((sel_view *)&sel_screen)->h0[side_no] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf, 0xa0000);
    if ((((sel_view *)&sel_screen)->flags[side_no] & 8) == 0) aet_param_flags_set(&buf, 2);
    aet_param_layer_set(&buf, sel[side_no]);
    aet_param_depth_range_set(&buf, 0.008f, 0.008f);
    aet_param_pos_set(&buf, x, y);
    ((sel_view *)&sel_screen)->h0[side_no] = aet_obj_start(&buf);
  }
  if (state != -1) {
    if (((sel_view *)&sel_screen)->h1[side_no] == -1) {
      aet_param_init(&buf);
      aet_param_scene_set(&buf, 0xa0000);
      aet_param_layer_set(&buf, flash[state][side_no]);
      if (state == 1) depth = 0.007f;
      else if (mode_get_sub() <= 4) depth = 0.02f;
      else depth = 0.009f;
      aet_param_depth_range_set(&buf, depth, depth);
      if (state == 0) aet_param_flags_set(&buf, 4);
      aet_param_pos_set(&buf, x, y);
      ((sel_view *)&sel_screen)->h1[side_no] = aet_obj_start(&buf);
    }
    if (state != -1) { /* redundant, but game code: repeats the enclosing test, but the game reloads state and tests it again. */
      h = &((sel_view *)&sel_screen)->h2[side_no];
      if (*h == -1) {
        aet_param_init(&buf);
        aet_param_scene_set(&buf, 0xa0000);
        aet_param_layer_set(&buf, names[6][side_no]);
        aet_param_depth_range_set(&buf, 0.05f, 0.05f);
        aet_param_flags_set(&buf, 2);
        aet_param_pos_set(&buf, x, y);
        *h = aet_obj_start(&buf);
      }
    }
  }
}

void sel_cursor_layers_release(void)
{
  uchar i;
  int base;
  int *p;
  int v;
  i = 0;
  base = (int)&sel_screen;
loop:
    p = (int *)(i * 4 + base);
    v = p[15];
    if (v != -1) {
      aet_obj_stop(v);
      p[15] = -1;
    }
    p = (int *)(&sel_screen.cursor_flash_obj_1p + i * 4);
    v = *p;
    if (v != -1) {
      aet_obj_stop(v);
      *p = -1;
    }
    p = (int *)(&sel_screen.cursor_base_obj_1p + i * 4);
    v = *p;
    if (v != -1) {
      aet_obj_stop(v);
      *p = -1;
    }
    p = (int *)(&sel_screen.decide_line_obj_1p + i * 4);
    v = *p;
    if (v != -1) {
      aet_obj_stop(v);
      *p = -1;
    }
    p = (int *)((char *)&sel_screen + i * 4);
    v = p[13];
    if (v != -1) {
      aet_obj_stop(v);
      p[13] = -1;
    }
    i++;
    if (i < 2) goto loop;
}

void sel_chara_screen_draw(void)
{
  char i;
  aet_obj_param buf;
  
  if (sel_frame_count > 0x26) {
    if (sel_bg_alpha < 1.0) {
      sel_bg_alpha = (float)(sel_frame_count - 0x27) * 0.1;
    }
    else {
      sel_bg_alpha = 1.0;
    }
    sel_bg_draw();
  }
  if (mode_get_sub() < 5) {
    if (sel_opening_frames < 'Z') {
      if (sel_screen.opening_obj == -1) {
        aet_param_init(&buf);
        aet_param_scene_set(&buf,0xa0000);
        aet_param_layer_set(&buf,"SEL_OPNING");
        aet_param_depth_range_set(&buf,0.02,0.02);
        sel_screen.opening_obj = aet_obj_start(&buf);
        sound_play_id(0x376,0);
      }
    }
    else {
      if (mode_get_sub() != 4) {
        sel_timer_draw(270.0,5.0,0.006);
      }
      sel_data_titles_show(0.04);
      sel_cha_def_show_all();
      if ((sel_both_players_in() != 0) && (sel_screen.vs_batu_obj == -1)) {
        aet_param_init(&buf);
        aet_param_scene_set(&buf,0xa0000);
        aet_param_layer_set(&buf,"CHA_VS_BATU");
        aet_param_flags_set(&buf,4);
        aet_param_depth_range_set(&buf,0.02,0.02);
        sel_screen.vs_batu_obj = aet_obj_start(&buf);
      }
      for (i = 0; i < 2; i++) {
        char *b0 = (char *)(int)side_entry_flags;
        if (((uint)b0[i] >> 2 & 1) != 0) {
          char *b1 = (char *)(int)&sel_cursor_prev;
          sel_chara_cursor_draw(i,b1[i]);
          {
            char *b3 = (char *)(int)sel_costume_preview_prev;
            char *b2 = (char *)(int)sel_costume_preview;
            b3[i] = b2[i];
            b2[i] = sel_costume_choose(i,sel_cursor_chara(i),0);
          }
          {
            char *b4 = (char *)(int)sel_cursor;
            sel_chara_small_draw(i,b4[i]);
            sel_chara_large_draw(i,b4[i]);
          }
          if (sel_cursor_still_frames[i] == 0xb) {
            char *p = (char *)&sel_screen + i * 4; uint *f = (uint *)(p + 0x104); *f |= 1;
          }
          {
            char *b5 = (char *)(int)sel_costume_preview_prev;
            char *b6 = (char *)(int)sel_costume_preview;
            if (b5[i] != b6[i]) {
              sel_chara_pictures_release(i);
            }
          }
          {
            int p = (int)&sel_screen + i * 4; uint *f = (uint *)(p + 0x104);
            if (*f & 1) {
              char *b7 = (char *)(int)sel_cursor;
              sel_chara_scene_load(i,b7[i]);
            }
          }
        }
      }
    }
  }
}

void sel_stage_init(void)
{
  aet_obj_param buf;
  
  mode_timer = 0x4b0;
  mode_timer_float = 1200.0;
  mode_req_sub(6);
  side_entry_flags[2] = 1;
  stage_sel_player = ((uchar)champion_side ^ 1) & 1;
  stage_random_slot = 7;
  stage_cursor_prev = '\a';
  stage_cursor = '\a';
  stage_cursor_still_frames = 0x78;
  { sel_slot_marks_rec *p = &sel_slot_marks; char c = 14; p->field_0xe = c; p->field_0xf = c; }
  sel_stage_reset_word = 0;
  stage_cursor_before_move = 0;
  sel_screen.cursor_flags_1p = 0;
  sel_screen.cursor_flags_2p = 0;
  sel_chara_objects_release();
  sel_cursor_layers_release();
  sel_chara_scenes_unload();
  sel_screen.chara_scenes_unloaded = 1;
  sel_screen.stage_preview_scene = 0xffffffff;
  sel_screen.stage_preview_set = 0xffffffff;
  sel_stage_scenes_load();
  sel_stage_preview_load(7);
  if (sel_screen.chara_to_stage_obj == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,"CHARA2STAGE");
    aet_param_depth_range_set(&buf,0.03,0.03);
    sel_screen.chara_to_stage_obj = aet_obj_start(&buf);
    sound_play_id(0x378,0);
  }
  sel_screen.fade_frame = 0;
  sel_screen.fade_alpha = 0.0;
  return;
}

void sel_stage_select(void)
{
  char random_prev;
  char *src;
  int side;
  sel_screen_rec *screen;

  random_prev = (char)stage_random_slot;
  screen = &sel_screen;
  if (aet_obj_end_reached(screen->chara_to_stage_obj) == 0) {
    sel_stage_screen_draw();
    if (screen->stage_preview_fade > 0) {
      screen->stage_preview_fade = screen->stage_preview_fade - 1;
    }
  }
  else {
    if (side_entry_flags[2] != 0) {
      sel_stage_cursor_update();
    }
    side = stage_sel_player;
    if (stage_cursor != stage_cursor_prev) {
      stage_cursor_still_frames = 0;
      sel_move_wait[side] = 0x24;
      sel_move_scale[side] = 0.7;
      stage_cursor_before_move = stage_cursor_prev;
    }
    else if (pause_frame_held() == 0 && stage_cursor_still_frames < 0x78) {
      stage_cursor_still_frames = stage_cursor_still_frames + 1;
    }
    if ((sel_stage_grid[stage_cursor].flags & 4) != 0) {
      stage_random_slot = sel_stage_random_slot(random_prev);
    }
    sel_stage_screen_draw();
    if (mode_timer < 1 || side_entry_flags[2] == 0) {
      side_entry_flags[2] = 0;
      if ((sel_stage_grid[stage_cursor].flags & 4) != 0) src = &stage_random_slot;
      else src = &stage_cursor;
      match_stage = sel_stage_slot_stage(*src);
      sel_decide_lines_show(stage_sel_player);
      {
        char *b = (char *)((uint *)&sel_screen + side);
        uint *f = (uint *)(b + 0x104);
        if ((*f & 4) == 0) *f = *f | 4;
      }
      user_back_data.bookkeep.stage_select_counts[match_stage]++;
      sound_play_id(0x3ae,0);
      mode_timer_float = 90.0;
      mode_timer = 0x5a;
      mode_req_sub(7);
      sel_screen.fade_frame = 0;
      sel_screen.fade_alpha = 0.0;
    }
    if (pause_frame_held() != 0) {
      return;
    }
    if (sel_move_wait[side] > 0) {
      sel_move_wait[side] = sel_move_wait[side] - 1;
    }
    sel_frame_count = sel_frame_count + 1;
    {
      char *b = (char *)((int *)&sel_screen + stage_sel_player);
      int *t = (int *)(b + 0x114);
      if (*t > 0) *t = *t - 1;
    }
    if (sel_screen.stage_preview_fade > 0) {
      sel_screen.stage_preview_fade = sel_screen.stage_preview_fade - 1;
    }
  }
}

void sel_stage_decide(void)
{
  uint held;
  int wait;
  int side;

  side = (int)stage_sel_player;
  sel_stage_screen_draw();
  if (sel_screen.fade_frame < 10) {
    sel_fade_draw(10,0.1);
    sel_screen.fade_frame = sel_screen.fade_frame + 1;
  }
  if ((int)mode_timer < 1) {
    sel_stage_scenes_release();
    mode_req_sub(0xb);
  }
  held = pause_frame_held();
  if (held == 0) {
    sel_frame_count = sel_frame_count + 1;
    wait = sel_move_wait[side];
    if (0 < wait) {
      sel_move_wait[side] = wait + -1;
    }
    if (stage_cursor_still_frames < 0x78) {
      stage_cursor_still_frames = stage_cursor_still_frames + 1;
    }
  }
  return;
}

int sel_stage_random_slot(char prev)
{
  char slot;
  int not_eligible;
  int same;
  char *t;

  do {
    slot = (int)lcg_next15() % 0x18;
    t = (char *)sel_stage_grid;
    not_eligible = (t[slot * 8] >> 1 ^ 1) & 1;
    same = slot == prev;
  } while (not_eligible | same);
  return slot;
}

void sel_stage_cursor_update(void)
{
  player_input_rec *in;
  uint move_bits;

  in = player_input_get((int)stage_sel_player);
  move_bits = 0;
  stage_cursor_prev = stage_cursor;
  if ((0 < mode_timer) && (sel_move_wait[stage_sel_player] < 0x1a)) {
    if (((in->pressed | in->now) & 0x10) != 0) {
      stage_cursor = sel_cursor_move(stage_cursor,3,0x18,(uchar *)sel_stage_grid,5);
      if ((&sel_slot_marks.field_0x0)[stage_cursor * 2] == '\0') {
        (&sel_slot_marks.field_0x0)[stage_cursor * 2] = '\x01';
      }
      move_bits |= 8;
      sel_cursor_moved_reset(stage_sel_player);
      sel_stage_preview_release(1);
    }
    else if (((in->pressed | in->now) & 0x20) != 0) {
      stage_cursor = sel_cursor_move(stage_cursor,2,0x18,(uchar *)sel_stage_grid,5);
      if ((&sel_slot_marks.field_0x0)[stage_cursor * 2] == '\0') {
        (&sel_slot_marks.field_0x0)[stage_cursor * 2] = '\x01';
      }
      move_bits |= 4;
      sel_cursor_moved_reset(stage_sel_player);
      sel_stage_preview_release(1);
    }
    else if (((in->pressed | in->now) & 0x80) != 0) {
      stage_cursor = sel_cursor_move(stage_cursor,0,0x18,(uchar *)sel_stage_grid,5);
      if ((&sel_slot_marks.field_0x1)[stage_cursor * 2] == '\0') {
        (&sel_slot_marks.field_0x1)[stage_cursor * 2] = '\x01';
      }
      move_bits |= 1;
      sel_cursor_moved_reset(stage_sel_player);
      sel_stage_preview_release(1);
    }
    else if (((in->pressed | in->now) & 0x40) != 0) {
      stage_cursor = sel_cursor_move(stage_cursor,1,0x18,(uchar *)sel_stage_grid,5);
      if ((&sel_slot_marks.field_0x1)[stage_cursor * 2] == '\0') {
        (&sel_slot_marks.field_0x1)[stage_cursor * 2] = '\x01';
      }
      move_bits |= 2;
      sel_cursor_moved_reset(stage_sel_player);
      sel_stage_preview_release(1);
    }
  }
  if (stage_cursor_prev != stage_cursor) {
    sel_cursor_dir[stage_sel_player] =
         sel_cursor_dir[stage_sel_player] & 0xfffffff0 | move_bits;
    sound_play_id(0x3af,0);
  }
  if (((in->pressed & 0xf) != 0) &&
     ((((struct { char pad[0x104]; uint f[4]; } *)&sel_screen)->f[stage_sel_player] & 2) != 0)) {
    side_entry_flags[2] = 0;
    sound_play_id(0x3ae,0);
  }
}

void sel_stage_icon_show(int slot)
{
  char c = (char)slot;
  char *tb;
  aet_obj_param buf;

  if (sel_screen.stage_icon_obj == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0x1a0000);
    if (stage_cursor == '\n') aet_param_flags_set(&buf,2);
    else aet_param_flags_set(&buf,4);
    tb = (char *)sel_stage_info;
    aet_param_layer_set(&buf,*(char **)(tb + c * 28 + 12));
    aet_param_depth_range_set(&buf,0.01,0.01);
    if ((stage_cursor_prev == '\n') && (side_entry_flags[2] == '\0')) {
      aet_param_pos_set(&buf,
                   (float)(((struct { char pad[0x12c]; int x; int y; } *)tb)->x - *(int *)(tb + c * 28 + 20)) * 124.0,
                   (float)(((struct { char pad[0x12c]; int x; int y; } *)tb)->y - *(int *)(tb + c * 28 + 24)) * 74.0);
    }
    sel_screen.stage_icon_obj = aet_obj_start(&buf);
  }
}

void sel_stage_screen_draw(void)
{
  int sub;
  sel_screen_rec *p = &sel_screen;

  if (p->fade_done == 0) {
    sel_fade_draw(0x50,0.015);
    p->fade_frame = p->fade_frame + 1;
    sel_bg_draw();
  }
  else {
    sub = mode_get_sub();
    if (sub != 7) {
      sel_timer_draw(270.0,5.0,0.006);
    }
    sel_chara_cursor_draw((int)stage_sel_player,(int)stage_cursor_prev);
    sel_stage_icon_show((int)stage_cursor);
    sel_stage_preview_load((int)stage_cursor);
    sel_stage_preview_draw((int)stage_cursor);
  }
  return;
}

void sel_stage_scenes_load(void)
{
  unsigned char i;
  int set;
  sel_stage_info_rec *p;

  aet_set_load(0x1a);
  aet_scene_textures_load(0x1a0000);
  for (i = 0, p = sel_stage_info; i < 0xf; i++) {
    set = p[i].preview_aet_set;
    if (set != -1) {
      aet_set_load(set);
    }
  }
}

void sel_stage_scenes_release(void)
{
  uchar i;
  sel_stage_info_rec *arr;

  aet_scene_textures_free(sel_screen.stage_preview_scene);
  i = 0xf;
  arr = sel_stage_info;
  do {
    sel_stage_info_rec *e = arr + i;
    int *pb = &e[-1].preview_aet_set;
    if (*pb != -1) {
      aet_objs_stop_by_scene(e[-1].preview_scene);
      aet_set_free(*pb);
    }
    i--;
  } while (i != 0);
  aet_objs_stop_by_scene(0xa0000);
  aet_objs_stop_by_scene(0x1a0000);
  aet_scene_textures_free(0x1a0000);
  aet_set_free(0x1a);
  return;
}

void sel_stage_preview_load(char slot)
{
  int ok;
  sel_stage_info_rec *e;
  int scene;
  ok = 0;
  if (sel_screen.stage_preview_fade < 7) ok = 1;
  if (ok && sel_screen.stage_preview_scene == -1) {
    e = &sel_stage_info[slot];
    scene = e->preview_scene;
    if (scene != -1) {
      sel_screen.stage_preview_scene = scene;
      sel_screen.stage_preview_set = e->preview_sprite_set;
      aet_scene_textures_load(scene);
    }
  }
  return;
}

void sel_stage_preview_release(uchar fade)
{
  if (fade != '\0') {
    sel_screen.stage_preview_fade = 8;
  }
  aet_obj_stop(sel_screen.stage_icon_obj);
  sel_screen.stage_icon_obj = 0xffffffff;
  aet_obj_stop(sel_screen.stage_preview_obj);
  sel_screen.stage_preview_obj = 0xffffffff;
  if (sel_screen.stage_preview_scene != -1) {
    aet_scene_textures_free(sel_screen.stage_preview_scene);
    sel_screen.stage_preview_scene = -1;
  }
}

void sel_stage_preview_draw(int slot)
{
  char c = (char)slot;
  float alpha;
  sel_screen_rec *q = &sel_screen;
  aet_obj_param buf;
  
  if (sel_screen.stage_preview_fade != 0) {
    alpha = 0.75 - (float)(8 - sel_screen.stage_preview_fade) * 0.1;
    if (alpha < 0.0) {
      alpha = 0.0;
    }
    sprite_colors_set(0xffffffff,0xffffffff);
    sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.04,80.0,60.0,alpha);
    sprite_colors_reset();
  }
  if ((((sel_screen.stage_preview_fade < 4) &&
       (sprite_set_loaded[sel_screen.stage_preview_set] != '\0')) &&
      (sel_screen.stage_preview_scene != -1)) && (q->stage_preview_obj == -1)) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,sel_screen.stage_preview_scene);
    if (c == 10) aet_param_flags_set(&buf,2);
    else aet_param_flags_set(&buf,4);
    { char *tb = (char *)sel_stage_info; aet_param_layer_set(&buf,*(char **)(tb + c * 28 + 16)); }
    aet_param_depth_range_set(&buf,0.05,0.05);
    sel_screen.stage_preview_obj = aet_obj_start(&buf);
  }
  return;
}

void sel_exit(void)
{
  sel_scene_layers_clear();
  aet_objs_stop_by_scene(0xa0001);
  aet_objs_stop_by_scene(0xa0000);
  aet_stills_stop_by_scene(0xa0000);
  mode_req_main(MODE_MAIN_GAME);
  mode_req_sub(0);
  user_back_data.bookkeep.select_time = user_back_data.bookkeep.select_time + select_frames;
  user_back_data.bookkeep.select_runs = user_back_data.bookkeep.select_runs + 1;
  sel_exit_hold_check();
  input_repeat_set(0x14,1);
  bg_color_set(bg_color_presets[4]);
  return;
}

int sel_cursor_chara(int side)
{
  return (int)sel_chara_grid[(sel_cursor)[(char)side]].chara;
}

uint sel_chara_slot(int side)
{
  int c = chara_grid_slot[((int *)&player_chara)[(char)side]];
  if (c == -1) c = 0;
  return c;
}

int sel_stage_slot_stage(int slot)
{
  return (int)sel_stage_grid[(char)slot].stage;
}

int stage_random_pick(void)
{
  char stages[24];
  short i;
  short n;
  short k;
  short j;
  short cnt;

  for (i = 0, n = 0; i < 24; i++) {
    if (sel_stage_grid[i].flags & 2) {
      stages[n] = sel_stage_grid[i].stage;
      n++;
    }
  }
  k = (sel_frame_count + vblank_count) % n;
  if (k == stage_random_last) {
    k = (k + 1) % n;
  }
  if (stage_random_count >= n) {
    stage_random_count = 0;
  }
  cnt = 0;
  if (stage_random_count > 0) {
    for (j = 0; j < stage_random_count; ) {
      if (cnt >= n) break;
      if (stage_random_history[j] == k) {
        k = (k + 1) % n;
        j = 0;
        cnt++;
      }
      else {
        j++;
      }
    }
  }
  else {
    for (i = 0; i < 24; i++) {
      stage_random_history[i] = -1;
    }
  }
  stage_random_history[stage_random_count++] = (stage_random_last = k);
  return stages[k];
}

void sel_card_event_reset(void)
{
  sel_card_event_done[0] = 0;
  sel_card_event_done[1] = 0;
  return;
}

void sel_card_event_once(int side)
{
  uint flags;
  
  flags = card_flags_get(side);
  if (((sel_card_event_done[side] == 0) && ((flags & 1) != 0)) && ((flags & 0x80) != 0)) {
    net_select_request_send(side);
    sel_card_event_done[side] = 1;
  }
  return;
}

void sel_exit_hold_check(void)
{
  int i;
  int on;
  for (i = 0; i < 2; i++) {
    char *a = (char *)&sel_just_decided;
    if (a[i] != 0) {
      char c;
      char *b = (char *)side_entry_flags;
      char *d;
      char *e;
      if (card_record_get(i)->in_use != 0 &&
          ((int)b[i] >> 2 & 1) != 0 &&
          item_has_carded(i, 0x88) != 0 &&
          (player_input_get(i)->now & 0xf) == 6)
        c = 1;
      else
        c = 0;
      d = rob_alt_model_card;
      d[i] = c;
      e = (char *)&sel_just_decided;
      e[i] = 0;
    }
  }
  if (msnm_active() == 0 && rob_alt_model_card[0] != 0 && rob_alt_model_card[1] != 0)
    on = 1;
  else
    on = 0;
  oldgauge_flag_set(on);
}

void sel_screen_reset(void)
{
  typedef struct {
    char c0; char p1[3];
    int a[43];
    int b[17];
    int c[2];
    int d[2];
    char p2[0x1a];
    char e; char p3;
    int f; int g;
    char p4[12];
    char h; char p5[3];
  } S;
  uchar i;
  
  memset(&sel_screen,0,0x138);
  for (i = 0; i < 0x2b; i++) ((S *)&sel_screen)->a[i] = -1;
  for (i = 0; i < 0x11; i++) ((S *)&sel_screen)->b[i] = -1;
  ((S *)&sel_screen)->c0 = 0;
  for (i = 0; i < 2; i++) {
    ((S *)&sel_screen)->c[i] = -1;
    ((S *)&sel_screen)->d[i] = -1;
  }
  ((S *)&sel_screen)->f = -1;
  ((S *)&sel_screen)->g = -1;
  ((S *)&sel_screen)->e = 0;
  ((S *)&sel_screen)->h = 0;
}

void sel_chara_pictures_release(int side)
{
  typedef struct { char pad[0x10]; int a[2]; int b[2]; char pad2[0xec - 0x20]; int c[2]; } fc50;
  int i = (char)side;
  aet_obj_stop(((fc50 *)&sel_screen)->b[i]);
  ((fc50 *)&sel_screen)->b[i] = -1;
  aet_still_stop(((fc50 *)&sel_screen)->c[i]);
  ((fc50 *)&sel_screen)->c[i] = -1;
  aet_obj_stop(((fc50 *)&sel_screen)->a[i]);
  ((fc50 *)&sel_screen)->a[i] = -1;
}

void sel_chara_labels_release(int side)
{
  int c = (char)side;
  char *p = (char *)&sel_screen;
  char *t1 = (char *)&sel_cursor_prev;
  char *t2;

  aet_still_stop(*(int *)(p + t1[c] * 4 + 0xb0));
  *(int *)(p + t1[c] * 4 + 0xb0) = -1;
  t2 = (char *)sel_cursor;
  aet_still_stop(*(int *)(p + t2[c] * 4 + 0xb0));
  *(int *)(p + t2[c] * 4 + 0xb0) = -1;
  aet_obj_stop(*(int *)(p + c * 4 + 0x28));
  *(int *)(p + c * 4 + 0x28) = -1;
  aet_obj_stop(*(int *)(p + 0x30));
  *(int *)(p + 0x30) = -1;
  aet_obj_stop(*(int *)(p + c * 4 + 8));
  *(int *)(p + c * 4 + 8) = -1;
}

void sel_chara_display_release(int side)
{
  typedef struct { int x0; int x4; int a[(0xf4 - 8) / 4]; int b[2]; int c[2]; } S;
  int i;
  int scene;
  
  i = (int)(char)side;
  aet_obj_stop(((S *)&sel_screen)->x4);
  ((S *)&sel_screen)->x4 = -1;
  aet_obj_stop(((S *)&sel_screen)->a[i]);
  ((S *)&sel_screen)->a[i] = -1;
  sel_chara_pictures_release(i);
  sel_chara_labels_release(i);
  scene = ((S *)&sel_screen)->b[i];
  if ((scene != -1) && (sel_cursor_prev.side0 != sel_cursor_prev.side1)) {
    aet_scene_textures_free(scene);
    sprite_set_loaded[56] = 1;
    sprite_set_loaded[57] = 1;
    sprite_set_loaded[58] = 1;
    ((S *)&sel_screen)->b[i] = -1;
  }
  if (sel_cursor_prev.side0 == sel_cursor_prev.side1) {
    ((S *)&sel_screen)->b[i] = -1;
  }
  if (((S *)&sel_screen)->c[i] != -1) {
    ((S *)&sel_screen)->c[i] = -1;
  }
}

void sel_scene_layers_clear(void)
{
  aet_objs_stop_by_set(9);
  aet_objs_stop_by_set(8);
  aet_objs_stop_by_set(7);
  if (sel_screen.chara_scene_1p != -1) {
    aet_scene_textures_free(sel_screen.chara_scene_1p);
  }
  if (sel_screen.chara_scene_2p != -1) {
    aet_scene_textures_free(sel_screen.chara_scene_2p);
  }
  aet_objs_stop_by_set(0xb);
  aet_objs_stop_by_scene(0x50000);
  aet_stills_stop_by_scene(0x60000);
  aet_stills_stop_by_scene(0x60001);
  aet_stills_stop_by_scene(0x60002);
  aet_stills_stop_by_scene(0x60003);
  aet_stills_stop_by_scene(0x60004);
  aet_stills_stop_by_scene(0x60005);
  aet_stills_stop_by_scene(0x60006);
  aet_stills_stop_by_scene(0x60007);
  aet_stills_stop_by_scene(0x60008);
  aet_stills_stop_by_scene(0x60009);
  aet_stills_stop_by_scene(0x6000a);
  aet_stills_stop_by_scene(0x6000b);
  aet_stills_stop_by_scene(0x6000c);
  aet_stills_stop_by_scene(0x6000d);
  aet_stills_stop_by_scene(0x6000e);
  return;
}

void sel_chara_scenes_unload(void)
{
  if (sel_screen.chara_scenes_unloaded == '\0') {
    sel_scene_layers_clear();
    aet_set_free(9);
    aet_set_free(8);
    aet_set_free(7);
    sprite_set_free(0x38);
    sprite_set_free(0x39);
    sprite_set_free(0x3a);
    aet_scene_textures_free(0xb000e);
    aet_scene_textures_free(0xb000d);
    aet_scene_textures_free(0xb000c);
    aet_scene_textures_free(0xb000b);
    aet_scene_textures_free(0xb000a);
    aet_scene_textures_free(0xb0009);
    aet_scene_textures_free(0xb0008);
    aet_scene_textures_free(0xb0007);
    aet_scene_textures_free(0xb0006);
    aet_scene_textures_free(0xb0005);
    aet_scene_textures_free(0xb0004);
    aet_scene_textures_free(0xb0003);
    aet_scene_textures_free(0xb0002);
    aet_scene_textures_free(0xb0001);
    aet_scene_textures_free(0xb0000);
    aet_set_free(0xb);
    aet_scene_textures_free(0x6000e);
    aet_scene_textures_free(0x6000d);
    aet_scene_textures_free(0x6000c);
    aet_scene_textures_free(0x6000b);
    aet_scene_textures_free(0x6000a);
    aet_scene_textures_free(0x60009);
    aet_scene_textures_free(0x60008);
    aet_scene_textures_free(0x60007);
    aet_scene_textures_free(0x60006);
    aet_scene_textures_free(0x60005);
    aet_scene_textures_free(0x60004);
    aet_scene_textures_free(0x60003);
    aet_scene_textures_free(0x60002);
    aet_scene_textures_free(0x60001);
    aet_scene_textures_free(0x60000);
    aet_set_free(6);
    aet_scene_textures_free(0x50000);
    aet_set_free(5);
  }
  return;
}

void sel_chara_scene_load(int side, int slot)
{
  typedef struct { char pad[0xf4]; int a[2]; int b[2]; } S;
  int i = (char)side;
  int j = (char)slot;

  if (j > 0xf) j = 0xf;
  if (sel_cursor[0] == sel_cursor[1])
    ((S *)&sel_screen)->a[i] = -1;
  if (((S *)&sel_screen)->a[i] == -1) {
    ((S *)&sel_screen)->a[i] = sel_chara_info[j].chara_scene;
    if (((S *)&sel_screen)->a[0] != ((S *)&sel_screen)->a[1])
      aet_scene_textures_load(((S *)&sel_screen)->a[i]);
  }
  if (((S *)&sel_screen)->b[i] == -1)
    ((S *)&sel_screen)->b[i] = sel_chara_info[j].name_scene;
}

void sel_chara_small_draw(int side, int slot)
{
  typedef struct { char pad[0x10]; int a10[2]; int a18[2]; int a20[2]; char p2[0xb0 - 0x28]; int b0[15]; int ec[2]; char p3[0x10c - 0xf4]; int x10c[2]; } S;
  aet_obj_param b1;
  aet_still_param b2;
  float xs[2];
  int j = (char)side;
  int i = (char)slot;
  int c;
  float x;
  float y;
  S *s;

  xs[0] = -200.0;
  xs[1] = 190.0;
  if (sel_both_players_in() != 0) {
    if (((S *)&sel_screen)->a18[j] != -1)
      aet_obj_pos_set(((S *)&sel_screen)->a18[j], xs[j] + sel_chara_info[i].versus_offsets[j][sel_costume_preview[j]], 0.0);
    if (((S *)&sel_screen)->a10[j] != -1)
      aet_obj_pos_set(((S *)&sel_screen)->a10[j], xs[j] + sel_chara_info[i].versus_offsets[j][sel_costume_preview[j]], 0.0);
    if (((S *)&sel_screen)->a20[j] != -1)
      aet_obj_pos_set(((S *)&sel_screen)->a20[j], xs[j] + sel_chara_info[i].versus_offsets[j][sel_costume_preview[j]], 0.0);
  }
  s = (S *)&sel_screen;
  c = ({ char *t = &sel_cursor[0]; t[j]; });
  if (s->b0[c] == -1) {
    x = (char)(c % 5) * 124.0;
    y = (char)(c / 5) * 74.0;
    aet_still_param_init(&b2);
    aet_still_param_scene_set(&b2,0xa0000);
    aet_still_param_layer_set(&b2,"GLASS_DEF_CHA");
    aet_still_param_depth_range_set(&b2,0.03,0.03);
    aet_still_param_pos_set(&b2,x,y);
    s->b0[i] = aet_still_start(&b2);
  }
  if (sel_chara_icon_needed(j) != 0) {
    aet_still_param_init(&b2);
    aet_still_param_scene_set(&b2,sel_chara_info[i].icon_scene);
    aet_still_param_layer_set(&b2,sel_chara_info[i].icon_layers[sel_costume_preview[j]]);
    aet_still_param_depth_range_set(&b2,0.01,0.01);
    aet_still_param_pos_set(&b2,sel_chara_info[i].icon_pos[sel_costume_preview[j]][0],sel_chara_info[i].icon_pos[sel_costume_preview[j]][1]);
    ((S *)&sel_screen)->ec[j] = aet_still_start(&b2);
  }
  if (((S *)&sel_screen)->x10c[j] < 1) {
    if (i > 0xf) i = 0xf;
    if (((S *)&sel_screen)->a18[j] == -1) {
      aet_param_init(&b1);
      aet_param_scene_set(&b1,0x50000);
      aet_param_layer_set(&b1,sel_chara_info[i].small_layers[sel_costume_preview[j]]);
      aet_param_depth_range_set(&b1,0.07,0.07);
      if (sel_both_players_in() != 0)
        aet_param_pos_set(&b1, xs[j] + sel_chara_info[i].versus_offsets[j][sel_costume_preview[j]], 0.0);
      ((S *)&sel_screen)->a18[j] = aet_obj_start(&b1);
    }
  }
}

void sel_chara_large_draw(int side, int slot)
{
  typedef struct {
    int f0; int f4; int a8[2]; int a10[2]; int a18[2]; int r20[2]; int a28[2]; int f30;
    char p34[0xc0]; int af4[2]; int afc[2]; char p104[8]; int a10c[2]; char p114[8]; char b11c[2];
  } S;
  int p;
  int n;
  int flag;
  aet_obj_param buf;
  float x[2];
  float y[2];
  float z[2];

  p = (char)side;
  n = (char)slot;
  flag = 0;
  x[0] = -200.0;
  x[1] = 190.0;
  y[0] = y[1] = -60.0;
  z[0] = z[1] = 0.0;
  if (sel_both_players_in() != 0) {
    aet_obj_stop(((S *)&sel_screen)->f30);
    ((S *)&sel_screen)->f30 = -1;
    flag = 1;
    y[0] = -345.0;
    y[1] = -120.0;
    z[0] = -275.0;
    z[1] = -65.0;
  }
  if (((S *)&sel_screen)->b11c[p] != flag) {
    aet_obj_stop(((S *)&sel_screen)->a28[p]);
    ((S *)&sel_screen)->a28[p] = -1;
  }
  ((S *)&sel_screen)->b11c[p] = flag;
  if (((S *)&sel_screen)->af4[p] == -1) return;
  if (n > 15) n = 15;
  if (((S *)&sel_screen)->f4 == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_layer_set(&buf,"BG_LOGOS");
    aet_param_depth_range_set(&buf,0.08,0.08);
    ((S *)&sel_screen)->f4 = aet_obj_start(&buf);
  }
  if (((S *)&sel_screen)->a8[p] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_layer_set(&buf,"CHA_NA_LINE");
    aet_param_depth_range_set(&buf,0.04,0.04);
    if (sel_both_players_in() != 0) {
      aet_param_pos_set(&buf,z[p],0.0);
    }
    ((S *)&sel_screen)->a8[p] = aet_obj_start(&buf);
  }
  if (((S *)&sel_screen)->a10c[p] > 0) return;
  if (((S *)&sel_screen)->a18[p] != -1 &&
      aet_obj_is_playing(((S *)&sel_screen)->a18[p]) == 0 &&
      ((S *)&sel_screen)->a10[p] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,((S *)&sel_screen)->af4[p]);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,sel_chara_info[n].large_layers[sel_costume_preview[p]]);
    aet_param_depth_range_set(&buf,0.06,0.06);
    if (sel_both_players_in() != 0) {
      aet_param_pos_set(&buf,x[p] + sel_chara_info[n].versus_offsets[p][sel_costume_preview[p]],0.0);
    }
    ((S *)&sel_screen)->a10[p] = aet_obj_start(&buf);
  }
  if (((S *)&sel_screen)->a10[p] != -1 && ((S *)&sel_screen)->a28[p] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,((S *)&sel_screen)->afc[p]);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,sel_chara_info[n].name_layers[flag]);
    aet_param_depth_range_set(&buf,0.05,0.05);
    aet_param_pos_set(&buf,y[p],-100.0);
    ((S *)&sel_screen)->a28[p] = aet_obj_start(&buf);
  }
  if (((S *)&sel_screen)->a10[p] != -1 && sel_both_players_in() == 0 &&
      ((S *)&sel_screen)->f30 == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,((S *)&sel_screen)->afc[p]);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,sel_chara_info[n].profile_layer);
    aet_param_depth_range_set(&buf,0.05,0.05);
    aet_param_pos_set(&buf,-170.0,0.0);
    ((S *)&sel_screen)->f30 = aet_obj_start(&buf);
  }
}

void sel_cursor_moved_reset(char side)
{
  typedef struct { char p0[0x3c]; int a3c[2]; int a44[2]; int a4c[2]; char p1[0xfc - 0x54]; uint f0fc[2]; uint f104[2]; int f10c[2]; int f114[2]; } S;

  ((S *)&sel_screen)->f10c[side] = 7;
  ((S *)&sel_screen)->f114[side] = 10;
  if (((S *)&sel_screen)->f104[side] & 0x10) {
    ((S *)&sel_screen)->f104[0] &= ~0x12;
    ((S *)&sel_screen)->f104[1] &= ~0x12;
  }
  else if (((S *)&sel_screen)->f104[side] & 0x10) {
    ((S *)&sel_screen)->f104[side] &= ~0x12;
  }
  else if (((S *)&sel_screen)->f104[side] & 2) {
    ((S *)&sel_screen)->f104[side] &= ~2;
  }
  aet_obj_stop(((S *)&sel_screen)->a3c[side]);
  ((S *)&sel_screen)->a3c[side] = -1;
  aet_obj_stop(((S *)&sel_screen)->a44[side]);
  ((S *)&sel_screen)->a44[side] = -1;
  aet_obj_stop(((S *)&sel_screen)->a4c[side]);
  ((S *)&sel_screen)->a4c[side] = -1;
}

void sel_chara_decided_show(int side)
{
  typedef struct {
    int f0; int f4; int a8[2]; int a10[2]; int a18[2]; int a20[2]; int a28[2]; int f30;
    char p34[0xc0]; int af4[2]; int afc[2]; int a104[2]; int a10c[2]; char p114[8]; char b11c[2];
  } S;
  char p;
  aet_obj_param buf;
  float x[2];

  p = side;
  x[0] = -200.0;
  x[1] = 190.0;
  sel_decide_lines_show(p);
  if ((((S *)&sel_screen)->a104[p] & 4) == 0) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,((S *)&sel_screen)->af4[p]);
    aet_param_layer_set(&buf,sel_chara_info[sel_cursor[p]].decided_layers[sel_costume_preview[p]]);
    aet_param_depth_range_set(&buf,0.06,0.06);
    if (sel_both_players_in() != 0) {
      aet_param_pos_set(&buf,x[p] + sel_chara_info[sel_cursor[p]].versus_offsets[p][sel_costume_preview[p]],0.0);
    }
    ((S *)&sel_screen)->a20[p] = aet_obj_start(&buf);
    ((S *)&sel_screen)->a104[p] |= 4;
  }
}

void sel_decide_lines_show(char side)
{
  typedef struct { char pad[0x34]; int a[8]; int b[8]; } X;
  int sub;
  aet_obj_param param;
  
  sub = mode_get_sub();
  if ((5 < sub) && ((sel_stage_grid[stage_cursor].flags & 4) != 0)) {
    sel_stage_preview_release(0);
    stage_cursor_prev = stage_cursor;
    stage_cursor = stage_random_slot;
  }
  if (((X *)&sel_screen)->b[side] == -1) {
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0001);
    aet_param_layer_set(&param,"LINE_1");
    aet_param_depth_range_set(&param,0.006,0.006);
    ((X *)&sel_screen)->b[side] = aet_obj_start(&param);
  }
  if (((X *)&sel_screen)->a[side] == -1) {
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0000);
    aet_param_layer_set(&param,"RED_LASER");
    aet_param_depth_range_set(&param,0.04,0.04);
    ((X *)&sel_screen)->a[side] = aet_obj_start(&param);
  }
}

void sel_chara_objects_release(void)
{
  char i;
  
  if (sel_screen.chara_icon_still_1p != -1) {
    aet_still_stop(sel_screen.chara_icon_still_1p);
    sel_screen.chara_icon_still_1p = -1;
  }
  if (sel_screen.chara_icon_still_2p != -1) {
    aet_still_stop(sel_screen.chara_icon_still_2p);
    sel_screen.chara_icon_still_2p = -1;
  }
  for (i = 0; i < 15; i++) {
    aet_still_stop(((struct { char pad[0xb0]; int a[15]; } *)&sel_screen)->a[i]);
    ((struct { char pad[0xb0]; int a[15]; } *)&sel_screen)->a[i] = -1;
  }
  if (sel_screen.vs_batu_obj != -1) {
    aet_obj_stop(sel_screen.vs_batu_obj);
    sel_screen.vs_batu_obj = -1;
  }
  return;
}

void sel_data_layer_show(int side)
{
  int obj;
  aet_obj_param param;
  char *names[2];
  
  if (*(int *)(&sel_screen.data_obj_1p + (char)side * 4) == -1) {
    names[0] = "SEL_DATA_1P";
    names[1] = "SEL_DATA_2P";
    aet_param_init(&param);
    aet_param_scene_set(&param,0xa0000);
    aet_param_layer_set(&param,names[(char)side]);
    aet_param_depth_range_set(&param,0.03,0.03);
    obj = aet_obj_start(&param);
    *(int *)(&sel_screen.data_obj_1p + (char)side * 4) = obj;
  }
  return;
}

void sel_data_layer_release(char side)
{
  int obj;
  int i;
  int *p;
  int *q;
  char *b;
  i = side;
  if (card_message_active(i) != 0) {
    b = (char *)&sel_screen + i * 4;
    p = (int *)(b + 0x98);
    obj = *p;
    if (obj != -1) {
      aet_obj_stop(obj);
      *p = -1;
    }
  }
  if (card_message_active(i) == 0) {
    q = (int *)(&sel_screen.data_obj_1p + i * 4);
    obj = *q;
    if (obj != -1) {
      aet_obj_stop(obj);
      *q = -1;
    }
  }
}

int sel_data_layer_done(char side)
{
  int result;
  int done;
  int b;
  int s;
  int o;
  done = 0;
  b = (int)&sel_screen.data_obj_1p;
  if ((*(int *)(side * 4 + b) != -1) && (result = aet_obj_is_playing(*(int *)(side * 4 + b)), result == 0)) {
    done = 1;
  }
  s = (int)&sel_screen;
  o = 0x98;
  if ((*(int *)(side * 4 + s + o) != -1) && (result = aet_obj_end_reached(*(int *)(side * 4 + s + o)), result != 0)) {
    done = 1;
  }
  return done;
}

bool sel_card_layer_idle(int side, int which)
{
  int i = (char)side;
  char m = (char)which;
  bool r = 0;
  if (m == 0) {
    if (!card_message_active(i)) r = 1;
    if (card_message_active(i)) {
      if (((struct { char pad[0x98]; int a[4]; } *)&sel_screen)->a[i] == -1) r = 1;
    }
    if (((struct { char pad[0x70]; int a[4]; } *)&sel_screen)->a[i] != -1) {
      if (aet_obj_is_playing(((struct { char pad[0x70]; int a[4]; } *)&sel_screen)->a[i]) == 0) r = 1;
    }
  }
  else if (m == 1) {
    if (((struct { char pad[0x88]; int a[4]; } *)&sel_screen)->a[i] == -1) r = 1;
    if (((struct { char pad[0x88]; int a[4]; } *)&sel_screen)->a[i] != -1) {
      if (aet_obj_end_reached(((struct { char pad[0x88]; int a[4]; } *)&sel_screen)->a[i])) r = 1;
    }
  }
  return r;
}

void sel_ksn_show(int side)
{
  char c = side;
  char *s;
  aet_obj_param buf;
  char *names[2];
  names[0] = "KSN_1P";
  names[1] = "KSN_2P";
  s = (char *)&sel_screen;
  if (((struct { char pad[0x88]; int f[2]; } *)s)->f[c] == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,names[c]);
    aet_param_depth_range_set(&buf,0.01,0.01);
    ((struct { char pad[0x88]; int f[2]; } *)s)->f[c] = aet_obj_start(&buf);
  }
}

void sel_ksn_release(char side)
{
  int obj;
  int *p;
  int *q;
  
  q = (int *)&sel_screen + side;
  p = q + 0x22;
  obj = *p;
  if (obj != -1) {
    aet_obj_stop(obj);
    *p = -1;
  }
  return;
}

bool sel_chara_icon_needed(int side)
{
  typedef struct { char pad[0xec]; int a[2]; } fc_arr;
  int i;
  int o;
  bool r;
  char *b;
  
  i = (char)side;
  r = 0;
  o = 1; if (i) o = 0;
  if (((fc_arr *)&sel_screen)->a[i] == -1) {
    if (sel_cursor[0] != sel_cursor[1]) {
      r = 1;
    }
    if ((side_entry_flags[0] & 2) != 0 && (side_entry_flags[1] & 2) != 0 &&
        sel_cursor[0] == sel_cursor[1] &&
        ((fc_arr *)&sel_screen)->a[0] == -1 && ((fc_arr *)&sel_screen)->a[1] == -1) {
      r = 1;
    }
    b = (char *)side_entry_flags;
    if ((b[o] & 1) != 0 &&
        sel_cursor[0] == sel_cursor[1] &&
        ((fc_arr *)&sel_screen)->a[i] == -1) {
      aet_still_stop(((fc_arr *)&sel_screen)->a[o]);
      ((fc_arr *)&sel_screen)->a[o] = 0xffffffff;
      r = 1;
    }
  }
  return r;
}

void sub_sel_1pmode_init(void)
{
  sel_1pmode_choice = 0;
  sel_1pmode_move_wait = 0;
  sel_1pmode_player = 0;
  if ((side_entry_flags[0] & 4U) != 0) {
    sel_1pmode_player = 0;
  }
  else if ((side_entry_flags[1] & 4U) != 0) {
    sel_1pmode_player = 1;
  }
  mode_timer = 0x4b0;
  mode_timer_float = 1200.0;
  mode_req_sub(9);
  sel_chara_objects_release();
  sel_cursor_layers_release();
  sel_screen.cursor_flags_1p = 0;
  sel_screen.cursor_flags_2p = 0;
  sel_1pmode_screen_draw();
}

void sub_sel_1pmode(void)
{
  int sub;
  int next_sub;
  player_input_rec *in;
  uchar i;

  for (i = 0; i < 2; i++) {
    if (sel_data_layer_done((char)i) != 0) {
      sel_data_layer_release((char)i);
    }
  }
  sel_1pmode_screen_draw();
  sub = mode_get_sub();
  if (sub != 10) {
    sel_timer_draw(270.0,5.0,0.006);
  }
  if (sel_both_players_in() != 0) {
    next_sub = 2;
  }
  else {
    if (aet_obj_end_reached(sel_screen.chara_to_mode_obj) == 0) {
      return;
    }
    in = player_input_get((int)sel_1pmode_player);
    if (sel_1pmode_move_wait < 1) {
      int choice;
      choice = sel_1pmode_choice;
      if (((in->pressed | in->now) & 0x20) != 0) {
        choice = 0;
      }
      else if (((in->pressed | in->now) & 0x10) != 0) {
        choice = 1;
      }
      if (choice != sel_1pmode_choice) {
        sel_1pmode_choice = choice;
        sel_1pmode_move_wait = 10;
        sound_play_id(0x3af,0);
        aet_obj_stop(sel_screen.mode_normal_obj);
        sel_screen.mode_normal_obj = 0xffffffff;
        aet_obj_stop(sel_screen.mode_chall_obj);
        sel_screen.mode_chall_obj = 0xffffffff;
        aet_obj_stop(sel_screen.mode_chall_obj);
        sel_screen.mode_chall_obj = 0xffffffff;
        aet_obj_stop(sel_screen.mode_flash_obj);
        sel_screen.mode_flash_obj = 0xffffffff;
        aet_obj_stop(sel_screen.mode_base_obj);
        sel_screen.mode_base_obj = 0xffffffff;
        sel_screen.cursor_idle_timer_1p = 0x11;
        sel_screen.cursor_flags_1p &= 0xfffffffd;
        aet_obj_stop(sel_screen.mode_cursor_obj);
        sel_screen.mode_cursor_obj = 0xffffffff;
      }
    }
    sel_1pmode_move_wait = sel_1pmode_move_wait + -1;
    if (0 < sel_screen.cursor_idle_timer_1p) {
      sel_screen.cursor_idle_timer_1p = sel_screen.cursor_idle_timer_1p + -1;
    }
    if (sel_1pmode_move_wait < 1) {
      sel_1pmode_move_wait = 0;
    }
    if (((in->pressed & 0xf) == 0) && (0 < mode_timer)) {
      return;
    }
    mode_timer = 0x5a;
    mode_timer_float = 90.0;
    sound_play_id(0x3ae,0);
    next_sub = 10;
  }
  mode_req_sub(next_sub);
}

void sub_sel_1pmode_decide(void)
{
  sel_1pmode_screen_draw();
  if (sel_both_players_in() != 0) {
    msnm_end();
    mode_req_sub(2);
    return;
  }
  if (0 < mode_timer) {
    return;
  }
  if (sel_1pmode_choice == 1) {
    msnm_start((int)sel_1pmode_player);
    msnm_resume();
    rank_round = 0;
  }
  else {
    msnm_end();
    msnm_side_keep((int)sel_1pmode_player);
  }
  mode_req_sub(0xb);
}

void sel_1pmode_screen_draw(void)
{
  int sub;
  
  int decided = 0;
  sub = mode_get_sub();
  if (sub == 10) decided = 1;
  sel_1pmode_cursor_draw(sel_1pmode_choice,decided);
  sel_1pmode_layers_show(sel_1pmode_choice);
  sel_chara_screen_draw();
  return;
}

void sel_1pmode_layers_show(int choice)
{
  int done;
  aet_obj_param buf;
  char *n0[2] = { "MOD_NORMAL_IN", "MOD_NORMAL_DEF" };
  char *n1[2] = { "MOD_CHALL_DEF", "MOD_CHALL_IN" };
  char *n2[2] = { "MOD_MESSAGE_IN", "MOD_MESSAGE" };

  if ((sel_screen.mode_normal_obj == -1) &&
     (done = aet_obj_end_reached(sel_screen.chara_to_mode_obj), done != 0)) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,n0[choice]);
    aet_param_depth_range_set(&buf,0.02,0.02);
    sel_screen.mode_normal_obj = aet_obj_start(&buf);
  }
  if ((sel_screen.mode_chall_obj == -1) &&
     (done = aet_obj_end_reached(sel_screen.chara_to_mode_obj), done != 0)) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,n1[choice]);
    aet_param_depth_range_set(&buf,0.02,0.02);
    sel_screen.mode_chall_obj = aet_obj_start(&buf);
  }
  if ((sel_screen.mode_or_obj == -1) &&
     (done = aet_obj_end_reached(sel_screen.chara_to_mode_obj), done != 0)) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,2);
    aet_param_layer_set(&buf,"MOD_OR");
    aet_param_depth_range_set(&buf,0.02,0.02);
    sel_screen.mode_or_obj = aet_obj_start(&buf);
  }
  if ((sel_screen.mode_message_state == '\0') && (sel_screen.mode_message_obj == -1)) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_layer_set(&buf,n2[(char)sel_screen.mode_message_state]);
    aet_param_depth_range_set(&buf,0.02,0.02);
    sel_screen.mode_message_obj = aet_obj_start(&buf);
    sel_screen.mode_message_state = '\x01';
  }
  if ((sel_screen.mode_message_state == '\x01') &&
     (done = aet_obj_is_playing(sel_screen.mode_message_obj), done == 0)) {
    aet_obj_stop(sel_screen.mode_message_obj);
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,2);
    aet_param_layer_set(&buf,n2[(char)sel_screen.mode_message_state]);
    aet_param_depth_range_set(&buf,0.02,0.02);
    sel_screen.mode_message_obj = aet_obj_start(&buf);
  }
  if (sel_screen.chara_to_mode_obj == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_flags_set(&buf,4);
    aet_param_layer_set(&buf,"CHARA2MODE");
    aet_param_depth_range_set(&buf,0.03,0.03);
    sel_screen.chara_to_mode_obj = aet_obj_start(&buf);
    sound_play_id(0x377,0);
  }
  return;
}

void sel_1pmode_cursor_draw(int choice, int decide)
{
  aet_obj_param buf;
  uchar decided = (uchar)decide;
  char *layer;
  int flash = -1;
  char *names[5][2] = {
    { "CSR_MOD_1P_L", "CSR_MOD_2P_L" },
    { "CSR_MOD_1P_R", "CSR_MOD_2P_R" },
    { "CSR_MOD_1P_TAIKI", "CSR_MOD_2P_TAIKI" },
    { "CSR_MOD_1P_KETTEI", "CSR_MOD_2P_KETTEI" },
    { "MOD_BASE_1P", "MOD_BASE_2P" }
  };
  char *flash_names[2][2] = {
    { "MOD_FLASH_1P_IN", "MOD_FLASH_2P_IN" },
    { "MOD_FLASH_1P_KETTEI", "MOD_FLASH_2P_KETTEI" }
  };
  int second;

  second = 0;
  if (side_entry_flags[1] & 4) second = 1;
  if (aet_obj_end_reached(sel_screen.chara_to_mode_obj) == 0) return;
  if (sel_screen.cursor_flags_1p & 8) return;
  layer = names[choice][second];
  if (sel_screen.cursor_idle_timer_1p == 0 && (sel_screen.cursor_flags_1p & 2) == 0) {
    layer = names[2][second];
    aet_obj_stop(sel_screen.mode_cursor_obj);
    sel_screen.mode_cursor_obj = -1;
    sel_screen.cursor_flags_1p |= 2;
    flash = 0;
  }
  if (decided != 0 && (sel_screen.cursor_flags_1p & 8) == 0) {
    layer = names[3][second];
    aet_obj_stop(sel_screen.mode_cursor_obj);
    sel_screen.mode_cursor_obj = -1;
    aet_obj_stop(sel_screen.mode_flash_obj);
    sel_screen.mode_flash_obj = -1;
    sel_screen.cursor_flags_1p |= 8;
    flash = 1;
  }
  if (sel_screen.mode_cursor_obj == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0000);
    aet_param_layer_set(&buf,layer);
    if ((sel_screen.cursor_flags_1p & 8) == 0) aet_param_flags_set(&buf,2);
    aet_param_depth_range_set(&buf,0.02,0.02);
    if (choice == 1 && (sel_screen.cursor_flags_1p & 10) != 0) aet_param_pos_set(&buf,372.0,0.0);
    sel_screen.mode_cursor_obj = aet_obj_start(&buf);
  }
  if (flash != -1) {
    if (sel_screen.mode_flash_obj == -1) {
      aet_param_init(&buf);
      aet_param_scene_set(&buf,0xa0000);
      aet_param_layer_set(&buf,flash_names[flash][second]);
      aet_param_depth_range_set(&buf,0.006,0.006);
      if (flash == 0) aet_param_flags_set(&buf,4);
      if (choice == 1) aet_param_pos_set(&buf,372.0,0.0);
      sel_screen.mode_flash_obj = aet_obj_start(&buf);
    }
    if (flash != -1 && sel_screen.mode_base_obj == -1) {
      aet_param_init(&buf);
      aet_param_scene_set(&buf,0xa0000);
      aet_param_layer_set(&buf,names[4][second]);
      aet_param_depth_range_set(&buf,0.05,0.05);
      aet_param_flags_set(&buf,2);
      if (choice == 1) aet_param_pos_set(&buf,372.0,0.0);
      sel_screen.mode_base_obj = aet_obj_start(&buf);
    }
  }
  if (flash == 1 && sel_screen.mode_line_obj == -1) {
    aet_param_init(&buf);
    aet_param_scene_set(&buf,0xa0001);
    aet_param_layer_set(&buf,"LINE_1");
    aet_param_depth_range_set(&buf,0.006,0.006);
    sel_screen.mode_line_obj = aet_obj_start(&buf);
  }
}

void sel_1pmode_layers_release(void)
{
  int ids[9];
  char i;
  int *t;
  ids[0] = 0x19;
  ids[1] = 0x17;
  ids[2] = 0x18;
  ids[3] = 0x16;
  ids[4] = 0x1a;
  ids[5] = 0x20;
  ids[6] = 0x29;
  ids[7] = 0x2a;
  ids[8] = -1;
  i = 0;
  t = (int *)&sel_screen;
  do {
    if (t[ids[i] + 1] != -1) {
      aet_obj_stop(t[ids[i] + 1]);
      t[ids[i] + 1] = -1;
    }
    i++;
  } while (ids[i] != -1);
}

void sel_fade_draw(byte frames, float step)
{
  if (sel_screen.fade_frame < frames) {
    sel_screen.fade_alpha = (float)sel_screen.fade_frame * step;
    if (0.9 < sel_screen.fade_alpha) {
      sel_screen.fade_alpha = 0.9;
      sel_screen.fade_done = 1;
    }
  }
  else {
    sel_screen.fade_alpha = 0.9 - (float)(int)((sel_screen.fade_frame - frames) + 1) * step;
    if (sel_screen.fade_alpha < 0.0) {
      sel_screen.fade_alpha = 0.0;
    }
  }
  sprite_colors_set(0xffffffff,0xffffffff);
  sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.04,80.0,60.0,sel_screen.fade_alpha);
  sprite_colors_reset();
}

void empty_fn_0c04f4c0(void)
{
  return;
}

void empty_fn_0c04f4e0(void)
{
  return;
}

void frame_hook_empty(void)
{
  return;
}
