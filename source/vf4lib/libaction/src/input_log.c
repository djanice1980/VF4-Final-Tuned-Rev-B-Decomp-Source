/*
 * vf4lib/libaction/src/input_log.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char input_log[2][168]; /* 0x0c421ed8 */
extern uchar input_mute; /* 0x0c422028 */

void input_log_clear(void)
{
  int i;
  int j;
  input_log_rec *log;

  for (i = 0; i < 2; i++) {
    log = (input_log_rec *)input_log[i];
    log->flags |= 1;
    log->word = 0;
    for (j = 0; j < 0x10; j++) {
      log->dirs[j] = 0;
      log->buttons[j] = 0;
      log->presses[j] = 0;
    }
    log->idx = 0;
  }
  input_mute = 0;
}

void move_request_kind2_demote(uint side)
{
  rob_rec *rob;
  input_log_rec *log;
  if (side < 2) {
    log = (input_log_rec *)input_log[side];
    rob = rob_get(side & 0xff);
    if (((log->flags & 2) != 0) && ((rob->request.word & 0xff080000) == 0x2000000)) {
      rob->request.word = 0x1000000;
    }
  }
}

void input_log_push(int side, uint dir_word, uint button_word, uint press_word, ushort held_word)
{
  ushort dirs = dir_word;
  ushort buttons = button_word;
  ushort presses = press_word;
  ushort held = held_word;
  input_log_rec *log;
  char *n;
  int k;

  log = (input_log_rec *)input_log[side];
  n = &log->idx;
  if (++*n > 15) {
    *n = 0;
  }
  k = log->idx;
  log->dirs[k] = dirs;
  log->buttons[k] = buttons;
  log->presses[k] = presses;
  log->held[k] = held;
  return;
}

ushort input_log_dir_get(uint side, int back)
{
  input_log_rec *log;
  float *screen_xy;
  rob_rec *opp;
  ushort r;

  log = (input_log_rec *)input_log[side];
  if (back + 0xfU >= 0x10) return 0;
  r = log->dirs[(log->idx + back + 0x10) % 16];
  if ((r & 0x30) != 0) {
    screen_xy = (rob_get(side & 0xff))->screen_xy;
    opp = rob_get((side ^ 1) & 0xff);
    if (*screen_xy > opp->screen_xy[0]) {
      r = r ^ 0x30;
    }
  }
  return r;
}

ushort input_log_button_get(uint side, int back)
{
  input_log_rec *log = (input_log_rec *)input_log[side];
  if (back + 0xfU >= 0x10) return 0;
  return log->buttons[(log->idx + back + 0x10) % 16];
}

ushort input_log_press_get(uint side, int back)
{
  input_log_rec *log = (input_log_rec *)input_log[side];
  ushort r;
  if (back + 0xfU >= 0x10) return 0;
  r = log->presses[(log->idx + back + 0x10) % 16];
  if ((r & 0x30) != 0) {
    float *screen_xy = (rob_get(side & 0xff))->screen_xy;
    rob_rec *opp = rob_get((side ^ 1) & 0xff);
    if (*screen_xy > opp->screen_xy[0]) {
      r = r ^ 0x30;
    }
  }
  return r;
}

ushort input_log_held_get(uint side, int back)
{
  input_log_rec *log;
  ushort r;
  float *screen_xy;
  rob_rec *opp;

  log = (input_log_rec *)input_log + side;
  if ((uint)(back + 0xf) > 0xf) return 0;
  r = log->held[(log->idx + back + 0x10) % 0x10];
  if ((r & 0x30) != 0) {
    screen_xy = (rob_get(side & 0xff))->screen_xy;
    opp = rob_get((side ^ 1) & 0xff);
    if (*screen_xy > opp->screen_xy[0]) {
      r = r ^ 0x30;
    }
  }
  return r;
}

void input_log_extra_set(int side, short value)
{
  input_log_rec *log = (input_log_rec *)input_log[side];
  log->extra[log->idx] = value;
}

int input_log_extra_get(uint side, int back)
{
  input_log_rec *log = (input_log_rec *)input_log[side];
  if (back + 0xfU >= 0x10) return 0;
  return log->extra[(log->idx + back + 0x10) % 16];
}

bool input_log_word_set(uint side, ushort value)
{
  char *offset;
  if (side >= 2) return 0;
  offset = (char *)(side * 0xa8);
  *(ushort *)(offset + (int)&input_log[0][4]) = value;
  input_mute_on();
  return 1;
}

bool input_log_flag_bit1_set(uint side, uchar on)
{
  input_log_rec *log;

  if (side >= 2) return 0;
  log = (input_log_rec *)input_log[side];
  if (on != 0) {
    log->flags |= 2;
  }
  else {
    log->flags &= ~2;
  }
  return 1;
}

void input_mute_on(void)
{
  input_mute = 1;
  return;
}

void input_mute_off(void)
{
  input_mute = 0;
  return;
}

int input_mute_get(void)
{
  return input_mute;
}
