/*
 * gfxlib/model_merge.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */

uint model_copy_to_elan_ram(compiled_model_rec *model)
{
  uint addr;

  addr = elan_ram_alloc_top(model->size + 0x1fU & 0xffffffe0);
  model_header_sq_copy((uint *)model,addr);
  model_body_sq_copy((uint *)model,addr);
  return addr;
}

int model_copy_to_addr(int dst, compiled_model_rec *model)
{
  int n = model->size + 0x1fU & 0xffffffe0;
  model_header_sq_copy((uint *)model,dst);
  model_body_sq_copy((uint *)model,dst);
  return dst + n;
}

int models_merge_xformed_into(int dst, int count, model_xform_pair *pairs)
{
  int offset;
  model_part_span_rec *src;
  model_xform_pair *pair;
  int j;
  model_part_span_rec *slot;
  int out;
  uint part[2];
  int i;

  model_header_sq_copy((uint *)pairs[0].model,dst);
  out = dst + 0x60;
  slot = (model_part_span_rec *)(dst + 0x40);
  for (i = 0; i < 3; i++) {
    slot->offset = out - (int)slot;
    slot->size = 0;
    for (j = 0; j < count; j++) {
      pair = &pairs[j];
      src = &pair->model->parts[i];
      offset = src->offset;
      if (offset != 0) {
        part[0] = offset + (int)src;
        part[1] = src->size;
        words_copy_aligned8((uint *)out,part);
        mtx_set_current_wordwise(pair->mtx);
        part[0] = out;
        dlist_xform_in_place(part);
        out = out + part[1];
        slot->size = slot->size + part[1];
      }
    }
    if (slot->size == 0) {
      slot->offset = 0;
    }
    slot = slot + 1;
  }
  return out;
}

uint models_merge_to_elan_ram(int count, model_xform_pair *pairs)
{
  uint addr;
  int i;
  int total;

  total = 0;
  i = 0;
  for (; i < count; total = total + pairs[i].model->size, i = i + 1) {

  }
  addr = elan_ram_alloc_top(total + 0x1fU & 0xffffffe0);
  models_merge_xformed_into(addr,count,pairs);
  return addr;
}

void model_parts_xform_in_place(compiled_model_rec *model)
{
  model_part_span_rec *slot;
  int i;
  uint part[2];

  if (model != 0) {
    i = 0;
    slot = model->parts;
    for (; i < 5; i += 2, slot++) {
      if (slot->offset != 0) {
        part[0] = slot->offset + (int)slot;
        part[1] = slot->size;
        dlist_xform_in_place(part);
      }
    }
  }
}

void model_parts_hdr_and_mask(compiled_model_rec *model, uint mask)
{
  model_part_span_rec *slot;
  int i;
  uint part[2];

  if (model != 0) {
    for (i = 0, slot = model->parts; i < 5; i += 2, slot++) {
      if (slot->offset != 0) {
        part[0] = slot->offset + (int)slot;
        part[1] = slot->size;
        dlist_hdr_and_mask(part,mask);
      }
    }
  }
}

void dlist_call_block_queue(list_block_ref *part, int list, int unused)
{
  list_block_ref block;
  uint top;
  uint hi;
  uint *p;
  uint mode;
  uint w0;
  uint w4;

  top = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = top >> 0x18;
  *(volatile uint *)0xff00003c = top >> 0x18;
  p = (uint *)(top & 0x3ffffff | 0xe0000000);
  hi = top & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  last_link_cmd = (uint *)((uint)p & 0x3ffffff | hi);
  w0 = poly_header_or_words.control_or;
  if ((int)w0 >= 0) {
    mode = 0x18000000;
  }
  else {
    mode = 0x10000000;
  }
  w4 = poly_header_or_words.tsp_or;
  p[0] = w0 | 0x08000800;
  p[1] = mode;
  p[2] = w4;
  p[3] = 0;
  p[4] = part->addr & 0x1fffff8 | 0x80000000;
  p[5] = 1;
  p[6] = part->length;
  p[7] = 0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  block.length = (int)p - block.addr;
  block.addr = block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
  elan_list_block_queue(list,&block);
}

