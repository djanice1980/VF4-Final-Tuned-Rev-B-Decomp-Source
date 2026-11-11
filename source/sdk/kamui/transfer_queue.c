/*
 * sdk/kamui/transfer_queue.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint bank_build_index; /* 0x0c41e58c */
extern uint direct_send_held; /* 0x0c41e590 */
extern uint xfer_pending[3]; /* 0x0c41e9d4 */
extern uint direct_send_hold_depth; /* 0x0c41e9e0 */
extern xfer_job_rec *xfer_current; /* 0x0c41e9e4 */
extern uint list_kind_sent; /* 0x0c41e9e8 */
extern uint bank_render_index; /* 0x0c41e9ec */
extern uint sq_save_top; /* 0x0c41eb28 */
extern xfer_job_rec xfer_node_pool[512]; /* 0x0c601ffc */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern list_block_ref blank_end_block; /* 0x0c41e9f0 */
extern elan_rev_rec elan_rev_config; /* 0x0c41eb20 */
extern xfer_node_queue xfer_free_nodes; /* 0x0c606ffc */

static inline void sq_pref(void *a)
{
  __asm__ __volatile__ ("pref @%0" : : "r" (a));
}

static inline void dl_open(int i, uint *p)
{
  typedef struct { uint w[2]; } dma_pair;
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[0] = (uint)p;
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[1] = 0;
}

static inline void dl_close(int i, uint *p, uint hi)
{
  typedef struct { uint w[2]; } dma_pair;
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[1] = (uint)p - ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[0];
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[0] = ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c))->w[0] & 0x3ffffff | hi;
}

static inline void dl_close_b(int i, uint *p, uint hi)
{
  typedef struct { uint w[2]; } dma_pair;
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x24))->w[1] = (uint)p - ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x24))->w[0];
  ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x24))->w[0] = ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x24))->w[0] & 0x3ffffff | hi;
}

static inline uint *dl_blank(uint *p)
{
  p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0; p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
  sq_pref(p);
  p += 8;
  return p;
}

static inline uint *dl_end(uint *p)
{
  p[0] = 0x08000000; p[2] = 0x08000000; p[4] = 0x08000000; p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  sq_pref(p);
  p += 8;
  return p;
}

void xferq_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void xferq_sq_save(uint *saved)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint slot;
  uint area;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  { unsigned int o = (sr & 0xf0) >> 4; lvl = o; }
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  slot = sq_save_top & 0x3ffffff | 0xe0000000;
  xferq_check_or_halt((slot & 0x3f) == 0);
  *saved = *(volatile uint *)0xff000038;
  saved[1] = *(volatile uint *)0xff00003c;
  saved[2] = sq_save_top;
  sq_save_top = sq_save_top + 0x40;
  area = sq_save_top >> 0x18 & 0x1c;
  *(volatile uint *)0xff000038 = area;
  *(volatile uint *)0xff00003c = area;
  __asm__ volatile ("pref @%0" : : "r" (slot));
  __asm__ volatile ("pref @%0" : : "r" (slot + 0x20));
  *(volatile uint *)0xff000038 = *saved;
  *(volatile uint *)0xff00003c = saved[1];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void xferq_sq_restore(uint *saved)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  uint *dst;
  uint *src;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dst = (uint *)0xe0000000;
  src = (uint *)(saved[2] & 0x1fffffff | 0xa0000000);
  sq_save_top = sq_save_top + -0x40;
  xferq_check_or_halt(saved[2] == sq_save_top);
  *(volatile uint *)0xff000038 = saved[0];
  *(volatile uint *)0xff00003c = saved[1];
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
  dst[5] = src[5];
  dst[6] = src[6];
  dst[7] = src[7];
  dst[8] = src[8];
  dst[9] = src[9];
  dst[10] = src[10];
  dst[11] = src[11];
  dst[12] = src[12];
  dst[13] = src[13];
  dst[14] = src[14];
  dst[15] = src[15];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void jobq_append(uint *queue, uint *node)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if ((uint *)queue[1] != (uint *)0x0) {
    *(uint **)queue[1] = node;
    queue[1] = (uint)node;
  }
  else {
    queue[1] = (uint)node;
    *queue = (uint)node;
  }
  *node = 0;
  queue[2] = queue[2] + 1;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void jobq_concat(uint *dst, uint *src)
{
  unsigned int sr;
  unsigned int m;
  unsigned int old;
  unsigned int sr2;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  old = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if ((uint *)dst[1] != (uint *)0x0) {
    *(uint *)dst[1] = *src;
    dst[1] = src[1];
  }
  else {
    *dst = *src;
    dst[1] = src[1];
    dst[2] = src[2];
  }
  dst[2] = dst[2] + src[2];
  *src = 0;
  src[1] = 0;
  src[2] = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr2));
  sr2 = (sr2 & 0xffffff0f) | (old << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (sr2));
  return;
}

