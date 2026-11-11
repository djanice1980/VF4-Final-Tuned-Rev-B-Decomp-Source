/*
 * vf4lib/libcore/src/malloc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern heap_block heap_snapshot_blocks[32]; /* 0x0c3d88e4 */

static inline int disable_interrupts(void)
{
  unsigned int sr;
  int lvl;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  return lvl;
}

static inline void set_imask_level(int lvl)
{
  unsigned int sr, t;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  t = sr & 0xffffff0f;
  sr = t | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

void block_init(heap_block *b, heap *h, uint units)
{
  b->prev = (heap_block *)0x0;
  b->next = (heap_block *)0x0;
  b->user_request_size = 0;
  b->tag = (void *)0x0;
  b->serial = 0;
  b->bsize = units;
  b->state = 0x20202020;
  b->heap = h;
  return;
}

uint align_up(uint x, uint a)
{
  return (x + a) - 1 & -a;
}

uint align32(uint x)
{
  uint r;
  
  r = align_up(x,0x20);
  return r;
}

uint size_to_units(int size)
{
  uint bytes;
  
  bytes = align32(size + 4);
  return (bytes >> 5) + 1;
}

void * block_to_user(heap_block *b)
{
  return b + 1;
}

heap_block * user_to_block(void *p)
{
  return (heap_block *)((int)p + -0x20);
}

void fill_new_user_area(heap_block *b)
{
  void *s;
  
  s = block_to_user(b);
  memset(s,0xcd,b->user_request_size);
  return;
}

void poison_freed_user_area(heap_block *b)
{
  uint u;
  void *s;
  if (b->user_request_size == -1) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0x84,"never used user_area");
  }
  else {
    u = size_to_units(b->user_request_size);
    if (b->bsize == u) {
      s = block_to_user(b);
      memset(s,0xdd,b->user_request_size);
      return;
    }
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0x8e,"probably user_request_size is broken");
  }
}

char * guard_addr(heap_block *b)
{
  return (char *)((int)&b[1].prev + b->user_request_size);
}

void set_guard(heap_block *b)
{
  char *s;
  
  s = guard_addr(b);
  memset(s,0x2b,4);
  return;
}

int guard_intact(heap_block *b)
{
  char *g;
  
  g = guard_addr(b);
  return (uint)(*g == '+');
}

uint check_block(heap *h, heap_block *b)
{
  uint r;
  
  r = 0;
  if (b->heap != h) {
    r = 1;
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xc0,"mixed heap or broken %p",b);
  }
  if (b->state == BLOCK_USED) {
    if (guard_intact(b) == 0) {
      r |= 2;
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xcc,"over write %p",b);
    }
  }
  else if (b->state == BLOCK_FREE) {
    r |= 4;
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xd4,"double free %p",b);
  }
  else {
    r |= 8;
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xdd,"not heap or under write %p",b);
  }
  return r;
}

void check_heap_internal(heap *h)
{
  heap_block *x;
  heap_block *prev;
  
  if (h->check != '\0') {
    if ((h->head).state != HEAP_READY) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xee,"probably uninitilized heap %p",
               h);
    }
    prev = h->rover;
    for (x = prev->next; x != h->rover; x = x->next) {
      if (x != (heap_block *)align32((uint)x)) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xf4,"bad align");
      }
      if ((x->prev != prev) || ((x->state == BLOCK_USED) | (x->state == BLOCK_FREE)) == 0) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0xfd,"check_heap_internal()");
      }
      prev = x;
    }
  }
  return;
}

void list_remove(heap_block *b)
{
  heap_block *next;
  heap_block *prev;
  
  prev = b->prev;
  next = b->next;
  prev->next = next;
  next->prev = prev;
  b->next = (heap_block *)0x0;
  b->prev = (heap_block *)0x0;
  return;
}

void list_insert_after(heap_block *b, heap_block *after)
{
  heap_block *next;
  
  next = after->next;
  b->next = next;
  after->next = b;
  b->prev = after;
  next->prev = b;
  return;
}

