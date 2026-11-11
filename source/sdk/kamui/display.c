/*
 * sdk/kamui/display.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_elan_layer_reset
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void fb_regs_reload_blanked(void)
{
  pvr_reg_write_both(0x68,gfx_state.x_clip_word);
  pvr_reg_write_both(0x6c,gfx_state.y_clip_word);
  pvr_reg_write_both(0x5c,gfx_state.read_size_word);
  pvr_reg_write_both(0x4c,gfx_state.line_stride_word);
  pvr_reg_write_both(0x118,gfx_state.y_coeff_word);
  pvr_reg_write_both(0xf4,gfx_state.scaler_word);
  pvr_reg_write_both(0x48,gfx_state.write_ctrl_word);
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
  pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word | 8);
  return;
}

void vblank_tick_await_next(void)
{
  int v = gfx_state.vblank_ticks;
  while (v == gfx_state.vblank_ticks);
}

int lib_start_with_video_out(uint video_out)
{
  elan_layer_reset();
  lib_state_set_up_from_cfg();
  if (!(video_out < 3)) {
    return 2;
  }
  gfx_state.video_out = cfg_setting_or_default("KAMUI","VideoMode",video_out & 3);
  chips_pool_irq_set_up();
  return 0;
}

int display_timing_and_fb_program(uint requested_mode, uint format, uint dither, uint anti_alias)
{
  uint mode;
  int ret;
  int w;
  int h;
  int interlace;
  int f_aa;
  int t;
  int f_dither;

  ret = 0;
  render_drain();
  mode = cfg_setting_or_default("KAMUI","DisplayMode",requested_mode);
  anti_alias = cfg_setting_or_default("KAMUI","AntiAlias",anti_alias);
  dither = cfg_setting_or_default("KAMUI","Dither",dither);
  gfx_state.video_out = mode & 3;
  switch (format) {
  case 0:
  case 1:
  case 3:
  case 4:
  case 5:
    break;
  default:
    ret = 1;
  }
  t = anti_alias == 0;
  f_aa = t == 0;
  t = dither == 0;
  f_dither = t == 0;
  if (ret != 0) {
    return ret;
  }
  gfx_state.display_mode = mode;
  gfx_state.fb_format = format;
  gfx_state.dither = f_dither;
  gfx_state.anti_alias = f_aa;
  if (gfx_state.display_mode == 1) {
    interlace = 0;
    w = 0x280;
    h = 0x1e0;
    gfx_state.display_lines = 0x1e0;
  }
  else {
    if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) != 0) interlace = 1;
    else interlace = 0;
    if ((gfx_state.display_mode & 0x10) != 0) w = 0x280; else w = 0x140;
    if ((gfx_state.display_mode & 0x20) != 0) h = 0x1e0; else h = 0xf0;
    gfx_state.display_lines = 0xf0;
  }
  switch (gfx_state.video_out) {
  case 1:
    ret = 0;
    spg_timing_vga();
    break;
  case 0:
    ret = 0;
    spg_timing_ntsc();
    break;
  case 2:
    ret = 0;
    spg_timing_pal();
    break;
  default:
    ret = 1;
  }
  fb_layout_compute(w,h,format,interlace,f_dither,f_aa);
  fb_regs_reload_blanked();
  return ret;
}

int vo_blank_per_chip(uint chips)
{
  if (chips == 1) {
    pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
    pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word | 8);
    pvr_reg_write_both(0x44,gfx_state.read_ctrl_word);
  }
  else if ((chips & 0xc0000000) != 0) {
    if ((chips & 0x40000000) != 0) {
      pvr_reg_write_first(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
      pvr_reg_write_first(0xe8,gfx_state.vo_ctrl_word | 8);
      pvr_reg_write_first(0x44,gfx_state.read_ctrl_word);
    }
    else {
      pvr_reg_write_first(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
      pvr_reg_write_first(0xe8,gfx_state.vo_ctrl_word);
      pvr_reg_write_first(0x44,gfx_state.read_ctrl_word);
    }
    if ((int)chips < 0) {
      pvr_reg_write_second(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
      pvr_reg_write_second(0xe8,gfx_state.vo_ctrl_word | 8);
      pvr_reg_write_second(0x44,gfx_state.read_ctrl_word);
    }
    else {
      pvr_reg_write_second(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
      pvr_reg_write_second(0xe8,gfx_state.vo_ctrl_word);
      pvr_reg_write_second(0x44,gfx_state.read_ctrl_word);
    }
  }
  else {
    pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
    pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word);
    pvr_reg_write_both(0x44,gfx_state.read_ctrl_word);
  }
  return 0;
}

int fb_rebuild_dither_aa(int dither, int anti_alias)
{
  int pixel_bits[6];
  int w;
  int dither_on;
  int t;
  int aa_on;
  int interlace;
  int h;

  pixel_bits[0] = 0x10;
  pixel_bits[1] = 0x10;
  pixel_bits[2] = 0x10;
  pixel_bits[3] = 0x10;
  pixel_bits[4] = 0x18;
  pixel_bits[5] = 0x20;
  render_drain();
  t = anti_alias == 0;
  aa_on = t == 0;
  t = dither == 0;
  dither_on = t == 0;
  gfx_state.dither = dither_on;
  gfx_state.anti_alias = aa_on;
  if (gfx_state.display_mode == 1) {
    w = 0x280;
    interlace = 0;
    h = 0x1e0;
  }
  else {
    if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) != 0) {
      interlace = 1;
    }
    else {
      interlace = 0;
    }
    if ((gfx_state.display_mode & 0x10) != 0) {
      w = 0x280;
    }
    else {
      w = 0x140;
    }
    h = (gfx_state.display_mode & 0x20) != 0 ? 0x1e0 : 0xf0;
  }
  vblank_tick_await_next();
  fb_layout_compute(w,h,gfx_state.fb_format,interlace,dither_on,aa_on);
  fb_regs_reload_blanked();
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
  pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word);
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word);
  pvr_reset_pulse();
  ta_buffers_lay_out(gfx_state.area_offsets[0].region >> 7,gfx_state.area_offsets[0].objlist >> 7,
               gfx_state.area_offsets[0].isp >> 7,gfx_state.area_offsets[0].tsp >> 7,
               gfx_state.list_ctrl, 0);
  gfx_state.display_regs_pending = 1;
  vblank_tick_await_next();
  return 0;
}

int display_blank_and_quiesce(void)
{
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
  pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word | 8);
  render_drain();
  pvr_reset_pulse();
  pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
  pvr_reg_write_both(0xe8,gfx_state.vo_ctrl_word | 8);
  vblank_tick_await_next();
  gfx_state.display_regs_pending = 1;
  return 0;
}

int vo_start_offset_apply(int x, int y)
{
  int r = 0;
  uint kept_bits;
  uint lines;

  if ((10 < x + gfx_state.vo_start_x) && (gfx_state.vo_offset_x != x)) {
    gfx_state.vo_offset_x = x;
    gfx_state.vo_startx_word = gfx_state.vo_start_x + gfx_state.vo_offset_x;
    pvr_reg_write_both(0xec,gfx_state.vo_startx_word);
  }
  if ((0 < gfx_state.vo_start_y1 + y) || (0 < gfx_state.vo_start_y2 + y)) {
  if (gfx_state.vo_offset_y != y) {
    gfx_state.vo_offset_y = y;
    gfx_state.vo_starty_word = gfx_state.vo_start_y1 + gfx_state.vo_offset_y | (gfx_state.vo_start_y2 + gfx_state.vo_offset_y) << 16;
    pvr_reg_write_both(0xf0,gfx_state.vo_starty_word);
    if (0 < y) {
      kept_bits = gfx_state.read_size_word & 0xfff003ff;
      lines = gfx_state.read_height;
      if (gfx_state.top_skip_lines < gfx_state.vo_offset_y) lines -= gfx_state.vo_offset_y;
      else lines -= gfx_state.top_skip_lines;
      if (lines + gfx_state.vo_offset_y > gfx_state.read_height) lines = gfx_state.read_height - gfx_state.top_skip_lines;
      if (gfx_state.read_height < lines) gfx_state.read_height = lines;
      gfx_state.read_size_word = (lines & 0x3ff) << 10 | kept_bits;
      pvr_reg_write_both(0x44,gfx_state.read_ctrl_word & 0xfffffffe);
      pvr_reg_write_both(0x5c,gfx_state.read_size_word);
      pvr_reg_write_both(0x44,gfx_state.read_ctrl_word);
    }
  }
  }
  else r = 0;
  return r;
}
