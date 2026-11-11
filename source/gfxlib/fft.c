/*
 * gfxlib/fft.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float fft8_inverse_scale; /* 0x0c32a470 */
extern float fft16_inverse_scale; /* 0x0c32a474 */
extern float fft32_inverse_scale; /* 0x0c32a478 */
extern float fft_twiddles_fwd[64]; /* 0x0c5d45f0 */
extern float fft_twiddles_inv[64]; /* 0x0c5d46f0 */

int bitrev_counter_step(int index, int half)
{
  while (half <= index) {
    index -= half;
    half >>= 1;
  }
  index += half;
  return index;
}

void fft8_twiddles_build(void)
{
  typedef struct { float c; float s; } ent;
  int j;
  uint i;
  float c;
  float s;
  float k = 0.7853982;
  uint n = 4;
  
  j = -2;
  for (i = 0; i < n; i++) {
    j = bitrev_counter_step(j,2);
    c = cos_radians(k * (float)i);
    s = sin_radians(k * (float)i);
    {
      ent *t = (ent *)fft_twiddles_fwd;
      t[j].c = c;
      t[j].s = s;
    }
    ((ent *)fft_twiddles_inv)[j].c = c;
    ((ent *)fft_twiddles_inv)[j].s = -s;
  }
  return;
}

void fft16_twiddles_build(void)
{
  typedef struct { float c; float s; } cs;
  int k;
  uint i;
  uint n = 8;
  float c;
  float s;
  float step = 0.3926991;
  k = -4;
  for (i = 0; i < n; i++) {
    cs *a;
    k = bitrev_counter_step(k, 4);
    c = cos_radians(step * i);
    s = sin_radians(step * i);
    a = (cs *)fft_twiddles_fwd;
    a[k].c = c;
    a[k].s = s;
    ((cs *)fft_twiddles_inv)[k].c = c;
    ((cs *)fft_twiddles_inv)[k].s = -s;
  }
}

void fft32_twiddles_build(void)
{
  float k = 0.19634955f;
  int j = -8;
  uint i = 0;
  uint n;
  do {
    float c;
    float s;
    float *p;
    float *q;
    char *t;
    j = bitrev_counter_step(j, 8);
    c = cos_radians(k * (float)i);
    s = sin_radians(k * (float)i);
    t = (char *)fft_twiddles_fwd;
    {
      int o = j * 8;
      p = (float *)(o + t);
      p[0] = c;
      p[1] = s;
      q = (float *)(o + (char *)fft_twiddles_inv);
      q[0] = c;
      q[1] = -s;
    }
    i++;
  } while (n = 16, i < n);
}

void fft8_bitrev_permute(float *data)
{
  float *p;
  float a;
  float b;
  float c;
  float d;
  p = data;
  a = p[2];
  b = p[3];
  c = p[8];
  d = p[9];
  p[8] = a;
  p[9] = b;
  p[2] = c;
  p[3] = d;
  a = p[6];
  b = p[7];
  c = p[12];
  d = p[13];
  p[12] = a;
  p[13] = b;
  p[6] = c;
  p[7] = d;
}

void fft16_bitrev_permute(float *p)
{
  float a;
  float b;
  float c;
  float d;
  a = p[2]; b = p[3]; c = p[16]; d = p[17];
  p[16] = a; p[17] = b; p[2] = c; p[3] = d;
  a = p[4]; b = p[5]; c = p[8]; d = p[9];
  p[8] = a; p[9] = b; p[4] = c; p[5] = d;
  a = p[6]; b = p[7]; c = p[24]; d = p[25];
  p[24] = a; p[25] = b; p[6] = c; p[7] = d;
  a = p[10]; b = p[11]; c = p[20]; d = p[21];
  p[20] = a; p[21] = b; p[10] = c; p[11] = d;
  a = p[14]; b = p[15]; c = p[28]; d = p[29];
  p[28] = a; p[29] = b; p[14] = c; p[15] = d;
  a = p[22]; b = p[23]; c = p[26]; d = p[27];
  p[26] = a; p[27] = b; p[22] = c; p[23] = d;
}

