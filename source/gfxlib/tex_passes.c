/*
 * gfxlib/tex_passes.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint bank_ring_index; /* 0x0c32a498 */
extern uint list_end_hook_count; /* 0x0c32a4a0 */
extern uint pass_queue_read_base; /* 0x0c32a4a4 */
extern uint pass_queue_read; /* 0x0c32a4a8 */
extern uint pass_queue_write_base; /* 0x0c32a4ac */
extern pass_entry_rec *pass_current; /* 0x0c32a4b4 */
extern uint pass_entry_pool_next; /* 0x0c32a4b8 */
extern uint pass_queue_half; /* 0x0c32a4bc */
extern uint pass_fb_select; /* 0x0c32a4c0 */
extern uint ta_regs_need_restore; /* 0x0c32a4c4 */
extern uint ta_param_used_last; /* 0x0c32a4c8 */
extern uint ta_param_used_max; /* 0x0c32a4dc */
extern pass_entry_rec *pass_open_entry; /* 0x0c32a4f0 */
extern render_target_rec *target_current; /* 0x0c32a4f4 */
extern uint ta_target_buf_select; /* 0x0c32a4f8 */
extern uint cpu_ol_param_base; /* 0x0c416b00 */
extern uint saved_ta_tile_clip; /* 0x0c416b04 */
extern uint saved_ta_block_sizes; /* 0x0c416b08 */
extern char mtx_stack_selected; /* 0x0c41c79c */
extern float proj_rect_width; /* 0x0c41c950 */
extern float proj_rect_height; /* 0x0c41ca20 */
extern cpu_ol_target_rec cpu_ol_target; /* 0x0c416b0c */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

int target_pass_next_start(void)
{
  pass_entry_rec *entry;

  entry = 0;
  if (pass_current != 0) {
    tex_queue_push(pass_current->target);
    pass_current = 0;
  }
  if (pass_queue_read != 0) {
    entry = *(pass_entry_rec **)pass_queue_read;
    pass_queue_read = pass_queue_read + 4;
  }
  if (entry != 0) {
    tile_grid_set_from_counts(entry->target->tiles_x + -1,
                 entry->target->tiles_y + -1);
    elan_input_reset_tile_word(0);
    ta_continue_into_target(entry->target,pass_fb_select + 1U & 1);
    bank_ring_replay(entry->ring_index);
    tex_target_bg_plane_rewrite(entry->target,pass_fb_select + 1U & 1,entry->bg_words[0],
                 entry->bg_words[1],entry->bg_words[2]);
    pass_current = entry;
    return 1;
  }
  pass_queue_read = 0;
  tile_grid_set_from_counts(0x13,0xe);
  elan_input_reset_tile_word(tile_split_word);
  if (ta_regs_need_restore != 0) {
    ta_regs_need_restore = 0;
    ta_tile_alloc_regs_restore();
  }
  return 0;
}

void target_queues_flip(void)
{
  pass_fb_select = gfx_state.frame_idx & 1;
  ta_tile_alloc_regs_save();
  ta_regs_need_restore = 1;
  pass_queue_read_base = pass_queue_write_base;
  pass_queue_read = pass_queue_write_base;
  pass_queue_half = pass_queue_half + 1 & 1;
  pass_queue_write_base = (uint)pass_queues[pass_queue_half];
  pass_queue_write = pass_queues[pass_queue_half];
  pass_entry_pool_next = (uint)pass_entry_pools[pass_queue_half];
  pass_current = 0;
  return;
}

void target_queue_terminate(void)
{
  if (pass_queue_write_base != 0) {
    *pass_queue_write = 0;
  }
  return;
}

void target_queue_push(pass_entry_rec *entry)
{
  pass_entry_rec **p = pass_queue_write;
  if (p != 0) {
    *p = entry;
    pass_queue_write = p + 1;
  }
}

pass_entry_rec * target_entry_alloc(void)
{
  pass_entry_rec *entry;

  if (!(pass_entry_pool_next == 0)) {
    entry = (pass_entry_rec *)pass_entry_pool_next;
    pass_entry_pool_next = pass_entry_pool_next + 0x40;
  }
  else {
    entry = 0;
  }
  return entry;
}

