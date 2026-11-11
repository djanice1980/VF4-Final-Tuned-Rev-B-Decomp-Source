/*
 * vf4lib/libpoly/src/pol_uti.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_model_tex_load_step
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 tex_vram_free; /* 0x0c292740 */
extern int obj_tex_base; /* 0x0c3226a4 */
extern uint chunk_copy_size; /* 0x0c3226d4 */
extern int objbank_freeing; /* 0x0c32271c */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern float objbank_tex_step_usec; /* 0x0c3b6c20 */
extern int obj_set_bank[241]; /* 0x0c3b6fc4 */
extern object_mem_rec object_mem_last; /* 0x0c3b7758 */
extern undefined4 poly_shadow_scale; /* 0x0c3226a0 */
extern int obj_set_bank_saved[241]; /* 0x0c3b6858 */
extern objbank objbanks_saved[21]; /* 0x0c3b6c24 */
extern uint obj_file_sizes[241]; /* 0x0c3b7390 */
extern int poly_tex_vram_free_boot; /* 0x0c3b7754 */
extern objbank objbank_empty; /* 0x0c3226a8 */


void poly_system_init(void)
{
  bg_plane_corner_colors(0,0,0);
  vo_border_write_opaque(0);
  render_pipeline_three_frames();
  render_config_texmem_size_put(0x280000);
  elan_lists_start(0x80000,0x10000,0x20000,0x10000,0x10000);
  elan_list_enable_mask_put(0x1f);
  stall_limits_set(1,1);
  texidx_tables_place((int)poly_texidx_work,0x600,12000);
  poly_tex_vram_free_boot = tex_vram_free_total();
  sprite_work_partition(poly_sprite_work,0x600,18000);
  objbanks_init();
  pal_entry_format_select(0);
  pal_groups_setup(poly_pal_groups,0x40);
  obj_palette_table_put(0);
  object_put_init();
  obj_alt_lighting_on();
  obj_vertex_hook_select(1);
  cull_size_threshold_put(0.01);
  modvol_shadow_set_up(0);
  modvol_shadow_scale_put(poly_shadow_scale);
  modvol_instruction_put(1);
  poly_frame_list_end = frame_list_write_end();
  poly_model_heap_top = model_heap_top();
  elan_heaps_init(0x14c0000);
  return;
}

void objbanks_init(void)
{
  int i;

  for (i = 0; i < 0x15; i++) {
    objbanks[i] = objbank_empty;
  }
  for (i = 0; i < 0xf1; i++) {
    obj_set_bank[i] = -1;
  }
  objbanks_save();
  rob_hand_tables_clear();
  txn_lists_clear_all();
  txn_list_current_clear();
  return;
}

int obj_node_typed(obj_node *node)
{
  int t = node->type == 0;
  return t == 0;
}

byte objbank_in_use(int objbank_num)
{
  byte r = 1;
  if (objbank_num < 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0xd0,"assertion `%s\' failed.\n",
             "objbank_num >= 0");
  }
  if (objbanks[objbank_num].nodes == 0) r = 0;
  return r;
}

void chunk_copy_start(void *dst, void *src, uint size)
{
#define RAM_BASE ((void *)0x0c000000)
  int bad;
  chunk_copy_dst = dst;
  chunk_copy_src = src;
  chunk_copy_left = size;
  bad = (dst < RAM_BASE) | (src < RAM_BASE);
  if (bad) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0xe5,"assertion `%s\' failed.\n","0");
  }
  if (chunk_copy_left > chunk_copy_size) chunk_copy_len = chunk_copy_size;
  else chunk_copy_len = size;
}

void * chunk_copy_step(void)
{
  if ((chunk_copy_dst == (void *)0x0) || (chunk_copy_src == 0)) {
    return (void *)0x0;
  }
  memcpy(chunk_copy_dst,chunk_copy_src,chunk_copy_len);
  chunk_copy_dst = (void *)((int)chunk_copy_dst + chunk_copy_len);
  chunk_copy_src = chunk_copy_src + chunk_copy_len;
  chunk_copy_left = chunk_copy_left - chunk_copy_len;
  if (chunk_copy_left == 0) {
    chunk_copy_dst = (void *)0x0;
  }
  if (chunk_copy_left < chunk_copy_len) {
    chunk_copy_len = chunk_copy_left;
  }
  return chunk_copy_dst;
}

void chunk_copy_reset(void)
{
  chunk_copy_size = 0x400;
  chunk_copy_dst = 0;
  chunk_copy_src = 0;
  chunk_copy_left = 0;
  chunk_copy_len = 0;
  return;
}

