/*
 * vf4lib/libcard/src/card_mode.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sel_arrow_show
#define KRPROTO_sprite_put_flags_xyz
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int card_message_table[33][5]; /* 0x0c187b74 */
extern int sample_cards[28][2]; /* 0x0c188240 */
extern int game_frame_count; /* 0x0c292540 */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern char play_style; /* 0x0c2925cc */
extern char champion_side; /* 0x0c2925d4 */
extern int win_streak; /* 0x0c2925dc */
extern int card_renew_text_recs[2][4]; /* 0x0c2a01b8 */
extern int card_renew_write_begun[2]; /* 0x0c2a01d8 */
extern int card_result_write_error; /* 0x0c2a01e0 */
extern int card_unit_cmd_cooldown[2]; /* 0x0c2a01e4 */
extern float card_message_fade[2]; /* 0x0c2a01ec */
extern int sample_card_index; /* 0x0c2a01f4 */
extern int card_user_count; /* 0x0c37e3b0 */
extern char card_renew_state[2][16]; /* 0x0c37e3b4 */
extern int card_upgraded[2]; /* 0x0c37e3d4 */
extern char card_unit_used[2]; /* 0x0c37e3dc */
extern char card_mode_slots[2][4]; /* 0x0c37e3de */
extern char card_debug_requests[2][4]; /* 0x0c37e3e6 */
extern int card_renew_times[2][3]; /* 0x0c37e3f0 */
extern int card_start_write_error; /* 0x0c37e408 */
extern uint card_flags[2]; /* 0x0c37e40c */
extern char card_messages[2][8]; /* 0x0c43137c */
extern int streak_end_flag; /* 0x0c43138c */
extern card_renew_text card_renew_text_table[27]; /* 0x0c187e08 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

static inline int player_word104(int side)
{
  return card_records[side].in_use;
}

static inline struct
{
  char c[1];
} *byte_view(char *bytes)
{
  return (void *) bytes;
}

int card_write_step(int side, card_mode_slot *slot)
{
  int result;
  int r = 0;

  switch(slot->sub_step) {
  case 0:
    slot->retries = 0;
    slot->sub_step = 1;
  case 1:
    if ((card_flags[side] & 1) == 0) {
      slot->sub_step = 0xb;
    }
    else if (card_write_start(side,1) == 0) {
      slot->sub_step = 2;
    }
    break;
  case 2:
    result = card_unit_result(side);
    if (result == 1) break;
    if (result == 0) {
      slot->sub_step = 10;
    }
    else if (slot->retries < 4) {
      slot->retries = slot->retries + 1;
      slot->sub_step = 1;
    }
    else {
      slot->sub_step = 0xb;
    }
    break;
  case 10:
    r = 1;
    break;
  case 11:
  default:
    r = 2;
  }
  return r;
}

int card_read_step(int side, card_mode_slot *slot)
{
  int r;
  uint *flags;
  int result;

  r = 0;
  switch (slot->sub_step) {
  case 0:
    slot->retries = 0;
    slot->sub_step = 1;
  case 1:
    flags = &card_flags[side];
    if ((*flags & 1) != 0) {
      if (card_read_start(side) == 0) {
        *flags |= 0x200;
        slot->sub_step = 2;
      }
    }
    else {
      slot->sub_step = 11;
    }
    break;
  case 2:
    result = card_unit_result(side);
    if (result == 1) break;
    if (result == 0) {
      card_read_finish(side);
      slot->sub_step = 10;
      break;
    }
    if (slot->retries < 1) {
      slot->retries += 1;
      slot->sub_step = 1;
    }
    else {
      slot->sub_step = 11;
    }
    break;
  case 10:
    r = 1;
    break;
  case 11:
  default:
    r = 2;
    break;
  }
  return r;
}

int card_insert_wait_step(side, slot, kind, new_play_id, old_play_id) int side; card_mode_slot *slot; int kind; ushort new_play_id; ushort old_play_id;
{
  ushort play_id;
  int result;
  uint *flags;
  int ret;

  ret = 0;
  switch (slot->sub_step) {
  case 0:
    if ((card_flags[side] & 2) != 0) {
      slot->sub_step = 1;
    }
    break;
  case 1:
    flags = &card_flags[side];
    if ((*flags & 1) != 0) {
      if (card_read_start(side) == 0) {
        slot->sub_step = 2;
        *flags = *flags | 0x200;
      }
    }
    else {
      slot->sub_step = 0;
    }
    break;
  case 2:
    result = card_unit_result(side);
    if (result == 1) break;
    if (result == 0) {
      card_read_finish(side);
      slot->sub_step = 3;
    }
    else {
      slot->sub_step = 0xb;
    }
    break;
  case 3:
    slot->sub_step = 4;
    if (kind == 1) {
      if (card_raw_is_blank(side) != 0) {
        slot->sub_step = 10;
      }
      break;
    }
    if (card_raw_is_blank(side) != 0) break;
    play_id = card_records[side].body.play_id;
    result = card_format_check_upgrade(side);
    if (result == 3) {
      slot->sub_step = 5;
    }
    else if (result == 2) {
      if ((play_id == new_play_id) | (play_id == old_play_id)) {
        slot->sub_step = 8;
      }
    }
    else if (play_id == old_play_id) {
      slot->sub_step = 9;
    }
    break;
  case 4:
    slot->sub_step = 0;
    ret = 3;
    break;
  case 5:
    slot->sub_step = 0;
    ret = 4;
    break;
  case 8:
    ret = 5;
    break;
  case 9:
    ret = 6;
    break;
  case 10:
    ret = 1;
    break;
  case 11:
  default:
    ret = 2;
    slot->sub_step = 0;
    break;
  }
  return ret;
}

void card_mode_entry_read(int side)
{
  card_mode_slot *slot;
  uint *flags;
  uint bits;
  card_record_rec *record;
  card_image_rec *image;
  char side_byte;
  int result;

  slot = (card_mode_slot *)card_mode_slots[side];
  switch (slot->step) {
  case 0:
    {
    uint *flags0 = &card_flags[side];
    bits = *flags0;
    bits &= ~0x100;
    bits &= ~0x80;
    bits &= ~0x10;
    bits &= ~0x20;
    bits &= ~0x8;
    *flags0 = bits;
    }
    slot->sub_step = 0;
    slot->step = 1;
    card_upgraded[side] = 0;
  case 1:
    result = card_read_step(side, slot);
    if (result == 1) {
      if (card_records[side].in_use == 1) {
        slot->step = 3;
      }
      else {
        slot->step = 2;
      }
    }
    else if (result == 2) {
      slot->step = 8;
      card_message_post((char)side, 1, 60, 0);
    }
    break;
  case 2:
    if (card_header_check(side) == 0) {
      slot->step = 8;
      card_message_post((char)side, 17, -1, 0);
      break;
    }
    if (card_raw_is_blank(side) != 0) {
      slot->step = 10;
      break;
    }
    switch (card_format_check_upgrade(side)) {
    case 0:
    case 1:
      slot->step = 11;
      break;
    case 3:
      slot->step = 8;
      card_message_post((char)side, 10, -1, 0);
      break;
    case 4:
      slot->step = 8;
      card_message_post((char)side, 12, -1, 0);
      break;
    default:
      slot->step = 8;
      card_message_post((char)side, 17, -1, 0);
      break;
    }
    break;
  case 3:
    if (card_header_check(side) == 0) {
      slot->step = 8;
      card_message_post((char)side, 17, -1, 0);
      break;
    }
    switch (card_format_check_upgrade(side)) {
    case 0:
    case 1:
      if (card_play_id_matches(side) == 0) {
        slot->step = 8;
        card_message_post((char)side, 20, -1, 0);
      }
      else {
        slot->step = 12;
      }
      break;
    case 3:
      slot->step = 8;
      card_message_post((char)side, 10, -1, 0);
      break;
    case 4:
      slot->step = 8;
      card_message_post((char)side, 12, -1, 0);
      break;
    default:
      slot->step = 8;
      card_message_post((char)side, 17, -1, 0);
      break;
    }
    break;
  case 8:
    card_flags[side] |= 0x100;
    slot->step = 20;
    break;
  case 10:
    card_flags[side] |= 0x88;
    side_byte = (char)side;
    card_side_reset(side_byte);
    record = &card_records[side];
    record->header.use_count = 500;
    image = &card_images[side];
    image->header.use_count = 500;
    record->header.flags = (record->header.flags & 0x8c) | 0x33;
    record->header.ftf.b[0] &= 0xf9;
    record->header.ftf.f.ex_ver = 1;
    image->header.ftf.b[0] = record->header.ftf.b[0];
    image->header.flags = record->header.flags;
    card_images[side].access_key = card_records[side].access_key;
    card_message_post(side_byte, 16, -1, 0);
    slot->step = 20;
    break;
  case 11:
    flags = &card_flags[side];
    *flags |= 0x90;
    if (card_plays_exhausted(side)) {
      *flags |= 0x20;
    }
    card_record_to_image(side);
    card_message_post((char)side, 0, -1, 0);
    slot->step = 20;
    break;
  case 12:
    flags = &card_flags[side];
    *flags |= 0x90;
    if (card_plays_exhausted(side)) {
      *flags |= 0x20;
    }
    card_message_post((char)side, 0, -1, 0);
    slot->step = 20;
    break;
  case 20:
  default:
    card_mode_slot_clear(side);
    break;
  }
}

void card_mode_select_write(int side)
{
  card_mode_slot *slot;
  card_record_rec *record;
  card_body_rec *body;
  card_image_rec *image;
  int result;
  int count_use;
  slot = (card_mode_slot *)card_mode_slots[side];
  record = &card_records[side];
  body = &card_records[side].body;
  net_info_get();
  switch (slot->step) {
  case 0:
    card_start_write_error = 0;
    card_flags[side] |= 0x2000;
    slot->step = 1;
  case 1:
    if ((card_flags[side] & 1) == 0) {
      slot->step = 8;
    }
    else {
      if (card_header_check(side) == 0) {
        slot->step = 8;
        card_start_write_error = 1;
        return;
      }
      slot->step = 2;
    }
    break;
  case 2:
    if ((card_flags[side] & 8) != 0) {
      item_lottery_kind1_set(side);
    }
    card_flags[side] &= ~8;
    if (((&side_entry_flags[0])[side + 1 & 1] >> 2 & 1) != 0) {
      card_data_log_pick();
      card_data_log_snapshot((char)side, 0);
      card_loss_apply(side);
      card_data_log_snapshot((char)side, 1);
    }
    body->play_id = lcg_next15();
    image = &card_images[side];
    image->body.play_id = body->play_id;
    record->header.flags |= 0x80;
    image->header.flags |= 0x80;
    if (net_ready_and_idle() == 0) {
      record->header.flags &= 0xf7;
      image->header.flags &= 0xf7;
    }
    count_use = 1;
    if (card_records[side].in_use == 1) { int t = win_streak == 0; count_use = t == 0; }
    if (count_use && record->header.use_count > 0) {
      record->header.use_count -= 1;
    }
    card_images[side].header.use_count = record->header.use_count;
    slot->sub_step = 0;
    slot->step = 3;
  case 3:
    result = card_write_step(side, slot);
    if (result == 1) {
      slot->step = 10;
    }
    else if (result == 2) {
      slot->step = 8;
      card_start_write_error = result;
    }
    break;
  case 8:
    card_records[side].in_use = 0;
    card_message_post((char)side, 2, -1, 0);
    card_side_reset((char)side);
    card_flags[side] &= ~0x80;
    slot->step = 20;
    break;
  case 10:
    card_records[side].in_use = 1;
    slot->step = 20;
    break;
  case 20:
  default:
    card_mode_slot_clear(side);
    break;
  }
}

void card_mode_renew(int side)
{
  typedef struct { int step; int count; int retries; int request_index; } unit_step;
  player_input_rec *input;
  card_mode_slot *slot;
  card_renew_state_rec *renew;
  card_record_rec *record;
  card_header_rec *header2;
  card_body_rec *body;
  uint *card_words;
  int result;
  char *backup;

  input = player_input_get(side);
  slot = (card_mode_slot *)card_mode_slots[side];
  renew = (card_renew_state_rec *)card_renew_state[side];
  record = &card_records[side];
  header2 = &card_records[side].header2;
  body = &card_records[side].body;
  card_words = &card_records[side].card_word0;
  net_info_get();
  if (slot->step == 6 && renew->cancel != 0) {
    slot->step = 10;
    card_renew_countdown_set(side, 0);
    card_renew_text_post(side, 120, 0, 9);
  }
  switch (slot->step) {
  case 0:
    if (sel_card_layer_idle((char)side, 1) == 0) {
      return;
    }
    slot->sub_step = 0;
    slot->step = 1;
  case 1:
    slot->sub_step = 0;
    slot->step = 2;
    card_renew_countdown_set(side, 600);
  case 2:
    if ((input->pressed & 0x30) != 0) {
      slot->sub_step = (slot->sub_step + 1) & 1;
    }
    if ((input->pressed & 7) != 0) {
      sel_arrow_hide(side);
      if (slot->sub_step != 0) {
        card_renew_text_post(side, 0, 1, 2);
        slot->step = 3;
      }
      else {
        slot->step = 18;
        card_renew_text_post(side, 120, 0, 9);
      }
      card_renew_countdown_set(side, 0);
    }
    else if ((card_flags[side] & 0x10) == 0 || card_renew_countdown_done(side) != 0) {
      sel_arrow_hide(side);
      slot->step = 18;
      card_renew_text_post(side, 120, 0, 9);
      card_renew_countdown_set(side, 0);
    }
    else {
      if ((card_images[side].header.flags & 4) == 0 || net_ready_and_idle() != 0) {
        card_renew_text_post(side, 0, 1, slot->sub_step + 21);
      }
      else if (net_is_on() != 0) {
        card_renew_text_post(side, 0, 1, slot->sub_step + 23);
      }
      else {
        card_renew_text_post(side, 0, 1, slot->sub_step + 25);
      }
    }
    break;
  case 3:
    slot->sub_step = 0;
    slot->step = 4;
    ((card_renew_times_rec *)card_renew_times)[side].old_access_key = card_words[1];
    *(card_record_rec *)card_renew_backup[side] = card_records[side];
    renew->old_play_id = body->play_id;
    body->play_id = renew->new_play_id = lcg_next15();
    card_haiki_set(side, 1);
  case 4:
    card_renew_text_post(side, 0, 1, 2);
    result = card_write_step(side, slot);
    if (result == 1) {
      slot->step = 5;
    }
    else if (result == 2) {
      slot->step = 9;
      card_renew_countdown_set(side, 0);
      card_renew_text_post(side, 60, 0, 3);
    }
    break;
  case 5:
    slot->sub_step = 0;
    slot->step = 6;
    card_renew_countdown_set(side, 0x4b0);
  case 6:
    card_renew_text_post(side, 0, 1, 4);
    result = card_insert_wait_step(side, slot, 1, 0, 0);
    if (result == 1) {
      slot->step = 7;
      ((card_renew_times_rec *)card_renew_times)[side].new_access_key = card_words[1];
    }
    else if (result == 2) {
      card_renew_text_post(side, 60, 0, 3);
    }
    else if (result == 3) {
      card_renew_text_post(side, 60, 0, 6);
    }
    else if (card_renew_countdown_done(side) != 0) {
      slot->step = 10;
      card_renew_countdown_set(side, 0);
      card_renew_text_post(side, 120, 0, 9);
    }
    break;
  case 7:
    card_renew_countdown_set(side, 0);
    slot->sub_step = 0;
    slot->step = 8;
    backup = card_renew_backup[side];
    memcpy(record, backup, 28);
    memcpy(header2, backup + 100, 28);
    *body = *(card_body_rec *)(backup + 28);
    record->header.use_count = 500;
    card_images[side].header.use_count = 500;
    card_renew_write_begun[side] = 0;
  case 8:
    if (((unit_step *)&card_steps)[side].request_index > 2) {
      card_renew_write_begun[side] = 1;
    }
    card_renew_text_post(side, 0, 1, 2);
    result = card_write_step(side, slot);
    if (result == 1) {
      slot->step = 15;
      card_renew_text_post(side, 60, 0, 10);
    }
    else if (result == 2) {
      card_unit_result(side);
      if (card_renew_write_begun[side] != 0) {
        slot->step = 17;
        card_renew_text_post(side, 60, 0, 3);
      }
      else {
        slot->step = 9;
        card_renew_text_post(side, 60, 0, 11);
      }
      renew->result_bits |= 1;
    }
    break;
  case 10:
    if (card_unit_result(side) == 1) {
      break;
    }
  case 9:
    if (card_renew_text_idle(side) != 0) {
      slot->step = 11;
    }
    break;
  case 11:
    if ((card_flags[side] & 1) != 0) {
      slot->sub_step = 1;
    }
    else {
      slot->sub_step = 0;
    }
    slot->step = 12;
    card_renew_countdown_set(side, 0x4b0);
  case 12:
    card_renew_text_post(side, 0, 1, 5);
    result = card_insert_wait_step(side, slot, 2, renew->new_play_id, renew->old_play_id);
    if (result == 3) {
      card_renew_text_post(side, 60, 0, 7);
    }
    else if (result == 4) {
      card_renew_text_post(side, 60, 0, 8);
    }
    else if (result == 5) {
      renew->result_bits |= 2;
      slot->step = 13;
    }
    else if (result == 6) {
      slot->step = 13;
    }
    else if (result == 2) {
      card_renew_text_post(side, 60, 0, 3);
    }
    else if (card_renew_countdown_done(side) != 0) {
      slot->step = 17;
      card_renew_countdown_set(side, 0);
    }
    break;
  case 13:
    card_renew_countdown_set(side, 0);
    slot->sub_step = 0;
    slot->step = 14;
    card_records[side] = *(card_record_rec *)card_renew_backup[side];
    card_renew_write_begun[side] = 0;
  case 14:
    if (((unit_step *)&card_steps)[side].request_index > 0) {
      card_renew_write_begun[side] = 1;
    }
    card_renew_text_post(side, 0, 1, 2);
    result = card_write_step(side, slot);
    if (result == 1) {
      slot->step = 16;
      card_renew_text_post(side, 60, 0, 12);
    }
    else if (result == 2) {
      card_unit_result(side);
      if (card_renew_write_begun[side] != 0) {
        renew->result_bits &= ~2;
      }
      card_renew_text_post(side, 60, 0, 3);
      slot->step = 17;
    }
    break;
  case 15:
    card_message_post((char)side, 0, -1, 0);
    if (card_renew_text_idle(side) == 0) {
      break;
    }
    card_flags[side] |= 0x90;
    card_renew_text_post(side, 0xb4, 0, 14);
    slot->step = 18;
    ((card_renew_times_rec *)card_renew_times)[side].outcome = 1;
    item_lottery_kind2_set(side);
    card_images[side].access_key = card_records[side].access_key;
    break;
  case 16:
    card_message_post((char)side, 0, -1, 0);
    if (card_renew_text_idle(side) == 0) {
      break;
    }
    {
      uint *flags = card_flags + side;
      *flags |= 0x90;
      if (card_plays_exhausted(side)) {
        *flags |= 0x20;
      }
    }
    if ((renew->result_bits & 1) != 0) {
      card_renew_text_post(side, 0xb4, 0, 15);
    }
    else {
      card_renew_text_post(side, 120, 0, 12);
    }
    slot->step = 18;
    break;
  case 17:
    if (card_renew_text_idle(side) == 0) {
      break;
    }
    card_flags[side] |= 0x100;
    {
      uint bits = renew->result_bits;
      if ((bits & 2) != 0) {
        ((card_renew_times_rec *)card_renew_times)[side].outcome = 3;
        if ((bits & 1) != 0) {
          card_renew_text_post(side, 300, 0, 19);
        }
        else {
          card_renew_text_post(side, 300, 0, 17);
        }
      }
      else {
        ((card_renew_times_rec *)card_renew_times)[side].outcome = 2;
        if ((renew->result_bits & 1) != 0) {
          card_renew_text_post(side, 0xb4, 0, 18);
        }
        else {
          card_renew_text_post(side, 0xb4, 0, 16);
        }
      }
    }
    slot->step = 18;
    break;
  case 18:
  default:
    if (card_renew_text_idle(side) != 0) {
      sel_arrow_hide(side);
      card_mode_slot_clear(side);
      renew->done = 1;
    }
    break;
  }
}

void card_mode_reinsert_verify(int side)
{
  card_mode_slot *slot = (card_mode_slot *)&card_mode_slots[0][0] + side;
  int result;
  
  if (card_records[side].in_use == 0) {
    slot->step = 0x14;
  }
  switch (slot->step) {
  case 0:
    card_flags[side] &= ~0x100;
    card_flags[side] &= ~0x80;
    card_flags[side] &= ~0x10;
    card_flags[side] &= ~0x20;
    card_flags[side] &= ~0x8;
    slot->sub_step = 0;
    slot->step = 1;
  case 1:
    result = card_read_step(side,slot);
    if (result == 1) {
      slot->step = 2;
    }
    else if (result == 2) {
      slot->step = 8;
    }
    break;
  case 2:
    if (card_header_check(side) == 0) {
      slot->step = 8;
      card_message_post((char)side,0x11,-1,0);
    }
    else {
      switch (card_format_check_upgrade(side)) {
      case 0:
      case 1:
        if (card_play_id_matches(side) == 0) {
          slot->step = 8;
          card_message_post((char)side,0x13,0x3c,0);
        }
        else {
          slot->step = 10;
        }
        break;
      case 3:
        slot->step = 8;
        card_message_post((char)side,0xb,0x3c,0);
        break;
      case 4:
        slot->step = 8;
        card_message_post((char)side,0xd,0x3c,0);
        break;
      default:
        slot->step = 8;
        card_message_post((char)side,0x11,-1,0);
        break;
      }
    }
    break;
  case 8:
    card_flags[side] |= 0x100;
    slot->step = 0x14;
    break;
  case 10:
    {
      uint *flags = card_flags + side;
      *flags |= 0x90;
      if (card_plays_exhausted(side)) {
        *flags |= 0x20;
      }
      card_message_post((char)side,0x19,0x3c,0);
      slot->step = 0x14;
    }
    break;
  case 0x14:
  default:
    card_mode_slot_clear(side);
    break;
  }
}

void card_mode_result_write(int side)
{
  card_mode_slot *slot;
  int result;
  uint *flags;

  slot = (card_mode_slot *)&card_mode_slots[0][0] + side;
  switch (slot->step) {
  case 0:
    card_message_post((char)side, 8, 0x3c, 0);
    slot->sub_step = 0;
    slot->step = 1;
  case 1:
    result = card_read_step(side, slot);
    if (result == 1) {
      slot->step = 2;
      return;
    }
    if (result == 2) {
      slot->step = 8;
      card_result_write_error = 1;
      return;
    }
    return;
  case 2:
    if (card_header_check(side) == 0) {
      slot->step = 8;
      card_result_write_error = 2;
      return;
    }
    if (card_play_id_matches(side) == 0) {
      slot->step = 8;
      card_result_write_error = 4;
      return;
    }
    card_image_to_record(side);
    slot->sub_step = 0;
    slot->step = 3;
    return;
  case 3:
    result = card_write_step(side, slot);
    if (result == 1) {
      slot->step = 10;
      return;
    }
    if (result == 2) {
      slot->step = 8;
      card_result_write_error = 5;
      return;
    }
    return;
  case 8:
    card_message_post((char)side, 5, 0x3c, 0);
    flags = card_flags + side;
    *flags = (*flags | 0x10000) & 0xffffff7f;
    card_records[side].in_use = 0;
    slot->step = 20;
    return;
  case 10:
    card_message_post((char)side, 3, 0x3c, 0);
    slot->step = 20;
    return;
  case 20:
    if (play_style != 2 || champion_side != side) {
      card_images[side].header.flags = card_images[side].header.flags & 0x7f;
      card_records[side].in_use = 0;
    }
    if (streak_limit_hit != 0) {
      card_images[side].header.flags = card_images[side].header.flags & 0x7f;
      card_records[side].in_use = 0;
      streak_end_flag_set(1);
    }
  default:
    card_mode_slot_clear(side);
    return;
  }
}

void card_mode_cmd_d9(int side)
{
  card_mode_slot *slot;

  slot = (card_mode_slot *)card_mode_slots[side];
  switch (slot->step) {
  case 0:
    slot->step = 1;
  case 1:
    if (card_lock_start(side) == 0) {
      slot->step = 2;
    }
    break;
  case 2:
    if (card_unit_result(side) != 1) {
      slot->step = 10;
    }
    break;
  case 10:
  default:
    card_mode_slot_clear(side);
    break;
  }
  return;
}

void card_mode_cmd_da(int side)
{
  card_mode_slot *slot;
  
  slot = (card_mode_slot *)card_mode_slots[side];
  switch (slot->step) {
  case 0:
    slot->step = 1;
  case 1:
    if (card_unlock_start(side) == 0) {
      slot->step = 2;
    }
    break;
  case 2:
    if (card_unit_result(side) != 1) {
      slot->step = 10;
    }
    break;
  case 10:
  default:
    card_mode_slot_clear(side);
    break;
  }
}

void card_mode_player_reset(char side)
{
  char *slots = &card_mode_slots[0][0];
  char *steps = &card_mode_slots[0][0] + 1;
  int i = side * 4;
  steps[i] = 0;
  (&card_mode_slots[0][0] + 2)[i] = 0;
  (&card_mode_slots[0][0])[i] = 0;
  (&card_mode_slots[0][0] + 3)[i] = 0;
  card_flags[side] = 0;
  card_message_close(side);
}

void card_mode_init(void)
{
  int i;
  for (i = 0; i < 2; i++)
  {
    if ((user_back_data.card_use[i] != 0) && card_unit_ok(i))
    {
      ((struct 
      {
        char c[1];
      } *) (card_unit_used))->c[i] = 1;
    }
    else
    {
      ((struct 
      {
        char c[1];
      } *) (card_unit_used))->c[i] = 0;
    }
    card_mode_player_reset((char) i);
    byte_view(&card_debug_requests[0][0])->c[i * 4] = 0;
    byte_view(&card_debug_requests[0][1])->c[i * 4] = 0;
  }

  card_user_count = 0;
  card_crc_tables_init();
}

void card_reinsert_mode_start(char side)
{
  e_mode_main main_mode;

  if (((card_flags[side] & 1) != 0) &&
     ((card_flags[side] & 0x40000) != 0)) {
    main_mode = mode_get_main();
    if (!(main_mode != MODE_MAIN_SELECTOR)) card_mode_start_entry(side); else card_mode_start_reinsert(side);
    card_flags[side] =
         card_flags[side] & ~0x40000;
  }
  return;
}

void card_presence_update(int side)
{
  typedef struct { unsigned int present : 1; unsigned int arrived : 1; unsigned int removed : 1; unsigned int flag_0x8 : 1; unsigned int flag_0x10 : 1; unsigned int used_up : 1; unsigned int flag_0x40 : 1; unsigned int accepted : 1; unsigned int failed : 1; unsigned int read_queued : 1; unsigned int : 3; unsigned int flag_0x2000 : 1; unsigned int : 4; unsigned int came_back : 1; unsigned int : 13; } card_flag_bits;
  typedef struct { unsigned char : 3; unsigned char b3 : 1; unsigned char : 4; } gb;
  
  if ((&card_status)[side * 8] != 0) {
    if (((card_flag_bits *)&card_flags)[side].present == 0) {
      ((card_flag_bits *)&card_flags)[side].present = 1;
      ((card_flag_bits *)&card_flags)[side].arrived = 1;
      ((card_flag_bits *)&card_flags)[side].removed = 0;
    }
    else {
      ((card_flag_bits *)&card_flags)[side].arrived = 0;
    }
    if (mode_get_main() != MODE_MAIN_SELECTOR) {
      ((card_flag_bits *)&card_flags)[side].flag_0x2000 = 0;
    }
    if (({ char *entry = (char *)side_entry_flags; (entry[side] >> 3) & 1; })) {
      ((card_flag_bits *)&card_flags)[side].read_queued = 0;
    }
  }
  else if (((card_flag_bits *)&card_flags)[side].present) {
    card_flags[side] = 4;
    if (card_records[side].in_use == 1) {
      card_flags[side] = 0x40004;
    }
  }
  else {
    ((card_flag_bits *)&card_flags)[side].removed = 0;
    ((card_flag_bits *)&card_flags)[side].flag_0x2000 = 0;
    ((card_flag_bits *)&card_flags)[side].arrived = 0;
    ((card_flag_bits *)&card_flags)[side].flag_0x8 = 0;
    ((card_flag_bits *)&card_flags)[side].flag_0x10 = 0;
    ((card_flag_bits *)&card_flags)[side].used_up = 0;
    ((card_flag_bits *)&card_flags)[side].accepted = 0;
    ((card_flag_bits *)&card_flags)[side].failed = 0;
  }
  if (card_records[side].in_use == 0) {
    ((card_flag_bits *)&card_flags)[side].came_back = 0;
  }
  else {
    card_reinsert_mode_start((char)side);
  }
}

void card_unused_unit_cmd_da(int side)
{
  if ((&card_status)[side * 8 + 1] == '\x01') {
    card_unlock_start(side);
  }
  return;
}

void card_unit_cmd_pick(int side)
{
  int busy;
  int flag;
  
  flag = 0;
  busy = 1;
  if (0 < card_unit_cmd_cooldown[side]) {
    card_unit_cmd_cooldown[side] = card_unit_cmd_cooldown[side] + -1;
    return;
  }
  if ((card_records[side].in_use == 1) && ((card_flags[side] & 1) != 0)
      && ((card_flags[side] & 0x80) != 0)) {
    flag = 1;
  }
  if (flag == 1) {
    if ((&card_status)[side * 8 + 1] == '\0') busy = card_mode_start_d9(side);
  }
  else {
    if ((&card_status)[side * 8 + 1] == '\x01') busy = card_mode_start_da(side);
  }
  if (busy == 0) {
    card_unit_cmd_cooldown[side] = 0x3c;
  }
  return;
}

void card_drop_on_state(int side)
{
  char *state;
  state = card_service_state_get();
  if ((((*state == '\0') && (play_style == '\x01')) || (*(int *)(state + 4) == -1)) &&
     (*(int *)(state + 4) != 3)) {
    card_records[side].in_use = 0;
    card_flags[side] = card_flags[side] & 0xffffff7f;
  }
  return;
}

void card_mode_step(char side)
{
  if (mode_get_main() == MODE_MAIN_SELECTOR || mode_get_main() == MODE_MAIN_GAME ||
      mode_get_main() == MODE_MAIN_ENDING || mode_get_main() == MODE_MAIN_OVER ||
      mode_get_main() == MODE_MAIN_INTRUDE) {
    switch (card_mode_slots[side][0]) {
    case 1: card_mode_entry_read(side); break;
    case 2: card_mode_select_write(side); break;
    case 3: card_mode_renew(side); break;
    case 4: card_mode_reinsert_verify(side); break;
    case 5: card_mode_result_write(side); break;
    case 6: card_mode_cmd_d9(side); break;
    case 7: card_mode_cmd_da(side); break;
    }
  }
}

void card_modes_frame(void)
{
  char i;
  
  for (i = 0; i < 2; i++) {
    char *flags = card_unit_used;
    if (flags[i] == '\0') {
      card_unused_unit_cmd_da(i);
    }
    else if (card_unit_ok(i) == 0) {
      card_mode_player_reset(i);
      { char *used = card_unit_used; used[i] = 0; }
      card_records[i].in_use = 0;
    }
    else {
      card_presence_update(i);
      card_mode_step(i);
      card_message_update(i);
      card_message_draw(i);
      card_renew_text_draw(i);
      card_drop_on_state(i);
      card_unit_cmd_pick(i);
    }
  }
  return;
}

void card_mode_slot_clear(int side)
{
  char *slots = &card_mode_slots[0][0];
  slots[side * 4] = 0;
  (&card_mode_slots[0][0] + 1)[side * 4] = 0;
  (&card_mode_slots[0][0] + 2)[side * 4] = 0;
  (&card_mode_slots[0][0] + 3)[side * 4] = 0;
}

bool card_mode_start_entry(int side)
{
  if (card_mode_slots[side][0] != '\0') return 1;
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 1;
  return 0;
}

bool card_mode_start_select_write(int side)
{
  if (card_mode_slots[side][0] != '\0') return 1;
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 2;
  return 0;
}

bool card_mode_start_renew(int side)
{
  if (card_mode_slots[side][0] != '\0') {
    return 1;
  }
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 3;
  return 0;
}

bool card_mode_start_reinsert(int side)
{
  if (card_mode_slots[side][0] != '\0') return 1;
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 4;
  return 0;
}

bool card_mode_start_result(int side)
{
  if (card_mode_slots[side][0] != '\0') return 1;
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 5;
  return 0;
}

bool card_mode_start_d9(int side)
{
  if (card_mode_slots[side][0] != '\0') return 1;
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 6;
  return 0;
}

bool card_mode_start_da(int side)
{
  if (card_mode_slots[side][0] != '\0') {
    return 1;
  }
  card_mode_slot_clear(side);
  card_mode_slots[side][0] = 7;
  return 0;
}

int card_write_mode_busy(void)
{
  int i;
  
  i = 0;
  while (i < 2) {
    if (card_mode_slots[i][0] == '\x02' || card_mode_slots[i][0] == '\x05') {
      return 1;
    }
    i = (int)(char)((char)i + '\x01');
  
  }
  return 0;
}

void card_renew_state_clear(int side)
{
  card_renew_state_rec *renew;
  
  renew = (card_renew_state_rec *)card_renew_state[side];
  renew->result_bits = 0;
  renew->cancel = 0;
  renew->done = 0;
  renew->new_play_id = 0;
  renew->old_play_id = 0;
}

int card_renew_done_get(int side)
{
  return ((card_renew_state_rec *)card_renew_state)[side].done;
}

void card_renew_cancel_request(int side)
{
  ((card_renew_state_rec *)card_renew_state)[side].cancel = 1;
}

int card_raw_is_blank(int side)
{
  int i;
  int r;
  uchar *raw;

  raw = card_records[side].raw;
  if (raw[0x6b] != 0xff) {
    r = 0;
  }
  else {
    for (i = 0; i < 0x6b; i++) {
      if (raw[i] != 0) {
        r = 0;
        goto out;
      }
    }
    r = 1;
  }
out:
  return r;
}

bool card_plays_exhausted(int side)
{
  int t = card_records[side].header.use_count > 0;
  return t == 0;
}

int card_format0_check(int side)
{
  uchar *raw;
  ushort sums[4];
  uint copy_crc;
  int r;
  short copy_stored;

  raw = card_records[side].raw;
  sums[0] = card_crc16(0,0x19,(char *)raw + 2);
  sums[1] = copy_crc = card_crc16(0,0x19,(char *)raw + 0x53);
  sums[2] = raw[1] << 8 | raw[0];
  copy_stored = sums[3] = raw[0x52] << 8 | raw[0x51];
  if (sums[0] != sums[2] && copy_crc != sums[3]) r = 3;
  else {
    if (card_format_get(&card_records[side].header) != 0) r = 3;
    else r = 0;
  }
  return r;
}

int card_format1_check(int side)
{
  ushort calc[3];
  ushort stored[3];
  int bad[3];
  int key;
  card_record_rec *record;
  card_header_rec *header;
  card_body_rec *body;
  card_header_rec *header2;
  int i;

  bad[0] = 0;
  bad[1] = 0;
  bad[2] = 0;
  key = card_records[side].access_key;
  header = &card_records[side].header;
  body = &card_records[side].body;
  header2 = &card_records[side].header2;
  calc[0] = card_crc16(0,0x18,&card_records[side].raw[2]);
  calc[1] = card_crc16(0,0x36,&card_records[side].raw[0x1c]);
  calc[2] = card_crc16(0,0x18,&card_records[side].raw[0x54]);
  stored[0] = header->crc;
  stored[1] = body->crc;
  stored[2] = header2->crc;
  if (calc[0] != stored[0]) bad[0] = 1;
  if (card_hdr_char_num_get(header) == 0xd) bad[0] = 1;
  if (header->level >= 0xee) bad[0] = 1;
  if (header->use_count < 0) bad[0] = 1;
  if (calc[1] != stored[1]) bad[1] = 1;
  if (calc[2] != stored[2]) bad[2] = 1;
  if (card_hdr_char_num_get(header2) == 0xd) bad[2] = 1;
  if (header2->level >= 0xee) bad[2] = 1;
  if (header2->use_count < 0) bad[2] = 1;
  if (bad[0] != 0) {
    if (bad[2] != 0) return 3;
    memcpy(header,header2,0x1c);
  }
  else memcpy(header2,header,0x1c);
  if (card_format_get(header) != 1) return 3;
  if (card_haiki_get(side) != 0) {
    if (discarded_code_take(key) != 0) card_haiki_set(side,0);
    else return 2;
  }
  if (bad[1] != 0) {
    memset(body,0,0x48);
    for (i = 0x1a; i < 0x52; i++) { card_records[side].raw[i] = 0; }
    card_level_colors_apply((int)(short)side,0);
    record = &card_records[side];
    record->raw[0x20] = body->color_1p.b[0];
    record->raw[0x21] = body->color_1p.b[1];
    record->raw[0x22] = body->color_2p.b[0];
    record->raw[0x23] = body->color_2p.b[1];
    body->crc = card_crc16(0,0x36,&card_records[side].raw[0x1c]);
    return 1;
  }
  return 0;
}

int card_format2_check(int side)
{
  ushort crc[3];
  ushort sum[3];
  int bad[3];
  int key;
  card_fmt2_header_rec *header;
  card_fmt2_body_rec *body;
  card_fmt2_header_rec *header2;
  int i;

  bad[0] = 0;
  bad[1] = 0;
  bad[2] = 0;
  key = card_records[side].access_key;
  header = &card_records[side].fmt2_header;
  body = &card_records[side].fmt2_body;
  header2 = &card_records[side].fmt2_header2;
  crc[0] = card_crc16(0,0x18,&card_records[side].raw[2]);
  crc[1] = card_crc16(0,0x36,&card_records[side].raw[0x1c]);
  crc[2] = card_crc16(0,0x18,&card_records[side].raw[0x54]);
  sum[0] = header->crc;
  sum[1] = body->crc;
  sum[2] = header2->crc;
  if (crc[0] != sum[0]) {
    bad[0] = 1;
  }
  if (card_format2_hdr_char_num_get(header) == 0xd) {
    bad[0] = 1;
  }
  if (header->level >= 0xee) {
    bad[0] = 1;
  }
  if (header->use_count < 0) {
    bad[0] = 1;
  }
  if (crc[1] != sum[1]) {
    bad[1] = 1;
  }
  if (crc[2] != sum[2]) {
    bad[2] = 1;
  }
  if (card_format2_hdr_char_num_get(header2) == 0xd) {
    bad[2] = 1;
  }
  if (header2->level >= 0xee) {
    bad[2] = 1;
  }
  if (header2->use_count < 0) {
    bad[2] = 1;
  }
  if (bad[0] != 0) {
    if (bad[2] != 0) return 3;
    memcpy(header,header2,0x1c);
  }
  else {
    memcpy(header2,header,0x1c);
  }
  if (card_format_field_get((card_header_rec *)header) != 2) {
    return 3;
  }
  if (card_haiki_get(side)) {
    if (discarded_code_take(key) != 0) card_haiki_set(side,0);
    else return 2;
  }
  if (bad[1] != 0) {
  card_record_rec *record;
  memset(body,0,0x3a);
  for (i = 0x1a; i < 0x52; i++) {
    record = &card_records[side];
    *(record->raw + i) = 0;
  }
  card_level_colors_apply((short)side,0);
  record = &card_records[side];
  record->raw[0x20] = body->color_1p_packed.b[0];
  record->raw[0x21] = body->color_1p_packed.b[1];
  record->raw[0x22] = body->color_2p_packed.b[0];
  record->raw[0x23] = body->color_2p_packed.b[1];
  body->crc = card_crc16(0,0x36,&card_records[side].raw[0x1c]);
  return 1;
  }
  return 0;
}

int card_format34_check(int side, int variant)
{
  int bad_header;
  int bad_body;
  int bad_header2;
  uint crc_mode;
  int key;
  card_body_rec *body;
  card_record_rec *record;
  card_header_rec *header2;
  int i;
  card_record_rec *rec;
  ushort calc[3];
  ushort stored[3];

  bad_header = 0;
  bad_body = 0;
  bad_header2 = 0;
  key = card_records[side].access_key;
  record = &card_records[side];
  body = &card_records[side].body;
  header2 = &card_records[side].header2;
  switch (variant) {
  case 0:
    crc_mode = 0;
    break;
  case 1:
    crc_mode = 1;
    break;
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card_mode.c",0x87e,"assertion `%s\' failed.\n","0");
    crc_mode = 1;
  }
  calc[0] = card_crc16(crc_mode, 0x16, &card_records[side].raw[2]);
  calc[1] = card_crc16(crc_mode, 0x3a, &card_records[side].raw[0x1a]);
  calc[2] = card_crc16(crc_mode, 0x16, &card_records[side].raw[0x56]);
  stored[0] = record->header.crc;
  stored[1] = body->crc;
  stored[2] = header2->crc;
  if (calc[0] != stored[0]) bad_header = 1;
  if (card_hdr_char_num_get(&record->header) == 0xd) bad_header = 1;
  if (record->header.level >= 0xee) bad_header = 1;
  if (record->header.use_count < 0) bad_header = 1;
  if (calc[1] != stored[1]) bad_body = 1;
  if (calc[2] != stored[2]) bad_header2 = 1;
  if (card_hdr_char_num_get(header2) == 0xd) bad_header2 = 1;
  if (header2->level >= 0xee) bad_header2 = 1;
  if (header2->use_count < 0) bad_header2 = 1;
  if (bad_header != 0) {
    if (bad_header2 != 0) return 3;
    memcpy(&record->header, header2, 0x1c);
  }
  else {
    memcpy(header2, &record->header, 0x1c);
  }
  switch (variant) {
  case 0:
    if (card_format_get(&record->header) != 3) return 3;
    break;
  case 1:
    if (card_format_get(&record->header) != 4) return 3;
    break;
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card_mode.c",0x8c2,"assertion `%s\' failed.\n","0");
    return 3;
  }
  if (card_haiki_get(side)) {
    if (discarded_code_take(key) != 0) card_haiki_set(side, 0);
    else return 2;
  }
  if (bad_body != 0) {
  memset(body, 0, 0x48);
  for (i = 0x18; i < 0x55; i++) {
    rec = &card_records[side];
    *(rec->raw + i) = 0;
  }

  card_level_colors_apply((short)side, 0);
  card_colors_to_packed(body);
  rec = &card_records[side];
  rec->raw[0x1e] = body->color_1p_packed.b[0];
  rec->raw[0x1f] = body->color_1p_packed.b[1];
  rec->raw[0x20] = body->color_2p_packed.b[0];
  rec->raw[0x21] = body->color_2p_packed.b[1];
  rec->raw[0x2a] = body->curse.b.curse;
  rec->raw[0x4d] = body->misc.b[0];
  rec->raw[0x4e] = body->misc.b[1];
  body->crc = card_crc16(crc_mode, 0x3a, &card_records[side].raw[0x1a]);
  card_hdr_recover_set(&record->header);
  return 1;
  }
  return 0;
}

int card_format_check(int side, uint format)
{
  int r;
  switch(format) {
  case 0:
    r = card_format0_check(side);
    break;
  case 1:
    r = card_format1_check(side);
    break;
  case 2:
    r = card_format2_check(side);
    break;
  case 3:
    r = card_format34_check(side,0);
    break;
  case 4:
    r = card_format34_check(side,1);
    break;
  default:
    r = 4;
    break;
  }
  return r;
}

int card_format_check_upgrade(int side)
{
  int format;
  int result;
  int (*check)(int, uint);
  format = card_format_get(&card_records[side].header);
  if (card_format_check(side,2) <= 1) {
    card_format2_upgrade(side);
    return 0;
  }
  if (card_format_check(side,3) <= 1) {
    card_format3_upgrade(side);
    return 0;
  }
  check = card_format_check;
  result = check(side,4);
  if (result < 0) result = 3;
  else if (result > 2) {
    if (result == 3) {
      if (format != 4 && check(side,format) == 0) result = 4;
    }
    else result = 3;
  }
  return result;
}

bool card_play_id_matches(int side)
{
  return card_records[side].body.play_id == card_images[side].body.play_id;
}

bool card_haiki_get(int side)
{
  int bit = card_records[side].header.flags & 0x40;
  int t = bit == 0;
  return t == 0;
}

void card_haiki_set(int side, int on)
{
  if (on != 0) {
    card_records[side].header.flags |= 0x40;
  }
  else {
    card_records[side].header.flags &= 0xbf;
  }
}

void card_renew_times_clear(int side)
{
  card_renew_times_rec *times = (card_renew_times_rec *)card_renew_times;
  times[side].outcome = 0;
  times[side].old_access_key = 0;
  times[side].new_access_key = 0;
}

void card_record_to_image(int side)
{
  card_image_rec *image;
  card_record_rec *record;
  image = &card_images[side];
  record = &card_records[side];
  image->header = record->header;
  image->body = record->body;
  card_images[side].access_key = card_records[side].access_key;
  return;
}

void card_image_to_record(int side)
{
  card_record_rec *record;
  card_image_rec *image;

  record = &card_records[side];
  image = &card_images[side];
  record->header = image->header;
  record->header2 = image->header;
  record->body = image->body;
}

void card_message_post(int side, int id, int frames, int word)
{
  card_message_rec *msg;
  
  msg = (card_message_rec *)&card_messages[0][0] + (char)side;
  msg->id = (char)id;
  msg->frames = (short)frames;
  msg->word = word;
}

void card_message_close(char side)
{
  card_message_rec *msg;
  msg = &((card_message_rec *)&card_messages[0][0])[side];
  msg->id = 0xff;
  msg->frames = 0;
  msg->word = 0;
}

void card_message_update(int side)
{
  card_message_rec *msgs;

  msgs = (card_message_rec *)&card_messages[0][0];
  if (msgs[side].frames > 0) {
    msgs[side].frames--;
  }
  switch (((card_message_rec *)&card_messages[0][0])[side].id) {
  case 1:
    if (((card_message_rec *)&card_messages[0][0])[side].frames == 0) {
      if ((card_flags[side] & 1) == 0) goto call60;
      ((card_message_rec *)&card_messages[0][0])[side].frames = -1;
      break;
    }
    if (((card_message_rec *)&card_messages[0][0])[side].frames != -1) break;
  default:
    if ((card_flags[side] & 4) != 0) goto call60;
    break;
  case 7:
    if (((card_message_rec *)&card_messages[0][0])[side].frames != 0) break;
    goto c17;
  case 3:
    if (((card_message_rec *)&card_messages[0][0])[side].frames == 0) {
      card_message_post((char)side, 4, 60, 0);
    }
    goto c25;
  case 5:
    if (((card_message_rec *)&card_messages[0][0])[side].frames == 0) {
      card_message_post((char)side, 6, 60, 0);
    }
    goto c25;
  case 4:
  case 6:
    if (((card_message_rec *)&card_messages[0][0])[side].frames == 0
        && (card_records[side].header.flags & 0x80) == 0) {
      card_message_post((char)side, 24, -1, 0);
    }
    goto c25;
  case 24:
    if (mode_get_main() == 2) goto call60;
    goto c24;
  case 11:
  case 13:
  case 19:
    if (((card_message_rec *)&card_messages[0][0])[side].frames != 0) break;
    card_message_post((char)side, 17, 60, 0);
    break;
  case 17:
    if (((card_message_rec *)&card_messages[0][0])[side].frames != 0) goto c24;
  c17:
    card_message_post((char)side, 22, -1, 0);
    break;
  case 22:
    if ((card_flags[side] & 2) != 0) goto call60;
    {
      char *entry = (char *)side_entry_flags;
      if ((entry[side] >> 2 & 1) != 0) break;
    }
    goto call60;
  case 23:
    if (((card_message_rec *)&card_messages[0][0])[side].frames == 0) {
      card_message_post((char)side, 22, -1, 0);
    }
  case 20:
    if ((card_flags[side] & 2) != 0) goto call60;
    break;
  case 25:
  c25:
    if (mode_get_main() == 2) goto call60;
    break;
  case 0:
    if (card_upgraded[side] != 0) {
      card_message_post((char)side, 14, -1, 0);
      break;
    }
  case 10:
  case 12:
  case 18:
    if ((card_flags[side] & 1) != 0) goto c2;
    goto call60;
  case 14:
    if ((card_flags[side] & 1) == 0) {
      card_message_close((char)side);
    } else if (mode_get_main() != 2) {
      card_message_close((char)side);
      card_upgraded[side] = 0;
    }
    break;
  case 16:
    if ((card_flags[side] & 1) == 0) goto call60;
    if (mode_get_sub() > 4) goto call60;
    break;
  case 2:
  c2:
    if (mode_get_main() != 2) goto call60;
    break;
  case 26:
  case 27:
  case 28:
  case 30:
    if (mode_get_main() != 3) goto call60;
    if ((game_flags & 0x400) != 0) goto call60;
  c24:
    if ((card_flags[side] & 1) != 0) break;
    goto call60;
  case 8:
  case 9:
    {
      card_message_rec *msg = &((card_message_rec *)&card_messages[0][0])[side];
      if (msg->frames != 0) break;
      if (msg->id == 8) card_message_post((char)side, 9, 60, 0);
      else card_message_post((char)side, 8, 60, 0);
    }
    break;
  case 15:
  case 21:
  case 29:
    if ((card_flags[side] & 4) == 0) break;
  call60:
    card_message_close((char)side);
  }
  if (player_word104(side) == 1) {
    if (mode_get_main() == 3 && mode_get_sub() == 4
        && (play_style == 2 || rank_round == 0) && level_change_flags_compute() == 0) {
      card_message_post((char)side, 25, 60, 0);
    }
    if ((card_flags[side] & 4) != 0
        && player_word104(side) == 1) {
      card_message_post((char)side, 23, 60, 0);
    }
  }
  if (((card_message_rec *)&card_messages[0][0])[side].frames == 0) {
    card_message_close((char)side);
  }
}

void card_message_draw(int side)
{
  typedef struct { int field_0x0; char red; char field_0x5[3]; char *text; int objects[2]; } message_def;
  card_message_rec *msg;
  int id;
  int layer_idle;
  int service_mode;
  float chars;
  char *offset;
  char *entry = (char *)side_entry_flags;
  float x;
  float y;
  float xr;
  char buf[100];
  int width;
  int lines;
  
  msg = &((card_message_rec *)&card_messages[0][0])[side];
  id = msg->id;
  layer_idle = 0;
  service_mode = *(int *)(card_service_state_get() + 4);
  if (service_mode == -1 && mode_get_main() == 2 &&
      ((uint)(int)entry[side] >> 2 & 1) != 0 &&
      card_records[side].in_use == 0 &&
      (((int *)&card_flags)[side] & 1) != 0) {
    id = 0x20;
    msg->frames = service_mode;
  }
  if (sel_card_layer_idle((char)side,0)) {
    layer_idle = 1;
  }
  if (id != -1 && ((((card_message_rec *)&card_messages[0][0])[side].frames != 0) & (layer_idle != 0))) {
  if (mode_get_main() == 2) {
    if (pause_frame_held() == 0) {
      card_message_fade[side] = card_message_fade[side] + 0.05;
    }
    if (card_message_fade[side] > 1.0) {
      card_message_fade[side] = 1.0;
    }
    if (id == 0) {
      card_panel_draw((char)side,card_message_fade[side]);
    }
    else if (mode_get_sub() > 1 && ((message_def *)&card_message_table[0][0])[id].text != 0) {
      snprintf(buf,100,"%s",((message_def *)&card_message_table[0][0])[id].text);
      y = 13.0;
      if (side == 0) {
        if (card_message_fade[0] < 1.0) goto tail;
        x = 28.0;
      }
      else {
        if (card_message_fade[1] < 1.0) goto tail;
        text_lines_measure(buf,&width,&lines);
        x = 386.0;
      }
      sprite_text_put(&euc_text_font,x,y,0.008,0,(byte *)buf);
    }
    else {
      card_message_fade[side] = 0.0;
    }
  }
  else if (((message_def *)&card_message_table[0][0])[id].text != 0) {
    snprintf(buf,100,"%s",((message_def *)&card_message_table[0][0])[id].text);
    chars = (float)(strlen(buf) + 2);
    if (side == 0) {
      offset = (char *)(id * 20);
      if (offset[(int)&card_message_table[0][1]] != 0) {
        sprite_colors_set(0xff4040,0);
      }
      sprite_text_put(&euc_text_font,72.0,448.0,0.008,0,(byte *)buf);
      sprite_colors_reset();
      sprite_put_xyz_scaled(0x12,64.0,445.0,0.009,chars,1.0);
      sprite_colors_set(0xff0000,0);
      sprite_put_xyz(0x13,chars * 8.0 + 64.0,445.0,0.009);
      sprite_colors_reset();
    }
    else {
      offset = (char *)(id * 20);
      if (offset[(int)&card_message_table[0][1]] != 0) {
        sprite_colors_set(0xff4040,0);
      }
      sprite_text_put(&euc_text_font,602.0 - (chars - 1.0) * 8.0,448.0,0.008,0,(byte *)buf);
      sprite_colors_reset();
      xr = 602.0 - chars * 8.0;
      sprite_put_xyz_scaled(0x12,xr,445.0,0.009,chars,1.0);
      sprite_colors_set(0xff,0);
      sprite_put_flags_xyz(0x13,xr,445.0,0.009,0x17);
      sprite_colors_reset();
    }
  }
tail:
  if (((message_def *)(card_message_table[id] + side))->objects[0] != 0) {
    sprite_list_put((int *)((message_def *)(card_message_table[id] + side))->objects[0],0.008);
  }
  }
  else {
    card_message_fade[side] = 0.0;
  }
}

int card_renew_text_idle(int side)
{
  int t;
  
  t = ((card_renew_text_rec *)card_renew_text_recs)[side].frames > 1;
  return t == 0;
}

void card_renew_text_post(int side, int frames, int flag, int text_no)
{
  card_renew_text_rec *text =
    (card_renew_text_rec *)(card_renew_text_recs[side]);
  if (text->frames < 2) {
    text->frames = frames;
    text->flag = flag;
    text->text_no = text_no;
  }
}

void card_renew_countdown_set(int side, int frames)
{
  ((card_renew_text_rec *)card_renew_text_recs)[side].countdown = frames;
}

int card_renew_countdown_done(int side)
{
  int t;
  
  t = ((card_renew_text_rec *)card_renew_text_recs)[side].countdown > 0;
  return t == 0;
}

void card_renew_text_draw(int side)
{
  card_renew_text_rec *text;
  int i;
  int *words;
  float x;
  float y;
  float dy;
  float chars;
  char buf[256];

  text = (card_renew_text_rec *)card_renew_text_recs + side;
  x = 15.0;
  y = 107.0;
  dy = 17.0;
  if (side == 1) x += 373.0;
  if (text->countdown > 0) {
    snprintf(buf,0x100,"%d",text->countdown / 60);
    sprite_text_put(&euc_text_font,x + 240.0 - 10.0,82.0,0.006,0x82,(byte *)buf);
    text->countdown--;
  }
  if (text->text_no < 0) return;
  if (text->text_no == 0) {
    snprintf(buf,0x100,"%s",card_renew_text_table[text->text_no].lines[0]);
    chars = strlen(buf) + 1;
    if (side == 0) {
      sprite_text_put(&euc_text_font,68.0,448.0,0.008,0,(byte *)buf);
      sprite_put_xyz_scaled(0x12,64.0,445.0,0.009,chars,1.0);
      sprite_colors_set(0xff0000,0);
      sprite_put_xyz(0x13,chars * 8.0 + 64.0,445.0,0.009);
    }
    else {
      float t = 602.0 - chars * 8.0;
      sprite_text_put(&euc_text_font,t,448.0,0.008,0,(byte *)buf);
      sprite_put_xyz_scaled(0x12,t,445.0,0.009,chars,1.0);
      sprite_colors_set(0xff,0);
      sprite_put_flags_xyz(0x13,t,445.0,0.009,0x17);
    }
  }
  else {
    if ((uint)(text->text_no - 21) <= 5) {
      sprite_colors_set(0xff0000,0);
      sprite_put_xyz(0xc8b,x,176.0,0.006);
      sprite_colors_set(0xd00000,0);
      sprite_text_put(&euc_text_font,x + 96.0,y + dy,0.00599,0,card_haiki_text);
      sprite_colors_reset();
      if ((game_frame_count & 8) != 0) {
        if (text->text_no == 21) {
          sel_arrow_show(side,x + 96.0 + 8.0,y + dy * 3.0,0.006,1);
        }
        else if (text->text_no == 22) {
          sel_arrow_show(side,x + 32.0 - 8.0,y + dy * 3.0,0.006,0);
        }
        if ((text->text_no == 23) | (text->text_no == 25)) {
          sel_arrow_show(side,x + 96.0 + 8.0,y + dy * 5.0,0.006,1);
        }
        else if ((text->text_no == 24) | (text->text_no == 26)) {
          sel_arrow_show(side,x + 32.0 - 8.0,y + dy * 5.0,0.006,0);
        }
      }
    }
    else if ((text->text_no == 1) | (text->text_no == 20) || text->text_no == 2) {
      sprite_colors_set(0xf00000,0);
      sprite_put_xyz(0xc8b,x,176.0,0.006);
      sprite_colors_reset();
      y += dy;
    }
    else if (text->text_no == 5) {
      sprite_colors_set(0xd00000,0);
      sprite_text_put(&euc_text_font,x + 48.0,y + dy * 0.0,0.00599,0,card_haiki_text);
      sprite_colors_set(0xf00000,0);
      sprite_put_xyz(0xc8b,x,176.0 + dy * 4.0,0.006);
      sprite_colors_reset();
    }
    sprite_colors_set(0xe0e000,0);
    i = 0;
    words = (int *)card_renew_text_table; /* ORIGINAL SHAPE (assumed; the 2026-11-10 audit replaced part: the cell view is gone, this pointer stays): the lines read through an int pointer as words 10 * text_no + 2 + i; spelled card_renew_text_table[text_no].lines[i] the routine does not match */
    for (; i < 8; i++, y += dy) {
      int line = words[text->text_no * 10 + i + 2];
      if (line == 0) break;
      snprintf(buf,0x100,"%s",line);
      sprite_text_put(&euc_text_font,x,y,0.006,0,(byte *)buf);
    }
  }
  sprite_colors_reset();
  if (--text->frames < 0) text->text_no = -1;
}

