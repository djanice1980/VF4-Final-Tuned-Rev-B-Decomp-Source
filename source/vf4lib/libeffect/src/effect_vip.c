/*
 * vf4lib/libeffect/src/effect_vip.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 game_flags; /* 0x0c2925b8 */
extern void *vip_fx_token; /* 0x0c2b0ae4 */
extern char vip_fx_on; /* 0x0c2b0ae8 */
extern int vip_fx_models[3]; /* 0x0c2b0aec */
extern float vip_fx_speeds[3]; /* 0x0c2b0af8 */
extern float vip_fx_angles[6]; /* 0x0c2b0b04 */

void vip_stage_fx_init(void)
{
  vip_fx_on = 0;
  vip_fx_token = 0;
  vip_fx_token = effect_malloc(4);
  if (vip_fx_token != 0) {
    vip_fx_on = 1;
  }
  vip_fx_angles[0] = lcg_next_unit();
  vip_fx_angles[1] = lcg_next_unit();
  vip_fx_angles[2] = lcg_next_unit();
  vip_fx_angles[3] = lcg_next_unit();
  vip_fx_angles[4] = lcg_next_unit();
  vip_fx_angles[5] = lcg_next_unit();
  return;
}

void vip_stage_fx_move(void)
{
  return;
}

void vip_stage_fx_draw(void)
{
  rob_rec *rob;
  int model;
  float *angle;
  int i;
  int *model_p;
  float *speed;
  float step;
  float angle_old;
  int side;
  
  if ((vip_fx_on != '\0') && ((game_flags & 0x2000) != 0)) {
    mtx_push(0);
    camera_view_load();
    switch(vip_stage_no) {
    case '\x03':
    case '\x13':
    case '\x16':
      model = 0x100005;
      break;
    case '\x05':
    case '\x11':
    case '\x15':
    case '\x17':
      model = 0x100007;
      break;
    default:
      model = 0x100006;
      break;
    }
    obj_draw(model);
    rob = rob_get(0);
    side = 0;
    do {
      i = 0;
      angle = vip_fx_angles + side * 3;
      speed = vip_fx_speeds;
      model_p = vip_fx_models;
      do {
        angle_old = *angle;
        step = *speed;
        speed = speed + 1;
        *angle = angle_old + step;
        mtx_push(0);
        mtx_translate(rob->pos.x,0.0,rob->pos.z);
        mtx_rot_y_rad(angle_old + step);
        model = *model_p;
        model_p = model_p + 1;
        obj_draw(model);
        mtx_pop(1);
        i = i + 1;
        angle = angle + 1;
      } while (i < 3);
      side = side + 1;
      rob = rob + 1;
    } while (side < 2);
    mtx_pop(1);
  }
  return;
}

void vip_stage_fx_exit(void)
{
  if (vip_fx_on != '\0') {
    effect_free(vip_fx_token);
    vip_fx_token = 0;
  }
  return;
}
