/*
 * vf4lib/libcore/src/load.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int gz_inflating; /* 0x0c325530 */

void file_system_init(void)
{
  sffs_mount();
  dimm_files_init();
  load_init();
  return;
}

void load_file(char *name, void *dst, uint size)
{
  char *s1;
  int err;
  uint len;
  struct gzb { void *buf; uint size; } *g;
  
  s1 = strrchr(name,0x2e);
  err = memcmp(s1,".gz",4);
  if (err != 0) {
    perf_color_mark(0x7f);
    read_file(name,dst,size);
  }
  else {
    len = size;
    perf_color_mark(0x7f);
    g = (struct gzb *)&gz_buffer;
    read_file(name,g->buf,g->size);
    perf_color_mark(0x7f00);
    err = decode_gzip_data(dst,&len,g->buf,g->size);
    if (err != 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x67,"Error: %d = decode_gzip_data()",
               err);
    }
  }
  perf_color_mark(0);
}

void read_file(char *name, void *dst, uint max)
{
  dimm_file *fp;
  int r;
  if (((uint)dst & 0xfffffff) <= 0xbffffff) {
    read_file_pio(name,dst,max);
    return;
  }
  fp = dimm_fopen(name,"rb");
  if (fp == (dimm_file *)0x0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x7a,"%s open error",name);
    return;
  }
  dcache_tags_clear_uncached();
  r = dimm_fcheck(fp);
  if (r != -1) {
    if ((uint)fp->size > max) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x81,"%s buffer overflow",name);
    }
    else {
      r = dimm_fread_dma(dst,fp->size,1,fp);
      if (r == -1) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x86,"%s read error",name);
      }
    }
  }
  dimm_fclose(fp);
  dcache_tags_clear_uncached();
}

void read_file_pio(char *name, void *dst, uint max)
{
  dimm_file *fp;
  
  fp = dimm_fopen(name,"rb");
  if (fp == (dimm_file *)0x0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x94,"%s open error",name);
    return;
  }
  if (dimm_fcheck(fp) != -1) {
    if ((uint)fp->size > max) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x9a,"%s buffer overflow",name);
    }
    else if (dimm_fread_pio(dst,fp->size,1,fp) == -1) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0x9f,"%s read error",name);
    }
  }
  dimm_fclose(fp);
}

uint load_req(char *name, void *dst, uint size)
{
  int t = memcmp(strrchr(name,0x2e),".gz",4) == 0;
  return load_req_type(name,dst,size,t + 3);
}

void load_init(void)
{
  load_slot *s;
  int i;
  
  s = load_slots;
  gz_stream_clear();
  load_pending = 0;
  i = 0;
  do {
    s->state = LOAD_FREE;
    s->done = 0;
    snprintf(s->name,0x40,"%s","init");
    i = i + 1;
    s = s + 1;
  } while (i < 0x20);
  return;
}

