/*
 * sdk/libsnd/libsnd.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char dipan_to_pan_offset[32]; /* 0x0c211a08 */
extern uint dtpk_empty_header[25]; /* 0x0c211a28 */
extern void *libsnd_version_string; /* 0x0c325ac4 */
extern int aica_ring_busy; /* 0x0c5cab9c */
extern uint aica_ring_wp; /* 0x0c5caba0 */
extern uint drv_info_addr; /* 0x0c5caba4 */
extern uint dtpk_area_start; /* 0x0c5caba8 */
extern uint drv_top_size_word; /* 0x0c5cabac */
extern uint sndram_size; /* 0x0c5cabb0 */
extern int dtpk_bytes_used; /* 0x0c5cabb4 */
extern int pcm_buffer_bytes; /* 0x0c5cabb8 */
extern int sndram_copy_by_dma; /* 0x0c5cabbc */
extern uint drv_build_number; /* 0x0c5cabc4 */
extern dtpk_job_rec dtpk_job_record; /* 0x0c5cabc8 */
extern dtpk_job_rec *dtpk_job; /* 0x0c5cabe8 */
extern uchar pcm_port_table[224]; /* 0x0c5cabec */
extern uint dtpk_slot_table[24]; /* 0x0c5caccc */
extern uint snd_dma_handles[64]; /* 0x0c5cad2c */
extern uint *snd_dma_ring[16]; /* 0x0c5cae2c */
extern g2_dma_channel *snd_dma_channel; /* 0x0c5cae74 */

int aica_word_store(uint value, uint addr)
{
  uint word[1];
  
  word[0] = value;
  g2_cpu_copy_to_bus(word,(uint *)addr,4,1,0,0);
  return 0;
}

uint aica_word_fetch(uint addr)
{
  uint word[1];
  
  g2_cpu_copy_from_bus((uint *)addr,word,4,1,0,0);
  return word[0];
}

int snd_cmd_18a0_post_retry(void)
{
  int r;
  int tries;
  
  tries = 0;
  do {
    r = aica_ring_push(0x18a0);
    if (r == 0) {
      return 0;
    }
    tries = tries + 1;
  } while (tries < 0x400000);
  return 0xffffff00;
}

int snd_ack_word_wait_nonzero(void)
{
  int r;
  int tries;
  
  tries = 0;
  do {
    r = aica_word_fetch(0xa08000b4);
    if (r != 0) {
      return 0;
    }
    tries = tries + 1;
  } while (tries < 0x400000);
  return 0xffffff00;
}

int snd_ram_copy_wait_done(void)
{
  int r;
  int tries;
  
  tries = 0;
  do {
    r = snd_ram_copy_busy();
    if (r == 0) {
      return 0;
    }
    tries = tries + 1;
  } while (tries < 0x1000000);
  return 0xffffff00;
}

int snd_flag_raise_wait_clear(void)
{
  int i;

  aica_word_store(1,0xa08000b0);
  for (i = 0; ; ) {
    if (aica_word_fetch(0xa08000b4) == 0) break;
    i++;
    if (i >= 0x400000) return -0x100;
  }
  aica_word_store(0,0xa08000b0);
  return 0;
}

int snd_ram_copy_request(int src, int dst, uint size)
{
  int i;
  aica_word_store(src + 0x5f800000,0xa0800090);
  aica_word_store(dst + 0x5f800000,0xa0800094);
  aica_word_store(size,0xa0800098);
  for (i = 0; ; ) {
    if (aica_word_fetch(0xa0800098) != 0) break;
    i++;
    if (i > 0x3fffff) return 0xffffff00;
  }
  for (i = 0; ; ) {
    if (aica_ring_push(0x2000a0) == 0) break;
    i++;
    if (i > 0x3fffff) return 0xffffff00;
  }
  return 0;
}

bool snd_ram_copy_busy(void)
{
  int v = aica_word_fetch(0xa0800098);
  char r = v == 0;
  return !r;
}

int dtpk_insert_check(char *image, uint slot, uint slot_size)
{
  char *s;
  char *t;
  int i;
  int size;
  int avail;
  
  if (slot >= 8) return 0xffffffff;
  s = image;
  t = "DTPK";
  for (i = 0; i < 4; i++) {
    if (*s++ != *t++) return 0xfffffffe;
  }
  if (dtpk_slot_table[slot * 3] != -1) return 0xfffffffd;
  size = *(int *)(image + 8);
  avail = aica_word_fetch(0xa0800088);
  if (slot_size != 0) {
    if ((slot_size & 3) != 0) return 0xfffffffa;
    if ((int)slot_size < size) return 0xfffffffa;
    size = slot_size;
  }
  if ((int)(dtpk_bytes_used + size) > avail) return 0xfffffffc;
  if (dtpk_job->job != 0) return 0xfffffffb;
  return 0;
}

int dtpk_overwrite_check(char *image, uint slot)
{
  int i;
  char *s;
  char *d;
  int n;
  
  if (slot >= 8) return 0xffffffff;
  d = image;
  s = "DTPK";
  for (i = 0; i < 4; i++) {
    if (*d++ != *s++) return 0xfffffffe;
  }
  if (((struct { int bank_no; int addr; int size; } *)&dtpk_slot_table)[slot].bank_no == -1) return 0xfffffffd;
  n = *(int *)(image + 8);
  if (((struct { int bank_no; int addr; int size; } *)&dtpk_slot_table)[slot].size < n) return 0xfffffffc;
  if (dtpk_job->job != 0) return 0xfffffffb;
  return 0;
}

int dtpk_slot_insert(int slot, uint bank_no, int size, int *move_bytes)
{
  typedef struct { int bank_no; int addr; int size; } dtpk_slot;
  int i;
  int sum;
  int addr;
  
  dtpk_bytes_used = dtpk_bytes_used + size;
  ((dtpk_slot *)&dtpk_slot_table)[slot].size = size;
  ((dtpk_slot *)&dtpk_slot_table)[slot].bank_no = bank_no;
  aica_word_store(bank_no,slot * 4 + -0x5f7fffa0);
  sum = 0;
  addr = 0;
  for (i = slot - 1; i >= 0; i--) {
    dtpk_slot *t = (dtpk_slot *)&dtpk_slot_table; dtpk_slot *e = &t[i];
    if (e->bank_no != -1) {
      addr = e->addr + e->size;
      break;
    }
  }
  if (addr == 0) {
    addr = dtpk_area_start;
  }
  ((dtpk_slot *)&dtpk_slot_table)[slot].addr = addr;
  for (i = slot + 1; i < 8; i++) {
    dtpk_slot *t = (dtpk_slot *)&dtpk_slot_table; dtpk_slot *e = &t[i];
    if (e->bank_no != -1) {
      sum = sum + e->size;
      e->addr = e->addr + size;
    }
  }
  *move_bytes = sum;
  return addr;
}

