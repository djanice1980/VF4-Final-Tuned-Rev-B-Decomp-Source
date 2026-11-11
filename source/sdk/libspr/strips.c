/*
 * sdk/libspr/strips.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_pal_group_unref
#define NOPROTO_pal_mark_blocks_used
#define NOPROTO_palette_shadow_write
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int strip_vertex_formats[36]; /* 0x0c2118d8 */
extern uint strip_ctx_records[40]; /* 0x0c325910 */
extern uint strip_error; /* 0x0c3e0364 */
extern int strip_pal_group; /* 0x0c3e0368 */
extern uint strip_matrix_mode; /* 0x0c3e036c */
extern vertex_buffer_desc_rec vertex_buffer_desc; /* 0x0c418458 */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void strip_state_clear(void)
{
  strip_error = 0;
  strip_matrix_mode = 0;
}

void strip_matrix_mode_set(int mode)
{
  strip_matrix_mode = mode;
  return;
}

int strip_texidx_ref(int n, int tex_header)
{
  texidx_map_entry *ent;
  tex_slot_rec *tex_slot;

  {
    int b = (int)texidx_map;
    ent = (texidx_map_entry *)(n * 8 + b);
  }
  if (ent->use_count == 0) {
    ent->slot_no = texidx_bit_take();
  }
  texidx_map[n].use_count++;
  n = texidx_map[n].slot_no;
  tex_slot = &tex_slot_table[n];
  if (tex_slot->in_use == 0) {
    if (tex_slot_create_and_load((short *)tex_header,n,*(int *)(tex_header + 8)) != 0) {
      strip_error = 1;
      return 1;
    }
    ((int (*)(int))tex_slot_words_or_fallback)(n);
    tex_slot->slot_id = n;
  }
  tex_slot->in_use++;
  return 0;
}

void strip_texidx_unref(int index)
{
  int slot;
  tex_slot_rec *tex_slot;

  if (texidx_map[index].use_count != 0) {
    texidx_map[index].use_count--;
    if (texidx_map[index].use_count == 0) {
      slot = texidx_map[index].slot_no;
      tex_slot = &tex_slot_table[slot];
      if ((tex_slot->status == 0) && (surface_memory_return_by_kind(&tex_slot->surface) != 0)) {
        strip_error = 2;
        for (;;) ;
      }
      tex_slot->in_use = 0;
      tex_slot->slot_id = 0xffffffff;
      ((int (*)(uint))texidx_bit_clear)(slot);
      texidx_map[index].slot_no = -1;
    }
  }
  return;
}

int strip_pal_group_ref(int group_no, int pal_header)
{
  int block;
  short *group;
  char *kind;
  int r;
  r = 0;
  if (pal_raw_mode == '\0') {
    graphics_work_rec *work = &graphics_work;
    r = 1;
    if (group_no < work->pal_group_count) {
      group = (short *)(group_no * 8 + work->pal_group_table);
      if (group[2] != 0) {
        group[2]++;
        r = 0;
      }
      else {
        kind = (char *)(pal_header + 2);
        block = pal_find_free_blocks((int)*(short *)kind,*(uint *)(pal_header + 8));
        if (block >= 0) {
        *(char *)(group + 1) = *kind;
        *(char *)((int)group + 3) = (char)(*(int *)(pal_header + 8) + 0xfU >> 4);
        group[2] = 1;
        group[3] = (short)group_no;
        *group = (short)block;
        pal_mark_blocks_used(block,*(uint *)(pal_header + 8));
        palette_shadow_write(block << 4,*(uint *)(pal_header + 4),*(uint *)(pal_header + 8));
        r = 0;
        } else return 1;
      }
    }
  }
  return r;
}

void strip_pal_group_unref(int group_no)
{
  if ((pal_raw_mode == '\0') && (group_no < graphics_work.pal_group_count)) {
    pal_group_unref(group_no);
  }
  return;
}

void strip_pal_group_set(int group)
{
  strip_pal_group = group;
  return;
}

void strip_send_plain_opaque(int count, uint *verts)
{
  strip_ctx_record_apply(strip_ctx_records);
  strip_send_untextured(count,0,verts);
  return;
}