void fft32_bitrev_permute(float *data)
{
  float *h = data + 32;
  float *f = data;
  float a0;
  float a1;
  float b0;
  float b1;
  {
    float c0 = f[2];
    float c1 = f[3];
    float d0 = h[0];
    float d1 = h[1];
    h[0] = c0;
    h[1] = c1;
    f[2] = d0;
    f[3] = d1;
  }
  a0 = f[4];
  a1 = f[5];
  b0 = f[16];
  b1 = f[17];
  f[16] = a0;
  f[17] = a1;
  f[4] = b0;
  f[5] = b1;
  a0 = f[6];
  a1 = f[7];
  b0 = h[16];
  b1 = h[17];
  h[16] = a0;
  h[17] = a1;
  f[6] = b0;
  f[7] = b1;
  a0 = f[10];
  a1 = f[11];
  b0 = h[8];
  b1 = h[9];
  h[8] = a0;
  h[9] = a1;
  f[10] = b0;
  f[11] = b1;
  a0 = f[12];
  a1 = f[13];
  b0 = f[24];
  b1 = f[25];
  f[24] = a0;
  f[25] = a1;
  f[12] = b0;
  f[13] = b1;
  a0 = f[14];
  a1 = f[15];
  b0 = h[24];
  b1 = h[25];
  h[24] = a0;
  h[25] = a1;
  f[14] = b0;
  f[15] = b1;
  a0 = f[18];
  a1 = f[19];
  b0 = h[4];
  b1 = h[5];
  h[4] = a0;
  h[5] = a1;
  f[18] = b0;
  f[19] = b1;
  a0 = f[22];
  a1 = f[23];
  b0 = h[20];
  b1 = h[21];
  h[20] = a0;
  h[21] = a1;
  f[22] = b0;
  f[23] = b1;
  a0 = f[26];
  a1 = f[27];
  b0 = h[12];
  b1 = h[13];
  h[12] = a0;
  h[13] = a1;
  f[26] = b0;
  f[27] = b1;
  a0 = f[30];
  a1 = f[31];
  b0 = h[28];
  b1 = h[29];
  h[28] = a0;
  h[29] = a1;
  f[30] = b0;
  f[31] = b1;
  a0 = h[6];
  a1 = h[7];
  b0 = h[18];
  b1 = h[19];
  h[18] = a0;
  h[19] = a1;
  h[6] = b0;
  h[7] = b1;
  a0 = h[14];
  a1 = h[15];
  b0 = h[26];
  b1 = h[27];
  h[26] = a0;
  h[27] = a1;
  h[14] = b0;
  h[15] = b1;
}

void fft_bitrev_permute(float *data, int n)
{
  float *a = data;
  int m;
  int nh;
  int nh1;
  int nq;
  int j;
  int k;
  float xr;
  float xi;
  float yr;
  float yi;
  m = n >> 1;
  nh = m;
  nh1 = m + 1;
  nq = m >> 1;
  j = 0;
  for (n = 0; n < (m - 1); n += 2)
  {
    xr = a[(n * 2) + 2];
    xi = a[(n * 2) + 3];
    yr = a[(j | nh) * 2];
    yi = a[((j | nh) * 2) + 1];
    a[(j | nh) * 2] = xr;
    a[((j | nh) * 2) + 1] = xi;
    a[(n * 2) + 2] = yr;
    a[(n * 2) + 3] = yi;
    if (n < j)
    {
      float sr;
      float si;
      float tr;
      float ti;
      sr = a[n * 2];
      si = a[(n * 2) + 1];
      tr = a[j * 2];
      ti = a[(j * 2) + 1];
      a[j * 2] = sr;
      a[(j * 2) + 1] = si;
      a[n * 2] = tr;
      a[(n * 2) + 1] = ti;
      sr = a[(n | nh1) * 2];
      si = a[((n | nh1) * 2) + 1];
      tr = a[(j | nh1) * 2];
      ti = a[((j | nh1) * 2) + 1];
      a[(j | nh1) * 2] = sr;
      a[((j | nh1) * 2) + 1] = si;
      a[(n | nh1) * 2] = tr;
      a[((n | nh1) * 2) + 1] = ti;
      nq++;
      nq--;
    }
    for (k = nq; k <= j; k >>= 1)
    {
      j -= k;
    }

    j += k;
  }

}

void fft_pass_spans_1_2(float *data, uint log2n, float *twiddles)
{
  float *x;
  float *w;
  float *w2;
  float *q0, *q1, *q2, *q3;
  int n, m;
  float ar, ai, br, bi, cr, ci;
  float tr, ur, ui, vi, wr, wi;

  m = (1 << log2n) >> 1;
  w = twiddles;
  w2 = w;
  n = m >> 1;
  x = data;
  ar = *x++;
  ai = *x++;
  q0 = x;
  do {
    br = *x++;
    bi = *x++;
    q1 = x;
    wr = *w2++;
    wi = *w2++;
    tr = ar - br;
    ar = ar + br;
    br = ai - bi;
    ai = ai + bi;
    ui = -wi * tr + br * wr;
    tr = tr * wr + br * wi;
    cr = *x++;
    ci = *x++;
    q2 = x;
    br = *x++;
    bi = *x++;
    q3 = x;
    wr = *w2++;
    wi = *w2++;
    ur = cr - br;
    cr = cr + br;
    br = ci - bi;
    ci = ci + bi;
    vi = -wi * ur + br * wr;
    ur = ur * wr + br * wi;
    wr = *w++;
    wi = *w++;
    br = ar - cr;
    ar = ar + cr;
    cr = ai - ci;
    ai = ai + ci;
    bi = -wi * br + cr * wr;
    br = br * wr + cr * wi;
    *--q0 = ai;
    *--q0 = ar;
    *--q2 = bi;
    *--q2 = br;
    br = tr - ur;
    tr = tr + ur;
    ur = ui - vi;
    ui = ui + vi;
    bi = -wi * br + ur * wr;
    br = br * wr + ur * wi;
    ar = *x++;
    ai = *x++;
    q0 = x;
    *--q1 = ui;
    *--q1 = tr;
    *--q3 = bi;
    *--q3 = br;
  } while (--n != 0);
}

