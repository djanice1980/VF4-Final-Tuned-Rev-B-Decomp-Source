/*
 * vf4lib/libpoly/src/load_poly_bg.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 tex_vram_free; /* 0x0c292740 */
extern uint poly_queue_cur; /* 0x0c322738 */
extern uint poly_queue_next; /* 0x0c32273c */
extern float poly_queue_time_limits[5]; /* 0x0c322740 */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern uint poly_queue_busy; /* 0x0c3ba1b0 */
extern int objbank_mem_sizes[21]; /* 0x0c3ba014 */
extern poly_load_entry poly_queue[32]; /* 0x0c551e20 */
extern char load_poly_name[256]; /* 0x0c3ba068 */


int load_poly_req(int objbank_num, int set, static_heap_rec *mem, static_heap_rec *obj_buf, static_heap_rec *mdl_buf, static_heap_rec *tex_buf, int tex_base)
{
  int r;
  poly_load_entry *e;
  char *obj;
  char *mdl;
  unsigned int mask;
  char *tex;
  char **t;
  r = poly_queue_entry_alloc(&e);
  mask = 0;
  if (r == 0)
  {
    return 0;
  }
  t = obj_files[set];
  obj = t[0];
  mdl = t[1];
  tex = t[2];
  strcpy(load_poly_name, obj);
  e->mask = mask;
  e->state = 1;
  e->tex_base = tex_base;
  mem_block_copy((static_heap_rec *)&e->mem, mem);
  mem_block_copy(&e->obj, obj_buf);
  mem_block_copy(&e->mdl, mdl_buf);
  mem_block_copy(&e->buf, tex_buf);
  e->bank = objbank_num;
  e->set = set;
  strcpy(e->obj_name, obj);
  strcpy(e->mdl_name, mdl);
  strcpy(e->tex_name, tex);
  e->txn_list = txn_list_current;
  e->node = mdl_keep_node_get();
  mdl_keep_node_set(-1);
  return r;
}

int objbank_free_req(int objbank_num)
{
  uint bit;
  poly_load_entry *e;

  bit = poly_queue_entry_alloc(&e);
  if (!(bit != 0)) {
    return 0;
  }
  e->state = 7;
  e->mask = 0;
  e->bank = objbank_num;
  e->set = -1;
  objbank_free_start(objbank_num);
  return bit;
}