void strip_send_plain_opaque_kind(int count, int format, uint *verts)
{
  strip_ctx_record_apply(&strip_ctx_records[5]);
  strip_send_untextured(count,format,verts);
  return;
}

void strip_send_plain_blended(int count, uint *verts)
{
  strip_ctx_record_apply(&strip_ctx_records[10]);
  strip_send_untextured(count,0,verts);
  return;
}

void strip_send_plain_blended_kind(int count, int format, uint *verts)
{
  strip_ctx_record_apply(&strip_ctx_records[15]);
  strip_send_untextured(count,format,verts);
  return;
}

void strip_send_textured_opaque(int count, uint *verts, int tex_index)
{
  int missing;
  
  missing = strip_ctx_record_apply_textured((int *)&strip_ctx_records[20],tex_index);
  if (missing == 0) {
    strip_send_uv_colour(count,verts);
  }
  return;
}

void strip_send_textured_blended(int count, uint *verts, int tex_index)
{
  int missing;
  
  missing = strip_ctx_record_apply_textured((int *)&strip_ctx_records[25],tex_index);
  if (missing == 0) {
    strip_send_uv_colour(count,verts);
  }
  return;
}

void strip_send_textured_offset_opaque(int count, uint *verts, int tex_index)
{
  int missing;
  
  missing = strip_ctx_record_apply_textured((int *)&strip_ctx_records[30],tex_index);
  if (missing == 0) {
    strip_send_uv_two_colours(count,verts);
  }
  return;
}

void strip_send_textured_offset_blended(int count, uint *verts, int tex_index)
{
  int missing;
  
  missing = strip_ctx_record_apply_textured((int *)&strip_ctx_records[35],tex_index);
  if (missing == 0) {
    strip_send_uv_two_colours(count,verts);
  }
  return;
}

void strip_ctx_record_apply(uint *rec)
{
  draw_ctx_ptr->change_mask = 0x600608;
  draw_ctx_ptr->list_type = *rec++;
  draw_ctx_ptr->color_format = *rec++;
  draw_ctx_ptr->tex_offset_bits = *rec++;
  draw_ctx_ptr->alpha_on = *rec++;
  draw_ctx_ptr->gouraud = *rec;
  context_hw_words_compute(draw_ctx_ptr);
  draw_ctx_ptr->ctrl_word = draw_ctx_ptr->ctrl_word & 0xfffcff3f;
  draw_ctx_ptr->ctrl_word = draw_ctx_ptr->ctrl_word | poly_header_or_words.control_or;
  draw_ctx_ptr->tsp_word = draw_ctx_ptr->tsp_word & 0x31ff0ff;
  draw_ctx_ptr->tsp_word =
       (blend_pair_bits ^ 0xfc) << 0x18 | poly_header_or_words.tsp_or |
       draw_ctx_ptr->tsp_word;
  context_header_select(draw_ctx_ptr);
  return;
}

int strip_ctx_record_apply_textured(int *rec, int n)
{
  short *group;
  if (texidx_map[n].use_count == 0) {
    return 1;
  }
  n = texidx_map[n].slot_no;
  draw_ctx_ptr->surface = &tex_slot_table[n].surface;
  draw_ctx_ptr->change_mask = 0x600608;
  draw_ctx_ptr->list_type = *rec;
  draw_ctx_ptr->color_format = rec[1];
  draw_ctx_ptr->tex_offset_bits = rec[2];
  draw_ctx_ptr->alpha_on = rec[3];
  draw_ctx_ptr->gouraud = rec[4];
  if (((int)tex_slot_table[n].tex_control_word >> 0x1b & 7U) - 5 < 2) {
    draw_ctx_ptr->change_mask = draw_ctx_ptr->change_mask | 0x80000;
    if (pal_raw_mode != 0) {
      draw_ctx_ptr->pal_group = strip_pal_group;
    }
    else {
      draw_ctx_ptr->pal_group = 0;
      if (strip_pal_group < graphics_work.pal_group_count) {
        group = (short *)(strip_pal_group * 8 + graphics_work.pal_group_table);
        if (group[2] != 0) draw_ctx_ptr->pal_group = (int)*group;
      }
    }
  }
  context_hw_words_compute(draw_ctx_ptr);
  draw_ctx_ptr->ctrl_word = draw_ctx_ptr->ctrl_word & 0xfffcff3f;
  draw_ctx_ptr->ctrl_word = draw_ctx_ptr->ctrl_word | poly_header_or_words.control_or;
  draw_ctx_ptr->tsp_word = draw_ctx_ptr->tsp_word & 0x31ff0ff;
  if (*rec == 0) {
    draw_ctx_ptr->tsp_word = draw_ctx_ptr->tsp_word | poly_header_or_words.tsp_or | 0x20000000;
  }
  else {
    draw_ctx_ptr->tsp_word = (blend_pair_bits ^ 0xfc) << 0x18 | poly_header_or_words.tsp_or | draw_ctx_ptr->tsp_word;
  }
  context_header_select(draw_ctx_ptr);
  return 0;
}

