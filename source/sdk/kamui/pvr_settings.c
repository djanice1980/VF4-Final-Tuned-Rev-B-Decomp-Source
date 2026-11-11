/*
 * sdk/kamui/pvr_settings.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern void *bg_plane_loaders[18]; /* 0x0c32abe8 */

int fb_w_ctrl_alpha_cutoff(int cutoff)
{
  int v;
  uint w;
  v = cutoff & -(cutoff >= 0);
  if ((v & 0x7fffff00) != 0) {
    v = 0xff;
  }
  gfx_state.alpha_cutoff = v;
  w = gfx_state.write_ctrl_word & 0xff00ffff | (v & 0xff) << 15;
  gfx_state.write_ctrl_word = w;
  pvr_reg_write_both(0x48,w);
  return 0;
}

int pvr_cull_val_put_abs(float value)
{
  union { float f; uint i; } u;
  u.f = value;
  pvr_reg_write_both(0x78,u.i & 0x7fffffff);
  return 0;
}

int pvr_fog_clamp_pair_put(uint max, uint min)
{
  pvr_reg_write_both(0xbc,max);
  pvr_reg_write_both(0xc0,min);
  return 0;
}

int pvr_fog_col_ram_put(uint color)
{
  pvr_reg_write_both(0xb0,color);
  return 0;
}

int pvr_fog_col_vert_put(uint color)
{
  pvr_reg_write_both(0xb4,color);
  return 0;
}

int pvr_fog_entries_from_floats(float *table)
{
  uint prev;
  uint t;
  int i;
  int o;
  
  i = 0x80;
  prev = (int)(table[i - 1] * 255.0) & 0xff;
  do {
    i = i + -1;
    t = prev;
    prev = (int)(table[i] * 255.0) & 0xff;
    o = (t | prev << 8) & 0xffff;
    pvr_reg_write_both(i * 4 + 0x200,o);
  } while (i != 0);
  return 0;
}

int pvr_fog_coeff_put(uint coeff)
{
  pvr_reg_write_both(0xb8,coeff);
  return 0;
}

int pvr_pal_ram_ctrl_put(uint format)
{
  if ((format & 0xfffffffc) == 0) {
    gfx_state.pal_format = format;
    pvr_reg_write_both(0x108,format);
  }
  return 0;
}

int pvr_pal_ram_load_all(uint *words)
{
  int i;
  int n = 0x400;
  for (i = 0; i < n; i++)
    pvr_reg_write_both(i * 4 + 0x1000,words[i]);
  return 0;
}

int pvr_pal_ram_load_range(int first, int count, uint *words)
{
  int e;
  int i;
  
  e = first + count;
  e = e > 0x400 ? 0x3ff : e;
  for (i = first; i < e; i++) {
    pvr_reg_write_both(i * 4 + 0x1000,words[i]);
  }
  return 0;
}

int pvr_vo_border_col_put(uint color)
{
  pvr_reg_write_both(0x40,color);
  return 0;
}

int bg_plane_context_store(strip_context_rec *ctx)
{
  gfx_state.bg_face_color = (int)(ctx->face_color[0] * 255.0) << 0x18 |
                 (int)(ctx->face_color[1] * 255.0) << 0x10 |
                 (int)(ctx->face_color[2] * 255.0) << 8 |
                 (int)(ctx->face_color[3] * 255.0);
  gfx_state.bg_offset_color =
       (int)(ctx->offset_color[0] * 255.0) << 0x18 |
       (int)(ctx->offset_color[1] * 255.0) << 0x10 |
       (int)(ctx->offset_color[2] * 255.0) << 8 | (int)(ctx->offset_color[3] * 255.0);
  gfx_state.bg_isp_word = ctx->isp_tsp_word;
  gfx_state.bg_tsp_word = ctx->tsp_word;
  gfx_state.bg_tex_word = ctx->tex_word;
  return 0;
}

int bg_plane_vertices_by_type(void *vertex_ptrs, int type, int unused)
{
  (*(code *)bg_plane_loaders[type])();
  gfx_state.bg_plane[0] = gfx_state.bg_plane[0] & 0xfff00000;
  return;
}

int region_presort_flip_on_match(int flag)
{
  int r;
  int v = gfx_state.presort_flag;
  if (v == flag) {
    gfx_state.region_rebuild = 3;
    if (v == 1) {
      gfx_state.presort_flag = 0;
    }
    else {
      gfx_state.presort_flag = 1;
    }
  }
  return r;
}

