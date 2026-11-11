/*
 * vf4lib/librobdisp/src/rob_hand.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int hand_prj_assert_flag; /* 0x0c2dd520 */
extern int rob_hand_build_index; /* 0x0c2dd508 */
extern int rob_hand_build_player; /* 0x0c2dd50c */
extern int rob_hand_build_id; /* 0x0c2dd510 */
extern int rob_hand_build_size; /* 0x0c2dd514 */
extern int rob_hand_build_dst; /* 0x0c2dd51c */
extern static_heap_rec objmem_rob_hand[3]; /* 0x0c36010c */

void rob_hand_tables_clear(void)
{
  int i;
  int player;

  player = 0;
  do {
    i = 0;
    do {
      rob_hand_models[player][i] = 0;
      i = i + 1;
    } while (i < 0x20);
    player = player + 1;
  } while (player < 2);
  return;
}

void rob_hand_models_build(int player, uint model_id)
{
  int done;

  rob_hand_build_start(player,model_id);
  do {
    done = rob_hand_build_step();
  } while (done == 0);
  return;
}

void rob_hand_build_start(int player, uint model_id)
{
  int now;
  compiled_model_rec **p = &rob_hand_build_src;

  *p = obj_model_get(model_id);
  if (*p != 0) {
    rob_hand_clear_usec = stopwatch_usec();
    rob_hand_models_clear(player);
    now = stopwatch_usec();
    rob_hand_clear_usec = now - rob_hand_clear_usec;
    rob_hand_build_index = 0;
    rob_hand_build_id = model_id;
    rob_hand_build_player = player;
    rob_hand_size_usec = stopwatch_usec();
    rob_hand_build_size = model_size((int *)*p) + 0x1fU & 0xffffffe0;
    now = stopwatch_usec();
    rob_hand_size_usec = now - rob_hand_size_usec;
    rob_hand_dst_usec = stopwatch_usec();
    rob_hand_build_dst = (int)objmem_rob_hand[player].addr;
    now = stopwatch_usec();
    rob_hand_dst_usec = now - rob_hand_dst_usec;
  }
  return;
}

int rob_hand_build_step(void)
{
  float t0;
  float f;
  int i;
  int n;

  t0 = (float)(int)stopwatch_usec();
  if (rob_hand_build_src == 0 || rob_hand_build_dst == 0) return 1;
  {
    i = rob_hand_build_index;
    if (i < 0x20) {
      f = ((float)i * 7.0) / 31.0;
      n = (int)f;
      obj_morph_set(rob_hand_build_id, n, f - (float)n);
      model_copy_to_addr(rob_hand_build_dst, rob_hand_build_src);
      rob_hand_models[rob_hand_build_player][i] = (compiled_model_rec *)rob_hand_build_dst;
      rob_hand_build_dst = rob_hand_build_dst + rob_hand_build_size;
      rob_hand_step_usec[rob_hand_build_index] = (float)(int)stopwatch_usec() - t0;
      rob_hand_build_index = rob_hand_build_index + 1;
      return 0;
    }
    rob_hand_build_index = 0;
    rob_hand_build_size = 0;
    rob_hand_build_src = 0;
    rob_hand_build_dst = 0;
  }
  return 1;
}

void rob_hand_models_clear(int player)
{
  int i;
  compiled_model_rec **slot;

  slot = rob_hand_models[player];
  i = 0;
  do {
    *slot = 0;
    i = i + 1;
    slot = slot + 1;
  } while (i < 0x20);
  return;
}

void rob_hand_draw(rob_rec *rob, uint hand)
{
  char player;
  e_mode_main main_mode;
  int sub_mode;
  compiled_model_rec *model;
  uchar k = hand;

  main_mode = mode_get_main();
  if ((main_mode == MODE_MAIN_ADVERTISE) && (sub_mode = mode_get_sub(), sub_mode == 8)) {
    player = rob->model_slot;
  }
  else {
    player = rob->side;
  }
  model = rob_hand_models[player][(int)rob->hands[k].shape];
  if (model != 0) {
    model_draw(model);
  }
  else if (hand_prj_assert_flag == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/librobdisp/src/rob_hand.c",200,"assertion `%s\' failed.\n",
             "hand_prj_assert_flag");
  }
  return;
}