int jobq_pop(int *queue)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  int next;
  int *node;
  
  if (*queue != 0) {
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  node = (int *)*queue;
  next = *node;
  *queue = next;
  if (next == 0) {
    xferq_check_or_halt((int *)queue[1] == node);
    queue[1] = next;
  }
  else if (*(int *)*queue == 0) {
    queue[1] = *node;
  }
  queue[2] = queue[2] + -1;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return (int)node;
  }
  return 0;
}

void jobq_reset(int *queue, int release)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  int node;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (release != 0) {
    while (*queue != 0) {
      node = jobq_pop(queue);
      xferq_check_or_halt(node != 0);
      jobq_append((uint *)&xfer_free_nodes,(uint *)node);
    }
  }
  *queue = 0;
  queue[1] = 0;
  queue[2] = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void xfer_job_pool_init(void)
{
  int i;
  int off;
  
  jobq_reset((int *)xfer_pending,0);
  jobq_reset((int *)&xfer_free_nodes,0);
  i = 0;
  off = 0;
  do {
    off = i * 0x28;
    jobq_append((uint *)&xfer_free_nodes,(uint *)((char *)xfer_node_pool + off));
    i = i + 1;
  } while (i < 0x200);
  return;
}

void xfer_job_start(void)
{
  unsigned int sr;
  unsigned int lvl[1];
  unsigned int t;
  uint a;
  uint q;
  uint *p;
  int r;
  volatile uint *hw;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
  if ((*(xfer_job_rec * volatile *)&xfer_current)->kind != -1) {
    elan_d0_set_for_list((*(xfer_job_rec * volatile *)&xfer_current)->kind);
  }
  if ((*(xfer_job_rec * volatile *)&xfer_current)->dma_ctrl != 0) {
    dma2_source = (*(xfer_job_rec * volatile *)&xfer_current)->dma_src;
    dma2_dest = (*(xfer_job_rec * volatile *)&xfer_current)->dest + 0x57000000;
    dma2_count = (*(xfer_job_rec * volatile *)&xfer_current)->dma_len >> 5;
    dma2_control = (*(xfer_job_rec * volatile *)&xfer_current)->dma_ctrl;
  }
  a = 0xa9000000;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  q = a >> 24;
  ((volatile uint *)0xff000038)[0] = q;
  ((volatile uint *)0xff000038)[1] = q;
  p = (uint *)(a & 0x3ffffff | 0xe0000000);
  if ((*(xfer_job_rec * volatile *)&xfer_current)->from_dest != 0) {
    p[0] = 0x08000f00;
    p[1] = ((*(xfer_job_rec * volatile *)&xfer_current)->mode & 1) << 0x1d |
         ((*(xfer_job_rec * volatile *)&xfer_current)->mode >> 1 & 1) << 0x1e | 0x80000000;
    p[2] = (*(xfer_job_rec * volatile *)&xfer_current)->dest & 0x1ffffff8;
    p[3] = (*(xfer_job_rec * volatile *)&xfer_current)->dma_len;
  }
  else {
    if ((*(xfer_job_rec * volatile *)&xfer_current)->kind == -1) {
      p[0] = 0x08000f00;
      p[1] = (*(xfer_job_rec * volatile *)&xfer_current)->src_addr & 0x1fffff8 | ((*(xfer_job_rec * volatile *)&xfer_current)->mode & 1) << 0x1d |
           ((*(xfer_job_rec * volatile *)&xfer_current)->mode >> 1 & 1) << 0x1e;
      p[2] = (*(xfer_job_rec * volatile *)&xfer_current)->dest & 0x1ffffff8;
      p[3] = (*(xfer_job_rec * volatile *)&xfer_current)->src_len;
    }
    else {
      p[0] = 0x08000f00;
      p[1] = (*(xfer_job_rec * volatile *)&xfer_current)->src_addr & 0x1fffff8 | ((*(xfer_job_rec * volatile *)&xfer_current)->mode & 1) << 0x1d |
           ((*(xfer_job_rec * volatile *)&xfer_current)->mode >> 1 & 1) << 0x1e;
      p[2] = 0x09000000;
      p[3] = (*(xfer_job_rec * volatile *)&xfer_current)->src_len;
    }
  }
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  if ((*(xfer_job_rec * volatile *)&xfer_current)->kind != -1) {
    r = 0;
    if ((*(xfer_job_rec * volatile *)&xfer_current)->kind >= 0 && (*(xfer_job_rec * volatile *)&xfer_current)->kind < 5) r = 1;
    xferq_check_or_halt(r);
    *(volatile uint *)&list_kind_sent = (*(xfer_job_rec * volatile *)&xfer_current)->kind;
  }
  else {
    hw = (volatile uint *)0xa85f81a8;
    hw[12] = 0xf0f0f0f0;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl[0] << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int xfer_job_restart(void)
{
  if (*(xfer_job_rec * volatile *)&xfer_current != 0) {
    xfer_job_start();
  }
}

void xfer_job_next(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;

  if (*(xfer_job_rec * volatile *)&xfer_current == (xfer_job_rec *)0x0) {
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    lvl = (sr & 0xf0) >> 4;
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
    xfer_current = (xfer_job_rec *)jobq_pop((int *)xfer_pending);
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
    if (xfer_current != (xfer_job_rec *)0x0) {
      xfer_trace_push(0);
      xfer_job_start();
    }
  }
  return;
}

int xfer_job_retire(void)
{
  xferq_check_or_halt(xfer_current != (xfer_job_rec *)0x0);
  if (xfer_current->dma_ctrl != 0) {
    *(volatile uint *)0xffa0002c &= 0xfffffffc;
  }
  jobq_append((uint *)&xfer_free_nodes,(uint *)*(xfer_job_rec * volatile *)&xfer_current);
  xfer_current = (xfer_job_rec *)0x0;
}

void xfer_job_enqueue(int *queue, uint kind, uint *src, uint dest, uint dma_ctrl, uint mode, uint from_dest)
{
  xfer_job_rec *job;
  uint addr;
  uint len;
  job = (xfer_job_rec *)jobq_pop((int *) (&xfer_free_nodes));
  if (!(job == 0))
  {
    addr = *src;
    len = src[1];
    job->src_addr = addr;
    job->src_len = len;
    job->dest = dest;
    job->dma_ctrl = dma_ctrl;
    job->dma_src = *src;
    job->dma_len = src[1];
    job->mode = mode;
    job->kind = kind;
    job->from_dest = from_dest;
    xferq_check_or_halt((uint) ((kind == 0xffffffff) || (kind < 5)));
    jobq_append((uint *)queue, (uint *)job);
  }
  else
  {
    xferq_check_or_halt(0);
  }
  return;
}

void bank_lists_seed(void)
{
  typedef struct { uint w[2]; } dma_pair;
  typedef struct { int pad[3]; int f0; int used; int busy; int count; dma_pair a; dma_pair b; dma_pair c; } slot;
  uint h;
  uint *p;
  uint hi;
  int i;
  int o;

  h = *(volatile int *)&bank_build_index * 0x1000 + 0xabffc000;
  *(volatile uint *)0xff000038 = h >> 24;
  *(volatile uint *)0xff00003c = h >> 24;
  p = (uint *)(h & 0x3ffffff | 0xe0000000);
  hi = h & 0xfc000000;
  {
    list_bank_rec *t = list_banks;
  ((volatile list_bank_rec *)&t[*(volatile int *)&bank_build_index])->state = 0;
  ((volatile list_bank_rec *)&t[*(volatile int *)&bank_build_index])->job_count = 0;
  }
  for (i = 0; i < 5; i++) {
    {
      char *t = (char *)list_banks;
    ((volatile slot *)(t + i * 0x34 + *(volatile int *)&bank_build_index * 0x110))->f0 = 0;
    ((volatile slot *)(t + i * 0x34 + *(volatile int *)&bank_build_index * 0x110))->count = 0;
    }
    switch (i) {
    case 0:
      dl_open(i, p);
      if ((gfx_state.kept_list & 0xf) != i) {
        p = dl_blank(p);
      }
      p[0] = (i & 7) << 24 | 0xa08c0000;
      p[1] = 0; p[2] = 0; p[3] = 0;
      sq_pref(p);
      p += 8;
      p[0] = 0xf0000000;
      ((float *)p)[1] = 0.0; ((float *)p)[2] = 0.0; ((float *)p)[3] = 0.0; ((float *)p)[4] = 0.0;
      ((float *)p)[5] = 0.0; ((float *)p)[6] = 0.0; ((float *)p)[7] = 0.0;
      sq_pref(p);
      ((float *)p)[8] = 0.0; ((float *)p)[9] = 0.0; ((float *)p)[10] = 0.0; ((float *)p)[11] = 0.0;
      p[13] = 0; p[14] = 0; p[15] = 0;
      sq_pref(p + 8);
      p += 16;
      p = dl_end(p);
      dl_close(i, p, hi);
      break;
    case 2:
    case 4:
      dl_open(i, p);
      if ((gfx_state.kept_list & 0xf) != i) {
        p = dl_blank(p);
      }
      p[0] = (i & 7) << 24 | 0xa08c0000;
      p[1] = 0; p[2] = 0; p[3] = 0;
      sq_pref(p);
      p += 8;
      p[0] = 0xf0000000;
      ((float *)p)[1] = 0.0; ((float *)p)[2] = 0.0; ((float *)p)[3] = 0.0; ((float *)p)[4] = 0.0;
      ((float *)p)[5] = 0.0; ((float *)p)[6] = 0.0; ((float *)p)[7] = 0.0;
      sq_pref(p);
      ((float *)p)[8] = 0.0; ((float *)p)[9] = 0.0; ((float *)p)[10] = 0.0; ((float *)p)[11] = 0.0;
      p[13] = 0; p[14] = 0; p[15] = 0;
      sq_pref(p + 8);
      p += 16;
      p = dl_end(p);
      dl_close(i, p, hi);
      break;
    default:
      dl_open(i, p);
      if ((gfx_state.kept_list & 0xf) != i) {
        p = dl_blank(p);
      }
      p[0] = (i & 7) << 24 | 0x808c0040;
      p[1] = 0x28000000;
      sq_pref(p);
      p += 8;
      p[0] = 0xf0000000;
      ((float *)p)[1] = 0.0; ((float *)p)[2] = 0.0; ((float *)p)[3] = 0.0; ((float *)p)[4] = 0.0;
      ((float *)p)[5] = 0.0; ((float *)p)[6] = 0.0; ((float *)p)[7] = 0.0;
      sq_pref(p);
      ((float *)p)[8] = 0.0; ((float *)p)[9] = 0.0;
      sq_pref(p + 8);
      p += 16;
      p = dl_end(p);
      dl_close(i, p, hi);
      break;
    }
    ((volatile slot *)(i * 0x34 + *(volatile int *)&bank_build_index * 0x110 + (int)list_banks))->used = 1; o = i * 0x34;
    ((volatile slot *)(o + *(volatile int *)&bank_build_index * 0x110 + (int)list_banks))->busy = 0;
    ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + o + (int)list_banks + 0x24))->w[0] = (uint)p;
    ((volatile dma_pair *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x24))->w[1] = 0;
    p += 8;
    {
      uint *q = p - 1;
      *--q = 13; *--q = 0xffffffff; *--q = 0x08000e00;
      q--;
      *--q = 13; *--q = 0xffffffff; *--q = 0x08000e00;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(q);
      q--;
      *--q = 13; *--q = 0xffffffff; *--q = 0x08000e00;
      q--;
      *--q = 13; *--q = 0xffffffff; *--q = 0x08000e00;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(q);
    }
    p = dl_end(p);
    dl_close_b(i, p, hi);
    jobq_reset((int *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x34), 0);
    xfer_job_enqueue((int *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x34), i, (uint *)(*(volatile int *)&bank_build_index * 0x110 + i * 0x34 + (int)list_banks + 0x1c), 0xa9000000, 0, 0, 0);
  }
  blank_end_block.addr = (uint)p;
  blank_end_block.length = 0;
  p = dl_blank(p);
  p = dl_end(p);
  blank_end_block.length = (uint)p - blank_end_block.addr;
  blank_end_block.addr = blank_end_block.addr & 0x3ffffff | hi;
}

