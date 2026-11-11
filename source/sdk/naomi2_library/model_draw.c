/*
 * sdk/naomi2_library/model_draw.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern blend_pass_bits_table blend_pass_bits; /* 0x0c325864 */
extern elan_matrix_param_rec elan_matrix_params; /* 0x0c3dc188 */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */
extern normal_block_scale_vec normal_block_scale; /* 0x0c5c9d84 */

void model_draw(compiled_model_rec *model)
{
  model_part_span_rec *part;
  int i;
  int n;
  list_block_ref span;

  n = 1;
  if (model != 0) {
    i = 0;
    part = model->parts;
    for (; i < 5; i += 2, part++) {
      if (part->offset != 0) {
        span.addr = part->offset + (int)part;
        span.length = part->size;
        model_list_link_queue(&span,i,n++);
      }
    }
  }
  return;
}

void model_volumes_draw(compiled_model_rec *model)
{
  uint n;
  list_block_ref span;

  n = 1;
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,1,n++,0x08000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,n++,0x08000000);
    }
  }
  return;
}

void model_volumes_draw_cull_cw(compiled_model_rec *model)
{
  int n;
  list_block_ref span;
  n = 1;
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,1,n++,0x18000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,n++,0x18000000);
    }
  }
}

void model_draw_opaque_part(compiled_model_rec *model)
{
  list_block_ref span;
  
  if ((model != 0) && (model->parts[0].offset != 0)) {
    span.addr = model->parts[0].offset + (int)&model->parts[0];
    span.length = model->parts[0].size;
    model_list_link_queue(&span,0,1);
  }
  return;
}

void model_draw_translucent_part(compiled_model_rec *model)
{
  list_block_ref span;
  
  if ((model != 0) && (model->parts[1].offset != 0)) {
    span.addr = model->parts[1].offset + (int)&model->parts[1];
    span.length = model->parts[1].size;
    model_list_link_queue(&span,2,1);
  }
  return;
}

void model_draw_all_as_opaque(compiled_model_rec *model)
{
  int n;
  list_block_ref span;
  n = 1;
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_list_link_queue(&span,0,n++);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_list_link_queue(&span,0,n++);
    }
  }
  return;
}

void model_draw_all_as_translucent(compiled_model_rec *model)
{
  uint saved;
  uint n;
  list_block_ref span;
  uint t;
  n = 1;
  if (model != 0) {
    saved = poly_header_or_words.tsp_or;
    t = saved & 0x3ffff3f;
    poly_header_or_words.tsp_or = (blend_pair_bits ^ 0xdc) << 0x18 | t | 0x100080;
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_list_link_queue(&span,2,n++);
    }
    poly_header_or_words.tsp_or = saved;
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_list_link_queue(&span,2,n++);
    }
  }
}

void model_volumes_draw_opaque_mod(compiled_model_rec *model)
{
  uint n;
  uint *n_ref;
  list_block_ref span;
  n = 1;
  if (model != 0)
  {
    if (model->parts[0].offset != 0)
    {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      n = 2;
      model_volume_link_queue(&span, 1, 1, 0x08000000);
    }
    if (model->parts[1].offset != 0)
    {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span, 1, *(n_ref = &n), 0x08000000);
    }
  }
  return;
}

void model_volumes_draw_opaque_mod_cull_cw(compiled_model_rec *model)
{
  uint n;
  uint *n_ref;
  list_block_ref span;
  n = 1;
  if (model != 0)
  {
    if (model->parts[0].offset != 0)
    {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      n = 2;
      model_volume_link_queue(&span, 1, 1, 0x18000000);
    }
    if (model->parts[1].offset != 0)
    {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span, 1, *(n_ref = &n), 0x18000000);
    }
  }
  return;
}

void model_volumes_draw_trans_mod(compiled_model_rec *model)
{
  uint n;
  list_block_ref span;

  n = 1;
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,3,n++,0x08000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,n++,0x08000000);
    }
  }
  return;
}

void model_volumes_draw_trans_mod_cull_cw(compiled_model_rec *model)
{
  uint n;
  list_block_ref span;

  n = 1;
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,3,n++,0x18000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,n++,0x18000000);
    }
  }
  return;
}