void fft_passes_middle(float *data, uint log2n, float *twiddles)
{
  int n;
  int m;
  int half;
  int k;
  int j;
  float *tw;
  float *a;
  float *b;
  float *pb;
  float *pa;
  float wr, wi, ar, ai, br, bi, dr, di, z, t1, t2;
  n = 1 << log2n;
  m = n >> 3;
  half = 4;
  log2n -= 4;
  do {
    z = 0.0;
    tw = twiddles;
    a = data;
    b = data + half * 2;
    k = m;
    do {
      wr = *tw++;
      wi = *tw++;
      ar = *a++;
      ai = *a++;
      pa = a;
      br = *b++;
      bi = *b++;
      z -= z;
      z -= wi;
      j = half;
      do {
        pb = b;
        dr = ar - br;
        di = ai - bi;
        ar += br;
        ai += bi;
        *--pa = ai;
        *--pa = ar;
        ar = *a++;
        ai = *a++;
        pa = a;
        br = *b++;
        bi = *b++;
        t1 = z * dr + di * wr;
        t2 = wr * dr + di * wi;
        *--pb = t1;
        *--pb = t2;
      } while (--j != 0);
      a += half * 2 - 2;
      b += half * 2 - 2;
    } while (--k != 0);
    m >>= 1;
    half *= 2;
  } while (--log2n != 0);
}

void fft_pass_quarter_fwd(float *data, uint log2n)
{
  short q;
  short k;
  float *r;
  float *w;
  float *v;
  float x0, y0, x1, y1, tx, ty;

  q = 1 << log2n;
  q >>= 2;
  r = data + q * 2;
  k = q;
  do {
    x0 = *data++;
    y0 = *data++;
    w = data;
    x1 = *r++;
    y1 = *r++;
    v = r;
    tx = x0 - x1;
    ty = y0 - y1;
    x0 = x0 + x1;
    y0 = y0 + y1;
    *--w = y0;
    *--w = x0;
    *--v = ty;
    *--v = tx;
  } while (--k != 0);
  data = r;
  r = r + q * 2;
  k = q;
  do {
    x0 = *data++;
    y0 = *data++;
    w = data;
    x1 = *r++;
    y1 = *r++;
    v = r;
    ty = x1 - x0;
    tx = y0 - y1;
    x0 = x0 + x1;
    y0 = y0 + y1;
    *--w = y0;
    *--w = x0;
    *--v = ty;
    *--v = tx;
  } while (--k != 0);
}

void fft_pass_quarter_inv(float *data, uint log2n)
{
  short n4 = 1 << log2n;
  n4 >>= 2;
  float *p = data;
  float *q = p + n4 * 2;
  short i;
  float ar, ai, br, bi, tr, ti;
  float *wp, *wq;
  i = n4;
  do {
    ar = *p++; ai = *p++; wp = p; br = *q++; bi = *q++; wq = q;
    tr = ar - br; ti = ai - bi;
    ar += br; ai += bi;
    *--wp = ai; *--wp = ar; *--wq = ti; *--wq = tr;
  } while (--i != 0);
  p = q;
  q = p + n4 * 2;
  i = n4;
  do {
    ar = *p++; ai = *p++; wp = p; br = *q++; bi = *q++; wq = q;
    ti = ar - br; tr = bi - ai;
    ar += br; ai += bi;
    *--wp = ai; *--wp = ar; *--wq = ti; *--wq = tr;
  } while (--i != 0);
}