void list_seg_link_to(int *seg, uint *next)
{
  uint a;
  uint area;
  uint *p;
  a = (*seg + seg[1]) - 0x20;
  area = a >> 0x18;
  *(uint *)0xff000038 = area;
  *(uint *)0xff00003c = area;
  p = (uint *)(a & 0x3ffffff | 0xe0000000);
  if ((*next & 0x1e000000) == 0x0c000000) {
    *p = 0x08000f00;
    p[1] = 0x80000000;
  }
  else {
    *p = 0x08000f00;
    p[1] = *next & 0x1fffff8;
  }
  p[2] = 0x09000000;
  p[3] = next[1];
  __asm__ volatile ("pref @%0" : : "r"(p));
}

void list_seg_end_mark(int *seg, int bit29, int bit30)
{
  uint a;
  uint *p;
  a = *seg + seg[1] - 0x20;
  ((volatile uint *)0xff000038)[0] = a >> 24;
  ((volatile uint *)0xff000038)[1] = a >> 24;
  p = (uint *)(a & 0x3ffffff | 0xe0000000);
  p[0] = 0x08000f00;
  p[1] = bit29 << 0x1d | bit30 << 0x1e;
  p[2] = 0x09000000;
  p[3] = 0;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
}

void direct_send_hold(int on)
{
  if (on != 0) {
    if (*(volatile int *)&direct_send_hold_depth == 0) {
      xferq_check_or_halt(direct_send_held == 0);
      direct_send_held = 1;
    }
    *(volatile int *)&direct_send_hold_depth = *(volatile int *)&direct_send_hold_depth + 1;
  }
  else {
    *(volatile int *)&direct_send_hold_depth = *(volatile int *)&direct_send_hold_depth - 1;
    if (*(volatile int *)&direct_send_hold_depth == 0) {
      xferq_check_or_halt(direct_send_held == 1);
      direct_send_held = 0;
    }
  }
  xferq_check_or_halt(*(volatile int *)&direct_send_hold_depth >= 0);
}

