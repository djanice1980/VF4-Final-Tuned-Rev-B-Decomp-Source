/*
 * vf4lib/libcore/src/sffs.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int sffs_mounted; /* 0x0c325504 */
extern char sffs_volume_header[64]; /* 0x0c55dc68 */
extern int sffs_bank; /* 0x0c55dca8 */
extern uint sffs_bank_size; /* 0x0c55dcac */
extern uint rom_port_mode_bits; /* 0x0c55dcb0 */
extern uint rom_port_bank_bit; /* 0x0c55dcb4 */
extern int g1_bus_hold_count; /* 0x0c55dcb8 */
extern int dimm_dma_active; /* 0x0c55dcbc */
extern uint dimm_dma_next_key; /* 0x0c55dcc0 */
extern int dimm_dma_cur; /* 0x0c55dcc4 */
extern int dimm_dma_started; /* 0x0c55dcc8 */
extern int dimm_dma_queued; /* 0x0c55dccc */
extern void *dimm_irq_prev_handler; /* 0x0c55dcd0 */
extern dimm_dma_rec dimm_dma_queue[32]; /* 0x0c55dcd4 */
extern void *dimm_dma_done_hook; /* 0x0c55e154 */
extern char sffs_cwd[128]; /* 0x0c5b25d8 */

static inline unsigned int get_sr(void)
{
  unsigned int sr;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  return sr;
}

int string_hash_x5(char *str)
{
  uchar *s = (uchar *)str;
  int h = 0;
  while (*s != 0) {
    h = h * 5 + *s;
    s++;
  }
  return h;
}

void spin_delay(int count)
{
  volatile int i;
  for (i = 0; i < count * 100; i = i + 1) {
  }
  return;
}

void g1_bus_hold(void)
{
  g1_bus_hold_count = g1_bus_hold_count + 1;
  while ((dimm_dma_active != 0 || (g1_dma_status_reg != 0))) {
    spin_delay(2);
  }
  return;
}

void g1_bus_release(void)
{
  g1_bus_hold_count = g1_bus_hold_count + -1;
  if ((g1_bus_hold_count == 0) && (g1_dma_status_reg == 0)) {
    dimm_dma_kick();
  }
  return;
}

void * irq_handler_swap(int code, void *handler)
{
  unsigned int sr;
  unsigned int old;
  void *prev;
  int a;
  int b;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  old = sr;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  a = code - 0x200;
  b = (uint)a >> 3;
  a = (int)(irq_handler_table + b);
  prev = *(void **)a;
  *(void **)a = handler;
  *(void **)((uint)a | 0x80000000) = handler;
  *(void **)((uint)a | 0xa0000000) = handler;
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return prev;
}

int rom_port_write32(uint offset, uint value)
{
  *(volatile uint *)0xa05f7000 = rom_port_mode_bits | rom_port_bank_bit | offset >> 0x10 | 0x8000;
  *(volatile uint *)0xa05f7004 = offset & 0xfffe;
  *(volatile uint *)0xa05f7008 = value & 0xffff;
  *(volatile uint *)0xa05f7008 = value >> 0x10;
  return 0;
}

int dimm_write(uint *src, int offset, int size)
{
  uint *p;
  uint n;
  int a;
  g1_bus_hold();
  p = src;
  a = offset;
  for (n = size + 3U >> 2; n != 0; n--) {
    rom_port_write32(a, *p);
    p++;
    a += 4;
  }
  g1_bus_release();
  return 0;
}

int dimm_read_unlocked(void *dst, uint offset, int size)
{
  uint *p;
  uint n, lo, hi;
  p = (uint *)((uint)dst & 0xfffffffc);
  n = size + 3U >> 2;
  cart_control_reg = rom_port_mode_bits | rom_port_bank_bit | offset >> 0x10 | 0x8000;
  cart_addr_low_reg = offset & 0xfffe;
  for (; n != 0; n--) {
    lo = cart_data_reg;
    hi = cart_data_reg;
    *p = hi << 16 | lo & 0xffff;
    p++;
  }
  return 0;
}

int dimm_read(void *dst, uint offset, int size)
{
  int r;

  g1_bus_hold();
  r = dimm_read_unlocked(dst,offset,size);
  g1_bus_release();
  return r;
}