void list_input_end_with_passes(void)
{
  uint lo;
  uint v;
  uint w;
  uint p;
  uint t;

  list_end_hook_count = list_end_hook_count + 1;
  if (gfx_state.slot_states[gfx_state.ta_slot] != -1) {
    if (target_pass_next_start() != 0) return;
    if (gfx_state.tex_render_on == 0) {
      gfx_state.pending_events = gfx_state.pending_events | 0x20;
      gfx_state.slot_states[gfx_state.ta_slot] = 3;
    }
    lo = *(volatile uint *)0xa05f8138;
    v = *(volatile uint *)0xa25f8138;
    if (v < lo) v = lo;
    if ((gfx_state.flags & 1) != 0) p = v;
    else {
      p = gfx_state.ta_bufs[gfx_state.ta_slot & 1].isp_limit;
    }
    ta_param_used_last = v;
    ta_param_limit_last = *(volatile uint *)0xa05f8130;
    render_stats.param_used[gfx_state.ta_slot & 1] = v;
    w = *(volatile uint *)0xa05f8134;
    lo = *(volatile uint *)0xa25f8134;
    if (lo > w) lo = w;
    ta_obj_next_last = lo;
    ta_obj_limit_last = *(volatile uint *)0xa05f812c;
    {
    gfx_state.bg_tag = gfx_state.bg_tag_bits & 0xff000000 |
         (p - gfx_state.ta_bufs[gfx_state.ta_slot & 1].isp_base) * 2 & 0xfffff8;
    }
    {
      uint *src = (uint *)&gfx_state.bg_plane[0];
      p += 0xa5000000;
      for (v = 0; v < 0x30; v++) {
        ((uint *)p)[v] = src[v];
      }
    }
    lo = ta_param_used_last;
    if (lo > 0x3fffff) lo += 0xffc00000;
    ta_param_used_wrapped = lo;
    {
      uint *e = (uint *)&ta_obj_next_wrapped;
      *e = ta_obj_next_last * 4;
      t = *e;
      if (t > 0x3fffff) t -= 0x400000;
      *e = t;
    }
    if (ta_param_used_wrapped > ta_param_used_max) ta_param_used_max = ta_param_used_wrapped;
    if (ta_obj_next_wrapped < ta_obj_next_min) ta_obj_next_min = ta_obj_next_wrapped;
    ta_obj_param_gap = ta_obj_next_wrapped - ta_param_used_wrapped;
    if (ta_obj_param_gap < ta_obj_param_gap_min) ta_obj_param_gap_min = ta_obj_param_gap;
    *(volatile int *)0xa85f8008 = 1;
    t = *(volatile uint *)0xa05f8138;
    t = *(volatile uint *)0xa25f8138;
    *(volatile int *)0xa85f8008 = 0;
  }
  gfx_state.ta_busy = 0;
  render_stats.ta_time = render_stats.ta_start_time - *(volatile int *)0xffd8000c;
}

void target_passes_install(void)
{
  pass_queue_read_base = 0;
  pass_queue_read = 0;
  pass_queue_write_base = 0;
  pass_queue_write = 0;
  pass_current = 0;
  pass_entry_pool_next = 0;
  code_jump_patch((uint)ta_input_finish,(uint)list_input_end_with_passes);
  return;
}

uint target_list_block_bits(render_target_rec *rt)
{
  uint bits;
  int i;

  bits = 0;
  rt->list_mask = gfx_state.used_lists;
  for (i = 0; i < 5; i++) {
    if ((1 << i) & rt->list_mask) {
      if (i == 0 || i == 2) {
        bits |= 3 << (i * 4);
      }
      else {
        bits |= 1 << (i * 4);
      }
    }
  }
  return bits;
}