int heap_init(heap *h, void *base, uint size)
{
  heap_block *f;
  heap_block *b;
  uint a;
  h->base = base;
  h->size = size;
  a = align_up((uint)base,0x20);
  h->first = (heap_block *)a;
  h->usable = size - (a - (uint)base) & 0xffffffdf;
  h->alloc_count = 0;
  h->break_count = 0;
  b = &h->head;
  block_init(b,h,0x20);
  (h->head).next = b;
  (h->head).prev = b;
  h->rover = b;
  (h->head).state = HEAP_READY;
  f = h->first;
  block_init(f,h,h->usable >> 5);
  f->state = BLOCK_FREE;
  f->user_request_size = -1;
  list_insert_after(f,b);
  return 0;
}

int return_zero_0c0f3e80(void)
{
  return 0;
}

void free_internal(heap *h, void *tag, void *p)
{
  heap_block *b;
  uint err;
  heap_block *other;
  
  if (p != (void *)0x0) {
    b = user_to_block(p);
    err = check_block(h,b);
    if (err == 0) {
      poison_freed_user_area(b);
      if ((b->next->state == BLOCK_FREE) && (other = b + b->bsize, other == b->next)) {
        list_remove(other);
        b->bsize = b->bsize + other->bsize;
      }
      other = b->prev;
      if ((other->state == BLOCK_FREE) && (other + other->bsize == b)) {
        list_remove(b);
        other->bsize = other->bsize + b->bsize;
      }
      else {
        b->state = BLOCK_FREE;
      }
    }
  }
  return;
}

void * malloc_internal(heap *h, void *tag, int size)
{
  uint nunits;
  heap_block *b;
  heap_block *p;

  if (++h->alloc_count == h->break_count) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0x1bb,
             "break point %d by malloc_internal", h->alloc_count);
  }
  nunits = size_to_units(size);
  p = h->rover->next;