void fft_pass_half(float *data, uint log2n)
{
  uint n;
  uint h;
  float *p;
  float *q;
  float *p0, *q0, *p1, *q1;
  float ar, ai, br, bi, cr, ci, dr, di, xr, xi, yr, yi;
  n = 1 << log2n;
  h = n >> 1;
  p = data;
  q = data + h * 2;
  h = n >> 2;
  ar = *p++; ai = *p++; p0 = p; br = *q++; bi = *q++; q0 = q;
  do {
    xr = ar - br; xi = ai - bi; ar = ar + br; ai = ai + bi;
    cr = *p++; ci = *p++; p1 = p; dr = *q++; di = *q++; q1 = q;
    *--q0 = xi; *--q0 = xr; *--p0 = ai; *--p0 = ar;
    yr = cr - dr; yi = ci - di; cr = cr + dr; ci = ci + di;
    ar = *p++; ai = *p++; p0 = p; br = *q++; bi = *q++; q0 = q;
    *--q1 = yi; *--q1 = yr; *--p1 = ci; *--p1 = cr;
  } while (--h);
}

void fft_pass_half_scaled(float *data, uint log2n, float *scale)
{
  float s;
  uint n;
  uint cnt;
  float *p;
  float *q;
  float *pw;
  float *qw;
  float *pw2;
  float *qw2;
  float ar, ai, br, bi;
  float t1, t2, t3, t4;
  float cr, ci;

  s = *scale;
  n = 1 << log2n;
  cnt = n >> 1;
  q = data + cnt * 2;
  p = data;
  ar = *p++;
  ai = *p++;
  pw = p;
  br = *q++;
  bi = *q++;
  qw = q;
  cnt = n >> 2;
  do {
    t1 = ar - br;
    t2 = ai - bi;
    ar = ar + br;
    ai = ai + bi;
    t1 = t1 * s;
    t2 = t2 * s;
    ar = ar * s;
    ai = ai * s;
    cr = *p++;
    ci = *p++;
    pw2 = p;
    br = *q++;
    bi = *q++;
    qw2 = q;
    *--qw = t2;
    *--qw = t1;
    *--pw = ai;
    *--pw = ar;
    t3 = cr - br;
    t4 = ci - bi;
    cr = cr + br;
    ci = ci + bi;
    t3 = t3 * s;
    t4 = t4 * s;
    cr = cr * s;
    ci = ci * s;
    ar = *p++;
    ai = *p++;
    pw = p;
    br = *q++;
    bi = *q++;
    qw = q;
    *--qw2 = t4;
    *--qw2 = t3;
    *--pw2 = ci;
    *--pw2 = cr;
  } while (--cnt != 0);
}

void fft8_forward(float *data)
{
  fft8_bitrev_permute(data);
  fft_pass_spans_1_2(data,3,fft_twiddles_fwd);
  fft_pass_half(data,3);
  return;
}

void fft8_inverse(float *data)
{
  fft8_bitrev_permute(data);
  fft_pass_spans_1_2(data,3,fft_twiddles_inv);
  fft_pass_half_scaled(data,3,&fft8_inverse_scale);
  return;
}

void fft16_forward(float *data)
{
  fft16_bitrev_permute(data);
  fft_pass_spans_1_2(data,4,fft_twiddles_fwd);
  fft_pass_quarter_fwd(data,4);
  fft_pass_half(data,4);
  return;
}

void fft16_inverse(float *data)
{
  fft16_bitrev_permute(data);
  fft_pass_spans_1_2(data,4,fft_twiddles_inv);
  fft_pass_quarter_inv(data,4);
  fft_pass_half_scaled(data,4,&fft16_inverse_scale);
  return;
}

void fft32_forward(float *data)
{
  fft32_bitrev_permute(data);
  fft_pass_spans_1_2(data,5,fft_twiddles_fwd);
  fft_passes_middle(data,5,fft_twiddles_fwd);
  fft_pass_quarter_fwd(data,5);
  fft_pass_half(data,5);
  return;
}

void fft32_inverse(float *data)
{
  fft32_bitrev_permute(data);
  fft_pass_spans_1_2(data,5,fft_twiddles_inv);
  fft_passes_middle(data,5,fft_twiddles_inv);
  fft_pass_quarter_inv(data,5);
  fft_pass_half_scaled(data,5,&fft32_inverse_scale);
  return;
}

void fft_forward(float *data, uint log2n)
{
  fft_bitrev_permute(data,1 << log2n);
  fft_pass_spans_1_2(data,log2n,fft_twiddles_fwd);
  fft_passes_middle(data,log2n,fft_twiddles_fwd);
  fft_pass_quarter_fwd(data,log2n);
  fft_pass_half(data,log2n);
  return;
}

void fft_inverse(float *data, uint log2n)
{
  uint n;
  float f;
  n = 1 << log2n;
  f = 1.0 / (float)n;
  fft_bitrev_permute(data,n);
  fft_pass_spans_1_2(data,log2n,fft_twiddles_inv);
  fft_passes_middle(data,log2n,fft_twiddles_inv);
  fft_pass_quarter_inv(data,log2n);
  fft_pass_half_scaled(data,log2n,&f);
}
