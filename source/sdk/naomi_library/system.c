/*
 * sdk/naomi_library/system.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern video_mode_rec video_mode_table[29]; /* 0x0c211ebc */
extern int pixel_format_by_mode[6]; /* 0x0c212018 */
extern draw_context_preset_words draw_context_presets[48]; /* 0x0c212030 */
extern uint builtin_texture_loaded; /* 0x0c417948 */
extern int frame_pace_vblank_n; /* 0x0c417a18 */
extern float tmu0_usec_per_tick; /* 0x0c417a20 */
extern int gfx_counters_saved[5]; /* 0x0c417a24 */
extern uint stopwatch_start_count; /* 0x0c417a3c */
extern int holly_rev8_or_later; /* 0x0c417a5c */
extern int frame_pixel_format; /* 0x0c417ac8 */
extern int punch_list_share; /* 0x0c417acc */
extern uint render_busy_flag; /* 0x0c41bb00 */
extern uint tex_load_count; /* 0x0c41bb0c */
extern float proj_rect_width; /* 0x0c41c950 */
extern float proj_rect_left; /* 0x0c41c998 */
extern float proj_rect_height; /* 0x0c41ca20 */
extern float proj_rect_top; /* 0x0c41ca24 */
extern bg_plane_colors_rec bg_plane_colors; /* 0x0c32a598 */
extern frame_stats_rec frame_stats; /* 0x0c41794c */
extern frame_surface_pair render_frame_surfaces; /* 0x0c417a40 */
extern draw_context_preset_words draw_context_preset_cur; /* 0x0c417a48 */
extern render_config_block render_config; /* 0x0c417a60 */
extern vertex_buffer_desc_rec vertex_buffer_desc; /* 0x0c418458 */
extern display_state_rec display_state; /* 0x0c41bb38 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void dmac_ch2_and_master_enable(void)
{
  volatile int i;
  volatile int v;
  
  *(volatile int *)0xffa00020 = 0;
  *(volatile int *)0xffa0002c = 0x1201;
  *(volatile int *)0xffa00040 = 0x8201;
  v = *(volatile int *)0xffa00040;
  while (v != 0x8201) {
    for (i = 0; i < 0x10000; i++) ;
    *(volatile int *)0xffa00040 = 0x8201;
    v = *(volatile int *)0xffa00040;
  }
  return;
}

void render_with_busy_flag(void)
{
  render_busy_flag = 1;
  scene_lists_close();
  render_busy_flag = 0;
  frame_lists_send_and_rearm();
  return;
}

void draw_context_preset_apply(int preset)
{
  draw_context_preset_words *e;
  uint w2, w3, w8, w17, w16;
  
  draw_ctx_ptr->change_mask = 0x600608;
  draw_ctx_ptr->list_type = w2 = (e = draw_context_presets + preset)->list_type;
  draw_ctx_ptr->color_format = w3 = e->color_format;
  draw_ctx_ptr->tex_offset_bits = w8 = e->tex_offset_bits;
  draw_ctx_ptr->alpha_on = w17 = e->alpha_on;
  draw_ctx_ptr->gouraud = w16 = e->gouraud;
  draw_context_preset_cur.list_type = w2;
  draw_context_preset_cur.color_format = w3;
  draw_context_preset_cur.tex_offset_bits = w8;
  draw_context_preset_cur.alpha_on = w17;
  draw_context_preset_cur.gouraud = w16;
  context_hw_words_compute(draw_ctx_ptr);
}

void draw_ctx_tex_offset_from_preset(int index)
{
  uint word;
  
  word = draw_ctx_ptr->isp_tsp_word;
  draw_ctx_ptr->tex_offset_bits = draw_context_presets[index].tex_offset_bits;
  word &= 0xfcffffff;
  word |= draw_ctx_ptr->tex_offset_bits << 24;
  draw_ctx_ptr->isp_tsp_word = word;
  return;
}

void draw_ctx_filter_word_set_bit21(uint value)
{
  uint word;
  
  word = draw_ctx_ptr->tsp_word;
  draw_ctx_ptr->tex_filter = value;
  word &= 0xffdfffff;
  word |= draw_ctx_ptr->gouraud << 21;
  draw_ctx_ptr->tsp_word = word;
  return;
}

void draw_ctx_filter_word_set_bit20(uint value)
{
  uint word;
  
  word = draw_ctx_ptr->tsp_word;
  draw_ctx_ptr->tex_filter = value;
  word &= 0xffefffff;
  word |= draw_ctx_ptr->alpha_on << 20;
  draw_ctx_ptr->tsp_word = word;
  return;
}

void draw_ctx_tex_filter_set(int index)
{
  uint word;
  
  word = draw_ctx_ptr->tsp_word;
  draw_ctx_ptr->tex_filter = tex_filter_codes[index];
  word &= 0xffff9fff;
  word |= draw_ctx_ptr->tex_filter << 13;
  draw_ctx_ptr->tsp_word = word;
  return;
}

void strip_context_defaults(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 0x3f3ffff;
  ctx->header_type = 0;
  ctx->list_type = 0;
  ctx->color_format = 1;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 0;
  ctx->z_write_off = 0;
  ctx->src_blend = 8;
  ctx->dst_blend = 6;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  q[4] = 0;
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
  return;
}

void bg_plane_context_defaults(strip_context_rec *ctx)
{
  uint *q;
  ctx->change_mask = 66322431;
  ctx->header_type = 0;
  ctx->list_type = 0;
  ctx->color_format = 0;
  q = &ctx->gouraud;
  ctx->uv16 = 0;
  ctx->depth_compare = 4;
  ctx->cull_mode = 0;
  ctx->field_0x1c = 0;
  ctx->tex_offset_bits = 1;
  ctx->two_volume = 0;
  ctx->z_write_off = 0;
  ctx->src_blend = 10;
  ctx->dst_blend = 11;
  ctx->src_select = 0;
  ctx->dst_select = 0;
  ctx->fog_mode = 2;
  ctx->gouraud = 0;
  q[1] = 0;
  q[2] = 1;
  q[3] = 0;
  q[4] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 4;
  q[8] = 1;
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
  bg_plane_context_store(ctx);
}

void bg_plane_corner_colors(uint top_left, uint top_right, uint bottom_left)
{
  bg_plane_corner_colors_z(top_left,top_right,bottom_left,100000.0);
  return;
}

int ta_list_sizes_set_up_old_holly(int size0, int size1, int size2, int size3)
{
  listbuf_lay_out_four((uint *)&vertex_buffer_desc,size0,size1,size2,size3);
  fb_vram_alloc(&frame_surface_0,0,0,0,0);
  fb_vram_alloc(&frame_surface_1,0,0,0,0);
  fb_pair_install(&frame_surface_0,&frame_surface_1,0,1);
  if (builtin_texture_loaded == 0) {
    tex_builtin_8x8_upload();
    builtin_texture_loaded = 1;
  }
  return 1;
}

void graphics_bring_up(int mode, uint flags, int vblanks, int unused)
{
  unsigned int sr;
  unsigned int sr_unmask;
  video_mode_rec *e;

  video_mode_is_0 = 0;
  if (mode == 0) {
    video_mode_is_0 = 1;
  }
  clip_stretch_on = 0;
  if ((uint)(mode - 0x18) < 4) {
    clip_stretch_on = 1;
  }
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (*(volatile int *)0xa05f689c > 7) holly_rev8_or_later = 1;
  else holly_rev8_or_later = 0;
  dmac_ch2_and_master_enable();
  display_state.dither = 0;
  display_state.anti_alias = 0;
  if ((flags & 0x80) != 0) {
    display_state.dither = 1;
  }
  if ((flags & 0x8000) != 0) {
    display_state.anti_alias = 1;
  }
  frame_pixel_format = pixel_format_by_mode[flags & 0xf];
  lib_start_with_video_out(video_mode_table[mode].video_out);
  if (clip_stretch_on == 1) {
    hook_request_ignored((uint)clip_stretch_step_report,0);
  }
  e = video_mode_table + mode;
  {
    int dither = display_state.dither;
    int anti_alias = display_state.anti_alias;
    display_timing_and_fb_program(e->timing,frame_pixel_format,dither,anti_alias);
  }
  display_state.screen_width = e->width;
  display_state.screen_height = e->height;
  bg_plane_context_defaults(&bg_plane_context);
  strip_context_defaults(&strip_context_main);
  draw_ctx_ptr = &strip_context_main;
  bring_up_word_860c = 0;
  bring_up_word_bb04 = 0xffffffff;
  bring_up_word_bb08 = 0;
  bring_up_word_bb34 = 0;
  tex_load_count = 0;
  bring_up_floats_bb24[0] = 0.0;
  bring_up_floats_bb24[1] = 0.0;
  bring_up_floats_bb24[2] = 0.0;
  bring_up_floats_bb24[3] = 0.0;
  bring_up_words_bb10[0] = 0;
  bring_up_words_bb10[1] = 0;
  bring_up_words_bb10[2] = 0;
  bring_up_words_bb10[3] = 0;
  graphics_work.coord_scale = 1.0;
  graphics_work.field_0x44 = 0;
  proj_map_to_rect(0.0,0.0,(float)display_state.screen_width,
               (float)display_state.screen_height);
  pvr_cull_val_put_abs(0.2);
  { volatile int *q = (volatile int *)0xff000038; q[0] = 0xc; q[1] = 0xc; }
  frame_pace_vblank_n = vblanks;
  frame_vblank_quota_put(vblanks);
  render_config_fill_defaults(0x200000);
  error_handler_pick(0);
  builtin_texture_loaded = 0;
  blend_pair_bits = 0x68;
  __asm__ volatile ("stc sr,%0" : "=r" (sr_unmask));
  sr_unmask = sr_unmask & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr_unmask));
}

void graphics_bring_up_entry(int mode, uint flags, int vblanks)
{
  graphics_bring_up(mode,flags,vblanks,1);
  return;
}

void graphics_bring_up_entry_alt(int mode, uint flags, int vblanks)
{
  graphics_bring_up(mode,flags,vblanks,0);
  return;
}

void frame_pace_vblanks(int vblanks)
{
  frame_pace_vblank_n = vblanks;
  frame_vblank_quota_put(vblanks);
  return;
}

void fog_col_ram_write_word(uint color)
{
  pvr_fog_col_ram_put(color);
  return;
}

void fog_col_vert_write_word(uint color)
{
  pvr_fog_col_vert_put(color);
  return;
}

void fog_table_regs_write_twin(float *table)
{
  pvr_fog_entries_from_floats(table);
  return;
}

void draw_ctx_cull_bits_set(int index)
{
  draw_ctx_ptr->isp_tsp_word = (draw_ctx_ptr->isp_tsp_word & 0xe7ffffff) | cull_mode_bits[index];
  return;
}

void cull_size_threshold_put(float threshold)
{
  pvr_cull_val_put_abs(threshold);
  return;
}

void vo_border_write_opaque(uint color)
{
  pvr_vo_border_col_put(color | 0xff000000);
  return;
}

void gfx_counters_snapshot(void)
{
  int i;
  int *dst;
  int *src;
  
  src = (int *)list_write_ptrs;
  dst = gfx_counters_saved;
  i = 0;
  do {
    *dst = (*src);
    i = 1 + i;
    src = 1 + src;
    dst = 1 + dst;
  } while (i < 5);
  return;
}

void gfx_counters_since_snapshot(int *out)
{
  int now;
  int kept;
  int i;
  int *saved;
  int *cur;
  
  cur = (int *)list_write_ptrs;
  saved = gfx_counters_saved;
  i = 0;
  do {
    now = *cur;
    cur = cur + 1;
    kept = *saved;
    saved = 1 + saved;
    *out = now - kept;
    i = 1 + i;
    out = out + 1;
  } while (i < 5);
  return;
}

void clip_rect_per_list_bits(int x_min, int y_min, int x_max, int y_max, uint lists)
{
  int list;
  
  list = 0;
  do {
    if ((lists & 1) != 0) {
      list_tile_clip_param_write(&vertex_buffer_desc,list,x_min,y_min,x_max,y_max);
    }
    lists = (int)lists >> 1;
    list = list + 1;
  } while (list < 5);
  return;
}

void poly_header_bits16_set(uint value)
{
  value &= 3;
  if (value == 1) value = 0;
  poly_header_bits16 = value;
  return;
}

void pcw_user_clip_mode_put(int unused, int mode)
{
  *(uint *)vertex_buffer_desc.header = mode << 0x10 | 0x800000;
  return;
}

void display_flag_bits_reapply(uint flags)
{
  display_state_rec *ds;
  display_state.dither = 0;
  if (flags & 0x80) display_state.dither = 1;
  display_state.anti_alias = 0;
  if (flags & 0x8000) display_state.anti_alias = 1;
  ds = &display_state;
  fb_rebuild_dither_aa((int)ds->dither,(int)ds->anti_alias);
  bg_plane_corner_colors(bg_plane_colors.top_left,bg_plane_colors.top_right,
               bg_plane_colors.bottom_left);
  proj_map_to_rect(proj_rect_left,proj_rect_top,proj_rect_width,proj_rect_height);
}

int beam_line_read(void)
{
  int line [1];
  
  spg_status_display_line(line);
  return line[0];
}

void event_hook_slot0_put(uint handler)
{
  render_done_hook_put(handler,0);
  return;
}

void event_hook_slot5_put(uint handler)
{
  slot5_hook_put(handler,0);
  return;
}

void event_hook_slot1_put(uint handler)
{
  vblank_hook_put(handler,0);
  return;
}

void event_hook_slot2_put(uint handler)
{
  hblank_hook_put(handler,0);
  return;
}

void hblank_int_reg_write(int line)
{
  spg_hblank_line_write(line);
  return;
}

void event_hook_slot3_put(uint handler)
{
  surface_alloc_fail_hook_put(handler,0);
  return;
}

void event_hook_slot4_put(uint handler)
{
  strip_hazard_hook_put(handler,0);
  return;
}

void render_config_fill_defaults(uint texmem_size)
{
  render_config.block_size = 0x60;
  render_config.flags = 1;
  render_frame_surfaces.surface0 = &frame_surface_0;
  render_frame_surfaces.surface1 = &frame_surface_1;
  render_config.frame_surfaces = &render_frame_surfaces;
  render_config.frame_buffer_count = 2;
  render_config.field_0x10 = 0x280;
  render_config.field_0x14 = 0;
  render_config.field_0x18 = 0;
  render_config.texmem_size = texmem_size;
  render_config.vertex_buf_desc = (uint)&vertex_buffer_desc;
  render_config.vertex_area_base = 0xacc00000;
  render_config.vertex_area_size = 0x100000;
  render_config.list_shares[0] = 70.0;
  render_config.list_shares[1] = 5.0;
  render_config.list_shares[2] = 25.0;
  render_config.list_shares[3] = 0.0;
  render_config.list_shares[4] = 0.0;
  render_config.bypass_list = 0;
  render_frames_in_flight = 3;
  punch_list_share = 0;
}

void render_config_texmem_size_put(uint texmem_size)
{
  render_config.texmem_size = texmem_size;
  return;
}

void render_pipeline_two_frames(void)
{
  if (holly_rev8_or_later == 1) {
    render_config.bypass_list = 0x20;
    render_config.flags = render_config.flags | 0x10;
    render_frames_in_flight = 2;
  }
  else {
    render_pipeline_choose(1,0x20);
  }
  return;
}

void render_pipeline_three_frames(void)
{
  if (holly_rev8_or_later == 1) {
    render_config.bypass_list = 0;
    render_config.flags = render_config.flags & 0xffffffef;
    render_frames_in_flight = 3;
  }
  else {
    render_pipeline_choose(0,0);
  }
  return;
}

void render_pipeline_list_bypass(int list)
{
  uint bypass;
  bypass = 0x20;
  switch (list) {
  case 0: bypass = 0x20; break;
  case 2: bypass = 0x22; break;
  case 4: bypass = 0x24; break;
  }
  if (holly_rev8_or_later == 1) {
    render_config.bypass_list = bypass;
  }
  else {
    render_pipeline_choose(1,bypass);
  }
}

void render_config_flag2_on(int value)
{
  render_config.flags |= 2;
  render_config.field_0x10 = 0x280;
  render_config.field_0x14 = value;
  render_config.field_0x18 = frame_pixel_format;
  return;
}

void render_config_flag2_off(void)
{
  render_config.flags &= ~2;
  return;
}

void stall_limits_vertex_only(void)
{
  frame_stall_limits_put(10,0);
  return;
}

void stall_limits_vertex_and_render(void)
{
  frame_stall_limits_put(10,10);
  return;
}

void punch_list_share_set(int share)
{
  punch_list_share = share;
  return;
}

void ta_param_base_put(uint addr)
{
  if (holly_rev8_or_later == 1) {
    render_config.vertex_area_base = addr | 0xa0000000;
  }
  else {
    vertex_area_base_size_put(addr | 0xa0000000,0);
  }
  return;
}

int ta_list_sizes_apply(int size0, int size1, int size2, int size3)
{
  render_config_block *cfg;
  int acc;
  int total;
  int r;

  cfg = &render_config;
  acc = punch_list_share;
  if (render_frames_in_flight == 3) {
    total = (size0 + size1 + size2 + size3 + acc) * 2;
  }
  else if (cfg->bypass_list == 0x20) {
    total = size1 + size2 + size3 + acc;
  }
  else if (cfg->bypass_list == 0x22) {
    total = size0 + size1 + size3 + acc;
  }
  else if (cfg->bypass_list == 0x24) {
    total = size0 + size1 + size2 + size3;
  }
  cfg->vertex_area_size = total;
  if (render_frames_in_flight == 3) {
    total = total / 2;
    cfg->list_shares[0] = (float)size0 * 100.0 / (float)total;
    cfg->list_shares[1] = (float)size1 * 100.0 / (float)total;
    cfg->list_shares[2] = (float)size2 * 100.0 / (float)total;
    cfg->list_shares[3] = (float)size3 * 100.0 / (float)total;
    cfg->list_shares[4] = (float)acc * 100.0 / (float)total;
  }
  else if (cfg->bypass_list == 0x20) {
    cfg->list_shares[0] = 0.0;
    cfg->list_shares[1] = (float)size1 * 100.0 / (float)total;
    cfg->list_shares[2] = (float)size2 * 100.0 / (float)total;
    cfg->list_shares[3] = (float)size3 * 100.0 / (float)total;
    cfg->list_shares[4] = (float)acc * 100.0 / (float)total;
  }
  else if (cfg->bypass_list == 0x22) {
    cfg->list_shares[0] = (float)size0 * 100.0 / (float)total;
    cfg->list_shares[1] = (float)size1 * 100.0 / (float)total;
    cfg->list_shares[2] = 0.0;
    cfg->list_shares[3] = (float)size3 * 100.0 / (float)total;
    cfg->list_shares[4] = (float)acc * 100.0 / (float)total;
  }
  else if (cfg->bypass_list == 0x24) {
    cfg->list_shares[0] = (float)size0 * 100.0 / (float)total;
    cfg->list_shares[1] = (float)size1 * 100.0 / (float)total;
    cfg->list_shares[2] = (float)size2 * 100.0 / (float)total;
    cfg->list_shares[3] = (float)size3 * 100.0 / (float)total;
    cfg->list_shares[4] = 0.0;
  }
  r = render_buffers_lay_out(&render_config);
  if (r != 0) {
    __asm__ volatile ("trapa #0x41");
  }
  *(uint *)vertex_buffer_desc.header = 0x800000;
  if (builtin_texture_loaded == 0) {
    tex_builtin_8x8_upload();
    builtin_texture_loaded = 1;
  }
  return r;
}

void ta_list_sizes_set_up(void)
{
  if (holly_rev8_or_later == 1) ((int(*)())ta_list_sizes_apply)(); else ((int(*)())ta_list_sizes_set_up_old_holly)();
}

void render_config_flag20_off(void)
{
  render_config.flags &= ~0x100000;
  return;
}

void render_config_flag20_on(void)
{
  render_config.flags |= 0x100000;
  return;
}

void tmu0_restart_usec_scale(void)
{
  unsigned int sr;
  char tstr;
  unsigned int sr_unmask;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  tstr = *(char *)0xffd80004;
  *(volatile byte *)0xffd80000 = 1;
  *(volatile ushort *)0xffd80010 = 2;
  *(volatile uint *)0xffd80008 = 0xffffffff;
  *(volatile uint *)0xffd8000c = 0xffffffff;
  *(volatile byte *)0xffd80004 = tstr | 1;
  tmu0_usec_per_tick = 1.28;
  __asm__ volatile ("stc sr,%0" : "=r" (sr_unmask));
  sr_unmask = sr_unmask & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr_unmask));
}

void tmu0_restart_at_rate(uint rate)
{
  unsigned int sr;
  char tstr;
  unsigned int sr_unmask;
  
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | 0xe0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  if (rate > 4) {
    rate = 2;
  }
  tstr = *(char *)0xffd80004;
  *(volatile byte *)0xffd80000 = 1;
  *(volatile ushort *)0xffd80010 = rate;
  *(volatile uint *)0xffd80008 = 0xffffffff;
  *(volatile uint *)0xffd8000c = 0xffffffff;
  *(volatile byte *)0xffd80004 = tstr | 1;
  tmu0_usec_per_tick = tmu_usec_per_tick[rate];
  __asm__ volatile ("stc sr,%0" : "=r" (sr_unmask));
  sr_unmask = sr_unmask & 0xffffff0f;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr_unmask));
}

void frame_stats_capture(void)
{
  render_stats_read(&frame_stats);
  return;
}

int frame_stats_vertex_input_usec(void)
{
  return (int)(tmu0_usec_per_tick * (float)frame_stats.vertex_input_ticks);
}

int frame_stats_drawing_usec(void)
{
  return (int)(tmu0_usec_per_tick * (float)frame_stats.drawing_ticks);
}

int frame_stats_ta_param_used(void)
{
  int r; r = frame_stats.ta_param_start; r -= frame_stats.ta_param_write_pos; if (r < 0) r = -r; return r;
}

int frame_stats_ta_param_area_size(void)
{
  int r = frame_stats.ta_param_start;
  r -= frame_stats.ta_param_end;
  if (r < 0) r = -r;
  return r;
}

void frame_buffer_read_words(uint *out)
{
  uint *pixels;
  int line_words;
  int x;
  int y;
  
  line_words = display_state.screen_width;
  pixels = (uint *)(frame_stats.shown_buffer_addr + 0xa5000000);
  if (frame_pixel_format <= 3) {
    line_words *= 2;
  }
  else if (frame_pixel_format <= 4) {
    line_words *= 3;
  }
  else if (frame_pixel_format <= 5) {
    line_words *= 4;
  }
  else {
    for (;;) ;
  }
  line_words /= 4;
  for (y = 0; y < display_state.screen_height; y++) {
    for (x = 0; x < line_words; x++) {
      *out++ = *pixels++;
    }
  }
  return;
}

void frame_buffer_read_fifth(int out)
{
  ushort *pixels;
  ushort *dst;
  int line_shorts;
  int x;
  int y;
  
  line_shorts = display_state.screen_width;
  pixels = (ushort *)(frame_stats.shown_buffer_addr + 0xa5000000);
  if (frame_pixel_format <= 3) {
    line_shorts *= 2;
  }
  else if (frame_pixel_format <= 4) {
    line_shorts *= 3;
  }
  else if (frame_pixel_format <= 5) {
    line_shorts *= 4;
  }
  else {
    for (;;) ;
  }
  line_shorts /= 2;
  dst = (ushort *)(out + 0x1000);
  for (y = 0; y < display_state.screen_height; y += 5) {
    for (x = 0; x < line_shorts; x += 5) {
      *dst++ = *pixels;
      pixels += 5;
    }
    pixels += 0xa00;
  }
  return;
}

uint tmu0_count_now(void)
{
  uint count;
  
  count = tmu0_count_reg;
  return count;
}

int tmu0_ticks_as_usec(int ticks)
{
  return (int)((float)ticks * tmu0_usec_per_tick);
}

int down_count_elapsed(int earlier, int later)
{
  return earlier - later;
}

void stopwatch_start(void)
{
  stopwatch_start_count = tmu0_count_now();
  return;
}

uint stopwatch_usec(void)
{
  int count;
  
  count = tmu0_count_now();
  count = down_count_elapsed(stopwatch_start_count,count);
  count = tmu0_ticks_as_usec(count);
  return count;
}

void stopwatch_usec_dropped(void)
{
  int count;
  
  count = tmu0_count_now();
  count = down_count_elapsed(stopwatch_start_count,count);
  tmu0_ticks_as_usec(count);
  return;
}

void fog_clamp_regs_write(uint max_color, uint min_color)
{
  pvr_fog_clamp_pair_put(max_color,min_color);
  return;
}

void blend_factor_pair_keep(uint factor_a, uint factor_b)
{
  blend_pair_bits = ((~factor_a & 7) << 5) + ((~factor_b & 7) << 2);
}

void error_border_and_hang(int code)
{
  uint color;

  switch (code) {
  case 1:
    color = 0xff0000ff;
    break;
  case 2:
    color = 0xffff0000;
    break;
  case 3:
    color = 0xff00ff00;
    break;
  case 4:
    color = 0xffff00ff;
    break;
  }
  pvr_vo_border_col_put(color);
  for (;;) {
  }
}

void error_return_quietly(void)
{
  return;
}

void error_handler_pick(int mode)
{
  if (mode == 0) error_hook_put((uint)error_border_and_hang);
  else error_hook_put((uint)error_return_quietly);
}

void modvol_lists_merge_into_opaque(void)
{
  region_modvol_lists_merge(1);
  return;
}

void pvr_reg_811c_put(uint value)
{
  *(volatile uint *)0xa05f811c = value;
  return;
}

int vertex_area_bytes_get(void)
{
  return render_config.vertex_area_size << 2;
}

uint frame_stats_render_stalls_get(void)
{
  return frame_stats.render_stalls;
}

uint frame_stats_vertex_stalls_get(void)
{
  return frame_stats.vertex_stalls;
}

void stall_limits_set(int vertex_limit, int render_limit)
{
  frame_stall_limits_put(vertex_limit,render_limit);
  return;
}

void ta_param_area_free(void)
{
  ta_param_area_release((uint)&vertex_buffer_desc);
  return;
}

void bg_plane_corner_colors_z(uint top_left, uint top_right, uint bottom_left, float depth)
{
  typedef struct { uint cmd; float x; float y; float z; uint color_a; int pad0; uint color_b; int pad1; } quad_vtx;
  quad_vtx v[3];
  quad_vtx *verts[3];
  depth = 1.0 / depth;
  bg_plane_colors.top_left = top_left;
  bg_plane_colors.top_right = top_right;
  bg_plane_colors.bottom_left = bottom_left;
  v[0].cmd = 0xe0000000;
  v[0].x = 0.0;
  v[0].y = 0.0;
  v[0].z = depth;
  v[0].color_a = top_left;
  v[0].color_b = top_left;
  v[1].cmd = 0xe0000000;
  if (display_state.anti_alias != 0) v[1].x = display_state.screen_width * 2;
  else v[1].x = display_state.screen_width;
  v[1].y = 0.0;
  v[1].z = depth;
  v[1].color_a = top_right;
  v[1].color_b = top_right;
  v[2].cmd = 0xf0000000;
  v[2].x = 0.0;
  v[2].y = display_state.screen_height;
  v[2].z = depth;
  v[2].color_a = bottom_left;
  v[2].color_b = bottom_left;
  verts[0] = &v[0];
  verts[1] = &v[1];
  verts[2] = &v[2];
  bg_plane_vertices_by_type(verts,0,0x20);
}

void presort_flag_flip_if_equal(unsigned char value)
{
  ((int (*)(int))region_presort_flip_on_match)(value);
}

int video_reconfigure(int mode, uint flags)
{
  int pixel_format;
  int r;
  video_mode_rec *e;
  
  video_mode_is_0 = 0;
  if (mode == 0) video_mode_is_0 = 1;
  clip_stretch_on = 0;
  if (mode - 0x18U < 4) clip_stretch_on = 1;
  display_blank_and_quiesce();
  display_state.dither = 0;
  display_state.anti_alias = 0;
  if (flags & 0x80) display_state.dither = 1;
  if (flags & 0x8000) display_state.anti_alias = 1;
  pixel_format = pixel_format_by_mode[flags & 0xf];
  if (clip_stretch_on == 1) {
    hook_request_ignored((uint)clip_stretch_step_report,0);
  }
  e = video_mode_table + mode;
  { int dither = display_state.dither; int anti_alias = display_state.anti_alias; display_timing_and_fb_program(e->timing,pixel_format,dither,anti_alias); }
  display_state.screen_width = e->width;
  display_state.screen_height = e->height;
  r = render_buffers_lay_out(&render_config);
  tex_builtin_8x8_upload();
  builtin_texture_loaded = 1;
  bg_plane_corner_colors(bg_plane_colors.top_left,bg_plane_colors.top_right,
               bg_plane_colors.bottom_left);
  proj_map_to_rect(0.0,0.0,(float)(int)display_state.screen_width,
               (float)(int)display_state.screen_height);
  return r;
}
