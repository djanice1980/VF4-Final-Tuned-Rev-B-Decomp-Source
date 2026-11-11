/*
 * vf4lib/libbase/src/load_obj.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint obj_file_sizes[241]; /* 0x0c3b7390 */
extern char demo_kind; /* 0x0c2925db */
extern static_heap_rec heap_debug_obj; /* 0x0c360088 */
extern static_heap_rec rob_obj_ofs[3]; /* 0x0c3600ac */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern static_heap_rec heap_stage_obj; /* 0x0c3600e8 */
extern static_heap_rec heap_efcstg_obj; /* 0x0c36013c */
extern static_heap_rec newopt_obj_ofs[2]; /* 0x0c360148 */
extern static_heap_rec heap_common_obj; /* 0x0c360188 */
extern static_heap_rec objmem_heap_chr[3]; /* 0x0c3601bc */
extern rob_body_vram_sides rob_body_vram_used; /* 0x0c292750 */
extern rob_items_vram_sides rob_items_vram_used; /* 0x0c292758 */
extern char side_entry_flags[3]; /* 0x0c370c84 */

void common_obj_load(void)
{
  int free_before;
  int free_after;
  
  objbanks_restore();
  free_before = tex_vram_free_total();
  base_obj_sets_load();
  free_after = tex_vram_free_total();
  common_obj_vram_used = (uint)(free_before - free_after) >> 10;
  common_sprite_sets_load();
  objbanks_save();
  rob_zanzou_trails_init();
  return;
}

void stage_obj_load(int file_no)
{
  obj_tex_base_set(0x46d0);
  load_poly(3,file_no,&heap_stage_obj,0);
  return;
}

void stage_ef_obj_load(int file_no)
{
  if (file_no != -1) {
    obj_tex_base_set(0x4d50);
    load_poly(4,file_no,&heap_efcstg_obj,0);
  }
  return;
}

void base_obj_sets_load(void)
{
  obj_tex_base_set(0x4670);
  load_poly(0,0,&heap_debug_obj,0);
  obj_tex_base_set(0x4b90);
  load_poly(1,1,&heap_common_obj,0);
  return;
}

void rob_obj_alt_request(int side)
{
  rob_obj_alt_req[side] = 1;
  return;
}

void rob_obj_load(uint side, int chara, int costume)
{
  int chara_no;
  int costume_no;
  int j;
  int item_obj;
  int *item_objs;
  int obj_set;
  static_heap_rec item_blocks[4];
  int free_start;
  int free_body;
  int free_items;

  chara_no = (int)(char)chara;
  costume_no = (int)(char)costume;
  if (chara_no < 0) return;
  if ((side_entry_flags[side] >> 2 & 1) == 0) {
    rob_alt_model_card[side] = 0;
  }
  if (rob_alt_model_card[side] != 0) {
    rob_obj_alt_req[side] = 1;
  }
  obj_tex_base_set(rob_tex_bases[side]);
  rob_obj_alt[side] = rob_obj_alt_req[side];
  rob_obj_alt_req[side] = 0;
  if (rob_obj_alt[side] != 0) {
    obj_set = *(int *)(costume_no * 0x20 + (int)chara_data_get(chara_no)[0x12] + 0x44);
    rob_items_clear(rob_get(side & 0xff));
    txn_list_current_set(0);
  }
  else {
    obj_set = *(int *)(costume_no * 0x20 + (int)chara_data_get(chara_no)[0x12] + 4);
    net_select_pending_apply(side);
    rob_items_setup((int)(char)side,chara_no,(int)(char)costume_no);
    rob_color_build_custom(side,chara_no,(int)(char)costume_no);
  }
  free_start = tex_vram_free_total();
  load_poly(side + 5,obj_set,&rob_obj_ofs[side],&objmem_heap_chr[side]);
  free_body = tex_vram_free_total();
  if (rob_obj_alt[side] == '\0') {
    uchar **data = chara_data_get(chara_no);
    item_objs = (int *)((int)data[0x12] + costume_no * 0x10 + 0x9c);
    txn_list_current_set(0);
    newopt_obj_addr_assign(item_objs,item_blocks,&newopt_obj_ofs[side]);
    for (j = 0; j < 4; j++) {
      item_obj = *(int *)rob_card_item_obj_list((char)side,chara_no,(int)(char)costume_no,j);
      if (0 < item_obj) {
        rob_texcol_part_select(side,j);
        mdl_keep_node_set(item_obj);
        obj_tex_base_set(rob_item_tex_bases[side] + j * 0x80);
        load_poly(side + item_objbank_offsets[j],item_objs[j],&item_blocks[j],0);
      }
    }
  }
  free_items = tex_vram_free_total();
  ((int *)&rob_body_vram_used)[side] = free_start - free_body;
  ((int *)&rob_items_vram_used)[side] = free_body - free_items;
}

