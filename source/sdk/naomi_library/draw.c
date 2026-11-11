/*
 * sdk/naomi_library/draw.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern vertex_buffer_desc_rec vertex_buffer_desc; /* 0x0c418458 */

void polystrip_plain_opaque(int count, uint *verts)
{
  struct { uint cmd; float x; float y; float z; int pad[2]; int color; int pad2; } vtx;
  int i;
  
  draw_context_preset_apply(2);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  for (i = 0; i < count - 1; i++, verts += 4) {
    vtx.cmd = 0xe0000000;
    vtx.x = ((float *)verts)[0];
    vtx.y = ((float *)verts)[1];
    vtx.z = ((float *)verts)[2];
    vtx.color = verts[3];
    list_block_sq_write(&vertex_buffer_desc,&vtx,0,0x20);
  }
  vtx.cmd = 0xf0000000;
  vtx.x = ((float *)verts)[0];
  vtx.y = ((float *)verts)[1];
  vtx.z = ((float *)verts)[2];
  vtx.color = verts[3];
  list_block_sq_write(&vertex_buffer_desc,&vtx,0,0x20);
}

void polystrip_plain_blended(int count, uint *verts)
{
  typedef struct { uint cmd; float x; float y; float z; int p0[2]; int color; int p1; } strip_vertex;
  strip_vertex vtx;
  int i;
  float *p = (float *)verts;

  draw_context_preset_apply(7);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  for (i = 0; i < count - 1; i++) {
    vtx.cmd = 0xe0000000;
    vtx.x = p[0];
    vtx.y = p[1];
    vtx.z = p[2];
    vtx.color = ((int *)p)[3];
    list_block_sq_write(&vertex_buffer_desc,&vtx,1,0x20);
    p += 4;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = p[0];
  vtx.y = p[1];
  vtx.z = p[2];
  vtx.color = ((int *)p)[3];
  list_block_sq_write(&vertex_buffer_desc,&vtx,1,0x20);
}

void polystrip_tex_opaque(int count, uint *verts, int slot_no)
{
  int n;
  struct { uint cmd; float x; float y; float z; float u; float v; int base_color; int offset_color; } vtx;

  draw_ctx_ptr->surface = &tex_slot_table[slot_no].surface;
  draw_context_preset_apply(0x12);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  n = count - 1;
  vtx.offset_color = 0;
  while (n != 0) {
    vtx.cmd = 0xe0000000;
    vtx.x = *(float *)verts;
    vtx.y = *(float *)(verts + 1);
    vtx.z = *(float *)(verts + 2);
    vtx.u = *(float *)(verts + 3);
    vtx.v = *(float *)(verts + 4);
    vtx.base_color = verts[5];
    list_block_sq_write(&vertex_buffer_desc, &vtx, 3, 0x20);
    verts = verts + 7;
    n--;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = *(float *)verts;
  vtx.y = *(float *)(verts + 1);
  vtx.z = *(float *)(verts + 2);
  vtx.u = *(float *)(verts + 3);
  vtx.v = *(float *)(verts + 4);
  vtx.base_color = verts[5];
  list_block_sq_write(&vertex_buffer_desc, &vtx, 3, 0x20);
}

void polystrip_tex_blended(int count, uint *verts, int slot_no)
{
  int n;
  struct { uint cmd; float x; float y; float z; float u; float v; int base_color; int offset_color; } vtx;

  draw_ctx_ptr->surface = &tex_slot_table[slot_no].surface;
  draw_context_preset_apply(0x17);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  n = count - 1;
  vtx.offset_color = 0;
  while (n != 0) {
    vtx.cmd = 0xe0000000;
    vtx.x = *(float *)verts;
    vtx.y = *(float *)(verts + 1);
    vtx.z = *(float *)(verts + 2);
    vtx.u = *(float *)(verts + 3);
    vtx.v = *(float *)(verts + 4);
    vtx.base_color = verts[5];
    list_block_sq_write(&vertex_buffer_desc, &vtx, 3, 0x20);
    verts = verts + 7;
    n--;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = *(float *)verts;
  vtx.y = *(float *)(verts + 1);
  vtx.z = *(float *)(verts + 2);
  vtx.u = *(float *)(verts + 3);
  vtx.v = *(float *)(verts + 4);
  vtx.base_color = verts[5];
  list_block_sq_write(&vertex_buffer_desc, &vtx, 3, 0x20);
}

void polystrip_tex_offset_opaque(int count, uint *verts, int slot_no)
{
  typedef struct { float x; float y; float z; float u; float v; int base_color; int offset_color; } src_vertex;
  struct { int cmd; float x; float y; float z; float u; float v; int base_color; int offset_color; } vtx;
  int n;

  draw_ctx_ptr->surface = &tex_slot_table[slot_no].surface;
  draw_context_preset_apply(0x12);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  n = count - 1;
  vtx.offset_color = 0;
  while (n != 0) {
    vtx.cmd = 0xe0000000;
    vtx.x = ((src_vertex *)verts)->x;
    vtx.y = ((src_vertex *)verts)->y;
    vtx.z = ((src_vertex *)verts)->z;
    vtx.u = ((src_vertex *)verts)->u;
    vtx.v = ((src_vertex *)verts)->v;
    vtx.base_color = ((src_vertex *)verts)->base_color;
    vtx.offset_color = ((src_vertex *)verts)->offset_color;
    list_block_sq_write(&vertex_buffer_desc, (uint *)&vtx, 3, 0x20);
    verts = verts + 7;
    n--;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = ((src_vertex *)verts)->x;
  vtx.y = ((src_vertex *)verts)->y;
  vtx.z = ((src_vertex *)verts)->z;
  vtx.u = ((src_vertex *)verts)->u;
  vtx.v = ((src_vertex *)verts)->v;
  vtx.base_color = ((src_vertex *)verts)->base_color;
  vtx.offset_color = ((src_vertex *)verts)->offset_color;
  list_block_sq_write(&vertex_buffer_desc, (uint *)&vtx, 3, 0x20);
}

void polystrip_tex_offset_blended(int count, uint *verts, int slot_no)
{
  typedef struct { float x; float y; float z; float u; float v; int base_color; int offset_color; } src_vertex;
  struct { int cmd; float x; float y; float z; float u; float v; int base_color; int offset_color; } vtx;
  int n;

  draw_ctx_ptr->surface = &tex_slot_table[slot_no].surface;
  draw_context_preset_apply(0x1f);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  n = count - 1;
  vtx.offset_color = 0;
  while (n != 0) {
    vtx.cmd = 0xe0000000;
    vtx.x = ((src_vertex *)verts)->x;
    vtx.y = ((src_vertex *)verts)->y;
    vtx.z = ((src_vertex *)verts)->z;
    vtx.u = ((src_vertex *)verts)->u;
    vtx.v = ((src_vertex *)verts)->v;
    vtx.base_color = ((src_vertex *)verts)->base_color;
    vtx.offset_color = ((src_vertex *)verts)->offset_color;
    list_block_sq_write(&vertex_buffer_desc, (uint *)&vtx, 3, 0x20);
    verts = verts + 7;
    n--;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = ((src_vertex *)verts)->x;
  vtx.y = ((src_vertex *)verts)->y;
  vtx.z = ((src_vertex *)verts)->z;
  vtx.u = ((src_vertex *)verts)->u;
  vtx.v = ((src_vertex *)verts)->v;
  vtx.base_color = ((src_vertex *)verts)->base_color;
  vtx.offset_color = ((src_vertex *)verts)->offset_color;
  list_block_sq_write(&vertex_buffer_desc, (uint *)&vtx, 3, 0x20);
}

void dualvol_ctx_init(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 0x3f3ffff;
  ctx->header_type = 0;
  ctx->list_type = 0;
  ctx->color_format = 0;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 1;
  ctx->z_write_off = 0;
  ctx->src_blend = 8;
  ctx->dst_blend = 6;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[3] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 4;
  q[8] = 3;
  q[11] = 0;
  q[9] = 0;
  q[10] = 0;
  ctx->face_color[0] = 1.0;
  ctx->face_color[1] = 1.0;
  ctx->face_color[2] = 1.0;
  ctx->face_color[3] = 1.0;
  ctx->offset_color[0] = 1.0;
  ctx->offset_color[1] = 1.0;
  ctx->offset_color[2] = 1.0;
  ctx->offset_color[3] = 1.0;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  context_volume2_header_select(ctx);
  return;
}

void modvol_ctx_init_first(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 0x3f3ffff;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->color_format = 0;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 1;
  ctx->z_write_off = 0;
  ctx->src_blend = 8;
  ctx->dst_blend = 6;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[3] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 4;
  q[8] = 3;
  q[11] = 0;
  q[9] = 0;
  q[10] = 0;
  ctx->face_color[0] = 1.0f;
  ctx->face_color[1] = 1.0f;
  ctx->face_color[2] = 1.0f;
  ctx->face_color[3] = 1.0f;
  ctx->offset_color[0] = 1.0f;
  ctx->offset_color[1] = 1.0f;
  ctx->offset_color[2] = 1.0f;
  ctx->offset_color[3] = 1.0f;
  ctx->volume_word = 0;
  ctx->bounds[0] = 0.0f;
  ctx->bounds[1] = 0.0f;
  ctx->bounds[2] = 639.0f;
  ctx->bounds[3] = 479.0f;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_ctx_init_closing(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 0x3f3ffff;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->color_format = 0;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 1;
  ctx->z_write_off = 0;
  ctx->src_blend = 8;
  ctx->dst_blend = 6;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[3] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 4;
  q[8] = 3;
  q[11] = 0;
  q[9] = 0;
  q[10] = 0;
  ctx->face_color[0] = 1.0;
  ctx->face_color[1] = 1.0;
  ctx->face_color[2] = 1.0;
  ctx->face_color[3] = 1.0;
  ctx->offset_color[0] = 1.0;
  ctx->offset_color[1] = 1.0;
  ctx->offset_color[2] = 1.0;
  ctx->offset_color[3] = 1.0;
  ctx->volume_word = 0x20000000;
  ctx->bounds[0] = 0.0;
  ctx->bounds[1] = 0.0;
  ctx->bounds[2] = 639.0;
  ctx->bounds[3] = 479.0;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_ctx_init_inner(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 0x3f3ffff;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->color_format = 0;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 1;
  ctx->z_write_off = 0;
  ctx->src_blend = 8;
  ctx->dst_blend = 6;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[3] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 4;
  q[8] = 3;
  q[11] = 0;
  q[9] = 0;
  q[10] = 0;
  ctx->face_color[0] = 1.0;
  ctx->face_color[1] = 1.0;
  ctx->face_color[2] = 1.0;
  ctx->face_color[3] = 1.0;
  ctx->offset_color[0] = 1.0;
  ctx->offset_color[1] = 1.0;
  ctx->offset_color[2] = 1.0;
  ctx->offset_color[3] = 1.0;
  ctx->volume_word = 0;
  ctx->bounds[0] = 0.0;
  ctx->bounds[1] = 0.0;
  ctx->bounds[2] = 639.0;
  ctx->bounds[3] = 479.0;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_tex_ctx_init_first(strip_context_rec *ctx, int slot_no)
{
  ctx->change_mask = 0x300008;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->tex_offset_bits = 3;
  ctx->surface = &tex_slot_table[slot_no].surface;
  ctx->volume_word = 0;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_tex_ctx_init_closing(strip_context_rec *ctx, int slot_no)
{
  ctx->change_mask = 0x300008;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->tex_offset_bits = 3;
  ctx->surface = &tex_slot_table[slot_no].surface;
  ctx->volume_word = 0x20000000;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_tex_ctx_init_inner(strip_context_rec *ctx, int slot_no)
{
  ctx->change_mask = 0x300008;
  ctx->header_type = 1;
  ctx->list_type = 1;
  ctx->tex_offset_bits = 3;
  ctx->surface = &tex_slot_table[slot_no].surface;
  ctx->volume_word = 0;
  context_hw_words_compute(ctx);
  context_header_select(ctx);
  return;
}

void modvol_ctxs_init(void)
{
  dualvol_ctx_init(&dualvol_ctx);
  modvol_tex_ctx_init_first(&modvol_ctx_first,0);
  modvol_tex_ctx_init_closing(&modvol_ctx_closing,0);
  modvol_tex_ctx_init_inner(&modvol_ctx_inner,0);
  return;
}

void modvol_send_triangles(uint *tris, int count)
{
  struct { uint cmd; float v[9]; int pad[6]; } tri;
  float *p = (float *)tris;

  context_header_select(&modvol_ctx_first);
  list_header_sq_write(&vertex_buffer_desc);
  tri.cmd = 0xe0000000;
  tri.v[0] = p[0];
  tri.v[1] = p[1];
  tri.v[2] = p[2];
  tri.v[3] = p[3];
  tri.v[4] = p[4];
  tri.v[5] = p[5];
  tri.v[6] = p[6];
  tri.v[7] = p[7];
  tri.v[8] = p[8];
  list_block_sq_write(&vertex_buffer_desc,&tri,0x11,0x40);
  p += 9;
  count--;
  context_header_select(&modvol_ctx_inner);
  list_header_sq_write(&vertex_buffer_desc);
  for (; count > 1; count--, p += 9) {
    tri.cmd = 0xe0000000;
    tri.v[0] = p[0];
    tri.v[1] = p[1];
    tri.v[2] = p[2];
    tri.v[3] = p[3];
    tri.v[4] = p[4];
    tri.v[5] = p[5];
    tri.v[6] = p[6];
    tri.v[7] = p[7];
    tri.v[8] = p[8];
    list_block_sq_write(&vertex_buffer_desc,&tri,0x11,0x40);
  }
  context_header_select(&modvol_ctx_closing);
  list_header_sq_write(&vertex_buffer_desc);
  tri.cmd = 0xe0000000;
  tri.v[0] = p[0];
  tri.v[1] = p[1];
  tri.v[2] = p[2];
  tri.v[3] = p[3];
  tri.v[4] = p[4];
  tri.v[5] = p[5];
  tri.v[6] = p[6];
  tri.v[7] = p[7];
  tri.v[8] = p[8];
  list_block_sq_write(&vertex_buffer_desc,&tri,0x11,0x40);
}

void polystrip_plain_dualvol(int count, uint *verts)
{
  struct { int cmd; float x; float y; float z; uint color_a; uint color_b; int pad[2]; } vtx;
  int i;

  context_header_select(&dualvol_ctx);
  context_volume2_header_select(&dualvol_ctx);
  list_header_sq_write(&vertex_buffer_desc);
  for (i = 0; i < count - 1; i++, verts = verts + 5) {
    vtx.cmd = 0xe0000000;
    vtx.x = ((float *)verts)[0];
    vtx.y = ((float *)verts)[1];
    vtx.z = ((float *)verts)[2];
    vtx.color_a = verts[3];
    vtx.color_b = verts[4];
    list_block_sq_write(&vertex_buffer_desc,&vtx,9,0x20);
  }
  vtx.cmd = 0xf0000000;
  vtx.x = ((float *)verts)[0];
  vtx.y = ((float *)verts)[1];
  vtx.z = ((float *)verts)[2];
  vtx.color_a = verts[3];
  vtx.color_b = verts[4];
  list_block_sq_write(&vertex_buffer_desc,&vtx,9,0x20);
}

void polystrip_tex_dualvol_opaque(int count, uint *verts, int slot_no)
{
  typedef struct { float x, y, z, u, v; uint base_color, offset_color; int pad[2]; uint base_color2; int pad2; } src_vertex;
  struct { uint cmd; float x, y, z, u, v; uint base_color, offset_color; float u2, v2; uint base_color2, offset_color2; int pad[4]; } vtx;
  src_vertex *p = (src_vertex *)verts;
  int n;
  
  draw_ctx_ptr->surface = &tex_slot_table[slot_no].surface;
  draw_context_preset_apply(0x12);
  context_header_select(draw_ctx_ptr);
  list_header_sq_write(&vertex_buffer_desc);
  for (n = count - 1; n != 0; n--) {
    vtx.cmd = 0xe0000000;
    vtx.x = p->x;
    vtx.y = p->y;
    vtx.z = p->z;
    vtx.u = p->u;
    vtx.v = p->v;
    vtx.base_color = p->base_color;
    vtx.offset_color = p->offset_color;
    vtx.u2 = vtx.u;
    vtx.v2 = vtx.v;
    vtx.base_color2 = p->base_color2;
    vtx.offset_color2 = vtx.offset_color;
    list_block_sq_write(&vertex_buffer_desc,&vtx,0xb,0x40);
    p++;
  }
  vtx.cmd = 0xf0000000;
  vtx.x = p->x;
  vtx.y = p->y;
  vtx.z = p->z;
  vtx.u = p->u;
  vtx.v = p->v;
  vtx.base_color = p->base_color;
  vtx.offset_color = p->offset_color;
  vtx.u2 = vtx.u;
  vtx.v2 = vtx.v;
  vtx.base_color2 = vtx.base_color;
  vtx.offset_color2 = vtx.offset_color;
  list_block_sq_write(&vertex_buffer_desc,&vtx,0xb,0x40);
}

void work_areas_set(int area_a,int area_b)
{
  area_a = (area_a + 31) & ~31;
  graphics_work.work_a_base = area_a;
  graphics_work.mark_depth = 0;
  work_a_marks[0] = area_a;
  graphics_work.work_a_out = area_a;
  graphics_work.work_a_out_end = area_a;
  area_b = (area_b + 31) & ~31;
  graphics_work.work_b_base = area_b;
  work_b_marks[0] = area_b;
  graphics_work.work_b_out = area_b;
  graphics_work.work_b_out_end = area_b;
}

void work_areas_rewind(void)
{
  graphics_work.work_a_out = graphics_work.work_a_out_end;
  graphics_work.work_b_out = graphics_work.work_b_out_end;
  return;
}

void work_areas_reset(void)
{
  graphics_work.mark_depth = 0;
  graphics_work.work_a_out_end = graphics_work.work_a_out = work_a_marks[0];
  graphics_work.work_b_out_end = graphics_work.work_b_out = work_b_marks[0];
}

int work_areas_mark_push(void)
{
  int depth;
  int result;

  result = -1;
  depth = graphics_work.mark_depth;
  if (depth < 0x30) {
    graphics_work.mark_depth = ++depth;
    work_a_marks[depth] = graphics_work.work_a_out;
    work_b_marks[depth] = graphics_work.work_b_out;
    result = 0;
  }
  return result;
}

int work_areas_mark_pop(void)
{
  int depth;
  int result;

  result = -1;
  depth = graphics_work.mark_depth;
  if (depth > 0) {
    graphics_work.work_b_out = work_b_marks[depth];
    graphics_work.work_a_out = work_a_marks[depth];
    graphics_work.mark_depth = depth - 1;
    result = 0;
  }
  return result;
}
