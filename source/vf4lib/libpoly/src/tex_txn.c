/*
 * vf4lib/libpoly/src/tex_txn.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar * txn_tune_list; /* 0x0c322754 */
extern uchar * txn_color_list; /* 0x0c322758 */
extern uchar * txn_swap_list; /* 0x0c32275c */
extern uchar * texdesc_free_cursor; /* 0x0c322770 */
extern int texdesc_free_slot; /* 0x0c322774 */
extern int txn_tune_entries[97][4]; /* 0x0c5540a0 */
extern int txn_color_entries[97][3]; /* 0x0c5546b0 */
extern int txn_swap_entries[97][2]; /* 0x0c554b3c */

void txn_list_current_set(txn_entry *list)
{
  txn_list_current = list;
  return;
}

void txn_list_current_clear(void)
{
  txn_list_current = 0;
  return;
}

void txn_list_append(txn_entry *list, txn_entry *other)
{
  txn_entry *d = list;
  txn_entry *s = other;
  if (s != (txn_entry *)0x0) {
    while (d->tex_no != -2) d++;
    while (s->tex_no != -2) {
      if (s->tex_no != -1) {
        { txn_entry *t = d; d++; memcpy(t,s,0x10); }
      }
      s++;
    }
  }
  d->tex_no = -2;
  return;
}

void txn_color_list_build(txn_entry *list)
{
  int n;
  int i;
  txn_entry *s;
  int tex_no;

  n = 0;
  if (list != 0) {
    for (i = 0; i < 0x60; i++) {
      s = (txn_entry *)(i * 0x10 + (int)list);
      tex_no = s->tex_no;
      if (tex_no == -2) break;
      if (tex_no != -1) {
        txn_color_entry *t = (txn_color_entry *)&txn_color_entries[0][0];
        txn_color_entry *q = &t[n];
        q->tex_no = tex_no;
        q->color_a[0] = s->color_a[0];
        q->color_a[1] = s->color_a[1];
        q->color_a[2] = s->color_a[2];
        q->color_on = s->color_on;
        q->color_b[0] = s->color_b[0];
        q->color_b[1] = s->color_b[1];
        q->color_b[2] = s->color_b[2];
        n = n + 1;
      }
    }
    { txn_color_entry *t = (txn_color_entry *)&txn_color_entries[0][0]; t[n].tex_no = -1; }
    txn_color_list_put(&txn_color_entries[0][0]);
  }
  return;
}

void txn_tune_list_build(txn_entry *list)
{
  typedef struct { txn_tune_entry e[0x61]; } tune_tab;
  int n;
  int i;
  int tex_no;
  int swap;
  txn_entry *e;

  n = 0;
  if (list != 0) {
    for (i = 0; i < 0x60; i++) {
      e = (txn_entry *)(i * 16 + (int)list);
      tex_no = e->tex_no;
      if (tex_no == -2) break;
      if (tex_no != -1 && (*(short *)&e->brightness != 0 || e->hue != 0)) {
        swap = ((txn_entry *)(i * 16 + (int)list))->swap_tex_no;
        if (swap >= 0) ((tune_tab *)&txn_tune_entries[0][0])->e[n].tex_no = swap;
        else ((tune_tab *)&txn_tune_entries[0][0])->e[n].tex_no = tex_no;
        ((tune_tab *)&txn_tune_entries[0][0])->e[n].brightness = ((txn_entry *)(i * 16 + (int)list))->brightness;
        ((tune_tab *)&txn_tune_entries[0][0])->e[n].saturation = ((txn_entry *)(i * 16 + (int)list))->saturation;
        ((tune_tab *)&txn_tune_entries[0][0])->e[n].hue = (((txn_entry *)(i * 16 + (int)list))->hue << 15) / 180;
        n++;
      }
    }
    {
      txn_tune_entry *t = (txn_tune_entry *)&txn_tune_entries[0][0];
      t[n].tex_no = -1;
      txn_tune_list_put(t);
    }
  }
}