void card_record_set_blank(int side)
{
  uchar raw[108];

  memset(raw,0,0x6c);
  raw[107] = 0xff;
  card_unpack(side,raw);
}

void card_blank_write_step(int side)
{
  int result;
  card_mode_slot *slot;

  slot = (card_mode_slot *)card_debug_requests[side];
  switch (slot->step) {
  case 0:
    slot->sub_step = 0;
    slot->step = 1;
  case 1:
    result = card_read_step(side,slot);
    if (result == 1) {
      slot->step = 2;
    }
    else if (result == 2) {
      slot->step = 8;
    }
    break;
  case 2:
    if (card_header_check(side) != 0) {
      card_record_set_blank(side);
      slot->sub_step = 0;
      slot->step = 3;
    }
    else {
      slot->step = 20;
    }
    break;
  case 3:
    if (card_write_step(side,slot) != 0) {
      slot->step = 20;
    }
    break;
  case 20:
  default:
    slot->step = 0;
    slot->mode = 0;
    break;
  }
}

void card_blank_write_request_1p(void)
{
  card_debug_requests[0][1] = 0;
  card_debug_requests[0][0] = 8;
  return;
}

void card_blank_write_request_2p(void)
{
  card_debug_requests[1][1] = 0;
  card_debug_requests[1][0] = 8;
  return;
}