void model_draw_blend_passes(compiled_model_rec *model, uint first_mode, uint second_mode)
{
  typedef struct { uint pass1_bits; uint pass2_bits; uint pass3_bits; } blend_ent;
  list_block_ref span;
  int n;
  uint saved;
  uint blend;
  int three_pass;
  int list;
  
  n = 1;
  if (model == 0) {
    return;
  }
  saved = poly_header_or_words.tsp_or;
  if ((model->parts[0].offset != 0) & (first_mode < 2)) {
    span.addr = (int)((char *)&model->parts[0] + model->parts[0].offset);
    span.length = model->parts[0].size;
    blend = *(uint *)(span.addr + 0x48) & 0xfc000000;
    poly_header_or_words.tsp_or = saved ^ (blend ^ 0x20000000 | ((blend_ent *)&blend_pass_bits)[first_mode].pass1_bits);
    three_pass = first_mode == 1;
    list = (three_pass > 0) * 2;
    model_list_link_queue(&span,list,n++);
    poly_header_or_words.tsp_or = (blend ^ 0x24000000 | ((blend_ent *)&blend_pass_bits)[first_mode].pass2_bits) ^ saved;
    model_list_link_queue(&span,2,n++);
    if (three_pass > 0) {
      poly_header_or_words.tsp_or = ((blend_ent *)&blend_pass_bits)[1].pass3_bits ^ saved;
      model_list_link_queue(&span,2,n++);
    }
  }
  if ((model->parts[1].offset != 0) & (second_mode < 2)) {
    span.addr = (int)((char *)&model->parts[1] + model->parts[1].offset);
    span.length = model->parts[1].size;
    blend = *(uint *)(span.addr + 0x48) & 0xfc000000;
    poly_header_or_words.tsp_or = saved ^ (blend ^ 0x20000000 | ((blend_ent *)&blend_pass_bits)[second_mode].pass1_bits);
    three_pass = second_mode == 1;
    list = (three_pass > 0) * 2;
    model_list_link_queue(&span,list,n++);
    poly_header_or_words.tsp_or = (blend ^ 0x24000000 | ((blend_ent *)&blend_pass_bits)[second_mode].pass2_bits) ^ saved;
    model_list_link_queue(&span,2,n++);
    if (three_pass > 0) {
      poly_header_or_words.tsp_or = ((blend_ent *)&blend_pass_bits)[1].pass3_bits ^ saved;
      model_list_link_queue(&span,2,n++);
    }
  }
  poly_header_or_words.tsp_or = saved;
}

void matrix_cmds_queue_on_lists(uint list_mask)
{
  int list;
  
  if ((list_mask & 0x1f) != 0) {
    list = 0;
    do {
      if ((list_mask & 1) != 0) {
        matrix_cmds_build();
        elan_list_block_queue(list,&matrix_cmds_block);
      }
      list_mask = (int)list_mask >> 1;
      list = list + 1;
    } while (list < 5);
  }
  return;
}

void matrix_cmds_queue_normal_from_pushed(uint list_mask)
{
  int list;
  
  if ((list_mask & 0x1f) != 0) {
    list = 0;
    do {
      if ((list_mask & 1) != 0) {
        matrix_cmds_build_normal_from_pushed();
        elan_list_block_queue(list,&matrix_cmds_block);
      }
      list_mask = (int)list_mask >> 1;
      list = list + 1;
    } while (list < 5);
  }
  return;
}

void matrix_cmds_queue_vertex_from_pushed(uint list_mask)
{
  int list;
  
  if ((list_mask & 0x1f) != 0) {
    list = 0;
    do {
      if ((list_mask & 1) != 0) {
        matrix_cmds_build_vertex_from_pushed();
        elan_list_block_queue(list,&matrix_cmds_block);
      }
      list_mask = (int)list_mask >> 1;
      list = list + 1;
    } while (list < 5);
  }
  return;
}

void model_draw_no_matrix(compiled_model_rec *model)
{
  list_block_ref span;
  
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_list_link_queue(&span,0,0);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_list_link_queue(&span,2,0);
    }
    if (model->parts[2].offset != 0) {
      span.addr = model->parts[2].offset + (int)&model->parts[2];
      span.length = model->parts[2].size;
      model_list_link_queue(&span,4,0);
    }
  }
  return;
}

void model_volumes_draw_no_matrix(compiled_model_rec *model)
{
  list_block_ref span;

  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,1,0,0x08000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,0,0x08000000);
    }
  }
}