int dtpk_drop_check(uint slot)
{
  if (slot > 7) return 0xffffffff;
  if (dtpk_slot_table[slot * 3] == -1) return 0xfffffffe;
  if (dtpk_job->job != 0) return 0xfffffffd;
  return 0;
}

int dtpk_slot_delete(int slot, int size, int *move_bytes)
{
  typedef struct { int bank_no; int addr; int size; } dtpk_slot;
  int sum;
  int first_addr;
  
  dtpk_bytes_used = dtpk_bytes_used - size;
  ((dtpk_slot *)&dtpk_slot_table)[slot].bank_no = -1;
  aica_word_store(0xffffffff,slot * 4 + -0x5f7fffa0);
  sum = 0;
  first_addr = 0;
  for (slot++; slot < 8; slot++) {
    dtpk_slot *t = (dtpk_slot *)&dtpk_slot_table;
    if (t[slot].bank_no != -1) {
      if (first_addr == 0) {
        first_addr = ((dtpk_slot *)&dtpk_slot_table)[slot].addr;
      }
      {
        dtpk_slot *u = (dtpk_slot *)&dtpk_slot_table;
        sum = sum + u[slot].size;
        u[slot].addr -= size;
      }
    }
  }
  *move_bytes = sum;
  return first_addr;
}

int aica_driver_boot(int image, int board_type)
{
  typedef struct { uint bank_no; uint addr; uint size; } dtpk_slot;
  typedef struct { uchar state; uchar playing; short sample_bits; uint buf_addr0; uint buf_addr1; char field_0xc[16]; } pcm_port;
  uchar *ram_size;
  uint word;
  int i;
  int ready;
  uint slot_word;
  int info_off = 200;

  drv_build_number = aica_word_fetch(image + 0xc4);
  if (drv_build_number <= 0x6f3) return 0xffffffff;
  switch (board_type) {
  case 0: ram_size = (uchar *)0x200000; break;
  case 0x200: ram_size = (uchar *)0x800000; break;
  default: return 0xfffffffe;
  }
  sndram_size = (uint)ram_size;
  word = aica_word_fetch(0xa0702c00);
  aica_word_store(word & 0xfffe | 1,0xa0702c00);
  aica_word_store(board_type,0xa0702800);
  word = aica_word_fetch(0xa0702c00);
  aica_word_store(word & 0xfffe | 1,0xa0702c00);
  word = aica_word_fetch(image + 0x54);
  g2_cpu_copy_to_bus((uint *)image,(uint *)0xa0800000,4,word >> 2,1,1);
  aica_word_store(board_type,0xa0800050);
  for (slot_word = 0xa0800060, i = 0; i < 8; i++, slot_word += 4) {
    aica_word_store(0xffffffff,slot_word);
  }
  aica_word_store(aica_word_fetch(0xa0702c00) & 0xfffe,0xa0702c00);
  for (i = 0; i < 8; i++) {
    dtpk_slot *q = (dtpk_slot *)&dtpk_slot_table;
    q[i].bank_no = 0xffffffff;
    q[i].addr = 0;
    q[i].size = 0;
  }
  for (i = 0; i < 8; i++) {
    pcm_port *q = (pcm_port *)&pcm_port_table;
    q[i].state = 0xff;
    q[i].playing = 0;
    q[i].buf_addr1 = 0xffffffff;
    q[i].buf_addr0 = 0xffffffff;
  }
  dtpk_area_start = *(int *)(image + 0x84) + 0xa0800000;
  drv_top_size_word = *(int *)(image + 0x80) + 0xa0800010;
  drv_info_addr = *(int *)((uchar *)image + info_off) + 0xa0800000;
  aica_ring_wp = 0xa0800400;
  dtpk_bytes_used = 0;
  aica_ring_busy = 0;
  dtpk_job = &dtpk_job_record;
  dtpk_job_record.job = 0;
  i = 0;
  while (1) {
    ready = aica_word_fetch(0xa080005c);
    if (ready != 0) break;
    i++;
    if (i > 0x3fffff) return 0xffffff00;
  }
  aica_word_store(aica_word_fetch(0xa070289c) & 0x7f7,0xa070289c);
  if (snd_dma_take_channel() != 0) return 0xffffff80;
  return 0;
}

int sound_off_dma_release(void)
{
  int r;
  
  snd_cmd_a0_post(0,0x100a0,0);
  snd_cmd_a0_post(0,0x400a0,0);
  r = snd_dma_channel_give_back();
  if (!(r == 0)) {
    return 0xffffff80;
  }
  return 0;
}

int dtpk_insert_copy(int image, uint slot)
{
  int r;
  int addr;
  uint size;
  int n;
  
  r = dtpk_insert_check((char *)image,slot,0);
  if (r != 0) return r;
  size = *(uint *)(image + 8);
  addr = dtpk_slot_insert(slot,*(uint *)(image + 4),size,&n);
  r = snd_cmd_18a0_post_retry();
  if (r != 0) return r;
  r = snd_ack_word_wait_nonzero();
  if (r != 0) return r;
  if (n != 0) {
    r = snd_ram_copy_request(addr,addr + size,n);
    if (r != 0) return r;
    r = snd_ram_copy_wait_done();
    if (r != 0) return r;
  }
  g2_cpu_copy_to_bus((uint *)image,(uint *)addr,4,size >> 2,1,1);
  r = snd_flag_raise_wait_clear();
  if (r != 0) return r;
  return 0;
}

int dtpk_insert_copy_sized(int image, uint slot, int slot_size)
{
  int r;
  int addr;
  uint size;
  int move_bytes;
  if (slot_size == 0) return -6;
  r = dtpk_insert_check((char *)image,slot,slot_size);
  if (r != 0) return r;
  size = *(uint *)(image + 8);
  addr = dtpk_slot_insert(slot,*(uint *)(image + 4),slot_size,&move_bytes);
  r = snd_cmd_18a0_post_retry();
  if (r != 0) return r;
  r = snd_ack_word_wait_nonzero();
  if (r != 0) return r;
  if (move_bytes != 0) {
    r = snd_ram_copy_request(addr,addr + slot_size,move_bytes);
    if (r != 0) return r;
    r = snd_ram_copy_wait_done();
    if (r != 0) return r;
  }
  g2_cpu_copy_to_bus((uint *)image,(uint *)addr,4,size >> 2,1,1);
  aica_word_store(slot_size,addr + 8);
  r = snd_flag_raise_wait_clear();
  if (r != 0) return r;
  return 0;
}