void dlist_call_block_queue_tex(list_block_ref *part, int list, int unused, uint hdr_bits, uint tex_word)
{
  list_block_ref block;
  uint top;
  uint hi;
  uint *q;
  uint w0;
  uint mode;
  uint w;
  top = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = top >> 24;
  *(volatile uint *)0xff00003c = top >> 24;
  q = (uint *)(top & 0x3ffffff | 0xe0000000);
  hi = top & 0xfc000000;
  block.addr = (uint)q;
  block.length = 0;
  last_link_cmd = (uint *)((uint)q & 0x3ffffff | hi);
  w0 = poly_header_or_words.control_or;
  if ((int)w0 >= 0) mode = 0x18000000; else mode = 0x10000000;
  w = poly_header_or_words.tsp_or;
  w |= hdr_bits;
  q[0] = w0 | 0x08000800;
  q[1] = mode;
  q[2] = w;
  q[3] = tex_word;
  q[4] = part->addr & 0x1fffff8 | 0x80000000;
  q[5] = 1;
  q[6] = part->length;
  q[7] = 0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  q += 8;
  q[0] = 0x08000000;
  q[2] = 0x08000000;
  q[4] = 0x08000000;
  q[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  q += 8;
  block.length = (char *)q - (char *)block.addr;
  block.addr = block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)q & 0x3ffffff | hi);
  elan_list_block_queue(list,&block);
}

void model_parts_queue_three_lists(compiled_model_rec *model)
{
  list_block_ref part;

  if (model != 0) {
    if (model->parts[0].offset != 0) {
      part.addr = model->parts[0].offset + (int)&model->parts[0];
      part.length = model->parts[0].size;
      dlist_call_block_queue(&part,0,0);
    }
    if (model->parts[1].offset != 0) {
      part.addr = model->parts[1].offset + (int)&model->parts[1];
      part.length = model->parts[1].size;
      dlist_call_block_queue(&part,2,0);
    }
    if (model->parts[2].offset != 0) {
      part.addr = model->parts[2].offset + (int)&model->parts[2];
      part.length = model->parts[2].size;
      dlist_call_block_queue(&part,4,0);
    }
  }
}

void model_parts_queue_translucent(compiled_model_rec *model)
{
  list_block_ref part;

  if (model != 0) {
    if (model->parts[0].offset != 0) {
      part.addr = model->parts[0].offset + (int)&model->parts[0];
      part.length = model->parts[0].size;
      dlist_call_block_queue(&part,2,0);
    }
    if (model->parts[1].offset != 0) {
      part.addr = model->parts[1].offset + (int)&model->parts[1];
      part.length = model->parts[1].size;
      dlist_call_block_queue(&part,2,0);
    }
  }
}

void model_parts_queue_textured(compiled_model_rec *model, int tex_index, uint hdr_bits)
{
  struct { uchar head[8]; uint control_bits; uint tex_word; uchar rest[16]; } hdr;
  list_block_ref part;

  if (model != 0) {
    {
      int slot_no = texidx_map[tex_index].slot_no;
      tex_slot_bits_into_poly_hdr((uint *)&hdr,slot_no);
    }
    hdr_bits = hdr_bits | hdr.control_bits;
    if (model->parts[0].offset != 0) {
      part.addr = model->parts[0].offset + (int)&model->parts[0];
      part.length = model->parts[0].size;
      dlist_call_block_queue_tex(&part,0,0,hdr_bits,hdr.tex_word);
    }
    if (model->parts[1].offset != 0) {
      part.addr = model->parts[1].offset + (int)&model->parts[1];
      part.length = model->parts[1].size;
      dlist_call_block_queue_tex(&part,2,0,hdr_bits,hdr.tex_word);
    }
    if (model->parts[2].offset != 0) {
      part.addr = model->parts[2].offset + (int)&model->parts[2];
      part.length = model->parts[2].size;
      dlist_call_block_queue_tex(&part,4,0,hdr_bits,hdr.tex_word);
    }
  }
  return;
}