void strip_send_untextured(int count, int format, uint *verts)
{
  uint buf[8];
  int *fmt = (int *)((char *)&strip_vertex_formats + format * 8);
  int idx;
  uint size;
  uint hdr_addr;
  int i;
  uint addr;

  idx = *(int *)vertex_buffer_desc.list_and_size;
  hdr_addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[idx];
  size = ((uint *)vertex_buffer_desc.list_and_size)[1];
  *(volatile uint *)0xff000038 = hdr_addr >> 24;
  *(volatile uint *)0xff00003c = hdr_addr >> 24;
  hdr_addr = hdr_addr & 0x3ffffff | 0xe0000000;
  copy_by_pairs_or_words((uint *)hdr_addr,(uint *)vertex_buffer_desc.header,size);
  __asm__ volatile ("pref @%0" : : "r" (hdr_addr));
  if (size == 0x40) {
    __asm__ volatile ("pref @%0" : : "r" (hdr_addr + 32));
  }
  ((int **)vertex_buffer_desc.list_write_ptrs)[idx] += size >> 2;
  buf[0] = 0xe0000000;
  for (; count > 0; count--) {
    if (count == 1) {
      buf[0] = 0xf0000000;
    }
    if (strip_matrix_mode != 0) {
      mtx_xform_point((float *)verts, (float *)(buf + 1));
      *(float *)(buf + 3) = 1.0f / *(float *)(buf + 3);
    }
    else {
      *(float *)(buf + 1) = ((float *)verts)[0];
      *(float *)(buf + 2) = ((float *)verts)[1];
      *(float *)(buf + 3) = 1.0f / ((float *)verts)[2];
    }
    buf[6] = verts[fmt[1]];
    i = *(int *)vertex_buffer_desc.list_and_size;
    addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[i];
    *(volatile uint *)0xff000038 = addr >> 24;
    *(volatile uint *)0xff00003c = addr >> 24;
    addr = addr & 0x3ffffff | 0xe0000000;
    copy_by_pairs_or_words((uint *)addr,(uint *)buf,0x20);
    __asm__ volatile ("pref @%0" : : "r" (addr));
    ((uint *)vertex_buffer_desc.list_write_ptrs)[i] += 0x20;
    verts = verts + fmt[0];
  }
}

