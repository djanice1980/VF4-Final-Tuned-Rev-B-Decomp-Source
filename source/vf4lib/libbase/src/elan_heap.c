/*
 * vf4lib/libbase/src/elan_heap.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int elan_heap_log; /* 0x0c299afc */
extern int elan_gap_size; /* 0x0c3778e4 */
extern uint elan_gap_base; /* 0x0c3778e8 */
extern heap_block *elan_heap_blocks[4096]; /* 0x0c3778ec */
extern uint elan_heap_base; /* 0x0c41fdac */
extern uint elan_heap_end; /* 0x0c41fdb0 */
extern uint elan_heap_size; /* 0x0c41fdb4 */
extern heap elan_heap; /* 0x0c41fdb8 */
extern heap elan_gap_heap; /* 0x0c41fe24 */

void elan_heaps_init(uint size)
{
  elan_heap_size = size;
  elan_heap_end = model_heap_top();
  elan_heap_base = (elan_heap_end - (size + 0x1f & 0xffffffe0));
  elan_gap_base = (frame_list_write_end() + 0x2001fU & 0xffffffe0);
  elan_gap_size = elan_heap_base - elan_gap_base;
  heap_init(&elan_heap,(void *)elan_heap_base,size);
  heap_stats_get(&elan_heap,&elan_heap.stats);
  heap_init(&elan_gap_heap,(void *)elan_gap_base,elan_gap_size);
  heap_stats_get(&elan_gap_heap,&elan_gap_heap.stats);
}

void * elan_heap_alloc(int size)
{
  void *p;
  heap *h = &elan_heap;

  p = heap_malloc(h,size);
  heap_blocks_snapshot(h,elan_heap_blocks,0x1000);
  heap_stats_get(h,&h->stats);
  if (elan_heap_log != 0) {
    ((int (*)())console_printf)(debug_out,"EAlloc: %p[% 6x]\n",p,size);
  }
  return p;
}

void elan_heap_free(void *p)
{
  heap *h = &elan_heap;

  heap_blocks_snapshot(h,elan_heap_blocks,0x1000);
  heap_free(h,p);
  heap_stats_get(h,&h->stats);
  if (elan_heap_log != 0) {
    ((int (*)())console_printf)(debug_out,"EFree : %p\n",p);
  }
}

heap * elan_heap_get(void)
{
  return &elan_heap;
}

void elan_heap_blocks_snapshot(void)
{
  heap_blocks_snapshot(&elan_heap,elan_heap_blocks,0x1000);
  return;
}

void * elan_gap_alloc(int size)
{
  void *p;
  heap *h = &elan_gap_heap;
  p = heap_malloc(h,size);
  heap_stats_get(h,&h->stats);
  return p;
}

void elan_gap_free(void *p)
{
  heap *h = &elan_gap_heap;
  heap_free(h,p);
  heap_stats_get(h,&h->stats);
  return;
}
