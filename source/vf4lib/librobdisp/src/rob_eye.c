/*
 * vf4lib/librobdisp/src/rob_eye.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar eye_index; /* 0x0c2b0fdc */

void eye_draw(rob_rec *rob, int model_id)
{
  obj_node *node;

  node = (obj_node *)obj_node_get(model_id);
  if (node != 0) {
    mtx_translate_by_vec3(node->pos);
    mtx_rot_z_s16((int)node->rot[2]);
    mtx_rot_y_s16((int)node->rot[1]);
    mtx_rot_x_s16((int)node->rot[0]);
    mtx_rot_z_s16((int)rob->eye_angles[eye_index + 2]);
    mtx_rot_y_s16(rob->eye_angles[eye_index]);
    mtx_rot_x_s16(rob->eye_angles[eye_index + 4]);
    obj_draw(model_id);
  }
  return;
}

void eyes_draw(rob_rec *rob)
{
  rob_eye_rec *eye;

  eye = eye_table[rob->chara][rob->costume];
  eye_index = '\0';
  mtx_push(0);
  mtx_translate_by_vec3(eye->offset);
  if (0 < eye->model_id) {
    eye_draw(rob,eye->model_id);
  }
  eye = eye + 1;
  eye_index = eye_index + '\x01';
  mtx_peek_pushed(0);
  mtx_translate_by_vec3(eye->offset);
  if (0 < eye->model_id) {
    eye_draw(rob,eye->model_id);
  }
  mtx_pop(1);
  return;
}