void list_seg_append(int kind, uint *seg, int no_send)
{
  list_bank_list_rec *e;
  xfer_job_rec *tail;
  int k;
  volatile int dummy;
  volatile uint *sq;
  e = &list_banks[*(volatile int *)&bank_build_index].lists[kind];
  if ((*seg & 0x1e000000) == 0x0c000000) k = 0x12c1;
  else k = 0;
  tail = e->job_tail;
  if (k != 0) {
    if (tail == 0 || tail->dma_ctrl == 0) {
      if (e->seg_count != 0) {
        list_seg_link_to((int *)&e->last_seg, seg);
      }
      else if (e->opened != 0) {
        list_seg_link_to((int *)&e->first_seg, seg);
      }
      else {
        e->first_seg = *(list_block_ref *)seg;
        e->opened = 1;
        e->last_seg = *(list_block_ref *)seg;
        jobq_reset((int *)&e->job_head, 1);
        xfer_job_enqueue((int *)&e->job_head, kind, (uint *)&e->first_seg, 0xa9000000, k, 0, 1);
      }
      if (tail == 0) tail = e->job_tail;
      xferq_check_or_halt(tail != 0);
      tail->dma_ctrl = k;
      tail->dma_src = seg[0];
      tail->dma_len = seg[1];
    }
    else {
      list_seg_end_mark((int *)&e->last_seg, 0, 1);
      e->last_seg = *(list_block_ref *)seg;
      xfer_job_enqueue((int *)&e->job_head, kind, seg, 0xa9000000, k, 0, 0);
    }
    e->dma_busy = 1;
    e->seg_count = e->seg_count + 1;
  }
  else {
    if (e->seg_count != 0) {
      list_seg_link_to((int *)&e->last_seg, seg);
      e->seg_count = e->seg_count + 1;
    }
    else if (e->opened != 0) {
      list_seg_link_to((int *)&e->first_seg, seg);
      e->seg_count = e->seg_count + 1;
    }
    else {
      e->first_seg = *(list_block_ref *)seg;
      e->opened = 1;
      e->last_seg = *(list_block_ref *)seg;
      jobq_reset((int *)&e->job_head, 1);
      xfer_job_enqueue((int *)&e->job_head, kind, (uint *)&e->first_seg, 0xa9000000, 0, 0, 0);
      xferq_check_or_halt(e->seg_count == 0);
    }
  }
  e->last_seg = *(list_block_ref *)seg;
  e = &list_banks[bank_build_index].lists[0];
  if (direct_send_held == 0) if (no_send == 0 && e->job_count != 0 && e->opened != 0 && e->dma_busy == 0 &&
      gfx_state.kept_list == 0 && gfx_state.single_bank == 1 &&
      *(volatile uint *)&elan_cmd_queue_level < 2) {
    dummy = *(volatile int *)0xaa000000;
    elan_d0_set_for_list(0);
    *(volatile uint *)0xff000038 = 0xa9;
    *(volatile uint *)0xff00003c = 0xa9;
    sq = (volatile uint *)0xe1000000;
    sq[0] = 0x08000f00;
    sq[1] = e->first_seg.addr & 0x1fffff8;
    sq[2] = 0x09000000;
    sq[3] = e->first_seg.length;
    __asm__ __volatile__ ("pref @%0" : : "r" (sq));
    e->opened = 0;
    e->seg_count = 0;
    jobq_reset((int *)&e->job_head, 1);
  }
}

