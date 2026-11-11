/*
 * zlib/inftrees.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int inflate_trees_fixed(uIntf *bl, uIntf *bd, inflate_huft **tl, inflate_huft **td, z_streamp z)
{
  *bl = fixed_bl;
  *bd = fixed_bd;
  *tl = fixed_tl;
  *td = fixed_td;
  return 0;
}

int inflate_trees_bits(uIntf *c, uIntf *bb, inflate_huft **tb, inflate_huft *hp, z_streamp z)
{
  int r;
  uInt hn = 0;
  uIntf *v;

  if ((v = (uIntf *)(*z->zalloc)(z->opaque,19,sizeof(uInt))) == 0)
    return -4;
  r = huft_build(c, 19, 19, (uIntf*)0, (uIntf*)0, tb, bb, hp, &hn, v);
  if (r == -3)
    z->msg = (char*)"oversubscribed dynamic bit lengths tree";
  else if (r == -5 || *bb == 0)
  {
    z->msg = (char*)"incomplete dynamic bit lengths tree";
    r = -3;
  }
  (*z->zfree)(z->opaque, v);
  return r;
}

int inflate_trees_dynamic(uInt nl, uInt nd, uIntf *c, uIntf *bl, uIntf *bd, inflate_huft **tl, inflate_huft **td, inflate_huft *hp, z_streamp z)
{
  int r;
  uInt hn = 0;
  uIntf *v;

  if ((v = (uIntf *)(*z->zalloc)(z->opaque, 288, sizeof(uInt))) == 0)
    return -4;
  r = huft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
  if (r != 0 || *bl == 0)
  {
    if (r == -3)
      z->msg = (char*)"oversubscribed literal/length tree";
    else if (r != -4)
    {
      z->msg = (char*)"incomplete literal/length tree";
      r = -3;
    }
    (*z->zfree)(z->opaque, v);
    return r;
  }
  r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
  if (r != 0 || (*bd == 0 && nl > 257))
  {
    if (r == -3)
      z->msg = (char*)"oversubscribed distance tree";
    else if (r == -5) {
      z->msg = (char*)"incomplete distance tree";
      r = -3;
    }
    else if (r != -4)
    {
      z->msg = (char*)"empty distance tree with lengths";
      r = -3;
    }
    (*z->zfree)(z->opaque, v);
    return r;
  }
  (*z->zfree)(z->opaque, v);
  return 0;
}

int huft_build(uIntf *b, uInt n, uInt s, const uIntf *d, const uIntf *e, inflate_huft **t, uIntf *m, inflate_huft *hp, uInt *hn, uIntf *v)
{
  uInt a;
  uInt c[16];
  uInt f;
  int g;
  int h;
  register uInt i;
  register uInt j;
  register int k;
  int l;
  uInt mask;
  register uIntf *p;
  inflate_huft *q;
  struct inflate_huft_s r;
  inflate_huft *u[15];
  register int w;
  uInt x[16];
  uIntf *xp;
  int y;
  uInt z;

  p = c;
  *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 0;
  *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 0;
  *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 0;
  *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 0;
  p = b;  i = n;
  do {
    c[*p++]++;
  } while (--i);
  if (c[0] == n)
  {
    *t = (inflate_huft *)0;
    *m = 0;
    return 0;
  }

  l = *m;
  for (j = 1; j <= 15; j++)
    if (c[j])
      break;
  k = j;
  if ((uInt)l < j)
    l = j;
  for (i = 15; i; i--)
    if (c[i])
      break;
  g = i;
  if ((uInt)l > i)
    l = i;
  *m = l;

  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return -3;
  if ((y -= c[i]) < 0)
    return -3;
  c[i] += y;

  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {
    *xp++ = (j += *p++);
  }

  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  n = x[g];

  x[0] = i = 0;
  p = v;
  h = -1;
  w = -l;
  u[0] = (inflate_huft *)0;
  q = (inflate_huft *)0;
  z = 0;

  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      while (k > w + l)
      {
        h++;
        w += l;

        z = g - w;
        z = z > (uInt)l ? l : z;
        if ((f = 1 << (j = k - w)) > a + 1)
        {
          f -= a + 1;
          xp = c + k;
          if (j < z)
            while (++j < z)
            {
              if ((f <<= 1) <= *++xp)
                break;
              f -= *xp;
            }
        }
        z = 1 << j;

        if (*hn + z > 1440)
          return -3;
        u[h] = q = hp + *hn;
        *hn += z;

        if (h)
        {
          x[h] = i;
          r.word.what.Bits = (Byte)l;
          r.word.what.Exop = (Byte)j;
          j = i >> (w - l);
          r.base = (uInt)(q - u[h-1] - j);
          u[h-1][j] = r;
        }
        else
          *t = q;
      }

      r.word.what.Bits = (Byte)(k - w);
      if (p >= v + n)
        r.word.what.Exop = 128 + 64;
      else if (*p < s)
      {
        r.word.what.Exop = (Byte)(*p < 256 ? 0 : 32 + 64);
        r.base = *p++;
      }
      else
      {
        r.word.what.Exop = (Byte)(e[*p - s] + 16 + 64);
        r.base = d[*p++ - s];
      }

      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;

      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;

      mask = (1 << w) - 1;
      while ((i & mask) != x[h])
      {
        h--;
        w -= l;
        mask = (1 << w) - 1;
      }
    }
  }

  return y != 0 && g != 1 ? -5 : 0;
}