int dtpk_overwrite_copy(char *image, uint slot)
{
  typedef struct { uint bank_no; uint addr; uint size; } dtpk_slot;
  int r;
  int i;
  uint bank_no;
  uint size;
  dtpk_slot *e;
  r = dtpk_overwrite_check(image,slot);
  if (r != 0) return r;
  i = 0;
  while (1) {
    r = snd_cmd_a0_post((ushort)slot,0x1100a0,0);
    i++;
    if (r == 0) break;
    if (i > 0x3fffff) return -0x100;
  }
  bank_no = *(uint *)(image + 4);
  size = *(uint *)(image + 8);
  if (((dtpk_slot *)&dtpk_slot_table)[slot].bank_no == bank_no) bank_no |= 0x80000000;
  ((dtpk_slot *)&dtpk_slot_table)[slot].bank_no = bank_no;
  aica_word_store(bank_no,slot * 4 + 0xa0800060);
  r = snd_cmd_18a0_post_retry();
  if (r != 0) return r;
  r = snd_ack_word_wait_nonzero();
  if (r != 0) return r;

  {
    uint *paddr;
    uint *psize;
    g2_cpu_copy_to_bus((uint *)image,(uint *)*(paddr = &(e = (dtpk_slot *)&dtpk_slot_table + slot)->addr),4,size >> 2,1,1);
    psize = &e->size;
    aica_word_store(*psize,*paddr + 8);
  }
  r = snd_flag_raise_wait_clear();
  if (r != 0) return r;
  return 0;
}

int dtpk_insert_bg_begin(int image, uint slot, int words_per_step)
{
  int r;
  uint size, bank_no;
  int move_bytes;
  
  r = dtpk_insert_check((char *)image,slot,0);
  if (r != 0) return r;
  size = *(uint *)(image + 8);
  bank_no = *(uint *)(image + 4);
  dtpk_job->dst =
  dtpk_job->bank_start = dtpk_slot_insert(slot,bank_no,size,&move_bytes);
  dtpk_job->src = image;
  dtpk_job->job = 1;
  dtpk_job->words_per_step = words_per_step;
  dtpk_job->whole_size = dtpk_job->bytes_left = size;
  dtpk_job->move_bytes = move_bytes;
  return 0;
}

int dtpk_insert_sized_bg_begin(int image, uint slot, int words_per_step, int slot_size)
{
  dtpk_job_rec *job;
  int r;
  uint addr;
  int move_bytes;
  if (slot_size == 0) return -6;
  r = dtpk_insert_check((char *)image,slot,slot_size);
  if (r != 0) return r;
  job = dtpk_job;
  addr = dtpk_slot_insert(slot,*(uint *)(image + 4),slot_size,&move_bytes);
  job->bank_start = addr;
  job->dst = addr;
  dtpk_job->src = image;
  dtpk_job->job = 0x8001;
  dtpk_job->words_per_step = words_per_step;
  dtpk_job->whole_size = slot_size;
  dtpk_job->bytes_left = *(uint *)(image + 8);
  dtpk_job->move_bytes = move_bytes;
  return 0;
}

int dtpk_overwrite_bg_begin(char *image, uint slot, int words_per_step)
{
  typedef struct { uint bank_no; uint addr; uint size; } dtpk_slot;
  int r;
  uint bank_no;
  int tries;
  dtpk_slot *e;
  uint addr;
  dtpk_job_rec *job;

  r = dtpk_overwrite_check(image,slot);
  if (r != 0) return r;
  for (tries = 0; ; ) {
    r = snd_cmd_a0_post((ushort)slot,0x1100a0,0);
    tries = tries + 1;
    if (r == 0) break;
    if (tries > 0x3fffff) return -0x100;
  }
  bank_no = *(uint *)(image + 4);
  if (((dtpk_slot *)&dtpk_slot_table)[slot].bank_no == bank_no) {
    bank_no = bank_no | 0x80000000;
  }
  e = &((dtpk_slot *)&dtpk_slot_table)[slot];
  e->bank_no = bank_no;
  aica_word_store(bank_no,slot * 4 + -0x5f7fffa0);
  job = dtpk_job;
  addr = e->addr;
  job->bank_start = addr;
  job->dst = addr;
  dtpk_job->src = (uint)image;
  dtpk_job->job = 0x8001;
  dtpk_job->words_per_step = words_per_step;
  dtpk_job->whole_size = e->size;
  dtpk_job->bytes_left = *(uint *)(image + 8);
  dtpk_job->move_bytes = 0;
  return 0;
}

int dtpk_insert_fed_begin(int image, uint slot, int words_per_step)
{
  int r = dtpk_insert_bg_begin(image,slot,words_per_step); if (r == 0) { dtpk_job->job = 0x1001; return 0; } return r;
}

int dtpk_insert_sized_fed_begin(int image, uint slot, int words_per_step, int slot_size)
{
  int r = dtpk_insert_sized_bg_begin(image,slot,words_per_step,slot_size);
  if (r != 0) return r;
  dtpk_job->job = 0x9001;
  return 0;
}

int dtpk_overwrite_fed_begin(char *image, uint slot, int words_per_step)
{
  int r = dtpk_overwrite_bg_begin(image,slot,words_per_step);
  if (r != 0) return r;
  dtpk_job->job = 0x9001;
  return 0;
}

int dtpk_feed_chunk(uint *src)
{
  uint n;
  int left;

  if ((dtpk_job->job & 0x10ff) != 0x1004) return 0xffffffff;
  if (dtpk_job->bytes_left < dtpk_job->words_per_step << 2)
    n = (uint)dtpk_job->bytes_left >> 2;
  else
    n = dtpk_job->words_per_step;
  g2_cpu_copy_to_bus(src,(uint *)dtpk_job->dst,4,n,1,1);
  left = dtpk_job->bytes_left - n * 4;
  dtpk_job->bytes_left = left;
  if (left == 0) {
    if ((dtpk_job->job & 0x8000) != 0) {
      aica_word_store(dtpk_job->whole_size,dtpk_job->bank_start + 8);
    }
    aica_word_store(1,0xa08000b0);
    dtpk_job->job = dtpk_job->job + 1;
    return 0;
  }
  dtpk_job->dst = n * 4 + dtpk_job->dst;
  return 1;
}

uint dtpk_feed_dest(void)
{
  if (!((dtpk_job->job & 0x10ff) == 0x1004)) {
    return 0;
  }
  return dtpk_job->dst;
}

int dtpk_feed_bytes_left(void)
{
  if (!((dtpk_job->job & 0x10ff) == 0x1004)) {
    return 0xffffffff;
  }
  return dtpk_job->bytes_left;
}

int dtpk_feed_done(void)
{
  if (!((dtpk_job->job & 0x10ff) == 0x1004)) {
    return 0xffffffff;
  }
  if ((dtpk_job->job & 0x8000) != 0) {
    aica_word_store(dtpk_job->whole_size,dtpk_job->bank_start + 8);
  }
  aica_word_store(1,0xa08000b0);
  dtpk_job->job = dtpk_job->job + 1;
  return 0;
}

