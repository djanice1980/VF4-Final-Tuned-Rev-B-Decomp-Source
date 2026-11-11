/*
 * gfxlib/tex_render.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint tex_render_done_count; /* 0x0c32a47c */
extern uint tex_render_done_chained; /* 0x0c32a484 */
extern uint tex_queue_run_start; /* 0x0c32a488 */
extern uint tex_queue_run_cursor; /* 0x0c32a48c */
extern uint tex_queue_fill_start; /* 0x0c32a490 */
extern uint bank_ring_index; /* 0x0c32a498 */
extern uint bank_record_saved; /* 0x0c32a49c */
extern tex_target_rec *tex_queue_buffers[2][32]; /* 0x0c41595c */
extern uint pvr_saved_reg_f4; /* 0x0c415a5c */
extern uint pvr_saved_reg_118; /* 0x0c415a60 */
extern uint tex_queue_half; /* 0x0c415a64 */
extern uint tex_render_bank; /* 0x0c415a68 */
extern list_bank_rec bank_record_copy; /* 0x0c415a6c */
extern char bank_ring[8][220]; /* 0x0c415b7c */
extern uint bank_ring_last; /* 0x0c41625c */
extern uint bank_build_index; /* 0x0c41e58c */
extern uint xfer_pending[3]; /* 0x0c41e9d4 */
extern uint bank_render_index; /* 0x0c41e9ec */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

typedef struct { int w[2]; } dma_word_pair; typedef struct { int e[3]; } dma_word_three; typedef struct { int pad[4]; int on; int z; int c; dma_word_pair d[3]; dma_word_three e; } dma_sub_view; static inline volatile dma_sub_view *dma_sub_at(char *tbl, int slot, int i)
{
  return (volatile dma_sub_view *)(slot * 0x110 + i * 0x34 + tbl);
}

void list_block_sizes_from_alloc(int *sizes, uint alloc_word)
{
  int i;
  uint v;
  for (i = 0; i < 5; i++) {
    v = (alloc_word >> (i * 4)) & 3;
    *sizes++ = v ? 1 << (v + 4) : 0;
  }
}

void list_area_starts_lay_out(int *starts, int start, int tile_count, int *sizes)
{
  int size;
  int i;

  i = 0;
  do {
    *starts = start;
    starts = starts + 1;
    size = *sizes;
    sizes = sizes + 1;
    start = start + tile_count * size;
    i = i + 1;
  } while (i < 5);
  *starts = start;
  return;
}

int fb_pack_mode_for_texture(tex_slot_rec *tex, uint *write_ctrl)
{
  uint v;

  if ((tex->surface.format_flags & 4) != 0) return 4;
  if (tex->surface.texel_size_class != 1) return 4;
  v = 0;
  switch (tex->surface.pixel_format & 0x38000000) {
  case 0:
    v = 3;
    break;
  case 0x08000000:
    v = 1;
    break;
  case 0x10000000:
    v = 2;
    break;
  }
  *write_ctrl = gfx_state.write_ctrl_word & 0xfffffff8 | v;
  return 0;
}

void texture_clip_words_get(tex_slot_rec *tex, uint *x_clip, uint *y_clip)
{
  uint saved_x;
  int saved_y;

  saved_x = gfx_state.x_clip_word;
  saved_y = gfx_state.y_clip_word;
  fb_clip_rect_store(0,0,tex->surface.width + -1,tex->surface.height + -1);
  *x_clip = gfx_state.x_clip_word;
  *y_clip = gfx_state.y_clip_word;
  gfx_state.x_clip_word = saved_x;
  gfx_state.y_clip_word = saved_y;
  return;
}

int texture_fb_write_regs_build(fb_write_regs *regs, tex_slot_rec *tex)
{
  int status;

  status = fb_pack_mode_for_texture(tex,&regs->write_ctrl);
  regs->stride_units = (tex->surface.width << 4) >> 6;
  regs->write_start = tex->surface.tex_address | 0x1000000;
  texture_clip_words_get(tex,&regs->x_clip,&regs->y_clip);
  return status;
}

void list_area_lay_out(int *area, int tile_count, uint alloc_word)
{
  list_block_sizes_from_alloc(area + 6,alloc_word);
  list_area_starts_lay_out(area,0,tile_count,area + 6);
  return;
}