void xfer_dma_enqueue(uint src, uint len, uint dest)
{
  int k;
  uint buf[2];
  unsigned int sr;
  unsigned int lvl;

  buf[0] = src;
  buf[1] = len;
  if ((src & 0x1e000000) == 0x0c000000) k = 0x12c1;
  else k = 0;
  while (xfer_free_nodes.count == 0) ;
  xfer_job_enqueue((int *)xfer_pending,0xffffffff,buf,dest,k,1,k != 0);
  ({
    unsigned int s, m;
    __asm__ volatile ("stc sr,%0" : "=r" (s));
    m = s & 0xf0;
    lvl = m >> 4;
    s |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (s));
  });
  if (((volatile list_bank_rec *)list_banks)[bank_render_index].state == 2) {
    {
      unsigned int t;
      __asm__ volatile ("stc sr,%0" : "=r" (t));
      t = (t & 0xffffff0f) | (lvl << 4);
      __asm__ volatile ("ldc %0,sr" : : "r" (t));
    }
    do {
      volatile list_bank_rec *tbl = (volatile list_bank_rec *)list_banks;
      if (tbl[*(volatile uint *)&bank_render_index].state != 2) break;
    } while (1);
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  }
  xfer_job_next();
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
}

void list_seg_append_padded(int kind, uint addr, int len)
{
  uint seg[2];
  
  seg[0] = addr;
  seg[1] = len + 0x20;
  list_seg_append(kind,seg,0);
  return;
}

