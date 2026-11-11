/*
 * sdk/kamui/cfg_lookup.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * test.bin's copy of main.bin's source/sdk/kamui/cfg_lookup.c: the same C
 * as main.bin's cfg_setting_or_default (0x0C147480).
 */
#include "vf4ft.h"

int cfg_setting_or_default(void *section, void *key, int def)
{
  int ret;

  if (memcmp(section, "HARDWARE", 9) == 0) {
    if (memcmp(key, "EnableTexture16MB", 0x12) == 0) {
      int *p;
      int *end;
      int w1, w2, w3;
      int next;

      ret = 1;
      p = (int *)0xa5800000;
      next = p[4]; /* ORIGINAL SHAPE (assumed from the match; revisit if more turns up): the game's code has no test here, but its unmerged pointer steps and the unused r2 need a fourth word read, an end pointer and two tests that set ret to the 1 it already holds (GCC deletes them after allocation); read as a probe whose default became 1. The exact wording is not in the bytes: many nearby forms match */
      if (next) ret = 1;
      w1 = *(volatile int *)++p;
      w2 = *(volatile int *)++p;
      w3 = *(volatile int *)++p;
      p -= 3;
      end = p + 3;
      *++p = w1;
      *++p = w2;
      *++p = w3;
      if (p == end && next) ret = 1;
    }
  }
  else {
    ret = def;
  }
  return ret;
}
