/*
 * gfxlib/list_capture.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint slot_list_end_word[5]; /* 0x0c211e08 */
extern capture_rec *capture_open; /* 0x0c32a4fc */
extern int write_ptrs_saved; /* 0x0c32a520 */
extern capture_rec *capture_first; /* 0x0c32a524 */
extern uint capture_count; /* 0x0c32a528 */
extern int capture_mode_on; /* 0x0c32a52c */
extern uint *list_starts_saved; /* 0x0c32a530 */
extern capture_rec capture_records[8]; /* 0x0c4174b8 */
extern list_start_block captured_list_starts; /* 0x0c417618 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void ring_slot_lists_record(int slot)
{
  volatile int *start;
  volatile uint *bytes;
  uint i;

  start = &gfx_state.slot_list_start[0][slot];
  bytes = &gfx_state.slot_list_bytes[0][slot];
  gfx_state.slot_list_start[0][slot] = list_starts[0];
  gfx_state.slot_list_start[1][slot] = list_starts[1];
  gfx_state.slot_list_start[2][slot] = list_starts[2];
  gfx_state.slot_list_start[3][slot] = list_starts[3];
  gfx_state.slot_list_start[4][slot] = list_starts[4];
  for (i = 0; i < 5; bytes += 4, start += 4, i++) {
    *bytes = (((int *)list_write_ptrs)[i] - *start) >> 2 << 2;
    if (gfx_state.single_bank == 0 || i != gfx_state.direct_list) {
      if (((1 << i) & gfx_state.used_lists) != 0) {
        copy_words(((uint **)list_write_ptrs)[i],slot_list_end_word,4);
        list_write_ptrs[i] = list_write_ptrs[i] + 0x20;
        if (*bytes > gfx_state.list_limits[i]) {
          error_hook_run(2);
        }
      }
      else {
        *bytes = 0;
      }
    }
  }
}

void ring_slot_send_all(int slot)
{
  gfx_state.ta_slot = slot;
  gfx_state.send_mask = gfx_state.used_lists;
  gfx_state.field_0x364 = 0;
  if (gfx_state.single_bank == 0) {
    lists_send_to_ta(0xff);
    return;
  }
  for (;;) ;
}

void capture_send_via_slot(capture_rec *rec, int slot)
{
  volatile uint *bytes;
  uint i;
  volatile int *start;
  if (gfx_state.single_bank == 0) {
    start = &gfx_state.slot_list_start[0][slot];
    bytes = &gfx_state.slot_list_bytes[0][slot];
    for (i = 0; i < 5; i++) {
      *start = rec->list_start[i];
      *bytes = rec->list_size[i];
      bytes = bytes + 4;
      start = start + 4;
    }
    gfx_state.ta_slot = slot;
    gfx_state.send_mask = rec->list_mask;
    gfx_state.field_0x364 = 0;
    lists_send_to_ta(0xff);
  }
}

void capture_first_begin(void)
{
  capture_count = 0;
  capture_first = capture_records;
  capture_open = capture_records;
  capture_records[0].list_mask = gfx_state.used_lists;
  capture_records[0].list_start[0] = list_starts[0];
  capture_records[0].list_start[1] = list_starts[1];
  capture_records[0].list_start[2] = list_starts[2];
  capture_records[0].list_start[3] = list_starts[3];
  capture_records[0].list_start[4] = list_starts[4];
  return;
}

void capture_first_open(void)
{
  capture_rec *rec;
  uint i;

  capture_first = &capture_records[capture_count];
  rec = capture_first;
  capture_open = rec;
  rec->list_mask = gfx_state.used_lists;
  for (i = 0; i < 5; i++) {
    rec->list_start[i] = list_write_ptrs[i];
  }
  return;
}

void capture_first_close(void)
{
  capture_rec *rec;
  uint i;
  uint empty;

  empty = 0;
  rec = capture_open;
  if (rec != 0 && rec == capture_first) {
    for (i = 0; i < 5; i++) {
      if ((1 << i) & rec->list_mask) {
        rec->list_size[i] = list_write_ptrs[i] - rec->list_start[i];
        if (rec->list_size[i] != 0) {
          list_write_ptrs[i] += 0x20;
        }
        else {
          rec->list_size[i] = 0;
          empty++;
        }
      }
      else {
        rec->list_size[i] = 0;
        empty++;
      }
    }
    capture_first = 0;
    capture_open = 0;
    if (empty < 5) {
      capture_count++;
    }
  }
  return;
}

void capture_rec_replay(capture_rec *rec)
{
  int i;
  for (i = 0; i < 5; i++) {
    if (rec->list_size[i] != 0) list_seg_append_padded(i, rec->list_start[i], rec->list_size[i]);
  }
}

void capture_all_replay(void)
{
  uint i;

  for (i = 0; i < capture_count; i = i + 1) {
      capture_rec_replay(&capture_records[i]);
  }
  return;
}

void list_starts_from_capture(capture_rec *rec)
{
  list_starts_saved = list_starts;
  list_starts = (uint *)&captured_list_starts;
  captured_list_starts.list0_start = rec->list_start[0];
  captured_list_starts.list1_start = rec->list_start[1];
  captured_list_starts.list2_start = rec->list_start[2];
  captured_list_starts.list3_start = rec->list_start[3];
  captured_list_starts.list4_start = rec->list_start[4];
  return;
}

void write_ptrs_saved_clear(void)
{
  write_ptrs_saved = 0;
  return;
}

void write_ptrs_save_once(void)
{
  int i;

  if (write_ptrs_saved == 0) {
    for (i = 0; i < 5; i++) {
      write_ptrs_save[i] = list_write_ptrs[i];
    }
    write_ptrs_saved = 1;
  }
  return;
}

void write_ptrs_restore(void)
{
  int i;

  if (write_ptrs_saved != 0) {
    for (i = 0; i < 5; i++) {
      list_write_ptrs[i] = write_ptrs_save[i];
    }
    write_ptrs_saved = 0;
  }
  return;
}

void capture_rec_open(capture_rec *rec)
{
  uint i;

  capture_open = rec;
  for (i = 0; i < 5; i++) {
    if ((1 << i) & rec->list_mask) {
      rec->list_start[i] = list_write_ptrs[i];
    }
  }
  return;
}

void capture_rec_close(void)
{
  capture_rec *rec;
  uint i;

  rec = capture_open;
  for (i = 0; i < 5; i++) {
    if ((1 << i) & rec->list_mask) {
      rec->list_size[i] = list_write_ptrs[i] - rec->list_start[i];
      if (rec->list_size[i] != 0) {
        list_write_ptrs[i] += 0x20;
      }
      else {
        rec->list_size[i] = 0;
      }
    }
    else {
      rec->list_size[i] = 0;
    }
  }
  return;
}

void capture_mode_begin(void)
{
  capture_mode_on = 1;
  capture_open = 0;
  capture_first_begin();
  return;
}

void capture_mode_finish(void)
{
  if (((capture_open != 0) && (capture_open == capture_first)) && (capture_count != 0)) {
    list_starts_from_capture(capture_open);
  }
  capture_all_replay();
  return;
}
