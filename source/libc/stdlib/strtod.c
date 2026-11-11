/*
 * libc/stdlib/strtod.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

float strtod(char *str, char **end)
{
  char *s = str;
  char **se = end;
  char *sorg = str;
  float accum = 0.0;
  int flags = 0;
  int texp = 0;
  int e = 0;
  int conv_done = 0;

  while ((ctype_table + 1)[*s] & 8)
    s++;
  if (*s == '\0') {
    if (se)
      *se = sorg;
    return 0.0;
  }
  if (*s == '-' || *s == '+') {
    if (*s == '-')
      flags = 1;
    if (*++s == '\0') {
      if (se)
        *se = sorg;
      return 0.0;
    }
  }
  while (((ctype_table + 1)[*s] & 4) || *s == '.') {
    conv_done = 1;
    if (*s == '.')
      flags |= 4;
    else {
      if (__ten_mul(&accum, *s - '0'))
        texp++;
      if (flags & 4)
        texp--;
    }
    s++;
  }
  if (*s == 'e' || *s == 'E' || *s == 'd' || *s == 'D') {
    conv_done = 1;
    if (*++s != '\0') {
      while ((ctype_table + 1)[*s] & 8)
        s++;
      if (*s != '\0') {
        if (*s == '-' || *s == '+')
          if (*s++ == '-')
            flags |= 2;
        if (*s != '\0') {
          for (; (ctype_table + 1)[*s] & 4; s++)
            if (e < 38)
              e = e * 10 + *s - '0';
          if (flags & 2)
            texp -= e;
          else
            texp += e;
        }
      }
    }
  }
  if (se)
    *se = !conv_done ? sorg : s;
  return strtod_scale(&accum, texp, flags & 1);
}
