/*
 * vf4lib/libcore/src/tm_sys.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

tm * get_now_tm(tm *t)
{
  int now_time;
  struct { ushort year; byte mon; byte day; byte hour; byte min; byte sec; byte pad[5]; } now;
  
  if (t != (tm *)0x0) {
    clock_to_calendar(&now.year);
    t->tm_wday = 0;
    t->tm_yday = 0;
    t->tm_sec = now.sec;
    t->tm_min = now.min;
    t->tm_hour = now.hour;
    t->tm_mday = now.day;
    t->tm_mon = now.mon - 1;
    t->tm_year = now.year - 0x76c;
    t->tm_isdst = -1;
    now_time = mktime(t);
    if (now_time == -1) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/tm_sys.c",0x21,"assertion `%s\' failed.\n",
               "now_time != -1");
    }
  }
  return t;
}

int now_seconds_get(int *out)
{
  int seconds;
  tm t;
  
  get_now_tm(&t);
  seconds = mktime(&t);
  if (out != (int *)0x0) {
    *out = seconds;
  }
  return seconds;
}
