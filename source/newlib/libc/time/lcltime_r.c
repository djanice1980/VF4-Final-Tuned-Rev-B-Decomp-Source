/*
 * newlib/libc/time/lcltime_r.c
 * Written from the decompiled output of the game's bytes (it was an
 * assembly listing, source/asm/0c159300.s, until 2026-11-12); compiles to
 * them with the tree's flags (python3 -I tools/match/match.py checks it).
 */
#include "vf4ft.h"

#define LEAP(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

tm *localtime_r(int *timep, tm *res)
{
  long days, rem;
  int y;
  int yleap;
  int *ip;

  days = *timep / 86400;
  rem = *timep % 86400;
  while (rem < 0) {
    rem += 86400;
    --days;
  }
  while (rem >= 86400) {
    rem -= 86400;
    ++days;
  }

  res->tm_hour = rem / 3600;
  rem %= 3600;
  res->tm_min = rem / 60;
  res->tm_sec = rem % 60;

  if ((res->tm_wday = (4 + days) % 7) < 0)
    res->tm_wday += 7;

  y = 1970;
  if (days >= 0) {
    for (;;) {
      yleap = LEAP(y);
      if (days < year_lengths[yleap])
        break;
      y++;
      days -= year_lengths[yleap];
    }
  }
  else {
    do {
      --y;
      yleap = LEAP(y);
      days += year_lengths[yleap];
    } while (days < 0);
  }

  res->tm_year = y - 1900;
  res->tm_yday = days;
  ip = mon_lengths[yleap];
  for (res->tm_mon = 0; days >= ip[res->tm_mon]; ++res->tm_mon)
    days -= ip[res->tm_mon];
  res->tm_mday = days + 1;
  res->tm_isdst = -1;
  return res;
}