int dtpk_words_per_step_set(int words)
{
  if (words == 0) {
    return -1;
  }
  dtpk_job->words_per_step = words;
  return 0;
}

int dtpk_job_step(void)
{
  uint n;
  int left;
  uint kind;
  switch(dtpk_job->job & 0xff) {
  case 0:
    return 0xffffffff;
  case 1:
    if (snd_cmd_18a0_post_retry() != 0) {
      return 1;
    }
    dtpk_job->job = dtpk_job->job + 1;
    return 1;
  case 2:
    if (aica_word_fetch(0xa08000b4) == 0) {
      return 1;
    }
    if (dtpk_job->move_bytes != 0) {
      snd_ram_copy_request(dtpk_job->dst,
                   dtpk_job->dst + dtpk_job->whole_size,
                   dtpk_job->move_bytes);
    }
    dtpk_job->job = dtpk_job->job + 1;
    return 1;
  case 3:
    if (snd_ram_copy_busy() != 0) {
      return 1;
    }
    dtpk_job->job = dtpk_job->job + 1;
  case 4:
    kind = dtpk_job->job & 0x7f00;
    if (kind == 0) {
      if (dtpk_job->bytes_left < dtpk_job->words_per_step * 4) {
        n = (uint)dtpk_job->bytes_left >> 2;
      } else {
        n = dtpk_job->words_per_step;
      }
      g2_cpu_copy_to_bus((uint *)dtpk_job->src,(uint *)dtpk_job->dst,4,n,1,1);
      left = dtpk_job->bytes_left - n * 4;
      dtpk_job->bytes_left = left;
      if (left == 0) {
        if ((dtpk_job->job & 0x8000) != 0) {
          aica_word_store(dtpk_job->whole_size,dtpk_job->bank_start + 8);
        }
        aica_word_store(1,0xa08000b0);
        dtpk_job->job = dtpk_job->job + 1;
        return 1;
      }
      dtpk_job->dst = dtpk_job->dst + n * 4;
      dtpk_job->src = n * 4 + dtpk_job->src;
      return 1;
    }
    if (kind == 0x100) {
      aica_word_store(1,0xa08000b0);
      dtpk_job->job = dtpk_job->job + 1;
      return 1;
    }
    return 0;
  case 5:
    if (aica_word_fetch(0xa08000b4) != 0) {
      return 1;
    }
    aica_word_store(0,0xa08000b0);
    dtpk_job->job = 0;
    return 0;
  default:
    return 0xffffffff;
  }
}

int dtpk_job_blank_out(void)
{
  uint job;
  
  if (dtpk_job->job == 0) return 0xffffffff;
  job = dtpk_job->job;
  if ((job & 0xff) == 4) {
    if ((job & 0x7f00) == 0x100) return 0xffffffff;
    g2_cpu_copy_to_bus(&dtpk_empty_header,(uint *)dtpk_job->bank_start,4,0x19,1,1);
    aica_word_store(dtpk_job->whole_size,dtpk_job->bank_start + 8);
    aica_word_store(1,0xa08000b0);
    dtpk_job->job = dtpk_job->job + 1;
    return 0;
  }
  return 1;
}

int dtpk_drop_compact(int slot)
{
  int r;
  int src;
  int size;
  int move_bytes;
  
  r = dtpk_drop_check(slot);
  if (r != 0) return r;
  size = ((struct { uint bank_no; uint addr; int size; } *)&dtpk_slot_table)[slot].size;
  src = dtpk_slot_delete(slot,size,&move_bytes);
  r = snd_cmd_18a0_post_retry();
  if (r != 0) return r;
  r = snd_ack_word_wait_nonzero();
  if (r != 0) return r;
  if (src != 0) {
    r = snd_ram_copy_request(src,src - size,move_bytes);
    if (r != 0) return r;
    r = snd_ram_copy_wait_done();
    if (r != 0) return r;
  }
  r = snd_flag_raise_wait_clear();
  if (r != 0) return r;
  return 0;
}

int dtpk_drop_compact_bg_begin(int slot)
{
  dtpk_job_rec *job;
  int r;
  int size;
  int move_bytes;
  
  r = dtpk_drop_check(slot);
  if (r != 0) return r;
  size = ((struct { int bank_no; int addr; int size; } *)&dtpk_slot_table)[slot].size;
  job = dtpk_job;
  job->dst = dtpk_slot_delete(slot,size,&move_bytes);
  dtpk_job->job = 0x101;
  dtpk_job->whole_size = -size;
  dtpk_job->move_bytes = move_bytes;
  return 0;
}

uint * dtpk_empty_header_get(void)
{
  return dtpk_empty_header;
}

int snd_cmd_a0_post(uint slot, uint code, uint value)
{
  int word;

  word = (slot & 0xf) << 24;
  word += code;
  if ((code & 0xff00) != 0) {
    switch (code) {
    case 0x1a0: case 0x4a0: case 0x9a0: case 0xaa0: case 0x10a0: case 0x19a0: case 0x1ca0:
      word += (value & 0x7f) << 16;
      break;
    case 0x5a0: case 0x6a0: case 0x7a0: case 0x11a0:
      word += ((value + 0x40) & 0x7f) << 16;
      break;
    case 0x1ba0:
      word += (value & 0xf) << 16;
      break;
    case 0x28a0: case 0x29a0: case 0x30a0:
      word += ((value - 1) & 0xf) << 16;
      break;
    case 0x2a0: case 0x3a0:
      break;
    default:
      return 0xfffffffe;
    }
  }
  return aica_ring_push(word);
}

int snd_cmd_b0_post(uint slot, int channel, uint code, uint value)
{
  uint word = ((slot & 0xf) << 24) + (channel - 1U & 0xf) + code;
  switch (code) {
  case 0x1b0:
  case 0x2b0:
  case 0x7b0:
  case 0x27b0:
  case 0x40b0:
  case 0x41b0:
    word += (value & 0x7f) << 16;
    break;
  case 0xab0:
  case 0x2ab0:
    word += (value + 0x40 & 0x7f) << 16;
    break;
  case 0xcb0:
  case 0xdb0:
  case 0x2cb0:
  case 0x2db0:
    word += (value & 0xf) << 16;
    break;
  case 0x78b0:
  case 0x7bb0:
    break;
  default:
    return -2;
  }
  return aica_ring_push(word);
}

int snd_cmd_a5_post(uint channel, int code, uint value)
{
  int word;
  int r;

  word = ((channel & 0xf) << 8) + code;
  switch (code) {
  case 0xa5:
    word = ((channel & 0xf) << 8) + ((value & 0x7f00) << 8) + 0x70a5;
    r = aica_ring_push(word);
    if (r != 0) return r;
    word = ((channel & 0xf) << 8) + ((value & 0x7f) << 16) + 0xa5;
    break;
  case 0x10a5:
    word += (value & 0x7f) << 16;
    break;
  case 0xa7:
  case 0x10a7:
  case 0x20a5:
  case 0x30a5:
  case 0x40a5:
    word += ((value + 0x40) & 0x7f) << 16;
    break;
  case 0x50a5:
    word += (value & 0xf) << 16;
    break;
  case 0xa6:
    word += ((value & 0x7f) << 16) + ((value & 0x80) << 5);
    break;
  default:
    return -2;
  }
  return aica_ring_push(word);
}

