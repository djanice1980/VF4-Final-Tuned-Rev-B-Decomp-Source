/*
 * vf4lib/libbase/src/chara_data.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char play_style; /* 0x0c2925cc */
extern char sel_cursor[2]; /* 0x0c3778dc */

uchar ** chara_data_get(int chara)
{
  if ((chara == 10) &&
     ((sel_cursor[0] == '\x13' || (sel_cursor[1] == '\x13')))) {
    return chara_data_table[20];
  }
  if (chara == 0xd && play_style == '\x01' && msnm_active() == 0 && dural_alt_active != '\0') {
    return chara_data_table[21];
  }
  return chara_data_table[chara];
}

void empty_fn_0c043700(void)
{
  return;
}