void card_menu_update(void)
{
  if (card_debug_requests[0][0] == '\b') {
    card_blank_write_step(0);
  }
  if (card_debug_requests[1][0] == '\b') {
    card_blank_write_step(1);
  }
  return;
}

void card_blank_on_insert_step(int side, card_mode_slot *slot)
{
  int result;

  if ((card_flags[side] & 2) != 0) {
    slot->step = 1;
  }
  switch(slot->step) {
  case 1:
    slot->sub_step = 0;
    slot->step = 2;
  case 2:
    result = card_read_step(side,slot);
    if (result == 1) {
      slot->step = 3;
      slot->sub_step = 0;
      card_record_set_blank(side);
    }
    else if (result == 2) {
      slot->step = 0x14;
    }
    break;
  case 3:
    if (card_write_step(side,slot) != 0) {
      slot->step = 0x14;
    }
    break;
  case 0x14:
  default:
    slot->step = 0;
    slot->mode = 0;
    break;
  }
}

void sample_card_write_step(uint side, card_mode_slot *slot)
{
  int result;
  int chara;
  int level;
  int *entry;
  
  if ((card_flags[side] & 2) != 0) {
    slot->step = 1;
  }
  if (sample_card_index < 0) {
    return;
  }
  switch(slot->step) {
  case 1:
    slot->sub_step = 0;
    slot->step = 2;
  case 2:
    chara = *sample_cards[sample_card_index];
    if (chara < 0) {
      slot->step = 0x14;
      sample_card_index = -1;
      return;
    }
    slot->step = 3;
    return;
  case 3:
    result = card_read_step(side,slot);
    if (result == 1) {
      card_record_set_blank(side);
      entry = sample_cards[sample_card_index];
      chara = entry[0];
      level = entry[1];
      sample_card_index = sample_card_index + 1;
      card_record_char_set(side & 0xff,chara & 0xff);
      card_records[side].header.level = level;
      slot->step = 4;
      slot->sub_step = 0;
      return;
    }
    if (result == 2) {
      slot->step = 0x14;
    }
    return;
  case 4:
    result = card_write_step(side,slot);
    if (result != 0) {
      slot->step = 0x14;
    }
    return;
  case 0x14:
  default:
    slot->step = 0;
    slot->mode = 0;
    return;
  }
}

