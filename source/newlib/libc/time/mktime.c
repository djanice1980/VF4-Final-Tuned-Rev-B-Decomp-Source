/*
 * newlib/libc/time/mktime.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void validate_structure(tm *tp)
{
  typedef struct { uint sec, min, hour; int day, mon, year; } tm_t;
  tm_t *t = (tm_t *)tp;
  div_t d;
  int feb = 28;
  int md;

  if (t->sec > 59) {
    d = div(t->sec, 60);
    t->min += d.quot;
    t->sec = d.rem;
    if (d.rem < 0) {
      t->sec = d.rem + 60;
      t->min -= 1;
    }
  }
  if (t->min > 59) {
    d = div(t->min, 60);
    t->hour += d.quot;
    t->min = d.rem;
    if (d.rem < 0) {
      t->min = d.rem + 60;
      t->hour -= 1;
    }
  }
  if (t->hour > 23) {
    d = div(t->hour, 24);
    t->day += d.quot;
    t->hour = d.rem;
    if (d.rem < 0) {
      t->hour = d.rem + 24;
      t->day -= 1;
    }
  }
  if (t->mon > 11) {
    d = div(t->mon, 12);
    t->year += d.quot;
    t->mon = d.rem;
    if (d.rem < 0) {
      t->mon = d.rem + 12;
      t->year -= 1;
    }
  }
  if ((t->year & 3) == 0 && (t->year % 100 != 0 || (t->year + 1900) % 400 == 0))
    feb = 29;
  if (t->day <= 0) {
    do {
      if (--t->mon == -1) {
        t->year--;
        t->mon = 11;
        if ((t->year & 3) == 0 && (t->year % 100 != 0 || (t->year + 1900) % 400 == 0))
          feb = 29;
        else
          feb = 28;
      }
      t->day += (t->mon == 1) ? feb : month_days[t->mon];
    } while (t->day <= 0);
  } else {
    while (t->day > ((t->mon == 1) ? feb : month_days[t->mon])) {
      t->day -= (t->mon == 1) ? feb : month_days[t->mon];
      if (++t->mon == 12) {
        t->year++;
        t->mon = 0;
        if ((t->year & 3) == 0 && (t->year % 100 != 0 || (t->year + 1900) % 400 == 0))
          feb = 29;
        else
          feb = 28;
      }
    }
  }
}

int mktime(tm *t)
{
  int tim = 0;
  int days = 0;
  int year;

  validate_structure(t);

  tim += t->tm_sec + (t->tm_min * 60) + (t->tm_hour * 3600);

  days += t->tm_mday - 1;
  days += days_before_month[t->tm_mon];
  if (t->tm_mon > 1 && ((t->tm_year % 4) == 0 && ((t->tm_year % 100) != 0 || ((t->tm_year + 1900) % 400) == 0) ? 366 : 365) == 366)
    days++;

  t->tm_yday = days;

  if (t->tm_year > 10000 || t->tm_year < -10000)
    {
      return -1;
    }

  if (t->tm_year > 70)
    {
      for (year = 70; year < t->tm_year; year++)
        days += ((year % 4) == 0 && ((year % 100) != 0 || ((year + 1900) % 400) == 0) ? 366 : 365);
    }
  else if (t->tm_year < 70)
    {
      for (year = 69; year > t->tm_year; year--)
        days -= ((year % 4) == 0 && ((year % 100) != 0 || ((year + 1900) % 400) == 0) ? 366 : 365);
      days -= ((year % 4) == 0 && ((year % 100) != 0 || ((year + 1900) % 400) == 0) ? 366 : 365);
    }

  if ((t->tm_wday = (days + 4) % 7) < 0)
    t->tm_wday += 7;

  tim += (days * 86400);

  return tim;
}