int mdl_total_size(obj_header *obj, void **mdl)
{
  char * volatile base = (char *)mdl;
  obj_node **nodes;
  int total;
  int i;
  uint sz;
  void **tab;

  mdl_obj_last = obj;
  nodes = obj->nodes;
  tab = mdl;
  total = 0;
  for (i = 0; nodes[i] != 0; i++) {
    if (tab[i] != 0) {
      int *m = (int *)(base + (int)tab[i]);
      obj_node *n = nodes[i];
      sz = model_size(m) + 0x1fU & 0xffffffe0;
      total += sz;
      if (obj_node_typed(n) != 0) total += sz;
    }
  }
  return total;
}

void get_object_mem(obj_mem *mem, int size)
{
  mem->p = 0;
  mem->a = size;
  mem->b = (int)"get_object_mem()";
  if (size != 0) {
    mdl_copy_dst = elan_heap_alloc(size);
    if (mdl_copy_dst == 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x132,"assertion `%s\' failed.\n",
               "0");
    }
    mem->p = mdl_copy_dst;
  }
  object_mem_last.addr = (uint)mem->p;
  object_mem_last.size = mem->a;
  object_mem_last.tag = mem->b;
  return;
}

void mdl_copy_start(obj_header *obj, void **mdl, void *dest)
{
  mdl_copy_busy = 0;
  mdl_copy_index = 0;
  mdl_copy_pass = 0;
  mdl_copy_obj = obj;
  mdl_copy_table = mdl;
  mdl_obj_last = obj;
  mdl_copy_dst = dest;
  return;
}

int mdl_copy_step(void)
{
  obj_node *node;
  int off;
  compiled_model_rec *model;
  uint size;
  compiled_model_rec **dst;
  int idx = mdl_copy_index;
  obj_header *obj = mdl_copy_obj;
  int *mdl = (int *)mdl_copy_table;
  if (obj == 0 || mdl == 0) return 1;
  node = obj->nodes[idx];
  if (node == 0) return 1;
  if (mdl_copy_busy == 0) {
    off = mdl[idx];
    if (off != 0) {
      model = (compiled_model_rec *)((char *)mdl + off);
      size = model_size((int *)model) + 0x1fU & 0xffffffe0;
      if (mdl_copy_pass == 0) dst = &node->model;
      else dst = &node->model2;
      *dst = 0;
      if (size != 0) {
        *dst = (compiled_model_rec *)mdl_copy_dst;
        chunk_copy_start(mdl_copy_dst, model, model->size);
        mdl_copy_dst = mdl_copy_dst + size;
      }
    }
  }
  mdl_copy_busy = (int)chunk_copy_step();
  if (mdl_copy_busy == 0) {
    mdl_copy_pass++;
    if ((mdl_copy_pass == 1 && obj_node_typed(node) == 0) || mdl_copy_pass == 2) {
      mdl_copy_pass = 0;
      mdl_copy_index++;
    }
  }
  return 0;
}

void mdl_copy_reset(void)
{
  mdl_copy_dst = 0;
  mdl_copy_busy = 0;
  mdl_copy_index = 0;
  mdl_copy_pass = 0;
  mdl_copy_obj = 0;
  mdl_copy_table = 0;
  chunk_copy_reset();
  return;
}

void obj_relocate_bones(obj_node *node)
{
  obj_header *f;
  int *p;
  int n;
  int i;
  f = node->file;
  n = 0x7b;
  if (node->type == 0x15) n = 0xa2;
  p = (int *)((int)&f->nodes + node->bones);
  for (i = 0; i < n; i++, p++) {
    if (*p != 0) *p = (int)&f->nodes + *p;
  }
  if (node->pairs_90 != 0) {
    p = (int *)((int)&f->nodes + node->pairs_90);
    while (*p != 0) {
      *p = (int)&f->nodes + *p;
      p += 2;
    }
  }
}

bool obj_set_loaded(int set)
{
  int r = obj_set_bank[set] == -1;
  return r == 0;
}

void objbank_register(int objbank_num, int set, obj_header *obj, int mem)
{
  obj_node **p;
  int n;
  char **nm;
  n = 0;
  p = obj->nodes;
  if (*p != (obj_node *)0x0) {
    if (objbank_num < 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1c6,"assertion `%s\' failed.\n",
               "objbank_num >= 0");
    }
    obj_set_bank[set] = objbank_num;
    objbanks[objbank_num].nodes = p;
    nm = obj->names;
    objbanks[objbank_num].names = nm;
    objbanks[objbank_num].set = set;
    objbanks[objbank_num].tex_base = obj_tex_base;
    objbanks[objbank_num].tex_names = obj->list3;
    objbanks[objbank_num].name = *nm;
    while (*p++ != 0) n++;
    objbanks[objbank_num].count = n;
    objbanks[objbank_num].mem = mem;
  }
}

