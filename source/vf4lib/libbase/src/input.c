/*
 * vf4lib/libbase/src/input.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint player_start_bit[2]; /* 0x0c168080 */
extern int jvs_ack_errors_last[3][8]; /* 0x0c3777c4 */
extern player_input_rec player_input[4]; /* 0x0c41f7b0 */
extern char jvs_board_count; /* 0x0c41fb40 */
extern uint input_error_flags; /* 0x0c41fbd0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern jvs_board_status_rec jvs_board_status; /* 0x0c41fb44 */
extern jvs_board_input_rec board_buttons[2]; /* 0x0c41fb48 */
extern button_word input_merged; /* 0x0c41fbb0 */
extern int jvs_check_frames[2]; /* 0x0c41fbec */
extern int jvs_bad_frames[2]; /* 0x0c41fbf4 */
extern int jvs_bad_run[2]; /* 0x0c41fbfc */
extern input_repeat_cfg_rec input_repeat_cfg; /* 0x0c41fc04 */

void button_word_update(button_word *word, uint now)
{
  uint old;

  old = word->now;
  word->now = now;
  word->on = ~old & now;
  word->off = old & ~now;
  word->old = old;
  return;
}

void input_init(void)
{
  char i;

  jvs_bus_setup();
  coin_state_clear();
  for (i = 0; i < 2; i++) {
    (&board_buttons[0].buttons.now)[i * 6] = 0;
    jvs_board_reads[i].buttons.now = 0;
    jvs_coin_counts[i][0] = 0;
    jvs_coin_counts[i][1] = 0;
    jvs_switch_errors[i] = 0;
    jvs_coin_errors[i] = 0;
    jvs_analog_errors[i] = 0;
  }
  jvs_error_counts_clear();
  input_error_flags = 0;
  jvs_board_count = jvs_node_count;
  jvs_board_status.boards_present = 0;
  jvs_board_status.boards_in_use = 0;
  if (jvs_board_count < 1) {
    jvs_board_count = 0;
    input_error_flags = 1;
  }
  else if (jvs_board_count == 1) {
    jvs_board_status.boards_present = 1;
    jvs_board_status.boards_in_use = 1;
  }
  else if (jvs_board_count == 2) {
    jvs_board_status.boards_in_use = 2;
    jvs_board_status.boards_present = 1;
  }
  else if (jvs_board_count > 2) {
    jvs_board_count = 0;
    input_error_flags |= 1;
  }
  if (jvs_nodes[1].switches[1] < 9) {
    input_error_flags = input_error_flags | 8;
  }
  input_enabled = 0;
}

void empty_fn_0c0487c0(void)
{
  return;
}

void input_read(void)
{
  uint switches;
  int node;

  if (!(jvs_board_count < '\x01')) {
    for (node = 1; node <= jvs_board_count; node = node + 1) {
        jvs_node_input_read(node);
    }
  }
  else {
    switches = board_switch_bits();
    button_word_update(&jvs_board_reads[0].buttons,switches);
  }
  system_switches_read = board_switch_bits();
  return;
}

uint input_error_flags_get(void)
{
  return input_error_flags;
}

int jvs_board_count_get(void)
{
  return jvs_board_count;
}