void tex_target_bank_lay_out(tex_target_bank *layout, int *area, int tile_count, uint alloc_word)
{
  int offset;

  offset = 0;
  list_area_lay_out(area,tile_count,alloc_word);
  layout->obj_list_base = offset;
  offset = area[5];
  layout->region_array = offset;
  if ((gfx_state.flags & 0x80) != 0) {
    offset = offset + 0x18;
  }
  offset = offset + tile_count * 0x18;
  layout->field_0xc = 0;
  layout->bg_plane = offset;
  offset += 0x3c;
  layout->size = offset;
}

void tex_target_bank_addresses_set(tex_target_bank *bank, tex_target_bank *layout, int param_base, int block)
{
  bank->param_base = param_base;
  bank->region_array = layout->region_array + param_base + block;
  bank->obj_list_base = layout->obj_list_base + param_base + block;
  bank->field_0xc = layout->field_0xc + param_base + block;
  bank->bg_plane = param_base + layout->bg_plane + block;
  bank->size = layout->size;
  return;
}

void tex_target_banks_alloc(tex_target_rec *target)
{
  uint block;
  tex_target_bank layout;

  tex_target_bank_lay_out(&layout,target->list_starts,target->tile_count,
               target->list_alloc_word);
  block = vram_pair_pool_take(layout.size);
  block = block & 0x3fffff;
  target->vram_block = block;
  target->vram_block_size = layout.size;
  tex_target_bank_addresses_set(&target->bank[0],&layout,gfx_state.ta_bufs[0].isp_base,block);
  tex_target_bank_addresses_set(&target->bank[1],&layout,gfx_state.ta_bufs[1].isp_base,block);
  return;
}

void tex_target_banks_place(tex_target_rec *target, uint block)
{
  tex_target_bank layout;

  tex_target_bank_lay_out(&layout,target->list_starts,target->tile_count,
               target->list_alloc_word);
  tex_target_bank_addresses_set(&target->bank[0],&layout,gfx_state.ta_bufs[0].isp_base,block);
  tex_target_bank_addresses_set(&target->bank[1],&layout,gfx_state.ta_bufs[1].isp_base,block);
  return;
}

int tex_target_region_array_write(tex_target_rec *target, int addr, int *starts, int *sizes)
{
  uint x;
  uint y;
  uint n;
  uint w;
  uint h;
  struct { uint hdr; int v[5]; } s;

  w = target->tiles_x;
  h = target->tiles_y;
  if ((gfx_state.flags & 0x80) != 0) {
    tile_regions_put_flush_entry(addr);
    addr = addr + 0x18;
  }
  for (x = 0; x < w; x++) {
    n = x;
    for (y = 0; y < h; y++) {
      s.hdr = y << 8 | x << 2;
      if (sizes[0] != 0) {
        s.v[0] = n * sizes[0] + starts[0];
      } else {
        s.v[0] = 0x80000000;
      }
      if (sizes[1] != 0) {
        s.v[1] = n * sizes[1] + starts[1];
      } else {
        s.v[1] = 0x80000000;
      }
      if (sizes[2] != 0) {
        s.v[2] = n * sizes[2] + starts[2];
      } else {
        s.v[2] = 0x80000000;
      }
      if (sizes[3] != 0) {
        s.v[3] = n * sizes[3] + starts[3];
      } else {
        s.v[3] = 0x80000000;
      }
      if (sizes[4] != 0) {
        s.v[4] = n * sizes[4] + starts[4];
      } else {
        s.v[4] = 0x80000000;
      }
      if (x == w - 1 && y == h - 1) {
        s.hdr |= 0x80000000;
      }
      vram32_write(addr,(uint *)&s,0x18);
      addr += 0x18;
      n += w;
    }
  }
  return addr;
}

void tex_target_bg_plane_write(uint addr, tex_slot_rec *tex, uint color0, uint color1, uint color2)
{
  float k;
  float *d;

  k = 1e-05;
  d = (float *)(addr | 0xa5000000);
  *(uint *)d = gfx_state.bg_isp_word;
  ((uint *)d)[1] = gfx_state.bg_tsp_word | 0x100000;
  ((uint *)d)[2] = gfx_state.bg_tex_word;
  d[3] = 0.0;
  d[4] = 0.0;
  d[5] = k;
  ((uint *)d)[6] = color0;
  d[7] = (float)tex->surface.width;
  d[8] = 0.0;
  d[9] = k;
  ((uint *)d)[10] = color1;
  d[11] = 0.0;
  d[12] = (float)tex->surface.height;
  d[13] = k;
  ((uint *)d)[14] = color2;
}