void obj_relocate(obj_header *obj, void *buf)
{
  obj_node *n;
  obj_node **np;
  char **names;
  short *desc;
  void **l;
  char *s;

  if ((obj_header *)0xe000000 < obj) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1e4,"assertion `%s\' failed.\n","0");
  }
  np = (obj_node **)((int)obj + (int)obj->nodes);
  if ((obj_node **)0xe000000 < np) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1e7,"assertion `%s\' failed.\n","0");
  }
  obj->nodes = np;
  if ((obj_node **)0xe000000 < np) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1e9,"assertion `%s\' failed.\n","0");
  }
  names = (char **)((int)obj + (int)obj->names);
  if ((char **)0xe000000 < names) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1eb,"assertion `%s\' failed.\n","0");
  }
  obj->names = names;
  desc = (short *)((int)obj + (int)obj->tex_desc);
  if ((short *)0xe000000 < desc) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x1ee,"assertion `%s\' failed.\n","0");
  }
  obj->tex_desc = desc;
  np = obj->nodes;
  while (*np != 0) {
    if (0xe000000 < (int)*np) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",499,"assertion `%s\' failed.\n","0");
    }
    *np = (obj_node *)((int)*np + (int)obj);
    n = *np;
    np++;
    n->model = 0;
    n->model2 = 0;
    n->file = obj;
    if (n->type == 0x13 || n->type == 0x15) {
      obj_relocate_bones(n);
      obj_relocate_list_80(n);
      obj_relocate_list_8c(n);
    }
  }
  np = obj->nodes;
  while (*names != 0) {
    *names = *names + (int)obj;
    n = *np++;
    n->name = *names;
    names++;
  }
  l = obj->list3;
  if (l != 0) {
    l = (void **)((int)l + (int)obj);
    obj->list3 = l;
    while (*l != 0) {
      *l = (void *)((int)*l + (int)obj);
      l++;
    }
  }
  tex_desc_relocate((tex_desc_rec *)desc,(int)buf);
  return;
}

void obj_relocate_list_80(obj_node *node)
{
  void **p;
  char *v;
  obj_header *file;

  file = node->file;
  if (node->list_80 != (void **)0x0) {
    node->list_80 = (void **)((int)node->list_80 + (int)file);
    p = node->list_80;
    goto test;
  loop:
    v = v + (int)file;
    *p = v;
    osage_chain_relocate((int *)v,(int)file);
    p++;
  test:
    if ((v = *p) != 0) goto loop;
  }
}

void obj_relocate_list_8c(obj_node *node)
{
  int *p;
  int v;
  int base;

  base = (int)node->file;
  if (node->list_8c != (void **)0x0) {
    node->list_8c = (void **)((int)node->list_8c + base);
    p = (int *)node->list_8c;
    goto test;
  body:
    *p = v + base;
    p++;
  test:
    v = *p;
    if (v != 0) goto body;
  }
}

void objbank_tex_load_start(int objbank_num, tex_desc_rec *descs)
{
  obj_node **nodes;
  objbank *p;

  nodes = objbanks[objbank_num].nodes;
  if (objbank_num < 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x24e,"assertion `%s\' failed.\n",
             "objbank_num >= 0");
  }
  objbank_tex_node = 0;
  objbank_tex_cursor = 0;
  objbank_tex_pass = 0;
  objbank_tex_nodes = nodes;
  objbank_tex_descs = descs;
  p = &objbanks[objbank_num];
  objbank_tex_bank = p;
  p->tex_descs = (int)descs;
  return;
}

int objbank_tex_load_step(void)
{
  obj_node **nodes;
  float t0;
  int idx;
  tex_desc_rec *descs;
  compiled_model_rec *model;

  idx = objbank_tex_node;
  nodes = objbank_tex_nodes;
  descs = objbank_tex_descs;
  t0 = (float)(int)stopwatch_usec();
  if ((nodes == 0) || (nodes[idx] == 0)) {
    if (objbank_tex_bank != 0) {
      objbank_tex_bank->state = 1;
      objbank_tex_bank = 0;
    }
    return 1;
  }
  if (objbank_tex_pass == 0) {
    model = nodes[idx]->model;
  }
  else {
    model = nodes[idx]->model2;
  }
  objbank_tex_model = model;
  if (model != 0) {
    model_tex_load_step(model,descs,objbank_tex_cursor,(int)&objbank_tex_cursor);
  }
  if (objbank_tex_cursor == 0) {
    objbank_tex_pass = objbank_tex_pass + 1;
    if ((objbank_tex_pass == 1 && nodes[idx]->model2 == 0) || objbank_tex_pass == 2) {
      objbank_tex_pass = 0;
      objbank_tex_node = objbank_tex_node + 1;
    }
  }
  objbank_tex_step_usec = (float)(int)stopwatch_usec() - t0;
  return 0;
}