int snd_cmd_a4_post(uint channel, int code, uint value)
{
  int r;
  uint word = code + (channel & 0xf) * 0x100;
  switch (code) {
  case 0xa4:
    word = (value & 0x7f00) * 0x100 + 0x70a4;
    r = aica_ring_push(word);
    if (r != 0) return r;
    word = 0xa4;
    word |= (value & 0x7f) << 0x10;
    break;
  case 0x10a4:
    word += (value & 0xf) << 16;
    break;
  case 0x20a4:
    word += (value + 0x40 & 0x7f) << 16;
    break;
  default:
    return -2;
  }
  return aica_ring_push(word);
}

int snd_cmd_a3_post(int code, uint value)
{
  uint hi = value;
  int word = code;
  int r;
  
  if (word == 0xa3) {
    word += (hi & 0xf) << 16;
    r = aica_ring_push(word);
    if (r != 0) return r;
    word += 0x100;
    return aica_ring_push(word);
  }
  return -2;
}

int aica_ring_push(uint word)
{
  int r = 0;
  
  if ((*(volatile int *)&aica_ring_busy) != 0) return 0xfffffffd;
  (*(volatile int *)&aica_ring_busy) = 0xffffffff;
  if ((word & 0x80) == 0) {
    r = 0xfffffffe;
  }
  else if (aica_word_fetch(aica_ring_wp) != 0) {
    r = 0xffffffff;
  }
  else {
    aica_word_store(word,aica_ring_wp);
    aica_ring_wp = aica_ring_wp + 4;
    if (aica_ring_wp == -0x5f7ffb00) {
      aica_ring_wp = -0x5f7ffc00;
    }
  }
  (*(volatile int *)&aica_ring_busy) = 0;
  return r;
}

uint dtpk_slot_volume_read(uint slot, int code)
{
  uint r;
  char *a;
  
  r = 0xffffff80;
  if (slot < 8) {
    a = (char *)(aica_word_fetch(drv_info_addr) + 0xa0800000);
    a = a + slot * 0x20 + 0x14;
    switch (code) {
    case 0x4a0:
      r = aica_word_fetch((uint)a) & 0x7f;
      break;
    case 0x10a0:
      r = (aica_word_fetch((uint)a) & 0x7f00) >> 8;
      break;
    default:
      r = 0xffffff80;
    }
  }
  return r;
}

uint midi_ch_value_read(uint slot, int channel, int code)
{
  uint r;
  uint n;
  uint rec;
  char *a;

  r = 0xffffff80;
  if (slot < 8) {
    n = channel - 1;
    if (n < 0x10) {
      a = (char *)(aica_word_fetch(drv_info_addr + 4) + 0xa0800000);
      a = a + (slot * 0x10 + n) * 4;
      rec = aica_word_fetch((uint)a) & 0xff;
      if (rec == 0xff) {
        r = 0xffffff7f;
      }
      else {
        a = (char *)(aica_word_fetch(drv_info_addr + 8) + 0xa0800000);
        a = a + (rec & 0x3f) * 0x40;
        switch (code) {
        case 0x1b0:
          r = aica_word_fetch((uint)(a + 0x14)) & 0x7f;
          break;
        case 0x2b0:
          r = (aica_word_fetch((uint)(a + 0x14)) & 0x7f00) >> 8;
          break;
        case 0x27b0:
          r = (int)(aica_word_fetch((uint)(a + 8)) & 0x7f0000) >> 16;
          break;
        case 0x2ab0:
          r = (aica_word_fetch((uint)(a + 8)) & 0x1f000000) >> 24;
          r = ((char *)&dipan_to_pan_offset)[r];
          break;
        default:
          r = 0xffffff80;
        }
      }
    }
  }
  return r;
}

int drv_event_word_take(void)
{
  int word;
  int addr;
  
  addr = drv_info_addr + 0x10;
  word = aica_word_fetch(addr);
  if (word != 0) {
    aica_word_store(0,addr);
  }
  return word;
}

int song_state_by_bank(uint bank_no)
{
  int ret;
  int p;
  int i;
  uchar s;
  ret = 0;
  p = aica_word_fetch(drv_info_addr + 0xc) + 0xa0800000;
  for (i = 0; i < 16; i++, p += 0x30) {
    s = aica_word_fetch(p);
    if ((s & 0x80) != 0) {
      uint m = 0xff0000;
      if ((aica_word_fetch(p) & m) >> 16 == bank_no) {
        { int n = 0x100; if ((s & 1) == 0) ret = n; else ret = n + 1; }
        break;
      }
    }
  }
  return ret;
}

uint drv_word_ec_read(void)
{
  uint word;
  
  word = aica_word_fetch(0xa08000ec);
  return word;
}

uint drv_word_e8_read(void)
{
  uint word;
  
  word = aica_word_fetch(0xa08000e8);
  return word;
}

int dtpk_space_left(void)
{
  int free_bytes;
  
  free_bytes = aica_word_fetch(0xa0800088);
  return free_bytes - dtpk_bytes_used;
}

char * libsnd_build_text(void)
{
  return libsnd_version_string;
}

