/*
 * vf4lib/libbase/src/static_heap.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern static_heap_ent static_heap_table[18]; /* 0x0c168088 */
extern static_heap_rec rob_obj_ofs[3]; /* 0x0c3600ac */
extern static_heap_rec aet_buf; /* 0x0c3600f4 */
extern static_heap_rec heap_scr_obj; /* 0x0c360100 */
extern static_heap_rec objmem_rob_hand[3]; /* 0x0c36010c */
extern static_heap_rec spr_sel2_top; /* 0x0c360130 */
extern static_heap_rec newopt_obj_ofs[2]; /* 0x0c360148 */
extern static_heap_rec static_heap_empty; /* 0x0c360160 */
extern static_heap_rec spr_sel_top; /* 0x0c36017c */
extern static_heap_rec spr_sel_base_top; /* 0x0c3601a4 */
extern static_heap_rec spr_sel3_top; /* 0x0c3601b0 */
extern static_heap_rec objmem_heap_chr[3]; /* 0x0c3601bc */
extern static_heap_rec nick_comm_buf; /* 0x0c360018 */
extern static_heap_rec nick_game_buf; /* 0x0c3600dc */
extern static_heap_rec spr_cmn_top; /* 0x0c3601e0 */

void static_heap_alloc_main(static_heap_rec *block, size_t size, char *label)
{
  void *p;

  p = malloc(size);
  mem_block_set(block,p,size,label);
  if (block->addr == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/static_heap.c",0x6e,
             "sh_bdi_malloc(): No memory for [%s]",label);
  }
  return;
}

void static_heap_alloc_elan(static_heap_rec *block, int size, char *label)
{
  void *p;

  p = elan_heap_alloc(size);
  mem_block_set(block,p,size,label);
  if (block->addr == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/static_heap.c",0x79,
             "sh_bdi_elan(): No memory for [%s]",label);
  }
  return;
}

void static_heaps_alloc_all(void)
{
  uint i;

  for (i = 0; i < 0x12; i++)
    static_heap_alloc_main((static_heap_table + i)->block,(static_heap_table + i)->size,
                 (static_heap_table + i)->label);
  for (i = 0; i < 3; i++)
    static_heap_alloc_main(&rob_obj_ofs[i],0xc0000,"rob_obj_ofs");
  for (i = 0; i < 2; i++)
    static_heap_alloc_main(&newopt_obj_ofs[i],0x180000,"newopt_obj_ofs");
  mem_block_set(&static_heap_empty,0,0,"");
  static_heap_alloc_main(&heap_scr_obj,0x10000,"heap_scr_obj");
  static_heap_alloc_main(&spr_cmn_top,0x201000,"spr_cmn_top");
  static_heap_alloc_main(&aet_buf,0xccccc,"aet_buf");
  static_heap_alloc_main(&nick_game_buf,0x2d000,"nick_game_buf");
  static_heap_alloc_main(&nick_comm_buf,0x27000,"nick_comm_buf");
  mem_block_set(&spr_sel_top,elan_gap_alloc(0x300000),0x300000,"spr_sel_top");
  if (spr_sel_top.addr == 0)
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/static_heap.c",0xb6,
             "get_static_heap(): No memory for [%s]",spr_sel_top.label);
  static_heap_alloc_elan(&spr_sel2_top,0x300000,"spr_sel2_top");
  static_heap_alloc_elan(&spr_sel3_top,0x180000,"spr_sel3_top");
  static_heap_alloc_elan(&spr_sel_base_top,0x200000,"spr_sel_base_top");
  for (i = 0; i < 3; i++)
    static_heap_alloc_elan(&objmem_heap_chr[i],0x140000,"objmem_heap_chr[]");
  for (i = 0; i < 3; i++)
    static_heap_alloc_elan(&objmem_rob_hand[i],0xc0000,"objmem_rob_hand[]");
  zanzou_heaps_create();
  rob_zanzou_heaps_alloc();
  item_zanzou_heaps_alloc();
}