void txn_swap_list_build(txn_entry *list)
{
  int n;
  int i;
  txn_entry *s;
  txn_swap_entry *d;

  n = 0;
  if (list == 0) return;
  for (i = 0; i < 0x60; i++) {
    s = list + i;
    if (s->tex_no == -2) break;
    if (s->tex_no != -1 && s->swap_tex_no >= 0) {
      d = (txn_swap_entry *)&txn_swap_entries[0][0] + n;
      d->tex_no = s->tex_no;
      d->swap_tex_no = s->swap_tex_no;
      d->swap_tex_no = s->swap_tex_no;
      n++;
    }
  }
  {
    txn_swap_entry *e = (txn_swap_entry *)&txn_swap_entries[0][0];
    e[n].tex_no = -1;
    txn_swap_list_put(e);
  }
}

void txn_tune_list_put(void *list)
{
  txn_tune_list = list;
  return;
}

void txn_tunes_apply(tex_desc_rec *descs)
{
  txn_tune_entry *p;
  tex_desc_rec *desc;

  p = (txn_tune_entry *)txn_tune_list;
  if (p != 0) {
    for (; p->tex_no != -1; p++) {
      desc = descs + p->tex_no;
      tex_tune_brightness = p->brightness / 100.0;
      tex_tune_saturation = p->saturation / 100.0;
      tex_tune_hue = p->hue;
      tex_tune_start(desc);
      while (tune_texture_step() != 0);
    }
    txn_tune_list = 0;
  }
}

void txn_tune_start(tex_desc_rec *descs)
{
  if (txn_tune_list != 0) {
    txn_tune_descs = descs;
    txn_tune_cursor = (txn_tune_entry *)txn_tune_list;
    txn_tune_running = 0;
  }
  return;
}

int tex_desc_count(tex_desc_rec *desc)
{
  short width;
  int n;

  n = 0;
  width = desc->width;
  while (width != 0) {
    desc = desc + 1;
    n = n + 1;
    width = desc->width;
  }
  return n;
}

int txn_tune_step(void)
{
  if ((txn_tune_descs == 0) || (txn_tune_cursor == (txn_tune_entry *)0x0)) {
    return 1;
  }
  else {
    if (txn_tune_running == 0) {
      if ((txn_tune_cursor->tex_no == -1) ||
         (tex_desc_count(txn_tune_descs) <= txn_tune_cursor->tex_no)) {
        txn_tune_reset();
        return 1;
      }
      tex_tune_brightness = txn_tune_cursor->brightness / 100.0;
      tex_tune_saturation = txn_tune_cursor->saturation / 100.0;
      tex_tune_hue = txn_tune_cursor->hue;
      tex_tune_start(txn_tune_descs + txn_tune_cursor->tex_no);
      txn_tune_running = 1;
      txn_tune_cursor = txn_tune_cursor + 1;
    }
    else {
      txn_tune_running = tune_texture_step();
    }
    return 0;
  }
}

void txn_tune_reset(void)
{
  txn_tune_descs = 0;
  txn_tune_cursor = 0;
  txn_tune_running = 0;
  txn_tune_list = 0;
  return;
}

void txn_color_list_put(void *list)
{
  txn_color_list = list;
  return;
}

void txn_colors_apply(obj_node **nodes)
{
  int tex_no;
  obj_node *node;
  obj_node **q;
  txn_color_entry *e;
  uint c[2];
  if (txn_color_list != 0) {
    for (e = (txn_color_entry *)txn_color_list; e->tex_no != -1; e++) {
      tex_no = e->tex_no;
      if (e->color_on != 0) {
        c[1] = e->color_b[0] << 16 | e->color_b[1] << 8 | e->color_b[2];
        c[0] = e->color_a[0] << 16 | e->color_a[1] << 8 | e->color_a[2];
        q = nodes;
        while ((node = *q++) != 0) {
          node_tex_color_set(node,node->model,tex_no,c);
          node_tex_color_set(node,node->model2,tex_no,c);
        }
      }
    }
    txn_color_list = 0;
  }
}

void node_tex_color_set(obj_node *node, compiled_model_rec *model, uint tex_no, uint *colors)
{
  uint *p;
  uint *d;
  uint a, b;

  if ((model != 0) && (node->tex_refs_offset != 0)) {
    for (p = (uint *)(node->tex_refs_offset + (int)node->file); *p != 0; p += 4) {
      if (p[3] == tex_no) {
        uint w = *p;
        a = colors[0];
        b = colors[1];
        d = (uint *)((int)model + (w >> 2) * 4);
        *(d - 21) = a;
        *(d - 20) = b;
        *(d - 19) = a;
        *(d - 18) = b;
      }
    }
  }
}