void tex_target_bg_plane_rewrite(tex_target_rec *target, int bank, uint color0, uint color1, uint color2)
{
  tex_target_bank *b = &target->bank[bank];
  tex_target_bg_plane_write(b->bg_plane,target->tex,color0,color1,color2);
}

int bg_plane_256x128_write(int param_base, uint link_addr, uint plane_addr, uint color)
{
  uint *p;

  p = (uint *)(link_addr | 0xa5000000);
  *p = plane_addr - param_base >> 2 | 0x80200000;
  p = (uint *)(plane_addr | 0xa5000000);
  p[0] = 0xe0800000;
  p[1] = 0x20800000;
  p[2] = 0;
  ((float *)p)[3] = 0.0;
  ((float *)p)[4] = 0.0;
  ((float *)p)[5] = 1e-10;
  p[6] = color;
  ((float *)p)[7] = 0.0;
  ((float *)p)[8] = 128.0;
  ((float *)p)[9] = 1e-10;
  p[10] = color;
  ((float *)p)[11] = 256.0;
  ((float *)p)[12] = 0.0;
  ((float *)p)[13] = 1e-10;
  p[14] = color;
  plane_addr += 0x3c;
  return plane_addr;
}

int bg_plane_256x128_write_link_out(uint *link, int param_base, uint plane_addr, uint color)
{
  uint *p;

  *link = plane_addr - param_base >> 2 | 0x80200000;
  p = (uint *)(plane_addr | 0xa5000000);
  p[0] = 0xe0800000;
  p[1] = 0x20800000;
  p[2] = 0;
  ((float *)p)[3] = 0.0;
  ((float *)p)[4] = 0.0;
  ((float *)p)[5] = 1e-10;
  p[6] = color;
  ((float *)p)[7] = 0.0;
  ((float *)p)[8] = 128.0;
  ((float *)p)[9] = 1e-10;
  p[10] = color;
  ((float *)p)[11] = 256.0;
  ((float *)p)[12] = 0.0;
  ((float *)p)[13] = 1e-10;
  p[14] = color;
  plane_addr += 0x3c;
  return plane_addr;
}

int region_array_one_list_write(int addr, uint tiles_x, uint tiles_y, uint list_addr)
{
  uint x;
  uint y;
  int p;
  struct { uint w[6]; uint tiles_y; uint list_addr; } s;

  s.tiles_y = tiles_y;
  s.list_addr = list_addr;
  if ((gfx_state.flags & 0x80) != 0) {
    tile_regions_put_flush_entry(addr);
    addr = addr + 0x18;
  }
  p = addr;
  for (y = 0; y < s.tiles_y; y++) {
    for (x = 0; x < tiles_x; x++) {
          s.w[0] = (x << 2 | y << 8) | 0x20000000;
          s.w[1] = s.list_addr;
          s.w[2] = 0x80000000;
          s.w[3] = 0x80000000;
          s.w[4] = 0x80000000;
          s.w[5] = 0x80000000;
          if ((x == tiles_x - 1) && (y == s.tiles_y - 1)) {
            s.w[0] |= 0x80000000;
          }
          vram32_write(p,(uint *)&s,0x18);
          p = p + 0x18;
    }
  }
  return p;
}

uint bg_plane_and_region_array_write(uint param_base, uint link_addr, uint tiles_x, uint tiles_y, uint color)
{
  int link;
  int plane_end;
  uint regions;
  link = link_addr;
  plane_end = bg_plane_256x128_write(param_base, link, link + 8, color);
  *((uint *) ((link | 0xa5000000) + 4)) = 0xf0000000;
  regions = (plane_end + 99U) & 0xffffff9c;
 if (tiles_x) { region_array_one_list_write(regions, tiles_x, tiles_y, link); return regions; } else { region_array_one_list_write(regions, tiles_x, tiles_y, link); return regions; }
}

