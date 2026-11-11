/*
 * vf4lib/libeffect/src/effect_malloc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined *effect_heap_mem; /* 0x0c454900 */
extern heap effect_heap; /* 0x0c454904 */
extern struct mallinfo effect_heap_stats; /* 0x0c454970 */

void effect_heap_init(void)
{
  effect_heap_mem = malloc(0xa0000);
  heap_init(&effect_heap,effect_heap_mem,0xa0000);
  return;
}

void * effect_malloc(int size)
{
  void *ptr;

  ptr = heap_malloc(&effect_heap,size);
  heap_stats_get(&effect_heap,&effect_heap_stats);
  return ptr;
}

void effect_free(void *ptr)
{
  heap_free(&effect_heap,ptr);
  heap_stats_get(&effect_heap,&effect_heap_stats);
  return;
}

void effect_heap_snapshot(void)
{
  heap_blocks_snapshot(&effect_heap,effect_heap_blocks,0x1000);
  return;
}

int effect_heap_used_get(void)
{
  return effect_heap_stats.uordblks;
}

void effect_malloc_freed_check(void *ptr)
{
  if (*(int *)ptr == -0x22222223) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libeffect/src/effect_malloc.c",100,
             "effect malloc ptr is freed!?\n");
  }
  return;
}