void txn_swap_list_put(void *list)
{
  txn_swap_list = list;
  return;
}

void txn_swaps_apply(obj_node **nodes)
{
  txn_swap_entry *list;
  txn_swap_entry *q;
  obj_node *node;

  q = (txn_swap_entry *)txn_swap_list;
  list = q;
  if (q != (txn_swap_entry *)0x0) {
    while ((node = *nodes++) != 0) {
      for (q = list; q->tex_no != -1; q++) {
        node_tex_swap(node,node->model,q);
        node_tex_swap(node,node->model2,q);
      }
    }
    txn_swap_list = 0;
  }
  return;
}

void node_tex_swap(obj_node *node, compiled_model_rec *model, txn_swap_entry *swap)
{
  uint *p;
  uint w;
  if ((model != 0) && (node->tex_refs_offset != 0)) {
    p = (uint *)(node->tex_refs_offset + (int)node->file);
    while (*p != 0) {
      w = *p;
      if (p[3] == swap->tex_no) {
        *(uint *)((int)model + (w >> 2) * 4 - 0x3c) = swap->swap_tex_no;
        *(uint *)((int)model + (w >> 2) * 4 - 0x2c) = swap->swap_tex_no;
      }
      p += 4;
    }
  }
}

void texidx_snapshot_restore(void)
{
  int i;
  for (i = 0; i < texidx_count; i++) {
    if ((texidx_map[i].use_count != 0) && (texidx_map_saved[i].use_count == 0)) {
      tex_slot_current = tex_slot_table + texidx_map[i].slot_no;
      if ((tex_slot_current->in_use != 0) && (tex_slot_current->status == 0)) {
        surface_memory_return_by_kind(&tex_slot_current->surface);
      }
      tex_slot_current->status = -1;
      tex_slot_current->in_use = 0;
      tex_slot_current->slot_id = -1;
    }
  }
  memcpy(texidx_map,texidx_map_snapshot,texidx_count << 3);
  memcpy(texidx_bitmap,texidx_bitmap_snapshot,(uint)(texidx_count << 2) >> 5);
}

void texidx_snapshot_save(void)
{
  texidx_map_snapshot = texidx_map_saved;
  texidx_bitmap_snapshot = texidx_bitmap_saved;
  memcpy(texidx_map_saved,texidx_map,texidx_count << 3);
  memcpy(texidx_bitmap_snapshot,texidx_bitmap,(uint)(texidx_count << 2) >> 5);
}

void texdesc_free(tex_desc_rec *descs)
{
  int done;

  texdesc_free_start(descs);
  do {
    done = texdesc_free_step();
  } while (done == 0);
  return;
}

void texdesc_free_start(tex_desc_rec *descs)
{
  texdesc_free_cursor = (uchar *)descs;
  texdesc_free_slot = texidx_base;
  return;
}

int texdesc_free_step(void)
{
  int idx;
  int n;
  idx = texdesc_free_slot;
  if (texdesc_free_cursor == 0 || texdesc_free_slot == -1) {
    return 1;
  }
  if (((tex_desc_rec *)texdesc_free_cursor)->format == 0) {
    texdesc_free_cursor = 0;
    texdesc_free_slot = -1;
    return 1;
  }
  if (texidx_map[idx].use_count != 0) {
    n = texidx_map[idx].slot_no;
    tex_slot_current = tex_slot_table + n;
    if ((tex_slot_current->in_use != 0) && (tex_slot_current->status == 0)) {
      surface_memory_return_by_kind(&tex_slot_current->surface);
    }
    texidx_bit_clear(n);
    texidx_map[idx].use_count = 0;
    texidx_map[idx].slot_no = 0xffffffff;
    tex_slot_current->status = -1;
    tex_slot_current->in_use = 0;
    tex_slot_current->slot_id = -1;
  }
  texdesc_free_slot = texdesc_free_slot + 1;
  texdesc_free_cursor = texdesc_free_cursor + 0x10;
  return 0;
}
