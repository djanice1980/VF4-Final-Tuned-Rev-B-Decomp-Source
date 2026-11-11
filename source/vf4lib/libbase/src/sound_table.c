/*
 * vf4lib/libbase/src/sound_table.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined sound_se_table; /* 0x0c298f10 */
extern undefined sound_voice_table; /* 0x0c299000 */
extern undefined sound_music_table; /* 0x0c2990b4 */
extern chara_sound_tables_rec chara_sound_tables; /* 0x0c298dbc */

void sound_tables_install(void)
{
  chara_se_table_set(0,chara_sound_tables.se0);
  chara_se_table_set(1,((struct { char pad[0x10]; short *se1; } *)&chara_sound_tables)->se1);
  chara_voice_table_set(0,chara_sound_tables.voice0);
  chara_voice_table_set(1,chara_sound_tables.voice1);
  stage_sound_record_set(&stage_sound_record_default);
  sound_se_table_copy(&sound_se_table);
  sound_voice_table_copy(&sound_voice_table);
  sound_music_table_copy(&sound_music_table);
  return;
}
