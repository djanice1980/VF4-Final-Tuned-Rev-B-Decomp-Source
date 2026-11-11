/*
 * vf4lib/libsound/src/sound_cmn.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int vblank_count; /* 0x0c292544 */
extern int sound_load_vblanks; /* 0x0c29ff30 */
extern char sound_pack_name[68]; /* 0x0c37d59c */
extern char sound_pack_header[64]; /* 0x0c37d6d4 */
extern sound_reload_entry sound_reload_queue[9]; /* 0x0c37d614 */
extern sound_rec sound_state; /* 0x0c37d65c */
extern sound_reload_rec sound_reload_job; /* 0x0c37d714 */
extern int sound_bank_slot_sizes[8]; /* 0x0c29ff44 */
extern short sound_default_banks[8]; /* 0x0c29ff64 */
extern short sound_slot_table_a[8]; /* 0x0c37d58c */
extern char sound_driver_id[33]; /* 0x0c37d5e0 */
extern char *sound_bank_files[80]; /* 0x0c29ffd4 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */

void sound_driver_id_copy(char *src)
{
  char c;
  uint i;
  
  i = 0;
  do {
    c = *src;
    src = src + 1;
    sound_driver_id[i] = c;
    i = i + 1 & 0xff;
  } while (i < 0x20);
  sound_driver_id[0x20] = 0;
  return;
}

void sound_system_init(void)
{
  sound_rec *s = &sound_state;
  void *drv;
  uint *config;
  int i;

  drv = heap_texdata.addr;
  load_file("aicadrv.bin",heap_texdata.addr,heap_texdata.size);
  sound_driver_id_copy((char *)drv + 0x20);
  s->boot_result = aica_driver_boot((int)drv,0x200);
  if (s->boot_result != 0) for (;;);
  sound_state_clear();
  for (i = 0; i < 8; i++) {
    if ((short)sound_default_banks[i] != -1) {
      sound_pack_load_lock(i,sound_bank_files[(short)sound_default_banks[i]],sound_bank_slot_sizes[i]);
    }
  }
  for (i = 0; i < 9; i++) {
    sound_reload_queue[i].slot = -1;
    sound_reload_queue[i].name = 0;
  }
  sound_reload_job.list = (int *)&sound_reload_queue;
  sound_reload_job.state = 0;
  sound_reload_job.abort_step = 0;
  s->cmd_result = snd_cmd_a0_post(0,0x100a0,0);
  config = board_config_ptr();
  attract_sound_setting = config[1];
  sound_init_hook_a();
  sound_init_hook_b();
  s->libsnd_build = libsnd_build_text();
  snd_cmd_a4_post(0,0xa4,0);
  s->req_peak = s->req_count = 0;
}

void sound_state_clear(void)
{
  int i;
  short *a;
  short *b;

  sound_state.flags = 0;
  sound_state.field_0x49 = 0;
  sound_state.field_0x48 = 0;
  i = 0;
  a = sound_slot_table_a;
  b = sound_slot_table_b;
  for (; i < 8; i++) {
    b[i] = -1;
    a[i] = -1;
  }
}

void sound_init_hook_a(void)
{
  return;
}

void sound_init_hook_b(void)
{
  return;
}

void sound_effect_defaults_post(void)
{
  sound_state.fx_result = snd_cmd_a4_post(0,0xa4,0);
  snd_cmd_a4_post(0,0x10a4,0xf);
  snd_cmd_a4_post(1,0x10a4,0xf);
  snd_cmd_a4_post(2,0x10a4,0xf);
  snd_cmd_a4_post(3,0x10a4,0xf);
  return;
}

uint sound_request_post(int request)
{
  uint r;
  sound_rec *p;
  
  if (request != 0) {
    p = &sound_state;
    p->req_count = p->req_count + 1;
    p->last_request = request;
    r = aica_ring_push(request);
    p->ring_result = r;
    return r;
  }
  return 0xffffffff;
}

void sound_all_off(void)
{
  int i;
  
  sound_state.cmd_result = snd_cmd_a0_post(0,0x100a0,0);
  i = 0;
  do {
    snd_cmd_a5_post(i,0xa6,0);
    i = i + 1;
  } while (i < 8);
  return;
}

void bgm_sound_off(void)
{
  snd_cmd_a0_post(7,0x1100a0,0);
  return;
}

void voice_slots_off(void)
{
  snd_cmd_a0_post(4,0x1100a0,0);
  snd_cmd_a0_post(6,0x1100a0,0);
  return;
}

void sound_slots_1_3_5_off(void)
{
  snd_cmd_a0_post(1,0x1100a0,0);
  snd_cmd_a0_post(3,0x1100a0,0);
  snd_cmd_a0_post(5,0x1100a0,0);
  return;
}

int sound_pack_load_lock(int slot, char *name, int size)
{
  dimm_file *fp;
  int err;
  
  strcpy(sound_pack_name,name);
  fp = sound_pack_open(name);
  err = dtpk_insert_sized_fed_begin((int)sound_pack_header,slot,0x100,size);
  if (err != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x172,"packloadlock_wait ERROR.");
  }
  sound_pack_feed(fp);
  return 0;
}

int sound_pack_load(int slot, char *name)
{
  dimm_file *fp;
  int err;
  
  strcpy(sound_pack_name,name);
  fp = sound_pack_open(name);
  err = dtpk_insert_fed_begin((int)sound_pack_header,slot,0x100);
  if (err != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x180,"packload_wait ERROR.");
  }
  sound_pack_feed(fp);
  return 0;
}

