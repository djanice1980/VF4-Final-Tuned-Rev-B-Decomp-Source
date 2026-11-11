/*
 * vf4lib/libeffect/src/effect_adv.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float adv_fx_positions[6][3]; /* 0x0c2b0208 */
extern adv_fx_anim adv_fx_anims[6]; /* 0x0c489844 */
extern short adv_fx_frame; /* 0x0c489874 */

void adv_fx_init(void)
{
  adv_fx_anim *p;
  int i;
  p = adv_fx_anims;
  for (i = 0; i < 6; i++, p++) {
    p->t = (float)((int)lcg_next15() % 80);
  }
  adv_fx_frame = 0;
}

void adv_fx_update(void)
{
  adv_fx_anim *p;
  int i;
  p = adv_fx_anims;
  if (demo_stage_select == 0) {
    for (i = 0; i < 6; i++, p++) {
      p->t += adv_fx_anim_step;
      p->frame = p->t;
      if (p->frame > 79) {
        p->t = 0.0;
        p->frame = 0;
      }
    }
    if (++adv_fx_frame > 29) adv_fx_frame = 0;
  }
}

void adv_fx_draw(void)
{
  e_mode_main mode;
  int i;
  adv_fx_anim *p;
  
  mode = mode_get_main();
  if (((mode != MODE_MAIN_ADVERTISE) || ((demo_flags & 0x800) == 0)) && (demo_stage_select == '\0')) {
    p = adv_fx_anims;
    mtx_push(0);
    i = 0;
    do {
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(adv_fx_positions[i]);
      obj_draw(p->frame + 0x170000);
      i = i + 1;
      p = p + 1;
    } while (i < 6);
    mtx_peek_pushed(0);
    obj_draw(((short)adv_fx_frame) + 0x170053);
    mtx_pop(1);
  }
  return;
}

void adv_fx_free(void)
{
  return;
}