void strip_send_uv_colour(int count, uint *verts)
{
  typedef struct { uint cmd; float x, y, z, u, v; uint color; uint pad; } strip_vertex;
  strip_vertex vtx;
  int idx;
  uint hdr_addr;
  uint size;
  float *p = (float *)verts;

  idx = *(int *)vertex_buffer_desc.list_and_size;
  hdr_addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[idx];
  size = *(uint *)(vertex_buffer_desc.list_and_size + 4);
  *(volatile uint *)0xff000038 = hdr_addr >> 24;
  *(volatile uint *)0xff00003c = hdr_addr >> 24;
  hdr_addr = hdr_addr & 0x3ffffff | 0xe0000000;
  copy_by_pairs_or_words((uint *)hdr_addr, (uint *)vertex_buffer_desc.header, size);
  __asm__ __volatile__ ("pref @%0" : : "r" (hdr_addr));
  if (size == 64) {
    __asm__ __volatile__ ("pref @%0" : : "r" (hdr_addr + 32));
  }
  ((uint *)vertex_buffer_desc.list_write_ptrs)[idx] += (size >> 2) << 2;
  vtx.cmd = 0xe0000000;
  for (; count > 0; count--) {
    int i;
    uint addr;
    if (count == 1) vtx.cmd = 0xf0000000;
    if (strip_matrix_mode != 0) {
      mtx_xform_point(p, &vtx.x);
      vtx.z = 1.0 / vtx.z;
    } else {
      vtx.x = p[0];
      vtx.y = p[1];
      vtx.z = 1.0 / p[2];
    }
    vtx.u = p[3];
    vtx.v = p[4];
    vtx.color = *(uint *)(p + 5);
    i = *(int *)vertex_buffer_desc.list_and_size;
    addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[i];
    *(volatile uint *)0xff000038 = addr >> 24;
    *(volatile uint *)0xff00003c = addr >> 24;
    addr = addr & 0x3ffffff | 0xe0000000;
    copy_by_pairs_or_words((uint *)addr, (uint *)&vtx, 32);
    __asm__ __volatile__ ("pref @%0" : : "r" (addr));
    ((uint *)vertex_buffer_desc.list_write_ptrs)[i] += 32;
    p += 7;
  }
}

void strip_send_uv_two_colours(int count, uint *verts)
{
  typedef struct { uint cmd; float x, y, z, u, v; uint base_color, offset_color; } strip_vertex;
  typedef struct { float x, y, z, u, v; uint base_color, offset_color; } input_vertex;
  strip_vertex vtx;
  int idx;
  uint hdr_addr;
  uint size;

  idx = *(int *)vertex_buffer_desc.list_and_size;
  hdr_addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[idx];
  size = *(uint *)(vertex_buffer_desc.list_and_size + 4);
  *(volatile uint *)0xff000038 = hdr_addr >> 24;
  *(volatile uint *)0xff00003c = hdr_addr >> 24;
  hdr_addr = (hdr_addr & 0x3ffffff) | 0xe0000000;
  copy_by_pairs_or_words((uint *)hdr_addr, (uint *)vertex_buffer_desc.header, size);
  __asm__ volatile ("pref @%0" : : "r" (hdr_addr));
  if (size == 64) {
    __asm__ volatile ("pref @%0" : : "r" (hdr_addr + 32));
  }
  ((uint *)vertex_buffer_desc.list_write_ptrs)[idx] += (size >> 2) << 2;
  vtx.cmd = 0xe0000000;
  for (; count > 0; count--) {
    int i;
    uint addr;
    if (count == 1) {
      vtx.cmd = 0xf0000000;
    }
    if (strip_matrix_mode != 0) {
      mtx_xform_point((float *)verts, &vtx.x);
      vtx.z = 1.0 / vtx.z;
    } else {
      vtx.x = ((input_vertex *)verts)->x;
      vtx.y = ((input_vertex *)verts)->y;
      vtx.z = 1.0 / ((input_vertex *)verts)->z;
    }
    vtx.u = ((input_vertex *)verts)->u;
    vtx.v = ((input_vertex *)verts)->v;
    vtx.base_color = ((input_vertex *)verts)->base_color;
    vtx.offset_color = ((input_vertex *)verts)->offset_color;
    i = *(int *)vertex_buffer_desc.list_and_size;
    addr = ((uint *)vertex_buffer_desc.list_write_ptrs)[i];
    *(volatile uint *)0xff000038 = addr >> 24;
    *(volatile uint *)0xff00003c = addr >> 24;
    addr = (addr & 0x3ffffff) | 0xe0000000;
    copy_by_pairs_or_words((uint *)addr, (uint *)&vtx, 32);
    __asm__ volatile ("pref @%0" : : "r" (addr));
    ((uint *)vertex_buffer_desc.list_write_ptrs)[i] += 32;
    verts += 7;
  }
}
