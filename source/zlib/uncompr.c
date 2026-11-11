/*
 * zlib/uncompr.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int uncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef *)source;
  stream.avail_in = (uInt)sourceLen;
  stream.next_out = dest;
  stream.avail_out = (uInt)*destLen;
  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  err = inflateInit_(&stream, "1.1.4", sizeof(z_stream));
  if (err != 0) return err;
  err = inflate(&stream, 4);
  if (err != 1) {
    inflateEnd(&stream);
    return err == 0 ? -5 : err;
  }
  *destLen = stream.total_out;
  err = inflateEnd(&stream);
  return err;
}