int tile_limit_set(int x, int y)
{
  gfx_state.tile_limit_on = 1;
  gfx_state.tile_limit_x = x;
  gfx_state.tile_limit_y = y;
  gfx_state.region_rebuild = 3;
  return 0;
}

int fb_clip_rect_store(uint x0, uint y0, uint x1, uint y1)
{
  if ((int)x0 > (int)x1) return 0xdddd;
  if ((int)y0 > (int)y1) return 0xdddd;
  if (0x7ff < (int)x1) {
    x1 = 0x7ff;
  }
  if (0x7ff < (int)y1) {
    y1 = 0x7ff;
  }
  if (gfx_state.fb_format == 4) {
    x0 = x0 + 1 & 0xfffffffe;
    y0 = y0 + 1 & 0xfffffffe;
    x1 = x1 & 0xfffffffe;
    y1 = y1 & 0xfffffffe;
  }
  x0 = x0 & 0x7ff | (x1 & 0x7ff) << 0x10;
  gfx_state.x_clip_word = x0;
  x0 = y0 & 0x7ff | (y1 & 0x7ff) << 0x10;
  gfx_state.y_clip_word = x0;
  return 0;
}

int fb_top_lines_skip_set(int mode, int *lines_out)
{
  int t;
  uint v;
  uint m;

  if (gfx_state.top_skip_mode == 1) {
    t = gfx_state.top_skip_lines * gfx_state.line_stride_word * 8;
    gfx_state.field_read_offsets[0] -= t;
    gfx_state.field_read_offsets[1] -= t;
  }
  switch (mode) {
  case 0:
    gfx_state.top_skip_lines = 0;
    break;
  case 1:
    if (gfx_state.display_mode % 2 == 1 || (gfx_state.display_mode & 0x20) != 0) {
      gfx_state.top_skip_lines = 0x10;
    }
    else {
      gfx_state.top_skip_lines = 8;
    }
    t = gfx_state.top_skip_lines * gfx_state.line_stride_word * 8;
    gfx_state.field_read_offsets[0] += t;
    gfx_state.field_read_offsets[1] += t;
    break;
  }
  m = gfx_state.read_size_word & 0xfff003ff;
  v = gfx_state.read_height;
  if (gfx_state.top_skip_lines < gfx_state.vo_offset_y) v -= gfx_state.vo_offset_y;
  else v -= gfx_state.top_skip_lines;
  if (v + gfx_state.vo_offset_y > gfx_state.read_height) v = gfx_state.read_height - gfx_state.top_skip_lines;
  if (gfx_state.read_height < v) gfx_state.read_height = v;
  gfx_state.read_size_word = (v & 0x3ff) << 10 | m;
  pvr_reg_write_both(0x44,0);
  pvr_reg_write_both(0x5c,gfx_state.read_size_word);
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word);
  gfx_state.top_skip_mode = mode;
  if (lines_out != (int *)0x0) {
    *lines_out = gfx_state.top_skip_lines;
  }
  return 0;
}

int return_0x78_0c141f80(void)
{
  return 0x78;
}

int ta_isp_softreset_after_drain(void)
{
  render_drain();
  pvr_reset_pulse();
  return 0;
}

int region_modvol_lists_merge(uint mode)
{
  uint status;
  
  status = 0xb;
  if ((mode & 1) != 0) {
    gfx_state.modvol_merge = 1;
    gfx_state.modvol_merge_mode = mode;
    status = 0;
  }
  else {
    gfx_state.modvol_merge = 0;
  }
  gfx_state.region_rebuild = 3;
  return status;
}

int frame_vblank_quota_put(int count)
{
  gfx_state.vsync_count = count + -1;
  return 0;
}

int spg_latched_beam_xy_read(uint *x, uint *y)
{
  uint pos;
  
  pos = pvr_reg_read(0xc4);
  *x = pos & 0x3ff;
  *y = pos >> 0x10 & 0x3ff;
  return 0;
}

int vram_upload_dma_idle(void)
{
  int done;
  
  if (gfx_state.upload_wait == 1) {
    return 0;
  }
  done = ticket_done_poll(gfx_state.upload_ticket);
  if (done == 1) {
    return 0;
  }
  return 0x11;
}

void halt_magenta_unless(int ok)
{
  if (ok == 0) {
    *(volatile uint *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
  return;
}

int pvr_text_ctrl_stride_put(uint width)
{
  uint ctrl;
  int stride;
  
  stride = (int)(width & 0x3e0) >> 5;
  ctrl = pvr_reg_read(0xe4);
  pvr_reg_write_both(0xe4,ctrl & 0xffffffe0 | stride);
  return 0;
}