void newopt_obj_addr_assign(int *obj_nos, static_heap_rec *blocks, static_heap_rec *heap)
{
  int obj_no;
  uint size;
  char *addr;
  int i;
  uint total;
  
  total = 0;
  addr = heap->addr;
  for (i = 0; i < 4; i++) {
    obj_no = obj_nos[i];
    size = obj_file_sizes[obj_no] + 0x100 & 0xffffff80;
    total = total + size;
    if (total > heap->size) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/load_obj.c",0x11e,"ITEM MEMORY OVER %s(%d)",
               obj_files[obj_no][0],heap->size);
    }
    mem_block_set(&blocks[i],addr,size,"get_newopt_objaddr()");
    addr = addr + size;
  }
  return;
}

void rob_obj_load_queued(uint side, int chara, int costume)
{
  int chara_no;
  int costume_no;
  int *item_objs;
  int j;
  int item_obj;
  int obj_set;
  static_heap_rec item_blocks[4];

  chara_no = (int)(char)chara;
  costume_no = (int)(char)costume;
  if (chara_no < 0) return;
  if ((side_entry_flags[side] >> 2 & 1) == 0) {
    rob_alt_model_card[side] = 0;
  }
  if (rob_alt_model_card[side] != 0) {
    rob_obj_alt_req[side] = 1;
  }
  rob_obj_alt[side] = rob_obj_alt_req[side];
  rob_obj_alt_req[side] = 0;
  if (rob_obj_alt[side] != 0) {
    obj_set = *(int *)(costume_no * 0x20 + (int)chara_data_get(chara_no)[0x12] + 0x44);
    rob_items_clear(rob_get(side & 0xff));
    txn_list_current_set(0);
  }
  else {
    obj_set = *(int *)(costume_no * 0x20 + (int)chara_data_get(chara_no)[0x12] + 4);
    rob_items_setup((int)(char)side,chara_no,costume_no);
    rob_color_build_custom(side,chara_no,costume_no);
  }
  load_poly_req(side + 5,obj_set,&objmem_heap_chr[side],&rob_obj_ofs[side],
               &heap_texdata,&heap_texdata,
               rob_tex_bases[side]);
  if (mode_get_main() != MODE_MAIN_ADVERTISE || demo_kind != '\0') {
    if (rob_obj_alt[side] == '\0') {
    uchar **data = chara_data_get(chara_no);
    item_objs = (int *)((int)data[0x12] + costume_no * 0x10 + 0x9c);
    txn_list_current_set(0);
    newopt_obj_addr_assign(item_objs,item_blocks,&newopt_obj_ofs[side]);
    for (j = 0; j < 4; j++) {
      item_obj = *(int *)rob_card_item_obj_list((char)side,chara_no,costume_no,j);
      if (0 < item_obj) {
        rob_texcol_part_select(side,j);
        mdl_keep_node_set(item_obj);
        load_poly_req(side + item_objbank_offsets[j],item_objs[j],0,
                     &item_blocks[j],&heap_texdata,&heap_texdata,
                     rob_item_tex_bases[side] + j * 0x80);
      }
    }
  }
  }
}

void adv_fly_obj_load_queued(void)
{
  txn_list_current_set(0);
  load_poly_req(3,0x6b,0,&heap_stage_obj,&heap_texdata,&heap_texdata,0x46d0);
  return;
}