int pcm_ports_lay_out(short mono_ports, short stereo_ports, short third_count)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr0; int buf_addr1; int rate; int offset_word_addr; int channel_word_addr; int drv_rec_addr; } pcm_port;
  short i;
  short channels;
  int stride;
  int free_bytes;
  int top_size;
  int buf_addr;
  int drv_rec;
  if (third_count != 0) return -2;
  channels = stereo_ports * 2 + mono_ports;
  if (channels > 8) return -1;
  for (i = 0; i < 8; i++) {
    pcm_port *t = (pcm_port *)&pcm_port_table;
    if (t[i].state == 1) return -3;
  }
  aica_word_store(mono_ports,0xa08000a0);
  aica_word_store(stereo_ports,0xa08000a4);
  free_bytes = aica_word_fetch(0xa080008c);
  if (channels != 0) {
    pcm_buffer_bytes = ((uint)(free_bytes - channels * 64) / (uint)channels) & 0x7fffffe0;
    if ((int)pcm_buffer_bytes > 0x4000) pcm_buffer_bytes = 0x4000;
    stride = pcm_buffer_bytes + 64;
    top_size = aica_word_fetch(drv_top_size_word);
    if ((top_size & 0x1f) != 0) top_size = (top_size & 0xffffffe0) + 0x20;
    buf_addr = sndram_size - top_size - stride;
    drv_rec = 0xa0800100;
  }
  else {
    stride = 0;
    pcm_buffer_bytes = 0;
  }
  aica_word_store(stride,0xa08000a8);
  for (i = 0; i < mono_ports; i++, drv_rec += 0x60) {
    pcm_port *t = (pcm_port *)&pcm_port_table;
    t[i].state = t[i].playing = 0;
    t[i].drv_rec_addr = drv_rec;
    t[i].buf_addr0 = buf_addr + 0xa0800000;
    aica_word_store(buf_addr,drv_rec + 0xc);
    buf_addr -= stride;
    t[i].buf_addr1 = 0;
    aica_word_store(0,drv_rec + 0x10);
    t[i].offset_word_addr = drv_rec + 0x14;
    t[i].channel_word_addr = drv_rec + 4;
  }
  for (; i < stereo_ports + mono_ports; i++, drv_rec += 0x60) {
    pcm_port *t = (pcm_port *)&pcm_port_table;
    t[i].state = t[i].playing = 0;
    t[i].drv_rec_addr = drv_rec;
    t[i].buf_addr1 = buf_addr + 0xa0800000;
    aica_word_store(buf_addr,drv_rec + 0x10);
    buf_addr -= stride;
    t[i].buf_addr0 = buf_addr + 0xa0800000;
    aica_word_store(buf_addr,drv_rec + 0xc);
    buf_addr -= stride;
    t[i].offset_word_addr = drv_rec + 0x14;
    t[i].channel_word_addr = drv_rec + 4;
  }
  for (; i < 8; i++) {
    pcm_port *t = (pcm_port *)&pcm_port_table;
    t[i].state = 0xff;
    t[i].playing = 0;
    t[i].buf_addr1 = -1;
    t[i].buf_addr0 = -1;
  }
  aica_ring_push(0xa1);
  return 0;
}

int pcm_header_size_get(int header, int *size)
{
  *size = *(uchar *)(header + 7) << 4;
  return header;
}

void pcm_port_header_send(int port, int header)
{
  typedef struct { char state; char playing; short sample_bits; char field_0x4[8]; uint rate; char field_0x10[12]; } pcm_port;
  short i = port;

  if (((pcm_port *)&pcm_port_table)[i].state == 1) {
    if (((pcm_port *)&pcm_port_table)[i].playing == 0) {
      uint *drv_rec = (uint *)(i * 0x60 + -0x5f7ffee0);
      g2_cpu_copy_to_bus((uint *)header,drv_rec,4,0x10,1,1);
      ((pcm_port *)&pcm_port_table)[i].rate = *(ushort *)(header + 0x2a);
      switch(*(uchar *)(header + 8)) {
      case 0:
        ((pcm_port *)&pcm_port_table)[i].sample_bits = 0x10;
        break;
      case 1:
        ((pcm_port *)&pcm_port_table)[i].sample_bits = 8;
        break;
      case 2:
      case 3:
        ((pcm_port *)&pcm_port_table)[i].sample_bits = 4;
        break;
      }
    }
  }
}

int pcm_port_buffer_get(short port, short channel, int *size)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr[2]; int rate; char field_0x10[12]; } pcm_port;
  int *words;

  if (((pcm_port *)&pcm_port_table)[port].state != 1) return 0;
  *size = pcm_buffer_bytes;
  words = (int *)&pcm_port_table;
  return words[port * 7 + channel + 1];
}

int pcm_port_rate_get(short port)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr[2]; int rate; char field_0x10[12]; } pcm_port;

  if (((pcm_port *)&pcm_port_table)[port].state != 1) return 0;
  return ((pcm_port *)&pcm_port_table)[port].rate;
}

int pcm_port_bits_get(short port)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr[2]; int rate; char field_0x10[12]; } pcm_port;

  if (((pcm_port *)&pcm_port_table)[port].state != 1) return 0;
  return ((pcm_port *)&pcm_port_table)[port].sample_bits;
}

uint pcm_port_offset_read(short port)
{
  char *rec = (char *)&pcm_port_table + port * 28;
  if (rec[0] != 1) return 0;
  if (rec[1] == 0) return 0;
  return aica_word_fetch(*(uint *)(rec + 16));
}

int pcm_port_open(short channels)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr0; int buf_addr1; int rate; int offset_word_addr; int channel_word_addr; int drv_rec_addr; } pcm_port;
  short i;
  int found;

  found = 0;
  if (channels == 1) {
    for (i = 0; i < 8; i++) {
      pcm_port *t = (pcm_port *)&pcm_port_table;
      if (t[i].state == 0 && t[i].buf_addr1 == 0) {
        found = 1;
        break;
      }
    }
    if (!found) return -1;
  }
  else if (channels == 2) {
    for (i = 0; i < 8; i++) {
      pcm_port *t = (pcm_port *)&pcm_port_table;
      if (t[i].state == 0 && t[i].buf_addr1 != 0) {
        found = 1;
        break;
      }
    }
    if (!found) return -1;
  }
  else {
    return -1;
  }
  ((pcm_port *)&pcm_port_table)[i].state = 1;
  return i;
}

void pcm_port_close(short port)
{
  typedef struct { char state; char playing; short sample_bits; int buf_addr[2]; int rate; char field_0x10[12]; } pcm_port;

  if (((pcm_port *)&pcm_port_table)[port].state == 1) {
    if (((pcm_port *)&pcm_port_table)[port].playing == 0) {
      ((pcm_port *)&pcm_port_table)[port].state = 0;
    }
  }
  return;
}

void pcm_port_playback_on(int port)
{
  char *rec;
  int i = (short)port;
  
  rec = pcm_port_table + i * 0x1c;
  if ((rec[0] == '\x01') && (rec[1] == '\0')) {
    aica_ring_push(i * 0x1000 + 0x1a1);
    rec[1] = '\x01';
  }
  return;
}

void pcm_port_key_off(short port)
{
  typedef struct { char state; char playing; char field_0x2[18]; uint channel_word_addr; char field_0x18[4]; } pcm_port;
  pcm_port *e;
  int i;
  uint fmt_word;
  uint code;
  uint word_addr;
  uint tries;
  uint reg;
  uint reg_value;
  uint reg2;

  i = port;
  e = &((pcm_port *)&pcm_port_table)[i];
  if (e->state != 1) return;
  if (e->playing == 0) return;
  fmt_word = port * (short)0x60 + 0xa0800128;
  code = i << 12;
  code += 0x2a1;
  word_addr = e->channel_word_addr;
  for (tries = 0; ; ) {
    if (aica_word_fetch(word_addr) != 0) break;
    tries++;
    if (tries >= 0x40000) goto done;
  }
  reg = aica_word_fetch(word_addr) | 0xa0000000;
  reg_value = aica_word_fetch(reg) & 0x7ff;
  aica_word_store(reg_value, reg);
  if ((aica_word_fetch(fmt_word) & 0x300) == 0x100) {
    word_addr = ((pcm_port *)&pcm_port_table)[i].channel_word_addr + 4;
    for (tries = 0; ; ) {
      if (aica_word_fetch(word_addr) != 0) break;
      tries++;
      if (tries >= 0x40000) goto done;
    }
    reg2 = aica_word_fetch(word_addr) | 0xa0000000;
    aica_word_store(aica_word_fetch(reg2) & 0x7ff, reg2);
  }
  reg_value |= 0x8000;
  aica_word_store(reg_value, reg);
done:
  aica_ring_push(code);
  ((pcm_port *)&pcm_port_table)[i].playing = 0;
}