void dimm_dma_wait_ack(void)
{
  uint status;
  volatile int busy;

  busy = g1_dma_status_reg;
  while (busy == 1) {
    spin_delay(2);
    busy = g1_dma_status_reg;
  }
  spin_delay(2);
  status = irq_status_reg;
  if ((status & 0x4000) != 0) {
    irq_status_reg = 0x4000;
  }
  cart_dma_ctrl_reg = 0x4000;
  return;
}

void dimm_dma_abort(void)
{
  dimm_dma_rec *e;
  unsigned int sr;
  int i;

  sr = get_sr();
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  if (dimm_dma_active != 0) {
    *(volatile uint *)0xa05f7414 = 0;
    for (i = 0; i < 99; i++) {
      if ((*(volatile uint *)0xa05f7418 & 1) == 0) break;
      spin_delay(2);
    }
    if (i == 99) {
      __asm__ volatile ("trapa #0x50");
    }
    if ((*(volatile uint *)0xa05f6900 & 0x4000) != 0) {
      *(volatile uint *)0xa05f6900 = 0x4000;
    }
    *(volatile uint *)0xa05f700c = 0x4000;
    e = &dimm_dma_queue[dimm_dma_cur];
    e->offset -= e->chunk_size;
    e->dst -= e->chunk_size;
    e->remaining += e->chunk_size;
    dimm_dma_active = 0;
    dimm_dma_started = 0;
  }
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

int dimm_dma_start(uint dst, uint offset, uint size)
{
  dimm_dma_started = 1;
  *(volatile uint *)0xa05f7000 = rom_port_mode_bits | rom_port_bank_bit | 0x8000;
  *(volatile uint *)0xa05f7004 = 0;
  *(volatile uint *)0xffa00040;
  *(volatile uint *)0xffa00040 = 0x8201;
  *(volatile uint *)0xa05f7010 = offset & 0xfffe;
  *(volatile uint *)0xa05f700c = offset >> 0x10 | rom_port_bank_bit | 0x8000;
  *(volatile uint *)0xa05f74b8 = 0x8843085f;
  *(volatile uint *)0xa05f7404 = dst;
  *(volatile uint *)0xa05f7408 = size;
  *(volatile uint *)0xa05f740c = 1;
  *(volatile uint *)0xa05f7414 = 1;
  *(volatile uint *)0xa05f7418 = 1;
  return 0;
}

void dimm_dma_chunk_start(dimm_dma_rec *e)
{
  uint head;
  head = (0x20 - e->dst % 0x20) % 0x20;

  if (head != 0) {
    dimm_read_unlocked((void *)e->dst,e->offset,head);
    e->dst = e->dst + head;
    e->offset = e->offset + head;
    e->remaining = e->remaining - head;
    e->done = e->done + head;
  }
  if (e->remaining < 0x8000) {
    e->chunk_size = e->remaining & 0xffffffe0;
  }
  else {
    e->chunk_size = 0x8000;
  }

  e->chunk_dst = e->dst;
  e->chunk_offset = e->offset;
  e->offset = e->offset + e->chunk_size;
  e->dst = e->dst + e->chunk_size;

  e->remaining = e->remaining - e->chunk_size;
  ((void (*)(int, int))dcache_discard_range_uncached)(e->chunk_dst, e->chunk_size);
  dimm_dma_start(e->chunk_dst,e->chunk_offset,e->chunk_size);
}

void dimm_dma_kick(void)
{
  int u;
  int i;
  unsigned int s[1];
  unsigned int old;

  if (dimm_dma_queued != 0) {
    __asm__ volatile ("stc sr,%0" : "=r" (s[0]));
    old = s[0];
    __asm__ volatile ("ldc %0,sr" : : "r" (s[0] | 0xf0));
    dimm_dma_active = 1;
    u = dimm_dma_cur;
    for (i = 0; i < 0x20; i++) {
      dimm_dma_rec *t = dimm_dma_queue;
      if (t[u].busy != 0) {
        dimm_dma_cur = u;
        dimm_dma_chunk_start(&dimm_dma_queue[u]);
        goto out;
      }
      u++;
      if (u >= 0x20) u = 0;
    }
    dimm_dma_active = 0;
  out:
    __asm__ volatile ("ldc %0,sr" : : "r" (old));
  }
}

int dimm_dma_irq(void)
{
  dimm_dma_rec *e;
  int i;
  int r;

  if ((*(volatile uint *)0xa05f6900 & 0x4000) == 0) {
    return ((int (*)(void))dimm_irq_prev_handler)();
  }
  for (i = 0; i < 100; i++) {
    if ((*(volatile uint *)0xa05f7418 & 1) == 0) break;
  }
  if (i == 100) return -1;
  *(volatile uint *)0xa05f6900 = 0x4000;
  *(volatile uint *)0xa05f7414 = 0;
  if (dimm_dma_started == 0) return 0;
  dimm_dma_started = 0;
  *(volatile uint *)0xa05f700c = 0x4000;
  e = &dimm_dma_queue[dimm_dma_cur];
  e->done += e->chunk_size;
  if (e->remaining < 32) {
    if (e->remaining != 0) {
      dimm_read_unlocked((void *)e->dst, e->offset, e->remaining);
      e->done += e->remaining;
      e->dst += e->remaining;
      e->offset += e->remaining;
      e->remaining = 0;
    }
    e->busy = 0;
    if (dimm_dma_done_hook != 0) {
      r = ((int (*)(dimm_dma_rec *))dimm_dma_done_hook)(e);
      if (r != 0) return r;
    }
    e->key = 0;
    dimm_dma_queued--;
  }
  if ((int)++dimm_dma_cur > 31) dimm_dma_cur = 0;
  dimm_dma_active = 0;
  if (g1_bus_hold_count == 0) dimm_dma_kick();
  return 0;
}

uint dimm_dma_queue_add(uint dst, uint offset, uint size)
{
  dimm_dma_rec *e;
  int i;
  int idx;
  uint r;

  if ((((dst | offset | size) & 3) != 0) | (size <= 0x3f)) {
    return 0;
  }
  idx = -1;
  for (i = 0; i < 0x20; i++) {
    dimm_dma_rec *t = dimm_dma_queue;
    if (t[i].busy == 0) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    return 0;
  }
  e = &dimm_dma_queue[idx];
  e->dst = dst;
  e->offset = offset;
  e->remaining = size;
  e->done = 0;
  e->busy = 1;
  r = dimm_dma_next_key;
  e->key = r;
  dimm_dma_next_key = r + 2 & 0xffffff;
  dimm_dma_queued = dimm_dma_queued + 1;
  return r;
}

dimm_dma_rec * dimm_dma_find(uint key)
{
  int i;
  dimm_dma_rec *tab;

  if (key != 0) {
    i = 0;
    for (tab = dimm_dma_queue; i < 0x20; i++) {
      if (key == tab[i].key) {
        return &tab[i];
      }
    }
  }
  return (dimm_dma_rec *)0x0;
}

void sffs_load_dir(void)
{
  uint remaining;
  uint offset;
  int n;
  int t;

  offset = sffs_bank * sffs_bank_size + 0x1000;
  n = 0;
  dimm_read(sffs_dir,offset,0x100);
  remaining = sffs_dir[0].remaining;
  while (remaining != 0xffffffff) {
    sffs_dir[n].image_offset = offset;
    if (sffs_dir[n].taken == 0xffffffff) {
      offset = offset + sffs_dir[n].remaining;
    }
    else {
      offset = offset + sffs_dir[n].taken;
      t = sffs_dir[n].alive == 0;
      n += t == 0;
      if (0x4b0 < n) {
        __asm__ volatile ("trapa #0x50");
      }
    }
    dimm_read(sffs_dir + n,offset,0x100);
    remaining = sffs_dir[n].remaining;
  }
  return;
}

sffs_entry * sffs_find(char *path)
{
  int i;
  for (i = 0; sffs_dir[i].remaining != 0xffffffff; i++) {
    if (strncmp(sffs_dir[i].name, path, 0x80) == 0) return &sffs_dir[i];
  }
  return 0;
}

void sffs_mount(void)
{
  int r;
  int i;
  char *p;
  unsigned int sr;
  unsigned int old;

  *(volatile uint *)0xa05f7490 = 0x511;
  *(volatile uint *)0xa05f7494 = 0x511;
  *(volatile uint *)0xa05f74a0 = 0x1106;
  *(volatile uint *)0xa05f74a4 = 0x1106;
  rom_port_bank_bit = 0;
  sffs_bank = 1;
  rom_port_mode_bits = 0x2000;
  sffs_bank_size = 0x800000;
  for (i = 0; i < 2; i++) {
    dimm_read_unlocked(sffs_volume_header,sffs_bank * sffs_bank_size,0x40);
    r = memcmp(sffs_volume_header,"SimpleFlashFS",0xd);
    if (r == 0) break;
    rom_port_bank_bit = rom_port_bank_bit ^ 0x1000;
  }
  if (i == 2) {
    __asm__ volatile ("trapa #0x50");
  }
  p = &sffs_volume_header[0x20];
  memcmp(p,"1999.5.5",9);
  if (*(uint *)(p + 0x14) == 0x400000) {
    rom_port_mode_bits = 0;
    sffs_bank_size = *(uint *)(p + 0x14);
  }
  g1_bus_hold_count = 0;
  dimm_dma_active = 0;
  for (i = 0; i < 0x20; i++) {
    dimm_dma_rec *q = dimm_dma_queue;
    q[i].busy = 0;
    q[i].key = 0;
  }
  dimm_dma_cur = 0;
  dimm_dma_next_key = 1;
  dimm_dma_done_hook = 0;
  dimm_dma_started = 0;
  dimm_dma_queued = 0;
  if (sffs_mounted == 0) {
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    old = sr;
    sr |= 0xf;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
    dimm_irq_prev_handler = irq_handler_swap(800,dimm_dma_irq);
    *(volatile uint *)0xa05f6930 |= 0x4000;
    *(volatile uint *)0xa05f6920 &= 0xffffbfff;
    __asm__ volatile ("ldc %0,sr" : : "r" (old));
  }
  sffs_mounted = 1;
}

int dimm_dma_done_release(dimm_dma_rec *e)
{
  int i;

  for (i = 0; i < 32; i++) {
    if (dimm_files[i].dma == (void *)e->key) break;
  }
  if (i == 32) return -1;
  dimm_files[i].dma = (void *)0x0;
  return 0;
}

int dimm_fbusy(dimm_file *fp)
{
  unsigned int sr;
  unsigned int old;
  unsigned int s[1];
  dimm_dma_rec *p;
  int r;

  __asm__ volatile ("stc sr,%0" : "=r" (s[0]));
  old = s[0];
  sr = s[0] | 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  p = dimm_dma_find((uint)fp->dma);
  if (p != 0) {
    r = p->busy;
  }
  else {
    r = 0;
  }
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return r;
}

uint dimm_fread_progress(dimm_file *fp)
{
  unsigned int sr;
  unsigned int old;
  unsigned int s[1];
  dimm_dma_rec *p;
  uint r;

  if (fp->size == -1) {
    return 0xffffffff;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (s[0]));
  old = s[0];
  sr = s[0] | 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  p = dimm_dma_find((uint)fp->dma);
  if (p == 0) {
    r = fp->dma_items;
  }
  else {
    r = p->done / fp->dma_item_size;
  }
  __asm__ volatile ("ldc %0,sr" : : "r" (old));
  return r;
}

void dimm_files_init(void)
{
  int i;

  for (i = 0; i < 0x20; i++) {
    dimm_files[i].size = -1;
    dimm_files[i].area_ptr = dimm_files[i].area;
    dimm_files[i].field_0x494 = -1;
    dimm_files[i].dma = (void *)0x0;
  }
  dimm_dma_done_hook = &dimm_dma_done_release;
  sffs_load_dir();
  sffs_cwd[0] = 0;
}

void path_resolve(char *out, char *in)
{
  char *tok;
  int r;
  size_t len;
  char buf [125];

  strcpy(out,sffs_cwd);
  strcpy(buf,in);
  if (buf[0] == '/') {
    *out = '\0';
  }
  tok = strtok(buf,"/");
  while (tok != (char *)0x0) {
    r = memcmp(tok,".",2);
    if (r != 0) {
      r = memcmp(tok,"..",3);
      if (r == 0) {
        tok = strrchr(out,0x2f);
        if (tok == (char *)0x0) {
          *out = '\0';
        }
        else {
          *tok = '\0';
        }
      }
      else {
        if (*out != '\0') {
          strcat(out,"/");
        }
        strcat(out,tok);
        len = strlen(out);
        if (0x7f < len) {
          __asm__ volatile ("trapa #0x50");
        }
      }
    }
    tok = strtok((char *)0x0,"/");
  }
  return;
}

int sffs_chdir(uchar *path)
{
  sffs_entry *e;
  char buf [128];

  if (*path == 0) {
    sffs_cwd[0] = 0;
  }
  else {
    path_resolve(buf,(char *)path);
    if (buf[0] != '\0') {
      e = sffs_find(buf);
      if (e == (sffs_entry *)0x0) {
        return -1;
      }
      if ((e->mode & 0x4000) == 0) {
        return -1;
      }
    }
    strcpy(sffs_cwd,buf);
  }
  return 0;
}

dimm_file * dimm_fopen(char *name, char *mode)
{
  sffs_entry *e;
  dimm_file *f;
  int i;
  uint start;
  char buf [128];

  if (*mode != 'r') return 0;
  f = 0;
  for (i = 0; i < 0x20; i++) {
    if (dimm_files[i].size == -1) {
      f = &dimm_files[i];
      break;
    }
  }
  if (f == 0) return 0;
  path_resolve(buf,name);
  e = sffs_find(buf);
  if (e == 0) return 0;
  f->size = e->size;
  start = e->image_offset + 0x100;
  f->start = start;
  f->end = e->size + start;
  f->pos = start;
  strncpy(f->name,buf,0x7f);
  return f;
}

int dimm_fclose(dimm_file *fp)
{
  if (fp->size == -1) return -1;
  while (dimm_fbusy(fp) != 0) {
    if ((dimm_dma_active == 0) && (g1_dma_status_reg == 0)) {
      dimm_dma_kick();
    }
  }
  fp->size = -1;
  return 0;
}

uint dimm_fread_pio(void *dst, uint size, uint n, dimm_file *fp)
{
  uint i;
  char *d;

  if (fp->size == -1) return -1;
  d = dst;
  if ((((uint)d | fp->pos) & 3) != 0) return -1;
  if (n > 1 && (size & 3) != 0) return -1;
  for (i = 0; i < n; i++) {
    if (size + fp->pos > fp->end) {
      dimm_read(d,fp->pos,fp->end - fp->pos);
      fp->pos = fp->end;
      break;
    }
    dimm_read(d,fp->pos,size);
    d += size;
    fp->pos = size + fp->pos;
    if (fp->pos >= fp->end) break;
  }
  return i;
}

int dimm_fread_dma(void *dst, uint size, uint n, dimm_file *fp)
{
  int r;
  uint len;
  unsigned int sr;
  unsigned int s[1];

  if (fp->size == -1) return -1;
  if ((((uint)dst | fp->pos) & 3) != 0) return -1;
  if (fp->dma != (void *)0x0) return -1;
  len = size * n + 3 & 0xfffffffc;
  if (len + fp->pos > fp->end) {
    len = (fp->end - fp->pos) + 3 & 0xfffffffc;
  }
  if (len == 0) return -1;
  if (len < 0x40) {
    dimm_read(dst,fp->pos,len);
    fp->dma_items = len / size;
    return 0;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (s[0]));
  sr = s[0];
  __asm__ volatile ("ldc %0,sr" : : "r" (s[0] | 0xf0));
  fp->dma = (void *)dimm_dma_queue_add((uint)dst,fp->pos,len);
  if (fp->dma != (void *)0x0) {
    fp->dma_items = len / size;
    fp->dma_item_size = size;
    fp->pos = fp->pos + len;
    r = 0;
  }
  else {
    r = -1;
  }
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (g1_bus_hold_count == 0 && dimm_dma_active == 0 && g1_dma_status_reg == 0) {
    dimm_dma_kick();
  }
  return r;
}

int dimm_fseek(dimm_file *fp, int offset, int whence)
{
  if (fp->size == -1) {
    return -1;
  }
  switch (whence) {
  case 0:
    fp->pos = fp->start + offset;
    break;
  case 1:
    fp->pos = fp->pos + offset;
    break;
  case 2:
    fp->pos = fp->end + offset;
    break;
  default:
    return -1;
  }
  if (fp->pos < fp->start || fp->pos > fp->end) {
    return -1;
  }
  return 0;
}

int dimm_ftell(dimm_file *fp)
{
  if (fp->size == -1) {
    return -1;
  }
  if (fp->pos < fp->start || fp->pos > fp->end) {
    return -1;
  }
  return fp->pos - fp->start;
}

int dimm_fcheck(dimm_file *fp)
{
  if (fp->size == -1) return -1;
  if (fp->pos < fp->start || fp->pos >= fp->end) return -1;
  return 0;
}