void load_poly_bg_service(void)
{
  poly_load_entry *q;
  int timeout;
  int i;
  int size;
  int t0;
  obj_mem mem;

  timeout = 0;
  perf_color_mark(0x7f7f7f);
  while (1) {
    for (i = 0; i < 32; i++) {
      poly_load_entry *e = poly_queue;
      if (e[i].state != 0)
        break;
    }
    if (i == 32) {
      poly_queue_next = 0;
      poly_queue_cur = 0;
      break;
    }
    if (timeout || (float)(int)stopwatch_usec() > poly_queue_time_limits[0])
      break;
    q = &poly_queue[poly_queue_cur];
    if (load_pending_get() & q->mask)
      break;
    switch (q->state) {
    case 1:
      perf_color_mark(0x7f00);
      q->mask = load_req(q->obj_name, q->obj.addr, q->obj.size) | load_req(q->mdl_name, q->mdl.addr, q->mdl.size);
      q->state = 2;
      if (q->bank < 0)
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/load_poly_bg.c", 0xf7, "assertion `%s' failed.\n", "queue->objbank_num >= 0");
      objbanks[q->bank].state = 2;
      break;
    case 2:
      t0 = stopwatch_usec();
      if (obj_set_loaded(q->set)) {
        q->state = 0;
        break;
      }
      load_poly_bg_loaded_check_usec = stopwatch_usec() - t0;
      load_poly_bg_relocate_usec = stopwatch_usec();
      perf_color_mark(0x7f0000);
      load_poly_obj = q->obj.addr;
      obj_relocate(load_poly_obj, q->buf.addr);
      load_poly_bg_relocate_usec = stopwatch_usec() - load_poly_bg_relocate_usec;
      load_poly_bg_keep_node_usec = stopwatch_usec();
      mdl_keep_node(q->obj.addr, q->mdl.addr, q->node);
      load_poly_bg_keep_node_usec = stopwatch_usec() - load_poly_bg_keep_node_usec;
      load_poly_bg_mem_usec = stopwatch_usec();
      size = mdl_total_size(q->obj.addr, q->mdl.addr);
      if (q->mem.p == 0)
        get_object_mem(&mem, size);
      else
        mem = q->mem;
      load_poly_bg_mem_usec = stopwatch_usec() - load_poly_bg_mem_usec;
      objbank_mem_sizes[q->bank] = mem.a;
      load_poly_bg_copy_start_usec = stopwatch_usec();
      mdl_copy_start(q->obj.addr, q->mdl.addr, mem.p);
      load_poly_bg_copy_start_usec = stopwatch_usec() - load_poly_bg_copy_start_usec;
      obj_tex_base_set(q->tex_base);
      load_poly_bg_register_usec = stopwatch_usec();
      objbank_register(q->bank, q->set, load_poly_obj, (int)mem.p);
      load_poly_bg_register_usec = stopwatch_usec() - load_poly_bg_register_usec;
      if (q->mem.p != 0)
        objbanks[q->bank].mem = 0;
      objbanks[q->bank].size = size;
      load_poly_bg_build_usec = stopwatch_usec() - t0;
      q->state = 4;
      break;
    case 4:
      if ((float)(int)stopwatch_usec() > poly_queue_time_limits[1]) {
        timeout = 1;
        break;
      }
      if (load_pending_get() & q->mask)
        break;
      perf_color_mark(0x7f7f);
      if (mdl_copy_step() == 0)
        break;
      txn_swap_list_build(q->txn_list);
      txn_swaps_apply(load_poly_obj->nodes);
      txn_color_list_build(q->txn_list);
      txn_colors_apply(load_poly_obj->nodes);
      objbank_tex_load_start(q->bank, (tex_desc_rec *)load_poly_obj->tex_desc);
      q->mask |= load_req(q->tex_name, q->buf.addr, q->buf.size);
      txn_tune_list_build(q->txn_list);
      txn_tune_start((tex_desc_rec *)load_poly_obj->tex_desc);
      q->state = 3;
      break;
    case 3:
      if ((float)(int)stopwatch_usec() > poly_queue_time_limits[2]) {
        timeout = 1;
        break;
      }
      perf_color_mark(0x7f007f);
      if (txn_tune_step())
        q->state = 5;
      break;
    case 5:
      if ((float)(int)stopwatch_usec() > poly_queue_time_limits[3]) {
        timeout = 1;
        break;
      }
      perf_color_mark(0x7f7f00);
      obj_tex_base_set(q->tex_base);
      load_poly_obj = q->obj.addr;
      if (objbank_tex_load_step()) {
        q->state = 0;
        q->mask = 0;
        txn_list_current_clear();
      }
      break;
    case 7:
      if ((float)(int)stopwatch_usec() > poly_queue_time_limits[4]) {
        timeout = 1;
        break;
      }
      perf_color_mark(0xff7f7f);
      if (objbank_free_step())
        q->state = 0;
      break;
    case 8:
      q->mask = load_req(q->tex_name, q->buf.addr, q->buf.size);
      q->state = 9;
      break;
    case 9:
      if ((float)(int)stopwatch_usec() > poly_queue_time_limits[1]) {
        timeout = 1;
        break;
      }
      if ((load_pending_get() & q->mask) == 0) {
        perf_color_mark(0xffff);
        obj_tex_base_set(q->tex_base);
        sprite_records_make_list((int)q->obj.addr, 0);
        q->mask = 0;
        q->state = 0;
      }
      break;
    case 10:
      break;
    case 0:
      if (++poly_queue_cur > 31)
        poly_queue_cur = 0;
      break;
    }
  }
  for (i = 0; i < 32; i++) {
    poly_load_entry *e = poly_queue;
    if (e[i].state == 0)
      poly_queue_busy &= ~(1 << i);
  }
  tex_vram_free = tex_vram_free_total();
  perf_color_mark(0);
}

void poly_queue_reset(void)
{
  poly_load_entry *p;
  int i;

  p = poly_queue;
  poly_queue_busy = 0;
  for (i = 0; i < 0x20; i++, p++) {
    p->state = 0;
  }
  p = &poly_queue[poly_queue_cur];
  if (p->state != 0 && 0 < p->bank) {
    objbank_free(p->bank);
  }
  mdl_copy_reset();
  objbank_tex_load_reset();
  txn_tune_reset();
  poly_queue_cur = 0;
  poly_queue_next = 0;
  txn_list_current_clear();
}

uint poly_queue_entry_alloc(poly_load_entry **out)
{
  int i;
  uint cur;
  poly_load_entry *e;

  for (i = 0; i < 32; i++) {
    cur = poly_queue_next;
    e = &poly_queue[cur];
    if (e->state == 0) {
      poly_queue_busy |= 1 << cur;
      if (++poly_queue_next > 31) poly_queue_next = 0;
      break;
    }
  }
  if (i == 32) return 0;
  *out = e;
  return 1 << cur;
}

uint poly_queue_busy_get(void)
{
  return poly_queue_busy;
}