bool card_message_active(int side)
{
  int active = 0;
  if (card_messages[side][0] != -1) active = 1;
  return active;
}

uint card_flags_get(int side)
{
  return card_flags[side];
}

int * card_renew_times_get(int side)
{
  return card_renew_times[side];
}

int card_unit_used_get(int side)
{
  return (int)(char)card_unit_used[side];
}

char * card_mode_slot_get(int side)
{
  return card_mode_slots[side];
}

char * card_message_get(int side)
{
  return card_messages[side];
}

void streak_end_flag_set(int flag)
{
  streak_end_flag = flag;
  return;
}

int streak_end_flag_get(void)
{
  return streak_end_flag;
}

void card_format2_upgrade(int side)
{
  card_header_rec *header;
  card_header_rec *header2;
  card_body_rec *body;
  card_fmt2_header_rec *old_header;
  card_fmt2_body_rec *old_body;
  card_fmt2_header_rec *old_header2;
  int i;
  uint color;

  header = &card_records[side].header;
  header2 = &card_records[side].header2;
  body = &card_records[side].body;
  old_header = &card_records[side].fmt2_header;
  old_body = &card_records[side].fmt2_body;
  old_header2 = &card_records[side].fmt2_header2;
  card_upgraded[side] = 1;
  header->crc = 0;
  header->flags = old_header->flags | 0x30;
  header->char_num = old_header->char_num;
  header->level = old_header->level;
  header->all_win = old_header->all_win;
  header->all_lose = old_header->all_lose;
  for (i = 0; i < 8; i++)
    header->item_have[i] = old_header->item_have[i];
  header->use_count = old_header->use_count & 0x1ff;
  header->ftf.b[0] = 0;
  header->ftf.f.ex_ver = 1;
  for (i = 0; i < 3; i++)
    header->card_id[i] = old_header->card_id[i];
  header->nickname = 0;
  body->crc = 0;
  body->play_id = old_body->play_id;
  body->exp = old_body->exp;
  body->point = old_body->point;
  body->color_1p_packed.h = old_body->color_1p_packed.h;
  body->color_2p_packed.h = old_body->color_2p_packed.h;
  color = 0;
  for (i = 0; i < 4; i++)
    color |= ((body->color_1p_packed.h >> (i * 4)) & 15) << (i * 5);
  body->color_1p.w = color;
  color = 0;
  for (i = 0; i < 4; i++)
    color |= ((body->color_2p_packed.h >> (i * 4)) & 15) << (i * 5);
  body->color_2p.w = color;
  for (i = 0; i < 4; i++) {
    body->equip_1p[i] = old_body->equip_1p[i];
    body->equip_2p[i] = old_body->equip_2p[i];
  }
  body->curse.b.curse = old_body->curse & 15;
  body->curse.b.emblem = old_body->emblem;
  for (i = 0; i < 20; i++)
    body->name[i] = old_body->name[i];
  body->other_col = old_body->other_col;
  body->temp = old_body->temp;
  body->my_tenpo_id = old_body->my_tenpo_id;
  body->money = old_body->money;
  body->syu.b.emblem_dan = old_body->emblem_dan;
  card_body_prize_misses_set(body, 0);
  card_body_syu_b11_set(body, 0);
  card_body_syu_top3_set(body, 0);
  body->quest.b.quest0 = old_body->quest0;
  body->quest.b.quest1 = old_body->quest1;
  body->misc.b[0] = old_body->misc[0] & 31;
  body->misc.b[1] = old_body->misc[1] & 0xfb;
  body->misc.b[2] = old_body->misc[2];
  body->rank_pnt = old_header->rank_pnt;
  body->tenpo_pnt = old_header->tenpo_pnt;
  header2->crc = 0;
  header2->flags = old_header2->flags | 0x30;
  header2->char_num = old_header2->char_num;
  header2->level = old_header2->level;
  header2->all_win = old_header2->all_win;
  header2->all_lose = old_header2->all_lose;
  for (i = 0; i < 8; i++)
    header2->item_have[i] = old_header2->item_have[i];
  header2->use_count = old_header2->use_count & 0x1ff;
  header2->ftf.b[0] = 0;
  header2->ftf.f.ex_ver = 1;
  for (i = 0; i < 3; i++)
    header2->card_id[i] = old_header2->card_id[i];
  header2->nickname = 0;
}

void card_format3_upgrade(int side)
{
  card_header_rec *header = &card_records[side].header;
  card_header_rec *header2 = &card_records[side].header2;
  card_body_rec *body = &card_records[side].body;
  card_upgraded[side] = 1;
  body->crc = 0;
  header2->crc = 0;
  header->crc = 0;
  header->ftf.f.ex_ver = 1;
  header2->ftf.f.ex_ver = 1;
}

void card_boot_nop(void)
{
  return;
}