void tex_target_build(tex_target_rec *target, tex_slot_rec *tex, uint alloc_word, int block, uint width, uint height)
{
  uint tiles_y;
  uint tiles_x;
  int tile_count;

  target->width = width;
  target->height = height;
  target->tex = tex;
  tiles_x = width >> 5;
  target->tiles_x = tiles_x;
  tiles_y = height >> 5;
  target->tiles_y = tiles_y;
  target->tile_clip = (tiles_y - 1) << 16 | tiles_x - 1;
  tile_count = tiles_x * tiles_y;
  target->tile_count = tile_count;
  target->list_alloc_word = alloc_word | 0x100000;
  if (block != 0) {
    tex_target_banks_place(target,block);
  }
  else {
    tex_target_banks_alloc(target);
  }
  list_area_starts_lay_out(target->list_starts,target->bank[0].obj_list_base,tile_count,target->block_sizes);
  tex_target_region_array_write(target,target->bank[0].region_array,target->list_starts,target->block_sizes);
  list_area_starts_lay_out(target->list_starts,target->bank[1].obj_list_base,tile_count,target->block_sizes);
  tex_target_region_array_write(target,target->bank[1].region_array,target->list_starts,target->block_sizes);
  tex_target_bg_plane_write(target->bank[0].bg_plane,tex,0,0,0);
  tex_target_bg_plane_write(target->bank[1].bg_plane,tex,0,0,0);
  texture_fb_write_regs_build(&target->fb,target->tex);
  return;
}

void tex_target_size_set(tex_target_rec *target, uint alloc_word, uint width, uint height)
{
  uint tiles_x;
  uint tiles_y;

  target->width = width;
  target->height = height;
  tiles_x = width >> 5;
  target->tiles_x = tiles_x;
  tiles_y = height >> 5;
  target->tiles_y = tiles_y;
  target->tile_clip = (tiles_y - 1) << 16 | tiles_x - 1;
  target->tile_count = tiles_x * tiles_y;
  target->list_alloc_word = alloc_word | 0x100000;
  tex_target_banks_alloc(target);
  return;
}

void tex_target_regs_load(tex_target_rec *target, int bank)
{
  tex_target_bank *b;
  int d;

  b = &target->bank[bank];
  gfx_state.render_busy = 3;
  gfx_state.slot_states[gfx_state.render_slot] = 4;
  if ((gfx_state.flags & 0x10) != 0) {
    pvr_reg_write_both(8,2);
    pvr_reg_write_both(8,0);
  }
  pvr_reg_write_both(0x2c,b->region_array);
  pvr_reg_write_both(0x20,b->param_base);
  pvr_reg_write_both(0x60,target->fb.write_start);
  pvr_reg_write_both(0x48,target->fb.write_ctrl);
  pvr_reg_write_both(0x4c,target->fb.stride_units);
  pvr_reg_write_both(0x68,target->fb.x_clip);
  pvr_reg_write_both(0x6c,target->fb.y_clip);
  d = b->bg_plane - b->param_base;
  pvr_reg_write_both(0x8c,gfx_state.bg_tag_bits & 0xff000000 | d * 2 & 0xfffff8);
  pvr_reg_write_both(0x88,gfx_state.bg_depth & 0xfffffff0);
  return;
}

void tex_render_start(void)
{
  pvr_reg_write_both(0x14,0xffffffff);
  return;
}

int tex_render_wait(void)
{
  int timed_out;
  gfx_state.vblank_ticks = 0;
  timed_out = 0;
  while (gfx_state.render_busy != 0) {
    (*(void (*)())gfx_state.hooks[7].fn)(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
      timed_out = 1;
      break;
    }
  }
  return timed_out;
}

void tex_render_done_step(void)
{
  tex_target_rec *next;
  next = 0;
  tex_render_done_count = tex_render_done_count + 1;
  if (*(int **)&tex_queue_run_cursor != (int *)0x0) {
    next = **(tex_target_rec ***)&tex_queue_run_cursor;
    *(int **)&tex_queue_run_cursor = *(int **)&tex_queue_run_cursor + 1;
  }
  if (next != 0) {
    tex_target_regs_load(next,tex_render_bank);
    tex_render_start();
  }
  else {
    *(int **)&tex_queue_run_cursor = (int *)0x0;
    if (tex_render_done_chained != 0) {
      (*(code *)tex_render_done_chained)();
    }
  }
}

void tex_queue_flip(void)
{
  tex_render_bank = gfx_state.frame_idx & 1;
  tex_queue_run_start = tex_queue_fill_start;
  tex_queue_run_cursor = tex_queue_fill_start;
  tex_queue_half = tex_queue_half + 1 & 1;
  tex_queue_fill_start = (uint)tex_queue_buffers[tex_queue_half];
  tex_queue_fill_write = tex_queue_buffers[tex_queue_half];
  return;
}