void model_volumes_draw_cull_cw_no_matrix(compiled_model_rec *model)
{
  list_block_ref span;
  
  if (model != 0) {
    if (model->parts[0].offset != 0) {
      span.addr = model->parts[0].offset + (int)&model->parts[0];
      span.length = model->parts[0].size;
      model_volume_link_queue(&span,1,0,0x18000000);
    }
    if (model->parts[1].offset != 0) {
      span.addr = model->parts[1].offset + (int)&model->parts[1];
      span.length = model->parts[1].size;
      model_volume_link_queue(&span,3,0,0x18000000);
    }
  }
}

void dummy_tri_link_on_lists(uint clip_bits)
{
  int saved;
  int bits;
  int i;
  char unused[32];
  saved = poly_header_or_words.control_or;
  poly_header_or_words.control_or = clip_bits | poly_header_or_words.control_or & 0xfffcffff;
  bits = elan_list_enable_mask;
  for (i = 0; i < 5; i++) {
    if (bits & 1) {
      model_list_link_queue(dummy_tri_never, i, 0);
    }
    bits >>= 1;
  }
  poly_header_or_words.control_or = saved;
}

void matrix_cmds_build(void)
{
  char span[64];
  uint v;
  uint hi;
  uint *p;

  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  matrix_cmds_block.addr = (uint)p;
  matrix_cmds_block.length = 0;
  p = elan_matrix_cmds_write(p,&elan_matrix_params);
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  matrix_cmds_block.length = (int)p - matrix_cmds_block.addr;
  matrix_cmds_block.addr = matrix_cmds_block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
}

void matrix_cmds_build_normal_from_pushed(void)
{
  float m[16];
  uint v;
  uint hi;
  uint *p;
  uint *q;
  uint *r;

  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  matrix_cmds_block.addr = (uint)p;
  matrix_cmds_block.length = 0;
  mtx_peek_pushed(m);
  p = p + 8;
  q = p;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0x7f;
  *--q = 0xf;
  *--q = 0x08000400;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  p = p + 16;
  q = p - 1;
  *(float *)--q = -normal_block_scale.z;
  *(float *)--q = normal_block_scale.y;
  *(float *)--q = -normal_block_scale.x;
  *(float *)--q = elan_matrix_params.env_v_offset;
  *(float *)--q = m[10];
  *(float *)--q = m[6];
  *(float *)--q = m[2];
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  *(float *)--q = m[9];
  *(float *)--q = m[5];
  *(float *)--q = m[1];
  *(float *)--q = m[8];
  *(float *)--q = m[4];
  *(float *)--q = m[0];
  *(float *)--q = elan_matrix_params.env_u_offset;
  *--q = 0x08000200;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  mtx_store_wordwise(m);
  p = p + 16;
  r = p;
  *(float *)--r = -elan_matrix_params.neg_inv_near;
  *(float *)--r = 100000000.0;
  *(float *)--r = m[14] * -1.0;
  *(float *)--r = m[13];
  *(float *)--r = m[12] * -1.0;
  *(float *)--r = m[10] * -1.0;
  *(float *)--r = m[9];
  *(float *)--r = m[8] * -1.0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (r));
  *(float *)--r = m[6] * -1.0;
  *(float *)--r = m[5];
  *(float *)--r = m[4] * -1.0;
  *(float *)--r = m[2] * -1.0;
  *(float *)--r = m[1];
  *(float *)--r = m[0] * -1.0;
  *(float *)--r = elan_matrix_params.near;
  *--r = 0x08000100;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (r));
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  matrix_cmds_block.length = (int)p - matrix_cmds_block.addr;
  matrix_cmds_block.addr = matrix_cmds_block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
}