void target_bind_texture_core(render_target_rec *rt, int block, int texidx, int width, int height)
{
  tex_slot_rec *slot;
  uint bits;
  int w;
  int h;

  rt->texidx = texidx;
  slot = texidx_slot_get(texidx);
  if (slot == 0) {
    do {
    } while( true );
  }
  rt->slot = slot;
  bits = target_list_block_bits(rt);
  if (width == 0) {
    texidx_size_get(texidx,&w,&h);
  }
  else {
    w = width;
    h = height;
  }
  tex_target_build(&rt->target,rt->slot,bits,block,w,h);
  rt->bg_words[0] = gfx_state.bg_plane[6];
  rt->bg_words[1] = gfx_state.bg_plane[10];
  rt->bg_words[2] = gfx_state.bg_plane[14];
  return;
}

void target_bind_texture(render_target_rec *rt, int texidx)
{
  target_bind_texture_core(rt,0,texidx,0,0);
  return;
}

void target_bind_texture_sized(render_target_rec *rt, int texidx, int width, int height)
{
  target_bind_texture_core(rt,0,texidx,width,height);
  return;
}

void target_rebind_texture(render_target_rec *rt, int texidx)
{
  target_bind_texture_core(rt,rt->target.vram_block,texidx,0,0);
  return;
}

void target_resize(render_target_rec *rt, uint width, uint height)
{
  uint bits;

  bits = target_list_block_bits(rt);
  tex_target_size_set(&rt->target,bits,width,height);
  return;
}

void target_draw_select(render_target_rec *rt)
{
  pass_entry_rec *entry;
  int saved_stack;

  target_current = rt;
  if (rt == 0) {
    bank_record_restore();
    proj_rect_restore();
    screen_unproject_consts_set((int)proj_rect_width,(int)proj_rect_height);
  }
  else {
    bank_record_save();
    proj_rect_save_once();
    entry = target_entry_alloc();
    pass_open_entry = entry;
    if (entry == 0) return;
    bank_ring_open_next();
    entry->target = &rt->target;
    entry->list_mask = rt->list_mask;
    entry->ring_index = bank_ring_index;
    proj_map_to_rect(0.0,0.0,(float)rt->target.width,(float)rt->target.height);
    saved_stack = mtx_stack_selected;
    mtx_stack_select(3);
    proj_rebuild_rect_proj();
    proj_cmds_send();
    mtx_stack_select(saved_stack);
    screen_unproject_consts_set(rt->target.width,rt->target.height);
  }
}

void target_draw_finish(void)
{
  pass_entry_rec *entry;
  render_target_rec *rt;

  entry = pass_open_entry;
  rt = target_current;
  if (pass_open_entry) {
    bank_lists_store_to_ring();
    bank_ring_close(rt->list_mask);
    entry->bg_words[0] = rt->bg_words[0];
    entry->bg_words[1] = rt->bg_words[1];
    entry->bg_words[2] = rt->bg_words[2];
    target_queue_push(entry);
    pass_open_entry = 0;
    target_draw_select(0);
  }
  return;
}

void target_background_words_set(uint word0, uint word1, uint word2)
{
  render_target_rec *rt;

  rt = target_current;
  target_current->bg_words[0] = word0;
  rt->bg_words[1] = word1;
  rt->bg_words[2] = word2;
  return;
}

void target_frame_flags_clear(void)
{
  pass_open_entry = 0;
  bank_record_saved_clear();
  write_ptrs_saved_clear();
  proj_rect_saved_clear();
  return;
}

void target_screen_state_restore(void)
{
  bank_record_restore();
  proj_rect_restore();
  return;
}

void cpu_ol_target_select(tex_target_rec *target, int half)
{
  cpu_ol_target_rec *ol = &cpu_ol_target;

  cpu_ol_param_base = target->bank[half].param_base;
  ol->obj_list_base = target->bank[half].obj_list_base;
  ol->param_start = target->bank[half].field_0xc;
  ol->tile_clip = target->tile_clip;
  ol->block_size_word = target->list_alloc_word;
  return;
}

uint cpu_ol_background_write(uint color)
{
  uint link [1];

  cpu_ol_target.param_write_addr =
       bg_plane_256x128_write_link_out(link,cpu_ol_param_base,cpu_ol_target.param_write_addr,color);
  return link[0];
}

