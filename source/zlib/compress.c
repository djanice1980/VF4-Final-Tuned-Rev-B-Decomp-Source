/*
 * zlib/compress.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef *)source;
  stream.avail_in = sourceLen;
  stream.next_out = dest;
  stream.avail_out = *destLen;
  stream.zalloc = (alloc_func)0;
  stream.zfree = (free_func)0;
  stream.opaque = (voidpf)0;
  err = deflateInit_(&stream,level,"1.1.4",sizeof(z_stream));
  if (err != 0) return err;
  err = deflate(&stream,4);
  if (err != 1) {
    deflateEnd(&stream);
    return err == 0 ? -5 : err;
  }
  *destLen = stream.total_out;
  err = deflateEnd(&stream);
  return err;
}

int compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
  return compress2(dest,destLen,source,sourceLen,-1);
}