void tex_queue_terminate(void)
{
  if (tex_queue_fill_start != 0) {
    *tex_queue_fill_write = 0;
  }
  return;
}

void tex_queue_push(tex_target_rec *target)
{
  tex_target_rec **p = tex_queue_fill_write;
  if (p != 0) {
    *p = target;
    tex_queue_fill_write = p + 1;
  }
}

void tex_queue_install(void)
{
  tex_queue_run_start = 0;
  tex_queue_run_cursor = 0;
  tex_queue_fill_start = 0;
  tex_queue_fill_write = (tex_target_rec **)0x0;
  irq_slot_fn_fetch(0,&tex_render_done_chained);
  irq_slot_fn_install(0,(uint)tex_render_done_step);
}

void pvr_scaler_regs_save(void)
{
  pvr_saved_reg_f4 = pvr_reg_read(0xf4);
  pvr_saved_reg_118 = pvr_reg_read(0x118);
  return;
}

void pvr_scaler_regs_restore(void)
{
  pvr_reg_write_both(0xf4,pvr_saved_reg_f4);
  pvr_reg_write_both(0x118,pvr_saved_reg_118);
  return;
}

void list_chunk_copy_to_cmd_buffer(uint *out, uint *chunk)
{
  uint *d;
  uint *s;
  uint len;
  int n;
  int i;

  d = elan_cmd_write_ptr;
  s = (uint *)*chunk;
  len = chunk[1];
  n = len >> 2;
  *out = (uint)d;
  out[1] = len;
  for (i = 0; i < n; i++) {
    *d++ = *s++;
  }
  elan_cmd_write_ptr = d;
  return;
}

void list_chain_append(uint *chain, uint *piece)
{
  uint *p = (uint *)chain[1];
  if (p != 0) {
    *p = *piece;
    chain[1] = piece[1];
    chain[2] = chain[2] + piece[2];
  }
  else {
    *chain = *piece;
    chain[1] = piece[1];
    chain[2] = piece[2];
  }
  *piece = 0;
  piece[1] = 0;
  piece[2] = 0;
}

void bank_record_saved_clear(void)
{
  bank_record_saved = 0;
  return;
}

void bank_record_save(void)
{
  if (bank_record_saved == 0) {
    bank_record_copy = list_banks[bank_build_index];
    bank_record_saved = 1;
  }
  return;
}

void bank_record_restore(void)
{
  if (bank_record_saved != 0) {
    list_banks[bank_build_index] = bank_record_copy;
    bank_record_saved = 0;
  }
  return;
}

void bank_lists_load_from_ring(uint *rec)
{
  typedef struct { int w[2]; } dma_pair;
  typedef struct { int e[3]; } dma_three;
  typedef struct { dma_pair d[3]; int c; dma_three e; } dma_src_ent;
  typedef struct { int a; int on; int z; int c; dma_pair d[3]; dma_three e; } dma_dst_ent;
  typedef struct { int pad[4]; int on; int z; int c; dma_pair d[3]; dma_three e; } dma_dst_view;
  typedef struct { int w0; int pad[2]; dma_dst_ent s[5]; } dma_dst;
  typedef struct { int w0; dma_src_ent s[5]; } dma_src;
  int i;
  dma_dst_view *d;
  dma_src_ent *s;
  
  *(volatile uint *)(((struct { int v; } *)&bank_build_index)->v * 0x110 + (int)list_banks) = *rec;
  for (i = 0; i < 5; i++) {
    char *b;
    s = &((dma_src *)rec)->s[i];
    b = (char *)list_banks;
    d = (dma_dst_view *)(((struct { int v; } *)&bank_build_index)->v * 0x110 + i * 0x34 + (int)b);
    d->on = 1;
    d->z = 0;
    d->c = (&((dma_src *)rec)->s[i])->c;
    d->d[0] = s->d[0];
    d->d[1] = (&((dma_src *)rec)->s[i])->d[1];
    d->d[2] = (&((dma_src *)rec)->s[i])->d[2];
    d->e = (&((dma_src *)rec)->s[i])->e;
  }
}

