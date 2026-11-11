/*
 * vf4lib/libeffect/src/effect.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int effect_slots_ready; /* 0x0c2aa68c */
extern float effect_slot_times[2][50]; /* 0x0c38969c */
extern effect_slot effect_slots[50]; /* 0x0c38982c */

void effect_task_start(void)
{
  return;
}

void effect_list_start(effect_slot *list)
{
  if (!(effect_slots_ready == 0)) {
    effect_slots_free_all();
  }
  else {
    effect_slots_clear();
    effect_slots_ready = 1;
  }
  effect_slots_add(list);
  effect_slots_init_all();
  return;
}

void effect_slots_add(effect_slot *list)
{
  effect_slot *s;
  int i;

  s = effect_slots;
  i = 0;
  do {
    if (list->on == -1) {
      return;
    }
    if (s->on == 0) {
      s->on = list->on;
      s->name = list->name;
      s->init = list->init;
      s->exec = list->exec;
      s->draw = list->draw;
      s->free = list->free;
      list = list + 1;
    }
    i = i + 1;
    s++;
  } while (i < 0x32);
  return;
}

void effect_slots_clear(void)
{
  effect_slot *s;
  int i;

  s = effect_slots;
  i = 0;
  do {
    s->on = 0;
    s->init = 0;
    s->exec = 0;
    s->draw = 0;
    s->free = 0;
    i = i + 1;
    s++;
  } while (i < 0x32);
  return;
}

void effect_slots_free_all(void)
{
  effect_slot *s;
  int i;
  int used;

  s = effect_slots;
  i = 0;
  do {
    if (s->free != 0) {
      s->free();
    }
    s->on = 0;
    s->init = 0;
    s->exec = 0;
    s->draw = 0;
    s->free = 0;
    i = i + 1;
    s = s + 1;
  } while (i < 0x32);
  used = effect_heap_used_get();
  if (used != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect.c",0x165,"assertion `%s\' failed.\n","0"
            );
  }
  return;
}

void effect_slots_init_all(void)
{
  effect_slot *s;
  int i;

  s = effect_slots;
  i = 0;
  do {
    if ((s->on != 0) && (s->init != 0)) {
      s->init();
    }
    i = i + 1;
    s = s + 1;
  } while (i < 0x32);
  return;
}

void effect_task_exec(void)
{
  uint paused;
  int i;
  effect_slot *s;
  
  for (i = 0; i < 0x32; i++) {
    float *p = &effect_slot_times[0][0] + i;
    p[50] = 0.0;
    p[0] = 0.0;
  }
  paused = pause_frame_held();
  if ((paused == 0) || (deb_sw != 0)) {
    s = effect_slots;
    for (i = 0; i < 0x32; i++, s++) {
      if ((s->on != 0) && (s->exec != 0)) {
        s->exec();
      }
    }
  }
  effect_slots_draw_all();
}

void effect_slots_draw_all(void)
{
  effect_slot *s;
  int i;

  if ((demo_flags & 0x800) == 0) {
    s = effect_slots;
    i = 0;
    do {
      if ((s->on != 0) && (s->draw != 0)) {
        s->draw();
      }
      i = i + 1;
      s = s + 1;
    } while (i < 0x32);
  }
  return;
}
