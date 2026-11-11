/*
 * sdk/naomi_library/scalar_math.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint random_lcg_state; /* 0x0c32a5b8 */

static inline float fsqrt_inline(float x)
{
  float r;
  __asm__ ("fsqrt %0" : "=f" (r) : "0" (x));
  return r;
}

ushort acos_ang16(float x)
{
  ushort r;

  if (__builtin_fabsf(x) > 1.0) {
    r = (x <= 0.0) << 15;
  }
  else {
    r = 0x4000 - asin_ang16(x);
  }
  return r;
}

uint acsc_ang16(float x)
{
  uint ang;
  
  ang = 0;
  if (x != 0.0) {
    ang = asin_ang16(1.0 / x);
  }
  return ang;
}

uint acot_ang16(float x)
{
  uint ang;
  
  ang = 0;
  if (x != 0.0) {
    ang = atan_ang16(1.0 / x);
  }
  return ang;
}

uint asec_ang16(float x)
{
  uint ang;
  
  ang = 0;
  if (x != 0.0) {
    ang = acos_ang16(1.0 / x);
  }
  return ang;
}

uint asin_ang16(float x)
{
  float c;

  if (__builtin_fabsf(x) > 1.0) {
    if (x > 0.0) return 0x4000;
    return -0x4000;
  }
  c = 1.0 - x * x;
  __asm__ ("fsqrt %0" : "=f" (c) : "0" (c));
  if (c != 0.0) return atan_ang16(x / c);
  if (x > 0.0) return 0x4000;
  return 0xc000;
}

float signed_sqrt(float x)
{
  if (x < 0.0) {
    return -fsqrt_inline(-x);
  }
  return fsqrt_inline(x);
}

uint atan_ang16(float x)
{
  float y;
  float t;
  float fi;
  int i;

  t = 0.0;
  if (__builtin_fabsf(x) <= 1.0) {
    y = x;
  }
  else {
    y = 1.0 / x;
  }
  for (i = 13; i > 0; i--) {
    fi = (float)i;
    t = (fi * y * fi * y) / (t + 1.0 + (float)(i * 2));
  }
  y = y / (t + 1.0);
  i = (int)(((y + 6.2831855) * 65536.0) / 6.2831855 + 0.5) & 0xffff;
  if (x > 1.0) {
    return 0x4000 - i & 0xffff;
  }
  if (x < -1.0) {
    return 0xc000 - i & 0xffff;
  }
  return i;
}

uint atan2_ang16(float y, float x)
{
  if (y <= x) {
    if (-y <= x) {
      if (x == 0.0) {
        return 0;
      }
      return atan_ang16(y / x);
    }
    if (y == 0.0) {
      return 0xc000;
    }
    return 0xc000 - atan_ang16(x / y) & 0xffff;
  }
  else {
    if (-y <= x) {
      if (y == 0.0) {
        return 0x4000;
      }
      return 0x4000 - atan_ang16(x / y) & 0xffff;
    }
    else {
      if (x == 0.0) {
        return 0x8000;
      }
      return atan_ang16(y / x) + 0x8000U & 0xffff;
    }
  }
}

float float_ceil(float x)
{
  float down;
  
  down = (float)float_floor(-x);
  return -down;
}

float cos_ang16(uint angle)
{
  int a;
  
  a = angle & 0xffff;
  if (0x8000 < a) {
    a = 0x10000 - a;
  }
  return sin_ang16(0x4000 - a);
}

float csc_ang16(uint angle)
{
  uint a;
  
  a = angle & 0xffff;
  if (a == 0) return 3.4028235e+38;
  if (a == 0x8000) return 3.4028235e+38;
  return 1.0 / (float)sin_ang16(a);
}

float cos_radians(float angle)
{
  return sin_radians(1.5707964 - __builtin_fabsf(angle));
}

float cosh_ang16(uint angle)
{
  float e;
  e = (float)(ushort)angle * 6.2831855 * 1.5258789e-05;
  e = exp_float(e);
  return (e + 1.0 / e) * 0.5;
}

float cot_ang16(ushort angle)
{
  if (angle == 0) return 3.4028235e+38;
  if (angle == 0x4000) return 0.0;
  if (angle == 0x8000) return 3.4028235e+38;
  if (angle == 0xc000) return 0.0;
  return 1.0f / tan_ang16(angle);
}