void jvs_node_input_read(int node)
{
  char switches[4][16];
  short analog[8];
  short coins[4];
  jvs_node_desc *desc;
  char test;
  int i;
  uint buttons;
  int p1_byte0, p1_byte1, p2_byte0, p2_byte1;
  int mask;

  test = 0;
  desc = &jvs_nodes[node];
  {
    char *p;
    int n;
    int w;
    int i;
    int j;
    char *q;
    n = desc->switches[0];
    w = desc->switches[2];
    p = jvs_reply_data[node];
    p += (int)jvs_reply_offset[node][0x20];
    if (*p++ == 1) {
      test = *p++;
      for (i = 0; i < n; i++) {
        q = switches[i];
        for (j = 0; j < w; j++, q++) *q = *p++;
      }
    }
    else {
      jvs_switch_errors[node - 1]++;
    }
  }
  {
    char *p;
    int n;
    int i;
    n = desc->coin_slots;
    if (n > 0) {
      p = jvs_reply_data[node];
      p += (int)jvs_reply_offset[node][0x21];
      if (*p++ == 1) {
        for (i = 0; i < n; i++) {
          int k = *p++ << 8;
          k |= *p++;
          coins[i] = k;
        }
      }
      else {
        jvs_coin_errors[node - 1]++;
      }
    }
  }
  p1_byte0 = switches[0][0];
  p1_byte1 = switches[0][1];
  p2_byte0 = switches[1][0];
  p2_byte1 = switches[1][1];
  buttons = (test >> 7 & 1) << 2 | (p1_byte0 >> 6 & 1) << 3 | (p1_byte0 >> 7 & 1) << 4 |
      (p1_byte0 >> 1 & 1) << 8 | (p1_byte0 & 1) << 9 | (p1_byte1 >> 7 & 1) << 10 |
      (p1_byte1 >> 6 & 1) << 11 | (p1_byte0 >> 4 & 1) << 12 | (p1_byte0 >> 5 & 1) << 13 |
      (p1_byte0 >> 2 & 1) << 14 | (p1_byte0 >> 3 & 1) << 15 |
      (p2_byte0 >> 7 & 1) << 5 | (p2_byte0 >> 1 & 1) << 16 | (p2_byte0 & 1) << 17 |
      (p2_byte1 >> 7 & 1) << 18 | (p2_byte1 >> 6 & 1) << 19 | (p2_byte0 >> 4 & 1) << 20 |
      (p2_byte0 >> 5 & 1) << 21 | (p2_byte0 >> 2 & 1) << 22 | (p2_byte0 >> 3 & 1) << 23;
  mask = 0x3fff;
  if (jvs_coin_counts[node - 1][0] != (short)(coins[0] & mask)) buttons |= 1;
  if (desc->coin_slots == 2) {
    if (jvs_coin_counts[node - 1][1] != (short)(coins[1] & mask)) buttons |= 2;
  }
  jvs_coin_counts[node - 1][0] = coins[0] & mask;
  jvs_coin_counts[node - 1][1] = coins[1] & mask;
  button_word_update(&jvs_board_reads[node - 1].buttons,buttons);
  {
    char *p;
    int n;
    n = desc->analog_channels;
    if (n > 0) {
      p = jvs_reply_data[node];
      p += (int)jvs_reply_offset[node][0x22];
      if (*p++ == 1) {
        for (i = 0; i < n; i++) {
          int k = *p++ << 8;
          k |= *p++;
          analog[i] = k;
        }
      }
      else {
        jvs_analog_errors[node - 1]++;
      }
    }
    else {
      for (i = 0; i < 8; i++) analog[i] = 0;
    }
  }
  for (i = 0; i < 8; i++) {
    jvs_board_reads[node - 1].analog[i] = analog[i] >> 8;
  }
}

uint board_switch_bits(void)
{
  int high;
  short bits;
  bits = ~((int) legacy_io_inputs.bytes[5]);
  high = (bits & 0x10) << 0x18;
  return ((((high | (((((int) bits) >> 5) & 1U) << 0x1d)) | ((bits & 1) << 0x18)) | ((bits & 2) << 0x18)) | ((bits & 4) << 0x18)) | ((bits & 8) << 0x18);
}

uint dip_switch_bit_get(int index)
{
  char map [4];
  char bit;
  uint result;
  memcpy(map,dip_switch_bit_map,4);
  bit = map[index];
  if (bit >= 0) result = (system_switches >> bit) & 1;
  else result = 0;
  return result;
}

void input_frame_update(void)
{
  int board;

  system_switches = system_switches_read;
  if (!(jvs_board_count < '\x01')) {
    for (board = 0; board < jvs_board_count; board = board + 1) {
        button_word_update(&board_buttons[board].buttons,jvs_board_reads[board].buttons.now);
        jvs_node_error_check((int)(char)board);
    }
  }
  else {
    button_word_update(&board_buttons[0].buttons,jvs_board_reads[0].buttons.now);
  }
  input_merged_update();
  return;
}

