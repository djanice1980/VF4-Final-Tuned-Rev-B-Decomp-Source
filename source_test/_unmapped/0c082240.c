/*
 * _unmapped/0c082240.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

int FUN_0c082260(heap_block *b)
{
  return *((char *)b + b->user_request_size + 0x20) == '+';
}

void FUN_0c0822c0(heap_block *b)
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

void FUN_0c0822e0(heap_block *b, heap_block *after)
{
  heap_block *next;

  next = after->next;
  b->next = next;
  after->next = b;
  b->prev = after;
  next->prev = b;
  return;
}
