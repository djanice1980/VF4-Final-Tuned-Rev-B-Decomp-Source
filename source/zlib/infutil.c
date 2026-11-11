/*
 * zlib/infutil.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int inflate_flush(inflate_blocks_state *s, z_streamp z, int r)
{
  uInt n;
  Bytef *p;
  Bytef *q;

  p = z->next_out;
  q = s->read;

  n = (uInt)((q <= s->write ? s->write : s->end) - q);
  if (n > z->avail_out) n = z->avail_out;
  if (n && r == -5) r = 0;

  z->avail_out -= n;
  z->total_out += n;

  if (s->checkfn != 0)
    z->adler = s->check = (*s->checkfn)(s->check, q, n);

  memcpy(p, q, n);
  p += n;
  q += n;

  if (q == s->end)
  {
    q = s->window;
    if (s->write == s->end)
      s->write = s->window;

    n = (uInt)(s->write - q);
    if (n > z->avail_out) n = z->avail_out;
    if (n && r == -5) r = 0;

    z->avail_out -= n;
    z->total_out += n;

    if (s->checkfn != 0)
      z->adler = s->check = (*s->checkfn)(s->check, q, n);

    memcpy(p, q, n);
    p += n;
    q += n;
  }

  z->next_out = p;
  s->read = q;

  return r;
}