uint input_boards_merge(uint board0, uint board1, uint system)
{
  return (board1 | board0) & 0xc | board1 & 0xff10 | board0 & 0xff0020 |
         system & 0x3f000000;
}

void input_merged_update(void)
{
  uint merged;

  if (jvs_board_count == '\x01')
    merged = input_boards_merge(board_buttons[0].buttons.now,board_buttons[0].buttons.now,system_switches);
  else if (jvs_board_count == '\x02')
    merged = input_boards_merge(board_buttons[0].buttons.now,board_buttons[1].buttons.now,system_switches);
  else
    merged = system_switches;
  button_word_update(&input_merged,merged);
}

int input_is_1_or_2(int n)
{
  return (uint)(n - 1) < 2;
}

uint input_merged_bit_pressed(uint bit)
{
  return (1 << bit) & input_merged.on;
}

uint input_merged_bit_held(uint bit)
{
  return (1 << bit) & input_merged.now;
}

uint input_merged_pressed(uint mask)
{
  return mask & input_merged.on;
}

uint input_merged_held(uint mask)
{
  return mask & input_merged.now;
}

void jvs_error_counts_clear(void)
{
  jvs_check_frames[0] = 0;
  jvs_bad_frames[0] = 0;
  jvs_bad_run[0] = 0;
  jvs_check_frames[1] = 0;
  jvs_bad_frames[1] = 0;
  jvs_bad_run[1] = 0;
  return;
}

void jvs_node_error_check(int node)
{
  int j;
  int bad;
  int errors;

  node = (char)node;
  if (++jvs_check_frames[node] > 300) {
    bad = 0;
    for (j = 0; j < 8; j++) {
      jvs_ack_errors_last[node + 1][j] = errors = jvs_ack_status_errors[node + 1][j];
      if (errors != 0) {
        bad = 1;
      }
      jvs_ack_status_errors[node + 1][j] = 0;
    }
    if (bad) {
      jvs_bad_frames[node] = jvs_bad_frames[node] + 1;
      jvs_bad_run[node] = jvs_bad_run[node] + 1;
    }
    else {
      jvs_bad_run[node] = 0;
    }
    if (0x14 < jvs_bad_run[node]) {
      input_error_flags = input_error_flags | 2;
    }
    if ((jvs_bad_frames[node] * 100) / jvs_check_frames[node] > 20) {
      input_error_flags = input_error_flags | 4;
    }
  }
  return;
}

void input_repeat_set(int delay, int rate)
{
  input_repeat_cfg.repeat_delay = delay;
  input_repeat_cfg.repeat_rate = rate;
  return;
}

void player_input_init(void)
{
  memset(player_input,0,0x390);
  input_repeat_cfg.repeat_delay = 0x14;
  input_repeat_cfg.repeat_rate = 1;
  input_repeat_cfg.repress_window = 0xf;
  input_repeat_cfg.long_hold = 0xc;
  input_repeat_cfg.repeat_mask = 0xff;
  input_repeat_cfg.long_hold_mask = 0xf;
  input_enabled = 1;
  return;
}

void empty_fn_0c049120(void)
{
  return;
}

void empty_fn_0c049140(void)
{
  return;
}

void player_input_buttons_copy(player_input_rec *input, ushort *dst)
{
  *dst = *(ushort *)&input->now;
  return;
}

uint player_buttons_from_switches(uint switches, uint start_bit, int shift)
{
  uint bits;
  char button_type;
  uint buttons;
  uint a, b, c, d;

  buttons = 0;
  bits = (int)switches >> shift;
  button_type = user_back_data.button_type;
  c = 1; /* WORKAROUND (kept as possibly original; no game instruction of its own): redundant before the switch, since every arm sets c. Without it the game loses no instruction of its own, but GCC stops reusing this 1 for the cmp/gt below and the arms' constants move. Kept as a plausible original default. */
  switch (button_type) {
  case 0:
  default:
    a = 2;
    b = 4;
    c = 1;
    d = 8;
    break;
  case 1:
    a = 2;
    b = 4;
    c = 8;
    d = 1;
    break;
  case 2:
    a = 1;
    b = 2;
    c = 4;
    d = 8;
    break;
  }
  if (bits & 0x100) buttons |= a;
  if (bits & 0x200) buttons |= b;
  if (bits & 0x400) buttons |= c;
  if (bits & 0x800) buttons |= d;
  if (bits & 0x1000) buttons |= 0x40;
  if (bits & 0x2000) buttons |= 0x80;
  if (bits & 0x4000) buttons |= 0x10;
  if (bits & 0x8000) buttons |= 0x20;
  if (switches & start_bit) buttons |= 0x8000;
  return buttons;
}