void ta_buffers_reinit(int set)
{
  pvr_reg_write_both(8,1);
  pvr_reg_write_both(8,0);
  pvr_reg_write_both(0x124,gfx_state.ta_bufs[set].ol_base);
  pvr_reg_write_both(300,gfx_state.ta_bufs[set].ol_limit);
  pvr_reg_write_both(0x128,gfx_state.ta_bufs[set].isp_base);
  pvr_reg_write_both(0x130,gfx_state.ta_bufs[set].isp_limit);
  pvr_reg_write_both(0x164,gfx_state.ta_bufs[set].opb_start);
  pvr_reg_write_both(0x144,0x80000000);
  pvr_reg_read(0x144);
}

void ta_continue_into_target(tex_target_rec *target, int half)
{
  ta_target_buf_select = half;
  pvr_reg_write_both(0x13c,target->tile_clip);
  pvr_reg_write_both(0x140,target->list_alloc_word);
  pvr_reg_write_both(0x124,target->bank[half].obj_list_base);
  pvr_reg_write_both(0x160,0x80000000);
  pvr_reg_read(0x160);
  return;
}

void ta_tile_alloc_regs_save(void)
{
  saved_ta_tile_clip = pvr_reg_read(0x13c);
  saved_ta_block_sizes = pvr_reg_read(0x140);
  return;
}

void ta_tile_alloc_regs_restore(void)
{
  pvr_reg_write_both(0x13c,saved_ta_tile_clip);
  pvr_reg_write_both(0x140,saved_ta_block_sizes);
  return;
}

void words_fill_arith(int *dst, uint count, int value, int step)
{
  uint i;

  i = 0;
  for (; i < count; dst = dst + 1, value = value + step, i = i + 1) {
      *dst = value;
  }
  return;
}

void vram64_word_put_advance(uint *addr, uint value)
{
  uint a;

  a = *addr;
  *(uint *)(a | 0xa5000000) = value;
  *addr = a + 4;
  return;
}

void cpu_ol_all_tiles_put(uint value)
{
  char *s = (char *)&cpu_ol_target;
  uint w1;
  uint h1;
  uint i;
  uint j;
  uint x0;
  uint *row;
  uint *p;
  w1 = *(uint *)(s + 0x97c) - 1;
  h1 = *(uint *)(s + 0x980) - 1;
  row = (uint *)(s + 28);
  x0 = 0;
  for (j = 0; j <= h1; j++) {
    p = &row[x0];
    for (i = x0; i <= w1; i++) {
      vram64_word_put_advance(p, value);
      p++;
    }
    row += *(uint *)(s + 0x97c);
  }
}

void cpu_ol_tile_ptrs_build(void)
{
  typedef struct { char pad[0x97c]; int tiles_x; int tiles_y; int tile_count; } S;
  int starts [6];
  int sizes [5];
  int v;
  ((S *)&cpu_ol_target)->tiles_x = ((v = cpu_ol_target.tile_clip) & 0xff) + 1;
  ((S *)&cpu_ol_target)->tiles_y = ((uint)v >> 0x10 & 0xff) + 1;
  ((S *)&cpu_ol_target)->tile_count = ((S *)&cpu_ol_target)->tiles_x * ((S *)&cpu_ol_target)->tiles_y;
  list_block_sizes_from_alloc(sizes,cpu_ol_target.block_size_word);
  list_area_starts_lay_out(starts,cpu_ol_target.obj_list_base,((S *)&cpu_ol_target)->tile_count,sizes);
  cpu_ol_target.param_write_addr = cpu_ol_target.param_start;
  words_fill_arith((int *)((char *)&cpu_ol_target + 0x1c),((S *)&cpu_ol_target)->tile_count,starts[0],sizes[0]);
}

int rt_tex_side_code(int side)
{
  int i;
  for (i = 0; i < 8; i++) {
    if (side <= 1 << (i + 3)) return i;
  }
  return 7;
}

