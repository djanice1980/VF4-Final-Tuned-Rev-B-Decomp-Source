/*
 * libc/stdio/ungetc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int scan_stream_ungetc(int c, scan_stream_rec *stream)
{
  short fl;

  if (c == -1) return -1;
  fl = stream->flags;
  stream->flags = fl & 0xffef;
  if ((fl & 1) == 0) return -1;
  c = (uchar)c;
  if (stream->pushback != 0) {
    if (stream->left >= stream->pushback_size) return -1;
    *--stream->pos = c;
    stream->left = stream->left + 1;
  }
  else {
    if ((stream->left < stream->size) && (stream->pos[-1] == c)) {
      stream->left = stream->left + 1;
      stream->pos = stream->pos + -1;
      return c;
    }
    stream->saved_pos = stream->pos;
    stream->saved_left = stream->left;
    stream->pushback = stream->pushback_buf;
    stream->pushback_size = 4;
    stream->pushback_buf[3] = c;
    stream->pos = &stream->pushback_buf[3];
    stream->left = 1;
  }
  return c;
}
