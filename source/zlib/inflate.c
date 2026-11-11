/*
 * zlib/inflate.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int inflateEnd(z_streamp z)
{
  int err;
  
  if (((z == (z_streamp)0x0) || (z->state == (internal_state *)0x0)) || (z->zfree == (free_func)0x0)
     ) {
    err = -2;
  }
  else {
    if (((inflate_state *)z->state)->blocks != (inflate_blocks_state *)0x0) {
      inflate_blocks_free(((inflate_state *)z->state)->blocks,z);
    }
    (*z->zfree)(z->opaque,z->state);
    z->state = (internal_state *)0x0;
    err = 0;
  }
  return err;
}

int inflateSyncPoint(z_streamp z)
{
  int err;
  
  if (((z == (z_streamp)0x0) || (z->state == (internal_state *)0x0)) ||
     (((inflate_state *)z->state)->blocks == (inflate_blocks_state *)0x0)) {
    err = -2;
  }
  else {
    err = inflate_blocks_sync_point(((inflate_state *)z->state)->blocks);
  }
  return err;
}

int inflateReset(z_streamp z)
{
  if (z == (z_streamp)0x0 || z->state == (internal_state *)0x0)
    return -2;
  z->total_in = z->total_out = 0;
  z->msg = (char *)0x0;
  ((inflate_state *)z->state)->mode = ((inflate_state *)z->state)->nowrap ? BLOCKS : METHOD;
  inflate_blocks_reset(((inflate_state *)z->state)->blocks, z, (uLongf *)0x0);
  return 0;
}

int inflateSync(z_streamp z)
{
  uInt n;
  Bytef *p;
  uInt m;
  uLong r, w;

  if (z == 0 || z->state == 0)
    return -2;
  if (((inflate_state *)z->state)->mode != I_BAD)
  {
    ((inflate_state *)z->state)->mode = I_BAD;
    ((inflate_state *)z->state)->sub.marker = 0;
  }
  if ((n = z->avail_in) == 0)
    return -5;
  p = z->next_in;
  m = ((inflate_state *)z->state)->sub.marker;

  while (n && m < 4)
  {
    if (*p == inflate_sync_mark[m])
      m++;
    else if (*p)
      m = 0;
    else
      m = 4 - m;
    p++, n--;
  }

  z->total_in += p - z->next_in;
  z->next_in = p;
  z->avail_in = n;
  ((inflate_state *)z->state)->sub.marker = m;

  if (m != 4)
    return -3;
  r = z->total_in;  w = z->total_out;
  inflateReset(z);
  z->total_in = r;  z->total_out = w;
  ((inflate_state *)z->state)->mode = BLOCKS;
  return 0;
}

int inflateSetDictionary(z_streamp z, const Bytef *dictionary, uInt dictLength)
{
  uInt length = dictLength;

  if (z == (z_streamp)0x0 || z->state == (internal_state *)0x0 || ((inflate_state *)z->state)->mode != DICT0)
    return -2;

  if (adler32(1,dictionary,dictLength) != z->adler) return -3;
  z->adler = 1;

  if (length >= ((uInt)1 << ((inflate_state *)z->state)->wbits))
  {
    length = (1 << ((inflate_state *)z->state)->wbits) - 1;
    dictionary += dictLength - length;
  }
  inflate_set_dictionary(((inflate_state *)z->state)->blocks, dictionary, length);
  ((inflate_state *)z->state)->mode = BLOCKS;
  return 0;
}

int inflateInit2_(z_streamp z, int w, const char *version, int stream_size)
{
  if (version == 0 || version[0] != '1' || stream_size != sizeof(z_stream))
    return -6;
  if (z == 0)
    return -2;
  z->msg = 0;
  if (z->zalloc == 0)
  {
    z->zalloc = zcalloc;
    z->opaque = (voidpf)0;
  }
  if (z->zfree == 0) z->zfree = zcfree;
  if ((z->state = (struct internal_state *)
       (*z->zalloc)(z->opaque, 1, sizeof(inflate_state))) == 0)
    return -4;
  ((inflate_state *)z->state)->blocks = 0;
  ((inflate_state *)z->state)->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    ((inflate_state *)z->state)->nowrap = 1;
  }
  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return -2;
  }
  ((inflate_state *)z->state)->wbits = (uInt)w;
  if ((((inflate_state *)z->state)->blocks =
      inflate_blocks_new(z, ((inflate_state *)z->state)->nowrap ? 0 : adler32, (uInt)1 << w))
      == 0)
  {
    inflateEnd(z);
    return -4;
  }
  inflateReset(z);
  return 0;
}

int inflateInit_(z_streamp z, const char *version, int stream_size)
{
  return inflateInit2_(z,15,version,stream_size);
}

int inflate(z_streamp z, int f)
{
  int r;
  uInt b;

  if (z == (z_streamp)0x0 || z->state == (internal_state *)0x0 || z->next_in == (Bytef *)0x0)
    return -2;
  f = f == 4 ? -5 : 0;
  r = -5;
  while (1) switch (((inflate_state *)z->state)->mode)
  {
    case METHOD:
      if (z->avail_in == 0) return r; r = f;
      if (((((inflate_state *)z->state)->sub.method = (z->avail_in--, z->total_in++, *z->next_in++)) & 0xf) != 8)
      {
        ((inflate_state *)z->state)->mode = I_BAD;
        z->msg = (char *)"unknown compression method";
        ((inflate_state *)z->state)->sub.marker = 5;
        break;
      }
      if ((((inflate_state *)z->state)->sub.method >> 4) + 8 > ((inflate_state *)z->state)->wbits)
      {
        ((inflate_state *)z->state)->mode = I_BAD;
        z->msg = (char *)"invalid window size";
        ((inflate_state *)z->state)->sub.marker = 5;
        break;
      }
      ((inflate_state *)z->state)->mode = FLAG;
    case FLAG:
      if (z->avail_in == 0) return r; r = f;
      b = (z->avail_in--, z->total_in++, *z->next_in++);
      if (((((inflate_state *)z->state)->sub.method << 8) + b) % 31)
      {
        ((inflate_state *)z->state)->mode = I_BAD;
        z->msg = (char *)"incorrect header check";
        ((inflate_state *)z->state)->sub.marker = 5;
        break;
      }
      if (!(b & 0x20))
      {
        ((inflate_state *)z->state)->mode = BLOCKS;
        break;
      }
      ((inflate_state *)z->state)->mode = DICT4;
    case DICT4:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need = (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 24;
      ((inflate_state *)z->state)->mode = DICT3;
    case DICT3:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 16;
      ((inflate_state *)z->state)->mode = DICT2;
    case DICT2:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 8;
      ((inflate_state *)z->state)->mode = DICT1;
    case DICT1:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++);
      z->adler = ((inflate_state *)z->state)->sub.check.need;
      ((inflate_state *)z->state)->mode = DICT0;
      return 2;
    case DICT0:
      ((inflate_state *)z->state)->mode = I_BAD;
      z->msg = (char *)"need dictionary";
      ((inflate_state *)z->state)->sub.marker = 0;
      return -2;
    case BLOCKS:
      r = inflate_blocks(((inflate_state *)z->state)->blocks, z, r);
      if (r == -3)
      {
        ((inflate_state *)z->state)->mode = I_BAD;
        ((inflate_state *)z->state)->sub.marker = 0;
        break;
      }
      if (r == 0)
        r = f;
      if (r != 1)
        return r;
      r = f;
      inflate_blocks_reset(((inflate_state *)z->state)->blocks, z, &((inflate_state *)z->state)->sub.check.was);
      if (((inflate_state *)z->state)->nowrap)
      {
        ((inflate_state *)z->state)->mode = I_DONE;
        break;
      }
      ((inflate_state *)z->state)->mode = CHECK4;
    case CHECK4:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need = (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 24;
      ((inflate_state *)z->state)->mode = CHECK3;
    case CHECK3:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 16;
      ((inflate_state *)z->state)->mode = CHECK2;
    case CHECK2:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++) << 8;
      ((inflate_state *)z->state)->mode = CHECK1;
    case CHECK1:
      if (z->avail_in == 0) return r; r = f;
      ((inflate_state *)z->state)->sub.check.need += (uLong)(z->avail_in--, z->total_in++, *z->next_in++);

      if (((inflate_state *)z->state)->sub.check.was != ((inflate_state *)z->state)->sub.check.need)
      {
        ((inflate_state *)z->state)->mode = I_BAD;
        z->msg = (char *)"incorrect data check";
        ((inflate_state *)z->state)->sub.marker = 5;
        break;
      }
      ((inflate_state *)z->state)->mode = I_DONE;
    case I_DONE:
      return 1;
    case I_BAD:
      return -3;
    default:
      return -2;
  }
}