void rt_tex_hw_words_build(tex_slot_rec *slot)
{
  int size_word;
  uint flags;
  size_word = rt_tex_side_code(slot->surface.width) << 3;
  size_word |= rt_tex_side_code(slot->surface.height);
  slot->size_word = size_word;
  flags = slot->surface.format_flags;
  slot->tex_control_word =
       flags << 0x1f | (flags & 8) << 0x1b | slot->surface.pixel_format & 0x38000000 |
       (~flags & 4) << 0x18 | (flags & 0x10) << 0x15 | slot->surface.tex_address >> 3;
}

int bits_set_count(uint value)
{
  int n;

  n = 0;
  for (; 0 < (int)value; value = (int)value >> 1) {
    n = n + (value & 1);
  }
  return n;
}

int target_tex_place(tex_slot_rec *slot, uint width, uint height, uint type)
{
  int status;
  int bits;
  uint group;

  group = type & 0xff00;
  bits = bits_set_count(width);
  bits += bits_set_count(height);
  if (group == 0xb00) {
    status = surface_set_up_and_place(&slot->surface,width,height,type);
    if (status != 0) {
      status = surface_vram_alloc(slot->surface.bytes,&slot->surface);
      if (status == 3) hook_slot_run(3);
    }
  }
  else if (bits != 2) {
    status = surface_set_up_and_place(&slot->surface,width,0x100,type);
    slot->surface.height = height;
  }
  else {
    status = surface_set_up_and_place(&slot->surface,width,height,type);
  }
  return status;
}

int target_texidx_create(int texidx, int width, uint height, uint type)
{
  int slot_no;
  int status;
  tex_slot_rec *slot;

  if (texidx_map[texidx].use_count != 0) return -1;
  texidx_map[texidx].use_count = 1;
  slot_no = texidx_map[texidx].slot_no = texidx_bit_take();
  slot = tex_slot_table + slot_no;
  if (slot->in_use != 0) return -1;
  if (width == 0) {
    slot->status = 0xffffffff;
    slot->in_use = 1;
    return 0;
  }
  status = target_tex_place(slot,width,height,type);
  slot->status = status;
  if (status != 0) return status;
  rt_tex_hw_words_build(slot);
  slot->slot_id = slot_no;
  slot->in_use = 1;
  if (texidx_map[texidx].use_count == 0) return -1;
  return status;
}

int tex_type_to_desc(surface_rec *desc, int width, int height, uint type)
{
  int status = 0;
  uint group = type & 0xff00;
  uint format = type & 0xff;

  if (group == 0x1000 || group == 0x1100) desc->kind = 3;
  else desc->kind = 2;
  if (group == 0x500 || group == 0x600) {
    if (*(int *)(lib_state_ptr + 0x9c) == 3) desc->texel_size_class = 3;
    else desc->texel_size_class = 1;
    desc->pixel_format = 0x28000000;
  }
  else if (group == 0x700 || group == 0x800) {
    if (*(int *)(lib_state_ptr + 0x9c) == 3) desc->texel_size_class = 3;
    else desc->texel_size_class = 1;
    desc->pixel_format = 0x30000000;
  }
  else {
    desc->texel_size_class = 1;
    switch (format) {
    case 0: desc->pixel_format = 0; break;
    case 1: desc->pixel_format = 0x08000000; break;
    case 2: desc->pixel_format = 0x10000000; break;
    case 3: desc->pixel_format = 0x18000000; break;
    case 4: desc->pixel_format = 0x20000000; break;
    default: status = 4; break;
    }
  }
  desc->width = width;
  desc->height = height;
  switch (width) {
  case 8: case 0x10: case 0x20: case 0x40: case 0x80: case 0x100: case 0x200: case 0x400: break;
  default: status = 4;
  }
  switch (height) {
  case 8: case 0x10: case 0x20: case 0x40: case 0x80: case 0x100: case 0x200: case 0x400: break;
  default: status = 4;
  }
  if ((group != 0x900 && group != 0xb00) && (group != 0xd00 && group != 0x500) && (group != 0x700 && width != height)) {
    status = 4;
  }
  desc->format_flags = 0;
  if ((group == 0xf00 || group == 0x200) || (group == 0x400 || group == 0x1100) || (group == 0x600 || group == 0x800)) {
    desc->format_flags |= 1;
  }
  if ((group == 0xe00 || group == 0xf00) || (group == 0x100 || group == 0x200) || group == 0xd00) {
    desc->format_flags |= 4;
  }
  if ((group == 0x300 || group == 0x400) || (group == 0x1000 || group == 0x1100)) {
    desc->format_flags |= 0xc;
  }
  if (group == 0xb00) {
    desc->format_flags |= 0x10;
  }
  if ((group == 0x500 || group == 0x600) || (group == 0x700 || group == 0x800)) {
    desc->format_flags |= 0x24;
  }
  return status;
}