uint file_ungzip_req(void *dst, void *src)
{
  uint n;
  uint bit;
  load_slot *s;
  int i;
  
  for (i = 0; ; ) {
    n = load_next_slot;
    s = &load_slots[n];
    bit = 1 << n;
    load_next_slot = n + 1;
    if (load_next_slot > 0x1f) {
      load_next_slot = 0;
    }
    if (s->state == LOAD_FREE) {
      snprintf(s->name,0x40,"%s","file_ungzip_req");
      load_pending = load_pending | bit;
      s->read_to = src;
      s->cursor = src;
      s->dst = dst;
      s->state = 6;
      s->size = 0x1000000;
      s->done = 0;
      return bit;
    }
    if (++i > 0x1f) break;
  }
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c",0xdd,"assertion `%s\' failed.\n","0");
  return 0xffffffff;
}

uint load_req_type(char *name, void *dst, uint size, int type)
{
  int tries = 0;
  uint i, bit;
  load_slot *slot;

  for (;;) {
    i = load_next_slot;
    slot = &load_slots[i];
    bit = 1 << i;
    load_next_slot++;
    if (load_next_slot > 31) load_next_slot = 0;
    if (slot->state == LOAD_FREE) {
      load_pending |= bit;
      slot->state = type;
      strcpy(slot->name, name);
      if (type == 4) {
        slot->read_to = gz_buffer;
        slot->cursor = gz_buffer;
        slot->dst = dst;
        slot->state = LOAD_GZIP;
      } else {
        slot->read_to = dst;
        slot->cursor = dst;
        slot->dst = dst;
        slot->state = LOAD_RAW;
      }
      slot->size = size;
      slot->done = 0;
      return bit;
    }
    if (++tries > 31) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c", 0x103, "<%s> IS OVER %d FILES", name, 0x20);
      return -1;
    }
  }
}

void load_service(void)
{
  load_slot *s;
  dimm_file *fp;
  uint chunk;
  uint r;
  int i;
  uint a;

  chunk = load_read_chunk_size;
  for (;;) {
    s = load_slots;
    for (i = 0; i < 0x20; i++, s++) {
      if (s->state != LOAD_FREE) break;
    }
    if (i == 0x20) {
      load_pending = 0;
      break;
    }
    s = &load_slots[load_cur_slot];
    if ((uint)(s->state - LOAD_RAW) < 2) {
      fp = dimm_fopen(s->name, "rb");
      if (fp == (dimm_file *)0x0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/load.c", 0x124, "<%s> IS CANT OPEN", s->name);
      }
      s->done = (uint)fp;
      if (s->state == LOAD_RAW) s->state = 3;
      else s->state = 4;
      continue;
    }
    if ((uint)(s->state - 3) < 2) {
      perf_color_mark(0x7f);
      if (!(load_read_usec_limit > (float)(int)stopwatch_usec())) break;
      a = (uint)s->read_to & 0xfffffff;
      if (chunk > s->size) chunk = s->size;

      if (a <= 0xbffffff) {
        r = dimm_fread_pio(s->read_to, chunk, 1, (dimm_file *)s->done);
      }
      else {
        dcache_writeback_discard_range_uncached((uint)s->read_to, chunk);
        dimm_fread_dma(s->read_to, chunk, 1, (dimm_file *)s->done);
        while (dimm_fbusy((dimm_file *)s->done) != 0) ;
        dcache_writeback_discard_range_uncached((uint)s->read_to, chunk);
        r = dimm_fread_progress((dimm_file *)s->done);
      }
      s->size -= chunk;
      s->read_to = (char *)s->read_to + chunk;
      if ((s->size == 0) | (r == 0)) {
        if (s->done != 0) dimm_fclose((dimm_file *)s->done);
        s->done = 0;
        if (s->state == 4) {
          s->state = 5;
          gz_inflate_begin(s->dst, s->cursor, 0x1000000);
        }
        else {
          s->state = LOAD_FREE;
          s->name[0] = '\0';
          s->name[1] = '\0';
          load_pending &= ~(1 << load_cur_slot);
        }
      }
      continue;
    }
    if (s->state == 5) {
      perf_color_mark(0x7f00);
      if (!(load_inflate_usec_limit > (float)(int)stopwatch_usec())) break;
      gz_inflate_step();
      if (gz_inflating != 0) continue;
      s->state = LOAD_FREE;
      s->name[0] = '\0';
      s->name[1] = '\0';
      load_pending &= ~(1 << load_cur_slot);
      if (++load_cur_slot > 0x1f) load_cur_slot = 0;
      break;
    }
    if (s->state == 6) {
      s->state = 5;
      gz_inflate_begin(s->dst, s->cursor, 0x1000000);
      continue;
    }
    load_pending &= ~(1 << load_cur_slot);
    if (++load_cur_slot > 0x1f) load_cur_slot = 0;
  }
  perf_color_mark(0);
}

void load_reset(void)
{
  load_slot *s;
  int i;
  
  s = load_slots;
  load_pending = 0;
  load_next_slot = 0;
  load_cur_slot = 0;
  i = 0;
  do {
    s->state = LOAD_FREE;
    s->name[0] = '\0';
    if ((dimm_file *)s->done != (dimm_file *)0x0) {
      dimm_fclose((dimm_file *)s->done);
      s->done = 0;
    }
    i = i + 1;
    s = s + 1;
  } while (i < 0x20);
  gz_inflate_end();
  return;
}

uint load_pending_get(void)
{
  return load_pending;
}
