/*
 * vf4lib/libeffect/src/effect_mot.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern short light_mot_pairs[4][4]; /* 0x0c2b0a80 */
extern char light_mot_choice; /* 0x0c2b0aa0 */
extern char light_mot_force; /* 0x0c2b0aa1 */
extern mot_path_slot mot_path_slots[3]; /* 0x0c397b6c */
extern undefined * mot_paths_block; /* 0x0c397b84 */
extern undefined * light_mot_paths_block; /* 0x0c397b88 */
extern short fish_mot_list[2]; /* 0x0c2b0aa2 */

void * mot_paths_bind(uchar first, uchar end, short *motions)
{
  int i;
  void *block;
  char *p;
  mot_path_slot *slot;
  
  slot = &mot_path_slots[first];
  block = p = effect_malloc(900);
  for (i = first; i < end; i++, motions += 2) {
      slot->bone = (bone_work_rec *)p;
      p += 0x6c;
      mot_channel_pool_set((mot_channel *)p);
      slot->mot = (int)p;
      p += 300 - 0x6c;
      rob_bones_init(0,1,slot->bone,bone_defs + 0x284);
      mot_bind(0,1,slot->bone,(int)*motions);
      motions[1] = mot_bind_frame_count();
      slot = slot + 1;
  }
  return block;
}

void mot_path_eval(mot_path_slot *slot, float frame)
{
  bone_mot_eval_flat(slot->bone,frame * 0.033333335);
  return;
}

void light_mot_paths_bind(void)
{
  char *p;
  uint rnd;
  uint rnd_adj;
  p = &light_mot_force;
  if (*p != '\0') {
    light_mot_choice = *p + -1;
  }
  else {
    rnd = lcg_next15();
    rnd_adj = rnd;
    if ((int)rnd < 0) {
      rnd_adj = rnd + 3;
    }
    light_mot_choice = (char)rnd - ((byte)rnd_adj & 0xfc);
  }
  light_mot_paths_block = mot_paths_bind(0,2,light_mot_pairs[light_mot_choice]);
}

void light_mot_path_eval(uchar slot, float frame)
{
  mot_path_eval(&mot_path_slots[slot],frame);
}

void fish_mot_path_bind(void)
{
  mot_paths_block = mot_paths_bind(0,1,fish_mot_list);
  return;
}

void fish_mot_path_eval(uchar slot, float frame)
{
  mot_path_eval(&mot_path_slots[slot],frame);
}

void mot_paths_free(void)
{
  effect_free(mot_paths_block);
  mot_paths_block = 0;
  return;
}

void light_mot_paths_free(void)
{
  effect_free(light_mot_paths_block);
  light_mot_paths_block = 0;
  return;
}
