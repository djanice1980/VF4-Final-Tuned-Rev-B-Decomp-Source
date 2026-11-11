/*
 * vf4lib/libbase/src/title_disp.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 title_aet_list; /* 0x0c1632cc */
extern int title_disp_frames; /* 0x0c28c224 */
extern float title_overlay_alpha; /* 0x0c28c228 */

void title_disp_task_start(void)
{
  typedef struct { int scene; float speed; } ent;
  int i;
  aet_obj_param param;

  for (i = 0; i < 5; i++) {
    ent *e = &((ent *)&title_aet_list)[i];
    aet_param_init(&param);
    aet_param_scene_set(&param, e->scene);
    aet_param_flags_set(&param, 4);
    aet_param_depth_range_set(&param, e->speed, e->speed);
    aet_obj_start(&param);
  }
  title_disp_frames = 0;
  title_overlay_alpha = 0.0;
  sel_bg_reset();
  sel_bg_alpha_set(0.0);
}

void title_disp_task_frame(void)
{
  if (0xf6 < title_disp_frames) {
    title_overlay_alpha = title_overlay_alpha + 0.027027028;
    if (1.0 < title_overlay_alpha) {
      title_overlay_alpha = 1.0;
    }
    sel_bg_alpha_set(title_overlay_alpha);
    sel_bg_draw();
  }
  title_disp_frames = title_disp_frames + 1;
  return;
}