int sound_pack_reload(int slot, char *name)
{
  dimm_file *fp;
  int err;
  
  strcpy(sound_pack_name,name);
  fp = sound_pack_open(name);
  err = dtpk_overwrite_fed_begin(sound_pack_header,slot,0x100);
  if (err != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x18e,"packreload_wait ERROR.");
  }
  sound_pack_feed(fp);
  return 0;
}

void sound_reload_queue_add(int slot, char *name)
{
  int i;
  sound_reload_entry *e;

  e = sound_reload_queue;
  i = 0;
  do {
    if (e->slot == -1) {
      e->slot = slot;
      e->name = name;
      return;
    }
    i = i + 1;
    e++;
  } while (i < 8);
  return;
}

void sound_reload_queue_clear(void)
{
  sound_reload_entry *e;
  int i;

  e = sound_reload_queue;
  i = 0;
  do {
    e->slot = -1;
    e->name = 0;
    i = i + 1;
    e++;
  } while (i < 9);
  return;
}

void sound_reload_bg_poll(void)
{
  if ((sound_state.flags & 0x10000) != 0) {
    if (!((sound_state.flags & 0x20000) == 0)) sound_reload_abort_step(); else sound_reload_queue_step();
  }
  return;
}

void sound_reload_queue_step(void)
{
  sound_reload_rec *p = &sound_reload_job;
  dimm_file *fp;
  float t;
  int n;

  fp = p->fp;
  if (*p->list == -1) {
    sound_state.flags = sound_state.flags & 0xfffeffff;
    return;
  }
  for (;;) {
    t = (float)(int)stopwatch_usec();
    if (14000.0 <= t) break;
    switch (p->state) {
    case 0:
      strcpy(sound_pack_name, (char *)p->list[1]);
      fp = sound_pack_open((char *)p->list[1]);
      if (dtpk_overwrite_fed_begin(sound_pack_header, p->list[0], 0x100) != 0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c", 0x1e1, "nlsndPackReload_Open ERROR.");
      }
      p->state++;
      break;
    case 1:
      if (dtpk_job_step() == 0) {
        p->addr = (char *)(dtpk_feed_dest() & 0xffffff);
        p->size = dtpk_feed_bytes_left();
        if ((p->size & 0x1f) != 0) {
          fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c", 0x1ec, "pack size ERROR.");
        }
        p->state++;
        dimm_fseek(fp, 0, 0);
      }
      break;
    case 2:
      n = sound_reload_step_bytes(t);
      if (n == 0) goto done;
      if (n > p->size) n = p->size;
      dimm_fread_dma(p->addr, n, 1, fp);
      while (dimm_fbusy(fp) != 0) ;
      p->addr += n;
      p->size -= n;
      if (p->size <= 0) p->state++;
      goto done;
    case 3:
      dtpk_feed_done();
      p->state++;
      break;
    case 4:
      if (dtpk_job_step() == 0) {
        dimm_fclose(fp);
        p->list[0] = -1;
        p->list[1] = 0;
        p->list += 2;
        p->state = 0;
        if (*p->list == -1) {
          p->list = (int *)&sound_reload_queue;
          sound_state.flags = sound_state.flags & 0xfffeffff;
          sound_load_vblanks = vblank_count - sound_load_vblanks;
          goto done;
        }
      }
      break;
    }
  }
done:
  p->fp = fp;
}

void sound_reload_abort_step(void)
{
  sound_reload_rec *p = &sound_reload_job;
  dimm_file *fp = p->fp;
  int r;

  while (1) {
    if (14000.0 <= (float)(int)stopwatch_usec()) return;
    switch (p->abort_step) {
    case 0:
      switch (p->state) {
      case 0:
        sound_state.flags &= 0xfffdffff;
        p->list = (int *)&sound_reload_queue;
        break;
      case 1:
      case 2:
      case 3:
        p->abort_step = 1;
        break;
      case 4:
        p->abort_step = 2;
        break;
      }
      break;
    case 1:
      dtpk_job_step();
      if (dtpk_job_blank_out() == 0) p->abort_step++;
      break;
    case 2:
      r = dtpk_job_step();
      if (r == 0) {
        dimm_fclose(fp);
        sound_state.flags &= 0xfffdffff;
        p->abort_step = r;
        p->state = r;
        p->list = (int *)&sound_reload_queue;
      }
      break;
    }
  }
}

dimm_file * sound_pack_open(char *name)
{
  dimm_file *fp;
  uint n;
  
  fp = dimm_fopen(name,"rb");
  if (fp == (dimm_file *)0x0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x268,"%s open error",name);
  }
  n = dimm_fread_pio(sound_pack_header,0x40,1,fp);
  if (n == 0xffffffff) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x26b,"%s read error",name);
  }
  return fp;
}

void sound_pack_feed(dimm_file *fp)
{
  void *addr;
  uint size;
  while (dtpk_job_step() != 0) ;
  addr = (void *)(dtpk_feed_dest() & 0xffffff);
  size = dtpk_feed_bytes_left();
  if ((size & 0x1f) != 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsound/src/sound_cmn.c",0x27c,"PACKLOAD size error");
  }
  dimm_fseek(fp,0,0);
  dimm_fread_dma(addr,size,1,fp);
  while (dimm_fbusy(fp) != 0) ;
  dtpk_feed_done();
  while (dtpk_job_step() != 0) ;
  dimm_fclose(fp);
}

int sound_reload_step_bytes(float now_us)
{
  return sound_reload_bytes_per_100us * ((int)(15000.0 - now_us) / 100);
}