void objbank_tex_load_reset(void)
{
  objbank_tex_cursor = 0;
  objbank_tex_node = 0;
  objbank_tex_pass = 0;
  objbank_tex_nodes = 0;
  objbank_tex_descs = 0;
  return;
}

void objbank_tex_load(int objbank_num, tex_desc_rec *descs)
{
  int done;

  objbank_tex_load_start(objbank_num,descs);
  do {
    done = objbank_tex_load_step();
  } while (done == 0);
  tex_vram_free = tex_vram_free_total();
  return;
}

void objbank_free(int objbank_num)
{
  int done;

  objbank_free_start(objbank_num);
  do {
    done = objbank_free_step();
  } while (done == 0);
  return;
}

void objbank_free_start(int objbank_num)
{
  objbank_free_start_bank = objbank_num;
  if (objbank_num < 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/pol_uti.c",0x2a3,"assertion `%s\' failed.\n",
             "objbank_num >= 0");
  }
  if (objbanks[objbank_num].nodes != 0) {
    obj_tex_base_set(objbanks[objbank_num].tex_base);
    if (objbanks[objbank_num].mem != 0) {
      elan_heap_free((void *)objbanks[objbank_num].mem);
      objbanks[objbank_num].mem = 0;
    }
    if (objbanks[objbank_num].tex_descs != 0) {
      texdesc_free_start((tex_desc_rec *)objbanks[objbank_num].tex_descs);
    }
    obj_set_bank[objbanks[objbank_num].set] = -1;
    tex_vram_free = tex_vram_free_total();
    objbank_freeing = objbank_num;
    objbanks[objbank_num].state = 3;
  }
}

int objbank_free_step(void)
{
  objbank *e;
  objbank *s;

  if (objbank_freeing == -1) return 1;
  if (texdesc_free_step() == 1) {
  e = &objbanks[objbank_freeing];
  s = &objbank_empty;
  e->nodes = s->nodes;
  e->names = s->names;
  e->count = s->count;
  e->set = s->set;
  e->tex_base = s->tex_base;
  e->mem = s->mem;
  e->tex_descs = s->tex_descs;
  e->tex_names = s->tex_names;
  e->name = s->name;
  e->size = s->size;
  e->state = 0;
  objbank_freeing = -1;
  return 1;
  }
  return 0;
}

void tex_desc_relocate(tex_desc_rec *desc, int base)
{
  short width;

  width = desc->width;
  while (width != 0) {
    desc->data = desc->data + base;
    desc = desc + 1;
    width = desc->width;
  }
  return;
}

void objbanks_restore(void)
{
  texidx_snapshot_restore();
  tex_vram_free = tex_vram_free_total();
  obj_set_bank_restore();
  return;
}

void objbanks_save(void)
{
  int i;
  for (i = 0; i < 0x15; i++) {
    objbanks_saved[i] = objbanks[i];
  }
  for (i = 0; i < 0xf1; i++) {
    obj_set_bank_saved[i] = obj_set_bank[i];
  }
  texidx_snapshot_save();
}

void obj_set_bank_restore(void)
{
  int i;
  int n;
  for (i = 0, n = 20; i <= n; i++) {
  }
  for (i = 0; i < 241; i++) {
    int *d = obj_set_bank;
    n = (int)obj_set_bank_saved;
    d[i] = *(int *)(n + i * 4);
  }
}

void obj_tex_base_set(int tex_base)
{
  obj_tex_base = tex_base;
  texidx_base_put(tex_base);
  return;
}

void obj_file_sizes_read(void)
{
  int i;
  char **tbl;
  uint size;
  dimm_file *fp;
  byte tail [8];
  uint pad;

  for (i = 0, tbl = obj_files[0]; i < 0xf1; i++) {
    size = 0xdeaddead;
    fp = dimm_fopen(tbl[i * 3],"rb");
    if (fp != (dimm_file *)0x0) {
      pad = fp->size & 3;
      dimm_fseek(fp,-(pad + 4),2);
      dimm_fread_pio(tail,8,1,fp);
      size = (int)(char)tail[pad + 3] << 0x18 | (uint)tail[pad + 2] << 0x10 |
              (uint)tail[pad + 1] << 8 | (uint)tail[pad];
      dimm_fclose(fp);
    }
    obj_file_sizes[i] = size;
  }
}
