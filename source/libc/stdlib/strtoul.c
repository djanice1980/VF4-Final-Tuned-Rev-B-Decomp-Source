/*
 * libc/stdlib/strtoul.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int lib_errno; /* 0x0c41eb2c */

ulong strtoul(char *str, char **end, int base)
{
  register const char *s = str;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  do {
    c = *s++;
  } while (((ctype_table + 1)[c] & 8) != 0);
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = (unsigned long)0xffffffff / (unsigned long)base;
  cutlim = (unsigned long)0xffffffff % (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (((ctype_table + 1)[c] & 4) != 0)
      c -= '0';
    else if (((ctype_table + 1)[c] & 3) != 0)
      c -= ((ctype_table + 1)[c] & 1) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = 0xffffffff;
    lib_errno = 34;
  } else if (neg)
    acc = -acc;
  if (end != 0)
    *end = (char *) (any ? s - 1 : str);
  return (acc);
}
