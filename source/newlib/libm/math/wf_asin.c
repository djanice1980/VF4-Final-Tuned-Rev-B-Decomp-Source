/*
 * newlib/libm/math/wf_asin.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int math_error_mode; /* 0x0c32be20 */
extern int lib_errno; /* 0x0c41eb2c */

float asinf(float x)
{
  math_error_rec exc;
  float z;
  z = __ieee754_asinf(x);
  if (math_error_mode == -1 || isnanf(x)) return z;
  if (fabsf(x) > 1.0f) {
    exc.kind = 1;
    exc.func_name = "asinf";
    exc.error_no = 0;
    exc.arg1 = exc.arg2 = x;
    exc.result = 0.0f;
    if (math_error_mode == 2)
      lib_errno = 0x21;
    else if (!matherr(&exc)) {
      lib_errno = 0x21;
    }
    if (exc.error_no != 0)
      lib_errno = exc.error_no;
    return exc.result;
  } else
    return z;
}

void asin(float x)
{
  ((void (*)(float))asinf)(x);
}
