/*
 * testmode/card_test.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int card_test_read_run(int unit, card_test_rec *rec)
{
  int r;
  int res;

  r = 0;
  switch (rec->sub_step) {
  case 0:
    if ((rec->flags & 1) != 0) {
      if (card_read_start(unit) == 0) {
        rec->sub_step = 1;
      }
    }
    else {
      rec->sub_step = 11;
    }
    break;
  case 1:
    res = card_unit_result(unit);
    if (res == 1) {
      break;
    }
    if (res == 0) {
      card_read_finish(unit);
      rec->sub_step = 10;
    }
    else {
      rec->sub_step = 11;
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

int card_test_write_run(int unit, card_test_rec *rec)
{
  int r = 0;
  int v;

  switch (rec->sub_step) {
  case 0:
    if (rec->flags & 1) {
      if (card_write_start(unit,0) == 0) {
        rec->sub_step = 1;
      }
    }
    else {
      rec->sub_step = 11;
      card_test_write_error = 1;
    }
    break;
  case 1:
    v = card_unit_result(unit);
    if (v == 1) {
      break;
    }
    if (v == 0) {
      rec->sub_step = 10;
    }
    else {
      rec->sub_step = 11;
      card_test_write_error = 1;
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

void card_test_read_step(int unit)
{
  card_test_rec *u;
  int r;

  u = &card_menu_state.unit[unit];
  switch (u->step) {
  case 0:
    u->step = 1;
    u->sub_step = 0;
    u->result = 0;
  case 1:
    card_test_message_set(unit, 3);
    r = card_test_read_run(unit, u);
    if (r == 1) {
      u->step = 2;
    }
    else if (r == 2) {
      u->step = 8;
    }
    break;
  case 2:
    if (card_header_check(unit) == 0) {
      u->result = 2;
      card_test_message_set(unit, 12);
    }
    else {
      u->result = 1;
      card_test_message_set(unit, 4);
    }
    u->step = 10;
    break;
  case 8:
    u->result = 3;
    card_test_message_set(unit, 5);
    u->step = 10;
    break;
  case 10:
    u->kind = 0;
    break;
  }
}

void card_test_write_step(int unit)
{
  card_test_rec *r = &card_menu_state.unit[unit];
  card_body_rec *c = &card_records[unit].body;
  int res;
  switch (r->step) {
  case 0:
    card_test_verify_error = 0;
    r->step = 1;
    r->sub_step = 0;
    r->word_0x28 = 0;
    r->word_0x24 = 0;
  case 1:
    r->step = 2;
    r->sub_step = 0;
    r->value = lcg_next15();
    c->play_id = r->value;
  case 2:
    card_test_message_set(unit,3);
    res = card_test_write_run(unit,r);
    if (res == 1) {
      r->step = 3;
      r->sub_step = 0;
    }
    else if (res == 2) {
      r->step = 8;
      card_test_verify_error = 1;
    }
    break;
  case 3:
    res = card_test_read_run(unit,r);
    if (res == 1) {
      r->step = 4;
    }
    else if (res == 2) {
      r->step = 8;
      card_test_verify_error = 2;
    }
    break;
  case 4:
    if (r->value == c->play_id) {
      card_test_message_set(unit,6);
      r->step = 10;
    }
    else {
      r->step = 8;
      card_test_verify_error = 3;
    }
    break;
  case 8:
    card_test_message_set(unit,7);
    r->step = 10;
    break;
  case 10:
    r->kind = 0;
    break;
  }
}

void card_test_format_step(int unit)
{
  int r;
  card_test_rec *rec;

  rec = &card_menu_state.unit[unit];
  switch (rec->step) {
  case 0:
    rec->step = 1;
    rec->sub_step = 0;
    card_record_blank_unpack(unit);
  case 1:
    card_test_message_set(unit, 3);
    r = card_test_write_run(unit, rec);
    if (r == 1) {
      rec->step = 10;
      card_test_message_set(unit, 8);
    }
    else if (r == 2) {
      rec->step = 8;
    }
    break;
  case 8:
    card_test_message_set(unit, 9);
    rec->step = 10;
    break;
  case 10:
    rec->kind = 0;
    break;
  }
}

bool card_test_start(int unit, int kind)
{
  card_test_rec *rec = &card_menu_state.unit[unit];
  if (rec->kind != 0) {
    return 0;
  }
  rec->kind = kind;
  rec->step = 0;
  rec->sub_step = 0;
  return 1;
}

void card_record_blank_unpack(int unit)
{
  uchar buf[108];
  int i;

  for (i = 0; i < 107; i++)
    buf[i] = 0;
  buf[107] = 0xff;
  card_unpack(unit, buf);
}