uchar pcm_port_state(int port)
{
  typedef struct { char state; char playing; char field_0x2[22]; int drv_rec_addr; } pcm_port;
  short i = (short)port;
  volatile uint r;
  if (((pcm_port *)&pcm_port_table)[i].state != 1) return 0;
  if (((pcm_port *)&pcm_port_table)[i].playing != 0) return 1;
  r = aica_word_fetch(((pcm_port *)&pcm_port_table)[i].drv_rec_addr);
  if (r & 0x40) return 2;
  {
    int t; if ((r & 0x80) == 0) t = 1; else t = 0; return !t;
  }
}

void empty_fn_0c1136c0(void)
{
  return;
}

void empty_fn_0c1136e0(void)
{
  return;
}

void pcm_header_init(char *header)
{
  char *src;
  int i;

  src = pcm_header_template;
  for (i = 0; i < 0x40; i++) {
    *header++ = *src++;
  }
  return;
}

void pcm_header_pitch_set(int header, short octave, ushort fns)
{
  *(ushort *)(header + 0x22) = fns | (octave << 11);
  return;
}

void pcm_header_channels_set(int header, short channels)
{
  uchar *mode;

  mode = (uchar *)(header + 9);
  switch (channels) {
  case 1:
    *mode = *mode & 0x80;
    break;
  case 2:
    *mode = (*mode & 0x80) | 1;
    break;
  case 4:
    *mode = (*mode & 0x80) | 2;
    break;
  }
  return;
}

void pcm_header_volume_set(int header, short channel, uchar volume)
{
  *(uchar *)(channel * 4 + header + 0x31) = volume;
  return;
}

void pcm_header_pan_set(int header, short channel, uchar pan)
{
  *(uchar *)(channel * 4 + header + 0x32) = pan;
  return;
}

void pcm_header_bits_set(int header, short bits)
{
  switch (bits) {
  case 4:
    *(uchar *)(header + 8) = 3;
    break;
  case 8:
    *(uchar *)(header + 8) = 1;
    break;
  case 16:
    *(uchar *)(header + 8) = 0;
    break;
  }
  return;
}

void pcm_header_dsp_send_set(int header, short dsp_channel, short level)
{
  uchar *p;
  int i;
  int send;

  p = (uchar *)(header + 0x33);
  send = (dsp_channel << 4) | level;
  for (i = 0; i < 4; i++) {
    *p = send;
    p += 4;
  }
  return;
}

void pcm_header_gain_set(int header, short gain)
{
  uchar *p;
  int i;

  p = (uchar *)(header + 0x30);
  for (i = 0; i < 4; i++) {
    *p = gain;
    p += 4;
  }
  return;
}

uint pcm_header_field(int header, uint field, short ch)
{
  int size = *(int *)(header + 0xc);
  uint bits;
  uint channels;
  switch (*(byte *)(header + 9) & 3) {
  case 0:
    channels = 1;
    break;
  case 1:
    channels = 2;
    break;
  case 2:
    channels = 4;
    break;
  }
  switch (*(char *)(header + 8)) {
  case 0:
    bits = 0x10;
    break;
  case 1:
    bits = 8;
    break;
  case 2:
  case 3:
    bits = 4;
    break;
  }
  switch (field) {
  case 0:
    return *(ushort *)(header + 0x2a);
  case 1:
    return bits;
  case 2:
    return (int)((size << 3) / (int)bits) / (int)channels;
  case 3:
    return size;
  case 4:
    return channels;
  case 5:
    return *(byte *)(ch * 4 + header + 0x31);
  case 6:
    return *(byte *)(ch * 4 + header + 0x32);
  case 7:
    return *(char *)(header + 0x33) & 0xf;
  case 8:
    return (uint)(*(char *)(header + 0x33) & 0xf0) >> 4;
  case 9:
    if (*(char *)(header + 0x1f) != 0) return 0;
    size = (ushort)(*(ushort *)(header + 0x16) >> 6);
    if (size == 0) return 1;
    return size;
  case 10:
    return *(byte *)(header + 0x1e);
  }
  return 0xffffffff;
}

void pcm_port_ch_volume_post(short port, short ch, short volume)
{
  if (pcm_port_table[(short)port * 0x1c] == '\x01') {
    aica_ring_push(((int)(short)port << 0xc | (int)(short)volume << 0x10 | 0xa2U) +
                 (short)ch * 0x800);
  }
  return;
}

void pcm_port_volume_offset_post(short port, short offset)
{
  if (pcm_port_table[port * 0x1c] == '\x01') {
    aica_ring_push((int)port << 0xc | (offset + 0x40) << 16 | 0xca1);
  }
}

void pcm_port_ch_pan_post(short port, short ch, short pan)
{
  if (pcm_port_table[(short)port * 0x1c] == '\x01') {
    aica_ring_push(((int)(short)port << 0xc | (int)(short)pan << 0x10 | 0x1a2U) +
                 (short)ch * 0x800);
  }
  return;
}

void pcm_port_semitone_post(short port, short semitones)
{
if (pcm_port_table[port * 0x1c] == 1) { aica_ring_push((int)port << 0xc | (semitones + 0x40) << 16 | 0x8a1); }
}

void pcm_port_fine_pitch_post(short port, short fine_pitch)
{
  if (pcm_port_table[port * 0x1c] == '\x01') {
    aica_ring_push(port << 0xc | (fine_pitch + 0x40) << 16 | 0x9a1);
  }
}

void pcm_port_dsp_send_post(short port, short dsp_ch, short dsp_level)
{
  int i;
  char *rec;
  uint level_word;
  uint ch_word;
  
  i = port;
  rec = (char *)&pcm_port_table + i * 0x1c;
  if (*rec == 1) {
    level_word = i << 0xc | dsp_level << 0x10 | 0x2a2;
    aica_ring_push(level_word);
    ch_word = i << 0xc | dsp_ch << 0x10 | 0x3a2;
    aica_ring_push(ch_word);
    if (*(int *)(rec + 8) != 0) {
      level_word += 0x800;
      aica_ring_push(level_word);
      ch_word += 0x800;
      aica_ring_push(ch_word);
    }
  }
}