float exp_float(float x)
{
  float h;
  float q;
  float r2;
  float c;
  float t;
  int i;
  int n;
  
  q = x / 0.6931472;
  if (!(x < 0.0)) {
    h = 0.5;
  } else {
    h = -0.5;
  }
  n = (int)(h + q);
  x -= (float)n * 0.6931472;
  r2 = x * x;
  c = r2 / 14.0;
  for (i = 10; i > 5; i -= 4) {
    c = r2 / ((float)i + c);
  }
  t = 2.0 + c;
  return float_times_pow2(n, (t + x) / (t - x));
}

float float_floor(float x)
{
  float a;
  int i;
  a = __builtin_fabsf(x);
  if (a >= 8388608.0) return x;
  i = (int)a;
  if (x < 0.0) {
    if (a - (float)i == 0.0) {
      return x;
    }
    return (float)~i;
  }
  return (float)i;
}

float float_fraction(float x)
{
  float a;
  a = __builtin_fabsf(x);
  if (a >= 8388608.0) return 0.0;
  a -= (float)(int)a;
  if (x < 0.0) {
    return -a;
  }
  return a;
}

float rsqrt_signed(float x)
{
  if (x == 0.0) return 3.4028235e+38;
  return 1.0 / signed_sqrt(x);
}

float rsqrt_float(float x)
{
  float s;
  if (x == 0.0) return 3.4028235e+38;
  __asm__ ("fsqrt %0" : "=f" (s) : "0" (x));
  return 1.0 / s;
}

float float_times_pow2(int exponent, float x)
{
  int m;
  int *p;
  float f;
  if (x == 0.0)
  {
    return 0.0;
  }
  f = x;
  p = (int *) (&f);
  m = exponent + (((*p) >> 23) & 0xff);
  if (m <= 0)
  {
    m = (((*p) & 0x7fffff) | 0x800000) >> (1 - m);
    *p = ((*p) & 0x80000000) | m;
  }
  else
    if (m > 0xfe)
  {
    *p = ((*p) & 0x80000000) | 0x7f800000;
  }
  else
  {
    *p = (*p) & 0x807fffff;
    *p = (*p) | (m << 23);
  }
  return f;
}

float log_natural(float x)
{
  int i;
  float r;
  float t;
  float prev;
  
  i = 1;
  r = 0.0;
  if (x < 0.0) return 0.0;
  if (x == 0.0) return 0.0;
  while (x >= 2.0) {
    x = x * 0.5;
    r = r + 0.6931472;
  }
  while (x < 1.0) {
    x = x + x;
    r = r - 0.6931472;
  }
  x = (x - 1.0) / (x + 1.0);
  t = x + x;
  x = x * x;
  do {
    prev = r;
    r = r + t / (float)i;
    t = t * x;
    i = i + 2;
  } while (r != prev);
  return r;
}

float log_base10(float x)
{
  float ln;
  
  ln = (float)log_natural(x);
  return ln / 2.3025851;
}

float log_base2(float x)
{
  float ln;
  
  ln = (float)log_natural(x);
  return ln / 0.6931472;
}

float pow_float(float x, float y)
{
  float r;
  int n;
  n = (int)y;
  if (x == 0.0) {
    if (!(0.0 < y)) return 1.0;
    return 0.0;
  }
  else if (y == (float)n) {
    if (n < 0) {
      n = -n;
      x = 1.0 / x;
    }
    r = 1.0;
    while (1) {
      if (n & 1) r = r * x;
      n = n / 2;
      if (n == 0) return r;
      x = x * x;
    }
  }
  if (x < 0.0) return 0.0;
  return exp_float(y * log_natural(x));
}

uint lcg_next15(void)
{
  random_lcg_state = random_lcg_state * 0x41c64e6d + 0x3039;
  return (random_lcg_state & 0x7fffffff) >> 0x10;
}

float lcg_next_unit(void)
{
  int r;
  
  r = lcg_next15();
  return (float)r * 3.0517578e-05;
}

void lcg_seed_set(uint seed)
{
  random_lcg_state = seed;
  return;
}

float float_trunc(float x)
{
  float a;
  int i;
  a = __builtin_fabsf(x);
  if (a >= 8388608.0) return x;
  i = (int)a;
  if (a - (float)i != 0.0) {
    if (x > 0.0) {
      return (float)i;
    }
    return (float)-i;
  }
  return x;
}