void bank_lists_store_to_ring(void)
{
  typedef struct { dma_word_pair d[3]; int c; dma_word_three e; } dma_src_ent;
  typedef struct { int w0; dma_src_ent s[5]; int tail[4]; } dma_src;
  int i;
  dma_src *p;
  dma_src_ent *q;

  p = &((dma_src *)bank_ring)[bank_ring_index];
  p->w0 = ((volatile list_bank_rec *)list_banks)[bank_build_index].state;
  for (i = 0; i < 5; i++) {
    q = &p->s[i];
    q->c = dma_sub_at((char *)list_banks, bank_build_index, i)->c;
    q->d[0] = dma_sub_at((char *)list_banks, bank_build_index, i)->d[0];
    q->d[1] = dma_sub_at((char *)list_banks, bank_build_index, i)->d[1];
    q->d[2] = dma_sub_at((char *)list_banks, bank_build_index, i)->d[2];
    q->e = dma_sub_at((char *)list_banks, bank_build_index, i)->e;
  }
}

void bank_list_tail_read(int list, uint *piece)
{
  list_bank_list_rec *e;

  e = &list_banks[bank_build_index].lists[list];
  piece[4] = e->seg_count;
  if (e->seg_count == 0) {
    piece[0] = e->first_seg.addr;
    piece[1] = e->first_seg.length;
  }
  else {
    piece[0] = e->last_seg.addr;
    piece[1] = e->last_seg.length;
  }
  return;
}

void bank_list_tail_swap(int list, uint *piece)
{
  list_bank_list_rec *e;
  uint *end;
  uint added;

  e = &list_banks[bank_build_index].lists[list];
  piece[2] = e->last_seg.addr;
  piece[3] = e->last_seg.length;
  added = piece[4] - e->seg_count;
  if (added != 0) {
    e->last_seg.addr = piece[0];
    e->last_seg.length = piece[1];
    e->seg_count = piece[4];
    end = (uint *)(piece[0] + piece[1] - 0x20);
    piece[0] = end[1] | 0xaa000000;
    piece[1] = end[3];
  }
  piece[4] = added;
  return;
}

void bank_list_piece_append(int list, uint *piece)
{
  list_bank_list_rec *e;

  if (piece[4] != 0) {
    e = &list_banks[bank_build_index].lists[list];
    if (e->seg_count == 0) {
      list_seg_link_to((int *)&e->first_seg,piece);
    }
    else {
      list_seg_link_to((int *)&e->last_seg,piece);
    }
    e->last_seg.addr = piece[2];
    e->last_seg.length = piece[3];
    e->seg_count = e->seg_count + piece[4];
  }
}

void elan_block_queue_relay(int list, list_block_ref *block)
{
  elan_list_block_queue(list,block);
  return;
}

void ring_list_piece_append(int rec, int list, uint *piece)
{
  typedef struct { uint addr; uint size; } pr;
  struct e { uint head[4]; pr tail; int count; char pad[12]; } *e;
  int *q;

  e = &((struct e *)(rec + 4))[list];
  if (e->count != 0) q = (int *)&e->tail;
  else q = (int *)e->head;
  list_seg_link_to(q,piece);
  e->count++;
  e->tail = *(pr *)piece;
}

void ring_list_block_append(uint rec, int list, uint addr, int size)
{
  uint piece [2];

  piece[0] = addr;
  piece[1] = size + 0x20;
  ring_list_piece_append(rec,list,piece);
  return;
}

void bank_ring_open_next(void)
{
  struct ring_list_ent { uint seg0[2]; uint seg1[2]; uint tail[2]; int count; int jobs[3]; };
  struct ring_rec { int closed; struct ring_list_ent s[5]; int jobs[3]; int last_list; } *rec;
  int i;
  struct bank_list_ent { uint pair0[2]; uint pair1[2]; char r[0x24]; };
  struct bank_list_row { struct bank_list_ent e[5]; char pad[12]; };

  bank_ring_index = bank_ring_index + 1 & 7;
  rec = (struct ring_rec *)bank_ring[bank_ring_index];
  rec->closed = 0;
  jobq_reset(rec->jobs,0);
  for (i = 0; i < 5; i++) {
    struct ring_list_ent *s = &rec->s[i];
    s->count = 0;
    s->tail[0] = 0;
    list_chunk_copy_to_cmd_buffer(s->seg0,((struct bank_list_row *)((char *)list_banks + 0x1c))[bank_build_index].e[i].pair0);
    list_chunk_copy_to_cmd_buffer(s->seg1,((struct bank_list_row *)((char *)list_banks + 0x1c))[bank_build_index].e[i].pair1);
    jobq_reset(s->jobs,0);
    xfer_job_enqueue(s->jobs,i,s->seg0,0xa9000000,0,0,0);
  }
  bank_lists_load_from_ring((uint *)rec);
}