void matrix_cmds_build_vertex_from_pushed(void)
{
  float m[16];
  uint v;
  uint hi;
  uint *p;
  uint *q;
  float *f;

  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  matrix_cmds_block.addr = (uint)p;
  matrix_cmds_block.length = 0;
  mtx_store_wordwise(m);
  p = p + 8;
  q = p;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0;
  *--q = 0x7f;
  *--q = 0xf;
  *--q = 0x08000400;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  p = p + 16;
  q = p - 1;
  *(float *)--q = -normal_block_scale.z;
  *(float *)--q = normal_block_scale.y;
  *(float *)--q = -normal_block_scale.x;
  *(float *)--q = elan_matrix_params.env_v_offset;
  *(float *)--q = m[10];
  *(float *)--q = m[6];
  *(float *)--q = m[2];
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  *(float *)--q = m[9];
  *(float *)--q = m[5];
  *(float *)--q = m[1];
  *(float *)--q = m[8];
  *(float *)--q = m[4];
  *(float *)--q = m[0];
  *(float *)--q = elan_matrix_params.env_u_offset;
  *--q = 0x08000200;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  mtx_peek_pushed(m);
  p = p + 16;
  f = (float *)p;
  *--f = -elan_matrix_params.neg_inv_near;
  *--f = 100000000.0;
  *--f = m[14] * -1.0;
  *--f = m[13];
  *--f = m[12] * -1.0;
  *--f = m[10] * -1.0;
  *--f = m[9];
  *--f = m[8] * -1.0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (f));
  *--f = m[6] * -1.0;
  *--f = m[5];
  *--f = m[4] * -1.0;
  *--f = m[2] * -1.0;
  *--f = m[1];
  *--f = m[0] * -1.0;
  *--f = elan_matrix_params.near;
  *(uint *)--f = 0x08000100;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (f));
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  matrix_cmds_block.length = (int)p - matrix_cmds_block.addr;
  matrix_cmds_block.addr = matrix_cmds_block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
}

void model_list_link_queue(list_block_ref *dlist, int list, int with_matrix)
{
  list_block_ref block;
  uint v;
  uint hi;
  uint *p;
  uint mode;
  uint w0;
  uint w4;

  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  if (with_matrix != 0) {
    p = elan_matrix_cmds_write(p,&elan_matrix_params);
  }
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
  p[4] = dlist->addr & 0x1fffff8 | 0x80000000;
  p[5] = 1;
  p[6] = dlist->length;
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

void model_volume_link_queue(list_block_ref *dlist, int list, int with_matrix, uint cull)
{
  list_block_ref block;
  uint v;
  uint hi;
  uint *p;
  uint *poly;
  list_block_ref *closer;
  uint w0;
  uint w4;
  uint w1;

  if ((*(uint *)dlist->addr >> 8 & 0xf) == 5) {
    poly = (uint *)dlist->addr + 0x10;
  }
  else {
    poly = (uint *)dlist->addr;
  }
  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  if (with_matrix != 0) {
    p = elan_matrix_cmds_write(p,&elan_matrix_params);
  }
  last_link_cmd = (uint *)((uint)p & 0x3ffffff | hi);
  w0 = poly_header_or_words.control_or & 0xffffff3f;
  w1 = cull | poly[1] & 0xe0000000;
  w4 = poly_header_or_words.tsp_or;
  p[0] = w0 | 0x08000800;
  p[1] = w1;
  p[2] = w4;
  p[3] = 0;
  p[4] = dlist->addr & 0x1fffff8 | 0x80000000;
  p[5] = 1;
  p[6] = dlist->length;
  p[7] = 0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  if (dlist != dummy_tri_never) {
    if (modvol_instruction == 0x40000000) {
      closer = dummy_tri_outside;
    }
    else {
      closer = dummy_tri_inside;
    }
    w0 = (poly_header_or_words.control_or ^ *poly) & 0x30000;
    p[0] = w0 | 0x08000800;
    p[1] = cull;
    p[2] = 0;
    p[3] = 0;
    p[4] = closer->addr & 0x1fffff8 | 0x80000000;
    p[5] = 1;
    p[6] = closer->length;
    p[7] = 0;
    list_end_block_count = list_end_block_count + 1 & 3;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
    p = p + 8;
  }
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

void env_uv_offset_set(float u, float v)
{
  elan_matrix_params.env_u_offset = u;
  elan_matrix_params.env_v_offset = v;
  return;
}

void env_uv_offset_get(float *u, float *v)
{
  *u = elan_matrix_params.env_u_offset;
  *v = elan_matrix_params.env_v_offset;
  return;
}

void normal_block_scale_set_inverse(float x, float y, float z)
{
  normal_block_scale.x = 1.0f / x;
  normal_block_scale.y = 1.0f / y;
  normal_block_scale.z = 1.0f / z;
  return;
}

void normal_block_scale_set(float x, float y, float z)
{
  normal_block_scale.x = x;
  normal_block_scale.y = y;
  normal_block_scale.z = z;
  return;
}

void normal_block_scale_reset(void)
{
  normal_block_scale.x = 1.0;
  normal_block_scale.y = 1.0;
  normal_block_scale.z = 1.0;
  return;
}