float float_round_away(float x)
{
  float a;
  int i;
  a = __builtin_fabsf(x);
  if (a >= 8388608.0) return x;
  i = (int)a;
  if (a - (float)i != 0.0) {
    return (float)(x > 0.0 ? i + 1 : ~i);
  }
  return x;
}

float sec_ang16(uint angle)
{
  uint a = angle & 0xffff;
  if (a == 0x4000) return 3.4028235e+38;
  if (a == 0xc000) return 3.4028235e+38;
  return 1.0 / cos_ang16(a);
}

float sin_ang16(uint angle)
{
  float x;
  float x2;
  float t;
  int n;
  int i;

  x = (float)(ushort)angle * 6.2831855 * 1.5258789e-05 * 0.5;
  n = (int)(x / 1.5707964 + 0.5);
  x = x - (float)n * 1.5707964;
  x2 = x * x;
  t = 0.0;
  for (i = 11; i > 2; i -= 2) {
    t = x2 / ((float)i - t);
  }
  t = x / (1.0 - t);
  t = (t + t) / (t * t + 1.0);
  if ((n & 1) == 0) return t;
  return -t;
}

float sinh_ang16(uint angle)
{
  float x;
  x = (float)(ushort)angle * 6.2831855 * 1.5258789e-05;
  if (__builtin_fabsf(x) > 0.01) {
    x = exp_float(x);
    return (x - 1.0 / x) * 0.5;
  }
  return x * ((x * x) / 6.0 + 1.0);
}

float sin_radians(float angle)
{
  float t;
  int quadrant;
  
  t = (float)sin_radians_core(&quadrant,angle * 0.5);
  t = (t + t) / (t * t + 1.0);
  if ((quadrant & 1) == 0) return t;
  t = -t;
  return t;
}

float sin_radians_core(int *quadrant, float x)
{
  float f;
  float y;
  float s;
  float t;
  int i;
  
  i = (int)(x / 1.5707964 + (!(x < 0.0) ? 0.5 : -0.5));
  *quadrant = i;
  f = (float)i;
  y = x - f * 1.5708008;
  y = y + f * 4.454455e-06;
  s = y * y;
  t = 0.0;
  for (i = 0xb; i > 2; i -= 2) {
    t = s / ((float)i - t);
  }
  return y / (1.0 - t);
}

float tan_radians(float angle)
{
  float t;
  int quadrant;

  t = tan_radians_core(&quadrant,angle);
  if (quadrant & 1) {
    if (t != 0.0) return -1.0 / t;
    return 3.4028235e+38;
  }
  return t;
}

float tan_radians_core(int *quadrant, float x)
{
  float q;
  float h;
  float fn;
  float r;
  float sq;
  float acc;
  int n;
  int i;
  
  q = x / 1.5707964f;
  if (!(x < 0.0f)) h = 0.5f; else h = -0.5f;
  h += q;
  n = (int)h;
  *quadrant = n;
  fn = (float)n;
  r = (x - fn * 1.5708008f) + fn * 4.454455e-06f;
  sq = r * r;
  acc = 0.0f;
  for (i = 11; i > 2; i -= 2) {
    acc = sq / ((float)i - acc);
  }
  return r / (1.0f - acc);
}

float tan_ang16(uint angle)
{
  int i;
  float x, x2, t;
  uint q;
  angle = (ushort)angle;
  
  if (((ushort)angle & 0x7fff) == 0x4000) return 3.4028235e+38;
  x = (float)(ushort)angle * 6.2831855 * 1.5258789e-05;
  q = (int)(x / 1.5707964 + 0.5);
  x = x - (float)(int)q * 1.5707964;
  x2 = x * x;
  t = 0.0;
  for (i = 13; i > 2; i -= 2) {
    t = x2 / ((float)i - t);
  }
  t = x / (1.0 - t);
  if (q & 1) {
    if (t == 0.0) return 3.4028235e+38;
    return -1.0 / t;
  }
  return t;
}

float tanh_ang16(uint angle)
{
  float x;

  x = (float)(ushort)angle * 6.2831855 * 1.5258789e-05;
  if (x > 0.001) {
    x = exp_float(x * -2.0);
    x = x + 1.0;
    return 2.0 / x - 1.0;
  }
  if (x < 0.001) {
    x = exp_float(x + x);
    x = x + 1.0;
    return 1.0 - 2.0 / x;
  }
  return x * (1.0 - (x * x) / 3.0);
}
