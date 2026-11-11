/*
 * newlib/libm/math/kf_rem_pio2.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int __kernel_rem_pio2f(float *x, float *y, int e0, int nx, int prec, const int *ipio2)
{
  int jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
  float z, fw, f[20], fq[20], q[20];

  jk = rem_pio2_terms[prec];
  jp = jk;

  jx = nx - 1;
  jv = (e0 - 3) / 8; if (jv < 0) jv = 0;
  q0 = e0 - 8 * (jv + 1);

  j = jv - jx; m = jx + jk;
  for (i = 0; i <= m; i++, j++) f[i] = (j < 0) ? 0.0 : (float) ipio2[j];

  for (i = 0; i <= jk; i++) {
    for (j = 0, fw = 0.0; j <= jx; j++) fw += x[j] * f[jx + i - j]; q[i] = fw;
  }

  jz = jk;
recompute:
  for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
    fw = (float)((int)(3.9062500000e-03 * z));
    iq[i] = (int)(z - 2.5600000000e+02 * fw);
    z = q[j - 1] + fw;
  }

  z = scalbnf(z, (int)q0);
  z -= (float)8.0 * floorf(z * (float)0.125);
  n = (int) z;
  z -= (float)n;
  ih = 0;
  if (q0 > 0) {
    i = (iq[jz - 1] >> (8 - q0)); n += i;
    iq[jz - 1] -= i << (8 - q0);
    ih = iq[jz - 1] >> (7 - q0);
  }
  else if (q0 == 0) ih = iq[jz - 1] >> 8;
  else if (z >= (float)0.5) ih = 2;

  if (ih > 0) {
    n += 1; carry = 0;
    for (i = 0; i < jz; i++) {
      j = iq[i];
      if (carry == 0) {
        if (j != 0) {
          carry = 1; iq[i] = 0x100 - j;
        }
      } else iq[i] = 0xff - j;
    }
    if (q0 > 0) {
      switch (q0) {
      case 1:
        iq[jz - 1] &= 0x7f; break;
      case 2:
        iq[jz - 1] &= 0x3f; break;
      }
    }
    if (ih == 2) {
      z = 1.0 - z;
      if (carry != 0) z -= scalbnf(1.0, (int)q0);
    }
  }

  if (z == 0.0) {
    j = 0;
    for (i = jz - 1; i >= jk; i--) j |= iq[i];
    if (j == 0) {
      for (k = 1; iq[jk - k] == 0; k++);

      for (i = jz + 1; i <= jz + k; i++) {
        f[jx + i] = (float) ipio2[jv + i];
        for (j = 0, fw = 0.0; j <= jx; j++) fw += x[j] * f[jx + i - j];
        q[i] = fw;
      }
      jz += k;
      goto recompute;
    }
  }

  if (z == (float)0.0) {
    jz -= 1; q0 -= 8;
    while (iq[jz] == 0) { jz--; q0 -= 8; }
  } else {
    z = scalbnf(z, -(int)q0);
    if (z >= 2.5600000000e+02) {
      fw = (float)((int)(3.9062500000e-03 * z));
      iq[jz] = (int)(z - 2.5600000000e+02 * fw);
      jz += 1; q0 += 8;
      iq[jz] = (int) fw;
    } else iq[jz] = (int) z;
  }

  fw = scalbnf(1.0, (int)q0);
  for (i = jz; i >= 0; i--) {
    q[i] = fw * (float)iq[i]; fw *= 3.9062500000e-03;
  }

  for (i = jz; i >= 0; i--) {
    for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++) fw += half_pi_pieces[k] * q[i + k];
    fq[jz - i] = fw;
  }

  switch (prec) {
    case 0:
      fw = 0.0;
      for (i = jz; i >= 0; i--) fw += fq[i];
      y[0] = (ih == 0) ? fw : -fw;
      break;
    case 1:
    case 2:
      fw = 0.0;
      for (i = jz; i >= 0; i--) fw += fq[i];
      y[0] = (ih == 0) ? fw : -fw;
      fw = fq[0] - fw;
      for (i = 1; i <= jz; i++) fw += fq[i];
      y[1] = (ih == 0) ? fw : -fw;
      break;
    case 3:
      for (i = jz; i > 0; i--) {
        fw = fq[i - 1] + fq[i];
        fq[i] += fq[i - 1] - fw;
        fq[i - 1] = fw;
      }
      for (i = jz; i > 1; i--) {
        fw = fq[i - 1] + fq[i];
        fq[i] += fq[i - 1] - fw;
        fq[i - 1] = fw;
      }
      for (fw = 0.0, i = jz; i >= 2; i--) fw += fq[i];
      if (ih == 0) {
        y[0] = fq[0]; y[1] = fq[1]; y[2] = fw;
      } else {
        y[0] = -fq[0]; y[1] = -fq[1]; y[2] = -fw;
      }
  }
  return n & 7;
}
