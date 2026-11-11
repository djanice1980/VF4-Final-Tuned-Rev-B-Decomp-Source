/*
 * vf4lib/libcore/src/gz.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint gz_step_bytes; /* 0x0c325528 */
extern int gz_step_count; /* 0x0c32552c */
extern int gz_inflating; /* 0x0c325530 */
extern float gz_step_start_us[1024]; /* 0x0c3d8d20 */
extern float gz_step_end_us[1024]; /* 0x0c3d9d20 */
extern float gz_step_time_us[1024]; /* 0x0c3dad20 */
extern z_stream gz_stream; /* 0x0c3d8ce8 */
extern float gz_begin_start_us; /* 0x0c325534 */
extern float gz_begin_end_us; /* 0x0c325538 */
extern int gz_heap_ready; /* 0x0c5b31e0 */
extern char gz_heap_area[65536]; /* 0x0c5b3200 */
extern heap gz_heap; /* 0x0c5c3200 */

void gz_heap_init(void)
{
  if (gz_heap_ready == 0) {
    heap_init(&gz_heap,gz_heap_area,0x10000);
    gz_heap_ready = 1;
  }
  return;
}

voidpf gz_heap_alloc(voidpf h, uInt n, uInt size)
{
  gz_heap_init();
  return heap_malloc(h,n * size);
}

void gz_heap_free(voidpf h, voidpf p)
{
  gz_heap_init();
  heap_free(h,p);
  return;
}

int gzip_header_skip(char *data)
{
  char *p;
  char method;
  uint flags;
  int len;
  
  p = data;
  if (*p++ != 0x1f || *p++ != (char)0x8b ||
      (method = *p++, flags = (uchar)*p++, method != 8) || (flags & 0xe0) != 0) return -3;
  p += 6;
  if (flags & 4) {
    len = (uchar)*p++;
    len += (uchar)*p++ << 8;
    while (len-- != 0) p++;
  }
  if (flags & 8) while (*p++ != 0) ;
  if (flags & 0x10) while (*p++ != 0) ;
  if (flags & 2) p += 2;
  return p - data;
}

int decode_gzip_data(Bytef *dst, uInt *dst_len, char *src, int src_len)
{
  int err;
  z_stream s;
  err = gzip_header_skip(src);
  if (err < 0) return err;
  src += err;
  src_len -= err;
  s.next_in = (Bytef *)src;
  s.avail_in = src_len;
  s.next_out = dst;
  s.avail_out = *dst_len;
  s.zalloc = (alloc_func)0x0;
  s.zfree = (free_func)0x0;
  s.opaque = (voidpf)0x0;
  err = inflateInit2_(&s,-0xf,"1.1.4",0x38);
  if (err != 0) return err;
  err = inflate(&s,4);
  if (err != 1) {
    inflateEnd(&s);
    return err == 0 ? -5 : err;
  }
  s.next_in = s.next_in + 8;
  s.avail_in = s.avail_in - 8;
  *dst_len = s.total_out;
  err = inflateEnd(&s);
  return err;
}

int gz_inflate_begin(Bytef *dst, char *src, uint size)
{
  int err;
  int start_us;
  int end_us;
  
  start_us = stopwatch_usec();
  gz_begin_start_us = (float)start_us;
  err = gzip_header_skip(src);
  if (err < 0) return err;
  src += err;
  gz_step_count = 0;
  gz_stream.next_in = (Bytef *)src;
  gz_stream.avail_out = 0x1000000;
  gz_stream.next_out = dst;
  gz_stream.zalloc = gz_heap_alloc;
  gz_stream.zfree = gz_heap_free;
  gz_stream.opaque = &gz_heap;
  err = inflateInit2_(&gz_stream,-0xf,"1.1.4",0x38);
  if (err == 0) {
    gz_inflating = 1;
  }
  end_us = stopwatch_usec();
  gz_begin_end_us = (float)end_us;
  return err;
}

void gz_stream_clear(void)
{
  gz_stream.next_in = 0;
  return;
}

void gz_inflate_step(void)
{
  int err;
  float *step_times;
  if (gz_inflating != 0)
  {
    step_times = gz_step_time_us;
    gz_step_time_us[gz_step_count] = (float) ((int) stopwatch_usec());
    gz_step_start_us[gz_step_count] = step_times[gz_step_count];
 do { gz_stream.avail_in = gz_step_bytes; err = inflate(&gz_stream, 0); gz_step_time_us[gz_step_count] = ((float) ((int) stopwatch_usec())) - gz_step_time_us[gz_step_count]; gz_step_end_us[gz_step_count] = (float) ((int) stopwatch_usec()); gz_step_count = gz_step_count + 1; } while (0);
    if (err != 0)
    {
      if (err < 0)
      {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/gz.c", 0x150, "assertion `%s\' failed.\n", "err>=0");
      }
      gz_inflating = 0;
      gz_step_end_us[gz_step_count] = 0.0;
      gz_step_start_us[gz_step_count] = 0.0;
      gz_step_time_us[gz_step_count] = 0.0;
      {
        Bytef *p = gz_stream.next_in;
        uInt n = gz_stream.avail_in;
        gz_stream.next_in = p + 8;
        gz_stream.avail_in = n - 8;
      }
      inflateEnd(&gz_stream);
      gz_stream.next_in = 0;
    }
  }
  return;
}

void gz_inflate_end(void)
{
  if (gz_stream.next_in != 0) {
    inflateEnd(&gz_stream);
  }
  gz_stream.next_in = 0;
  return;
}

void put_le32(Bytef *dst, uint value)
{
  int i;
  
  i = 0;
  do {
    *dst = (char)value;
    dst = dst + 1;
    value = value >> 8;
    i = i + 1;
  } while (i < 4);
  return;
}

int gzip_compress(Bytef *dst, uInt *dst_len, Bytef *src, uInt src_len)
{
  z_stream stream;
  int err;
  uint crc = 0;
  int hlen = 0;

  stream.next_in = src;
  stream.avail_in = src_len;
  stream.next_out = dst;
  stream.avail_out = *dst_len;
  stream.zalloc = (alloc_func)0x0;
  stream.zfree = (free_func)0x0;
  stream.opaque = (voidpf)0x0;
  sprintf(dst,"%c%c%c%c%c%c%c%c%c%c",0x1f,0x8b,8,0,0,0,0,0,0,3);
  hlen += 10;
  err = deflateInit2_(&stream,-1,8,-0xf,9,0,"1.1.4",0x38);
  if (err != 0) return err;
  err = deflate(&stream,4);
  if (err != 1) {
    deflateEnd(&stream);
    return err == 0 ? -5 : err;
  }
  put_le32(stream.next_out,crc);
  hlen += 4;
  stream.next_out += 4;
  stream.avail_out -= 4;
  put_le32(stream.next_out,stream.total_in);
  hlen += 4;
  stream.next_out += 4;
  stream.avail_out -= 4;
  *dst_len = stream.total_out + hlen;
  err = deflateEnd(&stream);
  return err;
}