int target_tex_retype(int texidx, uint type)
{
  tex_slot_rec *slot;
  int status;

  slot = texidx_slot_get(texidx);
  status = -1;
  if (slot == 0) return status;
  status = tex_type_to_desc(&slot->surface,slot->surface.width,slot->surface.height,type);
  rt_tex_hw_words_build(slot);
  return status;
}

int texidx_bind_frame_buffer(int texidx)
{
  int status;
  tex_slot_rec *slot;

  if (texidx_map[texidx].use_count == 0) return -1;
  slot = tex_slot_table + texidx_map[texidx].slot_no;
  status = fb_front_as_surface_desc(&slot->surface);
  slot->status = status;
  if (status != 0) return status;
  rt_tex_hw_words_build(slot);
  slot->tex_control_word = slot->tex_control_word | 0x200000;
  return status;
}

void target_texidx_release(int texidx)
{
  int slot_no;
  tex_slot_rec *slot;
  if (texidx_map[texidx].use_count != 0) {
    texidx_map[texidx].use_count--;
    if (texidx_map[texidx].use_count == 0) {
      slot_no = texidx_map[texidx].slot_no;
      slot = tex_slot_table + slot_no;
      if (slot->status == 0) {
        surface_memory_return_by_kind(&slot->surface);
      }
      slot->in_use = 0;
      slot->slot_id = 0xffffffff;
      texidx_bit_clear(slot_no);
      texidx_map[texidx].slot_no = -1;
    }
  }
}

tex_slot_rec * texidx_slot_get(int texidx)
{
  tex_slot_rec *slot;
  if (texidx_map[texidx].use_count == 0) return 0;
  slot = tex_slot_table + texidx_map[texidx].slot_no;
  if (slot->in_use == 0) return 0;
  return slot;
}

void texidx_hw_size_get(int texidx, int *width, int *height)
{
  tex_slot_rec *slot;

  slot = texidx_slot_get(texidx);
  if (slot != 0) {
    *width = 1 << (slot->size_word >> 3 & 7) + 3;
    *height = 1 << (slot->size_word & 7) + 3;
  }
  return;
}

void texidx_size_get(int texidx, int *width, int *height)
{
  tex_slot_rec *slot;

  slot = texidx_slot_get(texidx);
  if (slot != 0) {
    *width = slot->surface.width;
    *height = slot->surface.height;
  }
  return;
}

void pixels_to_argb1555(ushort *out, uchar *pixels, int count)
{
  int i;
  ushort pixel;

  for (i = 0; i < count; i++) {
    pixel = (uchar)(*pixels++ & 0xf8) << 7;
    pixel |= (uchar)(*pixels++ & 0xf8) << 2;
    pixel |= (uchar)(*pixels++ & 0xf8) >> 3;
    pixel |= (uchar)(*pixels++ & 0x80) << 8;
    *out++ = pixel;
  }
  return;
}

void pixels_to_rgb565(ushort *out, uchar *pixels, int count)
{
  int i;
  ushort pixel;

  for (i = 0; i < count; i++) {
    pixel = (uchar)(*pixels++ & 0xf8) << 8;
    pixel |= (uchar)(*pixels++ & 0xfc) << 3;
    pixel |= (uchar)(*pixels++ & 0xf8) >> 3;
    pixels++;
    *out++ = pixel;
  }
  return;
}
