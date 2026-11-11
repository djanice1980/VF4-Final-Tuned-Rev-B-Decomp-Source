/*
 * sdk/naomi_library/version.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void pal_raw_mode_set(char mode)
{
  pal_raw_mode = mode;
  return;
}

char * naomi_lib_build_date_get(void)
{
  return "Apr  5 2004 17:11:03";
}

void clip_stretch_step_set(int step)
{
  clip_stretch_step = step;
  return;
}

void clip_stretch_step_report(uint *report)
{
  report[1] = clip_stretch_step;
  return;
}