loop:
  if (p->state != BLOCK_FREE || p->bsize < nunits) goto next;
  if ((p->bsize != nunits) && (p->bsize != nunits + 1)) {
    if (!(p->bsize > nunits + 1)) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcore/src/malloc.c",0x1dc,"assertion `%s\' failed.\n",
               "p->bsize > nunits + 1");
    }
    b = p + nunits;
    block_init(b,h,p->bsize - nunits);
    list_insert_after(b,p);
    b->state = BLOCK_FREE;
    b->user_request_size = -1;
    p->bsize = nunits;
  }
  p->state = BLOCK_USED;
  p->user_request_size = size;
  p->tag = tag;
  p->serial = h->alloc_count;
  fill_new_user_area(p);
  set_guard(p);
  return block_to_user(p);
next:
  if (p == h->rover) {
    return (void *)0x0;
  }
  p = p->next;
  goto loop;
}

void * realloc_internal(heap *h, void *tag, void *p, int size)
{
  void *dst;
  uint nunits;
  heap_block *b;

  if (p == (void *)0x0) {
    return malloc_internal(h,tag,size);
  }
  nunits = size_to_units(size);
  b = user_to_block(p);
  if (nunits <= b->bsize) {
    b->user_request_size = size;
    set_guard(b);
    dst = p;
  }
  else {
    dst = malloc_internal(h,tag,size);
    memcpy(dst,p,b->bsize << 5);
    free_internal(h,tag,p);
  }
  return dst;
}

void * heap_malloc(heap *h, int size)
{
  void *ra;
  void *p;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t, u;
  ra = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  check_heap_internal(h);
  p = malloc_internal(h,ra,size);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  u = t & 0xffffff0f; t = u | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return p;
}

void heap_free(heap *h, void *p)
{
  void *tag;
  int lvl;

  tag = __builtin_return_address(0);
  lvl = disable_interrupts();
  check_heap_internal(h);
  free_internal(h,tag,p);
  set_imask_level(lvl);
}

void heap_realloc(heap *h, void *p, int size)
{
  void *ra;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t, u;
  ra = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  check_heap_internal(h);
  realloc_internal(h,ra,p,size);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  u = t & 0xffffff0f; t = u | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void * heap_calloc(heap *h, int n, int size)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  void *tag;
  int total;
  void *s;

  tag = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  total = size * n;
  check_heap_internal(h);
  s = malloc_internal(h,tag,total);
  if (s != 0) {
    memset(s,0,total);
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  { unsigned int u = t & 0xffffff0f; t = u | (lvl << 4); }
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return s;
}

void heap_stats_get(heap *h, struct mallinfo *info)
{
  unsigned int sr;
  unsigned int lvl[1];
  uint n;
  uint sum;
  uint mx;
  heap_block *b;
  uint s;
  uint fs;
  uint ms;
  uint u;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl[0] = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  n = 0;
  sum = 0;
  mx = 0;
  check_heap_internal(h);
  for (b = h->rover->next; b != h->rover; b = b->next) {
    if (b->state == 0x20202020) {
      n++;
      s = b->bsize;
      sum += s;
      if (mx < s) mx = s;
    }
  }
  fs = sum << 5;
  ms = mx << 5;
  u = h->usable;
  info->arena = u;
  info->ordblks = n;
  info->smblks = 0;
  info->hblks = 0;
  info->hblkhd = 0;
  info->usmblks = 0;
  info->fsmblks = ms;
  info->uordblks = u - fs;
  info->fordblks = fs;
  info->keepcost = 0;
  __asm__ volatile ("stc sr,%0" : "=r" (b));
  b = (heap_block *)(((uint)b & 0xffffff0f) | (lvl[0] << 4));
  __asm__ volatile ("ldc %0,sr" : : "r" (b));
}

void heap_blocks_snapshot(heap *h, heap_block **out, int count)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  heap_block *p;
  heap_block *n;
  heap_block *e;
  uint i;
  int j;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  i = 0;
  p = h->rover;
  for (n = p->next; n != h->rover; p = n, n = n->next, i++) {
    if (i > 31) break;
    out[i] = p;
    e = &heap_snapshot_blocks[i];
    e->prev = p->prev;
    e->next = p->next;
    e->bsize = p->bsize;
    e->user_request_size = p->user_request_size;
    e->tag = p->tag;
    e->heap = p->heap;
    e->serial = p->serial;
    e->state = p->state;
  }
  for (j = i; j < count; j++) {
    out[j] = 0;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  { unsigned int u = t & 0xffffff0f; t = u | (lvl << 4); }
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

heap * main_heap_get(void)
{
  return &main_heap;
}

void main_heap_init(void)
{
#define MAIN_HEAP_END ((char *)0x0dff0000) /* top of main RAM, less 64 KB */
  if (main_heap_ready == 0) {
    heap_init(&main_heap, main_heap_area, MAIN_HEAP_END - main_heap_area);
    main_heap.check = '\x01';
    main_heap_ready = 1;
  }
  return;
}

void * malloc(size_t size)
{
  void *ra;
  unsigned int sr;
  int lvl;
  heap *h;
  void *p;
  unsigned int t;

  ra = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr | 0xf0));
  h = &main_heap;
  main_heap_init();
  check_heap_internal(h);
  p = malloc_internal(h,ra,size);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  { unsigned int u = t & 0xffffff0f; t = u | (lvl << 4); }
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return p;
}

void free(void *p)
{
  heap *h;
  void *tag;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t, u;
  tag = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  h = &main_heap;
  main_heap_init();
  check_heap_internal(h);
  free_internal(h,tag,p);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  u = t & 0xffffff0f; t = u | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void realloc(void *p, int size)
{
  void *ra;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t, u;
  heap *h;
  ra = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  h = &main_heap;
  main_heap_init();
  check_heap_internal(h);
  realloc_internal(h,ra,p,size);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  u = t & 0xffffff0f; t = u | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));

}

void * calloc(size_t n, size_t size)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int m;
  void *tag;
  heap *h;
  int total;
  void *s;

  tag = __builtin_return_address(0);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  h = &main_heap;
  total = size * n;
  main_heap_init();
  check_heap_internal(h);
  s = malloc_internal(h,tag,total);
  if (s != 0) {
    memset(s,0,total);
  }
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  { unsigned int u = t & 0xffffff0f; t = u | (lvl << 4); }
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return s;
}

void main_heap_stats_get(struct mallinfo *info)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  unsigned int u;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  main_heap_init();
  heap_stats_get(&main_heap,info);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  u = t & 0xffffff0f;
  t = u | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return;
}

struct mallinfo main_heap_mallinfo(void)
{
  struct mallinfo mi;

  main_heap_stats_get(&mi);
  return mi;
}
