/*
 * vf4lib/libpoly/src/load_poly.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int mdl_keep_node_req; /* 0x0c322734 */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern int load_poly_bank; /* 0x0c3ba010 */
extern char load_poly_name[256]; /* 0x0c3ba068 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern int objbank_mem_sizes[21]; /* 0x0c3ba014 */

void load_poly(int objbank_num, int set, static_heap_rec *buf, static_heap_rec *mem)
{
  char *obj_name;
  char *mdl_name;
  char *tex_name;
  int size;
  obj_mem m;
  static_heap_rec *mb;
  
  mb = &heap_texdata;
  if (buf == 0) {
    buf = &load_poly_default_buf;
  }
  load_poly_obj = buf->addr;
  obj_name = obj_files[set][0];
  mdl_name = obj_files[set][1];
  tex_name = obj_files[set][2];
  strcpy(load_poly_name,obj_name);
  load_poly_bank = objbank_num;
  if (obj_set_loaded(set)) return;
  load_file(obj_name,buf->addr,buf->size);
  load_file(mdl_name,mb->addr,mb->size);
  dcache_tags_clear_uncached();
  load_poly_bank = objbank_num;
  if (obj_set_loaded(set)) return;
  obj_relocate(load_poly_obj,mb->addr);
  mdl_keep_node(load_poly_obj,mb->addr,mdl_keep_node_req);
  size = mdl_total_size(buf->addr,mb->addr);
  if (mem == 0) {
    get_object_mem(&m,size);
  }
  else {
    m = *(obj_mem *)mem;
  }
  objbank_mem_sizes[objbank_num] = m.a;
  mdl_copy_start(buf->addr,mb->addr,m.p);
  while (mdl_copy_step() == 0) ;
  perf_color_mark(0x7f0000);
  objbank_register(objbank_num,set,load_poly_obj,(int)m.p);
  if (objbank_num < 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/load_poly.c",0x73,"assertion `%s\' failed.\n","objbank_num >= 0");
  }
  if (mem != 0) {
    objbanks[objbank_num].mem = 0;
  }
  objbanks[objbank_num].size = size;
  txn_color_list_build(txn_list_current);
  txn_colors_apply(load_poly_obj->nodes);
  txn_swap_list_build(txn_list_current);
  txn_swaps_apply(load_poly_obj->nodes);
  load_file(tex_name,mb->addr,mb->size);
  dcache_tags_clear_uncached();
  perf_color_mark(0x7f007f);
  txn_tune_list_build((txn_entry *)txn_list_current);
  txn_tunes_apply((tex_desc_rec *)load_poly_obj->tex_desc);
  perf_color_mark(0x7f7f);
  objbank_tex_load(objbank_num,(tex_desc_rec *)load_poly_obj->tex_desc);
  txn_list_current_clear();
}

void mdl_keep_node(obj_header *obj, void **mdl, int node)
{
  obj_node **nodes;
  obj_node *nd;
  void **lp;
  int *bp;
  int i, j, k, n, cnt, type;
  uint idx;
  char keep [4096];
  
  idx = node & 0xffff;
  if (node < 0) return;
  nodes = obj->nodes;
  for (i = 0; nodes[i] != 0; i++)
    keep[i] = 0;
  n = i;
  if ((int)idx < n) {
    keep[idx] = 1;
    nd = nodes[idx];
    type = nd->type;
    if (type == 0x13 || type == 0x15) {
      char *f = (char *)nd->file;
      bp = (int *)(f + nd->bones);
      cnt = 0x7b;
      if (type == 0x15) cnt = 0xa2;
      for (k = 0; k < cnt; k++, bp++) {
        if (*bp != 0) {
          for (j = 0; j < n; j++) {
            if (nodes[j] == (obj_node *)*bp) keep[j] = 1;
          }
        }
      }
      lp = nd->list_8c;
      if (lp != 0) {
        for (; *lp != 0; lp++) {
          if (*lp != 0) {
            for (j = 0; j < n; j++) {
              if (nodes[j] == *lp) keep[j] = 1;
            }
          }
        }
      }
    }
  }
  for (i = 0; i < n; i++) {
    if (keep[i] == 0) mdl[i] = 0;
  }
  mdl_keep_node_req = -1;
}

void mdl_keep_node_set(int node)
{
  mdl_keep_node_req = node;
  return;
}

int mdl_keep_node_get(void)
{
  return mdl_keep_node_req;
}