bool bank_poll_done(int bank)
{
  pvr_irq_service();
  return *(volatile int *)&list_banks[bank].state == 0;
}

uint bank_close_and_submit(uint list_mask, uint first_list)
{
  int order[5];
  int found;
  unsigned int lvl;
  unsigned int sr;
  int v;
  int i;
  list_bank_list_rec *e;

  order[0] = 0;
  order[1] = 1;
  order[2] = 2;
  order[3] = 3;
  order[4] = 4;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  { unsigned int m = sr & 0xf0; lvl = m >> 4; }
  if (first_list != 0xff) {
    xferq_check_or_halt(first_list < 5);
    for (i = first_list; i > 0; i--) {
      order[i] = order[i - 1];
    }
    order[0] = first_list;
  }
  for (i = 4; i >= 0; i--) {
    v = order[i];
    if ((list_mask & (1 << v)) != 0) {
      found = v;
      break;
    }
  }
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  for (i = 0; i < 5; i++) {
    v = order[i];
    e = &list_banks[*(volatile int *)&bank_build_index].lists[v];
    if ((list_mask & (1 << v)) != 0) {
      list_seg_end_mark((int *)&e->end_seg,0,1);
      list_seg_append(v,(uint *)&e->end_seg,1);
      if (e->opened != 0) {
        { volatile list_bank_rec *tbl = (volatile list_bank_rec *)list_banks; tbl[*(volatile uint *)&bank_build_index].job_count += e->job_count; }
        jobq_concat(xfer_pending,(uint *)&e->job_head);
      }
    }
    else {
      jobq_reset((int *)&e->job_head,1);
    }
  }
  {
    volatile list_bank_rec *tbl = (volatile list_bank_rec *)list_banks;
    tbl[*(volatile uint *)&bank_build_index].last_list = found;
    tbl[*(volatile uint *)&bank_build_index].state = 1;
  }
  {
    volatile uint *hw = (volatile uint *)0xa85f8184;
    hw[12] = 0xf0f0f0f0;
  }
  if (*(volatile uint *)&bank_build_index > 3) {
    xferq_check_or_halt(0);
  }
  xfer_job_next();
  i = *(volatile uint *)&bank_build_index;
  *(volatile uint *)&bank_build_index = (*(volatile uint *)&bank_build_index + 1) & 3;
  bank_lists_seed();
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
  if ((elan_rev_config.flags & 4) != 0) {
    while (bank_poll_done(i) == 0) ;
  }
  return i;
}

int xfer_wait_idle(void)
{
  while (*(volatile int *)&xfer_current != 0 || *(volatile int *)xfer_pending != 0) {
  }
}

void frame_split_word_note(uint word)
{
  gfx_state.slot_split_words[gfx_state.frame_idx] = word;
}