void pcm_port_filter_post(short port, short cutoff, short resonance)
{
  int word;
  if (pcm_port_table[port * 0x1c] == '\x01') {
    word = port << 0xc; word |= cutoff << 0x10; word |= 0xaa1;
    aica_ring_push(word);
    word = port << 0xc; word |= resonance << 0x10; word |= 0xba1;
    aica_ring_push(word);
  }
  return;
}

int snd_dma_take_channel(void)
{
  int i;
  uint **ring;
  uint *handle;
  
  libsnd_spare_word = 0;
  handle = snd_dma_handles;
  ring = snd_dma_ring;
  i = 0;
  do {
    *handle = 0;
    handle = handle + 4;
    *ring = 0;
    i = i + 1;
    ring = ring + 1;
  } while (i < 0x10);
  sndram_copy_path_select(1);
  snd_dma_ring_head = snd_dma_ring;
  snd_dma_ring_tail = snd_dma_ring;
  return (g2dma_slot_init_in_use(&snd_dma_channel,0) == 0) - 1;
}

int snd_dma_channel_give_back(void)
{
  int i;
  uint *p = (uint *)&snd_dma_handles;
  for (i = 0; i < 0x10; i++) {
    if (*p & 1) { while (snd_dma_handle_dequeue(p) != 0) ; }
  }
  i = g2dma_slot_mark_unused(snd_dma_channel);
  return (i == 0) - 1;
}

int sndram_copy_path_select(uint use_dma)
{
  int r;
  
  if (use_dma < 2) {
    sndram_copy_by_dma = use_dma;
    r = 0;
  }
  else {
    r = 0xffffffff;
  }
  return r;
}

int sndram_copy_by_dma_get(void)
{
  return sndram_copy_by_dma;
}

uint * snd_dma_handle_alloc(void)
{
  uint *handle;
  int i;

  handle = snd_dma_handles;
  for (i = 0; i < 16; i++) {
    if (*handle == 0) {
      *handle = 1;
      return handle;
    }
    handle += 4;
  }
  return 0;
}

int snd_dma_handle_dequeue(uint *handle)
{
  uint **slot;
  uint **next;
  
  if ((*handle & 1) == 0) return 0xffffffff;
  if (snd_dma_handle_poll(handle) != 0x100) {
    if ((*handle & 0x100) != 0) {
      snd_dma_cut_and_advance(handle);
    }
    else if ((*handle & 0x200) != 0) {
      slot = snd_dma_ring_head;
      while (*slot != handle) {
        slot++;
        if (slot == snd_dma_ring + 16) slot -= 16;
      }
      next = slot + 1;
      if (next == snd_dma_ring + 16) next -= 16;
      while (next != snd_dma_ring_tail) {
        *slot = *next++;
        if (next == snd_dma_ring + 16) next -= 16;
        slot++;
        if (slot == snd_dma_ring + 16) slot -= 16;
      }
      snd_dma_ring_tail = slot;
    }
    else {
      return 0xffffff80;
    }
  }
  *handle = 0;
  return 0;
}

int snd_dma_copy_queue(uint *handle, uint src, uint dst, uint size)
{
  if ((*handle & 1) == 0) {
    return 0xffffffff;
  }
  if ((src & 0x1f) != 0) {
    return 0xfffffffe;
  }
  if ((dst & 0x1f) != 0) {
    return 0xfffffffd;
  }
  if ((size & 0x1f) != 0) {
    return 0xfffffffc;
  }
  if (sndram_copy_by_dma == 0) {
    g2_cpu_copy_to_bus((uint *)src,(uint *)dst,4,size >> 2,1,1);
    *handle = *handle & 0xfffffcff;
  }
  else if (sndram_copy_by_dma == 1) {
    if (snd_dma_handle_poll(handle) != 0x100) {
      return 0xfffffffb;
    }
    handle[1] = src;
    handle[2] = dst;
    handle[3] = size;
    *handle = *handle | 0x200;
    *snd_dma_ring_tail = handle;
    if (snd_dma_ring_tail == snd_dma_ring_head) {
      snd_dma_handle_program_start(handle);
    }
    snd_dma_ring_tail = snd_dma_ring_tail + 1;
    if (snd_dma_ring_tail == snd_dma_ring + 16) {
      snd_dma_ring_tail = snd_dma_ring;
    }
  }
  else {
    return 0xffffff80;
  }
  return 0;
}

int snd_dma_cut_and_advance(uint *handle)
{
  if ((*handle & 1) == 0) {
    return -1;
  }
  if ((*handle & 0x100) == 0) {
    return -2;
  }
  if (snd_dma_handle_poll(handle) != 0x101) {
    return -3;
  }
  g2dma_start_bit_clear(snd_dma_channel);
  snd_dma_ring_advance();
  return 0;
}

int snd_dma_handle_poll(uint *handle)
{
  char started[4];
  int r;
  int sr;
  int imask;
  uint *s = handle;
  
  if (sndram_copy_by_dma == 0) return 0x100;
  if (sndram_copy_by_dma == 1) {
    __asm__ ("stc sr,%0" : "=r"(sr));
    sr &= 0xf0;
    imask = sr >> 4;
    g2dma_start_bit_read(snd_dma_channel, (byte *)started);
    if ((*s & 1) == 0) {
      r = -1;
    }
    else if (*s & 0x100) {
      r = (started[0] == 1) ? 0x101 : 0x100;
    }
    else {
      r = (*s & 0x200) ? 0x101 : 0x100;
    }
    if (started[0] == 0 && snd_dma_ring_tail != snd_dma_ring_head) {
      if (imask == 0) {
        snd_dma_ring_advance();
      }
      else {
        g2dma_done_isr(snd_dma_channel);
      }
    }
    return r;
  }
  return -0x80;
}

int snd_dma_handle_program_start(uint *handle)
{
  g2dma_slot_store_transfer(snd_dma_channel,handle[2],handle[1],handle[3],0,0,1,(g2_dma_done_fn)snd_dma_end_callback,0);
  *handle = *handle | 0x100;
  g2dma_regs_load_and_start(snd_dma_channel,0);
  return 0;
}

void snd_dma_ring_advance(void)
{
  **snd_dma_ring_head &= 0xfffffcff;
  snd_dma_ring_head = snd_dma_ring_head + 1;
  if (snd_dma_ring_head == snd_dma_ring + 16) {
    snd_dma_ring_head = snd_dma_ring;
  }
  if (snd_dma_ring_tail != snd_dma_ring_head) {
    snd_dma_handle_program_start(*snd_dma_ring_head);
  }
}

void snd_dma_end_callback(void)
{
    if (snd_dma_ring_tail != snd_dma_ring_head)
        snd_dma_ring_advance();
}