void player_input_read(void)
{
  int i;
  player_input_rec *input;
  int is_first;
  int shift;
  uint start_bit;
  uint old;
  uint now;

  for (i = 0; i < 2; i++) {
    input = &player_input[i];
    is_first = i == 0; shift = (is_first != 1) << 3;
    start_bit = player_start_bit[i % 2];
    memmove(&input->history[1], &input->history[0], 0x13);
    old = input->now;
    input->history[0] = old;
    now = player_buttons_from_switches(input_merged.now, start_bit, shift);
    input->now = now;
    input->pressed = ~old & now;
    input->released = old & ~now;
    player_input_buttons_copy(input, &player_buttons[i]);
  }
}

void player_input_tap_hold_repeat(void)
{
  player_input_rec *input;
  uint side;
  int i;
  uint bit;
  uint repressed;
  uint long_held;
  uint repeated;
  uint pressed;
  uint released;
  uint now;
  char *frames;

  for (side = 0; side < 2; side++) {
    input = &player_input[side];
    now = input->now;
    pressed = input->pressed;
    released = input->released;
    repressed = 0;
    for (i = 0; i < 16; i++) {
      bit = 1 << i;
      switch (input->repress_state[i]) {
      case 0:
      default:
        if ((released & bit) != 0) {
          input->repress_state[i] = 1;
          input->repress_frames[i] = 0;
        }
        break;
      case 1:
        if (++input->repress_frames[i] > input_repeat_cfg.repress_window) {
          input->repress_state[i] = 0;
        }
        else if ((pressed & bit) != 0) {
          repressed |= bit;
          input->repress_state[i] = 0;
        }
        break;
      }
    }
    input->repressed = repressed;
    long_held = 0;
    for (i = 0; i < 16; i++) {
      bit = 1 << i;
      if ((now & bit) != 0 && (input_repeat_cfg.long_hold_mask & bit) != 0) {
        input->hold_frames[i]++;
      }
      else if ((released & bit) == 0) {
        input->hold_frames[i] = 0;
      }
      if (input->hold_frames[i] >= input_repeat_cfg.long_hold) {
        long_held |= bit;
      }
    }
    input->long_held = long_held;
    repeated = 0;
    for (i = 0; i < 16; i++) {
      bit = 1 << i;
      if ((now & bit) != 0 && (input_repeat_cfg.repeat_mask & bit) != 0) {
        switch (input->repeat_state[i]) {
        case 0:
        default:
          repeated |= bit;
          input->repeat_state[i] = 1;
          input->repeat_frames[i] = 0;
          break;
        case 1:
          frames = (char *)(i + (int)input) + 0xc4;
          *frames = *frames + 1;
          if (*frames > input_repeat_cfg.repeat_delay) {
            repeated |= bit;
            input->repeat_state[i] = 2;
            *frames = 0;
          }
          break;
        case 2:
          { char *q = (char *)(i + (int)input); frames = q + 0xc4; }
          *frames = *frames + 1;
          if (*frames > input_repeat_cfg.repeat_rate) {
            repeated |= bit;
            *frames = 0;
          }
          break;
        }
      }
      else {
        input->repeat_state[i] = 0;
      }
    }
    input->repeated = repeated;
  }
}

void input_per_frame(void)
{
  if (input_enabled != '\0') {
    input_frame_update();
    player_input_read();
    player_input_tap_hold_repeat();
  }
  return;
}

player_input_rec *player_input_get(int side)
{
  return &player_input[side];
}
