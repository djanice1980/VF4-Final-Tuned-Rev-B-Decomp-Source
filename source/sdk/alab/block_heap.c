/*
 * sdk/alab/block_heap.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int block_heap_irq_mask_all(void)
{
  unsigned int sr;
  int lvl;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  return lvl;
}

void block_heap_irq_level_restore(int level)
{
  unsigned int sr;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | ((level & 15) << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}

block_heap_rec * block_heap_init(block_heap_rec *heap, uint size)
{
  block_heap_unit *first;
  heap->first_block = first = (block_heap_unit *)((uint)heap + 0x8b & 0xffffffc0);
  heap->usable_bytes = size - 0x4c & 0xffffffc0;
  heap->free_list = &heap->anchor;
  heap->anchor.next = &heap->anchor;
  heap->anchor.size = 0;
  first->size = heap->usable_bytes >> 6;
  block_heap_free(heap,first + 1);
  return heap;
}

void empty_fn_0c0fbc00(void)
{
  return;
}

void block_heap_free_stats(block_heap_rec *heap, int *free_bytes, int *largest_bytes)
{
  block_heap_unit *p;
  block_heap_unit *h;
  int level;
  int total;
  uint big;
  int n;
  level = block_heap_irq_mask_all();
  h = heap->free_list;
  total = 0;
  big = 0;
  p = h->next;
  while (1) {
    n = p->size;
    if (n != 0) {
      total += n - 1;
      if (big < n - 1U) big = n - 1U;
    }
    if (p == heap->free_list) break;
    p = p->next;
  }
  *free_bytes = total << 6;
  *largest_bytes = big << 6;
  block_heap_irq_level_restore(level);
}

void * block_heap_alloc(block_heap_rec *heap, int size)
{
  block_heap_unit *p;
  block_heap_unit *prevp;
  block_heap_unit *bestp;
  uint nunits;
  int level;

  level = block_heap_irq_mask_all();
  bestp = 0;
  nunits = (size + sizeof(block_heap_unit) - 1) / sizeof(block_heap_unit) + 1;
  prevp = heap->free_list;
  for (p = prevp->next; ; prevp = p, p = p->next) {
    if (p->size >= nunits && (bestp == 0 || p->size < bestp->next->size))
      bestp = prevp;
    if (p == heap->free_list)
      break;
  }
  if (bestp == 0) {
    block_heap_irq_level_restore(level);
    return 0;
  }
  p = bestp->next;
  if (p->size == nunits)
    bestp->next = p->next;
  else {
    p->size -= nunits;
    p += p->size;
    p->size = nunits;
  }
  block_heap_irq_level_restore(level);
  return (void *)(p + 1);
}

void block_heap_free(block_heap_rec *heap, void *block)
{
  block_heap_unit *bp;
  block_heap_unit *p;
  int level;
  level = block_heap_irq_mask_all();
  if (block != 0) {
    bp = (block_heap_unit *)block - 1;
    for (p = heap->free_list; !(bp > p && bp < p->next); p = p->next)
      if (p >= p->next && (bp > p || bp < p->next))
        break;
    if (bp + bp->size == p->next) {
      bp->size += p->next->size;
      bp->next = p->next->next;
    }
    else
      bp->next = p->next;
    if (p + p->size == bp) {
      p->size += bp->size;
      p->next = bp->next;
    }
    else
      p->next = bp;
  }
  block_heap_irq_level_restore(level);
}