void bank_ring_add_slot_lists(uint rec, int slot)
{
  if (gfx_state.slot_list_bytes[0][slot] != 0) {
    ring_list_block_append(rec,0,gfx_state.slot_list_start[0][slot],gfx_state.slot_list_bytes[0][slot]);
  }
  if (gfx_state.slot_list_bytes[1][slot] != 0) {
    if (gfx_state.mv_hdr_on == 1) {
      *gfx_state.mv_hdr = *gfx_state.mv_hdr & 0xf0ffffff | 0x1000000;
      ring_list_block_append(rec,1,(uint)gfx_state.mv_hdr,gfx_state.mv_hdr_bytes);
    }
    ring_list_block_append(rec,1,gfx_state.slot_list_start[1][slot],gfx_state.slot_list_bytes[1][slot]);
  }
  if (gfx_state.slot_list_bytes[2][slot] != 0) {
    ring_list_block_append(rec,2,gfx_state.slot_list_start[2][slot],gfx_state.slot_list_bytes[2][slot]);
  }
  if (gfx_state.slot_list_bytes[3][slot] != 0) {
    if (gfx_state.mv_hdr_on == 1) {
      *gfx_state.mv_hdr = *gfx_state.mv_hdr & 0xf0ffffff | 0x3000000;
      ring_list_block_append(rec,3,(uint)gfx_state.mv_hdr,gfx_state.mv_hdr_bytes);
    }
    ring_list_block_append(rec,3,gfx_state.slot_list_start[3][slot],gfx_state.slot_list_bytes[3][slot]);
  }
  if (gfx_state.slot_list_bytes[4][slot] != 0) {
    ring_list_block_append(rec,4,gfx_state.slot_list_start[4][slot],gfx_state.slot_list_bytes[4][slot]);
  }
}

uint bank_ring_add_capture(int index, int capture)
{
  char *rec = bank_ring[index];
  uint mask = 0;
  int i;
  for (i = 0; i < 5; i++) {
    if (*(int *)(i * 4 + capture + 0x18) != 0) {
      mask |= (1 << i) | (0x20 << i);
      ring_list_block_append((uint)rec, i, *(int *)(i * 4 + capture + 4), *(int *)(i * 4 + capture + 0x18));
    }
  }
  return mask;
}

void bank_ring_close(uint list_mask)
{
  char *rec;
  char *e;
  int i;
  int list;
  int order[6];

  rec = bank_ring[bank_ring_index];
  order[5] = -1;
  order[0] = 0;
  order[1] = 1;
  order[2] = 2;
  order[3] = 3;
  order[4] = 4;
  for (i = 4; i >= 0; i--) {
    list = order[i];
    if (list_mask & (1 << list)) {
      order[5] = list;
      break;
    }
  }
  for (i = 0; i < 5; i++) {
    list = order[i];
    e = rec + list * 40 + 4;
    if (list_mask & (1 << list)) {
      list_seg_end_mark((int *)(e + 8),0,1);
      ring_list_piece_append((int)rec,list,(uint *)(e + 8));
      list_chain_append((uint *)(rec + 0xcc),(uint *)(e + 0x1c));
    }
    else {
      jobq_reset((int *)(e + 0x1c),1);
    }
  }
  *(int *)(rec + 0xd8) = order[5];
  *(int *)rec = 1;
}

void bank_ring_replay(int index)
{
  typedef struct { char pad[0xcc]; uint jobs_head; uint jobs_tail; uint job_count; uint last_list; } src_ent;
  src_ent *s = &((src_ent *)bank_ring)[index];
  volatile list_bank_rec *b;
  bank_ring_last = index;
  bank_render_index = bank_render_index - 1 & 3;
  b = (volatile list_bank_rec *)list_banks;
  b[*(volatile int *)&bank_render_index].job_count = s->job_count;
  b[*(volatile int *)&bank_render_index].last_list = s->last_list;
  b[*(volatile int *)&bank_render_index].state = 1;
  jobq_concat(xfer_pending,&s->jobs_head);
}

void bank_ring_drop_jobs(int index)
{
  char *rec = bank_ring[index];
  bank_ring_last = index;
  jobq_reset((int *)(rec + 0xcc),1);
}
